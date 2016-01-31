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

- Makefile

```c
CFILES = devone.c 

obj-m += sample.o
sample-objs := $(CFILES:.c=.o)

all:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
    make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```


- sample.c 

```c
/*
 * cc -Wall -o app app.c
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "devone_ioctl.h"

#define DEVFILE "/dev/devone0"

void read_buffer(int fd)
{
    unsigned char buf[64];
    int ret;
    int i;

    ret = read(fd, buf, sizeof(buf));
    if (ret == -1) {
        perror("read");
    }
    for (i = 0 ; i < ret ; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

int main(void)
{
    struct ioctl_cmd cmd;
    int ret;
    int fd;

    fd = open(DEVFILE, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    memset(&cmd, 0, sizeof(cmd));
    ret = ioctl(fd, IOCTL_VALGET, &cmd);
    if (ret == -1) {
        printf("errno %d\n", errno);
        perror("ioctl");
    }
    printf("val %d\n", cmd.val);

    // read buffer
    read_buffer(fd);

    memset(&cmd, 0, sizeof(cmd));
    cmd.val = 0xCC;
    ret = ioctl(fd, IOCTL_VALSET, &cmd);
    if (ret == -1) {
        printf("errno %d\n", errno);
        perror("ioctl");
    }

    // read buffer
    read_buffer(fd);

    memset(&cmd, 0, sizeof(cmd));
    ret = ioctl(fd, IOCTL_VALGET, &cmd);
    if (ret == -1) {
        printf("errno %d\n", errno);
        perror("ioctl");
    }
    printf("val %d\n", cmd.val);

    close(fd);

    return 0;
}
```

執行範例：
```c
insmod sample.ko 
lsmod | grep sample 
ls -l /dev/devone0 
hexdump -C -n 16 /dev/devone0 
./a.out 
```

### 6-2、64-bit kernel 
在 64-bit Linux 的 kernel space 可以執行 64-bit 與 32-bit 的應用程式，所以驅動程式也必須注意應用程式有 32-bit 與 64-bit 兩種情形。 

`Data Model(資料模型) `

OS 從32-bit 轉移到 64-bit 帶來的開發差異，在於資料型別的大小改變了，稱為「資料模型的差異」。 

`轉移到 64-bit 環境的要點 `

sizeof 
32-bit linux 與 64-bit linux 不同之處，除了 pointer 的大小由 4 bytes 改變為 8 bytes 之外，long 的長度也由 4 bytes 改為 8bytes ，所以使用 sizeof(long) 所傳回的值會不一樣，在 32-bit linux 可以順利執行的程式，在 64-bit linux 則可能造成緩衝區溢位。 

數字型別轉換 
在比較兩個數值時，如果彼此資料型別不同，可能會造成邏輯錯誤的現象，也可能會造成 32-bit linux 與 64-bit linux 執行結果不同的情況，算術型別轉換機制如下： 
幾乎在所有的情況下，都會把 signed 數字 轉換成 unsigned 數字的型別，之後再作比較，除非 unsigned 數字這端的級別比較低，而且 signed 的資料型別的 bit 數目比 unsigned 的資料型別的 bit 數目多。 

printf 
顯示指標的位址通常使用「%x」，但它的寬度是 32-bit，無法在 64-bit 環境下顯示正確結果，需改用「%p」。 

整數的上限 
在 C 語言中，將整數視為 int 來計算，如果在 64-bit 環境下，想用 64-bit 來計算，要在數值後面加上「L」使用成為 long long。 

指標的差值 
指標可以相減，但在 64-bit 環境下指標是 8 bytes 長，必須將相減結果放到 long 變數內。 

「-1」的處理方式 
在 32-bit 環境下，-1是「0xffffffff」，若是在 64-bit 環境下，-1 轉成 long 的話是「0xffffffffffffffff」，但是如果把 -1 儲存到 unsigned int 之類的 32-bit 變數時，就會維持「0xffffffff」的形式。 

NULL 與 0 
空指標由「NULL」巨集定義的，在 C 語言通常以指標實作： 

```c
#define NULL ((void*)0) 
```

`實作 IOCTL 介面 `

實作驅動程式的 IOCTL 介面時，需考慮 64-bit 環境帶來的影響，特別是傳遞 ioctl() 引數結構的動作。 
比如說，下面這個結構由於內含指標成員，所以在 32-bit 與 64-bit 環境下的大小、變數偏移量並不相同。 

```c
struct ioctl_com {
    int cmd;
    unsigned int size;
    unsigned char *buf;
    int flag;
}
```

如果沒有用 `pack pragma(#pragma pack())`
指定變數打包方式的話，編譯器會為了提高記憶體讀寫效率、應對處理器的限制，而自動把變數配置在適當的偏移量上。 
在 32-bit 環境是以 4 bytes 為一個單位，而在 64-bit 環境則是以 8 bytes 為配置單位，為了讓結構的大小能貼齊邊界，結構最後也可能自動補上空白。 
這種結構大小的差異會影響在 64-bit linux 環境下執行的 32-bit 應用程式，如果驅動程式跟應用程式對記憶體的佈局認知不同，就會參考到不對的地方，進而引發錯誤。


gcc 定義的巨集 
32-bit 應用程式發出的 IOCTL 結構必須與 64-bit 應用程式一致才行，如此，驅動程式就不必區分這兩種結構佈局差異。 
要達到這個目的，可以使用編譯器定義的巨集，如此，在編譯應用程式時，判斷是 32-bit 還是 64-bit，讓結構的偏移量能一致。 
64-bit gccc 會定義下面這類巨集，代表編譯的是 64-bit 應用程式： 
```c
*__LP64__ 
*__ia64__
```

下面這兩個指令，都可以用來列出巨集的定義： 

```c
echo | gcc -v -E -dM - 
echo | cpp -dM 
```

IOCTL 結構的寫法 
將新改寫前面的範例結構，如下： 

```c
struct ioctl_com {
    int cmd;
    unsigned int size;
#ifdef __LP64__
    unsigned char *buf; /* 8 bytes */
#else
    unsigned char *buf; /* 4 bytes */
    unsigned int mbz; /* 4 bytes padding*/
#endif /* __LP64__ */
    int flag;
    int dummy_pad
}
```

