# （四）：下半部机制之tasklet


tasklet是利用软中断实现的一种下半部机制。tasklet相比于软中断，其接口更加简单方便，锁保护要求较低。
tasklet由tasklet_struct结构体表示：


```c
struct tasklet_struct  
{  
    struct tasklet_struct *next;    //链表中下一个tasklet  
    unsigned long state;        //tasklet状态  
    atomic_t count;        //引用计数  
    void (*func)(unsigned long);    //tasklet处理函数  
    unsigned long data;    //给tasklet处理函数的参数  
};  
```

tasklet还分为了高优先级tasklet与一般tasklet，前面分析软中断时softirq_init()注册的两个tasklet软中断。


```c
void __init softirq_init(void)  
{  
    ......  
    //此处注册两个软中断  
    open_softirq(TASKLET_SOFTIRQ, tasklet_action);  
    open_softirq(HI_SOFTIRQ, tasklet_hi_action);      
......  
}  
```

其处理函数分别为 tasklet_action()和tasklet_hi_action()。

tasklet_action()函数实现为：


```c
static void tasklet_action(struct softirq_action *a)  
{  
    struct tasklet_struct *list;  
  
    local_irq_disable();  
    list = __get_cpu_var(tasklet_vec).head;  
    __get_cpu_var(tasklet_vec).head = NULL;  
    __get_cpu_var(tasklet_vec).tail = &__get_cpu_var(tasklet_vec).head;  
    local_irq_enable();  
  
    while (list) {  
        struct tasklet_struct *t = list;  
  
        list = list->next;  
  
        if (tasklet_trylock(t)) {      
            if (!atomic_read(&t->count)) {        //t->count为零才会调用task_struct里的函数  
                if (!test_and_clear_bit(TASKLET_STATE_SCHED, &t->state))  
                    BUG();  
  
                 t->func(t->data);    //设置了TASKLET_STATE_SCHED标志才会被遍历到链表上对应的函数  
                tasklet_unlock(t);  
                continue;  
            }  
            tasklet_unlock(t);  
        }  
  
        local_irq_disable();  
        t->next = NULL;  
        *__get_cpu_var(tasklet_vec).tail = t;  
        __get_cpu_var(tasklet_vec).tail = &(t->next);  
        __raise_softirq_irqoff(TASKLET_SOFTIRQ);  
        local_irq_enable();  
    }  
}  
  
//tasklet_hi_action函数实现类似  
static void tasklet_hi_action(struct softirq_action *a)  
{  
    struct tasklet_struct *list;  
  
    local_irq_disable();  
    list = __get_cpu_var(tasklet_hi_vec).head;  
    __get_cpu_var(tasklet_hi_vec).head = NULL;  
    __get_cpu_var(tasklet_hi_vec).tail = &__get_cpu_var(tasklet_hi_vec).head;  
    local_irq_enable();  
  
    while (list) {  
        struct tasklet_struct *t = list;  
  
        list = list->next;  
  
        if (tasklet_trylock(t)) {  
            if (!atomic_read(&t->count)) {  
                if (!test_and_clear_bit(TASKLET_STATE_SCHED, &t->state))  
                    BUG();  
                t->func(t->data);  
                tasklet_unlock(t);  
                continue;  
            }  
            tasklet_unlock(t);  
        }  
  
        local_irq_disable();  
        t->next = NULL;  
        *__get_cpu_var(tasklet_hi_vec).tail = t;  
        __get_cpu_var(tasklet_hi_vec).tail = &(t->next);  
        __raise_softirq_irqoff(HI_SOFTIRQ);  
        local_irq_enable();  
    }  
}  
```

tasklet_hi_action函数实现类似：


