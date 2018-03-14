---
title: Linux 系統調用
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,內核,kernel,系統調用
description: Intel 的 x86 架構的 CPU 提供了 0 到 3 四個特權級，而在 Linux 操作系統中主要採用了 0 和 3 兩個特權級，也就是我們通常所說的內核態和用戶態。從用戶態向內核態切換通常有 3 種情況： A) 系統調用(主動)，用戶態的進程申請操作系統的服務，通常用軟中斷實現；B) 產生異常，如缺頁異常、除0異常；C) 外設產生中斷，如鍵盤、磁盤等。下面以系統調用來講解。
---

Intel 的 x86 架構的 CPU 提供了 0 到 3 四個特權級，而在 Linux 操作系統中主要採用了 0 和 3 兩個特權級，也就是我們通常所說的內核態和用戶態。

從用戶態向內核態切換通常有 3 種情況： A) 系統調用(主動)，用戶態的進程申請操作系統的服務，通常用軟中斷實現；B) 產生異常，如缺頁異常、除0異常；C) 外設產生中斷，如鍵盤、磁盤等。

下面以系統調用來講解。

<!-- more -->

## 簡介

在 x86 中，通過中斷來調用系統調用的效率被證明是非常低的，如果用戶程序頻繁使用系統調用接口，那麼會顯著降低執行效率。Intel 很早就注意到了這個問題，並且引進了一個更有效的 sysenter 和 sysexit 形式的系統調用接口。

快速系統調用最初在 Pentium Pro 處理器中出現，但是由於硬件上的 bug ，實際上並沒有被大量 CPU 採用。這就是為什麼可以看到 PentiumⅡ 甚至 Pentium Ⅲ 最後實際引入了 sysenter 。

由於硬件的問題，操作系統經歷了很長時間才支持快速系統調用，Linux 最早在 2002.11 才開始支持，此時已經過去了 10 年。通過反彙編可以發現，```__kernel_vsyscall``` 實際會調用 ```sysenter``` 。

## 使用系統調用

系統調用號可以從 ```/usr/include/syscall.h``` 中查看，在 C 中調用系統調用可以使用 ```syscall()``` 或者 glibc 封裝的系統調用。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <syscall.h>       /* for SYS_write etc. */
#include <sys/types.h>

int main(void) {
    char s[] = "Hello World\n";
    int ret;

    /* direct system call */
    ret = syscall(SYS_write, 2, s, sizeof(s)); /* man 2 syscall */
    printf("syscall(SYS_write) return %d\n", ret);

    /* or "libc" wrapped system call */
    ret = write(2, s, sizeof(s));
    printf("libc write() return %d\n", ret);

    return(0);
}
{% endhighlight %}

或者使用匯編。

{% highlight asm %}
# Writes "Hello, World" to the console using only system calls. Runs on 64-bit Linux only.
# gcc -c hello.s && ld hello.o && ./a.out or gcc -nostdlib hello.s && ./a.out

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
    .ascii  "Hello, world\n"
{% endhighlight %}

對於相應的系統調用可以通過 ```strace -e trace=write ./a.out``` 查看，注意，如果通過如下的 vdso 則不會捕捉到相應的系統調用。

## 系統實現

在比較老的系統中，是通過軟中斷實現。在 32-bits 系統中，系統調用號通過 ```eax``` 傳入，各個參數依次通過 ```ebx```, ```ecx```, ```edx```, ```esi```, ```edi```, ```ebp``` 傳入，然後調用 ```int 0x80``` ；返回值通過 ```eax``` 傳遞。所有寄存器的值都會保存。

在 64-bits 系統中，系統調用號通過 ```rax``` 傳入，各個參數依次通過 ```rdi```, ```rsi```, ```rdx```, ```r10```, ```r8```, ```r9``` 傳入, 然後調用 ```syscall```；返回值通過 ```rax``` 傳遞。系統調用時 ```rcx``` 和 ```r11``` 不會保存。

對於系統調用的執行過程，可通過解析反彙編代碼查看，真正的程序入口是 ```_start```，下面解析查看過程。

### 示例程序

以 ```write()``` 系統調用為例，通過 ```gcc -o write write.c -static``` 編譯，注意最好靜態編譯，否則不方便查看。

{% highlight c %}
#include <unistd.h>
int main(int argc, char **argv)
{
    write(2, "Hello World!\n", 13);
    return 0;
}
{% endhighlight %}

通過如下方式查看函數調用過程，write() 是 glibc 封裝的函數，具體實現可以查看源碼，callq 等同於 call 。

