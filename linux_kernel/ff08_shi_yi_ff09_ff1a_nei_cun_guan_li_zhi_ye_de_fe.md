# （十一）：内存管理之页的分配与回收


内存管理单元（MMU）负责将管理内存，在把虚拟地址转换为物理地址的硬件的时候是按页为单位进行处理，从虚拟内存的角度来看，页就是内存管理中的最小单位。页的大小与体系结构有关，在 x86 结构中一般是4KB（32位）或者8KB（64位）。
通过 getconf 命令可以查看系统的page的大小：


```c
# getconf -a | grep PAGE  
PAGESIZE                           4096  
PAGE_SIZE                          4096  
_AVPHYS_PAGES                      230873  
_PHYS_PAGES                        744957  
```

内核中的每个物理页用struct  page结构表示，结构定义于文件`<include/linux/mm_types.h>`：


```c
struct page {
    unsigned long flags;        /*页的状态*/
    atomic_t _count;        /* 页引用计数 */
    union {
        atomic_t _mapcount; /* 已经映射到mms的pte的个数*/
        struct {        /* */
            u16 inuse;
            u16 objects;
        };
    };
    union {
        struct {
            unsigned long private;
            struct address_space* mapping;
        };
#if USE_SPLIT_PTLOCKS
        spinlock_t ptl;
#endif
        struct kmem_cache* slab;    /* 指向slab层 */
        struct page* first_page;    /* Compound tail pages */
    };
    union {
        pgoff_t index;      /* Our offset within mapping. */
        void* freelist;     /* SLUB: freelist req. slab lock */
    };
    struct list_head lru;       /* 将页关联起来的链表项 */

#if defined(WANT_PAGE_VIRTUAL)
    void* virtual;          /* Kernel virtual address (NULL if
                       not kmapped, ie. highmem) */
#endif /* WANT_PAGE_VIRTUAL */
#ifdef CONFIG_WANT_PAGE_DEBUG_FLAGS
    unsigned long debug_flags;  /* Use atomic bitops on this */
#endif

#ifdef CONFIG_KMEMCHECK
    void* shadow;
#endif
};
```

内核使用这一结构来管理系统中所有的页，因为内核需要知道一个该页是否被分配，是被谁拥有的等信息。

由于ISA总线的DMA处理器有严格的限制，只能对物理内存前16M寻址，内核线性地址空间只有1G,CPU不能直接访问所有的物理内存。这样就导致有一些内存不能永久地映射在内核空间上。所以在linux中，把页分为不同的区，使用区来对具有相似特性的页进行分组。分组如下（以x86-32为例）：

ZONE_DMA 小于16M内存页框，这个区包含的页用来执行DMA操作。

ZONE_NORMAL 16M~896M内存页框，个区包含的都是能正常映射的页。

ZONE_HIGHMEM 大于896M内存页框，这个区包"高端内存"，其中的页能不永久地映射到内核地址空间。

linux 把系统的页划分区，形成不同的内存池，这样就可以根据用途进行分配了。

每个区都用struct zone表示，定义于<include/linux/mmzone.h>中。该结构体较大，详细结构体信息可以查看源码文件。

linux提供了几个以页为单位分配释放内存的接口，定义于<include/linux/gfp.h>中。分配内存主要有以下方法：

```c
alloc_page(gfp_mask)	只分配一页，返回指向页结构的指针
alloc_pages(gfp_mask, order)	分配 2^order 个页，返回指向第一页页结构的指针
__get_free_page(gfp_mask)	只分配一页，返回指向其逻辑地址的指针
__get_free_pages(gfp_mask, order)	分配 2^order 个页，返回指向第一页逻辑地址的指针
get_zeroed_page(gfp_mask)	只分配一页，让其内容填充为0，返回指向其逻辑地址的指针
```


alloc_*函数返回的是内存的`物理地址`，
get_* 函数返回内存物理地址映射后的`逻辑地址`。
如果无须直接操作物理页结构体的话，一般使用 get_*函数。

`释放页`的函数有：

