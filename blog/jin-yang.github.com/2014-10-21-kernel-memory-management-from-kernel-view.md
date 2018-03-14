---
title: 內存-內核空間
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,內存,kernel,內存空間
description:
---

線性地址到物理地址的映射是通過 page table 完成的，內核會在啟動分頁機制前完成初始化；而且內核會將 A) 不可用物理地址空間，B) 內核代碼以及內核初始數據結構對應的地址空間保留。

接下來，看看內核中是如何管理內存的。

<!-- more -->

## 簡介

系統啟動時，BIOS 會自動發現當前物理內存的地址，而 Linux 內核會在啟動時 (還在實地址模式) 通過 e820 BIOS 系統調用獲取當前系統的物理內存地址，當然還有 IO 的映射地址等；開始內核會初始化部分內存供內核使用，然後調用 Bootmem 系統。

Linux 將內存分為不同的 Zone 管理，每個 Zone 中通過一個 buddy 系統分配內存，每次分配內存都是以頁的 2 指數倍分配，例如內核頁大小為 4K(`getconf PAGE_SIZE`)，那麼分配的頁為 4K, 8K, 16K, ..., 128K，一般系統最大為 128K 。

Buddy Allocator 最大的問題就是碎片，所以一般系統不會直接使用，通常其上層還包括了 Page Cache、Slab Allocator 。

![memory managment overview]({{ site.url }}/images/linux/kernel/memory-managment-overview.jpg "memory managment overview"){: .pull-center width="70%" }

### 地址空間

虛擬地址空間分成了兩部分：A) 用戶進程，使用的是地址空間的低地址部分 `0~TASK_SIZE`；B) 內核空間，地址空間的高地址部分。

<!--
PAGE_OFFSET：內核虛擬地址空間中低端內存的起始地址
PHYSICAL_START：內核物理地址的起始地址
MEMORY_START：內核低端內存的物理起始地址
-->

## 物理內存探測

Linux 被 bootloader 加載到內存後，首先執行的是 `_start()@arch/x86/boot/header.S/header.S`，該函數在做了一些準備工作後會跳轉到 boot 目錄下的 `main()@main.c` 函數執行，第一次內存相關的調用是在實模式時，通過調用 `detect_memory()` 實現。

如下是該函數的實現。

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

該函數會依次嘗試調用 `detect_memory_e820()`、`detect_memory_e801()`、`detect_memory_88()` 獲得系統物理內存佈局，這三個函數都在 memory.c 中實現。

其內部都會以內聯彙編的形式調用 bios 中斷以取得內存信息，該中斷調用形式為 `int 0x15`，同時調用前分別把 `AX` 寄存器設置為 `E820h`、`E801h`、`88h`，該功能分別用於獲取系統內存佈局、獲取內存大小、獲取擴展內存大小，關於 `0x15` 號中斷詳細信息可以去查詢相關手冊。

在 x86 中，IO 設備也會映射到內存空間，也就是說系統使用的物理內存空間是不連續的，被分成了很多段，而且每段的屬性也不一樣。通過 `int 0x15` 查詢物理內存時，每次返回一個內存段的信息，因此要想返回系統中所有的物理內存，必須以迭代的方式去查詢。

### 內存查詢

目前使用較多的是 `e820` ，可以通過 `dmesg` 查看內核啟動輸出，一般有類似 `e820: BIOS-provided physical RAM map` 的輸出，下面以 `e820` 為例。

> e820 是和 BIOS 的 int 0x15 中斷相關的，之所以叫 e820 是因為在用這個中斷時 AX 必須是 0xe820。

其中，與次相關的結構體如下。

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

在 `detect_memory_e820()` 函數中，把 `int 0x15` 放到一個 do-while 循環裡，將每次得到的內存段放到 `struct e820entry` 裡。像其它啟動時獲得的結果一樣，最終都會被放到 `boot_params` 裡，探測到的各個內存段情況被放到了 `boot_params.e820_map` 。

