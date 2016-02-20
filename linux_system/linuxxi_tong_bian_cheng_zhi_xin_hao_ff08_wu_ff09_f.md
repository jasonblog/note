# linux系统编程之信号（五）：信号集操作函数，信号阻塞与未决


## 一，信号集及相关操作函数
信号集被定义为一种数据类型：

```c
typedef struct {
    unsigned long sig[_NSIG_WORDS]；
} sigset_t
```

信号集用来描述信号的集合，每个信号占用一位（64位）。Linux所支持的所有信号可以全部或部分的出现在信号集中，主要与信号阻塞相关函数配合使用。下面是为信号集操作定义的相关函数：

```c
#include <signal.h>

int sigemptyset(sigset_t *set)；

int sigfillset(sigset_t *set)；

int sigaddset(sigset_t *set, int signum)

int sigdelset(sigset_t *set, int signum)；

int sigismember(const sigset_t *set, int signum)；
```
 
```
sigemptyset(sigset_t *set)初始化由set指定的信号集，信号集里面的所有信号被清空，相当于64为置0；

sigfillset(sigset_t *set)调用该函数后，set指向的信号集中将包含linux支持的64种信号，相当于64为都置1；

sigaddset(sigset_t *set, int signum)在set指向的信号集中加入signum信号，相当于将给定信号所对应的位置1；

sigdelset(sigset_t *set, int signum)在set指向的信号集中删除signum信号，相当于将给定信号所对应的位置0；

sigismember(const sigset_t *set, int signum)判定信号signum是否在set指向的信号集中，相当于检查给定信号所对应的位是0还是1。
```

示例程序：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
void print_sigset(sigset_t *set);
int main(void)
{
    sigset_t myset;
    sigemptyset(&myset);
    sigaddset(&myset,SIGINT);
    sigaddset(&myset,SIGQUIT);
    sigaddset(&myset,SIGUSR1);
    sigaddset(&myset,SIGRTMIN);
    print_sigset(&myset);

    return 0;

}
void print_sigset(sigset_t *set)
{
    int i;
    for(i = 1; i < NSIG; ++i){
        if(sigismember(set,i))
            printf("1");
        else
            printf("0");
    }
    putchar('\n');
}
```

结果：

![](./images/mickole/15154939-5338c18bf100462facfc7389b4cbd62b.png)

可以看到添加信号的相应位置1.

## 二，信号阻塞与未决

man帮助说明：

```c
Signal mask and pending signals
       A signal may be blocked, which means that it will not be delivered
       until it is later unblocked.  Between the time when it is generated
       and when it is delivered a signal is said to be pending.

       Each thread in a process has an independent signal mask, which
       indicates the set of signals that the thread is currently blocking.
       A thread can manipulate its signal mask using pthread_sigmask(3).  In
       a traditional single-threaded application, sigprocmask(2) can be used
       to manipulate the signal mask.
```

执行信号的处理动作称为信号递达（Delivery），信号从产生到递达之间的状态，称为信号未决（Pending）。进程可以选择阻塞（Block）某个信号。被阻塞的信号产生时将保持在未决状态，直到进程解除对此信号的阻塞，才执行递达的动作。注意，阻塞和忽略是不同的，只要信号被阻塞就不会递达，而忽略是在递达之后可选的一种处理动作。每个进程都有一个用来描述哪些信号递送到进程时将被阻塞的信号集，该信号集中的所有信号在递送到进程后都将被阻塞。
信号在内核中的表示可以看作是这样的：
![](./images/mickole/15154939-8bed754413d541949f099cf6c1dc2fc5.png)

看图说话：
block集（阻塞集、屏蔽集）：一个进程所要屏蔽的信号，在对应要屏蔽的信号位置1
pending集（未决信号集）：如果某个信号在进程的阻塞集中，则也在未决集中对应位置1，表示该信号不能被递达，不会被处理
handler（信号处理函数集）：表示每个信号所对应的信号处理函数，当信号不在未决集中时，将被调用
 
以下是与信号阻塞及未决相关的函数操作：

```c
#include <signal.h>

int sigprocmask(int  how,  const  sigset_t *set, sigset_t *oldset))；

int sigpending(sigset_t *set));

