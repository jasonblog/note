# 中斷


控制週邊裝置時，與同步、鎖定等功能同樣不可或缺的是「中斷」機制。 


##11-1、中斷的概念 
在 kernel 中，所謂的「中斷」是讓軟體(kernel) 可以接收到來自硬體的緊急報告，並做出反應的機制。 

例如，網路卡在收到外面傳來的封包時，就會以中斷向 CPU 報告，CPU 偵側到裝置傳來的中斷後，就會立刻執行 OS 的「中斷處理程序(interrupt handler)」。 

「中斷處理程序(interrupt handler)」涵蓋了「中斷程序(interrupt routine)」以及「中斷服務程序(ISR: Interrupt Service Routine)」，各種 OS 用的稱呼都不太一樣，但概念都是相同的。 


linux kernel 則常常以「IRQ (Interrupt ReQuest」來代表「中斷」或「中斷處理程序」，IRQ最初是中斷控制晶片的用詞，與 CPU 的中斷號碼(中斷向量、位址)不同，但 linux 也把 CPU 的中斷號碼稱為「IRQ」。 

系統內，各個裝置的中斷訊號透過「可程式化中斷控制器」整合在一起，裝置與中斷控制器之間有著稱為「IRQ line」的中斷線路，如果要知道是哪個裝置要求中斷的話，只要看是哪條 IRQ line 送來的訊號即可。 

可程式化中斷控制器中的「可程式化」指的是 kernel 可以修改它的設定，用以決定某些 IRQ line 是否接收中斷。 
中斷控制器在收到中斷之後會把 IRQ line 轉換成對應的中斷編號(向量)，並向 CPU 的 INTR 腳位送出訊號，通知發生中斷。 
「EOI(End Of Interrupt」是 CPU 通知中斷控制器，已處理好中斷情形的訊號，中斷控制器收到這個訊號後，就可以把 IRQ line 放掉，準備接收下一次中斷訊號。 
從 /proc/interrupts 可以看到 IRQ 的狀態，最左邊的數值即是 IRQ 編號：

```sh
cat /proc/interrupts 
```

如果裝置不支援中斷，則 kernel 就要用 「輪詢(polling)」的機制來監視裝置的狀態，這會秏費 CPU 的處理資源，可且處理可能不夠及時。 



##11-2、驅動程式的中斷問題 
驅動程式如果要收到裝置送出的中斷訊號，必須先登記中斷處理程序，中斷處理程序的登記有時隨裝置而異，大多為下列兩者之一：
驅動程式載入後
裝置初始化完成後

I2C 驅動程式屬於前者，而網卡、序列埠驅動程式則屬於後者。 
網卡、序列埠驅動程式必須先設定好硬體才能開始使用，所以在裝置初始化完成後才登記中斷處理程序，反過來說，如果不先關閉裝置(中斷連線)，就不能卸除中斷處理程序，在卸除驅動程式時需注意，舉例來說，網卡要透過 ifconfig 的 up 以及 down 來啟動、關閉。 
而不管是哪種類型，在卸除驅動程式之前，都必須卸除中斷處理程序。 

登錄中斷處理程序用的是 request_irq() 這個 kernel 函式： 

```c
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long irqflags, const char *devname, void *dev_id); 
```

irq 引數指定的是 IRQ 編號。 
handler 引數是中斷處理程序的指標， prototype 如下： 

```c
typedef irqreturn_t (*irq_handler_t) (int, void *); 
```

第一個引數是 IRQ 編號，第二個引數則會傳入 dev_id 的內容。 
回傳值是 irqreturn_t 型別(實際上是 int 型別)，需要傳回適當的數值：

<table border="1">
<tbody><tr><td>傳回值</td><td>意義</td></tr>
<tr><td>IRQ_NONE</td><td>沒有處理這個中斷</td></tr>
<tr><td>IRQ_HANDLED</td><td>已經處理這個中斷</td></tr>
</tbody></table>


irqflags 引數是選用的，不指定時會傳入「0」。 
devname 引數是要在 /proc/interrupts 顯示的名稱，以字串字面常數指定。 
dev_id 引數是在共享 IRQ 時讓 kernel 區分中斷處理程序用的，傳入驅動程式的私有位址即可，如果沒有共享 IRQ時，則設為 NULL。 

驅動程式的中斷處理程序很可能同時並列執行，可以需寫成可以重複進入(reentrant)的形式。 

linux kernel 會依序執行同一個 IRQ 的中斷處理程序，不會同時並列呼叫中斷處理程序。 
另外如果安裝了許多個相同的裝置，而這些裝置又分配到不同的 IRQ 時，呼叫的中斷處理程序仍然是同一份。 

如果是使用 APIC 中斷控制器的多處理器環境下，則中斷處理程序就十分有機會同時並列執行， 

中斷處理程序的範例，這個驅動程式會共享網路介面(eth0) 的 IRQ，接下網路裝置的中斷訊號： 

```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

const int IRQ_NUM = 17;
void *irq_dev_id = (void *)&IRQ_NUM;

irqreturn_t sample_isr(int irq, void *dev_instance)
{
    if (printk_ratelimit()) {
        printk("%s: irq %d dev_instance %p\n", __func__, irq, dev_instance);
    }

    return IRQ_NONE;
}

static int sample_init(void)
{
    int ret;

    printk("sample driver installed.\n");

    ret = request_irq(IRQ_NUM, sample_isr, IRQF_SHARED, "sample", irq_dev_id);
    if (ret) {
        printk("request_irq() failed (%d)\n", ret);
        return (ret);
    }

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");

    free_irq(IRQ_NUM, irq_dev_id);
}

module_init(sample_init);
module_exit(sample_exit);
```

載入驅動程式後，會發現 IRQ 17 最後多了「sample」： 

```c
cat /proc/interrupts 
```

中斷處理程序會在 interrupt context 運作，所以可以運用的 kernel 函式有限制，同時也要極力縮短處理時間。 
中斷處理程序的內容隨裝置而異，大體工作如下：
- (有需要的話)取得 spinlock
- 檢查中斷狀態
- 解決造成中斷的原因
- 處理中斷工作
- 傳回 IRQ_HANDLED


##11-3、Tasklet 
中斷處理工作須極力縮短時間，同時在中斷控制器的 IRQ line 送出訊號期間，無法進一步處高其它中斷，而 CPU 更會優先處理中斷，而沒辦法作其它事。


因此 linux kernel 提供了「tasklet」機制，讓中斷處理程序只需要完成最重要的部分即可，稍後再執行剩下的工作。 
一般用語常為「延遲中斷」或「軟體中斷」，Windows 稱為「DPC(Deferred Procedue Call)」，HP-UX 則稱為「software trigger」，各平台的稱呼都不盡相同。 
使用 tasklet 機制，可以讓中斷處理程序儘快結束，以便中斷控制器接收下一個中斷，提高系統整體效能。 
在中斷處理程序呼叫 tasklet 時，tasklet 並不會立刻開始執行，它會等到系統有空閒時才開始執行。 

驅動程式想使用 tasklet，必須準備好「tasklet 變數(tasklet_struct 結構)」，並填入資料、做好初始化，初始化工作由 tasklet_init() 完成：

```c
void tasklet_init(struct tasklet_struct *t, void (*func) (unsigned long), unsigned long data); 
```

func 引數是要以 tasklet 形式執行的驅動程式自有函式。 
data 引數則是要傳給這個函式的資料。 
驅動程式卸除時，必須以 tasklet_kill() 清掉不需要的 tasklet： 

```c 
void tasklet_kill(struct tasklet_struct *t); 
```

啟動 tasklet 的工作由 tasklet_schedule() 負責，由中斷處理程序負責呼叫： 
```c
void tasklet_schedule(struct tasklet_struct *t); 
```

若要暫時取消 tasklet 或重新啟動的話，可以使用： 

```c
void tasklet_disable(struct tasklet_struct *t); 
void tasklet_enable(struct tasklet_struct *t); 
```

tasklet 的範例程式： 


```c
/*
 * tasklet sample code
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

const int IRQ_NUM = 17;
void *irq_dev_id = (void *)&IRQ_NUM;
struct tasklet_struct tasklet;

void sample_tasklet(unsigned long data)
{
    printk("%s called (%ld, %ld, %ld)\n", __func__,
           in_irq(), in_softirq(), in_interrupt());
}

irqreturn_t sample_isr(int irq, void *dev_instance)
{
    if (printk_ratelimit()) {
        printk("%s: irq %d (%ld, %ld, %ld)\n", __func__, irq,
               in_irq(), in_softirq(), in_interrupt());
        tasklet_schedule(&tasklet);
    }

    return IRQ_NONE;
}

static int sample_init(void)
{
    int ret = 0;

    printk("sample driver installed.\n");

    tasklet_init(&tasklet, sample_tasklet, 0);

    ret = request_irq(IRQ_NUM, sample_isr, IRQF_SHARED, "sample", irq_dev_id);
    if (ret) {
        printk("request_irq() failed (%d)\n", ret);
        tasklet_kill(&tasklet);
        goto out;
    }

out:
    return (ret);
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");

    tasklet_kill(&tasklet);
    free_irq(IRQ_NUM, irq_dev_id);
}

module_init(sample_init);
module_exit(sample_exit);
```

##11-4、Work queue 
tasklet 雖然可以用來延後處理一些中斷情形留下的工作，但 tasklet 還是只能在 interrupt context 中執行。 
因此 linux 還提供了「work queue」機制，允許中斷情形的後續處理工作在 user space 執行。 

work queu 的原理是延後處理工作時，排進 wait queue 的等待隊伍之中，一個 work queue 有一個對應的 kernel daemon，會在適當時機以 kernel daemon 處理這些延後的工作，而 kernel daemon 是一隻在 user context 執行的程式。 

驅動程式要使用 work queu，要先準備「work queu 變數 (work_struct 結構)」並以 INIT_WORK 巨集初始化： 
```c
void INIT_WORK(struct work_struct *work, work_func_t func); 
```

work_func_t 的定義如下： 

```c
typedef void (*work_func_t) (struct work_struct *work); 
```

在 func 引數登記的函式是要延後處理的工作，稍後由 work queue 執行。 

中斷處理程序呼叫 schedule_work() 後，會在適當的時機執行指定的工作、呼叫 INIT_WORK 巨集指定的函式： 

```c
int schedule_work(struct work_struct *work); 
```

這邊使用的 work queue 是「keventd_wq」，負責執行的 kernel daemon 是「events」，「keventd_wq」是泛用的 work queue，許多驅動程式都有用到： 

```sh
ps aux | grep events 
```

卸除驅動程式時，如果 work queue 之內還有工作要處理的話，須以 flush_scheduled_work() 把它們執行完成： 

```c
void flush_scheduled_work(void); 
```

如果要取消 work queue 之內剩餘的工作，可以改用 cancel_work_sync()：
```c
int cancel_work_sync(struct work_struct *work); 
```
使用 work queue 的範例程式： 

```c
/*
 * workqueue sample code
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

const int IRQ_NUM = 17;
void *irq_dev_id = (void *)&IRQ_NUM;
struct work_struct workq;

void sample_workqueue(struct work_struct *work)
{
    printk("%s called (%ld, %ld, %ld) pid %d\n", __func__,
           in_irq(), in_softirq(), in_interrupt(),
           current->pid
          );

    msleep(3000);  /* sleep */
}

irqreturn_t sample_isr(int irq, void *dev_instance)
{
    if (printk_ratelimit()) {
        printk("%s: irq %d (%ld, %ld, %ld)\n", __func__, irq,
               in_irq(), in_softirq(), in_interrupt());

        schedule_work(&workq);
    }

    return IRQ_NONE;
}

static int sample_init(void)
{
    int ret = 0;

    printk("sample driver installed.\n");

    INIT_WORK(&workq, sample_workqueue);

    ret = request_irq(IRQ_NUM, sample_isr, IRQF_SHARED, "sample", irq_dev_id);
    if (ret) {
        printk("request_irq() failed (%d)\n", ret);
        goto out;
    }

out:
    return (ret);
}

static void sample_exit(void)
{
    flush_scheduled_work();
    free_irq(IRQ_NUM, irq_dev_id);

    printk("sample driver removed.\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

使用 work queu 時，函式只會拿到 struct work_struct 的指標，從這個指標要讀寫驅動程式的私有空間需要花點功夫：
- 在驅動程式的 struct 之內儲存 work_struct 變數
- 使用 container_of 巨集



##11-5、結語 
驅動程式控制裝置時，幾乎都必須用到「中斷」的概念，但處理中斷時，有不少限制，在開發驅動程式時須特別留意。 