{% highlight text %}
main()@arch/x86/boot/main.c
 |-detect_memory()                 ← 探測物理內存
   |-detect_memory_e820()
   |-detect_memory_e801()
   |-detect_memory_88()

start_kernel()
 |-setup_arch()                    ← 完成與體系結構相關的初始化工作
   |-setup_memory_map()
     |-e820_print_map()
{% endhighlight %}

Linux 物理內存管理區會在 `start_kernel()` 函數中進行初始化，此時啟動分配器已經建立，所以可以從bootmem中分配需要的內存。

在 `e820_print_map()` 函數中，會打印如下內容。

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

## 內存初始化

通過 BIOS 取得所有的內存佈局之後，Linux 會對所獲取的內存塊做相關的檢查並保存對其處理後的區域，也就是內存活動區域，會保存在 `struct node_active_region` 中。

另外，在 `arch/x86/kernel/x86_init.c` 中，維護了一個 `x86_init` 變量，用於保存常見的初始化函數，如頁表的初始化函數 `pagetable_init()`，實際指向的是 `paging_init()` 函數。

接著會對 zones 進行初始化，設置最終的分頁機制等；系統的內存信息保存在 `/proc/meminfo` 中，內核實現可以參考 `fs/proc/meminfo.c` 。

> MTRR (Memory Type Range Register) 用來確定系統內存中一段物理內存的類型，進而可以控制處理器對內存區域的訪問，也就是告訴 CPU 在解析或者說操作內存的時候應該用什麼手段，常見的有 Write Through(WT)、Write Back(WB)、Write Protected(WP)等。
>
> 系統當前的 mtrr 信息保存在 /proc/mtrr 中。

如下，詳細介紹 Linux 中內存的初始化過程，首先需要注意的是，64-bits 沒有 HIGH 。

{% highlight text %}
start_kernel()@init/main.c
 |-setup_arch()                           ← 完成與體系結構相關的初始化工作
 | |-setup_memory_map()                   ← 建立內存圖
 | | |-x86_init.resources.memory_setup()
 | | | |-sanitize_e820_map()              ← 消除內存重疊部分
 | | | |-append_e820_map()                ← 將內存配置從boot_params.e820_map拷貝到e820中
 | | |   |-e820_add_region()              ← 將內存段的信息保存到e820的map數組中
 | | |-e820_print_map()                   ← 打印出物理內存的分佈
 | |
 | |-e820_end_of_ram_pfn()                ← 找出最大的可用頁幀號，後面會找出低端內存的最大頁面號
 | | |-e820_end_pfn()                     ← 會打印last_pfn、max_arch_pfn
 | |
 | |-mtrr_bp_init()                       ← 設置MTRR
 | |-init_mem_mapping()                   ← 設置最終的內存映射機制
 | | |-probe_page_size_mask()
 | | |  |-init_gbpages()
 | | |-init_memory_mapping()              ← 入參是(0,0x100000)，該函數中會打印一系列的mapping信息
 | | | |-split_mem_range()
 | | | |-kernel_physical_mapping_init()   ← 完成虛擬地址到物理地址的映射
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
 | |-reserve_initrd()                     ← 設置RADDISK
 | |-acpi_boot_table_init()               ← ACPI設置
 | |
 | |-initmem_init()                       ← 初始化內存分配器
 | |-x86_init.paging.pagetable_init()     ← 建立完整的頁表，實際調用paging_init()
 |   |-sparse_init()
 |   |-node_clear_state()
 |   |-zone_sizes_init()                  ← 在此設置各個區，可以查看/proc/meminfo
 |     |-free_area_init_nodes()           ← 打印Zone ranges信息
 |
 |-build_all_zonelists()                  ← 區域鏈表設置
 |-page_alloc_init()

{% endhighlight %}

<!--
page_table_range_init()
kmem_cache_init()   # 打印SLUB相關信息
-->

在設置內存時，首先會打印出系統的物理內存分佈，然後找出最大物理頁面幀號 `max_pfn`，低端內存的最大頁面號 `max_low_pfn` 。

### 頁表初始化

