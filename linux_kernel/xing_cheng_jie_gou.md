# 行程結構


Linux 中的行程 (Process) 被稱為任務 (Task)，其資料結構是一個稱為 task_struct 的 C 語言結構，該結構所記錄的欄位相當多，在 Linux 2.6.29.4 版當中光是該結構的宣告就佔了 306 行的程式碼，範例 1 顯示了該結構的開頭與結束部分，由此可見要實作一個作業系統是相當不容易的工程。

範例 1 Linux 中的 Task 之結構

行號    Linux 2.6.29.4 版核心原始碼 include/linux/sched.h 檔案

```c
…    …
1115    struct task_struct {
1116    volatile long state; /* -1 unrunnable, 0 runnable, >0 stopped */
1117    void *stack;
1118    atomic_t usage;
1119    unsigned int flags; /* per process flags, defined below */
1120    unsigned int ptrace;
…    …
1263    /* CPU-specific state of this task */
1264    struct thread_struct thread;
1265    /* filesystem information */
1266    struct fs_struct *fs;
1267    /* open file information */
1268    struct files_struct *files;
1269    /* namespaces */
1270    struct nsproxy *nsproxy;
1271    /* signal handlers */
1272    struct signal_struct *signal;
1273    struct sighand_struct *sighand;
…    …
1417    #ifdef CONFIG_TRACING
1418    /* state flags for use by tracers */
1419    unsigned long trace;
1420    #endif
1421    };
…    …
```

Linux 的行程結構中包含了一個 thread 欄位 (1264 行)，該欄位用來儲存與 CPU 相關的暫存器、分段表等資訊，由於暫存器資訊是與處理器密切相關的，所以每一種 CPU 都會擁有不同的執行緒結構，IA32 (x86) 的thread_struct之程式片段如範例 10.3所示。

範例 2. Linux 中的IA32 (x86) 處理器的Thread 之結構

行號    Linux 2.6.29.4 版核心原始碼 arch/x86/include/asm/processor.h 檔案

```c
…    …
391    struct thread_struct {
392    /* Cached TLS descriptors: */
393    struct desc_struct tls_array[GDT_ENTRY_TLS_ENTRIES];
394    unsigned long sp0;
395    unsigned long sp;
…    …
```

##行程狀態

Linux 行程的狀態有 Running, Interruptible, Uninterruptible, Zombie, Stopped 等五種，但後來又增加了 Traced, EXIT_DEAD, TASK_DEAD, TASK_WAKEKILL 等四種，形成了九種狀態 (2.6.29.4 版)，如範例 10.4所示。

範例 3. Linux 中的行程狀態

行號    Linux 2.6.29.4 版核心原始碼 include/linux/sched.h 檔案

```c
…    …
174    #define TASK_RUNNING 0
175    #define TASK_INTERRUPTIBLE 1
176    #define TASK_UNINTERRUPTIBLE 2
177    #define __TASK_STOPPED 4
178    #define __TASK_TRACED 8
179    /* in tsk->exit_state */
180    #define EXIT_ZOMBIE 16
181    #define EXIT_DEAD 32
182    /* in tsk->state again */
183    #define TASK_DEAD 64
184    #define TASK_WAKEKILL 128
…    …
```

##行程切換

行程切換 (內文切換) 是與處理器密切相關的程式碼，每個處理器的實作方式均有相當大的差異，但基本原理都是將上一個行程 (prev, 以下稱為舊行程) 的暫存器保存後，再將程式計數器設定給下一個行程 (next, 以下稱為新行程)。在 IA32 (x86) 的處理器中，Linux 的行程切換程式碼 如範例 4 所示，該行程切換函數 switch_to(prev, next, last) 是一個內嵌於 C 語言的組合語言巨集，採用 GNU 的內嵌組合語言語法 。

首先，switch_to 最外層是一個 do { … } while (0) 的迴圈，這個語法很奇怪，因為該迴圈根本永遠都只會執行一次，那又為何要用迴圈呢？這純粹只是為了要把中間的組合語言用區塊結構 {…} 包起來而已，但卻很容易誤導讀者，以為那是一個無窮迴圈 。

範例 4. Linux在 IA32 (x86) 處理器上的行程切換程式碼

