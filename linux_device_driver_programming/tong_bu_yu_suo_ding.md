# 同步與鎖定

##十、同步與鎖定 
前一章的計時器有「錯過」的問題，但被這個問題困擾的不只計時器，本章要繼續介紹「同步與鎖定」。 



###10-1、同步與鎖定的必要性 
驅動程式要能夠強固的執行，則必須考慮到「同步(synchronization)」與「鎖定(exclusion)」的相關知識，另一種說法為「競爭狀態(race condition; racing)」，指的是相同的東西。 

驅動程式分成 user context 與 interrupt context ，而這兩種 context 十分可能同時並列執行，因此在開發驅動程式時，必須隨時記得「現在寫的程式碼，必須能毫無問題地同時並列執行」。 



###10-2、read-modify-write 
要理解為何需要鎖定，常見的描述是「read-modify-write」，這指的是三個連續動作：
```
讀取資料
變更資料
寫入資料
```

如果這段動作有兩個 thread 同時執行的話，則難保執行結果會是正確的。 



###10-3、Atomic 操作 
要讓遞增、遞減變動的動作「atomic」時，可以使用 kernel 提供的機制： 
將變數定義為 atomic_t 型別，並使用 ATOMIC_INIT 巨集為 atomic 變數設定初始值，如： 
```c
atomic_t counter = ATOMIC_INIT(0); 
```

操作 atomic 變數的函式，在 IA-32 版本是定義在「include/asm-x86/atomic_32.h」檔案內。 

Atomic 操作的範例： 
```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

atomic_t counter = ATOMIC_INIT(0);

static int sample_init(void)
{
    int n;

    atomic_inc(&counter);
    n = atomic_read(&counter);

    printk("n %d\n", n);

    if (atomic_dec_and_test(&counter)) {
        printk("atomic_dec_and_test() TRUE\n");
    } else {
        printk("atomic_dec_and_test() FALSE\n");
    }

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");
}

module_init(sample_init);
module_exit(sample_exit);
```


###10-4、旗標 
程式處理的過程中，有一些不適合同時並行執行的地方稱為「critical section」，要保護 critical section 的話，必須：
- 一次只讓一個 thread 執行 critical section
也因為這個作法，可能導致系統的效能降低，所以有另一個鐵則：
- critical section 的程式碼極力縮短

linux kernel 提供了「semaphore(旗標)」作為 user context 進行同步與鎖定的機制，用法只要在 critical section 前、後，執行「P操作」、「V操作」即可。 
執行「P操作」，就是取得進入 critical section 的許可，並拿到 semaphore，此時若有其它 context 要求「P操作」，就會進入等待狀態。 
離開 critical section 時，作做「V操作」，釋放 semaphore，此時如果有其它 context 正在等待，就會喚醒其中一個，讓它獲得 semaphore。 
也因為 semaphore 會「sleep」的緣故，所以：
- 不能在 interrupt context 使用 semaphore

驅動程式要使用 semaphore 時，必須先定義 semaphore 結構變數： 
```c
struct semaphore sem; 
```

要初始化 semaphore 變數，要呼叫 init_MUTEX()： 
```c
void init_MUTEX(struct semaphore *sem); 
```

取得 semaphore 的動作(P操作)可透過 down() 或 down_interruptible() 完成： 
```c
void down(struct semaphore *sem); 
int down_interruptible(struct semaphore *sem); 
```

釋放 semaphore 的動作(V操作)則是 up()： 
```c
void up(struct semaphore *sem); 
```
取得 semaphore 的函式有兩種，差異在於 user context(process) 進入 sleep 後：
- 能不能被 signal 解除 sleep

關於 down() 與 down_interruptible() 的選用方式，大多數情況下可直接使用 down()，力求讓驅動程式簡化。 
而使用 down_interruptible() 的情況包括 socket interface 收封包資料等情況，在收到封包前都無法處理系統呼叫的話很麻煩，所以會希望能透過 signal 中斷等待。 

使用 semaphore 的範例： 

