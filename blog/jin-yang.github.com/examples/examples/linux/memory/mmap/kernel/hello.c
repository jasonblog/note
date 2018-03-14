#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>

static unsigned char *buffer;
static unsigned char array[10]={0,1,2,3,4,5,6,7,8,9};

static int my_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int my_map(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long page;
    unsigned char i;
    unsigned long start = (unsigned long)vma->vm_start;
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);

    page = virt_to_phys(buffer); // get the physical address from virtual.
    // 将用户空间的一个vma虚拟内存区映射到以page开始的一段连续物理页面上
    // 第三个参数是页帧号，由物理地址右移PAGE_SHIFT得到
    if(remap_pfn_range(vma, start, page>>PAGE_SHIFT, size, PAGE_SHARED))
        return -1;

    for(i=0; i<10; i++)  // write some data
        buffer[i] = array[i];

    return 0;
}

static struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .open  = my_open,
    .mmap  = my_map,
};

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mymmap",
    .fops = &dev_fops,
};

static int __init hello_init(void)
{
    int ret;

    ret = misc_register(&misc);  // register a device
    buffer = (unsigned char *)kmalloc(PAGE_SIZE, GFP_KERNEL);  // alloc some mem.
    SetPageReserved(virt_to_page(buffer));  // set this page as reserved.

    return ret;
}

static void __exit hello_exit(void)
{
    misc_deregister(&misc); // unregister device
    ClearPageReserved(virt_to_page(buffer)); // reset reserved
    kfree(buffer);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("Dual BSD/GPL");
