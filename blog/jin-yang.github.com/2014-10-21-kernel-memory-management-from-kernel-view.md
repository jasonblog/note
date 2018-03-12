---
title: 内存-内核空间
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,内存,kernel,内存空间
description:
---

线性地址到物理地址的映射是通过 page table 完成的，内核会在启动分页机制前完成初始化；而且内核会将 A) 不可用物理地址空间，B) 内核代码以及内核初始数据结构对应的地址空间保留。

接下来，看看内核中是如何管理内存的。

<!-- more -->

## 简介

系统启动时，BIOS 会自动发现当前物理内存的地址，而 Linux 内核会在启动时 (还在实地址模式) 通过 e820 BIOS 系统调用获取当前系统的物理内存地址，当然还有 IO 的映射地址等；开始内核会初始化部分内存供内核使用，然后调用 Bootmem 系统。

Linux 将内存分为不同的 Zone 管理，每个 Zone 中通过一个 buddy 系统分配内存，每次分配内存都是以页的 2 指数倍分配，例如内核页大小为 4K(`getconf PAGE_SIZE`)，那么分配的页为 4K, 8K, 16K, ..., 128K，一般系统最大为 128K 。

Buddy Allocator 最大的问题就是碎片，所以一般系统不会直接使用，通常其上层还包括了 Page Cache、Slab Allocator 。

![memory managment overview]({{ site.url }}/images/linux/kernel/memory-managment-overview.jpg "memory managment overview"){: .pull-center width="70%" }

### 地址空间

虚拟地址空间分成了两部分：A) 用户进程，使用的是地址空间的低地址部分 `0~TASK_SIZE`；B) 内核空间，地址空间的高地址部分。

<!--
PAGE_OFFSET：内核虚拟地址空间中低端内存的起始地址
PHYSICAL_START：内核物理地址的起始地址
MEMORY_START：内核低端内存的物理起始地址
-->

## 物理内存探测

Linux 被 bootloader 加载到内存后，首先执行的是 `_start()@arch/x86/boot/header.S/header.S`，该函数在做了一些准备工作后会跳转到 boot 目录下的 `main()@main.c` 函数执行，第一次内存相关的调用是在实模式时，通过调用 `detect_memory()` 实现。

如下是该函数的实现。

{% highlight c %}
int detect_memory(void)
{
    int err = -1;

    if (detect_memory_e820() > 0)
        err = 0;

    if (!detect_memory_e801())
        err = 0;

    if (!detect_memory_88())
        err = 0;

    return err;
}
{% endhighlight %}

该函数会依次尝试调用 `detect_memory_e820()`、`detect_memory_e801()`、`detect_memory_88()` 获得系统物理内存布局，这三个函数都在 memory.c 中实现。

其内部都会以内联汇编的形式调用 bios 中断以取得内存信息，该中断调用形式为 `int 0x15`，同时调用前分别把 `AX` 寄存器设置为 `E820h`、`E801h`、`88h`，该功能分别用于获取系统内存布局、获取内存大小、获取扩展内存大小，关于 `0x15` 号中断详细信息可以去查询相关手册。

在 x86 中，IO 设备也会映射到内存空间，也就是说系统使用的物理内存空间是不连续的，被分成了很多段，而且每段的属性也不一样。通过 `int 0x15` 查询物理内存时，每次返回一个内存段的信息，因此要想返回系统中所有的物理内存，必须以迭代的方式去查询。

### 内存查询

目前使用较多的是 `e820` ，可以通过 `dmesg` 查看内核启动输出，一般有类似 `e820: BIOS-provided physical RAM map` 的输出，下面以 `e820` 为例。

> e820 是和 BIOS 的 int 0x15 中断相关的，之所以叫 e820 是因为在用这个中断时 AX 必须是 0xe820。

其中，与次相关的结构体如下。

{% highlight c %}
struct e820entry {
    __u64 addr;             /* start of memory segment */
    __u64 size;             /* size of memory segment */
    __u32 type;             /* type of memory segment */
} __attribute__((packed));

struct e820map {
    __u32 nr_map;
    struct e820entry map[E820_X_MAX];
};
{% endhighlight %}

在 `detect_memory_e820()` 函数中，把 `int 0x15` 放到一个 do-while 循环里，将每次得到的内存段放到 `struct e820entry` 里。像其它启动时获得的结果一样，最终都会被放到 `boot_params` 里，探测到的各个内存段情况被放到了 `boot_params.e820_map` 。

