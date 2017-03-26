# （三）：下半部機制之軟中斷


中斷處理程序以異步方式執行，其會打斷其他重要代碼，其運行時該中斷同級的其他中斷會被屏蔽，並且當前處理器上所有其他中斷都有可能會被屏蔽掉，還有中斷處理程序不能阻塞，所以中斷處理需要儘快結束。由於中斷處理程序的這些缺陷，導致了中斷處理程序只是整個硬件中斷處理流程的一部分，對於那些對時間要求不高的任務，留給中斷處理流程的另外一部分，也就是本節要講的中斷處理流程的下半部。

那哪些工作由中斷處理程序完成，哪些工作留給下半部來執行呢？其實上半部和下半部的工作劃分不存在某種嚴格限制，這主要取決於驅動程序開發者自己的判斷，一般最好能將中斷處理程序執行時間縮短到最小。中斷處理程序幾乎都需要通過操作硬件對中斷的到達進行確認，有時還會做對時間非常敏感的工作（如拷貝數據），其餘的工作基本上留給下半部來處理，下半部就是執行與中斷處理密切相關但中斷處理程序本身不執行的工作。一般對時間非常敏感、和硬件相關、要保證不被其它中斷(特別是相同的中斷)打斷的這些任務放在中斷處理程序中執行，其他任務考慮放在下半部執行。

那下半部什麼時候執行呢？下半部不需要指定明確執行時間，只要把任務推遲一點，讓它們在系統不太忙且中斷恢復後執行就可以了，而且執行期間可以相應所有中斷。

上半部只能通過中斷處理程序實現，而下半部可以有多種機制來實現，在2.6.32版本中，有三種不同形式的下半部實現機制：軟中斷、tasklet、工作隊列。下面來看一下這三種下半部的實現。

##軟中斷
在start_kernerl()函數中，系統初始化軟中斷。


```c
asmlinkage void __init start_kernel(void)  
{  
    char * command_line;  
    extern struct kernel_param __start___param[], __stop___param[];  
  
    smp_setup_processor_id();  
......  
    softirq_init();//初始化軟中斷  
......  
  
    /* Do the rest non-__init'ed, we're now alive */  
    rest_init();  
}
```

在softirq_init()中會註冊兩個常用類型的軟中斷, 具體代碼如下（位於kernel/softirq.c）:

```c
void __init softirq_init(void)  
{  
    int cpu;  
  
    for_each_possible_cpu(cpu) {  
        int i;  
  
        per_cpu(tasklet_vec, cpu).tail =  
            &per_cpu(tasklet_vec, cpu).head;  
        per_cpu(tasklet_hi_vec, cpu).tail =  
            &per_cpu(tasklet_hi_vec, cpu).head;  
        for (i = 0; i < NR_SOFTIRQS; i++)  
            INIT_LIST_HEAD(&per_cpu(softirq_work_list[i], cpu));  
    }  
  
    register_hotcpu_notifier(&remote_softirq_cpu_notifier);  
  
    //此處註冊兩個軟中斷  
    open_softirq(TASKLET_SOFTIRQ, tasklet_action);     open_softirq(HI_SOFTIRQ, tasklet_hi_action);  
}  
```

註冊函數open_softirq()參數含義:
nr:軟中斷類型 action:軟中斷處理函數


```c
void open_softirq(int nr, void (*action)(struct softirq_action *))  
{  
    softirq_vec[nr].action = action;  
}  
```

softirq_action結構表示軟中斷，定義在`<include/linux/interrupt.h>`


```c
struct softirq_action  
{  
    void    (*action)(struct softirq_action *);  
}  
```

文件`<kernel/softirq.c>`中定義了32個該結構體的數組：

```c
static struct softirq_action softirq_vec[NR_SOFTIRQS] __cacheline_aligned_in_smp;  
```

每註冊一個軟中斷都會佔該數組一個位置，因此係統中最多有32個軟中斷。從上面的代碼中,我們可以看到:open_softirq()中.其實就是對softirq_vec數組的nr項賦值.softirq_vec是一個32元素的數組,實際上linux內核只使用了幾項：

