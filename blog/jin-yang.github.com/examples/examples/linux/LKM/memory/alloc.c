#include <linux/mm.h>
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    struct page *page;
    void *address;
    char data[] = "hello linux";

    page = alloc_pages(GFP_KERNEL, 0);
    if (!page) {
        printk(KERN_ALERT "alloc_pages() error!\n");
        return -1;
    }

    address = page_address(page);
    if (!address) {
        printk(KERN_ALERT "page_address() error!\n");
        goto err;
    }

    memcpy(address, data, strlen(data));
    printk(KERN_ALERT "%s\n", (char *)address);
    free_pages((unsigned long)address, 0);

    return 0;
err:
    free_pages((unsigned long)page, 0);
    return -1;
}

static void __exit hello_exit(void)
{
    printk(KERN_ALERT "bye module!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Andy <justkidding@gmail.com>");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_ALIAS("the simplest module");
