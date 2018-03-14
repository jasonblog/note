---
title: Linux IO 調度器
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,io,scheduler
description: 簡單記錄一下一些與 Markdown 相關的內容，包括了一些使用模版。
---

用來決定塊設備上 IO 操作提交順序的方法，主要是用於提高吞吐量、降低響應時間。然而兩者是相互矛盾的，為了儘量平衡這兩者，Linux 內核提供了多種調度算法來適應不同的 IO 請求場景。

這裡簡單介紹下 Linux 中的 IO 調度器。

<!-- more -->

## Linux IO 調度算法

Linux 2.6 引入了新的 IO 調度子系統，其總體目標是希望讓磁頭可以沿著一個方向移動，移動到底了再往反方向走，這恰恰類似於現實生活中的電梯模型，所以對應的調度算法也就叫做電梯算法。

內核中的電梯算法包括了：AS(Anticipatory)、CFQ(Complete Fairness Queueing)、Deadline、NOOP(No Operation)，可以在啟動的時候通過內核參數指定，默認使用的是 CFQ 。

可以通過如下方式查看或者設置 IO 的調度算法。

{% highlight text %}
----- 查看當前系統支持的IO調度算法
# dmesg | grep -i scheduler
io scheduler noop registered
io scheduler anticipatory registered
io scheduler deadline registered
io scheduler cfq registered (default)

----- 查看當前系統的IO調度方法
$ cat /sys/block/BLOCK-DEVICE/queue/scheduler
noop anticipatory deadline [cfq]

----- 臨時修改IO調度方法
$ echo "noop" > /sys/block/BLOCK-DEVICE/queue/scheduler

----- 永久修改參數
# vim /boot/grub/menu.lst
kernel /boot/vmlinuz-2.6.32-504.el6 ro root=LABEL=/ elevator=deadline quiet
{% endhighlight %}

## NOOP

全稱為 No Operation，實際上就是實現了最簡單的 FIFO 隊列，所有 IO 請求基本會按照先進先出的規則操作，不過對於一些相鄰的還是做了 IO 請求合併，而非完全按照 FIFO 規則。

{% highlight text %}
----- 有如下的IO請求序列
100, 500, 101, 10, 56, 1000
----- 經過NOOP算法處理之後會按照如下順序處理
100(101), 500, 10, 56, 1000
{% endhighlight %}

這一算法是 2.4 之前版本的唯一調度算法，計算比較簡單，從而減少了 CPU 的使用，不過容易造成 IO 請求餓死。

<!---
關於IO餓死的描述如下：因為寫請求比讀請求更容易。寫請求通過文件系統cache，不需要等一次寫完成，就可以開始下一次寫操作，寫請求通過合併，堆積到I/O隊列中。讀請求需要等到它前面所有的讀操作完成，才能進行下一次讀操作。在讀操作之間有幾毫秒時間，而寫請求在這之間就到來 ，餓死了後面的讀請求 。其適用於SSD或Fusion IO環境下。
-->

其應用環境主要有以下兩種：一是物理設備中包含了自己的 IO 調度程序，比如 SCSI 的 TCQ；二是尋道時間可以忽略不計的設備，比如 SSD 等。

## CFQ

CFQ (Complete Fair Queuing) 完全公平的排隊。

該算法會按照 IO 請求的地址進行排序，而非按照 FIFO 的順序進行響應。該算法為每個進程/線程單獨創建一個隊列來管理該進程所產生的請求，各隊列之間的調度使用時間片來調度，以此來保證每個進程都能被很好的分配到 IO 帶寬。

這是一種 QoS 的 IO 調度算法，為每一個進程分配一個時間窗口，在該時間窗口內，允許進程發出 IO 請求。通過時間窗口在不同進程間的移動，保證了對於所有進程而言都有公平的發出 IO 請求的機會；同時 CFQ 也實現了進程的優先級控制，可保證高優先級進程可以獲得更長的時間窗口。

{% highlight text %}
----- 有如下的IO請求序列
100，500，101，10，56，1000
----- 經過CFQ算法處理之後會按照如下順序處理
100，101，500，1000，10，56
{% endhighlight %}

在傳統機械磁盤上，尋道會消耗絕大多數的 IO 響應時間，所以 CFQ 儘量減小磁盤的尋道時間。

CFQ 適用於系統中存在多任務 IO 請求的情況，通過在多進程中輪換，保證了系統 IO 請求整體的低延遲。但是，對於只有少數進程存在大量密集的 IO 請求的情況，會出現明顯的 IO 性能下降。

通過 CFQ 算法可以有效提高 SATA 盤的整體吞吐量，但是對於先來的 IO 請求並不一定能被滿足，也可能會出現餓死的情況，對於通用的服務器也是比較好的選擇。

CFQ 是 Deadline 和 AS 調度器的折中，對於多媒體應用和桌面系統是最好的選擇。

### 配置參數

CFQ 調度器主要提供如下參數。

{% highlight text %}
$ ls /sys/block/BLOCK-DEVICE/queue/iosched/

slice_idle
	如果一個進程在自己的時間窗口裡，經過 slice_idle 時間都沒有發送 IO 請求，則調度選擇下一個程序。
quantum
	該參數控制在一個時間窗口內可以發送的 IO 請求的最大數目。
low_latency
	對於 IO 請求延時非常重要的任務，可以打開低延遲模式來降低 IO 請求的延時。
{% endhighlight %}

## DEADLINE

在 CFQ 的基礎上，解決了 IO 請求可能會被餓死的極端情況，除了 CFQ 本身具有的 IO 排序隊列之外，還分別為讀 IO 和寫 IO 提供了 FIFO 隊列，讀 FIFO 隊列的最大等待時間為 500ms，寫 FIFO 隊列的最大等待時間為 5s。

