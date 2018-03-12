---
title: Kernel 调度系统
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,scheduling policy,调度策略
description: 操作系统的调度算法必须实现几个互相冲突的目标：进程响应时间尽可能快，后台作业的吞吐量尽可能高，进程的饥饿现象尽可能避免，低优先级和高优先级进程的需要尽可能调和等等。什么时候以怎样的方式选择一个进程运行，就是所谓的调度策略 (Scheduling Policy)。本文中，介绍下 Linux Kernel 中时如何实现的。
---

操作系统的调度算法必须实现几个互相冲突的目标：进程响应时间尽可能快，后台作业的吞吐量尽可能高，进程的饥饿现象尽可能避免，低优先级和高优先级进程的需要尽可能调和等等。

什么时候以怎样的方式选择一个进程运行，就是所谓的调度策略 (Scheduling Policy)。

本文中，介绍下 Linux Kernel 中时如何实现的。

<!-- more -->

## 调度器

Linux 的调度算法通过模块的方式实现，每种类型的调度器会有一个优先级，调度时会按照优先级遍历调度类，拥有一个可执行进程的最高优先级的调度器类胜出，去选择下面要执行的那个程序。

### 调度类型

目前 Linux 中主要有两大类调度算法，CFS (完全公平调度算法) 以及 实时调度算法，在应用中，可以通过 ```sched_setscheduler()``` 函数修改进程的调度策略，目前有 5 种调度策略：

* SCHED_NORMAL<br>最常用的，调度策略主要用于 CFS 调度，存在静态优先级和动态优先级。
* SCHED_BATCH<br>除了不能抢占外，与上相同，可让任务延长执行的时间 (time slice)，减小上下文切换的次数，以提高 cache 的利用率 (每次 context-switch 都会导致 cache-flush)。<br>该调度策略适用于周期批量执行的任务，而不适合交互性的产品，主要是由于任务的切换延迟，让人感觉系统性能不佳。
* SCHED_IDLE<br>它甚至比 nice 19 还弱，用于空闲时需要处理的任务。
* SCHED_RR<br>多次循环调度，拥有时间片，结束后会放在队列末尾。
* SCHED_FIFO<br>先进先出规则，一次机会做完，没有时间片可以运行任意长的时间。

其中前面三种策略使用的是 CFS 调度器类，后面两种使用 RT 调度器类。任何时候，实时进程的优先级都高于普通进程，实时进程只会被更高级的实时进程抢占。

### 调度时机

Linux 调度时机主要有：

1. 进程状态转换的时刻：进程终止、进程睡眠；<br>进程调用 ```sleep()``` 或 ```exit()``` 等进行状态转换，此时会主动调用调度程序进行进程调度；

2. 当前进程的时间片用完时（current->counter=0）<br>由于进程的时间片是由时钟中断来更新的，因此，这种情况和时机 4 是一样的。

3. 设备驱动程序<br>当设备驱动程序执行长而重复的任务时，直接调用调度程序。在每次反复循环中，驱动程序都检查 ```need_resched``` 的值，如果必要，则调用调度程序 ```schedule()``` 主动放弃 CPU 。

4. 进程从中断、异常及系统调用返回到用户态时<br>如前所述，不管是从中断、异常还是系统调用返回，最终都调用 ```ret_from_sys_call()```，由这个函数进行调度标志的检测，如果必要，则调用调用调度程序。

对于最后一条，那么为什么从系统调用返回时要调用调度程序呢？这主要是从效率考虑，从系统调用返回意味着要离开内核态而返回到用户态，而状态的转换要花费一定的时间，因此，在返回到用户态前，系统把在内核态该处理的事全部做完。

### SMP 负载均衡

在目前的 CFS 调度器中，每个 CPU 维护本地 RQ 所有进程的公平性，为了实现跨 CPU 的调度公平性，CFS 必须定时进行 load balance，将一些进程从繁忙的 CPU 的 RQ 中移到其他空闲的 RQ 中。

这个 load balance 的过程需要获得其它 RQ 的锁，这种操作降低了多运行队列带来的并行性。当然，load balance 引入的加锁操作依然比全局锁的代价要低，这种代价差异随着 CPU 个数的增加而更加显著。