身為指標的 buf 成員位置之後，在以 32-bit 編譯時明確插入平移(mbz 成員) 
為了讓結構大小是 8 bytes 的倍數，所以尾端也加上平移(dummy_pad 成員)。 
如此，就算以 32-bit 編譯，結構的記憶體配置也對與 64-bit 時相同。 
驅動程式讀出 buf 成員時會讀到 8 bytes 資料，但只要事先將 mbz 設為 0，就能得到正確的 32-bit 應用程式指標(4 bytes)。 
目前的結構是專屬於「little endian」，如果要再加上支援「big endian」的話，需再作修改： 


```c
struct ioctl_com {
    int cmd;
    unsigned int size;
#ifdef __LP64__
    unsigned char *buf; /* 8 bytes */
#else

#ifdef __LITTLE_ENDIAN_BITFIELD
    unsigned char *buf; /* 4 bytes */
    unsigned int mbz; /* 4 bytes padding*/
#elif __BIG_ENDIAN_BITFIELD
    unsigned int mbz; /* 4 bytes padding*/
    unsigned char *buf; /* 4 bytes */
#else
#error "Please fix "
#endif

#endif /* __LP64__ */
    int flag;
    int dummy_pad
}
```

`DMA `

DMA (Direct Memory Access) 傳送資料時必須注意的問題之一，是 DMA 暫存區的位址變成 64 bits 時，會不會出現問題。 
執行 DMA 的是硬體本身，如果硬體不支援 64 bits 位址，就必須限制在 32-bit 位址範圍內。

### 6-3、select/poll 

以 open 系統呼叫開啟檔案時，預設會以「blocking mode」開啟，`block 指的是 process 為了等待某件事情發生，而進入 sleep 狀態的情形。 `
Linux 應用程式多以 blocking mode 開發，而 windows 的主流則是 non-blocking mode。 
對應用程式來說，如果 read 系統呼叫會被 block 的話，有時可能會引發設計上的問題，所以 linux 準備了以下的措施：
Non-block 模式
同時執行多個同步 I/O 工作

啟用 non-blocking 模式後，read() 與 write() 就不會被 block 了，而是傳回「EAGAIN」錯誤碼(errno)。 
在應用程式中，如果要使用 non-blocking 模式的話，就要在 open() 開檔時，以 OR 指定「O_NONBLOCK」選項。 

```c
fd = open(DEVFILE, O_RDWR | O_NONBLOCK);
if ( fd == -1 ) {
    perror("open");
    exit(1);
}
```

在驅動程式的部分，如果要支援 non-blocking 模式的話，驅動程式也需要改寫才行，會在下面章節中提到。


`同時執行多個同步 I/O 工作 `

同時執行多個同步 I/O 工作(synchronous I/O multiplexing) 指的是使用 select 系統呼叫。 
透過 select 系統呼叫，可以監視多個 file handle 在三種狀態之的變化(可讀取、可寫入、發生錯誤)。 
select 系統呼叫本身會被 block，但是可以指定 timeout。 
read() 呼叫後，會一直等到資料抵達為止，所以可以在呼叫 read() 之前先呼叫 select() 判斷資料抵達了沒有(能不能讀取)，如果可以讀取，再呼叫 read()。 


```c
retval = select(fd+1, &rfds, NULL, NULL, &tv);
if ( retval ) {
    read(fd, buf, sizeof(buf));
}
```

使用 select() 時，有幾個經常用到的函式巨集，請情可以參考「man 2」。 

```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); 
void FD_CLR(int fd, fd_set *set); 
void FD_ISSET(int fd, fd_set *set); 
void FD_SET(int fd, fd_set *set); 
void FD_ZERO(fd_set *set); 
```
另外還有個跟 select() 很類似的「poll()」系統呼叫，以基本功能來說 select() 與 poll() 都一樣，但指定 file handle 的方式不同。 
而且，在指定多個 file handle 時，poll() 走訪所有 file handle 的速度較快，請細用法可以參考「man 2 poll」。 


```c
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
struct pollfd {
    int fd; /* file descriptor */
    short events; /* requested events */
    short revents; /* returned events */
};
```


`驅動程式的實作 `
應用程式想同時執行多個同步 I/O 工作時，可以使用的系統呼叫有好幾個，包含 select 與 poll 等，但驅動程式只需要準備一個函式就夠了，kernel 只會呼叫驅動程式提供的這個函式。 
Character 類型的裝置想支援同時執行多個同步 I/O 工作的話，只要在驅動程式準備 poll 方法即可。 

```c
unsigned int (*poll) (struct file *, struct poll_table_struct *); 
```

poll 方法會在 kernel 處理 select 與 poll 之類的系統呼叫時用到，它必須執行的工作很簡單，包含：
- 在 wait queue 登記
- 傳回目前可以操作的狀態

在呼叫執行多個同步 I/O 工作的系統呼叫時，「block 直到狀態變化」的動作，指的是在 kernel 裡面 sleep。 
要 sleep ，需要先準備 wait queue (wait_queue_head_t)，由驅動程式負責提供。 
Wait queue 可以參考以下 6.6 節 -「Sleep 與 Wake Up」。 
登記 wait queue 的工作可透過 poll_wait() 完成，定義在「linux/poll.h」： 

```c
void poll_wait(struct file *filp, wait_queue_head_t *wait_address, poll_table *p); 
```

在呼叫同時執行多個同步 I/O 工作的系統呼叫時，解除 block 的時機，是驅動程式透過傳給 poll_wait() 的 wait queue 被喚醒的時候。 
Kernel 在被 wait queue 喚醒之後，會再次呼叫驅動程式的 poll() 確認是否已成為等待中的狀態 (可寫入或可讀出)。 
如果是的話 FD_ISSET 巨集就會成立，要判斷是否為這種狀態，當然還是需要驅動程式提供資訊才行，這個資訊就是透過 poll() 方法的傳回值來表示。 
傳回值要透過 linux/poll.h 定義的巨集 OR 起來表示，以下是常用到的組合：

<table border="1">
<caption></caption><tbody><tr><td>Bit OR</td><td>意義</td>
</tr><tr><td>POLLIN | POLLRDNORM</td><td>可讀取</td></tr>
<tr><td>POLLOUT | POLLWRNORM</td><td>可寫入</td></tr>
<tr><td>POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM</td><td>可讀寫</td></tr>
<tr><td>POLLERR</td><td>發生錯誤</td></tr>
<tr><td>POLLHUP</td><td>裝置離線(EOF)</td></tr>
</tbody></table>


下列是 poll 方法的程式片段，poll 方法在向 wait queue 登記完成之後必須立刻返回，不能在 poll 方法裡 sleep： 

