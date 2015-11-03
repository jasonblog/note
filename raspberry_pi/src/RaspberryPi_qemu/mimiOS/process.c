/*
   This process and round-robin scheduler are from PIqueno
   Tzu Hang modified it to mimiOS <karose.tzu@gmail.com>
*/


#include "scheduler.h"

// Just some counting for easy debug on the screen. Simulate user process.
void sample_process_1() {
    
    printk("Starting process 1 ");
    
    int to = 100;
    
    int i, j;
    for (i=0; i<to; i++) {
        printk("%d",i);
        printk(" ");
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }
    
    // Call software interrupt #0 (terminate)
    asm volatile("SWI #0");
}

// Just some counting for easy debug on the screen. Simulate user process.
void sample_process_2() {

    printk("Starting process 2 ");
    
    int to = 100;
    
    int i, j;
    for (i=0; i<to; i++) {
        printk("%d ",i);
        printk(" ");
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }
    
    // Call software interrupt #0 (terminate)
    asm volatile("SWI #0");
}
