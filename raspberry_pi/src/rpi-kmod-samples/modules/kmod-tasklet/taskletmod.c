/*
 * Basic kernel module using a tasklet.
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
#include <linux/delay.h>
#include <linux/interrupt.h>

static void tasklet_func(unsigned long data)
{
	printk(KERN_INFO "%s\n", __func__);

	printk("Tasklet started\n");
	mdelay(1000);
	printk("Tasklet ended\n");	
}

DECLARE_TASKLET(tl_descr, tasklet_func, 0L);

/*
 * Module init function
 */
static int __init taskletmod_init(void)
{
	printk(KERN_INFO "%s\n", __func__);

	tasklet_schedule(&tl_descr);

	return 0;
}

/**
 * Module exit function
 */
static void __exit taskletmod_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);

	tasklet_kill(&tl_descr);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module using a tasklet");

module_init(taskletmod_init);
module_exit(taskletmod_exit);