{% highlight text %}
(gdb) disassemble main                                      # 反彙編main函數
Dump of assembler code for function main:
   0x0000000000400dc0 <+0>:     push   %rbp
   0x0000000000400dc1 <+1>:     mov    %rsp,%rbp            # 保存棧楨
   0x0000000000400dc4 <+4>:     mov    $0xd,%edx            # 字符串長度
   0x0000000000400dc9 <+9>:     mov    $0x48f230,%esi       # 字符串地址，打印數據 x/s 0x48f230
   0x0000000000400dce <+14>:    mov    $0x2,%edi            # 傳入的第一個參數
   0x0000000000400dd3 <+19>:    callq  0x40ed30 <write>
   0x0000000000400dd8 <+24>:    mov    $0x0,%eax
   0x0000000000400ddd <+29>:    pop    %rbp
   0x0000000000400dde <+30>:    retq
End of assembler dump.

(gdb) disassemble 0x40ed30                                   # 反彙編write函數
Dump of assembler code for function write:
   0x000000000040ed30 <+0>:     cmpl   $0x0,0x2ae155(%rip)
   0x000000000040ed37 <+7>:     jne    0x40ed4d <write+29>
   0x000000000040ed39 <+0>:     mov    $0x1,%eax             # wirte的系統調用號
   0x000000000040ed3e <+5>:     syscall                      # 執行系統調用
   ... ...
{% endhighlight %}

系統會採用 syscall 和 sysenter，x32 大多采用 sysenter，而 x64 採用的是 syscall 。

### 內核實現

Linux 內核中維護了一張系統調用表 ```sys_call_table[ ]@arch/x86/kernel/syscall_64.c``` ，這是一個一維數組，索引為系統調用號，表中的元素是系統調用函數。

{% highlight text %}
const sys_call_ptr_t sys_call_table[__NR_syscall_max+1] = {
    [0 ... __NR_syscall_max] = &sys_ni_syscall,
#include &lt;asm/syscalls_64.h&gt;
};
{% endhighlight %}

如上所示，默認所有調用都初始化為 ```sys_ni_syscall()``` ，而 ```asm/syscalls_64.h``` 實際是在編譯時動態產生的。

編譯內核時，會執行 ```arch/x86/syscalls/Makefile``` ，該文件會調用 shell 腳本 ```syscalltdr.sh``` ，該腳本以 ```syscall_64.tbl``` 文件作為輸入，然後生成 ```arch/x86/include/generated/asm/syscalls_64.h``` ，最後生成的內容如下。

{% highlight c %}
//### arch/x86/include/generated/asm/syscalls_64.h
__SYSCALL_COMMON(0, sys_read, sys_read)
__SYSCALL_64(19, sys_readv, sys_readv)
__SYSCALL_X32(515, compat_sys_readv, compat_sys_readv)

//### arch/x86/kernel/syscall_64.c
extern asmlinkage void sys_read(void) ;
extern asmlinkage void sys_readv(void) ;
const sys_call_ptr_t sys_call_table[__NR_syscall_max+1] = {
    [0 ... __NR_syscall_max] = &sys_ni_syscall,
    [0] = sys_read, ...
    [19] = sys_readv, ...
};
{% endhighlight %}

最早的時候，在 x86 中，需要通過門進入內核態，系統調用通過 ```int $0x80``` 指令產生一個編號為 128 的軟中斷，對應於是中斷描述符表 IDT 中的第 128 項，在此預設了一個內核空間的地址，它指向了系統調用處理程序 ```system_call()``` (該函數在 ```arch/x86/kernel/entry_64.S``` 中定義)。

由於傳統的 ```int 0x80``` 系統調用浪費了很多時間，2.6 以後會採用 sysenter/sysexit/syscall 。

### 添加系統調用

添加系統調用方法如下。

#### 1. 定義系統調用號

在系統調用向量表裡添加自定義的系統調用號。

向 ```arch/x86/syscalls/syscall_64.tbl``` 中加入自定義的系統調用號和函數名，添加時可以參考文件的註釋，在 x86-64 中可以定義 common/64/x32 三種類型。

{% highlight text %}
555 common foobar    sys_foobar
{% endhighlight %}

#### 2. 添加函數聲明

在 ```arch/x86/include/asm/syscalls.h``` 中添加函數聲明。

{% highlight text %}
asmlinkage void sys_foobar(void) ;
{% endhighlight %}

#### 3. 添加函數的定義

