# （四）：映射设备内核空间到用户态


一个进程的内存映象由下面几部分组成：代码段、数据段、BSS段和堆栈段，以及内存映射的区域等部分，内存映射函数mmap(), 负责把文件内容映射到进程的虚拟内存空间, 通过对这段内存的读取和修改，来实现对文件的读取和修改，而文件可以是设备驱动文件节点。通过把内核驱动的内存空间映射到应用层，可以实现应用和内核空间的数据交换。

linux设备分三种，字符设备、块设备、网络接口设备。每一个字符设备或块设备都在/dev目录下对应一个设备文件。linux用户态程序通过设备文件（或称设备节点）来使用驱动程序操作字符设备和块设备。

本节使用字符设备驱动为例来实现映射，有关字符设备驱动相关内容可参考作者这篇文章：
http://blog.csdn.net/shallnet/article/details/17734309
实现内存映射的关键在于实现字符设备驱动的mmap()函数，mmap()函数原型为:


```c
#include <sys/mman.h>  
void *mmap(void *addr, size_t length, int prot, int flags,  int fd, off_t offset);  
```

该函数负责把文件内容映射到进程的虚拟地址空间，通过对这段内存的读取和修改来实现对文件的读取和修改，而不需要再调用read和write；

mmap方法是file_operations结构的成员，在mmap系统调用的发出时被调用。mmap设备方法所需要做的就是建立虚拟地址到物理地址的页表。其实在在我们调用系统调用mmap时，内核中的sys_mmap函数首先根据用户提供给mmap的参数（如起始地址、空间大小、行为修饰符等）创建新的vma。然后再调用相应文件的file_operations中的mmap函数。

进程虚拟地址空间相关内容可参考作者这篇文章：
http://blog.csdn.net/shallnet/article/details/47701225
使用remap_pfn_range()函数将设备内存线性地映射到用户地址空间中。该函数原型为：


```c
/**
 * remap_pfn_range - remap kernel memory to userspace
 * @vma: user vma to map to
 * @addr: target user address to start at
 * @pfn: physical address of kernel memory
 * @size: size of map area
 * @prot: page protection flags for this mapping
 *
 *  Note: this is only safe if the mm semaphore is held when called.
 */
int remap_pfn_range(struct vm_area_struct* vma, unsigned long addr,
                    unsigned long pfn, unsigned long size, pgprot_t prot)

```

其中vma为虚拟内存区域，在一定范围内的页将被映射到该区域内。
addr为重新映射时的起始地址，该函数为处于addr和addr+size之间的虚拟地址建立页表。
pfn为与物理内存对于的页帧号，页帧号只是将物理地址右移PAGE_SHIFT位。
size为以字节为单位，被重新映射的大小。
prot为新VMA要求的“保护”属性。
下面看一看file_operations中的mmap成员的实现：


```c
static struct vm_operations_struct sln_remap_vm_ops = {  
    .open   = sln_vma_open,  
    .close  = sln_vma_close  
};  
  
static int chrmem_dev_mmap(struct file*filp, struct vm_area_struct *vma)  
{  
      struct mem_dev *dev = filp->private_data;  
       
      if (remap_pfn_range(vma,vma->vm_start,virt_to_phys(dev->data)>>PAGE_SHIFT, vma->vm_end - vma->vm_start, vma->vm_page_prot))  
          return  -EAGAIN;  
                  
    vma->vm_ops = &sln_remap_vm_ops;  
  
    sln_vma_open(vma);  
      return 0;  
}
```

该函数中函数page_to_pfn(shm_page)将表示物理页面的page结构转换为其对应的页帧号。该字符设备驱动的主要思想是建立一个字符设备，在它的驱动程序中申请一块物理内存区域，并利用mmap将这段物理内存区域映射到进程的地址空间中。该驱动源码如下：


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
#include "chr_memdev.h"

int                 chrmem_major;
struct chrmem_dev*   chrmem_devp;

int chrmem_open(struct inode* inode, struct file* filp)
{
    filp->private_data = chrmem_devp;

    return 0;
}

void sln_vma_open(struct vm_area_struct* vma)
{
    printk("===vma_open: %s===\n", chrmem_devp->data);
}

void sln_vma_close(struct vm_area_struct* vma)
{
    printk("===vma_close: %s===\n", chrmem_devp->data);
}

static struct vm_operations_struct sln_remap_vm_ops = {
    .open   = sln_vma_open,
    .close  = sln_vma_close
};

int chrmem_release(struct inode* inode, struct file* filp)
{
    return 0;
}