但是，如果系统中的 CPU 个数有限，多 RQ 的优势便不明显了；而采用单一队列，每一个需要调度的新进程都可以在全局范围内查找最合适的 CPU ，而无需 CFS 那样等待 load balance 代码来决定，这减少了多 CPU 之间裁决的延迟，最终的结果是更小的调度延迟。

也就是说，CFS 为了维护多 CPU 上的公平性，所采用的负载平衡机制，可能会抵消了 per cpu queue 曾带来的好处。

## CFS 完全公平调度

Completely Fair Schedule, CFS 在 2.6.23 引入，同时包括了模块化、完全公平调度、组调度等一系列特性；按作者的说法：CFS 80% 的工作可以用一句话来概括，**CFS 在真实的硬件上模拟了理想且精确的多任务处理器**。

{% highlight text %}
80% of CFS's design can be summed up in a single sentence: CFS basically models
an "ideal, precise multi-tasking CPU" on real hardware.
{% endhighlight %}

关于 CFS 的简单介绍可以查看内核文档 [sched-design-CFS.txt]( {{ site.kernel_docs_url }}/Documentation/scheduler/sched-design-CFS.txt ) 。

### 简介

该模型是从 RSDL/SD 中吸取了完全公平的思想，不再跟踪进程的睡眠时间，也不再企图区分交互式进程，它将所有的进程都统一对待，在既定的时间内每个进程都获得了公平的 CPU 占用时间，调度的依据就是每个进程的权重，这就是公平的含义。

为了实现完全公平调度，内核引入了虚拟时钟（virtual clock）的概念，统计进程已经运行的时间采用虚拟运行时间 (virtual runtime)，该值与具体的时钟晶振没有关系，只是为了公平分配 CPU 时间而提出的一种时间量度。

vt 是递增的，该值与其实际的运行时间成正比，与权重成反比；也就是说权重越高，对应的优先级越高，进而该进程虚拟时钟增长的就慢。权重的计算与静态优先级相关，该值在 ```set_load_weight()``` 函数中设置。

注意，优先级和权重之间的关系并不是简单的线性关系，内核使用一些经验数值来进行转化。

如上所述，这就意味着，vt 用来作为对进程进行排序的参考，而不能用来反映进程真实执行时间。

<!--
CFS 的核心思想是把 CPU 总时间按运行队列所占权重进行分配，即占用 CPU 后能够执行的理想运行时间 (ideal runtime)，而在调度时选择 vt 最小的运行，并且保证在某个时间周期 (__sched_period) 内运行队列里的所有调度单元都能够至少被调度执行一次。

并且为了保证新进程能够获得合理的 vruntime，不至于一开始太小导致长期占用 cpu，在每个 cfs_rq 上维护了一个 min_vruntime，所有调度单元的 vruntime 调整按它自己 cfs_rq 中的 min_vruntime 做为基准。

假设有 a、b、c 三个进程，权重分别是1、2、3,那么所有的进程的权重和为6, 按照cfs的公平原则来分配，那么a的重要性为1/6, b、c 为2/6, 3/6。这样如果a、b、c 运行一次的总时间为6个时间单位，a占1个，b占2个，c占3个。
-->



### FAQs

CFS 的基本原理是在一个调度周期 (sched_latency_ns) 内让每个进程至少有机会运行一次，也就是说每个进程等待 CPU 的最长时间不超过这个调度周期。

然后根据进程的数量平分这个调度周期内的 CPU 使用权，由于进程的优先级 (nice) 不同，分割调度周期的时候要加权；每个进程的累计运行时间保存在自己的 vruntime 字段里，哪个进程的 vruntime 最小就获得本轮运行的权利。

#### 新进程vruntime的初始值是0？

假如新进程 vruntime 的初值为 0 的话，也就是比老进程的值小很多，那么它在相当长的时间内都会保持抢占 CPU 的优势，老进程就要饿死了，这显然是不公平的。

所以 CFS 为每个 CPU 的运行队列 cfs_rq 维护了一个 min_vruntime 字段，记录该运行队列中所有进程的最小 vruntime 值，新进程的初始 vruntime 值就以其所在运行队列的 min_vruntime 为基础来设置，与老进程保持在合理的差距范围内。

