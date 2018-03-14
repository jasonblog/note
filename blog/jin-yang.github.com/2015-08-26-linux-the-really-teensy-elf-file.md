---
title: 最小的ELF文件
layout: post
comments: true
language: chinese
category: [linux,program,misc]
keywords: mysql,database,handler
description: 從最經典 "Hello World" C 程序實例，逐步演示如何通過各種常用工具來分析 ELF 文件，並逐步精簡代碼，儘量減少可執行文件的大小。接下來，看看一個可打印 "Hello World" 的可執行文件能夠小到什麼樣的地步。
---

從最經典 "Hello World" C 程序實例，逐步演示如何通過各種常用工具來分析 ELF 文件，並逐步精簡代碼，儘量減少可執行文件的大小。

接下來，看看一個可打印 "Hello World" 的可執行文件能夠小到什麼樣的地步。

<!-- more -->

## 簡介

為了最小化可執行文件，需要了解可執行文件的格式，鏈接生成可執行文件時的後臺細節，有哪些內容被鏈接到了目標代碼中，通過選擇合適的可執行文件格式並剔除對可執行文件的最終運行沒有影響的內容，就可以實現目標代碼的裁減。

因此，通過探索減少可執行文件大小的方法，就相當於實踐性地去探索了可執行文件的格式以及鏈接過程的細節。

### 可執行文件格式

需要找到一個目標系統支持該可執行文件格式，在 *NIX 平臺下主要包括了以下的三種可執行文件格式，這三種格式基本上代表了可執行文件的一個發展過程：

* a.out，非常緊湊，只包含了程序運行所必須的信息 (文本、數據、BSS)，而且每個 section 的順序是固定的。
* coff，為了提高擴展性，引入了一個分區表以支持更多分區信息，重定位在鏈接時就已經完成，因此不支持動態鏈接，不過擴展 coff 支持。
* elf，不僅支持動態鏈接，而且有很好的擴展性，可描述可重定位文件、可執行文件和可共享文件 (動態鏈接庫) 三類文件。

如下是 ELF 文件的結構圖：

![linux elf format]({{ site.url }}/images/linux/elf-format.png "linux elf format"){: .pull-center }

無論是文件頭部、程序頭部表、節區頭部表還是各個節區，都是通過特定的結構體描述的，這些結構在 elf.h 文件中定義，例如 CentOS 保存在 `/usr/include/elf.h` 文件中。

文件頭部用於描述整個文件的類型、大小、運行平臺、程序入口、程序頭部表和節區頭部表等信息，可以通過如下方式查看文件頭部信息。

{% highlight text %}
----- Hello World示例程序
$ cat << EOF > hello.c
#include <stdio.h>
int main(void)
{
    printf("Hello World!\n");
    return 0;
}
EOF

----- 編譯產生可重定向的目標代碼，查看類型
$ gcc -c hello.c
$ readelf -h hello.o | grep Type
  Type:                              REL (Relocatable file)

----- 生成可執行文件，查看類型
$ gcc -o hello hello.o
$ readelf -h hello | grep Type
  Type:                              EXEC (Executable file)

----- 生成共享庫，並查看文件類型
$ gcc -fpic -shared -W1,-soname,libhello.so.0 -o libhello.so.0.0 hello.o
$ readelf -h libhello.so.0.0 | grep Type
  Type:                              DYN (Shared object file)
{% endhighlight %}

那麼 `Section Header Table, SHT` 和 `Program Header Table, PHT` 有什麼用呢？前者只對可重定向文件有用，而後者只對可執行文件和可共享文件有用。

### Section Header Table

SHT 用來描述各分區，包括各分區的名字、大小、類型、虛擬內存中的位置、相對文件頭的位置等，連接器可以根據文件頭部表和節區表的描述信息對各種輸入的可重定位文件進行合適的鏈接。

