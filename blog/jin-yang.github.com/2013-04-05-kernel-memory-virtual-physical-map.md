---
title: Kernel 內存映射
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,內存,memory,映射
description: 現在的操作系統在內存使用時，一般為邏輯地址，那麼邏輯地址和物理地址之間是怎樣映射的，或者說，通過邏輯地址如何找到物理內存中的內容。其實，包括動態共享庫，mmap 映射等，都使用到了虛擬內存，也就是邏輯地址的映射。這裡，簡單介紹下邏輯地址到物理地址的映射關係。
---

現在的操作系統在內存使用時，一般為邏輯地址，那麼邏輯地址和物理地址之間是怎樣映射的，或者說，通過邏輯地址如何找到物理內存中的內容。

其實，包括動態共享庫，mmap 映射等，都使用到了虛擬內存，也就是邏輯地址的映射。

這裡，簡單介紹下邏輯地址到物理地址的映射關係。

<!-- more -->

## 簡介

如下，是 x86 系統中與內存相關的寄存器以及相關的轉換關係。

![memory system level registers and data structure]({{ site.url }}/images/linux/kernel/memory-system-level-registers-and-data-structure.png "memory system level registers and data structure"){: .pull-center width="90%" }

## 地址轉換

在如下的程序中打印出 tmp 變量的地址，變量保存在棧中，假設地址為 ```address:0x7FFF797B9698``` ，該結果是邏輯地址。

注意，現在的 Interl CPU 寄存器採用 64-bit，而其內存的尋址空間為 48-bits，也就是說，多餘的位實際上是無效的。

{% highlight c %}
#include <stdio.h>
int main()
{
    unsigned long tmp = 0x12345678;
    printf("tmp variable address:0x%08lX\n", &tmp);
    return 0;
}
{% endhighlight %}

現在的 CPU 都會有內存管理單元，對於 IA-32e 類似架構的 CPU 規定，地址映射過程是 ```邏輯地址 -> 線性地址 -> 物理地址```。當然運行在 Intel 上的 Linux 內核也採用相同的方法。

Intel 通過 ```IA32_EFER``` 寄存器定義了不同的 CPU 模式，不過暫時不太清除如何讀取該寄存器。

![memory extended feature enable register]({{ site.url }}/images/linux/kernel/memory-extended-feature-enable-register.png "memory extended feature enable register"){: .pull-center width="80%" }

除此之外，先介紹下這裡會使用到的工具。

<!-- 變量 a 定在了數據段， 通過 ds 段寄存器的值可以看出使用的是 GDT ，entry 是 15.-->

### x86-64 示例

以如下的值為例。

{% highlight text %}
$ ./address
tmp address:0x7FFE07DB9A70
CR4=1427E0 PSE(0) PAE(1)
CR3=12A6E0000 CR0=80050033, pgd:0xFFFF88012A6E0000
gdtr address:FFFF88021FB09000, limit:7F
{% endhighlight %}

這裡獲取的 GDTR 基址是 ```0xFFFF88021FB09000```，注意這裡是線性地址，實際上對應的物理地址是減去一個偏移 ```__PAGE_OFFSET``` ，也就是 ```0xFFFF880000000000```，因此對應的物理地址是 ```0x21FB09000``` 。

{% highlight c %}
#define __PAGE_OFFSET           _AC(0xffff880000000000, UL)
{% endhighlight %}

應用程序中，tmp 變量的邏輯地址或者說線性地址為 ```0x7FFE07DB9A70```，那麼接下來，我們逐步映射到實際物理地址，看這個地址的數據是否真的為 ```0x12345678beaf5dde``` 。

如下，是 x86 的分段與分頁機制。

![memory segmentation paging]({{ site.url }}/images/linux/kernel/memory-segmentation-paging.png "memory segmentation paging"){: .pull-center width="80%" }

### 分段機制

分段的存在更多就是為了兼容性，在 x86-64 下的 64-bit 程序該功能近似於 bypass，對於 CS 會根據 GDT 中的信息判斷是 64-bit 還是 32-bit compatibility 模式，而對於數據段的選擇器如 DS、ES、FS、GS、SS 可以為 00H。而對於 GS、SS 可以不為 00H，手冊說仍然有效，不過沒有仔細研究。

上述輸出的地址為 ```0x7FFE07DB9A70```，這個就是邏輯地址，首先需要將邏輯地址轉化成線性地址，也就是分段機制。

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

實際上有 6 個段寄存器，不過 FS、DS、ES、GS 的值都是 0，只有 CS 和 DS 的值有所區別，每個進程的 6 個寄存器是一樣的，不同的是 IP 和 SP，從上面的代碼中也可以看到。

如下，是 x86 寄存器的段選擇符。

![memory segment selector]({{ site.url }}/images/linux/kernel/memory-segment-selector.png "memory segment selector"){: .pull-center width="40%" }

