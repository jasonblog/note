# （二）：硬中斷及中斷處理



操作系統負責管理硬件設備，為了使系統和硬件設備的協同工作不降低機器性能，系統和硬件的通信使用中斷的機制，也就是讓硬件在需要的時候向內核發出信號，這樣使得內核不用去輪詢設備而導致做很多無用功。

中斷使得硬件可以發出通知給處理器，硬件設備生成中斷的時候並不考慮與處理器的時鐘同步，中斷可以隨時產生。也就是說，內核隨時可能因為新到來的中斷而被打斷。當接收到一箇中斷後，中斷控制器會給處理器發送一個電信號，處理器檢測到該信號便中斷自己當前工作而處理中斷。

在響應一箇中斷時，內核會執行一個函數，該函數叫做中斷處理程序或中斷服務例程（ISR）。中斷處理程序運行與中斷上下文，中斷上下文中執行的代碼不可阻塞，應該快速執行，這樣才能保證儘快恢復被中斷的代碼的執行。中斷處理程序是管理硬件驅動的驅動程序的組成部分，如果設備使用中斷，那麼相應的驅動程序就註冊一箇中斷處理程序。

在驅動程序中,通常使用request_irq()來註冊中斷處理程序。該函數在文件`<include/linux/interrupt.h>`中聲明：


```c
extern int __must_check  
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,  
        const char *name, void *dev);  
```


第一個參數為要分配的中斷號；第二個參數為指向中斷處理程序的指針；第三個參數為中斷處理標誌。該函數實現如下：


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

    //分配一個irqaction
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

    //將創建並初始化完在的action加入desc
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

下面看一下中斷處理程序的實例，以rtc驅動程序為例，代碼位於<drivers/char/rtc.c>中。當RTC驅動裝載時，rtc_init()函數會被調用來初始化驅動程序，包括註冊中斷處理函數：


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
處理程序函數rtc_interrupt()：

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
        &rtc_lock);    //保證rtc_irq_data不被SMP機器上其他處理器同時訪問
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
        &rtc_task_lock);    //避免rtc_callback出現系統情況，RTC驅動允許註冊一個回調函數在每個RTC中斷到來時執行。

    if (rtc_callback) {
        rtc_callback->func(rtc_callback->private_data);
    }

    spin_unlock(&rtc_task_lock);
    wake_up_interruptible(&rtc_wait);
    kill_fasync(&rtc_async_queue, SIGIO, POLL_IN);
    return IRQ_HANDLED;
}
```

在內核中，中斷的旅程開始於預定義入口點，這類似於系統調用。對於每條中斷線，處理器都會跳到對應的一個唯一的位置。這樣，內核就可以知道所接收中斷的IRQ號了。初始入口點只是在棧中保存這個號，並存放當前寄存器的值(這些值屬於被中斷的任務)；然後，內核調用函數do_IRQ().從這裡開始，大多數中斷處理代碼是用C寫的。do_IRQ()的聲明如下：

unsigned int do_IRQ(struct pt_regs regs)

因為C的調用慣例是要把函數參數放在棧的頂部，因此pt_regs結構包含原始寄存器的值，這些值是以前在彙編入口例程中保存在棧上的。中斷的值也會得以保存，所以，do_IRQ()可以將它提取出來，X86的代碼為：
```c
int irq = regs.orig_eax & 0xff
```

計算出中斷號後，do_IRQ()對所接收的中斷進行應答，禁止這條線上的中斷傳遞。在普通的PC機器上，這些操作是由mask_and_ack_8259A()來完成的，該函數由do_IRQ()調用。接下來，do_IRQ()需要確保在這條中斷線上有一個有效的處理程序，而且這個程序已經啟動但是當前沒有執行。如果這樣的話， do_IRQ()就調用handle_IRQ_event()來運行為這條中斷線所安裝的中斷處理程序，函數位於`<kernel/irq/handle.c>`:


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

    //如果沒有設置IRQF_DISABLED，將CPU中斷打開，應該儘量避免中斷關閉情況，本地中斷關閉情況下會導致中斷丟失。
    if (!(action->flags & IRQF_DISABLED)) {
        local_irq_enable_in_hardirq();
    }

    do {    //遍歷運行中斷處理程序
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

    local_irq_disable();//關中斷

    return retval;
}
```

前面說到中斷應該儘快執行完，以保證被中斷代碼可以儘快的恢復執行。但事實上中斷通常有很多工作要做，包括應答、重設硬件、數據拷貝、處理請求、發送請求等。為了求得平衡，內核把中斷處理工作分成兩半，中斷處理程序是上半部——接收到中斷就開始執行。能夠稍後完成的工作推遲到下半部操作，下半部在合適的時機被開中段執行。例如網卡收到數據包時立即發出中斷，內核執行網卡已註冊的中斷處理程序，此處工作就是通知硬件拷貝最新的網絡數據包到內存，然後將控制權交換給系統之前被中斷的任務，其他的如處理和操作數據包等任務被放到隨後的下半部中去執行。下一節我們將瞭解中斷處理的下半部。