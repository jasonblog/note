# 記憶體

在 Kernel 內管理記憶體的方式與一般應用程式不同，所以驅動程式需要取用記憶體時，必須要知道 kernel 管理記憶體的方式及其特性。 
本章會解說 kernel 記憶體的用法，以及驅動程式經常用到的 DMA 功能。 



###8-1、Linux kernel 的記憶體管理 
最近的 OS 都以「paging」的方式來管理記憶體， Linux 也是其中之一。 
Paging 是利用 CPU 與 MMU(Memory Management Unit) 的硬體機制實現的。 
Paging 可以讓作業系統實現下列功能：

```c
Virtual Memory (虛擬記憶體)
Swap (置換檔)
Demand Paging (按需求分頁；用到時才載入資料)
Copy on Write (只有資料有更動時才實際複製)
Shared Memory (共享資料)
mmap (將磁碟內容映射到記憶體內)
```

特別是 virtual memory 可以讓 kernel 與一般應用程式在虛擬位址空間內執行。 
虛擬位址轉成物理位址(實體位址) 的工作由硬體完成，轉換速度夠快。 

`Paging 指的是把物理記憶體切割成長度固定的「page」，並以此作為管理單位。 `

一個 page 的大小隨著硬體架構而不同， IA-32 的 page size 是 4 KB。 
物理記憶體固定以 page size 當成管理單位，也就是說，就算只要求分配 1 byte 記憶體，還是會分配到最小單位，也就是 1 page。 
系統啟動時，OS 會從 BIOS 得知系統安裝了多少物理記憶體。 
OS 以 page 為管理物理記憶體的單位，實作了「buddy system(buddy allocator)」。 
而 Buddy system 當然就是以 page size 為單位管理物理記憶體，以 2 的冪次方控制單位數量。 
例如，當 OS 內的 module 要求 128 bytes 記憶體時，會回傳 1 page 的記憶體，如果要求 4097 bytes 記憶體時，就會回傳 2 pages 記憶體，因為是以 page 當成管理單位，所以有著出現無謂浪費的缺點。 
在分配 2 pages 以上的記憶體時，得到的物理記憶體空間會是連續的，適合拿來進行 DMA 傳送。 

Buddy system 的空間使用狀態可以透過以下指令查詢： 
```c
cat /proc/buddyinfo 
```

Buddy system 因為只能以 page size 當成單位，所以在驅動程式需備記憶體時，是個不怎麼好用的介面，因此在 buddy system 上層，還準備了「Slab allocator」介面。 

slab allocator 是以稱為「cache」的單位來管理記憶體，slab allocator 是比較容易使用的介面，驅動程式常用的 kmalloc() 及 kfree() 就是它所提供的。 

###8-2、Stack 
Stack 是存放暫時性資料的記憶體空間，這邊會說明 kernel 的 stack 管理機制。 

####8-2.1、Kernel Stack 
一般應用程式使用區域變數時，會在 user space 的 stack 範單取用記憶體，而驅動程式也是一樣，使用區域變數、管理返回位址的時候，就需要用到 kernel 內的 stack 範圍。 
kernel stack 是透過 thread_info 結構管理的，緊鄰分配於 thread_info 結構(include/linux/sched.h)： 

```c
union thread_union {
    struct thread_info thread_info
    unsigned long stack[THREAD_SIZE/sizeof(long)];
};
```

THREAD_SIZE 是 thread_info 結構與 stack 合在一起的大小，在 IA-32 環境下預設是「8KB」。 
kernel stack 是從記憶體的高位址側往低位址側使用的，kernel 內部如果發生 stack overflow 的話，就會破壞 process 結構。 
Stack 大小設為 8KB 乍看之下或許很小，但其實已經十分足夠，一般來說 kernel 之內的程式碼，為了抑制記憶體使用量，會極力避免使用 stack 才對。 

user process 呼叫 system call，讓驅動程式在 user context 運作的時候，就會使用 user process 定義的 kernel stack。 
那 interrupt context 又是怎樣的情況呢？發生中斷時，驅動程式所在的 interrupt context 沒有對應的 user process，這時中斷處理程序會拿發生中斷時「正在執行」的 user process 擁有的 kernel stack 來用。 
呼叫函式會使用 kernel stack 空間，因此了函式都是 reentrant(可重進入) 的。 


#### 8-2.2、4K Stacks 
8KB 的 kernel stack 有個問題：
- 8192 bytes 的大小無法放入 1 page

所以 kernel 配直選項能讓它變成 4096 bytes，剛好能放進一個 page，稱為「4K Stacks」機制：
- Kernel hacking -> Use 4Kb for kernel stacks instead of 8Kb


在開發驅動程式時，為了避免因為 stack overflow 而造成 kernel panic，必須特別注意：
- 盡量不要使用區域變數
- 不要套疊呼叫太多層函式


