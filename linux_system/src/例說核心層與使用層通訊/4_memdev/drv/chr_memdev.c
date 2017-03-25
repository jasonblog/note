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

static int chrmem_ioctl(struct inode* inode, struct file* filp,
                        unsigned int cmd, unsigned long arg)
{
    struct chrmem_dev* dev = filp->private_data;

    printk(KERN_ALERT"=========%s()=========\n", __func__);

    switch (cmd) {
    case MEM_CLEAR:
        memset(dev->data, 0, CHR_MEMDEV_DATA_SIZE);
        break;

    case MEM_RESET:
        snprintf(dev->data, CHR_MEMDEV_DATA_SIZE, "%s", "hello, user!");
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

static ssize_t chrmem_read(struct file* filp, char __user* buf, size_t size,
                           loff_t* ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct chrmem_dev* dev = filp->private_data;

    printk(KERN_ALERT"=========%s()=========\n", __func__);

    if (p >= CHR_MEMDEV_DATA_SIZE) {
        return 0;
    }

    if (count > CHR_MEMDEV_DATA_SIZE - p) {
        return 0;
    }

    if (copy_to_user(buf, (void*)(dev->data + p), count)) {
        return -EINVAL;
    } else {
        *ppos += count;
        ret = count;
    }

    return ret;
}

static ssize_t chrmem_write(struct file* filp, const char __user* buf,
                            size_t size, loff_t* ppos)
{
    unsigned long p = *ppos;
    unsigned int count = size;
    int ret = 0;
    struct chrmem_dev* dev = filp->private_data;

    printk(KERN_ALERT"=========%s()=========\n", __func__);

    if (p >= CHR_MEMDEV_DATA_SIZE) {
        return 0;
    }

    if (count > CHR_MEMDEV_DATA_SIZE - p) {
        count = CHR_MEMDEV_DATA_SIZE - p;
    }

    if (copy_from_user(dev->data + p, buf, count)) {
        ret = -EINVAL;
    } else {
        *ppos += count;
        ret = count;
    }

    return ret;
}

static loff_t chrmem_llseek(struct file* filp, loff_t offset, int orig)
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

        if ((unsigned int) offset > CHR_MEMDEV_DATA_SIZE) {
            ret = -EINVAL;
            break;
        }

        filp->f_pos = (unsigned int) offset;
        ret = filp->f_pos;
        break;

    case 1:
        if ((filp->f_pos + offset) > CHR_MEMDEV_DATA_SIZE) {
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

    //      vma->vm_flags |= VM_IO;
    //      vma->vm_flags |= VM_RESERVED;


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

    /* 1. alloc char device No. */
    result = alloc_chrdev_region(&devno, 0, 1, "chrmem_dev");

    if (result < 0) {
        return result;
    }

    // 2. kmalloc device structure memory
    chrmem_devp = kmalloc(CHR_MEMDEV_NUM * sizeof(struct chrmem_dev), GFP_KERNEL);

    if (!chrmem_devp) {
        result =  - ENOMEM;
        goto err;
    }

    memset(chrmem_devp, 0, sizeof(struct chrmem_dev));

    /*3.Initial cdev*/
    cdev_init(&chrmem_devp->cdev, &chrmem_fops);
    chrmem_devp->cdev.owner = THIS_MODULE;

    /*4. add char device */
    chrmem_major = MAJOR(devno);
    cdev_add(&chrmem_devp->cdev, MKDEV(chrmem_major, 0), CHR_MEMDEV_NUM);

    /*4. Initial device memory data*/
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
