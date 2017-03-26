# （九）：進程調度


Linux為多任務系統，正常情況下都存在成百上千個任務。由於linux提供搶佔式的多任務模式，所以linux能同時併發地交互執行多個進程，而調度程序將決定哪一個進程投入運行、何時運行、以及運行多長時間。調度程序是像linux這樣的多任務操作系統的基礎， 只有通過調度程序的合理調度，系統資源才能最大限度地發揮作用，多進程才會有併發執行的效果。當系統中可運行的進程數目比處理器的個數多，就註定在某一時刻有一些進程不能執行，這些不能執行的進程在等待執行。調度程序的基本工作就是停止一個進程的運行，再在這些等待執行的進程中選擇一個來執行。

調度程序停止一個進程的運行，再選擇一個另外進程的動作開始運行的動作被稱作搶佔（preemption）。一個進程在被搶佔之前能夠運行的時間是預先設置好的，這個預先設置好的時間就是進程的的時間片（timeslice）。時間片就是分配給每個可運行進程的處理器時間段，它表明進程在被搶佔前所能持續運行時間。

處理器的調度策略決定調度程序在何時讓什麼進程投入運行。調度策略通常需要在進程響應迅速(相應時間短)和進程吞吐量高之間尋找平衡。所以調度程序通常採用一套非常複雜的算法來決定最值得運行的進程投入運行。調度算法中最基本的一類當然就是基於優先級的調度，也就是說優先級高的先運行，相同優先級的按輪轉式進行調度。優先級高 的進程使用的時間片也長。調度程序總是選擇時間片未用盡且優先級最高的進程運行。這句話就是說用戶和系統可以通過設置進程的優先級來響應系統的調度。基於此，linux設計上一套動態優先級的調度方法。一開始，先為進程設置一個基本的優先級，然而它允許調度程序根據需要來加減優先級。linux內核提供了兩組獨立的優先級範圍。第一種是nice值，範圍從-20到19，默認為0。nice值越大優先級越小。另外nice值也用來決定分配給進程時間片的長短。linux下通過命令可以查看進程對應nice值，如下：



```sh
$ ps -el  
  
F S   UID   PID  PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD   
4 S     0     1     0  0  80   0 -   725 ?      ?        00:00:01 init   
1 S     0     2     0  0  80   0 -     0 ?      ?        00:00:00 kthreadd   
1 S     0     3     2  0 -40   - -     0 ?      ?        00:00:01 migration/0   
1 S     0     4     2  0  80   0 -     0 ?      ?        00:00:00 ksoftirqd/0   
1 S     0     9     2  0  80   0 -     0 ?      ?        00:00:00 ksoftirqd/1  
  
......  
  
1 S     0    39     2  0  85   5 -     0 ?      ?        00:00:00 ksmd   
......  
  
1 S     0   156     2  0  75  -5 -     0 ?      ?        00:00:00 kslowd000   
1 S     0   157     2  0  75  -5 -     0 ?      ?        00:00:00 kslowd001   
......   
4 S   499  2951     1  0  81   1 -  6276 ?      ?        00:00:00 rtkit-daemon   
......   
```

第二種範圍是實時優先級，默認範圍是從0到99。任何實時的優先級都高於普通優先級。

進程執行時，它會根據具體情況改變狀態，進程狀態是調度和對換的依據。Linux 將進程狀態分為五種： TASK_RUNNING 、 TASK_INTERRUPTIBLE 、 TASK_UNINTERRUPTIBLE 、 TASK_STOPPED 和 TASK_ZOMBILE 。進程的狀態隨著進程的調度發生改變 。


<table cellpadding="2" cellspacing="0" border="1" style="background-color:#FFFFFF">
<tbody>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_RUNNING</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">可運行</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_INTERRUPTIBLE</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">可中斷的等待狀態</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_UNINTERRUPTIBLE</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">不可中斷的等待狀態</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_STOPPED</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">停止狀態</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_TRACED</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">被跟蹤狀態</span> <br>
</div>
</td>
</tr>
</tbody>
</table>

`TASK_RUNNING （運行）`：無論進程是否正在佔用 CPU ，只要具備運行條件，都處於該狀態。 Linux 把處於該狀態的所有 PCB 組織成一個可運行隊列 run_queue ，調度程序從這個隊列中選擇進程運行。事實上， Linux 是將就緒態和運行態合併為了一種狀態。

