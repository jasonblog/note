#include <linux/kernel.h>
#include <linux/fs.h>                
#include <linux/module.h>

static int helloworld_init(void)
{
    
	printk("hello world init\n");
	return 0;
}
static void helloworld_exit(void)
{
	printk("hello world exit\n");
}
module_init(helloworld_init);
module_exit(helloworld_exit);