```c
/*
 * devone.c
 *
 * Update: 2008/03/03 Yutaka Hirata
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

static int devone_devs = 1;        /* device count */
static int devone_major = 0;       /* MAJOR: dynamic allocation */
static int devone_minor = 0;       /* MINOR: static allocation */
static struct cdev devone_cdev;
static struct class *devone_class = NULL;
static dev_t devone_dev;

struct devone_data {
    struct semaphore sem;
};

#define DATA_MAX 64
static unsigned char data[DATA_MAX];


ssize_t devone_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    struct devone_data *dev = filp->private_data;
    int len;
    int retval;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (count > DATA_MAX)
        len = DATA_MAX;
    else
        len = count;

    if (copy_from_user(data, buf, len)) {
        retval = -EFAULT;
        goto out;
    }
    retval = len;

out:
    up(&dev->sem);

    return (retval);
}

ssize_t devone_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct devone_data *dev = filp->private_data;
    int len;
    int retval;

    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (count > DATA_MAX)
        len = DATA_MAX;
    else
        len = count;

    if (copy_to_user(buf, data, len)) {
        retval = -EFAULT;
        goto out;
    }
    retval = len;

out:
    up(&dev->sem);

    return (retval);
}

int devone_close(struct inode *inode, struct file *filp)
{
    struct devone_data *dev = filp->private_data;

    if (dev) {
        kfree(dev);
    }

    return 0;   /* success */
}

int devone_open(struct inode *inode, struct file *filp)
{
    struct devone_data *dev;

    dev = kmalloc(sizeof(struct devone_data), GFP_KERNEL);
    if (dev == NULL) {
        return -ENOMEM;
    }

    /* initialize members */
    init_MUTEX(&dev->sem);

    filp->private_data = dev;

    return 0;   /* success */
}

struct file_operations devone_fops = {
    .owner = THIS_MODULE,
    .open = devone_open,
    .release = devone_close,
    .read = devone_read,
    .write = devone_write,
};

static int devone_init(void)
{
    dev_t dev = MKDEV(devone_major, 0);
    int alloc_ret = 0;
    int major;
    int cdev_err = 0;
    struct class_device *class_dev = NULL;

    alloc_ret = alloc_chrdev_region(&dev, 0, devone_devs, "devone");
    if (alloc_ret)
        goto error;
    devone_major = major = MAJOR(dev);

    cdev_init(&devone_cdev, &devone_fops);
    devone_cdev.owner = THIS_MODULE;
    devone_cdev.ops = &devone_fops;
    cdev_err = cdev_add(&devone_cdev, MKDEV(devone_major, devone_minor), 1);
    if (cdev_err)
        goto error;

    /* register class */
    devone_class = class_create(THIS_MODULE, "devone");
    if (IS_ERR(devone_class)) {
        goto error;
    }
    devone_dev = MKDEV(devone_major, devone_minor);
    class_dev = class_device_create(
                    devone_class,
                    NULL,
                    devone_dev,
                    NULL,
                    "devone%d",
                    devone_minor);

    printk(KERN_ALERT "devone driver(major %d) installed.\n", major);

    return 0;

error:
    if (cdev_err == 0)
        cdev_del(&devone_cdev);

    if (alloc_ret == 0)
        unregister_chrdev_region(dev, devone_devs);

    return -1;
}

static void devone_exit(void)
{
    dev_t dev = MKDEV(devone_major, 0);

    /* unregister class */
    class_device_destroy(devone_class, devone_dev);
    class_destroy(devone_class);

    cdev_del(&devone_cdev);
    unregister_chrdev_region(dev, devone_devs);

    printk(KERN_ALERT "devone driver removed.\n");

}

module_init(devone_init);
module_exit(devone_exit);
```

對 linux kernel 來說，locked 的 semaphore 有些問題，所以通常使用「等待完成(completion)」機制。 

###10-5、等待完成 
要等待某件處理中的工作完成，可以使用「等待完成(completion)」機制，只要先準備好 completion 變數，再呼叫要等待的函式，之前此函式處理結束後，會自動呼叫通報完成的函式，並返回等待的函式，也由於過程中會 sleep，所以無法在 interrupt context 中使用。 
相關函式定義在 linux/completion.h 檔案內。 

init_completion() 負責初始化「completion變數」： 
```c
void init_completion(struct completion *x); 
```

呼叫 wait_for_completion() 之後，會讓 user context(process) 進入 sleep，在等待對象完成前都不會醒來： 