```c
unsigned int devone_poll(struct file *filp, poll_table *wait) {
    struct devone_data *dev = filp->private_data;
    unsigned int mask = 0;

    if (dev == NULL)
        return -EBADFD;
    poll_wait(filp, &dev->read_wait, wait);
    if (dev->timeout_done == 1) { /* readable */
        mask |= POLLIN | POLLRDNORM;
    }
    printk("%s returned (mask 0x%x)\n", __func__, mask);
    return mask;
}
```
`範例驅動程式 `

接著來寫滿足下列條件的驅動程式：
- 支援 user process 發出 read() 系統呼叫
- 有資料時，read() 會立刻反回
- 無資料時，read() 會 sleep (block)
- 支援 poll() 系統呼叫
- 發出 write() 會導致錯誤

模擬有無資料的方式，是在收到 open() 系統呼叫時啟動 kernel timer，經過一段時間後 wake up，來模擬收到新資料。 <br>
此範例是假想應用程式為了從硬體取得資料，所以送出 read() 系統呼叫的情形。 
沒有資料時，驅動程式內部需要 sleep，但是要能在 user process 收到 signal 的時候解除 sleep 才行。 <br>
這種用途可以使用 wait_event_interruptible() 這個 kernel 函式，如果要避免錯過 wake up 的時機，可以改用 wait_event_interruptible_timeout() 這個附帶 time out 功能的函式。 <br>
順道一提，在收網路封包時，處理的方式也十分類似，是透過實作於 net/core/datagram.c 的 wiat_for_packet() 來等待的。 <br>
另一個要考慮的地方是 non-blocking mode 的支援，User process 以 non-blocking mode 開啟裝置檔時，read() 就不能 block，也就是驅動程式內不能 sleep，沒有資料的話必須傳回錯誤碼才行。 <br>

main.c ，產生 .ko 檔的驅程程式程式碼： 

