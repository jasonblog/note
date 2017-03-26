# （九）：进程调度


Linux为多任务系统，正常情况下都存在成百上千个任务。由于linux提供抢占式的多任务模式，所以linux能同时并发地交互执行多个进程，而调度程序将决定哪一个进程投入运行、何时运行、以及运行多长时间。调度程序是像linux这样的多任务操作系统的基础， 只有通过调度程序的合理调度，系统资源才能最大限度地发挥作用，多进程才会有并发执行的效果。当系统中可运行的进程数目比处理器的个数多，就注定在某一时刻有一些进程不能执行，这些不能执行的进程在等待执行。调度程序的基本工作就是停止一个进程的运行，再在这些等待执行的进程中选择一个来执行。

调度程序停止一个进程的运行，再选择一个另外进程的动作开始运行的动作被称作抢占（preemption）。一个进程在被抢占之前能够运行的时间是预先设置好的，这个预先设置好的时间就是进程的的时间片（timeslice）。时间片就是分配给每个可运行进程的处理器时间段，它表明进程在被抢占前所能持续运行时间。

处理器的调度策略决定调度程序在何时让什么进程投入运行。调度策略通常需要在进程响应迅速(相应时间短)和进程吞吐量高之间寻找平衡。所以调度程序通常采用一套非常复杂的算法来决定最值得运行的进程投入运行。调度算法中最基本的一类当然就是基于优先级的调度，也就是说优先级高的先运行，相同优先级的按轮转式进行调度。优先级高 的进程使用的时间片也长。调度程序总是选择时间片未用尽且优先级最高的进程运行。这句话就是说用户和系统可以通过设置进程的优先级来响应系统的调度。基于此，linux设计上一套动态优先级的调度方法。一开始，先为进程设置一个基本的优先级，然而它允许调度程序根据需要来加减优先级。linux内核提供了两组独立的优先级范围。第一种是nice值，范围从-20到19，默认为0。nice值越大优先级越小。另外nice值也用来决定分配给进程时间片的长短。linux下通过命令可以查看进程对应nice值，如下：



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

第二种范围是实时优先级，默认范围是从0到99。任何实时的优先级都高于普通优先级。

进程执行时，它会根据具体情况改变状态，进程状态是调度和对换的依据。Linux 将进程状态分为五种： TASK_RUNNING 、 TASK_INTERRUPTIBLE 、 TASK_UNINTERRUPTIBLE 、 TASK_STOPPED 和 TASK_ZOMBILE 。进程的状态随着进程的调度发生改变 。


<table cellpadding="2" cellspacing="0" border="1" style="background-color:#FFFFFF">
<tbody>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_RUNNING</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">可运行</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_INTERRUPTIBLE</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">可中断的等待状态</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_UNINTERRUPTIBLE</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">不可中断的等待状态</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_STOPPED</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">停止状态</span> </div>
</td>
</tr>
<tr>
<td valign="top" style="width:278px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px; color:#1A1A1A; background-color:#FFFFFF">TASK_TRACED</span><br>
</div>
</td>
<td valign="top" style="width:279px; height:31px">
<div><span style="font-family:Microsoft YaHei; font-size:14px">被跟踪状态</span> <br>
</div>
</td>
</tr>
</tbody>
</table>

`TASK_RUNNING （运行）`：无论进程是否正在占用 CPU ，只要具备运行条件，都处于该状态。 Linux 把处于该状态的所有 PCB 组织成一个可运行队列 run_queue ，调度程序从这个队列中选择进程运行。事实上， Linux 是将就绪态和运行态合并为了一种状态。

`TASK_INTERRUPTIBLE （可中断阻塞）`： Linux 将阻塞态划分成 TASK_INTERRUPTIBLE 、 TASK_UNINTERRUPTIBLE 、 TASK_STOPPED 三种不同的状态。处于 TASK_INTERRUPTIBLE 状态的进程在资源有效时被唤醒，也可以通过信号或定时中断唤醒。

`TASK_UNINTERRUPTIBLE （不可中断阻塞）`：另一种阻塞状态，处于该状态的进程只有当资源有效时被唤醒，不能通过信号或定时中断唤醒。在执行ps命令时，进程状态为D且不能被杀死。

`TASK_STOPPED （停止）`：第三种阻塞状态，处于该状态的进程只能通过其他进程的信号才能唤醒。

`TASK_TRACED （被跟踪）`：进程正在被另一个进程监视，比如在调试的时候。

我们在设置这些状态的时候是可以直接用语句进行的比如：p—>state = `TASK_RUNNING`。同时内核也会使用set_task_state()和set_current_state()函数来进行。

linux调度器是以模块方式提供的，这样允许不同类型的进程可以有针对性地选择调度算法。完全公平调度（CFS）是针对普通进程的调度类，CFS采用的方法是对时间片分配方式进行根本性的重新设计，完全摒弃时间片而是分配给进程一个处理器使用比重。通过这种方式，CFS确保了进程调度中能有恒定的公平性，而将切换频率置于不断变动之中。

