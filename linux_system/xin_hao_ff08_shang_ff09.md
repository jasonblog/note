# 信號（上）


信號類似於中斷請求，一個進程不會阻塞在某處等待信號的到來，也不會知道信號何時能到來，信號的產生是隨機的，進程只需要註冊信號處理函數，在信號到來時執行信號處理函數即可。

linux系統支持的信號可以通過命令kill -l來查看：

```sh
# kill -l 
1) SIGHUP 2) SIGINT 3) SIGQUIT 4) SIGILL 5) SIGTRAP 
6) SIGABRT 7) SIGBUS 8) SIGFPE 9) SIGKILL 10) SIGUSR1 
11) SIGSEGV 12) SIGUSR2 13) SIGPIPE 14) SIGALRM 15) SIGTERM 
16) SIGSTKFLT 17) SIGCHLD 18) SIGCONT 19) SIGSTOP 20) SIGTSTP 
21) SIGTTIN 22) SIGTTOU 23) SIGURG 24) SIGXCPU 25) SIGXFSZ 
26) SIGVTALRM 27) SIGPROF 28) SIGWINCH 29) SIGIO 30) SIGPWR 
31) SIGSYS 34) SIGRTMIN 35) SIGRTMIN+1 36) SIGRTMIN+2 37) SIGRTMIN+3 
38) SIGRTMIN+4 39) SIGRTMIN+5 40) SIGRTMIN+6 41) SIGRTMIN+7 42) SIGRTMIN+8 
43) SIGRTMIN+9 44) SIGRTMIN+10 45) SIGRTMIN+11 46) SIGRTMIN+12 47) SIGRTMIN+13 
48) SIGRTMIN+14 49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-13 52) SIGRTMAX-12 
53) SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9 56) SIGRTMAX-8 57) SIGRTMAX-7 
58) SIGRTMAX-6 59) SIGRTMAX-5 60) SIGRTMAX-4 61) SIGRTMAX-3 62) SIGRTMAX-2 
63) SIGRTMAX-1 64) SIGRTMAX 
```

其中SIGRTMIN 前面信號是早期UNIX支持的信號，是不可靠信號，後面信號（SIGRTMIN 到SIGRTMAX ）為後期添加，為可靠信號。在linux上信號是否可靠主要體現在信號是否支持排隊，不支持排隊的信號可能會丟失。

信號的`產生`可能有很多，比如：

```sh
1. 用戶按下CTRL+C是產生中斷信號（SIGINT ）；
2. 硬件異常產生信號，比如除數為0，非法訪問內存（SIGSEGV ）等；
3. 使用系統命令發送信號，如kill -s <signo> <pid>，將發送值為signo的信號給進程號為pid的進程；
4.調用kill函數等其他可以發送信號的函數，給指定進程發送信號。可以發送信號的函數有：alarm（）和setitimer（）發送SIGALRM信號，abort（）發送SIGABRT 信號，raise給進程自身發送任意信號，kill函數給其它進程或自身進程發送任意信號。
```

進程`收到`信號處理方式有三種：

```
1. 忽略信號；大多數信號可以忽略，但兩種信號除外：SIGKILl和SIGSTOP。
2.執行系統默認操作；大部分信號的默認操作是終止該進程。
3.捕捉信號並執行信號處理函數；即在收到信號時，執行用戶自己定義的函數。
```

註冊信號處理函數可以使用函數signal：

```c
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```

參數signum是將要註冊處理函數的信號，handler是對信號（參數一）的處理函數或SIG_IGN或SIG_DFL，如果為參數SIG_DFL，進程收到信號時將執行默認操作，如果為SIG_IGN時進程將忽略信號，如果為信號處理函數，進程收到信號時會執行該函數，並將參數一signum傳給該函數；

發送信號可以使用kill函數：

```c
#include <sys/types.h>
#include <signal.h>
int kill(pid_t pid, int sig);
```

參數pid為要發送給信號的目標進程id，sig為將要發送的信號，當為0時（即空信號），實際不發送任何信號，但照常進行錯誤檢查，因此，可用於檢查目標進程是否存在，以及當前進程是否具有向目標發送信號的權限（root權限的進程可以向任何進程發送信號，非root權限的進程只能向屬於同一個session或者同一個用戶的進程發送信號）。