```c
/*
 * devone.c
 *
 * Update: 2007/12/19 Yutaka Hirata
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
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/semaphore.h>

MODULE_LICENSE("Dual BSD/GPL");

static unsigned int timeout_value = 15;
/* driver parameter */
module_param(timeout_value, uint, 0);

static int devone_devs = 1;        /* device count */
static int devone_major = 0;       /* MAJOR: dynamic allocation */
static int devone_minor = 0;       /* MINOR: static allocation */
static struct cdev devone_cdev;
static struct class *devone_class = NULL;
static dev_t devone_dev;

struct devone_data {
    struct timer_list timeout;
    spinlock_t lock;
    wait_queue_head_t read_wait;
    int timeout_done;
    struct semaphore sem;
};

static void devone_timeout(unsigned long arg)
{
    struct devone_data *dev = (struct devone_data *)arg;
    unsigned long flags;

    printk("%s called\n", __func__);

    spin_lock_irqsave(&dev->lock, flags);

    dev->timeout_done = 1;
    wake_up_interruptible(&dev->read_wait);  // 喚醒 sleep process

    spin_unlock_irqrestore(&dev->lock, flags);
}

unsigned int devone_poll(struct file *filp, poll_table *wait)
{
    struct devone_data *dev = filp->private_data;
    unsigned int mask = POLLOUT | POLLWRNORM;

    printk("%s called\n", __func__);
    printk(KERN_ALERT "0x%08X\n", __builtin_return_address(0));

    if (dev == NULL)
        return -EBADFD;

    down(&dev->sem);
    poll_wait(filp, &dev->read_wait, wait);
    if (dev->timeout_done == 1) {    /* readable */
        mask |= POLLIN | POLLRDNORM;
    }
    up(&dev->sem);

    printk("%s returned (mask 0x%x)\n", __func__,  mask);
    return (mask);
}

ssize_t devone_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    printk("%s called\n", __func__);
    return -EFAULT;
}


ssize_t devone_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct devone_data *dev = filp->private_data;
    int i;
    unsigned char val;
    int retval;

    printk("%s called\n", __func__);


    if (down_interruptible(&dev->sem))
        return -ERESTARTSYS;

    if (dev->timeout_done == 0) {   /* no read */
        up(&dev->sem);
        if (filp->f_flags & O_NONBLOCK)   /* Non-blocking mode */
            return -EAGAIN;
        do {
            retval = wait_event_interruptible_timeout(dev->read_wait,
                     dev->timeout_done == 1, 1*HZ);
            if (retval == -ERESTARTSYS)
                return -ERESTARTSYS;
        } while (retval == 0);  /* timeout elapsed */

        if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    }

    val = 0xff;
    for (i = 0 ; i < count ; i++) {
        if (copy_to_user(&buf[i], &val, 1)) {
            retval = -EFAULT;
            goto out;
        }
    }
    retval = count;

out:
    dev->timeout_done = 0;
    /* restart timer */
    mod_timer(&dev->timeout, jiffies + timeout_value*HZ);  // timeout_value 秒後call devone_timeout funtionset dev->timeout_done = 1 and devone_poll 函數中 mask |= POLLIN | POLLRDNORM;  , user space retval value 0 to 1

    up(&dev->sem);

    return (retval);
}

int devone_close(struct inode *inode, struct file *filp)
{
    struct devone_data *dev = filp->private_data;

    if (dev) {
        del_timer_sync(&dev->timeout);
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

    init_waitqueue_head(&dev->read_wait); // 初始化等待隊列頭

    sema_init(&dev->sem, 1);

    init_timer(&dev->timeout);
    dev->timeout.function = devone_timeout;
    dev->timeout.data = (unsigned long)dev;

    filp->private_data = dev;

    /* start timer */
    dev->timeout_done = 0;
    mod_timer(&dev->timeout, jiffies + timeout_value*HZ);

    return 0;   /* success */
}

struct file_operations devone_fops = {
    .owner = THIS_MODULE,
    .open = devone_open,
    .release = devone_close,
    .read = devone_read,
    .write = devone_write,
    .poll = devone_poll,
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

read.c - 依照 read() 的 flag 的設定，此程式可以寫成 block mode 或 non-block mode，依 blocking 這個參數來選擇。 


```c
/*
 * # cc blk.c && ./a.out
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#define DEVFILE "/dev/devone0"

int main(void) {
    int fd;
    unsigned char buf[64];
    ssize_t sz;
    int i;
    int blocking = 0;

    if (blocking) 
        fd = open(DEVFILE, O_RDWR);
    else
        fd = open(DEVFILE, O_RDWR|O_NONBLOCK);

    if (fd == -1) {
        perror("open");
        exit(1);
    }

    printf("read() ...\n");
    sz = read(fd, buf, sizeof(buf));
    printf("read() %d\n", sz);

    if ( sz > 0 ) {
        for (i = 0 ; i < sz ; i++) {
            printf("%02x ", buf[i]);
        }
        printf("\n");

    }
    else {
        printf("errno %d\n", errno);
        perror("read");
    }

    close(fd);
    return 0; 
}
```

select.c - 透過 select() 等待，直到可以讀取資料時才送出 read()： 

```c
/*
 * # cc select.c && ./a.out
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVFILE "/dev/devone0"

int main(void)
{
    int fd;
    fd_set rfds;
    struct timeval tv;
    int retval;
    unsigned char buf[64];
    ssize_t sz;
    int i;

    fd = open(DEVFILE, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }


    do {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        printf("select() ...\n");
        retval = select(fd + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            perror("select");
            break;
        }

        if (retval) {
            break;
        }
    } while (retval == 0);   /* timeout elapsed */

    if (FD_ISSET(fd, &rfds)) {
        printf("read() ...\n");
        sz = read(fd, buf, sizeof(buf));
        printf("read() %d\n", sz);
        for (i = 0 ; i < sz ; i++) {
            printf("%02x ", buf[i]);
        }
        printf("\n");
    }

    close(fd);

    return 0;
}
```
poll.c - 透過 poll() 等待，直到可以讀取資料時才送出 read()： 

```c
/*
 * # cc poll.c && ./a.out
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

#define DEVFILE "/dev/devone0"

int main(void)
{
    int fd;
    struct pollfd fds;
    int retval;
    unsigned char buf[64];
    ssize_t sz;
    int i;

    fd = open(DEVFILE, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    do {
        fds.fd = fd;
        fds.events = POLLIN;

        printf("poll() ...\n");
        retval = poll(&fds, 1, 5 * 1000); // 5秒 timeout
        if (retval == -1) {
            perror("poll");
            break;
        }

        if (retval) {
            break;
        }
    } while (retval == 0);   /* timeout elapsed */

    if (fds.revents & POLLIN) {
        printf("read() ...\n");
        sz = read(fd, buf, sizeof(buf));
        printf("read() %ld\n", sz);
        for (i = 0 ; i < sz ; i++) {
            printf("%02x ", buf[i]);
        }
        printf("\n");
    }

    close(fd);

    return 0;
}
```

### 6-4、procfs 
profs (/proc file system) 指的是位於 /proc 的虛擬檔案系統 (實體在 RAM 之內)，內含 kernel 與各驅動程式的設定選項。 
/proc 裡面的大多數檔案都是唯讀的，但也有些檔案可讓使用者寫入新設定。 



`建立唯讀的 procfs` <br>

在驅動程式初始化時，呼叫 create_proc_read_entry() 即可建立唯讀的 procfs 檔案，定義的標頭檔是 linux/proc_fs.h： 

```c
struct proc_dir_entry *create_proc_read_entry(const char *name, mode_t mode, struct proc_dir_entry *base, read_proc_t *read_proc, void *data); 
```

各引數的意義如下：

<table border="1">
<tbody><tr><td>name</td><td>在 /proc 之內的檔案名稱 </td></tr>
<tr><td>mode</td><td>權限 (可 NULL) </td></tr>
<tr><td>base</td><td>上層目錄 (可 NULL) </td></tr>
<tr><td>read_proc</td><td>讀取處理函式 </td></tr>
<tr><td>data</td><td>私有資料 (可 NULL) </td></tr>
</tbody></table>


其中 read_proc 引數要傳入讀取時呼叫的處理函式指標，這個函式的 prototype 已 typedef 為 read_proc_t： 

```c
typedef int (read_proc_t) (char *page, char **start, off_t off, int count, int *eof, void *data);
```


各引數的意義為：

<table border="1">
<tbody><tr><td>page</td><td>Kernel 配置的記憶體空間 </td></tr>
<tr><td>start</td><td>驅動程式寫入資料的開始位址指標(由驅動程式回傳)</td></tr>
<tr><td>off</td><td>驅動程式傳回資料的偏移量</td></tr>
<tr><td>count</td><td>User process 一次讀回的量</td></tr>
<tr><td>eof</td><td>通知已達資料終點(由驅動程式回傳)</td></tr>
<tr><td>data</td><td>create_proc_read_entry() 設定的私有資料</td></tr>
</tbody></table>

page 引數是 kernel 分配的記憶體，所以驅動程式可以直接寫入，但是因為只分配了一個 page 的大小(PAGE_SIZE)，所以寫入的資料量超過 PAGE_SIZE 就會破壞記憶體內容。 
start 引數在呼叫讀取處理函式之後的值是「*start = NULL」，start 指向 page 這塊記憶體空間裡開始寫入資料的位置，由驅動程式負責設定，如果資料量沒超過 PAGE_SIZE 的話，驅動程式就不必動到這個引數。 
off 引數是 kernel 負責更新的偏移量，代表驅動程式回傳資料的偏移量，如果資料量沒超過 PAGE_SIZE 的話，就不必特地處理這個引數。 
count 引數是 user process 一次讀回的資料量，通常比 PAGE_SIZE 小，資料量沒超過 PAGE_SIZE 的話，也不必特地處理這個引數。 
讀取處理函式的工作內容，實作於 fs/proc/generic 的 proc_file_read()。 
驅動程式卸載時，要呼叫 remove_proc_entry() 清除 procfs 的內容： 

```c
void remove_proc_entry(const char *name, struct proc_dir_entry *parent); 
```

- procfs.c

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("Dual BSD/GPL");

#define PROCNAME "driver/sample"

#define DUMMY_BUFSIZ 4096
static char dummy[DUMMY_BUFSIZ];

/*執行cat /proc/xxxx時,什麼時候調用read_proc,什麼時候調用proc_fops->read呢?
 * 答案:
 * 在註冊entry的時候，如果提供read_proc,讀文件時調用路徑為
 * proc_file_operations->read->proc_file_read->read_proc
 * */

static int sample_read_proc(char *page, char **start, off_t off,
                            int count, int *eof, void *data)
{
    int len;

    dump_stack();

    printk("page=%p *start=%p off=%d count=%d *eof=%d data=%p\n",
           page, *start, (int)off, count, *eof, data);

    len = DUMMY_BUFSIZ;
    if (len > PAGE_SIZE)
        return -ENOBUFS;

    memcpy(page, dummy, len);  // dump 內容ABC~Z, totoal 4096byte 存放到page , user space 在讀取page , len = 4096
    *eof = 1;

    printk("len=%d (%s)\n", len, __func__);
    return (len);
}

static int sample_init(void)
{
    struct proc_dir_entry *entry;
    int i;

    /* add /proc */
    entry = create_proc_read_entry(PROCNAME, S_IRUGO | S_IWUGO, NULL, sample_read_proc, NULL);
    if (entry == NULL) {
        printk(KERN_WARNING "sample: unable to create /proc entry\n");
    }

    for (i = 0 ; i < DUMMY_BUFSIZ ; i++) {
        dummy[i] = 'A' + (i % 26);
    }

    printk("driver loaded\n");

    return 0;
}

static void sample_exit(void)
{
    remove_proc_entry(PROCNAME, NULL);

    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

執行範例：
```c 
# sudo /sbin/insmod ./sample.ko 
# cat /proc/driver/sample | wc -c 
# dmesg 
```

`建立可讀寫的 procfs `

要建立可讀寫的 procfs 項目時，可透過 create_proc_entry() 指記到 /proc 之內： 

```c
struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode, struct proc_dir_entry *parent); 
```
各引數的意義為：

<table border="1">
<tbody><tr><td>name</td><td>/proc 之內的項目名稱</td></tr>
<tr><td>mode</td><td>權限 (可NULL)</td></tr>
<tr><td>base</td><td>上層目錄 (可NULL)</td></tr>
</tbody></table>


登記處理函式的方式，是為 create_proc_entry() 傳回的 proc_dir_entry 結構設定 read_proc 與 write_proc 成員。 read_proc 成員的 prototype 與上節相同，已被 typedef 為 read_proc_t： 

```c
typedef int (read_proc_t) (char *page, char **start, off_t off, int count, int *eof, void *data); 
```
write_proc 成員的 prototype 也已被 typedef 為 write_proc_t： 

```c
typedef int (write_proc_t) (struct file *file, const char __user **buffer, unsigned long count, void *data); 
```

這兩個 prototype 並不相同，且 write_proc 會收到 user process 傳來的資料，所以必須透過 copy_from_user() 接收這些資料。 

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define PROCNAME "driver/sample"

static int sample_flag = 0;

int sample_proc_read(char *page, char **start, off_t off,
                     int count, int *eof, void *data)
{
    int len;

    printk(KERN_INFO "%s called\n", __func__);

    if (off > 0) {
        len = 0;
    } else {
        len = sprintf(page, "%d\n", sample_flag);
    }

    return (len);
}

int sample_proc_write(struct file *file, const char *buffer, unsigned long count,
                      void *data)
{
    char buf[16];
    unsigned long len = count;
    int n;

    printk(KERN_INFO "%d (%s)\n", (int)len, __func__);

    if (len >= sizeof(buf))
        len = sizeof(buf) - 1;

    if (copy_from_user(buf, buffer, len))
        return -EFAULT;
    buf[len] = '\0';

    n = simple_strtol(buf, NULL, 10);
    if (n == 0)
        sample_flag = 0;
    else
        sample_flag = 1;

    return (len);
}

static int sample_init(void)
{
    struct proc_dir_entry *entry;

    /* add /proc */
    entry = create_proc_entry(PROCNAME, 0666, NULL);
    if (entry == NULL) {
        printk(KERN_WARNING "sample: unable to create /proc entry\n");
        return -ENOMEM;
    }

    entry->read_proc = sample_proc_read;
    entry->write_proc = sample_proc_write;
    // entry->owner = THIS_MODULE;

    printk("driver loaded\n");

    return 0;
}

static void sample_exit(void)
{
    remove_proc_entry(PROCNAME, NULL);

    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```