{% highlight text %}
main()@arch/x86/boot/main.c
 |-detect_memory()                 ← 探测物理内存
   |-detect_memory_e820()
   |-detect_memory_e801()
   |-detect_memory_88()

start_kernel()
 |-setup_arch()                    ← 完成与体系结构相关的初始化工作
   |-setup_memory_map()
     |-e820_print_map()
{% endhighlight %}

Linux 物理内存管理区会在 `start_kernel()` 函数中进行初始化，此时启动分配器已经建立，所以可以从bootmem中分配需要的内存。

在 `e820_print_map()` 函数中，会打印如下内容。

{% highlight text %}
[    0.000000] e820: BIOS-provided physical RAM map:
[    0.000000] BIOS-e820: [mem 0x0000000000000000-0x0000000000057fff] usable
[    0.000000] BIOS-e820: [mem 0x0000000000058000-0x0000000000058fff] reserved
[    0.000000] BIOS-e820: [mem 0x0000000000059000-0x000000000009cfff] usable
[    0.000000] BIOS-e820: [mem 0x000000000009d000-0x00000000000fffff] reserved
[    0.000000] BIOS-e820: [mem 0x0000000000100000-0x00000000cc8e2fff] usable
[    0.000000] BIOS-e820: [mem 0x00000000cc8e3000-0x00000000cc8e3fff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000cc8e4000-0x00000000cc90dfff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000cc90e000-0x00000000d6202fff] usable
[    0.000000] BIOS-e820: [mem 0x00000000d6203000-0x00000000d7f52fff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000d7f53000-0x00000000d7fa2fff] ACPI NVS
[    0.000000] BIOS-e820: [mem 0x00000000d7fa3000-0x00000000d7ffefff] ACPI data
[    0.000000] BIOS-e820: [mem 0x00000000d7fff000-0x00000000d7ffffff] usable
[    0.000000] BIOS-e820: [mem 0x00000000d8000000-0x00000000d80fffff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000d8200000-0x00000000db7fffff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000f80fa000-0x00000000f80fafff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000f80fd000-0x00000000f80fdfff] reserved
[    0.000000] BIOS-e820: [mem 0x00000000fe000000-0x00000000fe010fff] reserved
[    0.000000] BIOS-e820: [mem 0x0000000100000000-0x00000002237fffff] usable
{% endhighlight %}

<!--
http://tomhibolu.iteye.com/blog/1214876
-->

## 内存初始化

通过 BIOS 取得所有的内存布局之后，Linux 会对所获取的内存块做相关的检查并保存对其处理后的区域，也就是内存活动区域，会保存在 `struct node_active_region` 中。

另外，在 `arch/x86/kernel/x86_init.c` 中，维护了一个 `x86_init` 变量，用于保存常见的初始化函数，如页表的初始化函数 `pagetable_init()`，实际指向的是 `paging_init()` 函数。

接着会对 zones 进行初始化，设置最终的分页机制等；系统的内存信息保存在 `/proc/meminfo` 中，内核实现可以参考 `fs/proc/meminfo.c` 。

> MTRR (Memory Type Range Register) 用来确定系统内存中一段物理内存的类型，进而可以控制处理器对内存区域的访问，也就是告诉 CPU 在解析或者说操作内存的时候应该用什么手段，常见的有 Write Through(WT)、Write Back(WB)、Write Protected(WP)等。
>
> 系统当前的 mtrr 信息保存在 /proc/mtrr 中。

如下，详细介绍 Linux 中内存的初始化过程，首先需要注意的是，64-bits 没有 HIGH 。