在文件 ```kernel/sys.c``` 文件中加入對 ```sys_foobar()``` 的定義。

{% highlight c %}
SYSCALL_DEFINE0(foobar)
{
    printk(KERN_WARN "hello world foobar!");
    return 0;
}
EXPORT_SYMBOL(sys_foobar);
{% endhighlight %}

#### 4. 編譯安裝

通過 ```make bzImage && make install``` 編譯安裝。




## 優化

```vsyscall``` 和 ```vDSO``` 是兩種用來加速系統調用的機制。兩者在執行系統調用時，都不需要改變優先級進入內核模式，不過相比來說後者更安全。如果獲得了 ```system()``` 的入口，那麼可以執行幾乎任意的程序，```vDSO``` 採用隨機地址 (```cat /proc/self/maps```)，而且去除了一些可能有風險的代碼。

靜態連接都會調用"調用系統"，如果通過 strace 跟蹤，則都會觀察到系統調用。

### 示例

簡單介紹 ```gettimeofday()``` 的使用方法，一般不需要獲取時區，所以第二個參數通常設置為 ```NULL``` 。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
    struct  timeval    tv;
    struct  timezone   tz;
    gettimeofday(&tv, &tz);

    printf("tv_sec:%d\n", (int)tv.tv_sec);
    printf("tv_usec:%d\n", (int)tv.tv_usec);
    printf("tz_minuteswest:%d\n", tz.tz_minuteswest);
    printf("tz_dsttime:%d\n", tz.tz_dsttime);

    return 0;
}
{% endhighlight %}

在 x32 上，```gettimeofday()``` 會調用 ```__kernel_vsyscall()```，然後到 ```sysenter``` 指令。而在 x64 上，同時使用 ```vsyscall``` 和 ```vdso```（這個可以通過```/proc/PID/maps```查看），應該優先使用 ```vdso``` 。

{% highlight text %}
$ gcc gettimeofday.c -o gettimeofday -Wall -g
$ gdb gettimeofday -q
Reading symbols from /tmp/gettimeofday...done.
(gdb) b gettimeofday
Breakpoint 1 at 0x400460
(gdb) run
(gdb) disassemble
... ...
0x00007ffff7acb082 <+50>:    callq  0x7ffff7b4e8c0 <_dl_vdso_vsym>
... ...

(gdb) b _dl_vdso_vsym
(gdb) b __gettimeofday
{% endhighlight %}

## VDSO

Virtual Dynamic Shared Object, VDSO 是內核提供的功能，也就是為什麼需要 ```asm/vsyscall.h``` 頭文件，它提供了一種快速廉價的系統調用方式。

有些系統調用如 ```gettimeofday()``` ，會經常調用，傳統的系統調用方式是通過軟中斷指令 ```int 0x80``` 實現的，最新的採用 ```syscall()``` 。不論何種操作，都需要進行壓棧、跳轉、權限級別提升，恢復用戶棧，並跳轉回低級別代碼。

vdso 是將內核態的調用映射到用戶態的地址空間中，會將當前時間放置到其它應用都可以訪問的固定地方，這樣應用不需要系統調用即可以。linux 中通過 ```vsyscall``` 實現，現在只支持三種系統調用，詳見 ```asm/vsyscall.h``` 。

<!--
"快速系統調用指令"比起中斷指令來說，其消耗時間必然會少一些，但是隨著 CPU 設計的發展，將來應該不會再出現類似 Intel Pentium4 這樣懸殊的差距。而"快速系統調用指令"比起中斷方式的系統調用方式，還存在一定侷限，例如無法在一個系統調用處理過程中再通過"快速系統調用指令"調用別的系統調用。因此，並不一定每個系統調用都需要通過"快速系統調用指令"來實現。比如，對於複雜的系統調用例如 fork，兩種系統調用方式的時間差和系統調用本身運行消耗的時間來比，可以忽略不計，此處採取"快速系統調用指令"方式沒有什麼必要。而真正應該使用"快速系統調用指令"方式的，是那些本身運行時間很短，對時間精確性要求高的系統調用，例如 getuid、gettimeofday 等等。因此，採取靈活的手段，針對不同的系統調用採取不同的方式，才能得到最優化的性能和實現最完美的功能。
-->

vdso.so 就是內核提供的虛擬的 .so ，這個 .so 文件不在磁盤上，而是在內核裡頭。內核把包含某 .so 的內存頁在程序啟動的時候映射入其內存空間，對應的程序就可以當普通的 .so 來使用裡頭的函數，比如 ```syscall()``` 。

