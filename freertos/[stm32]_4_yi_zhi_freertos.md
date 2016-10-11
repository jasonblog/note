# [STM32] 4. 移植 FreeRTOS


FreeRTOS 是一個非常小型的 RTOS ，提供了任務排程器，同步機制和記憶體管理。如果只是需要一個單純的多工的環境，它是一個非常適合的輕量化 RTOS。其實這一篇嚴格來說稱不上移植，只能說是安裝。因為 FreeRTOS 本來就支援 ARM Cortex-m4，只需將原始碼加到專案中，聯結所需的函式就能使用了。

##4.1 移植 FreeRTOS 主程式

>1. 到官網下載程原始碼，這篇使用的是 FreeRTOSv9.0.0.zip 。不過各版本的移植方式大同小異，是可通用的。
2. 將壓縮檔解開後可以發現裡面有兩個目錄 FreeRTOS/ 和 FreeRTOS-Plus/ 。FreeRTOS 是主要核心，FreeRTOS-Plus 則是應用工具。目前只要看 FreeRTOS 就可以了。
3. 在專案下建立目錄 lib/FreeRTOS/src 和 lib/FreeRTOS/inc 放置檔案。
4. 將 FreeRTOS/License 複製到 lib/FreeRTOS/ 下
5. 將 FreeRTOS/Source/ 下所有 .c 檔複製到 lib/FreeRTOS/src
6. 將 FreeRTOS/Source/include 下所有 .h 檔複製到 lib/FreeRTOS/inc
7. 將 FreeRTOS/Source/portable/MemMang 下所有 .c 檔複製到 lib/FreeRTOS/src
8. 將 FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c 複製到 lib/FreeRTOS/src
9. 將 FreeRTOS/Source/portable/GCC/ARM_CM4F/portmacro.h 複製到 lib/FreeRTOS/inc
10. 將 FreeRTOS/Demo/CORTEX_M4F_STM32F407ZG-SK/FreeRTOSConfig.h 複製到 /inc
11. 修改 FreeRTOSConfig.h 移除 #ifdef __ICCARM__ 和相對應的 #endif
12. 修改 src/stm32f4xx_it.c ，將 SVC_Handler()， PendSV_Handler()，SysTick_Handler() 三個函式前面加上 __attribute__((weak))
13. 修改 src/main.c 在前面增加 #include "FreeRTOS.h" 和 #include "task.h"
14. 修改 src/main.c 增加 vApplicationMallocFailedHook() ， vApplicationIdleHook() 和 vApplicationStackOverflowHook() 三個函式，可從 FreeRTOS/Demo/CORTEX_M4F_STM32F407ZG-SK/main.c 中複製
15. 修改 src/main.c 增加 vApplicationTickHook() 函式，內容為空即可。
16. 將 lib/FreeRTOS/src 中的 .c 檔加入專案中編譯。注意：heap_x.c 只需選擇一個即可，可以先使用 heap_1.c
17. 修改 main() ，加入一行 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);





- 步驟 5 ~ 9 只是將原始碼加到專案中，擺放路徑其實可以隨自己喜好
- 步驟 8 和 9 中的 port.c 和 portmacro.h 是 mcu 的底層實作，如果使用不同的 mcu 只要換成相對應目錄的檔案即可。
- 步驟 10 中 FreeRTOSConfig.h 是 FreeRTOS 的設定檔，包含各項功能的開關。詳細的設定方式請參考原廠說明 FreeRTOS Configuration
- 步驟 12 中的三個中斷處理函式因為被 FreeRTOS 使用了，所以將 stm32f4xx_it.c 中的加上 weak 屬性避免連結時出錯
- 步驟 16 中 heap_x.c 是 FreeRTOS 提供的多種記憶體管理方式。如果系統中配置的記憶體不會釋放，可以使用 head_1.c 。否則就要選擇其他的管理方式。FreeRTOS Memory Management - 步驟 17 是為了配合 FreeRTOS ，將 NVIC 的優先權設定成 NVIC_PriorityGroup_4


如果有參考前一篇建立了 newlib 的 system call ，要記得修改 _sbrk 這個函式。因為在 RTOS 下， 會因為 stack_ptr 位置改變造成判斷錯誤 。


```c
caddr_t _sbrk(int incr)
{
    extern char _end; /* Defined by the linker */
    static char* heap_end;
    char* prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;
#if 0 // this check will fail in FreeRTOS task

    if (heap_end + incr > stack_ptr) {
        _write(1, "Heap and stack collision\n", 25);

        while (1);
    }

#endif
    heap_end += incr;
    return (caddr_t) prev_heap_end;
}
```

