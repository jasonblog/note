# （四）：映射設備內核空間到用戶態


一個進程的內存映象由下面幾部分組成：代碼段、數據段、BSS段和堆棧段，以及內存映射的區域等部分，內存映射函數mmap(), 負責把文件內容映射到進程的虛擬內存空間, 通過對這段內存的讀取和修改，來實現對文件的讀取和修改，而文件可以是設備驅動文件節點。通過把內核驅動的內存空間映射到應用層，可以實現應用和內核空間的數據交換。

linux設備分三種，字符設備、塊設備、網絡接口設備。每一個字符設備或塊設備都在/dev目錄下對應一個設備文件。linux用戶態程序通過設備文件（或稱設備節點）來使用驅動程序操作字符設備和塊設備。

本節使用字符設備驅動為例來實現映射，有關字符設備驅動相關內容可參考作者這篇文章：
http://blog.csdn.net/shallnet/article/details/17734309
實現內存映射的關鍵在於實現字符設備驅動的mmap()函數，mmap()函數原型為:


```c
#include <sys/mman.h>  
void *mmap(void *addr, size_t length, int prot, int flags,  int fd, off_t offset);  
```

該函數負責把文件內容映射到進程的虛擬地址空間，通過對這段內存的讀取和修改來實現對文件的讀取和修改，而不需要再調用read和write；

mmap方法是file_operations結構的成員，在mmap系統調用的發出時被調用。mmap設備方法所需要做的就是建立虛擬地址到物理地址的頁表。其實在在我們調用系統調用mmap時，內核中的sys_mmap函數首先根據用戶提供給mmap的參數（如起始地址、空間大小、行為修飾符等）創建新的vma。然後再調用相應文件的file_operations中的mmap函數。

進程虛擬地址空間相關內容可參考作者這篇文章：
http://blog.csdn.net/shallnet/article/details/47701225
使用remap_pfn_range()函數將設備內存線性地映射到用戶地址空間中。該函數原型為：


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

其中vma為虛擬內存區域，在一定範圍內的頁將被映射到該區域內。
addr為重新映射時的起始地址，該函數為處於addr和addr+size之間的虛擬地址建立頁表。
pfn為與物理內存對於的頁幀號，頁幀號只是將物理地址右移PAGE_SHIFT位。
size為以字節為單位，被重新映射的大小。
prot為新VMA要求的“保護”屬性。
下面看一看file_operations中的mmap成員的實現：


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

該函數中函數page_to_pfn(shm_page)將表示物理頁面的page結構轉換為其對應的頁幀號。該字符設備驅動的主要思想是建立一個字符設備，在它的驅動程序中申請一塊物理內存區域，並利用mmap將這段物理內存區域映射到進程的地址空間中。該驅動源碼如下：


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

    /* 分配設備號 */
    result = alloc_chrdev_region(&devno, 0, 1, "chrmem_dev");

    if (result < 0) {
        return result;
    }


    // 為自定義設備結構體分配內存空間
    
    mem_devp = kmalloc(MEMDEV_NR_DEVS * sizeof(struct mem_dev), GFP_KERNEL);

    if (!mem_devp) {
        result =  -ENOMEM;
        goto err;
    }

    memset(mem_devp, 0, sizeof(struct mem_dev));

    /*初始化字符設備*/
    cdev_init(&mem_devp->cdev, &mem_fops);
    mem_devp->cdev.owner = THIS_MODULE;

    /*添加註冊字符設備 */
    mem_major = MAJOR(devno);
    cdev_add(&mem_devp->cdev, MKDEV(mem_major, 0), MEMDEV_NR_DEVS);

    /*初始化自定義設備數據內容*/
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

    /* 分配設備號 */
    result = alloc_chrdev_region(&devno, 0, 1, "chrmem_dev");

    if (result < 0) {
        return result;
    }

    // 為自定義設備結構體分配內存空
    chrmem_devp = kmalloc(CHR_MEMDEV_NUM * sizeof(struct chrmem_dev), GFP_KERNEL);

    if (!chrmem_devp) {
        result =  - ENOMEM;
        goto err;
    }

    memset(chrmem_devp, 0, sizeof(struct chrmem_dev));

    /*初始化字符設備*/
    cdev_init(&chrmem_devp->cdev, &chrmem_fops);
    chrmem_devp->cdev.owner = THIS_MODULE;

    /*添加註冊字符設備 */
    chrmem_major = MAJOR(devno);
    cdev_add(&chrmem_devp->cdev, MKDEV(chrmem_major, 0), CHR_MEMDEV_NUM);

    /*初始化自定義設備數據內容*/
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

在應用程序中調用mmap來實現內存映射，應用程序代碼如下：


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

應用程序在實現映射之後，首先讀取輸出共享內存內容，然後寫入，然後清空該共享內存內容以及重設共享內存。在編譯驅動和應用程序之後首先插入驅動，在創建設備節點，最後運行應用程序看是否成功，如下:

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

可以看到字符設備驅動的內核空間被成功映射到用戶態，現在用戶空間的一段內存關聯到設備內存上，對用戶空間的讀寫就相當於對字符設備的讀寫。

本節源碼下載：
http://download.csdn.net/detail/gentleliu/9035831

