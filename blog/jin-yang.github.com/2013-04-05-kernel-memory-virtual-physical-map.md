---
title: Kernel 内存映射
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,内存,memory,映射
description: 现在的操作系统在内存使用时，一般为逻辑地址，那么逻辑地址和物理地址之间是怎样映射的，或者说，通过逻辑地址如何找到物理内存中的内容。其实，包括动态共享库，mmap 映射等，都使用到了虚拟内存，也就是逻辑地址的映射。这里，简单介绍下逻辑地址到物理地址的映射关系。
---

现在的操作系统在内存使用时，一般为逻辑地址，那么逻辑地址和物理地址之间是怎样映射的，或者说，通过逻辑地址如何找到物理内存中的内容。

其实，包括动态共享库，mmap 映射等，都使用到了虚拟内存，也就是逻辑地址的映射。

这里，简单介绍下逻辑地址到物理地址的映射关系。

<!-- more -->

## 简介

如下，是 x86 系统中与内存相关的寄存器以及相关的转换关系。

![memory system level registers and data structure]({{ site.url }}/images/linux/kernel/memory-system-level-registers-and-data-structure.png "memory system level registers and data structure"){: .pull-center width="90%" }

## 地址转换

在如下的程序中打印出 tmp 变量的地址，变量保存在栈中，假设地址为 ```address:0x7FFF797B9698``` ，该结果是逻辑地址。

注意，现在的 Interl CPU 寄存器采用 64-bit，而其内存的寻址空间为 48-bits，也就是说，多余的位实际上是无效的。

{% highlight c %}
#include <stdio.h>
int main()
{
    unsigned long tmp = 0x12345678;
    printf("tmp variable address:0x%08lX\n", &tmp);
    return 0;
}
{% endhighlight %}

现在的 CPU 都会有内存管理单元，对于 IA-32e 类似架构的 CPU 规定，地址映射过程是 ```逻辑地址 -> 线性地址 -> 物理地址```。当然运行在 Intel 上的 Linux 内核也采用相同的方法。

Intel 通过 ```IA32_EFER``` 寄存器定义了不同的 CPU 模式，不过暂时不太清除如何读取该寄存器。

![memory extended feature enable register]({{ site.url }}/images/linux/kernel/memory-extended-feature-enable-register.png "memory extended feature enable register"){: .pull-center width="80%" }

除此之外，先介绍下这里会使用到的工具。

<!-- 变量 a 定在了数据段， 通过 ds 段寄存器的值可以看出使用的是 GDT ，entry 是 15.-->

### x86-64 示例

以如下的值为例。

{% highlight text %}
$ ./address
tmp address:0x7FFE07DB9A70
CR4=1427E0 PSE(0) PAE(1)
CR3=12A6E0000 CR0=80050033, pgd:0xFFFF88012A6E0000
gdtr address:FFFF88021FB09000, limit:7F
{% endhighlight %}

这里获取的 GDTR 基址是 ```0xFFFF88021FB09000```，注意这里是线性地址，实际上对应的物理地址是减去一个偏移 ```__PAGE_OFFSET``` ，也就是 ```0xFFFF880000000000```，因此对应的物理地址是 ```0x21FB09000``` 。

{% highlight c %}
#define __PAGE_OFFSET           _AC(0xffff880000000000, UL)
{% endhighlight %}

应用程序中，tmp 变量的逻辑地址或者说线性地址为 ```0x7FFE07DB9A70```，那么接下来，我们逐步映射到实际物理地址，看这个地址的数据是否真的为 ```0x12345678beaf5dde``` 。

如下，是 x86 的分段与分页机制。

![memory segmentation paging]({{ site.url }}/images/linux/kernel/memory-segmentation-paging.png "memory segmentation paging"){: .pull-center width="80%" }

### 分段机制

分段的存在更多就是为了兼容性，在 x86-64 下的 64-bit 程序该功能近似于 bypass，对于 CS 会根据 GDT 中的信息判断是 64-bit 还是 32-bit compatibility 模式，而对于数据段的选择器如 DS、ES、FS、GS、SS 可以为 00H。而对于 GS、SS 可以不为 00H，手册说仍然有效，不过没有仔细研究。

