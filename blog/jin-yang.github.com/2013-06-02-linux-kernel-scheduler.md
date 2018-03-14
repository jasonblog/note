---
title: Kernel 調度系統
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,scheduling policy,調度策略
description: 操作系統的調度算法必須實現幾個互相沖突的目標：進程響應時間儘可能快，後臺作業的吞吐量儘可能高，進程的飢餓現象儘可能避免，低優先級和高優先級進程的需要儘可能調和等等。什麼時候以怎樣的方式選擇一個進程運行，就是所謂的調度策略 (Scheduling Policy)。本文中，介紹下 Linux Kernel 中時如何實現的。
---

操作系統的調度算法必須實現幾個互相沖突的目標：進程響應時間儘可能快，後臺作業的吞吐量儘可能高，進程的飢餓現象儘可能避免，低優先級和高優先級進程的需要儘可能調和等等。

什麼時候以怎樣的方式選擇一個進程運行，就是所謂的調度策略 (Scheduling Policy)。

本文中，介紹下 Linux Kernel 中時如何實現的。

<!-- more -->

## 調度器

Linux 的調度算法通過模塊的方式實現，每種類型的調度器會有一個優先級，調度時會按照優先級遍歷調度類，擁有一個可執行進程的最高優先級的調度器類勝出，去選擇下面要執行的那個程序。

### 調度類型

目前 Linux 中主要有兩大類調度算法，CFS (完全公平調度算法) 以及 實時調度算法，在應用中，可以通過 ```sched_setscheduler()``` 函數修改進程的調度策略，目前有 5 種調度策略：

* SCHED_NORMAL<br>最常用的，調度策略主要用於 CFS 調度，存在靜態優先級和動態優先級。
* SCHED_BATCH<br>除了不能搶佔外，與上相同，可讓任務延長執行的時間 (time slice)，減小上下文切換的次數，以提高 cache 的利用率 (每次 context-switch 都會導致 cache-flush)。<br>該調度策略適用於週期批量執行的任務，而不適合交互性的產品，主要是由於任務的切換延遲，讓人感覺系統性能不佳。
* SCHED_IDLE<br>它甚至比 nice 19 還弱，用於空閒時需要處理的任務。
* SCHED_RR<br>多次循環調度，擁有時間片，結束後會放在隊列末尾。
* SCHED_FIFO<br>先進先出規則，一次機會做完，沒有時間片可以運行任意長的時間。

其中前面三種策略使用的是 CFS 調度器類，後面兩種使用 RT 調度器類。任何時候，實時進程的優先級都高於普通進程，實時進程只會被更高級的實時進程搶佔。

### 調度時機

Linux 調度時機主要有：

1. 進程狀態轉換的時刻：進程終止、進程睡眠；<br>進程調用 ```sleep()``` 或 ```exit()``` 等進行狀態轉換，此時會主動調用調度程序進行進程調度；

2. 當前進程的時間片用完時（current->counter=0）<br>由於進程的時間片是由時鐘中斷來更新的，因此，這種情況和時機 4 是一樣的。

3. 設備驅動程序<br>當設備驅動程序執行長而重複的任務時，直接調用調度程序。在每次反覆循環中，驅動程序都檢查 ```need_resched``` 的值，如果必要，則調用調度程序 ```schedule()``` 主動放棄 CPU 。

4. 進程從中斷、異常及系統調用返回到用戶態時<br>如前所述，不管是從中斷、異常還是系統調用返回，最終都調用 ```ret_from_sys_call()```，由這個函數進行調度標誌的檢測，如果必要，則調用調用調度程序。

對於最後一條，那麼為什麼從系統調用返回時要調用調度程序呢？這主要是從效率考慮，從系統調用返回意味著要離開內核態而返回到用戶態，而狀態的轉換要花費一定的時間，因此，在返回到用戶態前，系統把在內核態該處理的事全部做完。

### SMP 負載均衡

