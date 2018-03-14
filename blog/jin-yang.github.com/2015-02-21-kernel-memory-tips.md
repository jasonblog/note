---
title: Kernel 內存雜項
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,內存,kernel,overcommit,dentry cache,buffer,cache
description: 簡單介紹下內核中與內存相關的內容。
---

簡單介紹下內核中與內存相關的內容。

<!-- more -->

## OverCommit

內核中內存的 OverCommit 就是操作系統承諾給所有進程的內存大小超過了實際可用的內存，主要是由於物理內存頁的分配發生在使用時，而非申請時，也就是所謂的 COW 機制。

例如，程序通過 ```malloc()``` 申請了 200MB 內存，不過實際只使用了 100MB 的內存，也就是說，只要你來申請內存我就給你，寄希望於進程實際上用不到那麼多內存。

如果申請了內存之後，確實會有大量進程使用內存，那麼就會發生類似銀行的 "擠兌"，此時就會使用 Linux 的 OOM killer 機制。

<!--
(OOM = out-of-memory)來處理這種危機：挑選一個進程出來殺死，以騰出部分內存，如果還不夠就繼續殺…也可通過設置內核參數 vm.panic_on_oom 使得發生OOM時自動重啟系統。這都是有風險的機制，重啟有可能造成業務中斷，殺死進程也有可能導致業務中斷，我自己的這個小網站就碰到過這種問題，參見前文。所以Linux 2.6之後允許通過內核參數 vm.overcommit_memory 禁止memory overcommit。
-->

其行為可以通過 ```vm.overcommit_memory``` 參數修改，包括了三種取值：

