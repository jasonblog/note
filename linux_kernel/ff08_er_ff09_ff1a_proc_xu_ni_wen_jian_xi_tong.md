# （二）：proc虚拟文件系统


proc虚拟文件系统主要用于内核向用户导出信息，通过它可以在 Linux 内核空间和用户空间之间进行通信。在/proc 文件系统中，我们可以将对虚拟文件的读写作为与内核中实体进行通信的一种手段，与普通文件不同的是，这些虚拟文件的内容都是动态创建的。下面是笔者系统上/proc下已经创建的文件：


```sh
# ls /proc/  
1      1513  1955  2171  2465  270   33    528   5356  5449  5646       bus          irq         net            uptime  
10     16    1960  22    2466  2700  331   5286  5358  5452  5655       cgroups      kallsyms    pagetypeinfo   version  
10364  1691  1978  2206  2467  271   332   5293  5362  5470  6          cmdline      kcore       partitions     vmallocinfo  
10427  17    1979  23    2468  2781  333   5298  5365  549   6504       cpuinfo  
......  
```

通过在应用上对这些文件的读写，可以将一些信息传递到内核空间中。linux下本身很多命令也通过分析/proc下的文件来完成，如ps等。在 Linux 设备驱动程序中，驱动工程师常常通过自定义/proc 节点以向应用传递信息，所以我们也可以通过在/proc下创建文件来向用户空间传递信息。

使用create_proc_entry()函数创建在/proc文件系统中创建一个虚拟文件， 为了不至于/proc根目录文件太多显得太乱，也可以使用proc_mkdir()先在/proc下创建一个目录，remove_proc_entry()函数可以删除/proc 节点。


```c
struct proc_dir_entry *proc_mkdir(const char *name, struct proc_dir_entry *parent);   
struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode,struct proc_dir_entry *parent);  
void remove_proc_entry(const char *name, struct proc_dir_entry *parent);    
```

其中struct proc_dir_entry结构体定义为：


```c
struct proc_dir_entry {
    unsigned int low_ino;
    unsigned short namelen;
    const char* name;
    mode_t mode;
    nlink_t nlink;
    uid_t uid;
    gid_t gid;
    loff_t size;
    const struct inode_operations* proc_iops;

    const struct file_operations* proc_fops;
    struct proc_dir_entry* next, *parent, *subdir;
    void* data;
    read_proc_t* read_proc;
    write_proc_t* write_proc;
    atomic_t count;        /* use count */
    int pde_users;    /* number of callers into module in progress */
    spinlock_t pde_unload_lock; /* proc_fops checks and pde_users bumps */
    struct completion* pde_unload_completion;
    struct list_head pde_openers;    /* who did ->open, but not ->release */
};
```

函数返回值的 proc_dir_entry 结构体中包含了/proc 节点的读函数指针(read_proc_t*read_proc)、写函数指针(write_proc_t *write_proc)以及父节点、子节点信息等。读写函数的原型为：


```c
typedef    int (read_proc_t)(char* page, char** start, off_t off,
                             int count, int* eof, void* data);
typedef    int (write_proc_t)(struct file* file, const char __user* buffer,
                              unsigned long count, void* data);
```


这两个函数指针需要我们在创建完了节点之后再给其赋值，而函数也需要自己来实现。其实这两个函数也就是当用户空间读写该文件时，内核所做的动作。

page指针指向用于内核写入数据的缓冲区，start 用于返回实际的数据写到内存页的位置，off是读的偏移，count 是要读的数据长度，eof 是用于返回读结束标志。filp参数实际上是一个打开文件结构，buff 参数是传递给您的字符串数据，len参数定义了在 buff 中有多少数据要被写入，data参数是一个指向私有数据的指针。

下面是一个简单使用proc的示例，应用上面传入数据，内核经过处理之后，应用再获取经过处理的数据：


```c
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

static int proc_write(struct file* filep,
                      const char __user* buffer,
                      unsigned long len,
                      void* data)
{
    if (copy_from_user(kbuff->buf, buffer, len)) {
        return -1;
    }

    kbuff->buf[len] = '\0';
    return len;
}

static int proc_read(char* page,
                     char** start,
                     off_t off,
                     int count,
                     int* eof,
                     void* data)
{
    int     len;

    len = sprintf(page, "You get msg from kernel is: %s", kbuff->buf);

    return len;
}

static int __init proc_init(void)
{
    printk("Hello, %s\n", __func__);

    kbuff = kmalloc(sizeof(struct sln_kbuff), GFP_KERNEL);

    if (NULL == kbuff) {
        printk("kmalloc() failed!");
        return -1;
    }

    root_dir = proc_mkdir(USER_ROOT_DIR, NULL);

    if (NULL == root_dir) {
        printk("proc_mkdir create dir %s failed!\n", USER_ROOT_DIR);
        return -1;
    }

    entry = create_proc_entry(USER_ENTRY, 0666, root_dir);

    if (NULL == entry) {
        printk("create_proc_entry create entry %s failed\n", USER_ENTRY);
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
    printk("Bye, %s\n", __func__);

    remove_proc_entry(USER_ENTRY, root_dir);
    remove_proc_entry(USER_ROOT_DIR, NULL);
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shallnet");
MODULE_DESCRIPTION("blog.csdn.net/shallnet");
```

在insmod模块之后，就可以向文件中读写数据了：


```c
# echo "Proc" > /proc/shallnet/file  
# cat /proc/shallnet/file  
You get msg from kernel is: Proc  
```

其实也可以在上面的程序中加入内核与应用同步的机制，使用信号量可以实现。有关信号量的只是可以参见笔者这两篇文章：
http://blog.csdn.net/shallnet/article/details/17734419
http://blog.csdn.net/shallnet/article/details/47176683

上面有关的实现源码可以查看本节源码，本节源码下载：
http://download.csdn.net/detail/gentleliu/9035793