`TASK_INTERRUPTIBLE （可中斷阻塞）`： Linux 將阻塞態劃分成 TASK_INTERRUPTIBLE 、 TASK_UNINTERRUPTIBLE 、 TASK_STOPPED 三種不同的狀態。處於 TASK_INTERRUPTIBLE 狀態的進程在資源有效時被喚醒，也可以通過信號或定時中斷喚醒。

`TASK_UNINTERRUPTIBLE （不可中斷阻塞）`：另一種阻塞狀態，處於該狀態的進程只有當資源有效時被喚醒，不能通過信號或定時中斷喚醒。在執行ps命令時，進程狀態為D且不能被殺死。

`TASK_STOPPED （停止）`：第三種阻塞狀態，處於該狀態的進程只能通過其他進程的信號才能喚醒。

`TASK_TRACED （被跟蹤）`：進程正在被另一個進程監視，比如在調試的時候。

我們在設置這些狀態的時候是可以直接用語句進行的比如：p—>state = `TASK_RUNNING`。同時內核也會使用set_task_state()和set_current_state()函數來進行。

linux調度器是以模塊方式提供的，這樣允許不同類型的進程可以有針對性地選擇調度算法。完全公平調度（CFS）是針對普通進程的調度類，CFS採用的方法是對時間片分配方式進行根本性的重新設計，完全摒棄時間片而是分配給進程一個處理器使用比重。通過這種方式，CFS確保了進程調度中能有恆定的公平性，而將切換頻率置於不斷變動之中。

與Linux 2.6之前調度器不同，2.6版本內核的CFS沒有將任務維護在鏈表式的運行隊列中，它拋棄了active/expire數組，而是對每個CPU維護一個以時間為順序的紅黑樹。該樹方法能夠良好運行的原因在於：

* 紅黑樹可以始終保持平衡，這意味著樹上沒有路徑比任何其他路徑長兩倍以上。
* 由於紅黑樹是二叉樹，查找操作的時間複雜度為O(log n)。但是除了最左側查找以外，很難執行其他查找，並且最左側的節點指針始終被緩存。
* 對於大多數操作（插入、刪除、查找等），紅黑樹的執行時間為O(log n)，而以前的調度程序通過具有固定優先級的優先級數組使用 O(1)。O(log n) 行為具有可測量的延遲，但是對於較大的任務數無關緊要。Molnar在嘗試這種樹方法時，首先對這一點進行了測試。

* 紅黑樹可通過內部存儲實現，即不需要使用外部分配即可對數據結構進行維護。

要實現平衡，CFS使用“虛擬運行時”表示某個任務的時間量。任務的虛擬運行時越小，意味著任務被允許訪問服務器的時間越短，其對處理器的需求越高。 CFS還包含睡眠公平概念以便確保那些目前沒有運行的任務（例如，等待 I/O）在其最終需要時獲得相當份額的處理器。由於篇幅原因，這裡不詳細講解CFS的實現。

對於實時進程， Linux 採用了兩種調度策略，即先來先服務調度（ First-In, First-Out ， FIFO ）和時間片輪轉調度（ Round Robin ， RR ）。因為實時進程具有一定程度的緊迫性，所以衡量一個實時進程是否應該運行， Linux 採用了一個比較固定的標準。

下面是調度相關的一些數據結構：
調度實體：struct sched_entity

