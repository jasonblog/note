---
title: Linux 監控之 CPU
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,monitor,監控,cpu
description: Linux 中的系統監控，包括了常見的計算、內存、網絡等監控指標，以及相應的工具。
---

Linux 中的系統監控，包括了常見的計算、內存、網絡等監控指標，以及相應的工具。

<!-- more -->

## 簡介

最初 CPU 的頻率越高，則性能越好，而頻率與製程密切相關，但是製程存在一個天花板，目前一般到 10nm；為了提升性能， cpu 開始採用多核，即在一個封裝裡放多個 core，而且又發明了超線程技術 Hyper-threading 。

> CPU 製程技術最小能做到多少納米？
>
> 其實從科技發展的角度來說，並沒有絕對的極限，我們能確定的是，芯片製程越小，單位體積的集成度越高，就意味著處理效率和發熱量越小。但受制於切割工藝的極限，以目前的情況來看，理論上的製程極限我們還是可以簡單的分析出來的。
>
> 我們知道，硅原子大小半徑為110皮米，也就是0.11納米，直徑0.22nm。雖然3D晶體管的出現已經讓芯片不再全部依賴製程大小，而製程工藝的提升，也意味著會決定3D晶體管橫面積大小，不過，在不破壞硅原子本身的前提下，芯片製造目前還是有理論極限的，在0.5nm左右，之所以不是0.2nm，是因為本身硅原子之間也要保持一定的距離。而從實際角度上看，Intel在9nm製程上已經出現了切割良品率低和漏電率低的問題，所以0.5nm這個理論極限在目前的科學技術上看，幾乎是不可能的。

