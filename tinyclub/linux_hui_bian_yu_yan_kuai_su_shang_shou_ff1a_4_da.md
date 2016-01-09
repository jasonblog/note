# Linux 彙編語言快速上手：4大架構一塊學

---
title: Linux 彙編語言快速上手：4大架構一塊學
author: Wu Zhangjin
layout: post
permalink: /linux-assembly-language-quick-start/
tags:
  - Linux
  - Qemu-User-Static
  - X86
categories:
  - 彙編
  - ARM
  - MIPS
  - PowerPC
  - X86
  - Qemu
---

> By Falcon of [TinyLab.org][1]
> 2015/05/13


## 前言

萬事開頭難。如果初次接觸，可能會覺得彙編語言很難下手。但現如今，學習彙編語言非常方便，本文就此展開。

## 實驗環境

早期學習彙編語言困難，有很大一個原因是沒有合適的實驗環境：

  * 沒有錢買開發板
  * 找不到合適的開發板
  * 有了開發板跑起來也沒那麼容易

現在學彙編語言根本不需要開發板，可以用 `qemu-user-static` 直接運行各種架構的彙編語言。

以 Ubuntu 為例，Windows 和 Mac 下的用戶可以先安裝 VirtualBox + Ubuntu，再安裝這個。

    sudo apt-get install qemu-user-static


接著安裝 gcc。

    sudo apt-get install gcc
    sudo apt-get install gcc-arm-linux-gnueabi gcc-aarch64-linux-gnu
    sudo apt-get install gcc-powerpc-linux-gnu gcc-powerpc64le-linux-gnu


因為 Ubuntu 自帶的交叉編譯工具不全，可以從 emdebian 項目安裝更多交叉編譯工具。

    sudo -s
    echo deb http://www.emdebian.org/debian/ wheezy main >> /etc/apt/sources.list.d/emdebian.list
    apt-get install emdebian-archive-keyring
    apt-get update
    apt-get install gcc-4.3-mipsel-linux-gnu


## Hello World

同大多數資料一樣，我們也從 Hello World 入手。

學習一個東西比較高效的方式是照貓畫虎，咱們先直接從 C 語言生成一個彙編語言程序。

### C 語言版本

先寫一個 C 語言的 `hello.c`：

```c
#include &lt;stdio.h>

int main(int argc, char *argv[])
{
        printf("Hello World\n");

        return 0;
}
```

### 彙編語言版本

生成彙編語言：

    gcc -S hello.c


默認會生成 hello.s，可以用 `-o hello-x86_64.s` 指定輸出文件名稱。

    gcc -S hello.c -o hello-x86_64.s


下面類似地，列出所有 4 個平臺 32位 和 64位 彙編語言生成辦法。

  * X86

        gcc -m32 -S hello.c -o hello-x86.s
        gcc -S hello.c -o hello-x86_64.s


  * MIPS

        mipsel-linux-gnu-gcc -S hello.c hello-mips.s
        mipsel-linux-gnu-gcc -mabi=64 -S hello.c -o hello-mips64.s


  * ARM

        arm-linux-gnueabi-gcc -S hello.c -o hello-arm.s
        aarch64-linux-gnu-gcc -S hello.c -o hello-arm64.s


  * PowerPC

        powerpc-linux-gnu-gcc -S hello.c -o hello-powerpc.s
        powerpc64le-linux-gnu-gcc -S hello.c -o hello-powerpc64.s


我們就這樣輕鬆地獲得了所有平臺的第一個可以打印 Hello World 的彙編語言程序：hello-xx.s。

大家可以用 `vim` 等編輯工具打開這些文件試讀，讀不懂也沒關係，我們下一節會結合後續的參考資料做進一步分析。

### 編譯彙編語言程序

在進一步分析前，我們演示如何把彙編語言編譯成可執行文件。

#### 靜態編譯

