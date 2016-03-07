# 进程的创建与可执行程序的加载


## 1.进程管理
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

waitpid的第一个参数是-1，则等待集合由父进程的所有子进程组成。大于0的话就是等待进程的pid。 

waitpid的第三个参数是-1，则waitpid会挂起调用进程的执行，直到它的等待集合的一个子进程终止。如果等待集合中的一个进程终止了，那么waitpid就立即返回。

程序运行的结果就是waitpid函数不按照特定的顺序回收僵死的子进程。

提一下wait函数，它就是waitpid函数的简单版本，原型如下：

```sh
pid_t wait(int *status)
```

等价于waitpid(-1, &status, 0)

### execve
在Linux中要使用exec函数族来在 一个进程中启动另一个程序。系统调用execve（）对当前进程进行替换，替换者为一个指定的程序，其参数包括文件名（filename）、参数列表（argv）以及环境变量（envp）。exec函数族当然不止一个，但它们大致相同，在 Linux中，它们分别是`：execl，execlp，execle，execv，execve和execvp`，下面我只以execve为例，其它函数究竟与execlp有何区别，请通过man exec命令来了解它们的具体情况。


一个进程一旦调用exec类函数，它本身就"死亡"了，系统把代码段替换成新的程序的代码，废弃原有的数据段和堆栈段，并为新程序分配新的数据段与堆栈段，唯一留下的，就是进程号，也就是说，对系统而言，还是同一个进程，不过已经是另一个程序了。（不过exec类函数中有的还允许继承环境变量之类的信息。）
原型如下：

```sh
int execve(const char *filename, const char *argv[], const char *envp[]);
```

成功调用不会返回，出错返回-1.
execve函数加载并运行可执行目标文件filename,且带参数列表argv和环境变量列表envp.只有当出现错误时，例如找不到filename，execve才会返回到调用程序。所以，与fork一次调用返回两次，execve调用一次并从不返回。
argv的在内存中组织方式如下图：

![](./images/20130520193450965)

argv[0]是可执行目标文件的名字。
envp的在内存中组织方式如下图：


![](./images/20130520193542916)

环境变量的列表是由一个和指针数组类似的数据结构表示，envp变量指向一个以null结尾的指针数组，其中每个指针指向一个环境变量串，其中每个串都是形如“NAME=VALUE”的键值对。
可以用下面的命令来打印命令行参数和环境变量：

```c
#include"csapp.h"

int main(int argc, char* argv[], char* envp[])
{
    int i;

    printf("Command line arguments:\n");

    for (i = 0; argv[i] != NULL; i++) {
        printf("argv[%2d]: %s\n", i, argv[i]);
    }

    printf("\n");
    printf("Environment variables:\n");

    for (i = 0; envp[i] != NULL; i++) {
        printf("envp[%2d]: %s\n", i, envp[i]);
    }

    exit(0);
}
```

![](./images/20130520200402518)

##2.简单的shell

结合上面的fork，wait和exec，下面来实现一个简单shell。
先搭建一个shell框架，步骤是读取一个来自用户的命令行，求值并解析命令行。

```c
#include<stdio.h>
#include"csapp.h"
#define MAXARGS 128
void eval(char* cmdline);
int parseline(char* buf, char** argv);
int builtin_command(char** argv);

int main()
{
    char cmdline[MAXLINE];

    while (1) {
        printf("> ");
        Fgets(cmdline, MAXLINE, stdin);

        if (feof(stdin)) {
            exit(0);
        }

        eval(cmdline);
    }

    //printf("Hello\n");
    return 1;
}

int builtin_command(char** argv)
{
    if (!strcmp(argv[0], "quit")) {
        exit(0);
    }

    if (!strcmp(argv[0], "&")) {
        return 1;
    }

    if (!strcmp(argv[0], "-help")) {
        printf("-help    help infomation.\n");
        printf("ls       list files and folders of current path.\n");
        printf("pwd      show current path.\n");
        return 1;
    }

    if (!strcmp(argv[0], "pwd")) {
        printf("%s\n", getcwd(NULL, 0));
        return 1;
    }

    return 0;
}

void eval(char* cmdline)
{
    char* argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if (argv[0] == NULL) {
        return;
    }

    if (!builtin_command(argv)) {
        if ((pid = Fork()) == 0) {
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s:Command not found.\n", argv[0]);
                exit(0);
            }
        }

        if (!bg) {
            int status;

            if (waitpid(pid, &status, 0) < 0) {
                unix_error("waitfg:waitpid error");
            }
        } else {
            printf("%d %s", pid, cmdline);
        }
    }

    return;
}

int parseline(char* buf, char** argv)
{
    char* delim;
    int argc;
    int bg;
    buf[strlen(buf) - 1] = ' ';

    while (*buf && (*buf == ' ')) {
        buf++;
    }

    argc = 0;

    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;

        while (*buf && (*buf == ' ')) {
            buf++;
        }
    }

    argv[argc] = NULL;

    if (argc == 0) {
        return 1;
    }

    bg = (*argv[argc - 1] == '&');

    if (bg != 0) {
        argv[--argc] = NULL;
    }

    return bg;
}
```
解释一下代码。

