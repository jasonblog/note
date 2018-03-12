//-------------------------------------------------------------------
//	dram.c
//
//	This module implements a Linux character-mode device-driver
//	for the processor's installed physical memory.  It utilizes
//	the kernel's 'kmap()' function, as a uniform way to provide
//	access to all the memory-zones (including the "high memory"
//	on systems with more than 896MB of installed physical ram).
//	The access here is 'read-only' because we deem it too risky
//	to the stable functioning of our system to allow every user
//	the unrestricted ability to arbitrarily modify memory-areas
//	which might contain some "critical" kernel data-structures.
//	We implement an 'llseek()' method so that users can readily
//	find out how much physical processor-memory is installed.
//
//	NOTE: Developed and tested with Linux kernel version 2.6.10
//
//	programmer: ALLAN CRUSE
//	written on: 30 JAN 2005
//	revised on: 28 JAN 2008 -- for Linux kernel version 2.6.22.5
//	revised on: 06 FEB 2008 -- for machines having 4GB of memory
//-------------------------------------------------------------------

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/highmem.h>

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>	// for copy_to_user()
#include <linux/module.h>	// for module_init()
#include <linux/highmem.h>	// for kmap(), kunmap()

char modname[] = "dram";	// for displaying driver's name
int dram_major = 85;		    // NOTE: static major assignment

unsigned long dram_size;		// total bytes of system memory

typedef struct {
    struct cdev cdev;
    dev_t dev;
    struct class* class;
} phy_mem_cdev_t;
phy_mem_cdev_t phy_mem_cdev;



loff_t my_llseek( struct file *file, loff_t offset, int whence );
ssize_t my_read( struct file *file, char *buf, size_t count, loff_t *pos );

static const struct file_operations dram_fops =	{
    owner:		THIS_MODULE,
    llseek:		my_llseek,
    read:		my_read,
};

static int __init dram_init( void )
{
    int ret;
	struct device *device;
    unsigned long pages;

	printk( "Installing \'%s\' module (major=%d)\n", modname, dram_major );

    // 1. allocate the device.
    ret = alloc_chrdev_region(&phy_mem_cdev.dev, dram_major, 1, modname);
    if(ret < 0){
        printk("alloc_chrdev_region fail\n");
        goto err_register_chrdev_region;
    }

    // 2. set up the dev.
    cdev_init(&phy_mem_cdev.cdev, &dram_fops);
    phy_mem_cdev.cdev.owner = THIS_MODULE;
    ret = cdev_add(&phy_mem_cdev.cdev, phy_mem_cdev.dev, 1);
    if(ret < 0){
        printk("cdev_add failed, ret(%d)\n", ret);
        goto err_cdev_add;
    }

	// 3. register to sysfs.
    phy_mem_cdev.class = class_create(THIS_MODULE, modname);
	if(IS_ERR(phy_mem_cdev.class)){
		ret = PTR_ERR(phy_mem_cdev.class);
		printk("Fail to class_create\n");
		goto err_class_create;
	}

	// 4. export the device info to /sys/class/hello_char/dev_fifo
    device = device_create(phy_mem_cdev.class, NULL, phy_mem_cdev.dev, NULL, modname, 0);
	if(IS_ERR(device)){
		ret = PTR_ERR(device);
		printk("Fail to device_create\n");
		goto err_device_create;
	}

    //dram_size = (loff_t)num_physpages << PAGE_SHIFT;
    pages = get_num_physpages();
    dram_size = pages << PAGE_SHIFT;
	printk( "  ramtop=%08lX (%lu MB)\n", dram_size, dram_size >> 20 );
    return 0;

err_device_create:
	class_destroy(phy_mem_cdev.class);
err_class_create:
	cdev_del(&phy_mem_cdev.cdev);
err_cdev_add:
	unregister_chrdev_region(phy_mem_cdev.dev, 1);
err_register_chrdev_region:
	return ret;
}

static void __exit dram_exit( void )
{
    device_destroy(phy_mem_cdev.class, phy_mem_cdev.dev);
    class_destroy(phy_mem_cdev.class);
    cdev_del(&phy_mem_cdev.cdev);
    unregister_chrdev_region(phy_mem_cdev.dev, 1);
	printk( KERN_INFO "Removing \'%s\' module\n", modname );
}

ssize_t my_read( struct file *file, char *buf, size_t count, loff_t *pos )
{
	struct page	*pp;
	void		*from;
	int		page_number, page_indent, more;

	// we cannot read beyond the end-of-file
	if ( *pos >= dram_size ) return 0;

	// determine which physical page to temporarily map
	// and how far into that page to begin reading from
	page_number = *pos / PAGE_SIZE;
	page_indent = *pos % PAGE_SIZE;
	// cannot reliably read beyond the end of this mapped page
	if ( page_indent + count > PAGE_SIZE )
        count = PAGE_SIZE - page_indent;

    printk("read addr %16llX (%d:%d)\n", *pos, page_number, page_indent );

	// map the designated physical page into kernel space
    pp = pfn_to_page( page_number);
	from = kmap( pp ) + page_indent;

	// now transfer count bytes from mapped page to user-supplied buffer
	more = copy_to_user( buf, from, count );
    if(more) { // an error occurred if less than count bytes got copied
        printk("copy fail ret %d, %p, %p, %d\n", more, buf, from, (int)count);
        kunmap(pp);
        return -EFAULT;
    }

	// ok now to discard the temporary page mapping
	kunmap( pp );

	// otherwise advance file-pointer and report number of bytes read
	*pos += count;
	return	count;
}

loff_t my_llseek( struct file *file, loff_t offset, int whence )
{
    unsigned long	newpos = -1;
    printk("lseek: offset:0x%llx, whence:%d\n", offset, whence);

    switch( whence ) {
        case 0: newpos = offset; break;                 // SEEK_SET
        case 1: newpos = file->f_pos + offset; break;   // SEEK_CUR
        case 2: newpos = dram_size + offset; break;     // SEEK_END
        default: /* can't happen */
        printk("lseek default\n");
        return -EINVAL;
    }

	if (( newpos < 0 )||( newpos > dram_size )) return -EINVAL;
	file->f_pos = newpos;

	return	newpos;
}

MODULE_LICENSE("GPL");
module_init( dram_init );
module_exit( dram_exit );