int sigsuspend(const sigset_t *mask))；
```
 
- sigprocmask()函数能够根据参数how来实现对信号集的操作，操作主要有三种：
    - SIG_BLOCK 在进程当前阻塞信号集中添加set指向信号集中的信号，相当于：mask=mask|set
    - SIG_UNBLOCK 如果进程阻塞信号集中包含set指向信号集中的信号，则解除对该信号的阻塞，相当于：mask=mask|~set
    - SIG_SETMASK 更新进程阻塞信号集为set指向的信号集，相当于mask=set
    

sigpending(sigset_t *set))获得当前已递送到进程，却被阻塞的所有信号，在set指向的信号集中返回结果。

sigsuspend(const sigset_t *mask))用于在接收到某个信号之前, 临时用mask替换进程的信号掩码, 并暂停进程执行，直到收到信号为止。

sigsuspend 返回后将恢复调用之前的信号掩码。信号处理函数完成后，进程将继续执行。该系统调用始终返回-1，并将errno设置为EINTR。

示例程序：

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
void printsigset(sigset_t *set)
{
    int i;
    for (i=1; i<NSIG; ++i)
    {
        if (sigismember(set, i))
            putchar('1');
        else
            putchar('0');
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    sigset_t pset;
    sigset_t bset;
    sigemptyset(&bset);
    sigaddset(&bset, SIGINT);
    if (signal(SIGINT, handler) == SIG_ERR)
        ERR_EXIT("signal error");
    if (signal(SIGQUIT, handler) == SIG_ERR)
        ERR_EXIT("signal error");

    sigprocmask(SIG_BLOCK, &bset, NULL);//将信号加入进程阻塞集中
    for (;;)
    {
        sigpending(&pset);
        printsigset(&pset);
        sleep(1);
    }
    return 0;
}

void handler(int sig)
{
    if (sig == SIGINT)
        printf("recv a sig=%d\n", sig);
    else if (sig == SIGQUIT)
    {
        sigset_t uset;
        sigemptyset(&uset);
        sigaddset(&uset, SIGINT);
        sigprocmask(SIG_UNBLOCK, &uset, NULL);
    }
}
```

结果：


![](./images/mickole/15154940-680c08c9e5d84c6fa2648c067be4cd70.png)

说明：程序首先将SIGINT信号加入进程阻塞集（屏蔽集）中，一开始并没有发送SIGINT信号，所以进程未决集中没有处于未决态的信号，当我们连续按下ctrl+c时，向进程发送SIGINT信号，由于SIGINT信号处于进程的阻塞集中，所以发送的SIGINT信号不能递达，也是就是处于未决状态，所以当我打印未决集合时发现SIGINT所对应的位为1，现在我们按下ctrl+\，发送SIGQUIT信号，由于此信号并没被进程阻塞，所以SIGQUIT信号直接递达，执行对应的处理函数，在该处理函数中解除进程对SIGINT信号的阻塞，所以之前发送的SIGINT信号递达了，执行对应的处理函数，但由于SIGINT信号是不可靠信号，不支持排队，所以最终只有一个信号递达。

```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <unistd.h>

/* 版本1, 可靠信号将被递送多次 */
#define MYSIGNAL SIGRTMIN+5
/* 版本2, 不可靠信号只被递送一次 */
//#define MYSIGNAL SIGTERM

void sig_handler(int signum)
{
    psignal(signum, "catch a signal");
}

int main(int argc, char **argv)
{
    sigset_t block, pending;
    int sig, flag;

    /* 设置信号的handler */
    signal(MYSIGNAL, sig_handler);

    /* 屏蔽此信号 */
    sigemptyset(&block);
    sigaddset(&block, MYSIGNAL);
    printf("block signal\n");
    sigprocmask(SIG_BLOCK, &block, NULL);

    /* 发两次信号, 看信号将会被触发多少次 */
    printf("---> send a signal --->\n");
    kill(getpid(), MYSIGNAL);
    printf("---> send a signal --->\n");
    kill(getpid(), MYSIGNAL);

    /* 检查当前的未决信号 */
    flag = 0;
    sigpending(&pending);
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(&pending, sig)) {
            flag = 1;
            psignal(sig, "this signal is pending");
        } 
    }
    if (flag == 0) {
        printf("no pending signal\n");
    }

    /* 解除此信号的屏蔽, 未决信号将被递送 */
    printf("unblock signal\n");
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    /* 再次检查未决信号 */
    flag = 0;
    sigpending(&pending);
    for (sig = 1; sig < NSIG; sig++) {
        if (sigismember(&pending, sig)) {
            flag = 1;
            psignal(sig, "a pending signal");
        } 
    }
    if (flag == 0) {
        printf("no pending signal\n");
    }

    return 0;
}
```

结果：



![](./images/mickole/15154941-796b58cf14884f3bb3d6fc9277641cbf.png)

两次执行结果不同：第一次连续发送两次不可靠信号，最后解除阻塞时，只有一个递达，说明不可靠信号不支持排队。

第二次执行时，连续两次发送可靠信号，解除阻塞后，都递达，说明可靠信号支持排队。

ok，这节就写到这吧