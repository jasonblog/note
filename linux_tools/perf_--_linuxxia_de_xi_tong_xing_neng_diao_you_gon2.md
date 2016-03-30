# Perf -- Linux下的系統性能調優工具，第 2 部分


###特殊用法以及內核調優示例

perf event 是一款隨 Linux 內核代碼一同發佈和維護的性能診斷工具，由內核社區維護和發展。perf 不僅可以用於應用程序的性能統計分析，也可以應用於內核代碼的性能統計和分析。得益於其優秀的體系結構設計，越來越多的新功能被加入 perf，使其已經成為一個多功能的性能統計工具集 。第二部分將介紹 perf 在內核代碼開發上的應用。

##本文內容簡介
之前介紹了 perf 最常見的一些用法，關注於 Linux 系統上應用程序的調優。現在讓我們把目光轉移到內核以及其他 perf 命令上面來。

在內核方面，人們的興趣五花八門，有些內核開發人員熱衷於尋找整個內核中的熱點代碼；另一些則只關注某一個主題，比如 slab 分配器，對於其餘部分則不感興趣。對這些人而言，perf 的一些奇怪用法更受歡迎。當然，諸如 perf top，perf stat, perf record 等也是內核調優的基本手段，但用法和 part1 所描述的一樣，無需重述

此外雖然內核事件對應用程序開發人員而言有些陌生，但一旦瞭解，對應用程序的調優也很有幫助。我曾經參與開發過一個數據庫應用程序，其效率很低。通過常規的熱點查詢，IO 統計等方法，我們找到了一些可以優化的地方，以至於將程序的效率提高了幾倍。可惜對於擁有海量數據的用戶，其運行時間依然無法達到要求。進一步調優需要更加詳細的統計信息，可惜本人經驗有限，實在是無計可施。。。從客戶反饋來看，該應用的使用頻率很低。作為一個程序員，為此我時常心情沮喪。。。

假如有 perf，那麼我想我可以用它來驗證自己的一些猜測，比如是否太多的系統調用，或者系統中的進程切換太頻繁 ? 針對這些懷疑使用 perf 都可以拿出有用的報告，或許能找到問題吧。但過去的便無可彌補，時光不會倒流，無論我如何傷感，世界絕不會以我的意志為轉移。所以我們好好學習 perf，或許可以預防某些遺憾吧。

這裡我還要提醒讀者注意，講述 perf 的命令和語法容易，但說明什麼時候使用這些命令，或者說明怎樣解決實際問題則很困難。就好象說明電子琴上 88 個琴鍵的唱名很容易，但想說明如何彈奏動聽的曲子則很難。

在簡述每個命令語法的同時，我試圖通過一些示例來說明這些命令的使用場景，但這隻能是一種微薄的努力。因此總體說來，本文只能充當那本隨同電子琴一起發售的使用說明書。。。

## 使用 tracepoint

當 perf 根據 tick 時間點進行採樣後，人們便能夠得到內核代碼中的 hot spot。那什麼時候需要使用 tracepoint 來採樣呢？

我想人們使用 tracepoint 的基本需求是對內核的運行時行為的關心，如前所述，有些內核開發人員需要專注於特定的子系統，比如內存管理模塊。這便需要統計相關內核函數的運行情況。另外，內核行為對應用程序性能的影響也是不容忽視的：

以之前的遺憾為例，假如時光倒流，我想我要做的是統計該應用程序運行期間究竟發生了多少次系統調用。在哪裡發生的？

