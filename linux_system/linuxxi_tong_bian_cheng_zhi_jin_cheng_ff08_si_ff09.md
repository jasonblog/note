# linux系统编程之进程（四）：进程退出exit，_exit区别即atexit函数


## 一，进程终止有5种方式：

- 正常退出：
    - 从main函数返回
    - 调用exit
    - 调用_exit


- 异常退出：
    - 调用abort
    - 由信号终止

##二，exit和_exit区别：


![](./images/mickole/12164358-cc25532dd77e43e59caab3472e9f319e.png)

- 关于_exit()：

```c
       #include <unistd.h>

       void _exit(int status);

       #include <stdlib.h>

       void _Exit(int status);

```
```c
DESCRIPTION 
       The function _exit() terminates the calling process "immediately".  Any 
       open file descriptors belonging to the process are closed; any children 
       of the process are inherited by process 1, init, and the process’s par- 
       ent is sent a SIGCHLD signal.

       The value status is returned to the parent  process  as  the  process’s 
       exit  status,  and  can be collected using one of the wait(2) family of 
       calls.

       The function _Exit() is equivalent to _exit().

```

- 关于exit()：

```c
#include <stdlib.h>

void exit(int status);
```

```c

DESCRIPTION 
       The  exit() function causes normal process termination and the value of 
       status & 0377 is returned to the parent (see wait(2)).

       All functions registered with atexit(3) and on_exit(3) are  called,  in 
       the  reverse  order  of their registration.  (It is possible for one of 
       these functions to use atexit(3) or on_exit(3)  to  register  an  addi- 
       tional  function  to be executed during exit processing; the new regis- 
       tration is added to the front of the list of functions that  remain  to 
       be  called.) If one of these functions does not return (e.g., it calls 
       _exit(2), or kills itself with a signal), then none  of  the  remaining 
       functions is called, and further exit processing (in particular, flush- 
       ing of stdio(3) streams) is abandoned.  If a function has  been  regis- 
       tered  multiple  times using atexit(3) or on_exit(3), then it is called 
       as many times as it was registered.

       All open stdio(3) streams are flushed and  closed.   Files  created  by 
       tmpfile(3) are removed.

       The  C standard specifies two constants, EXIT_SUCCESS and EXIT_FAILURE, 
       that may be passed to exit() to  indicate  successful  or  unsuccessful 
       termination, respectively. 
```

`和exit比较一下，exit()函数定义在stdlib.h中，而_exit()定义在unistd.h中`，

注：exit()就是退出，传入的参数是程序退出时的状态码，0表示正常退出，其他表示非正常退出，一般都用-1或者1，标准C里有EXIT_SUCCESS和EXIT_FAILURE两个宏，用exit(EXIT_SUCCESS);

- `_exit()函数的作用最为简单：直接使进程停止运行，清除其使用的内存空间，并销毁其在内核中的各种数据结构；exit() 函数则在这些基础上作了一些包装，在执行退出之前加了若干道工序。 `


- `exit()函数与_exit()函数最大的区别就在于exit()函数在调用exit系统调用之前要检查文件的打开情况，把文件缓冲区中的内容写回文件，就是"清理I/O缓冲"。`

- exit()在结束调用它的进程之前，要进行如下步骤： 
    - 调用atexit()注册的函数（出口函数）；按ATEXIT注册时相反的顺序调用所有由它注册的函数,这使得我们可以指定在程序终止时执行自己的清理动作.例如,保存程序状态信息于某个文件,解开对共享数据库上的锁等.
    - cleanup()；关闭所有打开的流，这将导致写所有被缓冲的输出，删除用TMPFILE函数建立的所有临时文件.
    - 最后调用_exit()函数终止进程。

- _exit做3件事（man）： 
    - Any  open file descriptors belonging to the process are closed 
    - any children of the process are inherited  by process 1, init 
    - the process's parent is sent a SIGCHLD signal

`exit执行完清理工作后就调用_exit来终止进程。`

## 三，atexit()
atexit可以注册终止处理程序，ANSI C规定最多可以注册32个终止处理程序。

终止处理程序的调用与注册次序相反

```c
#include <stdlib.h>

int atexit(void (*function)(void));
```

```c
DESCRIPTION 
       The atexit() function registers the given function to be called at nor- 
       mal process termination, either via exit(3) or via return from the pro- 
       gram’s main(). Functions so registered are called in the reverse order 
       of their registration; no arguments are passed.

       The  same  function may be registered multiple times: it is called once 
       for each registration.

       POSIX.1-2001 requires that an implementation allow at least  ATEXIT_MAX 
       (32) such functions to be registered.  The actual limit supported by an 
       implementation can be obtained using sysconf(3).

       When a child process is created via fork(2), it inherits copies of  its 
       parent’s  registrations.   Upon a successful call to one of the exec(3) 
       functions, all registrations are removed.

RETURN VALUE 
       The atexit() function returns the value 0 if successful;  otherwise  it 
       returns a non-zero value.
```

示例程序：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void fun1()
{
    printf("fun1 is called\n");
}

void fun2()
{
    printf("fun2 is called\n");
}

int main(void)
{
    printf("main function\n");
    atexit(fun1);
    atexit(fun2);
    atexit(fun1);
    exit(EXIT_SUCCESS);
}
```
运行结果：

![](./images/mickole/12164359-8da2be7eef5b4ae5873e25a615a30a0a.png)

当调用fork时，子进程继承父进程注册的atexit：

示例程序：
```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define ERR_EXIT(m) \
    do\
    {\
        perror(m);\
        exit(EXIT_FAILURE);\
    }\
    while (0)\

void fun1()
{
    printf("fun1 is called\n");
}

void fun2()
{
    printf("fun2 is called\n");
}
int main(void)
{
    pid_t pid;
    pid = fork();
    atexit(fun1);
    atexit(fun2);
    atexit(fun1);
    if(pid == -1)
        ERR_EXIT("fork error");
    if(pid == 0){
        printf("this is child process\n");
    }
    if(pid > 0){
        printf("this is parent process\n");
    }

    return 0;
}
```

运行结果：


![](./images/mickole/12164359-fb9c4a3d1b28464193ae697731f0e023.png)

当atexit注册的函数中有一个没有正常返回或被kill，则后续的注册函数都不会被执行

示例程序：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void fun1()
{
    printf("fun1 is called\n");
}

void fun2()
{
    printf("fun2 is called\n");
    kill(getpid(),SIGINT);
}

int main(void)
{
    printf("main function\n");
    if(signal(SIGINT,SIG_DFL) == SIG_ERR){
        perror("signal error");
        exit(EXIT_FAILURE);
    }
    atexit(fun1);
    atexit(fun2);
    atexit(fun1);
    exit(EXIT_SUCCESS);
}
```
运行结果：


![](./images/mickole/12164400-8ea08b6b9d844e819bb05c70de415127.png)

可见最后那个fun1没有执行
