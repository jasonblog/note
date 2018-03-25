# Linux内核中线程的实现方式


本文通过对Linux内核源码的研究和两个C/C++程序，探讨了 Linux内核对线程的支持。并得出了一个结论：Linux内核的线程实现是货真价实的。因此，Linux上的多线程实现是真正的多线程实现。所谓Linux内核其实并不支持线程的说法是错误的。

##1.    前言
关于Linux 内核中线程的实现方式，有一种说法认为：Linux内核其实并不支持线程，因此，Linux上的多线程实现其实是“伪多线程”。

这种说法到底正确吗？Linux内核到底是否支持多线程？本文作者通过对Linux内核源码的研究和一些C/C++程序来回答这一问题。

##2.    进程与线程
按照操作系统教科书中的定义。进程与线程有以下特点：

1） 进程是程序的执行。<br>
2） 在一个进程中，可以有一到多个线程。<br>
3） 这些线程共享同一个地址空间。<br>
4） 但是每个线程有自己独立的运行栈。<br>
5） 每个线程可以被操作系统独立地调度。<br>

##3.    描述线程的数据结构
根据http://en.wikipedia.org/wiki/Multithreading_(computer_architecture),多线程编程模式兴起于90年代末。因此，当Linus Torvalds于1991年实现Linux的第一个版本是，他根本没有考虑对线程的支持。

在早期的Linux版本中，Linux只支持进程，不支持线程。在早期的Linux的版本中，描述进程的数据结构式struct task_struct，这也就是操作系统教科书中所说的PCB（Process Control Block）。

为了支持线程，当代Linux采用的方式是用struct task_struct既描述进程，也描述线程。图1给出了2.6.32.27内核中描述进程/线程关系的数据结构。

![](images/20141118140939250.png)


图1：描述进程/线程关系的数据结构

从上图我们可以看出：

1） 每个线程都用一个独立的task_struct来描述。<br>
2） 同一个进程的多个线程通过task_struct的thread_group指针字段链接成一个双向循环链表。为了清晰起见，上图只是画出了一个方向的链接。<br>
3） 同一个进程的多个线程共享同一个内存地址空间，因为它们task_struct的mm指针字段都指向了同一个mm_struct结构。<br>
4） 对于每个信号，同一个进程的多个线程共享同一个信号处理程序。因为它们task_struct的sighand字段都指向了同一个sighand_struct结构。<br>
5） 同一个进程的多个线程共享同一个文件描述表。因此，一个线程打开的文件，对其它线程也是可见的。<br>


为了验证上面的结论，我们下面通过一个用户态的多线程程序和一个内核态的模块来进行验证。本文的实验环境如下：

1） Cent OS 版本: 6.5<br>
2） Linux内核2.6.32<br>
3） GCC版本：4.4.7<br>

##4.    一个用户态的多线程程序

下面是该用户态程序的源码：

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
static void handler(int sig)
{
    printf("CTRL+C captured\n");
}
static void* threadFunc(void* arg)
{
    printf("In threadFunc()\n");
    int fd = dup(0);
    printf("fd = %d\n", fd);
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        printf("Set SIGINT handler error\n");
    }

    sleep(60 * 10); // in seconds
    return (void*)1;
}
int main(void)
{
    pthread_t t[100];
    void* res;
    int s;
    int thread_count = 2;
    int i;

    for (i = 0; i < thread_count; i++) {
        s = pthread_create(&t[i], NULL, threadFunc, NULL);

        if (s != 0) {
            printf("pthread_create() call failed. Return value:%d.\n", s);
        }
    }

    for (i = 0; i < thread_count; i++) {
        s = pthread_join(t[i], &res);

        if (s != 0) {
            printf("pthread_join() call failed. Return value: %d\n", s);
        } else {
            printf("pthread_join() call suceeded. Thread exit code: %ld\n", (long)res);
        }
    }

    return 0;
}
```

下面是编译该程序的Makefile：

```sh
TARGET = pthread_test
SOURCES = main.cpp
CC = g++
FLAGS = -g -Wall
LIBS = -lm -lstdc++ -pthread
# Objs are all the sources, with .cpp replaced by .o
OBJS := $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(FLAGS) $(LIBS)

.cpp.o:
	$(CC) $(FLAGS) $(INCLUDES) -c $<

clean:
	rm -f *.o
	rm -f $(TARGET)
```


该用户态程序在主线程中通过POSIX Thread库创建了两个线程。在每个线程中：

1） 调用dup(0)复制一个新的标准输入的描述符。<br>
2） 设置了SIGINT信号的处理程序。<br>
3） 睡眠一个小时。<br>

##5.    一个内核态的模块
该内核态模块的源码：


```c
#include <linux/module.h>   // included for all kernel modules
#include <linux/kernel.h>   // included for KERN_INFO
#include <linux/init.h>     // included for __init and __exit macros
#include <linux/sched.h>
#include <linux/fdtable.h>