```c
struct sched_entity {  
    struct load_weight  load;       /* for load-balancing */  
    struct rb_node      run_node;  
    struct list_head    group_node;  
    unsigned int        on_rq;  
  
    u64         exec_start;  
    u64         sum_exec_runtime;  
    u64         vruntime;  
    u64         prev_sum_exec_runtime;  
  
    u64         last_wakeup;  
    u64         avg_overlap;  
  
    u64         nr_migrations;  
  
    u64         start_runtime;  
    u64         avg_wakeup;  
  
    u64         avg_running;  
  
#ifdef CONFIG_SCHEDSTATS  
    u64         wait_start;  
    u64         wait_max;  
    u64         wait_count;  
    u64         wait_sum;  
    u64         iowait_count;  
    u64         iowait_sum;  
  
    u64         sleep_start;  
    u64         sleep_max;  
    s64         sum_sleep_runtime;  
  
    u64         block_start;  
    u64         block_max;  
    u64         exec_max;  
    u64         slice_max;  
  
    u64         nr_migrations_cold;  
    u64         nr_failed_migrations_affine;  
    u64         nr_failed_migrations_running;  
    u64         nr_failed_migrations_hot;  
    u64         nr_forced_migrations;  
    u64         nr_forced2_migrations;  
  
    u64         nr_wakeups;  
    u64         nr_wakeups_sync;  
    u64         nr_wakeups_migrate;  
    u64         nr_wakeups_local;  
    u64         nr_wakeups_remote;  
    u64         nr_wakeups_affine;  
    u64         nr_wakeups_affine_attempts;  
    u64         nr_wakeups_passive;  
    u64         nr_wakeups_idle;  
#endif  
  
#ifdef CONFIG_FAIR_GROUP_SCHED  
    struct sched_entity *parent;  
    /* rq on which this entity is (to be) queued: */  
    struct cfs_rq       *cfs_rq;  
    /* rq "owned" by this entity/group: */  
    struct cfs_rq       *my_q;  
#endif  
};  
```

該結構在./linux/include/linux/sched.h中，表示一個可調度實體（進程，進程組，等等）。它包含了完整的調度信息，用於實現對單個任務或任務組的調度。調度實體可能與進程沒有關聯。這裡包括負載權重load、對應的紅黑樹結點run_node、虛擬運行時vruntime（表示進程的運行時間，並作為紅黑樹的索引）、開始執行時間、最後喚醒時間、各種統計數據、用於組調度的CFS運行隊列信息cfs_rq，等等。


調度類：struct sched_class
該調度類也在sched.h中，是對調度器操作的面向對象抽象，協助內核調度程序的各種工作。調度類是調度器管理器的核心，每種調度算法模塊需要實現struct sched_class建議的一組函數。


```c
struct sched_class {
    const struct sched_class* next;

    void (*enqueue_task)(struct rq* rq, struct task_struct* p, int wakeup);
    void (*dequeue_task)(struct rq* rq, struct task_struct* p, int sleep);
    void (*yield_task)(struct rq* rq);

    void (*check_preempt_curr)(struct rq* rq, struct task_struct* p, int flags);

    struct task_struct* (*pick_next_task)(struct rq* rq);
    void (*put_prev_task)(struct rq* rq, struct task_struct* p);

#ifdef CONFIG_SMP
    int (*select_task_rq)(struct task_struct* p, int sd_flag, int flags);

    unsigned long (*load_balance)(struct rq* this_rq, int this_cpu,
                                  struct rq* busiest, unsigned long max_load_move,
                                  struct sched_domain* sd, enum cpu_idle_type idle,
                                  int* all_pinned, int* this_best_prio);

    int (*move_one_task)(struct rq* this_rq, int this_cpu,
                         struct rq* busiest, struct sched_domain* sd,
                         enum cpu_idle_type idle);
    void (*pre_schedule)(struct rq* this_rq, struct task_struct* task);
    void (*post_schedule)(struct rq* this_rq);
    void (*task_wake_up)(struct rq* this_rq, struct task_struct* task);

    void (*set_cpus_allowed)(struct task_struct* p,
                             const struct cpumask* newmask);

    void (*rq_online)(struct rq* rq);
    void (*rq_offline)(struct rq* rq);
#endif

    void (*set_curr_task)(struct rq* rq);
    void (*task_tick)(struct rq* rq, struct task_struct* p, int queued);
    void (*task_new)(struct rq* rq, struct task_struct* p);

    void (*switched_from)(struct rq* this_rq, struct task_struct* task,
                          int running);
    void (*switched_to)(struct rq* this_rq, struct task_struct* task,
                        int running);
    void (*prio_changed)(struct rq* this_rq, struct task_struct* task,
                         int oldprio, int running);

    unsigned int (*get_rr_interval)(struct task_struct* task);

#ifdef CONFIG_FAIR_GROUP_SCHED
    void (*moved_group)(struct task_struct* p);
#endif
};
```
其中的主要函數：
* enqueue_task：當某個任務進入可運行狀態時，該函數將得到調用。它將調度實體（進程）放入紅黑樹中，並對 nr_running 變量加 1。從前面“Linux進程管理”的分析中可知，進程創建的最後會調用該函數。
* dequeue_task：當某個任務退出可運行狀態時調用該函數，它將從紅黑樹中去掉對應的調度實體，並從 nr_running 變量中減 1。
* yield_task：在 compat_yield sysctl 關閉的情況下，該函數實際上執行先出隊後入隊；在這種情況下，它將調度實體放在紅黑樹的最右端。
* check_preempt_curr：該函數將檢查當前運行的任務是否被搶佔。在實際搶佔正在運行的任務之前，CFS 調度程序模塊將執行公平性測試。這將驅動喚醒式（wakeup）搶佔。
* pick_next_task：該函數選擇接下來要運行的最合適的進程。
* load_balance：每個調度程序模塊實現兩個函數，load_balance_start() 和 load_balance_next()，使用這兩個函數實現一個迭代器，在模塊的 load_balance 例程中調用。內核調度程序使用這種方法實現由調度模塊管理的進程的負載平衡。
* set_curr_task：當任務修改其調度類或修改其任務組時，將調用這個函數。
* task_tick：該函數通常調用自 time tick 函數；它可能引起進程切換。這將驅動運行時（running）搶佔。