##4.2 LED 閃爍測試程式

下面是一個簡單的小程式，建立一個獨立的任務操作 GPIO 開關 LED 。

```c
void LED_Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

__INLINE void LED_On(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

__INLINE void LED_Off(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

static void Blink_Task(void* pvParameters)
{
    TickType_t xLastFlashTime;

    LED_Init();

    xLastFlashTime = xTaskGetTickCount();

    while (1) {
        LED_On();
        vTaskDelayUntil(&xLastFlashTime, 500);
        LED_Off();
        vTaskDelayUntil(&xLastFlashTime, 4500);
    }
}

int main(void)
{
    // Initialize system
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    xTaskCreate(Blink_Task, "LED", configMINIMAL_STACK_SIZE, NULL,
                BLINK_TASK_PRIORITY, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
```


##4.3 移植 FreeRTOS CLI

FreeRTOS CLI 是一個處理命令列的框架，可以透過它提供的 API 輕鬆的增加命令。對嵌入式系統來說是很方便的介面，建立一開始就先移植到系統中會讓開發方便很多。

>1. 將 FreeRTOS-Plus-CLI 整個目錄複製到專案 lib 下
2. 修改 /inc/FreeRTOSConfig.h ，在檔案末端的 #endif 前增加一行 #define configCOMMAND_INT_MAX_OUTPUT_SIZE 1024
3. 將 FreeRTOS_CLI.c 加入專案中編譯
4. 參考 FreeRTOS-Plus/Demo/Common/FreeRTOS_Plus_CLI_Demos/UARTCommandConsole.c 建立一個任務來處理

以下是一個簡單的範例程式。USART2 收到字元後，利用 queue 傳給 CONSOLE_Task。等到蒐集到一行命令時，再傳給 FreeRTOS_CLIProcessCommand 處理。

