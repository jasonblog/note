---
title: Linux 監控之 IO
layout: post
language: chinese
comments: true
category: [linux]
keywords: linux,monitor,監控,IO,fio,iostat,iotop,pidstat,iodump,ioprofile,blktrace
description: 簡單介紹下 Linux 中與 IO 相關的內容。
---

簡單介紹下 Linux 中與 IO 相關的內容。

<!-- more -->

## 簡介

可以通過如下命令查看與 IO 相關的系統信息。

{% highlight text %}
# tune2fs -l /dev/sda7                       ← 讀取superblock信息
# blockdev --getbsz /dev/sda7                ← 獲取block大小
# tune2fs -l /dev/sda7 | grep "Block size"   ← 同上
# dumpe2fs /dev/sda7 | grep "Block size"     ← 同上
# stat /boot/ | grep "IO Block"              ← 同上
# fdisk -l                                   ← 硬盤的扇區大小(Sector Size)
{% endhighlight %}

> 在 [WiKi](https://en.wikipedia.org/wiki/Disk_formatting) 中的定義：A "block", a contiguous number of bytes, is the minimum unit of memory that is read from and written to a disk by a disk driver。

塊是文件系統的抽象，而非磁盤的屬性，一般是 Sector Size 的倍數；扇區大小則是磁盤的物理屬性，它是磁盤設備尋址的最小單元。另外，內核中要求 Block_Size = Sector_Size * (2的n次方)，且 Block_Size <= 內存的 Page_Size (頁大小)。


### 磁盤類型

主要是要獲取當前系統使用的什麼類型的磁盤 (SCSI、IDE、SSD等)，甚至是製造商、機器型號、序列號等信息。

{% highlight text %}
$ dmesg | grep scsi
{% endhighlight %}

### 監控指標

簡單列舉磁盤監控時常見的指標。

{% highlight text %}
IOPS 每秒IO數
  對磁盤來說，一次磁盤的連續讀或寫稱為一次磁盤 IO，當傳輸小塊不連續數據時，該指標有重要參考意義。
Throughput 吞吐量
  硬盤傳輸數據流的速度，單位一般為 MB/s，在傳輸大塊不連續數據的數據，該指標有重要參考作用。
IO平均大小
  實際上就是吞吐量除以 IOPS，用於判斷磁盤使用模式，一般大於 32K 為順序讀取為主，否則隨機讀取為主。
Utilization 磁盤活動時間百分比
  磁盤處於活動狀態 (數據傳輸、尋道等) 的時間百分比，也即磁盤利用率，一般該值越高對應的磁盤資源爭用越高。
Service Time 服務時間
  磁盤讀寫操作執行的時間，對於機械磁盤包括了尋道、旋轉、數據傳輸等，與磁盤性能相關性較高，另外，也受 CPU、內存影響。
Queue Length 等待隊列長度
  待處理的 IO 請求的數目，注意，如果該磁盤為磁盤陣列虛擬的邏輯驅動器，需要除以實際磁盤數，以獲取單盤的 IO 隊列。
Wait Time 等待時間
  在隊列中排隊的時間。
{% endhighlight %}


## iostat 系統級

除了可以通過該命令查看磁盤信息之外，還可以用來查看 CPU 信息，分別通過 ```-d``` 和 ```-c``` 參數控制；可直接通過 ```iostat -xdm 1``` 命令顯示磁盤隊列的長度等信息。

{% highlight text %}
Device: rrqm/s wrqm/s   r/s   w/s  rMB/s  wMB/s avgrq-sz avgqu-sz await r_await w_await svctm %util
sda       0.02   1.00  0.99  1.84   0.03   0.04    46.98     0.01  2.44    0.47    3.49  0.25  0.07
{% endhighlight %}

其中參數如下：

{% highlight text %}
rrqm/s wrqm/s
  讀寫請求每秒合併後發送給磁盤的請求。

r/s w/s
  應用發送給系統的請求數目。

argrq-sz
  提交給驅動層IO請求的平均大小(sectors)，一般不小於4K，不大於max(readahead_kb, max_sectors_kb)；
  可用於判斷當前的 IO 模式，越大代表順序，越小代表隨機；計算公式如下：
  argrq-sz = (rsec + wsec) / (r + w)

argqu-sz Average Queue Size
  在驅動層的隊列排隊的平均長度。

await Average Wait
  平均的等待時間，包括了在隊列中的等待時間，以及磁盤的處理時間。

svctm(ms) Service Time
  請求發送給IO設備後的響應時間，也就是一次磁盤IO請求的服務時間，不過該指標官網說不準確，要取消。
  對於單塊SATA盤，完全隨機讀時，基本在7ms左右，既尋道+旋轉延遲時間。

%util
  一秒內IO操作所佔的比例，計算公式是(r/s+w/s)*(svctm/1000)，例如採集時間為 1s 其中有 0.8s 在處
  理 IO 請求，那麼 util 為 80% ；對於一塊磁盤，如果沒有併發IO的概念，那麼這個公式是正確的，但
  是對於RAID磁盤組或者SSD來說，這個計算公式就有問題了，就算這個值超過100%，也不代表存儲有瓶頸，
  容易產生誤導。
{% endhighlight %}

iostat 統計的是通用塊層經過合併 (rrqm/s, wrqm/s) 後，直接向設備提交的 IO 數據，可以反映系統整體的 IO 狀況，但是距離應用層比較遠，由於系統預讀、Page Cache、IO調度算法等因素，很難跟代碼中的 write()、read() 對應。

簡言之，這是系統級，沒辦法精確到進程，比如只能告訴你現在磁盤很忙，但是沒辦法告訴你是那個進程在忙，在忙什麼？

<!--
rsec/s    : The number of sectors read from the hard disk per second
wsec/s    : The number of sectors written to the hard disk per second
-->

### /proc/diskstats

該命令會讀取 ```/proc/diskstats``` 文件，各個指標詳細的含義可以參考內核文檔 [iostats.txt](https://www.kernel.org/doc/Documentation/iostats.txt)，其中各個段的含義如下。

{% highlight text %}
filed1  rd_ios
  成功完成讀的總次數；
filed2  rd_merges
  合併寫完成次數，通過合併提高效率，例如兩次4K合併為8K，這樣只有一次IO操作；合併操作是由IO Scheduler(也叫 Elevator)負責。
filed3  rd_sectors
  成功讀過的扇區總次數；
filed4  rd_ticks
  所有讀操作所花費的毫秒數，每個讀從__make_request()開始計時，到end_that_request_last()為止，包括了在隊列中等待的時間；
filed5  wr_ios
  成功完成寫的總次數；
filed6  wr_merges
  合併寫的次數；
filed7  wr_sectors
  成功寫過的扇區總次數；
filed8  wr_ticks
  所有寫操作所花費的毫秒數；
filed9  in_flight
  現在正在進行的IO數目，在IO請求進入隊列時該值加1，在IO結束時該值減1，注意是在進出隊列時，而非交給磁盤時；
filed10 io_ticks
  輸入/輸出操作花費的毫秒數；
filed11 time_in_queue
  是一個權重值，當有上面的IO操作時，這個值就增加。
{% endhighlight %}

需要注意 ```io_ticks``` 與 ```rd/wr_ticks``` 的區別，後者是把每一個 IO 所消耗的時間累加在一起，因為硬盤設備通常可以並行處理多個 IO，所以統計值往往會偏大；而前者表示該設備有 IO 請求在處理的時間，也就是非空閒，不考慮 IO 有多少，只考慮現在有沒有 IO 操作。在實際計算時，會在字段 ```in_flight``` 不為零的時候 ```io_ticks``` 保持計時，為 0 時停止計時。

另外，```io_ticks``` 在統計時不考慮當前有幾個 IO，而 ```time_in_queue``` 是用當前的 IO 數量 (in_flight) 乘以時間，統計時間包括了在隊列中的時間以及磁盤處理 IO 的時間。

<!-- iostat在計算avgqu-sz時會用到這個字段。 -->


### 重要指標

簡單介紹下常見的指標，包括了經常誤解的指標。

#### util

這裡重點說一下 iostat 中 util 的含義，該參數可以理解為磁盤在處理 IO 請求的總時間，如果是 100% 則表明磁盤一直在處理 IO 請求，這也就意味著 IO 在滿負載運行。

對於一塊磁盤，如果沒有併發 IO 的概念，所以這個公式是正確的，但是現在的磁盤或者對於RAID磁盤組以及SSD來說，這個計算公式就有問題了，就算這個值超過100%，也不代表存儲有瓶頸，容易產生誤導。

舉個簡化的例子：某硬盤處理單個 IO 需要 0.1 秒，也就是有能力達到 10 IOPS，那麼當 10 個 IO 請求依次順序提交的時候，需要 1 秒才能全部完成，在 1 秒的採樣週期裡 %util 達到 100%；而如果 10 個 IO 請求一次性提交的話，0.1 秒就全部完成，在 1 秒的採樣週期裡 %util 只有 10%。

可見，即使 %util 高達 100%，硬盤也仍然有可能還有餘力處理更多的 IO 請求，即沒有達到飽和狀態。不過遺憾的是現在 iostat 沒有提供類似的指標。

在 CentOS 中使用的是 [github sysstat](https://github.com/sysstat/sysstat/blob/master/iostat.c)，如下是其計算方法。

{% highlight text %}
rw_io_stat_loop()  循環讀取
 |-read_diskstats_stat()            從/proc/diskstats讀取狀態
 |-write_stats()                    輸出採集的監控指標
   |-write_ext_stat()
     |-compute_ext_disk_stats()     計算ext選項，如util
     |-write_plain_ext_stat()
{% endhighlight %}

關於該參數的代碼詳細介紹如下。


{% highlight c %}
#define S_VALUE(m,n,p)  (((double) ((n) - (m))) / (p) * HZ)

void read_diskstats_stat(int curr)
{
 struct io_stats sdev;
 ... ...
 if ((fp = fopen(DISKSTATS, "r")) == NULL)
  return;

 while (fgets(line, sizeof(line), fp) != NULL) {
  /* major minor name rio rmerge rsect ruse wio wmerge wsect wuse running use aveq */
  i = sscanf(line, "%u %u %s %lu %lu %lu %lu %lu %lu %lu %u %u %u %u",
      &major, &minor, dev_name,
      &rd_ios, &rd_merges_or_rd_sec, &rd_sec_or_wr_ios, &rd_ticks_or_wr_sec,
      &wr_ios, &wr_merges, &wr_sec, &wr_ticks, &ios_pgr, &tot_ticks, &rq_ticks);

  if (i == 14) {
   /* Device or partition */
   if (!dlist_idx && !DISPLAY_PARTITIONS(flags) &&
       !is_device(dev_name, ACCEPT_VIRTUAL_DEVICES))
    continue;
   sdev.rd_ios     = rd_ios;
   sdev.rd_merges  = rd_merges_or_rd_sec;
   sdev.rd_sectors = rd_sec_or_wr_ios;
   sdev.rd_ticks   = (unsigned int) rd_ticks_or_wr_sec;
   sdev.wr_ios     = wr_ios;
   sdev.wr_merges  = wr_merges;
   sdev.wr_sectors = wr_sec;
   sdev.wr_ticks   = wr_ticks;
   sdev.ios_pgr    = ios_pgr;
   sdev.tot_ticks  = tot_ticks;
   sdev.rq_ticks   = rq_ticks;
        }
     ... ...
  save_stats(dev_name, curr, &sdev, iodev_nr, st_hdr_iodev);
 }
 fclose(fp);
}

void write_json_ext_stat(int tab, unsigned long long itv, int fctr,
      struct io_hdr_stats *shi, struct io_stats *ioi,
      struct io_stats *ioj, char *devname, struct ext_disk_stats *xds,
      double r_await, double w_await)
{
 xprintf0(tab,
   "{\"disk_device\": \"%s\", \"rrqm\": %.2f, \"wrqm\": %.2f, "
   "\"r\": %.2f, \"w\": %.2f, \"rkB\": %.2f, \"wkB\": %.2f, "
   "\"avgrq-sz\": %.2f, \"avgqu-sz\": %.2f, "
   "\"await\": %.2f, \"r_await\": %.2f, \"w_await\": %.2f, "
   "\"svctm\": %.2f, \"util\": %.2f}",
   devname,
   S_VALUE(ioj->rd_merges, ioi->rd_merges, itv),
   S_VALUE(ioj->wr_merges, ioi->wr_merges, itv),
   S_VALUE(ioj->rd_ios, ioi->rd_ios, itv),
   S_VALUE(ioj->wr_ios, ioi->wr_ios, itv),
   S_VALUE(ioj->rd_sectors, ioi->rd_sectors, itv) / fctr,
   S_VALUE(ioj->wr_sectors, ioi->wr_sectors, itv) / fctr,
   xds->arqsz,
   S_VALUE(ioj->rq_ticks, ioi->rq_ticks, itv) / 1000.0,
   xds->await,
   r_await,
   w_await,
   xds->svctm,
   shi->used ? xds->util / 10.0 / (double) shi->used
      : xds->util / 10.0); /* shi->used should never be zero here */
}


void compute_ext_disk_stats(struct stats_disk *sdc, struct stats_disk *sdp,
       unsigned long long itv, struct ext_disk_stats *xds)
{
 double tput
  = ((double) (sdc->nr_ios - sdp->nr_ios)) * HZ / itv;

 xds->util  = S_VALUE(sdp->tot_ticks, sdc->tot_ticks, itv);
 xds->svctm = tput ? xds->util / tput : 0.0;
 /*
  * Kernel gives ticks already in milliseconds for all platforms
  * => no need for further scaling.
  */
 xds->await = (sdc->nr_ios - sdp->nr_ios)
?  ((sdc->rd_ticks - sdp->rd_ticks) + (sdc->wr_ticks - sdp->wr_ticks)) /
  ((double) (sdc->nr_ios - sdp->nr_ios)) : 0.0;
 xds->arqsz = (sdc->nr_ios - sdp->nr_ios)
?  ((sdc->rd_sect - sdp->rd_sect) + (sdc->wr_sect - sdp->wr_sect)) /
  ((double) (sdc->nr_ios - sdp->nr_ios)) : 0.0;
}
{% endhighlight %}


實際上就是 ```/proc/diskstats``` 中的 filed10 消耗時間佔比。


#### await

在 Linux 中，每個 IO 的平均耗時用 await 表示，包括了磁盤處理時間以及隊列排隊時間，所以該指標不能完全表示設備的性能，包括 IO 調度器等，都會影響該參數值。一般來說，內核中的隊列時間幾乎可以忽略不計，而 SSD 不同產品從 0.01ms 到 1.00 ms 不等，對於機械磁盤可以參考 [io](http://cseweb.ucsd.edu/classes/wi01/cse102/sol2.pdf) 。


#### svctm

這個指標在 iostat 以及 sar 上都有註釋 ```Warning! Do not trust this field any more. This field will be removed in a future sysstat version.```，該指標包括了隊列中排隊時間以及磁盤處理時間。

實際上，在 UNIX 中通常通過 avserv 表示硬盤設備的性能，它是指 IO 請求從 SCSI 層發出到 IO 完成之後返回 SCSI 層所消耗的時間，不包括在 SCSI 隊列中的等待時間，所以該指標體現了硬盤設備處理 IO 的速度，又被稱為 disk service time，如果 avserv 很大，那麼肯定是硬件出問題了。

#### iowait

從 top 中的解釋來說，就是 CPU 在 ```time waiting for I/O completion``` 中消耗的時間，而實際上，如果需要等待 IO 完成，實際 CPU 不會一直等待該進程，而是切換到另外的進程繼續執行。

所以在 [Server Fault](https://serverfault.com/questions/12679/can-anyone-explain-precisely-what-iowait-is) 中將該指標定義為如下的含義：

{% highlight text %}
iowait is time that the processor/processors are waiting (i.e. is in an
idle state and does nothing), during which there in fact was outstanding
disk I/O requests.
{% endhighlight %}

那麼對於多核，iowait 是隻在一個 CPU 上，還是會消耗在所有 CPU ？如果有 4 個 CPUs，那麼最大是 20% 還是 100% ？

可以通過 ```dd if=/dev/sda of=/dev/null bs=1MB``` 命令簡單測試下，一般來說，為了提高 cache 的命中率，會一直使用同一個 CPU ，不過部分系統會將其均分到不同的 CPU 上做均衡。另外，也可以通過 ```taskset 1 dd if=/dev/sda of=/dev/null bs=1MB``` 命令將其綁定到單個 CPU 上。

> 按照二進制形式，從最低位到最高位代表物理 CPU 的 #0、#1、#2、...、#n 號核，例如：0x01 代表 CPU 的 0 號核，0x05 代表 CPU 的 0 號和 2 號核。
>
> 例如，將 9865 綁定到 #0、#1 上面，命令為 ```taskset -p 0x03 9865```；將進程 9864 綁定到 #1、#2、#5~#11 號核上面，從 1 開始計數，命令為 ```taskset -cp 1,2,5-11 9865``` 。

可以看出，如果是 ```top <1>``` 顯示各個 CPU 的指標，則是 100% 計算，而總的統計值則按照 25% 統計。

<!--
http://veithen.github.io/2013/11/18/iowait-linux.html
-->

### 其它

常見問題處理。

##### 問題1：如何獲取真正的 serviice time(svctm)

?可以通過 fio 等壓測工具，通過設置為同步 IO，僅設置一個線程，以及 io_depth 也設置為 1，壓測出來的就是真正的 service time(svctm)。

##### 問題2：怎樣獲得 IO 最大並行度，或者說如何獲得真正的 util% 使用率？

{% highlight text %}
最大並行度 = 壓測滿(r/s + w/s) * (真實svctm / 1000)
{% endhighlight %}

公式基本一樣，只是將 svctm 換成了上次計算的值。

##### 問題3：如何判斷存在 IO 瓶頸了？

實際上在如上算出真實的最大並行度，可以直接參考 avgqu-sz 值，也就是隊列中的值，一般來說超過兩倍可能就會存在問題。例如一塊機械盤，串行 IO (每次1個IO)，那麼 avgqu-sz 持續大於 2 既代表持續有兩倍讀寫能力的 IO 請求在等待；或者當 RAIDs、SSD 等並行，這裡假定並行度為 5.63，那麼 avgqu-sz 持續大於10，才代表持續有兩倍讀寫能力的 IO 請求在等待。

<!--
https://www.symantec.com/connect/articles/getting-hang-iops
-->

## iotop pidstat iodump 進程級

一個 Python 腳本，可以查看官網 [guichaz.free.fr/iotop](http://guichaz.free.fr/iotop/)，另一個通過 C 實現的監控可參考 [IOPP](https://github.com/markwkm/iopp)。

pidstat 用於統計進程的狀態，包括 IO 狀況，可以查看當前系統哪些進程在佔用 IO 。

{% highlight text %}
----- 只顯示IO
# pidstat -d 1
{% endhighlight %}

上述兩者均是統計的 ```/proc/pid/io``` 中的信息；另可參考 io/iotop.stp，這是 iotop 的複製版。

iodump 是一個統計每一個進程(線程)所消耗的磁盤 IO 工具，是一個 perl 腳本，其原理是打開有關 IO 的內核記錄消息開關，而後讀取消息然後分析輸出。

{% highlight text %}
# echo 1 >/proc/sys/vm/block_dump                        # 打開有關IO內核消息的開關
# while true; do sleep 1; dmesg -c ; done | perl iodump  # 然後分析
{% endhighlight %}

上述輸出的單位為塊 (block)，每塊的大小取決於創建文件系統時指定的塊大小。


## ioprofile 業務級

ioprofile 命令本質上等價於 lsof + strace，可以查看當前進程。

<!--
具體下載可見 http://code.google.com/p/maatkit/

ioprofile 可以回答你以下三個問題:
1  當前進程某時間內,在業務層面讀寫了哪些文件(read, write)？
2  讀寫次數是多少?(read, write的調用次數)
3  讀寫數據量多少?(read, write的byte數)
假設某個行為會觸發程序一次IO動作,例如: "一個頁面點擊,導致後臺讀取A,B,C文件"
-->


## blktrace

blktrace 是塊層 IO 路徑監控和分析工具，作者 Jens Axboe 是內核 IO 模塊的維護者，目前就職於 FusionIO，同時他還是著名 IO 評測工具 fio 的作者，使用它可以深入瞭解 IO 通路。

{% highlight text %}
# yum install blktrace                    # 在CentOS中安裝
$ make                                    # 解壓源碼後直接安裝
$ man -l doc/blktrace.8                   # 查看幫助
{% endhighlight %}

其源碼可以從 [brick.kernel.dk](http://brick.kernel.dk/snaps/) 下載，詳細使用參考 [blktrace User Guide](http://www.cse.unsw.edu.au/~aaronc/iosched/doc/blktrace.html) 。

### 原理

該工具包括了內核空間和用戶空間兩部分實現，內核空間裡主要是給塊層 IO 路徑上的關鍵點添加 tracepoint，然後藉助於 relayfs 系統特性將收集到的數據寫到 buffer 去，再從用戶空間去收集。

目前，內核空間部分的代碼已經集成到主線代碼裡面去了，可以看看內核代碼 block/blktrace.c 文件是不是存在，編譯的時候把對應的這個 trace 選項選擇上就可以了。

{% highlight text %}
$ grep 'CONFIG_BLK_DEV_IO_TRACE' /boot/config-`uname -r`
{% endhighlight %}

大部分實現代碼都在 blktrace.c，利用 tracepoint 的特性，註冊了一些 trace 關鍵點，可以查看 Documentation/tracepoint.txt 文件；交互機制利用了 relayfs 特性，看看 Documentation/filesystems/relay.txt 。

此時撈取的信息還比較原始，可以通過用戶空間的 blkparse、btt、seekwatcher 這樣的工具來分析收集到的數據。

注意，使用之前要確保 debugfs 已經掛載，默認會掛載在 /sys/kernel/debug 。

### 使用

典型的使用如下，其中 /dev/sdaa、/dev/sdc 作為 LVM volume adb3/vol。

{% highlight text %}
# blktrace -d /dev/sda -o - | blkparse -i - -o blkparse.out       # 簡單用法，Ctrl-C退出
# btrace /dev/sda                                                 # 同上

# blktrace /dev/sdaa /dev/sdc &                                   # 離線處理。1. 後臺運行採集
% mkfs -t ext3 /dev/adb3/vol                                      # 2. 做些IO操作
% kill -15 9713                                                   # 3. 停止採集
% blkparse sdaa sdc sdo > events                                  # 4. 解析後查看
{% endhighlight %}

在 blktrace 中，-d 表示監控哪個設備，-o - 表示將監控輸出到標準輸出；在 blkparse 中，-i - 表示從標準輸入獲取信息，-o 表示將解析的內容記錄在 blkparse.out 。

如下是輸出的詳細信息。

![monitor io blktrace]({{ site.url }}/images/linux/monitor-io-blktrace.png "monitor io blktrace"){: .pull-center width="80%"}

其中 event 對應了事件表；後面一列代表了操作類型，包括了 R(read)、W(write)、B(barrier operation)、S(synchronous operation)，其中 event 有如下類型：

|事件|說明|源碼(block目錄下) SetPosition|
|:---:|:---|:---|
|A |IO was remapped to a different device | blk-core.c/trace_block_remap |
|B |IO bounced                            | bounce.c/trace_block_bio_bounce |
|C |IO completion                         | blk-core.c/trace_block_rq_complete |
|D |IO issued to driver                   | elevator.c/trace_block_rq_issue |
|F |IO front merged with request on queue | blk-core.c/trace_block_bio_frontmerge |
|G |Get request                           | blk-core.c/trace_block_getrq |
|I |IO inserted onto request queue        | elevator.c/trace_block_rq_insert |
|M |IO back merged with request on queue  | blk-core.c/trace_block_bio_backmerge |
|P |Plug request                          | blk-core.c/trace_block_plug |
|Q |IO handled by request queue code      | blk-core.c/trace_block_bio_queue |
|S |Sleep request                         | blk-core.c/trace_block_sleeprq |
|T |Unplug due to timeout                 | blk-core.c/trace_block_unplug_timer |
|U |Unplug request                        | blk-core.c/trace_block_unplug_io |
|X |Split                                 | bio.c/trace_block_split |


### 詳解

仍以如下簡單命令為例。

{% highlight text %}
$ blktrace -d /dev/sda -o sda                 # 輸出 sda.blktrace.N 文件，N 為物理 CPU 個數。
$ ls /sys/kernel/debug/block/sda              # 查看debugfs中的文件
dropped  msg  trace0  trace1  trace2  trace3
$ blkparse -i sda.blktrace.0                  # 解析成可讀內容
$ blkrawverify sda                            # 校驗，其中sda為blktrace的-o選項
{% endhighlight %}

其中 blktrace 通過 ioctl() 執行 BLKTRACESETUP、BLKTRACESTART、BLKTRACESTOP、BLKTRACETEARDOWN 操作，此時會在 debugfs 目錄的 block/DEV 目錄下寫入數據。

<!--
<pre style="font-size:0.8em; face:arial;">
main()
   run_tracers()
      setup_buts()
      use_tracer_devpaths()
      start_tracers()
         start_tracer()          通過pthread_create()創建
            thread_main()
               open_ios()
               handle_pfds()     對於寫入本地文件實際採用handle_pfds_entries()
</pre>
  struct io_info 輸入輸出結構
-f "%D %2c %8s %5T.%9t %5p %2a %3d "
-->


## FIO

FIO 是個非常強大的 IO 性能測試工具，其作者 Jens Axboe 是 Linux 內核 IO 部分的 maintainer，可以毫不誇張的說，如果你把所有的 FIO 參數都搞明白了，基本上就把 Linux IO 協議棧的問題搞的差不多明白了。

一個 IO 壓測工具，源碼以及二進制文件可以參考 [github-axboe](https://github.com/axboe/fio/)，或者直接從 [freecode.com](http://freecode.com/projects/fio/) 上下載。另外，該工具同時提供了一個圖形界面 gfio 。

在 CentOS 中可以通過如下方式安裝。

{% highlight text %}
# yum --enablerepo=epel install fio
{% endhighlight %}

<!-- fio2gnuplot.py fio_generate_plots -->

<!--
fio --name=global --rw=randread --size=128m --name=job1
job1: (g=0): rw=randread, bs=4K-4K/4K-4K/4K-4K, ioengine=sync, iodepth=1
fio-2.1.2
Starting 1 process
job1: Laying out IO file(s) (1 file(s) / 128MB)
Jobs: 1 (f=1): [r] [100.0% done] [6228KB/0KB/0KB /s] [1557/0/0 iops] [eta 00m:00s]
job1: (groupid=0, jobs=1): err= 0: pid=122727: Fri Mar 31 10:52:45 2017
  read : io=131072KB, bw=4485.9KB/s, iops=1121, runt= 29224msec
    clat (usec): min=250, max=72421, avg=888.51, stdev=932.95
     lat (usec): min=250, max=72422, avg=888.73, stdev=932.96
    clat percentiles (usec):
     |  1.00th=[  406],  5.00th=[  434], 10.00th=[  450], 20.00th=[  474],
     | 30.00th=[  494], 40.00th=[  516], 50.00th=[  548], 60.00th=[  604],
     | 70.00th=[  756], 80.00th=[ 1144], 90.00th=[ 1960], 95.00th=[ 2544],
     | 99.00th=[ 3632], 99.50th=[ 4448], 99.90th=[ 6496], 99.95th=[ 8096],
     | 99.99th=[34560]
    bw (KB  /s): min= 2691, max= 6088, per=99.97%, avg=4483.69, stdev=724.50
    lat (usec) : 500=32.24%, 750=37.59%, 1000=7.10%
    lat (msec) : 2=13.41%, 4=8.95%, 10=0.68%, 20=0.01%, 50=0.01%
    lat (msec) : 100=0.01%
  cpu          : usr=0.42%, sys=2.05%, ctx=32789, majf=0, minf=22
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued    : total=r=32768/w=0/d=0, short=r=0/w=0/d=0

Run status group 0 (all jobs):
   READ: io=131072KB, aggrb=4485KB/s, minb=4485KB/s, maxb=4485KB/s, mint=29224msec, maxt=29224msec

Disk stats (read/write):
  xvde: ios=32737/21354, merge=0/113646, ticks=27852/53296, in_queue=81132, util=95.60%


首先，每個任務的第一行非常明確，如下也就是讀取(131072KB=128MB)數據，速率是4.48MB/s，其中IOPS是1121(4K block size)，執行了2.94秒。
  read : io=131072KB, bw=4485.9KB/s, iops=1121, runt= 29224msec

接下來是latency的計量值，在老版本中，先是slat(submission latency)也就是將本次的IO提交到內核所消耗的時間，不過在新版本中該統計值已經取消。
    slat (usec): min=3, max=335, avg= 9.73, stdev= 5.76

下面是clat(completion latency)，也就是提交到內核後到IO完成的時間，不包括上述的slat時間，老版本中這個是從應用層比較接近的延遲時間，而新版本中可以參考lat。
    clat (usec): min=250, max=72421, avg=888.51, stdev=932.95
 
lat(latency)這個統計值是從fio創建結構體開始，到IO完成的時間值，所以，也就是最接近從應用層看到的延遲時間。
     lat (usec): min=250, max=72422, avg=888.73, stdev=932.96

接下來，是clat的分佈情況，TODODO: 嘛意思啊
    clat percentiles (usec):
     |  1.00th=[  406],  5.00th=[  434], 10.00th=[  450], 20.00th=[  474],
     | 30.00th=[  494], 40.00th=[  516], 50.00th=[  548], 60.00th=[  604],
     | 70.00th=[  756], 80.00th=[ 1144], 90.00th=[ 1960], 95.00th=[ 2544],
     | 99.00th=[ 3632], 99.50th=[ 4448], 99.90th=[ 6496], 99.95th=[ 8096],
     | 99.99th=[34560]

Bandwidth就比較好看了，只是per有些疑問TODODO:
    bw (KB  /s): min= 2691, max= 6088, per=99.97%, avg=4483.69, stdev=724.50

接下來是延遲的分佈，也就是32.24%的請求小於500us，37.59%的請求小於750us，以此類推。TODODO:(37.59%是不是大於500us且小於500us的)
    lat (usec) : 500=32.24%, 750=37.59%, 1000=7.10%
    lat (msec) : 2=13.41%, 4=8.95%, 10=0.68%, 20=0.01%, 50=0.01%
    lat (msec) : 100=0.01%

CPU使用率、上下文切換context switch，以及major/minor page fault，如果使用 direct IO 時，page fault 會比較少。
  cpu          : usr=0.42%, sys=2.05%, ctx=32789, majf=0, minf=22
 
這個IO depths實際上完全是應用的行為，也就是一次扔給系統的IO請求數，更過的請求實際內核可以利用電梯算法等進行優化處理。
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%

https://tobert.github.io/post/2014-04-17-fio-output-explained.html

https://segmentfault.com/a/1190000003880571

https://www.linux.com/learn/inspecting-disk-io-performance-fio

應用使用IO通常有二種方式：同步和異步。 同步的IO一次只能發出一個IO請求，等待內核完成才返回，這樣對於單個線程iodepth總是小於1，但是可以透過多個線程併發執行來解決，通常我們會用16-32根線程同時工作把iodepth塞滿。 異步的話就是用類似libaio這樣的linux native aio一次提交一批，然後等待一批的完成，減少交互的次數，會更有效率。


http://blog.yufeng.info/archives/2104  包括了Cgroup的隔離、MySQL模擬等等

-->


### 源碼編譯

可以直接從 github 上下載源碼，然後通過如下方式進行編譯。

{% highlight text %}
----- 編譯，注意依賴libaio
$ make

----- 查看幫助
$ man -l fio.1

----- 通過命令行指定參數，進行簡單測試
$ fio --name=global --rw=randread --size=128m --name=job1 --name=job2

----- 也可以通過配置文件進行測試
$ cat foobar.fio
[global]
rw=randread
size=128m
[job1]
[job2]
$ fio foobar.fio
{% endhighlight %}

可以通過命令行啟動，不過此時參數較多，可以使用配置文件。

<!--
{% highlight text %}
# 順序讀
fio --filename=/dev/sda --direct=1 --rw=read --bs=4k --size=10G --numjobs=30 --runtime=20 --group_reporting --name=test-read

順序寫

fio --filename=/dev/sda --direct=1 --rw=write --bs=4k --size=10G --numjobs=30 --runtime=20 --group_reporting --name=test-write

隨機讀測試

fio --filename=/dev/sda --direct=1 --rw=randread --bs=4k --size=10G --numjobs=64 --runtime=20 --group_reporting --name=test-rand-read

隨機寫測試

fio --filename=/dev/sda --direct=1 --rw=randwrite --bs=4k --size=10G --numjobs=64 --runtime=20 --group_reporting --name=test-rand-write


ioengine=psync            # 設置io引擎，通過-i查看當前支持的
rw=randwrite              # 讀寫模型，包括了順序讀寫、隨機讀寫等
bs=16k                    # 單次io的塊文件大小為16k
direct=1                  # 測試過程繞過機器自帶的buffer，使測試結果更真實
size=5g                   # 指定測試文件的大小




filename=/dev/sdb1       測試文件名稱，通常選擇需要測試的盤的data目錄。
bsrange=512-2048         同上，提定數據塊的大小範圍
numjobs=30               本次的測試線程為30.
runtime=1000             測試時間為1000秒，如果不寫則一直將5g文件分4k每次寫完為止。
rwmixwrite=30            在混合讀寫的模式下，寫佔30%
group_reporting          關於顯示結果的，彙總每個進程的信息。

此外
lockmem=1g               只使用1g內存進行測試。
zero_buffers             用0初始化系統buffer。
nrfiles=8                每個進程生成文件的數量
{% endhighlight %}

應用使用IO通常有二種方式：同步和異步。 同步的IO一次只能發出一個IO請求，等待內核完成才返回，這樣對於單個線程iodepth總是小於1，但是可以透過多個線程併發執行來解決，通常我們會用16-32根線程同時工作把iodepth塞滿。 異步的話就是用類似libaio這樣的linux native aio一次提交一批，然後等待一批的完成，減少交互的次數，會更有效率。
-->


### 源碼解析

其版本通過 FIO_VERSION 宏定義，並通過 fio_version_string 變量定義。

{% highlight text %}
main()
  |-parse_options()
  |  |-parse_cmd_line()                    解析命令行，如-i顯示所有的ioengines
  |  |  |-add_job()                        file1: xxxxxx 打印job信息
  |  |-log_info()                          fio-2.10.0
  |-fio_backend()
  |  |-create_disk_util_thread()           用於實時顯示狀態
  |  |  |-setup_disk_util()
  |  |  |-disk_thread_main()               通過pthread創建線程
  |  |     |-print_thread_status()
  |  |
  |  |-run_threads()                       Starting N processes
  |  |  |-setup_files()                    Laying out IO file(s)
  |  |  |-pthread_create()                 如果配置使用線程，調用thread_main
  |  |  |-fork()                           或者調用創建進程，同樣為thread_main
  |  |
  |  |-show_run_stats()
  |     |-show_thread_status_normal()      用於顯示最終的狀態
  |        |-show_latencies()              顯示lat信息
  |        |-... ...                       CPU、IO depth
{% endhighlight %}

ioengines 通過 fio_libaio_register() 類似的函數初始化。





## 其它

### ionice

獲取或設置程序的 IO 調度與優先級。

{% highlight text %}
ionice [-c class] [-n level] [-t] -p PID...
ionice [-c class] [-n level] [-t] COMMAND [ARG]

----- 獲取進程ID為89、91的IO優先級
$ ionice -p 89 91
{% endhighlight %}

<!--
<P><STRONG>ilde</STRONG><STRONG>：</STRONG>空閒磁盤調度，該調度策略是在當前系統沒有其他進程需要進行磁盤IO時，才能進行磁盤；因此該策略對當前系統的影響基本為0；當然，該調度策略不能帶有任何優先級參數；目前，普通用戶是可以使用該調度策略（自從內核2.6.25開始）。</P>
<P><STRONG>Best effort</STRONG><STRONG>：</STRONG>是缺省的磁盤IO調度策略；(1)該調度策略可以指定優先級參數(範圍是0~7，數值越小，優先級越高)；(2)針對處於同一優先級的程序將採round-robin方式；(3)對於best effort調度策略，8個優先級等級可以說明在給定的一個調度窗口中時間片的大小。(4)目前，普調用戶(非root用戶)是可以使用該調度策略。(5)在內核2.6.26之前，沒有設置IO優先級的進程會使用“none”作為調度策略，但是這種策略使得進程看起來像是採用了best effort調度策略，因為其優先級是通過關於cpu nice有關的公式計算得到的：io_priority = (cpu_nice + 20) / 5。(6)在內核2.6.26之後，如果當前系統使用的是CFQ調度器，那麼如果進程沒有設置IO優先級級別，將採用與內核2.6.26之前版本同樣的方式，推到出io優先級級別。</P>
<P><STRONG>Real time</STRONG><STRONG>：</STRONG>實時調度策略，如果設置了該磁盤IO調度策略，則立即訪問磁盤，不管系統中其他進程是否有IO。因此使用實時調度策略，需要注意的是，該訪問策略可能會使得其他進程處於等待狀態。</P>

<P>&nbsp;</P>
<P><STRONG>參數說明：</STRONG></P>
<P>-c class ：class表示調度策略，其中0 for none, 1 for real time, 2 for best-effort, 3 for idle。</P>
<P>-n classdata：classdata表示IO優先級級別，對於best effort和real time，classdata可以設置為0~7。</P>
<P>-p pid：指定要查看或設置的進程號或者線程號，如果沒有指定pid參數，ionice will run the listed program with the given parameters。</P>
<P>-t ：忽視設置優先級時產生的錯誤。</P>
<P>COMMAND：表示命令名</P>
<P>&nbsp;</P>
<P><STRONG>實例：</STRONG></P>
<P># ionice -c 3 -p 89</P>
<P>設置進程號為89的進程的調度策略是idle。</P>
<P>&nbsp;</P>
<P># ionice -c 2 -n 0 bash</P>
<P>運行bash，調度策略是best-effort，最高優先級。</P>
<P>&nbsp;</P>
<P># ionice -p 89 91</P>
<P>打印進程號為89和91進程的調度策略和IO優先級。</P>
<P>&nbsp;</P>
<P>#ionice -c3 -p$$</P>
<P>將當前的進程(就是shell)磁盤IO調度策略設置為idle類型.</P>
-->



## 參考

關於 FIO 可以查看源碼中的 [HOWTO](/reference/linux/monitor/HOWTO.txt)，其它的壓測工具可以參考 [Benchmarking](https://wiki.mikejung.biz/Benchmarking) ，或者參考 [本地文檔](/reference/linux/monitor/Benchmarking.mht)，該網站還包括了很多有用文章。

[Block IO Layer Tracing: blktrace](/reference/linux/monitor/gelato_ICE06apr_blktrace_brunelle_hp.pdf) 介紹 blktrace 命令的使用；關於內核的 trace 功能參考 [Kernel Trace Systems](http://elinux.org/Kernel_Trace_Systems) 。

[Linux Block IO: Introducing Multi-queue SSD Access on Multi-core Systems](/reference/linux/monitor/linux-block-io-multi-queue-ssd.pdf) 。

<!--
http://www.cnblogs.com/quixotic/p/3258730.html

http://hustcat.github.io/iostats/
http://ykrocku.github.io/blog/2014/04/11/diskstats/
http://www.udpwork.com/item/12931.html

The precise meaning of I/O wait time in Linux
http://veithen.github.io/2013/11/18/iowait-linux.html
-->



 

{% highlight text %}
{% endhighlight %}