<table border="1" cellpadding="2" cellspacing="0" width="60%" style="font-size:undefined; border-collapse:collapse; margin-top:10px; margin-bottom:10px">
<thead style="background-color:inherit">
<tr style="background-color:inherit">
<th style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
參數pid的值</th>
<th style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
信號的接收進程</th>
</tr>
</thead>
<tbody style="background-color:inherit">
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
pid&gt;0</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
進程ID為pid的進程</td>
</tr>
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
pid=0</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
同一個進程組的進程</td>
</tr>
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
pid&lt;-1</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
進程組ID為 -pid的所有進程</td>
</tr>
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:29px; background-color:inherit">
pid=-1</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:29px; background-color:inherit">
除進程1外，所有有向目標進程發送信號權限的進程</td>
</tr>
</tbody>
</table>

既然是說IPC，那就舉一個父子進程通過信號來實現簡單通信的程序，即父進程向子進程發送信號，子進程收到信號後執行信號處理函數。


```c
void sig_func(int signo)
{
    printf("====%s== [child] handle signo: %d===\n", __func__, signo);
}

void child_process_do(void)
{
    printf("====%s==child pid: %d===\n", __func__, getpid());

    signal(SIGRTMIN, sig_func); //子進程註冊信號SIGRTMIN處理函數sig_func
    //signal(SIGALRM, sig_func);

    while (1) {
        sleep(10);
    }
}

void parent_process_do(pid_t pid)
{
    int i;

    sleep(1); //等待子進程先註冊完信號處理函數，否則系統如果先調用父進程先執行，在調用kill之後子進程會退出，看不到我們需要的效果
    printf("====%s==parent pid: %d===\n", __func__, getpid());

    for (i = 0; i < 5; i++) {
        printf("====%s==[parent] send signal <%d> to pid <%d>==\n", __func__, SIGRTMIN,
               pid);
        kill(pid, SIGRTMIN); //父進程連續每隔一秒向子進程發一個SIGRTMIN信號
        //kill(pid, SIGALRM);
        sleep(1);
    }

    waitpid(pid, NULL, 0); //父進程等待子進程結束
}


int main(int argc, const char* argv[])
{
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork: %s\n", strerror(errno));
        return -1;
    }

    if (0 == pid) {
        child_process_do(); //子進程
    } else {
        parent_process_do(pid); //父進程
    }

    return 0;
}
```

運行時，父進程發5個信號給子進程，子進程收到並處理。

```sh
# ./target_bin
====child_process_do==child pid: 7072===
====parent_process_do==parent pid: 7071===
====parent_process_do==[parent] send signal <34> to pid <7072>==
====sig_func== [child] handle signo: 34===
====parent_process_do==[parent] send signal <34> to pid <7072>==
====sig_func== [child] handle signo: 34===
====parent_process_do==[parent] send signal <34> to pid <7072>==
====sig_func== [child] handle signo: 34===
====parent_process_do==[parent] send signal <34> to pid <7072>==
====sig_func== [child] handle signo: 34===
====parent_process_do==[parent] send signal <34> to pid <7072>==
====sig_func== [child] handle signo: 34===
```

現在在該程序稍作修改，讓子進程在處理信號處理函數時隨眠一段時間，這裡我們可以測試一下該信號是否支持排隊。

將信號處理函數修改為如下：

```c
void sig_func(int signo)
{
    sleep(6);
    printf("====%s== [child] handle signo: %d===\n", __func__, signo);
} 
```

重新運行：

```sh
# ./target_bin
====child_process_do==child pid: 7159===
====parent_process_do==parent pid: 7158===
====parent_process_do==[parent] send signal <34> to pid <7159>==
====parent_process_do==[parent] send signal <34> to pid <7159>==
====parent_process_do==[parent] send signal <34> to pid <7159>==
====parent_process_do==[parent] send signal <34> to pid <7159>==
====parent_process_do==[parent] send signal <34> to pid <7159>==
====sig_func== [child] handle signo: 34===
====sig_func== [child] handle signo: 34===
====sig_func== [child] handle signo: 34===
====sig_func== [child] handle signo: 34===
====sig_func== [child] handle signo: 34===
```