```c
/* Private define ------------------------------------------------------------*/
#define CONSOLE_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2UL )
#define CONSOLE_TASK_STACK      ( 2048/4 )        // 2048 bytes
#define CONSOLE_QUEUE_LENGTH     16
#define MAX_INPUT_SIZE       64

#define DBG_CONSOLE        TRUE

#define CHAR_ASCII_DEL       ( 0x7F )        // DEL acts as a backspace.
#define CHAR_ASCII_ESC       ( 0x1B )        // ESCAPE Key

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
QueueHandle_t xConsoleQueue;
static const char* const pcWelcomeMessage =
    "FreeRTOS command server.\nType Help to view a list of registered commands.\n\n>";
static const char* const pcEndOfOutputMessage =
    "\n[Press ENTER to execute the previous command again]\n[Press UP ARROW to show the previous command]\n>";

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void USART2_IRQHandler(void)
{
    char data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    while (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        data = USART_ReceiveData(USART2);
        xQueueSendFromISR(xConsoleQueue, &data, &xHigherPriorityTaskWoken);
    }
}

/**
 * @brief Displays a table showing the state of each FreeRTOS task
 * @param pcWriteBuffer
 * @param xWriteBufferLen
 * @param pcCommandString
 * @return
 */
static BaseType_t prvTaskStatsCommand(char* pcWriteBuffer,
                                      size_t xWriteBufferLen, const char* pcCommandString)
{
    const char* const pcHeader =
        "     State   Priority  Stack    #\r\n************************************************\r\n";
    BaseType_t xSpacePadding;

    /* Remove compile time warnings about unused parameters, and check the
     write buffer is not NULL.  NOTE - for simplicity, this example assumes the
     write buffer length is adequate, so does not check for buffer overflows. */
    (void) pcCommandString;
    (void) xWriteBufferLen;
    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Minus three for the null terminator and half the number of characters in
     "Task" so the column lines up with the centre of the heading. */
    configASSERT(configMAX_TASK_NAME_LEN > 3);

    for (xSpacePadding = strlen("Task");
         xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }

    strcpy(pcWriteBuffer, pcHeader);
    vTaskList(pcWriteBuffer + strlen(pcHeader));

    /* There is no more data to return after this single string, so return
     pdFALSE. */
    return pdFALSE;
}

static const CLI_Command_Definition_t xTaskStats = {
    "task-stats", /* The command string to type. */
    "task-stats:\n    Displays a table showing the state of each FreeRTOS task\n",
    prvTaskStatsCommand, /* The function to run. */
    0 /* No parameters are expected. */
};

/**
 * @brief Console task
 * @param pvParameters
 */
static void CONSOLE_Task(void* pvParameters)
{
    char data;
    uint8_t inputIndex = 0;
    uint8_t controlModeIndex = 0;
    static char inputString[MAX_INPUT_SIZE], lastInputString[MAX_INPUT_SIZE];
    char* outputString;
    BaseType_t ret;

    outputString = FreeRTOS_CLIGetOutputBuffer();
    DEBUG_printf(DBG_CONSOLE, "\nBuild on %s at %s. ", __DATE__, __TIME__);
    DEBUG_printf(DBG_CONSOLE, "Git Version : %s\n", GIT_VERSION);
    DEBUG_printf(DBG_CONSOLE, "%s", pcWelcomeMessage);
    fflush(stdout);

    while (1) {
        xQueueReceive(xConsoleQueue, &data, portMAX_DELAY);

        if (data == '\r') {
            DEBUG_printf(DBG_CONSOLE, "\n");

            // See if the command is empty, indicating that the last command is to be executed again.
            if (inputIndex == 0) {
                strcpy(inputString, lastInputString);
            }

            do {
                // Get the next output string from the command interpreter.
                ret = FreeRTOS_CLIProcessCommand(inputString, outputString,
                                                 configCOMMAND_INT_MAX_OUTPUT_SIZE);

                // Write the generated string to the UART.
                DEBUG_printf(DBG_CONSOLE, "%s\n", outputString);

            } while (ret != pdFALSE);

            strcpy(lastInputString, inputString);
            inputIndex = 0;
            memset(inputString, 0x00, MAX_INPUT_SIZE);

            DEBUG_printf(DBG_CONSOLE, "%s", pcEndOfOutputMessage);
            fflush(stdout);

        } else if (data == CHAR_ASCII_ESC) {
            controlModeIndex = 1;
        } else if (controlModeIndex != 0) {
            if (controlModeIndex == 1) {
                if (data == '[') {
                    controlModeIndex = 2;
                } else {
                    controlModeIndex = 0;
                }
            } else if (controlModeIndex == 2) {
                if (data == 'A') { // UP ARROR KEY: Show last command
                    while (inputIndex > 0) {
                        inputIndex--;
                        inputString[inputIndex] = '\0';
                        DEBUG_printf(DBG_CONSOLE, "\b \b");
                        fflush(stdout);
                    }

                    strcpy(inputString, lastInputString);
                    DEBUG_printf(DBG_CONSOLE, "%s", inputString);
                    inputIndex = strlen(inputString);
                    fflush(stdout);
                }

                controlModeIndex = 0;
            } else {
                controlModeIndex = 0;
            }

        } else if ((data == '\b') || (data == CHAR_ASCII_DEL)) {
            // Backspace was pressed.  Erase the last character in the string - if any
            if (inputIndex > 0) {
                inputIndex--;
                inputString[inputIndex] = '\0';
                DEBUG_printf(DBG_CONSOLE, "\b \b");
                fflush(stdout);
            }
        } else if ((data >= ' ') && (data <= '~')) {
            if (inputIndex < MAX_INPUT_SIZE) {
                inputString[inputIndex] = data;
                inputIndex++;
                DEBUG_printf(DBG_CONSOLE, "%c", data);
                fflush(stdout);
            }
        } else {
            // Ignore other character
        }
    }
}

/**
 * @brief Initialize and create console task
 */
void CONSOLE_Init(void)
{
    FreeRTOS_CLIRegisterCommand(&xTaskStats);

    xConsoleQueue = xQueueCreate(CONSOLE_QUEUE_LENGTH, sizeof(char));

    //assert(xConsoleQueue != NULL);

    // Create that task that handles the console itself.
    xTaskCreate(
        CONSOLE_Task,      /* The task that implements the command console. */
        "CONSOLE",     /* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
        CONSOLE_TASK_STACK,   /* The size of the stack allocated to the task. */
        NULL,      /* The parameter is not used, so NULL is passed. */
        CONSOLE_TASK_PRIORITY,  /* The priority allocated to the task. */
        NULL);       /* A handle is not required, so just pass NULL. */
}
```

##4.4 Nucleo-F446RE template

這裡提供一份包含了 FreeRTOS 和 FreeRTOS-CLI 的樣板程式 [Nucleo-F446RE template](https://github.com/dinowchang/stm32f4-template)