{% highlight text %}
start_kernel()@init/main.c
 |-setup_arch()                           ← 完成与体系结构相关的初始化工作
 | |-setup_memory_map()                   ← 建立内存图
 | | |-x86_init.resources.memory_setup()
 | | | |-sanitize_e820_map()              ← 消除内存重叠部分
 | | | |-append_e820_map()                ← 将内存配置从boot_params.e820_map拷贝到e820中
 | | |   |-e820_add_region()              ← 将内存段的信息保存到e820的map数组中
 | | |-e820_print_map()                   ← 打印出物理内存的分布
 | |
 | |-e820_end_of_ram_pfn()                ← 找出最大的可用页帧号，后面会找出低端内存的最大页面号
 | | |-e820_end_pfn()                     ← 会打印last_pfn、max_arch_pfn
 | |
 | |-mtrr_bp_init()                       ← 设置MTRR
 | |-init_mem_mapping()                   ← 设置最终的内存映射机制
 | | |-probe_page_size_mask()
 | | |  |-init_gbpages()
 | | |-init_memory_mapping()              ← 入参是(0,0x100000)，该函数中会打印一系列的mapping信息
 | | | |-split_mem_range()
 | | | |-kernel_physical_mapping_init()   ← 完成虚拟地址到物理地址的映射
 | | | | |-pgd_populate()
 | | | | |-__flush_tlb_all()
 | | | |-add_pfn_range_mapped()
 | | |-load_cr3()
 | | |-__flush_tlb_all()
 | | |-early_memtest()
 | |
 | |-early_trap_pf_init()
 | |-setup_real_mode()
 | |-memblock_set_current_limit()
 | |-reserve_initrd()                     ← 设置RADDISK
 | |-acpi_boot_table_init()               ← ACPI设置
 | |
 | |-initmem_init()                       ← 初始化内存分配器
 | |-x86_init.paging.pagetable_init()     ← 建立完整的页表，实际调用paging_init()
 |   |-sparse_init()
 |   |-node_clear_state()
 |   |-zone_sizes_init()                  ← 在此设置各个区，可以查看/proc/meminfo
 |     |-free_area_init_nodes()           ← 打印Zone ranges信息
 |
 |-build_all_zonelists()                  ← 区域链表设置
 |-page_alloc_init()

{% endhighlight %}

<!--
page_table_range_init()
kmem_cache_init()   # 打印SLUB相关信息
-->

在设置内存时，首先会打印出系统的物理内存分布，然后找出最大物理页面帧号 `max_pfn`，低端内存的最大页面号 `max_low_pfn` 。

### 页表初始化

在 2.6.11 后，Linux 采用四级分页模型：1) 页全局目录 (Page Global Directory, PGD)；2) 页上级目录 (Page Upper Directory, PUD)；3) 页中间目录 (Page Middle Directory, PMD)；4) 页表 (Page Table, PT)。

对于没有启动 PAE 的 32 位系统，实际上只用到了两级分页，将 PUD、PMD 设置为 0 达到使用两级分页的目的，但为了保证程序能 32 位和 64 系统上都能运行，内核保留了页上级目录和页中间目录在指针序列中的位置。

## 内存结构

![memory architecture node zone page]({{ site.url }}/images/linux/kernel/memory-architecture-zone-page.png "memory architecture node zone page"){: .pull-center width="70%" }

为了实现了良好的可伸缩性，Linux 采用了与具体架构不相关的设计模型，由内存节点 node、内存区域 zone 和物理页框 page 三级架构组成。

#### NODE

一个总线设备访问位于同一个节点中的任意内存单元所花的代价相同，而访问任意两个不同节点中的内存单元所花的代价不同，也就是对于 UMA 只有一个节点，对于 NUMA 则会有多个节点。

内核中使用 `struct pg_data_t` 来表示内存节点 node 。

<!-- Uniform Memory Architecture, UMA -->

#### ZONE

同一个内存节点内，由于各种原因它们的用途和使用方法可能并不一样，如 IA32，由于历史原因使得 ISA 设备只能使用最低 16MB 来进行 DMA 传输。

一般来说，分为了 `ZONE_DMA`、`ZONE_DMA32`、`ZONE_NORMAL`、`ZONE_HIGHMEM` 几种，不同的平台会有所区别，例如 64 位中有 `ZONE_DMA32` ，而没有 `ZONE_HIGHMEM` 。


#### PAGE

内存管理的最小单元是 Page ，这也就意味着在一页内的线性地址和物理地址是连续的。另外，需要注意 page 与 page frame 的区别，后者是物理内存的分割单元；前者包含数据，可能保存在 page frame 中，也可能保存在磁盘上。

详细参见下面的介绍。

![memory node zone page layout]({{ site.url }}/images/linux/kernel/memory-node-zone-page-layout.jpg "memory node zone page layout"){: .pull-center width="90%" }






## 页相关操作

如下，是用户空间和内核空间中创建内存的流程。

![memory kernel userspace management]({{ site.url }}/images/linux/kernel/memory-kernel-userspace-management.jpg "memory kernel userspace management"){: .pull-center width="80%" }