```c
static void tasklet_hi_action(struct softirq_action *a)  
{  
    struct tasklet_struct *list;  
  
    local_irq_disable();  
    list = __get_cpu_var(tasklet_hi_vec).head;  
    __get_cpu_var(tasklet_hi_vec).head = NULL;  
    __get_cpu_var(tasklet_hi_vec).tail = &__get_cpu_var(tasklet_hi_vec).head;  
    local_irq_enable();  
  
    while (list) {  
        struct tasklet_struct *t = list;  
  
        list = list->next;  
  
        if (tasklet_trylock(t)) {  
            if (!atomic_read(&t->count)) {  
                if (!test_and_clear_bit(TASKLET_STATE_SCHED, &t->state))  
                    BUG();  
                t->func(t->data);  
                tasklet_unlock(t);  
                continue;  
            }  
            tasklet_unlock(t);  
        }  
  
        local_irq_disable();  
        t->next = NULL;  
        *__get_cpu_var(tasklet_hi_vec).tail = t;  
        __get_cpu_var(tasklet_hi_vec).tail = &(t->next);  
        __raise_softirq_irqoff(HI_SOFTIRQ);  
        local_irq_enable();  
    }  
}  
```

这两个函数主要是做了如下动作：
1.禁止中断，并为当前处理器检索tasklet_vec或tasklet_hi_vec链表。<br>
2.将当前处理器上的该链表设置为NULL,达到清空的效果。<br>
3.运行相应中断。<br>
4.循环遍历获得链表上的每一个待处理的tasklet。<br>
5.如果是多处理器系统，通过检查TASKLET_STATE_RUN来判断这个tasklet是否正在其他处理器上运行。如果它正在运行，那么现在就不要执行，跳
   到下一个待处理的tasklet去。<br>
6.如果当前这个tasklet没有执行，将其状态设置为TASKLETLET_STATE_RUN,这样别的处理器就不会再去执行它了。<br>
7.检查count值是否为0，确保tasklet没有被禁止。如果tasklet被禁止，则跳到下一个挂起的tasklet去。<br>
8.现在可以确定这个tasklet没有在其他地方执行，并且被我们设置为执行状态，这样它在其他部分就不会被执行，并且引用计数器为0，现在可以执行tasklet的处理程序了。<br>
9.重复执行下一个tasklet，直至没有剩余的等待处理的tasklets。<br>

一般情况下，都是用tasklet来实现下半部，tasklet可以动态创建、使用方便、执行速度快。下面来看一下如何创建自己的tasklet呢？
##第一步，声明自己的tasklet。

既可以静态也可以动态创建，这取决于选择是想有一个对tasklet的直接引用还是间接引用。静态创建方法(直接引用)，可以使用下列两个宏的一个(在linux/interrupt.h中定义)：


```c
DECLARE_TASKLET(name,func,data)  
DECLARE_TASKLET_DISABLED(name,func,data)  
```

这两个宏的实现为：

```c
#define DECLARE_TASKLET(name, func, data) \  
struct tasklet_struct name = { NULL, 0, ATOMIC_INIT(0), func, data }  
  
#define DECLARE_TASKLET_DISABLED(name, func, data) \  
struct tasklet_struct name = { NULL, 0, ATOMIC_INIT(1), func, data }  
```

这两个宏之间的区别在于引用计数器的初始值不同，前面一个把创建的tasklet的引用计数器设置为0，使其处于激活状态，另外一个将其设置为1，处于禁止状态。而动态创建(间接引用)的方式如下：
tasklet_init(t,tasklet_handler,dev);
其实现代码为：

```c
void tasklet_init(struct tasklet_struct *t,  
          void (*func)(unsigned long), unsigned long data)  
{  
    t->next = NULL;  
    t->state = 0;  
    atomic_set(&t->count, 0);  
    t->func = func;  
    t->data = data;  
}
```

##第二步，编写tasklet处理程序。

tasklet处理函数类型是void tasklet_handler(unsigned long data)。因为是靠软中断实现，所以tasklet不能休眠，也就是说不能在tasklet中使用信号量或者其他什么阻塞式的函数。由于tasklet 运行时允许响应中断，所以必须做好预防工作，如果新加入的tasklet和中断处理程序之间共享了某些数据额的话。两个相同的tasklet绝不能同时执 行，如果新加入的tasklet和其他的tasklet或者软中断共享了数据，就必须要进行适当地锁保护。



第三步，调度自己的tasklet。

