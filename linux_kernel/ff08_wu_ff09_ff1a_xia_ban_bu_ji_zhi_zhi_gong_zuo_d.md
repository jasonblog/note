# （五）：下半部机制之工作队列及几种机制的选择


工作队列是下半部的另外一种将工作推后执行形式。和软中断、tasklet不同，工作队列将工作推后交由一个内核线程去执行，并且该下半部总会在进程上下文中执行。这样，工作队列允许重新调度甚至是睡眠。

所以，如果推后执行的任务需要睡眠，就选择工作队列。如果不需要睡眠，那就选择软中断或tasklet。工作队列是唯一能在进程上下文中运行的下半部实现机制，也只有它才可以睡眠。

工作队列子系统是一个用于创建内核线程的接口，通过它创建的进程负责执行由内核其他部分排到队列里的任务。它创建的这些内核线程称作工作者线程。工作队列可以让你的驱动程序创建一个专门的工作者线程来处理需要推后的工作。不过，工作队列子系统提供了一个缺省的工作者线程来处理这些工作。因此，工作队列最基本的表现形式就转变成一个把需要推后执行的任务交给特定的通用线程这样一种接口。缺省的工作线程叫做event/n.每个处理器对应一个线程，这里的n代表了处理器编号。除非一个驱动程序或者子系统必须建立一个属于自己的内核线程，否则最好还是使用缺省线程。

使用下面命令可以看到默认event工作者线程，每个处理器对应一个线程：


```c
ps x | grep event | grep -v grep  
9 ?        S      0:00 [events/0]  
10 ?        S      0:00 [events/1]  
```

 工作者线程使用workqueue_struct结构表示（位于`<kernel/workqueue.c>`中）：
 
 ```c
 struct workqueue_struct {  
    struct cpu_workqueue_struct *cpu_wq;    //该数组每一项对应系统中的一个处理器  
    struct list_head list;  
    const char *name;  
    int singlethread;  
    int freezeable;     /* Freeze threads during suspend */  
    int rt;  
#ifdef CONFIG_LOCKDEP  
    struct lockdep_map lockdep_map;  
#endif  
} 
```

每个处理器，每个工作者线程对应对应一个cpu_workqueue_struct结构体（位于`<kernel/workqueue.c>`中）：

```c
struct cpu_workqueue_struct {  
  
    spinlock_t lock;    //保护该结构  
  
    struct list_head worklist;    //工作列表  
    wait_queue_head_t more_work;    //等待队列，其中的工作者线程因等待而处于睡眠状态  
    struct work_struct *current_work;  
  
    struct workqueue_struct *wq;    //关联工作队列结构  
    struct task_struct *thread;    // 关联线程,指向结构中工作者线程的进程描述符指针  
} ____cacheline_aligned;
```

每个工作者线程类型关联一个自己的workqueue_struct，在该结构体里面，给每个线程分配一个cpu_workqueue_struct ，因而也就是给每个处理器分配一个，因为每个处理器都有一个该类型的工作者线程。

所有的工作者线程都是使用普通的内核线程实现的，他们都要执行worker_thread()函数。在它初始化完以后，这个函数执行一个死循环执行一个循环并开始休眠，当有操作被插入到队列的时候，线程就会被唤醒，以便执行这些操作。当没有剩余的时候，它又会继续休眠。工作由work_struct（位于<kernel/workqueue.c>中）结构表示：


```c
struct work_struct {                                                                                                                                      
    atomic_long_t data;  
......  
    struct list_head entry;//连接所有链表  
    work_func_t func;  
.....  
}; 
```

当一个工作线程被唤醒时，它会执行它的链表上的所有工作。工作一旦执行完毕，它就将相应的work_struct对象从链表上移去，当链表不再有对象时，它就继续休眠。woker_thread()函数如下：


```c
static int worker_thread(void *__cwq)  
{  
    struct cpu_workqueue_struct *cwq = __cwq;  
    DEFINE_WAIT(wait);  
  
    if (cwq->wq->freezeable)  
        set_freezable();  
  
    for (;;) {  
    //线程将自己设置为休眠状态并把自己加入等待队列  
        prepare_to_wait(&cwq->more_work, &wait, TASK_INTERRUPTIBLE);  
        if (!freezing(current) &&  
            !kthread_should_stop() &&  
            list_empty(&cwq->worklist))  
            schedule();//如果工作对列是空的，线程调用schedule()函数进入睡眠状态  
        finish_wait(&cwq->more_work, &wait);  
  
        try_to_freeze();  
  
//如果链表有对象，线程就将自己设为运行态，脱离等待队列  
        if (kthread_should_stop())  
            break;  
  
//再次调用run_workqueue()执行推后的工作  
        run_workqueue(cwq);  
    }  
  
    return 0;  
}  
```

之后由run_workqueue()函数来完成实际推后到此的工作：

