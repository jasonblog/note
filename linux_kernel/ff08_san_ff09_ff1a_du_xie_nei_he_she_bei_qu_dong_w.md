# （三）：讀寫內核設備驅動文件


讀寫設備文件也就是調用系統調用read()和write()，系統調用就是內核提供給應用程序的接口，應用程序對底層的操作大部分都是通過系統調用來完成。幾乎所有的系統調用都涉及到內核和應用的數據交換，本節並非講述如何添加一個系統調用（那是第一節的內容），而是講解如何利用現有系統調用來實現特定的內核與應用交互需求。

本節將建立一個字符設備驅動來作為應用和內核之間數據通信的渠道，字符設備驅動有關信息可以參考作者這篇文章：

http://blog.csdn.net/shallnet/article/details/17734309

建立字符設備驅動有如下步驟：

第一步、註冊設備號。
可以使用如下函數分別靜態和動態註冊:

```c
int register_chrdev_region(dev_t from, unsigned count, const char *name)；   
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)；  
```

第二步、初始化設備結構體。
使用函數：


```c
void cdev_init(struct cdev *cdev, const struct file_operations *fops)；  
```

在調用該函數之前，還需要初始化file_operations結構體，該結構體成員函數是字符設備驅動設計的主要內容，當在應用上調用read和write等函數時，該結構體中對應的函數會被調用。
       
第三步、添加設備。
使用函數:

```c
int cdev_add(struct cdev *p, dev_t dev, unsigned count)； 
```

第四步、實現file_operations結構體中open()、read()、write()、ioctl()等函數。

file_operations中的read()和write()函數，就是用來在驅動程序和應用程序間交換數據的。通過數據交換，驅動程序和應用程序可以彼此瞭解對方的情況。但是驅動程序和應用程序屬於不同的地址空間。驅動程序不能直接訪問應用程序的地址空間；同樣應用程序也不能直接訪問驅動程序的地址空間，否則會破壞彼此空間中的數據，從而造成系統崩潰，或者數據損壞。安全的方法是使用內核提供的專用函數，完成數據在應用程序空間和驅動程序空間的交換。這些函數對用戶程序傳過來的指針進行了嚴格的檢查和必要的轉換，從而保證用戶程序與驅動程序交換數據的安全性。這些函數有：


```c
unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);  
unsigned long copy_from_user(void *to, const void __user *from, unsigned long n); put_user(local,user);  
get_user(local,user);  
```

當不是該設備時，應當刪除該設備、釋放申請的設備號。


```c
void cdev_del(struct cdev *dev);  
void unregister_chrdev_region(dev_t from, unsigned count)；  
```

這兩函數一般在卸載模塊中調用。
下面看一下字符設備的完整實現代碼：


```c
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include "chrdev.h"

int             chr_major;
struct chr_dev*  chr_devp;

int chr_open(struct inode* inode, struct file* filp)
{
    filp->private_data = chr_devp;

    return 0;
}

static int chr_ioctl(struct inode* inode, struct file* filp, unsigned int cmd,
                     unsigned long arg)
{
    struct chr_dev* dev = filp->private_data;

    printk(KERN_ALERT"=========%s()=========\n", __func__);

    switch (cmd) {
    case MEM_CLEAR:
        memset(dev->data, 0, CHAR_DEV_DATA_SIZE);
        break;

    case MEM_RESET:
        snprintf(dev->data, CHAR_DEV_DATA_SIZE, "%s", "hello, user!");
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static ssize_t chr_read(struct file* filp, char __user* buf, size_t size,
                        loff_t* ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct chr_dev* dev = filp->private_data;

    printk(KERN_ALERT"=========%s()=========\n", __func__);

    if (p >= CHAR_DEV_DATA_SIZE) {
        return 0;
    }

    if (count > CHAR_DEV_DATA_SIZE - p) {
        return 0;
    }

    //將內核中數據dev->data，讀取到用戶空間buf中，讀取count字節
    if (copy_to_user(buf, (void*)(dev->data + p), count)) {
        return -EINVAL;
    } else {
        *ppos += count;
        ret = count;
    }

    return ret;
}

static ssize_t chr_write(struct file* filp, const char __user* buf, size_t size,
                         loff_t* ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct chr_dev* dev = filp->private_data;

    printk(KERN_ALERT"=========%s()=========\n", __func__);

    if (p >= CHAR_DEV_DATA_SIZE) {
        return 0;
    }

    if (count > CHAR_DEV_DATA_SIZEE - p) {
        count = CHAR_DEV_DATA_SIZE - p;
    }

    //將用戶空間buf中數據copy到內核空間dev->data中，copy count字節數據
    if (copy_from_user(dev->data + p, buf, count)) {
        ret = -EINVAL;
    } else {
        *ppos += count;
        ret = count;
    }

    return ret;
}

static loff_t chr_llseek(struct file* filp, loff_t offset, int orig)
{
    loff_t ret = 0;

    printk(KERN_ALERT"=========%s()=========\n", __func__);

    /* orig can be SEEK_SET, SEEK_CUR, SEEK_END */
    switch (orig) {
    case 0:
        if (offset < 0) {
            ret = -EINVAL;
            break;
        }

        if ((unsigned int) offset > CHAR_DEV_DATA_SIZE) {
            ret = -EINVAL;
            break;
        }

        filp->f_pos = (unsigned int) offset;
        ret = filp->f_pos;
        break;

    case 1:
        if ((filp->f_pos + offset) > CHAR_DEV_DATA_SIZE) {
            ret = -EINVAL;
            break;
        }

        if ((filp->f_pos + offset) < 0) {
            ret = -EINVAL;
            break;
        }

        filp->f_pos += offset;
        ret = filp->f_pos;
        break;

    default:
        ret = - EINVAL;
        break;
    }

    return ret;
}

int chr_release(struct inode* inode, struct file* filp)
{
    return 0;
}

static const struct file_operations chr_fops = {
    .owner    = THIS_MODULE,
    .open     = chr_open,
    .release  = chr_release,
    .read     = chr_read,
    .write    = chr_write,
    .llseek   = chr_llseek,
    .ioctl    = chr_ioctl,

};

static int chr_dev_init(void)
{
    int result;
    dev_t devno;

    /* 註冊設備號 */
    result = alloc_chrdev_region(&devno, 0, 1, "chardev");

    if (result < 0) {
        return result;
    }

    // 分配自定義設備結構體內存
    chr_devp = kmalloc(CHAR_DEV_NO * sizeof(struct chr_dev), GFP_KERNEL);

    if (!chr_devp) {
        result =  - ENOMEM;
        goto err;
    }

    memset(chr_devp, 0, sizeof(struct chr_dev));

    /*初始化設備*/
    cdev_init(&chr_devp->cdev, &chr_fops);
    chr_devp->cdev.owner = THIS_MODULE;

    /* 添加設備 */
    chr_major = MAJOR(devno);
    cdev_add(&chr_devp->cdev, MKDEV(chr_major, 0), CHAR_DEV_NO);

    /*初始自定義設備結構體內存數據*/
    chr_devp->data = kmalloc(CHAR_DEV_DATA_SIZE, GFP_KERNEL);
    memset(chr_devp->data, '*',
           CHAR_DEV_DATA_SIZE / 100);  //為避免輸出太多影響結果顯示，此處僅僅初始化40個字節。

    return 0;

err:
    unregister_chrdev_region(devno, 1);

    return result;
}

static void chr_dev_exit(void)
{
    cdev_del(&chr_devp->cdev); //delete device
    kfree(chr_devp); // release device memory
    unregister_chrdev_region(MKDEV(chr_major, 0),
                             1); // unregister char device No.
}

module_init(chr_dev_init);
module_exit(chr_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

應用程序實現代碼如下：


```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#define SHR_MEMSIZE           4096
#define MEM_CLEAR               0x0
#define MEM_RESET               0x1
#define CHAR_DEV_FILENAME       "/dev/sln_chardev"

