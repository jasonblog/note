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


![](./images/mickole/0_13131295725nwM.gif)

3、sa_mask指定在信号处理程序执行过程中，哪些信号应当被阻塞。缺省情况下当前信号本身被阻塞，防止信号的嵌套发送，除非指定SA_NODEFER或者SA_NOMASK标志位，处理程序执行完后，被阻塞的信号开始执行。

注：请注意sa_mask指定的信号阻塞的前提条件，是在由sigaction（）安装信号的处理函数执行过程中由sa_mask指定的信号才被阻塞。

4、sa_flags中包含了许多标志位，包括刚刚提到的SA_NODEFER及SA_NOMASK标志位。另一个比较重要的标志位是SA_SIGINFO，当设定了该标志位时，表示信号附带的参数可以被传递到信号处理函数中，因此，应该为sigaction结构中的sa_sigaction指定处理函数，而不应该为sa_handler指定信号处理函数，否则，设置该标志变得毫无意义。即使为sa_sigaction指定了信号处理函数，如果不设置SA_SIGINFO，信号处理函数同样不能得到信号传递过来的数据，在信号处理函数中对这些信息的访问都将导致段错误（Segmentation fault）。

注：很多文献在阐述该标志位时都认为，如果设置了该标志位，就必须定义三参数信号处理函数。实际不是这样的，验证方法很简单：自己实现一个单一参数信号处理函数，并在程序中设置该标志位，可以察看程序的运行结果。实际上，可以把该标志位看成信号是否传递参数的开关，如果设置该位，则传递参数；否则，不传递参数。

##二，sigqueue()

之前学过kill,raise,alarm,abort等功能稍简单的信号发送函数，现在我们学习一种新的功能比较强大的信号发送函数sigqueue.

```c
#include <sys/types.h>
#include <signal.h>
int sigqueue(pid_t pid, int sig, const union sigval val)
```

调用成功返回 0；否则，返回 -1。

sigqueue()是比较新的发送信号系统调用，主要是针对实时信号提出的（当然也支持前32种），支持信号带有参数，与函数sigaction()配合使用。

sigqueue的第一个参数是指定接收信号的进程ID，第二个参数确定即将发送的信号，第三个参数是一个联合数据结构union sigval，指定了信号传递的参数，即通常所说的4字节值。

```c
typedef union sigval {
    int  sival_int;
    void* sival_ptr;
} sigval_t;
```

sigqueue()比kill()传递了更多的附加信息，但sigqueue()只能向一个进程发送信号，而不能发送信号给一个进程组。如果signo=0，将会执行错误检查，但实际上不发送任何信号，0值信号可用于检查pid的有效性以及当前进程是否有权限向目标进程发送信号。

在调用sigqueue时，sigval_t指定的信息会拷贝到对应sig 注册的3参数信号处理函数的siginfo_t结构中，这样信号处理函数就可以处理这些信息了。由于sigqueue系统调用支持发送带参数信号，所以比kill()系统调用的功能要灵活和强大得多。

##三，sigqueue与sigaction应用实例
实例一：利用sigaction安装SIGINT信号

```c
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

void handler(int sig);

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    //因为不关心SIGINT上一次的struct sigaction所以，oact为NULL
    //与signal(handler,SIGINT)相同
    if (sigaction(SIGINT, &act, NULL) < 0)
        ERR_EXIT("sigaction error\n");

    for (;;)
        pause();
    return 0;
}

void handler(int sig)
{
    printf("recv a sig=%d\n", sig);
}
```

结果：

![](./images/mickole/15192104-5f531ba85b4f496f9a51f5c42b96d7ce.png)

实例二：利用sigaction实现signal，实际上signal底层实现就是利用sigaction