<!--
新进程的vruntime初值的设置与两个参数有关：
sched_child_runs_first：规定fork之后让子进程先于父进程运行;
sched_features的START_DEBIT位：规定新进程的第一次运行要有延迟。

注：
sched_features是控制调度器特性的开关，每个bit表示调度器的一个特性。在sched_features.h文件中记录了全部的特性。START_DEBIT是其中之一，如果打开这个特性，表示给新进程的vruntime初始值要设置得比默认值更大一些，这样会推迟它的运行时间，以防进程通过不停的fork来获得cpu时间片。

如果参数 sched_child_runs_first打开，意味着创建子进程后，保证子进程会在父进程之前运行。

子进程在创建时，vruntime初值首先被设置为min_vruntime；然后，如果sched_features中设置了START_DEBIT位，vruntime会在min_vruntime的基础上再增大一些。设置完子进程的vruntime之后，检查sched_child_runs_first参数，如果为1的话，就比较父进程和子进程的vruntime，若是父进程的vruntime更小，就对换父、子进程的vruntime，这样就保证了子进程会在父进程之前运行。
-->

#### 休眠进程的vruntime一直保持不变吗？

如果休眠进程的 vruntime 保持不变，而其它运行进程的 vruntime 一直在增加，那么休眠进程唤醒时，由于其 vruntime 相比要小很多，就会使它获得长时间抢占 CPU 的优势，从而导致其它进程饿死。