不管是内核还是还是用户空间，分配内存时，底层都是以 page 为单位分配内存，这个 page 可以作为：

1. 页缓存使用 (mapping域指向address_space对象)<!--这个东西主要是用来对磁盘数据进行缓存，我们平时监控服务器时，经常会用top/free看到cached参数，这个参数其实就是页缓存(page cache)，一般如果这个值很大，就说明内核缓冲了许多文件，读IO就会较小-->
2. 作为私有数据 (由private域指向)<!--可以是作为块冲区中所用，也可以用作swap，当是空闲的page时，那么会被伙伴系统使用。-->
3. 作为进程页表中的映射<!--映射到进程页表后，我们用户空间的malloc才能获得这块内存-->

page 的所有信息通过 `struct page` 表示，该结构体在 `include/linux/mm_types.h` 中定义：

{% highlight c %}
struct page {
    unsigned long flags;                  // 是否脏、锁定，可以查看page-flags.h
    union {
        struct address_space *mapping;
        void *s_mem;
    };
    atomic_t _count;                      // 页引用计数，-1表示未使用
    atomic_t _mapcount;                   // 页映射计数
    void *virtual;                        // 页在虚拟内存中的地址
}
{% endhighlight %}

内核通过 `struct page` 表示每个物理页，占用 40 个字节，假定系统物理页大小为 4KB 。

在 `include/asm-x86/page.h` 中定义了内核中和 page 相关的一些常量：

{% highlight c %}
#define PAGE_SHIFT  12
#define PAGE_SIZE   (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))
{% endhighlight %}

可以看出一个 page 所对应的物理块的大小 (PAGE_SIZE) 是 4096 ；内核会将所有 `struct page* ` 放到一个全局数组 (mem_map) 中，而内核中常会看到的页帧号 (pfn)，也就是该数组的 index 。

在 `arch/x86/kernel/e820.c` 文件中，定义了最大的 pfn 。

{% highlight c %}
#ifdef CONFIG_X86_32
# ifdef CONFIG_X86_PAE
#  define MAX_ARCH_PFN      (1ULL<<(36-PAGE_SHIFT))
# else
#  define MAX_ARCH_PFN      (1ULL<<(32-PAGE_SHIFT))
# endif
#else /* CONFIG_X86_32 */
# define MAX_ARCH_PFN MAXMEM>>PAGE_SHIFT
#endif
{% endhighlight %}

这里的 `MAX_ARCH_PFN` 就是系统的最大页帧号，但这个只是理论上的最大值，在 `e820_end_pfn()` 函数中，会计算最终的 `max_pfn`，可以通过 `dmesg | grep max_arch_pfn` 命令查看 。

{% highlight text %}
start_kernel()
 |-setup_arch()
   |-e820_end_of_ram_pfn()
     |-e820_end_pfn()
{% endhighlight %}

接着看下与 page 结构的相关宏以及函数。


{% highlight c %}
// include/asm-generic/memory_model.h
#define page_to_pfn __page_to_pfn
#define pfn_to_page __pfn_to_page
{% endhighlight %}

如上宏定义的作用是将 `struct page*` 和前面提到的 `pfn` 页帧号之间相互转换。

根据在内核编译时的不同参数，那么对应的 `__page_to_pfn()` 和 `__pfn_to_page()` 函数也不相同，可以通过如下命令查看当前发行版本所使用的宏定义。

{% highlight text %}
$ grep -E '(\<CONFIG_FLATMEM\>|\<CONFIG_DISCONTIGMEM\>|\<CONFIG_SPARSEMEM_VMEMMAP\>|\<CONFIG_SPARSEMEM\>)' \
    /boot/config-$(uname -r)
CONFIG_SPARSEMEM=y
CONFIG_SPARSEMEM_VMEMMAP=y
{% endhighlight %}

### 页分配与释放

如果一个模块需要分配大块的内存，最好使用面向页的分配技术，下面列举所有的页为单位进行连续物理内存分配，也称为低级页分配器：

{% highlight text %}
alloc_pages(gfp_mask, order)
    分配2^order个页，返回指向第一页的指针
alloc_pages(gfp_mask)
    分配一页，返回指向页的指针
__get_free_pages(gfp_mask, order)
    分配2^order个连续页，返回指向其逻辑地址的指针
__get_free_pages(gfp_mask)
    分配一页，返回指向其逻辑地址的指针，未清零
