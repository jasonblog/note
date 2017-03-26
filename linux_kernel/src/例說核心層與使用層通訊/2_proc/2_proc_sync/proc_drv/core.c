#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define USER_ROOT_DIR   "shallnet"
#define USER_ENTRY      "file"

struct sln_kbuff {
    char    buf[1024];
};

struct proc_dir_entry*   root_dir, *entry;
struct sln_kbuff*        kbuff;

//static  struct semaphore monitor_sem;
static struct mutex mutex_sem;

static int proc_write(struct file* filep,
                      const char __user* buffer,
                      unsigned long len,
                      void* data)
{
    printk(KERN_ALERT"====%d=====\n", __LINE__);

    if (copy_from_user(kbuff->buf, buffer, len)) {
        printk(KERN_ALERT"====%d=====\n", __LINE__);
        return -1;
    }

    kbuff->buf[len] = '\0';

    //  up(&monitor_sem);
    mutex_unlock(&mutex_sem);

    return len;
}

static int proc_read(char* page,
                     char** start,
                     off_t off,
                     int count,
                     int* eof,
                     void* data)
{
    int     len/*, ret*/;

    printk(KERN_ALERT"====%d=====\n", __LINE__);
    //down(&monitor_sem);
    /*    ret = down_interruptible(&monitor_sem);
        if (0 != ret) {
            printk(KERN_ALERT"Read is interrupted!,ret=%d\n", ret);
            return 0;
        }
    */
    mutex_lock(&mutex_sem);
    len = sprintf(page, "You get msg from kernel is: %s", kbuff->buf);

    return len;
}

static int __init proc_init(void)
{
    printk(KERN_ALERT"Hello, %s\n", __func__);

    //  init_MUTEX_LOCKED(&monitor_sem);

    mutex_init(&mutex_sem);
    kbuff = kmalloc(sizeof(struct sln_kbuff), GFP_KERNEL);

    if (NULL == kbuff) {
        printk(KERN_ALERT"kmalloc() failed!");
        return -1;
    }

    root_dir = proc_mkdir(USER_ROOT_DIR, NULL);

    if (NULL == root_dir) {
        printk(KERN_ALERT"proc_mkdir create dir %s failed!\n", USER_ROOT_DIR);
        return -1;
    }

    entry = create_proc_entry(USER_ENTRY, 0666, root_dir);

    if (NULL == entry) {
        printk(KERN_ALERT"create_proc_entry create entry %s failed\n", USER_ENTRY);
        goto err;
    }

    entry->read_proc = proc_read;
    entry->write_proc = proc_write;
    return 0;

err:
    remove_proc_entry(USER_ROOT_DIR, NULL);
    return -1;
}

static void __exit proc_exit(void)
{
    printk(KERN_ALERT"Bye, %s\n", __func__);

    remove_proc_entry(USER_ENTRY, root_dir);
    remove_proc_entry(USER_ROOT_DIR, NULL);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