在 2.6.11 後，Linux 採用四級分頁模型：1) 頁全局目錄 (Page Global Directory, PGD)；2) 頁上級目錄 (Page Upper Directory, PUD)；3) 頁中間目錄 (Page Middle Directory, PMD)；4) 頁表 (Page Table, PT)。

對於沒有啟動 PAE 的 32 位系統，實際上只用到了兩級分頁，將 PUD、PMD 設置為 0 達到使用兩級分頁的目的，但為了保證程序能 32 位和 64 系統上都能運行，內核保留了頁上級目錄和頁中間目錄在指針序列中的位置。

## 內存結構

![memory architecture node zone page]({{ site.url }}/images/linux/kernel/memory-architecture-zone-page.png "memory architecture node zone page"){: .pull-center width="70%" }

為了實現了良好的可伸縮性，Linux 採用了與具體架構不相關的設計模型，由內存節點 node、內存區域 zone 和物理頁框 page 三級架構組成。

#### NODE

一個總線設備訪問位於同一個節點中的任意內存單元所花的代價相同，而訪問任意兩個不同節點中的內存單元所花的代價不同，也就是對於 UMA 只有一個節點，對於 NUMA 則會有多個節點。

內核中使用 `struct pg_data_t` 來表示內存節點 node 。

<!-- Uniform Memory Architecture, UMA -->

#### ZONE

同一個內存節點內，由於各種原因它們的用途和使用方法可能並不一樣，如 IA32，由於歷史原因使得 ISA 設備只能使用最低 16MB 來進行 DMA 傳輸。

一般來說，分為了 `ZONE_DMA`、`ZONE_DMA32`、`ZONE_NORMAL`、`ZONE_HIGHMEM` 幾種，不同的平臺會有所區別，例如 64 位中有 `ZONE_DMA32` ，而沒有 `ZONE_HIGHMEM` 。


#### PAGE

內存管理的最小單元是 Page ，這也就意味著在一頁內的線性地址和物理地址是連續的。另外，需要注意 page 與 page frame 的區別，後者是物理內存的分割單元；前者包含數據，可能保存在 page frame 中，也可能保存在磁盤上。

詳細參見下面的介紹。

![memory node zone page layout]({{ site.url }}/images/linux/kernel/memory-node-zone-page-layout.jpg "memory node zone page layout"){: .pull-center width="90%" }






## 頁相關操作

如下，是用戶空間和內核空間中創建內存的流程。

![memory kernel userspace management]({{ site.url }}/images/linux/kernel/memory-kernel-userspace-management.jpg "memory kernel userspace management"){: .pull-center width="80%" }

不管是內核還是還是用戶空間，分配內存時，底層都是以 page 為單位分配內存，這個 page 可以作為：

1. 頁緩存使用 (mapping域指向address_space對象)<!--這個東西主要是用來對磁盤數據進行緩存，我們平時監控服務器時，經常會用top/free看到cached參數，這個參數其實就是頁緩存(page cache)，一般如果這個值很大，就說明內核緩衝了許多文件，讀IO就會較小-->
2. 作為私有數據 (由private域指向)<!--可以是作為塊衝區中所用，也可以用作swap，當是空閒的page時，那麼會被夥伴系統使用。-->
3. 作為進程頁表中的映射<!--映射到進程頁表後，我們用戶空間的malloc才能獲得這塊內存-->

page 的所有信息通過 `struct page` 表示，該結構體在 `include/linux/mm_types.h` 中定義：

{% highlight c %}
struct page {
    unsigned long flags;                  // 是否髒、鎖定，可以查看page-flags.h
    union {
        struct address_space *mapping;
        void *s_mem;
    };
    atomic_t _count;                      // 頁引用計數，-1表示未使用
    atomic_t _mapcount;                   // 頁映射計數
    void *virtual;                        // 頁在虛擬內存中的地址
}
{% endhighlight %}

內核通過 `struct page` 表示每個物理頁，佔用 40 個字節，假定系統物理頁大小為 4KB 。

在 `include/asm-x86/page.h` 中定義了內核中和 page 相關的一些常量：