<!--
包括節區的合併與重組、符號的重定位（確認符號在虛擬內存中的地址）等，把各個可重定向輸入文件鏈接成一個可執行文件（或者是可共享文件）。如果可執行文件中使用了動態連接庫，那麼將包含一些用於動態符號鏈接的節區。
-->

可以通過 `readelf -S` 或者 `objdump -h` 查看分區表信息。

{% highlight text %}
----- 可執行文件、可共享庫、可重定位文件默認都生成有分區表
$ readelf -S hello
...
Section Headers:
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        08048114 000114 000013 00   A  0   0  1
  [ 2] .note.ABI-tag     NOTE            08048128 000128 000020 00   A  0   0  4
  [ 3] .hash             HASH            08048148 000148 000028 04   A  5   0  4
...
  [ 7] .gnu.version      VERSYM          0804822a 00022a 00000a 02   A  5   0  2
...
  [11] .init             PROGBITS        08048274 000274 000030 00  AX  0   0  4
...
  [13] .text             PROGBITS        080482f0 0002f0 000148 00  AX  0   0 16
  [14] .fini             PROGBITS        08048438 000438 00001c 00  AX  0   0  4
...
{% endhighlight %}

其中不同文件類型，其分區會有所區別，不過有部分分區，包括 `.text`、`.data`、`.bss` 是必須的，如果使用了動態鏈接庫，那麼需要 `.interp` 分區告知系統使用什麼動態連接器程序來進行動態符號鏈接，進行某些符號地址的重定位。

<!--
通常，.rel.text節區只有可重定向文件有，用於鏈接時對代碼區進行重定向，而.hash,.plt,.got等節區則只有可執行文件（或可共享庫）有，這些節區對程序的運行特別重要。還有一些節區，可能僅僅是用於註釋，比如.comment，這些對程序的運行似乎沒有影響，是可有可無的，不過有些節區雖然對程序的運行沒有用處，但是卻可以用來輔助對程序進行調試或者對程序運行效率有影響。
-->

### Program Header Table

雖然上述的三類文件都包含一些分區，不過分區表只有可重定位文件才是必須的，而程序的執行並不需要分區表，只需要程序頭部表以便知道如何加載和執行文件。另外，如果需要對可執行文件或者動態連接庫進行調試，那麼分區表是必要的，否則調試器將無法工作。

下面來介紹程序頭部表，可通過 `readelf -l` 或者 `objdump -p` 查看。

{% highlight text %}
----- 對於可重定向文件，gcc沒有產生程序頭部，因為它對可重定向文件沒用
$ readelf -l hello.o
There are no program headers in this file.

----- 可執行文件和可共享文件都有程序頭部
$  readelf -l hello
...
Program Headers:
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  PHDR           0x000034 0x08048034 0x08048034 0x000e0 0x000e0 R E 0x4
  INTERP         0x000114 0x08048114 0x08048114 0x00013 0x00013 R   0x1
      [Requesting program interpreter: /lib/ld-linux.so.2]
  LOAD           0x000000 0x08048000 0x08048000 0x00470 0x00470 R E 0x1000
  LOAD           0x000470 0x08049470 0x08049470 0x0010c 0x00110 RW  0x1000
  DYNAMIC        0x000484 0x08049484 0x08049484 0x000d0 0x000d0 RW  0x4
  NOTE           0x000128 0x08048128 0x08048128 0x00020 0x00020 R   0x4
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RW  0x4
 Section to Segment mapping:
  Segment Sections...
   00
   01     .interp
   02     .interp .note.ABI-tag .hash .gnu.hash .dynsym .dynstr .gnu.version ...
   03     .ctors .dtors .jcr .dynamic .got .got.plt .data .bss
   04     .dynamic
   05     .note.ABI-tag
{% endhighlight %}

從上面可看出程序頭部表描述了一些段 (Segment)，這些段對應著一個或者多個 Sections，可以通過 `readelf -l` 顯示了各個段與分區的映射。