与Linux 2.6之前调度器不同，2.6版本内核的CFS没有将任务维护在链表式的运行队列中，它抛弃了active/expire数组，而是对每个CPU维护一个以时间为顺序的红黑树。该树方法能够良好运行的原因在于：

* 红黑树可以始终保持平衡，这意味着树上没有路径比任何其他路径长两倍以上。
* 由于红黑树是二叉树，查找操作的时间复杂度为O(log n)。但是除了最左侧查找以外，很难执行其他查找，并且最左侧的节点指针始终被缓存。
* 对于大多数操作（插入、删除、查找等），红黑树的执行时间为O(log n)，而以前的调度程序通过具有固定优先级的优先级数组使用 O(1)。O(log n) 行为具有可测量的延迟，但是对于较大的任务数无关紧要。Molnar在尝试这种树方法时，首先对这一点进行了测试。

* 红黑树可通过内部存储实现，即不需要使用外部分配即可对数据结构进行维护。

要实现平衡，CFS使用“虚拟运行时”表示某个任务的时间量。任务的虚拟运行时越小，意味着任务被允许访问服务器的时间越短，其对处理器的需求越高。 CFS还包含睡眠公平概念以便确保那些目前没有运行的任务（例如，等待 I/O）在其最终需要时获得相当份额的处理器。由于篇幅原因，这里不详细讲解CFS的实现。

对于实时进程， Linux 采用了两种调度策略，即先来先服务调度（ First-In, First-Out ， FIFO ）和时间片轮转调度（ Round Robin ， RR ）。因为实时进程具有一定程度的紧迫性，所以衡量一个实时进程是否应该运行， Linux 采用了一个比较固定的标准。

下面是调度相关的一些数据结构：
调度实体：struct sched_entity

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

该结构在./linux/include/linux/sched.h中，表示一个可调度实体（进程，进程组，等等）。它包含了完整的调度信息，用于实现对单个任务或任务组的调度。调度实体可能与进程没有关联。这里包括负载权重load、对应的红黑树结点run_node、虚拟运行时vruntime（表示进程的运行时间，并作为红黑树的索引）、开始执行时间、最后唤醒时间、各种统计数据、用于组调度的CFS运行队列信息cfs_rq，等等。


调度类：struct sched_class
该调度类也在sched.h中，是对调度器操作的面向对象抽象，协助内核调度程序的各种工作。调度类是调度器管理器的核心，每种调度算法模块需要实现struct sched_class建议的一组函数。


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
其中的主要函数：
* enqueue_task：当某个任务进入可运行状态时，该函数将得到调用。它将调度实体（进程）放入红黑树中，并对 nr_running 变量加 1。从前面“Linux进程管理”的分析中可知，进程创建的最后会调用该函数。
* dequeue_task：当某个任务退出可运行状态时调用该函数，它将从红黑树中去掉对应的调度实体，并从 nr_running 变量中减 1。
* yield_task：在 compat_yield sysctl 关闭的情况下，该函数实际上执行先出队后入队；在这种情况下，它将调度实体放在红黑树的最右端。
* check_preempt_curr：该函数将检查当前运行的任务是否被抢占。在实际抢占正在运行的任务之前，CFS 调度程序模块将执行公平性测试。这将驱动唤醒式（wakeup）抢占。
* pick_next_task：该函数选择接下来要运行的最合适的进程。
* load_balance：每个调度程序模块实现两个函数，load_balance_start() 和 load_balance_next()，使用这两个函数实现一个迭代器，在模块的 load_balance 例程中调用。内核调度程序使用这种方法实现由调度模块管理的进程的负载平衡。
* set_curr_task：当任务修改其调度类或修改其任务组时，将调用这个函数。
* task_tick：该函数通常调用自 time tick 函数；它可能引起进程切换。这将驱动运行时（running）抢占。

调度类的引入是接口和实现分离的设计典范，你可以实现不同的调度算法（例如普通进程和实时进程的调度算法就不一样），但由于有统一的接口，使得调度策略 被模块化，一个Linux调度程序可以有多个不同的调度策略。调度类显著增强了内核调度程序的可扩展性。每个任务都属于一个调度类，这决定了任务将如何调 度。 调度类定义一个通用函数集，函数集定义调度器的行为。例如，每个调度器提供一种方式，添加要调度的任务、调出要运行的下一个任务、提供给调度器等等。每个 调度器类都在一对一连接的列表中彼此相连，使类可以迭代（例如， 要启用给定处理器的禁用）。注意，将任务函数加入队列或脱离队列只需从特定调度结构中加入或移除任务。 核心函数 pick_next_task 选择要执行的下一个任务（取决于调度类的具体策略）。

sched_rt.c, sched_fair.c, sched_idletask.c等（都在kernel/目录下）就是不同的调度算法实现。不要忘了调度类是任务结构本身的一部分（参见 task_struct）。这一点简化了任务的操作，无论其调度类如何。因为进程描述符中有sched_class引用，这样就可以直接通过进程描述符来 调用调度类中的各种操作。在调度类中，随着调度域的增加，其功能也在增加。 这些域允许您出于负载平衡和隔离的目的将一个或多个处理器按层次关系分组。 一个或多个处理器能够共享调度策略（并在其之间保持负载平衡），或实现独立的调度策略。

