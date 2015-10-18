
#include "scheduler.h"
#include "syscall.h"
#include "framebuffer.h"
#include "textutils.h"

/*
 * Procedure to handle software interrupts
 */
void syscall(unsigned int swi) {
	
	console_write("Handling syscall: ");
	console_write(todec(swi, 0));
	console_write("\n");
	
	switch (swi) {
		
		case SYSCALL_TERMINATE_PROCESS:
			console_write("Invoking syscall terminate_process()");
			console_write("\n");
			
			// Calling the right intra-kernel procedure
			terminate_process();
			break;
	}

	console_write("Turning interrupt on again");
	console_write("\n");
	
	// Turn on interrupt again
	asm volatile("cpsie i");
	
	// Wait for timer interrupt. Probably not the best thing to do.
	halt();
	
}
