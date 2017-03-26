# （十一）：內存管理之頁的分配與回收


內存管理單元（MMU）負責將管理內存，在把虛擬地址轉換為物理地址的硬件的時候是按頁為單位進行處理，從虛擬內存的角度來看，頁就是內存管理中的最小單位。頁的大小與體系結構有關，在 x86 結構中一般是4KB（32位）或者8KB（64位）。
通過 getconf 命令可以查看系統的page的大小：


```c
# getconf -a | grep PAGE  
PAGESIZE                           4096  
PAGE_SIZE                          4096  
_AVPHYS_PAGES                      230873  
_PHYS_PAGES                        744957  
```

內核中的每個物理頁用struct  page結構表示，結構定義於文件`<include/linux/mm_types.h>`：


```c
struct page {
    unsigned long flags;        /*頁的狀態*/
    atomic_t _count;        /* 頁引用計數 */
    union {
        atomic_t _mapcount; /* 已經映射到mms的pte的個數*/
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
        struct kmem_cache* slab;    /* 指向slab層 */
        struct page* first_page;    /* Compound tail pages */
    };
    union {
        pgoff_t index;      /* Our offset within mapping. */
        void* freelist;     /* SLUB: freelist req. slab lock */
    };
    struct list_head lru;       /* 將頁關聯起來的鏈表項 */

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

內核使用這一結構來管理系統中所有的頁，因為內核需要知道一個該頁是否被分配，是被誰擁有的等信息。

由於ISA總線的DMA處理器有嚴格的限制，只能對物理內存前16M尋址，內核線性地址空間只有1G,CPU不能直接訪問所有的物理內存。這樣就導致有一些內存不能永久地映射在內核空間上。所以在linux中，把頁分為不同的區，使用區來對具有相似特性的頁進行分組。分組如下（以x86-32為例）：

ZONE_DMA 小於16M內存頁框，這個區包含的頁用來執行DMA操作。

ZONE_NORMAL 16M~896M內存頁框，個區包含的都是能正常映射的頁。

ZONE_HIGHMEM 大於896M內存頁框，這個區包"高端內存"，其中的頁能不永久地映射到內核地址空間。

linux 把系統的頁劃分區，形成不同的內存池，這樣就可以根據用途進行分配了。

每個區都用struct zone表示，定義於<include/linux/mmzone.h>中。該結構體較大，詳細結構體信息可以查看源碼文件。

linux提供了幾個以頁為單位分配釋放內存的接口，定義於<include/linux/gfp.h>中。分配內存主要有以下方法：

```c
alloc_page(gfp_mask)	只分配一頁，返回指向頁結構的指針
alloc_pages(gfp_mask, order)	分配 2^order 個頁，返回指向第一頁頁結構的指針
__get_free_page(gfp_mask)	只分配一頁，返回指向其邏輯地址的指針
__get_free_pages(gfp_mask, order)	分配 2^order 個頁，返回指向第一頁邏輯地址的指針
get_zeroed_page(gfp_mask)	只分配一頁，讓其內容填充為0，返回指向其邏輯地址的指針
```


alloc_*函數返回的是內存的`物理地址`，
get_* 函數返回內存物理地址映射後的`邏輯地址`。
如果無須直接操作物理頁結構體的話，一般使用 get_*函數。

`釋放頁`的函數有：

```c
externvoid__free_pages( struct page *page, unsignedintorder); 
externvoidfree_pages(unsigned longaddr, unsigned intorder); 
externvoidfree_hot_page( struct page *page);
```


當需要以頁為單位的連續物理頁時，可以使用上面這些分配頁的函數，對於常用以字節為單位的分配來說，內核提供來kmalloc()函數。

kmalloc()函數和用戶空間一族函數類似，它可以以字節為單位分配內存，對於大多數內核分配來說，kmalloc函數用得更多。


- 行為標誌 ：控制分配內存時，分配器的一些行為，如何分配所需內存。
- 區標誌   ：控制內存分配在那個區(ZONE_DMA, ZONE_NORMAL, ZONE_HIGHMEM 之類)。
- 類型標誌 ：由上面2種標誌組合而成的一些常用的場景。

行為標誌主要有以下幾種：

```sh
行為標誌     描述
__GFP_WAIT	分配器可以睡眠
__GFP_HIGH	分配器可以訪問緊急事件緩衝池
__GFP_IO	分配器可以啟動磁盤I/O
__GFP_FS	分配器可以啟動文件系統I/O
__GFP_COLD	分配器應該使用高速緩存中快要淘汰出去的頁
__GFP_NOWARN	分配器將不打印失敗警告
__GFP_REPEAT	分配器在分配失敗時重複進行分配，但是這次分配還存在失敗的可能
__GFP_NOFALL	分配器將無限的重複進行分配。分配不能失敗
__GFP_NORETRY	分配器在分配失敗時不會重新分配
__GFP_NO_GROW	由slab層內部使用
__GFP_COMP	添加混合頁元數據，在 hugetlb 的代碼內部使用
```

標誌主要有以下3種：

```sh
區標誌        描述
__GFP_DMA	從 ZONE_DMA 分配
__GFP_DMA32	只在 ZONE_DMA32 分配 ，和 ZONE_DMA 類似，該區包含的頁也可以進行DMA操作
__GFP_HIGHMEM	從 ZONE_HIGHMEM 或者 ZONE_NORMAL 分配，優先從 ZONE_HIGHMEM 分配，如果 ZONE_HIGHMEM 沒有多餘的頁則從 ZONE_NORMAL 分配
```
類型標誌是編程中最常用的，在使用標誌時，應首先看看類型標誌中是否有合適的，如果沒有，再去自己組合 行為標誌和區標誌。


```sh
類型標誌      描述                                                         實際標誌
GFP_ATOMIC	 這個標誌用在中斷處理程序，下半部，持有自旋鎖以及其他不能睡眠的地方	__GFP_HIGH
GFP_NOWAIT	與 GFP_ATOMIC 類似，不同之處在於，調用不會退給緊急內存池。
這就增加了內存分配失敗的可能性	0
GFP_NOIO	這種分配可以阻塞，但不會啟動磁盤I/O。
這個標誌在不能引發更多磁盤I/O時能阻塞I/O代碼，可能會導致遞歸	__GFP_WAIT
GFP_NOFS	這種分配在必要時可能阻塞，也可能啟動磁盤I/O，但不會啟動文件系統操作。
這個標誌在你不能再啟動另一個文件系統的操作時，用在文件系統部分的代碼中	(__GFP_WAIT
｜ __GFP_IO)  
GFP_KERNEL	這是常規的分配方式，可能會阻塞。這個標誌在睡眠安全時用在進程上下文代碼中。
為了獲得調用者所需的內存，內核會盡力而為。這個標誌應當為首選標誌	(__GFP_WAIT
｜ __GFP_IO 
｜ __GFP_FS ) 
GFP_USER	這是常規的分配方式，可能會阻塞。用於為用戶空間進程分配內存時	(__GFP_WAIT
｜ __GFP_IO 
｜ __GFP_FS ) 
GFP_HIGHUSER	從 ZONE_HIGHMEM 進行分配，可能會阻塞。用於為用戶空間進程分配內存	(__GFP_WAIT
｜ __GFP_IO 
｜ __GFP_FS )
｜__GFP_HIGHMEM) 
GFP_DMA	從 ZONE_DMA 進行分配。需要獲取能供DMA使用的內存的設備驅動程序使用這個標誌
通常與以上的某個標誌組合在一起使用。
```

```sh
以上各種類型標誌的使用場景總結：
場景                         相應標誌
進程上下文，可以睡眠	使用 GFP_KERNEL
進程上下文，不可以睡眠	使用 GFP_ATOMIC，在睡眠之前或之後以 GFP_KERNEL 執行內存分配
中斷處理程序	使用 GFP_ATOMIC
軟中斷	使用 GFP_ATOMIC
tasklet	使用 GFP_ATOMIC
需要用於DMA的內存，可以睡眠	使用 (GFP_DMA｜GFP_KERNEL)
需要用於DMA的內存，不可以睡眠	使用 (GFP_DMA｜GFP_ATOMIC)，或者在睡眠之前執行內存分配
```

kmalloc  所對應的釋放內存的方法為：


```c
void kfree(const void *)；
vmalloc()也可以按字節來分配內存。
void *vmalloc(unsigned long size)
```

和kmalloc是一樣的作用，不同在於前者分配的內存虛擬地址是連續的，而物理地址則無需連續。kmalloc()可以保證在物理地址上都是連續的，虛擬地址當然也是連續的。vmalloc()函數只確保頁在虛擬機地址空間內是連續的。它通過分配非聯繫的物理內存塊，再“修正”頁表，把內存映射到邏輯地址空間的連續區域中，就能做到這點。但很顯然這樣會降低處理性能，因為內核不得不做“拼接”的工作。所以這也是為什麼不得已才使用 vmalloc()的原因 。 vmalloc()可能睡眠，不能從中斷上下文中進行調用，也不能從其他不允許阻塞的情況下進行調用。釋放時必須使用vfree()。

```c
void vfree(const void *)；
```

對於內存頁面的管理，通常是先在虛存空間中分配一個虛存區間，然後才根據需要為此區間分配相應的物理頁面並建立起映射，也就是說，虛存區間的分配在前，而物理頁面的分配在後。但由於頻繁的請求和釋放不同大小的連續頁框，必然導致在已分配頁框的塊內分散了許多小塊的空閒頁框，由此產生的問題是：即使有足夠的空閒頁框可以滿足請求，但當要分配一個大塊的連續頁框時，無法滿足請求。這就是著名的內存管理問題：外碎片問題。Linux採用著名的夥伴（Buddy）系統算法來解決外碎片問題。


 把所有的空閒頁框分組為11個塊鏈表。每個塊鏈表包含大小為1,2,4,8,16,32,64,128,256,512，1024個的頁框。夥伴系統算法原理為：
 
 假設請求一個256個頁框的塊，先在256個頁框的鏈表內檢查是否有一個空閒的塊。如果沒有這樣的塊，算法會查找下一個更大的塊，在512個頁框的鏈表中找一個空閒塊。如果存在這樣的塊，內核就把512的頁框分成兩半，一半用作滿足請求，另一半插入256個頁框的鏈表中。如果512個頁框的塊鏈表也沒有空閒塊，就繼續找更大的塊，1024個頁框的塊。如果這樣的塊存在，內核把1024個頁框的256個頁框用作請求，然後從剩餘的768箇中拿出512個插入512個頁框的鏈表中，把最後256個插入256個頁框的鏈表中。
 
 頁框塊的釋放過程如下：
如果兩個塊具有相同的大小：a，並且他們的物理地址連續那麼這兩個塊成為夥伴，內核就會試圖把大小為a的一對空閒夥伴塊合併為一個大小為2a的單獨塊。該算法還是迭代的，如果合併成功的話，它還會試圖合併2a的塊。

管理分區數據結構struct zone_struct中，涉及到空閒區數據結構。

```c
struct free_area    free_area[MAX_ORDER];

struct free_area {
    struct list_head    free_list[MIGRATE_TYPES];
    unsigned long        nr_free;
};
```

採用夥伴算法分配內存時，每次至少分配一個頁面。但當請求分配的內存大小為幾十個字節或幾百個字節時應該如何處理？如何在一個頁面中分配小的內存區，小內存區的分配所產生的內碎片又如何解決？slab的分配模式可以解決該問題，下一節我們將開始分析slab分配器。