```c
/* PLEASE, avoid to allocate new softirqs, if you need not _really_ high 
   frequency threaded job scheduling. For almost all the purposes 
   tasklets are more than enough. F.e. all serial device BHs et 
   al. should be converted to tasklets, not to softirqs. 
 */  
  
enum  
{  
    HI_SOFTIRQ=0,  
    TIMER_SOFTIRQ,  
    NET_TX_SOFTIRQ,  
    NET_RX_SOFTIRQ,  
    BLOCK_SOFTIRQ,  
    BLOCK_IOPOLL_SOFTIRQ,  
    TASKLET_SOFTIRQ,  
    SCHED_SOFTIRQ,  
    HRTIMER_SOFTIRQ,  
    RCU_SOFTIRQ,    /* Preferable RCU should always be the last softirq */  
  
    NR_SOFTIRQS  
};  
```

那麼軟中斷註冊完成之後，什麼時候觸發軟中斷處理函數執行呢？通常情況下，軟中斷會在中斷處理程序返回前標記它，使其在稍後合適的時候被執行。在下列地方，待處理的軟中斷會被檢查和執行：
###1.處理完一個硬件中斷以後；
###2.在ksoftirqd內核線程中；
###3.在那些顯示檢查和執行待處理的軟中斷的代碼中，如網絡子系統中。

有關網絡子系統內容可參考文章：http://blog.csdn.net/shallnet/article/details/26269781

無論如何，軟中斷會在do_softirq()（位於`<kernel/softirq.c>`中）中執行，如果有待處理的軟中斷，do_softirq會循環遍歷每一個，調用他們的軟中斷處理程序。


```c
asmlinkage void do_softirq(void) {     __u32 pending;     unsigned long flags;     //如果在硬件中斷環境中就退出，軟中斷不可以在硬件中斷上下文或者是在軟中斷環境中使用，使用in_interrupt()來防止軟中斷嵌套，和搶佔硬中斷環境。     if (in_interrupt())         return;     //禁止本地中斷     local_irq_save(flags);     pending = local_softirq_pending();  
    //如果有軟中斷要處理，則進入__do_softirq()  
    if (pending) __do_softirq();     
    local_irq_restore(flags);    
```

下面看一下__do_softirq()的實現：


```c
asmlinkage void __do_softirq(void)
{
    struct softirq_action* h;
    __u32 pending;
    int max_restart = MAX_SOFTIRQ_RESTART;
    int cpu;

    pending = local_softirq_pending();    //pending用於保留待處理軟中斷32位位圖
    account_system_vtime(current);

    __local_bh_disable((unsigned long)__builtin_return_address(0));
    lockdep_softirq_enter();

    cpu = smp_processor_id();
restart:
    /* Reset the pending bitmask before enabling irqs */
    set_softirq_pending(0);

    local_irq_enable();

    h = softirq_vec;

    do {
        if (pending &
            1) {    //如果pending第n位被設置為1，那麼處理第n位對應類型的軟中斷
            int prev_count = preempt_count();
            kstat_incr_softirqs_this_cpu(h - softirq_vec);

            trace_softirq_entry(h, softirq_vec);
            h->action(h);    //執行軟中斷處理函數
            trace_softirq_exit(h, softirq_vec);

            if (unlikely(prev_count != preempt_count())) {
                printk(KERN_ERR "huh, entered softirq %td %s %p"
                       "with preempt_count %08x,"
                       " exited with %08x?\n", h - softirq_vec,
                       softirq_to_name[h - softirq_vec],
                       h->action, prev_count, preempt_count());
                preempt_count() = prev_count;
            }

            rcu_bh_qs(cpu);
        }

        h++;
        pending >>= 1;    //pending右移一位，循環檢查其每一位
    } while (pending);    //直到pending變為0，pending最多32位，所以循環最多執行32次。

    local_irq_disable();

    pending = local_softirq_pending();

    if (pending && --max_restart) {
        goto restart;
    }

    if (pending) {
        wakeup_softirqd();
    }

    lockdep_softirq_exit();

    account_system_vtime(current);
    _local_bh_enable();
}
```

使用軟中斷必須要在編譯期間靜態註冊，一般只有像網絡這樣對性能要求高的情況才使用軟中斷，文章前面我們也看到，系統中註冊的軟中斷就那麼幾個。大部分時候，使用下半部另外一種機制tasklet的情況更多一些，tasklet可以動態的註冊，可以被看作是一種性能和易用性之間尋求平衡的一種產物。事實上，大部分驅動程序都是用tasklet來實現他們的下半部。