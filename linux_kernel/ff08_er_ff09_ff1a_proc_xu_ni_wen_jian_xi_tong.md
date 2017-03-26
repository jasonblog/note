# （二）：proc虛擬文件系統


proc虛擬文件系統主要用於內核向用戶導出信息，通過它可以在 Linux 內核空間和用戶空間之間進行通信。在/proc 文件系統中，我們可以將對虛擬文件的讀寫作為與內核中實體進行通信的一種手段，與普通文件不同的是，這些虛擬文件的內容都是動態創建的。下面是筆者系統上/proc下已經創建的文件：


```sh
# ls /proc/  
1      1513  1955  2171  2465  270   33    528   5356  5449  5646       bus          irq         net            uptime  
10     16    1960  22    2466  2700  331   5286  5358  5452  5655       cgroups      kallsyms    pagetypeinfo   version  
10364  1691  1978  2206  2467  271   332   5293  5362  5470  6          cmdline      kcore       partitions     vmallocinfo  
10427  17    1979  23    2468  2781  333   5298  5365  549   6504       cpuinfo  
......  
```

通過在應用上對這些文件的讀寫，可以將一些信息傳遞到內核空間中。linux下本身很多命令也通過分析/proc下的文件來完成，如ps等。在 Linux 設備驅動程序中，驅動工程師常常通過自定義/proc 節點以嚮應用傳遞信息，所以我們也可以通過在/proc下創建文件來向用戶空間傳遞信息。

使用create_proc_entry()函數創建在/proc文件系統中創建一個虛擬文件， 為了不至於/proc根目錄文件太多顯得太亂，也可以使用proc_mkdir()先在/proc下創建一個目錄，remove_proc_entry()函數可以刪除/proc 節點。


```c
struct proc_dir_entry *proc_mkdir(const char *name, struct proc_dir_entry *parent);   
struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode,struct proc_dir_entry *parent);  
void remove_proc_entry(const char *name, struct proc_dir_entry *parent);    
```

其中struct proc_dir_entry結構體定義為：


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

函數返回值的 proc_dir_entry 結構體中包含了/proc 節點的讀函數指針(read_proc_t*read_proc)、寫函數指針(write_proc_t *write_proc)以及父節點、子節點信息等。讀寫函數的原型為：


```c
typedef    int (read_proc_t)(char* page, char** start, off_t off,
                             int count, int* eof, void* data);
typedef    int (write_proc_t)(struct file* file, const char __user* buffer,
                              unsigned long count, void* data);
```


這兩個函數指針需要我們在創建完了節點之後再給其賦值，而函數也需要自己來實現。其實這兩個函數也就是當用戶空間讀寫該文件時，內核所做的動作。

page指針指向用於內核寫入數據的緩衝區，start 用於返回實際的數據寫到內存頁的位置，off是讀的偏移，count 是要讀的數據長度，eof 是用於返回讀結束標誌。filp參數實際上是一個打開文件結構，buff 參數是傳遞給您的字符串數據，len參數定義了在 buff 中有多少數據要被寫入，data參數是一個指向私有數據的指針。

下面是一個簡單使用proc的示例，應用上面傳入數據，內核經過處理之後，應用再獲取經過處理的數據：


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

在insmod模塊之後，就可以向文件中讀寫數據了：


```c
# echo "Proc" > /proc/shallnet/file  
# cat /proc/shallnet/file  
You get msg from kernel is: Proc  
```

其實也可以在上面的程序中加入內核與應用同步的機制，使用信號量可以實現。有關信號量的只是可以參見筆者這兩篇文章：
http://blog.csdn.net/shallnet/article/details/17734419
http://blog.csdn.net/shallnet/article/details/47176683

上面有關的實現源碼可以查看本節源碼，本節源碼下載：
http://download.csdn.net/detail/gentleliu/9035793