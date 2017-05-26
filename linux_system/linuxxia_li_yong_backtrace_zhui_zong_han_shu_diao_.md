# Linux下利用backtrace追蹤函數調用堆棧以及定位段錯誤


一般察看函數運行時堆棧的方法是使用GDB（bt命令）之類的外部調試器,但是,有些時候為了分析程序的BUG,(主要針對長時間運行程序的分析),在程序出錯時打印出函數的調用堆棧是非常有用的。

在glibc頭文件"execinfo.h"中聲明瞭三個函數用於獲取當前線程的函數調用堆棧。

```c
int backtrace(void **buffer,int size) 
```

該函數用於獲取當前線程的調用堆棧,獲取的信息將會被存放在buffer中,它是一個指針列表。參數 size 用來指定buffer中可以保存多少個void* 元素。函數返回值是實際獲取的指針個數,最大不超過size大小

在buffer中的指針實際是從堆棧中獲取的返回地址,每一個堆棧框架有一個返回地址

注意:某些編譯器的優化選項對獲取正確的調用堆棧有干擾,另外內聯函數沒有堆棧框架;刪除框架指針也會導致無法正確解析堆棧內容

```c
char ** backtrace_symbols (void *const *buffer, int size) 
```

backtrace_symbols將從backtrace函數獲取的信息轉化為一個字符串數組. 參數buffer應該是從backtrace函數獲取的指針數組,size是該數組中的元素個數(backtrace的返回值)

函數返回值是一個指向字符串數組的指針,它的大小同buffer相同.每個字符串包含了一個相對於buffer中對應元素的可打印信息.它包括函數名，函數的偏移地址,和實際的返回地址

現在,只有使用ELF二進制格式的程序才能獲取函數名稱和偏移地址.在其他系統,只有16進制的返回地址能被獲取.另外,你可能需要傳遞相應的符號給鏈接器,以能支持函數名功能(比如,在使用GNU ld鏈接器的系統中,你需要傳遞(-rdynamic)， -rdynamic可用來通知鏈接器將所有符號添加到動態符號表中，如果你的鏈接器支持-rdynamic的話，建議將其加上！)

該函數的返回值是通過malloc函數申請的空間,因此調用者必須使用free函數來釋放指針.

注意:如果不能為字符串獲取足夠的空間函數的返回值將會為NULL

```c
void backtrace_symbols_fd (void *const *buffer, int size, int fd) 
```

backtrace_symbols_fd與backtrace_symbols 函數具有相同的功能,不同的是它不會給調用者返回字符串數組,而是將結果寫入文件描述符為fd的文件中,每個函數對應一行.它不需要調用malloc函數,因此適用於有可能調用該函數會失敗的情況
 
下面是glibc中的實例（稍有修改）：

```c
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

/* Obtain a backtrace and print it to @code{stdout}. */
void print_trace(void)
{
    void* array[10];
    size_t size;
    char** strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    if (NULL == strings) {
        exit(EXIT_FAILURE);
    }

    printf("Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings);
    strings = NULL;
}

/* A dummy function to make the backtrace more interesting. */
void dummy_function(void)
{
    print_trace();
}

int main(int argc, char* argv[])
{
    dummy_function();
    return 0;
}
```

