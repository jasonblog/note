# GDB技巧：使用checkpoint解決難以復現的Bug


作為程序員，調試是一項很重要的基本功。調試的技巧和水平，直接決定了解決問題的時間。一般情況下，GDB的基本命令已經足以應付大多數問題了。但是，對於有些問題，還是需要更高級一些的命令。今天介紹一下checkpoint。

有一些bug，可能很難復現，當好不容易復現一次，且剛剛進入程序的入口時，我們需要珍惜這個來之不易的機會。如果只使用基本命令的話，對於大部分代碼，我們都需要使用step來步進。這樣無疑會耗費大量的時間，因為大部分的代碼可能都沒有問題。可是一旦不小心使用next，結果恰好該語句的函數調用返回出錯。那麼對於這次來之不易的機會，我們只得到了部分信息，即確定問題出在該函數，但是哪裡出錯還是不清楚。於是還需要再一次的復現bug，時間就這樣浪費了。

所以，對於這種問題，就是checkpoint大顯身手的時候。先看一下GDB關於checkpoint的說明：
On certain operating system(Currently, only GNU/Linux), GDB is able to save a snapshot of a program's state, called a checkpoint and come back to it later.
Returning to a checkpoint effectively undoes everything that has happened in the program since the checkpoint was saved. This includes changes in memory, register, and even(within some limits) system state. Effectively, it is like going back in time to the moment when the checkpoint was saved.
也就是說checkpoint是程序在那一刻的快照，當我們發現錯過了某個調試機會時，可以再次回到checkpoint保存的那個程序狀態。

舉例說明一下：

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
當我們執行這個程序時，發現程序返回1，不是期望的成功0。於是開始調試程序，由於函數調用的嵌套過多，我們沒法一眼看出是main中的哪個函數調用出錯了。於是在ret += func1()前，我們保存一個checkpoint。

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

然後使用next步進，並每次調用完畢，打印ret的值

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

結果發現，在調用func2()調用後，ret的值變為了1。可是此時，我們已經錯過了調試func2的機會。如果沒有checkpoint，就需要再次從頭調試了——對於這個問題從頭調試很容易，但是對於很難復現的bug可就不說那麼容易的事情了。

ok，使用checkpoint恢復

```sh
(gdb) restart 1
Switching to process 2060
#0 main () at test.c:33
33 ret += func1();
(gdb)
```

很簡單，現在GDB恢復到了保存checkpoint時的狀態了。上面“restart 1“中的1為checkpoint的id號，可以使用info查看。
```sh
(gdb) info checkpoints
* 1 process 2060 at 0x80483e7, file test.c, line 33
  0 process 2059 (main process) at 0x80483f7, file test.c, line 35
```

從上面可以看出checkpoint的用法很簡單，但是很有用。就是在平時的簡單的bug修正中，也可以加快我們的調試速度——畢竟減少了不必要的重現bug的時間。