如果要直接在當前系統中運行，簡便起見，需要把各類庫靜態編譯進去（X86實際不需要，因為主機本身就是X86平臺），可以這麼做：

  * X86

        gcc -m32 -o hello-x86 hello-x86.s -static
        gcc -o hello-x86_64 hello-x86_64.s -static


  * MIPS

        mipsel-linux-gnu-gcc -o hello-mips hello-mips.s -static


    <!--        mipsel-linux-gnu-gcc -mabi=64 -Wl,-melf64ltsmip -o hello-mips64 hello-mips64.s -static -->

  * ARM

        arm-linux-gnueabi-gcc -o hello-arm hello-arm.s -static
        aarch64-linux-gnu-gcc -o hello-arm64 hello-arm64.s -static


  * PowerPC

        powerpc-linux-gnu-gcc -o hello-powerpc hello-powerpc.s -static


    <!--        powerpc64le-linux-gnu-gcc -o hello-powerpc64 hello-powerpc64.s -static -->

#### 動態編譯

靜態編譯的缺點是把所有用到的庫都默認編譯進了可執行文件，會導致編譯出來的可執行文件佔用較多磁盤，而且在運行時佔用更多內存。

所以可以考慮用動態編譯。動態編譯與靜態編譯的區別是，動態編譯需要有動態庫裝載和鏈接器：`ld.so` 或者 `ld-linux.so`，這個工具的路徑默認在 `/lib` 下。例如：

    $ ldd hello-x86
    linux-gate.so.1 =>  (0xf76ea000)
    libc.so.6 => /lib/i386-linux-gnu/libc.so.6 (0xf7508000)
    /lib/ld-linux.so.2 (0xf76eb000)
    $ mipsel-linux-gnu-readelf -l hello-mips | grep interpreter
      [Requesting program interpreter: /lib/ld.so.1]


所以，除了 x86 以外，對於相關庫都安裝在非標準路徑下，所以動態編譯或者運行時，其他架構需要明確指定庫的路徑。先通過如下命令獲取 `ld.so` 的安裝路徑：

    $ dpkg -L libc6-mipsel-cross | grep ld.so
    /usr/mipsel-linux-gnu/lib/ld.so.1


發現所有庫都安裝在 `/usr/ARCH-linux-gnu[eabixx]/lib/` 下面，所以，可以這麼執行：

    $ LD_LIBRARY_PATH=/usr/mipsel-linux-gnu/lib/
    $ qemu-mipsel $LD_LIBRARY_PATH/ld.so.1 --library-path $LD_LIBRARY_PATH ./hello-mips

    或者

    $ qemu-mipsel -E LD_LIBRARY_PATH=$LD_LIBRARY_PATH $LD_LIBRARY_PATH/ld.so.1 ./hello-mips


通過上面的方法在 x86 下執行其他架構的程序確實不方便，不過比買開發板划算多了吧。何況咱們還可以寫個腳本來替代上面的一長串的命令。

實際上咱們可以更簡化一些，可以在編譯時指定 `ld.so` 的全路徑：

    $ mipsel-linux-gnueabi-gcc -Wl,--dynamic-linker=/usr/mipsel-linux-gnueabi/lib/ld.so.1 -o hello hello.c
    $ readelf -l hello | grep interpreter
      [Requesting program interpreter: /usr/arm-linux-gnueabi/lib/ld-linux.so.3]
    $ qemu-mipsel -E LD_LIBRARY_PATH=$LD_LIBRARY_PATH ./hello-mips


不過這種方法也不是那麼靠譜。

可選的辦法是，用 `debootstrap` 安裝一個完整的支持其他架構的文件系統，然後把 `/usr/bin/qemu-XXX-static` 拷貝到目標文件系統的 `/usr/bin` 下，然後 `chroot` 過去使用。這裡不做進一步介紹了。

### 彙編語言分析

上面介紹瞭如何快速獲得一個可以打印 Hello World 的彙編語言程序。不過咋一看，簡直是天書。

作為快速上手，咱們也沒有過多篇幅來介紹太多的背景，因為涉及的背景實在太多。會涉及到：

  * [ELF][2] 可執行文件格式以及各類 Sections
  * [函數調用約定 ABI][3]，包括參數傳遞，棧操作，返回地址處理等
  * [各種 gas 偽指令][4]
  * [庫函數的動態鏈接][5]

這些內容是不可能在幾百文字裡頭描述清楚的，所以乾脆跳過交給同學們自己參考後續資料後再回過頭來閱讀。咱們進入下一節，看看更簡單的實現。

## 進階學習