上述输出的地址为 ```0x7FFE07DB9A70```，这个就是逻辑地址，首先需要将逻辑地址转化成线性地址，也就是分段机制。

{% highlight c %}
#ifdef CONFIG_X86_32
//... ...
#else
#define GDT_ENTRY_KERNEL_CS 2
#define GDT_ENTRY_KERNEL_DS 3
#define GDT_ENTRY_DEFAULT_USER_DS 5
#define GDT_ENTRY_DEFAULT_USER_CS 6
#endif

#define __KERNEL_CS (GDT_ENTRY_KERNEL_CS*8)
#define __KERNEL_DS (GDT_ENTRY_KERNEL_DS*8)
#define __USER_DS   (GDT_ENTRY_DEFAULT_USER_DS*8+3)
#define __USER_CS   (GDT_ENTRY_DEFAULT_USER_CS*8+3)

static void
start_thread_common(struct pt_regs *regs, unsigned long new_ip,
            unsigned long new_sp,
            unsigned int _cs, unsigned int _ss, unsigned int _ds)
{
    loadsegment(fs, 0);
    loadsegment(es, _ds);
    loadsegment(ds, _ds);
    load_gs_index(0);
    current->thread.usersp  = new_sp;
    regs->ip        = new_ip;
    regs->sp        = new_sp;
    this_cpu_write(old_rsp, new_sp);
    regs->cs        = _cs;
    regs->ss        = _ss;
    regs->flags     = X86_EFLAGS_IF;
}

void start_thread(struct pt_regs *regs, unsigned long new_ip, unsigned long new_sp)
{
    start_thread_common(regs, new_ip, new_sp,
                __USER_CS, __USER_DS, 0);
}
{% endhighlight %}

实际上有 6 个段寄存器，不过 FS、DS、ES、GS 的值都是 0，只有 CS 和 DS 的值有所区别，每个进程的 6 个寄存器是一样的，不同的是 IP 和 SP，从上面的代码中也可以看到。

如下，是 x86 寄存器的段选择符。

![memory segment selector]({{ site.url }}/images/linux/kernel/memory-segment-selector.png "memory segment selector"){: .pull-center width="40%" }

TI 表示选择的段描述符是存在 GDT 中还是 LDT 中，一般都只存在 GDT 中，几乎没有选择 LDT 的，只有像 wine 这种进程才会用到 LDT 。

RPL 表示特权等级，Linux 中只使用了 0 和 3 两个特权级别，其中 0 表示最高权限，3 表示无特权；如上，也就是说 CS 和 SS 段是无特权的。

另外，前面的 13 位表示在 GDT 表中的 index，或者说是第几项，而 GDT 存放的地址就保存在了 GDTR 寄存器中。

#### 查找地址

如上，对应的 GDTR 寄存器的地址是 ```0xffff880223489000 - 0xffff880000000000 = 0x223489000```，不过这里有个 bug ，暂时还没有想好怎么解决。

dram 获取的是可用页数，不过很多系统的可用内存的空间是不连续的，这就导致了部分的内存无法访问。

<!-- 也就是说对于 intel x86-64 我们可以直接忽略分段功能，而直接查看分页的处理。 -->

由于每个 GDT 项长 64-bits，所以对应的地址为 ```0x223489000 + idx*8``` ，然后根据对应的值参照下图的内容，然后计算地址即可。

![memory segment descriptor]({{ site.url }}/images/linux/kernel/memory-segment-descriptor.png "memory segment descriptor"){: .pull-center width="80%" }

不过一般计算之后都是 ```0x000000``` ，也就是说虚拟地址直接等于线性地址。

### 分页机制

分页过程会将 48-bit 的线性地址转换为 52-bit 的物理地址, 可以看出虽然是 64bit 的操作系统但在处理器层面并没有提供 2^64 大小的内存访问范围。

另外需要注意的是，通过 CR3 映射的内存地址实际是 "物理地址"。对于分页机制 IA-32e 有 4K、2M、1G 的分页方式，Linux 中采用的通常为 4K ，在此仅以此为例。