{% highlight c %}
#define PAGE_SHIFT  12
#define PAGE_SIZE   (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK   (~(PAGE_SIZE-1))
{% endhighlight %}

可以看出一個 page 所對應的物理塊的大小 (PAGE_SIZE) 是 4096 ；內核會將所有 `struct page* ` 放到一個全局數組 (mem_map) 中，而內核中常會看到的頁幀號 (pfn)，也就是該數組的 index 。

在 `arch/x86/kernel/e820.c` 文件中，定義了最大的 pfn 。

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

這裡的 `MAX_ARCH_PFN` 就是系統的最大頁幀號，但這個只是理論上的最大值，在 `e820_end_pfn()` 函數中，會計算最終的 `max_pfn`，可以通過 `dmesg | grep max_arch_pfn` 命令查看 。

{% highlight text %}
start_kernel()
 |-setup_arch()
   |-e820_end_of_ram_pfn()
     |-e820_end_pfn()
{% endhighlight %}

接著看下與 page 結構的相關宏以及函數。


{% highlight c %}
// include/asm-generic/memory_model.h
#define page_to_pfn __page_to_pfn
#define pfn_to_page __pfn_to_page
{% endhighlight %}

如上宏定義的作用是將 `struct page*` 和前面提到的 `pfn` 頁幀號之間相互轉換。

根據在內核編譯時的不同參數，那麼對應的 `__page_to_pfn()` 和 `__pfn_to_page()` 函數也不相同，可以通過如下命令查看當前發行版本所使用的宏定義。

{% highlight text %}
$ grep -E '(\<CONFIG_FLATMEM\>|\<CONFIG_DISCONTIGMEM\>|\<CONFIG_SPARSEMEM_VMEMMAP\>|\<CONFIG_SPARSEMEM\>)' \
    /boot/config-$(uname -r)
CONFIG_SPARSEMEM=y
CONFIG_SPARSEMEM_VMEMMAP=y
{% endhighlight %}

### 頁分配與釋放

如果一個模塊需要分配大塊的內存，最好使用面向頁的分配技術，下面列舉所有的頁為單位進行連續物理內存分配，也稱為低級頁分配器：

{% highlight text %}
alloc_pages(gfp_mask, order)
    分配2^order個頁，返回指向第一頁的指針
alloc_pages(gfp_mask)
    分配一頁，返回指向頁的指針
__get_free_pages(gfp_mask, order)
    分配2^order個連續頁，返回指向其邏輯地址的指針
__get_free_pages(gfp_mask)
    分配一頁，返回指向其邏輯地址的指針，未清零
get_zeroed_page(gfp_mask)
    分配一頁，並填充內容為0，返回指向其邏輯地址的指針

__free_pages(page, order)
    從page開始，釋放2^order個頁
free_pages(addr, order)
    從地址addr開始，釋放2^order個頁
free_page(addr)
    釋放addr所在的那一頁
{% endhighlight %}

<!--
get_zeroed_page：對於用戶空間，這個方法能保障系統敏感數據不會洩露
page_address: 把給定的頁轉換成邏輯地址
-->

### 字節分配與釋放

`kmalloc()`、`vmalloc()` 分配都是以字節為單位，所分配到的內存在物理內存中連續且保持原有的數據不清零。

{% highlight text %}
#include <linux/slab.h>
void * kmalloc(size_t size, gfp_t flags);
  返回內存塊的指針，其內存塊大小至少為size，分配內存在物理內存中連續，數據不清零
size  : 申請內存大小。
flags : 取值說明。
  GFP_USER  : 用戶空間的分配內存，可能休眠；
  GFP_KERNEL: 內核空間的內存分配，當空閒內存較少時可能休眠等待一個頁面，休眠時內核會用適當的動作獲取空閒頁；
  GFP_ATOMIC: 原子性的內存分配，內核會預留部分空閒頁，不會休眠，如中斷處理程序、軟中斷、tasklet等；
  __GFP_DMA : 要求分配可用於DMA的內存。
{% endhighlight %}

