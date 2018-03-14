---
title: Kernel 進程相關
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,process
description: 實際上一個進程，包括了代碼、數據和分配給進程的資源，這是一個動態資源。這裡簡單介紹與進程相關的東西，例如進程創建、優先級、進程之間的關係、進程組和會話、進程狀態等。
---

實際上一個進程，包括了代碼、數據和分配給進程的資源，這是一個動態資源。

這裡簡單介紹與進程相關的東西，例如進程創建、優先級、進程之間的關係、進程組和會話、進程狀態等。

<!-- more -->

## 進程

一個進程，包括了代碼、數據和分配給進程的資源。

### 初始進程

在 Linux 中，進程基本都是通過複製其它進程的結構來實現的，利用 slabs 來動態分配，系統沒有提供用於創建進程的接口。

唯一的一個例外是第一個 `task_struct`，這是由一個靜態或者說是固化的結構表示的 (`init_task`)，該進程的 `PID=0`，可以參看 `arch/x86/kernel/init_task.c`，也可以稱之為空閒進程。

當內核執行到 `sched_init()` 時，`task_struct` 的核心 `TR`、`LDT` 就被手工設置好了，這時，0 號進程就有了，而在 `sched_init()` 之前，是沒有 "進程" 這個概念的，而 `init(PID=1)` 進程是 0 號進程 `fork()` 出來的。

{% highlight text %}
struct task_struct init_task = INIT_TASK(init_task);
{% endhighlight %}

查看 `INIT_TASK()` 宏定義時，會發現很多有意思的東西，如 `init_task` 的棧通過 `.stack = &init_thread_info` 指定，而該棧實際時通過如下的方式靜態分配。

{% highlight text %}
union thread_union init_thread_union __init_task_data = { INIT_THREAD_INFO(init_task) };
{% endhighlight %}

定義中的 `__init_task_data` 表明該內核棧所在的區域位於內核映像的 `init data` 區，可以通過編譯完內核後所產生的 System.map 來看到該變量及其對應的邏輯地址。

{% highlight text %}
$ cat System.map-`uname -r` | grep init_thread_union
ffffffff818fc000 D init_thread_union
{% endhighlight %}

而內核在無進程的情況下，將一直從初始化部分的代碼執行到 `start_kernel()`，然後再到其最後一個函數調用 `rest_init()`，在該函數中將產生第一個進程 `PID=1`。

{% highlight text %}
start_kernel()
 |-rest_init()
   |-kernel_thread()        實際上調用kernel_init()創建第一個進程PID=1

kernel_init()
 |-run_init_process()
   |-do_execve()
{% endhighlight %}

最後 `init_task` 任務會變成 `idle` 進程。

在 Linux 系統中，可創建進程的最大值是由 `max_threads@kernel/fork.c` 變量確定的，也可以通過 `/proc/sys/kernel/threads-max` 查看/更改此值。

### fork

通過 `fork()` 將創建一個與原來進程幾乎完全相同的進程，系統給新的進程分配資源，例如存儲數據和代碼的空間，然後把原來的進程的所有值都複製到新的新進程中，只有少數值與原來的進程的值不同，相當於克隆了一個自己。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t pid;
    int count = 0;

    pid = fork();            // vfork(); error
    if (pid < 0) {
        printf("error in fork!");
    } else if (pid == 0) {   // child process
        count++;
        printf(" child: PID is %d, count is %d\n", getpid(), count);
    } else {                 // parent process
        count++;
        printf("parent: PID is %d, count is %d\n", getpid(), count);
    }

    return 0;
}
{% endhighlight %}

在語句 `fork()` 之前，只有一個進程在執行這段代碼，但在這條語句之後，就變成兩個進程在執行了，這兩個進程的幾乎完全相同。其中 fork() 的一個奇妙之處就是它僅僅被調用一次，卻能夠返回兩次，它可能有三種不同的返回值：

1. 在父進程中，fork() 返回新創建子進程的進程 ID；
2. 在子進程中，fork() 返回 0；
3. 如果出現錯誤，fork() 返回一個負值；

創建新進程成功後，系統中出現兩個基本完全相同的進程，這兩個進程執行沒有固定的先後順序，哪個進程先執行要看系統的進程調度策略。

### 執行流程

1. 進程可以看做程序的一次執行過程。在 linux 下，每個進程有唯一的 PID 標識進程。 PID 是一個從 1 到 32768 的正整數，其中 1 一般是特殊進程 init ，其它進程從 2 開始依次編號。當用完 32768 後，從 2 重新開始。

2. Linux 中有一個叫進程表的結構用來存儲當前正在運行的進程。可以使用 `ps aux` 命令查看所有正在運行的進程。

3. 進程在 linux 中呈樹狀結構， init 為根節點，其它進程均有父進程，某進程的父進程就是啟動這個進程的進程，這個進程叫做父進程的子進程。

4. fork 的作用是複製一個與當前進程一樣的進程。新進程的所有數據（變量、環境變量、程序計數器等）數值都和原進程一致，但是是一個全新的進程，並作為原進程的子進程。


![process fork]({{ site.url }}/images/linux/process-fork.gif "process fork"){: .pull-center }

上圖表示一個含有 fork 的程序，而 fork 語句可以看成將程序切為 A、B 兩個部分。然後整個程序會如下運行：

1.  設由 shell 直接執行程序，生成了進程 M 。 M 執行完 Part.A 的所有代碼。

2. 當執行到 pid = fork(); 時， M 啟動一個進程 S ，S 是 M 的子進程，和 M 是同一個程序的進程。S 繼承 M 的所有變量、環境變量、程序計數器的當前值。

3. 在 M 進程中，fork() 將 S 的 PID 返回給變量 pid ，並繼續執行 Part.B 的代碼。

4. 在進程 S 中，將 0 賦給 pid ，並繼續執行 Part.B 的代碼。

這裡有三個點非常關鍵:

* M 執行了所有程序，而 S 只執行了 Part.B ，即 fork() 後面的程序，(這是因為 S 繼承了 M 的 PC-程序計數器)。

* S 繼承了 fork() 語句執行時當前的環境，而不是程序的初始環境。

* M 中 fork() 語句啟動子進程 S ，並將 S 的 PID 返回，而 S 中的 fork() 語句不啟動新進程，僅將 0 返回。

#### 舉例

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    pid_t pid1;
    pid_t pid2;

    pid1 = fork();
    pid2 = fork();

    printf("pid1:%d, pid2:%d\n", pid1, pid2);
}
{% endhighlight %}

對於如上的程序，在執行之後將會生成 4 個進程，如果其中一個進程的輸出結果是 `pid1:1001, pid2:1002` ，那麼其他的分別為。

{% highlight text %}
pid1:1001, pid2:0
pid1:0, pid2:1003
pid1:0, pid2:0
{% endhighlight %}

實際的執行過程如下所示：

![process fork]({{ site.url }}/images/linux/process-fork-2.gif "process fork"){: .pull-center }

最後的實際執行過程為：

1. 從 shell 中執行此程序，啟動了一個進程，我們設這個進程為 P0 ，設其 PID 為 XXX (解題過程不需知道其 PID)。

2. 當執行到 pid1 = fork(); 時， P0 啟動一個子進程 P1 ，由題目知 P1 的 PID 為 1001 。我們暫且不管 P1 。

3. P0 中的 fork 返回 1001 給 pid1 ，繼續執行到 pid2 = fork(); ，此時啟動另一個新進程，設為 P2 ，由題目知 P2 的 PID 為 1002 。同樣暫且不管 P2 。