下面我用 ls 命令來演示 sys_enter 這個 tracepoint 的使用：
```sh
 [root@ovispoly /]# perf stat -e raw_syscalls:sys_enter ls 
 bin dbg etc  lib  media opt root  selinux sys usr 
 boot dev home lost+found mnt proc sbin srv  tmp var 

  Performance counter stats for 'ls': 

 101 raw_syscalls:sys_enter 

  0.003434730 seconds time elapsed 


 [root@ovispoly /]# perf record -e raw_syscalls:sys_enter ls 

 [root@ovispoly /]# perf report 
 Failed to open .lib/ld-2.12.so, continuing without symbols 
 # Samples: 70 
 # 
 # Overhead Command Shared Object Symbol 
 # ........ ............... ............... ...... 
 # 
 97.14% ls ld-2.12.so [.] 0x0000000001629d 
 2.86% ls [vdso] [.] 0x00000000421424 
 # 
 # (For a higher level overview, try: perf report --sort comm,dso) 
 #
 ```
 
 這個報告詳細說明了在 ls 運行期間發生了多少次系統調用 ( 上例中有 101 次 )，多數系統調用都發生在哪些地方 (97% 都發生在 ld-2.12.so 中 )。
 
有了這個報告，或許我能夠發現更多可以調優的地方。比如函數 foo() 中發生了過多的系統調用，那麼我就可以思考是否有辦法減少其中有些不必要的系統調用。

您可能會說 strace 也可以做同樣事情啊，的確，統計系統調用這件事完全可以用 strace 完成，但 perf 還可以幹些別的，您所需要的就是修改 -e 選項後的字符串。

羅列 tracepoint 實在是不太地道，本文當然不會這麼做。但學習每一個 tracepoint 是有意義的，類似背單詞之於學習英語一樣，是一項緩慢痛苦卻不得不做的事情。

##perf probe
tracepoint 是靜態檢查點，意思是一旦它在哪裡，便一直在那裡了，您想讓它移動一步也是不可能的。內核代碼有多少行？我不知道，100 萬行是至少的吧，但目前 tracepoint 有多少呢？我最大膽的想象是不超過 1000 個。所以能夠動態地在想查看的地方插入動態監測點的意義是不言而喻的。

Perf 並不是第一個提供這個功能的軟件，systemTap 早就實現了。但假若您不選擇 RedHat 的發行版的話，安裝 systemTap 並不是件輕鬆愉快的事情。perf 是內核代碼包的一部分，所以使用和維護都非常方便。

我使用的 Linux 版本為 2.6.33。因此您自己做實驗時命令參數有可能不同。

```sh
[root@ovispoly perftest]# perf probe schedule:12 cpu 
 Added new event: 
 probe:schedule (on schedule+52 with cpu) 

 You can now use it on all perf tools, such as: 

   perf record -e probe:schedule -a sleep 1 

 [root@ovispoly perftest]# perf record -e probe:schedule -a sleep 1 
 Error, output file perf.data exists, use -A to append or -f to overwrite. 

 [root@ovispoly perftest]# perf record -f -e probe:schedule -a sleep 1 
 [ perf record: Woken up 1 times to write data ] 
 [ perf record: Captured and wrote 0.270 MB perf.data (~11811 samples) ] 
 [root@ovispoly perftest]# perf report 
 # Samples: 40 
 # 
 # Overhead Command Shared Object Symbol 
 # ........ ............... ................. ...... 
 # 
 57.50% init 0 [k] 0000000000000000 
 30.00% firefox [vdso] [.] 0x0000000029c424 
 5.00% sleep [vdso] [.] 0x00000000ca7424 
 5.00% perf.2.6.33.3-8 [vdso] [.] 0x00000000ca7424 
 2.50% ksoftirqd/0 [kernel] [k] 0000000000000000 
 # 
 # (For a higher level overview, try: perf report --sort comm,dso) 
 #
 ```
 
 上例利用 probe 命令在內核函數 schedule() 的第 12 行處加入了一個動態 probe 點，和 tracepoint 的功能一樣，內核一旦運行到該 probe 點時，便會通知 perf。可以理解為動態增加了一個新的 tracepoint。
 
此後便可以用 record 命令的 -e 選項選擇該 probe 點，最後用 perf report 查看報表。如何解讀該報表便是見仁見智了，既然您在 shcedule() 的第 12 行加入了 probe 點，想必您知道自己為什麼要統計它吧？

##Perf sched
調度器的好壞直接影響一個系統的整體運行效率。在這個領域，內核黑客們常會發生爭執，一個重要原因是對於不同的調度器，每個人給出的評測報告都各不相同，甚至常常有相反的結論。因此一個權威的統一的評測工具將對結束這種爭論有益。Perf sched 便是這種嘗試。