TI 表示選擇的段描述符是存在 GDT 中還是 LDT 中，一般都只存在 GDT 中，幾乎沒有選擇 LDT 的，只有像 wine 這種進程才會用到 LDT 。

RPL 表示特權等級，Linux 中只使用了 0 和 3 兩個特權級別，其中 0 表示最高權限，3 表示無特權；如上，也就是說 CS 和 SS 段是無特權的。

另外，前面的 13 位表示在 GDT 表中的 index，或者說是第幾項，而 GDT 存放的地址就保存在了 GDTR 寄存器中。

#### 查找地址

如上，對應的 GDTR 寄存器的地址是 ```0xffff880223489000 - 0xffff880000000000 = 0x223489000```，不過這裡有個 bug ，暫時還沒有想好怎麼解決。

dram 獲取的是可用頁數，不過很多系統的可用內存的空間是不連續的，這就導致了部分的內存無法訪問。

<!-- 也就是說對於 intel x86-64 我們可以直接忽略分段功能，而直接查看分頁的處理。 -->

由於每個 GDT 項長 64-bits，所以對應的地址為 ```0x223489000 + idx*8``` ，然後根據對應的值參照下圖的內容，然後計算地址即可。

![memory segment descriptor]({{ site.url }}/images/linux/kernel/memory-segment-descriptor.png "memory segment descriptor"){: .pull-center width="80%" }

不過一般計算之後都是 ```0x000000``` ，也就是說虛擬地址直接等於線性地址。

### 分頁機制

分頁過程會將 48-bit 的線性地址轉換為 52-bit 的物理地址, 可以看出雖然是 64bit 的操作系統但在處理器層面並沒有提供 2^64 大小的內存訪問範圍。

另外需要注意的是，通過 CR3 映射的內存地址實際是 "物理地址"。對於分頁機制 IA-32e 有 4K、2M、1G 的分頁方式，Linux 中採用的通常為 4K ，在此僅以此為例。

x86-64 採用了四層地址映射，

{% highlight text %}
+-------+-------+-------+-------+-------+---------+
| Index | PTE   | PMD   | PUD   | PGD   | Reseved |
| 00~11 | 12~20 | 21~29 | 30~38 | 39~47 | 48~63   |
+-------+-------+-------+-------+-------+---------+
{% endhighlight %}

如上，PGD、PUD、PMD、PTE 各佔了 9 位，加上 12 位的頁內 index，共用了 48 位，即可管理的地址空間為 ```2^48=256T```，而在 32 位地址模式時，該值僅為 ```2^32=4G``` 。

另外 64 位地址時支持的物理內存最大為 ```2^46=64T```，見 ```e820.c``` 中 ```MAX_ARCH_PFN``` 的定義：

{% highlight c %}
# define MAX_PHYSMEM_BITS   46
# define MAXMEM       _AC(__AC(1, UL) << MAX_PHYSMEM_BITS, UL)
# define MAX_ARCH_PFN MAXMEM>>PAGE_SHIFT
{% endhighlight %}

而在 32 位地址時，在開啟 PAE 選項後最大支持的物理內存為 64G 。

### 實踐

這裡的示例代碼可以參考 [github memory]({{ site.example_repository }}/linux/memory/to_physical/) ，主要包括瞭如下的文件：

* registers.c 內核模塊，用於讀取 cr0、gdtr 等信息，可以通過 ```/proc/registers``` 讀取；
* address.c 返回臨時對象的地址，也就是棧地址，目前是 48bits ；

![memory cr0 register]({{ site.url }}/images/linux/kernel/memory_cr0_register.png "memory cr0 registe"){: .pull-center width="80%" }

#### 分頁機制

在滿足 ```CR0.PG = 1```、```CR4.PAE = 1```、```IA32_EFER.LME = 1``` 條件時會打開 IA-32E Paging 機制，另外，在 ```CR4.PCIDE = 0``` 不同時，對應的 ```CR3``` 使用也略有區別，可以查看手冊的 ```4.5 IA-32E PAGING``` 部分。

線性地址到物理地址的轉換關係如下。

![memory linear address translation]({{ site.url }}/images/linux/kernel/memory-linear-address-translation.png "memory linear address translation"){: .pull-center width="80%" }

在分頁機制中，有效的虛擬地址是 48-bits，會映射到 52-bits 的物理地址；將臨時變量 tmp 的地址 ```0x7FFE07DB9A70``` 映射為二進制格式為。

{% highlight text %}
0111 1111 1111 1110 0000 0111 1101 1011 1001 1010 0111 0000
011111111 111111000 000111110 110111001 101001110000
       FF       1F8        3E       1B9          A70
{% endhighlight %}

##### 第一級映射

