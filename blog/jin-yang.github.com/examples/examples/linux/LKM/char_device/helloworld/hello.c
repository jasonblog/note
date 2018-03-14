#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/fcntl.h>
#include <linux/module.h>


int device_major = 240;
int device_minor = 0;
struct cdev    cdev;

static int device_open(struct inode *inode, struct file *flip)
{
    int num = MINOR(inode->i_rdev);
    printk("hello char device open, minor: %d\n", num);
    return 0;
}
static ssize_t device_read(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)
{
    printk("hello char device read, buf: 0x%p count 0x%08X, pos %lld\n",
            buf, (unsigned int)count, *f_ops);
	return 0;
}
static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
    printk("hello char device write, buf: 0x%p count 0x%08X, pos %lld\n",
            buf, (unsigned int)count, *f_ops);
	return count;
}
static int device_close(struct inode *inode, struct file *flip)
{
    printk("hello char device close\n");
    return 0;
}
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .open = device_open,
    .write = device_write,
    .release = device_close,
};

static int __init mydevice_init(void)
{
    int ret;
    dev_t dev = 0;
    printk(KERN_ALERT "hello char module\n");

    // 1. allocate the device.
    if (device_major) {
        dev = MKDEV(device_major, device_minor);
        ret = register_chrdev_region(dev, 1, "hello_char");
    } else {
        ret = alloc_chrdev_region(&dev, device_minor, 1, "hello_char");
        device_major = MAJOR(dev);
    }
    if (ret < 0) {
        printk(KERN_WARNING "hello_char: can't get major %d\n", device_major);
        goto err_register_chrdev_region;
    }

    // 2. set up the dev.
    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;
    if ((ret = cdev_add(&cdev, dev, 1))) {
        printk(KERN_NOTICE "Error %d adding hello_char\n", ret);
        goto err_cdev_add;
    }
#if 0
    ret = register_chrdev(device_major, "hello_char", &fops);
    if (ret < 0) {
        printk("Unable to register character device %d!\n", device_major);
        return ret;
    }
#endif

    return 0; // everything's ok

err_cdev_add:
	unregister_chrdev_region(dev, 1);
err_register_chrdev_region:
	return ret;
}

static void __exit mydevice_exit(void)
{
    dev_t dev;
    printk(KERN_ALERT "bye hello_char module\n");
    dev = MKDEV(device_major, device_minor);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
    //unregister_chrdev(device_major,  "simple");
}

module_init (mydevice_init);
module_exit (mydevice_exit);
MODULE_LICENSE("Dual BSD/GPL");