```c
externvoid__free_pages( struct page *page, unsignedintorder); 
externvoidfree_pages(unsigned longaddr, unsigned intorder); 
externvoidfree_hot_page( struct page *page);
```


当需要以页为单位的连续物理页时，可以使用上面这些分配页的函数，对于常用以字节为单位的分配来说，内核提供来kmalloc()函数。

kmalloc()函数和用户空间一族函数类似，它可以以字节为单位分配内存，对于大多数内核分配来说，kmalloc函数用得更多。


- 行为标志 ：控制分配内存时，分配器的一些行为，如何分配所需内存。
- 区标志   ：控制内存分配在那个区(ZONE_DMA, ZONE_NORMAL, ZONE_HIGHMEM 之类)。
- 类型标志 ：由上面2种标志组合而成的一些常用的场景。

行为标志主要有以下几种：

```sh
行为标志     描述
__GFP_WAIT	分配器可以睡眠
__GFP_HIGH	分配器可以访问紧急事件缓冲池
__GFP_IO	分配器可以启动磁盘I/O
__GFP_FS	分配器可以启动文件系统I/O
__GFP_COLD	分配器应该使用高速缓存中快要淘汰出去的页
__GFP_NOWARN	分配器将不打印失败警告
__GFP_REPEAT	分配器在分配失败时重复进行分配，但是这次分配还存在失败的可能
__GFP_NOFALL	分配器将无限的重复进行分配。分配不能失败
__GFP_NORETRY	分配器在分配失败时不会重新分配
__GFP_NO_GROW	由slab层内部使用
__GFP_COMP	添加混合页元数据，在 hugetlb 的代码内部使用
```

标志主要有以下3种：

```sh
区标志        描述
__GFP_DMA	从 ZONE_DMA 分配
__GFP_DMA32	只在 ZONE_DMA32 分配 ，和 ZONE_DMA 类似，该区包含的页也可以进行DMA操作
__GFP_HIGHMEM	从 ZONE_HIGHMEM 或者 ZONE_NORMAL 分配，优先从 ZONE_HIGHMEM 分配，如果 ZONE_HIGHMEM 没有多余的页则从 ZONE_NORMAL 分配
```
类型标志是编程中最常用的，在使用标志时，应首先看看类型标志中是否有合适的，如果没有，再去自己组合 行为标志和区标志。


```sh
类型标志      描述                                                         实际标志
GFP_ATOMIC	 这个标志用在中断处理程序，下半部，持有自旋锁以及其他不能睡眠的地方	__GFP_HIGH
GFP_NOWAIT	与 GFP_ATOMIC 类似，不同之处在于，调用不会退给紧急内存池。
这就增加了内存分配失败的可能性	0
GFP_NOIO	这种分配可以阻塞，但不会启动磁盘I/O。
这个标志在不能引发更多磁盘I/O时能阻塞I/O代码，可能会导致递归	__GFP_WAIT
GFP_NOFS	这种分配在必要时可能阻塞，也可能启动磁盘I/O，但不会启动文件系统操作。
这个标志在你不能再启动另一个文件系统的操作时，用在文件系统部分的代码中	(__GFP_WAIT
｜ __GFP_IO)  
GFP_KERNEL	这是常规的分配方式，可能会阻塞。这个标志在睡眠安全时用在进程上下文代码中。
为了获得调用者所需的内存，内核会尽力而为。这个标志应当为首选标志	(__GFP_WAIT
｜ __GFP_IO 
｜ __GFP_FS ) 
GFP_USER	这是常规的分配方式，可能会阻塞。用于为用户空间进程分配内存时	(__GFP_WAIT
｜ __GFP_IO 
｜ __GFP_FS ) 
GFP_HIGHUSER	从 ZONE_HIGHMEM 进行分配，可能会阻塞。用于为用户空间进程分配内存	(__GFP_WAIT
｜ __GFP_IO 
｜ __GFP_FS )
｜__GFP_HIGHMEM) 
GFP_DMA	从 ZONE_DMA 进行分配。需要获取能供DMA使用的内存的设备驱动程序使用这个标志
通常与以上的某个标志组合在一起使用。
```

