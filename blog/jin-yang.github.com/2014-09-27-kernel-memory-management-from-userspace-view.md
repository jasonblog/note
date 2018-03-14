---
title: 內存-用戶空間
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,內存,用戶空間
description: 簡單介紹下 Linux 中用戶空間的內存管理，包括了內存的佈局、內存申請等操作。
---

簡單介紹下 Linux 中用戶空間的內存管理，包括了內存的佈局、內存申請等操作。

<!-- more -->

## 內存佈局

關於 Linux 的內存分佈可以查看內核文檔 [x86/x86_64/mm.txt]( {{ site.kernel_docs_url }}/Documentation/x86/x86_64/mm.txt)，也就是低 128T 為用戶空間。

{% highlight text %}
0000000000000000 - 00007fffffffffff (=47 bits) user space, different per mm
{% endhighlight %}

在內核中通過 ```TASK_SIZE_MAX``` 宏定義，同時還減去了一個頁面的大小做為保護。

{% highlight c %}
#define TASK_SIZE_MAX   ((1UL << 47) - PAGE_SIZE)
{% endhighlight %}

而 ```0xffff,8000,0000,0000``` 以上為系統空間地址；注意：該地址的高 16bits 是 ```0xffff```，這是因為目前實際上只用了 64 位地址中的 48 位，也就是高 16 位沒有使用，而從地址 ```0x0000,7fff,ffff,ffff``` 到 ```0xffff,8000,0000,0000``` 中間是一個巨大的空洞，是為以後的擴展預留的。

而真正的系統空間的起始地址，是從 ```0xffff,8800,0000,0000``` 開始的，參見：

{% highlight c %}
#define __PAGE_OFFSET     _AC(0xffff,8800,0000,0000, UL)
{% endhighlight %}

而 32 位地址時系統空間的起始地址為 ```0xC000,0000``` 。

{% highlight text %}
ffff800000000000 - ffff87ffffffffff (=43 bits) guard hole, reserved for hypervisor
ffff880000000000 - ffffc7ffffffffff (=64 TB) direct mapping of all phys. memory
ffffc80000000000 - ffffc8ffffffffff (=40 bits) hole
ffffc90000000000 - ffffe8ffffffffff (=45 bits) vmalloc/ioremap space
{% endhighlight %}

<!--
而 32 位地址空間時，當物理內存大於896M時（Linux2.4內核是896M，3.x內核是884M，是個經驗值），由於地址空間的限制，內核只會將0~896M的地址進行映射，而896M以上的空間用做一些固定映射和vmalloc/ioremap。而64位地址時是將所有物理內存都進行映射。
-->

### 佈局詳解

內核中有兩個參數影響了內存的佈局，如下。

{% highlight text %}
----- 查看當前配置
$ cat /proc/sys/vm/legacy_va_layout
$ cat /proc/sys/kernel/randomize_va_space

----- 配置參數
# sysctl -w vm.legacy_va_layout=0
# sysctl -w kernel.randomize_va_space=2

----- 可以修改參數後通過如下方式測試
$ cat /proc/self/maps
00400000-0040b000 r-xp 00000000 08:07 786680                 /usr/bin/cat
0060b000-0060c000 r--p 0000b000 08:07 786680                 /usr/bin/cat
0060c000-0060d000 rw-p 0000c000 08:07 786680                 /usr/bin/cat
011ed000-0120e000 rw-p 00000000 00:00 0                      [heap]
7f924042c000-7f9246955000 r--p 00000000 08:07 793717         /usr/lib/locale/locale-archive
7f9246955000-7f9246b0c000 r-xp 00000000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246b0c000-7f9246d0b000 ---p 001b7000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246d0b000-7f9246d0f000 r--p 001b6000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246d0f000-7f9246d11000 rw-p 001ba000 08:07 793610         /usr/lib64/libc-2.17.so
7f9246d11000-7f9246d16000 rw-p 00000000 00:00 0
7f9246d16000-7f9246d36000 r-xp 00000000 08:07 793718         /usr/lib64/ld-2.17.so
7f9246f19000-7f9246f1c000 rw-p 00000000 00:00 0
7f9246f34000-7f9246f35000 rw-p 00000000 00:00 0
7f9246f35000-7f9246f36000 r--p 0001f000 08:07 793718         /usr/lib64/ld-2.17.so
7f9246f36000-7f9246f37000 rw-p 00020000 08:07 793718         /usr/lib64/ld-2.17.so
7f9246f37000-7f9246f38000 rw-p 00000000 00:00 0
7ffd158bf000-7ffd158e0000 rw-p 00000000 00:00 0              [stack]
7ffd15945000-7ffd15947000 r-xp 00000000 00:00 0              [vdso]
ffffffffff600000-ffffffffff601000 r-xp 00000000 00:00 0      [vsyscall]
{% endhighlight %}