Perf sched 有五個子命令：


```sh
perf sched record            # low-overhead recording of arbitrary workloads 
perf sched latency           # output per task latency metrics 
perf sched map               # show summary/map of context-switching 
perf sched trace             # output finegrained trace 
perf sched replay            # replay a captured workload using simlated threads
```

用戶一般使用’ perf sched record ’收集調度相關的數據，然後就可以用’ perf sched latency ’查看諸如調度延遲等和調度器相關的統計數據。

其他三個命令也同樣讀取 record 收集到的數據並從其他不同的角度來展示這些數據。下面一一進行演示。


```sh
perf sched record sleep 10     # record full system activity for 10 seconds 
 perf sched latency --sort max  # report latencies sorted by max 

 -------------------------------------------------------------------------------------
  Task               |   Runtime ms  | Switches | Average delay ms | Maximum delay ms | 
 -------------------------------------------------------------------------------------
  :14086:14086        |      0.095 ms |        2 | avg:    3.445 ms | max:    6.891 ms | 
  gnome-session:13792   |   31.713 ms |      102 | avg:    0.160 ms | max:    5.992 ms | 
  metacity:14038      |     49.220 ms |      637 | avg:    0.066 ms | max:    5.942 ms | 
  gconfd-2:13971     | 48.587 ms |      777 | avg:    0.047 ms | max:    5.793 ms | 
  gnome-power-man:14050 |  140.601 ms | 434 | avg:  0.097 ms | max:    5.367 ms | 
  python:14049        |  114.694 ms |      125 | avg:    0.120 ms | max:    5.343 ms | 
  kblockd/1:236       |   3.458 ms |      498 | avg:    0.179 ms | max:    5.271 ms | 
  Xorg:3122         |   1073.107 ms |     2920 | avg:    0.030 ms | max:    5.265 ms | 
  dbus-daemon:2063   |   64.593 ms |      665 | avg:    0.103 ms | max:    4.730 ms | 
  :14040:14040       |   30.786 ms |      255 | avg:    0.095 ms | max:    4.155 ms | 
  events/1:8         |    0.105 ms |       13 | avg:    0.598 ms | max:    3.775 ms | 
  console-kit-dae:2080  | 14.867 ms |   152 | avg:    0.142 ms | max:    3.760 ms | 
  gnome-settings-:14023 |  572.653 ms |  979 | avg:    0.056 ms | max:    3.627 ms | 
 ... 
 -----------------------------------------------------------------------------------
  TOTAL:                |   3144.817 ms |    11654 | 
 --------------------------------------------------- 

```

上面的例子展示了一個 Gnome 啟動時的統計信息。
各個 column 的含義如下：
- Task: 進程的名字和 pid 
- Runtime: 實際運行時間
- Switches: 進程切換的次數
- Average delay: 平均的調度延遲
- Maximum delay: 最大延遲


這裡最值得人們關注的是 Maximum delay，一般從這裡可以看到對交互性影響最大的特性：調度延遲，如果調度延遲比較大，那麼用戶就會感受到視頻或者音頻斷斷續續的。

其他的三個子命令提供了不同的視圖，一般是由調度器的開發人員或者對調度器內部實現感興趣的人們所使用。

首先是 map:

```sh
  $ perf sched map 
  ... 

   N1  O1  .   .   .   S1  .   .   .   B0  .  *I0  C1  .   M1  .    23002.773423 secs 
   N1  O1  .  *Q0  .   S1  .   .   .   B0  .   I0  C1  .   M1  .    23002.773423 secs 
   N1  O1  .   Q0  .   S1  .   .   .   B0  .  *R1  C1  .   M1  .    23002.773485 secs 
   N1  O1  .   Q0  .   S1  .  *S0  .   B0  .   R1  C1  .   M1  .    23002.773478 secs 
  *L0  O1  .   Q0  .   S1  .   S0  .   B0  .   R1  C1  .   M1  .    23002.773523 secs 
   L0  O1  .  *.   .   S1  .   S0  .   B0  .   R1  C1  .   M1  .    23002.773531 secs 
   L0  O1  .   .   .   S1  .   S0  .   B0  .   R1  C1 *T1  M1  .    23002.773547 secs 
                       T1 => irqbalance:2089 
   L0  O1  .   .   .   S1  .   S0  .  *P0  .   R1  C1  T1  M1  .    23002.773549 secs 
  *N1  O1  .   .   .   S1  .   S0  .   P0  .   R1  C1  T1  M1  .    23002.773566 secs 
   N1  O1  .   .   .  *J0  .   S0  .   P0  .   R1  C1  T1  M1  .    23002.773571 secs 
   N1  O1  .   .   .   J0  .   S0 *B0  P0  .   R1  C1  T1  M1  .    23002.773592 secs 
   N1  O1  .   .   .   J0  .  *U0  B0  P0  .   R1  C1  T1  M1  .    23002.773582 secs 
   N1  O1  .   .   .  *S1  .   U0  B0  P0  .   R1  C1  T1  M1  .    23002.773604 secs
```

星號表示調度事件發生所在的 CPU。
點號表示該 CPU 正在 IDLE。

Map 的好處在於提供了一個的總的視圖，將成百上千的調度事件進行總結，顯示了系統任務在 CPU 之間的分佈，假如有不好的調度遷移，比如一個任務沒有被及時遷移到 idle 的 CPU 卻被遷移到其他忙碌的 CPU，類似這種調度器的問題可以從 map 的報告中一眼看出來。

如果說 map 提供了高度概括的總體的報告，那麼 trace 就提供了最詳細，最底層的細節報告。


```sh
pipe-test-100k-13520 [001]  1254.354513808: sched_stat_wait: 
task: pipe-test-100k:13521 wait: 5362 [ns] 
pipe-test-100k-13520 [001]  1254.354514876: sched_switch: 
task pipe-test-100k:13520 [120] (S) ==> pipe-test-100k:13521 [120] 
:13521-13521 [001]  1254.354517927: sched_stat_runtime: 
task: pipe-test-100k:13521 runtime: 5092 [ns], vruntime: 133967391150 [ns] 
:13521-13521 [001]  1254.354518984: sched_stat_sleep: 
task: pipe-test-100k:13520 sleep: 5092 [ns] 
:13521-13521 [001]  1254.354520011: sched_wakeup: 
task pipe-test-100k:13520 [120] success=1 [001]
 ```
 
 要理解以上的信息，必須對調度器的源代碼有一定了解，對一般用戶而言，理解他們十分不易。幸好這些信息一般也只有編寫調度器的人感興趣。。。
 
Perf replay 這個工具更是專門為調度器開發人員所設計，它試圖重放 perf.data 文件中所記錄的調度場景。很多情況下，一般用戶假如發現調度器的奇怪行為，他們也無法準確說明發生該情形的場景，或者一些測試場景不容易再次重現，或者僅僅是出於“偷懶”的目的，使用 perf replay，perf 將模擬 perf.data 中的場景，無需開發人員花費很多的時間去重現過去，這尤其利於調試過程，因為需要一而再，再而三地重複新的修改是否能改善原始的調度場景所發現的問題。

下面是 replay 執行的示例：