```c
void fastcall __wait_for_completion(struct completion *x); 
```

其它的變化還包括可以用 signal 喚醒的 wait_for_completion_interruptible() 及可指定等待時限的 wait_for_completion_timeout() 等等： 

```c
int fastcall __wait_for_completion_interruptible(struct completion *x); 
unsigned long fastcall __wait_for_completion_interruptible(struct completion *x, unsigned long timeout); 
```
用來喚醒等待者的是 complete()，就算錯過時機，在 wait_for_completion() 進入 sleep 之前呼叫了 complete() 也不會造成鎖死，因為內部有用 spin lock 保護 sleep 動作： 
```c
void fastcall complete(struct completion *x); 
```

另外也有通報完同，同時終止 kernel thread 的 complete_and exit() 函式：
```c
void fastcall complete_and_exit(struct completion *x, long code); 
```
Completion 的範例程式，它會在載入驅動程式時，建立 kernel thread，並在卸除驅動程式時，會先結束 thread 的執行： 

```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/completion.h>

MODULE_LICENSE("Dual BSD/GPL");

static wait_queue_head_t wait;
static struct completion comp;
static pid_t thr_pid;

static int sample_thread(void *num)
{
    printk("%s called\n", __func__);

    daemonize("sample_thread");
    allow_signal(SIGTERM);

    for (;;) {
        sleep_on_timeout(&wait, 3 * HZ);

        if (signal_pending(current))
            break;
    }

    printk("%s leaved\n", __func__);

    complete_and_exit(&comp, 0);

    return 0;
}

static int sample_init(void)
{
    printk("sample driver installed.\n");

    init_completion(&comp);
    init_waitqueue_head(&wait);

    thr_pid = kernel_thread(sample_thread, NULL, CLONE_FS | CLONE_FILES);
    if (thr_pid < 0) {
        return -EINVAL;
    }
    printk("kernel_thread %d PID\n", thr_pid);

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");

    kill_proc(thr_pid, SIGTERM, 1);
    wait_for_completion(&comp);
}

module_init(sample_init);
module_exit(sample_exit);
```

載入此驅動程式後，可下 ps 來看 sample_thread 已被執行：

```sh
ps aux | grep sample_thread 
```

在卸除驅動程式時，會先向 kernel thread 送出 SIGTERM，並以 wait_for_completion() 等待 thread 結束，而 kernel thread 會在脫離無窮迴圈後，會以 complete_and_exit() 通知完成。 

###10-6、Kernel Thread 
上述範例使用 kernel_thread() 函式來建立 kernel thread，但現在的 Linux kernel 推薦的是使用 kthread_create() 函式，需 include linux/kthread.h： 

```c
struct task_struct *kthread_create(int (*threadfn) (void *data), void *data, const char namefmt[], ...);
```

與 kernel_thread() 不同的地方是 kthread_create() 在呼叫之後，只是建立 thread，並不會開始執行。 
namefmt[] 引數是 kernel daemon 的名稱字串(最多16 bytes)，之後可透過 wake_up_process() 來啟動 kernel thread： 

```c
int fastcall wake_up_process(struct task_struct *p); 
```

結束 kernel thread 時，是呼叫 kthread_stop() 函式： 

```c
int kthread_stop(struct task_struct *k) 
```

在呼叫 kthread_stop() 之後，kernel thread 如果再呼叫 kthread_should_stop() 函式，就會得到「true」值，此時即可讓 kernel thread 結束： 
```c
int kthread_should_stop(void) 
```

而 kthread_stop() 內部會自動透過 completion 等待 thread 結束。 

kthread_create() 範例： 