也就是說針對 IO 請求的延時而設計，每個 IO 請求都被附加一個最後執行期限。該算法維護兩類隊列，一是按照扇區排序的讀寫請求隊列；二是按照過期時間排序的讀寫請求隊列。

如果當前沒有 IO 請求過期，則會按照扇區順序執行 IO 請求；如果發現過期的 IO 請求，則會處理按照過期時間排序的隊列，直到所有過期請求都被髮送為止；在處理請求時，該算法會優先考慮讀請求。

FIFO 隊列內的 IO 請求優先級要比 CFQ 隊列中的高，而讀 FIFO 隊列的優先級又比寫 FIFO 隊列的優先級高，也就是說 `FIFO(Read) > FIFO(Write) > CFQ` 。

<!--Deadline 確保了在一個截止時間內服務請求，這個截止時間是可調整的，為了防止了寫操作由於不能讀取而餓死，默認讀期限短於寫期限。 -->

### 參數配置

當系統中存在的 IO 請求進程數量比較少時，與 CFQ 算法相比，DEADLINE 算法可以提供較高的 IO 吞吐率，主要提供如下參數。

{% highlight text %}
$ ls /sys/block/BLOCK-DEVICE/queue/iosched/

writes_starved
	該參數控制當讀寫隊列均不為空時，發送多少個讀請求後，允許發射寫請求。
read_expire
	參數控制讀請求的過期時間，單位毫秒。
write_expire
	參數控制寫請求的過期時間，單位毫秒。
{% endhighlight %}

該算法適用於數據庫環境，如 MySQL、PostgreSQL、ES 等。

## Anticipatory

CFQ 和 DEADLINE 主要聚焦於一些零散的 IO 請求上，而對於連續的 IO 請求，比如順序讀寫，並沒有做優化。

該算法在 DEADLINE 的基礎上，為每個讀 IO 都設置了 6ms 的等待時間窗口，如果在這 6ms 內 OS 收到了相鄰位置的讀 IO 請求，就可以立即處理。

<!--
本質上與Deadline一樣，但在最後一次讀操作後，要等待6ms，才能繼續進行對其它I/O請求進行調度。可以從應用程序中預訂一個新的讀請求，改進讀操作的執行，但以一些寫操作為代價。它會在每個6ms中插入新的I/O操作，而會將一些小寫入流合併成一個大寫入流，用寫入延時換取最大的寫入吞吐量。AS適合於寫入較多的環境，比如文件服務器，但對對數據庫環境表現很差。
-->



<!--
讀測試
# time dd if=/dev/sda1 of=/dev/null bs=2M count=300
寫測試
# time dd if=/dev/zero of=/tmp/test bs=2M count=300


ionice可以更改任務的類型和優先級，不過只有cfq調度程序可以用ionice。

採用cfq的實時調度，優先級為7
ionice -c1 -n7 -ptime dd if=/dev/sda1 f=/tmp/test bs=2M count=300&
採用缺省的磁盤I/O調度，優先級為3
ionice -c2 -n3 -ptime dd if=/dev/sda1 f=/tmp/test bs=2M count=300&
採用空閒的磁盤調度，優先級為0
ionice -c3 -n0 -ptime dd if=/dev/sda1 f=/tmp/test bs=2M count=300&
ionice的磁盤調度優先級有8種，最高是0，最低是7。注意，磁盤調度的優先級與進程nice的優先級沒有關係。一個是針對進程I/O的優先級，一個是針對進程CPU的優先級。

七、總結

CFQ和DEADLINE考慮的焦點在於滿足零散IO請求上。對於連續的IO請求，比如順序讀，並沒有做優化。為了滿足隨機IO和順序IO混合的場景，Linux還支持ANTICIPATORY調度算法。ANTICIPATORY的在DEADLINE的基礎上，為每個讀IO都設置了6ms的等待時間窗口。如果在這6ms內OS收到了相鄰位置的讀IO請求，就可以立即滿足。

IO調度器算法的選擇，既取決於硬件特徵，也取決於應用場景。
在傳統的SAS盤上，CFQ、DEADLINE、ANTICIPATORY都是不錯的選擇；對於專屬的數據庫服務器，DEADLINE的吞吐量和響應時間都表現良好。然而在新興的固態硬盤比如SSD、Fusion IO上，最簡單的NOOP反而可能是最好的算法，因為其他三個算法的優化是基於縮短尋道時間的，而固態硬盤沒有所謂的尋道時間且IO響應時間非常短。

https://github.com/torvalds/linux/tree/master/Documentation/block
http://wiki.linuxquestions.org/wiki/IOSched
https://liwei.life/2016/03/14/linux_io_scheduler/
http://www.cnblogs.com/zhenjing/archive/2012/06/20/linux_writeback.html







<br><br><br><h1>IO 調度源碼分析</h1><p>
其中調度算法的更改等操作保存在 block/elevator.c 中，相關的調度算法為 block/*-iosched.c 。
<pre style="font-size:0.8em; face:arial;">
.elevator_dispatch_fn    取出
.elevator_add_req_fn     添加
</pre>
</p>
-->



## 參考

[Linux IO Stack Diagram](/reference/linux/linux-io-stack-diagram_v0.1.pdf) 文件系統與 IO 的調用層次。

<!--
https://www.xaprb.com/blog/2010/01/09/how-linux-iostat-computes-its-results/
http://hustcat.github.io/iostats/
http://ykrocku.github.io/blog/2014/04/11/diskstats/
https://www.percona.com/live/mysql-conference-2013/sites/default/files/slides/Monitoring-Linux-IO.pdf
-->

{% highlight text %}
{% endhighlight %}