在目前的 CFS 調度器中，每個 CPU 維護本地 RQ 所有進程的公平性，為了實現跨 CPU 的調度公平性，CFS 必須定時進行 load balance，將一些進程從繁忙的 CPU 的 RQ 中移到其他空閒的 RQ 中。

這個 load balance 的過程需要獲得其它 RQ 的鎖，這種操作降低了多運行隊列帶來的並行性。當然，load balance 引入的加鎖操作依然比全局鎖的代價要低，這種代價差異隨著 CPU 個數的增加而更加顯著。

但是，如果系統中的 CPU 個數有限，多 RQ 的優勢便不明顯了；而採用單一隊列，每一個需要調度的新進程都可以在全局範圍內查找最合適的 CPU ，而無需 CFS 那樣等待 load balance 代碼來決定，這減少了多 CPU 之間裁決的延遲，最終的結果是更小的調度延遲。

也就是說，CFS 為了維護多 CPU 上的公平性，所採用的負載平衡機制，可能會抵消了 per cpu queue 曾帶來的好處。

## CFS 完全公平調度

Completely Fair Schedule, CFS 在 2.6.23 引入，同時包括了模塊化、完全公平調度、組調度等一系列特性；按作者的說法：CFS 80% 的工作可以用一句話來概括，**CFS 在真實的硬件上模擬了理想且精確的多任務處理器**。

{% highlight text %}
80% of CFS's design can be summed up in a single sentence: CFS basically models
an "ideal, precise multi-tasking CPU" on real hardware.
{% endhighlight %}

關於 CFS 的簡單介紹可以查看內核文檔 [sched-design-CFS.txt]( {{ site.kernel_docs_url }}/Documentation/scheduler/sched-design-CFS.txt ) 。

### 簡介

該模型是從 RSDL/SD 中吸取了完全公平的思想，不再跟蹤進程的睡眠時間，也不再企圖區分交互式進程，它將所有的進程都統一對待，在既定的時間內每個進程都獲得了公平的 CPU 佔用時間，調度的依據就是每個進程的權重，這就是公平的含義。

為了實現完全公平調度，內核引入了虛擬時鐘（virtual clock）的概念，統計進程已經運行的時間採用虛擬運行時間 (virtual runtime)，該值與具體的時鐘晶振沒有關係，只是為了公平分配 CPU 時間而提出的一種時間量度。

vt 是遞增的，該值與其實際的運行時間成正比，與權重成反比；也就是說權重越高，對應的優先級越高，進而該進程虛擬時鐘增長的就慢。權重的計算與靜態優先級相關，該值在 ```set_load_weight()``` 函數中設置。

注意，優先級和權重之間的關係並不是簡單的線性關係，內核使用一些經驗數值來進行轉化。

如上所述，這就意味著，vt 用來作為對進程進行排序的參考，而不能用來反映進程真實執行時間。

<!--
CFS 的核心思想是把 CPU 總時間按運行隊列所佔權重進行分配，即佔用 CPU 後能夠執行的理想運行時間 (ideal runtime)，而在調度時選擇 vt 最小的運行，並且保證在某個時間週期 (__sched_period) 內運行隊列裡的所有調度單元都能夠至少被調度執行一次。

並且為了保證新進程能夠獲得合理的 vruntime，不至於一開始太小導致長期佔用 cpu，在每個 cfs_rq 上維護了一個 min_vruntime，所有調度單元的 vruntime 調整按它自己 cfs_rq 中的 min_vruntime 做為基準。

假設有 a、b、c 三個進程，權重分別是1、2、3,那麼所有的進程的權重和為6, 按照cfs的公平原則來分配，那麼a的重要性為1/6, b、c 為2/6, 3/6。這樣如果a、b、c 運行一次的總時間為6個時間單位，a佔1個，b佔2個，c佔3個。
-->



### FAQs

CFS 的基本原理是在一個調度週期 (sched_latency_ns) 內讓每個進程至少有機會運行一次，也就是說每個進程等待 CPU 的最長時間不超過這個調度週期。

