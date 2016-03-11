/*
 * Basic kernel module.
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

/*
 * Module init function
 */
static int __init basicmod_init(void)
{
	printk(KERN_INFO "%s\n", __func__);

	/* 
 	 * A non 0 return means init_module failed; module can't be loaded. 
 	 */
	return 0;
}

/*
 * Module exit function
 */
static void __exit basicmod_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module");

module_init(basicmod_init);
module_exit(basicmod_exit);