get_zeroed_page(gfp_mask)
    分配一页，并填充内容为0，返回指向其逻辑地址的指针

__free_pages(page, order)
    从page开始，释放2^order个页
free_pages(addr, order)
    从地址addr开始，释放2^order个页
free_page(addr)
    释放addr所在的那一页
{% endhighlight %}

<!--
get_zeroed_page：对于用户空间，这个方法能保障系统敏感数据不会泄露
page_address: 把给定的页转换成逻辑地址
-->

### 字节分配与释放

`kmalloc()`、`vmalloc()` 分配都是以字节为单位，所分配到的内存在物理内存中连续且保持原有的数据不清零。

{% highlight text %}
#include <linux/slab.h>
void * kmalloc(size_t size, gfp_t flags);
  返回内存块的指针，其内存块大小至少为size，分配内存在物理内存中连续，数据不清零
size  : 申请内存大小。
flags : 取值说明。
  GFP_USER  : 用户空间的分配内存，可能休眠；
  GFP_KERNEL: 内核空间的内存分配，当空闲内存较少时可能休眠等待一个页面，休眠时内核会用适当的动作获取空闲页；
  GFP_ATOMIC: 原子性的内存分配，内核会预留部分空闲页，不会休眠，如中断处理程序、软中断、tasklet等；
  __GFP_DMA : 要求分配可用于DMA的内存。
{% endhighlight %}

`kmalloc()` 最终调用 `__get_free_pages()` 来分配内存，故前缀都是 `GFP_` 开头，最多只能分配 32 个 page 大小的内存，也就是 `32*page=32*4K=128K` 大小，其中 16 个字节用来记录页描述结构。

<!-- kmalloc分配的是常驻内存，不会被交换到文件中。最小分配单位是32或64字节。-->

`kzalloc()` 等价于先用 `kmalloc()` 申请空间，再初始化，所有申请的元素都被初始化为 0 。

{% highlight c %}
static inline void *kzalloc(size_t size, gfp_t flags)
{
    return kmalloc(size, flags | __GFP_ZERO);  // 通过标志位表示初始化元素为0
}
{% endhighlight %}

`vmalloc()` 是一个基本的 Linux 内存分配机制，在虚拟内存空间分配一块连续的内存区，尽管这些页在物理内存中不连续 (使用单独的 `alloc_page()` 来获得每个页) ，但内核认为它们地址是连续的，也就是说所分配的内存是逻辑上连续的。

另外，需要注意的是，在内核中，`vmalloc()` 在大部分情况下不推荐使用，因为在某些体系上留给 `vmalloc()` 的地址空间相对小，且效率不高，默认是可以休眠。

简言之，`vmalloc()` 返回一个指向内存块的指针，其内存块大小至少为 size，函数允许休眠。

{% highlight text %}
#include <linux/vmalloc.h>
void * vmalloc(unsigned long size);
void vfree(void * addr);
{% endhighlight %}

`vmalloc()` 使用虚拟地址，每次分配都要通过适当地设置页表来建立 (虚拟) 内存区域，其分配范围是 `VMALLOC_START~VAMLLOC_END` 。

<!--
vmalloc分配的地址只在处理器有MMU之上才有意义。当驱动需要真正的物理地址时，就不能使用vmalloc。调用vmalloc的正确场合是分配一个大的、只存在于软件中的、用于缓存的内存区域时。注意：vamlloc比__get_free_pages要更多开销，因为它必须即获取内存又建立页表。因此，调用vmalloc 来分配仅仅一页是不值得的。vmalloc的一个小的缺点在于它无法在原子上下文中使用。因为它内部使用kmalloc(GFP_KERNEL) 来获取页表的存储空间，因此可能休眠。vmalloc是面向页的 (它们会修改页表) ，重定位的或分配的空间都会被上调到最近的页边界。
-->



<!--
kmalloc     内核空间    物理地址连续    最大值128K-16   kfree   性能更佳
vmalloc     内核空间    虚拟地址连续    更大            vfree   更易分配大内存
malloc      用户空间    虚拟地址连续    更大            free
-->

### SLAB

内核的驱动程序常常需要反复分配许多相同大小内存块，为此增加了一些特殊的内存池，称为高速缓存。而 Linux 内核的高速缓存也被称为 "SLAB 分配器"，对应了 `kmem_cache_t` 类型，相关函数有：

