# （四）：下半部機制之tasklet


tasklet是利用軟中斷實現的一種下半部機制。tasklet相比於軟中斷，其接口更加簡單方便，鎖保護要求較低。
tasklet由tasklet_struct結構體表示：


```c
struct tasklet_struct  
{  
    struct tasklet_struct *next;    //鏈表中下一個tasklet  
    unsigned long state;        //tasklet狀態  
    atomic_t count;        //引用計數  
    void (*func)(unsigned long);    //tasklet處理函數  
    unsigned long data;    //給tasklet處理函數的參數  
};  
```

tasklet還分為了高優先級tasklet與一般tasklet，前面分析軟中斷時softirq_init()註冊的兩個tasklet軟中斷。


```c
void __init softirq_init(void)  
{  
    ......  
    //此處註冊兩個軟中斷  
    open_softirq(TASKLET_SOFTIRQ, tasklet_action);  
    open_softirq(HI_SOFTIRQ, tasklet_hi_action);      
......  
}  
```

其處理函數分別為 tasklet_action()和tasklet_hi_action()。

tasklet_action()函數實現為：


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
            if (!atomic_read(&t->count)) {        //t->count為零才會調用task_struct裡的函數  
                if (!test_and_clear_bit(TASKLET_STATE_SCHED, &t->state))  
                    BUG();  
  
                 t->func(t->data);    //設置了TASKLET_STATE_SCHED標誌才會被遍歷到鏈表上對應的函數  
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
  
//tasklet_hi_action函數實現類似  
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

tasklet_hi_action函數實現類似：


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

這兩個函數主要是做了如下動作：
1.禁止中斷，併為當前處理器檢索tasklet_vec或tasklet_hi_vec鏈表。<br>
2.將當前處理器上的該鏈表設置為NULL,達到清空的效果。<br>
3.運行相應中斷。<br>
4.循環遍歷獲得鏈表上的每一個待處理的tasklet。<br>
5.如果是多處理器系統，通過檢查TASKLET_STATE_RUN來判斷這個tasklet是否正在其他處理器上運行。如果它正在運行，那麼現在就不要執行，跳
   到下一個待處理的tasklet去。<br>
6.如果當前這個tasklet沒有執行，將其狀態設置為TASKLETLET_STATE_RUN,這樣別的處理器就不會再去執行它了。<br>
7.檢查count值是否為0，確保tasklet沒有被禁止。如果tasklet被禁止，則跳到下一個掛起的tasklet去。<br>
8.現在可以確定這個tasklet沒有在其他地方執行，並且被我們設置為執行狀態，這樣它在其他部分就不會被執行，並且引用計數器為0，現在可以執行tasklet的處理程序了。<br>
9.重複執行下一個tasklet，直至沒有剩餘的等待處理的tasklets。<br>

一般情況下，都是用tasklet來實現下半部，tasklet可以動態創建、使用方便、執行速度快。下面來看一下如何創建自己的tasklet呢？
##第一步，聲明自己的tasklet。

既可以靜態也可以動態創建，這取決於選擇是想有一個對tasklet的直接引用還是間接引用。靜態創建方法(直接引用)，可以使用下列兩個宏的一個(在linux/interrupt.h中定義)：


```c
DECLARE_TASKLET(name,func,data)  
DECLARE_TASKLET_DISABLED(name,func,data)  
```

這兩個宏的實現為：

```c
#define DECLARE_TASKLET(name, func, data) \  
struct tasklet_struct name = { NULL, 0, ATOMIC_INIT(0), func, data }  
  
#define DECLARE_TASKLET_DISABLED(name, func, data) \  
struct tasklet_struct name = { NULL, 0, ATOMIC_INIT(1), func, data }  
```

這兩個宏之間的區別在於引用計數器的初始值不同，前面一個把創建的tasklet的引用計數器設置為0，使其處於激活狀態，另外一個將其設置為1，處於禁止狀態。而動態創建(間接引用)的方式如下：
tasklet_init(t,tasklet_handler,dev);
其實現代碼為：

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

##第二步，編寫tasklet處理程序。

tasklet處理函數類型是void tasklet_handler(unsigned long data)。因為是靠軟中斷實現，所以tasklet不能休眠，也就是說不能在tasklet中使用信號量或者其他什麼阻塞式的函數。由於tasklet 運行時允許響應中斷，所以必須做好預防工作，如果新加入的tasklet和中斷處理程序之間共享了某些數據額的話。兩個相同的tasklet絕不能同時執 行，如果新加入的tasklet和其他的tasklet或者軟中斷共享了數據，就必須要進行適當地鎖保護。



第三步，調度自己的tasklet。

調用tasklet_schedule()（或tasklet_hi_schedule()）函數，tasklet就會進入掛起狀態以便執行。如果在還沒有得到運行機會之前，如果有一個相同的tasklet又被調度了，那麼它仍然只會運行一次。如果這時已經開始運行，那麼這個新的tasklet會被重新調度並再次運行。一種優化策略是一個tasklet總在調度它的處理器上執行。

調用tasklet_disable()來禁止某個指定的 tasklet，如果該tasklet當前正在執行，這個函數會等到它執行完畢再返回。調用tasklet_disable_nosync()也是來禁止 的，只是不用在返回前等待tasklet執行完畢，這麼做不太安全，因為沒法估計該tasklet是否仍在執行。 tasklet_enable()激活一個tasklet。可以使用tasklet_kill()函數從掛起的對列中去掉一個tasklet。這個函數會 首先等待該tasklet執行完畢，然後再將其移去。當然，沒有什麼可以阻止其他地方的代碼重新調度該tasklet。由於該函數可能會引起休眠，所以禁止在中斷上下文中使用它。

下面來看一下函數tasklet_schedule的實現：

```c
static inline void tasklet_schedule(struct tasklet_struct* t)
{
    //檢查tasklet的狀態是否為TASKLET_STATE_SCHED.如果是，說明tasklet已經被調度過了，函數返回。
    if (!test_and_set_bit(TASKLET_STATE_SCHED, &t->state)) {
        <span style = "font-family:微軟雅黑;">          < / span >
                      __tasklet_schedule(t);
    }
}

void __tasklet_schedule(struct tasklet_struct* t)
{
    unsigned long flags;

    //保存中斷狀態，然後禁止本地中斷。在執行tasklet代碼時，這麼做能夠保證處理器上的數據不會弄亂。
    local_irq_save(flags);

    //把需要調度的tasklet加到每個處理器一個的tasklet_vec鏈表或task_hi_vec鏈表的表頭上去。
    t->next = NULL;
    *__get_cpu_var(tasklet_vec).tail = t;
    __get_cpu_var(tasklet_vec).tail = &(t->next);

    //喚起TASKLET_SOFTIRQ或HI_SOFTIRQ軟中斷，這樣在下一次調用do_softirq()時就會執行該tasklet。
    raise_softirq_irqoff(TASKLET_SOFTIRQ);

    //恢復中斷到原狀態並返回。
    local_irq_restore(flags);
}
```

tasklet_hi_schedule()函數的實現細節類似。

對於軟中斷，內核會選擇幾個特殊的實際進行處理(常見的是中 斷處理程序返回時)。軟中斷被觸發的頻率有時會很好，而且還可能會自行重複觸發，這帶來的結果就是用戶空間的進程無法獲得足夠的處理器時間，因為處於飢餓 狀態。

同時，如果單純的對重複觸發的軟中斷採取不立即處理的策略也是無法接受的。

內核選中的方案是不會立即處理重新觸發的軟中斷，作為改進， 當大量軟中斷出現的時候，內核會喚醒一組內核線程來處理這些負載。這些線程在最低優先級上運行(nice值為19)。這種這種方案能夠保證在軟中斷負擔很 重的時候用戶程序不會因為得不到處理時間而處理飢餓狀態。相應的，也能保證“過量”的軟中斷終究會得到處理。最後，在空閒系統上，這個方案同樣表現良好， 軟中斷處理得非常迅速(因為僅存的內存線程肯定會馬上調度)。為了保證只要有空閒的處理器，它們就會處理軟中斷，所以給每個處理器都分配一個這樣的線程。 所有線程的名字都叫做ksoftirad/n，區別在於n，它對應的是處理器的編號。一旦該線程被初始化，它就會執行類似下面這樣的死循環：


```c
for(;;){  
    if(!softirq_pending(cpu))//softirq_pending()負責發現是否有待處理的軟中斷  
        schedule();    //沒有待處理軟中斷就喚起調度程序選擇其他可執行進程投入運行  
    set_current_state(TASK_RUNNING);  
    while(softirq_pending(cpu)){  
        do_softirq();//有待處理的軟中斷，ksoftirq調用do_softirq()去處理他。  
        if(need_resched())    //如果有必要的話，每次軟中斷完成之後調用schedule函數讓其他重要進程得到處理機會  
            schedule();  
    }  
  
//當所有需要執行的操作都完成以後，該內核線程將自己設置為 TASK_INTERRUPTIBLE狀態  
    set_current_state(TASK_INTERRUPTIBLE);  
}  
```