### ASLR

Address-Space Layout Randomization, ASLR 將用戶的一些地址隨機化，如 stack, mmap region, heap, text ，可以通過 ```randomize_va_space``` 配置，對應的值有三種：

* 0 - 表示關閉進程地址空間隨機化。
* 1 - 表示將mmap的基址，stack和vdso頁面隨機化。
* 2 - 表示在1的基礎上增加棧（heap）的隨機化。

可通過 ```echo '0' > /proc/sys/kernel/randomize_va_space``` 或 ```sysctl -w kernel.randomize_va_space=0``` 設置。然後，可以通過如下方式獲取 ```vdso.so``` 文件。

{% highlight text %}
$ cat /proc/sys/kernel/randomize_va_space
# sysctl -w kernel.randomize_va_space=0                           # 關閉隨機映射
$ cat /proc/self/maps                                             # 查看vdso在內存中的映射位置
... ...
7ffff7ffa000-7ffff7ffc000 r-xp 00000000 00:00 0  [vdso]           # 佔用了2pages
... ...
# dd if=/proc/self/mem of=linux-gate.so bs=4096 skip=$[7ffff7ffa] count=2  # 從內存中複製

$ readelf -h linux-gate.so                                        # 查看頭信息
$ file linux-gate.so                                              # 查看文件類型，為shared-library ELF
$ objdump -T linux-gate.so                                        # 打印符號表，或者用-d反彙編
$ objdump -d linux-gate.so | grep -A5 \<__vdso.*:                 # 查看包含vdso的函數
{% endhighlight %}

可以從內存中讀取該文件，或者使用 ```extract_vdso.c``` 直接讀取該文件。

<!--
#!/bin/bash
PID=14080 # Such as top's PID.
VDSO_FILE_NAME=linux-gate.dso

VDSO_ADDR=`cat /proc/${PID}/maps | grep "vdso" | awk -F '-' '{print $1}'`
echo "Current VDSO address is 0x${VDSO_ADDR}"