x86-64 采用了四层地址映射，

{% highlight text %}
+-------+-------+-------+-------+-------+---------+
| Index | PTE   | PMD   | PUD   | PGD   | Reseved |
| 00~11 | 12~20 | 21~29 | 30~38 | 39~47 | 48~63   |
+-------+-------+-------+-------+-------+---------+
{% endhighlight %}

如上，PGD、PUD、PMD、PTE 各占了 9 位，加上 12 位的页内 index，共用了 48 位，即可管理的地址空间为 ```2^48=256T```，而在 32 位地址模式时，该值仅为 ```2^32=4G``` 。

另外 64 位地址时支持的物理内存最大为 ```2^46=64T```，见 ```e820.c``` 中 ```MAX_ARCH_PFN``` 的定义：

{% highlight c %}
# define MAX_PHYSMEM_BITS   46
# define MAXMEM       _AC(__AC(1, UL) << MAX_PHYSMEM_BITS, UL)
# define MAX_ARCH_PFN MAXMEM>>PAGE_SHIFT
{% endhighlight %}

而在 32 位地址时，在开启 PAE 选项后最大支持的物理内存为 64G 。

### 实践

这里的示例代码可以参考 [github memory]({{ site.example_repository }}/linux/memory/to_physical/) ，主要包括了如下的文件：

* registers.c 内核模块，用于读取 cr0、gdtr 等信息，可以通过 ```/proc/registers``` 读取；
* address.c 返回临时对象的地址，也就是栈地址，目前是 48bits ；

![memory cr0 register]({{ site.url }}/images/linux/kernel/memory_cr0_register.png "memory cr0 registe"){: .pull-center width="80%" }

#### 分页机制

在满足 ```CR0.PG = 1```、```CR4.PAE = 1```、```IA32_EFER.LME = 1``` 条件时会打开 IA-32E Paging 机制，另外，在 ```CR4.PCIDE = 0``` 不同时，对应的 ```CR3``` 使用也略有区别，可以查看手册的 ```4.5 IA-32E PAGING``` 部分。

线性地址到物理地址的转换关系如下。

![memory linear address translation]({{ site.url }}/images/linux/kernel/memory-linear-address-translation.png "memory linear address translation"){: .pull-center width="80%" }

在分页机制中，有效的虚拟地址是 48-bits，会映射到 52-bits 的物理地址；将临时变量 tmp 的地址 ```0x7FFE07DB9A70``` 映射为二进制格式为。

{% highlight text %}
0111 1111 1111 1110 0000 0111 1101 1011 1001 1010 0111 0000
011111111 111111000 000111110 110111001 101001110000
       FF       1F8        3E       1B9          A70
{% endhighlight %}

##### 第一级映射

CR3 寄存器中的值是 ```0x12A6E0000```，这是第一级映射表 (PML4) 的起始物理地址，这张表中保存着第二级映射表的物理地址。而线性地址中的 ```bits[47:39]``` 对应 ```PML4E``` 的序号。

{% highlight text %}
0x12A6E0000 + 011111111b(0xFF) * 8 = 0x12A6E0000 + 0x7F8 = 0x12A6E07F8

(FILEVIEW)
000012A6E07F0    00000001DB9A6067      00000001F9ACC067
000012A6E0800    0000000000000000      0000000000000000
{% endhighlight %}

注意，因为每个单元是 64-bits 因此需要在序号基础上乘以 8 获得地址。最后，该地址对应的二级映射表的起始地址为：```0x1F9ACC000``` (067 后面的 12-bits 是页面属性)。

<!--Bit17:PCIDE-->

##### 第二级映射

二级映射的任务是找到第三级映射表的起始地址。

{% highlight text %}
0x1F9ACC000 + 111111000b * 8 = 0x1F9ACC000 + 0xFC0 = 0x1F9ACCFC0

(FILEVIEW)
00001F9ACCFC0    000000018B96D067      0000000000000000
00001F9ACCFD0    0000000000000000      0000000000000000
{% endhighlight %}

