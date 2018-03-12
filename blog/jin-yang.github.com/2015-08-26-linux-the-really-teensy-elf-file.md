---
title: 最小的ELF文件
layout: post
comments: true
language: chinese
category: [linux,program,misc]
keywords: mysql,database,handler
description: 从最经典 "Hello World" C 程序实例，逐步演示如何通过各种常用工具来分析 ELF 文件，并逐步精简代码，尽量减少可执行文件的大小。接下来，看看一个可打印 "Hello World" 的可执行文件能够小到什么样的地步。
---

从最经典 "Hello World" C 程序实例，逐步演示如何通过各种常用工具来分析 ELF 文件，并逐步精简代码，尽量减少可执行文件的大小。

接下来，看看一个可打印 "Hello World" 的可执行文件能够小到什么样的地步。

<!-- more -->

## 简介

为了最小化可执行文件，需要了解可执行文件的格式，链接生成可执行文件时的后台细节，有哪些内容被链接到了目标代码中，通过选择合适的可执行文件格式并剔除对可执行文件的最终运行没有影响的内容，就可以实现目标代码的裁减。

因此，通过探索减少可执行文件大小的方法，就相当于实践性地去探索了可执行文件的格式以及链接过程的细节。

### 可执行文件格式

需要找到一个目标系统支持该可执行文件格式，在 *NIX 平台下主要包括了以下的三种可执行文件格式，这三种格式基本上代表了可执行文件的一个发展过程：

* a.out，非常紧凑，只包含了程序运行所必须的信息 (文本、数据、BSS)，而且每个 section 的顺序是固定的。
* coff，为了提高扩展性，引入了一个分区表以支持更多分区信息，重定位在链接时就已经完成，因此不支持动态链接，不过扩展 coff 支持。
* elf，不仅支持动态链接，而且有很好的扩展性，可描述可重定位文件、可执行文件和可共享文件 (动态链接库) 三类文件。

如下是 ELF 文件的结构图：

![linux elf format]({{ site.url }}/images/linux/elf-format.png "linux elf format"){: .pull-center }

无论是文件头部、程序头部表、节区头部表还是各个节区，都是通过特定的结构体描述的，这些结构在 elf.h 文件中定义，例如 CentOS 保存在 `/usr/include/elf.h` 文件中。

文件头部用于描述整个文件的类型、大小、运行平台、程序入口、程序头部表和节区头部表等信息，可以通过如下方式查看文件头部信息。

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

----- 编译产生可重定向的目标代码，查看类型
$ gcc -c hello.c
$ readelf -h hello.o | grep Type
  Type:                              REL (Relocatable file)

----- 生成可执行文件，查看类型
$ gcc -o hello hello.o
$ readelf -h hello | grep Type
  Type:                              EXEC (Executable file)

----- 生成共享库，并查看文件类型
$ gcc -fpic -shared -W1,-soname,libhello.so.0 -o libhello.so.0.0 hello.o
$ readelf -h libhello.so.0.0 | grep Type
  Type:                              DYN (Shared object file)
{% endhighlight %}

那么 `Section Header Table, SHT` 和 `Program Header Table, PHT` 有什么用呢？前者只对可重定向文件有用，而后者只对可执行文件和可共享文件有用。

### Section Header Table

SHT 用来描述各分区，包括各分区的名字、大小、类型、虚拟内存中的位置、相对文件头的位置等，连接器可以根据文件头部表和节区表的描述信息对各种输入的可重定位文件进行合适的链接。

<!--
包括节区的合并与重组、符号的重定位（确认符号在虚拟内存中的地址）等，把各个可重定向输入文件链接成一个可执行文件（或者是可共享文件）。如果可执行文件中使用了动态连接库，那么将包含一些用于动态符号链接的节区。
-->

可以通过 `readelf -S` 或者 `objdump -h` 查看分区表信息。

{% highlight text %}
----- 可执行文件、可共享库、可重定位文件默认都生成有分区表
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

其中不同文件类型，其分区会有所区别，不过有部分分区，包括 `.text`、`.data`、`.bss` 是必须的，如果使用了动态链接库，那么需要 `.interp` 分区告知系统使用什么动态连接器程序来进行动态符号链接，进行某些符号地址的重定位。

<!--
通常，.rel.text节区只有可重定向文件有，用于链接时对代码区进行重定向，而.hash,.plt,.got等节区则只有可执行文件（或可共享库）有，这些节区对程序的运行特别重要。还有一些节区，可能仅仅是用于注释，比如.comment，这些对程序的运行似乎没有影响，是可有可无的，不过有些节区虽然对程序的运行没有用处，但是却可以用来辅助对程序进行调试或者对程序运行效率有影响。
-->

### Program Header Table

虽然上述的三类文件都包含一些分区，不过分区表只有可重定位文件才是必须的，而程序的执行并不需要分区表，只需要程序头部表以便知道如何加载和执行文件。另外，如果需要对可执行文件或者动态连接库进行调试，那么分区表是必要的，否则调试器将无法工作。

下面来介绍程序头部表，可通过 `readelf -l` 或者 `objdump -p` 查看。

{% highlight text %}
----- 对于可重定向文件，gcc没有产生程序头部，因为它对可重定向文件没用
$ readelf -l hello.o
There are no program headers in this file.

----- 可执行文件和可共享文件都有程序头部
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

从上面可看出程序头部表描述了一些段 (Segment)，这些段对应着一个或者多个 Sections，可以通过 `readelf -l` 显示了各个段与分区的映射。