行號     Linux 2.6.29.4 檔案 arch/x86/include/asm/system.h

```c
…    …
30    #define switch_to(prev, next, last) \
31    do {\
32    /* \
33     * Context-switching clobbers all registers, so we clobber\
34     * them explicitly, via unused output variables.\
35     * (EAX and EBP is not listed because EBP is saved/restored \
36     * explicitly for wchan access and EAX is the return value of \
37     * __switch_to()) \
38     */ \
39    unsigned long ebx, ecx, edx, esi, edi; \
40    \
41    asm volatile("pushfl\n\t" /* save flags */ \
42    "pushl %%ebp\n\t" /* save EBP */ \
43    "movl %%esp,%[prev_sp]\n\t"/* save ESP */ \
44    "movl %[next_sp],%%esp\n\t"/* restore ESP */ \
45    "movl $1f,%[prev_ip]\n\t" /* save EIP */ \
46    "pushl %[next_ip]\n\t" /* restore EIP */ \
47    "jmp __switch_to\n" /* regparm call */ \
48    "1:\t" \
49    "popl %%ebp\n\t" /* restore EBP */ \
50    "popfl\n" /* restore flags */ \
51    \
52    /* output parameters */ \
53    [prev_sp] "=m" (prev->thread.sp), \
54    [prev_ip] "=m" (prev->thread.ip), \
55    "=a" (last), \
56    \
57    /* clobbered output registers: */ \
58    "=b" (ebx), "=c" (ecx), "=d" (edx), \
59    "=S" (esi), "=D" (edi) \
60    \
61    /* input parameters: */ \
62    : [next_sp]  "m" (next->thread.sp), \
63     [next_ip]  "m" (next->thread.ip), \
64    \
65    /* regparm parameters for __switch_to(): */ \
66    [prev]     "a" (prev), \
67    [next]     "d" (next) \
68    \
69    : /* reloaded segment registers */ \
70    "memory"); \
71    } while (0)
…    …
```

第41行開始才是行程切換的動作，指令pushfl 用來儲存旗標暫存器到堆疊中，pushl %%ebp 用來儲存框架暫存器 (ebp) 到堆疊中，movl %%esp, %[prev_sp] 則用來儲存舊行程的堆疊暫存器 (esp) 到 (prev->thread.sp) 欄位中，而 pushl %[next_sp], %%esp 則是將新行程的堆疊暫存器 (next->thread.sp) 取出後，放到CPU的esp暫存器中，於是建構好新行程的堆疊環境。接著，第45行的movl $1f, %[prev_ip] 將標記 1 的位址放入舊行程的 prev->thread.ip 欄位中。接著46行用指令pushl %[next_ip] 將新行程的程式計數器 next->thread.ip 推入堆疊中，然後利用指令jmp __switch_to跳入C語言的 switch_to() 函數 中，當該函數的返回指令被執行時，將會發生一個奇妙的結果。

由於 switch_to() 函數是一個C語言函數，原本應該被其他C語言函數呼叫的，呼叫前原本上層函數會先將下一個指令的位址存入堆疊中，然後才進行呼叫。C語言函數在返回前會從堆疊中取出返回點，以返回上一層函數繼續執行 。雖然我們是利用組合語言指令 jmp __switch_to 跳入該函數的，但C語言的編譯器仍然會以同樣的方式編譯，於是返回時仍然會從堆疊中取出 pushl %[next_ip] 指令所推入的位址，因而在 switch_to() 函數返回時，就會將程式計數器設為 next->thread.ip，於是透過函數返回的過程，間接的完成了行程切換的動作。

既然新行程已經在 switch_to() 函數返回時就開始執行了，那麼內文切換的動作不就已經完成了嗎？既然如此為何又需要第 49-50 兩行的程式呢？我們必須進一步回答這個問題。

