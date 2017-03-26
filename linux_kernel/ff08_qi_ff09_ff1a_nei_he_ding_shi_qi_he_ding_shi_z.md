# （七）：内核定时器和定时执行


前面章节说到了把工作推后到除现在以外的时间执行的机制是下半部机制，但是当你需要将工作推后到某个确定的时间段之后执行，使用定时器是很好的选择。

上一节内核时间管理中讲到内核在始终中断发生执行定时器，定时器作为软中断在下半部上下文中执行。时钟中断处理程序会执行update_process_times函数，在该函数中运行run_local_timers()函数来标记一个软中断去处理所有到期的定时器。如下：

```c
void update_process_times(int user_tick)  
{  
    struct task_struct *p = current;  
    int cpu = smp_processor_id();  
    /* Note: this timer irq context must be accounted for as well. */  
    account_process_tick(p, user_tick);  
    run_local_timers();  
    rcu_check_callbacks(cpu, user_tick);  
    printk_tick();  
    scheduler_tick();  
    run_posix_cpu_timers(p);  
}  

void run_local_timers(void)  
{  
    hrtimer_run_queues();  
    raise_softirq(TIMER_SOFTIRQ);  
    softlockup_tick();  
}  
```

在分析定时器的实现之前我们先来看一看使用内核定时器的一个实例，具体使用可查看这篇文章：http://blog.csdn.net/shallnet/article/details/17734571，示例如下:


```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/timer.h>
#include <linux/delay.h>
struct timer_list   sln_timer;
void sln_timer_do(unsigned long l)
{
    mod_timer(&sln_timer, jiffies + HZ);
    printk(KERN_ALERT"param: %ld, jiffies: %ld\n", l, jiffies);
}

void sln_timer_set(void)
{
    init_timer(&sln_timer);
    sln_timer.expires = jiffies + HZ;   //1s
    sln_timer.function = sln_timer_do;
    sln_timer.data = 9527;
    add_timer(&sln_timer);
}

static int __init sln_init(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);
    sln_timer_set();
    return 0;
}

static void __exit sln_exit(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);
    del_timer(&sln_timer);
}

module_init(sln_init);
module_exit(sln_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("allen");
```

该示例作用是每秒钟打印出当前系统jiffies的值。
内核定时器由结构timer_list表示，定义在文件`<include/linux/timer.h>`中。


```c
struct timer_list {  
    struct list_head entry;  
    unsigned long expires;  
    void (*function)(unsigned long);  
    unsigned long data;  
    struct tvec_base *base;  
#ifdef CONFIG_TIMER_STATS  
    void *start_site;  
    char start_comm[16];  
    int start_pid;  
#endif  
#ifdef CONFIG_LOCKDEP  
    struct lockdep_map lockdep_map;  
#endif  
};  
```

如示例，内核提供部分操作接口来简化管理定时器，
`第一步`、定义一个定时器：


```c
struct timer_list   sln_timer;  
```

`第二步`、初始化定时器数据结构的内部值。

```c
init_timer(&sln_timer);//初始化定时器  
```

```c
#define init_timer(timer)\
    init_timer_key((timer), NULL, NULL)
void init_timer_key(struct timer_list* timer,
                    const char* name,
                    struct lock_class_key* key)
{
    debug_init(timer);
    __init_timer(timer, name, key);
}
static void __init_timer(struct timer_list* timer,
                         const char* name,
                         struct lock_class_key* key)
{
    timer->entry.next = NULL;
    timer->base = __raw_get_cpu_var(tvec_bases);
#ifdef CONFIG_TIMER_STATS
    timer->start_site = NULL;
    timer->start_pid = -1;
    memset(timer->start_comm, 0, TASK_COMM_LEN);
#endif
    lockdep_init_map(&timer->lockdep_map, name, key, 0);
}
```

`第三步`、填充timer_list结构中需要的值：

```c
sln_timer.expires = jiffies + HZ;   //1s后执行    
sln_timer.function = sln_timer_do;    //执行函数  
sln_timer.data = 9527;  
```

sln_timer.expires表示超时时间，它以节拍为单位的绝对计数值。如果当前jiffies计数等于或大于sln_timer.expires的值，那么sln_timer.function所指向的处理函数sln_timer_do就会执行，并且该函数还要使用长整型参数sln_timer.dat。
void sln_timer_do(unsigned long l)；

`第四步`、激活定时器：

```c
add_timer(&sln_timer);    //向内核注册定时器  
```

这样定时器就可以运行了。
add_timer()的实现如下：

```c
void add_timer(struct timer_list *timer)  
{  
    BUG_ON(timer_pending(timer));  
    mod_timer(timer, timer->expires);  
}
```

add_timer()调用了mod_timer()。mod_timer()用于修改定时器超时时间。

mod_timer(&sln_timer, jiffies + HZ);