为此，CFS 会在休眠进程被唤醒时重新设置 vruntime 值，以 min_vruntime 值为基础，并进行一定的补偿。

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
    if (initial && sched_feat(START_DEBIT))  /* initial表示新进程 */
        vruntime += sched_vslice(cfs_rq, se);

    /* sleeps up to a single latency don't count. */
    if (!initial) { /* 休眠进程 */
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

#### 休眠进程在唤醒时会立刻抢占CPU吗？

这由 CFS 的唤醒抢占特性决定，也即 sched_features 的 WAKEUP_PREEMPT 位。

由于休眠进程在唤醒时会获得 vruntime 的补偿，所以在醒来时抢占 CPU 是大概率事件，这也是 CFS 调度算法的本意，即保证交互式进程的响应速度，因为交互式进程等待用户输入会频繁休眠。

<!--
除了交互式进程以外，主动休眠的进程同样也会在唤醒时获得补偿，例如通过调用sleep()、nanosleep()的方式，定时醒来完成特定任务，这类进程往往并不要求快速响应，但是CFS不会把它们与交互式进程区分开来，它们同样也会在每次唤醒时获得vruntime补偿，这有可能会导致其它更重要的应用进程被抢占，有损整体性能。我曾经处理过的一个案例：服务器上有两类应用进程，A进程定时循环检查有没有新任务，如果有的话就简单预处理后通知B进程，然后调用nanosleep()主动休眠，醒来后再重复下一个循环；B进程负责数据运算，是CPU消耗型的；B进程的运行时间很长，而A进程每次运行时间都很短，但睡眠/唤醒却十分频繁，每次唤醒就会抢占B，导致B的运行频繁被打断，大量的进程切换带来很大的开销，整体性能下降很厉害。那有什么办法吗？有，CFS可以禁止唤醒抢占 特性：
# echo NO_WAKEUP_PREEMPT > /sys/kernel/debug/sched_features

禁用唤醒抢占 特性之后，刚唤醒的进程不会立即抢占运行中的进程，而是要等到运行进程用完时间片之后。在以上案例中，经过这样的调整之后B进程被抢占的频率大大降低了，整体性能得到了改善。

如果禁止唤醒抢占特性对你的系统来说太过激进的话，你还可以选择调大以下参数：

sched_wakeup_granularity_ns
这个参数限定了一个唤醒进程要抢占当前进程之前必须满足的条件：只有当该唤醒进程的vruntime比当前进程的vruntime小、并且两者差距(vdiff)大于sched_wakeup_granularity_ns的情况下，才可以抢占，否则不可以。这个参数越大，发生唤醒抢占就越不容易。
-->

#### 进程占用的CPU时间片可以无穷小吗？

假设有两个进程，它们的 vruntime 初值一样，当其中的一个进程运行后，它的 vruntime 就比另外的进大了，那么正在运行的进程什么时候会被抢占呢？

答案是：为了避免进程切换过于频繁造成太大的资源消耗，CFS 设定了进程占用 CPU 的最小时间值 (sched_min_granularity_ns)，正在 CPU 上运行的进程如果不足这个时间是不可以被调离 CPU 的。

另外，CFS 默认会把调度周期 sched_latency_ns 按照进程的数量平分，给每个进程平均分配相同的 CPU 时间片，但是如果进程数量太多的话，就会造成 CPU 时间片太小，如果小于上述的最小值，那么就以最小值为准，而调度周期也不再遵守 sched_latency_ns 。

<!-- 而是以 (sched_min_granularity_ns * 进程数量) 的乘积为准。-->

#### 进程切换CPU时vruntime会不会改变？

在 SMP 系统上，当 CPU 的负载不同时会进行负载均衡，而每个 CPU 都有自己的运行队列，而每个队列中的 vruntime 也各不相同，比如可以对比下每个运行队列的 min_vruntime 值：

{% highlight text %}
# grep min_vruntime /proc/sched_debug
  .min_vruntime                  : 526279705.695991
  .min_vruntime                  : 532370994.256253
  .min_vruntime                  : 720871453.830955
  .min_vruntime                  : 692323575.852029
{% endhighlight %}

显然，如果对 vruntime 不做处理直接切换，必然会导致不公平。

当进程从一个 CPU 的中出队 (dequeue_entity) 时，它的 vruntime 要减去队列的 min_vruntime 值；而当进程加入另一个 CPU 的运行队列 (enqueue_entiry) 时，它的 vruntime 要加上该队列的 min_vruntime 值。

这样，进程从一个 CPU 迁移到另一个 CPU 之后 vruntime 保持相对公平。

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




## 源码解析

接下来看看 Linux 中代码的实现。

### 相关结构体

Linux2.6.24 内核采用分层管理调度，也就是两层：第一层被称为核心调度器，在核心调度器下面为调度器类。调度算法实现相关的数据结构主要有运行实体 (struct rq)、调度类 (struct sched_class) 和运行队列。

![scheduler stucture]({{ site.url }}/images/linux/kernel/scheduler_stucture.png "scheduler stucture"){: .pull-center }

首先是进程描述符中与调度相关的信息。

{% highlight c %}
struct task_struct {
    ... ...
    int prio, static_prio, normal_prio;      // 进程优先级
    unsigned int rt_priority;                // RT优先级
    const struct sched_class *sched_class;   // 响应的调度类
    struct sched_entity se;
    struct sched_rt_entity rt;
    struct sched_dl_entity dl;
    unsigned int policy;                     // 调度策略，默认为SCHED_NORMAL
    cpumask_t cpus_allowed;                  // 限制此进程在哪个处理器上运行
    ... ...
};
{% endhighlight %}

就绪队列用于存储一些基本的用于调度的信息，包括实时调度的、CFS 调度的以及 DL 调度的，两者属于不同的调度实体，每个 CPU 会有一个 rq 结构体，也就是就绪队列。

{% highlight c %}
struct rq {                      // @ kernel/sched/sched.h
    spinlock_t lock;                     // 锁
    unsigned long nr_running;            // 当前就绪对列进程的数目
    struct load_weight load;             // 当前就绪队列负荷
    struct task_struct *curr, *idle;     // 分别指向当前以及空闲进程描述符
    struct cfs_rq cfs;                   // 分别表示三个不同的就绪队列
    struct rt_rq rt;
    struct dl_rq dl;
    u64 clock;                           // 就绪队列的时钟，这个是周期更新的，真实的系统晶振时钟
};

struct cfs_rq {                  // @ kernel/sched/sched.h
    struct load_weight load;             // 队列上所有进程的权重值weight总和
    unsigned int nr_running;             // 当前就绪队列的进程数=队列进程的总数+正在运行的那个进程
    struct sched_entity *curr;           // 指向当前进程的运行实体
    struct rb_root tasks_timeline;       // 就绪队列的树跟
    struct rb_node *rb_leftmost;         // 保存红黑树最左边的节点，直接作为下一个运行进程
};
{% endhighlight %}

调度类 ```sched_class``` 为模块编程的上层支持，对于每个 Linux 新添加进来的调度算法都有自己的调度类实例，包括了 ```fair_sched_class```、```rt_sched_class```、```dl_sched_class```、```idle_sched_class``` 等。

{% highlight c %}
struct sched_class {            // @ kernel/sched/sched.h
    const struct sched_class *next;                                           // 调度类组成单向链表
    void (*enqueue_task) (struct rq *rq, struct task_struct *p, int wakeup);  // 向就绪队列插入进程
    void (*dequeue_task) (struct rq *rq, struct task_struct *p, int flags);   // 从就绪队列中删除
    void (*yield_task) (struct rq *rq);                                       // 进程主动放弃处理器
    void (*check_preempt_curr) (struct rq *rq, struct task_struct *p);        // 用一个新唤醒的进程抢占当前进程
    struct task_struct * (*pick_next_task) (struct rq *rq);                   // 选择下一个将要运行的进程
    void (*put_prev_task) (struct rq *rq, struct task_struct *p);
    void (*task_tick) (struct rq *rq, struct task_struct *p);                 // 由周期调度器调用
    void (*task_new) (struct rq *rq, struct task_struct *p);                  // 每次建立新进程调用此函数通知调度器
};
{% endhighlight %}

调度运行队列，也就是就绪队列，用于保存处于 ready 状态的进程，不同的调度算法使用不同的运行队列，对于 CFS 调度，运用的是红黑树；而对于实时调度为组链表。

运行实体，也就是调度单位，对应的结构为 ```sched_entity```。调度器的调度单位不再是进程，而是可调度的实体，可以将多个进程捆绑在一起作为一个调度单位 (即调度实体) 进行调度，也就是说可调度实体可以是一个进程，也可以是多个进程构成的一个组。

另外，由于 CFS 不再有时间片的概念，但仍需要对每个进程运行的时间记账，从而确保每个进程只在公平分配给他的处理器时间内运行，相关信息同样保存在 sched_entity 中。

{% highlight c %}
struct sched_entity {
    unsigned int        on_rq;                    // 是否在运行队列或正在执行，当前任务不会保存在红黑树中
    struct load_weight  load;                     // 该调度实体的权重，决定了运行时间以及被调用次数
    u64                 vruntime;                 // 存放进程的虚拟运行时间，用于调度器的选择
    u64                 exec_start                // 记录上次执行update_curr()的时间
    u64                 sum_exec_runtime;         // 进程总共执行的cpu clock，占用cpu的物理时间
    u64                 pre_sum_exec_runtime;     // 进程在切换经CPU时的sum_exec_runtime值
};
{% endhighlight %}

每个 ```task_struct``` 都嵌入了一个 ```sched_entity```，这也就是为什么进程也是一个可调度实体。

### 代码实现

调度器的核心代码在 ```kernel/sched``` 目录下，包括 ```sched_init()```、```schedule()```、```scheduler_tick()``` 等，其中核心调度器包括了：

* 周期性调度器，schedule_tick()<br>不负责进程的切换，只是定时更新调度相关的统计信息，以备主调度器使用。

* 主调度器，schedule()<br>完成进程的切换，将 CPU 的使用权从一个进程切换到另一个进程。

我们知道在通过 ```fork()```、```vfork()```、```clone()``` 等函数时，进程创建最终都会调用 ```do_fork()```，而该函数会调用 ```copy_process()->sched_fork()``` 。

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

在 ```sched_fork()``` 函数中会复制父进程的优先级，并将 ```fair_sched_class``` 调度器类实例的地址赋给新进程中的 ```sched_class``` 指针。


### scheduler_tick()

```scheduler_tick()``` 会以 HZ 为周期调度，用来更新运行队列的时钟及 load，然后调用当前进程的调度器类的周期调度函数。

{% highlight text %}
scheduler_tick()
  |-update_rq_clock()                 更新运行队列的时钟rq->clock
  |-curr->sched_class->task_tick()    执行调度器的周期性函数，以CFS为例
  | |-task_tick_fair()                CFS对应的task_tick()
  |   |-entity_tick()
  |     |-update_curr()
  |     |-update_cfs_shares()         对SMP有效
  |     |-check_preempt_tick()        检查当前进程是否运行了足够长的时间，是否需要抢占
  |-update_cpu_load_active()          更新运行队列load，将数组中先前存储的load向后移动一个位置，并插入新值
{% endhighlight %}

在 ```update_cpu_load_active()``` 中，更新运行队列的 load 本质是将数组中先前存储的负荷值向后移动一个位置，将当前负荷记入数组的第一个位置。

另外，```check_preempt_tick()``` 函数用于检查当前进程是否运行了足够长的时间，如果超过了理想运行时间则无条件 resched；如果运行时间小于 ```sysctl_sched_min_granularity``` 那么也直接返回。

可以看到每个时钟都对会当前运行的 se 进行实质执行时间及虚拟执行时间进行更新，最后检查该进程是否运行的足够长的时间，如果是的话则将它置为 ```TIF_NEED_RESCHED``` 供主调度在适当的时机进行切换。

### schedule()

核心调度函数为 ```schedule()```，作为内核和其他部分用于调用进程调度器的入口，选择哪个进程可以运行，何时将其投入运行。

<!--
<pre style="font-size:0.8em; face:arial;">
schedule()
  |-sched_submit_work()
  |-__schedule()
     |-deactivate_task()                     // 从运行队列中删除prev进程，与具体的调度类相关
     |-pick_next_task()                      // 从各自的运行队列中选择下一个进程来运行
     |  |-class->pick_next_task()            // 调用调度类的相应函数，以CFS为例
     |  |-pick_next_task_fair()
     |     |-pick_next_entity()
     |-context_switch()                      // 执行进程的上下文切换
</pre>

schedule() 通常会找到一个最高优先级的调度类，查找自己的可运行队列，然后找到下一个该运行的进程，该函数唯一重要的事情是调用 pick_next_task() 。<br><br>

其主要流程为：<ol><li>
将当前进程从相应的运行队列中删除；</li><br><li>

计算和更新调度实体和进程的相关调度信息；</li><br><li>

将当前进重新插入到调度运行队列中，对于CFS调度，根据具体的运行时间进行插入而对于实时调度插入到对应优先级队列的队尾；</li><br><li>

从运行队列中选择运行的下一个进程；</li><br><li>

进程调度信息和上下文切换；
</li></ol>
当进程上下文切换后（关于进程切换在前面的文章中有介绍），调度就基本上完成了，当前运行的进程就是切换过来的进程了。
在上述结构体中，sum_exec_runtime - pre_sum_exec_runtime 等于进程获得 CPU 使用权后总的总时间，也就是 ideal_runtime 已被消耗了多少。<br><br>

在抽象模型中，计算 ideal_runtime 的时候需要求所有进程的权重值的和；而实际实现的时候，没有求和的过程，而是把该值记录在就绪队列的 load.weight 中。在向就绪队列中添加新进程时，就加上新进程的权重值，反之则减去。<br><br>

每个进程的 ideal_runtime 并没有用变量保存起来，而是在需要用到时用函数 sched_slice() 计算得到，计算规则为 (task->se.load.weight/cfs_rq->load.weight)*period 。<br><br>

period 也没有用变量来保存，用 __sched_period() 计算得到，sysctl_sched_latency * (nr_running / sysctl_nr_latency)。
-->

## 参考

<!--
http://linuxperf.com/?p=42

linux cfs调度器_理论模型
http://www.cnblogs.com/openix/archive/2013/08/13/3254394.html
 Linux内核学习笔记（一）CFS完全公平调度类
http://blog.chinaunix.net/uid-24757773-id-3266304.html
Linux内核-CFS进程调度器
https://wongxingjun.github.io/2015/09/09/Linux%E5%86%85%E6%A0%B8-CFS%E8%BF%9B%E7%A8%8B%E8%B0%83%E5%BA%A6%E5%99%A8/
Completely Fair Scheduler
http://www.linuxjournal.com/magazine/completely-fair-scheduler
linux内核分析——CFS
http://muyunzhe.blog.51cto.com/9164050/1651853
-->

{% highlight text %}
{% endhighlight %}