<!--
这些段描述了段的名字、类型、大小、第一个字节在文件中的位置、将占用的虚拟内存大小、在虚拟内存中的位置等。这样系统程序解释器将知道如何把可执行文件加载到内存中以及进行动态链接等动作。

该可执行文件包含7个段，PHDR指程序头部，INTERP正好对应.interp节区，两个LOAD段包含程序的代码和数据部分，分别包含有.text和.data，.bss节区，DYNAMIC段包含.daynamic，这个节区可能包含动态连接库的搜索路径、可重定位表的地址等信息，它们用于动态连接器。NOTE和GNU_STACK段貌似作用不大，只是保存了一些辅助信息。因此，对于一个不使用动态连接库的程序来说，可能只包含LOAD段，如果一个程序没有数据，那么只有一个LOAD段就可以了。
-->

简单来说，Linux 支持多种可执行文件格式，但是目前 ELF 较通用，所以这里选择 ELF 作为讨论对象。<!--通过上面对ELF文件分析发现一个可执行的文件可能包含一些对它的运行没用的信息，比如节区表、一些用于调试、注释的节区。如果能够删除这些信息就可以减少可执行文件的大小，而且不会影响可执行文件的正常运行。-->

## 最小化

仅仅从是否影响一个 C 语言程序运行的角度上说，GCC 默认链接到可执行文件的几个可重定位文件 (`crt1.o` `rti.o` `crtbegin.o` `crtend.o` `crtn.o` ) 并不是必须的。

不过需要注意的是，如果没有链接那些文件但在程序末尾使用了 return 语句，`main()` 将无法返回，需要替换为 `_exit()`<!--；另外，既然程序在进入main之前有一个入口，那么main入口就不是必须的。因此，如果不采用默认链接也可以减少可执行文件的大小-->。

下面以 Hello World 为例，尝试最小化示例程序。

### 1. 默认大小

{% highlight text %}
$ cat << EOF > hello.c
#include <stdio.h>
int main(void)
{
    printf("Hello World!\n");
    return 0;
}
EOF

----- 生成二进制文件，并查看其大小
$ gcc -o hello hello.c
$ wc -c hello
8520 hello
{% endhighlight %}

### 2. 自定义编译

可以考虑把 `return 0` 替换成 `_exit(0)` 并包含定义该函数的 `unistd.h` 头文件，然后接着使用如下的 `Makefile` 进行编译。

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

----- 生成汇编语言
$ gcc -S hello.c

----- 将main入口替换为_start
$ sed -i -e "s/main/_start/g" hello.s

----- 编译链接生成可执行文件
$ gcc -c hello.s
$ ld -o hello hello.o -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2
$ wc -c hello
6192 hello
{% endhighlight %}

如果报 `Accessing a corrupted shared library` 错误，那么可能是由于 ld 与编译的程序位数不同，对于 32-bits 的程序，可以通过如下方式编译链接。

{% highlight text %}
$ gcc -m32 -S hello.c
$ gcc -m32 -c hello.s
$ ld -o hello hello.o -m elf_i386 -lc -dynamic-linker /lib/ld-linux.so.2
{% endhighlight %}

### 3. 删除无用分区

部分分区对于程序的运行并非必要的，可以通过 `strip -R` 或者 `objcop -R` 删除。

{% highlight text %}
----- 查看分区信息
$ readelf -l hello | grep "Segment Sections..." -A 10
  Segment Sections...
   00     
   01     .interp 
   02     .interp .hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.plt .plt .text .rodata .eh_frame 
   03     .dynamic .got.plt 
   04     .dynamic 
   05     
   06     .dynamic 

----- 删除两个无用的分区
$ strip -R .hash hello
$ strip -R .gnu.version hello
$ wc -c hello
5192 hello
{% endhighlight %}

另外，可以从 [Kickers of ELF](http://www.muppetlabs.com/~breadbox/software/elfkickers.html) 下载编译 sstrip 程序，然后执行 `sstrip hello` 删除分区表，在此就不再测试了。

### 4. 使用汇编

接下来看看如何通过汇编进行优化，如下是直接通过 `gcc -S hello.c` 生成的汇编语言，然后以此为基础进行修改。

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

修改为如下。

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

接着看看直接通过 `_start()` 作为入口。

{% highlight text %}
----- 替换掉main
$ sed -i -e "s/main/_start/g" hello.s
----- 编译链接
$ as --64 -o  hello.o hello.s
$ ld -o hello hello.o -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2

----- 删除两个无用的分区以及分区表
$ strip -R .hash hello
$ strip -R .gnu.version hello
$ sstrip hello
$ wc -c hello
4136 hello
{% endhighlight %}

### 5. 使用系统调用

也可以直接使用系统调用，此时链接时就不需要依赖 lib 库了。

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

----- 编译链接
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

也可以通过 `gcc -nostdlib hello.s -o hello` 进行编译，不过对应的字节会较多，当然，也可以参考 [为可执行文件"减肥"](www.tinylab.org/as-an-executable-file-to-slim-down) 中的介绍在汇编上进行优化。

## 参考

关于最小化可执行 ELF 文件可以参考 [The Teensy Files](http://www.muppetlabs.com/~breadbox/software/tiny/) 中的相关文章，以及相关的 ELF 操作程序 [github BR903/ELFkickers](https://github.com/BR903/ELFkickers) 。

<!--
为可执行文件"减肥"
www.tinylab.org/as-an-executable-file-to-slim-down/
/reference/linux/as-an-executable-file-to-slim.maff

http://www.muppetlabs.com/~breadbox/software/elfkickers.html

X86汇编调用框架浅析与CFI简介
http://blog.csdn.net/permike/article/details/41550991
-->

{% highlight text %}
{% endhighlight %}