4. P0 中的第二個 fork 返回 1002 給 pid2 ，繼續執行完後續程序，結束。所以， P0 的結果為 “pid1:1001, pid2:1002” 。

5. 再看 P2 ， P2 生成時， P0 中 pid1=1001 ，所以 P2 中 pid1 繼承 P0 的 1001 ，而作為子進程 pid2=0 。 P2 從第二個 fork 後開始執行，結束後輸出 “pid1:1001, pid2:0” 。

6. 接著看 P1 ， P1 中第一條 fork 返回 0 給 pid1 ，然後接著執行後面的語句。而後面接著的語句是 pid2 = fork(); 執行到這裡， P1 又產生了一個新進程，設為 P3 。先不管 P3 。

7. P1 中第二條 fork 將 P3 的 PID 返回給 pid2 ，由預備知識知 P3 的 PID 為 1003 ，所以 P1 的 pid2=1003 。 P1 繼續執行後續程序，結束，輸出 “pid1:0, pid2:1003” 。

8. P3 作為 P1 的子進程，繼承 P1 中 pid1=0 ，並且第二條 fork 將 0 返回給 pid2 ，所以 P3 最後輸出 “pid1:0, pid2:0” 。

9. 至此，整個執行過程完畢。


## 進程創建

進程和線程相關的函數在內核中，最終都會調用 `do_fork()`，只是最終傳入的參數不同。

其中在用戶空間中與進程相關的接口有 `fork()`、`vfork()`、`clone()`、`exec()`；線程相關的有 `pthread_create` (glibc) 和 `kernel_thread` (Kernel內部函數)，`pthread_create()` 是對 `clone()` 的封裝，`kernel_thread()` 用於創建內核線程，兩者最終同樣會調用 `do_fork()`。

<!--
fork()
fork創建一個進程時，子進程只是完全複製父進程的資源，複製出來的子進程有自己的task_struct結構和pid,但卻複製父進程其它所有的資源。例如，要是父進程打開了五個文件，那麼子進程也有五個打開的文件，而且這些文件的當前讀寫指針也停在相同的地方。所以，這一步所做的是複製。這樣得到的子進程獨立於父進程， 具有良好的併發性，但是二者之間的通訊需要通過專門的通訊機制，如：pipe，共享內存等機制， 另外通過fork創建子進程，需要將上面描述的每種資源都複製一個副本。

這樣看來，fork是一個開銷十分大的系統調用，這些開銷並不是所有的情況下都是必須的，比如某進程fork出一個子進程後，其子進程僅僅是為了調用exec執行另一個可執行文件，那麼在fork過程中對於虛存空間的複製將是一個多餘的過程。但由於現在Linux中是採取了copy-on-write(COW寫時複製)技術，為了降低開銷，fork最初並不會真的產生兩個不同的拷貝，因為在那個時候，大量的數據其實完全是一樣的。寫時複製是在推遲真正的數據拷貝。若後來確實發生了寫入，那意味著parent和child的數據不一致了，於是產生複製動作，每個進程拿到屬於自己的那一份，這樣就可以降低系統調用的開銷。所以有了寫時複製後呢，vfork其實現意義就不大了。

fork()調用執行一次返回兩個值，對於父進程，fork函數返回子程序的進程號，而對於子程序，fork函數則返回零，這就是一個函數返回兩次的本質。

在fork之後，子進程和父進程都會繼續執行fork調用之後的指令。子進程是父進程的副本。它將獲得父進程的數據空間，堆和棧的副本，這些都是副本，父子進程並不共享這部分的內存。也就是說，子進程對父進程中的同名變量進行修改並不會影響其在父進程中的值。但是父子進程又共享一些東西，簡單說來就是程序的正文段。正文段存放著由cpu執行的機器指令，通常是read-only的。


vfork()
vfork系統調用不同於fork，用vfork創建的子進程與父進程共享地址空間，也就是說子進程完全運行在父進程的地址空間上，如果這時子進程修改了某個變量，這將影響到父進程。

其次，子進程在vfork()返回後直接運行在父進程的棧空間，並使用父進程的內存和數據。這意味著子進程可能破壞父進程的數據結構或棧，造成失敗。為了避免這些問題，需要確保一旦調用vfork()，子進程就不從當前的棧框架中返回，並且如果子進程改變了父進程的數據結構就不能調用exit函數。子進程還必須避免改變全局數據結構或全局變量中的任何信息，因為這些改變都有可能使父進程不能繼續。

但此處有一點要注意的是用vfork()創建的子進程必須顯示調用exit()來結束，否則子進程將不能結束，而fork()則不存在這個情況。

Vfork也是在父進程中返回子進程的進程號，在子進程中返回0。

用 vfork創建子進程後，父進程會被阻塞直到子進程調用exec(exec，將一個新的可執行文件載入到地址空間並執行之。)或exit。vfork的好處是在子進程被創建後往往僅僅是為了調用exec執行另一個程序，因為它就不會對父進程的地址空間有任何引用，所以對地址空間的複製是多餘的 ，因此通過vfork共享內存可以減少不必要的開銷