執行範例：

```c
# sudo /sbin/insmod ./sample.ko 
# ls -l /proc/driver/sample 
# cat /proc/driver/sample 
# echo 1 > /proc/driver/sample 
# cat /proc/driver/sample 
# echo 0 > /proc/driver/sample 
# cat /proc/driver/sample 
```


其它功能 
使用 proc_mkdir() 即可在 /proc 建立新目錄： 

```c
struct proc_dir_entry *proc_mkdir(const char *name, struct proc_dir_entry *parent); 
```

要在新建立的目錄中放入檔案的話，只要將 proc_mkdir() 的傳回值當成「上層目錄」透過 create_proc_entry() 的第三引數傳入即可。 
則除目錄時，可以呼叫 remove_proc_entry()。 
呼叫 proc_symlink() 即可在 /proc 之內建立 symbolic link： 

```c
struct proc_dir_entry *proc_symlink(const char *name, struct proc_dir_entry *parent, const char *dest); 
```

### 6-5、seq_file 

/proc 在處高小資料時很方便，但一旦想一次交換的資料超過 page size 就會變得很複雜，另外如果 /proc 的檔案同時被許多 process 讀取時，也可能造成問題。 
為瞭解決這些困擾，Linux 定義了新的「seq_file」介面。 

`seq_file 的位階 `

seq_file 不是用來取代 /proc 的東西，而是停用 procfs 的 read_proc 與 write_proc 改用 proc_fops 的作法。 
將 proc_fops 的成員登記為 seq_file 提供的函式群，即可改用「seq_file」介面。 
使用 seq_file 介面的話，就不必煩惱 page size，可以輕易輸出大量資料。 

`seq_file 的用法 `

驅動程式需要把 linux/seq_file.h 及 linux/proc_fs.h 這兩個標頭檔一起 include 進來。 
seq_file 的基本用法為： 

```c
static struct seq_operations sample_seq_op = {
    .start = as_start,
    .next = as_next,
    .stop = as_stop,
    .show = as_show,
};

static int sample_proc_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &sample_seq_op);
}

static struct file_operations sample_operations = {
    .open = sample_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

static int sample_init(void) {
    struct proc_dir_entry *entry;

    entry = create_proc_entry("file name inside procfs", S_IRUGO | S_IWUGO, NULL);
    if (entry)
        entry->proc_fops = &sample_operations;
}
```

重點在 seq_operations 結構定義的四個處理函式：
- as_start()
- as_next()
- as_stop()
- as_show()

驅動程式開發者只要設計這四個處理函式即可，seq_file 介面原則上只支援「讀取」，沒辦法寫入。 
各成員意義如下：