```c
static void run_workqueue(struct cpu_workqueue_struct *cwq)   
{  
    spin_lock_irq(&cwq->lock);  
    while (!list_empty(&cwq->worklist)) {  
        //链表不为空时，选取下一个节点对象  
        struct work_struct *work = list_entry(cwq->worklist.next,  
                        struct work_struct, entry);  
        //获取希望执行的函数func及其参数data  
        work_func_t f = work->func;  
......  
        trace_workqueue_execution(cwq->thread, work);  
        cwq->current_work = work;  
        //把该结点从链表上解下来  
        list_del_init(cwq->worklist.next);  
        spin_unlock_irq(&cwq->lock);  
  
        BUG_ON(get_wq_data(work) != cwq);  
        //将待处理标志位pending清0  
        work_clear_pending(work);  
        lock_map_acquire(&cwq->wq->lockdep_map);  
        lock_map_acquire(&lockdep_map);  
        //执行函数  
        f(work);  
        lock_map_release(&lockdep_map);  
        lock_map_release(&cwq->wq->lockdep_map);  
  
......  
        spin_lock_irq(&cwq->lock);  
        cwq->current_work = NULL;  
    }  
    spin_unlock_irq(&cwq->lock);  
}  
```

系统允许有多种类型工作者线程存在，默认情况下内核只有event这一种类型的工作者线程，每个工作者线程都由一个cpu_workqueue_struct 结构体表示，大部分情况下，驱动程序都使用现存的默认工作者线程。

工作队列的使用很简单。可以使用缺省的events任务队列，也可以创建新的工作者线程。

`第一步`、创建需要推后完成的工作。

DECLARE_WORK(name,void (*func)(void *),void *data);        //编译时静态创建
INIT_WORK(struct work_struct *work, void (*func)(void *));    //运行时动态创建

`第二步`、编写队列处理函数，处理函数会由工作者线程执行，因此，函数会运行在进程上下文中，默认情况下，允许相应中断，并且不持有锁。如果需要，函数可以睡眠。需要注意的是，尽管处理函数运行在进程上下文中，但它不能访问用户空间，因为内核线程在用户空间没有相应的内存映射。函数原型如下：

```c
void work_hander(void *data);
```
`第三步`、调度工作队列。

调用
schedule_work(&work)；
work马上就会被调度，一旦其所在的处理器上的工作者线程被唤醒，它就会被执行。当然如果不想快速执行，而是想延迟一段时间执行，调用

```c
schedule_delay_work(&work,delay);
```

delay是要延迟的时间节拍。
默认工作者线程的调度函数其实就是做了一层封装，减少了 默认工作者线程的参数输入，如下：


```c
#include <linux/init.h>
#include <linux/module.h>

#include <linux/workqueue.h> //work_strcut  


//struct work_struct      ws;
struct delayed_work     dw;

void workqueue_func(struct work_struct* ws)    //处理函数
{
    printk(KERN_ALERT"Hello, this is shallnet!\n");
}

static int __init kwq_init(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);

    //INIT_WORK(&ws, workqueue_func);    //建需要推后完成的工作
    //schedule_work(&ws);    //调度工作

    INIT_DELAYED_WORK(&dw, workqueue_func);
    schedule_delayed_work(&dw, 10000);

    return 0;
}

static void __exit kwq_exit(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);

    flush_scheduled_work();
}

module_init(kwq_init);
module_exit(kwq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

上面的操作是使用缺省的工作队列，下面来看一下创建一个新的工作队列是如何操作的？
创建一个新的工作队列和与之相应的工作者线程，方法很简单，使用如下函数：


```c
struct workqueue_struct *create_workqueue(const char *name);  
```

name是新内核线程的名字。比如缺省events队列的创建是这样使用的：

```c
struct workqueue_struct    *keventd_wq；  
kevent_wq = create_workqueue("event");  
```

这样就创建了所有的工作者线程，每个处理器都有一个。然后调用如下函数进行调度：

```c
int queue_work(struct workqueue_struct *wq, struct work_struct *work);  
int queue_delayed_work(struct workqueue_struct *wq,struct delayed_work *work,unsigned long delay);  
```

最后可以调用flush_workqueue(struct workqueue_struct *wq);刷新指定工作队列。
下面为自定义新的工作队列的示例：


```c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h> //work_strcut  

struct workqueue_struct* sln_wq = NULL;
//struct work_struct      ws;
struct delayed_work     dw;

void workqueue_func(struct work_struct* ws)
{
    printk(KERN_ALERT"Hello, this is shallnet!\n");
}

static int __init kwq_init(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);

    sln_wq = create_workqueue("sln_wq");    //创建名为sln_wq的工作队列

    //INIT_WORK(&ws, workqueue_func);
    //queue_work(sln_wq, &ws);

    INIT_DELAYED_WORK(&dw, workqueue_func);    //
    queue_delayed_work(sln_wq, &dw, 10000);    //

    return 0;
}

static void __exit kwq_exit(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);

    flush_workqueue(sln_wq);
}

module_init(kwq_init);
module_exit(kwq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

使用`ps`可以查看到名为sln_wq的工作者线程。

本节源码下载：
http://download.csdn.net/detail/gentleliu/8941433

在当前2.6.32版本中，我们讲了三种下半部机制：软中断、tasklet、工作队列。其中tasklet基于软中断，而工作队列靠内核线程实现。

使用软中断必须要确保共享数据的安全，因为相同类别的软中断可能在不同处理器上同时执行。在对于时间要求是否严格和执行频率很高的应用，或准备利用每一处理器上的变量或类型情形，可以考虑使用软中断，如网络子系统。

tasklet接口简单，可以动态创建，且两个通知类型的tasklet不能同时执行，所以实现起来较简单。驱动程序应该尽量选择tasklet而不是软中断。

工作队列工作于进程上下文，易于使用。由于牵扯到内核线程或上下文的切换，可能开销较大。如果你需要把任务推后到进程上下文中，或你需要休眠，那就只有使用工作队列了。