```sh
$ perf sched replay 
 run measurement overhead: 3771 nsecs 
 sleep measurement overhead: 66617 nsecs 
 the run test took 999708 nsecs 
 the sleep test took 1097207 nsecs 
 nr_run_events:        200221 
 nr_sleep_events:      200235 
 nr_wakeup_events:     100130 
 task      0 (                perf:     13519), nr_events: 148 
 task      1 (                perf:     13520), nr_events: 200037 
 task      2 (      pipe-test-100k:     13521), nr_events: 300090 
 task      3 (         ksoftirqd/0:         4), nr_events: 8 
 task      4 (             swapper:         0), nr_events: 170 
 task      5 (     gnome-power-man:      3192), nr_events: 3 
 task      6 (     gdm-simple-gree:      3234), nr_events: 3 
 task      7 (                Xorg:      3122), nr_events: 5 
 task      8 (     hald-addon-stor:      2234), nr_events: 27 
 task      9 (               ata/0:       321), nr_events: 29 
 task     10 (           scsi_eh_4:       704), nr_events: 37 
 task     11 (            events/1:         8), nr_events: 3 
 task     12 (            events/0:         7), nr_events: 6 
 task     13 (           flush-8:0:      6980), nr_events: 20 
 ------------------------------------------------------------ 
 #1  : 2038.157, ravg: 2038.16, cpu: 0.09 / 0.09 
 #2  : 2042.153, ravg: 2038.56, cpu: 0.11 / 0.09 
 ^C
 ```
##perf bench
除了調度器之外，很多時候人們都需要衡量自己的工作對系統性能的影響。benchmark 是衡量性能的標準方法，對於同一個目標，如果能夠有一個大家都承認的 benchmark，將非常有助於”提高內核性能”這項工作。
目前，就我所知，perf bench 提供了 3 個 benchmark:

```sh
1. Sched message
[lm@ovispoly ~]$ perf bench sched messaging 
# Running sched/messaging benchmark...# 20 sender and receiver processes per group# 10 groups == 400 processes run Total time: 1.918 [sec]sched message 是從經典的測試程序 hackbench 移植而來，用來衡量調度器的性能，overhead 以及可擴展性。該 benchmark 啟動 N 個 reader/sender 進程或線程對，通過 IPC(socket 或者 pipe) 進行併發的讀寫。一般人們將 N 不斷加大來衡量調度器的可擴展性。Sched message 的用法及用途和 hackbench 一樣。

2. Sched Pipe
[lm@ovispoly ~]$ perf bench sched pipe
# Running sched/pipe benchmark...# Extecuted 1000000 pipe operations between two tasks Total time: 20.888 [sec] 20.888017 usecs/op 47874 ops/secsched pipe 從 Ingo Molnar 的 pipe-test-1m.c 移植而來。當初 Ingo 的原始程序是為了測試不同的調度器的性能和公平性的。其工作原理很簡單，兩個進程互相通過 pipe 拼命地發 1000000 個整數，進程 A 發給 B，同時 B 發給 A。。。因為 A 和 B 互相依賴，因此假如調度器不公平，對 A 比 B 好，那麼 A 和 B 整體所需要的時間就會更長。

3. Mem memcpy

[lm@ovispoly ~]$ perf bench mem memcpy
# Running mem/memcpy benchmark...# Copying 1MB Bytes from 0xb75bb008 to 0xb76bc008 ... 364.697301 MB/Sec這個是 perf bench 的作者 Hitoshi Mitake 自己寫的一個執行 memcpy 的 benchmark。該測試衡量一個拷貝 1M 數據的 memcpy() 函數所花費的時間。我尚不明白該 benchmark 的使用場景。。。或許是一個例子，告訴人們如何利用 perf bench 框架開發更多的 benchmark 吧。


這三個 benchmark 給我們展示了一個可能的未來：不同語言，不同膚色，來自不同背景的人們將來會採用同樣的 benchmark，只要有一份 Linux 內核代碼即可。
 ```
 
 ### perf lock
 
 鎖是內核同步的方法，一旦加了鎖，其他準備加鎖的內核執行路徑就必須等待，降低了並行。因此對於鎖進行專門分析應該是調優的一項重要工作。
 
我運行 perf lock 後得到如下輸出：