然後根據進程的數量平分這個調度週期內的 CPU 使用權，由於進程的優先級 (nice) 不同，分割調度週期的時候要加權；每個進程的累計運行時間保存在自己的 vruntime 字段裡，哪個進程的 vruntime 最小就獲得本輪運行的權利。

#### 新進程vruntime的初始值是0？

假如新進程 vruntime 的初值為 0 的話，也就是比老進程的值小很多，那麼它在相當長的時間內都會保持搶佔 CPU 的優勢，老進程就要餓死了，這顯然是不公平的。

所以 CFS 為每個 CPU 的運行隊列 cfs_rq 維護了一個 min_vruntime 字段，記錄該運行隊列中所有進程的最小 vruntime 值，新進程的初始 vruntime 值就以其所在運行隊列的 min_vruntime 為基礎來設置，與老進程保持在合理的差距範圍內。

<!--
新進程的vruntime初值的設置與兩個參數有關：
sched_child_runs_first：規定fork之後讓子進程先於父進程運行;
sched_features的START_DEBIT位：規定新進程的第一次運行要有延遲。

注：
sched_features是控制調度器特性的開關，每個bit表示調度器的一個特性。在sched_features.h文件中記錄了全部的特性。START_DEBIT是其中之一，如果打開這個特性，表示給新進程的vruntime初始值要設置得比默認值更大一些，這樣會推遲它的運行時間，以防進程通過不停的fork來獲得cpu時間片。

如果參數 sched_child_runs_first打開，意味著創建子進程後，保證子進程會在父進程之前運行。

子進程在創建時，vruntime初值首先被設置為min_vruntime；然後，如果sched_features中設置了START_DEBIT位，vruntime會在min_vruntime的基礎上再增大一些。設置完子進程的vruntime之後，檢查sched_child_runs_first參數，如果為1的話，就比較父進程和子進程的vruntime，若是父進程的vruntime更小，就對換父、子進程的vruntime，這樣就保證了子進程會在父進程之前運行。
-->

#### 休眠進程的vruntime一直保持不變嗎？

如果休眠進程的 vruntime 保持不變，而其它運行進程的 vruntime 一直在增加，那麼休眠進程喚醒時，由於其 vruntime 相比要小很多，就會使它獲得長時間搶佔 CPU 的優勢，從而導致其它進程餓死。

為此，CFS 會在休眠進程被喚醒時重新設置 vruntime 值，以 min_vruntime 值為基礎，並進行一定的補償。

{% highlight c %}
static void place_entity(struct cfs_rq *cfs_rq, struct sched_entity *se, int initial)
{
    u64 vruntime = cfs_rq->min_vruntime;

    /*
     * The 'current' period is already promised to the current tasks,
     * however the extra weight of the new task will slow them down a
     * little, place the new task so that it fits in the slot that
     * stays open at the end.
     */
    if (initial && sched_feat(START_DEBIT))  /* initial表示新進程 */
        vruntime += sched_vslice(cfs_rq, se);

    /* sleeps up to a single latency don't count. */
    if (!initial) { /* 休眠進程 */
        unsigned long thresh = sysctl_sched_latency;

        /*
         * Halve their sleep time's effect, to allow
         * for a gentler effect of sleepers:
         */
        if (sched_feat(GENTLE_FAIR_SLEEPERS))
            thresh >>= 1;

        vruntime -= thresh;
    }

    /* ensure we never gain time by being placed backwards. */
    se->vruntime = max_vruntime(se->vruntime, vruntime);
}
{% endhighlight %}

#### 休眠進程在喚醒時會立刻搶佔CPU嗎？

這由 CFS 的喚醒搶佔特性決定，也即 sched_features 的 WAKEUP_PREEMPT 位。

由於休眠進程在喚醒時會獲得 vruntime 的補償，所以在醒來時搶佔 CPU 是大概率事件，這也是 CFS 調度算法的本意，即保證交互式進程的響應速度，因為交互式進程等待用戶輸入會頻繁休眠。

