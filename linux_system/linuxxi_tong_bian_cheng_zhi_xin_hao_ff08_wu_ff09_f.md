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