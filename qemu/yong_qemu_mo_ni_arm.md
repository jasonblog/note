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
.global _Start
_Start:
LDR sp, = sp_top
BL my_init
B .
```

處理器加電後，將跳轉到指定的內存地址,從此地址開始讀入並執行代碼。

_Start被聲明為全局函數，_Start的實現中，首先將棧地址指向sp_top, LDR(load), sp是棧地址寄存器(stack pointer),

BL則是跳轉指令，跳轉到my_init函數，事實上你可以跳轉到任何一個你想跳轉的函數，臨時寫一個their_init()跳轉過去也行。Debug時常更改這裡以調試不同的子系統功能。

「B.」可以理解為彙編裡的while(1)或for(;;)循環，處理器空轉，什麼也不做。如果不調用它，系統就會崩潰。所謂嵌入式編程的一個基本理念就是，代碼無限循環。

預編譯彙編文件start.s:

```sh
$ arm-none-eabi-as -mcpu=arm926ej-s startup.s -o startup.o
```

## 7. 接下來我們需要用一個可以被編譯器識別的鏈接腳本鏈接兩文件, linker.ld:

- linker.ld

```sh
ENTRY(_Start)
  SECTIONS
  {
  . = 0x10000;
  startup : { startup.o(.text)}
  .data : {*(.data)}
  .bss : {*(.bss)}
  . = . + 0x500;
  sp_top = .;
  }
```

ENTRY(_Start)用於告知鏈接器程序的入口點(entry point)是_Start(start.s中定義). Qemu模擬器如果加上-kernel選項時，將自動從0x10000開始執行，所以我們必須將代碼放到這個地址。所以第四行我們指定」. = 0x10000」. SECTIONS就是用於定義程序的不同部分的。

startup.o組成了代碼的text部分，然後是data部分和bss部分，最後一步則定義了棧指針(sp, stack pointer)地址. 棧通常是向下增長的，所以最好給它一個比較安全的地址， . = .+0x500就是用於避免棧被改寫的。sp_top用於存儲棧頂地址。

有關程序結構：

- BSS段: 在採用段式內存管理的架構中，BSS段（bss segment）通常是指用來存放程序中未初始化的全局變量的一塊內存區域。BSS是英文Block Started by Symbol的簡稱。BSS段屬於靜態內存分配。.bss section的空間結構類似於stack, 主要用於存儲靜態變量、未顯式初始化、在變量使用前由運行時初始化為零。

- 數據段(data segment): 通常是指用來存放程序中已初始化且不為0的全局變量的一塊內存區域。數據段屬於靜態內存分配。

- 代碼段(code segment/text segment): 通常是指用來存放程序執行代碼的一塊內存區域。這部分區域的大小在程序運行前就已經確定，並且內存區域通常屬於只讀,某些架構也允許代碼段為可寫，即允許程序自修改。在代碼段中，也有可能包含一些只讀的常數變量，例如字符串常量等。

編譯:

```sh
$ arm-none-eabi-ld -T linker.ld init.o startup.o -o output.elf
$ file output.elf 
output.elf: ELF 32-bit LSB  executable, ARM, EABI5 version 1 (SYSV),statically linked, not stripped
$  arm-none-eabi-objcopy -O binary output.elf output.bin
$ file output.bin 
output.bin: data
```

## 8. 使用qemu-system-arm運行output.bin:

```sh
$ qemu-system-arm --help | grep nographic 
-nographic      disable graphical output and redirect serial I/Os to console.
$ qemu-system-arm -M versatilepb -nographic -kernel output.bin
Hello Open World
```

## 9. Play more tricks: 改動init.c裡的串口輸出地址為串口1：

```sh
// 0x101f1000  --> 0x101f2000
volatile unsigned char* const UART0_PTR = (unsigned char*)0x0101f2000; 
```

按照步驟3～7里重新編譯，並運行以查看結果:

```sh
# 沒有反應！
$ qemu-system-arm -M versatilepb -nographic -kernel output.bin
# 終端有輸出字符。
$ qemu-system-arm -M versatilepb -kernel output.bin -serial vc:800x600 -serial stdio
Hello Open World
```

同樣你也可以把字符輸出到第三個串口，只不過前兩個-serial的重定向需要指定到別的設備而已。

## 1. 下載並交叉編譯u-boot。

新版本的u-boot我加載後總有問題，2009.11版則可以順利通過編譯和測試。


```sh
$ wget ftp://ftp.denx.de/pub/u-boot/u-boot-2009.11.tar.bz2
$ tar xjvf u-boot-2009.11.tar.bz2 
$ cd u-boot-2009.11
$ make versatilepb_config arch=ARM CROSS_COMPILE=arm-none-eabi-
$ make all arch=ARM CROSS_COMPILE=arm-none-eabi- 
```

編譯完成後會在目錄下生成u-boot.bin和u-boot文件。

## 2. 運行u-boot.bin:

```sh
$ qemu-system-arm -M versatilepb -kernel u-boot.bin -nographic
```

如果採用-nographic來運行qemu-system-arm，終端將無法再響應任何系統輸入譬如Ctrl+c/ctrl+d_，要終止qemu-system-arm就只能查到進程號再kill。所以我一般不帶-nographic選項，啟動後ctrl+alt+2去看serial0輸出,保留在終端窗口直接ctrl+c殺死qemu-sytem-arm進程的權力。

## 3. 用u-boot引導鏡像文件:

改動上一篇文章裡用於構建啟動鏡像的linker.ld文件，因為u-boot.bin文件大小的緣故，我們需要把啟動鏡像的起始地址整體上移.

```sh
$ ls -l -h u-boot.bin 
-rwxr-xr-x 1 dash root 85K Jul  8 15:57 u-boot.bin
```

linker.ld文件裡， 0x100000，這個大小相比於85K顯然已經足夠。

```sh
ENTRY(_Start)
SECTIONS
{
. = 0x100000;
startup : { startup.o(.text)}
.data : {*(.data)}
.bss : {*(.bss)}
. = . + 0x500;
sp_top = .;
}
```

按上一章的編譯方法生成output.bin，不再重述。

使用mkimage工具創建u-boot可識別的image文件：

```sh
$ mkimage -A arm -C none -O linux -T kernel -d output.bin -a 0x00100000 -e 0x00100000 output.uimg
Image Name:   
Created:      Mon Jul  8 16:04:11 2013
Image Type:   ARM Linux Kernel Image (uncompressed)
Data Size:    152 Bytes = 0.15 kB = 0.00 MB
Load Address: 00100000
Entry Point:  00100000