<!--
除了交互式進程以外，主動休眠的進程同樣也會在喚醒時獲得補償，例如通過調用sleep()、nanosleep()的方式，定時醒來完成特定任務，這類進程往往並不要求快速響應，但是CFS不會把它們與交互式進程區分開來，它們同樣也會在每次喚醒時獲得vruntime補償，這有可能會導致其它更重要的應用進程被搶佔，有損整體性能。我曾經處理過的一個案例：服務器上有兩類應用進程，A進程定時循環檢查有沒有新任務，如果有的話就簡單預處理後通知B進程，然後調用nanosleep()主動休眠，醒來後再重複下一個循環；B進程負責數據運算，是CPU消耗型的；B進程的運行時間很長，而A進程每次運行時間都很短，但睡眠/喚醒卻十分頻繁，每次喚醒就會搶佔B，導致B的運行頻繁被打斷，大量的進程切換帶來很大的開銷，整體性能下降很厲害。那有什麼辦法嗎？有，CFS可以禁止喚醒搶佔 特性：
# echo NO_WAKEUP_PREEMPT > /sys/kernel/debug/sched_features

禁用喚醒搶佔 特性之後，剛喚醒的進程不會立即搶佔運行中的進程，而是要等到運行進程用完時間片之後。在以上案例中，經過這樣的調整之後B進程被搶佔的頻率大大降低了，整體性能得到了改善。

如果禁止喚醒搶佔特性對你的系統來說太過激進的話，你還可以選擇調大以下參數：

sched_wakeup_granularity_ns
這個參數限定了一個喚醒進程要搶佔當前進程之前必須滿足的條件：只有當該喚醒進程的vruntime比當前進程的vruntime小、並且兩者差距(vdiff)大於sched_wakeup_granularity_ns的情況下，才可以搶佔，否則不可以。這個參數越大，發生喚醒搶佔就越不容易。
-->

#### 進程佔用的CPU時間片可以無窮小嗎？

假設有兩個進程，它們的 vruntime 初值一樣，當其中的一個進程運行後，它的 vruntime 就比另外的進大了，那麼正在運行的進程什麼時候會被搶佔呢？

答案是：為了避免進程切換過於頻繁造成太大的資源消耗，CFS 設定了進程佔用 CPU 的最小時間值 (sched_min_granularity_ns)，正在 CPU 上運行的進程如果不足這個時間是不可以被調離 CPU 的。

另外，CFS 默認會把調度週期 sched_latency_ns 按照進程的數量平分，給每個進程平均分配相同的 CPU 時間片，但是如果進程數量太多的話，就會造成 CPU 時間片太小，如果小於上述的最小值，那麼就以最小值為準，而調度週期也不再遵守 sched_latency_ns 。

<!-- 而是以 (sched_min_granularity_ns * 進程數量) 的乘積為準。-->

#### 進程切換CPU時vruntime會不會改變？

在 SMP 系統上，當 CPU 的負載不同時會進行負載均衡，而每個 CPU 都有自己的運行隊列，而每個隊列中的 vruntime 也各不相同，比如可以對比下每個運行隊列的 min_vruntime 值：

{% highlight text %}
# grep min_vruntime /proc/sched_debug
  .min_vruntime                  : 526279705.695991
  .min_vruntime                  : 532370994.256253
  .min_vruntime                  : 720871453.830955
  .min_vruntime                  : 692323575.852029
{% endhighlight %}

顯然，如果對 vruntime 不做處理直接切換，必然會導致不公平。

當進程從一個 CPU 的中出隊 (dequeue_entity) 時，它的 vruntime 要減去隊列的 min_vruntime 值；而當進程加入另一個 CPU 的運行隊列 (enqueue_entiry) 時，它的 vruntime 要加上該隊列的 min_vruntime 值。

這樣，進程從一個 CPU 遷移到另一個 CPU 之後 vruntime 保持相對公平。

