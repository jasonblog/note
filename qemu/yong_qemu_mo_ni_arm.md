# 用Qemu模拟ARM



前面已经安装并配置了编译链和qemu，现在可以用qemu来模拟arm平台了。

###1. Hello, Qemu!

输入下面的代码:
- hello.c

```c
#include<stdio.h> 

int main()
{
    printf("Hello, Qemu!\n");
    return 0;
}
```

编译并运行:

```sh
$ arm-none-linux-gnueabi-gcc -o hello hello.c -static
$ qemu-arm ./hello
$ file hello
hello: ELF 32-bit LSB  executable, ARM, EABI5 version 1 (SYSV), \
 statically linked, for GNU/Linux 2.6.16, not stripped
```

不加-static变量的话，运行时则需要使用-L选项链接到相应的运行库

```sh
$ qemu-arm -L /home/dash/CodeSourcery/\
Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/\
arm-none-linux-gnueabi/libc/  ./hello_1 
Hello, Qemu!
$ file hello_1
hello_1: ELF 32-bit LSB  executable, ARM, EABI5 version 1 (SYSV),\
 dynamically linked (uses shared libs), for GNU/Linux 2.6.16, not stripped
```


动态编译和静态编译生成的文件大小差别：

```sh
$ ls -l -h
total 656K
-rwxr-xr-x 1 dash root 640K Jul  7 18:46 hello
-rwxr-xr-x 1 dash root 6.6K Jul  7 18:48 hello_1
```

###小插曲1：

系统里安装了两套编译链arm-none-eabi-和arm-none-linux-eabi-,很容易让人混淆，可参考编译链的命名规则：

arch(架构)-vendor(厂商名)–(os(操作系统名)–)abi(Application Binary Interface，应用程序二进制接口)


举例说明：

- x86_64-w64-mingw32 = x86_64 “arch”字段 (=AMD64), w64 (=mingw-w64 是”vendor”字段), mingw32 (=GCC所见的win32 API)
- i686-unknown-linux-gnu = 32位 GNU/linux编译链
- arm-none-linux-gnueabi = ARM 架构, 无vendor字段, linux 系统, gnueabi ABI.
- arm-none-eabi = ARM架构, 无厂商, eabi ABI(embedded abi)

两种编译链的主要区别在于库的差别，前者没有后者的库多，后者主要用于在有操作系统的时候编译APP用的。前者不包括标准输入输出库在内的很多C标准库，适合于做面向硬件的类似单片机那样的开发。因而如果采用arm-none-eabi-gcc来编译hello.c会出现链接错误。

### 小插曲2：

qemu-arm和qemu-system-arm的区别：


- qemu-arm是用户模式的模拟器(更精确的表述应该是系统调用模拟器)，而qemu-system-arm则是系统模拟器，它可以模拟出整个机器并运行操作系统

- qemu-arm仅可用来运行二进制文件，因此你可以交叉编译完例如hello world之类的程序然后交给qemu-arm来运行，简单而高效。而qemu-system-arm则需要你把hello world程序下载到客户机操作系统能访问到的硬盘里才能运行。


###2. 使用qemu-system-arm运行Linux内核

从www.kernel.org下载最新内核,而后解压

```sh
$ tar xJf linux-3.10.tar.xz
$ cd linux-3.10
$ make ARCH=arm versatile_defconfig
$ make menuconfig ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-
```

上面的命令指定内核架构为arm，交叉编译链为arm-none-linux-gnueabi, 需要在make menuconfig弹出的窗口中选择到 “Kernel Features”, 激活“Use the ARM EABI to compile the kernel”, 如果不激活这个选项的话，内核将无法加载接下来要制作的initramfs。

如果需要在u-boot上加载内核，就要编译为uImage的格式，uImage通过mkimage程序来压缩的，ArchLinux的yaourt仓库里可以找到这个包：

```sh
$ yaourt -S mkimage
```
安装好mkimage后，开始编译内核，因为CPU有4核，所以开启了-j8选项以加速编译:

```sh
$ make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- all -j8 uImage 
```

接下来我们可以在qemu-system-arm中测试我们的内核了

```sh
$ qemu-system-arm -M versatilepb -m 128M -kernel ./arch/arm/boot/uImage
```

在弹出的窗口中可以内核运行到了kernel panic状态，这是因为内核无法加载root镜像的缘故，我们将制作一个最简单的hello world的文件系统，告知kernel运行之。

-  init.c

```c
#include <stdio.h>

void main()
{
    printf("Hello World!\n");
    while (1);
}
```

编译并制作启动镜像:

```sh
$ arm-none-linux-gnueabi-gcc -o init init.c -static
$ echo init |cpio -o --format=newc > initramfs
1280 blocks
$ file initramfs 
initramfs: ASCII cpio archive (SVR4 with no CRC)
```