```c
/*
 * main.c
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

static struct task_struct *kmain_task = NULL;
static wait_queue_head_t wait;

static int sample_thread(void *num)
{
    printk("%s called\n", __func__);

    for (;;) {
        sleep_on_timeout(&wait, 3 * HZ);

        if (kthread_should_stop())
            break;
    }

    printk("%s leaved\n", __func__);

    return 0;
}

static int sample_init(void)
{
    printk("sample driver installed.\n");

    init_waitqueue_head(&wait);

    kmain_task = kthread_create(sample_thread, NULL, "sample_kthread");
    if (IS_ERR(kmain_task)) {
        return PTR_ERR(kmain_task);
    }
    wake_up_process(kmain_task);

    return 0;
}

static void sample_exit(void)
{
    printk("sample driver removed.\n");

    kthread_stop(kmain_task);
}

module_init(sample_init);
module_exit(sample_exit);
```
###10-7、禁止中斷 
如果在 user context 進入 critical section 時，有 interrupt 進來的話，就會切到驅動程式的處理函式，而如果 interrupt 處理函式會進入到相同的 critical section 時，就必須作好鎖定的工作，否則將會造成 deadlock，因為此時 critical section 已被 user context 佔住，所以 interrupt 處理函式無法進去，又因為 interrupt 為最優先，所以 user context 也無法退出 critical section。 

解決的方法之一是在 user context 進入 critical section 之前暫時「禁止中斷」，直到脫離 critical section。 
但是這是假定只有一顆 CPU 的情形下，禁止、許可中斷的函式如下： 
```c
void local_irq_save(unsigned long flags); 
void local_irq_restore(unsigned long flags); 
```

如果系統是多 CPU 的話，則單單禁止中斷是不夠的，因為 critical section 還是有可能被 interrupt context 優先使用。 



###10-8、自旋鎖

要保護 interrupt context 之內的 critical section 時，不能用 semaphore，而是要使用「spinlock(自旋鎖)」，semaphore 在等待的時候會 sleep，而 spinlock 則是對在原地 busy-loop，也就是會佔住一顆 CPU，直到等到為止，因此 spinlock 在使用時有個鐵則：
極力縮短 critical section 的處理時間

使用 spinlock 時，如果有 critical section 同時出現在 interrupt context 與 user context 的話，則兩邊都必須使用 spinlock，若是只有在 user context 出現的 critical section 則應該用 semaphore 來鎖定。 

###10-8.1、限制 
在使用 spinlock 時，擁有鎖定時可以做的事是有限制的：
- 極力縮短 critical section 的處理時間
- 擁有鎖定時不能 sleep
因為在使用 spinlock 時，鎖定只能自己解除，所以如果 sleep 的話，會造成死結。 

在獲得鎖定時能不能呼叫某些 kernel 函式，需看 kernel 函式原始碼的註解判斷，如：
- printk() 的註解：This is printk(). It can be called from any context. ...
- copy_to_user() 的註解： User context only. This function may sleep. ...

###10-8.2、spinlock 相關函式 
使用 spinlock 時，要在全域空間配置 spinlock 變數，變數型別是 spinlock_t： 

```c
spinlock_t lock; 
```

初始化 spinlock 變數時是使用 spin_lock_init() 巨集： 
```c
spin_lock_init(spinlock_t *lock); 
```

要取得 spinlock 多半只要呼叫 spin_lock_irqsave() ： 
```c
spin_lock_irqsave(spinlock_t *lock, unsigned long flags); 
spin_lock_irqsave() 會將呼叫的程式碼所在的處理器中斷設定存到 flags 
```

變數中，禁止處理中斷並取得鎖定。 
解鎖時，則是呼叫 spin_unlock_irqrestore()： 
```c
spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags); 
```

spin_unlock_irqrestore() 會解除鎖定，並復原本地處理器的中斷設定。 

儘管 spin_lock_irqsave() 與 spin_unlock_irqrestore() 只要成對出現即可，但如果需要放在不同函式的話，則可能代表著驅動程式的設計有問題。 



###10-8.3、死結問題 
如果使用 spin_lock() 取代 spin_lock_irqsave() 的話，可能發生什麼樣的問題？ 

下例是使用 spinlock 的範例，在 user context 的 read 處理函式與 interrupt context 的 timer 處理函式讀寫 sample_data[] 時，會呼叫 spin_lock() 來鎖定： 