{% highlight text %}
----- 创建可容纳任意数目内存区域的、大小都相同的高速缓存对象
kmem_cache_t *kmem_cache_create(const char *name, size_t size,size_t offset, unsigned long flags,
    void (*constructor)(void *, kmem_cache_t *,unsigned long flags),
    void (*destructor)(void *, kmem_cache_t *, unsigned long flags));

----- 调用如下函数从已创建的后备高速缓存中分配对象，flags和kmalloc的相同
void *kmem_cache_alloc(kmem_cache_t *cache, int flags);

----- 使用如下函数释放一个对象
void kmem_cache_free(kmem_cache_t *cache, const void *obj);

----- 当驱动用完这个高速缓存时，一般在当模块被卸载时释放缓存
int kmem_cache_destroy(kmem_cache_t *cache);
{% endhighlight %}

slab 分配器的特点有：

* 对于频繁地分配和释放的数据结构，会缓存；
* 为了避免由于频繁分配和回收导致内存碎片，会通过空闲链表进行缓存；
* 部分缓存专属单个处理器，分配和释放操作可以不加 SMP 锁；

slab 层把不同的对象划分为高速缓存组，每个高速缓存组都存放不同类型的对象，每个对象类型对应一个高速缓存，每个高速缓存都是用 `kmem_cache` 结构来表示。

<!--
kmem_cache_crreate：创建高速缓存
kmem_cache_destroy: 撤销高速缓存
kmem_cache_alloc: 从高速缓存中返回一个指向对象的指针
kmem_cache_free：释放一个对象
-->

例如在内核初始化时，通过 `fork_init()` 中会创建一个名为 `struct task_struct` 的高速缓存，每当进程调用 `fork()` 时，会通过 `dup_task_struct()` 创建一个新的进程描述符，并调用 `do_fork()`，完成从高速缓存中获取对象。


<!--
1.6 栈的静态分配

当设置单页内核栈，那么每个进程的内核栈只有一页大小，这取决于编译时配置选项。 好处：

    可以减少每个进程内存的消耗；
    随着机器运行时间的增加，寻找两个未分配的、连续的页越来越困难，物理内存碎片化不断加重，那么给每个新进程分配虚拟内存的压力也增大；
    每个进程的调用链在自己的内核栈中，当单页栈选项被激活时，中断处理程序可获得自己的栈；

任意函数必须尽量节省栈资源， 方法就是所有函数让局部变量所占空间之和不要超过几百字节。
1.7 高端内存的映射

高端内存中的页不能永久地映射到内核地址空间。

    kmap：把给定page结构映射到内核地址空间；
        当page位于低端内存，函数返回该页的虚拟地址
        当page位于高端内存，建立一个永久映射，再返回地址
    kunmap: 永久映射的数量有限，应通过kunmap及时解除映射
    kmap_atomic: 临时映射
    kunmap_atomic: 解除临时映射

1.8 每个CPU数据

    alloc_percpu: 给系统的每个处理器分配一个指定类型对象的实例，以单字节对齐；
    free_percpu: 释放每个处理器的对象实例；
    get_cpu_var: 返回一个执行当前处理器数据的特殊实例，同时会禁止内核抢占
    put_cpu_var: 会重新激活内核抢占

使用每个CPU数据好处：

    减少了数据锁定，每个CPU访问自己CPU数据
    大大减少缓存失效，失效往往发生在一个处理器操作某个数据，而其他处理器缓存了该数据，那么必须清理或刷新缓存。持续不断的缓存失效称为缓存抖动。

1.9 小结

分配函数选择：

    连续的物理页，使用低级页分配器 或kmalloc();
    高端内存分配，使用alloc_pages(),返回page结构指针； 想获取地址指针，应使用kmap(),把高端内存映射到内核的逻辑地址空间；
    仅仅需要虚拟地址连续页，使用vmalloc()，性能有所损失；
    频繁创建和撤销大量数据结构，考虑建立slab高速缓存。
-->




