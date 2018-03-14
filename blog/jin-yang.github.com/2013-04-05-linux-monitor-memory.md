---
title: Linux 監控之 Memory
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,monitor,監控,內存,memory
description:
---


<!-- more -->

<!--

# 簡介

在 Linux 的內存分配機制中，優先使用物理內存，當物理內存還有空閒時（還夠用），不會釋放其佔用內存，就算佔用內存的程序已經被關閉了，該程序所佔用的內存用來做緩存使用，這樣對於開啟過的程序、或是讀取剛存取過得數據會比較快，可以提高整體 I/O 效率。

為了提高磁盤存取效率，Linux 做了一些精心的設計，除了對 dentry 進行緩存（用於 VFS，加速文件路徑名到 inode 的轉換），還採取了兩種主要 Cache 方式：Buffer Cache 和 Page Cache。前者針對磁盤塊的讀寫，後者針對文件 inode 的讀寫。這些 Cache 有效縮短了 I/O 系統調用（比如 read，write，getdents）的時間。

在 Linux 緩存機制中存在兩種，分別為 buffer 和 cache，主要區別為：A buffer is something that has yet to be "written" to disk. A cache is something that has been "read" from the disk and stored for later use。
具體的操作如下：<ul><li>
    當從磁盤讀取到內存的數據在被相關應用程序讀取後，如果有剩餘內存，則這部分數據會存入cache，以備第2次讀取時，避免重新讀取磁盤。</li><br><li>
    當一個應用程序在內存中修改過數據後，因為寫入磁盤速度相對較低，在有空閒內存的情況下，這些數據先存入buffer，在以後某個時間再寫入磁盤，從而應用程序可以繼續後面的操作，而不必等待這些數據寫入磁盤的操作完成。</li><br><li>
    如果在某個時刻，系統需要更多的內存，則會把cache部分擦除，並把buffer中的內容寫入磁盤，從而把這兩部分內存釋放給系統使用，這樣再次讀取cache中的內容時，就需要重新從磁盤讀取了。</li></ul>
</p>
-->

## free 命令

內存使用情況可以通過 free 命令查看，在 CentOS 中，是 procps-ng 包中的一個程序，該命令實際是讀取 /proc/meminfo 文件，對應內核源碼中的 fs/proc/meminfo.c。

{% highlight text %}
$ free
         total       used       free     shared     buffers      cached
Mem:    386024     377116       8908          0       21280      155468
-/+ buffers/cache: 200368     185656
Swap:    393552        0      393552
{% endhighlight %}

最新版本的 free 會將 buffer/cache 展示在一起，可以通過 free -w 分開查看 buffers 以及 cache；默認的單位是 KB 。

{% highlight text %}
Mem 表示物理內存統計
    total  : 物理內存總大小；
    used   : 已經分配的內存總計（含buffers 與cache ），但其中部分可能尚未使用；
    free   : 未被分配的內存；
    shared : 多個進程共享的內存總額；
    buffers: 系統分配但未被使用的 buffers 數量，磁盤buffers數量；
    cached: 系統分配但未被使用的 cache 數量，磁盤cache數量；

-/+ buffers/cached 表示物理內存的緩存統計
    used: 實際使用的 buffers 與 cache 總量(used-buffers-cached)，也是實際使用的內存總量；
    free: 未被使用的 buffers 與 cache 和未被分配的內存之和(buffers+cached+free)，這就是系統當前實際可用內存；

Swap 表示硬盤上交換分區的使用情況
    只有 mem 被當前進程實際佔用完，即沒有了 buffers 和 cache 時，才會使用到 swap 。
{% endhighlight %}

其實第一行可以從操作系統的角度去看，即 OS 認為自己總共有 total 的內存，分配了 used 的內存，還有 free 的內存；其中分配的內存中分別有 buffers 和 cached 的內存還沒有使用，也就是說 OS 認為還有空閒的內存，又懶得收回之前分配，但已經分配的不能再使用了。

第二行可以認為從進程的角度去看，可用的內存包括了空閒的+buffers+cached，total-空閒的就是時用的內存數。

可以通過如下方式計算：

{% highlight text %}
----- 實際可用內存大小
Free(-/+ buffers/cache) = Free(Mem) + buffers(Mem) + Cached(Mem)
                 185656 =      8908 +        21280 +      155468

----- 物理內存總大小
total(Mem) = used(-/+ buffers/cache) + free(-/+ buffers/cache)
    386024 =                  200368 +                  185656
           = used(Mem) + free(Mem)
           =    377116 +      8908