<!--
這些段描述了段的名字、類型、大小、第一個字節在文件中的位置、將佔用的虛擬內存大小、在虛擬內存中的位置等。這樣系統程序解釋器將知道如何把可執行文件加載到內存中以及進行動態鏈接等動作。

該可執行文件包含7個段，PHDR指程序頭部，INTERP正好對應.interp節區，兩個LOAD段包含程序的代碼和數據部分，分別包含有.text和.data，.bss節區，DYNAMIC段包含.daynamic，這個節區可能包含動態連接庫的搜索路徑、可重定位表的地址等信息，它們用於動態連接器。NOTE和GNU_STACK段貌似作用不大，只是保存了一些輔助信息。因此，對於一個不使用動態連接庫的程序來說，可能只包含LOAD段，如果一個程序沒有數據，那麼只有一個LOAD段就可以了。
-->

簡單來說，Linux 支持多種可執行文件格式，但是目前 ELF 較通用，所以這裡選擇 ELF 作為討論對象。<!--通過上面對ELF文件分析發現一個可執行的文件可能包含一些對它的運行沒用的信息，比如節區表、一些用於調試、註釋的節區。如果能夠刪除這些信息就可以減少可執行文件的大小，而且不會影響可執行文件的正常運行。-->

## 最小化

僅僅從是否影響一個 C 語言程序運行的角度上說，GCC 默認鏈接到可執行文件的幾個可重定位文件 (`crt1.o` `rti.o` `crtbegin.o` `crtend.o` `crtn.o` ) 並不是必須的。

不過需要注意的是，如果沒有鏈接那些文件但在程序末尾使用了 return 語句，`main()` 將無法返回，需要替換為 `_exit()`<!--；另外，既然程序在進入main之前有一個入口，那麼main入口就不是必須的。因此，如果不採用默認鏈接也可以減少可執行文件的大小-->。

下面以 Hello World 為例，嘗試最小化示例程序。

### 1. 默認大小

{% highlight text %}
$ cat << EOF > hello.c
#include <stdio.h>
int main(void)
{
    printf("Hello World!\n");
    return 0;
}
EOF

----- 生成二進制文件，並查看其大小
$ gcc -o hello hello.c
$ wc -c hello
8520 hello
{% endhighlight %}

### 2. 自定義編譯

可以考慮把 `return 0` 替換成 `_exit(0)` 幷包含定義該函數的 `unistd.h` 頭文件，然後接著使用如下的 `Makefile` 進行編譯。

{% highlight text %}
$ cat << EOF > hello.c
#include <stdio.h>
#include <unistd.h>
int main(void)
{
    printf("Hello World!\n");
    _exit(0);
}
EOF

----- 生成彙編語言
$ gcc -S hello.c

----- 將main入口替換為_start
$ sed -i -e "s/main/_start/g" hello.s

----- 編譯鏈接生成可執行文件
$ gcc -c hello.s
$ ld -o hello hello.o -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2
$ wc -c hello
6192 hello
{% endhighlight %}

如果報 `Accessing a corrupted shared library` 錯誤，那麼可能是由於 ld 與編譯的程序位數不同，對於 32-bits 的程序，可以通過如下方式編譯鏈接。

{% highlight text %}
$ gcc -m32 -S hello.c
$ gcc -m32 -c hello.s
$ ld -o hello hello.o -m elf_i386 -lc -dynamic-linker /lib/ld-linux.so.2
{% endhighlight %}

### 3. 刪除無用分區

部分分區對於程序的運行並非必要的，可以通過 `strip -R` 或者 `objcop -R` 刪除。

{% highlight text %}
----- 查看分區信息
$ readelf -l hello | grep "Segment Sections..." -A 10
  Segment Sections...
   00     
   01     .interp 
   02     .interp .hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.plt .plt .text .rodata .eh_frame 
   03     .dynamic .got.plt 
   04     .dynamic 
   05     
   06     .dynamic 

----- 刪除兩個無用的分區
$ strip -R .hash hello
$ strip -R .gnu.version hello
$ wc -c hello
5192 hello
{% endhighlight %}

