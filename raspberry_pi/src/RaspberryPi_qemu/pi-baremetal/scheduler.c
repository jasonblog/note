
#include "process.h"
#include "framebuffer.h"
#include "textutils.h"
#include "interrupts.h"

static process process_list[1024];
static unsigned int stack_base;
static unsigned int process_count = 0;
static unsigned int active_process_index;

extern void main_endloop();
extern void timer_reset();

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
	console_write("Main stack is 0x");
	console_write(tohex(stack_pointer, 4));
	console_write("\n");
    
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
	
	console_write("Forked stack is 0x");
	console_write(tohex(forked_stack_pointer, 4));
	console_write("\n");

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
	
	// Do this while the actual process isnt in the waiting status and not reach the actual running process
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
    
    console_write("\n");
    console_write("\n");
    console_write("Schedule timeout. Current active pid is ");
    console_write(todec(process_list[active_process_index].id, 0));
    console_write(" with name ");
    console_write(process_list[active_process_index].name);
    console_write(". Switching to next process.\n");      

	console_write("Saving stack...");
    console_write(" stack saved, was 0x");
    console_write(tohex(process_list[active_process_index].stack_pointer, 4));
    console_write("\n");

	console_write("Saving pc...");
    console_write(" pc saved, was 0x");
    console_write(tohex(process_list[active_process_index].pc, 4));
    console_write("\n");
    
    // Get next process id
    int next_process = next_waiting_process_index();
    
    // If -1, halt
    if (next_process < 0) {
		console_write("No more waiting processes, halting.");
		halt();
	}
    
    // Updating next running process
    active_process_index = next_process;
    
    // Increasing statistics, updating status to running
    process_list[active_process_index].times_loaded++;
    process_list[active_process_index].status = PROCESS_STATUS_RUNNING;
    
    console_write("Restoring stack 0x");
    console_write(tohex(process_list[active_process_index].stack_pointer, 4));
    console_write("\n");
	
    console_write("Restoring pc 0x");
    console_write(tohex(process_list[active_process_index].pc, 4));
    console_write("\n");
    
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
