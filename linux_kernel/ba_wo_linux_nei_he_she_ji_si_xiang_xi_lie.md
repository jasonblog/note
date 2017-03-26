# 把握linux内核设计思想系列


操作系统负责管理硬件设备，为了使系统和硬件设备的协同工作不降低机器性能，系统和硬件的通信使用中断的机制，也就是让硬件在需要的时候向内核发出信号，这样使得内核不用去轮询设备而导致做很多无用功。

中断使得硬件可以发出通知给处理器，硬件设备生成中断的时候并不考虑与处理器的时钟同步，中断可以随时产生。也就是说，内核随时可能因为新到来的中断而被打断。当接收到一个中断后，中断控制器会给处理器发送一个电信号，处理器检测到该信号便中断自己当前工作而处理中断。

在响应一个中断时，内核会执行一个函数，该函数叫做中断处理程序或中断服务例程（ISR）。中断处理程序运行与中断上下文，中断上下文中执行的代码不可阻塞，应该快速执行，这样才能保证尽快恢复被中断的代码的执行。中断处理程序是管理硬件驱动的驱动程序的组成部分，如果设备使用中断，那么相应的驱动程序就注册一个中断处理程序。

在驱动程序中,通常使用request_irq()来注册中断处理程序。该函数在文件`<include/linux/interrupt.h>`中声明：


```c
extern int __must_check  
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,  
        const char *name, void *dev);  
```


第一个参数为要分配的中断号；第二个参数为指向中断处理程序的指针；第三个参数为中断处理标志。该函数实现如下：


```c
static inline int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
            const char* name, void* dev)
{
    return request_threaded_irq(irq, handler, NULL, flags, name, dev);
}

int request_threaded_irq(unsigned int irq, irq_handler_t handler,
                         irq_handler_t thread_fn, unsigned long irqflags,
                         const char* devname, void* dev_id)
{
    struct irqaction* action;
    struct irq_desc* desc;
    int retval;

    /*
     * handle_IRQ_event() always ignores IRQF_DISABLED except for
     * the _first_ irqaction (sigh).  That can cause oopsing, but
     * the behavior is classified as "will not fix" so we need to
     * start nudging drivers away from using that idiom.
     */
    if ((irqflags & (IRQF_SHARED | IRQF_DISABLED)) ==
        (IRQF_SHARED | IRQF_DISABLED)) {
        pr_warning(
            "IRQ %d/%s: IRQF_DISABLED is not guaranteed on shared IRQs\n",
            irq, devname);
    }

#ifdef CONFIG_LOCKDEP
    /*
     * Lockdep wants atomic interrupt handlers:
     */
    irqflags |= IRQF_DISABLED;
#endif

    /*
     * Sanity-check: shared interrupts must pass in a real dev-ID,
     * otherwise we'll have trouble later trying to figure out
     * which interrupt is which (messes up the interrupt freeing
     * logic etc).
     */
    if ((irqflags & IRQF_SHARED) && !dev_id) {
        return -EINVAL;
    }

    desc = irq_to_desc(irq);

    if (!desc) {
        return -EINVAL;
    }

    if (desc->status & IRQ_NOREQUEST) {
        return -EINVAL;
    }

    if (!handler) {
        if (!thread_fn) {
            return -EINVAL;
        }

        handler = irq_default_primary_handler;
    }

    //分配一个irqaction
    action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);

    if (!action) {
        return -ENOMEM;
    }

    action->handler = handler;
    action->thread_fn = thread_fn;
    action->flags = irqflags;
    action->name = devname;
    action->dev_id = dev_id;
    chip_bus_lock(irq, desc);

    //将创建并初始化完在的action加入desc
    retval = __setup_irq(irq, desc, action);
    chip_bus_sync_unlock(irq, desc);

    if (retval) {
        kfree(action);
    }

#ifdef CONFIG_DEBUG_SHIRQ

    if (irqflags & IRQF_SHARED) {
        /*
         * It's a shared IRQ -- the driver ought to be prepared for it
         * to happen immediately, so let's make sure....
         * We disable the irq to make sure that a 'real' IRQ doesn't
         * run in parallel with our fake.
         */
        unsigned long flags;
        disable_irq(irq);
        local_irq_save(flags);
        handler(irq, dev_id);
        local_irq_restore(flags);
        enable_irq(irq);
    }

#endif
    return retval;
}
```

下面看一下中断处理程序的实例，以rtc驱动程序为例，代码位于<drivers/char/rtc.c>中。当RTC驱动装载时，rtc_init()函数会被调用来初始化驱动程序，包括注册中断处理函数：


```c
/*
 * XXX Interrupt pin #7 in Espresso is shared between RTC and
 * PCI Slot 2 INTA# (and some INTx# in Slot 1).
 */
if (request_irq(rtc_irq, rtc_interrupt, IRQF_SHARED, "rtc",
                (void*)&rtc_port))
{
    rtc_has_irq = 0;
    printk(KERN_ERR "rtc: cannot register IRQ %d\n", rtc_irq);
    return -EIO;
}
```
处理程序函数rtc_interrupt()：