CR3 寄存器中的值是 ```0x12A6E0000```，這是第一級映射表 (PML4) 的起始物理地址，這張表中保存著第二級映射表的物理地址。而線性地址中的 ```bits[47:39]``` 對應 ```PML4E``` 的序號。

{% highlight text %}
0x12A6E0000 + 011111111b(0xFF) * 8 = 0x12A6E0000 + 0x7F8 = 0x12A6E07F8

(FILEVIEW)
000012A6E07F0    00000001DB9A6067      00000001F9ACC067
000012A6E0800    0000000000000000      0000000000000000
{% endhighlight %}

注意，因為每個單元是 64-bits 因此需要在序號基礎上乘以 8 獲得地址。最後，該地址對應的二級映射表的起始地址為：```0x1F9ACC000``` (067 後面的 12-bits 是頁面屬性)。

<!--Bit17:PCIDE-->

##### 第二級映射

二級映射的任務是找到第三級映射表的起始地址。

{% highlight text %}
0x1F9ACC000 + 111111000b * 8 = 0x1F9ACC000 + 0xFC0 = 0x1F9ACCFC0

(FILEVIEW)
00001F9ACCFC0    000000018B96D067      0000000000000000
00001F9ACCFD0    0000000000000000      0000000000000000
{% endhighlight %}

與上述類似，第三級映射表的起始地址為 ```0x18B96D000``` (067 後面的 12-bits 是頁面屬性)。

##### 第三級映射

三級映射的任務是找到第四級映射表的起始地址。

{% highlight text %}
0x18B96D000 + 000111110b * 8 = 0x18B96D000 + 0x1F0 = 0x18B96D1F0

(FILEVIEW)
000018B96D1F0    0000000154F81067      0000000000000000
000018B96D200    0000000000000000      0000000000000000
{% endhighlight %}

第四級映射表的起始地址為 ```0x154F81000``` (067 後面 12-bits 是頁面屬性)。


##### 第四級映射

第四級映射的任務是找到臨時變量 tmp 所在的物理頁面起始地址。

{% highlight text %}
0x154F81000 + 110111001b * 8 = 0x154F81000 + 0xDC8 = 0x154F81DC8

(FILEVIEW)
0000154F81DC0    800000011FDD7067      800000014DD61067
0000154F81DD0    0000000000000000      800000017C7A6067
{% endhighlight %}

```0x154F81DC8``` 地址單元中的數據，就是物理頁面起始地址，也就是我們最後所得到的 tmp 對應的物理頁為 ```0x14DD61000``` 。

##### 最終物理地址計算

tmp 變量所在內存頁面物理地址為 ```0x14DD61000```，這個地址僅是物理頁面地址，tmp 變量所在的物理地址為。

{% highlight text %}
0x14DD61000 + 101001110000b = 0x14DD61000 + 0xA70 = 0x14DD61A70

(FILEVIEW)
000014DD61A70    12345678BEAF5DDE      0000000000400830
000014DD61A80    0000000000000000      00007F1FF67EFAF5
{% endhighlight %}

經過 4 級頁面映射，終於找到了 tmp 對應的實際物理地址為 ```0x14DD61A70``` ，對應的數據也為 ```0x12345678BEAF5DDE``` 。

## 訪問物理內存

用戶態的程序都是在內存保護模式下使用內存，無法直接訪問物理內存，且用戶程序使用的是邏輯地址，而非物理地址，dram 模塊可以使用戶態程序訪問所有物理內存。

內核模塊通過文件讀寫的方式，實現物理地址訪問，將物理地址，作為參數 pos 傳遞。

{% highlight text %}
ssize_t my_read( struct file *file, char *buf, size_t count, loff_t *pos )
{% endhighlight %}

在內核代碼中，是無法直接訪問物理地址的，代碼能訪問的都是邏輯地址。此時我們需要先將物理地址轉換成邏輯地址，才能在代碼中對地址讀寫。

物理地址轉換成邏輯地址方法：

### 1. 獲取PFN以及偏移

根據物理地址，計算出對應的頁面號和頁內偏移。

{% highlight text %}
page_number = *pos / PAGE_SIZE;
page_indent = *pos % PAGE_SIZE;
{% endhighlight %}

### 2. 獲取頁面指針

將頁面號找到對應的頁面指針，注意在 2.6.32 及以上內核中，沒有導出 mem_map 符號，只能通過 ```pfn_to_page()``` 來找到對應的頁面指針。

### 3. 映射為邏輯地址

通過 kmap 映射成邏輯地址，TODO: 暫時還沒有搞明白 kmap 是如何進行映射的。

{% highlight text %}
from = kmap( pp ) + page_indent;
{% endhighlight %}

映射成邏輯地址後，我們直接通過 from 指針來訪問物理地址 pos 了。

### 4. 使用示例

模塊加載和使用方式。

{% highlight text %}
----- 編譯模塊
# make
----- 加載模塊，會自動創建/dev/dram字符設備
# insmod dram.ko