<!--
TEXT：代碼段，映射程序的二進制代碼， 該區域為私有區域；在代碼段中，也會包含一些只讀的常數變量，比如字符串常量等。

一般來說，IA-32體系結構中進程空間的代碼段從0x08048000開始，這與最低可用地址(0x00000000)有128M的間距，按照linux內核架構一書介紹是為了捕獲NULL指針(具體不詳)。該值是在編譯階段就已經設定好的，其他體系結構也有類似的缺口，比如mips(), ppc() x86-64使用的是0x000000000400000。

DATA：數據段，映射程序中已經初始化的全局變量。

BSS段：存放程序中未初始化的全局變量，在ELF文件中，該區域的變量僅僅是個符號，並不佔用文件空間，但程序加載後，會佔用內存空間，並初始化為0。

HEAP：運行時的堆，在程序運行中使用malloc申請的內存區域。

該區域的起始地址受start_brk影響，和BSS段之間會有一個隨機值的空洞；該區域的內存增長方式是由低地址向高地址增長。

MMAP：共享庫及匿名文件的映射區域；該區域中會包含共享庫的代碼段和數據段。其中代碼段為共享的，但數據段為私有的，寫時拷貝。

該區域起始地址也會有一個隨機值，該區域的增長方式是從高地址向低地址增長（Linux經典佈局中不同，稍後討論）

STACK：用戶進程棧；

該區域起始地址也存在一個隨機值，通過PF_RANDOMIZE來設置。棧的增長方向是由高地址向低地址增長，並且若設置了RLIMIT_STACK即規定了棧的大小。

最後，是命令行參數和環境變量。
-->

#### 新舊佈局

不同之處在於 MMAP 區域的增長方向，新佈局導致了棧空間的固定，而堆區域和 MMAP 區域公用一個空間，這在很大程度上增長了堆區域的大小。

Linux 傳統內存佈局如下。

![memory userspace layout]({{ site.url }}/images/linux/kernel/memory-userspace-layout-lagency.jpg "memory userspace layout"){: .pull-center width="40%" }

現在用戶空間的內存空間佈局如下。

![memory userspace layout]({{ site.url }}/images/linux/kernel/memory-userspace-layout.jpg "memory userspace layout"){: .pull-center width="70%" }

從上圖可以看到，mmap 映射區域至頂向下擴展，mmap 映射區域和堆相對擴展，直至耗盡虛擬地址空間中的剩餘區域，彌補了經典內存佈局方式的不足。

為了使用此新的內存佈局，執行命令 ```sysctl -w vm.legacy_va_layout=0```，然後重新編譯運行程序並查看其輸出及 maps 文件內容。

<!--
多線程內存空間佈局
主線程與第一個線程的棧之間的距離：0xbfbaf648 – 0xb771d384 = 132M
第一個線程與第二個線程的棧之間的距離：0xb771d384 – 0xb6f1d384 = 8M
其它幾個線程的棧之間距離均為8M。
也就是說，主線程的棧空間最大為132M，而普通線程的棧空間僅為8M，超這個範圍就會造成棧溢出（後果很嚴重）。
-->

### 內核相關

利用虛擬內存，每個進程相當於佔用了全部的內存空間，所有和進程相關的信息都保存在內存描述符 (memory descriptor) 中，也就是 ```struct mm_struct *mm```；在進程的進程描述符 ```struct task_struct``` 中的 mm 域記錄該進程使用的內存描述符，也就是說 ```current->mm``` 代表當前進程的內存描述符。

{% highlight c %}
struct mm_struct {
    struct vm_area_struct *mmap;            // 指向內存區域對象，鏈表形式存放，利於高效地遍歷所有元素
    struct rb_root mm_rb;                   // 與mmap表示相同，以紅黑樹形式存放，適合搜索指定元素

    pgd_t                 *pgd;             // 指向頁目錄
    atomic_t               mm_count;        // 主引用計數，為0時結構體會被撤銷
    atomic_t               mm_users;        // 從計數器，代表正在使用該地址的進程數目
    int                    map_count;       // vma的數目

