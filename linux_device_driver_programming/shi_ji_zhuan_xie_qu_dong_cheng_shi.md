# 實際撰寫驅動程式


延續上一章的驅動程式基礎知識，繼續介紹實際應用技巧。 

### 6-1、IOCTL 
驅動程式準備了 read 與 write 介面就能與 user process 交換資料，但仍有許多事情辦不到，例如，控制硬體暫存器、或是改變驅動程式本身運作模式的情形等。 
Linux 為了實現這些無法透過 read 或 write 完成的 「資料交換」工作，另外準備「IOCTL」介面。 

`IOCTL 的機制`

IOCTL(I/O Control) 是一種系統呼叫，user process 呼叫 ioctl() 即可對驅動程式送出系統呼叫，而呼叫到驅動程式的 IOCTL 處理函式。 
與 read() 跟 write() 一樣，可跟驅動程式交換資料，但 IOCTL 交換資料的格式可由驅動程式開發者自行決定，應用較為靈活。 

user process 呼叫 ioctl() 的 prototype： 
```c
int ioctl(int d, int request, ...); 
```
第一引數(d) 是裝置置的 handle。 <br>
第二引數(request) 是驅動程式定義的指令代碼(32 bit)，一般透過驅動程式的巨集指定。 <br>
第三引數(…) 代表可變引數。 <br>
要支援 ioctl()，驅動程式必須提供 IOCTL 方法，並將 file_operations 結構的 ioctl 成員設定函數指標： 

```c
int (*ioctl) (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
```

第一引數(inode) 是 user process 開啟的裝置檔相關資訊。 <br>
第二引數(filp) 通常在取出驅動程式私有資料時，會透過 filp 進行。 <br>
第三引數(cmd) 是 IOCTL 的指令，透過此引數，驅動程式可以判斷 user process 究竟想做什麼。 <br>
第四引數(arg) 相當於 ioctl() 可變引數(...) 的參數，內含 user process 指標。 <br>
ioctl() 是 user process 呼叫的，所以驅動程式的 IOCTL 方法是在 「user context」底下運行，因此可以 sleep，若當 IOCTL 方法內有 critical section 時，可以使用「semaphore」來鎖定。 


`IOCTL 指令 `

IOCTL 指令是 unsigned int 型別的變數，它的格式是固定的，以巨集定義。
| dir(2bit) | Size(14bit) | type(8bit) | nr(8bit) | 
IOCTL 指令格式定義在 include/asm-generic/ioctl.h。 
這些巨集可以用來在標頭檔下定義，讓驅動程式及 user process 可以 include 進來共用。 
- dev_ioctl.h

```c
#ifndef _DEVONE_IOCTL_H
#define _DEVONE_IOCTL_H

#include

struct ioctl_cmd {
    unsigned int reg;
    unsigned int offset;
    unsigned int val;
};

#define IOC_MAGIC 'd'

#define IOCTL_VALSET _IOW(IOC_MAGIC, 1, struct ioctl_cmd)
#define IOCTL_VALGET _IOR(IOC_MAGIC, 2, struct ioctl_cmd)

#endif
```

此例的 IOCTL_VALSET 巨集是用來傳資料給驅動程式， IOCTL_GET 則是從驅動程式讀回資料。 
巨集的 type 引數長度為 1byte，所以通常用字元常數指定，驅動程式名稱的第一個字元是很常見的選擇。 
引數 nr 是數值(number) 的意思，通常為 IOCTL 指令連續編號。 
引數 size 是傳給 ioctl() 可變引數(...) 的介面型別。 
驅動程式可以使用 _IOC_SIZE 巨集得知傳給 ioctl() 可變引數的結構大小。 

```c
#define _IOC_SIZE(nr) (((nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)
```

在讀寫 user process 指標時，可以先呼叫 access_ok() 巨集判斷指標可否讀寫。 

```c
#define access_ok(type, addr, size) (likely(__range_ok(addr, size) == 0)) 
```

`權限 `

IOCTL 指令如果要限制只有 root 可以使用，此時 capable() 就很方便： 
```c
int capable(int cap); 
```