----- 已經分配的內存大小
Used(Mem) = Used(-/+ buffers/cache) + buffers(Mem) + Cached(Mem)
   377116 =                  200368 +        21280 +     155468
{% endhighlight %}

關於內存的其它信息可以參考如下文件。

{% highlight text %}
/proc/meminfo                  // 機器的內存使用信息
/proc/pid/maps                 // 顯示進程 PID 所佔用的虛擬地址
/proc/pid/statm                // 進程所佔用的內存
/proc/self/statm               // 當前進程的信息
{% endhighlight %}

#### 新版

如上所述，新版的命令行輸入如下。

{% highlight text %}
$ free -w
              total        used        free      shared     buffers       cache   available
Mem:        8070604     2928928      495012     1162676      104156     4542508     3624840
Swap:       8127484      232388     7895096

total     (MemTotal     @ /proc/meminfo)
free      (MemFree      @ /proc/meminfo)
shared    (Shmem        @ /proc/meminfo)  主要是tmpfs使用
buffers   (Buffers      @ /proc/meminfo)
cache     (Cached+Slab  @ /proc/meminfo)
available (MemAvailable @ /proc/meminfo)
used = total - free - buffers - cache     (注意，沒有去除shared)
{% endhighlight %}

可以通過 ```free -k -w && cat /proc/meminfo``` 命令進行測試。


### Swap對性能的影響

當內存不足的時候把磁盤的部分空間當作虛擬內存來使用，而實際上並非是在內存不足的時候才使用，有個參數叫做 swappiness，是用來控制 swap 分區使用的，可以直接查看 ```/proc/sys/vm/swappiness``` 文件。

默認值是 60，範圍是 [0, 100]；為 0 表示會最大限度使用物理內存，然後才是 swap；為 100 時，表示儘量使用 swap 分區，把內存上的數據及時的搬運到 swap 空間裡面。

分配太多的 Swap 空間會浪費磁盤空間，而 Swap 空間太少，則系統有可能會發生錯誤。當系統的物理內存用光了，系統就會跑得很慢，但仍能運行；但是如果 Swap 空間也用光了，那麼系統就會發生錯誤。

通常情況下，Swap 空間是物理內存的 2~2.5 倍，最小為 64M；這也根據不同的應用，有不同的配置：如果是桌面系統，則只需要較小的 Swap 空間；而大的服務器則視情況不同需要不同大小的 Swap 空間，一般來說對於 4G 以下的物理內存，配置 2 倍的 swap ，4G 以上配置 1 倍。

另外， Swap 分區的數量對性能也有很大的影響。因為 Swap 交換的操作是磁盤 IO 的操作，如果有多個 Swap 交換區， Swap 空間的分配會以輪流的方式操作於所有的 Swap ，這樣會大大均衡 IO 的負載，加快 Swap 交換的速度。

實際上，當空閒物理內存不多時，不一定表示系統運行狀態很差，因為內存的 cache 及 buffer 部分可以隨時被重用。**只有 swap 被頻繁調用，才是內存資源是否緊張的依據，可以通過 vmstat 查看**。

#### 狀態查看

可以使用 [getswapused.sh]({{ site.example_repository }}/linux/memory/getswapused.sh) 查看 swap 的使用情況，如果不使用 root 用戶則只會顯示當前用戶的 swap，其它用戶顯示為 0。

