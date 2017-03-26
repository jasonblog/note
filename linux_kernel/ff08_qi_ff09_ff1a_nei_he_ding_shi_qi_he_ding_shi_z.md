# （七）：內核定時器和定時執行


前面章節說到了把工作推後到除現在以外的時間執行的機制是下半部機制，但是當你需要將工作推後到某個確定的時間段之後執行，使用定時器是很好的選擇。

上一節內核時間管理中講到內核在始終中斷髮生執行定時器，定時器作為軟中斷在下半部上下文中執行。時鐘中斷處理程序會執行update_process_times函數，在該函數中運行run_local_timers()函數來標記一個軟中斷去處理所有到期的定時器。如下：

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

在分析定時器的實現之前我們先來看一看使用內核定時器的一個實例，具體使用可查看這篇文章：http://blog.csdn.net/shallnet/article/details/17734571，示例如下:


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

該示例作用是每秒鐘打印出當前系統jiffies的值。
內核定時器由結構timer_list表示，定義在文件`<include/linux/timer.h>`中。


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

如示例，內核提供部分操作接口來簡化管理定時器，
`第一步`、定義一個定時器：


```c
struct timer_list   sln_timer;  
```

`第二步`、初始化定時器數據結構的內部值。

```c
init_timer(&sln_timer);//初始化定時器  
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

`第三步`、填充timer_list結構中需要的值：

```c
sln_timer.expires = jiffies + HZ;   //1s後執行    
sln_timer.function = sln_timer_do;    //執行函數  
sln_timer.data = 9527;  
```

sln_timer.expires表示超時時間，它以節拍為單位的絕對計數值。如果當前jiffies計數等於或大於sln_timer.expires的值，那麼sln_timer.function所指向的處理函數sln_timer_do就會執行，並且該函數還要使用長整型參數sln_timer.dat。
void sln_timer_do(unsigned long l)；

`第四步`、激活定時器：

```c
add_timer(&sln_timer);    //向內核註冊定時器  
```

這樣定時器就可以運行了。
add_timer()的實現如下：

```c
void add_timer(struct timer_list *timer)  
{  
    BUG_ON(timer_pending(timer));  
    mod_timer(timer, timer->expires);  
}
```

add_timer()調用了mod_timer()。mod_timer()用於修改定時器超時時間。

mod_timer(&sln_timer, jiffies + HZ);

由於add_timer()是通過調用mod_timer()來激活定時器，所以也可以直接使用mod_timer()來激活定時器，如果定時器已經初始化但沒有激活，mod_timer()也會激活它。

如果需要在定時器超時前停止定時器，使用del_timer()函數來完成。

```c
del_timer(&sln_timer); 
```

該函數實現如下：

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

當使用del_timer()返回後，定時器就不會再被激活，但在多處理器機器上定時器上定時器中斷可能已經在其他處理器上運行了，所以刪除定時器時需要等待可能在其他處理器上運行的定時器處理I程序都退出，這時就要使用del_timer_sync()函數執行刪除工作：

```c
del_timer_sync(&sln_timer);
```

該函數不能再中斷上下文中使用。
該函數詳細實現如下：

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

    for (;;) {    //一直循環，直到刪除timer成功再退出
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

一般情況下應該使用del_timer_sync()函數代替del_timer()函數，因為無法確定在刪除定時器時，他是否在其他處理器上運行。為了防止這種情況的發生，應該調用del_timer_sync()函數而不是del_timer()函數。否則，對定時器執行刪除操作後，代碼會繼續執行，但它有可能會去操作在其它處理器上運行的定時器正在使用的資源，因而造成併發訪問，所有優先使用刪除定時器的同步方法。

除了使用定時器來推遲任務到指定時間段運行之外，還有其他的方法處理延時請求。有的方法會在延遲任務時掛起處理器，有的卻不會。實際上也沒有方法能夠保證實際的延遲時間剛好等於指定的延遲時間。

`1.` 最簡單的 延遲方法是忙等待，該方法實現起來很簡單，只需要在循環中不斷旋轉直到希望的時鐘節拍數耗盡。比如：

```c
unsigned long delay = jiffies+10;   //延遲10個節拍  
while(time_before(jiffies,delay))  
        ；  
```

這種方法當代碼等待時，處理器只能在原地旋轉等待，它不會去處理其他任何任務。最好在任務等待時，允許內核重新調度其它任務執行。將上面代碼修改如下：

```c
unsigned long delay = jiffies+10;   //10個節拍  
while(time_before(jiffies,delay))  
    cond_resched(); 