可以發現信號SIGRTMIN支持排隊，在父進程已經發送完所有信號之後，子進程仍然可以開始處理父進程之前所發全部信號，信號沒有丟失。現在在修改一下代碼，測試SIGRTMIN之前的信號是否支持排隊，我們將子進程和父進程代碼修改如下：

```c
void child_process_do(void)
{
    printf("====%s==child pid: %d===\n", __func__, getpid());

    //signal(SIGRTMIN, sig_func);
    signal(SIGALRM, sig_func);

    while (1) {
        sleep(10);
    }
}

void parent_process_do(pid_t pid)
{
    int i;

    sleep(1);
    printf("====%s==parent pid: %d===\n", __func__, getpid());

    for (i = 0; i < 5; i++) {
        printf("====%s==[parent] send signal <%d> to pid <%d>==\n", __func__, SIGRTMIN,
               pid);
        //kill(pid, SIGRTMIN);
        kill(pid, SIGALRM);
        sleep(1);
    }

    waitpid(pid, NULL, 0);
}
```

執行結果如下：

```sh
# ./target_bin
====child_process_do==child pid: 7259===
====parent_process_do==parent pid: 7258===
====parent_process_do==[parent] send signal <34> to pid <7259>==
====parent_process_do==[parent] send signal <34> to pid <7259>==
====parent_process_do==[parent] send signal <34> to pid <7259>==
====parent_process_do==[parent] send signal <34> to pid <7259>==
====parent_process_do==[parent] send signal <34> to pid <7259>==
====sig_func== [child] handle signo: 14===
====sig_func== [child] handle signo: 14===
```

發送後面四個信號時，子進程正在忙於處理第一個信號，當子進程處理完第一個時，父進程已經發送完了所有信號，由於SIGALRM不支持排隊，緩存裡面只能存放一個信號，中間三個信號丟失，子進程最終只捕捉到2個信號。

我們再次修改子進程函數，打算讓子進程在10秒之後退出，父進程在子進程退出自後收到子進程結束信號也將在waitpid處返回最後退出。將子進程代碼修改如下：

```sh
void sig_func(int signo)
{
    //sleep(6);
    printf("====%s== [child] handle signo: %d===\n", __func__, signo);
}

void child_process_do(void)
{
    printf("====%s==child pid: %d===\n", __func__, getpid());

    //signal(SIGRTMIN, sig_func);
    signal(SIGALRM, sig_func);

    /*
    while (1) {
        sleep(10);
    }
    */
    sleep(10);
}
```

執行如下：

```sh
# ./target_bin
====child_process_do==child pid: 7450===
====parent_process_do==parent pid: 7449===
====parent_process_do==[parent] send signal <34> to pid <7450>==
====sig_func== [child] handle signo: 14===
====parent_process_do==[parent] send signal <34> to pid <7450>==
====parent_process_do==[parent] send signal <34> to pid <7450>==
====parent_process_do==[parent] send signal <34> to pid <7450>==
====parent_process_do==[parent] send signal <34> to pid <7450>==
```

結果似乎有些出乎意料，子進程接收到一個信號時就退出了，但我們有sleep 10秒鐘，這是為什麼呢？實際上信號導致了sleep的返回，所以在子進程收到第一個信號時就退出了，所以沒有看到子進程處理後面的信號。我們可以判斷sleep的返回值，當它的返回值不等於0時，我們繼續讓它睡眠。所以我們將子進程吃力函數改為：


```c
void child_process_do(void)
{
    printf("====%s==child pid: %d===\n", __func__, getpid());

    //signal(SIGRTMIN, sig_func);
    signal(SIGALRM, sig_func);

    /*
    while (1) {
        sleep(10);
    }
    */
    while (sleep(10) > 0) {
    }
}
```

這樣執行結果就是我們預期的了。
這種方式安裝的信號通信只能做到一個進程向另一個進程發送信號，但不能攜帶信息進行數據交換，下一節我們來看看另外一種信號安裝方式，它可以在發送信號的同時傳遞一些信息。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/8193955