int main()
{
    int     fd;
    char    shm[SHR_MEMSIZE];

    /* 打開設備文件 */
    fd = open(CHAR_DEV_FILENAME, O_RDWR);

    if (fd < 0) {
        printf("open <%s> failed!\n", CHAR_DEV_FILENAME);
        return -1;
    }

    /* 直接設置共享內存數據 */
    snprintf(shm, sizeof(shm), "this data is writed by user!");

    /* 寫入數據 */
    printf("======== Write data========\n");

    if (write(fd, shm, strlen(shm)) < 0) {
        printf("write(): %s\n", strerror(errno));
        return -1;
    }

    /* 再讀取數據，以驗證應用上設置成功 */
    printf("======== Read data========\n");

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("llseek(): %s\n", strerror(errno));
        return -1;
    }

    if (read(fd, shm, SHR_MEMSIZE) < 0) {
        printf("read(): %s\n", strerror(errno));
        return -1;
    }

    printf("read data: %s\n", shm);

    /* 再清空數據之後再讀取 */
    printf("========= Clear it now: =======\n");

    if (ioctl(fd, MEM_CLEAR, NULL) < 0) {
        printf("ioctl(): %s\n", strerror(errno));
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("llseek(): %s\n", strerror(errno));
        return -1;
    }

    if (read(fd, shm, SHR_MEMSIZE) < 0) {
        printf("read(): %s\n", strerror(errno));
        return -1;
    }

    printf("read data: %s\n", shm);


    /* reset all data, read it and check whether it is ok */
    printf("========= Reset it now: =======\n");

    if (ioctl(fd, MEM_RESET, NULL) < 0) {
        printf("ioctl(): %s\n", strerror(errno));
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("llseek(): %s\n", strerror(errno));
        return -1;
    }

    if (read(fd, shm, SHR_MEMSIZE) < 0) {
        printf("read(): %s\n", strerror(errno));
        return -1;
    }

    printf("read data: %s\n", shm);

    close(fd);
    return 0;
}
```

在編譯驅動和應用程序之後就可以驗證內核和應用的數據交換是否成功了。運行驗證    如下：


```sh
# insmod chardev.ko  //首先插入模塊  
# cat /proc/devices | grep chardev //查看驅動模塊對應主設備號  
248 chardev  
# mknod  /dev/sln_chardev c 248 0     //為設備創建對應節點  
```

然後再運行應用程序：

```sh
# ./read_app  
======== Write data========  
======== Read data========  
read data: this data is writed by user!************     //讀取應用設置數據成功  
========= Clear it now: =======     //在內核清空數據，應用讀取數據為空  
read data:  
========= Reset it now: =======     //在內核重設空間數據為hello, user!，應用上再讀取該數據，輸出預期值！  
read data: hello, user!  
```

本節只實現了file_operations結構體中部分函數，在後面我們還可以實現其它的函數，也可以實現內核和應用交互數據。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/9035821