如果是簡單打印 Hello World，咱們其實可以不用調用庫函數，可以直接調用系統調用 `sys_write`。`sys_write` 是一個標準的 Posix 系統調用，各平臺都支持。參數完全一致，不過各平臺的系統調用號可能有差異：

    ssize_t write(int fd, const void *buf, size_t count);


系統調用號基本都定義在：`arch/ARCH/include/asm/unistd.h`。例如：

    $ grep __NR_write -ur arch/mips/include/asm/
    arch/mips/include/asm/unistd.h:#define __NR_write           (__NR_Linux +   4)


而 _\_NR\_Linux 為 4000：

     $ grep __NR_Linux -ur arch/mips/include/asm/ -m 1
     arch/mips/include/asm/unistd.h:#define __NR_Linux          4000


所以，在 MIPS 上，系統調用號為 4004，具體看後面的例子。

下面來看看簡化後的例子，例子全部摘自後文的參考資料。

### X86

```
.data                   # section declaration
msg:
    .string "Hello, world!\n"
    len = . - msg   # length of our dear string
.text                   # section declaration
                        # we must export the entry point to the ELF linker or
    .global _start      # loader. They conventionally recognize _start as their
                        # entry point. Use ld -e foo to override the default.
_start:
# write our string to stdout
    movl    $len,%edx   # third argument: message length
    movl    $msg,%ecx   # second argument: pointer to message to write
    movl    $1,%ebx     # first argument: file handle (stdout)
    movl    $4,%eax     # system call number (sys_write)
    int     $0x80       # call kernel
# and exit
    movl    $0,%ebx     # first argument: exit code
    movl    $1,%eax     # system call number (sys_exit)
    int     $0x80       # call kernel
```

編譯和鏈接：

    $ as -o ia32-hello.o ia32-hello.s
    $ ld -o ia32-hello ia32-hello.o


### MIPS

```
# File: hello.s -- "hello, world!" in MIPS Assembly Programming
# by falcon &lt;wuzhangjin@gmail.com>, 2008/05/21
# refer to:
#    [*] http://www.tldp.org/HOWTO/Assembly-HOWTO/mips.html
#    [*] MIPS Assembly Language Programmer’s Guide
#    [*] See MIPS Run Linux(second version)
# compile:
#       $ as -o hello.o hello.s
#       $ ld -e main -o hello hello.o

# data section
.rdata
hello: .asciiz "hello, world!\n"
length: .word . - hello            # length = current address - the string address

# text section
.text
.globl main
main:
    # if compiled with gcc-4.2.3 in 2.6.18-6-qemu the following three statements are needed

    .set noreorder
    .cpload $t9
    .set reorder

            # there is no need to include regdef.h in gcc-4.2.3 in 2.6.18-6-qemu
            # but you should use $a0, not a0, of course, you can use $4 directly

            # print "hello, world!" with the sys_write system call,
            # -- ssize_t write(int fd, const void *buf, size_t count);
    li $a0, 1    # first argumen: the standard output, 1
    la $a1, hello    # second argument: the string addr
    lw $a2, length  # third argument: the string length
    li $v0, 4004    # sys_write: system call number, defined as __NR_write in /usr/include/asm/unistd.h
    syscall        # causes a system call trap.

            # exit from this program via calling the sys_exit system call
    move $a0, $0    # or "li $a0, 0", set the normal exit status as 0
            # you can print the exit status with "echo $?" after executing this program
    li $v0, 4001    # 4001 is __NR_exit defined in /usr/include/asm/unistd.h
    syscall
```

編譯和鏈接：

    $ mipsel-linux-gnu-as -o mipsel-hello.o mipsel-hello.s
    $ mipsel-linux-gnu-ld -o mipsel-hello mipsel-hello.o


### ARM

#### ARM32

```
.data

msg:
    .ascii      "Hello, ARM!\n"
len = . - msg


.text

.globl _start
_start:
    /* syscall write(int fd, const void *buf, size_t count) */
    mov     %r0, $1     /* fd -> stdout */
    ldr     %r1, =msg   /* buf -> msg */
    ldr     %r2, =len   /* count -> len(msg) */
    mov     %r7, $4     /* write is syscall #4 */
    swi     $0          /* invoke syscall */

    /* syscall exit(int status) */
    mov     %r0, $0     /* status -> 0 */
    mov     %r7, $1     /* exit is syscall #1 */
    swi     $0          /* invoke syscall */
```