与上述类似，第三级映射表的起始地址为 ```0x18B96D000``` (067 后面的 12-bits 是页面属性)。

##### 第三级映射

三级映射的任务是找到第四级映射表的起始地址。

{% highlight text %}
0x18B96D000 + 000111110b * 8 = 0x18B96D000 + 0x1F0 = 0x18B96D1F0

(FILEVIEW)
000018B96D1F0    0000000154F81067      0000000000000000
000018B96D200    0000000000000000      0000000000000000
{% endhighlight %}

第四级映射表的起始地址为 ```0x154F81000``` (067 后面 12-bits 是页面属性)。


##### 第四级映射

第四级映射的任务是找到临时变量 tmp 所在的物理页面起始地址。

{% highlight text %}
0x154F81000 + 110111001b * 8 = 0x154F81000 + 0xDC8 = 0x154F81DC8

(FILEVIEW)
0000154F81DC0    800000011FDD7067      800000014DD61067
0000154F81DD0    0000000000000000      800000017C7A6067
{% endhighlight %}

```0x154F81DC8``` 地址单元中的数据，就是物理页面起始地址，也就是我们最后所得到的 tmp 对应的物理页为 ```0x14DD61000``` 。

##### 最终物理地址计算

tmp 变量所在内存页面物理地址为 ```0x14DD61000```，这个地址仅是物理页面地址，tmp 变量所在的物理地址为。

{% highlight text %}
0x14DD61000 + 101001110000b = 0x14DD61000 + 0xA70 = 0x14DD61A70

(FILEVIEW)
000014DD61A70    12345678BEAF5DDE      0000000000400830
000014DD61A80    0000000000000000      00007F1FF67EFAF5
{% endhighlight %}

经过 4 级页面映射，终于找到了 tmp 对应的实际物理地址为 ```0x14DD61A70``` ，对应的数据也为 ```0x12345678BEAF5DDE``` 。

## 访问物理内存

用户态的程序都是在内存保护模式下使用内存，无法直接访问物理内存，且用户程序使用的是逻辑地址，而非物理地址，dram 模块可以使用户态程序访问所有物理内存。

内核模块通过文件读写的方式，实现物理地址访问，将物理地址，作为参数 pos 传递。

{% highlight text %}
ssize_t my_read( struct file *file, char *buf, size_t count, loff_t *pos )
{% endhighlight %}

在内核代码中，是无法直接访问物理地址的，代码能访问的都是逻辑地址。此时我们需要先将物理地址转换成逻辑地址，才能在代码中对地址读写。

物理地址转换成逻辑地址方法：

### 1. 获取PFN以及偏移

根据物理地址，计算出对应的页面号和页内偏移。

{% highlight text %}
page_number = *pos / PAGE_SIZE;
page_indent = *pos % PAGE_SIZE;
{% endhighlight %}

### 2. 获取页面指针

将页面号找到对应的页面指针，注意在 2.6.32 及以上内核中，没有导出 mem_map 符号，只能通过 ```pfn_to_page()``` 来找到对应的页面指针。

### 3. 映射为逻辑地址

通过 kmap 映射成逻辑地址，TODO: 暂时还没有搞明白 kmap 是如何进行映射的。

{% highlight text %}
from = kmap( pp ) + page_indent;
{% endhighlight %}

映射成逻辑地址后，我们直接通过 from 指针来访问物理地址 pos 了。

### 4. 使用示例

模块加载和使用方式。

{% highlight text %}
----- 编译模块
# make
----- 加载模块，会自动创建/dev/dram字符设备
# insmod dram.ko

----- 通过fileview查看数据
# ./fileview /dev/dram
{% endhighlight %}

我们使用简单的程序 fileview 来看物理内存中的实际数据，物理地址可以手工输入，程序的前半部分会显示所需的数据，后半部分会接收键盘输入的命令。

## 物理地址转换

现在几乎所有的操作系统都支持虚拟地址 (Virtual Address) ，每个用户空间的进程都有自己的虚拟地址空间，内核配合 CPU 硬件 MMU 完成到物理地址的转换。

