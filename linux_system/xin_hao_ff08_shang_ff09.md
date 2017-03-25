# 信号（上）


信号类似于中断请求，一个进程不会阻塞在某处等待信号的到来，也不会知道信号何时能到来，信号的产生是随机的，进程只需要注册信号处理函数，在信号到来时执行信号处理函数即可。

linux系统支持的信号可以通过命令kill -l来查看：

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

其中SIGRTMIN 前面信号是早期UNIX支持的信号，是不可靠信号，后面信号（SIGRTMIN 到SIGRTMAX ）为后期添加，为可靠信号。在linux上信号是否可靠主要体现在信号是否支持排队，不支持排队的信号可能会丢失。

信号的`产生`可能有很多，比如：

```sh
1. 用户按下CTRL+C是产生中断信号（SIGINT ）；
2. 硬件异常产生信号，比如除数为0，非法访问内存（SIGSEGV ）等；
3. 使用系统命令发送信号，如kill -s <signo> <pid>，将发送值为signo的信号给进程号为pid的进程；
4.调用kill函数等其他可以发送信号的函数，给指定进程发送信号。可以发送信号的函数有：alarm（）和setitimer（）发送SIGALRM信号，abort（）发送SIGABRT 信号，raise给进程自身发送任意信号，kill函数给其它进程或自身进程发送任意信号。
```

进程`收到`信号处理方式有三种：

```
1. 忽略信号；大多数信号可以忽略，但两种信号除外：SIGKILl和SIGSTOP。
2.执行系统默认操作；大部分信号的默认操作是终止该进程。
3.捕捉信号并执行信号处理函数；即在收到信号时，执行用户自己定义的函数。
```

注册信号处理函数可以使用函数signal：

```c
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```

参数signum是将要注册处理函数的信号，handler是对信号（参数一）的处理函数或SIG_IGN或SIG_DFL，如果为参数SIG_DFL，进程收到信号时将执行默认操作，如果为SIG_IGN时进程将忽略信号，如果为信号处理函数，进程收到信号时会执行该函数，并将参数一signum传给该函数；

发送信号可以使用kill函数：

```c
#include <sys/types.h>
#include <signal.h>
int kill(pid_t pid, int sig);
```

参数pid为要发送给信号的目标进程id，sig为将要发送的信号，当为0时（即空信号），实际不发送任何信号，但照常进行错误检查，因此，可用于检查目标进程是否存在，以及当前进程是否具有向目标发送信号的权限（root权限的进程可以向任何进程发送信号，非root权限的进程只能向属于同一个session或者同一个用户的进程发送信号）。

<table border="1" cellpadding="2" cellspacing="0" width="60%" style="font-size:undefined; border-collapse:collapse; margin-top:10px; margin-bottom:10px">
<thead style="background-color:inherit">
<tr style="background-color:inherit">
<th style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
参数pid的值</th>
<th style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
信号的接收进程</th>
</tr>
</thead>
<tbody style="background-color:inherit">
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
pid&gt;0</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
进程ID为pid的进程</td>
</tr>
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
pid=0</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
同一个进程组的进程</td>
</tr>
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
pid&lt;-1</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:25px; background-color:inherit">
进程组ID为 -pid的所有进程</td>
</tr>
<tr style="background-color:inherit">
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:29px; background-color:inherit">
pid=-1</td>
<td style="word-break:break-all; border:1px solid rgb(153,153,153); padding:5px 10px; min-height:25px; min-width:25px; height:29px; background-color:inherit">
除进程1外，所有有向目标进程发送信号权限的进程</td>
</tr>
</tbody>
</table>

既然是说IPC，那就举一个父子进程通过信号来实现简单通信的程序，即父进程向子进程发送信号，子进程收到信号后执行信号处理函数。


```c
void sig_func(int signo)
{
    printf("====%s== [child] handle signo: %d===\n", __func__, signo);
}

void child_process_do(void)
{
    printf("====%s==child pid: %d===\n", __func__, getpid());

    signal(SIGRTMIN, sig_func); //子进程注册信号SIGRTMIN处理函数sig_func
    //signal(SIGALRM, sig_func);

    while (1) {
        sleep(10);
    }
}

void parent_process_do(pid_t pid)
{
    int i;

    sleep(1); //等待子进程先注册完信号处理函数，否则系统如果先调用父进程先执行，在调用kill之后子进程会退出，看不到我们需要的效果
    printf("====%s==parent pid: %d===\n", __func__, getpid());

    for (i = 0; i < 5; i++) {
        printf("====%s==[parent] send signal <%d> to pid <%d>==\n", __func__, SIGRTMIN,
               pid);
        kill(pid, SIGRTMIN); //父进程连续每隔一秒向子进程发一个SIGRTMIN信号
        //kill(pid, SIGALRM);
        sleep(1);
    }

    waitpid(pid, NULL, 0); //父进程等待子进程结束
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
        child_process_do(); //子进程
    } else {
        parent_process_do(pid); //父进程
    }

    return 0;
}
```

运行时，父进程发5个信号给子进程，子进程收到并处理。

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

现在在该程序稍作修改，让子进程在处理信号处理函数时随眠一段时间，这里我们可以测试一下该信号是否支持排队。

将信号处理函数修改为如下：

```c
void sig_func(int signo)
{
    sleep(6);
    printf("====%s== [child] handle signo: %d===\n", __func__, signo);
} 
```

重新运行：

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

可以发现信号SIGRTMIN支持排队，在父进程已经发送完所有信号之后，子进程仍然可以开始处理父进程之前所发全部信号，信号没有丢失。现在在修改一下代码，测试SIGRTMIN之前的信号是否支持排队，我们将子进程和父进程代码修改如下：

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

执行结果如下：

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

发送后面四个信号时，子进程正在忙于处理第一个信号，当子进程处理完第一个时，父进程已经发送完了所有信号，由于SIGALRM不支持排队，缓存里面只能存放一个信号，中间三个信号丢失，子进程最终只捕捉到2个信号。

我们再次修改子进程函数，打算让子进程在10秒之后退出，父进程在子进程退出自后收到子进程结束信号也将在waitpid处返回最后退出。将子进程代码修改如下：

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

执行如下：

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

结果似乎有些出乎意料，子进程接收到一个信号时就退出了，但我们有sleep 10秒钟，这是为什么呢？实际上信号导致了sleep的返回，所以在子进程收到第一个信号时就退出了，所以没有看到子进程处理后面的信号。我们可以判断sleep的返回值，当它的返回值不等于0时，我们继续让它睡眠。所以我们将子进程吃力函数改为：


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

这样执行结果就是我们预期的了。
这种方式安装的信号通信只能做到一个进程向另一个进程发送信号，但不能携带信息进行数据交换，下一节我们来看看另外一种信号安装方式，它可以在发送信号的同时传递一些信息。

本节源码下载：
http://download.csdn.net/detail/gentleliu/8193955