調度類的引入是接口和實現分離的設計典範，你可以實現不同的調度算法（例如普通進程和實時進程的調度算法就不一樣），但由於有統一的接口，使得調度策略 被模塊化，一個Linux調度程序可以有多個不同的調度策略。調度類顯著增強了內核調度程序的可擴展性。每個任務都屬於一個調度類，這決定了任務將如何調 度。 調度類定義一個通用函數集，函數集定義調度器的行為。例如，每個調度器提供一種方式，添加要調度的任務、調出要運行的下一個任務、提供給調度器等等。每個 調度器類都在一對一連接的列表中彼此相連，使類可以迭代（例如， 要啟用給定處理器的禁用）。注意，將任務函數加入隊列或脫離隊列只需從特定調度結構中加入或移除任務。 核心函數 pick_next_task 選擇要執行的下一個任務（取決於調度類的具體策略）。

sched_rt.c, sched_fair.c, sched_idletask.c等（都在kernel/目錄下）就是不同的調度算法實現。不要忘了調度類是任務結構本身的一部分（參見 task_struct）。這一點簡化了任務的操作，無論其調度類如何。因為進程描述符中有sched_class引用，這樣就可以直接通過進程描述符來 調用調度類中的各種操作。在調度類中，隨著調度域的增加，其功能也在增加。 這些域允許您出於負載平衡和隔離的目的將一個或多個處理器按層次關係分組。 一個或多個處理器能夠共享調度策略（並在其之間保持負載平衡），或實現獨立的調度策略。

可運行隊列：struct rq

調度程序每次在進程發生切換時，都要從可運行隊列中選取一個最佳的進程來運行。Linux內核使用rq數據結構（以前的內核中該結構為 runqueue）表示一個可運行隊列信息（也就是就緒隊列），每個CPU都有且只有一個這樣的結構。該結構在kernel/sched.c中，不僅描述 了每個處理器中處於可運行狀態（TASK_RUNNING），而且還描述了該處理器的調度信息。如下：


```c
struct rq {  
    /* runqueue lock: */  
    spinlock_t lock;  
  
    unsigned long nr_running;  
    #define CPU_LOAD_IDX_MAX 5  
    unsigned long cpu_load[CPU_LOAD_IDX_MAX];  
  
    /* capture load from *all* tasks on this cpu: */  
    struct load_weight load;  
    unsigned long nr_load_updates;  
    u64 nr_switches;  
    u64 nr_migrations_in;  
  
    struct cfs_rq cfs;  
    struct rt_rq rt;  
  
    unsigned long nr_uninterruptible;  
  
    struct task_struct *curr, *idle;  
    unsigned long next_balance;  
    struct mm_struct *prev_mm;  
  
    u64 clock;  
  
    atomic_t nr_iowait;  
  
  
    /* calc_load related fields */  
    unsigned long calc_load_update;  
    long calc_load_active;  
  
......  
};  
```

進程調度的入口點是函數schedule()，該函數調用pick_next_task()，pick_next_task()會以優先級為序，從高到低，一次檢查每一個調度類，且從最高優先級的調度類中，選擇最高優先級的進程。