這個函式的引數可以指定的巨集，定義在 include/linux/capability.h 內。 
要檢查是否擁有 root 權限，可以寫成： 

```c
if (!capable(CAP_SYS_ADMIN)) { retval = -EPERM goto done; } 
```

範例程式 
使用 IOCTL 的範例驅動程式，read() 傳回的資料可透過 ioctl() 改變，但需要 root 的權限才能修改。 
而驅動程式內的資料讀寫動作，也透過讀寫鎖定(read-write lock)來保護。 


- dev_ioctl.h 

```c
#ifndef _DEVONE_IOCTL_H
#define _DEVONE_IOCTL_H

#include

struct ioctl_cmd {
    unsigned int reg;
    unsigned int offset;
    unsigned int val;
};

#define IOC_MAGIC 'd'

#define IOCTL_VALSET _IOW(IOC_MAGIC, 1, struct ioctl_cmd)
#define IOCTL_VALGET _IOR(IOC_MAGIC, 2, struct ioctl_cmd)

#endif

```

- devone.c 

```c
/*
 * devone.c
 *
 * Update: 2007/12/25 Yutaka Hirata
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include "devone_ioctl.h"

MODULE_LICENSE("Dual BSD/GPL");

static int devone_devs = 1;        /* device count */
static int devone_major = 0;       /* MAJOR: dynamic allocation */
static int devone_minor = 0;       /* MINOR: static allocation */
static struct cdev devone_cdev;
static struct class *devone_class = NULL;
static dev_t devone_dev;

struct devone_data {
    rwlock_t lock;
    unsigned char val;
};

int devone_ioctl(struct file *filp,
                 unsigned int cmd,
                 unsigned long arg)
{
    struct devone_data *dev = filp->private_data;
    int retval = 0;
    unsigned char val;
    struct ioctl_cmd data;

    memset(&data, 0, sizeof(data));

    switch (cmd) {
    case IOCTL_VALSET:
        if (!capable(CAP_SYS_ADMIN)) {
            retval = -EPERM;
            goto done;
        }
        if (!access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd))) {
            retval = -EFAULT;
            goto done;
        }
        if ( copy_from_user(&data, (int __user *)arg, sizeof(data)) ) {
            retval = -EFAULT;
            goto done;
        }

        printk("IOCTL_cmd.val %u (%s)\n", data.val, __func__);

        write_lock(&dev->lock);
        dev->val = data.val;
        write_unlock(&dev->lock);

        break;

    case IOCTL_VALGET:
        if (!access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd))) {
            retval = -EFAULT;
            goto done;
        }
        read_lock(&dev->lock);
        val = dev->val;
        read_unlock(&dev->lock);
        data.val = val;
        if ( copy_to_user((int __user *)arg, &data, sizeof(data)) ) {
            retval = -EFAULT;
            goto done;
        }
        break;

    default:
        retval = -ENOTTY;
        break;
    }

done:
    return (retval);
}

ssize_t devone_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct devone_data *dev = filp->private_data;
    unsigned char val;
    int retval;
    int i;

    read_lock(&dev->lock);
    val = dev->val;
    read_unlock(&dev->lock);

    for (i = 0 ; i < count ; i++) {
        if (copy_to_user(&buf[i], &val, 1)) {
            retval = -EFAULT;
            goto out;
        }
    }
    retval = count;

out:
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
    rwlock_init(&dev->lock);
    dev->val = 0xFF;

    filp->private_data = dev;

    return 0;   /* success */
}

struct file_operations devone_fops = {
    .owner = THIS_MODULE,
    .open = devone_open,
    .release = devone_close,
    .read = devone_read,
    .unlocked_ioctl = devone_ioctl,
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
    class_dev = device_create(
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
    device_destroy(devone_class, devone_dev);
    class_destroy(devone_class);

    cdev_del(&devone_cdev);
    unregister_chrdev_region(dev, devone_devs);

    printk(KERN_ALERT "devone driver removed.\n");

}

module_init(devone_init);
module_exit(devone_exit);

```

Makefile 同上。