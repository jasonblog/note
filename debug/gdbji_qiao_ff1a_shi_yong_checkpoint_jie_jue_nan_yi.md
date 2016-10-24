# GDB技巧：使用checkpoint解决难以复现的Bug


作为程序员，调试是一项很重要的基本功。调试的技巧和水平，直接决定了解决问题的时间。一般情况下，GDB的基本命令已经足以应付大多数问题了。但是，对于有些问题，还是需要更高级一些的命令。今天介绍一下checkpoint。

有一些bug，可能很难复现，当好不容易复现一次，且刚刚进入程序的入口时，我们需要珍惜这个来之不易的机会。如果只使用基本命令的话，对于大部分代码，我们都需要使用step来步进。这样无疑会耗费大量的时间，因为大部分的代码可能都没有问题。可是一旦不小心使用next，结果恰好该语句的函数调用返回出错。那么对于这次来之不易的机会，我们只得到了部分信息，即确定问题出在该函数，但是哪里出错还是不清楚。于是还需要再一次的复现bug，时间就这样浪费了。

所以，对于这种问题，就是checkpoint大显身手的时候。先看一下GDB关于checkpoint的说明：
On certain operating system(Currently, only GNU/Linux), GDB is able to save a snapshot of a program's state, called a checkpoint and come back to it later.
Returning to a checkpoint effectively undoes everything that has happened in the program since the checkpoint was saved. This includes changes in memory, register, and even(within some limits) system state. Effectively, it is like going back in time to the moment when the checkpoint was saved.
也就是说checkpoint是程序在那一刻的快照，当我们发现错过了某个调试机会时，可以再次回到checkpoint保存的那个程序状态。

举例说明一下：

```c
#include <stdlib.h>
#include <stdio.h>

static int func()
{
    static int i = 0;
    ++i;
    if (i == 2) {
        return 1;
    }
    return 0;
}

static int func3()
{
    return func();
}

static int func2()
{
    return func();
}

static int func1()
{
    return func();
}

int main()
{
    int ret = 0;

    ret += func1();
    ret += func2();
    ret += func3();

    return ret;
}
```
当我们执行这个程序时，发现程序返回1，不是期望的成功0。于是开始调试程序，由于函数调用的嵌套过多，我们没法一眼看出是main中的哪个函数调用出错了。于是在ret += func1()前，我们保存一个checkpoint。

```sh
(gdb) b main
Breakpoint 1 at 0x80483e0: file test.c, line 31.
(gdb) r
Starting program: /home/fgao/works/test/a.out

Breakpoint 1, main () at test.c:31
31 int ret = 0;
Missing separate debuginfos, use: debuginfo-install glibc-2.12-1.i686
(gdb) n
33 ret += func1();
(gdb) checkpoint
checkpoint: fork returned pid 2060.
(gdb)
```

然后使用next步进，并每次调用完毕，打印ret的值

```sh
Breakpoint 1, main () at test.c:31
31 int ret = 0;
(gdb) n
33 ret += func1();
(gdb) checkpoint
checkpoint: fork returned pid 2060.
(gdb) n
34 ret += func2();
(gdb) p ret
$4 = 0
(gdb) n
35 ret += func3();
(gdb) p ret
$5 = 1
```

结果发现，在调用func2()调用后，ret的值变为了1。可是此时，我们已经错过了调试func2的机会。如果没有checkpoint，就需要再次从头调试了——对于这个问题从头调试很容易，但是对于很难复现的bug可就不说那么容易的事情了。

ok，使用checkpoint恢复

```sh
(gdb) restart 1
Switching to process 2060
#0 main () at test.c:33
33 ret += func1();
(gdb)
```

很简单，现在GDB恢复到了保存checkpoint时的状态了。上面“restart 1“中的1为checkpoint的id号，可以使用info查看。
```sh
(gdb) info checkpoints
* 1 process 2060 at 0x80483e7, file test.c, line 33
  0 process 2059 (main process) at 0x80483f7, file test.c, line 35
```

从上面可以看出checkpoint的用法很简单，但是很有用。就是在平时的简单的bug修正中，也可以加快我们的调试速度——毕竟减少了不必要的重现bug的时间。