{% highlight c %}
static void
dequeue_entity(struct cfs_rq *cfs_rq, struct sched_entity *se, int flags)
{
...
        /*
         * Normalize the entity after updating the min_vruntime because the
         * update can refer to the ->curr item and we need to reflect this
         * movement in our normalized position.
         */
        if (!(flags & DEQUEUE_SLEEP))
                se->vruntime -= cfs_rq->min_vruntime;
...
}

static void
enqueue_entity(struct cfs_rq *cfs_rq, struct sched_entity *se, int flags)
{
        /*
         * Update the normalized vruntime before updating min_vruntime
         * through callig update_curr().
         */
        if (!(flags & ENQUEUE_WAKEUP) || (flags & ENQUEUE_WAKING))
                se->vruntime += cfs_rq->min_vruntime;
...
}
{% endhighlight %}




## 源碼解析

接下來看看 Linux 中代碼的實現。

### 相關結構體

Linux2.6.24 內核採用分層管理調度，也就是兩層：第一層被稱為核心調度器，在核心調度器下面為調度器類。調度算法實現相關的數據結構主要有運行實體 (struct rq)、調度類 (struct sched_class) 和運行隊列。

![scheduler stucture]({{ site.url }}/images/linux/kernel/scheduler_stucture.png "scheduler stucture"){: .pull-center }

首先是進程描述符中與調度相關的信息。

{% highlight c %}
struct task_struct {
    ... ...
    int prio, static_prio, normal_prio;      // 進程優先級
    unsigned int rt_priority;                // RT優先級
    const struct sched_class *sched_class;   // 響應的調度類
    struct sched_entity se;
    struct sched_rt_entity rt;
    struct sched_dl_entity dl;
    unsigned int policy;                     // 調度策略，默認為SCHED_NORMAL
    cpumask_t cpus_allowed;                  // 限制此進程在哪個處理器上運行
    ... ...
};
{% endhighlight %}

就緒隊列用於存儲一些基本的用於調度的信息，包括實時調度的、CFS 調度的以及 DL 調度的，兩者屬於不同的調度實體，每個 CPU 會有一個 rq 結構體，也就是就緒隊列。

{% highlight c %}
struct rq {                      // @ kernel/sched/sched.h
    spinlock_t lock;                     // 鎖
    unsigned long nr_running;            // 當前就緒對列進程的數目
    struct load_weight load;             // 當前就緒隊列負荷
    struct task_struct *curr, *idle;     // 分別指向當前以及空閒進程描述符
    struct cfs_rq cfs;                   // 分別表示三個不同的就緒隊列
    struct rt_rq rt;
    struct dl_rq dl;
    u64 clock;                           // 就緒隊列的時鐘，這個是週期更新的，真實的系統晶振時鐘
};

struct cfs_rq {                  // @ kernel/sched/sched.h
    struct load_weight load;             // 隊列上所有進程的權重值weight總和
    unsigned int nr_running;             // 當前就緒隊列的進程數=隊列進程的總數+正在運行的那個進程
    struct sched_entity *curr;           // 指向當前進程的運行實體
    struct rb_root tasks_timeline;       // 就緒隊列的樹跟
    struct rb_node *rb_leftmost;         // 保存紅黑樹最左邊的節點，直接作為下一個運行進程
};
{% endhighlight %}

調度類 ```sched_class``` 為模塊編程的上層支持，對於每個 Linux 新添加進來的調度算法都有自己的調度類實例，包括了 ```fair_sched_class```、```rt_sched_class```、```dl_sched_class```、```idle_sched_class``` 等。

{% highlight c %}
struct sched_class {            // @ kernel/sched/sched.h
    const struct sched_class *next;                                           // 調度類組成單向鏈表
    void (*enqueue_task) (struct rq *rq, struct task_struct *p, int wakeup);  // 向就緒隊列插入進程
    void (*dequeue_task) (struct rq *rq, struct task_struct *p, int flags);   // 從就緒隊列中刪除
    void (*yield_task) (struct rq *rq);                                       // 進程主動放棄處理器
    void (*check_preempt_curr) (struct rq *rq, struct task_struct *p);        // 用一個新喚醒的進程搶佔當前進程
    struct task_struct * (*pick_next_task) (struct rq *rq);                   // 選擇下一個將要運行的進程
    void (*put_prev_task) (struct rq *rq, struct task_struct *p);
    void (*task_tick) (struct rq *rq, struct task_struct *p);                 // 由週期調度器調用
    void (*task_new) (struct rq *rq, struct task_struct *p);                  // 每次建立新進程調用此函數通知調度器
};
{% endhighlight %}