MODULE_LICENSE("GPL");

void dump_schedule_info(struct task_struct* task)
{
    printk("sched_class of task id(%d, comm='%s'):0x%x\n", task->pid, task->comm,
           task->sched_class);
}
void dump_file_struct(struct files_struct* files)
{
    int max_fds = files->fdt->max_fds;
    int fd;
    printk("files->fdt->max_fds=%d\n", max_fds);

    for (fd = 0; fd < max_fds; fd++) {
        if (FD_ISSET(fd, files->fdt->open_fds)) {
            printk("fd %d is open\n", fd);
        } // if
    } // for
}
void dump_sighand_struct(struct sighand_struct* sighand)
{
    int i;

    for (i = 0; i < _NSIG; i++) {
        struct k_sigaction* sigaction = &sighand->action[i];

        if (sigaction->sa.sa_handler == SIG_DFL) {
            printk("SIG %d: SIG_DFL\n", i);
        } else if (sigaction->sa.sa_handler == SIG_IGN) {
            printk("SIG %d: SIG_IGN\n", i);
        } else {
            printk("SIG %d: 0x%x\n", i, sigaction->sa.sa_handler);
        }
    } // for
}
void enum_threads_in_process(struct task_struct* proc)
{
    struct task_struct* t;
    printk("Start enumerating threads in process:%d\n", proc->pid);
    int count = 0;
    t = proc;

    do {
        printk("Thread id=%d, comm='%s', files=0x%x, mm=0x%x, active_mm=0x%x, sighand=0x%x\n",
               t->pid, t->comm, t->files, t->mm, t->active_mm, t->sighand);
        dump_schedule_info(t);
        dump_file_struct(t->files);
        dump_sighand_struct(t->sighand);
        count ++;
        t = next_thread(t);
    } while (t != proc);

    printk("Thread count in proc(pid=%d):%d\n", proc->pid, count);
}
void enum_processes()
{
    struct task_struct* proc;
    printk("\n\nStart enumerating processes:\n");
    int count = 0;
    for_each_process(proc) {
        printk("pid=%d, comm='%s', files=0x%x\n", proc->pid, proc->comm, proc->files);
        enum_threads_in_process(proc);
        printk("\n");
        count ++;
    } // for_each_process
    printk("Process count:%d\n", count);
}
int init_module(void)
{
    enum_processes();
    return 0;    // Non-zero return means that the module couldn't be loaded.
}
void cleanup_module(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}
```

下面是编译该模块的Makefile：

```sh
obj-m += enum_processes.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

在内核加载该内核态模块时，该模块：

1） 枚举出系统中的每一个进程。<br>
2） 对于每个进程，枚举出其每一个线程。<br>
3） 对于每个线程，打印其task_struct的mm指针字段。<br>
4） 对于每个线程，打印其文件描述符表。<br>
5） 对于每个线程，打印其每个信号的处理程序。<br>

##6.    实验


###6.1  运行用户态程序pthread_test
我们运行pthread_test，然后按CTRL+C。

```sh
[liqingxu@localhost posix_thread]$ ./pthread_test
In threadFunc()
fd = 3
In threadFunc()
fd = 4
^CCTRL+C captured
```

通过上面的输出，我们可以看到：

1） 线程成功的设置了SIGINT信号的处理程序。<br>
2） 第一个线程的dup(0)调用返回的文件描述符为3。<br>
3） 第二个线程的dup(0)调用返回的文件描述符为4。<br>

下面是该模块注册时打印的一部分信息。通过下面highlight出的信息，我们可以看出：

1） 每个线程都共享同一个地址空间，因为其task_struct 的mm字段指向了同样的mm_struct结构。<br>
2） 每个线程都共享同一个信号处理程序。因为它们task_struct的sighand字段都指向了同一个sighand_struct结构。<br>
3） 每个线程都共享同一个文件描述表。<br>

这验证了我们前面讨论描述线程的数据结构时得到的结论。<br>

