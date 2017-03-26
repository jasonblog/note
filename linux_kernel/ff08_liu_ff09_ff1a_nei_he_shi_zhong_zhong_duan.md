# （六）：内核时钟中断


内核中很多函数是基于时间驱动的，其中有些函数需要周期或定期执行。比如有的每秒执行100次，有的在等待一个相对时间之后执行。除此之外，内核还必须管理系统运行的时间日期。 

周期性产生的时间都是有系统定时器驱动的，系统定时器是一种可编程硬件芯片，它可以以固定频率产生中断，该中断就是所谓的定时器中断，其所对应的中断处理程序负责更新系统时间，也负责执行需要周期性运行的任务。

系统定时器以某种频率自行触发时钟中断，该频率可以通过编程预定，称作节拍率。当时钟中断发生时，内核就通过一种特殊的中断处理器对其进行处理。内核知道连续两次时钟中断的间隔时间，该间隔时间就称为节拍。内核就是靠这种已知的时钟中断间隔来计算实际时间和系统运行时间的。内核通过控制时钟中断维护实际时间，另外内核也为用户提供一组系统调用获取实际日期和实际时间。时钟中断对才操作系统的管理来说十分重要，系统更新运行时间、更新实际时间、均衡调度程序中个处理器上运行队列、检查进程是否用尽时间片等工作都利用时钟中断来周期执行。

系统定时器频率是通过静态预定义的，也就是HZ，体系结构不同，HZ的值也不同。内核在<asm/param.h>文件中定义，在x86上时钟中断频率为100HZ，也就是说在i386处理上每秒时钟中断100次。

linux内核众多子系统都依赖时钟中断工作，所以是时钟中断频率的选择必须考虑频率所有子系统的影响。提高节拍就使得时钟中断产生的更频繁，中断处理程序就会更加频繁的执行，这样就提高了时间驱动时间的准确度，误差更小。如HZ=100，那么时钟每10ms中断一次，周期事件每10ms运行一次，如果HZ=1000，那么周期事件每1ms就会运行一次，这样依赖定时器的系统调用能够以更高的精度运行。既然提高时钟中断频率这么好，那为何要将HZ设置为100呢？因为提高时钟中断频率也会产生副作用，中断频率越高，系统的负担就增加了，处理器需要花时间来执行中断处理程序，中断处理器占用cpu时间越多。这样处理器执行其他工作的时间及越少，并且还会打乱处理器高速缓存。所以选择时钟中断频率时要考虑多方面，要取得各方面的折中的一个合适频率。

内核有一个全局变量jiffies，该变量用来记录系统起来以后产生的节拍总数。系统启动是，该变量被设置为0，此后每产生一次时钟中断就增加该变量的值。jiffies每一秒增加的值就是HZ。jiffies定义于头文件`<include/linux/jiffies.h>`中：



```c
extern unsigned long volatile __jiffy_data jiffies;  
```

对于32位unsigned long，可以存放最大值为4294967295，所以当节拍数达到最大值后还要继续增加的话，它的值就会回到0值。内核提供了四个宏（位于文件`<include/linux/jiffies.h>`中）来比较节拍数，这些宏可以正确处理节拍计数回绕情况。


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

下面示例来打印出当前系统启动后经过的jiffies以及秒数：

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

执行输出结果为：

```sh
# insmod jfs.ko  
===jiffies_init===  
Current ticks is: 10106703, seconds: 10106  
```

时钟中断发生时，会触发时钟中断处理程序，始终中断处理程序部分和体系结构相关，下面简单分析一下x86体系的处理：
时钟的初始化在time_init()中，在start_kernel()中调用time_init()，如下：

```c
asmlinkage void __init start_kernel(void)  
{  
    ......  
    time_init();  
    ......  
}
```

下面分析一下time_init()的实现，该函数位于文件`<arch/x86/kernel/time.c>`中：

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

结构体x86_init位于arch/x86/kernel/x86_init.c中


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
默认timer初始化函数为：

```c
void __init hpet_time_init(void)                                                                                          
{  
    if (!hpet_enable())  
        setup_pit_timer();  
    setup_default_timer_irq();  
}
```
函数setup_default_timer_irq();注册中断处理函数：

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

对应的中断处理函数为:timer_interrupt():

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

    //在此处调用体系无关的时钟处理例程
    global_clock_event->event_handler(global_clock_event);

    /* MCA bus quirk: Acknowledge irq0 by setting bit 7 in port 0x61 */
    if (MCA_bus) {
        outb_p(inb_p(0x61) | 0x80, 0x61);
    }

    return IRQ_HANDLED;
}
```

时钟例程在系统启动时start_kernel()函数中调用tick_init()初始化：
（位于文件kernel/time/tick-common.c）

```c
void __init tick_init(void)  
{  
    clockevents_register_notifier(&tick_notifier);  
}  
```

tick_notifier定义如下：

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
 * 根据broadcast设置周期性的处理函数（kernel/time/tick-broadcast.c）,这里就设置了始终中断函数timer_interrupt中调用的时钟处理例程
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
 * ，以tick_handle_periodic为例，每一个始终节拍都调用该处理函数，而该处理过程中，主要处理工作处于tick_periodic()函数中。
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
tick_periodic()函数主要有以下工作：
下面来看分析一下该函数：

```c
/* 
 * Periodic tick 
 */  
static void tick_periodic(int cpu)  
{  
    if (tick_do_timer_cpu == cpu) {  
        write_seqlock(&xtime_lock);  
  
        /* 记录下一个节拍事件 */  
        tick_next_period = ktime_add(tick_next_period, tick_period);  
  
        do_timer(1);  
        write_sequnlock(&xtime_lock);  
    }  
  
    update_process_times(user_mode(get_irq_regs()));//更新所耗费的各种节拍数  
    profile_tick(CPU_PROFILING);  
}  
```

其中函数do_timer()(位于kernel/timer.c中)对jiffies_64做增加操作：


```c
void do_timer(unsigned long ticks)  
{  
    jiffies_64 += ticks;  
    update_wall_time();    //更新墙上时钟  
    calc_global_load();    //更新系统平均负载统计值  
}  
```

update_process_times更新所耗费的各种节拍数。


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
函数run_local_timers()会标记一个软中断去处理所有到期的定时器。

```c
void run_local_timers(void)  
{  
    hrtimer_run_queues();  
    raise_softirq(TIMER_SOFTIRQ);  
    softlockup_tick();  
}  
```

在时钟中断处理函数time_interrupt()函数调用体系结构无关的时钟处理例程完成之后，返回到与体系结构的相关的中断处理函数中。以上所有的工作每一次时钟中断都会运行，也就是说如果HZ=100，那么时钟中断处理程序每一秒就会运行100次。

本节源码下载：
http://download.csdn.net/detail/gentleliu/8944199





