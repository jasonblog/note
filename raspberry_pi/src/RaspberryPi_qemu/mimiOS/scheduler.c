/*
   This process and round-robin scheduler are from PIqueno
   Tzu Hang modified it to mimiOS <karose.tzu@gmail.com>
*/

#include "process.h"


static process process_list[1024];
static unsigned int stack_base;
static unsigned int process_count = 0;
static unsigned int active_process_index;

extern void main_endloop();
extern void timer_reset();

static int debug_counter=0;

void terminate_process() {
	// Set it to terminated status, so the scheduler will ignore it
	process_list[active_process_index].status = PROCESS_STATUS_TERMINATED;
}

// Function to create the main process
void create_main_process() {
	
	// Creating process
    process main_process;

    main_process.id = process_count;
    main_process.name = "Main";
    main_process.pc = &main_endloop;
    main_process.times_loaded = 1;
    main_process.status = PROCESS_STATUS_ZOMBIE;
    
    // Gets the actual stack address
    unsigned int stack_pointer;
	asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );
	
	// Use it as base address processes stack pointer
	stack_base = stack_pointer;
	
	// Output the stack address
	printk("Main stack is 0x");
	printk("%x",stack_pointer);
	printk("\n");
    
    // Saving the process in the process table
    process_list[process_count] = main_process;
    
    // Set it as the current active process
    active_process_index = process_count;
    
    // Increments the process counter
    process_count++;
}

/* 
 * Procedure to fork this process, creating a new one, pointing the pc
 * to the memory address of the desired procedure
 */
void fork(char * name, unsigned long * pc) {

    process fork_process;
	
	// Basic memory organization to get new stack addr. just add 1024 bytes from the main stack
	unsigned int * forked_stack_pointer = stack_base + (process_count * 1024);
	
	printk("Forked stack is 0x");
	printk("%x",forked_stack_pointer);
	printk("\n");

    fork_process.id = process_count;
    fork_process.name = name;
    fork_process.pc = pc;
    fork_process.parent = active_process_index;
    fork_process.times_loaded = 0;
    fork_process.stack_pointer = forked_stack_pointer;
    fork_process.status = PROCESS_STATUS_WAITING;
    
    process_list[process_count] = fork_process;

    process_count++;
}

/*
 * Function to get the next ready process in the list
 */
int next_waiting_process_index() {
	
	// Start in the active index
	int next_process_index = active_process_index;
	
	// Do this while the actual process isnt in the waiting status 
       //  and not reach the actual running process
	do {
		next_process_index++;
		
		// Just rewind the list
		if (next_process_index == process_count) {
			next_process_index = 0;
		}
	
	} while ((process_list[next_process_index].status != PROCESS_STATUS_WAITING) && (next_process_index != active_process_index));
	
	// If the found process isnt waiting
	if (process_list[next_process_index].status != PROCESS_STATUS_WAITING) {
		return -1;
	}
	
	return next_process_index;
}

/*
 * Just keep the processor busy
 */
void halt() {
	while(1);
}

/*
 * Procedure coming from the IRQ interrupt, to change the running process
 */
void schedule_timeout(unsigned long stack_pointer, unsigned long pc) {
	
	// Saving stack and pc
    process_list[active_process_index].stack_pointer = stack_pointer;
    process_list[active_process_index].pc = pc;
    
    // Updating process status to waiting
    if (process_list[active_process_index].status == PROCESS_STATUS_RUNNING) {
		process_list[active_process_index].status = PROCESS_STATUS_WAITING;
	}

    //halt system to check debug messages.
//	debug_counter++;	
	if(debug_counter >3)
	{
		printk("debug.... halting. \n");
		halt();
	}
		
    
    printk("\n");
    printk("\n");
    printk("Schedule timeout. Current active pid is ");
    printk("%d",process_list[active_process_index].id);
    printk(" with name ");
    printk(process_list[active_process_index].name);
    printk(". Switching to next process.\n");      

    printk("Saving stack...");
    printk(" stack saved, was 0x");
    printk("%x",process_list[active_process_index].stack_pointer);
    printk("\n");

    printk("Saving pc...");
    printk(" pc saved, was 0x");
    printk("%x",process_list[active_process_index].pc);
    printk("\n");
    
    // Get next process id
    int next_process = next_waiting_process_index();
    
    // If -1, halt
    if (next_process < 0) {
		printk("No more waiting processes, halting.");
		halt();
	}
    
    // Updating next running process
    active_process_index = next_process;
    
    // Increasing statistics, updating status to running
    process_list[active_process_index].times_loaded++;
    process_list[active_process_index].status = PROCESS_STATUS_RUNNING;
    
    printk("Restoring stack 0x");
    printk("%x",process_list[active_process_index].stack_pointer);
    printk("\n");
	
    printk("Restoring pc 0x");
    printk("%x",process_list[active_process_index].pc);
    printk("\n");
    
    // Assembly to load the sp with the new process stack address
	asm volatile("MOV SP, %[addr]" : : [addr] "r" ((unsigned long )(process_list[active_process_index].stack_pointer)) );
	
	// If its is not the first time
	if (process_list[active_process_index].times_loaded > 1) {
		
		timer_reset();
		
		// Pops registers from the stack
		asm volatile("pop {R0}");
		asm volatile("MSR   SPSR_cxsf, R0");
		asm volatile("pop {LR}");
		asm volatile("pop {R0 - R12}");
		
		// Turn on interrupt again
		asm volatile("cpsie i");
		
		// Pops the last register into PC to resume processing
		asm volatile("pop {PC}");
		
	} else {
		
		// Push the first pc address into the stack
		unsigned long addr = (unsigned long )(process_list[active_process_index].pc);
		asm volatile("MOV R0, %[addr]" : : [addr] "r" (addr) );
		asm volatile("push {R0}");
		
		timer_reset();
	
		// Turn on interrupt again
		asm volatile("cpsie i");
		
		// Pops the last register into PC to resume processing
		asm volatile("pop {PC}");
	
	}


		
	
}