接下来我们回到编译目录下执行：

```sh
$ qemu-system-arm -M versatilepb -kernel ./arch/arm/boot/uImage  -initrd
../initramfs -serial stdio -append "console=tty1"
```

这时候可以看到，kernel运行并在Qemu自带的终端里打印出”Hello World!“。

如果我们改变console变量为ttyAMA0, 将在启动qemu-system-arm的本终端上打印出qemu的输出。

## 1. 关于Bootloader:

(引导程序)位于电脑或其他计算机应用上，是指引导操作系统启动的程序。引导程序启动方式和程序视应用机型种类而不同。例如在普通的个人电脑上，引导程序通常分为两部分：第一阶段引导程序位于主引导记录（MBR），用以引导位于某个分区上的第二阶段引导程序，如NTLDR、GNU GRUB等。

嵌入式系统中常见的Bootloader主要有以下几种:

- Das U-Boot 是一个主要用于嵌入式系统的开机载入程序，可以支持多种不同的计算机系统结构，包括PPC、ARM、AVR32、MIPS、x86、68k、Nios与MicroBlaze。

- vivi是由mizi公司设计为ARM处理器系列设计的一个bootloader.

- Redboot (Red Hat Embedded Debug and Bootstrap)是Red Hat公司开发的一个独立运行在嵌入式系统上的BootLoader程序，是目前比较流行的一个功能、可移植性好的BootLoader。

## 2. 关于“裸机编程(Bare-Metal)”:


微控制器开发人员很熟悉这个概念， Bare-Metal是指的你的程序和处理器之间没有任何东西——你写的程序将直接运行在处理器上, 换言之，开发人员是在直接操控硬件。在裸机编程的场景中，需要由开发人员检查并排除任何一个可以导致系统崩溃的风险。

“Bare-Metal”要求开发人员了解关于硬件的细节，所以接下来我们将对编译链和qemu本身进行分析。


## 3. 下载qemu源码包并查询相关硬件信息：

ArchLinux采用ABS(Arch Build System)来管理源码包，下面的步骤将qemu源码包下载到本地，更详细的关于ABS的操作可以在ArchLinux的Wiki中找到

```sh
$ pacman -S abs
$ pacman -Ss qemu
extra/qemu 1.4.2-2 [installed]
$ abs extra/qemu 
$ cp -r /var/abs/extra/qemu/ ~/abs 
$ cd ~/abs && makepkg -s --asroot -o
```

得到versatilepb开发板的CPU型号, 可以看到”arm926”是我们要的结果。

```sh
$ grep "arm" src/qemu-1.4.2/hw/versatilepb.c 
#include "arm-misc.h"
static struct arm_boot_info versatile_binfo;
        args->cpu_model = "arm926";
    cpu = cpu_arm_init(args->cpu_model);
    cpu_pic = arm_pic_init_cpu(cpu);
    arm_load_kernel(cpu, &versatile_binfo);
```

得到versatilepb开发板的串口寄存器硬件信息：

```sh
$ grep "UART*" src/qemu-1.4.2/hw/versatilepb.c 
    /*  0x10009000 UART3.  */
    /*  0x101f1000 UART0.  */
    /*  0x101f2000 UART1.  */
    /*  0x101f3000 UART2.  */
```

所以说开源是王道嘛，很快就查到了每一个需要了解的细节。UART0在内存中map到的地址是0x101f1000, 我们直接往这个地址写数据，就可以在终端上看到数据输出了。

##4. 查看编译链支持的平台：

```sh
$ cat ~/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI/share/doc/arm-arm-none-eabi/info/gcc.info | grep arm926
     `arm926ej-s', `arm940t', `arm9tdmi', `arm10tdmi', `arm1020t',
```

arm926ej-s是被支持的，因此我们可以用这套编译链来生成需要的裸机调试代码。

##5. 启动应用程序init.c的编写:

首先创建应用程序init.c：

-  init.c

```c
volatile unsigned char* const UART0_PTR = (unsigned char*)0x0101f1000;

void display(const char* string)
{
    while (*string != '\0') {
        *UART0_PTR = *string;
        string++;
    }
}

int my_init()
{
    display("Hello Open World\n");
}
```

init.c中，我们首先声明一个volatile变UART0_PTR,volatile关键字用于告知编译器此变量是用于直接访问内存映像设备的，即串口0内存地址

display()函数则是用于将字符串中的字符按顺序输出到串口0, 直到遇到字符串结尾。

my_init()调用了display(), 接下来我们将把它作为C入口函数.

预编译init.c:

```sh
$ arm-none-eabi-gcc -c -mcpu=arm926ej-s init.c -o init.o
```

## 6. 启动代码start.s编写：
- start.s


```sh
```