`kmalloc()` 最終調用 `__get_free_pages()` 來分配內存，故前綴都是 `GFP_` 開頭，最多隻能分配 32 個 page 大小的內存，也就是 `32*page=32*4K=128K` 大小，其中 16 個字節用來記錄頁描述結構。

<!-- kmalloc分配的是常駐內存，不會被交換到文件中。最小分配單位是32或64字節。-->

`kzalloc()` 等價於先用 `kmalloc()` 申請空間，再初始化，所有申請的元素都被初始化為 0 。

{% highlight c %}
static inline void *kzalloc(size_t size, gfp_t flags)
{
    return kmalloc(size, flags | __GFP_ZERO);  // 通過標誌位表示初始化元素為0
}
{% endhighlight %}

`vmalloc()` 是一個基本的 Linux 內存分配機制，在虛擬內存空間分配一塊連續的內存區，儘管這些頁在物理內存中不連續 (使用單獨的 `alloc_page()` 來獲得每個頁) ，但內核認為它們地址是連續的，也就是說所分配的內存是邏輯上連續的。

另外，需要注意的是，在內核中，`vmalloc()` 在大部分情況下不推薦使用，因為在某些體系上留給 `vmalloc()` 的地址空間相對小，且效率不高，默認是可以休眠。

簡言之，`vmalloc()` 返回一個指向內存塊的指針，其內存塊大小至少為 size，函數允許休眠。

{% highlight text %}
#include <linux/vmalloc.h>
void * vmalloc(unsigned long size);
void vfree(void * addr);
{% endhighlight %}

`vmalloc()` 使用虛擬地址，每次分配都要通過適當地設置頁表來建立 (虛擬) 內存區域，其分配範圍是 `VMALLOC_START~VAMLLOC_END` 。

<!--
vmalloc分配的地址只在處理器有MMU之上才有意義。當驅動需要真正的物理地址時，就不能使用vmalloc。調用vmalloc的正確場合是分配一個大的、只存在於軟件中的、用於緩存的內存區域時。注意：vamlloc比__get_free_pages要更多開銷，因為它必須即獲取內存又建立頁表。因此，調用vmalloc 來分配僅僅一頁是不值得的。vmalloc的一個小的缺點在於它無法在原子上下文中使用。因為它內部使用kmalloc(GFP_KERNEL) 來獲取頁表的存儲空間，因此可能休眠。vmalloc是面向頁的 (它們會修改頁表) ，重定位的或分配的空間都會被上調到最近的頁邊界。
-->



<!--
kmalloc     內核空間    物理地址連續    最大值128K-16   kfree   性能更佳
vmalloc     內核空間    虛擬地址連續    更大            vfree   更易分配大內存
malloc      用戶空間    虛擬地址連續    更大            free
-->

### SLAB

內核的驅動程序常常需要反覆分配許多相同大小內存塊，為此增加了一些特殊的內存池，稱為高速緩存。而 Linux 內核的高速緩存也被稱為 "SLAB 分配器"，對應了 `kmem_cache_t` 類型，相關函數有：

{% highlight text %}
----- 創建可容納任意數目內存區域的、大小都相同的高速緩存對象
kmem_cache_t *kmem_cache_create(const char *name, size_t size,size_t offset, unsigned long flags,
    void (*constructor)(void *, kmem_cache_t *,unsigned long flags),
    void (*destructor)(void *, kmem_cache_t *, unsigned long flags));

----- 調用如下函數從已創建的後備高速緩存中分配對象，flags和kmalloc的相同
void *kmem_cache_alloc(kmem_cache_t *cache, int flags);

----- 使用如下函數釋放一個對象
void kmem_cache_free(kmem_cache_t *cache, const void *obj);

----- 當驅動用完這個高速緩存時，一般在當模塊被卸載時釋放緩存
int kmem_cache_destroy(kmem_cache_t *cache);
{% endhighlight %}

slab 分配器的特點有：