可运行队列：struct rq

调度程序每次在进程发生切换时，都要从可运行队列中选取一个最佳的进程来运行。Linux内核使用rq数据结构（以前的内核中该结构为 runqueue）表示一个可运行队列信息（也就是就绪队列），每个CPU都有且只有一个这样的结构。该结构在kernel/sched.c中，不仅描述 了每个处理器中处于可运行状态（TASK_RUNNING），而且还描述了该处理器的调度信息。如下：


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

进程调度的入口点是函数schedule()，该函数调用pick_next_task()，pick_next_task()会以优先级为序，从高到低，一次检查每一个调度类，且从最高优先级的调度类中，选择最高优先级的进程。


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
    <strong>next = pick_next_task(rq); //挑选最高优先级别的任务
  
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

    //从最高优先级类开始，遍历每一个调度类。每一个调度类都实现了，他会返回指向下一个可运行进程的指针，没有时返回NULL。

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

被阻塞（休眠）的进程处于不可执行状态，是不能被调度的。进程休眠一般是由于等待一些事件，内核首先把自己标记成休眠状态，从可执行红黑树中移出，放入等待队列，然后调用schedule()选择和执行一个其他进程。唤醒的过程刚好相反，进程设置为可执行状态，然后从等待队列中移到可执行红黑树中。    

等待队列是由等待某些事件发生的进程组成的简单链表。内核用wake_queue_head_t来代表队列。进程把自己放入等待队列中并设置成不可执状态。当等待队列相关事件发生时，队列上进程会被唤醒。函数inotify_read()是实现等待队列的一个典型用法：

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

        //进程的状态变更为TASK_INTERRUPTIBLE或TASK_UNINTERRUPTIBLE。
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

唤醒是通过wake_up()进行。她唤醒指定的等待队列上 的所有进程。它调用try_to_wake_up,该函数负责将进程设置为TASK_RUNNING状态，调用active_task()将此进程放入可 执行队列，如果被唤醒进程的优先级比当前正在执行的进程的优先级高，还要设置need_resched标志。

上下文切换，就是从一个可执行进程切换到另一个可执行进程，由定义在kernel/sched.c的context_switch函数负责处理。每当一个新的进程被选出来准备投入运行的时候，schedule就会调用该函数。它主要完成如下两个工作：

`1.`调用定义在include/asm/mmu_context.h中的switch_mm().该函数负责把虚拟内存从上一个进程映射切换到新进程中。

`2.`调用定义在include/asm/system.h的switch_to(),该函数负责从上一个进程的处理器状态切换到新进程的处理器状态，这包括保存，恢复栈信息和寄存器信息。

内核也必须知道什么时候调用schedule(),单靠用户 代码显示调用schedule(),他们可能就会永远地执行下去，相反，内核提供了一个need_resched标志来表明是否需要重新执行一次调度。当 某个进程耗尽它的时间片时，scheduler_tick()就会设置这个标志，当一个优先级高的进程进入可执行状态的时 候，try_to_wake_up()也会设置这个标志。内核检查该标志，确认其被设置，调用schedule()来切换到一个新的进程。该标志对内核来讲是一个信息，它表示应当有其他进程应当被运行了。

用于访问和操作need_resched的函数：

<table width="743" cellpadding="2" cellspacing="0" border="1">
<tbody>
<tr>
<td valign="top" width="353"><span style="font-family:微软雅黑; font-size:14px">set_tsk_need_resched(task)</span></td>
<td valign="top" width="388"><span style="font-family:微软雅黑; font-size:14px">设置指定进程中的need_resched标志</span></td>
</tr>
<tr>
<td valign="top" width="353"><span style="font-family:微软雅黑; font-size:14px">clear_tsk_need_resched(task)</span></td>
<td valign="top" width="388"><span style="font-family:微软雅黑; font-size:14px">清除指定进程中的nedd_resched标志</span></td>
</tr>
<tr>
<td valign="top" width="353"><span style="font-family:微软雅黑; font-size:14px">need_resched()</span></td>
<td valign="top" width="388"><span style="font-family:微软雅黑; font-size:14px">检查need_resched标志的值，如果被设置就返回真，否则返回</span></td>
</tr>
</tbody>
</table>

再返回用户空间以及从中断返回的时候，内核也会检查 need_resched标志，如果已被设置，内核会在继续执行之前调用该调度程序。最后，每个进程都包含一个need_resched标志，这是因为访 问进程描述符内的数值要比访问一个全局变量要快(因为current宏速度很快并且描述符通常都在高速缓存中)。在2.6内核中，他被移到了 thread_info结构体里。


###用户抢占发生在一下情况：
1. 从系统调用返回时；
2. 从终端处理程序返回用户空间时。

###内核抢占发生在:

1. 中断处理正在执行，且返回内核空间前；
2. 内核代码再一次具有可抢占性的时候；
3. 内核任务显示调用schedule()函数；
4. 内核中的任务阻塞的时候。
