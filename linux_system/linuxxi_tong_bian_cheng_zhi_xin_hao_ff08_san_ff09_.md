# linux系统编程之信号（三）：信号安装、signal、kill，arise讲解


## 一，信号安装
如果进程要处理某一信号，那么就要在进程中安装该信号。安装信号主要用来确定信号值及进程针对该信号值的动作之间的映射关系，即进程将要处理哪个信号；该信号被传递给进程时，将执行何种操作。

linux主要有两个函数实现信号的安装：signal()、sigaction()。其中signal()只有两个参数，不支持信号传递信息，主要是用于前32种非实时信号的安装；而sigaction()是较新的函数（由两个系统调用实现：sys_signal以及sys_rt_sigaction），有三个参数，支持信号传递信息，主要用来与 sigqueue() 系统调用配合使用，当然，sigaction()同样支持非实时信号的安装。sigaction()优于signal()主要体现在支持信号带有参数。

## 二，signal()用法

```c
#include <signal.h>
typedef void (*__sighandler_t) (int);
#define SIG_ERR ((__sighandler_t) -1)
#define SIG_DFL ((__sighandler_t) 0)
#define SIG_IGN ((__sighandler_t) 1)
void (*signal(int signum, void (*handler))(int)))(int);
```

如果该函数原型不容易理解的话，可以参考下面的分解方式来理解：

```c
typedef void (*sighandler_t)(int)；
sighandler_t signal(int signum, sighandler_t handler));
```

第一个参数指定信号的值，第二个参数指定针对前面信号值的处理，可以忽略该信号（参数设为SIG_IGN）；可以采用系统默认方式处理信号(参数设为SIG_DFL)；也可以自己实现处理方式(参数指定一个函数地址)。

如果signal()调用成功，返回最后一次也就是上一次为安装信号signum而调用signal()时的handler值；失败则返回SIG_ERR。

传递给信号处理例程的整数参数是信号值，这样可以使得一个信号处理例程处理多个信号。

man帮助说明：

```c
DESCRIPTION 
       The behavior of signal() varies across Unix versions, and has also var- 
       ied historically across different versions of Linux.   Avoid  its  use: 
       use sigaction(2) instead.  See Portability below.

       signal() sets the disposition of the signal signum to handler, which is 
       either SIG_IGN, SIG_DFL, or the address of a  programmer-defined  func- 
       tion (a "signal handler").

       If  the signal signum is delivered to the process, then one of the fol- 
       lowing happens:

       *  If the disposition is set to SIG_IGN, then the signal is ignored.

       *  If the disposition is set to SIG_DFL, then the default action  asso- 
          ciated with the signal (see signal(7)) occurs.

       *  If  the disposition is set to a function, then first either the dis- 
          position is reset to SIG_DFL, or the signal is blocked  (see  Porta- 
          bility  below), and then handler is called with argument signum.  If 
          invocation of the handler caused the signal to be blocked, then  the

          signal is unblocked upon return from the handler.

       The signals SIGKILL and SIGSTOP cannot be caught or ignored.

RETURN VALUE 
       signal()  returns  the previous value of the signal handler, or SIG_ERR 
       on error. 
```

示例程序：

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void sig_handler(int signo);
int main(void)
{
    printf("mian is waiting for a signal\n");
    if(signal(SIGINT,sig_handler) == SIG_ERR){
        perror("signal errror");
        exit(EXIT_FAILURE);
    }
    for(; ;);//有时间让我们发送信号


    return 0;
}

void sig_handler(int signo)
{
    printf("catch the signal SIGINT %d\n",signo);
}
```


结果：