<!--
为了确保在内存分配不允许失败情况下成功分配内存，内核提供了称为内存池( "mempool" )的抽象，它其实是某种后备高速缓存。它为了紧急情况下的使用。所以使用时必须注意：mempool会分配一些内存块，使其空闲而不真正使用，所以容易消耗大量内存。而且不要使用mempool处理可失败的分配。应避免在驱动代码中使用mempool。对应的函数有：
mempool_t *mempool_create(int min_nr,mempool_alloc_t *alloc_fn,mempool_free_t *free_fn,void *pool_data);
/*min_nr 参数是内存池应当一直保留的最小数量的分配对象，实际的分配和释放对象由alloc_fn和free_fn处理*/
typedef void *(mempool_alloc_t)(int gfp_mask, void *pool_data);
typedef void (mempool_free_t)(void *element, void *pool_data);
/*给 mempool_create 最后的参数 *pool_data 被传递给 alloc_fn 和 free_fn */
      创建内存池后，分配和释放对象：
void *mempool_alloc(mempool_t *pool, int gfp_mask);
void mempool_free(void *element, mempool_t *pool);
     若不再需要内存池，则返回给系统：
void mempool_destroy(mempool_t *pool);
/*在销毁 mempool 之前，必须返回所有分配的对象，否则会产生 oops*/
-->


### 内存模型

在 Linux 内核中支持 3 种不同的内存模型：Flat Memory Model、Discontiguous Memory Model 和 Sparse Memory Model；所
谓的 Memory Model 其实就是从 CPU 的角度看，其物理内存的分布情况。

<!--
Linux内存模型
http://www.wowotech.net/memory_management/memory_model.html
-->

{% highlight c %}
#define VMEMMAP_START    _AC(0xffffea0000000000, UL)
#define vmemmap ((struct page *)VMEMMAP_START)
#define __pfn_to_page(pfn)  (vmemmap + (pfn))
#define __page_to_pfn(page) (unsigned long)((page) - vmemmap)
{% endhighlight %}

<!--
linux内核研究笔记(一）内存管理 – page介绍
http://blogread.cn/it/article/6619
-->

## kswapd

该内核守护进程，通过 ```module_init(kswapd_init)@mm/vmscan.c``` 初始化，然后通过 ```kthread_run()``` 启动 ```kswapd()``` 。该线程会维护一个 LRU 队列，会将最近没有被访问过的 (PG_referenced) 的页放入到 inactive 队列中；否则放置到 active 队列中。

当系统中可用内存很少时，内核线程 kswapd 被唤醒，开始回收释放 page，通过 `pages_min`、`pages_low`、`pages_high` 这些阈值确定 Zone 的内存使用压力状态；注意，最新内核中这三个变量变成了 `watermark[]` 的成员，分别对应于 `WMARK_MIN`，`WMARK_LOW` 和 `WMARK_HIGH`。

![memory watermark]({{ site.url }}/images/linux/kernel/memory-watermark.png "memory watermark"){: .pull-center width="70%" }

各个状态如下：

* page_min：如果空闲页数目小于该值，则该zone非常缺页，页面回收压力很大。
* page_low: 如果空闲页数目小于该值，kswapd线程将被唤醒，并开始释放回收页面。
* page_high: 如果空闲页面的值大于该值，则该zone的状态很完美, kswapd线程将重新休眠。

内核在计算上述的几个值之前会首先计算一个关键参数 `min_free_kbytes`，它是为关键性分配保留的内存空间的最小值。

<!--
该关键参数有一个约束：不能小于128k，不能大于64M。其计算公式：

lowmem_kbytes = nr_free_buffer_pages() * (PAGE_SIZE >> 10);
min_free_kbytes = int_sqrt(lowmem_kbytes * 16);

阈值的计算由init_per_zone_pages_min（ 最新内核中是init_per_zone_wmark_min）完成。该函数还会完成每个zone的lowmem_reserve的计算，该数组用于为不能失败的关键分配预留的内存页。
-->







## 参考

<!--
Linux内核中内存相关的操作函数-1
http://blog.chinaunix.net/uid-27123777-id-3279787.html

Linux内核内存管理学习之一（基本概念，分页及初始化）
http://blog.csdn.net/goodluckwhh/article/details/9970845


http://www.tldp.org/LDP/khg/HyperNews/get/memory/linuxmm.html

-->

关于 Linux 内存介绍可以查看 [Where is the memory going?](https://www.halobates.de/memory.pdf) 以及 [相关 PDF](https://www.halobates.de/memorywaste.pdf) 。

另外，关于 Linux 内核中内存相关的介绍可以查看 [Page Cache, the Affair Between Memory and Files](http://duartes.org/gustavo/blog/post/page-cache-the-affair-between-memory-and-files/) 等一系列的文章。


{% highlight text %}
{% endhighlight %}