```sh
Name acquired contended total wait (ns) max wait (ns) min 

 &md->map_lock 396 0 0 0 
 &(&mm->page_tabl... 309 0 0 0 
 &(&tty->buf.lock... 218 0 0 0 
 &ctx->lock 185 0 0 0 
 key 178 0 0 0 
 &ctx->lock 132 0 0 0 
 &tty->output_loc... 126 0 0 0 
。。。
 &(&object->lock)... 1 0 0 0 
 &(&object->lock)... 0 0 0 0 
 &(&object->lock)... 0 0 0 0 
 &p->cred_guard_m... 0 0 0 0 

 === output for debug=== 

 bad: 28, total: 664 
 bad rate: 4.216867 % 
 histogram of events caused bad sequence 
  acquire: 8 
  acquired: 0 
  contended: 0 
  release: 20
```
對該報表的一些解釋如下：
- “Name”: 鎖的名字，比如 md->map_lock，即定義在 dm.c 結構 mapped_device 中的讀寫鎖。
- “acquired”: 該鎖被直接獲得的次數，即沒有其他內核路徑擁有該鎖的情況下得到該鎖的次數。
- “contended”衝突的次數，即在準備獲得該鎖的時候已經被其他人所擁有的情況的出現次數。
- “total wait”：為了獲得該鎖，總共的等待時間。
- “max wait”：為了獲得該鎖，最大的等待時間。
- “min wait”：為了獲得該鎖，最小的等待時間。

目前 perf lock 還處於比較初級的階段，我想在後續的內核版本中，還應該會有較大的變化，因此當您開始使用 perf lock 時，恐怕已經和本文這裡描述的有所不同了。不過我又一次想說的是，命令語法和輸出並不是最重要的，重要的是瞭解什麼時候我們需要用這個工具，以及它能幫我們解決怎樣的問題。

### perf Kmem

Perf Kmem 專門收集內核 slab 分配器的相關事件。比如內存分配，釋放等。可以用來研究程序在哪裡分配了大量內存，或者在什麼地方產生碎片之類的和內存管理相關的問題。

Perf kmem 和 perf lock 實際上都是 perf tracepoint 的特例，您也完全可以用 Perf record – e kmem:* 或者 perf record – e lock:* 來完成同樣的功能。但重要的是，這些工具在內部對原始數據進行了彙總和分析，因而能夠產生信息更加明確更加有用的統計報表。

perf kmem 的輸出結果如下：


```sh
[root@ovispoly perf]# ./perf kmem --alloc -l 10 --caller stat 
 --------------------------------------------------------------------------- 
 Callsite       | Total_alloc/Per | Total_req/Per | Hit | Ping-pong| Frag 
 --------------------------------------------------------------------------- 
 perf_mmap+1a8 | 1024/1024 | 572/572|1 | 0 | 44.141% 
 seq_open+15| 12384/96 | 8772/68 |129 | 0 | 29.167% 
 do_maps_open+0| 1008/16 | 756/12 |63 | 0 | 25.000% 
 ...| ... | ...| ... | ... | ... 
 __split_vma+50| 88/88 | 88/88 | 1 | 0 | 0.000% 
 --------------------------------------------------------------------------- 
  Alloc Ptr | Total_alloc/Per | Total_req/Per | Hit |Ping-pong| Frag 
 --------------------------------------------------------------------------- 
 0xd15d4600|64/64 | 33/33  1 |  0 | 48.438% 
 0xc461e000|1024/1024 | 572/572 |1 | 0 | 44.141% 
 0xd15d44c0| 64/64 | 38/38 |1 | 0 | 40.625% 
 ... | ... | ... | ... | ... | ... 
 --------------------------------------------------------------------------- 

 SUMMARY 
 ======= 
 Total bytes requested: 10487021 
 Total bytes allocated: 10730448 
 Total bytes wasted on internal fragmentation: 243427 
 Internal fragmentation: 2.268563% 
 Cross CPU allocations: 0/246458
 ```
 
 該報告有三個部分：根據 Callsite 顯示的部分，所謂 Callsite 即內核代碼中調用 kmalloc 和 kfree 的地方。比如上圖中的函數 perf_mmap，Hit 欄為 1，表示該函數在 record 期間一共調用了 kmalloc 一次，假如如第三行所示數字為 653，則表示函數 sock_alloc_send_pskb 共有 653 次調用 kmalloc 分配內存。
 
對於第一行 Total_alloc/Per 顯示為 1024/1024，第一個值 1024 表示函數 perf_mmap 總共分配的內存大小，Per 表示平均值。

