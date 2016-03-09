# NAPI與pure interrupt driver的效能比較


本例例化一個字符設備，該設備申請一塊內存，file_operations中有mmap的功能，在測試程序test.c中mmap這塊內存，操作這塊用戶內存即可以修改設備內存

驅動代碼 mmap_demo.c

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/gfp.h>
#include <linux/string.h>
#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <asm/io.h>

#include <linux/miscdevice.h>

#define mmap_printk(args...) do {printk(KERN_ALERT "MMAP_DEMO " args); } while(0)
#define KSTR_DEF    "hello world from kernel virtual space"
#define mmap_name   "mmap_demo"

static struct page* pg;
static struct timer_list timer;

static void
timer_func(unsigned long data)
{
    printk("timer_func:%s\n", (char*) data);
    timer.expires = jiffies + HZ * 10;
    add_timer(&timer);
}

static int
demo_open(struct inode* inode, struct file* filp)
{
    mmap_printk("mmap_demo device open\n");
    return 0;
}

static int
demo_release(struct inode* inode, struct file* filp)
{
    mmap_printk("mmap_demo device closed\n");
    return 0;
}

static int
demo_mmap(struct file* filp, struct vm_area_struct* vma)
{
    int err = 0;
    unsigned long start = vma->vm_start;
    unsigned long size = vma->vm_end - vma->vm_start;

    err = remap_pfn_range(vma, start, vma->vm_pgoff, size, vma->vm_page_prot);
    return err;
}

static struct file_operations mmap_fops = {
    .owner = THIS_MODULE,
    .open = demo_open,
    .release = demo_release,
    .mmap = demo_mmap,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = mmap_name,
    .fops = &mmap_fops,
};

static int __init
demo_map_init(void)
{
    int ret = 0;

    char* kstr;

    pg = alloc_pages(GFP_HIGHUSER, 0);

    SetPageReserved(pg);

    kstr = (char*) kmap(pg);
    strcpy(kstr, KSTR_DEF);
    printk("kpa = 0x%lx, kernel string = %s\n", page_to_phys(pg), kstr);

    init_timer(&timer);
    timer.function = timer_func;
    timer.data = (unsigned long) kstr;
    timer.expires = jiffies + HZ * 10;
    add_timer(&timer);

    ret = misc_register(&misc);
    printk("the mmap miscdevice registered\n");
    return ret;
}

module_init(demo_map_init);

static void
demo_map_exit(void)
{
    del_timer_sync(&timer);

    misc_deregister(&misc);
    printk("the device misc_mmap deregistered\n");

    kunmap(pg);
    ClearPageReserved(pg);
    __free_pages(pg, 0);
}

module_exit(demo_map_exit);
MODULE_LICENSE("DUAL BSD/GPL");
MODULE_AUTHOR("BG2BKK");

```

Makefile

```sh
KERNELDIR=/lib/modules/$(shell uname -r)/build
PWD=$(shell pwd)
obj-m = mmap_demo.o

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
test: test.c
	gcc $< -o $@.o

clean:
	rm -rf *.o *~ *.ko* *.order *.symvers *.mod*
```

測試代碼 test.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAP_SIZE 4096
#define USTR_DEF "String changed from the user space"

int main(int argc, char* argv[])
{
    int fd;
    char* pdata;

    if (argc <= 1) {
        printf("USAGE: main devfile pamapped\n");
        return 0;
    }

    fd = open(argv[1], O_RDWR | O_NDELAY);

    if (fd  >= 0) {
        pdata   = (char*)mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
                              strtoul(argv[2], 0, 16));
        printf("USERAddr = %p, DATA from kernel %s\n", pdata, pdata);
        printf("Writing a string to the kernel space...\n");
        strcpy(pdata, USTR_DEF);
        printf("Done\n");
        munmap(pdata, MAP_SIZE);
        close(fd);
    }

    return 0;
}
```


測試過程
```sh
make
make test
sudo insmod mmap_demo.ko
```

再dmesg查看mmap_demo設備中的內存頁面地址，打印輸出為

```sh
[10392.291764] kpa = 0x6e5ea3000, kernel string = hello world from kernel virtual space
[10392.291912] the mmap miscdevice registered
```
```sh
timer_func:hello world from kernel virtual space
timer_func:hello world from kernel virtual space
timer_func:hello world from kernel virtual space
timer_func:hello world from kernel virtual space
timer_func:hello world from kernel virtual space
timer_func:hello world from kernel virtual space
```


可知地址為 0x6e5ea3000
然後

```sh
sudo ./test.o  /dev/mmap_demo 0x6e5ea3000
```
然後輸出為

```sh
USERAddr = 0x7f7c65eb9000, DATA from kernel String changed from the user space
Writing a string to the kernel space...
Done
```

dmesg的輸出結果為

```sh
timer_func:hello world from kernel virtual space
timer_func:hello world from kernel virtual space
MMAP_DEMO mmap_demo device open
MMAP_DEMO mmap_demo device closed
timer_func:String changed from the user space
timer_func:String changed from the user space
```



