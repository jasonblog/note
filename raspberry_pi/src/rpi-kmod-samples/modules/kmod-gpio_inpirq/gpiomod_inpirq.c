/*
 * Basic Linux Kernel module using GPIO interrupts.
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
#include <linux/interrupt.h> 

/* Define GPIOs for LEDs */
static struct gpio leds[] = {
		{  4, GPIOF_OUT_INIT_LOW, "LED 1" },
};

/* Define GPIOs for BUTTONS */
static struct gpio buttons[] = {
		{ 17, GPIOF_IN, "BUTTON 1" },	// turns LED on
		{ 18, GPIOF_IN, "BUTTON 2" },	// turns LED off
};

/* Later on, the assigned IRQ numbers for the buttons are stored here */
static int button_irqs[] = { -1, -1 };

/*
 * The interrupt service routine called on button presses
 */
static irqreturn_t button_isr(int irq, void *data)
{
	if(irq == button_irqs[0] && !gpio_get_value(leds[0].gpio)) {
			gpio_set_value(leds[0].gpio, 1);
	}
	else if(irq == button_irqs[1] && gpio_get_value(leds[0].gpio)) {
			gpio_set_value(leds[0].gpio, 0);
	}

	return IRQ_HANDLED;
}

/*
 * Module init function
 */
static int __init gpiomode_init(void)
{
	int ret = 0;

	printk(KERN_INFO "%s\n", __func__);

	// register LED gpios
	ret = gpio_request_array(leds, ARRAY_SIZE(leds));

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs for LEDs: %d\n", ret);
		return ret;
	}
	
	// register BUTTON gpios
	ret = gpio_request_array(buttons, ARRAY_SIZE(buttons));

	if (ret) {
		printk(KERN_ERR "Unable to request GPIOs for BUTTONs: %d\n", ret);
		goto fail1;
	}

	printk(KERN_INFO "Current button1 value: %d\n", gpio_get_value(buttons[0].gpio));
	
	ret = gpio_to_irq(buttons[0].gpio);

	if(ret < 0) {
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}

	button_irqs[0] = ret;

	printk(KERN_INFO "Successfully requested BUTTON1 IRQ # %d\n", button_irqs[0]);

	ret = request_irq(button_irqs[0], button_isr, IRQF_TRIGGER_RISING | IRQF_DISABLED, "gpiomod#button1", NULL);

	if(ret) {
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}


	ret = gpio_to_irq(buttons[1].gpio);

	if(ret < 0) {
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail2;
	}
		
	button_irqs[1] = ret;

	printk(KERN_INFO "Successfully requested BUTTON2 IRQ # %d\n", button_irqs[1]);

	ret = request_irq(button_irqs[1], button_isr, IRQF_TRIGGER_RISING | IRQF_DISABLED, "gpiomod#button2", NULL);

	if(ret) {
		printk(KERN_ERR "Unable to request IRQ: %d\n", ret);
		goto fail3;
	}

	return 0;

// cleanup what has been setup so far
fail3:
	free_irq(button_irqs[0], NULL);

fail2: 
	gpio_free_array(buttons, ARRAY_SIZE(leds));

fail1:
	gpio_free_array(leds, ARRAY_SIZE(leds));

	return ret;	
}

/**
 * Module exit function
 */
static void __exit gpiomode_exit(void)
{
	int i;

	printk(KERN_INFO "%s\n", __func__);

	// free irqs
	free_irq(button_irqs[0], NULL);
	free_irq(button_irqs[1], NULL);
	
	// turn all LEDs off
	for(i = 0; i < ARRAY_SIZE(leds); i++) {
		gpio_set_value(leds[i].gpio, 0); 
	}
	
	// unregister
	gpio_free_array(leds, ARRAY_SIZE(leds));
	gpio_free_array(buttons, ARRAY_SIZE(buttons));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module using GPIO interrupts");

module_init(gpiomode_init);
module_exit(gpiomode_exit);