$ file *.uimg
output.uimg: u-boot legacy uImage, , Linux/ARM, OS Kernel Image (Not \
compressed), 152 bytes, Mon Jul  8 16:04:11 2013, Load Address: 0x00100000,\
Entry Point: 0x00100000, Header CRC: 0x3C62F575, Data CRC: 0x69CE9647
```

將u-boot.bin和output.uimg打包為一個文件：

```sh
$ cat u-boot.bin output.uimg >flash.bin
```

下面這條命令用於計算output.img在使用u-boot加載完flash.bin後在內存中的地址，-kernel選項告訴qemu從0x100000開始加載鏡像，即65536。 65536+u-boot.bin後的大小，即output.img在內存中的地址。printf則是用16進制的格式打印出來，以便加載.

```sh
$ printf "0x%X" $(expr $(stat -c%s u-boot.bin) + 65536)
0x2525C
```

啟動qemu-system-arm並運行自定義鏡像:


```sh
$ qemu-system-arm -M versatilepb -nographic -kernel flash.bin
# iminfo 0x2525c

## Checking Image at 0002525c ...
   Legacy image found
   Image Name:   
   Image Type:   ARM Linux Kernel Image (uncompressed)
   Data Size:    152 Bytes =  0.1 kB
   Load Address: 00100000
   Entry Point:  00100000
   Verifying Checksum ... OK

VersatilePB # bootm 0x2525c
## Booting kernel from Legacy Image at 0002525c ...
   Image Name:   
   Image Type:   ARM Linux Kernel Image (uncompressed)
   Data Size:    152 Bytes =  0.1 kB
   Load Address: 00100000
   Entry Point:  00100000
   Loading Kernel Image ... OK
OK

Starting kernel ...

Hello Open World
```


u-boot可以支持的選項還有很多，包括使用NFS/TFTP啟動等等，留待以後慢慢研究。

