# 信号（下）


上一节的说了使用kill函数来发送信号和使用signal函数来安装信号处理函数，这一节我们使用另外一种方式来实现安装信号处理和发送信号。

早期UNIX只支持SIGRTMIN之前的不可靠信号，后来增加了SIGRTMIN到SIGRTMAX的可靠信号，同时也增加了信号发送和安装的方式，使用sigqueue()函数可以发送信号，使用sigaction函数可以添加信号。一般signal函数用于安装不可靠信号，sigaction用于安装可靠信号，但实际上两个函数都可以安装可靠信号和不可靠信号。

使用sigqueue函数代替kill函数发送信号：

```c
#include <signal.h>
int sigqueue(pid_t pid, int sig, const union sigval value);
```

参数pid、sig和kill函数两个参数一样，分别为发送信号目标进程id和将要发送的信号，参数value是要随信号一起发送给目标信号的数据，其类型为

```c
union sigval {
   int sival_int;
   void *sival_ptr;
};
```

如果接收信号进程使用SA_SIGINFO标识安装了sa_sigaction处理函数，那么该值在siginfo_t的成员si_value中可以获得。
使用函数sigaction可以安装一个信号处理函数：


```c
#include <signal.h>
int sigaction(int signum, const struct sigaction *act,  struct sigaction *oldact);
```

参数signum和signal函数第一个参数一样，是将要安装处理函数的的信号；
参数act是将要安装的信号处理，其是一个结构体：

```c
struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t*, void*);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};
```

第一，二个成员是信号处理函数，成员sa_handler类似signal函数的第二个参数，可以为信号处理函数或SIG_DFL或SIG_IGN。sa_sigaction有三个参数，第一个处理的信号，第二个参数为一个结构体siginfo_t类型的变量；第三个参数没有使用。结构体siginfo_t为：


```c
siginfo_t {
    int si_signo; /* Signal number */
    int si_errno; /* An errno value */
    int si_code; /* Signal code */
    int si_trapno; /* Trap number that causedhardware-generated signal(unused on most architectures) */
    pid_t si_pid; /* Sending process ID */
    uid_t si_uid; /* Real user ID of sending process */
    int si_status; /* Exit value or signal */
    clock_t si_utime; /* User time consumed */
    clock_t si_stime; /* System time consumed */
    sigval_t si_value; /* Signal value */
    int si_int; /* POSIX.1b signal */
    void* si_ptr; /* POSIX.1b signal */
    int si_overrun; /* Timer overrun count; POSIX.1b timers */
    int si_timerid; /* Timer ID; POSIX.1b timers */
    void* si_addr; /* Memory location which caused fault */
    long si_band; /* Band event (was int in glibc 2.3.2 and earlier) */
    int si_fd; /* File descriptor */
    short si_addr_lsb; /* Least significant bit of address since Linux 2.6.32) */
}
```

sigaction成员sa_mask指定在信号处理程序执行过程中，哪些信号应当被阻塞。缺省情况下当前信号本身被阻塞，防止信号的嵌套发送，除非指定SA_NODEFER或者SA_NOMASK标志位。

sa_flags中包含了许多标志位，包括刚刚提到的SA_NODEFER及SA_NOMASK标志位。另一个比较重要的标志位是SA_SIGINFO，当设定了该标志位时，表示信号附带的参数可以被传递到信号处理函数中，因此，应该为sigaction结构中的sa_sigaction指定处理函数，而不应该为sa_handler指定信号处理函数，否则，设置该标志变得毫无意义。即使为sa_sigaction指定了信号处理函数，如果不设置SA_SIGINFO，信号处理函数同样不能得到信号传递过来的数据，在信号处理函数中对这些信息的访问都将导致段错误（Segmentation fault）。

sa_restorer已经不再使用。

```c
void sig_func(int signo, siginfo_t* info, void* arg)
{
    // sleep(6);
    printf("====%s== [child] handle signo: %d==arg: %d=\n", __func__, signo,
           info->si_int);
}

void child_process_do(void)
{
    struct sigaction act;
    printf("====%s==child pid: %d===\n", __func__, getpid());
    //signal(SIGRTMIN, sig_func);
    //signal(SIGALRM, sig_func);
    act.sa_sigaction = sig_func;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGALRM, &act, NULL) < 0) { //此处安装信号处理函数
        fprintf(stderr, "sigaction: %s\n", strerror(errno));
        return;
    }

    while (1) {
        sleep(10);
    }
}

void parent_process_do(pid_t pid)
{
    int i, val = 100;
    union sigval sigarg;
    sleep(1);
    printf("====%s==parent pid: %d===\n", __func__, getpid());

    for (i = 0; i < 5; i++) {
        printf("====%s==[parent] send signal <%d> to pid <%d>==\n", __func__, SIGRTMIN,
               pid);
        //kill(pid, SIGRTMIN);
        //kill(pid, SIGALRM);
        sigarg.sival_int = val + i;
        sigqueue(pid, SIGALRM,
                 sigarg); //此处发送SIGALRM信号，sigarg为要传送的参数
        sleep(1);
    }

    waitpid(pid, NULL, 0);
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
        child_process_do();
    } else {
        parent_process_do(pid);
    }

    return 0;
}
```

不论何种安装信号处理方式，都不能就不可靠信号改为可靠信号。使用信号的方式来进行进程间通信，需要知道对方进程的pid，并且信号通信的方式所能传输的信息量不是很多，在实际应用中信号可能更多的用在进程给自身发信号，用于进程间通信不多，所以使用信号来实现IPC不如前面几节讲的通信方式来得方便。

http://blog.csdn.net/shallnet/article/details/41451815
本节源码下载