```sh
Nov 10 03:43:30 localhost kernel: pid=5703, comm='pthread_test', files=0x4f12b200
Nov 10 03:43:30 localhost kernel: Start enumerating threads in process:5703
Nov 10 03:43:30 localhost kernel: Thread id=5703, comm='pthread_test', files=0x4f12b200,mm=0x4f00c180, active_mm=0x4f00c180, sighand=0x6462b540
Nov 10 03:43:30 localhost kernel: sched_class of task id(5703, comm='pthread_test'):0x8160be60
Nov 10 03:43:30 localhost kernel: files->fdt->max_fds=256
Nov 10 03:43:30 localhost kernel: fd 0 is open
Nov 10 03:43:30 localhost kernel: fd 1 is open
Nov 10 03:43:30 localhost kernel: fd 2 is open
Nov 10 03:43:30 localhost kernel: fd 3 is open
Nov 10 03:43:30 localhost kernel: fd 4 is open
Nov 10 03:43:30 localhost kernel: SIG 0: SIG_DFL
Nov 10 03:43:30 localhost kernel: SIG 1: 0x4007e4
Nov 10 03:43:30 localhost kernel: SIG 2: SIG_DFL
......
Nov 10 03:43:30 localhost kernel: Thread id=5704, comm='pthread_test', files=0x4f12b200,mm=0x4f00c180, active_mm=0x4f00c180, sighand=0x6462b540
Nov 10 03:43:30 localhost kernel: sched_class of task id(5704, comm='pthread_test'):0x8160be60
Nov 10 03:43:30 localhost kernel: files->fdt->max_fds=256
Nov 10 03:43:30 localhost kernel: fd 0 is open
Nov 10 03:43:30 localhost kernel: fd 1 is open
Nov 10 03:43:30 localhost kernel: fd 2 is open
Nov 10 03:43:30 localhost kernel: fd 3 is open
Nov 10 03:43:30 localhost kernel: fd 4 is open
Nov 10 03:43:30 localhost kernel: SIG 0: SIG_DFL
Nov 10 03:43:30 localhost kernel: SIG 1: 0x4007e4
Nov 10 03:43:30 localhost kernel: SIG 2: SIG_DFL
......
Nov 10 03:43:30 localhost kernel: Thread id=5705, comm='pthread_test', files=0x4f12b200,mm=0x4f00c180, active_mm=0x4f00c180, sighand=0x6462b540
Nov 10 03:43:30 localhost kernel: sched_class of task id(5705, comm='pthread_test'):0x8160be60
Nov 10 03:43:30 localhost kernel: files->fdt->max_fds=256
Nov 10 03:43:30 localhost kernel: fd 0 is open
Nov 10 03:43:30 localhost kernel: fd 1 is open
Nov 10 03:43:30 localhost kernel: fd 2 is open
Nov 10 03:43:30 localhost kernel: fd 3 is open
Nov 10 03:43:30 localhost kernel: fd 4 is open
Nov 10 03:43:30 localhost kernel: SIG 0: SIG_DFL
Nov 10 03:43:30 localhost kernel: SIG 1: 0x4007e4
Nov 10 03:43:30 localhost kernel: SIG 2: SIG_DFL
......
Nov 10 03:43:30 localhost kernel: Thread count in proc(pid=5703):3
```

##7.    PThread线程库是如何创建线程的
###7.1  Glibc源程序RPM包的下载
Pthread library的实现在glibc中。

wget http://vault.centos.org/6.5/os/Source/SPackages/glibc-2.12-1.132.el6.src.rpm

###7.2  从RPM包中获取Glibc的源码
1)      需要先安装rpm-build包 yum install rpm-build<br>
2)      mkdir -p ~/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}<br>
3)      编译但编译完成后不删除源程序 rpmbuild --recompile glibc-2.12-1.132.el6.src.rpm<br>
4)      编译完成后，Pthread的源程序在~/rpmbuild/BUILD/glibc-2.12-2-gc4ccff1/nptl目录下<br>

###7.3  pthread_create()的实现

Linux内核提供了一个系统调用clone()可以创建线程。该系统调用的实现函数是sys_clone()。

在文件nptl/sysdeps/pthread/createthread.c中，我们可以看到传递给clone()系统调用的flags参数如下：

```c
int clone_flags = (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGNAL
                   | CLONE_SETTLS | CLONE_PARENT_SETTID
                   | CLONE_CHILD_CLEARTID | CLONE_SYSVSEM
#if __ASSUME_NO_CLONE_DETACHED == 0
                   | CLONE_DETACHED
#endif
                   | 0);
```

我们可以看出，因为pthread_create()传递给sys_clone()的flags参数中包含了CLONE_VM |  CLONE_FILES | CLONE_SIGNAL标志，所以其创建出来的线程都共享一个地址空间、一套文件描述符表和一套信号处理程序表。

##8.    结论
通过前面对Linux内核源码的分析和实验，我们可以得出来一个结论：Linux内核的线程实现是货真价实的。因此，Linux上的多线程实现是真正的多线程实现。

##9.    参考资料
- 1.       Daniel P. Bovet & Marco Cesati著.  Uderstanding the Linux Kernel. O’Reilly Media, Inc. 2006.

- 2.       Michael KerrisK. The Linux Programming Interface. No Starch Press, Inc. 2010.

Syscalls on x86/x64: http://stackoverflow.com/questions/9506353/how-to-invoke-a-system-call-via-sysenter-in-inline-assembly-x86-amd64-linux