編譯和鏈接：

    $ arm-linux-gnueabi-as -o arm-hello.o arm-hello.s
    $ arm-linux-gnueabi-ld -o arm-hello arm-hello.o


#### ARM64

```
.text //code section
.globl _start
_start:
    mov x0, 0     // stdout has file descriptor 0
    ldr x1, =msg  // buffer to write
    mov x2, len   // size of buffer
    mov x8, 64    // sys_write() is at index 64 in kernel functions table
    svc #0        // generate kernel call sys_write(stdout, msg, len);

    mov x0, 123 // exit code
    mov x8, 93  // sys_exit() is at index 93 in kernel functions table
    svc #0      // generate kernel call sys_exit(123);

.data //data section
msg:
    .ascii      "Hello, ARM!\n"
len = . - msg
```

編譯和鏈接：

    aarch64-linux-gnu-as -o aarch64-hello.o aarch64-hello.s
    aarch64-linux-gnu-ld -o aarch64-hello aarch64-hello.o


### PowerPC

#### PPC32

```

.data                       # section declaration - variables only
msg:
    .string "Hello, world!\n"
    len = . - msg       # length of our dear string
.text                       # section declaration - begin code
    .global _start
_start:
# write our string to stdout
    li      0,4         # syscall number (sys_write)
    li      3,1         # first argument: file descriptor (stdout)
                        # second argument: pointer to message to write
    lis     4,msg@ha    # load top 16 bits of &#038;msg
    addi    4,4,msg@l   # load bottom 16 bits
    li      5,len       # third argument: message length
    sc                  # call kernel
# and exit
    li      0,1         # syscall number (sys_exit)
    li      3,1         # first argument: exit code
    sc                  # call kernel
```

編譯和鏈接：

    $ powerpc-linux-gnu-as -o ppc32-hello.o ppc32-hello.s
    $ powerpc-linux-gnu-ld -o ppc32-hello ppc32-hello.o


#### PPC64

```
.data                       # section declaration - variables only
msg:
    .string "Hello, world!\n"
    len = . - msg       # length of our dear string
.text                       # section declaration - begin code
        .global _start
        .section        ".opd","aw"
        .align 3
_start:
        .quad   ._start,.TOC.@tocbase,0
        .previous
        .global  ._start
._start:
# write our string to stdout
    li      0,4         # syscall number (sys_write)
    li      3,1         # first argument: file descriptor (stdout)
                        # second argument: pointer to message to write
    # load the address of 'msg':
                        # load high word into the low word of r4:
    lis 4,msg@highest   # load msg bits 48-63 into r4 bits 16-31
    ori 4,4,msg@higher  # load msg bits 32-47 into r4 bits  0-15
    rldicr  4,4,32,31   # rotate r4's low word into r4's high word
                        # load low word into the low word of r4:
    oris    4,4,msg@h   # load msg bits 16-31 into r4 bits 16-31
    ori     4,4,msg@l   # load msg bits  0-15 into r4 bits  0-15
    # done loading the address of 'msg'
    li      5,len       # third argument: message length
    sc                  # call kernel
# and exit
    li      0,1         # syscall number (sys_exit)
    li      3,1         # first argument: exit code
    sc                  # call kernel
```

編譯和鏈接：

    $ powerpc-linux-gnu-as -a64 -o ppc64-hello.o ppc64-hello.s
    $ powerpc-linux-gnu-ld -melf64ppc -o ppc64-hello ppc64-hello.o


## 小結

到這裡，四種主流處理器架構的最簡彙編語言都玩轉了，接下來就是根據後面的各類參考資料，把各項基礎知識研究透徹吧。

## 參考資料

### 書籍

  * X86: x86/x64 體系探索及編程
  * ARM: ARM System Developers’ Guide: Designing and Optimizing System Software
  * MIPS: See MIPS Run Linux
  * PowerPC: PowerPC™ Microprocessor Common Hardware Reference Platform: A System Architecture

### 指令手冊

  * [ARM][6]
  * [MIPS][7]
  * [X86][8]
  * [PowerPC][9]

