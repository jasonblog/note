/*
 * Basic kernel module using some GPIOs to drive LEDs.
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
#include <linux/gpio.h>

/*
 * Struct defining pins, direction and inital state 
 */
static struct gpio leds[] = {
		{  4, GPIOF_OUT_INIT_HIGH, "LED 1" },
		{ 25, GPIOF_OUT_INIT_HIGH, "LED 2" },
		{ 24, GPIOF_OUT_INIT_HIGH, "LED 3" },
};

/*
 * Module init function
 */
static int __init gpiomod_init(void)
{
	int ret = 0;

	printk(KERN_INFO "%s\n", __func__);

	// register LED GPIOs, turn LEDs on
	ret = gpio_request_array(leds, ARRAY_SIZE(leds));

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs: %d\n", ret);
	}

	return ret;
}

/*
 * Module exit function
 */
static void __exit gpiomod_exit(void)
{
	int i;

	printk(KERN_INFO "%s\n", __func__);

	// turn all LEDs off
	for(i = 0; i < ARRAY_SIZE(leds); i++) {
		gpio_set_value(leds[i].gpio, 0); 
	}
	
	// unregister all GPIOs
	gpio_free_array(leds, ARRAY_SIZE(leds));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module using GPIOs to drive LEDs");

module_init(gpiomod_init);
module_exit(gpiomod_exit);