```

看一下cond_resched()函數具體實現代碼：

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
    schedule(); //最終還是調用schedule()函數來重新調度其它程序運行
    sub_preempt_count(PREEMPT_ACTIVE);
}
```

函數cond_resched()將重新調度一個新程序投入運行，但它只有在設置完need_resched標誌後才能生效。換句話說，就是系統中存在更重要的任務需要運行。再由於該方法需要調用調度程序，所以它不能在中斷上下文中使用----只能在進程上下文中使用。事實上，所有延遲方法在進程上下文中使用，因為中斷處理程序都應該儘可能快的執行。另外，延遲執行不管在哪種情況下都不應該在持有鎖時或者禁止中斷時發生。

`2.` 有時內核需要更短的延遲，甚至比節拍間隔還要短。這時可以使用內核提供的ms、ns、us級別的延遲函數。

```c
void udelay(unsigned long usecs);    //arch/x86/include/asm/delay.h  
void ndelay(unsigned long nsecs);    //arch/x86/include/asm/delay.h  
void mdelay(unsigned long msecs);      
```

udelay()使用忙循環將任務延遲指定的ms後執行,其依靠執行數次循環達到延遲效果，mdelay()函數是通過udelay()函數實現，如下：

```c
#define mdelay(n) (\   
    (__builtin_constant_p(n) && (n)<=MAX_UDELAY_MS) ? udelay((n)*1000) : \  
    ({unsigned long __ms=(n); while (__ms--) udelay(1000);}))  
#endif 
```

udelay()函數僅能在要求的延遲時間很短的情況下執行，而在高速機器中時間很長的延遲會造成溢出。對於較長的延遲，mdelay()工作良好。



`3.`  schedule_timeout()函數是更理想的延遲執行方法。該方法會讓需要延遲執行的任務睡眠到指定的延遲時間耗盡後再重新運行。但該方法也不能保證睡眠時間正好等於指定的延遲時間，只能儘量是睡眠時間接近指定的延遲時間。當指定的時間到期後，內核喚醒被延遲的任務並將其重新放回運行隊列。用法如下：

```c
set_current_state(TASK_INTERRUPTIBLE);    //將任務設置為可中斷睡眠狀態  
schedule_timeout(s*HZ);    //小睡一會兒，“s”秒後喚醒  
```

唯一的參數是延遲的相對時間，單位是jiffies，上例中將相應的任務推入可中斷睡眠隊列，睡眠s秒。在調用函數schedule_timeout之前，不要要將任務設置成可中斷或不和中斷的一種，否則任務不會休眠。這個函數需要調用調度程序，所以調用它的代碼必須保證能夠睡眠，簡而言之，調用代碼必須處於進程上下文中，並且不能持有鎖。

事實上schedule_timeout()函數的實現就是內核定時器的一個簡單應用。


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

    //下一行代碼設置了超時執行函數process_timeout()。
    setup_timer_on_stack(&timer, process_timeout, (unsigned long)current);
    __mod_timer(&timer, expire, false, TIMER_NOT_PINNED);    //激活定時器
    schedule();    //調度其他新任務
    del_singleshot_timer_sync(&timer);

    /* Remove the timer from the object tracker */
    destroy_timer_on_stack(&timer);

    timeout = expire - jiffies;

out:
    return timeout < 0 ? 0 : timeout;
}
```

當定時器超時時，process_timeout()函數被調用：


```c
static void process_timeout(unsigned long __data)   
{  
    wake_up_process((struct task_struct *)__data);  
}  
```

當任務被重新調度時，將返回代碼進入睡眠前的位置繼續執行，位置正好在schedule()處。

進程上下文的代碼為了等待特定時間發生，可以將自己放入等待隊列。但是，等待隊列上的某個任務可能既在等待一個特定事件到來，又在等待一個特定時間到期，就看誰來得更快。這種情況下，代碼可以簡單的使用scedule_timeout()函數代替schedule()函數，這樣一來，當希望指定時間到期後，任務都會被喚醒，當然，代碼需要檢查被喚醒的原因，有可能是被事件喚醒，也有可能是因為延遲的時間到期，還可能是因為接收到了信號，然後執行相應的操作。

本文源碼下載：
http://download.csdn.net/detail/gentleliu/8944281