調度運行隊列，也就是就緒隊列，用於保存處於 ready 狀態的進程，不同的調度算法使用不同的運行隊列，對於 CFS 調度，運用的是紅黑樹；而對於實時調度為組鏈表。

運行實體，也就是調度單位，對應的結構為 ```sched_entity```。調度器的調度單位不再是進程，而是可調度的實體，可以將多個進程捆綁在一起作為一個調度單位 (即調度實體) 進行調度，也就是說可調度實體可以是一個進程，也可以是多個進程構成的一個組。

另外，由於 CFS 不再有時間片的概念，但仍需要對每個進程運行的時間記賬，從而確保每個進程只在公平分配給他的處理器時間內運行，相關信息同樣保存在 sched_entity 中。

{% highlight c %}
struct sched_entity {
    unsigned int        on_rq;                    // 是否在運行隊列或正在執行，當前任務不會保存在紅黑樹中
    struct load_weight  load;                     // 該調度實體的權重，決定了運行時間以及被調用次數
    u64                 vruntime;                 // 存放進程的虛擬運行時間，用於調度器的選擇
    u64                 exec_start                // 記錄上次執行update_curr()的時間
    u64                 sum_exec_runtime;         // 進程總共執行的cpu clock，佔用cpu的物理時間
    u64                 pre_sum_exec_runtime;     // 進程在切換經CPU時的sum_exec_runtime值
};
{% endhighlight %}

每個 ```task_struct``` 都嵌入了一個 ```sched_entity```，這也就是為什麼進程也是一個可調度實體。

### 代碼實現

調度器的核心代碼在 ```kernel/sched``` 目錄下，包括 ```sched_init()```、```schedule()```、```scheduler_tick()``` 等，其中核心調度器包括了：

* 週期性調度器，schedule_tick()<br>不負責進程的切換，只是定時更新調度相關的統計信息，以備主調度器使用。

* 主調度器，schedule()<br>完成進程的切換，將 CPU 的使用權從一個進程切換到另一個進程。

我們知道在通過 ```fork()```、```vfork()```、```clone()``` 等函數時，進程創建最終都會調用 ```do_fork()```，而該函數會調用 ```copy_process()->sched_fork()``` 。

{% highlight text %}
do_fork()
 |-copy_process()
 | |-security_task_create()
 | |-dup_task_struct()
 | |-sched_fork()
 |-trace_sched_process_fork()
 |-get_task_pid()
 |-wake_up_new_task()
 |-put_pid()
{% endhighlight %}

在 ```sched_fork()``` 函數中會複製父進程的優先級，並將 ```fair_sched_class``` 調度器類實例的地址賦給新進程中的 ```sched_class``` 指針。


### scheduler_tick()

```scheduler_tick()``` 會以 HZ 為週期調度，用來更新運行隊列的時鐘及 load，然後調用當前進程的調度器類的週期調度函數。

{% highlight text %}
scheduler_tick()
  |-update_rq_clock()                 更新運行隊列的時鐘rq->clock
  |-curr->sched_class->task_tick()    執行調度器的週期性函數，以CFS為例
  | |-task_tick_fair()                CFS對應的task_tick()
  |   |-entity_tick()
  |     |-update_curr()
  |     |-update_cfs_shares()         對SMP有效
  |     |-check_preempt_tick()        檢查當前進程是否運行了足夠長的時間，是否需要搶佔
  |-update_cpu_load_active()          更新運行隊列load，將數組中先前存儲的load向後移動一個位置，並插入新值
{% endhighlight %}