> 其中有常見的概念，直接複製 WikiPedia 中的解釋。
>
> [SMP(Symmetric multiprocessing)](https://en.wikipedia.org/wiki/Symmetric_multiprocessing), involves a multiprocessor computer hardware architecture where two or more identical processors are connected to a single shared main memory and are controlled by a single OS instance.
>
> [NUMA(Non-Uniform Memory Access)](https://en.wikipedia.org/wiki/Non-Uniform_Memory_Access), is a computer memory design used in multiprocessing, where the memory access time depends on the memory location relative to a processor. Under NUMA, a processor can access its own local memory faster than non-local memory, that is, memory local to another processor or memory shared between processors. NUMA architectures logically follow in scaling from symmetric multiprocessing (SMP) architectures.

下圖展示了這些術語之間的邏輯關係。

![cpu topology]({{ site.url }}/images/linux/monitor-cpu-topology.jpg "cpu topology"){: .pull-center width="350px" }

一個 NUMA node 包括一個或者多個 Socket，以及與之相連的 local memory；一個多核的 Socket 有多個 Core，如果 CPU 支持 HT，OS 還會把這個 Core 看成 2 個 Logical Processor。

NUMA 常見問題可參考 [Frequently Asked Questions About NUMA](http://lse.sourceforge.net/numa/faq/index.html)，或者 [本地文檔](/reference/linux/monitor/NUMA Frequently Asked Questions.html) 。

### 內存模型

通常有兩種不同的內存管理方式：

* UMA(Uniform Memory Acess)，將可用的內存以連續的方式組織起來，可能有小的缺口，每個處理器訪問各個內存區的速度都是一樣的；
* NUMA(Non-Uniform Memory Access)，系統中每個處理器都有本地內存，可以支持特別快速的訪問，各個處理器之間通過總線連接起來，以支持對其他的處理器的內存訪問。

在 CentOS 中可以通過如下命令查看是否為 NUMA。

{% highlight text %}
# yum install numactl -y             # 安裝numactl命令
# numactl --hardware
available: 2 nodes (0-1)             # 當前機器有2個NUMA node，編號0-1
node 0 cpus: 0 1 2 3
node 0 size: 8113 MB                 # node 0物理內存大小
node 0 free: 182 MB
node distances:                      # node距離，可簡單認為是CPU本node內存訪問和跨node內存訪問的成本
node   0   1                         # 可知跨node的內存訪問成本(20)是本地node內存(10)的2倍
  0:  10  20
  1:  20  10

$ grep -i numa /var/log/dmesg
[xxxxx] No NUMA configuration found  # 非NUMA，否則是
{% endhighlight %}

當然如果開機時間過長，可能會導致日誌已經被覆蓋，此時可以通過如下方式查看。

{% highlight text %}
$ cat /proc/cpuinfo | grep "physical id" | wc -l     # 查看當前計算機中的物理核個數
$ ls /sys/devices/system/node/ | grep node | wc -l   # 有多少個node

$ lscpu -p                                           # 查看CPU架構
{% endhighlight %}

在 /proc/cpuinfo 中的 physical id，描述的就是 Socket 的編號。

如果物理核數目有多個，而且 node 的個數也有多個，說明這是一個 NUMA 系統，每個目錄內部有多個文件和子目錄描述該 node 內 cpu、內存等信息，比如說 node0/meminfo 描述了 node0 內存相關信息；如果 node 的個數只有 1 個，說明這是一個 SMP 系統。

關於 NUMA 的詳細信息可以通過 dmidecode 查看。


### 內核數

通過 /proc/cpuinfo 可以查看跟 core 相關的信息，這裡包括了 socket、物理核、邏輯核的概念。

{% highlight text %}
# cat /proc/cpuinfo | grep "physical id" | sort -u | wc -l   # 顯示socket的數目

# cat /proc/cpuinfo
... ...
    cpu cores : 4              # 一個socket有4個物理核
    core id   : 1              # 一個core在socket內的編號，同樣是物理核編碼
... ...

# cat /proc/cpuinfo | grep "cpu cores" | uniq | cut -d: -f2  # 每個socket的物理核數目

# cat /proc/cpuinfo | grep processor                         # 查看有多少個邏輯核
{% endhighlight %}


### Cache

關於 CPU 中的 Cache 信息，可以通過 /proc/cpuinfo 查看，分為 L1、L2、L3，其中 L1 又分為獨立的指令 cache 和數據 cache。

{% highlight text %}
processor       : 0
cache size      : 12288 KB     # L3 Cache的總大小
cache_alignment : 64
{% endhighlight %}

詳細的 cache 信息可以通過 sysfs 查看，也即在 ```/sys/devices/system/cpu/``` 目錄下，其中 cpu 目錄下的編號是邏輯 cpu，且該目錄下包括了 index0 (L1數據cache)、index1 (L1指令cache)、index2 (L2 cache)、index3 (L3 cache，對應cpuinfo裡的cache) 。

<!--
在 indexN 目錄下的內容為：
* type，cache 的類型，分為數據和指令，常見的有 Data、Instruction。
* Level，相應的等級。
* Size，大小。
coherency_line_size * physical_line_partition * ways_of_associativity * number_of_sets = size 。
shared_cpu_map，標示被那些 CPU 共享。
-->


## CPU Usage VS. LOAD

Linux 系統的 CPU 使用率就是 CPU 的使用狀況，也就是一段時間之中，CPU 用於執行任務佔用的時間與總的時間的比率。

<!--
{% highlight text %}
%user Percentage of CPU utilization that occurred while executing at the user level (application). Note that this field includes time spent running virtual processors. （未標誌nice值的）用戶態程序的CPU佔用率。
%nice Percentage of CPU utilization that occurred while executing at the user level with nice priority. 標誌了nice值的用戶態程序的CPU佔用率。
%system Percentage of CPU utilization that occurred while executing at the system level (kernel). Note that this field includes time spent servicing hardware and software interrupts. 系統態（內核）程序的CPU佔用率。
%iowait Percentage of time that the CPU or CPUs were idle during which the system had an outstanding disk I/O request. I/O等待的CPU佔用率。
%steal Percentage of time spent in involuntary wait by the virtual CPU or CPUs while the hypervisor was servicing another virtual processor. 這個一般是在虛擬機中才能看到數值，比如：我的VPS供應商CPU overcommitment很嚴重，故我偶爾能看到%steal值有點高。
%idle Percentage of time that the CPU or CPUs were idle and the system did not have an outstanding disk I/O request. %idle越高，說明CPU越空閒。
{% endhighlight %}
-->

### 相關文件

CPU 的全局性能指標保存在 ```/proc/stat``` 文件中，大部分監控都是讀取該文件，如 dstat 。對於文件的解讀可以參考 man 5 proc 中的 ```/proc/stat``` 部分，也可以參考 [online man  5 proc](http://man7.org/linux/man-pages/man5/proc.5.html) 。

進程和線程的統計可參考 ```/proc/<pid>/stat``` 和 ```/proc/<pid>/task/<tid>/stat```；需要注意的是，對於不同的內核版本， ```/proc/stat``` 內容會有所區別：```steal (since Linux 2.6.11)```、```guest (since Linux 2.6.24)```、```guest_nice (since Linux 2.6.33)``` 。

{% highlight text %}
cpu  3816877 11951 424208 1254811 21564 0 2153 0 0 0
cpu0 958728 2517 108301 895882 15136 0 1048 0 0 0
cpu1 953763 3080 108605 117901 2376 0 431 0 0 0
cpu2 955060 3789 102803 119840 2140 0 588 0 0 0
cpu3 949325 2563 104497 121187 1910 0 85 0 0 0
intr 88396062 67 62144 0 0 0 0 0 0 1 126484 0 0 2126483 ... ...
ctxt 156849121
btime 1488368299
processes 126703
procs_running 2
procs_blocked 0
softirq 76359224 43 47319594 111 58877 726684 9 63206 16513537 0 11677163
{% endhighlight %}

對於上述文件的內容，簡單介紹如下。

{% highlight text %}
cpu cpuN
    CPU使用情況，都是從系統啟動到當前的值，單位是節拍 (Jiffies)，總計分為10列；
  **user
    普通進程處於用戶態的運行時間，不包括nice為負的進程消耗時間。
  **nice
    nice值為負的進程所佔用的用戶態時間。
  **system
    處於核心態的運行時間，也包括 IRQ 和 softirq 時間。
    如果系統CPU佔用率高，表明系統某部分存在瓶頸，通常值越低越好。
  **idle
    空閒時間，不包含 IO 等待時間。
  **iowait
    等待 IO 響應的時間(since 2.5.41)。
    如果系統花費大量時間等待 IO ，說明存在 IO 瓶頸。
  **irq
    處理硬中斷的時間(since 2.6.0-test4)。
  **softirq
    處理軟中斷的時間(since 2.6.0-test4)。
  **steal
    當採用虛擬環境時，運行在其它操作系統上的時間，此時hypervisor在為另一個虛擬處理器服務，具體監控指標詳見後面。
  **guest
    內核運行在虛擬機上的時間。
  **guest_nice
    同上。
intr
    中斷信息，首列為系統啟動以來，發生的所有的中斷的次數；接著的每個數對應特定中斷自系統啟動以來所發生的次數。
ctxt
    系統啟動以來CPU發生的上下文交換的次數。
btime
    給出了從系統啟動到現在為止的時間，單位為秒。
processes
    自系統啟動以來所創建的任務的個數目。
procs_running
    當前運行隊列的任務的數目。
procs_blocked
    當前被阻塞的任務的數目。
{% endhighlight %}

PS: 如果 iowait 的值過高，表示硬盤存在 I/O 瓶頸。如果 idle 值高但系統響應慢時，有可能是 CPU 等待分配內存，此時應加大內存容量。

### 內核實現

/proc/stat 文件在 ```proc_stat_init()@fs/proc/stat.c``` 函數中實現，會在內核初始化時調用；對上述文件的讀寫，會調用 proc_stat_operations 所定義的函數。

{% highlight c %}
static const struct file_operations proc_stat_operations = {
    .open       = stat_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};
{% endhighlight %}

打開文件會調用函數 stat_open() ，該函數首先申請大小為 size 的內存，來存放臨時數據，也是我們看到的 stat 裡的最終數據；文件中的數據由 show_stat() 函數填充。

{% highlight c %}
static int show_stat(struct seq_file *p, void *v)
{
    int i, j;
    unsigned long jif;
    u64 user, nice, system, idle, iowait, irq, softirq, steal;
    u64 guest, guest_nice;
    u64 sum = 0;
    u64 sum_softirq = 0;
    unsigned int per_softirq_sums[NR_SOFTIRQS] = {0};
    struct timespec boottime;

    user = nice = system = idle = iowait =
        irq = softirq = steal = 0;
    guest = guest_nice = 0;
    getboottime(&boottime);
    jif = boottime.tv_sec;

    for_each_possible_cpu(i) {
        user += kcpustat_cpu(i).cpustat[CPUTIME_USER];
        nice += kcpustat_cpu(i).cpustat[CPUTIME_NICE];
        system += kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
        idle += get_idle_time(i);
        iowait += get_iowait_time(i);
        irq += kcpustat_cpu(i).cpustat[CPUTIME_IRQ];
        softirq += kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];
        steal += kcpustat_cpu(i).cpustat[CPUTIME_STEAL];
        guest += kcpustat_cpu(i).cpustat[CPUTIME_GUEST];
        guest_nice += kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];
        sum += kstat_cpu_irqs_sum(i);
        sum += arch_irq_stat_cpu(i);

        for (j = 0; j < NR_SOFTIRQS; j++) {
            unsigned int softirq_stat = kstat_softirqs_cpu(j, i);

            per_softirq_sums[j] += softirq_stat;
            sum_softirq += softirq_stat;
        }
    }
    sum += arch_irq_stat();

    seq_puts(p, "cpu ");
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(user));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(nice));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(system));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(idle));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(iowait));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(irq));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(softirq));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(steal));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(guest));
    seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(guest_nice));
    seq_putc(p, '\n');

    for_each_online_cpu(i) {
        /* Copy values here to work around gcc-2.95.3, gcc-2.96 */
        user = kcpustat_cpu(i).cpustat[CPUTIME_USER];
        nice = kcpustat_cpu(i).cpustat[CPUTIME_NICE];
        system = kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];
        idle = get_idle_time(i);
        iowait = get_iowait_time(i);
        irq = kcpustat_cpu(i).cpustat[CPUTIME_IRQ];
        softirq = kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];
        steal = kcpustat_cpu(i).cpustat[CPUTIME_STEAL];
        guest = kcpustat_cpu(i).cpustat[CPUTIME_GUEST];
        guest_nice = kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];
        seq_printf(p, "cpu%d", i);
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(user));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(nice));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(system));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(idle));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(iowait));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(irq));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(softirq));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(steal));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(guest));
        seq_put_decimal_ull(p, ' ', cputime64_to_clock_t(guest_nice));
        seq_putc(p, '\n');
    }
    seq_printf(p, "intr %llu", (unsigned long long)sum);

    /* sum again ? it could be updated? */
    for_each_irq_nr(j)
        seq_put_decimal_ull(p, ' ', kstat_irqs_usr(j));

    seq_printf(p,
        "\nctxt %llu\n"
        "btime %lu\n"
        "processes %lu\n"
        "procs_running %lu\n"
        "procs_blocked %lu\n",
        nr_context_switches(),
        (unsigned long)jif,
        total_forks,
        nr_running(),
        nr_iowait());

    seq_printf(p, "softirq %llu", (unsigned long long)sum_softirq);

    for (i = 0; i < NR_SOFTIRQS; i++)
        seq_put_decimal_ull(p, ' ', per_softirq_sums[i]);
    seq_putc(p, '\n');

    return 0;
}
{% endhighlight %}