### 8-3、全域變數 
驅動程式可以一如往常地宣告、使用全域變數，全域變數放在 RAM 之內，只要不加上 const 就可自由讀寫，需注意的有：
- 全域變數會在整個驅動程式之內共享
- 對編譯成 module 的驅動程式而言，其它 module 看不到它的全域變數；但可用 EXPORT_SYMBOL 對外公開
- 對靜態連結到 kernel 的驅動程式而言，全域變數是整個 kernel 都看得到的

以下面這個驅動程式為例，假設它是 kernel module 類型的驅動程式： 

```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

int g_sample_counter __attribute__ ((unused));
const int g_sample_counter2 __attribute__ ((unused));
static int g_sample_counter3 __attribute__ ((unused));
int g_sample_counter4 __attribute__ ((unused));
EXPORT_SYMBOL(g_sample_counter4);

static int sample_init(void)
{
    printk("sample driver installed.\n");

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

定義了四個全域變數，接著來看外部 module 會看到哪些東西。 
因為這些個變數都沒在程式碼內用到，為了不讓 gcc 自動把它們刪除，所以加上「unused」屬性。 

Makefile 如下，因為希望建立 link map，所以在 LDFLAGS 環境變數加上 「-Map」選項： 

```c
LDFLAGS += -Map $(PWD)/sample.map
CFILES = main.c 

obj-m += sample.o
sample-objs := $(CFILES:.c=.o)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

