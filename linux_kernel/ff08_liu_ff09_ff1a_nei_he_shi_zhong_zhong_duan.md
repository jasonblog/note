# （六）：內核時鐘中斷


內核中很多函數是基於時間驅動的，其中有些函數需要週期或定期執行。比如有的每秒執行100次，有的在等待一個相對時間之後執行。除此之外，內核還必須管理系統運行的時間日期。 

週期性產生的時間都是有系統定時器驅動的，系統定時器是一種可編程硬件芯片，它可以以固定頻率產生中斷，該中斷就是所謂的定時器中斷，其所對應的中斷處理程序負責更新系統時間，也負責執行需要週期性運行的任務。

系統定時器以某種頻率自行觸發時鐘中斷，該頻率可以通過編程預定，稱作節拍率。當時鍾中斷髮生時，內核就通過一種特殊的中斷處理器對其進行處理。內核知道連續兩次時鐘中斷的間隔時間，該間隔時間就稱為節拍。內核就是靠這種已知的時鐘中斷間隔來計算實際時間和系統運行時間的。內核通過控制時鐘中斷維護實際時間，另外內核也為用戶提供一組系統調用獲取實際日期和實際時間。時鐘中斷對才操作系統的管理來說十分重要，系統更新運行時間、更新實際時間、均衡調度程序中個處理器上運行隊列、檢查進程是否用盡時間片等工作都利用時鐘中斷來週期執行。

系統定時器頻率是通過靜態預定義的，也就是HZ，體系結構不同，HZ的值也不同。內核在<asm/param.h>文件中定義，在x86上時鐘中斷頻率為100HZ，也就是說在i386處理上每秒時鐘中斷100次。

linux內核眾多子系統都依賴時鐘中斷工作，所以是時鐘中斷頻率的選擇必須考慮頻率所有子系統的影響。提高節拍就使得時鐘中斷產生的更頻繁，中斷處理程序就會更加頻繁的執行，這樣就提高了時間驅動時間的準確度，誤差更小。如HZ=100，那麼時鐘每10ms中斷一次，週期事件每10ms運行一次，如果HZ=1000，那麼週期事件每1ms就會運行一次，這樣依賴定時器的系統調用能夠以更高的精度運行。既然提高時鐘中斷頻率這麼好，那為何要將HZ設置為100呢？因為提高時鐘中斷頻率也會產生副作用，中斷頻率越高，系統的負擔就增加了，處理器需要花時間來執行中斷處理程序，中斷處理器佔用cpu時間越多。這樣處理器執行其他工作的時間及越少，並且還會打亂處理器高速緩存。所以選擇時鐘中斷頻率時要考慮多方面，要取得各方面的折中的一個合適頻率。

內核有一個全局變量jiffies，該變量用來記錄系統起來以後產生的節拍總數。系統啟動是，該變量被設置為0，此後每產生一次時鐘中斷就增加該變量的值。jiffies每一秒增加的值就是HZ。jiffies定義於頭文件`<include/linux/jiffies.h>`中：



```c
extern unsigned long volatile __jiffy_data jiffies;  
```

對於32位unsigned long，可以存放最大值為4294967295，所以當節拍數達到最大值後還要繼續增加的話，它的值就會回到0值。內核提供了四個宏（位於文件`<include/linux/jiffies.h>`中）來比較節拍數，這些宏可以正確處理節拍計數迴繞情況。


```c
#define time_after(a,b)         \  
        (typecheck(unsigned long, a) && \  
         typecheck(unsigned long, b) && \  
         ((long)(b) - (long)(a) < 0))  
#define time_before(a,b)        time_after(b,a)  
#define time_after_eq(a,b)      \  
        (typecheck(unsigned long, a) && \  
         typecheck(unsigned long, b) && \  
         ((long)(a) - (long)(b) >= 0))  
#define time_before_eq(a,b)     time_after_eq(b,a)   

```

下面示例來打印出當前系統啟動後經過的jiffies以及秒數：

```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h> //jiffies  
#include <asm/param.h> //HZ  
static int __init jiffies_init(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);
    printk(KERN_ALERT"Current ticks is: %lu, seconds: %lu\n", jiffies,
           jiffies / HZ);
    return 0;
}
static void __exit jiffies_exit(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);
}
module_init(jiffies_init);
module_exit(jiffies_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

執行輸出結果為：

```sh
# insmod jfs.ko  
===jiffies_init===  
Current ticks is: 10106703, seconds: 10106  
```

時鐘中斷髮生時，會觸發時鐘中斷處理程序，始終中斷處理程序部分和體系結構相關，下面簡單分析一下x86體系的處理：
時鐘的初始化在time_init()中，在start_kernel()中調用time_init()，如下：

```c
asmlinkage void __init start_kernel(void)  
{  
    ......  
    time_init();  
    ......  
}
```

下面分析一下time_init()的實現，該函數位於文件`<arch/x86/kernel/time.c>`中：

```c
void __init time_init(void)                                                                                                
{  
    late_time_init = x86_late_time_init;  
}  
  
static __init void x86_late_time_init(void)  
{  
    x86_init.timers.timer_init();        //  
    tsc_init();  
}  
```

結構體x86_init位於arch/x86/kernel/x86_init.c中


```c
struct x86_init_ops x86_init __initdata = {   
......  
    .timers = {  
        .setup_percpu_clockev>--= setup_boot_APIC_clock,  
        .tsc_pre_init       = x86_init_noop,  
        .timer_init     = hpet_time_init,  
    }  
} 
```
默認timer初始化函數為：

```c
void __init hpet_time_init(void)                                                                                          
{  
    if (!hpet_enable())  
        setup_pit_timer();  
    setup_default_timer_irq();  
}
```
函數setup_default_timer_irq();註冊中斷處理函數：

```c
void __init setup_default_timer_irq(void)                                                                                  
{  
    setup_irq(0, &irq0);  
}  
  