在上述的函數中，會依次統計 cpu 的各個參數，下面以 user 為例。


<!--
{% highlight text %}
user = kcpustat_cpu(i).cpustat[CPUTIME_USER];
user = per_cpu(kernel_cpustat, i).cpustat[CPUTIME_USER];
user = (*SHIFT_PERCPU_PTR(&(kernel_cpustat), per_cpu_offset(i))).cpustat[CPUTIME_USER];
{% endhighlight %}


kernel/sched/cputime.c


其中包括了 cpu 的心能指標，進程切換次數ctxt、內核啟動的時間btime、所有創建的進程processes、正在運行進程的數量procs_running、阻塞的進程數量procs_blocked、所有io等待的進程數量、軟中斷的信息。

在top、sar、vmstat、mpstat等命令中可以看到CPU使用率通常包含如下幾種統計（摘自 man sar）：

top
[root@jay-linux ~]# sar -u 1 5
[root@jay-linux ~]# vmstat -n 1 5
[root@jay-linux ~]# mpstat -P ALL 1 5


Linux的Load（系統負載），是一個讓新手不太容易瞭解的概念。top/uptime等工具默認會顯示1分鐘、5分鐘、15分鐘的平均Load。具體來說，平均Load是指，在特定的一段時間內統計的正在CPU中運行的(R狀態)、正在等待CPU運行的、處於不可中斷睡眠的(D狀態)的任務數量的平均值。
我估計也沒說的太清楚，看下wikipedia上的一段話吧：An idle computer has a load number of 0. Each process using or waiting for CPU (the ready queue or run queue) increments the load number by 1. Most UNIX systems count only processes in the running (on CPU) or runnable (waiting for CPU) states. However, Linux also includes processes in uninterruptible sleep states (usually waiting for disk activity), which can lead to markedly different results if many processes remain blocked in I/O due to a busy or stalled I/O system.
還有man sar中的解釋：The load average is calculated as the average number of runnable or running tasks (R state), and the number of tasks in uninterruptible sleep (D state) over the specified interval.
對於一個系統來說，多少的Load算合理，多少又算Load高呢？
一般來說，對於Load的數值不要大於系統的CPU核數（或者開啟了超線程，超線程也當成CPU core吧）。當然，有人覺得Load等於CPU core數量的2倍也沒事，不過，我自己是在Load達到CPU core數量時，一般都會去查看下是什麼具體原因導致load較高的。
Linux中查看Load的命令，推薦如下：
[root@jay-linux ~]# top
[root@jay-linux ~]# uptime
[root@jay-linux ~]# sar -q 1 5
最後，說一下CPU使用率和Load的關係吧。如果主要是CPU密集型的程序在運行（If CPU utilization is near 100 percent (user + nice + system), the workload sampled is CPU-bound.），那麼CPU利用率高，Load一般也會比較高。而I/O密集型的程序在運行，可能看到CPU的%user, %system都不高，%iowait可能會有點高，這時的Load通常比較高。同理，程序讀寫慢速I/O設備（如磁盤、NFS）比較多時，Load可能會比較，而CPU利用率不一定高。這種情況，還經常發生在系統內存不足並開始使用swap的時候，Load一般會比較高，而CPU使用率並不高。

