
#include "textutils.h"
#include "framebuffer.h"
#include "scheduler.h"

// Just some counting for easy debug on the screen. Simulate user process.
void sample_process_1() {
    
    console_write("Starting process 1 ");
    
    int to = 300;
    
    int i, j;
    for (i=0; i<to; i++) {
        console_write(todec(i,0));
        console_write(" ");
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }
    
    // Call software interrupt #0 (terminate)
    asm volatile("SWI #0");
}

// Just some counting for easy debug on the screen. Simulate user process.
void sample_process_2() {

    console_write("Starting process 2 ");
    
    int to = 300;
    
    int i, j;
    for (i=0; i<to; i++) {
        console_write(todec(i,0));
        console_write(" ");
        for (j=0; j<to*to; j++) {
			asm volatile("NOP");
		}
    }
    
    // Call software interrupt #0 (terminate)
    asm volatile("SWI #0");
}