主要的几个函数：
```sh
eval：解释收到的命令。
parseline：解析以空格分隔的命令行参数，并构造argv传递给execve，执行相应的程序。
builtin_command:  检测参数是否为shell的内建命令，如果是，就立即解释这个命令，并返回1，否则返回0.
```

下面用通过一些System Call，实现几个linux的常用命令。

###ls
显示当前路径下的文件和文件夹信息。
c代码实现

```c
#include<stdio.h>
#include<time.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<pwd.h>
#include<grp.h>
void do_ls(char[]);
void dostat(char*);
void show_file_info(char*, struct stat*);
void mode_to_letters(int, char[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);

void main(int argc, char* argv[])
{
    if (argc == 1) {
        do_ls(".");
    } else {
        printf("Error input\n");
    }
}

void do_ls(char dirname[])
{
    DIR* dir_ptr;   //Path
    struct dirent* direntp;     //Struct to save next file node

    if ((dir_ptr = opendir(dirname)) == 0) {
        fprintf(stderr, "ls:cannot open %s\n", dirname);
    } else {
        while ((direntp = readdir(dir_ptr)) != 0) {
            dostat(direntp->d_name);
        }

        closedir(dir_ptr);
    }
}

void dostat(char* filename)
{
    struct stat info;

    if (lstat(filename, &info) == -1) {
        perror("lstat");
    } else {
        show_file_info(filename, &info);
    }
}

void show_file_info(char* filename, struct stat* info_p)
{
    char modestr[11];
    mode_to_letters(info_p->st_mode, modestr);
    printf("%-12s", modestr);
    printf("%-4d", (int)info_p->st_nlink);
    printf("%-8s", uid_to_name(info_p->st_uid));
    printf("%-8s", gid_to_name(info_p->st_gid));
    printf("%-8ld", (long)info_p->st_size);
    time_t timelong = info_p->st_mtime;
    struct tm* htime = localtime(&timelong);
    printf("%-4d-%02d-%02d %02d:%02d", htime->tm_year + 1990, htime->tm_mon + 1,
           htime->tm_mday, htime->tm_hour, htime->tm_min);
    printf(" %s\n", filename);
}

//cope with permission
void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");

    if (S_ISDIR(mode)) {
        str[0] = 'd';
    }

    if (S_ISCHR(mode)) {
        str[0] = 'c';
    }

    if (S_ISBLK(mode)) {
        str[0] = 'b';
    }

    if (mode & S_IRUSR) {
        str[1] = 'r';
    }

    if (mode & S_IWUSR) {
        str[2] = 'w';
    }

    if (mode & S_IXUSR) {
        str[3] = 'x';
    }

    if (mode & S_IRGRP) {
        str[4] = 'r';
    }

    if (mode & S_IWGRP) {
        str[5] = 'w';
    }

    if (mode & S_IXGRP) {
        str[6] = 'x';
    }

    if (mode & S_IROTH) {
        str[7] = 'r';
    }

    if (mode & S_IWOTH) {
        str[8] = 'w';
    }

    if (mode & S_IXOTH) {
        str[9] = 'x';
    }
}

//transfor uid to username
char* uid_to_name(uid_t uid)
{
    struct passwd* pw_str;
    static char numstr[10];

    if ((pw_str = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    } else {
        return pw_str->pw_name;
    }
}

//transfor gid to username
char* gid_to_name(gid_t gid)
{
    struct group* grp_ptr;
    static char numstr[10];

    if ((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    } else {
        return grp_ptr->gr_name;
    }
}
```
实现思路：

主要是do_ls函数，通过opendir命令打开文件夹，然后用readdir来读取文件夹中的文件或文件夹，输出信息。

通过刚才的shell调用编译好ls程序，效果如下：


![](./images/20130526201730023)

## 3.信号
软中断信号（signal，又简称为信号）用来通知进程发生了异步事件。进程之间可以互相通过系统调用kill发送软中断信号。内核也可以因为内部事件而给进程发送信号，通知进程发生了某个事件。注意，信号只是用来通知某进程发生了什么事件，并不给该进程传递任何数据。

