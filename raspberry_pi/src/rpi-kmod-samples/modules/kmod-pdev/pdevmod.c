/*
 * Basic kernel module for platform device.
 *
 * Author:
 * 	Stefan Wendler (devnull@kaltpost.de)
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>	
#include <linux/kernel.h>	
#include <linux/platform_device.h>

/* 
 * Platform device data as passed in from platform 
 *
 * Normally this would go into a header file shared
 * between the module and the board definition.
 */
struct pdevmod_data {
    char *greeting;
    int   number;
};

/* 
 * Probe is called when the driver is about to be attached 
 */
int pdevmod_probe(struct platform_device *dev)
{
	// retrieve platform data
	struct pdevmod_data *pd = (struct pdevmod_data *)(dev->dev.platform_data);

	printk(KERN_INFO "%s\n", __func__);

	printk(KERN_INFO "Platform says: %s; %d\n", pd->greeting, pd->number);

	return 0;
}

/*
 * Remove is called when driver is about to be detached
 */
int pdevmod_remove(struct platform_device *dev)
{
	printk(KERN_INFO "%s\n", __func__);

	return 0;
}

/* Platform driver definition */
static struct platform_driver pdevmod_driver = {
	.driver		= {
		.name	= "pdevmod",
		.owner	= THIS_MODULE,
	},
	.probe		= pdevmod_probe,
	.remove		= __devexit_p(pdevmod_remove),
};

/*
 * Module init function
 */
static int __init pdevmod_init(void)
{
    int ret;

	printk(KERN_INFO "%s\n", __func__);

    ret = platform_driver_register(&pdevmod_driver);

	if(ret) {
		printk(KERN_ERR "Unable to register driver\n");
		return ret;
	}

	return 0;
}

/*
 * Module exit function
 */
static void __exit pdevmod_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);

    platform_driver_unregister(&pdevmod_driver);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module for a platform device");

module_init(pdevmod_init);
module_exit(pdevmod_exit);