```c
/*
 * devone.c
 *
 * Update: 2008/03/06 Yutaka Hirata
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

static int devone_devs = 1;        /* device count */
static int devone_major = 0;       /* MAJOR: dynamic allocation */
static int devone_minor = 0;       /* MINOR: static allocation */
static struct cdev devone_cdev;
static struct class *devone_class = NULL;
static dev_t devone_dev;

struct devone_data {
    spinlock_t lock;
    struct timer_list tickfn;
};

#define TIMEOUT_VALUE (5*HZ)
#define DATA_MAX 64
static unsigned char sample_data[DATA_MAX];

static void sample_timeout(unsigned long arg)
{
    struct devone_data *dev = (struct devone_data *)arg;
    int i;

    printk("%s entered\n",  __func__);

    spin_lock(&dev->lock);
    for (i = 0 ; i < DATA_MAX ; i++) {
        sample_data[i]++;
    }
    spin_unlock(&dev->lock);

    mod_timer(&dev->tickfn, jiffies + TIMEOUT_VALUE);
}


ssize_t devone_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t devone_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct devone_data *dev = filp->private_data;
    int i, len;
    int retval;
    unsigned char tmpbuf[DATA_MAX];

    /* Copying the global data to the temporary buffer while holding a spinlock. */
    spin_lock(&dev->lock);
    for (i = 0 ; i < DATA_MAX ; i++) {
        tmpbuf[i] = sample_data[i];
    }
    mdelay(3000);
    spin_unlock(&dev->lock);

    if (count > DATA_MAX)
        len = DATA_MAX;
    else
        len = count;

    if (copy_to_user(buf, tmpbuf, len)) {
        retval = -EFAULT;
        goto out;
    }
    retval = len;

out:

    return (retval);
}

int devone_close(struct inode *inode, struct file *filp)
{
    struct devone_data *dev = filp->private_data;

    if (dev) {
        del_timer_sync(&dev->tickfn);
        kfree(dev);
    }

    return 0;   /* success */
}

int devone_open(struct inode *inode, struct file *filp)
{
    struct devone_data *dev;

    dev = kmalloc(sizeof(struct devone_data), GFP_KERNEL);
    if (dev == NULL) {
        return -ENOMEM;
    }

    /* initialize members */
    spin_lock_init(&dev->lock);

    init_timer(&dev->tickfn);
    dev->tickfn.function = sample_timeout;
    dev->tickfn.data = (unsigned long)dev;
    dev->tickfn.expires = jiffies + TIMEOUT_VALUE;
    add_timer(&dev->tickfn);

    filp->private_data = dev;

    return 0;   /* success */
}

struct file_operations devone_fops = {
    .owner = THIS_MODULE,
    .open = devone_open,
    .release = devone_close,
    .read = devone_read,
    .write = devone_write,
};

static int devone_init(void)
{
    dev_t dev = MKDEV(devone_major, 0);
    int alloc_ret = 0;
    int major;
    int cdev_err = 0;
    struct class_device *class_dev = NULL;

    alloc_ret = alloc_chrdev_region(&dev, 0, devone_devs, "devone");
    if (alloc_ret)
        goto error;
    devone_major = major = MAJOR(dev);

    cdev_init(&devone_cdev, &devone_fops);
    devone_cdev.owner = THIS_MODULE;
    devone_cdev.ops = &devone_fops;
    cdev_err = cdev_add(&devone_cdev, MKDEV(devone_major, devone_minor), 1);
    if (cdev_err)
        goto error;

    /* register class */
    devone_class = class_create(THIS_MODULE, "devone");
    if (IS_ERR(devone_class)) {
        goto error;
    }
    devone_dev = MKDEV(devone_major, devone_minor);
    class_dev = class_device_create(
                    devone_class,
                    NULL,
                    devone_dev,
                    NULL,
                    "devone%d",
                    devone_minor);

    printk(KERN_ALERT "devone driver(major %d) installed.\n", major);

    return 0;

error:
    if (cdev_err == 0)
        cdev_del(&devone_cdev);

    if (alloc_ret == 0)
        unregister_chrdev_region(dev, devone_devs);

    return -1;
}

static void devone_exit(void)
{
    dev_t dev = MKDEV(devone_major, 0);

    /* unregister class */
    class_device_destroy(devone_class, devone_dev);
    class_destroy(devone_class);

    cdev_del(&devone_cdev);
    unregister_chrdev_region(dev, devone_devs);

    printk(KERN_ALERT "devone driver removed.\n");

}

module_init(devone_init);
module_exit(devone_exit);
```

spinlock 的測試程式： 

```c
```