收到信号的进程对各种信号有不同的处理方法。处理方法可以分为三类：第一种是类似中断的处理程序，对于需要处理的信号，进程可以指定处理函数，由该函数来处 理。第二种方法是，忽略某个信号，对该信号不做任何处理，就象未发生过一样。第三种方法是，对该信号的处理保留系统的默认值，这种缺省操作，对大部分的信 号的缺省操作是使得进程终止。进程通过系统调用signal来指定进程对某个信号的处理行为。 比如一个进程可以通过向另一个进程发送SIGKILL信号强制终止它。当一个子进程终止或者停止时，内核会发送一个SIGCHLD给父进程。

信号有很多种，每种信号类型都对应于某种系统事件。信号的处理流程如下：

![](./images/20130521153743876)

定义信号的接受处理函数原型如下:

```c
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
Returns: ptr to previous handler if OK, SIG_ERR on error (does not set errno)
```

看一个接受信号的例子:


```c
#include "csapp.h"

/* SIGINT handler */
void handler(int sig)
{
    return; /* Catch the signal and return */
}

unsigned int snooze(unsigned int secs)
{
    unsigned int rc = sleep(secs);
    printf("Slept for %u of %u secs.\n", secs - rc, secs);
    return rc;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <secs>\n", argv[0]);
        exit(0);
    }

    if (signal(SIGINT, handler) == SIG_ERR) { /* Install SIGINT handler */
        unix_error("signal error\n");
    }

    (void)snooze(atoi(argv[1]));
    exit(0);
}
```

程序解析：

程序接受一个int参数，用于设置sleep的秒数，正常情况下sleep相应的秒数之后就自动退出程序，由于注册了SIGINI，当按下键盘的Ctrl+C键的时候，跳转到handler函数，处理信号。

## 4.动态链接和静态链接
库有动态与静态两种，动态通常用.so为后缀，静态用.a为后缀。例如：libhello.so libhello.a 
为了在同一系统中使用不同版本的库，可以在库文件名后加上版本号为后缀,例如： libhello.so.1.0,由于程序连接默认以.so为文件后缀名。所以为了使用这些库，通常使用建立符号连接的方式。

```sh
ln -s libhello.so.1.0 libhello.so.1 
ln -s libhello.so.1 libhello.so 
```

使用库 
当 要使用静态的程序库时，连接器会找出程序所需的函数，然后将它们拷贝到执行文件，由于这种拷贝是完整的，所以一旦连接成功，静态程序库也就不再需要了。然 而，对动态库而言，就不是这样。动态库会在执行程序内留下一个标记‘指明当程序执行时，首先必须载入这个库。由于动态库节省空间，linux下进行连接的 缺省操作是首先连接动态库，也就是说，如果同时存在静态和动态库，不特别指定的话，将与动态库相连接。

##5.ELF文件格式与进程地址空间的联系

进程地址空间中典型的存储区域分配情况如下：

![](./images/20130526195532582)

从图中可以看出：

从低地址到高地址分别为：代码段、（初始化）数据段、（未初始化）数据段（BSS）、堆、栈、命令行参数和环境变量
堆向高内存地址生长
栈向低内存地址生长

对于ELF文件，一般有下面几个段

```sh
.text section：主要是编译后的源码指令，是只读字段。
.data section ：初始化后的非const的全局变量、局部static变量。
.bss：未初始化后的非const全局变量、局部static变量。
.rodata：是存放只读数据 
```

关于ELF的文件的只是这里就不赘述了。

在ELF文件中，使用section和program两种结构描述文件的内容。通常来说，ELF可重定位文件采用section，ELF可执行文件使用program，可重链接文件则两种都用。
装载文件，其实是一个很简单的过程，通过section或者program中的type属性判断是否需要加载，然后通过offset属性找到文件中的数据，将它读取（复制）到相应的内存位置就可以了。 这个位置，可以通过program里面的vaddr属性确定；对于section来说，则可以自己定义装载的位置。

动态连接的本质，就是对ELF文件进行重定位和符号解析。
重定位可以使得ELF文件可以在任意的执行（普通程序在链接时会给定一个固定执行地址）；符号解析，使得ELF文件可以引用动态数据（链接时不存在的数据）。
从流程上来说，我们只需要进行重定位。而符号解析，则是重定位流程的一个分支。

## 6.参考
程序员的自我修养—链接、装载与库 
  
Computer Systems: A Programmer's Perspective  3rd Edith

Linux内核编程

understanding the kernel  3rd Edith