比較有趣的兩個參數是 Ping-pong 和 Frag。Frag 比較容易理解，即內部碎片。雖然相對於 Buddy System，Slab 正是要解決內部碎片問題，但 slab 依然存在內部碎片，比如一個 cache 的大小為 1024，但需要分配的數據結構大小為 1022，那麼有 2 個字節成為碎片。Frag 即碎片的比例。

Ping-pong 是一種現象，在多 CPU 系統中，多個 CPU 共享的內存會出現”乒乓現象”。一個 CPU 分配內存，其他 CPU 可能訪問該內存對象，也可能最終由另外一個 CPU 釋放該內存對象。而在多 CPU 系統中，L1 cache 是 per CPU 的，CPU2 修改了內存，那麼其他的 CPU 的 cache 都必須更新，這對於性能是一個損失。Perf kmem 在 kfree 事件中判斷 CPU 號，如果和 kmalloc 時的不同，則視為一次 ping-pong，理想的情況下 ping-pone 越小越好。Ibm developerworks 上有一篇講述 oprofile 的文章，其中關於 cache 的調優可以作為很好的參考資料。

後面則有根據被調用地點的顯示方式的部分。

最後一個部分是彙總數據，顯示總的分配的內存和碎片情況，Cross CPU allocation 即 ping-pong 的彙總。

### Perf timechart

很多 perf 命令都是為調試單個程序或者單個目的而設計。有些時候，性能問題並非由單個原因所引起，需要從各個角度一一查看。為此，人們常需要綜合利用各種工具，比如 top,vmstat,oprofile 或者 perf。這非常麻煩。

此外，前面介紹的所有工具都是基於命令行的，報告不夠直觀。更令人氣餒的是，一些報告中的參數令人費解。所以人們更願意擁有一個“傻瓜式”的工具。
以上種種就是 perf timechart 的夢想，其靈感來源於 bootchart。採用“簡單”的圖形“一目瞭然”地揭示問題所在。

加註了引號的原因是，perf timechart 雖然有了美觀的圖形輸出，但對於新手，這個圖形就好象高科技節目中播放的 DNA 圖像一樣，不明白那些坐在屏幕前的人是如何從密密麻麻的點和線中找到有用的信息的。但正如受過訓練的科學家一樣，經過一定的練習，相信您也一定能從下圖中找到您想要的。

圖 1. perf timechart


![](./images/image001.jpg)

人們說，只有黑白兩色是一個人內心壓抑的象徵，Timechart 用不同的顏色代表不同的含義。上圖的最上面一行是圖例，告訴人們每種顏色所代表的含義。藍色表示忙碌，紅色表示 idle，灰色表示等待，等等。

接下來是 per-cpu 信息，上圖所示的系統中有兩個處理器，可以看到在採樣期間，兩個處理器忙碌程度的概括。藍色多的地方表示忙碌，因此上圖告訴我們，CPU1 很忙，而 CPU2 很閒。

再下面是 per-process 信息，每一個進程有一個 bar。上圖中進程 bash 非常忙碌，而其他進程則大多數時間都在等待著什麼。Perf 自己在開始的時候很忙，接下來便開始 wait 了。

總之這張圖告訴了我們一個系統的概況，但似乎不夠詳細？
Timechart 可以顯示更詳細的信息，上圖實際上是一個矢量圖形 SVG 格式，用 SVG viewer 的放大功能，我們可以將該圖的細節部分放大，timechart 的設計理念叫做”infinitely zoomable”。放大之後便可以看到一些更詳細的信息，類似網上的 google 地圖，找到國家之後，可以放大，看城市的分佈，再放大，可以看到某個城市的街道分佈，還可以放大以便得到更加詳細的信息。

完整的 timechart 圖形和顏色解讀超出了本文的範圍，感興趣的讀者可以到作者 Arjan 的博客上查看。這裡僅舉一個例子，上圖中有一條 bar 對應了 Xorg 進程。多數時候該進程都處於 waiting 狀態，只有需要顯示什麼的時候它才會開始和內核通信，以便進行繪圖所需的 IO 操作。

