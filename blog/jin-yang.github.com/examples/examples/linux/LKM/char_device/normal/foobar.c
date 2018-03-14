#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "foobar.h"

int device_major = 240;
int device_minor = 0;

#define       BUFFER_SIZE   4096
char          *buffer;      // buffer to store message.
int            len;         // actual length of the message.
struct cdev    cdev;
struct class  *cls;

static int device_open(struct inode *inode, struct file *flip)
{
    int num = MINOR(inode->i_rdev);
    printk("hello char device open, minor: %d\n", num);
    return 0;
}
static ssize_t device_read(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)
{
	int n, ret;
	char *kbuf;

    printk("hello char device read, buf: 0x%p count 0x%08X, pos %lld\n",
            buf, (unsigned int)count, *f_ops);

	if(*f_ops == len)                   // no more data
		return 0;

	if(count > len - *f_ops)            // require bytes > left, just return remaining.
		n = len  - *f_ops;
	else
		n = count;

	kbuf = buffer + *f_ops;            // start from the last position.

	ret = copy_to_user(buf, kbuf, n);  // copy to user
	if(ret != 0) return -EFAULT;

	*f_ops += n;                       // update the position.

	return n;
}
static ssize_t device_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
	int n, ret;
	char *kbuf;

    printk("hello char device write, buf: 0x%p count 0x%08X, pos %lld\n",
            buf, (unsigned int)count, *f_ops);

	if(*f_ops == BUFFER_SIZE)              // no more buffer.
		return -1;

	if(count > BUFFER_SIZE - *f_ops)       // require bytes > left, just return remaining.
		n = BUFFER_SIZE - *f_ops;
	else
		n = count;

	kbuf = buffer + *f_ops;               // start with the last offset.
	ret = copy_from_user(kbuf, buf, n);   // copy from user buffer.
	if(ret != 0)
		return -EFAULT;

	*f_ops += n;                          // update the position.
	len += n;                             // update the length.
    printk("buffer: %s\n", buffer);

	return n;
}

static long device_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    printk("ioctl cmd(%d) arg(%ld) dir(%d) type(%c) nr(0x%02x) size(%d)\n", cmd, arg,
                    _IOC_DIR(cmd), _IOC_TYPE(cmd), _IOC_NR(cmd), _IOC_SIZE(cmd));

    if (_IOC_TYPE(cmd) != DEV_FOOBAR_CHAR_TYPE)
        return -EINVAL;
    if (_IOC_NR(cmd) > DEV_FOOBAR_MAXNR)
        return -EINVAL;
/*
    if (_IOC_DIR(cmd) & _IOC_READ)
        ret = access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        ret = access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    if (!ret)
        return -EFAULT;
*/

    switch(cmd) {
        case DEV_FOOBAR_RESET:
            printk("CMD: CLEAN\n");
            memset(buffer, 0, BUFFER_SIZE);
            break;

        case DEV_FOOBAR_SETVALUE:
            //ret = __get_user(len, (int *)arg);
            len = arg;
            printk("CMD:SETVALUE %d %d\n", len, ret);
            break;

        case DEV_FOOBAR_GETVALUE:
            printk("CMD:GETVALUE %d\n", len);
            ret = __put_user(len, (int *)arg);
            break;

        default:
            return -EFAULT;
    }

    return ret;
}
static loff_t device_llseek(struct file *filp, loff_t off, int whence)
{
    loff_t newpos;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = off;
        break;

      case 1: /* SEEK_CUR */
        newpos = filp->f_pos + off;
        break;

      case 2: /* SEEK_END */
        newpos = len + off;
        break;

      default: // NEW in linux3, just ignore.
        return -EINVAL;
    }
    if (newpos < 0 || newpos > BUFFER_SIZE) return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}

static int device_close(struct inode *inode, struct file *flip)
{
    printk("hello char device close\n");
    return 0;
}
static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .read           = device_read,
    .open           = device_open,
    .write          = device_write,
    .release        = device_close,
    .llseek         = device_llseek,
    .unlocked_ioctl = device_unlocked_ioctl,
};

static int __init mydevice_init(void)
{
    int ret;
    dev_t dev = 0;
	struct device *device;
    printk(KERN_ALERT "foobar char module\n");

    buffer = kzalloc(4096, GFP_KERNEL);
	if(!buffer){
		return -ENOMEM;
	}
    len = 0;

    // 1. allocate the device.
    if (device_major) {
        dev = MKDEV(device_major, device_minor);
        ret = register_chrdev_region(dev, 1, "foobar_char");
    } else {
        ret = alloc_chrdev_region(&dev, device_minor, 1, "foobar_char");
        device_major = MAJOR(dev);
    }
    if (ret < 0) {
        printk(KERN_WARNING "foobar_char: can't get major %d\n", device_major);
        goto err_register_chrdev_region;
    }

    // 2. set up the dev.
    cdev_init(&cdev, &fops);
    cdev.owner = THIS_MODULE;
    if ((ret = cdev_add(&cdev, dev, 1))) {
        printk(KERN_NOTICE "Error %d adding foobar\n", ret);
        goto err_cdev_add;
    }

	// 3. register to sysfs.
	cls = class_create(THIS_MODULE, "foobar_char");
	if(IS_ERR(cls)){
		ret = PTR_ERR(cls);
		printk(KERN_NOTICE "Fail to class_create\n");
		goto err_class_create;
	}

	// 4. export the device info to /sys/class/hello_char/dev_fifo
	device = device_create(cls, NULL, dev, NULL, "foobar_dev%d", 0);
	if(IS_ERR(device)){
		ret = PTR_ERR(device);
		printk("Fail to device_create\n");
		goto err_device_create;
	}
    return 0; // everything's ok


err_device_create:
	class_destroy(cls);
err_class_create:
	cdev_del(&cdev);
err_cdev_add:
	unregister_chrdev_region(dev, 1);
err_register_chrdev_region:
	return ret;
}

static void __exit mydevice_exit(void)
{
    dev_t dev;
    printk(KERN_ALERT "bye foobar_char module\n");
    dev = MKDEV(device_major, device_minor);
	device_destroy(cls, dev);
	class_destroy(cls);
    cdev_del(&cdev);
    unregister_chrdev_region(dev, 1);
}

module_init (mydevice_init);
module_exit (mydevice_exit);
MODULE_LICENSE("Dual BSD/GPL");
