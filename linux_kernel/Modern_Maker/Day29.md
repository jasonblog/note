## Day 29：IRQ (Part 3) - 這是核心執行緒的味道！Threaded IRQ



這篇文章以實驗觀察 *threaded IRQ* 與傳統 IRQ 的不同。

關於這一切 IRQ 行為不同，可以參考 2016 年 ELCE 中，*Alison Chaiken* 的演講 [*IRQs: the Hard, the Soft, the Threaded and the Preemptible*](https://youtu.be/-pehAzaP1eg)，裡面介紹了比如 *threaded IRQ* 這類機制與傳統的 IRQ 有什麼不同。除此之外，*interrupt controller* 的驅動程式架構，可以參考也是 2016 年的 ELCE 中，[*How Dealing with Modern Interrupt Architectures can Affect Your Sanity*](https://youtu.be/YE8cRHVIM4E) 演講，聽他聊聊現代的電腦中，中斷相關周邊硬體是如何讓人失去理智的(?)。

實驗方面，今 (2020) 年的 OSS NA 中，`ftrace` 的維護者 *Steven Rostedt* 親自上陣，在 [*Finding Sources of Latency on your Linux System*](https://youtu.be/Tkra8g0gXAU) 中，提到如何用 `ftrace` 追蹤 IRQ，並且量測 IRQ 帶來的延遲 (以及 `ftrace` 相關的最新進展)(14:36 開始的部分)。而他本人也有寫一篇關於這個主題的[文章](https://static.lwn.net/images/conf/rtlws11/papers/proc/p02.pdf)。

## 程式

關於 *threaded IRQ* 的用法，文件可以在 [*kernel/irq/manage.c*](https://elixir.bootlin.com/linux/latest/source/kernel/irq/manage.c#L1961) 的註解中找到。如果就 API 來說的話，*threaded IRQ* 用起來很簡單：湊到 [`request_threaded_irq`](https://elixir.bootlin.com/linux/latest/source/kernel/irq/manage.c#L1961) 需要的參數：

```c
int request_threaded_irq(unsigned int irq, 
    irq_handler_t handler,
    irq_handler_t thread_fn, 
    unsigned long irqflags,
    const char *devname, void *dev_id)
```

這主要的參數大致上跟 `request_irq` 一樣，除了現在要傳進去兩個 `irq_handler_t` 以外。這兩個分別是：

1. `handler`：上半部的函式，會在 *atomic context* 下執行。
2. `thread_fn`：下半部 ，會在 *process context* 下執行。

提供完之後，用這個函數註冊他們。

這兩個函式不總是兩個都要：如果你覺得上半部就可以處理完，那麼下半部的函式可以傳進 `NULL`; 類似地，如果覺得不需要上半部，那麼上半部也可以傳進 `NULL`。只要兩個當中其中一個有回傳 `IRQ_HANDLED` 就好。只要提供兩個函數，不用再煩惱是不是要 *Workqueue* 設計新的資料結構等等。

這一系列函數的基本款是 `request_threaded_irq`，但也有其他的變形，比如說這邊使用到的 `devm_request_threaded_irq` 。

### Step 1：實作上半部

這個函數是上半部的函數，必須在 *atomic context* 中執行。如果 IRQ 不需要下半部，那麼要在這裡回傳 `IRQ_HANDLED`; 但如果有任務要給下半部處理，則要回傳 `IRQ_WAKE_THREAD`。這邊為了展示使用方法，所以就什麼都不做直接回傳 `IRQ_WAKE_THREAD`，把任務丟給下半部。

```c
static irqreturn_t minirq_top_half(int irq, void *p)
{
    return IRQ_WAKE_THREAD;
}
```

> 事實上可以在 `request_threaded_irq` 系列的函數中，把上半部函數的位置填進 `NULL` 來做到一樣的事情，而不用多此一舉寫一個「只會回傳 `IRQ_WAKE_THREAD`」的函數。但這邊為了展示使用方法，所以故意多此一舉。

### Step 2：實作下半部

也就是參數中的 `thread_fn`，會在 *process context* 下執行。這邊做的事情跟昨天一樣，就是印一個通知：

```c
static irqreturn_t minirq_bottom_half(int irq, void *p)
{
    struct minirq_dev *minirq = p;
    dev_info(minirq->dev, "Rising edge detected!\n");
    return IRQ_HANDLED;
}
```

### Step 3：request_threaded_irq

最後在 `probe` 中註冊：

```c
static int minirq_probe(struct platform_device *pdev)
{
    ...
    ret = devm_request_threaded_irq(dev, minirq->irq, 
        minirq_top_half, minirq_bottom_half, 
        IRQF_TRIGGER_RISING, "minirq", minirq);
    ...
}
```

## 實驗方法

硬體的配置跟昨天一樣：Raspberry Pi 的 `GPIO17` 透過 *Logic Level Shifter* 接到 Arduino 的 `A0`。而程式方面也跟昨天一樣。為了避免 *interrupt* 太多，建議可以是當調整時間間隔。而 Arduino 使用的程式如下：

```c
void setup() {
    pinMode(A0, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    digitalWrite(A0, LOW);
    delay(500);
    digitalWrite(A0, LOW);
    delay(500);
}
```

## 觀察工具

這邊使用兩個工具來觀察：`ftrace` 跟 `ps`。

### ps -aux

使用 `ps -aux` 的目的是看看有沒有新的 *kernel thread* 產生。在實驗開始之前，看起來是這個樣子：

```shell
$ ps -aux | grep irq
root         9  0.0  0.0      0     0 ?        S    09:29   0:00 [ksoftirqd/0]
root        15  0.0  0.0      0     0 ?        S    09:29   0:00 [ksoftirqd/1]
root        20  0.0  0.0      0     0 ?        S    09:29   0:00 [ksoftirqd/2]
root        25  0.0  0.0      0     0 ?        S    09:29   0:01 [ksoftirqd/3]
pi        2642  0.0  0.0   7348   516 pts/0    S+   10:29   0:00 grep --color=auto irq
```

### ftrace

`ftrace` 目的是哪個執行單元執行了 `minirq_*` 這些函數，這樣就可以知道處理這個 IRQ 時的路徑是什麼。而不管是 *threaded IRQ* 或是傳統的 IRQ，這邊使用的指令都一樣：用 `function` 這個 *tracer*，並且用 `-func-stack` 選項令他印出 *stack trace*：

```shell
$ sudo trace-cmd record \
    -p function \
    -T \
    --func-stack \
    -l minirq_*
```

## 實驗：ps -aux

### 觀察：ps -aux (傳統 IRQ)

如果沒有用 *threaded IRQ*，模組安裝上去之後，`ps -aux` 的結果還是一樣：

```shell
$ sudo insmod minirq.ko 
$ ps -aux | grep irq
 root         9  0.0  0.0      0     0 ?        S    09:29   0:00 [ksoftirqd/0]
 root        15  0.0  0.0      0     0 ?        S    09:29   0:00 [ksoftirqd/1]
 root        20  0.0  0.0      0     0 ?        S    09:29   0:00 [ksoftirqd/2]
 root        25  0.0  0.0      0     0 ?        S    09:29   0:01 [ksoftirqd/3]
 pi        2697  0.0  0.0   7348   492 pts/0    S+   10:29   0:00 grep --color=auto irq

(USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND)
```

### 觀察：ps -aux (Threaded IRQ)

而如果是 *threaded IRQ* 的版本，如果安裝上模組之後，立刻執行，就會發現在差不多時間多出了一個名為 `irq/167-minirq` 的核心執行緒：

```shell
$ sudo insmod minirq.ko 
$ ps -aux | grep irq
 root         9  0.0  0.0      0     0 ?        S    14:40   0:00 [ksoftirqd/0]
 root        15  0.0  0.0      0     0 ?        S    14:40   0:00 [ksoftirqd/1]
 root        20  0.0  0.0      0     0 ?        S    14:40   0:00 [ksoftirqd/2]
 root        25  0.0  0.0      0     0 ?        S    14:40   0:00 [ksoftirqd/3]
+root      2851  0.0  0.0      0     0 ?        S    16:40   0:00 [irq/167-minirq]
 pi        2922  0.0  0.0   7348   492 pts/0    S+   16:41   0:00 grep --color=auto irq
 
(USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND)
```

## 實驗：ftrace (上半部)

接著用 `ftrace` 去看看上半部，也就是 `minirq_top_half` 這個函數。為了幫變識別，昨天的模組中，IRQ Handler 的名稱從 `irq_top_half` 也改成 `minira_top_half`。改動過的程式碼也會附在最後面。

### 觀察：呼叫堆疊 (傳統 IRQ)

```javascript
<idle>-0     [000]  7169.646594: function:             minirq_top_half
<idle>-0     [000]  7169.646610: kernel_stack:         <stack trace>
=> ftrace_graph_call (801111c4)
=> __handle_irq_event_percpu (80186c78)
=> handle_irq_event_percpu (80186e90)
=> handle_irq_event (80186f38)
=> handle_edge_irq (8018bbc4)
=> generic_handle_irq (80185c64)
=> bcm2835_gpio_irq_handle_bank (8059f6f4)
=> bcm2835_gpio_irq_handler (8059f7a0)
=> generic_handle_irq (80185c64)
=> bcm2836_chained_handle_irq (80597aac)
=> generic_handle_irq (80185c64)
=> __handle_domain_irq (801863c4)
=> bcm2836_arm_irqchip_handle_irq (80102220)
=> __irq_svc (80101a3c)
=> arch_cpu_idle (80109b18)
=> default_idle_call (808f09a0)
=> do_idle (80158d8c)
=> cpu_startup_entry (801590a8)
=> rest_init (808e9d08)
=> arch_call_rest_init (80c00b68)
=> start_kernel (80c010a0)
=> 0
```

不管是傳統的 IRQ，還是 *threaded IRQ*，這兩者上半部的呼叫堆疊大致相同：

### 觀察：呼叫堆疊 (Threaded IRQ)

```javascript
<idle>-0     [000]  7222.508310: function:             minirq_top_half
<idle>-0     [000]  7222.508328: kernel_stack:         <stack trace>
=> ftrace_graph_call (801111c4)
=> __handle_irq_event_percpu (80186c78)
=> handle_irq_event_percpu (80186e90)
=> handle_irq_event (80186f38)
=> handle_edge_irq (8018bbc4)
=> generic_handle_irq (80185c64)
=> bcm2835_gpio_irq_handle_bank (8059f6f4)
=> bcm2835_gpio_irq_handler (8059f7a0)
=> generic_handle_irq (80185c64)
=> bcm2836_chained_handle_irq (80597aac)
=> generic_handle_irq (80185c64)
=> __handle_domain_irq (801863c4)
=> bcm2836_arm_irqchip_handle_irq (80102220)
=> __irq_svc (80101a3c)
=> arch_cpu_idle (80109b18)
=> default_idle_call (808f09a0)
=> do_idle (80158d8c)
=> cpu_startup_entry (801590a8)
=> rest_init (808e9d08)
=> arch_call_rest_init (80c00b68)
=> start_kernel (80c010a0)
=> 0
```

其實也不意外。雖然發展脈絡來說，是先有傳統的 IRQ，再有 *threaded IRQ*。但實際上不管是 `devm_request_irq` ，還是 `request_irq` ，他們其實都是用 *threaded IRQ* 的版本去實作的。以 [`request_irq`](https://elixir.bootlin.com/linux/latest/source/include/linux/interrupt.h#L156) 為例：

```c
static inline int __must_check
request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
	    const char *name, void *dev)
{
	return request_threaded_irq(irq, handler, NULL, flags, name, dev);
}
```

所以會有一樣的呼叫路徑其實一點也不意外。

## 實驗：ftrace (下半部)

下半部就會有明顯的差別。在原先的 IRQ 中，是在上半部 (也就是 `minirq_top_half` 函數中)，把一個 `struct work_struct` 排進全域的 *Workqueue* 中，所以執行下半部的會是處理 *Workqueue* 的執行緒; 而 *threaded IRQ* 的版本中，則是由前面 `ps -aux` 所看到的，新的 `irq/167-minirq` 執行緒專門去執行。

### 觀察：下半部呼叫堆疊 (傳統 IRQ)

可以發現是 `worker_thread` 在做事情。看起來也很合理，因為這就是在處理 *Workqueue* 的那個行程：

```javascript
kworker/0:3-2704  [000]  7169.646820: function:             minirq_bottom_half
kworker/0:3-2704  [000]  7169.646826: kernel_stack:         <stack trace>
=> ftrace_graph_call (801111c4)
=> process_one_work (801413bc)
=> worker_thread (8014173c)
=> kthread (80148aac)
=> ret_from_fork (801010ac)
=> 0
```

### 觀察：下半部呼叫堆疊 (Threaded IRQ)

這時可以發現：執行下半部的就是專門的 *kernel thread*，也就是剛剛在 `ps` 時所觀察到的，新的執行緒：

```javascript
irq/167-minirq-2851  [003]  7222.508489: function:             minirq_bottom_half
irq/167-minirq-2851  [003]  7222.508495: kernel_stack:         <stack trace>
=> ftrace_graph_call (801111c4)
=> irq_thread_fn (801881fc)
=> irq_thread (80187fdc)
=> kthread (80148aac)
=> ret_from_fork (801010ac)
=> 0
```

順帶一提，如果去使用其他 `ftrace` 的選項，比如：

```shell
$ sudo trace-cmd record -p function_graph \
    -e irq_handler_entry
```

會發現這個執行緒跟原先的執行緒交錯出現的狀況：

```c
...
  irq/167-minirq-1179  [000]   283.217546: funcgraph_entry:                   |                  __up_console_sem.constprop.17() {
       trace-cmd-1193  [003]   283.217547: funcgraph_entry:                   |                            rcu_all_qs() {
  irq/167-minirq-1179  [000]   283.217547: funcgraph_entry:        0.937 us   |                    __printk_safe_enter();
       trace-cmd-1193  [003]   283.217548: funcgraph_exit:         1.718 us   |                            }
  irq/167-minirq-1179  [000]   283.217549: funcgraph_entry:                   |                    up() {
       trace-cmd-1193  [003]   283.217550: funcgraph_exit:         5.208 us   |                          }
  irq/167-minirq-1179  [000]   283.217550: funcgraph_entry:        1.145 us   |                      _raw_spin_lock_irqsave();
       trace-cmd-1193  [003]   283.217552: funcgraph_entry:                   |                          should_failslab() {
  irq/167-minirq-1179  [000]   283.217552: funcgraph_entry:        0.989 us   |                      _raw_spin_unlock_irqrestore();
       trace-cmd-1193  [003]   283.217554: funcgraph_exit:         1.823 us   |                          }
  irq/167-minirq-1179  [000]   283.217554: funcgraph_exit:         5.104 us   |                    }
  irq/167-minirq-1179  [000]   283.217555: funcgraph_entry:                   |                    __printk_safe_exit() {
       trace-cmd-1193  [003]   283.217555: funcgraph_exit:       + 12.187 us  |                        }
  irq/167-minirq-1179  [000]   283.217556: funcgraph_exit:         0.937 us   |                    }
  irq/167-minirq-1179  [000]   283.217557: funcgraph_exit:       + 11.979 us  |                  }
       trace-cmd-1193  [003]   283.217557: funcgraph_entry:                   |                        start_this_handle() {
  irq/167-minirq-1179  [000]   283.217558: funcgraph_entry:                   |                  _raw_spin_lock() {
...
```

> 如果是使用這個選項，僅僅數秒就會產生大量資料 (約 100 MB 左右)，也會帶來大幅度的延遲。所以請謹慎實驗。

## 幕後花絮

### 如果使用 ply 的話

```c
kprobe:minirq_top_half
{
    @minirq_top_half[stack, arg0] = count();
}
kprobe:minirq_bottom_half
{
    @minitq_bottom_half[stack, arg0] = count();
}
```

會發現有不同的路徑：

```diff
@minirq_top_half:
{ 
+	snd_info_done+45544
+	handle_irq_event_percpu+60
+	handle_irq_event+84
+	handle_edge_irq+204
+	generic_handle_irq+52
+	bcm2835_gpio_irq_handle_bank+148
+	bcm2835_gpio_irq_handler+116
+	generic_handle_irq+52
+	bcm2836_chained_handle_irq+72
+	generic_handle_irq+52
+	__handle_domain_irq+108
+	bcm2836_arm_irqchip_handle_irq+96
	__irq_usr+76
, 167 }: 1
{ 
+	snd_info_done+45544
+	handle_irq_event_percpu+60
+	handle_irq_event+84
+	handle_edge_irq+204
+	generic_handle_irq+52
+	bcm2835_gpio_irq_handle_bank+148
+	bcm2835_gpio_irq_handler+116
+	generic_handle_irq+52
+	bcm2836_chained_handle_irq+72
+	generic_handle_irq+52
+	__handle_domain_irq+108
+	bcm2836_arm_irqchip_handle_irq+96
	__irq_svc+92
	default_idle_call+64
	do_idle+292
	cpu_startup_entry+40
	rest_init+180
	parse_early_param+128
	start_kernel+1212
, 167 }: 69
```

從 ARM 的[文件](https://developer.arm.com/documentation/den0013/d/Exception-Handling/Linux-exception-program-flow/Interrupt-dispatch) 可以知道，這是處理不同的 *interrupt* 進入點。

### 暴走的 ftrace

一開始我是想使用 `irq_handler_exit` 這個選項：

```shell
$ sudo trace-cmd list -e | grep irq
 irq:softirq_raise
 irq:softirq_exit
 irq:softirq_entry
 irq:irq_handler_exit
 irq:irq_handler_entry
 preemptirq:irq_enable
 preemptirq:irq_disable
 rtc:rtc_alarm_irq_enable
 rtc:rtc_irq_set_state
 rtc:rtc_irq_set_freq
```

所以本來是用像這樣的指令：

```shell
$ sudo trace-cmd record -p function_graph \
    -e irq_handler_entry
```

不過發現僅僅數秒就會跑出將近 100 MB 的資料，而且非常混亂。所以就決定換成僅用 `function` 這個 *tracer* 了。

## 附錄：完整程式

裝置樹跟昨天的實驗一樣，Makefile 亦同。

### 傳統 IRQ

```c
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct minirq_dev {
    struct gpio_desc *gpiod;
    struct work_struct work;
    int irq;
    struct device *dev;
};

void minirq_bottom_half(struct work_struct *work)
{
    struct minirq_dev *minirq;
    minirq = container_of(work, struct minirq_dev, work);
    dev_info(minirq->dev, "Rising edge detected!\n");
    return;
}

static irqreturn_t minirq_top_half(int irq, void *p)
{
    struct platform_device *pdev = p;
    struct minirq_dev *minirq;

    minirq = platform_get_drvdata(pdev);
    schedule_work(&(minirq -> work));
    return IRQ_HANDLED;
}

static int minirq_probe(struct platform_device *pdev)
{
    struct device *dev = &(pdev-> dev);
    struct minirq_dev *minirq;
    int ret = 0;

    minirq = devm_kzalloc(dev, sizeof(struct minirq_dev), GFP_KERNEL);
    if (!minirq) {
        dev_err(dev, "Failed to allocate memory.\n");
	    return -ENOMEM;
    }
    platform_set_drvdata(pdev, minirq);
    minirq->dev = dev;

    minirq->gpiod = devm_gpiod_get_index(dev, "minirq", 0, GPIOD_IN);
    if (IS_ERR(minirq->gpiod)) {
        dev_err(dev, "Failed to get gpio descriptor.\n");
        return PTR_ERR(minirq -> gpiod);
    }
    
    ret = gpiod_to_irq(minirq->gpiod);
    if (ret < 0) {
        dev_err(dev, "Failed to get irq from gpiod.\n");
        return ret;
    }
    minirq->irq = ret;

    INIT_WORK(&minirq->work, minirq_bottom_half);

    ret = devm_request_irq(dev, minirq->irq, minirq_top_half, 
            IRQF_TRIGGER_RISING, "minirq", pdev);
    if (ret < 0) {
        dev_err(dev, "Failed to request IRQ.\n");
        return ret;
    }

    return 0;
}

static const struct of_device_id minirq_ids[] = {
    {.compatible = "minirq",},
    {}
};

static struct platform_driver minirq_driver = {
    .driver = {
        .name = "minirq",
	    .of_match_table = minirq_ids,
    },
    .probe = minirq_probe
};
MODULE_LICENSE("GPL");
module_platform_driver(minirq_driver);
```

### Threaded IRQ

```c
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

struct minirq_dev {
    struct gpio_desc *gpiod;
    struct work_struct work;
    int irq;
    struct device *dev;
};


static irqreturn_t minirq_top_half(int irq, void *p)
{
    return IRQ_WAKE_THREAD;
}

static irqreturn_t minirq_bottom_half(int irq, void *p)
{
    struct minirq_dev *minirq = p;
    dev_info(minirq->dev, "Rising edge detected!\n");
    return IRQ_HANDLED;
}

static int minirq_probe(struct platform_device *pdev)
{
    struct device *dev = &(pdev-> dev);
    struct minirq_dev *minirq;
    int ret = 0;

    minirq = devm_kzalloc(dev, sizeof(struct minirq_dev), GFP_KERNEL);
    if (!minirq) {
        dev_err(dev, "Failed to allocate memory.\n");
	    return -ENOMEM;
    }
    minirq->dev = dev;

    minirq->gpiod = devm_gpiod_get_index(dev, "minirq", 0, GPIOD_IN);
    if (IS_ERR(minirq->gpiod)) {
        dev_err(dev, "Failed to get gpio descriptor.\n");
        return PTR_ERR(minirq -> gpiod);
    }
    
    ret = gpiod_to_irq(minirq->gpiod);
    if (ret < 0) {
        dev_err(dev, "Failed to get irq from gpiod.\n");
        return ret;
    }
    minirq->irq = ret;

    ret = devm_request_threaded_irq(dev, minirq->irq, 
            minirq_top_half, minirq_bottom_half, 
            IRQF_TRIGGER_RISING, "minirq", minirq);
    if (ret < 0) {
        dev_err(dev, "Failed to request IRQ.\n");
        return ret;
    }

    return 0;
}

static const struct of_device_id minirq_ids[] = {
    {.compatible = "minirq",},
    {}
};

static struct platform_driver minirq_driver = {
    .driver = {
        .name = "minirq",
	    .of_match_table = minirq_ids,
    },
    .probe = minirq_probe
};
MODULE_LICENSE("GPL");
module_platform_driver(minirq_driver);
```