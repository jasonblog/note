# linux系统编程之进程（六）：父进程查询子进程的退出,wait,waitpid


- 本节目标：
    - 僵进程
    - SIGCHLD
    - wait
    - waitpid


##一，僵尸进程
当一个子进程先于父进程结束运行时，它与其父进程之间的关联还会保持到父进程也正常地结束运行，或者父进程调用了wait才告终止。

子进程退出时，内核将子进程置为僵尸状态，这个进程称为僵尸进程，它只保留最小的一些内核数据结构，以便父进程查询子进程的退出状态。

进程表中代表子进程的数据项是不会立刻释放的，虽然不再活跃了，可子进程还停留在系统里，因为它的退出码还需要保存起来以备父进程中后续的wait调用使用。它将称为一个“僵进程”。

##二，如何避免僵尸进程
调用wait或者waitpid函数查询子进程退出状态，此方法父进程会被挂起。
如果不想让父进程挂起，可以在父进程中加入一条语句：signal(SIGCHLD,SIG_IGN);表示父进程忽略SIGCHLD信号，该信号是子进程退出的时候向父进程发送的。

## 三，SIGCHLD信号
当子进程退出的时候，内核会向父进程发送SIGCHLD信号，子进程的退出是个异步事件（子进程可以在父进程运行的任何时刻终止）

如果不想让子进程编程僵尸进程可在父进程中加入：signal(SIGCHLD,SIG_IGN);

如果将此信号的处理方式设为忽略，可让内核把僵尸子进程转交给init进程去处理，省去了大量僵尸进程占用系统资源。

示例：

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main(void)
{
    pid_t pid;
    if(signal(SIGCHLD,SIG_IGN) == SIG_ERR)
    {
        perror("signal error");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if(pid == -1)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    if(pid == 0)
    {
        printf("this is child process\n");
        exit(0);
    }
    if(pid > 0)
    {
        sleep(100);
        printf("this is parent process\n");
    }
    return 0;
}
```
![](./images/mickole/13113018-881e2058fb294bcb805836350d6f3aa0.png)
![](./images/mickole/)
![](./images/mickole/)
![](./images/mickole/)
![](./images/mickole/)
![](./images/mickole/)
![](./images/mickole/)
![](./images/mickole/)
