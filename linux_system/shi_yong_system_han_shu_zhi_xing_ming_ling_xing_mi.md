# 使用system()函数执行命令行命令简述

```c
#include<stdlib.h>
#include <process.h>
```

函数原型：int system(const char *command)

system函数可以调用一些DOS或者命令行命令,比如

system("cls");//清屏,等于在DOS上使用cls命令

system(“ls -al”);//清屏,等于在unix上使用ls -al命令

成功了返回0 失败了返回-1

需要注意的是：该函数对字符串的处理和c中的习惯一致，比如说：如果输出反斜杠需要 \\
例如:
system("dir c:\\windows");
注意转义字符等的限制就可以了


system()函数功能强大，很多人用却对它的原理知之甚少先看linux版system函数的源码：

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

int system(const char* cmdstring)
{
    pid_t pid;
    int status;

    if (cmdstring == NULL) {
        return (1);
    }

    if ((pid = fork()) < 0) {
        status = -1;
    } else if (pid = 0) {
        execl("/bin/sh", "sh", "-c", cmdstring, (char*)0);
        -exit(127); //子进程正常执行则不会执行此语句
    } else {
        while (waitpid(pid, &status, 0) < 0) {
            if (errno != EINTER) {
                status = -1;
                break;
            }
        }
    }

    return status;
}


```
先分析一下原理，然后再看上面的代码大家估计就能看懂了：  

当system接受的命令为NULL时直接返回，否则fork出一个子进程，因为fork在两个进程：父进程和子进程中都返回，这里要检查返回的pid，fork在子进程中返回0，在父进程中返回子进程的pid，父进程使用waitpid等待子进程结束，子进程则是调用execl来启动一个程序代替自己，execl("/bin/sh", "sh", "-c", cmdstring, (char*)0)是调用shell，这个shell的路径是/bin/sh，后面的字符串都是参数，然后子进程就变成了一个shell进程，这个shell的参数
是cmdstring，就是system接受的参数。在windows中的shell是command，想必大家很熟悉shell接受命令之后做的事了。
   
如果上面的你没有看懂，那我再解释下fork的原理：当一个进程A调用fork时，系统内核创建一个新的进程B，并将A的内存映像复制到B的进程空间中，因为A和B是一样的，那么他们怎么知道自己是父进程还是子进程呢，看fork的返回值就知道，上面也说了fork在子进程中返回0，在父进程中返回子进程的pid。

windows中的情况也类似，就是execl换了个又臭又长的名字，参数名也换的看了让人发晕的，我在MSDN中找到了原型，给大家看看：

```c
HINSTANCE   ShellExecute(
               HWND   hwnd,
               LPCTSTR   lpVerb,
               LPCTSTR   lpFile,
               LPCTSTR   lpParameters,
               LPCTSTR   lpDirectory, 
               INT   nShowCmd 
   );   
```

用法见下：

```c
ShellExecute(NULL,   "open",   "c:\\a.reg",   NULL,   NULL,   SW_SHOWNORMAL);  
```

你也许会奇怪 ShellExecute中有个用来传递父进程环境变量的参数 lpDirectory，linux中的 execl却没有，这是因为execl是编译器的函数（在一定程度上隐藏具体系统实现），在linux中它会接着产生一个linux系统的调用 execve, 原型见下：

```c
int execve(const char * file,const char **argv,const char **envp);
```

看到这里你就会明白为什么system（）会接受父进程的环境变量，但是用system改变环境变量后，system一返回主函数还是没变。原因从system的实现可以看到，它是通过产生新进程实现的，从我的分析中可以看到父进程和子进程间没有进程通信，子进程自然改变不了父进程的环境变量，DOS早死翘翘了，还是玩linux吧。



注： linux下执行系统命令用execl函数：

Linux下头文件

```c
#include <unistd.h>
int execl(const char *path, const char *arg, ...);
```

例如：
// 执行/bin目录下的ls, 第一参数为程序名ls, 第二个参数为"-al", 第三个参数为"/etc/passwd"

```c
execl("/bin/ls", "ls", "-al", "/etc/passwd", (char *) 0);
```