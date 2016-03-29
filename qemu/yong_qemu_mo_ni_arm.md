# 用Qemu模擬ARM



前面已經安裝並配置了編譯鏈和qemu，現在可以用qemu來模擬arm平臺了。

###1. Hello, Qemu!

輸入下面的代碼:
- hello.c

```c
#include<stdio.h> 

int main()
{
    printf("Hello, Qemu!\n");
    return 0;
}
```

編譯並運行:

```sh
$ arm-none-linux-gnueabi-gcc -o hello hello.c -static
$ qemu-arm ./hello
$ file hello
hello: ELF 32-bit LSB  executable, ARM, EABI5 version 1 (SYSV), \
 statically linked, for GNU/Linux 2.6.16, not stripped
```

不加-static變量的話，運行時則需要使用-L選項鍊接到相應的運行庫

```sh
$ qemu-arm -L /home/dash/CodeSourcery/\
Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/\
arm-none-linux-gnueabi/libc/  ./hello_1 
Hello, Qemu!
$ file hello_1
hello_1: ELF 32-bit LSB  executable, ARM, EABI5 version 1 (SYSV),\
 dynamically linked (uses shared libs), for GNU/Linux 2.6.16, not stripped
```


動態編譯和靜態編譯生成的文件大小差別：

```sh
$ ls -l -h
total 656K
-rwxr-xr-x 1 dash root 640K Jul  7 18:46 hello
-rwxr-xr-x 1 dash root 6.6K Jul  7 18:48 hello_1
```

###小插曲1：

系統裡安裝了兩套編譯鏈arm-none-eabi-和arm-none-linux-eabi-,很容易讓人混淆，可參考編譯鏈的命名規則：

arch(架構)-vendor(廠商名)–(os(操作系統名)–)abi(Application Binary Interface，應用程序二進制接口)


舉例說明：

- x86_64-w64-mingw32 = x86_64 “arch”字段 (=AMD64), w64 (=mingw-w64 是”vendor”字段), mingw32 (=GCC所見的win32 API)
- i686-unknown-linux-gnu = 32位 GNU/linux編譯鏈
- arm-none-linux-gnueabi = ARM 架構, 無vendor字段, linux 系統, gnueabi ABI.
- arm-none-eabi = ARM架構, 無廠商, eabi ABI(embedded abi)

兩種編譯鏈的主要區別在於庫的差別，前者沒有後者的庫多，後者主要用於在有操作系統的時候編譯APP用的。前者不包括標準輸入輸出庫在內的很多C標準庫，適合於做面向硬件的類似單片機那樣的開發。因而如果採用arm-none-eabi-gcc來編譯hello.c會出現鏈接錯誤。

### 小插曲2：

qemu-arm和qemu-system-arm的區別：


- qemu-arm是用戶模式的模擬器(更精確的表述應該是系統調用模擬器)，而qemu-system-arm則是系統模擬器，它可以模擬出整個機器並運行操作系統

- qemu-arm僅可用來運行二進制文件，因此你可以交叉編譯完例如hello world之類的程序然後交給qemu-arm來運行，簡單而高效。而qemu-system-arm則需要你把hello world程序下載到客戶機操作系統能訪問到的硬盤裡才能運行。


###2. 使用qemu-system-arm運行Linux內核

從www.kernel.org下載最新內核,而後解壓

```sh
$ tar xJf linux-3.10.tar.xz
$ cd linux-3.10
$ make ARCH=arm versatile_defconfig
$ make menuconfig ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-
```

上面的命令指定內核架構為arm，交叉編譯鏈為arm-none-linux-gnueabi, 需要在make menuconfig彈出的窗口中選擇到 “Kernel Features”, 激活“Use the ARM EABI to compile the kernel”, 如果不激活這個選項的話，內核將無法加載接下來要製作的initramfs。

如果需要在u-boot上加載內核，就要編譯為uImage的格式，uImage通過mkimage程序來壓縮的，ArchLinux的yaourt倉庫裡可以找到這個包：

```sh
$ yaourt -S mkimage
```
安裝好mkimage後，開始編譯內核，因為CPU有4核，所以開啟了-j8選項以加速編譯:

```sh
$ make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- all -j8 uImage 
```

接下來我們可以在qemu-system-arm中測試我們的內核了

```sh
$ qemu-system-arm -M versatilepb -m 128M -kernel ./arch/arm/boot/uImage
```

在彈出的窗口中可以內核運行到了kernel panic狀態，這是因為內核無法加載root鏡像的緣故，我們將製作一個最簡單的hello world的文件系統，告知kernel運行之。

-  init.c

```c
#include <stdio.h>

void main()
{
    printf("Hello World!\n");
    while (1);
}
```

編譯並製作啟動鏡像:

```sh
$ arm-none-linux-gnueabi-gcc -o init init.c -static
$ echo init |cpio -o --format=newc > initramfs
1280 blocks
$ file initramfs 
initramfs: ASCII cpio archive (SVR4 with no CRC)
```

接下來我們回到編譯目錄下執行：

```sh
$ qemu-system-arm -M versatilepb -kernel ./arch/arm/boot/uImage  -initrd
../initramfs -serial stdio -append "console=tty1"
```

這時候可以看到，kernel運行並在Qemu自帶的終端裡打印出”Hello World!“。

如果我們改變console變量為ttyAMA0, 將在啟動qemu-system-arm的本終端上打印出qemu的輸出。

## 1. 關於Bootloader:

(引導程序)位於電腦或其他計算機應用上，是指引導操作系統啟動的程序。引導程序啟動方式和程序視應用機型種類而不同。例如在普通的個人電腦上，引導程序通常分為兩部分：第一階段引導程序位於主引導記錄（MBR），用以引導位於某個分區上的第二階段引導程序，如NTLDR、GNU GRUB等。

嵌入式系統中常見的Bootloader主要有以下幾種:

- Das U-Boot 是一個主要用於嵌入式系統的開機載入程序，可以支持多種不同的計算機系統結構，包括PPC、ARM、AVR32、MIPS、x86、68k、Nios與MicroBlaze。

- vivi是由mizi公司設計為ARM處理器系列設計的一個bootloader.

- Redboot (Red Hat Embedded Debug and Bootstrap)是Red Hat公司開發的一個獨立運行在嵌入式系統上的BootLoader程序，是目前比較流行的一個功能、可移植性好的BootLoader。

## 2. 關於“裸機編程(Bare-Metal)”:


微控制器開發人員很熟悉這個概念， Bare-Metal是指的你的程序和處理器之間沒有任何東西——你寫的程序將直接運行在處理器上, 換言之，開發人員是在直接操控硬件。在裸機編程的場景中，需要由開發人員檢查並排除任何一個可以導致系統崩潰的風險。

“Bare-Metal”要求開發人員瞭解關於硬件的細節，所以接下來我們將對編譯鏈和qemu本身進行分析。


## 3. 下載qemu源碼包並查詢相關硬件信息：

ArchLinux採用ABS(Arch Build System)來管理源碼包，下面的步驟將qemu源碼包下載到本地，更詳細的關於ABS的操作可以在ArchLinux的Wiki中找到

```sh
$ pacman -S abs
$ pacman -Ss qemu
extra/qemu 1.4.2-2 [installed]
$ abs extra/qemu 
$ cp -r /var/abs/extra/qemu/ ~/abs 
$ cd ~/abs && makepkg -s --asroot -o
```

得到versatilepb開發板的CPU型號, 可以看到”arm926”是我們要的結果。

```sh
$ grep "arm" src/qemu-1.4.2/hw/versatilepb.c 
#include "arm-misc.h"
static struct arm_boot_info versatile_binfo;
        args->cpu_model = "arm926";
    cpu = cpu_arm_init(args->cpu_model);
    cpu_pic = arm_pic_init_cpu(cpu);
    arm_load_kernel(cpu, &versatile_binfo);
```

得到versatilepb開發板的串口寄存器硬件信息：

```sh
$ grep "UART*" src/qemu-1.4.2/hw/versatilepb.c 
    /*  0x10009000 UART3.  */
    /*  0x101f1000 UART0.  */
    /*  0x101f2000 UART1.  */
    /*  0x101f3000 UART2.  */
```

所以說開源是王道嘛，很快就查到了每一個需要了解的細節。UART0在內存中map到的地址是0x101f1000, 我們直接往這個地址寫數據，就可以在終端上看到數據輸出了。

##4. 查看編譯鏈支持的平臺：

```sh
$ cat ~/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI/share/doc/arm-arm-none-eabi/info/gcc.info | grep arm926
     `arm926ej-s', `arm940t', `arm9tdmi', `arm10tdmi', `arm1020t',
```

arm926ej-s是被支持的，因此我們可以用這套編譯鏈來生成需要的裸機調試代碼。

##5. 啟動應用程序init.c的編寫:

首先創建應用程序init.c：

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

init.c中，我們首先聲明一個volatile變UART0_PTR,volatile關鍵字用於告知編譯器此變量是用於直接訪問內存映像設備的，即串口0內存地址

display()函數則是用於將字符串中的字符按順序輸出到串口0, 直到遇到字符串結尾。

my_init()調用了display(), 接下來我們將把它作為C入口函數.

預編譯init.c:

```sh
$ arm-none-eabi-gcc -c -mcpu=arm926ej-s init.c -o init.o
```

## 6. 啟動代碼start.s編寫：
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

按照步驟3～7裡重新編譯，並運行以查看結果:

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