* 對於頻繁地分配和釋放的數據結構，會緩存；
* 為了避免由於頻繁分配和回收導致內存碎片，會通過空閒鏈表進行緩存；
* 部分緩存專屬單個處理器，分配和釋放操作可以不加 SMP 鎖；

slab 層把不同的對象劃分為高速緩存組，每個高速緩存組都存放不同類型的對象，每個對象類型對應一個高速緩存，每個高速緩存都是用 `kmem_cache` 結構來表示。

<!--
kmem_cache_crreate：創建高速緩存
kmem_cache_destroy: 撤銷高速緩存
kmem_cache_alloc: 從高速緩存中返回一個指向對象的指針
kmem_cache_free：釋放一個對象
-->

例如在內核初始化時，通過 `fork_init()` 中會創建一個名為 `struct task_struct` 的高速緩存，每當進程調用 `fork()` 時，會通過 `dup_task_struct()` 創建一個新的進程描述符，並調用 `do_fork()`，完成從高速緩存中獲取對象。


<!--
1.6 棧的靜態分配

當設置單頁內核棧，那麼每個進程的內核棧只有一頁大小，這取決於編譯時配置選項。 好處：

    可以減少每個進程內存的消耗；
    隨著機器運行時間的增加，尋找兩個未分配的、連續的頁越來越困難，物理內存碎片化不斷加重，那麼給每個新進程分配虛擬內存的壓力也增大；
    每個進程的調用鏈在自己的內核棧中，當單頁棧選項被激活時，中斷處理程序可獲得自己的棧；

任意函數必須儘量節省棧資源， 方法就是所有函數讓局部變量所佔空間之和不要超過幾百字節。
1.7 高端內存的映射

高端內存中的頁不能永久地映射到內核地址空間。

    kmap：把給定page結構映射到內核地址空間；
        當page位於低端內存，函數返回該頁的虛擬地址
        當page位於高端內存，建立一個永久映射，再返回地址
    kunmap: 永久映射的數量有限，應通過kunmap及時解除映射
    kmap_atomic: 臨時映射
    kunmap_atomic: 解除臨時映射

1.8 每個CPU數據

    alloc_percpu: 給系統的每個處理器分配一個指定類型對象的實例，以單字節對齊；
    free_percpu: 釋放每個處理器的對象實例；
    get_cpu_var: 返回一個執行當前處理器數據的特殊實例，同時會禁止內核搶佔
    put_cpu_var: 會重新激活內核搶佔

使用每個CPU數據好處：

    減少了數據鎖定，每個CPU訪問自己CPU數據
    大大減少緩存失效，失效往往發生在一個處理器操作某個數據，而其他處理器緩存了該數據，那麼必須清理或刷新緩存。持續不斷的緩存失效稱為緩存抖動。

1.9 小結

分配函數選擇：

    連續的物理頁，使用低級頁分配器 或kmalloc();
    高端內存分配，使用alloc_pages(),返回page結構指針； 想獲取地址指針，應使用kmap(),把高端內存映射到內核的邏輯地址空間；
    僅僅需要虛擬地址連續頁，使用vmalloc()，性能有所損失；
    頻繁創建和撤銷大量數據結構，考慮建立slab高速緩存。
-->




<!--
為了確保在內存分配不允許失敗情況下成功分配內存，內核提供了稱為內存池( "mempool" )的抽象，它其實是某種後備高速緩存。它為了緊急情況下的使用。所以使用時必須注意：mempool會分配一些內存塊，使其空閒而不真正使用，所以容易消耗大量內存。而且不要使用mempool處理可失敗的分配。應避免在驅動代碼中使用mempool。對應的函數有：
mempool_t *mempool_create(int min_nr,mempool_alloc_t *alloc_fn,mempool_free_t *free_fn,void *pool_data);
/*min_nr 參數是內存池應當一直保留的最小數量的分配對象，實際的分配和釋放對象由alloc_fn和free_fn處理*/
typedef void *(mempool_alloc_t)(int gfp_mask, void *pool_data);
typedef void (mempool_free_t)(void *element, void *pool_data);
/*給 mempool_create 最後的參數 *pool_data 被傳遞給 alloc_fn 和 free_fn */
      創建內存池後，分配和釋放對象：
