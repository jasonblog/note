/*
 * Basic kernel module using a timer and GPIOs to flash a LED.
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
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio.h>

#define LED1	4

static struct timer_list blink_timer;

/*
 * Timer function called periodically
 */
static void blink_timer_func(unsigned long data)
{
	printk(KERN_INFO "%s\n", __func__);

	gpio_set_value(LED1, data); 
	
	/* schedule next execution */
	blink_timer.data = !data;						// makes the LED toggle 
	blink_timer.expires = jiffies + (1*HZ); 		// 1 sec.
	add_timer(&blink_timer);
}

/*
 * Module init function
 */
static int __init gpiomod_init(void)
{
	int ret = 0;

	printk(KERN_INFO "%s\n", __func__);

	// register, turn off 
	ret = gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "led1");

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs: %d\n", ret);
		return ret;
	}

	/* init timer, add timer function */
	init_timer(&blink_timer);

	blink_timer.function = blink_timer_func;
	blink_timer.data = 1L;							// initially turn LED on
	blink_timer.expires = jiffies + (1*HZ); 		// 1 sec.
	add_timer(&blink_timer);

	return ret;
}

/*
 * Module exit function
 */
static void __exit gpiomod_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);

	// deactivate timer if running
	del_timer_sync(&blink_timer);

	// turn LED off
	gpio_set_value(LED1, 0); 
	
	// unregister GPIO 
	gpio_free(LED1);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic kernel module using a timer and GPIOs to flash a LED.");

module_init(gpiomod_init);
module_exit(gpiomod_exit);