----- 通過fileview查看數據
# ./fileview /dev/dram
{% endhighlight %}

我們使用簡單的程序 fileview 來看物理內存中的實際數據，物理地址可以手工輸入，程序的前半部分會顯示所需的數據，後半部分會接收鍵盤輸入的命令。

## 物理地址轉換

現在幾乎所有的操作系統都支持虛擬地址 (Virtual Address) ，每個用戶空間的進程都有自己的虛擬地址空間，內核配合 CPU 硬件 MMU 完成到物理地址的轉換。

Linux 內核保存了地址轉換相關的數據結構，正常來說無法在用戶空間訪問，在 2.6.25 之後，提供了 ```/proc/$(pid}/pagemap``` ```/proc/kpagecount``` ```/proc/kpageflags``` 完成虛擬地址到物理地址的轉換。

### pagemap

```/proc/${pid}/pagemap``` 文件包含了該進程的虛擬地址到物理地址轉換相關的映射關係，每個轉換包含了 64-bits，詳細的內容可以參考內核文檔 [pagemap.txt]({{ site.kernel_docs_url }}/Documentation/vm/pagemap.txt) 。

![memory proc pagemap]({{ site.url }}/images/linux/kernel/memory-proc-pagemap.png "memory proc pagemap"){: .pull-center width="80%" }

{% highlight text %}
$ cat /proc/$(pidof mysqld)/maps | head -3
00400000-0283f000 r-xp 00000000 08:09 1308936      /opt/mysql-5.7/bin/mysqld
02a3e000-02b18000 r--p 0243e000 08:09 1308936      /opt/mysql-5.7/bin/mysqld
02b18000-02bc7000 rw-p 02518000 08:09 1308936      /opt/mysql-5.7/bin/mysqld

$ ./pagemap `pidof mysqld` 0x00400000
{% endhighlight %}

可以查看源碼 [pagemap.c]( {{ site.example_repository }}/linux/memory/pagemap/pagemap.c ) 以及 [pagemap.py]( {{ site.example_repository }}/linux/memory/pagemap/pagemap.py ) 。

### 示例

另外的一個示例是直接從 [foobar.c]( {{ site.example_repository }}/linux/memory/pagemap/foobar.c ) 讀取物理地址，然後通過之前介紹的 dram 和 fileview 讀取物理地址對應的數據。

{% highlight text %}
----- 編譯後直接執行，獲取對應變量的物理地址
$ ./foobar
vaddr = 0x7ffdafae39ec, phy = 0x350ea9ec
Enter any key to exit
{% endhighlight %}

如下，可以直接通過 dram 和 fileview 讀取到對應物理地址的數據；注意，由於是小端存儲，所以正常的 ```0x12345678``` 顯示的內容如下。

![memory pagemap]({{ site.url }}/images/linux/kernel/memory-pagemap.png "memory pagemap"){: .pull-center width="70%" }

<!--
https://github.com/dwks/pagemap
http://blog.jeffli.me/blog/2014/11/08/pagemap-interface-of-linux-explained/
http://www.cnblogs.com/lanrenxinxin/p/6216925.html
-->

## 參考

[CS 635: Advanced Systems Programming](http://cs.usfca.edu/~cruse/cs635/) 網上一個牛掰的課程網站，包括了一些不錯的源碼，包括上面的 dram 和 fileview 。

[關於Linux內存地址映射的介紹](http://ilinuxkernel.com/?p=1276)，可以參考 [本地文檔](/reference/linux/kernel/linux_memap.tar.bz2) ，含有x86_32/64詳解，該博客對於Linux內存有比較詳細的介紹；同時可以參考 [Linux 從虛擬地址到物理地址](http://blog.chinaunix.net/uid-24774106-id-3427836.html) 。

[segmentation 情景分析](http://www.mouseos.com/arch/segmentation.html) 關於段選擇寄存器比較詳細的介紹。
<!-- [本地文檔](/reference/linux/kernel/segmentation.maff) -->

<!--

Intel開發文檔手冊
https://software.intel.com/en-us/articles/intel-sdm
Linux 從虛擬地址到物理地址
http://blog.chinaunix.net/uid-24774106-id-3427836.html
在 linux x86-32 模式下分析內存映射流程
http://www.cnblogs.com/ziyang3721/p/4269132.html

Linux-3.14.12內存管理筆記【開啟分頁管理】
http://blog.chinaunix.net/xmlrpc.php?r=blog/article&uid=26859697&id=4456082
http://blog.chinaunix.net/xmlrpc.php?r=blog/article&uid=26859697&id=4470758

/images/linux/kernel/memory-management-overview.jpg

Linux內存管理--基本概念
http://blog.csdn.net/myarrow/article/details/8624687
-->


{% highlight text %}
{% endhighlight %}