void *mempool_alloc(mempool_t *pool, int gfp_mask);
void mempool_free(void *element, mempool_t *pool);
     若不再需要內存池，則返回給系統：
void mempool_destroy(mempool_t *pool);
/*在銷燬 mempool 之前，必須返回所有分配的對象，否則會產生 oops*/
-->


### 內存模型

在 Linux 內核中支持 3 種不同的內存模型：Flat Memory Model、Discontiguous Memory Model 和 Sparse Memory Model；所
謂的 Memory Model 其實就是從 CPU 的角度看，其物理內存的分佈情況。

<!--
Linux內存模型
http://www.wowotech.net/memory_management/memory_model.html
-->

{% highlight c %}
#define VMEMMAP_START    _AC(0xffffea0000000000, UL)
#define vmemmap ((struct page *)VMEMMAP_START)
#define __pfn_to_page(pfn)  (vmemmap + (pfn))
#define __page_to_pfn(page) (unsigned long)((page) - vmemmap)
{% endhighlight %}

<!--
linux內核研究筆記(一）內存管理 – page介紹
http://blogread.cn/it/article/6619
-->

## kswapd

該內核守護進程，通過 ```module_init(kswapd_init)@mm/vmscan.c``` 初始化，然後通過 ```kthread_run()``` 啟動 ```kswapd()``` 。該線程會維護一個 LRU 隊列，會將最近沒有被訪問過的 (PG_referenced) 的頁放入到 inactive 隊列中；否則放置到 active 隊列中。

當系統中可用內存很少時，內核線程 kswapd 被喚醒，開始回收釋放 page，通過 `pages_min`、`pages_low`、`pages_high` 這些閾值確定 Zone 的內存使用壓力狀態；注意，最新內核中這三個變量變成了 `watermark[]` 的成員，分別對應於 `WMARK_MIN`，`WMARK_LOW` 和 `WMARK_HIGH`。

![memory watermark]({{ site.url }}/images/linux/kernel/memory-watermark.png "memory watermark"){: .pull-center width="70%" }

各個狀態如下：

* page_min：如果空閒頁數目小於該值，則該zone非常缺頁，頁面回收壓力很大。
* page_low: 如果空閒頁數目小於該值，kswapd線程將被喚醒，並開始釋放回收頁面。
* page_high: 如果空閒頁面的值大於該值，則該zone的狀態很完美, kswapd線程將重新休眠。

內核在計算上述的幾個值之前會首先計算一個關鍵參數 `min_free_kbytes`，它是為關鍵性分配保留的內存空間的最小值。

<!--
該關鍵參數有一個約束：不能小於128k，不能大於64M。其計算公式：

lowmem_kbytes = nr_free_buffer_pages() * (PAGE_SIZE >> 10);
min_free_kbytes = int_sqrt(lowmem_kbytes * 16);

閾值的計算由init_per_zone_pages_min（ 最新內核中是init_per_zone_wmark_min）完成。該函數還會完成每個zone的lowmem_reserve的計算，該數組用於為不能失敗的關鍵分配預留的內存頁。
-->







## 參考

<!--
Linux內核中內存相關的操作函數-1
http://blog.chinaunix.net/uid-27123777-id-3279787.html

Linux內核內存管理學習之一（基本概念，分頁及初始化）
http://blog.csdn.net/goodluckwhh/article/details/9970845


http://www.tldp.org/LDP/khg/HyperNews/get/memory/linuxmm.html

-->

關於 Linux 內存介紹可以查看 [Where is the memory going?](https://www.halobates.de/memory.pdf) 以及 [相關 PDF](https://www.halobates.de/memorywaste.pdf) 。

另外，關於 Linux 內核中內存相關的介紹可以查看 [Page Cache, the Affair Between Memory and Files](http://duartes.org/gustavo/blog/post/page-cache-the-affair-between-memory-and-files/) 等一系列的文章。


{% highlight text %}
{% endhighlight %}
