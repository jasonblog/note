/*
 * Basic kernel module taking commandline arguments.
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
 * The module commandline arguments ...
 */
static short int 	 myshort 		= 1;
static int 			 myint 			= 420;
static long int 	 mylong 		= 9999;
static char     	*mystring 		= "blah";
static int 			 myintArray[2] 	= { -1, -1 };
static int 			 arr_argc 		= 0;

module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer");
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");
module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");
module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");
module_param_array(myintArray, int, &arr_argc, 0000);
MODULE_PARM_DESC(myintArray, "An array of integers");

/*
 * Module init function
 */
static int __init clargmod_init(void)
{
	int i;

	printk(KERN_INFO "%s\n=============\n", __func__);
	printk(KERN_INFO "myshort is a short integer: %hd\n", myshort);
	printk(KERN_INFO "myint is an integer: %d\n", myint);
	printk(KERN_INFO "mylong is a long integer: %ld\n", mylong);
	printk(KERN_INFO "mystring is a string: %s\n", mystring);

	for (i = 0; i < (sizeof myintArray / sizeof (int)); i++)
	{
		printk(KERN_INFO "myintArray[%d] = %d\n", i, myintArray[i]);
	}

	printk(KERN_INFO "got %d arguments for myintArray.\n", arr_argc);

	return 0;
}

/*
 * Exit function
 */
static void __exit clargmod_exit(void)
{
	printk(KERN_INFO "%s\n", __func__);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module taking command line arguments");

module_init(clargmod_init);
module_exit(clargmod_exit);
