# linux系统编程之信号（六）：信号发送函数sigqueue和信号安装函数sigaction


## 一，sigaction()

```c
#include <signal.h> 
int sigaction(int signum,const struct sigaction *act,struct sigaction *oldact));
```

sigaction函数用于改变进程接收到特定信号后的行为。该函数的第一个参数为信号的值，可以为除SIGKILL及SIGSTOP外的任何一个特定有效的信号（为这两个信号定义自己的处理函数，将导致信号安装错误）。第二个参数是指向结构sigaction的一个实例的指针，在结构sigaction的实例中，指定了对特定信号的处理，可以为空，进程会以缺省方式对信号处理；第三个参数oldact指向的对象用来保存原来对相应信号的处理，可指定oldact为NULL。如果把第二、第三个参数都设为NULL，那么该函数可用于检查信号的有效性。

第二个参数最为重要，其中包含了对指定信号的处理、信号所传递的信息、信号处理函数执行过程中应屏蔽掉哪些函数等等。

sigaction结构定义如下：

```c
struct sigaction {
    union {
        __sighandler_t _sa_handler;
        void (*_sa_sigaction)(int, struct siginfo*, void*)；
    } _u
    sigset_t sa_mask；
    unsigned long sa_flags；
    void (*sa_restorer)(void)；
}
```
其中，sa_restorer，已过时，POSIX不支持它，不应再被使用。

1、联合数据结构中的两个元素_sa_handler以及*_sa_sigaction指定信号关联函数，即用户指定的信号处理函数。除了可以是用户自定义的处理函数外，还可以为SIG_DFL(采用缺省的处理方式)，也可以为SIG_IGN（忽略信号）。

2、由_sa_handler指定的处理函数只有一个参数，即信号值，所以信号不能传递除信号值之外的任何信息；由_sa_sigaction是指定的信号处理函数带有三个参数，是为实时信号而设的（当然同样支持非实时信号），它指定一个3参数信号处理函数。第一个参数为信号值，第三个参数没有使用（posix没有规范使用该参数的标准），第二个参数是指向siginfo_t结构的指针，结构中包含信号携带的数据值，参数所指向的结构如下：

```c
typedef struct siginfo_t {
    int si_signo;//信号编号
    int si_errno;//如果为非零值则错误代码与之关联
    int si_code;//说明进程如何接收信号以及从何处收到
    pid_t si_pid;//适用于SIGCHLD，代表被终止进程的PID
    pid_t si_uid;//适用于SIGCHLD,代表被终止进程所拥有进程的UID
    int si_status;//适用于SIGCHLD，代表被终止进程的状态
    clock_t si_utime;//适用于SIGCHLD，代表被终止进程所消耗的用户时间
    clock_t si_stime;//适用于SIGCHLD，代表被终止进程所消耗系统的时间
    sigval_t si_value;
    int si_int;
    void* si_ptr;
    void* si_addr;
    int si_band;
    int si_fd;
};
```

siginfo_t结构中的联合数据成员确保该结构适应所有的信号，比如对于实时信号来说，则实际采用下面的结构形式：

```c
typedef struct {
    int si_signo;
    int si_errno;
    int si_code;
    union sigval si_value;
} siginfo

```

结构的第四个域同样为一个联合数据结构：

```c
union sigval {
    int sival_int;
    void* sival_ptr;
}
```
采用联合数据结构，说明siginfo_t结构中的si_value要么持有一个4字节的整数值，要么持有一个指针，这就构成了与信号相关的数据。在信号的处理函数中，包含这样的信号相关数据指针，但没有规定具体如何对这些数据进行操作，操作方法应该由程序开发人员根据具体任务事先约定。

sigval结构体：系统调用sigqueue发送信号时，sigqueue的第三个参数就是sigval联合数据结构，当调用sigqueue时，该数据结构中的数据就将拷贝到信号处理函数的第二个参数中。这样，在发送信号同时，就可以让信号传递一些附加信息。信号可以传递信息对程序开发是非常有意义的。

`siginfo_t.si_value与sigqueue(pid_t pid, int sig, const union sigval val)第三个参数关联即：`

`所以通过siginfo_t.si_value可以获得sigqueue(pid_t pid, int sig, const union sigval val)第三个参数传递过来的数据。`

`如：siginfo_t.si_value.sival_int或siginfo_t.si_value.sival_ptr`

`其实siginfo_t.si_int直接与sigval.sival_int关联`

`siginfo_t.si_ptr直接与sigval.sival_ptr关联，所以也可同这种方式获得sigqueue发送过来的数据。`


信号参数的传递过程可图示如下：