本文就簡單說這麼多了，想了解更全面的信息，可參考以下方法：
1. man sar, man top （認真看相關解析，定有收穫）
2. wikipedia：http://en.wikipedia.org/wiki/Load_%28computing%29
3. 幫助理解Load：http://blog.scoutapp.com/articles/2009/07/31/understanding-load-averages
4. 幫助理解load的中文博客：http://www.blogjava.net/cenwenchu/archive/2008/06/30/211712.html
http://os.51cto.com/art/201012/240719.htm
</p>


> 通過 uptime 可以查看當前時間、已經運行時間、用戶、負載等信息，top 命令第一行也是。
> <pre style="font-size:0.8em;line-height:1.2em;">$ uptime
> 18:02:41 up 41 days, 23:42,  3 users,  load average: 0.20, 0.32, 0.45</pre>
> 顯示當前時間 "18:02:41" ，已經運行的時間 "up 41 days, 23:42" ，登錄用戶數 "3 users" ，過去 1, 5, 15 分鐘的平均負載 "load average: 0.20, 0.32, 0.45" 。






一般程序分為了 CPU/IO 密集型，不同類型的進程表現以及瓶頸也有所區別。如果是 CPU 密集型，當增大壓力時，就很容易把 CPU 利用率打滿；而對於 IO 密集型，增大壓力時，CPU 使用率不一定能上去。