<table border="1">
<tbody><tr><td>as_start</td><td>開始讀取時最先呼叫的函式。傳回最初資料(記錄)的索引(從1起算)，沒有資料時傳回「0」</td></tr>
<tr><td>as_next</td><td>有下一筆資料時，傳回下一筆資料的索引，否則傳回「0」</td></tr>
<tr><td>as_stop</td><td>在讀完所有資料之後呼叫的函式</td></tr>
<tr><td>as_show</td><td>顯示資料</td></tr>
</tbody></table>

seq_file 的觀念是從 C++ 與 Java 的迭代器(iterator) 學來的，這邊就不附上流程圖。 
各成員函式的 prototype 如下： 

```c
void * (*start) (struct seq_file *m, loff_t *pos); 
void (*stop) (struct seq_file *m, void *v); 
void * (*next) (struct seq_file *m, void *v, loff_t *pos); 
int (*show) (struct seq_file *m, void *v); 
```

start 與 next 接到的「*pos」變數會從 0 開始，隨著逐項處理資料的過程都加，增加這個變數的動作是由驅動程式進行的，kernel 不會修改。 
start 與 next 如果有要顯示的資料，就要以 

```c 
void*
```
的型別傳回「1以上」的索引，實際上，`void*` 的使用方式是由驅動程式自行決定，而 kernel 只會把以下兩種傳回值當成已經沒有資料：
- NULL
- IS_ERR 巨集成立的值(負數)

也就是說，如果把它當資料索引的話，須注意要「從1起算」。 
stop 如果不需要做額外處理的話，不實作也無妨，但如果在取用資料時必須鎖定的話，可以在 start 裡面鎖定、在 stop 釋放鎖定。 
show 負責將資料顯示給使用者，採用下列 seq_file 專用的介面： 

```c
int seq_printf(struct seq_file *m, const char *f, ...); 
int seq_putc(struct seq_file *m, char c); 
int seq_puts(struct seq_file *m, const char *s); 
```

seq_printf() 接受可變引數，seq_putc() 可輸出一個字元，seq_puts() 則輸出一個字串。 

seq_file.c - 範例程式 
```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("Dual BSD/GPL");

#define PROCNAME "sample"

static char *data_message[] = {
    "Fedora",
    "Red Hat Enterprise Server",
    "Debian",
    "Vine",
    "Ubuntu",
    0
};

static void *as_start(struct seq_file *m, loff_t *pos)
{
    loff_t n = *pos;
    char **p;
    int i;

    printk("%lld (%s)\n", n, __func__);

    if (n == 0) {
        seq_printf(m, "=== seq_file header ===\n");
    }

    p = data_message;
    for (i = 0 ; p[i] ; i++) {
        n--;
        if (n < 0)
            return (void *)(i + 1);
    }

    return 0;
}

static void *as_next(struct seq_file *m, void *p, loff_t *pos)
{
    int n = (int)p;
    char **ptr;

    printk("%u (%s)\n", n, __func__);

    (*pos)++;

    ptr = data_message;
    if (ptr[n]) {   /* exist next data */
        return (void *)(n + 1);   /* next index */
    }

    return 0;
}

static void as_stop(struct seq_file *m, void *p)
{
    int n = (int)p;

    printk("%u (%s)\n", n, __func__);
}

static int as_show(struct seq_file *m, void *p)
{
    int n = (int)p - 1;

    printk("%u (%s)\n", (int)p, __func__);

    seq_printf(m, "[%d] %s\n", n, data_message[n]);

    return 0;
}

/* seq_file handler */
static struct seq_operations sample_seq_op = {
    .start	= as_start,
    .next	= as_next,
    .stop	= as_stop,
    .show	= as_show,
};

static int sample_proc_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &sample_seq_op);
}

/* procfs handler */
static struct file_operations sample_operations = {
    .open = sample_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

static int sample_init(void)
{
    struct proc_dir_entry *entry;

    /* add /proc */
    entry = create_proc_entry(PROCNAME, S_IRUGO | S_IWUGO, NULL);
    if (entry)
        entry->proc_fops = &sample_operations;

    printk("driver loaded\n");

    return 0;
}

static void sample_exit(void)
{
    remove_proc_entry(PROCNAME, NULL);

    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```


執行範例： 

```
# sudo /sbin/insmod ./sample.ko 
# ls -l /proc/sample 
# cat /proc/sample 
# dmesg 
```
seq_file.c - 範例程式 - 2 
透過 kernel 的 task_init 變數，列出目前系統中執行的所有 process。 
在參考 task list 時，Linux 2.6.18 之前的版面需要透過讀取專用的 tasklist_lock 進行鎖定，但 Linux 2.6.18 之後的版本不再匯出 tasklist_lock 符號，改成需要取得 RCU (Read Copy Update) 鎖定。

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/version.h>

MODULE_LICENSE("Dual BSD/GPL");

#define PROCNAME "sample2"

static void *as_start(struct seq_file *m, loff_t *pos)
{
    loff_t n = *pos;
    struct task_struct *p = NULL;

    printk("%lld (%s)\n", n, __func__);

    if (n == 0) {
        seq_printf(m, "=== seq_file header ===\n");
    }

    /* Acquire a read-only spinlock */
#if ( LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18) )
    read_lock(&tasklist_lock);
#else
    rcu_read_lock();
#endif

    if (n == 0)
        return (&init_task);   /* swapper process */

    for_each_process(p) {  /* init later process */
        n--;
        if (n <= 0)
            return (p);
    }
    return 0;
}

static void *as_next(struct seq_file *m, void *p, loff_t *pos)
{
    struct task_struct *tp = (struct task_struct *)p;

    printk("%lld (%s)\n", *pos, __func__);

    (*pos)++;

    tp = next_task(tp);
    if (tp == &init_task)
        return NULL;

    return (tp);
}

static void as_stop(struct seq_file *m, void *p)
{
    printk("%p (%s)\n", p, __func__);

    /* Release a read-only spinlock */
#if ( LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18) )
    read_unlock(&tasklist_lock);
#else
    rcu_read_unlock();
#endif
}

static int as_show(struct seq_file *m, void *p)
{
    struct task_struct *tp = (struct task_struct *)p;

    printk("%p (%s)\n", tp, __func__);

    seq_printf(m, "[%s] pid=%d\n", tp->comm, tp->pid);
    seq_printf(m, "     tgid=%d\n", tp->tgid);
    seq_printf(m, "     state=%ld\n", tp->state);
    seq_printf(m, "     mm=0x%p\n", tp->mm);
    seq_printf(m, "     utime=%lu\n", tp->utime);
    seq_printf(m, "     stime=%lu\n", tp->stime);
    seq_printf(m, "     oomkilladj=%d\n", tp->oomkilladj);
    seq_puts(m, "\n");

    return 0;
}