```c
/*
 *  A very tiny interrupt handler. It runs with IRQF_DISABLED set,
 *  but there is possibility of conflicting with the set_rtc_mmss()
 *  call (the rtc irq and the timer irq can easily run at the same
 *  time in two different CPUs). So we need to serialize
 *  accesses to the chip with the rtc_lock spinlock that each
 *  architecture should implement in the timer code.
 *  (See ./arch/XXXX/kernel/time.c for the set_rtc_mmss() function.)
 */
static irqreturn_t rtc_interrupt(int irq, void* dev_id)
{
    /*
     *  Can be an alarm interrupt, update complete interrupt,
     *  or a periodic interrupt. We store the status in the
     *  low byte and the number of interrupts received since
     *  the last read in the remainder of rtc_irq_data.
     */
    spin_lock(
        &rtc_lock);    //保证rtc_irq_data不被SMP机器上其他处理器同时访问
    rtc_irq_data += 0x100;
    rtc_irq_data &= ~0xff;

    if (is_hpet_enabled()) {
        /*
         * In this case it is HPET RTC interrupt handler
         * calling us, with the interrupt information
         * passed as arg1, instead of irq.
         */
        rtc_irq_data |= (unsigned long)irq & 0xF0;
    } else {
        rtc_irq_data |= (CMOS_READ(RTC_INTR_FLAGS) & 0xF0);
    }

    if (rtc_status & RTC_TIMER_ON) {
        mod_timer(&rtc_irq_timer, jiffies + HZ / rtc_freq + 2 * HZ / 100);
    }

    spin_unlock(&rtc_lock);
    /* Now do the rest of the actions */
    spin_lock(
        &rtc_task_lock);    //避免rtc_callback出现系统情况，RTC驱动允许注册一个回调函数在每个RTC中断到来时执行。

    if (rtc_callback) {
        rtc_callback->func(rtc_callback->private_data);
    }

    spin_unlock(&rtc_task_lock);
    wake_up_interruptible(&rtc_wait);
    kill_fasync(&rtc_async_queue, SIGIO, POLL_IN);
    return IRQ_HANDLED;
}
```

在内核中，中断的旅程开始于预定义入口点，这类似于系统调用。对于每条中断线，处理器都会跳到对应的一个唯一的位置。这样，内核就可以知道所接收中断的IRQ号了。初始入口点只是在栈中保存这个号，并存放当前寄存器的值(这些值属于被中断的任务)；然后，内核调用函数do_IRQ().从这里开始，大多数中断处理代码是用C写的。do_IRQ()的声明如下：

unsigned int do_IRQ(struct pt_regs regs)

因为C的调用惯例是要把函数参数放在栈的顶部，因此pt_regs结构包含原始寄存器的值，这些值是以前在汇编入口例程中保存在栈上的。中断的值也会得以保存，所以，do_IRQ()可以将它提取出来，X86的代码为：
```c
int irq = regs.orig_eax & 0xff
```

计算出中断号后，do_IRQ()对所接收的中断进行应答，禁止这条线上的中断传递。在普通的PC机器上，这些操作是由mask_and_ack_8259A()来完成的，该函数由do_IRQ()调用。接下来，do_IRQ()需要确保在这条中断线上有一个有效的处理程序，而且这个程序已经启动但是当前没有执行。如果这样的话， do_IRQ()就调用handle_IRQ_event()来运行为这条中断线所安装的中断处理程序，函数位于`<kernel/irq/handle.c>`:


```c
/**
 * handle_IRQ_event - irq action chain handler
 * @irq:    the interrupt number
 * @action: the interrupt action chain for this irq
 *
 * Handles the action chain of an irq event
 */
irqreturn_t handle_IRQ_event(unsigned int irq, struct irqaction* action)
{
    irqreturn_t ret, retval = IRQ_NONE;
    unsigned int status = 0;

    //如果没有设置IRQF_DISABLED，将CPU中断打开，应该尽量避免中断关闭情况，本地中断关闭情况下会导致中断丢失。
    if (!(action->flags & IRQF_DISABLED)) {
        local_irq_enable_in_hardirq();
    }

    do {    //遍历运行中断处理程序
        trace_irq_handler_entry(irq, action);
        ret = action->handler(irq, action->dev_id);
        trace_irq_handler_exit(irq, action, ret);

        switch (ret) {
        case IRQ_WAKE_THREAD:
            /*
             * Set result to handled so the spurious check
             * does not trigger.
             */
            ret = IRQ_HANDLED;

            /*
             * Catch drivers which return WAKE_THREAD but
             * did not set up a thread function
             */
            if (unlikely(!action->thread_fn)) {
                warn_no_thread(irq, action);
                break;
            }

            /*
             * Wake up the handler thread for this
             * action. In case the thread crashed and was
             * killed we just pretend that we handled the
             * interrupt. The hardirq handler above has
             * disabled the device interrupt, so no irq
             * storm is lurking.
             */
            if (likely(!test_bit(IRQTF_DIED,
                                 &action->thread_flags))) {
                set_bit(IRQTF_RUNTHREAD, &action->thread_flags);
                wake_up_process(action->thread);
            }

        /* Fall through to add to randomness */
        case IRQ_HANDLED:
            status |= action->flags;
            break;

        default:
            break;
        }

        retval |= ret;
        action = action->next;
    } while (action);

    if (status & IRQF_SAMPLE_RANDOM) {
        add_interrupt_randomness(irq);
    }

    local_irq_disable();//关中断

    return retval;
}
```

前面说到中断应该尽快执行完，以保证被中断代码可以尽快的恢复执行。但事实上中断通常有很多工作要做，包括应答、重设硬件、数据拷贝、处理请求、发送请求等。为了求得平衡，内核把中断处理工作分成两半，中断处理程序是上半部——接收到中断就开始执行。能够稍后完成的工作推迟到下半部操作，下半部在合适的时机被开中段执行。例如网卡收到数据包时立即发出中断，内核执行网卡已注册的中断处理程序，此处工作就是通知硬件拷贝最新的网络数据包到内存，然后将控制权交换给系统之前被中断的任务，其他的如处理和操作数据包等任务被放到随后的下半部中去执行。下一节我们将了解中断处理的下半部。