對於後者，大文件讀寫的 CPU 開銷遠小於小文件讀寫的開銷；這時因為在 IO 吞吐量一定時，小文件的讀寫更加頻繁，需要更多的 CPU 來處理 IO 中斷。



在Linux/Unix下，CPU利用率分為用戶態，系統態和空閒態，分別表示CPU處於用戶態執行的時間，系統內核執行的時間，和空閒系統進程執行的時間。平時所說的CPU利用率是指：CPU執行非系統空閒進程的時間 / CPU總的執行時間。

在Linux的內核中，有一個全局變量：Jiffies。 Jiffies代表時間。它的單位隨硬件平臺的不同而不同。系統裡定義了一個常數HZ，代表每秒種最小時間間隔的數目。這樣jiffies的單位就是1/HZ。Intel平臺jiffies的單位是1/100秒，這就是系統所能分辨的最小時間間隔了。每個CPU時間片，Jiffies都要加1。 CPU的利用率就是用執行用戶態+系統態的Jiffies除以總的Jifffies來表示。

在Linux系統中，可以用/proc/stat文件來計算cpu的利用率(詳細的解釋可參考：http://www.linuxhowtos.org/System/procstat.htm)。這個文件包含了所有CPU活動的信息，該文件中的所有值都是從系統啟動開始累計到當前時刻。






-->




## 其它

### 線程相關

可以通過 ```ps -eLf``` 命令查看，也即通過 -L 參數顯示其 LWP(線程ID) 和 NLWP(線程的個數)，而且，還可以查看線程在哪個 CPU 上運行。

{% highlight text %}
$ ps -eo ruser,pid,ppid,lwp,psr,args -L
{% endhighlight %}

其中，每一列依次為：用戶ID，進程ID，父進程ID，線程ID，運行該線程的CPU的序號，命令行參數（包括命令本身）。

對於 top 命令，可以通過 ```top -p LWP``` 、```top -H``` 或者 ```top + H``` ，也即通過 H 命令打開線程。


## 參考

關於 CPU 的頂層佈局可以參考 [玩轉CPU Topology](http://www.searchtb.com/2012/12/%E7%8E%A9%E8%BD%ACcpu-topology.html)，或者 [本地文檔](/reference/linux/monitor/all_about_cpu_topology.mht)；另外一個可以參考的是 [13 種在 Linux 系統上檢測 CPU 信息的工具](https://linux.cn/article-5104-1.html?pr)，或者 [本地文檔](/reference/linux/monitor/How to check CPU info on Linux.mht)，[check-cpu-info-linux.pdf](/reference/linux/monitor/check-cpu-info-linux.pdf) 。

LIKWID 工具介紹可參考 [Lightweight performance tools](http://tools.zih.tu-dresden.de/2011/downloads/treibig-likwid-ParTools.pdf) 或者 [本地文檔](/reference/linux/monitor/treibig-likwid-ParTools.pdf)，源碼查看 [Github](https://github.com/RRZE-HPC/likwid) 。

<!--
https://linux.cn/article-6201-1.html    *****非常經典：關於現代 CPU，程序員應當更新的知識

http://www.cnblogs.com/yjf512/archive/2012/12/10/2811823.html

/proc/stat的CPU負載信息說明
http://myssh.igigo.net/post/xiao-za-sui/-proc-statde-cpufu-zai-xin-xi-shuo-ming

Understanding CPU Steal Time - when should you be worried
?http://blog.scoutapp.com/articles/2013/07/25/understanding-cpu-steal-time-when-should-you-be-worried

Linux CPU利用率計算原理及內核實現
http://ilinuxkernel.com/?p=333

https://github.com/Leo-G/DevopsWiki/wiki/How-Linux-CPU-Usage-Time-and-Percentage-is-calculated
http://www.samirchen.com/linux-cpu-performance/

https://linux.cn/article-6201-1.html    *****非常經典：關於現代 CPU，程序員應當更新的知識

-->

{% highlight text %}
{% endhighlight %}