Linux 内核保存了地址转换相关的数据结构，正常来说无法在用户空间访问，在 2.6.25 之后，提供了 ```/proc/$(pid}/pagemap``` ```/proc/kpagecount``` ```/proc/kpageflags``` 完成虚拟地址到物理地址的转换。

### pagemap

```/proc/${pid}/pagemap``` 文件包含了该进程的虚拟地址到物理地址转换相关的映射关系，每个转换包含了 64-bits，详细的内容可以参考内核文档 [pagemap.txt]({{ site.kernel_docs_url }}/Documentation/vm/pagemap.txt) 。

![memory proc pagemap]({{ site.url }}/images/linux/kernel/memory-proc-pagemap.png "memory proc pagemap"){: .pull-center width="80%" }

{% highlight text %}
$ cat /proc/$(pidof mysqld)/maps | head -3
00400000-0283f000 r-xp 00000000 08:09 1308936      /opt/mysql-5.7/bin/mysqld
02a3e000-02b18000 r--p 0243e000 08:09 1308936      /opt/mysql-5.7/bin/mysqld
02b18000-02bc7000 rw-p 02518000 08:09 1308936      /opt/mysql-5.7/bin/mysqld

$ ./pagemap `pidof mysqld` 0x00400000
{% endhighlight %}

可以查看源码 [pagemap.c]( {{ site.example_repository }}/linux/memory/pagemap/pagemap.c ) 以及 [pagemap.py]( {{ site.example_repository }}/linux/memory/pagemap/pagemap.py ) 。

### 示例

另外的一个示例是直接从 [foobar.c]( {{ site.example_repository }}/linux/memory/pagemap/foobar.c ) 读取物理地址，然后通过之前介绍的 dram 和 fileview 读取物理地址对应的数据。

{% highlight text %}
----- 编译后直接执行，获取对应变量的物理地址
$ ./foobar
vaddr = 0x7ffdafae39ec, phy = 0x350ea9ec
Enter any key to exit
{% endhighlight %}

如下，可以直接通过 dram 和 fileview 读取到对应物理地址的数据；注意，由于是小端存储，所以正常的 ```0x12345678``` 显示的内容如下。

![memory pagemap]({{ site.url }}/images/linux/kernel/memory-pagemap.png "memory pagemap"){: .pull-center width="70%" }

<!--
https://github.com/dwks/pagemap
http://blog.jeffli.me/blog/2014/11/08/pagemap-interface-of-linux-explained/
http://www.cnblogs.com/lanrenxinxin/p/6216925.html
-->

## 参考

[CS 635: Advanced Systems Programming](http://cs.usfca.edu/~cruse/cs635/) 网上一个牛掰的课程网站，包括了一些不错的源码，包括上面的 dram 和 fileview 。

[关于Linux内存地址映射的介绍](http://ilinuxkernel.com/?p=1276)，可以参考 [本地文档](/reference/linux/kernel/linux_memap.tar.bz2) ，含有x86_32/64详解，该博客对于Linux内存有比较详细的介绍；同时可以参考 [Linux 从虚拟地址到物理地址](http://blog.chinaunix.net/uid-24774106-id-3427836.html) 。

[segmentation 情景分析](http://www.mouseos.com/arch/segmentation.html) 关于段选择寄存器比较详细的介绍。
<!-- [本地文档](/reference/linux/kernel/segmentation.maff) -->

<!--

Intel开发文档手册
https://software.intel.com/en-us/articles/intel-sdm
Linux 从虚拟地址到物理地址
http://blog.chinaunix.net/uid-24774106-id-3427836.html
在 linux x86-32 模式下分析内存映射流程
http://www.cnblogs.com/ziyang3721/p/4269132.html

Linux-3.14.12内存管理笔记【开启分页管理】
http://blog.chinaunix.net/xmlrpc.php?r=blog/article&uid=26859697&id=4456082
http://blog.chinaunix.net/xmlrpc.php?r=blog/article&uid=26859697&id=4470758

/images/linux/kernel/memory-management-overview.jpg

Linux内存管理--基本概念
http://blog.csdn.net/myarrow/article/details/8624687
-->


{% highlight text %}
{% endhighlight %}