將 Xorg 條目放大的例子圖形如下：

圖 2. perf timechart detail
 
![](./images/image002.jpg)

上圖中需要注意的是幾條綠色的短線，表示進程通信，即準備繪圖。假如通信的兩個進程在圖中上下相鄰，那麼綠線可以連接他們。但如果不相鄰，則只會顯示如上圖所示的被截斷的綠色短線。

藍色部分表示進程忙碌，黃色部分表示該進程的時間片已經用完，但仍處於就緒狀態，在等待調度器給予 CPU。

通過這張圖，便可以較直觀地看到進程在一段時間內的詳細行為。

## 使用 Script 增強 perf 的功能

通常，面對看似複雜，實則較有規律的計算機輸出，程序員們總是會用腳本來進行處理：比如給定一個文本文件，想從中找出有多少個數字 0125，人們不會打開文件然後用肉眼去一個一個地數，而是用 grep 命令來進行處理。

perf 的輸出雖然是文本格式，但還是不太容易分析和閱讀。往往也需要進一步處理，perl 和 python 是目前最強大的兩種腳本語言。Tom Zanussi 將 perl 和 python 解析器嵌入到 perf 程序中，從而使得 perf 能夠自動執行 perl 或者 python 腳本進一步進行處理，從而為 perf 提供了強大的擴展能力。因為任何人都可以編寫新的腳本，對 perf 的原始輸出數據進行所需要的進一步處理。這個特性所帶來的好處很類似於 plug-in 之於 eclipse。

下面的命令可以查看系統中已經安裝的腳本：

```sh
 # perf trace -l 
    List of available trace scripts: 
      syscall-counts [comm]                system-wide syscall counts 
      syscall-counts-by-pid [comm]         system-wide syscall counts, by pid 
      failed-syscalls-by-pid [comm]        system-wide failed syscalls, by pid 
。。。
```

比如 failed-syscalls 腳本，執行的效果如下：

```sh
 # perf trace record failed-syscalls 
    ^C[ perf record: Woken up 11 times to write data ]                         
    [ perf record: Captured and wrote 1.939 MB perf.data (~84709 samples) ]   

 perf trace report failed-syscalls 
    perf trace started with Perl script \ 
	 /root/libexec/perf-core/scripts/perl/failed-syscalls.pl 

    failed syscalls, by comm: 

    comm                    # errors 
    --------------------  ---------- 
    firefox                     1721 
    claws-mail                   149 
    konsole                       99 
    X                             77 
    emacs                         56 
    [...] 

    failed syscalls, by syscall: 

    syscall                           # errors 
    ------------------------------  ---------- 
    sys_read                              2042 
    sys_futex                              130 
    sys_mmap_pgoff                          71 
    sys_access                              33 
    sys_stat64                               5 
    sys_inotify_add_watch                    4 
    [...]
```

該報表分別按進程和按系統調用顯示失敗的次數。非常簡單明瞭，而如果通過普通的 perf record 加 perf report 命令，則需要自己手工或者編寫腳本來統計這些數字。

我想重要的不僅是學習目前已經存在的這些腳本，而是理解如何利用 perf 的腳本功能開發新的功能。但如何寫 perf 腳本超出了本文的範圍，要想描述清楚估計需要一篇單獨的文章。因此不再贅述。


##結束語
從 2.6.31 開始，一晃居然也有幾個年頭了，期間每一個內核版本都會有新的 perf 特性。因此於我而言，閱讀新的 changelog 並在其中發現 perf 的新功能已經成為一項樂趣，類似喜歡陳奕迅的人們期待他創作出新的專輯一般。

本文寫到這裡可以暫時告一段落，還有一些命令沒有介紹，而且或許就在此時此刻，新的功能已經加入 perf 家族了。所以當您讀到這篇文章時，本文恐怕已經開始泛黃，然而我依舊感到高興，因為我正在經歷一個偉大時代，Linux 的黃金時代吧。

本人水平有限，必然有寫的不對的地方，還希望能和大家一起交流。