再次強調：在使用vfork()時，必須在子進程中調用exit()函數調用，否則會出現：__new_exitfn: Assertion `l != ((void *)0)' failed　錯誤！而且，現在這個函數已經很少使用了！


clone（）
    系統調用fork()和vfork()是無參數的，而clone()則帶有參數。fork()是全部複製，vfork()是共享內存，而clone()是則可以將父進程資源有選擇地複製給子進程，而沒有複製的數據結構則通過指針的複製讓子進程共享，具體要複製哪些資源給子進程，由參數列表中的clone_flags決決定。

fork不對父子進程的執行次序進行任何限制，fork返回後，子進程和父進程都從調用fork函數的下一條語句開始行，但父子進程運行順序是不定的，它取決於內核的調度算法；而在vfork調用中，子進程先運行，父進程掛起，直到子進程調用了exec或exit之後，父子進程的執行次序才不再有限制；clone中由標誌CLONE_VFORK來決定子進程在執行時父進程是阻塞還是運行，若沒有設置該標誌，則父子進程同時運行，設置了該標誌，則父進程掛起，直到子進程結束為止。-->

### 區別

`fork()` 會創建新的進程；`exec()` 的原進程將會被新的進程替換；而 `vfork()` 其實就是 `fork()` 的部分過程，通過簡化來提高效率。

`fork()` 是進程資源的完全複製，包括進程的 `PCB(task_struct)`、線程的系統堆棧、進程的用戶空間、進程打開的設備等，而在 `clone()` 中其實只有前兩項是被複制了的，後兩項都與父進程共享，對共享數據的保護必須有上層應用來保證。

`vfork()` 與 `fork()` 主要區別:

* `fork()` 子進程拷貝父進程的數據段，堆棧段；`vfork()` 子進程與父進程共享數據段。
* `fork()` 父子進程的執行次序不確定；`vfork()` 保證子進程先運行，在調用 `exec()` 或 `exit()` 之前與父進程數據是共享的，在它調用 `exec()` 或 `exit()` 之後父進程才可能被調度運行，否則會發生錯誤。
* 如果在此之前子進程有依賴於父進程的進一步動作，如調用函數，則會導致錯誤。

`fork()` 在執行復制時，採用 "寫時複製"，開始的時候內存並沒有被複制，而是共享的，直到有一個進程去寫某塊內存時，它才被複制。實際操作時，內核先將這些內存設為只讀，當它們被寫時，CPU 出現訪存異常，內核捕捉異常，複製空間，並改屬性為可寫。

但是，"寫時複製" 其實還是有複製，進程的 mm 結構、頁表都還是被複制了，而 `vfork()` 會忽略所有關於內存的東西，父子進程的內存是完全共享的。

不過此時，父子進程共用著棧，如果兩個進程並行執行，那麼可能會導致調用棧出錯。所以，`vfork()` 有個限制，當子進程生成後，父進程在 `vfork()` 中被內核掛起，直到子進程有了自己的內存空間 `(exec**)` 或退出 `(_exit)`。

並且，在此之前，子進程不能從調用 `vfork()` 的函數中返回，同時，不能修改棧上變量、不能繼續調用除 `_exit()` 或 `exec()` 系列之外的函數，否則父進程的數據可能被改寫。

雖然 `vfork()` 的限制很多，但是對於 shell 來說卻非常適合。


### 源碼解析

如下是內核中的接口，`do_fork()` 返回的是子進程的 PID。

{% highlight text %}
// kernel/fork.c
do_fork(SIGCHLD, 0, 0, NULL, NULL);                           // sys_fork
do_fork(CLONE_VFORK | CLONE_VM | SIGCHLD, 0, 0, NULL, NULL);  // sys_vfork
do_fork(clone_flags, newsp, 0, parent_tidptr, child_tidptr);  // sys_clone


do_fork() @ kernel/fork.c
  |-... ...                            // 做一些參數的檢查工作，主要是針對flag
  |-copy_process()                     // 複製一個進程描述符，PID不同
  |  |-security_task_create()          // 安全模塊的回調函數
  |  |-dup_task_struct()               // 新建task_struct和thread_info，並將當前進程相應的結構完全複製過去
  |  |-atomic_read()                   // 判斷是否超過了進程數
  |  |-copy_creds()                    // 應該是安全相關的設置
  |  |-... ...                         // 判斷創建的進程是否超了進程的總量等
  |  |-sched_fork()                    // 調度相關初始化
  |  |-copy_xxx()                      // 根據傳入的flag複製相應的數據結構
  |  |-alloc_pid()                     // 為新進程獲取一個有效的PID
  |  |-sched_fork()                    // 父子進程平分共享的時間片
  |
  |-wake_up_new_task()                 // 如果創建成功則執行
  |-wait_for_vfork_done()              // 如果是vfork()等待直到子進程exit()或者exec()
{% endhighlight %}

`clone_flags` 由 4 個字節組成，最低的一個字節為子進程結束時發送給父進程的信號代碼，`fork/vfork` 為 `SIGCHLD`，`clone` 可以指定；剩餘的三個字節則是各種 `clone()` 標誌的組合，用於選擇複製父進程那些資源。

內核有意選擇子進程首先執行。因為一般子進程都會馬上調用 exec 函數，這樣可以避免寫時拷貝的額外開銷，如果父進程首先執行的話，有可能開始向地址空間寫入。

### PID分配

pid 分配的範圍是 0~32767，`struct pidmap` 用來標示 pid 是不是已經分配出去了，採用位圖的方式，每個字節表示 8 個 PID，為了表示 32768 個進程號要用 32768/8=4096 個字節，也即一個 page，結構體中的 nr_free 表示剩餘 PID 的數量。

PID 相關的一些操作實際上就是 bitmap 的操作，常見的如下。

* int test_and_set_bit(int offset, void *addr);<br>將 offset 在 pidmap 變量當中相應的位置為 1，並返回舊值；也就是申請到一個 pid 號之後，修改位標誌，其中 addr 是 pidmap.page 變量的地址。

* void clear_bit(int offset, void *addr);<br>將 offset 在 pidmap 變量當中相應的位置為 0，也就是釋放一個 pid 後，修改位標誌，其中 addr 是 pidmap.page 變量的地址。

* int find_next_zero_bit(void *addr, int size, int offset);<br>從 offset 開始，找下一個是 0 (也就是可以分配) 的 pid 號，其中 addr 是 pidmap.page 變量的地址，size 是一個頁的大小。

* int alloc_pidmap();<br>分配一個 pid 號。

* void free_pidmap(int pid);<br>回收一個 pid 號。

目前 Linux 通過 PID 命名空間進行隔離，其中有一個變量 last_pid 用於標示上一次分配出去的 pid 編號。

在內核中通過移位操作來實現根據 PID 查找地址，可以想象抽象出一張表，這個表有 32 列，1024行，這個剛好是一個頁的大小，可以參考相應程序的示例。

### 系統進程遍歷

在內核中，會通過雙向鏈表保存任務列表，可以將 `init_task` 作為鏈表的開頭，然後進行迭代。

可以通過內核模塊 `procsview` 進行查看，通過 `printk` 將所有的進程打印出來，結果可以通過 `tail -f /var/log/messages` 查看。

可以通過宏定義 `next_task()@include/linux/sched.h` 簡化任務列表的迭代，該宏會返回下一個任務的 `task_struct` 引用；`current` 標識當前正在運行的進程，這實際是一個宏，在 `arch/x86/include/asm/current.h` 中定義。

通過 Make 進行編譯，用 `insmod procsview.ko` 插入模塊對象，用 `rmmod procsview` 刪除它。插入後，`/var/log/messages` 可顯示輸出，其中有一個空閒任務 (稱為 swapper) 和 init 任務 (pid 1)。


## 優先級

靜態優先級和 nice 值的區別。

只有 nice 值對用戶可見，而靜態優先級則隱藏在內核中，用戶可以通過修改 nice 值間接修改靜態優先級，而且只會影響靜態優先級，不會影響動態優先級。對於普通進程來說，動態優先級是基於靜態優先級算出來的。

靜態優先級在進程描述符中為 `static_prio` 成員變量，該優先級不會隨著時間而改變，內核不會修改它，只能通過系統調用 `nice` 去調整。

nice值是每個進程的一個屬性，不是進程的優先級，而是一個能影響優先級的數字；取值範圍為-20~19，默認為0。現在內核不再存儲 nice 值，而是存儲靜態優先級 static_prio，兩者在內核中通過兩個宏進行轉換。

{% highlight c %}
#define NICE_TO_PRIO(nice)  (MAX_RT_PRIO + (nice) + 20)
#define PRIO_TO_NICE(prio)  ((prio) - MAX_RT_PRIO - 20)
#define TASK_NICE(p)        PRIO_TO_NICE((p)->static_prio)
{% endhighlight %}

上述的宏存在於 `kernel/sched/sched.h` 中，靜態優先級的取值範圍為 `[MAX_RT_PRIO, MAX_PRIO-1]` 也即 `[100, 139]` 。

動態優先級的值影響任務的調度順序，調度程序通過增加或減少進程靜態優先級的值來獎勵 IO 消耗型進程或懲罰 CPU 消耗型進程，調整後的優先級稱為動態優先級，在進程描述符中用 prio 表示，通常所說的優先級指的是動態優先級。

動態優先級的取值範圍為 `[0, MAX_PRIO-1]` 也即 `[0, 139]`，其中 `[0, MAX_RT_PRIO-1]` 也即 `[0, 99]` 為實時進程範圍，數值越大表示優先級越小。

優先級可以通過 `top`、`ps -o pid,comm,nice -p PID` 或者 `ps -el` 查看，如果是實時進程就是靜態優先級，如果是非實時進程，就是動態優先級。

{% highlight text %}
# nice -n 10 commands               # 指定啟動時的優先級為10
# nice -10 commands                 # 同上
# nice --10 commands                # 指定優先級為-10
# renice 10 -p PID                  # 設置已經啟動進程的優先級，只有root可以設置為負值
# renice -10 PID                    # 設置為-10
{% endhighlight %}

可以修改 `/etc/security/limits.conf` 的值，指定特定用戶的優先級 `[username] [hard|soft] priority [nice value]` 。

<!--
 prio和normal_prio為動態優先級，static_prio為靜態優先級。static_prio是進程創建時分配的優先級，如果不人為的更 改，那麼在這個進程運行期間不會發生變化。 normal_prio是基於static_prio和調度策略計算出的優先級。prio是調度器類考慮的優先級，某些情況下需要暫時提高進程的優先級 (實時互斥量)，因此有此變量，對於優先級未動態提高的進程來說這三個值是相等的。以上三個優先級值越小，代表進程的優先級有高。一般情況下子進程的靜態 優先級繼承自父進程，子進程的prio繼承自父進程的normal_prio。
    rt_policy表示實時進程的優先級，範圍為0～99，該值與prio，normal_prio和static_prio不同，值越大代表實時進程的優先級越高。
    那麼內核如何處理這些優先級之間的關係呢？其實，內核使用0～139表示內部優先級，值越低優先級越高。其中0～99為實時進程，100～139為非實時進程。
    當static_prio分配好後，prio和normal_prio計算方法實現如下：
    首先，大家都知道進程創建過程中do_fork會調用wake_up_new_task,在該函數中會調用static int effective_prio(struct task_struct *p)函數。
    void fastcall wake_up_new_task(struct task_struct *p, unsigned long clone_flags)
    {
            unsigned long flags;
               struct rq *rq;
        ...
            p->prio = effective_prio(p);
        ...
    }
    static int effective_prio(struct task_struct *p)函數的實現如下：
    static int effective_prio(struct task_struct *p)
    {
            p->normal_prio = normal_prio(p);
                /*
             * If we are RT tasks or we were boosted to RT priority,
             * keep the priority unchanged. Otherwise, update priority
             * to the normal priority:
             */
            if (!rt_prio(p->prio))
                    return p->normal_prio;
            return p->prio;
    }
    在函數中設置了normal_prio的值，返回值有設置了prio，真是一箭雙鵰，對於實時進程需要特殊處理，總結主要涉及非實時進進程，就對實時進程的處理方法不解釋了。
    static inline int normal_prio(struct task_struct *p)的實現如下：
        static inline int normal_prio(struct task_struct *p)
    {
            int prio;

            if (task_has_rt_policy(p))
                    prio = MAX_RT_PRIO-1 - p->rt_priority;
            else
                    prio = __normal_prio(p);
            return prio;
    }
    對於普通進程會調用static inline int __normal_prio(struct task_struct *p)函數。
    static inline int __normal_prio(struct task_struct *p)函數的實現如下：
        static inline int __normal_prio(struct task_struct *p)
    {
            return p->static_prio;
    }
    這樣大家應該很清楚了，對於非實時進程prio，normal_prio和static_prio是一樣的，但是也有特殊情況，當使用實時互斥量時prio會暫時發生變化。



普通進程的優先級通過一個關於靜態優先級和進程交互性函數關係計算得到。隨實際任務的實際運行情況得到。實時優先級和它的實時優先級成線性，不隨進程的運行而改變。

3、實時優先級：

實時優先級只對實時進程有意義。在進程描述符rt_priority中。取值0~MAX_RT_PRIO-1。

    prio=MAX_RT_PRIO-1 – rt_priority
    時間片：

    在完全公平調度器CFS融入內核之前，時間片是各種調度器的一個重要的概念。它指定了進程在被搶佔之前所能運行的時間。調用器的一個重要目標便是有效的分配時間片，以便提供良好的用戶體驗。時間片分的過長會導致交互式進程響應不佳。時間片分的過長會導致進程切換帶來的消耗。為瞭解決這個矛盾內核採用了：

    1、提高交互進程的優先級，同時分配默認的時間片

    2、不需要進程一次性用完時間片，可多次使用。

    高的優先級可保證交互進程的頻繁調用，長的時間片可保證它們可長時間處於可執行狀態

           實時進程優先級：

                實時優先級分為SCHED_FIFO,SCHED_RR兩類，有軟實時硬實時之分，FIFO/RR 都有動態優先級，沒有靜態優先級。內核提供的修改優先級的函數，一般是修改rt_priority的值。rt_priority的取值範圍[1,99]。

                     prio  = MAX_RT_PRIO – 1 – rt_priority     其中MAX_RT_PRIO = 100

http://www.cnblogs.com/zhaoyl/archive/2012/09/04/2671156.html
-->

## 進程關係

在 `task_struct` 結構中，保存了一些字段，用來維護各個進程之間的關係。

在 Linux 中，線程是通過輕量級進程 (LWP) 實現，會為每個進程和線程分配一個 PID，同時我們希望由一個進程產生的輕量級進程具有相同的 PID，這樣當我們向進程發送信號時，此信號可以影響進程及進程產生的輕量級進程。

為此，採用線程組 (可以理解為輕量級進程組) 的概念，在線程組內，每個線程都使用此線程組內第一個線程 (thread group leader) 的 pid，並將此值存入tgid，當我們使用 `getpid()` 函數得到進程 ID 時，其實操作系統返回的是 `task_struct` 的 tgid 字段，而非 pid 字段。

{% highlight text %}
struct task_struct {
    pid_t pid; pid_t tgid;            // 用於標示線程和線程組ID
    struct task_struct *real_parent;  // 實際父進程real parent process
    struct task_struct *parent;       // SIGCHLD的接受者，由wait4()報告
    struct list_head children;        // 子進程列表
    struct list_head sibling;         // 兄弟進程列表
    struct task_struct *group_leader; // 線程組的leader
};
$ ps -eo  uid,pid,ppid,pgid,sid,pidns,tty,comm

getpid()[tgid]、gettid()[pid]、getppid()[real_parent]、getsid()

getuid()、getgid()、geteuid()、getegid()、getgroups()、getresuid()、getresgid()、getpgid()、<br><br>

getpgrp()/getpgid()/setpgid()        // 獲取或者設置進程組
{% endhighlight %}

`gettid()` 返回線程號，如果是單線程與 `getpid()` 相同，該值在整個 Linux 系統內是唯一的；`pthread_self()` 返回的線程號只能保證在進程中是唯一的。

![process relations]({{ site.url }}/images/linux/process-relations.jpg "process relations"){: .pull-center }

下面是內核提供的系統調用，實現有點複雜，可以通過註釋查看返回的 `task_strcut` 中的值。

Linux 中的進程組是為了方便對進程進行管理，假設要完成一個任務，需要同時併發 100 個進程，當用戶處於某種原因要終止這個任務時，可以將這些進程設置備為同一個進程組，然後向進程組發送信號。

進程必定屬於一個進程組，也只能屬於一個進程組；一個進程組中可以包含多個進程，進程組的生命週期從被創建開始，到其內所有進程終止或離開該組。

由於 Linux 是多用戶多任務的分時系統，所以必須要支持多個用戶同時使用一個操作系統，當一個用戶登錄一次系統就形成一次會話。

一個或多個進程組可以組成會話，由其中一個進程建立，該進程叫做會話的領導進程 (session leader)，會話領導進程的 PID 成為識別會話的 SID(session ID)；一個會話可包含多個進程組，但只能有一個前臺進程組。

會話中的每個進程組稱為一個作業 (job)，bash(Bourne-Again shell) 支持作業控制，而 sh(Bourne shell) 並不支持。

會話可以有一個進程組成為會話的前臺工作 (foreground)，而其他的進程組是後臺工作 (background)。每個會話可以連接一個控制終端 (control terminal)，當控制終端有輸入輸出時，都傳遞給該會話的前臺進程組，由終端產生的信號，比如 `CTRL+Z`、`CTRL+\` 會傳遞到前臺進程組。

會話的意義在於將多個工作囊括在一個終端，並取其中的一個工作作為前臺，來直接接收該終端的輸入輸出以及終端信號，其他工作在後臺運行。一般開始於用戶登錄，終止於用戶退出，此期間所有進程都屬於這個會話期。

一個工作可以通過 fg 從後臺工作變為前臺工作，如 `fg %1` 。

### 進程組和會話期

當通過 SSH 或者 telent 遠程登錄到 Linux 服務器時，如果執行一個長時間運行的任務，如果關掉窗口或者斷開連接，這個任務就會被殺掉，一切都半途而廢了，這主要是因為 `SIGHUP` 信號。

在 Linux/Unix 中，有這樣幾個概念：

* 進程組 (process group): 一個或多個進程的集合，每一個進程組有唯一一個進程組 ID ，即進程組長進程的 ID 。
* 會話期 (session): 一個或多個進程組的集合，有唯一一個會話期首進程 (session leader)，會話期 ID 為首進程的 ID 。
* 會話期可以有一個單獨的控制終端（controlling terminal）。與控制終端連接的會話期首進程叫做控制進程（controlling process）。當前與終端交互的進程稱為前臺進程組，其餘進程組稱為後臺進程組。

根據 POSIX.1 的定義，掛斷信號 (SIGHUP) 默認的動作是終止程序；當終端接口檢測到網絡連接斷開，將掛斷信號發送給控制進程 (會話期首進程)；如果會話期首進程終止，則該信號發送到該會話期前臺進程組；一個進程退出導致一個孤兒進程組中產生時，如果任意一個孤兒進程組進程處於 STOP 狀態，發送 SIGHUP 和 SIGCONT 信號到該進程組中所有進程。

因此當網絡斷開或終端窗口關閉後，控制進程收到 SIGHUP 信號退出，會導致該會話期內其他進程退出。

打開兩個 SSH 終端窗口，或者兩個 gnome-terminal，在其中一個運行 top 命令。在另一個終端窗口，找到 top 的進程 ID 為 24317 ，其父進程 ID 為 24230 ，即登錄 shell 。使用 pstree 命令可以更清楚地看到這個關係。

{% highlight text %}
# top

# ps -ef | grep top
UID        PID  PPID  C STIME    TTY       TIME CMD
andy     24317 24230  2 13:45 pts/16   00:00:05 top
andy     24526 24419  0 13:48 pts/17   00:00:00 grep --color=auto top

# pstree -H 24317 | grep top
|-sshd-+-sshd-+-sshd---bash---top
{% endhighlight %}

使用 `ps -xj` 命令可以看到，登錄 shell (PID 24230) 和 top 在同一個會話期， shell 為會話期首進程，所在進程組 PGID 為 24230， top 所在進程組 PGID 為 24317 ，為前臺進程組。

{% highlight text %}
~$ ps -xj | grep 24230
 PPID   PID  PGID   SID    TTY   TPGID STAT   UID   TIME COMMAND
24229 24230 24230 24230 pts/16   24317 Ss    1000   0:00 -bash
24230 24317 24317 24230 pts/16   24317 S+    1000   0:18 top
24419 24543 24542 24419 pts/17   24542 S+    1000   0:00 grep --color=auto 2423
{% endhighlight %}

關閉第一個 SSH 窗口，在另一個窗口中可以看到 top 也被殺掉了，如果我們可以忽略 SIGHUP 信號，關掉窗口應該就不會影響程序的運行了。

nohup 命令可以達到這個目的，如果程序的標準輸出/標準錯誤是終端， nohup 默認將其重定向到 nohup.out 文件。值得注意的是 nohup 命令只是使得程序忽略 SIGHUP 信號，還需要使用標記 `&` 把它放在後臺運行。

<!--
（1）進程必定屬於一個進程組，也只能屬於一個進程組。
     一個進程組中可以包含多個進程。
     進程組的生命週期從被創建開始，到其內所有進程終止或離開該組。

     獲取當前進程所在進程組ID使用函數getpgrp
     創建或加入其他組使用函數setpgid

（2）假設條件：pid1進程屬於pgid1進程組；pid2屬於pgid2進程組，並且是pgid2進程組組長；另有進程組pgid3，

     在pid1進程中調用setpgid（pid2，pgid3）；
     a）當pid2和pgid3都>0且不相等時
        功能：將pid2進程加入到pgid3組。此時pid2進程脫離pgid2進程組，進入pgid3進程組。
     b）當pid2和pgid3都>0且相等時
        功能：pid2進程創建新進程組，成為新進程組長（pgid3=pid2）。
     c）當pid2＝＝0，pgid>0時
        功能：將調用進程pid1加入到pgid3中。此時pid1脫離pgid1，進入pgid3。
     d）當pid2>0，pgid＝＝0時
        功能：將pid2加入到調用進程所在的pgid1進程組。此時pid2脫離pgid2，進入pgid1。
     e）當pid2和pgid3都＝＝0時，返回錯誤。

（3）一次登錄就形成一次會話，會話組長即創建會話的進程。
     只有不是進程組長的進程才能創建新會話。

（4）如果pid1進程屬於pgid1進程組，且不是組長，屬於會話sid1。
     在pid1進程中調用setsid（）；
     功能：pid1進程脫離pgid1進程組，創建一個新的會話sid2（sid2沒有控制終端），pid1進程加入到pgid2組（pgid2＝＝pid1）。
-->


## 進程狀態

Linux是一個多用戶，多任務的系統，可以同時運行多個用戶的多個程序，就必然會產生很多的進程，而每個進程會有不同的狀態，可以參考 `task_state_array[]@fs/proc/array.c` 中的內容。

{% highlight text %}
static const char * const task_state_array[] = {
    "R (running)",      /*   0 */
    "S (sleeping)",     /*   1 */
    "D (disk sleep)",   /*   2 */
    "T (stopped)",      /*   4 */
    "t (tracing stop)", /*   8 */
    "X (dead)",         /*  16 */
    "Z (zombie)",       /*  32 */
};
{% endhighlight %}

<!--
#### R (TASK_RUNNING，可執行狀態)

只有在該狀態的進程才可能在 CPU 上運行。而同一時刻可能有多個進程處於可執行狀態，這些進程的 task_struct 結構（進程控制塊）被放入對應 CPU 的可執行隊列中（一個進程最多隻能出現在一個 CPU 的可執行隊列中）。進程調度器的任務就是從各個 CPU 的可執行隊列中分別選擇一個進程在該 CPU 上運行。<br><br>

    很多教科書將正在 CPU 上執行的進程定義為 RUNNING 狀態、而將可執行但是尚未被調度執行的進程定義為 READY 狀態，這兩種狀態在 Linux 下統一為 TASK_RUNNING 狀態。</li><br><li>

    S (TASK_INTERRUPTIBLE，可中斷的睡眠狀態)<br>
    處於這個狀態的進程因為等待某某事件的發生（比如等待 socket 連接、等待信號量），而被掛起。這些進程的 task_struct 結構被放入對應事件的等待隊列中。當這些事件發生時（由外部中斷觸發、或由其他進程觸發），對應的等待隊列中的一個或多個進程將被喚醒。<br><br>

    通過 ps 命令我們會看到，一般情況下，進程列表中的絕大多數進程都處於 TASK_INTERRUPTIBLE 狀態（除非機器的負載很高）。畢竟 CPU 就這麼一兩個，進程動輒幾十上百個，如果不是絕大多數進程都在睡眠， CPU 又怎麼響應得過來。</li><br><li>

    D (TASK_UNINTERRUPTIBLE，不可中斷的睡眠狀態)<br>
    與 TASK_INTERRUPTIBLE 狀態類似，進程處於睡眠狀態，但是此刻進程是不可中斷的。不可中斷，指的並不是 CPU 不響應外部硬件的中斷，而是指進程不響應異步信號。<br><br>

    絕大多數情況下，進程處在睡眠狀態時，總是應該能夠響應異步信號的。否則你將驚奇的發現， kill -9 竟然殺不死一個正在睡眠的進程了！<br><br>

    而 TASK_UNINTERRUPTIBLE 狀態存在的意義就在於，內核的某些處理流程是不能被打斷的。如果響應異步信號，程序的執行流程中就會被插入一段用於處理異步信號的流程（這個插入的流程可能只存在於內核態，也可能延伸到用戶態），於是原有的流程就被中斷了。<br><br>

    在進程對某些硬件進行操作時（比如進程調用 read 系統調用對某個設備文件進行讀操作，而 read 系統調用最終執行到對應設備驅動的代碼，並與對應的物理設備進行交互），可能需要使用 TASK_UNINTERRUPTIBLE 狀態對進程進行保護，以避免進程與設備交互的過程被打斷，造成設備陷入不可控的狀態。這種情況下的 TASK_UNINTERRUPTIBLE 狀態總是非常短暫的，通過 ps 命令基本上不可能捕捉到。</li><br><li>

    T (TASK_STOPPED or TASK_TRACED，暫停狀態或跟蹤狀態)<br>
    向進程發送一個 SIGSTOP 信號，它就會因響應該信號而進入 TASK_STOPPED 狀態（除非該進程本身處於 TASK_UNINTERRUPTIBLE 狀態而不響應信號）。SIGSTOP 與 SIGKILL 信號一樣，是強制的，不允許用戶進程通過 signal 系列的系統調用重新設置對應的信號處理函數。<br><br>

    向進程發送一個 SIGCONT 信號，可以讓其從 TASK_STOPPED 狀態恢復到 TASK_RUNNING 狀態。<br><br>

    當進程正在被跟蹤時，它處於 TASK_TRACED 這個特殊的狀態。“正在被跟蹤”指的是進程暫停下來，等待跟蹤它的進程對它進行操作。比如在 gdb 中對被跟蹤的進程下一個斷點，進程在斷點處停下來的時候就處於 TASK_TRACED 狀態。而在其他時候，被跟蹤的進程還是處於前面提到的那些狀態。<br><br>

    對於進程本身來說， TASK_STOPPED 和 TASK_TRACED 狀態很類似，都是表示進程暫停下來。而 TASK_TRACED 狀態相當於在 TASK_STOPPED 之上多了一層保護，處於 TASK_TRACED 狀態的進程不能響應 SIGCONT 信號而被喚醒。只能等到調試進程通過 ptrace 系統調用執行 PTRACE_CONT 、 PTRACE_DETACH 等操作（通過 ptrace 系統調用的參數指定操作），或調試進程退出，被調試的進程才能恢復 TASK_RUNNING 狀態。
</li></ul>


有一類垃圾卻並非這麼容易打掃，那就是我們常見的狀態為 D (Uninterruptible sleep) ，以及狀態為 Z (Zombie) 的垃圾進程。這些垃圾進程要麼是求而不得，像怨婦一般等待資源(D)，要麼是僵而不死，像冤魂一樣等待超度(Z)，它們在 CPU run_queue 裡滯留不去，把 Load Average 弄的老高老高，沒看過我前一篇blog的國際友人還以為這兒民怨沸騰又出了什麼大事呢。怎麼辦？開槍！kill -9！看你們走是不走。但這兩種垃圾進程偏偏是刀槍不入的，不管換哪種槍法都殺不掉它們。無奈，只好reboot，像剿滅禽流感那樣不分青紅皁白地一律撲殺！

貧僧還是回來說正題。怨婦 D，往往是由於 I/O 資源得不到滿足，而引發等待，在內核源碼 fs/proc/array.c 裡，其文字定義為“ "D (disk sleep)", /* 2 */ ”（由此可知 D 原是Disk的打頭字母），對應著 include/linux/sched.h 裡的“ #define TASK_UNINTERRUPTIBLE 2 ”。舉個例子，當 NFS 服務端關閉之時，若未事先 umount 相關目錄，在 NFS 客戶端執行 df 就會掛住整個登錄會話，按 Ctrl+C 、Ctrl+Z 都無濟於事。斷開連接再登錄，執行 ps axf 則看到剛才的 df 進程狀態位已變成了 D ，kill -9 無法殺滅。正確的處理方式，是馬上恢復 NFS 服務端，再度提供服務，剛才掛起的 df 進程發現了其苦苦等待的資源，便完成任務，自動消亡。若 NFS 服務端無法恢復服務，在 reboot 之前也應將 /etc/mtab 裡的相關 NFS mount 項刪除，以免 reboot 過程例行調用 netfs stop 時再次發生等待資源，導致系統重啟過程掛起。

D是處於TASK_UNINTERRUPTIBLE的進程，深度睡眠，不響應信號。 一般只有等待非常關鍵的事件時，才把進程設為這個狀態。

　　冤魂 Z 之所以殺不死，是因為它已經死了，否則怎麼叫 Zombie（殭屍）呢？冤魂不散，自然是生前有結未解之故。在UNIX/Linux中，每個進程都有一個父進程，進程號叫PID（Process ID），相應地，父進程號就叫PPID（Parent PID）。當進程死亡時，它會自動關閉已打開的文件，捨棄已佔用的內存、交換空間等等系統資源，然後向其父進程返回一個退出狀態值，報告死訊。如果程序有 bug，就會在這最後一步出問題。兒子說我死了，老子卻沒聽見，沒有及時收棺入殮，兒子便成了殭屍。在UNIX/Linux中消滅殭屍的手段比較殘忍，執行 ps axjf 找出殭屍進程的父進程號（PPID，第一列），先殺其父，然後再由進程天子 init（其PID為1，PPID為0）來一起收拾父子殭屍，超度亡魂，往生極樂。注意，子進程變成殭屍只是礙眼而已，並不礙事，如果殭屍的父進程當前有要務在身，則千萬不可貿然殺之。
注意：不是所有狀態為Z的進程都是無法收拾的，很可能是那個短暫的狀態剛好被你發現了。
-->


### 特殊狀態處理

`TASK_STOPPED`，進程終止，通常是由於向進程發送了 `SIGSTOP`、`SIGTSTP`、`SIGTTIN`、`SIGTTOU` 信號，此時可以通過 `kill -9(SIGKILL) pid` 嘗試殺死進程，如果不起作用則 `kill -18 pid` ，也就是發個 `SIGCONT` 信號過去。


#### defunct

子進程是通過父進程創建的，子進程的結束和父進程的運行是一個異步過程，父進程永遠無法預測子進程到底什麼時候結束。當一個進程完成它的工作終止之後，它的父進程需要調用 `wait()` 或者 `waitpid()` 取得子進程的終止狀態。

孤兒進程：一個父進程退出，相應的一個或多個子進程還在運行，那麼那些子進程將成為孤兒進程。孤兒進程將被 init 進程所收養，並由 init 進程收集它們的完成狀態。注意，孤兒進程沒有危害，最終仍然回被 init 回收。

殭屍進程：一個進程使用 fork 創建子進程，如果子進程退出，而父進程並沒有調用 wait 或 waitpid 獲取子進程的狀態信息，那麼子進程的進程描述符仍然保存在系統中，仍然佔用進程表，顯示為 defunct 狀態。可以通過重啟或者殺死父進程解決。

在 Linux 中，進程退出時，內核釋放該進程所有的部分資源，包括打開的文件、佔用的內存等。但仍為其保留一定的信息，包括進程號 PID、退出的狀態、運行時間等，直到父進程通過 `wait()` 或 `waitpid()` 來獲取時才釋放。

如果父進程一直存在，那麼該進程的進程號就會一直被佔用，而系統所能使用的進程號是有限的，如果大量的產生僵死進程，將因為沒有可用的進程號而導致系統不能產生新的進程。

如下是兩個示例，分別為孤兒進程和殭屍進程。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    pid_t pid = fork();
    if (fpid &lt; 0) {
        printf("error in fork!");
        exit(1);
    }
    if (pid == 0) { // child process.
        printf("child process create, pid: %d\tppid:%d\n",getpid(),getppid());
        sleep(5);   // sleep for 5s until father process exit.
        printf("child process exit, pid: %d\tppid:%d\n",getpid(),getppid());
    } else {
        printf("father process create\n");
        sleep(1);
        printf("father process exit\n");
    }
    return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//static void sig_child(int signo)
//{
//     pid_t        pid;
//     int        stat;
//     while ((pid = waitpid(-1, &stat, WNOHANG)) &gt; 0)
//            printf("child %d terminated.\n", pid);
//}
int main ()
{
    pid_t fpid;
    // signal(SIGCHLD, sig_child);
    fpid = fork();
    if (fpid &lt; 0) {
        printf("error in fork!");
        exit(1);
    }
    if (fpid == 0) {
        printf("child process(%d)\n", getpid());
        exit(0);
    }
    printf("father process\n");
    sleep(2);
    system("ps -o pid,ppid,state,tty,command | grep defunct | grep -v grep");
    return 0;
}
{% endhighlight %}

第一個是孤兒進程，第二次輸出時其父進程 PID 變成了 `init(PID=1)`；第二個是殭屍進程，進程退出時會產生 `SIGCHLD` 信號，父進程可以通過捕獲該信號進行處理。


## 進程退出

當進程正常或異常終止時，內核就向其父進程發送 `SIGCHLD` 信號，對於 `wait()` 以及 `waitpid()` 進程可能會出現如下場景：

* 如果其所有子進程都在運行則阻塞；
* 如果某個子進程已經停止，則獲取該子進程的退出狀態並立即返回；
* 如果沒有任何子進程，則立即出錯返回。

如果進程由於接收到 `SIGCHLD` 信號而調用 wait，則一般會立即返回；但是，如果在任意時刻調用 wait 則進程可能會阻塞。

{% highlight text %}
#include <sys/wait.h>
pid_t wait(int *status);
pit_t wait(pid_t pid, int *status, int options);
{% endhighlight %}

### 獲取返回值

如果上述參數中的 status 不是 NULL，那麼會把子進程退出時的狀態返回，該返回值保存了是否為正常退出、正常結束的返回值、被那個信號終止等。

通常使用如下的宏定義。

{% highlight text %}
WIFEXITED(status)
    用於判斷子進程是否正常退出，正常退出返回0；否則返回一個非零值。

WEXITSTATUS(status)
    當WIFEXITED返回非零時，可以通過這個宏獲取子進程的返回值，如果exit(5)則返回5。
    注意，如果WIFEXITED返回零時，這個返回值則無意義。
{% endhighlight %}

可以參考如下示例。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	pid_t pid;
        int status;

	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	} else if (pid == 0) { /* child */
                exit(7);
	}

	/* parent */
	if (wait(&status) < 0) {
		perror("wait");
		exit(1);
	}

	if(WIFEXITED(status)) {
		fprintf(stdout, "Normal termination, exit status %d\n",
			WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)) {
		fprintf(stderr, "Abnormal termination, signal status %d\n",
			WTERMSIG(status),
			WCOREDUMP(status) ? "(core file)" : "");
        } else if(WIFSTOPPED(status)) {
		fprintf(stderr, "Child stopped, signal number %d\n",
			WSTOPSIG(status));
	}

        exit(0);
}

{% endhighlight %}

### waitpid()

當要等待一特定進程退出時，可調用 `waitpid()` 函數，其中第一個入參 `pid` 的入參含義如下：

* `pid=-1` 等待任一個子進程，與 wait 等效。
* `pid>0` 等待其進程 ID 與 pid 相等的子進程。
* `pid==0` 等待其進程組 ID 等於調用進程組 ID 的任一個子進程。
* `pid<-1` 等待其進程組 ID 等於 pid 絕對值的任一子進程。

`waitpid` 返回終止子進程的進程 ID，並將該子進程的終止狀態保存在 status 中。

<!---
waitpid的返回值比wait稍微複雜一些，一共有3種情況：
    當正常返回的時候，waitpid返回收集到的子進程的進程ID；
    如果設置了選項WNOHANG，而調用中waitpid發現沒有已退出的子進程可收集，則返回0；
    如果調用中出錯，則返回-1，這時errno會被設置成相應的值以指示錯誤所在；
當pid所指示的子進程不存在，或此進程存在，但不是調用進程的子進程，waitpid就會出錯返回，這時errno被設置為ECHILD；
-->




## 其它


### 指定CPU

Affinity 是進程的一個屬性，這個屬性指明瞭進程調度器能夠把這個進程調度到哪些 CPU 上，可以利用 CPU affinity 把一個或多個進程綁定到一個或多個 CPU 上。

CPU Affinity 分為 soft affinity 和 hard affinity； soft affinity 僅是一個建議，如果不可避免，調度器還是會把進程調度到其它的 CPU 上；hard affinity 是調度器必須遵守的規則。

增加 CPU 緩存的命中率。CPU 之間是不共享緩存的，如果進程頻繁的在各個 CPU 間進行切換，需要不斷的使舊 CPU 的 cache 失效。如果進程只在某個 CPU 上執行，則不會出現失效的情況。

另外，在多個線程操作的是相同的數據的情況下，如果把這些線程調度到一個處理器上，大大的增加了 CPU 緩存的命中率。但是可能會導致併發性能的降低。如果這些線程是串行的，則沒有這個影響。

適合 time-sensitive 應用，在 real-time 或 time-sensitive 應用中，我們可以把系統進程綁定到某些 CPU 上，把應用進程綁定到剩餘的 CPU 上。

CPU 集可以認為是一個掩碼，每個設置的位都對應一個可以合法調度的 CPU，而未設置的位則對應一個不可調度的 CPU。換而言之，線程都被綁定了，只能在那些對應位被設置了的處理器上運行。通常，掩碼中的所有位都被置位了，也就是可以在所有的 CPU 中調度。

另外可以通過 [schedutils](http://sourceforge.net/projects/schedutils/) 或者 python-schedutils 進行設置，後者現在更加常見。

對於如何將進程和線程綁定到指定的 CPU 可以參考 [github affinity_process.c]({{ site.example_repository }}/linux/process/affinity_process.c)、[github affinity_thread.c]({{ site.example_repository }}/linux/process/affinity_thread.c) 。

### 殺死進程的N中方法

查看進程通常可以通過如下命令

{% highlight text %}
$ ps -ef
$ ps -aux
......
smx    1827     1   4 11:38 ?        00:27:33 /usr/lib/firefox-3.6.18/firefox-bin
......
{% endhighlight %}

此時如果我想殺了火狐的進程就在終端輸入：

{% highlight text %}
$ kill -s 9 1827
{% endhighlight %}

其中 `-s 9` 制定了傳遞給進程的信號是 9，即強制、儘快終止進程。

無論是 `ps -ef` 還是 `ps -aux`，每次都要在一大串進程信息裡面查找到要殺的進程，看的眼都花了。因此通過如下的方法進行改進。

#### 使用grep

把 ps 的查詢結果通過管道給 grep 查找包含特定字符串的進程。管道符 `|` 用來隔開兩個命令，管道符左邊命令的輸出會作為管道符右邊命令的輸入。

{% highlight text %}
$ ps -ef | grep firefox
smx    1827     1   4 11:38 ?        00:27:33 /usr/lib/firefox-3.6.18/firefox-bin
smx    12029  1824  0 21:54 pts/0    00:00:00 grep --color=auto firefox
$ kill -s 9 1827
{% endhighlight %}

#### 使用pgrep

pgrep 的 p 表明了這個命令是專門用於進程查詢的 grep 。

{% highlight text %}
$ pgrep firefox
1827
$ kill -s 9 1827
{% endhighlight %}

#### 使用pidof

實際就是 pid of xx，字面翻譯過來就是 xx 的 PID ，和 pgrep 相比稍顯不足的是，pidof 必須給出進程的全名。

{% highlight text %}
$ pidof firefox-bin
1827
$kill -s 9 1827
{% endhighlight %}

無論是使用 ps 然後慢慢查找進程 PID 還是用 grep 查找包含相應字符串的進程，亦或者用 pgrep 直接查找包含相應字符串的進程 PID ，然後手動輸入給 Kill 殺掉，都稍顯麻煩。

#### 一步完成

{% highlight text %}
$ps -ef | grep firefox | grep -v grep | cut -c 9-15 | xargs kill -s 9
{% endhighlight %}

<!--
<ul><li>“grep firefox”的輸出結果是，所有含有關鍵字“firefox”的進程。</li><li>
“grep -v grep”是在列出的進程中去除含有關鍵字“grep”的進程。</li><li>
“cut -c 9-15”是截取輸入行的第9個字符到第15個字符，而這正好是進程號PID。</li><li>
“xargs kill -s 9”中的xargs命令是用來把前面命令的輸出結果（PID）作為“kill -s 9”命令的參數，並執行該命令。“kill -s 9”會強行殺掉指定進程。
-->

#### 使用 pgrep/pidof

{% highlight text %}
$ pgrep firefox | xargs kill -s 9
{% endhighlight %}

#### 使用awk

{% highlight text %}
$ ps -ef | grep firefox | awk '{print $2}' | xargs kill -9
kill: No such process
{% endhighlight %}

<!--
有一個比較鬱悶的地方，進程已經正確找到並且終止了，但是執行完卻提示找不到進程。<br><br>
其中awk '{print $2}' 的作用就是打印（print）出第二列的內容。根據常規篇，可以知道ps輸出的第二列正好是PID。就把進程相應的PID通過xargs傳遞給kill作參數，殺掉對應的進程。</li><br><li>
-->

#### 替換 xargs

{% highlight text %}
$ kill -s 9 `ps -aux | grep firefox | awk '{print $2}'`
{% endhighlight %}


#### 換成 pgrep

{% highlight text %}
$ kill -s 9 `pgrep firefox`
{% endhighlight %}


#### 使用 pkill

pkill＝pgrep+kill。

{% highlight text %}
$ pkill -9 firefox
{% endhighlight %}

說明："-9" 即發送的信號是9，pkill與kill在這點的差別是：pkill無須 “ｓ”，終止信號等級直接跟在 “-“ 後面。

#### 使用 killall

killall和pkill是相似的,不過如果給出的進程名不完整，killall會報錯。pkill或者pgrep只要給出進程名的一部分就可以終止進程。

{% highlight text %}
$ killall -9 firefox
{% endhighlight %}


### 信號量

<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="0" cellpadding="3" cellspacing="0" width="70%">
<tbody>
<tr align="center" bgcolor="lightblue">
	<td width="60">信號</td><td>說明</td><td>編號</td></tr>
	<tr><td align="center"><span class="text_import1">SIGHUP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Hangup</td><td>1</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGINT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Interrupt</td><td>2</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGQUIT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Quit and dump core</td><td>3</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGILL</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Illegal instruction</td><td>4</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTRAP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
 		Trace/breakpoint trap</td><td>5</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGABRT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Process aborted</td><td>6</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGBUS</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Bus error: "access to undefined portion of memory object"</td><td>7</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGFPE</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Floating point exception: "erroneous arithmetic operation"</td><td>8</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGKILL</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Kill (terminate immediately)</td><td>9</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGUSR1</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		User-defined 1</td><td>10</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGSEGV</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Segmentation violation</td><td>11</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGUSR2</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		User-defined 2</td><td>12</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGPIPE</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Write to pipe with no one reading</td><td>13</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGALRM</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Signal raised by alarm</td><td>14</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTERM</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Termination (request to terminate)</td><td>15</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGCHLD</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Child process terminated, stopped (or continued*)</td><td>17</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGCONT</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Continue if stopped</td><td>18</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGSTOP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Stop executing temporarily</td><td>19</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTSTP</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Terminal stop signal</td><td>20</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTTIN</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Background process attempting to read from tty ("in")</td><td>21</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGTTOU</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Background process attempting to write to tty ("out")</td><td>22</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGURG</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Urgent data available on socket</td><td>23</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGXCPU</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		CPU time limit exceeded</td><td>24</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGXFSZ</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		File size limit exceeded</td><td>25</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGVTALRM</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Signal raised by timer counting virtual time: "virtual timer expired"</td><td>26</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGPROF</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Profiling timer expired</td><td>27</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGPOLL</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Pollable event</td><td>29</td></font></tr>
	<tr><td align="center"><span class="text_import1">SIGSYS</span></td>
	<font style="margin: 5px 0 5px 0; padding-left:25px; color: #000088"><td>
		Bad syscall</td><td>31</td></font></tr>
</tbody></table></center>



## 參考

[神奇的vfork (local)](http://blog.csdn.net/ctthuangcheng/article/details/8914613)，一個針對錯誤使用 vfork() 時的場景分新。

[進程描述和進程創建](http://blog.csdn.net/zhoudaxia/article/details/7366832)，用於描述進程，其中包括了進程創建以及退出，一篇不錯的文章。


<!--
[進程的創建 do_fork() 函數詳解](http://blog.csdn.net/yunsongice/article/details/5508242)，介紹do_fork()的執行過程。

[進程切換和調度算法深入分析](http://blog.csdn.net/cxylaf/article/details/1626529)，

[Linux實時調度器](http://blog.chinaunix.net/uid-10705106-id-3541730.html) 。
-->


{% highlight text %}
{% endhighlight %}