由于add_timer()是通过调用mod_timer()来激活定时器，所以也可以直接使用mod_timer()来激活定时器，如果定时器已经初始化但没有激活，mod_timer()也会激活它。

如果需要在定时器超时前停止定时器，使用del_timer()函数来完成。

```c
del_timer(&sln_timer); 
```

该函数实现如下：

```c
int del_timer(struct timer_list* timer)
{
    struct tvec_base* base;
    unsigned long flags;
    int ret = 0;
    timer_stats_timer_clear_start_info(timer);

    if (timer_pending(timer)) {
        base = lock_timer_base(timer, &flags);

        if (timer_pending(timer)) {
            detach_timer(timer, 1);

            if (timer->expires == base->next_timer &&
                !tbase_get_deferrable(timer->base)) {
                base->next_timer = base->timer_jiffies;
            }

            ret = 1;
        }

        spin_unlock_irqrestore(&base->lock, flags);
    }

    return ret;
}

static inline void detach_timer(struct timer_list* timer,
                                int clear_pending)
{
    struct list_head* entry = &timer->entry;
    debug_deactivate(timer);
    __list_del(entry->prev, entry->next);

    if (clear_pending) {
        entry->next = NULL;
    }

    entry->prev = LIST_POISON2;
}
```

当使用del_timer()返回后，定时器就不会再被激活，但在多处理器机器上定时器上定时器中断可能已经在其他处理器上运行了，所以删除定时器时需要等待可能在其他处理器上运行的定时器处理I程序都退出，这时就要使用del_timer_sync()函数执行删除工作：

```c
del_timer_sync(&sln_timer);
```

该函数不能再中断上下文中使用。
该函数详细实现如下：

```c
int del_timer_sync(struct timer_list* timer)
{
#ifdef CONFIG_LOCKDEP
    unsigned long flags;
    local_irq_save(flags);
    lock_map_acquire(&timer->lockdep_map);
    lock_map_release(&timer->lockdep_map);
    local_irq_restore(flags);
#endif

    for (;;) {    //一直循环，直到删除timer成功再退出
        int ret = try_to_del_timer_sync(timer);

        if (ret >= 0) {
            return ret;
        }

        cpu_relax();
    }
}
int try_to_del_timer_sync(struct timer_list* timer)
{
    struct tvec_base* base;
    unsigned long flags;
    int ret = -1;
    base = lock_timer_base(timer, &flags);

    if (base->running_timer == timer) {
        goto out;
    }

    ret = 0;

    if (timer_pending(timer)) {
        detach_timer(timer, 1);

        if (timer->expires == base->next_timer &&
            !tbase_get_deferrable(timer->base)) {
            base->next_timer = base->timer_jiffies;
        }

        ret = 1;
    }

out:
    spin_unlock_irqrestore(&base->lock, flags);
    return ret;
}
```

一般情况下应该使用del_timer_sync()函数代替del_timer()函数，因为无法确定在删除定时器时，他是否在其他处理器上运行。为了防止这种情况的发生，应该调用del_timer_sync()函数而不是del_timer()函数。否则，对定时器执行删除操作后，代码会继续执行，但它有可能会去操作在其它处理器上运行的定时器正在使用的资源，因而造成并发访问，所有优先使用删除定时器的同步方法。

除了使用定时器来推迟任务到指定时间段运行之外，还有其他的方法处理延时请求。有的方法会在延迟任务时挂起处理器，有的却不会。实际上也没有方法能够保证实际的延迟时间刚好等于指定的延迟时间。

`1.` 最简单的 延迟方法是忙等待，该方法实现起来很简单，只需要在循环中不断旋转直到希望的时钟节拍数耗尽。比如：

```c
unsigned long delay = jiffies+10;   //延迟10个节拍  
while(time_before(jiffies,delay))  
        ；  
```

这种方法当代码等待时，处理器只能在原地旋转等待，它不会去处理其他任何任务。最好在任务等待时，允许内核重新调度其它任务执行。将上面代码修改如下：

```c
unsigned long delay = jiffies+10;   //10个节拍  
while(time_before(jiffies,delay))  
    cond_resched(); 
```

看一下cond_resched()函数具体实现代码：

```c
#define cond_resched() ({           \
        __might_sleep(__FILE__, __LINE__, 0);   \
        _cond_resched();            \
    })

int __sched _cond_resched(void)
{
    if (should_resched()) {
        __cond_resched();
        return 1;
    }

    return 0;
}

static void __cond_resched(void)
{
    add_preempt_count(PREEMPT_ACTIVE);
    schedule(); //最终还是调用schedule()函数来重新调度其它程序运行
    sub_preempt_count(PREEMPT_ACTIVE);
}
```

