# （五）：下半部機制之工作隊列及幾種機制的選擇


工作隊列是下半部的另外一種將工作推後執行形式。和軟中斷、tasklet不同，工作隊列將工作推後交由一個內核線程去執行，並且該下半部總會在進程上下文中執行。這樣，工作隊列允許重新調度甚至是睡眠。

所以，如果推後執行的任務需要睡眠，就選擇工作隊列。如果不需要睡眠，那就選擇軟中斷或tasklet。工作隊列是唯一能在進程上下文中運行的下半部實現機制，也只有它才可以睡眠。

工作隊列子系統是一個用於創建內核線程的接口，通過它創建的進程負責執行由內核其他部分排到隊列裡的任務。它創建的這些內核線程稱作工作者線程。工作隊列可以讓你的驅動程序創建一個專門的工作者線程來處理需要推後的工作。不過，工作隊列子系統提供了一個缺省的工作者線程來處理這些工作。因此，工作隊列最基本的表現形式就轉變成一個把需要推後執行的任務交給特定的通用線程這樣一種接口。缺省的工作線程叫做event/n.每個處理器對應一個線程，這裡的n代表了處理器編號。除非一個驅動程序或者子系統必須建立一個屬於自己的內核線程，否則最好還是使用缺省線程。

使用下面命令可以看到默認event工作者線程，每個處理器對應一個線程：


```c
ps x | grep event | grep -v grep  
9 ?        S      0:00 [events/0]  
10 ?        S      0:00 [events/1]  
```

 工作者線程使用workqueue_struct結構表示（位於`<kernel/workqueue.c>`中）：
 
 ```c
 struct workqueue_struct {  
    struct cpu_workqueue_struct *cpu_wq;    //該數組每一項對應系統中的一個處理器  
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

每個處理器，每個工作者線程對應對應一個cpu_workqueue_struct結構體（位於`<kernel/workqueue.c>`中）：

```c
struct cpu_workqueue_struct {  
  
    spinlock_t lock;    //保護該結構  
  
    struct list_head worklist;    //工作列表  
    wait_queue_head_t more_work;    //等待隊列，其中的工作者線程因等待而處於睡眠狀態  
    struct work_struct *current_work;  
  