```sh
以上各种类型标志的使用场景总结：
场景                         相应标志
进程上下文，可以睡眠	使用 GFP_KERNEL
进程上下文，不可以睡眠	使用 GFP_ATOMIC，在睡眠之前或之后以 GFP_KERNEL 执行内存分配
中断处理程序	使用 GFP_ATOMIC
软中断	使用 GFP_ATOMIC
tasklet	使用 GFP_ATOMIC
需要用于DMA的内存，可以睡眠	使用 (GFP_DMA｜GFP_KERNEL)
需要用于DMA的内存，不可以睡眠	使用 (GFP_DMA｜GFP_ATOMIC)，或者在睡眠之前执行内存分配
```

kmalloc  所对应的释放内存的方法为：


```c
void kfree(const void *)；
vmalloc()也可以按字节来分配内存。
void *vmalloc(unsigned long size)
```

和kmalloc是一样的作用，不同在于前者分配的内存虚拟地址是连续的，而物理地址则无需连续。kmalloc()可以保证在物理地址上都是连续的，虚拟地址当然也是连续的。vmalloc()函数只确保页在虚拟机地址空间内是连续的。它通过分配非联系的物理内存块，再“修正”页表，把内存映射到逻辑地址空间的连续区域中，就能做到这点。但很显然这样会降低处理性能，因为内核不得不做“拼接”的工作。所以这也是为什么不得已才使用 vmalloc()的原因 。 vmalloc()可能睡眠，不能从中断上下文中进行调用，也不能从其他不允许阻塞的情况下进行调用。释放时必须使用vfree()。

```c
void vfree(const void *)；
```

对于内存页面的管理，通常是先在虚存空间中分配一个虚存区间，然后才根据需要为此区间分配相应的物理页面并建立起映射，也就是说，虚存区间的分配在前，而物理页面的分配在后。但由于频繁的请求和释放不同大小的连续页框，必然导致在已分配页框的块内分散了许多小块的空闲页框，由此产生的问题是：即使有足够的空闲页框可以满足请求，但当要分配一个大块的连续页框时，无法满足请求。这就是著名的内存管理问题：外碎片问题。Linux采用著名的伙伴（Buddy）系统算法来解决外碎片问题。


 把所有的空闲页框分组为11个块链表。每个块链表包含大小为1,2,4,8,16,32,64,128,256,512，1024个的页框。伙伴系统算法原理为：
 
 假设请求一个256个页框的块，先在256个页框的链表内检查是否有一个空闲的块。如果没有这样的块，算法会查找下一个更大的块，在512个页框的链表中找一个空闲块。如果存在这样的块，内核就把512的页框分成两半，一半用作满足请求，另一半插入256个页框的链表中。如果512个页框的块链表也没有空闲块，就继续找更大的块，1024个页框的块。如果这样的块存在，内核把1024个页框的256个页框用作请求，然后从剩余的768个中拿出512个插入512个页框的链表中，把最后256个插入256个页框的链表中。
 
 页框块的释放过程如下：
如果两个块具有相同的大小：a，并且他们的物理地址连续那么这两个块成为伙伴，内核就会试图把大小为a的一对空闲伙伴块合并为一个大小为2a的单独块。该算法还是迭代的，如果合并成功的话，它还会试图合并2a的块。

管理分区数据结构struct zone_struct中，涉及到空闲区数据结构。

```c
struct free_area    free_area[MAX_ORDER];

struct free_area {
    struct list_head    free_list[MIGRATE_TYPES];
    unsigned long        nr_free;
};
```

采用伙伴算法分配内存时，每次至少分配一个页面。但当请求分配的内存大小为几十个字节或几百个字节时应该如何处理？如何在一个页面中分配小的内存区，小内存区的分配所产生的内碎片又如何解决？slab的分配模式可以解决该问题，下一节我们将开始分析slab分配器。