### 課程/文章

  * 基礎

      * [Linux Assembly HOWTO][10]
      * [Linux 彙編語言開發指南][11]
      * [Linux 彙編器：對比 GAS 和 NASM][12]
      * [Linux 彙編語言資料列表][13]
      * [Using as, the Gnu Assembler][4]

  * X86

      * [CS630][14]
      * [Learn CS630 on Qemu in Ubuntu][15]
      * [Linux 中 x86 的內聯彙編][16]
      * [史上可打印 Hello World的彙編語言程序][17]

  * ARM

      * [‘Hello World!’ in ARM assembly][18]
      * [ARM Assembly Language Programming][19]
      * [Whirlwind Tour of ARM Assembly][20]
      * [ARM GCC Inline Assembler Cookbook][21]
      * [Hello world in assembly language ARM64][22]

  * MIPS

      * [Programmed Introduction to MIPS Assembly Language][23]
      * [MIPS Architecture and Assembly Language Overview][34]
      * [MIPS Assembly Language Programmer’s Guide][24]
      * [MIPS Assembly Language Examples][25]
      * [MIPS GCC 嵌入式彙編（龍芯適用）][26]

  * PowerPC

      * [PowerPC 體系結構開發者指南][27]
      * [PowerPC 彙編][28]
      * 用於 Power 體系結構的彙編語言, [1][29]; [2][30], [3][31]; [4][32]
      * [PowerPC 內聯彙編 &#8211; 從頭開始][33]

  * ELF

      * [TN05.ELF.Format.Summary.pdf][2]





 [1]: http://tinylab.org
 [2]: http://www.xfocus.net/articles/200105/174.html
 [3]: http://blog.chinaunix.net/uid-16875687-id-2155704.html
 [4]: https://stuff.mit.edu/afs/athena/project/rhel-doc/3/rhel-as-en-3/index.html
 [5]: /details-of-a-dynamic-symlink/
 [6]: http://www.arm.com/products/processors/cortex-a/index.php
 [7]: http://www.imgtec.com/mips/architectures/mips32.asp
 [8]: http://www.intel.sg/content/www/xa/en/search.html?toplevelcategory=none&keyword=architectures
 [9]: http://www.ibm.com/developerworks/systems/library/es-archguide-v2.html
 [10]: http://www.tldp.org/HOWTO/html_single/Assembly-HOWTO/
 [11]: http://www.ibm.com/developerworks/cn/linux/l-assembly/index.html#icomments
 [12]: http://www.ibm.com/developerworks/cn/linux/l-gas-nasm.html
 [13]: http://asm.sourceforge.net/resources.html
 [14]: http://www.cs.usfca.edu/~cruse/cs630f06/
 [15]: /cs630-qemu/
 [16]: http://www.ibm.com/developerworks/cn/linux/sdk/assemble/inline/index.html
 [17]: /as-an-executable-file-to-slim-down/
 [18]: http://peterdn.com/post/e28098Hello-World!e28099-in-ARM-assembly.aspx
 [19]: http://www.peter-cockerell.net/aalp/html/frames.html
 [20]: http://www.coranac.com/tonc/text/asm.htm
 [21]: http://www.ethernut.de/en/documents/arm-inline-asm.html
 [22]: http://deker.ro/index.htm
 [23]: https://chortle.ccsu.edu/AssemblyTutorial/index.html
 [24]: https://courses.cs.washington.edu/courses/cse401/04sp/pl0/mips.pdf
 [25]: http://courses.cs.washington.edu/courses/cse378/03wi/lectures/mips-asm-examples.html
 [26]: http://blog.csdn.net/comcat/article/details/1557963
 [27]: http://www.ibm.com/developerworks/cn/linux/l-powarch/index.html
 [28]: http://www.ibm.com/developerworks/cn/linux/hardware/ppc/assembly/index.html
 [29]: http://www.ibm.com/developerworks/cn/linux/l-powasm1.html
 [30]: http://www.ibm.com/developerworks/cn/linux/l-powasm2.html
 [31]: http://www.ibm.com/developerworks/cn/linux/l-powasm3.html
 [32]: http://www.ibm.com/developerworks/cn/linux/l-powasm4.html
 [33]: http://www.ibm.com/developerworks/cn/aix/library/au-inline_assembly/index.html

 [34]: http://logos.cs.uic.edu/366/notes/MIPS%20Quick%20Tutorial.htm