/* seq_file handler */
static struct seq_operations sample_seq_op = {
    .start	= as_start,
    .next	= as_next,
    .stop	= as_stop,
    .show	= as_show,
};

static int sample_proc_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &sample_seq_op);
}

/* procfs handler */
static struct file_operations sample_operations = {
    .open = sample_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = seq_release,
};

static int sample_init(void)
{
    struct proc_dir_entry *entry;

    /* add /proc */
    entry = create_proc_entry(PROCNAME, S_IRUGO | S_IWUGO, NULL);
    if (entry)
        entry->proc_fops = &sample_operations;

    printk("driver loaded\n");

    return 0;
}

static void sample_exit(void)
{
    remove_proc_entry(PROCNAME, NULL);

    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);

```


執行範例： 
```c
# sudo /sbin/insmod ./sample.ko 
# ls -l /proc/sample2 
# cat /proc/sample2 
# dmesg 
```

### 6-6、Sleep 與 Wake Up 
介紹 sleep 與 wake up 觀念。 


`Preemptive (可被搶佔) `

執行 user process 時，會根據 kernel 內建的排程演算法，由 OS 定期強制切換執行的程式，因此 user process 稱為「preemptive(可被搶佔)」。 
切換 user process 的是 OS 本身 (Linux kernel)。 
為 user process 排程的演算法有許多種，但負責切換的都是 kernel/sched.c 裡面實作的 「schedule()」這個 kernel 函式。 
schedule() 切換 process 的動作稱為「context switch (環境切換)」。 
schedule() 會被 kernel 許多地方呼叫，主要呼叫的地方包含：
- User context 開始 sleep 的時候
- 嘗試鎖定 spin lock 的時候(啟用 CONFIG_PREEMPT 的情形)
- Timer scheduler 延遲呼叫的時候

而 Linux 的 kernel space 並不是 preemptive 的，包含驅動程式在內的 kernel 工作內容都不是 preemptive。 
但是有個例外的情形，在發生硬體中斷時，中斷函式會強製取得 CPU 控制權。 
負責在 user process 之間切換的就是 kernel，而 kernel 內部並沒有其它角色負責排程，但是 kernel 內部可以明確放出 CPU 使用權，也就是直接呼叫 schedule()。 
特別是驅動程式，有時需等待硬體完成工作，此時就需要暫時放下自己的工作，讓 CPU 去處理其它事情。 
為了讓驅動程式能在進行 DMA 之類工作時，可以明確釋放 CPU 使用權，所以 kernel 就提供了 sleep 與 wake up 的功能。 

`Sleep 有危險 `

呼叫 sleep_on() 函式後，就能在 kernel 之內進入 sleep，「進入sleep」具體來說是：
- 中斷 user context 要求 kernel 做的事情，明確把 CPU 使用權交給另一個 user context

這個函式無法在 interrupt context 中使用： 

```c
void sleep_on(wait_queue_head_t *q); 
```

引數要傳入 wait_queue_head_t 的指標，這個指標需先以 init_waitqueue_head() 初始化： 
```c
void init_waitqueue_head(wait_queue_head_t *q); 
```


sleep.c - 這段程式碼很危險，只要 sleep ，就會醒不來，因此無法卸載驅動程式，要復原只要重新開機一途，嚴格來說，這個程式連 insmod 都無法執行完成。


```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

static wait_queue_head_t wait;

static int sample_init(void)
{
    printk("driver loaded\n");

    init_waitqueue_head(&wait);
    sleep_on(&wait);

    return 0;
}

static void sample_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

執行範例： 

```c
# sudo /sbin/insmod ./sleep.ko 
# ps aux | grep insmod 
# kill -9 #pid 
# ps aux | grep insmod 
```


`Wake up `

要讓睡著的人起來，一定要別人叫它，負責個工作的是 wake_up()。 
wake_up() 的引數跟傳給 sleep_on() 的是同一個指標，但是如果在呼叫 sleep_on() 之前就呼叫 wake_up() 的話，則什麼都不會做。 
以下範例是 kernel module 呼叫 sleep_on() ，然後由另一個 kernel module 呼叫 wake_up() 把它叫醒的例子，其中為了讓其它模組也看得到 wait queue，所以用 EXPORT_SYMBOL 巨集將之匯出。 

```c
//
// devone.c
//
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

static int devone_devs = 1;
static int devone_major = 0;   // dynamic allocation
static struct cdev devone_cdev;

wait_queue_head_t yutaka_sample_wait;
EXPORT_SYMBOL(yutaka_sample_wait);

ssize_t devone_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    int i;
    unsigned char val = 0xff;
    int retval;

    init_waitqueue_head(&yutaka_sample_wait);
    sleep_on(&yutaka_sample_wait);

    printk("Wakeup! (%s)\n", __func__);

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

struct file_operations devone_fops = {
    .read = devone_read,
};

static int devone_init(void)
{
    dev_t dev = MKDEV(devone_major, 0);
    int ret;
    int major;
    int err;

    ret = alloc_chrdev_region(&dev, 0, devone_devs, "devone");
    if (ret < 0)
        return ret;
    devone_major = major = MAJOR(dev);

    cdev_init(&devone_cdev, &devone_fops);
    devone_cdev.owner = THIS_MODULE;
    devone_cdev.ops = &devone_fops;
    err = cdev_add(&devone_cdev, MKDEV(devone_major, 0), 1);
    if (err)
        return err;

    printk(KERN_ALERT "devone driver(major %d) installed.\n", major);

    return 0;
}

static void devone_exit(void)
{
    dev_t dev = MKDEV(devone_major, 0);

    printk(KERN_ALERT "devone driver removed.\n");

    cdev_del(&devone_cdev);

    unregister_chrdev_region(dev, devone_devs);

}

module_init(devone_init);
module_exit(devone_exit);

```

- wakeup.c

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

extern wait_queue_head_t yutaka_sample_wait;

static int sample_init(void)
{
    wait_queue_head_t *wq = &yutaka_sample_wait;

    printk("driver loaded\n");

    wake_up(wq);

    return 0;
}

static void sample_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);