    struct list_head mmlist;                // 所有mm_struct都通過mmlist連接在一個雙向鏈表中，
                                            // 該鏈表的首元素是init_mm內存描述符，代表init進程的地址空間
};
{% endhighlight %}

剩下字段中包含了該進程的代碼段、數據段、堆棧、命令行參數、環境變量的起始地址和終止地址；內存描述符在系統中通過 ```mmlist``` 組織成了一個雙向鏈表，這個鏈表的首元素是 ```init_mm.mmlist``` 。

其中 mm_count 和 mm_users 用來表示是否還有進程在使用該內存，這兩個是比較重要的字段，決定了該進程空間是否仍被使用。

<!--
mm_users 記錄了使用這片內存空間的所有輕量級進程的數量，包括從同一個父進程 fork() 出來的輕量級子進程在 mm_count 中算一個單元。

比如，某個進程空間由兩個擁有同樣父進程的輕量級進程共享，那麼其 mm_users 的值為 2，而 mm_count 的值為 1，當 mm_count 的值遞減為 0 時，該內存描述符 (進程空間) 被回收。
-->

一個進程的內存空間如上圖所示，其中各個內存區域通過 ```vm_area_struct``` 表示，類似如下所示。

![memory_process_vma_lists]({{ site.url }}/images/linux/kernel/memory_process_vma_lists.png  "memory_process_vma_lists"){: .pull-center width="80%" }

用戶進程的虛擬地址空間包含了若干區域，這些區域的分佈方式是特定於體系結構的，不過所有的方式都包含下列成分：

* 可執行文件的二進制代碼，也就是程序的代碼段；
* 存儲全局變量的數據段；
* 用於保存局部變量和實現函數調用的棧；
* 環境變量和命令行參數；
* 程序使用的動態庫的代碼；
* 用於映射文件內容的區域。

內核中的夥伴系統、SLAB 分配器都是儘快響應內核請求，而對於用戶空間的請求略有不同。

用戶空間動態申請內存時，往往只是獲得一塊線性地址的使用權，而並沒有將這塊線性地址區域與實際的物理內存對應上，只有當用戶空間真正操作申請的內存時，才會觸發一次缺頁異常，這時內核才會分配實際的物理內存給用戶空間。

#### 結構體

Linux 內核中，關於虛存管理的最基本的管理單元是虛擬內存區域 (Virtual Memory Areas, vma)，通過 ```struct vm_area_struct``` 表示，它描述了一段連續的、具有相同訪問屬性 (可讀、可寫、可執行等等) 的虛存空間，該虛存空間的大小為物理內存頁面的整數倍。

{% highlight c %}
struct vm_area_struct {   // include/linux/mm_types.h
    struct mm_struct * vm_mm;                   // 反向指向該進程所屬的內存描述符

    unsigned long vm_start, vm_end;             // 虛存空間的首地址，末地址後第一個字節的地址
    struct vm_area_struct *vm_next, *vm_prev;   // 每個進程的VM空間鏈表，按地址排序，用於遍歷
    struct rb_node vm_rb;                       // 紅黑樹中對應的節點，用於快速定位

    pgprot_t vm_page_prot;                      // vma的訪問控制權限
    unsigned long vm_flags;                     // 保護標誌位和屬性標誌位，共享(0)還是獨有(1)

    const struct vm_operations_struct *vm_ops;  // 該vma上的各種標準操作函數指針集
    unsigned long vm_pgoff;                     // 文件映射偏移，以PAGE_SIZE為單位
    struct file * vm_file;                      // 如果是文件映射，則指向文件描述符
    void * vm_private_data;                     // 設備驅動私有數據，與內存管理無關

};
{% endhighlight %}

該結構體描述了 ```[vm_start, vm_end)``` 的內存空間，以字節為單位。通常，進程所使用到的虛存空間不連續，且各部分虛存空間的訪問屬性也可能不同，所以一個進程的虛存空間需要多個 ```vm_area_struct``` 結構來描述。

在 ```vm_area_struct``` 較少時，各個結構體按照升序排序，通過 ```vm_next```、```vm_prev``` 以鏈表的形式組織數據；但當數據較多時，實現了 AVL 樹，以提高 ```vm_area_struct``` 的搜索速度。

<!--
另外，假如該 vm_area_struct 描述的是一個文件映射的虛存空間，成員 vm_file 便指向被映射的文件的 file 結構，vm_pgoff 是該虛存空間起始地址在 vm_file 文件裡面的文件偏移，單位為物理頁面。<br><br>