<!--
<br><br><h2>測試</h2><p>
通過執行 dd 查看緩存的變化。
<pre style="font-size:0.8em; face:arial;">
# free -w && dd if=/dev/zero of=foobar bs=4k count=40960 && free -w          // 寫入168M
</pre>
可以看到 cache 增長的數目與文件大小基本相符，而且 buffers 同樣由所增長；然後，通過 linux-ftools 中的工具查看。
<pre style="font-size:0.8em; face:arial;">
# linux-fincore foobar                    // cached_perc為100%
</pre>
該命令的執行過程大致如下。
<pre style="font-size:0.8em; face:arial;">
main()
  |-getpagesize()                         // 獲取系統中頁的大小
  |-open(path, O_RDONLY)                  // 只讀方式打開
  |-fstat(fd, &file_stat)                 // 查看文件屬性，主要是大小
    ....
    file_mmap = mmap((void *)0, file_stat.st_size, PROT_NONE, MAP_SHARED, fd, 0 );
    size_t calloc_size = (file_stat.st_size+page_size-1)/page_size;
    mincore_vec = calloc(1, calloc_size);
    if ( mincore(file_mmap, file_stat.st_size, mincore_vec) != 0 )
    ....
    if (mincore_vec[page_index]&1) {

            ++cached;
</pre>
頁面的大小是4K ，有了長度，我們就知道了，我們需要多少個int來存放結果。mmap建立映射關係，mincore獲取文件頁面的駐留情況，從起始地址開始，長度是filesize，結果保存在mincore_vec數組裡。如果mincore[page_index]&1 == 1,那麼表示該頁面駐留在內存中，否則沒有對應緩存。
-->

## 常用命令

介紹下與內存相關的命令。

### pmap

可以根據進程查看進程相關信息佔用的內存情況，可以通過 ```-x```、```-X```、```-XX``` 查看詳細信息。

{% highlight text %}
$ pmap -x $(pidof mysqld)
Address           Kbytes     RSS   Dirty Mode  Mapping
0000000000400000   37116    9120       0 r-x-- mysqld
0000000002a3e000     872     192      12 r---- mysqld
0000000002b18000     700     300     196 rw--- mysqld
0000000002bc7000     772     628     628 rw---   [ anon ]
00007f0df23ff000       4       0       0 -----   [ anon ]
00007f0df2400000   53248    8472    8472 rw---   [ anon ]
00007f0df5bfc000       4       0       0 -----   [ anon ]
... ...
00007fff995ca000     132      76      76 rw---   [ stack ]
00007fff995f2000       8       4       0 r-x--   [ anon ]
ffffffffff600000       4       0       0 r-x--   [ anon ]
---------------- ------- ------- -------
total kB          705216  141876  129408

輸出值: 顯示擴展格式
  Address : 虛擬內存開始地址；
  Kbytes  : 佔用內存的字節數，單位是KB；
  RSS     : (Resident Set Size)駐留內存的字節數(KB)；
  Dirty   : 髒頁的字節數（包括共享和私有的）(KB)；
  Mode    : 內存頁的權限(r=read, w=write, x=execute, s=shared, p=private)；
  Mapping : 佔用內存的文件、[anon] (分配的內存)、[stack] (堆棧)；

$ pmap -d $(pidof mysqld)
Address           Kbytes Mode  Offset           Device    Mapping
0000000000400000   23288 r-x-- 0000000000000000 0ca:00001 mysqld
0000000001cbe000     940 r---- 00000000016be000 0ca:00001 mysqld
0000000001da9000     680 rw--- 00000000017a9000 0ca:00001 mysqld
0000000001e53000     888 rw--- 0000000000000000 000:00000   [ anon ]
0000000001f31000   11680 rw--- 0000000000000000 000:00000   [ anon ]
00007fd784000000    4248 rw--- 0000000000000000 000:00000   [ anon ]
00007fd784426000   61288 ----- 0000000000000000 000:00000   [ anon ]
00007fd78c000000     132 rw--- 0000000000000000 000:00000   [ anon ]
00007fd78c021000   65404 ----- 0000000000000000 000:00000   [ anon ]
00007fd7d2c67000      12 rw-s- 0000000000000000 000:0000a [aio] (deleted)
00007fd7d2c6a000      12 rw-s- 0000000000000000 000:0000a [aio] (deleted)
00007fd7d2c6d000       4 rw-s- 0000000000000000 000:0000a [aio] (deleted)
00007fd7d2c6e000       4 rw--- 0000000000000000 000:00000   [ anon ]
00007fd7d2c6f000       4 r---- 000000000001f000 0ca:00001 ld-2.17.so
00007fd7d2c70000       4 rw--- 0000000000020000 0ca:00001 ld-2.17.so
00007fd7d2c71000       4 rw--- 0000000000000000 000:00000   [ anon ]
00007ffc8fef3000     132 rw--- 0000000000000000 000:00000   [ stack ]
mapped: 1248492K    writeable/private: 545308K    shared: 128K

輸出值: 顯示設備格式
  Offset: 文件偏移
  Device: 設備名 (major:minor)
最後一行輸出:
  mapped            : 該進程映射的虛擬地址空間大小，也就是該進程預先分配的虛擬內存大小，即ps出的vsz；
  writeable/private : 進程所佔用的私有地址空間大小，也就是該進程實際使用的內存大小，不含shared libraries；
  shared            : 進程和其他進程共享的內存大小；
假設上述結構導出到了/tmp/1文件中，可以通過如下命令計算：
  mapped: 所有行Kbytes字段的累加
    awk 'BEGIN{sum=0};{sum+=$2};END{print sum}' /tmp/1
  writeable/private: 模式為w+p，且不是s的內存
    awk 'BEGIN{sum=0};{if($3~/w/ && $3!~/s/) {sum+=$2}};END{print sum}' /tmp/1
  shared: 共享內存數
    awk 'BEGIN{sum=0};{if($3~/s/) {sum+=$2}};END{print sum}' /tmp/1
{% endhighlight %}

<!--

共享內存的庫實際包括了兩部分，分別是代碼 ```r----``` 以及數據 ```rw---``` 。

如果加載的插件比較多，那麼writeable/private非常大126M，而 top %MEM 只有 0.1M ？？？？

man 1 top
%MEM -- Memory usage (RES)
   A task's currently used share of available physical memory.

RES  --  Resident Memory Size (KiB)
   The non-swapped physical memory a task is using.

writeable/private 真正含義是什麼，為什麼大於 RSS ，而 -x 統計的 RSS 基本相同
-->


### vmstat

vmstat 不會將自己作為一個有效的進程。

{% highlight text %}
$ vmstat [options] [delay [count]]

常用選項：
  delay: 每多少秒顯示一次。
  count: 顯示多少次，默認為一次。
  -a: 顯示活躍和非活躍內存。
  -f: 顯示從系統啟動之後 forks 的數量；包括了 fork, vfork, clone 的系統調用，等價於創建的全部任務數。
  -m,--slabs: 顯示 slabinfo 。
  -n,--one-header: 只顯示一次，而不是週期性的顯示。
  -s,--stats: 顯示內存統計以及各種活動信息。
  -d,--disk: 顯示磁盤的相關統計。
  -D,--disk-sum: 顯示硬盤統計信息的摘要。
  -p,--partition device: 顯示某個磁盤的統計信息。
  -S,--unit char: 顯示輸出時的單位，1000(k),1024(K),1000*1000(m),1024*1024(M)，默認是 K，不會改變 swap 和 block 。

----- 每秒刷新一次
$ vmstat -S M 1
procs -----------memory---------- ---swap-- -----io---- --system-- ----cpu----
r  b   swpd   free   buff  cache   si   so    bi    bo   in    cs us sy id wa
3  0      0   1963    607   2359    0    0     0     0    0     1 32  0 68  0

各個段的含義為：
  procs 進程
    r: 等待運行的進程數，如果運行隊列過大，表示CPU很忙，一般會造成CPU使用率很高。
    b: 不可中斷睡眠(uninterruptible sleep)的進程數，也就是被阻塞的進程，通常為IO。
  memory 內存
    swpd: 虛擬內存(Virtual Memory)的使用量，應該是交換區。</li><li>
    free: 空閒內存數，同free命令中的free(Mem)。</li><li>
    buff: 同free命令，已分配未使用的內存大小。</li><li>
    cache: 同free命令，已分配未使用的內存大小。</li><li>
    inact: 非活躍(inactive)內存的大小，使用 -a 選項????。</li><li>
    active: 活躍(active)內存的大小，使用 -a 選項????。
  swap 交換區
    si: 每秒從交換區(磁盤)寫到內存的大小。
    so: 每秒寫入交換區(磁盤)的大小。
        如果這兩個值長期大於0，系統性能會受到影響，即我們平常所說的Thrashing(顛簸)。
  IO
    bi: 每秒讀取的塊數(blocks/s)。
    bo: 每秒寫入的塊數(blocks/s)。
  system 系統
    in: 每秒的中斷數，包括時鐘中斷。
    cs: 每秒上線文的交換數。
  CPU
    us: 用戶進程執行時間。
    sy: 系統進程執行時間。
    id: 空閒進程執行時間，2.5.41 之後含有 IO-wait 的時間。
    wa: 等待 IO 的時間，2.5.41 之後含有空閒任務。
{% endhighlight %}

### vmtouch

通過 [vmtouch offical site](http://hoytech.com/vmtouch/) 可以查看文件在內存中的佔比，可以下載 [本地源碼](/reference/linux/monitor/vmtouch.c) 。

使用參數可以直接通過 ```vmtouch -h``` 查看，如下是簡單的示例：

{% highlight text %}
----- 查看/etc目錄下有多少在緩存中
$ vmtouch /etc/
           Files: 2844
     Directories: 790
  Resident Pages: 274/9971  1M/38M  2.75%
         Elapsed: 0.15243 seconds

----- 查看一個大文件在緩存中的比例
$ vmtouch -v big-dataset.txt
big-dataset.txt
[                                                            ] 0/169321

           Files: 1
     Directories: 0
  Resident Pages: 0/169321  0/661M  0%
         Elapsed: 0.011822 seconds

----- 緩存部分內容到內存中，分別對應了文本和二進制文件
$ tail -n 10000 big-dataset.txt > /dev/null
$ hexdump -n 102400 big-dataset.txt > /dev/null

----- 再次查看緩存的文件，也就是62頁在內存中
$ vmtouch -v big-dataset.txt
big-dataset.txt
[o                                                           ] 62/169321

           Files: 1
     Directories: 0
  Resident Pages: 62/169321  248K/661M  0.0366%
         Elapsed: 0.003463 seconds

----- 將文件加載到內存中
$ vmtouch -vt big-dataset.txt
big-dataset.txt
[OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO] 169321/169321
           Files: 1
     Directories: 0
   Touched Pages: 169321 (661M)
         Elapsed: 1.7372 seconds

----- 手動從內存中刪除
$ vmtouch -ve big-dataset.txt
Evicting big-dataset.txt
           Files: 1
     Directories: 0
   Evicted Pages: 42116 (164M)
         Elapsed: 0.076824 seconds

----- 將目錄下的所有文件鎖定到內存中
$ vmtouch -dl /var/www/htdocs/critical/
{% endhighlight %}

在 Linux 中，主要是通過 posix_fadvise() 來清除緩存；通過 mincore() 判斷頁是否存在於緩存中。


<!---
Linux 內核模式使用 Buffer IO，以充分使用操作系統的 Page Cache；讀取時先檢查頁緩存裡面是否有需要的數據，沒有就從設備讀取，返回給用戶的同時，加到緩存一份；寫時，寫到緩存後返回系統調用，再由後臺的進程定期涮到磁盤去。

但是如果你的IO非常密集，就會出現問題。首先由於pagesize是4K，內存的利用效率比較低。其次緩存的淘汰算法很簡單，由操作系統自主進行，用戶不大好參與。當你的寫很多，超過系統內存的某個上限的時候，後臺的進程(swapd)要出來回收頁面，而且一旦回收的速度小於寫入的速度，就會出現不可預期的行為。

這裡面最大的問題是：當你使用的內存包括緩存，沒超過操作系統規定的上限的時候，操作系統選擇不作為，讓用戶充分使用緩存，從它的角度來看這樣效率最高。但是正是由於這種策略在實踐中會導致問題。

比如說MYSQL服務器，我們可以把數據直接走direct IO,但是它的日誌是走bufferio的。因為走directio需要對寫入文件的偏移和大小都要扇區對全，這對日誌系統來講太麻煩了。由於MYSQL是基於事務的，會涉及到大量的日誌動作，頻繁的寫入，然後fsync. 日誌一旦寫入磁盤，buffer page就沒用了，但是一直會在內存呆著，直到達到內存上限，引起操作系統突然大量回收
頁面，出現IO柱塞或者內存交換等負面問題。

那麼我們知道了困境在哪裡，我們可以主動避免這個現象的發生。有二種方法：
1. 日誌也走direct io,需要規模的修改MYSQL代碼，如percona就這麼做了，提供相應的patch。
2. 日誌還是走buffer io, 但是定期清除無用page cache.

第一張方法不是我們要討論的，我們重點討論第二種如何做：

我們在程序裡知道文件的句柄，是不是就可以很輕鬆的用：

    int posix_fadvise(int fd, off_t offset, off_t len, int advice);
    POSIX_FADV_DONTNEED The specified data will not be accessed in the near future.

來解決問題呢？
比如寫類似 posix_fadvise(fd, 0, len_of_file, POSIX_FADV_DONTNEED)；這樣的代碼來清掉文件所屬的緩存。

前面介紹的vmtouch就有這樣的功能，清某個文件的緩存。
vmtouch -ve logfile 就可以試驗，但是你會發現內存根本就沒下來，原因呢？
-->

### smem

Linux 使用的是虛擬內存 (virtual memory)，因此要準確的計算一個進程實際使用的物理內存就比較麻煩，如下是在計算內存使用率時比較重要的指標：

* RSS (Resident Set Size) 表示進程佔用的物理內存大小，可以使用 top 命令查詢；不過將各進程的 RSS 值相加，通常會超出整個系統的內存消耗，這是因為 RSS 中包含了各進程間共享的內存。
* PSS (Proportional Set Size) 它將共享內存的大小進行平均後，再分攤到各進程上去，相比來說更準確。
* USS (Unique Set Size) 也就是 PSS 中只屬於本進程的部分，只計算了進程獨自佔用的內存大小，不包含任何共享的部分。

可以通過 [smem](https://www.selenic.com/smem/) 查看，這是一個 Python 寫的程序，該程序也可以通過 matplotlib 輸出圖片；另外，同時提供了一個 smemcap.c 程序，可以打包嵌入式的相關數據，然後通過 ```-S/--source``` 指定打包文件。

在 CentOS 中，可以通過如下命令安裝。

{% highlight text %}
# yum --enablerepo=epel install smem python-matplotlib
{% endhighlight %}

<!--
{% highlight text %}
Show basic process information  smem
Show library-oriented view  smem -m
Show user-oriented view     smem -u
Show system view    smem -R 4G -K /path/to/vmlinux -w
Show totals and percentages     smem -t -p
Show different columns  smem -c "name user pss"
Sort by reverse RSS     smem -s rss -r
Show processes filtered by mapping  smem -M libxml
Show mappings filtered by process   smem -m -P [e]volution
Read data from capture tarball  smem --source capture.tar.gz
Show a bar chart labeled by pid     smem --bar pid -c "pss uss"
Show a pie chart of RSS labeled by name     smem --pie name -s rss
{% endhighlight %}
-->


### 進程內存使用量

簡單介紹下如何查看各個進程的內存使用量。

#### top

可以直接使用 top 命令後，查看 ```%MEM``` 的內容，表示該進程內容使用比例，可以通過 -u 指定用戶，也就是隻監控特定的用戶。

常用的命令有。

{% highlight text %}
P: 按%CPU使用率排行
T: 按TIME+排行
M: 按%MEM排行
{% endhighlight %}

#### ps

如下例所示：

{% highlight text %}
$ ps -e -o 'pid,comm,args,pcpu,rsz,vsz,stime,user,uid' | sort -nrk5
{% endhighlight %}

其中 rsz 為實際內存，上例實現按內存排序，由大到小。


## 小結

可以通過 free 查看整個系統內存的使用情況，free(Mem) 過小不會對系統的性能產生影響，實際剩餘的內存數為 free(-/+ buffers/cache)。

通過 vmstat 1 3 輸出的 swap(si/so) 可以查看交換區的切入/出情況，如果長期不為 0，則可能是因為內存不足，此時應該查看那個進程佔用內存較多。


## 參考

關於內存的介紹可以參考 [What every programmer should know about memory](https://www.akkadia.org/drepper/cpumemory.pdf)，包括了從硬件到軟件的詳細介紹，內容非常詳細。

關於 Linux 內存的介紹可以參考 [Linux ate my ram!](http://www.linuxatemyram.com/) 。

<!--
memtester 內存測試工具。

Linux ate my ram
http://www.linuxatemyram.com/
不錯介紹內存的文章
http://linuxperf.com/?cat=7
Linux內存中的Cache真的能被回收麼？
http://liwei.life/2016/04/26/linux%e5%86%85%e5%ad%98%e4%b8%ad%e7%9a%84cache%e7%9c%9f%e7%9a%84%e8%83%bd%e8%a2%ab%e5%9b%9e%e6%94%b6%e4%b9%88%ef%bc%9f/


舉例介紹 cache/buffers，比較有意思，介紹如何強制刷新 cache，<a href="http://www.linuxatemyram.com/">http://www.linuxatemyram.com/</a>，也可以參考 <a href="reference/block/Linux_ate_my_ram.html">本地版本</a> 。<br><br>

<a href="http://www.ibm.com/developerworks/cn/linux/l-cache/">Linux 內核的文件 Cache 管理機制介紹</a>，一篇很好的關於 cache 的詳解，可以參考 <a href="reference/block/Linux 內核的文件 Cache 管理機制介紹.html">本地文檔</a> 。<br><br>

<a href="http://blog.csdn.net/shanshanpt/article/details/39258373">Linux文件系統(五)---三大緩衝區之buffer塊緩衝區</a>，不錯的文章，介紹 buffer 的。
-->



{% highlight text %}
{% endhighlight %}