调用tasklet_schedule()（或tasklet_hi_schedule()）函数，tasklet就会进入挂起状态以便执行。如果在还没有得到运行机会之前，如果有一个相同的tasklet又被调度了，那么它仍然只会运行一次。如果这时已经开始运行，那么这个新的tasklet会被重新调度并再次运行。一种优化策略是一个tasklet总在调度它的处理器上执行。

调用tasklet_disable()来禁止某个指定的 tasklet，如果该tasklet当前正在执行，这个函数会等到它执行完毕再返回。调用tasklet_disable_nosync()也是来禁止 的，只是不用在返回前等待tasklet执行完毕，这么做不太安全，因为没法估计该tasklet是否仍在执行。 tasklet_enable()激活一个tasklet。可以使用tasklet_kill()函数从挂起的对列中去掉一个tasklet。这个函数会 首先等待该tasklet执行完毕，然后再将其移去。当然，没有什么可以阻止其他地方的代码重新调度该tasklet。由于该函数可能会引起休眠，所以禁止在中断上下文中使用它。

下面来看一下函数tasklet_schedule的实现：

```c
static inline void tasklet_schedule(struct tasklet_struct* t)
{
    //检查tasklet的状态是否为TASKLET_STATE_SCHED.如果是，说明tasklet已经被调度过了，函数返回。
    if (!test_and_set_bit(TASKLET_STATE_SCHED, &t->state)) {
        <span style = "font-family:微软雅黑;">          < / span >
                      __tasklet_schedule(t);
    }
}

void __tasklet_schedule(struct tasklet_struct* t)
{
    unsigned long flags;

    //保存中断状态，然后禁止本地中断。在执行tasklet代码时，这么做能够保证处理器上的数据不会弄乱。
    local_irq_save(flags);

    //把需要调度的tasklet加到每个处理器一个的tasklet_vec链表或task_hi_vec链表的表头上去。
    t->next = NULL;
    *__get_cpu_var(tasklet_vec).tail = t;
    __get_cpu_var(tasklet_vec).tail = &(t->next);

    //唤起TASKLET_SOFTIRQ或HI_SOFTIRQ软中断，这样在下一次调用do_softirq()时就会执行该tasklet。
    raise_softirq_irqoff(TASKLET_SOFTIRQ);

    //恢复中断到原状态并返回。
    local_irq_restore(flags);
}
```

tasklet_hi_schedule()函数的实现细节类似。

对于软中断，内核会选择几个特殊的实际进行处理(常见的是中 断处理程序返回时)。软中断被触发的频率有时会很好，而且还可能会自行重复触发，这带来的结果就是用户空间的进程无法获得足够的处理器时间，因为处于饥饿 状态。

同时，如果单纯的对重复触发的软中断采取不立即处理的策略也是无法接受的。

内核选中的方案是不会立即处理重新触发的软中断，作为改进， 当大量软中断出现的时候，内核会唤醒一组内核线程来处理这些负载。这些线程在最低优先级上运行(nice值为19)。这种这种方案能够保证在软中断负担很 重的时候用户程序不会因为得不到处理时间而处理饥饿状态。相应的，也能保证“过量”的软中断终究会得到处理。最后，在空闲系统上，这个方案同样表现良好， 软中断处理得非常迅速(因为仅存的内存线程肯定会马上调度)。为了保证只要有空闲的处理器，它们就会处理软中断，所以给每个处理器都分配一个这样的线程。 所有线程的名字都叫做ksoftirad/n，区别在于n，它对应的是处理器的编号。一旦该线程被初始化，它就会执行类似下面这样的死循环：


```c
for(;;){  
    if(!softirq_pending(cpu))//softirq_pending()负责发现是否有待处理的软中断  
        schedule();    //没有待处理软中断就唤起调度程序选择其他可执行进程投入运行  
    set_current_state(TASK_RUNNING);  
    while(softirq_pending(cpu)){  
        do_softirq();//有待处理的软中断，ksoftirq调用do_softirq()去处理他。  
        if(need_resched())    //如果有必要的话，每次软中断完成之后调用schedule函数让其他重要进程得到处理机会  
            schedule();  
    }  
  
//当所有需要执行的操作都完成以后，该内核线程将自己设置为 TASK_INTERRUPTIBLE状态  
    set_current_state(TASK_INTERRUPTIBLE);  
}  
```