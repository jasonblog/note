# （三）：读写内核设备驱动文件


读写设备文件也就是调用系统调用read()和write()，系统调用就是内核提供给应用程序的接口，应用程序对底层的操作大部分都是通过系统调用来完成。几乎所有的系统调用都涉及到内核和应用的数据交换，本节并非讲述如何添加一个系统调用（那是第一节的内容），而是讲解如何利用现有系统调用来实现特定的内核与应用交互需求。

本节将建立一个字符设备驱动来作为应用和内核之间数据通信的渠道，字符设备驱动有关信息可以参考作者这篇文章：

http://blog.csdn.net/shallnet/article/details/17734309

建立字符设备驱动有如下步骤：

第一步、注册设备号。
可以使用如下函数分别静态和动态注册:

```c
int register_chrdev_region(dev_t from, unsigned count, const char *name)；   
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name)；  
```

第二步、初始化设备结构体。
使用函数：


```c
void cdev_init(struct cdev *cdev, const struct file_operations *fops)；  
```

在调用该函数之前，还需要初始化file_operations结构体，该结构体成员函数是字符设备驱动设计的主要内容，当在应用上调用read和write等函数时，该结构体中对应的函数会被调用。
       
第三步、添加设备。
使用函数:

```c
int cdev_add(struct cdev *p, dev_t dev, unsigned count)； 
```

第四步、实现file_operations结构体中open()、read()、write()、ioctl()等函数。

file_operations中的read()和write()函数，就是用来在驱动程序和应用程序间交换数据的。通过数据交换，驱动程序和应用程序可以彼此了解对方的情况。但是驱动程序和应用程序属于不同的地址空间。驱动程序不能直接访问应用程序的地址空间；同样应用程序也不能直接访问驱动程序的地址空间，否则会破坏彼此空间中的数据，从而造成系统崩溃，或者数据损坏。安全的方法是使用内核提供的专用函数，完成数据在应用程序空间和驱动程序空间的交换。这些函数对用户程序传过来的指针进行了严格的检查和必要的转换，从而保证用户程序与驱动程序交换数据的安全性。这些函数有：


```c
unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);  
unsigned long copy_from_user(void *to, const void __user *from, unsigned long n); put_user(local,user);  
get_user(local,user);  
```

当不是该设备时，应当删除该设备、释放申请的设备号。


```c
void cdev_del(struct cdev *dev);  
void unregister_chrdev_region(dev_t from, unsigned count)；  
```

这两函数一般在卸载模块中调用。
下面看一下字符设备的完整实现代码：


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

    //将内核中数据dev->data，读取到用户空间buf中，读取count字节
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

    //将用户空间buf中数据copy到内核空间dev->data中，copy count字节数据
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

    /* 注册设备号 */
    result = alloc_chrdev_region(&devno, 0, 1, "chardev");

    if (result < 0) {
        return result;
    }

    // 分配自定义设备结构体内存
    chr_devp = kmalloc(CHAR_DEV_NO * sizeof(struct chr_dev), GFP_KERNEL);

    if (!chr_devp) {
        result =  - ENOMEM;
        goto err;
    }

    memset(chr_devp, 0, sizeof(struct chr_dev));

    /*初始化设备*/
    cdev_init(&chr_devp->cdev, &chr_fops);
    chr_devp->cdev.owner = THIS_MODULE;

    /* 添加设备 */
    chr_major = MAJOR(devno);
    cdev_add(&chr_devp->cdev, MKDEV(chr_major, 0), CHAR_DEV_NO);

    /*初始自定义设备结构体内存数据*/
    chr_devp->data = kmalloc(CHAR_DEV_DATA_SIZE, GFP_KERNEL);
    memset(chr_devp->data, '*',
           CHAR_DEV_DATA_SIZE / 100);  //为避免输出太多影响结果显示，此处仅仅初始化40个字节。

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

应用程序实现代码如下：


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

    /* 打开设备文件 */
    fd = open(CHAR_DEV_FILENAME, O_RDWR);

    if (fd < 0) {
        printf("open <%s> failed!\n", CHAR_DEV_FILENAME);
        return -1;
    }

    /* 直接设置共享内存数据 */
    snprintf(shm, sizeof(shm), "this data is writed by user!");

    /* 写入数据 */
    printf("======== Write data========\n");

    if (write(fd, shm, strlen(shm)) < 0) {
        printf("write(): %s\n", strerror(errno));
        return -1;
    }

    /* 再读取数据，以验证应用上设置成功 */
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

    /* 再清空数据之后再读取 */
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

在编译驱动和应用程序之后就可以验证内核和应用的数据交换是否成功了。运行验证    如下：


```sh
# insmod chardev.ko  //首先插入模块  
# cat /proc/devices | grep chardev //查看驱动模块对应主设备号  
248 chardev  
# mknod  /dev/sln_chardev c 248 0     //为设备创建对应节点  
```

然后再运行应用程序：

```sh
# ./read_app  
======== Write data========  
======== Read data========  
read data: this data is writed by user!************     //读取应用设置数据成功  
========= Clear it now: =======     //在内核清空数据，应用读取数据为空  
read data:  
========= Reset it now: =======     //在内核重设空间数据为hello, user!，应用上再读取该数据，输出预期值！  
read data: hello, user!  
```

本节只实现了file_operations结构体中部分函数，在后面我们还可以实现其它的函数，也可以实现内核和应用交互数据。

本节源码下载：
http://download.csdn.net/detail/gentleliu/9035821