```
執行範例： 

```c
# sudo /sbin/insmod ./devone.ko 
# mknod --mode=644 /dev/devone c `grep devone /proc/devices | awk '{print $1;}'` 0 
# ls -l /dev/devone 
# hexdump -C -n 16 /dev/devone 
# sudo /sbin/insmod ./wakeup.ko 
```

`禁止使用 sleep_on() `

從前面的例子可以發現，如果能保証在 sleep_on() 執行之後再呼叫 wake_up() 的話，就不會有問題，但如果先執行 wake_up() 再執行 sleep_on()，則就沒有人會去叫醒 sleep_on()，就算是送 KILL 信號給 process 也無法解除睡眠，所以目前 sleep_on() 正在朝著廢除的方向前進。

`正確的 Sleep 方式 `

sleep 與 wake up 的正確作法已經定型：
- 以 wait_event_timeout() 入睡
- 以 wake_up() 喚醒

wait_event_timeout() 會：
- 在滿足 wake up 條件的時候解除睡眠
- 未滿足 wake up 條件的時候保持睡眠
- 經過一段時間後，自動解除睡眠

使用這個函式，可以避免「錯過呼叫時間」而造成的「永久睡眠」問題，也可以在不動用 kernel timer 的情況下，達成 timeout 的效果： 
```c
long wait_event_timeout(wait_queue_head_t wq, condition, long timeout); 
```

wait_event_timeout巨集定義在 linux/wait.h，第一引數是 wait_queue_head_t 變數，第二引數則是 process 喚醒的條件，第三引數是時間(以 jiffies 為單位). 
wait_event_timeout() 會在滿足條件或是被喚醒之前讓 process 保持 sleep 狀態，但如果被喚醒之後，條件還是沒成立的話，就會再次落入 sleep 狀態。 
另外，也有可被 signal 解除 sleep 的 wait_event_interruptiable_timeout() 巨集可供使用： 
```c
long wait_event_interruptible_timeout(wait_queue_head_t wq, condition, long timeout); 
```

- main.c - wait_event_timeout() 的範例程式 

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

#define SLEEP_TIMEOUT  (6*HZ)
#define WAKEUP_TIMEOUT (5*HZ)

static struct timer_list tickfn;
static wait_queue_head_t wait;
static int condition = 0;

static void sample_timeout(unsigned long arg)
{
    wait_queue_head_t *wq = (wait_queue_head_t *)arg;

    printk("wq %p (%s)\n", wq, __func__);
    condition = 1;
    wake_up(wq);
}

static int sample_init(void)
{
    long val;

    printk("driver loaded: %p\n", &tickfn);

    init_waitqueue_head(&wait);

    init_timer(&tickfn);
    tickfn.function = sample_timeout;
    tickfn.data = (unsigned long)&wait;
    tickfn.expires = jiffies + WAKEUP_TIMEOUT;
    add_timer(&tickfn);

    condition = 0;
    val = wait_event_timeout(wait, condition, SLEEP_TIMEOUT);
    if (val == 0) { /* timeout */
        printk("Timeout ocurred. (%s)\n", __func__);
    }

    return 0;
}

static void sample_exit(void)
{
    del_timer_sync(&tickfn);

    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);

```

`驗証呼叫時機 `
wait_event_timeout() 與 wake_up() 的呼叫時機有各種變化，不管是以哪種順序呼叫，驅機程式都不能出問題。 
如果是因為硬體故障之類的原因，導致不停產生中斷，這種情形下，驅動程式也要有適當的應對才行。 

`I2C驅動程式 `
看不懂…目前。 

6-7、向 TTY 主控臺輸出訊息 
使用 prink() 可以把訊息送到 kernel buffer，但沒辦法顯示到使用者的終端機上。 
而應用程式在呼叫 printf() 把資料顯示到畫面上時，也是因為應用程式透過 shell 執行，而 shell 控制著 TTY(Tele-Typewriter) 的緣故。 
所以當程式切離 shell 成為 daemon (父程序變成 init process) 的時候，呼叫 printf() 也會失去作用。 
但如果限定在這種情形下的話：
- 驅動程式在 user context 運作

就可以讓驅動程式向使用者的終端機送出訊息，取得 user context 的方式是 current 巨集(linux/sched.h)，TTY 的指標取得方式如下： 

```c
courrnt->signal->tty 
```

tty.c - 傳送訊息到 tty 的範例 

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

static void hook_tty_console(char *msg)
{
    struct tty_struct *tty;

    tty = current->signal->tty;
    if (tty != NULL) {
        ((tty->driver)->write)(tty, msg, strlen(msg));
        ((tty->driver)->write)(tty, "\r\n", 2);
    }
}

static int sample_init(void)
{
    printk("driver loaded \n");

    hook_tty_console("This message is written by sample driver");

    return 0;
}

static void sample_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```
###6-8、Interrupt Hook 
在硬體產生中斷時，就會呼叫這個 IRQ (Interrupt Request) 對應的驅動程式，IRQ 的分配情形可到 /proc/interrupts 查閱。 <br>
IRQ 可以同時分配給多個不同的驅動程式，IRQ 在驅動程式之間是可以共享的。 <br>
中斷處理程序的登記工作可透過 request_irq() 完成，在 kernel 收到 IRQ 時，會呼叫所有對應的中斷處理程式。 <br>
舉個例子，來對網路卡驅動程式的 IRQ16 登記範例驅動程式內的中斷處理程序看看： 


```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

#define IRQ_NUM 17

static irqreturn_t sample_intr(int irq, void *dev_id)
{
    printk("irq %d dev_id %p (%s)\n", irq, dev_id, __func__);

    return IRQ_NONE;
}

static int sample_init(void)
{
    printk("driver loaded \n");

    if (request_irq(IRQ_NUM,
                    sample_intr,
                    IRQF_SHARED,
                    "sample_intr",
                    (void *)sample_intr)) {

    }

    return 0;
}

static void sample_exit(void)
{
    free_irq(IRQ_NUM, (void *)sample_intr);

    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

執行範例： 
```c
# cat /proc/interrupts 
# sudo /sbin/insmod ./devone.ko 
# sudo tail -f /var/log/messages 
# dmesg | tail 
# sudo /sbin/rmmod devone 
# cat /proc/interrupts 
```


### 6-9、結語 
如果 user process 透過 IOCTL 控制程式使用 select() 與 poll() 之類的系統呼叫的話，驅動程式這邊也必須配合實作，另外如果要在 /proc 提供資料時，也必須配合修改驅動程式。 
接著是 sleep 與 wake up 的手法，如果沒有慎重地進行設計，很可能會讓 user process 完全無法運作。