* 0 (Heuristic Overcommit Handling)，默認值<br>允許 OC ，不過如果申請的內存過大，那麼系統會拒絕請求，而判斷的算法在後面介紹。
* 1 (Always Overcommit)<br>允許 OC，對內存申請來者不拒。
* 2 (Don't Overcommit)<br>禁止 OC 。

當上述的值為 2 時，會禁止 OC，那麼怎麼才算是 OC 呢？實際上，Kernel 設有一個閾值，申請的內存總數超過這個閾值就算 OC，閥值可以通過 ```/proc/meminfo``` 文件查看：

{% highlight text %}
# grep -i commit /proc/meminfo
CommitLimit:    12162784 kB
Committed_AS:    6810268 kB
{% endhighlight %}

其中 ```CommitLimit``` 就是對應的閾值，超過了該值就算 Overcommit ，該值通過 ```vm.overcommit_ratio``` 或 ```vm.overcommit_kbytes``` 間接設置的，公式如下：

{% highlight text %}
CommitLimit = (Physical RAM * vm.overcommit_ratio / 100) + Swap
{% endhighlight %}

其中 ```vm.overcommit_ratio``` 的默認值是 50，也就是表示物理內存的 50% ，當然，也可以直接通過 ```vm.overcommit_kbytes``` 即可。注意，如果 huge pages 那麼就需要從物理內存中減去，公式變成：

{% highlight text %}
CommitLimit = ([total RAM] – [total huge TLB RAM]) * vm.overcommit_ratio / 100 + swap
{% endhighlight %}

另外，```/proc/meminfo``` 中的 ```Committed_AS``` 表示所有進程已經申請的內存總大小；注意，是已經申請的，不是已經分配的。如果 ```Committed_AS``` 超過 ```CommitLimit``` 就表示發生了 OC 。


<!--
“sar -r”是查看內存使用狀況的常用工具，它的輸出結果中有兩個與overcommit有關，kbcommit 和 %commit：
kbcommit對應/proc/meminfo中的 Committed_AS；
%commit的計算公式並沒有採用 CommitLimit作分母，而是Committed_AS/(MemTotal+SwapTotal)，意思是_內存申請_佔_物理內存與交換區之和_的百分比。
$ sar -r
05:00:01 PM kbmemfree kbmemused  %memused kbbuffers  kbcached  kbcommit   %commit  kbactive   kbinact   kbdirty
05:10:01 PM    160576   3648460     95.78         0   1846212   4939368     62.74   1390292   1854880         4

附：對Heuristic overcommit算法的解釋

內核參數 vm.overcommit_memory 的值0，1，2對應的源代碼如下，其中heuristic overcommit對應的是OVERCOMMIT_GUESS：

源文件：source/include/linux/mman.h

#define OVERCOMMIT_GUESS                0
#define OVERCOMMIT_ALWAYS               1
#define OVERCOMMIT_NEVER                2

Heuristic overcommit算法在以下函數中實現，基本上可以這麼理解：
單次申請的內存大小不能超過 【free memory + free swap + pagecache的大小 + SLAB中可回收的部分】，否則本次申請就會失敗。
源文件：source/mm/mmap.c 以RHEL內核2.6.32-642為例

0120 /*
0121  * Check that a process has enough memory to allocate a new virtual
0122  * mapping. 0 means there is enough memory for the allocation to
0123  * succeed and -ENOMEM implies there is not.
0124  *
0125  * We currently support three overcommit policies, which are set via the
0126  * vm.overcommit_memory sysctl.  See Documentation/vm/overcommit-accounting
0127  *
0128  * Strict overcommit modes added 2002 Feb 26 by Alan Cox.
0129  * Additional code 2002 Jul 20 by Robert Love.
0130  *
0131  * cap_sys_admin is 1 if the process has admin privileges, 0 otherwise.
0132  *
0133  * Note this is a helper function intended to be used by LSMs which
0134  * wish to use this logic.
0135  */
0136 int __vm_enough_memory(struct mm_struct *mm, long pages, int cap_sys_admin)
0137 {
0138         unsigned long free, allowed;
0139
0140         vm_acct_memory(pages);
0141
0142         /*
0143          * Sometimes we want to use more memory than we have
0144          */
0145         if (sysctl_overcommit_memory == OVERCOMMIT_ALWAYS)
0146                 return 0;
0147
0148         if (sysctl_overcommit_memory == OVERCOMMIT_GUESS) { //Heuristic overcommit算法開始
0149                 unsigned long n;
0150
0151                 free = global_page_state(NR_FILE_PAGES); //pagecache彙總的頁面數量
0152                 free += get_nr_swap_pages(); //free swap的頁面數
0153
0154                 /*
0155                  * Any slabs which are created with the
0156                  * SLAB_RECLAIM_ACCOUNT flag claim to have contents
0157                  * which are reclaimable, under pressure.  The dentry
0158                  * cache and most inode caches should fall into this
0159                  */
0160                 free += global_page_state(NR_SLAB_RECLAIMABLE); //SLAB可回收的頁面數
0161
0162                 /*
0163                  * Reserve some for root
0164                  */
0165                 if (!cap_sys_admin)
0166                         free -= sysctl_admin_reserve_kbytes >> (PAGE_SHIFT - 10); //給root用戶保留的頁面數
0167
0168                 if (free > pages)
0169                         return 0;
0170
0171                 /*
0172                  * nr_free_pages() is very expensive on large systems,
0173                  * only call if we're about to fail.
0174                  */
0175                 n = nr_free_pages(); //當前free memory頁面數
0176
0177                 /*
0178                  * Leave reserved pages. The pages are not for anonymous pages.
0179                  */
0180                 if (n <= totalreserve_pages)
0181                         goto error;
0182                 else
0183                         n -= totalreserve_pages;
0184
0185                 /*
0186                  * Leave the last 3% for root
0187                  */
0188                 if (!cap_sys_admin)
0189                         n -= n / 32;
0190                 free += n;
0191
0192                 if (free > pages)
0193                         return 0;
0194
0195                 goto error;
0196         }
0197
0198         allowed = vm_commit_limit();
0199         /*
0200          * Reserve some for root
0201          */
0202         if (!cap_sys_admin)
0203                 allowed -= sysctl_admin_reserve_kbytes >> (PAGE_SHIFT - 10);
0204
0205         /* Don't let a single process grow too big:
0206            leave 3% of the size of this process for other processes */
0207         if (mm)
0208                 allowed -= mm->total_vm / 32;
0209
0210         if (percpu_counter_read_positive(&vm_committed_as) < allowed)
0211                 return 0;
0212 error:
0213         vm_unacct_memory(pages);
0214
0215         return -ENOMEM;
0216 }
-->

<!--
https://unix.stackexchange.com/questions/60474/atop-shows-red-line-vmcom-and-vmlim-what-does-it-mean
-->

## Dentry Cache

free 命令主要顯示的用戶的內存使用 (新版available可用)，包括使用 top 命令 (可以通過 Shift+M 內存排序)，對於 slab 實際上沒有統計 (可以使用 slabtop 查看)，真實環境中，經常會出現 dentry(slab) 的內存消耗過大的情況。

dentry cache 是目錄項高速緩存，記錄了目錄項到 inode 的映射關係，用於提高目錄項對象的處理效率，當通過 stat() 查看文件信息、open() 打開文件時，都會創建 dentry cache 。

{% highlight text %}
----- 查看當前進程的系統調用
# strace -f -e trace=open,stat,close,unlink -p $(ps aux | grep 'cmd' | awk '{print $2}')
{% endhighlight %}

不過這裡有個比較奇怪的現象，從 ```/proc/meminfo``` 可以看出，slab 中的內存分為 SReclaimable 和 SUnreclaim 兩部分；而佔比比較大的是 SReclaimable 的內存，也就是可以回收的內存，但是，當通過 slabtop 命令查看 dentry 對應的使用率時，基本處於 100% 的使用狀態。

那麼，到底是什麼意思？這部分內存能不能回收？

首先，```/proc/meminfo``` 對應到源碼中為 ```fs/proc/meminfo.c```，在內核中與這兩個狀態相關的代碼在 ```mm/slub.c``` 中 (如果是新版本的內核)；簡單來說就是對應到了 ```allocate_slab()``` 和 ```__free_slab()``` 函數中，實際上源碼中直接搜索 ```NR_SLAB_RECLAIMABLE``` 即可。

對應到源碼碼中，實際只要申請 cache 時使用了 ```SLAB_RECLAIM_ACCOUNT``` 標示，那麼在進行統計時，就會被標記為 SReclaimable 。

<!-- 而從 /proc/slabinfo 中讀取對應源碼 mm/slab_common.c 中的 cache_show() 函數。 -->

也就是說 meminfo 中的 SReclaimable 標識的是可以回收的 cache，但是真正的回收操作是由各個申請模塊控制的，對於 dentry 來說，真正的回收操作還是 dentry 模塊自己負責。

### 解決方法

對於上述的 slab 內存佔用過多的場景，在 Linux 2.6.16 之後提供了 [drop caches](https://linux-mm.org/Drop_Caches) 機制，用於釋放 page cache、inode/dentry caches ，方法如下。

{% highlight text %}
----- 刷新文件系統緩存
# sync
----- 釋放Page Cache
# echo 1 > /proc/sys/vm/drop_caches
----- 釋放dentries+inodes
# echo 2 > /proc/sys/vm/drop_caches
----- 釋放pagecache, dentries, inodes
# echo 3 > /proc/sys/vm/drop_caches
----- 如果沒有root權限，但是有sudo權限
$ sudo sysctl -w vm.drop_caches=3
{% endhighlight %}

注意，修改 drop_caches 的操作是無損的，不會釋放髒頁，所以需要在執行前執行 sync 以確保確實有可以釋放的內存。

另外，除了上述的手動處理方式之外，還可以通過修改 ```/proc/sys/vm/vfs_cache_pressure``` 的參數，來調整自動清理 inode/dentry caches 的優先級，默認為 100 此時需要調大；關於該參數可以參考內核文檔 [vm.txt]( {{ site.kernel_docs_url }}/Documentation/sysctl/vm.txt )，簡單摘抄如下：

{% highlight text %}
This percentage value controls the tendency of the kernel to reclaim
the memory which is used for caching of directory and inode objects.

At the default value of vfs_cache_pressure=100 the kernel will attempt to
reclaim dentries and inodes at a "fair" rate with respect to pagecache and
swapcache reclaim.  Decreasing vfs_cache_pressure causes the kernel to prefer
to retain dentry and inode caches. When vfs_cache_pressure=0, the kernel will
never reclaim dentries and inodes due to memory pressure and this can easily
lead to out-of-memory conditions. Increasing vfs_cache_pressure beyond 100
causes the kernel to prefer to reclaim dentries and inodes.

Increasing vfs_cache_pressure significantly beyond 100 may have negative
performance impact. Reclaim code needs to take various locks to find freeable
directory and inode objects. With vfs_cache_pressure=1000, it will look for
ten times more freeable objects than there are.
{% endhighlight %}

也就是說，當該文件對應的值越大時，系統的回收操作的優先級也就越高，不過內核對該值好像不敏感，一般會設置為 10000 。

<!-- https://major.io/2008/12/03/reducing-inode-and-dentry-caches-to-keep-oom-killer-at-bay/ -->

<!--
### 自動回收 dcache

查了一些關於Linux dcache的相關資料，發現操作系統會在到了內存臨界閾值後，觸發kswapd內核進程工作才進行釋放，這個閾值的計算方法如下：

1. 首先，grep low /proc/zoneinfo，得到如下結果：

        low      1
        low      380
        low      12067

2. 將以上3列加起來，乘以4KB，就是這個閾值，通過這個方法計算後發現當前服務器的回收閾值只有48MB，因此很難看到這一現象，實際中可能等不到回收，操作系統就會hang住沒響應了。

3. 可以通過以下方法調大這個閾值：將vm.extra_free_kbytes設置為vm.min_free_kbytes和一樣大，則/proc/zoneinfo中對應的low閾值就會增大一倍，同時high閾值也會隨之增長，以此類推。

$ sudo sysctl -a | grep free_kbytes
vm.min_free_kbytes = 39847
vm.extra_free_kbytes = 0
$ sudo sysctl -w vm.extra_free_kbytes=836787  ######1GB


 4. 舉個例子，當low閾值被設置為1GB的時候，當系統free的內存小於1GB時，觀察到kswapd進程開始工作（進程狀態從Sleeping變為Running），同時dcache開始被系統回收，直到系統free的內存介於low閾值和high閾值之間，停止回收。

vm.overcommit_ratio=2
vm.dirty_background_ratio=5
vm.dirty_ratio=20
extra_free_kbytes
-->



接下來，看看如何保留部分內存。

### min_free_kbytes

該參數在內核文檔 [vm.txt]( {{ site.kernel_docs_url }}/Documentation/sysctl/vm.txt )，簡單摘抄如下：

{% highlight text %}
This is used to force the Linux VM to keep a minimum number
of kilobytes free.  The VM uses this number to compute a
watermark[WMARK_MIN] value for each lowmem zone in the system.
Each lowmem zone gets a number of reserved free pages based
proportionally on its size.

Some minimal amount of memory is needed to satisfy PF_MEMALLOC
allocations; if you set this to lower than 1024KB, your system will
become subtly broken, and prone to deadlock under high loads.

Setting this too high will OOM your machine instantly.
{% endhighlight %}


該參數值在 init_per_zone_wmark_min() 函數中設置，


<!--
1. 代表系統所保留空閒內存的最低限。
在系統初始化時會根據內存大小計算一個默認值，計算規則是：

  min_free_kbytes = sqrt(lowmem_kbytes * 16) = 4 * sqrt(lowmem_kbytes)(注：lowmem_kbytes即可認為是系統內存大小）

另外，計算出來的值有最小最大限制，最小為128K，最大為64M。
可以看出，min_free_kbytes隨著內存的增大不是線性增長，comments裡提到了原因“because network bandwidth does not increase linearly with machine size”。隨著內存的增大，沒有必要也線性的預留出過多的內存，能保證緊急時刻的使用量便足矣。
-->

該參數的主要用途是計算影響內存回收的三個參數 ```watermark[min/low/high]```，每個 Zone 都保存了各自的參數值，不同的範圍操作如下。

{% highlight text %}
< watermark[low]
  觸發內核線程kswapd回收內存，直到該Zone的空閒內存數達到watermark[high]；
< watermark[min]
  進行direct reclaim (直接回收)，即直接在應用程序的進程上下文中進行回收，再用
  回收上來的空閒頁滿足內存申請，因此實際會阻塞應用程序，帶來一定的響應延遲，
  而且可能會觸發系統OOM。
{% endhighlight %}

實際上，可以認為 ```watermark[min]``` 以下的內存屬於系統的自留內存，用以滿足特殊使用，所以不會給用戶態的普通申請來用。


<!--
3）三個watermark的計算方法：

 watermark[min] = min_free_kbytes換算為page單位即可，假設為min_free_pages。（因為是每個zone各有一套watermark參數，實際計算效果是根據各個zone大小所佔內存總大小的比例，而算出來的per zone min_free_pages）

 watermark[low] = watermark[min] * 5 / 4
 watermark[high] = watermark[min] * 3 / 2

所以中間的buffer量為 high - low = low - min = per_zone_min_free_pages * 1/4。因為min_free_kbytes = 4* sqrt(lowmem_kbytes），也可以看出中間的buffer量也是跟內存的增長速度成開方關係。
4）可以通過/proc/zoneinfo查看每個zone的watermark
例如：

Node 0, zone      DMA
pages free     3960
       min      65
       low      81
       high     97


3.min_free_kbytes大小的影響
min_free_kbytes設的越大，watermark的線越高，同時三個線之間的buffer量也相應會增加。這意味著會較早的啟動kswapd進行回收，且會回收上來較多的內存（直至watermark[high]才會停止），這會使得系統預留過多的空閒內存，從而在一定程度上降低了應用程序可使用的內存量。極端情況下設置min_free_kbytes接近內存大小時，留給應用程序的內存就會太少而可能會頻繁地導致OOM的發生。
min_free_kbytes設的過小，則會導致系統預留內存過小。kswapd回收的過程中也會有少量的內存分配行為（會設上PF_MEMALLOC）標誌，這個標誌會允許kswapd使用預留內存；另外一種情況是被OOM選中殺死的進程在退出過程中，如果需要申請內存也可以使用預留部分。這兩種情況下讓他們使用預留內存可以避免系統進入deadlock狀態。
http://kernel.taobao.org/index.php?title=Kernel_Documents/mm_sysctl
-->

<!--
一個Laravel隊列引發的報警
https://huoding.com/2015/06/10/444
Linux服務器Cache佔用過多內存導致系統內存不足問題的排查解決（續）
http://www.cnblogs.com/panfeng412/p/drop-caches-under-linux-system-2.html
Where is the memory going? 
https://www.halobates.de/memory.pdf
 一次linux內存問題排查-slab 
https://bhsc881114.github.io/2015/04/19/%E4%B8%80%E6%AC%A1linux%E5%86%85%E5%AD%98%E9%97%AE%E9%A2%98%E6%8E%92%E6%9F%A5-slab/
怎樣診斷slab洩露問題
http://linuxperf.com/?p=148
-->


## Cache VS. Buffer

簡單說，Buffer 的核心作用是用來緩衝，緩和衝擊。比如你每秒要寫 100 次硬盤，對系統衝擊很大，浪費了大量時間在忙著處理開始寫和結束寫這兩件事。用個 buffer 暫存起來，變成每 10 秒寫一次硬盤，對系統的衝擊就很小，寫入效率高了，極大緩和了衝擊。

Cache 的核心作用是加快取用的速度。比如你一個很複雜的計算做完了，下次還要用結果，就把結果放手邊一個好拿的地方存著，下次不用再算了，加快了數據取用的速度。

所以，如果你注意關心過存儲系統的話，你會發現硬盤的讀寫緩衝/緩存名稱是不一樣的，叫 write-buffer 和 read-cache；很明顯地說出了兩者的區別。

<!--
當然很多時候宏觀上說兩者可能是混用的。比如實際上memcached很多人就是拿來讀寫都用的。不少時候Non-SQL數據庫也是。嚴格來說，CPU裡的L2和L3 Cache也都是讀寫兼用——因為你沒法簡單地定義CPU用它們的方法是讀還是寫。硬盤裡也是個典型例子，buffer和cache都在一塊空間上，到底是buffer還是cache？

不過仔細想一下，你說拿cache做buffer用行不行？當然行，只要能控制cache淘汰邏輯就沒有任何問題。那麼拿buffer做cache用呢？貌似在很特殊的情況下，能確定訪問順序的時候，也是可以的。簡單想一下就明白——buffer根據定義，需要隨機存儲嗎？一般是不需要的。但cache一定要。所以大多數時候用cache代替buffer可以，反之就比較侷限。這也是技術上說cache和buffer的關鍵區別。

https://www.zhihu.com/question/26190832/answer/146259979
-->

那麼 free 命令中的 buffers 和 cache 是什麼意思？

### free

新版的 free 命令輸出如下。

{% highlight text %}
$ free -wm
              total        used        free      shared     buffers       cache   available
Mem:           7881        3109         797        1113         309        3665        3310
Swap:          7936         260        7676
{% endhighlight %}

* buffers，表示塊設備 (block device) 所佔用的緩存頁，包括了直接讀寫塊設備以及文件系統元數據 (metadata) 比如 SuperBlock 所使用的緩存頁；
* cached，表示普通文件數據所佔用的緩存頁。

該命令讀取的是 ```/proc/meminfo``` 文件中的 Buffers 和 Cached 數據，而在內核中的實現實際上對應了 ```meminfo_proc_show()@fs/proc/meminfo.c``` 函數，內容如下。

{% highlight c %}
static int meminfo_proc_show(struct seq_file *m, void *v)
{
    si_meminfo(&i);  // 通過nr_blockdev_pages()函數填充bufferram
    si_swapinfo(&i);

    cached = global_page_state(NR_FILE_PAGES) -
            total_swapcache_pages() - i.bufferram;
    if (cached < 0)
        cached = 0;

    seq_printf(m,
        "Buffers:        %8lu kB\n"
        "Cached:         %8lu kB\n"
        // ... ..
        K(i.bufferram),
        K(cached),
        // ... ..
}
{% endhighlight %}

如上計算 cached 公式中，```global_page_state(NR_FILE_PAGES)``` 實際讀取的 ```vmstat[NR_FILE_PAGES]```，也就是用於統計所有緩存頁 (page cache) 的總和，它包括：

<!--
mm/filemap.c:200:       __dec_zone_page_state(page, NR_FILE_PAGES);
mm/filemap.c:488:               __inc_zone_page_state(new, NR_FILE_PAGES);
mm/filemap.c:580:       __inc_zone_page_state(page, NR_FILE_PAGES);
mm/swap_state.c:105:            __inc_zone_page_state(page, NR_FILE_PAGES);
mm/swap_state.c:157:    __dec_zone_page_state(page, NR_FILE_PAGES);
mm/shmem.c:318:         __inc_zone_page_state(page, NR_FILE_PAGES);
mm/shmem.c:341: __dec_zone_page_state(page, NR_FILE_PAGES);
mm/shmem.c:1003:                __inc_zone_page_state(newpage, NR_FILE_PAGES);
mm/shmem.c:1004:                __dec_zone_page_state(oldpage, NR_FILE_PAGES);
mm/migrate.c:414:       __dec_zone_page_state(page, NR_FILE_PAGES);
mm/migrate.c:415:       __inc_zone_page_state(newpage, NR_FILE_PAGES);

    Cached
    buffers
    交換區緩存(swap cache)
-->


> swap cache 主要是針對匿名內存頁，例如用戶進程通過malloc()申請的內存頁，當要發生swapping換頁時，如果一個匿名頁要被換出時，會先計入到swap cache，但是不會立刻寫入物理交換區，因為Linux的原則是除非絕對必要，儘量避免IO。
>
> 所以swap cache中包含的是被確定要swapping換頁，但是尚未寫入物理交換區的匿名內存頁。

<!--
vmstat[NR_FILE_PAGES] 可以通過 /proc/vmstat 來查看，表示所有緩存頁的總數量：
# cat /proc/vmstat
...
nr_file_pages 587334
...


注意以上nr_file_pages是以page為單位，而不像free命令是以KB為單位，一個page等於4KB。

    直接修改 nr_file_pages 的內核函數是：
    __inc_zone_page_state(page, NR_FILE_PAGES) 和
    __dec_zone_page_state(page, NR_FILE_PAGES)，
    一個用於增加，一個用於減少。


先看”cached”：

“Cached” 就是除去 “buffers” 和 “swap cache” 之外的緩存頁的數量：
global_page_state(NR_FILE_PAGES) – total_swapcache_pages – i.bufferram
所以關鍵還是要理解 “buffers” 是什麼含義。


#### buffers

從源碼中可以看到 buffers 來自於 nr_blockdev_pages() 的返回值，該函數如下：

{% highlight c %}
long nr_blockdev_pages(void)
{
    struct block_device *bdev;
    long ret = 0;
    spin_lock(&bdev_lock);
    list_for_each_entry(bdev, &all_bdevs, bd_list) {
        ret += bdev->bd_inode->i_mapping->nrpages;
    }
    spin_unlock(&bdev_lock);
    return ret;
}
{% endhighlight %}

這段代碼就是遍歷所有的塊設備 (block device)，累加每個塊設備的 inode 的 i_mapping 的頁數，統計得到的就是 buffers 。所以很明顯，buffers 是與塊設備直接相關的。

那麼誰會更新塊設備的緩存頁數量(nrpages)呢？我們繼續向下看。

搜索kernel源代碼發現，最終更新mapping->nrpages字段的函數就是add_to_page_cache和__remove_from_page_cache：

static inline int add_to_page_cache(struct page *page,
                struct address_space *mapping, pgoff_t offset, gfp_t gfp_mask)
{
        int error;

        __set_page_locked(page);
        error = add_to_page_cache_locked(page, mapping, offset, gfp_mask);
        if (unlikely(error))
                __clear_page_locked(page);
        return error;
}

void remove_from_page_cache(struct page *page)
{
        struct address_space *mapping = page->mapping;
        void (*freepage)(struct page *) = NULL;
        struct inode *inode = mapping->host;

        BUG_ON(!PageLocked(page));

        if (IS_AOP_EXT(inode))
                freepage = EXT_AOPS(mapping->a_ops)->freepage;

        spin_lock_irq(&mapping->tree_lock);
        __remove_from_page_cache(page);
        spin_unlock_irq(&mapping->tree_lock);
        mem_cgroup_uncharge_cache_page(page);

        if (freepage)
                freepage(page);
}

page_cache_tree_insert()

__delete_from_page_cache()
 |-page_cache_tree_delete() --

這兩個函數是通用的，block device 和 文件inode 都可以調用，至於更新的是塊設備的(buffers)還是文件的(cached)，取決於調用參數變量mapping：如果mapping對應的是文件inode，自然就不會影響到 “buffers”；如果mapping對應的是塊設備，那麼相應的統計信息會反映在 “buffers” 中。我們下面看看kernel中哪些地方會把塊設備的mapping傳遞進來。

搜索內核源代碼發現，ext4_readdir 函數調用 page_cache_sync_readahead 時傳遞的參數是 sb->s_bdev->bd_inode->i_mapping，其中s_bdev就是塊設備，也就是說在讀目錄(ext4_readdir)的時候可能會增加 “buffers” 的值：
static int ext4_readdir(struct file *filp,
                         void *dirent, filldir_t filldir)
{

...
        struct super_block *sb = inode->i_sb;
...
                        if (!ra_has_index(&filp->f_ra, index))
                                page_cache_sync_readahead(
                                        sb->s_bdev->bd_inode->i_mapping,
                                        &filp->f_ra, filp,
                                        index, 1);
...
}
1
2
3
4
5
6
7
8
9
10
11
12
13
14

static int ext4_readdir(struct file *filp,
                         void *dirent, filldir_t filldir)
{

...
        struct super_block *sb = inode->i_sb;
...
                        if (!ra_has_index(&filp->f_ra, index))
                                page_cache_sync_readahead(
                                        sb->s_bdev->bd_inode->i_mapping,
                                        &filp->f_ra, filp,
                                        index, 1);
...
}

繼續琢磨上面的代碼，sb表示SuperBlock，屬於文件系統的metadata（元數據），突然間一切恍然大悟：因為metadata不屬於文件，沒有對應的inode，所以，對metadata操作所涉及的緩存頁都只能利用塊設備mapping，算入 buffers 的統計值內。

    打個岔：ext4_readdir() 中調用 page_cache_sync_readahead() 顯然是在進行預讀(read-ahead)，為什麼read-ahead沒有使用普通文件inode的mapping，而是使用了底層的塊設備呢？從記載在補丁中的說明來看，這是一個權宜之計，看這裡，所以不必深究了。

舉一反三，如果文件含有間接塊(indirect blocks)，因為間接塊屬於metadata，所以走的也是塊設備的mapping。查看源代碼，果然如此：
ext4_get_blocks
->  ext4_ind_get_blocks
    ->  ext4_get_branch
        ->  sb_getblk

static inline struct buffer_head *
sb_getblk(struct super_block *sb, sector_t block)
{
        return __getblk(sb->s_bdev, block, sb->s_blocksize);
}
1
2
3
4
5
6
7
8
9
10

ext4_get_blocks
->  ext4_ind_get_blocks
    ->  ext4_get_branch
        ->  sb_getblk

static inline struct buffer_head *
sb_getblk(struct super_block *sb, sector_t block)
{
        return __getblk(sb->s_bdev, block, sb->s_blocksize);
}

這樣，我們就知道了，”buffers” 是塊設備(block device)佔用的緩存頁，分為兩種情況：

    直接對塊設備進行讀寫操作；
    文件系統的metadata（元數據），比如 SuperBlock。
-->

#### 驗證

如上，讀取 EXT4 文件系統的目錄會使用到 "buffers"，這裡使用 find 命令掃描文件系統，觀察 "buffers" 增加的情況：

{% highlight text %}
# sync
# echo 3 > /proc/sys/vm/drop_caches
$ free -wk; find ~ -name "not exits file" >/dev/null 2>&1; free -wk
              total        used        free      shared     buffers       cache   available
Mem:        8070604     3260408     3445852     1102588        5236     1359108     3418844
Swap:       8127484      300172     7827312
              total        used        free      shared     buffers       cache   available
Mem:        8070604     3249764     3207336     1087716      250484     1363020     3417764
Swap:       8127484      300172     7827312
{% endhighlight %}

再測試下直接讀取 block device 並觀察 "buffers" 增加的現象：

{% highlight text %}
# sync
# echo 3 > /proc/sys/vm/drop_caches
# free -wk; dd if=/dev/sda1 of=/dev/null count=200M; free -wk
              total        used        free      shared     buffers       cache   available
Mem:        8070604     3244516     3486124     1094648         932     1339032     3451048
Swap:       8127484      300172     7827312
532480+0 records in
532480+0 records out
272629760 bytes (273 MB) copied, 0.612241 s, 445 MB/s
              total        used        free      shared     buffers       cache   available
Mem:        8070604     3245032     3218528     1094868      267196     1339848     3427012
Swap:       8127484      300172     7827312
{% endhighlight %}

<!--
 結論：

free 命令所顯示的 “buffers” 表示塊設備(block device)所佔用的緩存頁，包括直接讀寫塊設備、以及文件系統元數據(metadata)如SuperBlock所使用的緩存頁；
而 “cached” 表示普通文件所佔用的緩存頁。
-->


## 參考

關於內存的 OverCommit 可以參考內核文檔 [vm/overcommit-accounting]( {{ site.kernel_docs_url }}/Documentation/vm/overcommit-accounting ) 。

<!-- https://www.win.tue.nl/~aeb/linux/lk/lk-9.html -->



{% highlight text %}
{% endhighlight %}