VDSO_BLOCK=`echo | awk '{print substr("'${VDSO_ADDR}'",1,5)}'`
((SKIP_BLOCKS=16#${VDSO_BLOCK})) # Convert hex to dec.
echo "We have ${SKIP_BLOCKS} blocks before VDSO library ${VDSO_BLOCK}"

echo "Ready to generate ${VDSO_FILE_NAME} from block ${VDSO_BLOCK}"
dd if=/proc/${PID}/mem of=${VDSO_FILE_NAME} bs=4096 skip=${SKIP_BLOCKS} count=1
echo "Generate ${VDSO_FILE_NAME} done."

# dd if=/proc/self/mem of=linux-gate.dso bs=4096 skip=1048574 count=1

$ file -b linux-gate.dso
ELF 32-bit LSB shared object, Intel 80386, version 1 (SYSV), stripped

# objdump -T linux-gate.dso

linux-gate.dso:     file format elf32-i386

DYNAMIC SYMBOL TABLE:
ffffe400 g    DF .text  00000014  LINUX_2.5   __kernel_vsyscall
00000000 g    DO *ABS*  00000000  LINUX_2.5   LINUX_2.5
ffffe440 g    DF .text  00000007  LINUX_2.5   __kernel_rt_sigreturn
ffffe420 g    DF .text  00000008  LINUX_2.5   __kernel_sigreturn


$ objdump -d --start-address=0xffffe400 --stop-address=0xffffe414 linux-gate.dso

linux-gate.dso:     file format elf32-i386

Disassembly of section .text:

ffffe400 <__kernel_vsyscall>:
ffffe400:       51                      push   %ecx
ffffe401:       52                      push   %edx
ffffe402:       55                      push   %ebp
ffffe403:       89 e5                   mov    %esp,%ebp
ffffe405:       0f 34                   sysenter
ffffe407:       90                      nop
ffffe408:       90                      nop
ffffe409:       90                      nop
ffffe40a:       90                      nop
ffffe40b:       90                      nop
ffffe40c:       90                      nop
ffffe40d:       90                      nop
ffffe40e:       eb f3                   jmp    ffffe403 <__kernel_vsyscall+0x3>
ffffe410:       5d                      pop    %ebp
ffffe411:       5a                      pop    %edx
ffffe412:       59                      pop    %ecx
ffffe413:       c3                      ret

https://github.com/oliver/ptrace-sampler -->

### 內核實現

內核實現在 ```arch/x86/vdso``` 中，編譯生成 ```vdso.so``` 文件，通常來說該文件小於一個 page(4096) ，那麼在內存中會映射為一個 page ，如果大於 4k ，會映射為 2pages 。

細節可以直接查看內核代碼。

### 添加函數

在此通過 vdso 添加一個函數，返回一個值。

#### 1. 定義函數

在 ```arch/x86/vdso``` 目錄下創建 ```vfoobar.c```，其中 notrace 在 ```arch/x86/include/asm/linkage.h``` 中定義，也就是 ```#define notrace __attribute__((no_instrument_function))``` 。

同時需要告訴編譯器一個用戶態的函數 foobar() ，該函數屬性為 weak 。weak 表示該函數在運行時才會解析，而且可以被覆蓋。

{% highlight c %}
#include <asm/linkage.h>
notrace int __vdso_foobar(void)
{
    return 666;
}
int foobar(void) __attribute__((weak, alias("__vdso_foobar")));
{% endhighlight %}

#### 2. 添加到連接描述符

修改 ```arch/x86/vdso/vdso.lds.S``` ，這樣編譯的時或才會添加到 ```vdso.so``` 文件中。

{% highlight text %}
VERSION {
    LINUX_2.6 {
        global:
            ... ...
            getcpu;
            __vdso_getcpu;

            foobar;
            __vdso_foobar;
        local: *;
    };
}
{% endhighlight %}

#### 3. 添加Makefile

修改 ```arch/x86/vdso/Makefile``` 文件，從而可以在編譯內核時同時編譯該文件。

{% highlight text %}
# files to link into the vdso
vobjs-y := vdso-note.o vclock_gettime.o vgetcpu.o vfoobar.o
{% endhighlight %}

#### 4. 編輯用戶程序

通過 ```gcc foobar_u.c vdso.so``` 編譯如下文件，其中 vdso.so 提供了編譯時的符號解析。

{% highlight c %}
#include <stdio.h>
int main(void)
{
    printf("His number is %d\n", foobar());
    return 0;
}
{% endhighlight %}

不過這樣有一個缺陷，就是即使已經修改了內核，那麼該函數還是返回之前設置的值。實際上可以返回一個內核中的值，如 ```gettimeofday()``` ，實際通過 ```update_vsyscall()@arch/x86/kernel/vsyscall_64.c``` 進行更新，相應的設置可以參考相關函數調用。

## 參考

int 0x80 的系統調用方式可以參考 [Linux系統調用的實現機制分析](/reference/linux/kernel/syscall.doc)；關於系統調用表可查看 [LINUX System Call Quick Reference](http://www.digilife.be/quickreferences/qrc/linux%20system%20call%20quick%20reference.pdf) 或者 [本地文檔](/reference/linux/kernel/linux_system_call_reference.pdf) 。

簡單介紹系統調用，包含了簡單 hello world [Linux System Calls](http://cs.lmu.edu/~ray/notes/linuxsyscalls/)，也可以參考 [本地文檔](/reference/linux/kernel/Linux_System_Calls.maff) 。

[What is linux-gate.so.1?](http://www.trilithium.com/johan/2005/08/linux-gate/) by Johan Petersson 主要介紹 linux-gate.so.1 的作用。

[linux-gate.so 技術細節](http://www.newsmth.net/bbsanc.php?path=%2Fgroups%2Fcomp.faq%2FKernelTech%2Finnovate%2Fsolofox%2FM.1222336489.G0) 水木社區，介紹 linux-gate.so ，含有仿真的示例。

<!--
What is linux-gate.so.1?
/reference/linux/kernel/What_is_linux-gate.so.1.maff

linux-gate.so 技術細節
/reference/linux/kernel/linux_gate.maff
-->

[Intel P6 vs P7 system call performance](https://lkml.org/lkml/2002/12/18/218) 中有 Linus Torvalds 對於 ```__kernel_vsyscall``` 的介紹；關於 ```syscall``` 和 ```sysret``` 兩個指令非常詳細的介紹 [使用 syscall/sysret 指令](http://www.mouseos.com/arch/syscall_sysret.html)。

<!--
使用 syscall/sysret 指令
/reference/linux/kernel/syscall_sysret.maff
-->

[Creating a vDSO: the Colonel's Other Chicken](http://www.linuxjournal.com/content/creating-vdso-colonels-other-chicken) ，介紹如何添加一個 vdso 函數。

<!--
http://blog.csdn.net/anzhsoft/article/details/18776111
http://www.xfocus.net/articles/200109/269.html
-->

{% highlight text %}
{% endhighlight %}