函数cond_resched()将重新调度一个新程序投入运行，但它只有在设置完need_resched标志后才能生效。换句话说，就是系统中存在更重要的任务需要运行。再由于该方法需要调用调度程序，所以它不能在中断上下文中使用----只能在进程上下文中使用。事实上，所有延迟方法在进程上下文中使用，因为中断处理程序都应该尽可能快的执行。另外，延迟执行不管在哪种情况下都不应该在持有锁时或者禁止中断时发生。

`2.` 有时内核需要更短的延迟，甚至比节拍间隔还要短。这时可以使用内核提供的ms、ns、us级别的延迟函数。

```c
void udelay(unsigned long usecs);    //arch/x86/include/asm/delay.h  
void ndelay(unsigned long nsecs);    //arch/x86/include/asm/delay.h  
void mdelay(unsigned long msecs);      
```

udelay()使用忙循环将任务延迟指定的ms后执行,其依靠执行数次循环达到延迟效果，mdelay()函数是通过udelay()函数实现，如下：

```c
#define mdelay(n) (\   
    (__builtin_constant_p(n) && (n)<=MAX_UDELAY_MS) ? udelay((n)*1000) : \  
    ({unsigned long __ms=(n); while (__ms--) udelay(1000);}))  
#endif 
```

udelay()函数仅能在要求的延迟时间很短的情况下执行，而在高速机器中时间很长的延迟会造成溢出。对于较长的延迟，mdelay()工作良好。



`3.`  schedule_timeout()函数是更理想的延迟执行方法。该方法会让需要延迟执行的任务睡眠到指定的延迟时间耗尽后再重新运行。但该方法也不能保证睡眠时间正好等于指定的延迟时间，只能尽量是睡眠时间接近指定的延迟时间。当指定的时间到期后，内核唤醒被延迟的任务并将其重新放回运行队列。用法如下：

```c
set_current_state(TASK_INTERRUPTIBLE);    //将任务设置为可中断睡眠状态  
schedule_timeout(s*HZ);    //小睡一会儿，“s”秒后唤醒  
```

唯一的参数是延迟的相对时间，单位是jiffies，上例中将相应的任务推入可中断睡眠队列，睡眠s秒。在调用函数schedule_timeout之前，不要要将任务设置成可中断或不和中断的一种，否则任务不会休眠。这个函数需要调用调度程序，所以调用它的代码必须保证能够睡眠，简而言之，调用代码必须处于进程上下文中，并且不能持有锁。

事实上schedule_timeout()函数的实现就是内核定时器的一个简单应用。


```c
signed long __sched schedule_timeout(signed long timeout)
{
    struct timer_list timer;
    unsigned long expire;

    switch (timeout) {
    case MAX_SCHEDULE_TIMEOUT:
        /*
         * These two special cases are useful to be comfortable
         * in the caller. Nothing more. We could take
         * MAX_SCHEDULE_TIMEOUT from one of the negative value
         * but I' d like to return a valid offset (>=0) to allow
         * the caller to do everything it want with the retval.
         */
        schedule();
        goto out;

    default:

        /*
         * Another bit of PARANOID. Note that the retval will be
         * 0 since no piece of kernel is supposed to do a check
         * for a negative retval of schedule_timeout() (since it
         * should never happens anyway). You just have the printk()
         * that will tell you if something is gone wrong and where.
         */
        if (timeout < 0) {
            printk(KERN_ERR "schedule_timeout: wrong timeout "
                   "value %lx\n", timeout);
            dump_stack();
            current->state = TASK_RUNNING;
            goto out;
        }
    }

    expire = timeout + jiffies;

    //下一行代码设置了超时执行函数process_timeout()。
    setup_timer_on_stack(&timer, process_timeout, (unsigned long)current);
    __mod_timer(&timer, expire, false, TIMER_NOT_PINNED);    //激活定时器
    schedule();    //调度其他新任务
    del_singleshot_timer_sync(&timer);

    /* Remove the timer from the object tracker */
    destroy_timer_on_stack(&timer);

    timeout = expire - jiffies;

out:
    return timeout < 0 ? 0 : timeout;
}
```

当定时器超时时，process_timeout()函数被调用：


```c
static void process_timeout(unsigned long __data)   
{  
    wake_up_process((struct task_struct *)__data);  
}  
```

当任务被重新调度时，将返回代码进入睡眠前的位置继续执行，位置正好在schedule()处。

进程上下文的代码为了等待特定时间发生，可以将自己放入等待队列。但是，等待队列上的某个任务可能既在等待一个特定事件到来，又在等待一个特定时间到期，就看谁来得更快。这种情况下，代码可以简单的使用scedule_timeout()函数代替schedule()函数，这样一来，当希望指定时间到期后，任务都会被唤醒，当然，代码需要检查被唤醒的原因，有可能是被事件唤醒，也有可能是因为延迟的时间到期，还可能是因为接收到了信号，然后执行相应的操作。

本文源码下载：
http://download.csdn.net/detail/gentleliu/8944281

