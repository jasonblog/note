# ARM汇编指令调试方法


学习ARM汇编时，少不了对ARM汇编指令的调试。作为支持多语言的调试器，gdb自然是较好的选择。调试器工作时，一般通过修改代码段的内容构造trap软中断指令，实现程序的暂停和程序执行状态的监控。为了在x86平台上执行ARM指令，可以使用qemu模拟器执行ARM汇编指令。

##一、准备ARM汇编程序

首先，我们构造一段简单的ARM汇编程序作为测试代码main.s。

```c
.globl _start
_start:
    mov R0,#0
    swi 0x00900001
```

以上汇编指令完成了0号系统调用exit的调用。mov指令将系统调用号传入寄存器R0，然后使用0x00900001软中断陷入系统调用。

为了运行ARM汇编代码，需要使用交叉编译器arm-linux-gcc对ARM汇编代码进行编译。下载交叉编译器安装完毕后，对ARM汇编代码进行编译。

```sh
arm-linux-gcc main.s -o main -nostdlib
```

编译选项`-nostdlib`表示不使用任何运行时库文件，编译生成的可执行文件main只能在ARM体系结构的系统上运行。

##二、编译安装qemu模拟器
为了x86的Linux系统内运行ARM体系结构的可执行程序，需要安装qemu模拟器。

首先下载qemu源码，然后保证系统已经安装了flex和bison。

编译安装qemu。

```sh
./configure --prefix=/usr
sudo make && make install
```

然后使用qemu的ARM模拟器执行ARM程序。

```sh
qemu ./main
```

##三、编译安装arm-gdb
为了调试ARM程序，需要使用gdb的源码编译生成arm-gdb。

首先下载gdb源代码，编译安装。

```sh
./configure --target=arm-linux --prefix=/usr/local
sudo make && make install
```

为了和系统的gdb避免冲突，我们将gdb的安装目录安装到/usr/local，然后建立软链接即可。

```sh
ln -s /usr/local/gdb/gdb /usr/bin/arm-gdb
```

之后便可以使用arm-gdb命令调试ARM程序了。

##四、调试ARM程序
首先使用qemu打开远程调试端口。

```sh
qemu-arm -g 1024 ./main
```

然后导出环境变量QEMU_GDB。

```sh
export QEMU_GDB=/usr/local/gdb
```

最后，进入gdb调试。

```sh
arm-gdb ./main
```

进入arm-gdb后，首先连接远程调试端口。

```sh
(gdb) target remote localhost:1024
```

然后使用gdb常用的调试命令调试代码即可。

```sh
(gdb) disassemble           // 查看反汇编
(gdb) x /8xw 0x0000808e     // 查看内存
(gdb) info register         // 查看寄存器
(gdb) continue              // 继续执行
(gdb) stepi                 // 汇编级逐过程
(gdb) nexti                 // 汇编级逐语句
```

##参考资料
http://blog.sina.com.cn/s/blog_59fd92c40100h4v2.html
http://wenku.baidu.com/link?url=g8nb4PdsjaS4LLg6bDO3fo8LZjSVOFvglIi1b6OLgYbl_1Nav8_0QuDiWY4gTCByklxriyo-6s7s_JCErU6_RNcAP2FtB1FMpax1ATnUZn3