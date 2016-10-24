# GDB调试技巧：gdb at pid无法调试的问题


当我们使用GDB调试一个daemon的时候，一般有两种方式：
1. 直接在shell命令行键入gdb attach pid （要调试daemon的进程ID）。一般情况下，我都习惯于缩写gdb at pid；
2. 在shell中键入gdb，进入gdb，然后attach pid（同样是要调试daemon的进程ID）。我不常用这种方式，因为这种方式需要2个步骤——尽管键入的字母是一样的。

下面来说说今天遇到的问题。公司的一个实习生来问我问题。他目前正在做一个工作，大致的功能是得到某个daemon的某一时刻的调用栈——就像gdb一样。但是他发现当daemon处于系统调用状态时，从EBP得到的返回地址不正确。然后我让他直接使用gdb查看是否可以看到完整的调用栈，如果GDB可以，那么就没有问题，可以去看GDB如何实现的。如果不能，再想为什么。——这里先插一句，当时我去查看了他的EBP的值，根据其值获得的返回地址确实不正确。今晚查了查，知道在某些函数调用时，为了速度，EBP有可能并没有被压栈。但是这个不是今天的主题，让我们再转回来。他很快的试了试，告诉我gdb虽然可以使用bt(backtrace)得到函数栈，但是显示出来的结果却不正确。

怎么回事呢？不可能啊。我在他那写了一个很简单的测试程序：


```c
#include <stdlib.h>
#include <stdio.h>

static void test(void)
{
    pause();
}


int main(void)
{
    test();

    return 0;
}
```

编译并运行：

```sh
[xxx@xxx-vm-fc13 test]$ gcc -g test.c
[xxx@xxx-vm-fc13 test]$ ./a.out
```
然后使用另外一个终端进行调试：

```sh
[xxx@xxx-vm-fc13 test]$ ps auwx|grep a.out
xxx 2412 0.0 0.0 1816 288 pts/4 S+ 11:19 0:00 ./a.out
xxx 2415 0.0 0.0 4308 732 pts/5 S+ 11:20 0:00 grep a.out
[xxx@xxx-vm-fc13 test]$ gdb at 2412
GNU gdb (GDB) Fedora (7.1-18.fc13)
Copyright (C) 2010 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-redhat-linux-gnu".
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>...
Reading symbols from /usr/bin/at...(no debugging symbols found)...done.
Attaching to program: /usr/bin/at, process 2412
Reading symbols from /lib/ld-linux.so.2...(no debugging symbols found)...done.
Loaded symbols for /lib/ld-linux.so.2
0x009f4424 in __kernel_vsyscall ()
Missing separate debuginfos, use: debuginfo-install at-3.1.12-5.fc13.i686
(gdb) bt
#0 0x009f4424 in __kernel_vsyscall ()
#1 0x0065dac6 in ?? ()
#2 0x080483dc in ?? ()
#3 0x005d6cc6 in ?? ()
#4 0x08048331 in ?? ()
(gdb)
```

看到这个结果，我还真有些意外。我使用gdb也有些年头了，从来都是这样调试daemon的啊。怎么会这样呢啊？看这个输出，很像没有调试信息啊。可是前面的编译有-g选项啊。

使用第二种方法试试？

```sh
[xxx@xxx-vm-fc13 test]$ gdb
GNU gdb (GDB) Fedora (7.1-18.fc13)
Copyright (C) 2010 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. Type "show copying"
and "show warranty" for details.
This GDB was configured as "i686-redhat-linux-gnu".
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
(gdb) at 2412
Attaching to process 2412
Reading symbols from /home/fgao/works/test/a.out...done.
Reading symbols from /lib/libc.so.6...(no debugging symbols found)...done.
Loaded symbols for /lib/libc.so.6
Reading symbols from /lib/ld-linux.so.2...(no debugging symbols found)...done.
Loaded symbols for /lib/ld-linux.so.2
0x009f4424 in __kernel_vsyscall ()
Missing separate debuginfos, use: debuginfo-install glibc-2.12-1.i686
(gdb) bt
#0 0x009f4424 in __kernel_vsyscall ()
#1 0x0065dac6 in __pause_nocancel () from /lib/libc.so.6
#2 0x080483cf in test () at test.c:6
#3 0x080483dc in main () at test.c:12
(gdb)
```
<ol start="1" class="dp-css"><li>(gdb) q</li><li>
A debugging session is active.</li><li>
</li><li>
        Inferior 1 [process 2412] will be detached.</li><li>
</li><li>
Quit anyway? (y or n) y</li><li><font color="#f00000">
Detaching from program: /usr/bin/at, process 2412</font></li></ol>



啊，这种方式就没有问题啊。怎么回事呢？难道是因为这两种方式的attach还有区别不成？于是我又查了gdb的说明，应该没有区别啊。

想了半天，又试了几次第一种方式，还是没有结果。当我感觉我以前的GDB的世界观都要崩溃的时候，对自己的GDB技能产生了极度的不自信的时候，再次退出gdb的时候，我发现了原因。该原因差点没把我郁闷死。。。

大家想一下哦。。。

注意看这里的红色代码。进程号没有错，但是program却是/usr/bin/at。My god！原来我使用第一种方式的时候，一直在尝试调试at——这个linux命令。怪不得没有调试信息呢！在我以前的GDB应用中，因为系统中没有at这个命令，所以gdb at pid就等于gdb attach pid，是可以缩写的。但是在当前这个环境中，很并不幸的存在着at这个命令。结果GDB没有尝试去attach，而是直接去调试at命令。

不过这里，我觉得GDB可以做得更友好一些。虽然在这个情况下，GDB去调试at没有问题，但是后面的PID已经被我的测试程序占用了，它应该报个错误或者警告吧。

最后总结一下，GDB的缩写是好，但是小心过分的缩写会与已有命令冲突哦~~总之，缩写需谨慎。