一個程序可以選擇 MAP_SHARED 或 MAP_PRIVATE 將一個文件的某部分數據映射到自己的虛存空間裡面，這兩種映射方式的區別在於：MAP_SHARED 映射後在內存中對該虛存空間的數據進行修改會影響到其他以同樣方式映射該部分數據的進程，並且該修改還會被寫迴文件裡面去，也就是這些進程實際上是在共用這些數據；而 MAP_PRIVATE 映射後對該虛存空間的數據進行修改不會影響到其他進程，也不會被寫入文件中。<br><br>

來自不同進程，所有映射同一個文件的 vm_area_struct 結構都會根據其共享模式分別組織成兩個鏈表。鏈表的鏈頭分別是：vm_file->f_dentry->d_inode->i_mapping->i_mmap_shared,vm_file->f_dentry->d_inode->i_mapping->i_mmap。而vm_area_struct結構中的vm_next_share指向鏈表中的下一個節點；vm_pprev_share是一個指針的指針，它的值是鏈表中上一個節點（頭節點）結構的vm_next_share（i_mmap_shared或i_mmap）的地址。<br><br>

進程建立 vm_area_struct 結構後，只是說明進程可以訪問這個虛存空間，但有可能還沒有分配相應的物理頁面並建立好頁面映射。在這種情況下，若是進程執行中有指令需要訪問該虛存空間中的內存，便會產生一次缺頁異常。這時候，就需要通過 vm_area_struct 結構裡面的 vm_ops->nopage 所指向的函數來將產生缺頁異常的地址對應的文件數據讀取出來。<br><br>

vm_flags 主要保存了進程對該虛存空間的訪問權限，然後還有一些其他的屬性；vm_page_prot 是新映射的物理頁面的頁表項 pgprot 的默認值。
-->

#### 查看進程內存空間

可以通過 ```cat /proc/<pid>/maps``` 或者 ```pmap <pid>``` 查看。

<!--
每行數據格式： 開始-結束 訪問權限 偏移 主設備號：次設備號 i節點 文件
    設備表示為00：00, 索引節點標示頁為0，這個區域就是零頁（所有數據全為零）
    數據段和bss具有可讀、可寫但不可執行權限；而堆棧可讀、可寫、甚至可執行

2.4 內存區域操作
find_vma 查看mm_struct所屬於的VMA，搜索第一個vm_end大於addr的內存區域
struct vm_area_struct *find_vma(struct mm_struct *mm, usigned long addr)
    檢查mmap_cache，查看緩存VMA是否包含所需地址，如果沒有找到，進入2
    通過紅黑樹搜索；
find_vma_prev 查看mm_struct所屬於的VMA，搜索第一個vm_end小於addr的內存區域

struct vm_area_struct * find_vma_prev(struct mm_struct *mm, unsigned long addr, struct vm_area_struct **pprev)

mmap
    內核使用do_mmap()創建一個新的線性地址區間，如果創建的地址區間和一個已存在的相鄰地址區間有相同的訪問權限，則將兩個區間合併為一個。
    mmap()系統調用獲取內核函數do_mmap()的功能。
    do_mummap()從特定的進程地址空間中刪除指定地址區間
    mummap()與 mmap功能相反。
-->


## 內存申請

在用戶空間中會通過 ```malloc()``` 動態申請內存，而實際上，在用戶空間中對應了不同的實現方式，包括了 ```ptmalloc``` (glibc)、```tcmalloc``` (Google) 以及 ```jemalloc```，接下來簡單介紹這三種內存分配方式。

```ptmalloc``` 的早期版本是由 Doug Lea 實現的，它有一個重要問題就是無法保證線程安全，Wolfram Gloger 改進了其實現從而支持多線程；TCMalloc (Thread-Caching Malloc) 是 google 開發的開源工具 ```google-perftools``` 之一。

<!--
http://www.360doc.com/content/13/0915/09/8363527_314549128.shtml
http://blog.jobbole.com/91887/
http://blog.chinaunix.net/uid-26772535-id-3197173.html
http://blog.csdn.net/jltxgcy/article/details/44150429
http://blog.csdn.net/jltxgcy/article/details/44133307
http://www.cnblogs.com/vinozly/p/5489138.html
-->

### 簡介

從操作系統角度來看，進程分配內存有兩種方式，分別由系統調用 brk() 和 mmap() 完成：