static struct irqaction irq0  = {  
    .handler = timer_interrupt,  
    .flags = IRQF_DISABLED | IRQF_NOBALANCING | IRQF_IRQPOLL | IRQF_TIMER,  
    .name = "timer"  
};
```

對應的中斷處理函數為:timer_interrupt():

```c
static irqreturn_t timer_interrupt(int irq, void* dev_id)
{
    /* Keep nmi watchdog up to date */
    inc_irq_stat(irq0_irqs);

    /* Optimized out for !IO_APIC and x86_64 */
    if (timer_ack) {
        /*
         * Subtle, when I/O APICs are used we have to ack timer IRQ
         * manually to deassert NMI lines for the watchdog if run
         * on an 82489DX-based system.
         */
        spin_lock(&i8259A_lock);
        outb(0x0c, PIC_MASTER_OCW3);
        /* Ack the IRQ; AEOI will end it automatically. */
        inb(PIC_MASTER_POLL);
        spin_unlock(&i8259A_lock);
    }

    //在此處調用體系無關的時鐘處理例程
    global_clock_event->event_handler(global_clock_event);

    /* MCA bus quirk: Acknowledge irq0 by setting bit 7 in port 0x61 */
    if (MCA_bus) {
        outb_p(inb_p(0x61) | 0x80, 0x61);
    }

    return IRQ_HANDLED;
}
```

時鐘例程在系統啟動時start_kernel()函數中調用tick_init()初始化：
（位於文件kernel/time/tick-common.c）

```c
void __init tick_init(void)  
{  
    clockevents_register_notifier(&tick_notifier);  
}  
```

tick_notifier定義如下：

```c
static struct notifier_block tick_notifier = {
    .notifier_call = tick_notify,
};

static int tick_notify(struct notifier_block* nb, unsigned long reason,
                       void* dev)
{
    switch (reason) {

        ......

    case CLOCK_EVT_NOTIFY_RESUME:
        tick_resume();
        break;

    default:
        break;
    }

    return NOTIFY_OK;
}

static void tick_resume(void)
{
    struct tick_device* td = &__get_cpu_var(tick_cpu_device);
    unsigned long flags;
    int broadcast = tick_resume_broadcast();

    spin_lock_irqsave(&tick_device_lock, flags);
    clockevents_set_mode(td->evtdev, CLOCK_EVT_MODE_RESUME);

    if (!broadcast) {
        if (td->mode == TICKDEV_MODE_PERIODIC) {
            tick_setup_periodic(td->evtdev, 0);
        } else {
            tick_resume_oneshot();
        }
    }

    spin_unlock_irqrestore(&tick_device_lock, flags);
}

/*
 * Setup the device for a periodic tick
 */
void tick_setup_periodic(struct clock_event_device* dev, int broadcast)
{
    tick_set_periodic_handler(dev, broadcast);

    ......
}

/*
 * 根據broadcast設置週期性的處理函數（kernel/time/tick-broadcast.c）,這裡就設置了始終中斷函數timer_interrupt中調用的時鐘處理例程
 */
void tick_set_periodic_handler(struct clock_event_device* dev, int broadcast)
{
    if (!broadcast) {
        dev->event_handler = tick_handle_periodic;
    } else {
        dev->event_handler = tick_handle_periodic_broadcast;
    }
}

/*
 * ，以tick_handle_periodic為例，每一個始終節拍都調用該處理函數，而該處理過程中，主要處理工作處於tick_periodic()函數中。
 */
void tick_handle_periodic(struct clock_event_device* dev)
{
    int cpu = smp_processor_id();
    ktime_t next;

    tick_periodic(cpu);

    if (dev->mode != CLOCK_EVT_MODE_ONESHOT) {
        return;
    }

    next = ktime_add(dev->next_event, tick_period);

    for (;;) {
        if (!clockevents_program_event(dev, next, ktime_get())) {
            return;
        }

        if (timekeeping_valid_for_hres()) {
            tick_periodic(cpu);
        }

        next = ktime_add(next, tick_period);
    }
}
```
tick_periodic()函數主要有以下工作：
下面來看分析一下該函數：

```c
/* 
 * Periodic tick 
 */  
static void tick_periodic(int cpu)  
{  
    if (tick_do_timer_cpu == cpu) {  
        write_seqlock(&xtime_lock);  
  
        /* 記錄下一個節拍事件 */  
        tick_next_period = ktime_add(tick_next_period, tick_period);  
  
        do_timer(1);  
        write_sequnlock(&xtime_lock);  
    }  
  
    update_process_times(user_mode(get_irq_regs()));//更新所耗費的各種節拍數  
    profile_tick(CPU_PROFILING);  
}  
```

其中函數do_timer()(位於kernel/timer.c中)對jiffies_64做增加操作：


```c
void do_timer(unsigned long ticks)  
{  
    jiffies_64 += ticks;  
    update_wall_time();    //更新牆上時鐘  
    calc_global_load();    //更新系統平均負載統計值  
}  
```

update_process_times更新所耗費的各種節拍數。


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
```
函數run_local_timers()會標記一個軟中斷去處理所有到期的定時器。

```c
void run_local_timers(void)  
{  
    hrtimer_run_queues();  
    raise_softirq(TIMER_SOFTIRQ);  
    softlockup_tick();  
}  
```

在時鐘中斷處理函數time_interrupt()函數調用體系結構無關的時鐘處理例程完成之後，返回到與體系結構的相關的中斷處理函數中。以上所有的工作每一次時鐘中斷都會運行，也就是說如果HZ=100，那麼時鐘中斷處理程序每一秒就會運行100次。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/8944199