另外，可以從 [Kickers of ELF](http://www.muppetlabs.com/~breadbox/software/elfkickers.html) 下載編譯 sstrip 程序，然後執行 `sstrip hello` 刪除分區表，在此就不再測試了。

### 4. 使用匯編

接下來看看如何通過彙編進行優化，如下是直接通過 `gcc -S hello.c` 生成的彙編語言，然後以此為基礎進行修改。

{% highlight as %}
        .file   "hello.c"
        .section        .rodata
.LC0:
        .string "Hello World!"
        .text
.globl main
        .type   main, @function
main:
.LFB0:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        movl    $.LC0, %edi
        call    puts
        movl    $0, %edi
        call    _exit
        .cfi_endproc
.LFE0:
        .size   _start, .-_start
        .ident  "GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-11)"
        .section        .note.GNU-stack,"",@progbits
{% endhighlight %}

修改為如下。

{% highlight as %}
.LC0:
        .string "Hello World!"
        .text
.globl main
        .type   main, @function
main:
        .cfi_startproc
        pushq   %rbp
        .cfi_def_cfa_offset 16
        .cfi_offset 6, -16
        movq    %rsp, %rbp
        .cfi_def_cfa_register 6
        movl    $.LC0, %edi
        call    puts
        movl    $0, %edi
        call    _exit
        .cfi_endproc
{% endhighlight %}

接著看看直接通過 `_start()` 作為入口。

{% highlight text %}
----- 替換掉main
$ sed -i -e "s/main/_start/g" hello.s
----- 編譯鏈接
$ as --64 -o  hello.o hello.s
$ ld -o hello hello.o -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2

----- 刪除兩個無用的分區以及分區表
$ strip -R .hash hello
$ strip -R .gnu.version hello
$ sstrip hello
$ wc -c hello
4136 hello
{% endhighlight %}

### 5. 使用系統調用

也可以直接使用系統調用，此時鏈接時就不需要依賴 lib 庫了。

{% highlight text %}
$ cat hello.s
    .global _start
    .text
_start:
    # write(1, message, 13)
    mov     $1, %rax                # system call 1 is write
    mov     $1, %rdi                # file handle 1 is stdout
    mov     $message, %rsi          # address of string to output
    mov     $13, %rdx               # number of bytes
    syscall                         # invoke operating system to do the write

    # exit(0)
    mov     $60, %rax               # system call 60 is exit
    xor     %rdi, %rdi              # we want return code 0
    syscall                         # invoke operating system to exit
message:
    .ascii  "Hello World!\n"

----- 編譯鏈接
$ gcc -c hello.s
$ ld -o hello hello.o
$ readelf -l hello

Elf file type is EXEC (Executable file)
Entry point 0x400078
There are 1 program headers, starting at offset 64

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x00000000000000af 0x00000000000000af  R E    200000

 Section to Segment mapping:
  Segment Sections...
   00     .text 
$ sstrip hello
$ wc -c hello
175 hello
{% endhighlight %}

也可以通過 `gcc -nostdlib hello.s -o hello` 進行編譯，不過對應的字節會較多，當然，也可以參考 [為可執行文件"減肥"](www.tinylab.org/as-an-executable-file-to-slim-down) 中的介紹在彙編上進行優化。

## 參考

關於最小化可執行 ELF 文件可以參考 [The Teensy Files](http://www.muppetlabs.com/~breadbox/software/tiny/) 中的相關文章，以及相關的 ELF 操作程序 [github BR903/ELFkickers](https://github.com/BR903/ELFkickers) 。

<!--
為可執行文件"減肥"
www.tinylab.org/as-an-executable-file-to-slim-down/
/reference/linux/as-an-executable-file-to-slim.maff

http://www.muppetlabs.com/~breadbox/software/elfkickers.html

X86彙編調用框架淺析與CFI簡介
http://blog.csdn.net/permike/article/details/41550991
-->

{% highlight text %}
{% endhighlight %}