1. brk() 是將數據段 (```.data```) 的最高地址指針 ```_edata``` 往高地址推；
2. mmap() 是在進程的虛擬地址空間中，也就是堆和棧中間 (文件映射區域) 的一塊空閒虛擬內存。

如上所述，這兩種方式分配的都是虛擬內存，沒有分配物理內存，只有在第一次訪問已分配的虛擬地址空間的時候，發生缺頁中斷，操作系統負責分配物理內存，然後建立虛擬內存和物理內存之間的映射關係。

在標準 C 庫中，提供了 ```malloc()``` ```free()``` 分配釋放內存，而這兩個函數的底層是由 ```brk()``` ```mmap()``` ```munmap()``` 這些系統調用實現的。

<!-- do_page_fault() -->

### brk() sbrk()

如下是兩個函數的聲明。

{% highlight c %}
#include <unistd.h>
int brk(void *addr);
void *sbrk(intptr_t increment);
{% endhighlight %}

這兩個函數都用來改變 "program break" 的位置，如下圖所示：

![memory userspace layout]({{ site.url }}/images/linux/kernel/memory-userspace-layout.jpg "memory userspace layout"){: .pull-center width="70%" }

sbrk/brk 是從堆中分配空間，實際上就是移動一個位置，也就是 ```Program Break```，這個位置定義了進程數據段的終止處，增大就是分配空間，減小就是釋放空間。

sbrk 用相對的整數值確定位置，如果這個整數是正數，會從當前位置向後移若干字節，如果為負數就向前若干字節，為 0 時獲取當前位置；而 brk 則使用絕對地址。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    void *p0 = sbrk(0), *p1, *p2;
    printf("P0 %p\n", p0);
    brk(p0 + 4);     // 分配4字節
    p1 = sbrk(0);
    printf("P1 %p\n", p1);
    p2 = sbrk(4);
    printf("P2=%p\n", p2);

    return 0;
}
{% endhighlight %}



## 進程加載

```execve()``` 系統調用可用於加載一個可執行文件並代替當前的進程，它在 ```libc``` 庫中有幾個 API 封裝：```execl()```、```execve()```、```execlp()```、```execvp()```。這幾個函數的功能相同，只是參數不同，在內核中統一調用 ```sys_execve()``` 。

以 ```execve()``` 函數為例，創建一個進程，參考如下。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>

int main(int arg,char **args)
{
    char *argv[]={"ls", "-l", "/tmp", NULL};
    char *envp[]={0, NULL};

    execve("/bin/ls", argv, envp);
    return 0;
}
{% endhighlight %}

系統為進程運行初始化環境，無非就是完成內存分配和映射以及參數和數據段、代碼段和 bss 等的載入，以及對調用 exec 的舊進程進行回收。

其中有兩個可執行文件相關的數據結構，分別是 ```linux_binprm``` 和 ```linux_binfmt```，內核中為可執行程序的裝入定義了一個數據結構 ```linux_binprm```，以便將運行一個可執行文件時所需的信息組織在一起。

而 ```linux_binfmt``` 用表示每一個加載模塊，這個結構在系統中組成了一個鏈表結構。

{% highlight c %}
struct linux_binprm {
    char buf[BINPRM_BUF_SIZE];                // 保存可執行文件的頭部
    struct page *page[MAX_ARG_PAGES];         // 每個參數最大使用一個物理頁來存儲，最大為32個頁面
    struct mm_struct *mm;                     // 暫時存儲新進程的可執行文件名、環境變量等
    unsigned long p;                          // 當前內存的起始地址
    int argc, envc;                           // 參數變量和環境變量的數目
    const char * filename;                    // procps查看到的名稱
};

struct linux_binfmt {
    struct list_head lh;                                 // 用於形成一個列表
    struct module *module;                               // 定義該函數所屬的模塊
    int (*load_binary)(struct linux_binprm *);           // 加載可執行文件
    int (*load_shlib)(struct file *);                    // 加載共享庫
    int (*core_dump)(struct coredump_params *cprm);      // core dump
    unsigned long min_coredump;
};
{% endhighlight %}

程序的加載主要分為兩步：A) 準備階段，將參數讀如內核空間、判斷可執行文件的格式、並選擇相應的加載器；B) 載入階段，完成對新進程代碼段、數據段、BSS 等信息的載入。

### 進程創建

如上所述，實際調用的是內核中的 ```sys_execve()``` 函數，簡單介紹如下。