在 ```update_cpu_load_active()``` 中，更新運行隊列的 load 本質是將數組中先前存儲的負荷值向後移動一個位置，將當前負荷記入數組的第一個位置。

另外，```check_preempt_tick()``` 函數用於檢查當前進程是否運行了足夠長的時間，如果超過了理想運行時間則無條件 resched；如果運行時間小於 ```sysctl_sched_min_granularity``` 那麼也直接返回。

可以看到每個時鐘都對會當前運行的 se 進行實質執行時間及虛擬執行時間進行更新，最後檢查該進程是否運行的足夠長的時間，如果是的話則將它置為 ```TIF_NEED_RESCHED``` 供主調度在適當的時機進行切換。

### schedule()

核心調度函數為 ```schedule()```，作為內核和其他部分用於調用進程調度器的入口，選擇哪個進程可以運行，何時將其投入運行。

<!--
<pre style="font-size:0.8em; face:arial;">
schedule()
  |-sched_submit_work()
  |-__schedule()
     |-deactivate_task()                     // 從運行隊列中刪除prev進程，與具體的調度類相關
     |-pick_next_task()                      // 從各自的運行隊列中選擇下一個進程來運行
     |  |-class->pick_next_task()            // 調用調度類的相應函數，以CFS為例
     |  |-pick_next_task_fair()
     |     |-pick_next_entity()
     |-context_switch()                      // 執行進程的上下文切換
</pre>

schedule() 通常會找到一個最高優先級的調度類，查找自己的可運行隊列，然後找到下一個該運行的進程，該函數唯一重要的事情是調用 pick_next_task() 。<br><br>

其主要流程為：<ol><li>
將當前進程從相應的運行隊列中刪除；</li><br><li>

計算和更新調度實體和進程的相關調度信息；</li><br><li>

將當前進重新插入到調度運行隊列中，對於CFS調度，根據具體的運行時間進行插入而對於實時調度插入到對應優先級隊列的隊尾；</li><br><li>

從運行隊列中選擇運行的下一個進程；</li><br><li>

進程調度信息和上下文切換；
</li></ol>
當進程上下文切換後（關於進程切換在前面的文章中有介紹），調度就基本上完成了，當前運行的進程就是切換過來的進程了。
在上述結構體中，sum_exec_runtime - pre_sum_exec_runtime 等於進程獲得 CPU 使用權後總的總時間，也就是 ideal_runtime 已被消耗了多少。<br><br>

在抽象模型中，計算 ideal_runtime 的時候需要求所有進程的權重值的和；而實際實現的時候，沒有求和的過程，而是把該值記錄在就緒隊列的 load.weight 中。在向就緒隊列中添加新進程時，就加上新進程的權重值，反之則減去。<br><br>

每個進程的 ideal_runtime 並沒有用變量保存起來，而是在需要用到時用函數 sched_slice() 計算得到，計算規則為 (task->se.load.weight/cfs_rq->load.weight)*period 。<br><br>

period 也沒有用變量來保存，用 __sched_period() 計算得到，sysctl_sched_latency * (nr_running / sysctl_nr_latency)。
-->

## 參考

<!--
http://linuxperf.com/?p=42

linux cfs調度器_理論模型
http://www.cnblogs.com/openix/archive/2013/08/13/3254394.html
 Linux內核學習筆記（一）CFS完全公平調度類
http://blog.chinaunix.net/uid-24757773-id-3266304.html
Linux內核-CFS進程調度器
https://wongxingjun.github.io/2015/09/09/Linux%E5%86%85%E6%A0%B8-CFS%E8%BF%9B%E7%A8%8B%E8%B0%83%E5%BA%A6%E5%99%A8/
Completely Fair Scheduler
http://www.linuxjournal.com/magazine/completely-fair-scheduler
linux內核分析——CFS
http://muyunzhe.blog.51cto.com/9164050/1651853
-->

{% highlight text %}
{% endhighlight %}
