# linux系统编程之进程（五）：exec系列函数（execl,execlp,execle,execv,execvp)使用


- 本节目标：
    - exec替换进程映像
    - exec关联函数组（execl、execlp、execle、execv、execvp）
    
##一，exec替换进程映像
在进程的创建上Unix采用了一个独特的方法，它将进程创建与加载一个新进程映象分离。这样的好处是有更多的余地对两种操作进行管理。

当我们创建了一个进程之后，通常将子进程替换成新的进程映象，这可以用exec系列的函数来进行。当然，exec系列的函数也可以将当前进程替换掉。

例如：在shell命令行执行ps命令，实际上是shell进程调用fork复制一个新的子进程，在利用exec系统调用将新产生的子进程完全替换成ps进程。

##二，exec系列函数（execl、execlp、execle、execv、execvp）

```c
包含头文件<unistd.h>
```
功能：

用exec函数可以把当前进程替换为一个新进程，且新进程与原进程有相同的PID。exec名下是由多个关联函数组成的一个完整系列，

```c
头文件<unistd.h>
extern char **environ; 
```

原型：

```c
int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
```
参数：

path参数表示你要启动程序的名称包括路径名

arg参数表示启动程序所带的参数，一般第一个参数为要执行命令名，不是带路径且arg必须以NULL结束

返回值:成功返回0,失败返回-1

注：上述exec系列函数底层都是通过execve系统调用实现：
```c
#include <unistd.h>
int execve(const char *filename, char *const argv[],char *const envp[]);
```
```c
DESCRIPTION： 
       execve() executes the program pointed to by filename.  filename must be 
       either a binary executable, or a script starting with  a  line  of  the form 
```

以上exec系列函数区别：

1，带l 的exec函数：execl,execlp,execle，表示后边的参数以可变参数的形式给出且都以一个空指针结束。

示例：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("entering main process---\n");
    execl("/bin/ls","ls","-l",NULL);
    printf("exiting main process ----\n");
    return 0;
}
```

![](./images/mickole/12235435-4e7e41be4be34c82afeaedba02d13ace.png)

利用execl将当前进程main替换掉，所有最后那条打印语句不会输出

2，带 p 的exec函数：execlp,execvp，表示第一个参数path不用输入完整路径，只有给出命令名即可，它会在环境变量PATH当中查找命令

示例：

当不带p但没给出完整路径时：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("entering main process---\n");
    execl("/bin/ls","ls","-l",NULL);
    printf("exiting main process ----\n");
    return 0;
}
```
结果：

![](./images/mickole/12235435-2e6e368cb2ac4bdfac36d5b811526b9f.png)

结果显示找不到，所有替换不成功，main进程继续执行

现在带p：


![](./images/mickole/12235436-7f8e4291fcd74aff9c83bc763870ec8e.png)

替换成功

3，不带 l 的exec函数：execv,execvp表示命令所需的参数以char *arg[]形式给出且arg最后一个元素必须

是NULL

示例：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    printf("entering main process---\n");
    int ret;
    char *argv[] = {"ls","-l",NULL};
    ret = execvp("ls",argv);
    if(ret == -1)
        perror("execl error");
    printf("exiting main process ----\n");
    return 0;
}
```
结果：

![](./images/mickole/12235437-7c2b8a69045d4275a4558d207fdb68b4.png)

进程替换成功

4，带 e 的exec函数：execle表示，将环境变量传递给需要替换的进程

从上述的函数原型中我们发现：

extern char **environ;

此处的environ是一个指针数组，它当中的每一个指针指向的char为“XXX=XXX”

environ保存环境信息的数据可以env命令查看：

![](./images/mickole/12235437-b4a4514a1b9d4aae93d03258cc152d5b.png)

它由shell进程传递给当前进程，再由当前进程传递给替换的新进程

示例：execle.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    //char * const envp[] = {"AA=11", "BB=22", NULL};
    printf("Entering main ...\n");
    int ret;
    ret =execl("./hello", "hello", NULL);
    //execle("./hello", "hello", NULL, envp);
    if(ret == -1)
        perror("execl error");
    printf("Exiting main ...\n");
    return 0;
}
```

- hello.c

```c
#include <unistd.h>
#include <stdio.h>
extern char** environ;

int main(void)
{
    printf("hello pid=%d\n", getpid());
    int i;
    for (i=0; environ[i]!=NULL; ++i)
    {
        printf("%s\n", environ[i]);
    }
    return 0;
}
```
结果：

![](./images/mickole/12235438-9c91c25d61c14c8daff173f42c0038d2.png)

可知原进程确实将环境变量信息传递给了新进程

那么现在我们可以利用execle函数自己给的需要传递的环境变量信息：

示例程序：execle.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char * const envp[] = {"AA=11", "BB=22", NULL};
    printf("Entering main ...\n");
    int ret;
    //ret =execl("./hello", "hello", NULL);
    ret =execle("./hello", "hello", NULL, envp);
    if(ret == -1)
        perror("execl error");
    printf("Exiting main ...\n");
    return 0;
}
```

- hello.c

```c
#include <unistd.h>
#include <stdio.h>
extern char** environ;

int main(void)
{
    printf("hello pid=%d\n", getpid());
    int i;
    for (i=0; environ[i]!=NULL; ++i)
    {
        printf("%s\n", environ[i]);
    }
    return 0;
}
```
结果：

![](./images/mickole/12235439-76c7f522edcf4424a2d7eeeecf9b32db.png)

确实将给定的环境变量传递过来了

### 三，fcntl()函数中的FD_CLOEXEC标识在exec系列函数中的作用

```c
#include <unistd.h> 
#include <fcntl.h>

int fcntl(int fd, int cmd, ... /* arg */ );
```

```c
File descriptor flags 
      The following commands manipulate the  flags  associated  with  a  file 
      descriptor.   Currently, only one such flag is defined: FD_CLOEXEC, the 
      close-on-exec flag.  If the FD_CLOEXEC bit is 0,  the  file  descriptor 
      will remain open across an execve(2), otherwise it will be closed.

     //如果FD_CLOEXEC标识位为0，则通过execve调用后fd依然是打开的，否则为关闭的

      F_GETFD (void) 
             Read the file descriptor flags; arg is ignored.

      F_SETFD (long) 
             Set the file descriptor flags to the value specified by arg.

```

如：fcntl(fd, F_SETFD, FD_CLOEXEC);

测试示例：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
int main(int argc, char *argv[])
{

    printf("Entering main ...\n");
    int ret = fcntl(1, F_SETFD, FD_CLOEXEC);
    if (ret == -1)
        perror("fcntl error");
    int val;
    val =execlp("ls", "ls","-l", NULL);
    if(val == -1)
        perror("execl error");
    printf("Exiting main ...\n");
    return 0;
}
```

结果：


![](./images/mickole/12235439-31a996502f604f0bb6e9de2d8042106f.png)

1关闭（标准输出关闭）ls -l无法将结果显示在标准输出