```c
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

void handler(int sig);
__sighandler_t my_signal(int sig, __sighandler_t handler);

int main(int argc, char *argv[])
{
    my_signal(SIGINT, handler);
    for (;;)
        pause();
    return 0;
}

__sighandler_t my_signal(int sig, __sighandler_t handler)
{
    struct sigaction act;
    struct sigaction oldact;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(sig, &act, &oldact) < 0)
        return SIG_ERR;

    return oldact.sa_handler;
}

void handler(int sig)
{
    printf("recv a sig=%d\n", sig);
}
```

结果：



![](./images/mickole/15192104-80ffc43833ae4d34b190710202e86c18.png)

可知my_signal与系统调用signal具有相同的效果

实例三：验证sigaction.sa_mask效果

```c
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#define ERR_EXIT(m) \
    do \
    { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while(0)

void handler(int sig);

int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) < 0)
        ERR_EXIT("sigaction error");

    struct sigaction act2;
    act2.sa_handler = handler;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;

    if (sigaction(SIGQUIT, &act2, NULL) < 0)
        ERR_EXIT("sigaction error");

    for (;;)
        pause();
    return 0;
}

void handler(int sig)
{
    if(sig == SIGINT){

        printf("recv a SIGINT signal\n");
        sleep(5);
    }
    if (sig == SIGQUIT)
    {
        printf("recv a SIGQUIT signal\n");
    }
}
```

结果：



![](./images/mickole/15192105-8b8baf15acfd4ebbbbb4761b7aacf2b1.png)

可知，安装信号SIGINT时，将SIGQUIT加入到sa_mask阻塞集中，则当SIGINT信号正在执行处理函数时，SIGQUIT信号将被阻塞，只有当SIGINT信号处理函数执行完后才解除对SIGQUIT信号的阻塞，由于SIGQUIT是不可靠信号，不支持排队，所以只递达一次

示例四：给自身发送int型数据

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void sighandler(int signo, siginfo_t *info,void *ctx);
//给自身传递信息
int main(void)
{

    struct sigaction act;
    act.sa_sigaction = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;//信息传递开关
    if(sigaction(SIGINT,&act,NULL) == -1){
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
    sleep(2);
    union sigval mysigval;
    mysigval.sival_int = 100;
    if(sigqueue(getpid(),SIGINT,mysigval) == -1){
        perror("sigqueue error");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void sighandler(int signo, siginfo_t *info,void *ctx)
{
    //以下两种方式都能获得sigqueue发来的数据
    printf("receive the data from siqueue by info->si_int is %d\n",info->si_int);
    printf("receive the data from siqueue by info->si_value.sival_int is %d\n",info->si_value.sival_int);

}
```

结果：

![](./images/mickole/15192106-f58fa5147535489fa93ca157e2eea255.png)

示例五：进程间传递数据

接收端：

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void sighandler(int signo, siginfo_t *info,void *ctx);
//给自身传递信息
int main(void)
{

    struct sigaction act;
    act.sa_sigaction = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;//信息传递开关
    if(sigaction(SIGINT,&act,NULL) == -1){
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
    for(; ;){
        printf("waiting a SIGINT signal....\n");
        pause();
    }
    return 0;
}

void sighandler(int signo, siginfo_t *info,void *ctx)
{
    //以下两种方式都能获得sigqueue发来的数据
    printf("receive the data from siqueue by info->si_int is %d\n",info->si_int);
    printf("receive the data from siqueue by info->si_value.sival_int is %d\n",info->si_value.sival_int);

}
```

发送端：

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if(argc != 2){
        fprintf(stderr,"usage:%s pid\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    pid_t pid = atoi(argv[1]);    
    sleep(2);
    union sigval mysigval;
    mysigval.sival_int = 100;
    printf("sending SIGINT signal to %d......\n",pid);
    if(sigqueue(pid,SIGINT,mysigval) == -1){
        perror("sigqueue error");
        exit(EXIT_FAILURE);
    }
    return 0;
}
```

结果：

![](./images/mickole/15192107-710bc1936127483d92d46c875332e7f9.png)

由图可知接收成功