    struct workqueue_struct *wq;    //關聯工作隊列結構  
    struct task_struct *thread;    // 關聯線程,指向結構中工作者線程的進程描述符指針  
} ____cacheline_aligned;
```

每個工作者線程類型關聯一個自己的workqueue_struct，在該結構體裡面，給每個線程分配一個cpu_workqueue_struct ，因而也就是給每個處理器分配一個，因為每個處理器都有一個該類型的工作者線程。

所有的工作者線程都是使用普通的內核線程實現的，他們都要執行worker_thread()函數。在它初始化完以後，這個函數執行一個死循環執行一個循環並開始休眠，當有操作被插入到隊列的時候，線程就會被喚醒，以便執行這些操作。當沒有剩餘的時候，它又會繼續休眠。工作由work_struct（位於<kernel/workqueue.c>中）結構表示：


```c
struct work_struct {                                                                                                                                      
    atomic_long_t data;  
......  
    struct list_head entry;//連接所有鏈表  
    work_func_t func;  
.....  
}; 
```

當一個工作線程被喚醒時，它會執行它的鏈表上的所有工作。工作一旦執行完畢，它就將相應的work_struct對象從鏈表上移去，當鏈表不再有對象時，它就繼續休眠。woker_thread()函數如下：


```c
static int worker_thread(void *__cwq)  
{  
    struct cpu_workqueue_struct *cwq = __cwq;  
    DEFINE_WAIT(wait);  
  
    if (cwq->wq->freezeable)  
        set_freezable();  
  
    for (;;) {  
    //線程將自己設置為休眠狀態並把自己加入等待隊列  
        prepare_to_wait(&cwq->more_work, &wait, TASK_INTERRUPTIBLE);  
        if (!freezing(current) &&  
            !kthread_should_stop() &&  
            list_empty(&cwq->worklist))  
            schedule();//如果工作對列是空的，線程調用schedule()函數進入睡眠狀態  
        finish_wait(&cwq->more_work, &wait);  
  
        try_to_freeze();  
  
//如果鏈表有對象，線程就將自己設為運行態，脫離等待隊列  
        if (kthread_should_stop())  
            break;  
  
//再次調用run_workqueue()執行推後的工作  
        run_workqueue(cwq);  
    }  
  
    return 0;  
}  
```

之後由run_workqueue()函數來完成實際推後到此的工作：

```c
static void run_workqueue(struct cpu_workqueue_struct *cwq)   
{  
    spin_lock_irq(&cwq->lock);  
    while (!list_empty(&cwq->worklist)) {  
        //鏈表不為空時，選取下一個節點對象  
        struct work_struct *work = list_entry(cwq->worklist.next,  
                        struct work_struct, entry);  
        //獲取希望執行的函數func及其參數data  
        work_func_t f = work->func;  
......  
        trace_workqueue_execution(cwq->thread, work);  
        cwq->current_work = work;  
        //把該結點從鏈表上解下來  
        list_del_init(cwq->worklist.next);  
        spin_unlock_irq(&cwq->lock);  
  
        BUG_ON(get_wq_data(work) != cwq);  
        //將待處理標誌位pending清0  
        work_clear_pending(work);  
        lock_map_acquire(&cwq->wq->lockdep_map);  
        lock_map_acquire(&lockdep_map);  
        //執行函數  
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

系統允許有多種類型工作者線程存在，默認情況下內核只有event這一種類型的工作者線程，每個工作者線程都由一個cpu_workqueue_struct 結構體表示，大部分情況下，驅動程序都使用現存的默認工作者線程。

工作隊列的使用很簡單。可以使用缺省的events任務隊列，也可以創建新的工作者線程。

`第一步`、創建需要推後完成的工作。

DECLARE_WORK(name,void (*func)(void *),void *data);        //編譯時靜態創建
INIT_WORK(struct work_struct *work, void (*func)(void *));    //運行時動態創建

`第二步`、編寫隊列處理函數，處理函數會由工作者線程執行，因此，函數會運行在進程上下文中，默認情況下，允許相應中斷，並且不持有鎖。如果需要，函數可以睡眠。需要注意的是，儘管處理函數運行在進程上下文中，但它不能訪問用戶空間，因為內核線程在用戶空間沒有相應的內存映射。函數原型如下：

```c
void work_hander(void *data);
```
`第三步`、調度工作隊列。

調用
schedule_work(&work)；
work馬上就會被調度，一旦其所在的處理器上的工作者線程被喚醒，它就會被執行。當然如果不想快速執行，而是想延遲一段時間執行，調用

```c
schedule_delay_work(&work,delay);
```

delay是要延遲的時間節拍。
默認工作者線程的調度函數其實就是做了一層封裝，減少了 默認工作者線程的參數輸入，如下：


```c
#include <linux/init.h>
#include <linux/module.h>

#include <linux/workqueue.h> //work_strcut  


//struct work_struct      ws;
struct delayed_work     dw;

void workqueue_func(struct work_struct* ws)    //處理函數
{
    printk(KERN_ALERT"Hello, this is shallnet!\n");
}

static int __init kwq_init(void)
{
    printk(KERN_ALERT"===%s===\n", __func__);

    //INIT_WORK(&ws, workqueue_func);    //建需要推後完成的工作
    //schedule_work(&ws);    //調度工作

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

上面的操作是使用缺省的工作隊列，下面來看一下創建一個新的工作隊列是如何操作的？
創建一個新的工作隊列和與之相應的工作者線程，方法很簡單，使用如下函數：


```c
struct workqueue_struct *create_workqueue(const char *name);  
```

name是新內核線程的名字。比如缺省events隊列的創建是這樣使用的：

```c
struct workqueue_struct    *keventd_wq；  
kevent_wq = create_workqueue("event");  
```

這樣就創建了所有的工作者線程，每個處理器都有一個。然後調用如下函數進行調度：

```c
int queue_work(struct workqueue_struct *wq, struct work_struct *work);  
int queue_delayed_work(struct workqueue_struct *wq,struct delayed_work *work,unsigned long delay);  
```

最後可以調用flush_workqueue(struct workqueue_struct *wq);刷新指定工作隊列。
下面為自定義新的工作隊列的示例：


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

    sln_wq = create_workqueue("sln_wq");    //創建名為sln_wq的工作隊列

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

使用`ps`可以查看到名為sln_wq的工作者線程。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/8941433

在當前2.6.32版本中，我們講了三種下半部機制：軟中斷、tasklet、工作隊列。其中tasklet基於軟中斷，而工作隊列靠內核線程實現。

使用軟中斷必須要確保共享數據的安全，因為相同類別的軟中斷可能在不同處理器上同時執行。在對於時間要求是否嚴格和執行頻率很高的應用，或準備利用每一處理器上的變量或類型情形，可以考慮使用軟中斷，如網絡子系統。

tasklet接口簡單，可以動態創建，且兩個通知類型的tasklet不能同時執行，所以實現起來較簡單。驅動程序應該儘量選擇tasklet而不是軟中斷。

工作隊列工作於進程上下文，易於使用。由於牽扯到內核線程或上下文的切換，可能開銷較大。如果你需要把任務推後到進程上下文中，或你需要休眠，那就只有使用工作隊列了。
