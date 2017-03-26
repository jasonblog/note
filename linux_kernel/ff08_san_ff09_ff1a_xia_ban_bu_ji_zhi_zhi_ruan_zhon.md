# （三）：下半部机制之软中断


中断处理程序以异步方式执行，其会打断其他重要代码，其运行时该中断同级的其他中断会被屏蔽，并且当前处理器上所有其他中断都有可能会被屏蔽掉，还有中断处理程序不能阻塞，所以中断处理需要尽快结束。由于中断处理程序的这些缺陷，导致了中断处理程序只是整个硬件中断处理流程的一部分，对于那些对时间要求不高的任务，留给中断处理流程的另外一部分，也就是本节要讲的中断处理流程的下半部。

那哪些工作由中断处理程序完成，哪些工作留给下半部来执行呢？其实上半部和下半部的工作划分不存在某种严格限制，这主要取决于驱动程序开发者自己的判断，一般最好能将中断处理程序执行时间缩短到最小。中断处理程序几乎都需要通过操作硬件对中断的到达进行确认，有时还会做对时间非常敏感的工作（如拷贝数据），其余的工作基本上留给下半部来处理，下半部就是执行与中断处理密切相关但中断处理程序本身不执行的工作。一般对时间非常敏感、和硬件相关、要保证不被其它中断(特别是相同的中断)打断的这些任务放在中断处理程序中执行，其他任务考虑放在下半部执行。

那下半部什么时候执行呢？下半部不需要指定明确执行时间，只要把任务推迟一点，让它们在系统不太忙且中断恢复后执行就可以了，而且执行期间可以相应所有中断。

上半部只能通过中断处理程序实现，而下半部可以有多种机制来实现，在2.6.32版本中，有三种不同形式的下半部实现机制：软中断、tasklet、工作队列。下面来看一下这三种下半部的实现。

##软中断
在start_kernerl()函数中，系统初始化软中断。


```c
asmlinkage void __init start_kernel(void)  
{  
    char * command_line;  
    extern struct kernel_param __start___param[], __stop___param[];  
  
    smp_setup_processor_id();  
......  
    softirq_init();//初始化软中断  
......  
  
    /* Do the rest non-__init'ed, we're now alive */  
    rest_init();  
}
```

在softirq_init()中会注册两个常用类型的软中断, 具体代码如下（位于kernel/softirq.c）:

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
  
    //此处注册两个软中断  
    open_softirq(TASKLET_SOFTIRQ, tasklet_action);     open_softirq(HI_SOFTIRQ, tasklet_hi_action);  
}  
```

注册函数open_softirq()参数含义:
nr:软中断类型 action:软中断处理函数


```c
void open_softirq(int nr, void (*action)(struct softirq_action *))  
{  
    softirq_vec[nr].action = action;  
}  
```

softirq_action结构表示软中断，定义在`<include/linux/interrupt.h>`


```c
struct softirq_action  
{  
    void    (*action)(struct softirq_action *);  
}  
```

文件`<kernel/softirq.c>`中定义了32个该结构体的数组：

```c
static struct softirq_action softirq_vec[NR_SOFTIRQS] __cacheline_aligned_in_smp;  
```

每注册一个软中断都会占该数组一个位置，因此系统中最多有32个软中断。从上面的代码中,我们可以看到:open_softirq()中.其实就是对softirq_vec数组的nr项赋值.softirq_vec是一个32元素的数组,实际上linux内核只使用了几项：

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

那么软中断注册完成之后，什么时候触发软中断处理函数执行呢？通常情况下，软中断会在中断处理程序返回前标记它，使其在稍后合适的时候被执行。在下列地方，待处理的软中断会被检查和执行：
###1.处理完一个硬件中断以后；
###2.在ksoftirqd内核线程中；
###3.在那些显示检查和执行待处理的软中断的代码中，如网络子系统中。

有关网络子系统内容可参考文章：http://blog.csdn.net/shallnet/article/details/26269781

无论如何，软中断会在do_softirq()（位于`<kernel/softirq.c>`中）中执行，如果有待处理的软中断，do_softirq会循环遍历每一个，调用他们的软中断处理程序。


```c
asmlinkage void do_softirq(void) {     __u32 pending;     unsigned long flags;     //如果在硬件中断环境中就退出，软中断不可以在硬件中断上下文或者是在软中断环境中使用，使用in_interrupt()来防止软中断嵌套，和抢占硬中断环境。     if (in_interrupt())         return;     //禁止本地中断     local_irq_save(flags);     pending = local_softirq_pending();  
    //如果有软中断要处理，则进入__do_softirq()  
    if (pending) __do_softirq();     
    local_irq_restore(flags);    
```

下面看一下__do_softirq()的实现：


```c
asmlinkage void __do_softirq(void)
{
    struct softirq_action* h;
    __u32 pending;
    int max_restart = MAX_SOFTIRQ_RESTART;
    int cpu;

    pending = local_softirq_pending();    //pending用于保留待处理软中断32位位图
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
            1) {    //如果pending第n位被设置为1，那么处理第n位对应类型的软中断
            int prev_count = preempt_count();
            kstat_incr_softirqs_this_cpu(h - softirq_vec);

            trace_softirq_entry(h, softirq_vec);
            h->action(h);    //执行软中断处理函数
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
        pending >>= 1;    //pending右移一位，循环检查其每一位
    } while (pending);    //直到pending变为0，pending最多32位，所以循环最多执行32次。

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

使用软中断必须要在编译期间静态注册，一般只有像网络这样对性能要求高的情况才使用软中断，文章前面我们也看到，系统中注册的软中断就那么几个。大部分时候，使用下半部另外一种机制tasklet的情况更多一些，tasklet可以动态的注册，可以被看作是一种性能和易用性之间寻求平衡的一种产物。事实上，大部分驱动程序都是用tasklet来实现他们的下半部。