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

    if (count > CHAR_DEV_DATA_SIZE - p) {
        count = CHAR_DEV_DATA_SIZE - p;
    }

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

    /* alloc char device No. */
    result = alloc_chrdev_region(&devno, 0, 1, "chardev");

    if (result < 0) {
        return result;
    }

    // kmalloc device structure memory
    chr_devp = kmalloc(CHAR_DEV_NO * sizeof(struct chr_dev), GFP_KERNEL);

    if (!chr_devp) {
        result =  - ENOMEM;
        goto err;
    }

    memset(chr_devp, 0, sizeof(struct chr_dev));

    /*Initial cdev*/
    cdev_init(&chr_devp->cdev, &chr_fops);
    chr_devp->cdev.owner = THIS_MODULE;

    /* add char device */
    chr_major = MAJOR(devno);
    cdev_add(&chr_devp->cdev, MKDEV(chr_major, 0), CHAR_DEV_NO);

    /*Initial device memory data*/
    chr_devp->data = kmalloc(CHAR_DEV_DATA_SIZE, GFP_KERNEL);
    memset(chr_devp->data, '*', CHAR_DEV_DATA_SIZE / 100);

    return 0;

err:
    unregister_chrdev_region(devno, 1);

    return result;
}

static void chr_dev_exit(void)
{
    cdev_del(&chr_devp->cdev); //delete device
    kfree(chr_devp); // release device memory
    unregister_chrdev_region(MKDEV(chr_major, 0), 1); // unregister char device No.
}

module_init(chr_dev_init);
module_exit(chr_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