```c
asmlinkage void __sched schedule(void)  
{  
    struct task_struct *prev, *next;  
    unsigned long *switch_count;  
    struct rq *rq;  
    int cpu;  
  
need_resched:  
    preempt_disable();  
    cpu = smp_processor_id();  
    rq = cpu_rq(cpu);  
    rcu_sched_qs(cpu);  
    prev = rq->curr;  
    switch_count = &prev->nivcsw;  
  
    release_kernel_lock(prev);  
need_resched_nonpreemptible:  
  
    schedule_debug(prev);  
  
    if (sched_feat(HRTICK))  
        hrtick_clear(rq);  
  
    spin_lock_irq(&rq->lock);  
    update_rq_clock(rq);  
    clear_tsk_need_resched(prev);  
  
    if (prev->state && !(preempt_count() & PREEMPT_ACTIVE)) {  
        if (unlikely(signal_pending_state(prev->state, prev)))  
            prev->state = TASK_RUNNING;  
        else  
            deactivate_task(rq, prev, 1);  
        switch_count = &prev->nvcsw;  
    }  
  
    pre_schedule(rq, prev);  
  
    if (unlikely(!rq->nr_running))  
        idle_balance(cpu, rq);  
  
    put_prev_task(rq, prev);  
    <strong>next = pick_next_task(rq); //挑選最高優先級別的任務
  
    if (likely(prev != next)) {  
        sched_info_switch(prev, next);  
        perf_event_task_sched_out(prev, next, cpu);  
  
        rq->nr_switches++;  
        rq->curr = next;  
        ++*switch_count;  
  
        context_switch(rq, prev, next); /* unlocks the rq */  
        /* 
         * the context switch might have flipped the stack from under 
         * us, hence refresh the local variables. 
         */  
        cpu = smp_processor_id();  
        rq = cpu_rq(cpu);  
    } else  
        spin_unlock_irq(&rq->lock);  
  
    post_schedule(rq);  
  
    if (unlikely(reacquire_kernel_lock(current) < 0))  
        goto need_resched_nonpreemptible;  
  
    preempt_enable_no_resched();  
    if (need_resched())  
        goto need_resched;  
}  
```

```c
static inline struct task_struct*
pick_next_task(struct rq* rq)
{
    const struct sched_class* class;
    struct task_struct* p;

    /*
     * Optimization: we know that if all tasks are in
     * the fair class we can call that function directly:
     */
    if (likely(rq->nr_running == rq->cfs.nr_running)) {
        p = fair_sched_class.pick_next_task(rq);

        if (likely(p)) {
            return p;
        }
    }

    //從最高優先級類開始，遍歷每一個調度類。每一個調度類都實現了，他會返回指向下一個可運行進程的指針，沒有時返回NULL。

    for (; ;) {
        p = class->pick_next_task(rq);

        if (p) {
            return p;
        }

        /*
         * Will never be NULL as the idle class always
         * returns a non-NULL p:
         */
    }
}
```

被阻塞（休眠）的進程處於不可執行狀態，是不能被調度的。進程休眠一般是由於等待一些事件，內核首先把自己標記成休眠狀態，從可執行紅黑樹中移出，放入等待隊列，然後調用schedule()選擇和執行一個其他進程。喚醒的過程剛好相反，進程設置為可執行狀態，然後從等待隊列中移到可執行紅黑樹中。    

等待隊列是由等待某些事件發生的進程組成的簡單鏈表。內核用wake_queue_head_t來代表隊列。進程把自己放入等待隊列中並設置成不可執狀態。當等待隊列相關事件發生時，隊列上進程會被喚醒。函數inotify_read()是實現等待隊列的一個典型用法：

