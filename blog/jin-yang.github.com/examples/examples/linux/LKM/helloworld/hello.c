/* FILE: hello.c */
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    printk(KERN_ALERT "hello module!\n");
        return 0;
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