static int chrmem_dev_mmap(struct file* filp, struct vm_area_struct* vma)
{
    struct chrmem_dev* dev = filp->private_data;

    if (remap_pfn_range(vma, vma->vm_start, virt_to_phys(dev->data) >> PAGE_SHIFT,
                        vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        return  -EAGAIN;
    }

    vma->vm_ops = &sln_remap_vm_ops;

    sln_vma_open(vma);
    return 0;
}

static const struct file_operations chrmem_fops = {
    .owner    = THIS_MODULE,
    .open     = chrmem_open,
    .release  = chrmem_release,
    .read     = chrmem_read,
    .write    = chrmem_write,
    .llseek   = chrmem_llseek,
    .ioctl    = chrmem_ioctl,
    .mmap     = chrmem_dev_mmap

};

static int chrmem_dev_init(void)
{
    int result;
    dev_t devno;

    /* 分配设备号 */
    result = alloc_chrdev_region(&devno, 0, 1, "chrmem_dev");

    if (result < 0) {
        return result;
    }


    // 为自定义设备结构体分配内存空间
    
    mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);

    if (!mem_devp) {
        result =  -ENOMEM;
        goto err;
    }

    memset(mem_devp, 0, sizeof(struct mem_dev));

    /*初始化字符设备*/
    cdev_init(&mem_devp->cdev, &mem_fops);
    mem_devp->cdev.owner = THIS_MODULE;

    /*添加注册字符设备 */
    mem_major = MAJOR(devno);
    cdev_add(&mem_devp->cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

    /*初始化自定义设备数据内容*/
    mem_devp->data = kmalloc(MEMDEV_SIZE, GFP_KERNEL);
    memset(mem_devp->data, '*', MEMDEV_SIZE / 100);

    return 0;

err:
    unregister_chrdev_region(devno, 1);

    return result;
}

static int chrmem_dev_init(void)
{
    int result;
    dev_t devno;

    /* 分配设备号 */
    result = alloc_chrdev_region(&devno, 0, 1, "chrmem_dev");

    if (result < 0) {
        return result;
    }

    // 为自定义设备结构体分配内存空
    chrmem_devp = kmalloc(CHR_MEMDEV_NUM * sizeof(struct chrmem_dev), GFP_KERNEL);

    if (!chrmem_devp) {
        result =  - ENOMEM;
        goto err;
    }

    memset(chrmem_devp, 0, sizeof(struct chrmem_dev));

    /*初始化字符设备*/
    cdev_init(&chrmem_devp->cdev, &chrmem_fops);
    chrmem_devp->cdev.owner = THIS_MODULE;

    /*添加注册字符设备 */
    chrmem_major = MAJOR(devno);
    cdev_add(&chrmem_devp->cdev, MKDEV(chrmem_major, 0), CHR_MEMDEV_NUM);

    /*初始化自定义设备数据内容*/
    chrmem_devp->data = kmalloc(CHR_MEMDEV_DATA_SIZE, GFP_KERNEL);
    memset(chrmem_devp->data, '*', CHR_MEMDEV_DATA_SIZE / 100);

    return 0;

err:
    unregister_chrdev_region(devno, 1);

    return result;
}

static void chrmem_dev_exit(void)
{
    cdev_del(&chrmem_devp->cdev); //delete device
    kfree(chrmem_devp); // release device memory
    unregister_chrdev_region(MKDEV(chrmem_major, 0),
                             1); // unregister char device No.
}

module_init(chrmem_dev_init);
module_exit(chrmem_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

在应用程序中调用mmap来实现内存映射，应用程序代码如下：


```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#define SHR_MEMSIZE             4096
#define MEM_CLEAR               0x0
#define MEM_RESET               0x1
#define MEM_DEV_FILENAME       "/dev/sln_memdev"

int main()
{
    int         fd;
    char*        shm = NULL;

    fd = open(MEM_DEV_FILENAME, O_RDWR);

    if (fd < 0) {
        printf("open(): %s\n", strerror(errno));
        return -1;
    }

    shm = mmap(NULL, SHR_MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (MAP_FAILED == shm) {
        printf("mmap: %s\n", strerror(errno));
    }

    printf("Before Write, shm = %s\n", shm);

    strcpy(shm, "User write to share memory!");

    printf("After write, shm = %s\n", shm);

    if (0 > ioctl(fd, MEM_CLEAR, NULL)) {
        printf("ioctl: %s\n", strerror(errno));
        return -1;
    }

    printf("After clear, shm = %s\n", shm);

    if (0 > ioctl(fd, MEM_RESET, NULL)) {
        printf("ioctl: %s\n", strerror(errno));
        return -1;
    }

    printf("After reset, shm = %s\n", shm);

    munmap(shm, SHR_MEMSIZE);
    close(fd);
    return 0;
}
```

应用程序在实现映射之后，首先读取输出共享内存内容，然后写入，然后清空该共享内存内容以及重设共享内存。在编译驱动和应用程序之后首先插入驱动，在创建设备节点，最后运行应用程序看是否成功，如下:

```sh
# insmod memdev.ko  
# cat /proc/devices | grep chrmem_dev  
248 chrmem_dev  
# mknod  /dev/sln_memdev c 248 0  
# ls  
app  app_read  drv  Makefile  mem_app  memdev.ko  read_app  
# ./mem_app  
Before Write, shm = ****************************************  
After write, shm = User write to share memory!  
After clear, shm =  
After reset, shm = hello, user!  
#  
```

可以看到字符设备驱动的内核空间被成功映射到用户态，现在用户空间的一段内存关联到设备内存上，对用户空间的读写就相当于对字符设备的读写。

本节源码下载：
http://download.csdn.net/detail/gentleliu/9035831