```c
static ssize_t inotify_read(struct file* file, char __user* buf,
                            size_t count, loff_t* pos)
{
    struct fsnotify_group* group;
    struct fsnotify_event* kevent;
    char __user* start;
    int ret;
    DEFINE_WAIT(wait);

    start = buf;
    group = file->private_data;

    while (1) {

        //進程的狀態變更為TASK_INTERRUPTIBLE或TASK_UNINTERRUPTIBLE。
        prepare_to_wait(&group->notification_waitq, &wait, TASK_INTERRUPTIBLE);

        mutex_lock(&group->notification_mutex);
        kevent = get_one_event(group, count);
        mutex_unlock(&group->notification_mutex);

        if (kevent) {
            ret = PTR_ERR(kevent);

            if (IS_ERR(kevent)) {
                break;
            }

            ret = copy_event_to_user(group, kevent, buf);
            fsnotify_put_event(kevent);

            if (ret < 0) {
                break;
            }

            buf += ret;
            count -= ret;
            continue;
        }

        ret = -EAGAIN;

        if (file->f_flags & O_NONBLOCK) {
            break;
        }

        ret = -EINTR;

        if (signal_pending(current)) {
            break;
        }

        if (start != buf) {
            break;
        }

        schedule();
    }

    finish_wait(&group->notification_waitq, &wait);

    if (start != buf && ret != -EFAULT) {
        ret = buf - start;
    }

    return ret;
}
```

喚醒是通過wake_up()進行。她喚醒指定的等待隊列上 的所有進程。它調用try_to_wake_up,該函數負責將進程設置為TASK_RUNNING狀態，調用active_task()將此進程放入可 執行隊列，如果被喚醒進程的優先級比當前正在執行的進程的優先級高，還要設置need_resched標誌。

上下文切換，就是從一個可執行進程切換到另一個可執行進程，由定義在kernel/sched.c的context_switch函數負責處理。每當一個新的進程被選出來準備投入運行的時候，schedule就會調用該函數。它主要完成如下兩個工作：

`1.`調用定義在include/asm/mmu_context.h中的switch_mm().該函數負責把虛擬內存從上一個進程映射切換到新進程中。

`2.`調用定義在include/asm/system.h的switch_to(),該函數負責從上一個進程的處理器狀態切換到新進程的處理器狀態，這包括保存，恢復棧信息和寄存器信息。

內核也必須知道什麼時候調用schedule(),單靠用戶 代碼顯示調用schedule(),他們可能就會永遠地執行下去，相反，內核提供了一個need_resched標誌來表明是否需要重新執行一次調度。當 某個進程耗盡它的時間片時，scheduler_tick()就會設置這個標誌，當一個優先級高的進程進入可執行狀態的時 候，try_to_wake_up()也會設置這個標誌。內核檢查該標誌，確認其被設置，調用schedule()來切換到一個新的進程。該標誌對內核來講是一個信息，它表示應當有其他進程應當被運行了。

用於訪問和操作need_resched的函數：

<table width="743" cellpadding="2" cellspacing="0" border="1">
<tbody>
<tr>
<td valign="top" width="353"><span style="font-family:微軟雅黑; font-size:14px">set_tsk_need_resched(task)</span></td>
<td valign="top" width="388"><span style="font-family:微軟雅黑; font-size:14px">設置指定進程中的need_resched標誌</span></td>
</tr>
<tr>
<td valign="top" width="353"><span style="font-family:微軟雅黑; font-size:14px">clear_tsk_need_resched(task)</span></td>
<td valign="top" width="388"><span style="font-family:微軟雅黑; font-size:14px">清除指定進程中的nedd_resched標誌</span></td>
</tr>
<tr>
<td valign="top" width="353"><span style="font-family:微軟雅黑; font-size:14px">need_resched()</span></td>
<td valign="top" width="388"><span style="font-family:微軟雅黑; font-size:14px">檢查need_resched標誌的值，如果被設置就返回真，否則返回</span></td>
</tr>
</tbody>
</table>

再返回用戶空間以及從中斷返回的時候，內核也會檢查 need_resched標誌，如果已被設置，內核會在繼續執行之前調用該調度程序。最後，每個進程都包含一個need_resched標誌，這是因為訪 問進程描述符內的數值要比訪問一個全局變量要快(因為current宏速度很快並且描述符通常都在高速緩存中)。在2.6內核中，他被移到了 thread_info結構體裡。


###用戶搶佔發生在一下情況：
1. 從系統調用返回時；
2. 從終端處理程序返回用戶空間時。

###內核搶佔發生在:

1. 中斷處理正在執行，且返回內核空間前；
2. 內核代碼再一次具有可搶佔性的時候；
3. 內核任務顯示調用schedule()函數；
4. 內核中的任務阻塞的時候。