建構驅動程式後，會產生映射檔(sample.map)。 
因為 g_sample_counter3 變數宣告為 static，所以命名範圍限定在 main.c 檔案內，因此配置在「BSS(Block Started by Symbol」區段內的變數只有三個： 

```c
cat sample.map | grep g_sample_counter 
```
建構時產生的「Module.symvers」包含被 EXPORT_SYMBOL 的符號清單，這些符號可被其它 module 取用： 

```c
cat Module.symvers | grep g_sample_counter 
```

最後把驅動程式掛進 kernel，再看看 kernel 的符號表： 

```c
cat /proc/kallsyms | grep sample 
```

全域變數是一個驅動程式的 module 內共用的，如果在許多不同的 context 之內存取的話，就必須作好鎖定才行。 


###8-4、動態取得記憶體 
linux kernel 提供了許多種不同的記憶體管理機制，所以在取用記憶體時，要先弄清楚它們之間的差異。 


#### 8-4.1、kmalloc 
取用記憶體時，驅動程式最常用的方式應該是 kmalloc()，定義在 linux/slab.h 內，但不必特地 include。 

```c
void *kmalloc(size_t size, gfp_t flags); 
```

kmalloc() 會取得 size bytes 的 kernel 記憶體，適合用來取得 128 KB 左右的小 buffer。 
取用記憶體失敗時，會傳回 NULL 指標，所以一定要檢查傳回值。 
記憶體用完後，一定要呼叫 kfree() 釋放，否則會在 kernel 內引發 memory leak，驅動程式的 memory leak 並不會在卸除驅動程式後由 kernel 代為釋放。 

```c
void *kfree(const void *block); 
```

使用 kmalloc() 的範例： 


```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

void *memptr;

static int sample_init(void)
{
    void *ptr;

    printk("sample driver installed.\n");

    memptr = ptr = kmalloc(2, GFP_KERNEL);
    printk("ptr %p\n", ptr);

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");

    kfree(memptr);
}

module_init(sample_init);
module_exit(sample_exit);
```

####8-4.2、直接從 Cache 取用記憶體 
kmalloc() 與 kfree() 都是以「slab allocator」這個 cache 機制實作出來的，這邊的「cache」指的是「一塊記憶體」的意思。 
slab allocator 的下層是之前提過的 buddy system 記憶體管理機制，slab allocator 利用 buddy system 提供更靈活的記憶體管理功能。

####8-4.3、直接從 Cache 取用記憶體 
若要直接從 cache 取得記憶體，首先要以 kmem_cache_create() 建立最初的 cache 容器：

```c
struck kmem_cache *kmem_cache_create(const char *name, size_t size, size_t align, unsigned long flags, void (*ctor)(void *, struct kmem_cache *, unsigned long)); 
```

引數 name 指的是在 /proc/slabinfo 或 /sys/slab 中讓其它人看到的名稱。 
引數 size 指的是想取用的記憶體量(bytes)。 
其它的引數不常用到，填 0 或 NULL 都無所謂。 
呼叫 kmem_cache_create() 後，並不會實際配置記憶體，這個 cache 對 OS 來說還是「空」的，若呼叫失敗時，則會傳回 NULL值。 
不用要用到後，需須叫 kmem_cache_destroy() 將之釋放： 

```c
void *kmem_cache_alloc(struct kmem_cache *cachep, gfp_t flags); 
```

如果要在 cache 內配置記憶體，可以呼叫 kmem_cache_alloc()： 
```c
void *kmem_cache_alloc(struct kmem_cache *cachep, gfp_t flags); 
```

函式呼叫成功的話，代表成功配置到 kmem_cache_create() 要求的記憶體空間，並傳回指向這塊記憶體開頭的指標，若呼叫失敗時，則會傳回 NULL 。 
要釋放記憶體，可以呼叫 kmem_cache_free() ： 

```c
void kmem_cache_free(struct kmem_cache *cachep, void *disp); 
```

使用 kmem_cache_alloc() 的範例： 
略 


#### 8-4.4、Buddy System 


Linux 記憶體管理的最低層是「buddy system」，這是 slab allocator 的下層。 
驅動程式在需要取用剛好等於 page size 的記憶體時，可以直接使用這個介面。 
唯 buddy system 只能以 page size 為單位管理記憶體，並以 order 作為 page 的計量單位，取得 2 的 order 次方個 page。 
要從 buddy system 取用記憶體時，是呼叫 __get_free_pages() ： 

```c
unsigned long __get_free_pages(gfp_t gfp_mask, unsigned int order); 
```

order 引數是要取用的數量，若 order 為 3，則會取用 2 的 3 次方，也就是 8 pages 的記憶體，page size 隨系統架構而不同。 
函式呼叫成功的話，會傳回記憶體指標，傳回值的型別是 unsigned long，必須明確轉成「void *」或「char *」之類的型別。 
釋放記憶體時，則是呼叫 free_pages() ： 

```c
void free_pages(unsigned long addr, unsigned int order); 
```

使用 __get_free_pages() 的範例： 
略 


#### 8-4.5、vmalloc 
slab allocator 保證取到的記憶體範圍是連續的物理記憶體，可以用來進行 DMA 傳送工作。 
但在某些情況下，並不需要連續的物理記憶體，只要 kernel 虛擬記憶體空間內的位址連續就好，此時可以使用 vmalloc() 及 vfree() 函式： 

```c
void *vmalloc(unsigned long size); 
void vfree(void *addr) 
```


### 8-5、檢查 Slab Allocator 的洩漏情形 
略 



###8-6、kmalloc 檢查偵測 patch 
略 

### 8-7、DMA 
驅動程式在裝置與記憶體之間轉移資料時，經常用到 DMA(Direct Memory Access) 的機制，它代表：
`在裝置與物理記憶體之間傳送資料時，不必 CPU 插手`
的機制。 
也就是說，只要用了 DMA， CPU 就可以去做其它的事，可以提昇系統效能。 

####8-7.1、DMA Buffer 的限制 
因為裝置需要直接讀寫系統記憶體，所以一般來說 DMA Buffer 的限制有：
- DMA buffer 的記憶體範圍必須是連續的
- DMA buffer 的物理開頭位址必須位於 page 的邊界起點
- DMA buffer 的記憶體空間必須在開頭 4GB 之內
- DMA buffer 的記憶體不能被 swap


####8-7.2、取得 DMA Buffer 的 Kernel 函式 
略 


###8-8、快取一致性 
執行 DMA 傳送工作時，有時會遇到「cache coherency(快取一致性)」的問題。 <br>
一般來說，電腦架構為了提高記憶體讀寫效率，會利用 CPU 內的 L1/L2 cache，所以驅動程式對記憶體寫入資料時，不見得會立刻反應到記憶體內容上，而是會放在 CUP 的 cache 裡，等到適當時機才寫入記憶體。 
相同的情況下，由裝置寫入資料到記憶體時，CPU 的 cache 也未必能及時更新，而可能導致驅動程式讀取 cache 時，讀到舊的資料。 


linux kernel 為了應用這種問題，提供了控制 cache 的函式： 

```c
void dma_cache_sync(struct device *dev, void *vaddr, size_t size, enum 
dma_data_direction direction); 
```

引數 vaddr 是 DMA buffer 的指標(kernel 虛擬位址)。<br>
引數 size 是想 flush/purge 的 DMA buffer 大小。<br>
dma_data_direction 是列舉常數，指定 DMA_TO_DEVICE 代表「要把資料從記憶體傳給裝置」，也就是執行 flush cache 的動作。 
指定 DMA_FROM_DEVICE 代表「要讀取裝置傳到記憶體的資料」，也就是執行 purge cache 的動作。 

IA-32 在硬體層及保証 cache coherency，所以驅動程式不必特別控制 cache，但若要移植到其它平臺時，則需特別注意。 



8-9、結語 
開發驅動程式要應對各種不同的記憶體管理機制，依用途選最適合的方式。 
而 DMA buffer 有諸多限制，開發時需注意。