{% highlight text %}
sys_execve()         @ fs/exec.c
  |-getname()                               將文件名從用戶空間複製到內核空間
  | |-getname_flags()
  |  |-__getname()                          為文件名分配一個緩衝區
  |  |-strncpy_from_user()
  |   |-do_strncpy_from_user()
  |-do_execve()
    |-do_execve_common()
      |-unshare_files()
      |-do_open_exec()
      | |-do_filp_open()
      |-bprm_mm_init()                      bprm初始化，主要是bprm->mm
      | |-mm_alloc()
      | |-init_new_context()
      | |-__bprm_mm_init()
      |  |-kmem_cache_zalloc()              分配一個vma
      |  |-... ...                          設置用戶空間對應的棧頂STACK_TOP_MAX
      |  |-insert_vm_struct()               將vma插入mm表示的進程空間結構
      |-count() ... ...                     計算參數個數、環境變量個數
      |-prepare_binprm()                    查看權限並加載128(BINPRM_BUF_SIZE)個字節
      | |-kernel_read()                     讀取128字節
      |-search_binary_handler()             整個函數的處理核心
        |-security_bprm_check()             SELinux檢查函數
        |-fmt->load_binary()                加載二進制文件，不同二進制格式對應了不同回調函數
        |-load_elf_binary()                 elf對應load_elf_binary
          |-start_thread()                  不同平臺如x86會調用不同的函數
            |-start_thread_common()         主要是設置寄存器的值
{% endhighlight %}

其中有個全局變量 ```formats``` 作為鏈頭，可以通過 ```register_binfmt()``` 註冊一個可執行文件的加載模塊，該模塊一般在 ```fs/binfmt_xxx.c``` 文件中，每次加載可執行文件時只需要遍歷 formats 變量即可。

{% highlight c %}
static LIST_HEAD(formats);

void __register_binfmt(struct linux_binfmt * fmt, int insert)
{
    BUG_ON(!fmt);
    if (WARN_ON(!fmt->load_binary))
        return;
    write_lock(&binfmt_lock);
    insert ? list_add(&fmt->lh, &formats) :
         list_add_tail(&fmt->lh, &formats);
    write_unlock(&binfmt_lock);
}

static inline void register_binfmt(struct linux_binfmt *fmt)
{
    __register_binfmt(fmt, 0);
}
{% endhighlight %}

在 Linux 中最常用的是 ELF 為例，啟動時通過 ```core_initcall(init_elf_binfmt)``` 初始化。

<!--
（1） It releases all resources used by the old process.
（2） It maps the application into virtual address space. The following segments must be taken into  account (the variables specified are elements of the task structure and are             set to the correct values by binary format handler)
（3） The text segment contains the executable code of the program. start_code and end_code
      specify the area in address space where the segment resides.

（4） The pre-initialized data (variables supplied with a specific value at compilation time) are
      located between start_data and end_data and are mapped from the corresponding seg-
      ment of the executable file.
（5）The heap used for dynamic memory allocation is placed in virtual address space; start_brk
     and brk specify its boundaries.
（6）The position of the stack is defined by start_stack; the stack grows downward automati-
     cally on nearly all machines. The only exception is currently PA-Risc. The inverse direction
     of stack growth must be noted by the architecture by setting the configuration symbol
     STACK_GROWSUP.

（7）The program arguments and the environment are mapped into the virtual address space
     and are located between arg_start and arg_end and env_start and env_end, respec-
     tively.
回到do_execve_common（）中，在search_binary_handler後，做收尾工作：
-->

### 內存複製

在調用 ```fork()``` 函數時，會通過 ```copy_mm()``` 複製父進程的內存描述符，子進程通過 ```allcote_mm()``` 從高速緩存中分配 ```struct mm_struct``` 得到。通常，每個進程都有唯一的 ```struct mm_struct```，即唯一的進程地址空間。

當子進程與父進程是共享地址空間，可調用 ```clone()``` 此時不再調用 ```allcote_mm()```，而是僅僅是將 mm 域指向父進程的 mm ，即 ```task->mm = current->mm``` 。

<!--
相反地，撤銷內存是exit_mm()函數，該函數會進行常規的撤銷工作，更新一些統計量。
內核線程
    沒有進程地址空間，即內核線程對應的進程描述符中mm=NULL
    內核線程直接使用前一個進程的內存描述符，僅僅使用地址空間中和內核內存相關的信息
-->




## 參考

{% highlight text %}
{% endhighlight %}
