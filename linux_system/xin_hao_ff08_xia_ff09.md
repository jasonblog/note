# 信號（下）


上一節的說了使用kill函數來發送信號和使用signal函數來安裝信號處理函數，這一節我們使用另外一種方式來實現安裝信號處理和發送信號。

早期UNIX只支持SIGRTMIN之前的不可靠信號，後來增加了SIGRTMIN到SIGRTMAX的可靠信號，同時也增加了信號發送和安裝的方式，使用sigqueue()函數可以發送信號，使用sigaction函數可以添加信號。一般signal函數用於安裝不可靠信號，sigaction用於安裝可靠信號，但實際上兩個函數都可以安裝可靠信號和不可靠信號。

使用sigqueue函數代替kill函數發送信號：

```c
#include <signal.h>
int sigqueue(pid_t pid, int sig, const union sigval value);
```

參數pid、sig和kill函數兩個參數一樣，分別為發送信號目標進程id和將要發送的信號，參數value是要隨信號一起發送給目標信號的數據，其類型為

```c
union sigval {
   int sival_int;
   void *sival_ptr;
};
```

如果接收信號進程使用SA_SIGINFO標識安裝了sa_sigaction處理函數，那麼該值在siginfo_t的成員si_value中可以獲得。
使用函數sigaction可以安裝一個信號處理函數：


```c
#include <signal.h>
int sigaction(int signum, const struct sigaction *act,  struct sigaction *oldact);
```

參數signum和signal函數第一個參數一樣，是將要安裝處理函數的的信號；
參數act是將要安裝的信號處理，其是一個結構體：

```c
struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t*, void*);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};
```

第一，二個成員是信號處理函數，成員sa_handler類似signal函數的第二個參數，可以為信號處理函數或SIG_DFL或SIG_IGN。sa_sigaction有三個參數，第一個處理的信號，第二個參數為一個結構體siginfo_t類型的變量；第三個參數沒有使用。結構體siginfo_t為：


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

sigaction成員sa_mask指定在信號處理程序執行過程中，哪些信號應當被阻塞。缺省情況下當前信號本身被阻塞，防止信號的嵌套發送，除非指定SA_NODEFER或者SA_NOMASK標誌位。

sa_flags中包含了許多標誌位，包括剛剛提到的SA_NODEFER及SA_NOMASK標誌位。另一個比較重要的標誌位是SA_SIGINFO，當設定了該標誌位時，表示信號附帶的參數可以被傳遞到信號處理函數中，因此，應該為sigaction結構中的sa_sigaction指定處理函數，而不應該為sa_handler指定信號處理函數，否則，設置該標誌變得毫無意義。即使為sa_sigaction指定了信號處理函數，如果不設置SA_SIGINFO，信號處理函數同樣不能得到信號傳遞過來的數據，在信號處理函數中對這些信息的訪問都將導致段錯誤（Segmentation fault）。

sa_restorer已經不再使用。

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

    if (sigaction(SIGALRM, &act, NULL) < 0) { //此處安裝信號處理函數
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
                 sigarg); //此處發送SIGALRM信號，sigarg為要傳送的參數
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

不論何種安裝信號處理方式，都不能就不可靠信號改為可靠信號。使用信號的方式來進行進程間通信，需要知道對方進程的pid，並且信號通信的方式所能傳輸的信息量不是很多，在實際應用中信號可能更多的用在進程給自身發信號，用於進程間通信不多，所以使用信號來實現IPC不如前面幾節講的通信方式來得方便。

http://blog.csdn.net/shallnet/article/details/41451815
本節源碼下載