/*
 * Basic Linux Kernel module using a tasklet to blink LEDs.
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
#include <linux/gpio.h>

/* Define pins, directin and inital state of GPIOs for LEDs */
static struct gpio leds[] = {
		{  4, GPIOF_OUT_INIT_LOW, "LED 1" },
		{ 25, GPIOF_OUT_INIT_LOW, "LED 2" },
		{ 24, GPIOF_OUT_INIT_LOW, "LED 3" },
};

/* Tasklet to blink the LEDs */
static void blink_tasklet(unsigned long data)
{
	int i;

	printk(KERN_INFO "%s\n", __func__);

	printk("Tasklet started\n");

	for(i = 0; i < ARRAY_SIZE(leds); i++) {

		if(i - 1 >= 0) {
			gpio_set_value(leds[i - 1].gpio, 0); 
		}
		gpio_set_value(leds[i].gpio, 1); 
		mdelay(500);
	}

	gpio_set_value(leds[i - 1].gpio, 0); 

	printk("Tasklet ended\n");	
}

DECLARE_TASKLET(tl_descr, blink_tasklet, 0L);

/*
 * Module init function
 */
static int __init gpiomod_init(void)
{
	int ret = 0;

	printk(KERN_INFO "%s\n", __func__);

	// register, turn on
	ret = gpio_request_array(leds, ARRAY_SIZE(leds));

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs: %d\n", ret);
		return ret;
	}

	tasklet_schedule(&tl_descr);

	return ret;
}

/*
 * Module exit function
 */
static void __exit gpiomod_exit(void)
{
	int i;

	printk(KERN_INFO "%s\n", __func__);

	tasklet_kill(&tl_descr);

	// turn all off
	for(i = 0; i < ARRAY_SIZE(leds); i++) {
		gpio_set_value(leds[i].gpio, 0); 
	}
	
	// unregister
	gpio_free_array(leds, ARRAY_SIZE(leds));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module using a tasklet to blink LEDs");

module_init(gpiomod_init);
module_exit(gpiomod_exit);