第45行之所以將標記1放入prev->thread.ip中，是為了讓舊行程在下次被喚醒時，可以回到標記 1 的位置。當下次舊行程被喚醒後，就會從標記 1 的位址開始執行，舊行程可以利用第49-50行的popl %%ebp; popfl 兩個指令，恢復其 ebp (框架指標) 與旗標暫存器，然後再度透過 switch_to()，切換回舊行程 (只不過這次舊行程變成了函數switch_to(prev, next, last) 中的 next 角色，不再是『舊行程』了。

或許我們可以說 switch_to() 函數其實並不負責切換行程，因為該函數會將處理器中各種需要保存的值存入舊行程 prev 的 task_struct 結構中，以便下次 prev 行程被喚醒前可以回存這些暫存器值，其實並沒有切換或執行新行程的功能，但因為 jmp __switch_to 指令前的 pushl %[next_ip] 指令，導致該函數在返回時順便做了行程切換的動作，這種隱含性是作業系統設計時一種相當弔詭的技巧，也是學習 Linux 時對程式人員最大的挑戰之一。

##排程

解說完行程切換的原理後，我們就可以來看看排程系統了。Linux 採用的排程機制較為複雜，該排程式建構在一個稱為 goodness 的行程 (或執行緒) 評估值上，goodness 是一個從 0 到 1999 之間的整數。一般執行緒的 goodness 小於 1000，但即時執行緒的 goodness 則會從 1000 開始，因此保證了任何即時執行緒的 goodness 都會高於一般執行緒。

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int spawn(char *prog, char **arg_list) {    // Spawn為生育的意思
  pid_t child;    
  child = fork();                            // 用fork()函數分枝出子行程
  if (child != 0) {                            // 如果不成功
    return child;                            //   傳回失敗的行程代碼
  } else {                                     // 否則
    execvp(prog, arg_list);                    //   將 prog 參數所指定的
    fprintf(stderr, "spawn error\n");        // 程式載入到子行程中
    return -1;
  }
}

int main() {                                // 主程式開始
  char *arg_list[] = { "ls", "-l", "/etc", NULL };    // ls -l /etc
  spawn("ls", arg_list);                     // 開始分支
  printf("The end of program.\n");             // 印出主程式結束訊息
  return 0;
}
```

執行過程與結果

```sh
$ gcc fork.c -o fork
$ ./fork
```

```sh
The end of program.

$ total 94
-rwxr-x---  1 ccc Users  2810 Jun 13  2008 DIR_COLORS
drwxrwx---+ 2 ccc Users     0 Oct  7  2008 alternatives
-rwxr-x---  1 ccc Users    28 Jun 13  2008 bash.bashrc
drwxrwx---+ 4 ccc Users     0 Oct  7  2008 defaults
-rw-rw-rw-  1 ccc Users   716 Oct  7  2008 group
….
```
##執行緒


POSIX標準中支援的執行緒函式庫稱為 pthread，我們可以透過 pthread 結構與 pthread_create() 函數執行某個函數指標，以建立新的執行緒。範例 6 顯示了一個利用 pthread 建立兩個執行緒，分別不斷印出 George 與 Mary 的程式。該程式中總共有三個執行主體，第一個是 print_george()、第二個是 print_mary()、第三個是主程式本身。由於每隔 1 秒印一次 George ，但每隔 0.5 秒就印一次 Mary，因此執行結果會以 George, Mary, George, George,Mary 的形式印出。

範例 6 利用pthread 函式庫建立執行緒的範例

```c
#include <pthread.h>                // 引用 pthread 函式庫
#include <stdio.h>    

void *print_george(void *argu) {    // 每隔一秒鐘印出一次 George 的函數
  while (1) {    
    printf("George\n");    
    sleep(1);    
  }    
  return NULL;    
}    

void *print_mary(void *argu) {        // 每隔一秒鐘印出一次 Mary 的函數
  while (1) {    
    printf("Mary\n");    
    sleep(2);    
  }    
  return NULL;    
}    

int main() {                        // 主程式開始
  pthread_t thread1, thread2;        // 宣告兩個執行緒
  pthread_create(&thread1, NULL, &print_george, NULL);    // 執行緒 print_george
  pthread_create(&thread2, NULL, &print_mary, NULL);    // 執行緒 print_mary
  while (1) {                        // 主程式每隔一秒鐘
    printf("----------------\n");    // 就印出分隔行
    sleep(1);                        // 停止一秒鐘
  }    
  return 0;    
}
```

執行過程與結果
```sh
$ gcc thread.c -o thread
$ ./thread
George
Mary
-------------------------
George
-------------------------
George
Mary
-------------------------
George
-------------------------
…
```