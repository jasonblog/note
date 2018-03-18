/*
   This process and round-robin scheduler are from PIqueno
   Tzu Hang modified it to mimiOS <karose.tzu@gmail.com>
*/

#include "scheduler.h"
#include "syscall.h"


/*
 * Procedure to handle software interrupts
 */
void syscall(unsigned int swi) {
	
	printk("\nHandling syscall: ");
	printk("%d",swi);
	printk("\n");
	
	switch (swi) {
		
		case SYSCALL_TERMINATE_PROCESS:
			printk("Invoking syscall terminate_process()");
			printk("\n");
			
			// Calling the right intra-kernel procedure
			terminate_process();
                        break;
	}

	printk("Turning interrupt on again");
	printk("\n");
	
	// Turn on interrupt again
	asm volatile("cpsie i");
	
	// Wait for timer interrupt. Probably not the best thing to do.
	halt();
	
}
