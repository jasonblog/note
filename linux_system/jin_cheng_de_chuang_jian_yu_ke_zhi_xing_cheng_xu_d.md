# 进程的创建与可执行程序的加载


### 1.进程管理
Linux中的进程主要由kernel来管理。系统调用是应用程序与内核交互的一种方式。系统调用作为一种接口，通过系统调用，应用程序能够进入操作系统内核，从而使用内核提供的各种资源，比如操作硬件，开关中断，改变特权模式等等。
常见的系统调用：`exit,fork,read,write,open,close,waitpid,execve,lseek,getpid...`

###用户态和内核态
为了使操作系统提供一个很好的进程抽象，限制一个程序可以执行的指令和可以访问的地址空间。

处理器通常是使用某个控制寄存器中的一个模式位来提供这种功能，该寄存器描述了进程当前享有的特权。当设置了模式位时，进程就运行在内核态，可以执行指令集中的任何指令，并且可以访问系统中任何存储器位置。
没有设置模式位时，进程就运行在用户态，不允许执行特权指令，也不允许直接引用地址空间中内核区内的代码和数据。任何这样的尝试都会导致致命的保护故障，反之，用户程序必须通过系统调用接口间接地访问内核代码和数据。

关于fork的分析，参见这篇博文。


###waitpid

首先来了解一下僵尸进程，当一个进程由于某种原因终止时，内核并不是立即把它从系统中清除。相反，进程被保存在一种已终止的状态中，直到它的夫进程回收。当父进程回收已终止的子进程时，内核将子进程的退出状态传递给父进程，然后抛弃已终止的进程，从此时开始，该进程就不存在了。一个终止了但还未被回收的进程称为僵尸进程。

如果父进程没有回收子进程就终止了，子进程就成了僵尸进程，即时没有运行，但仍然消耗系统的存储器资源。

一个进程可以通过调用waitpid函数来等待它的子进程终止或是停止。
函数原型如下:
```sh
pid_t waitpid(pid_t pid, int *status, int options)
```

如果成功，则为子进程的PID，如果WNOHANG，则为0，如果其他错误，则为-1.
看一个waitpid函数的例子。

```c
#include"csapp.h"
#include<errno.h>
#define N 5
int main()
{
    int status, i;
    pid_t pid;

    for (i = 0; i < N; i++) {
        if ((pid = Fork()) == 0) {
            exit(100 + i);
        }
    }

    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status)) {
            printf("Child %d exited normally with status=%d!\n", pid, WIFEXITED(status));
        } else {
            printf("Child %d terminated abnormally!\n", pid);
        }
    }

    if (errno != ECHILD) {
        unix_error("waitpid error\n");
    }

    return 1;
}
```

运行结果


![](./images/20130520154759400)
![](./images/20130520193450965)
![](./images/20130520193542916)
![](./images/20130520200402518)
![](./images/20130526201730023)
![](./images/20130521153743876)
![](./images/20130526195532582)

