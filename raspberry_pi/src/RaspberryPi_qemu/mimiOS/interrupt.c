/*
   interrupt.s  
   Tzu Hang <karose.tzu@gmail.com>

   reference:brianwiddas https://github.com/brianwiddas/pi-baremetal
             tnishinaga  book:"BareMetalで遊ぶ　Raspberry Pi"
*/
static volatile unsigned int *irqEnable1 = (unsigned int *)0x2000b210;
static volatile unsigned int *irqEnable2 = (unsigned int *) 0x2000b214;
static volatile unsigned int *irqEnableBasic = (unsigned int *) 0x2000b218;

static volatile unsigned int *armTimerLoad = (unsigned int *) 0x2000b400;
static volatile unsigned int *armTimerValue = (unsigned int *) 0x2000b404;
static volatile unsigned int *armTimerControl = (unsigned int *) 0x2000b408;
static volatile unsigned int *armTimerIRQClear = (unsigned int *) 0x2000b40c;


__attribute__ ((naked, aligned(32))) static void interrupt_vectors(void)
{
	asm volatile("b __vector_reset\n"	/* RESET */
		"b __vector_undefined\n"	/* UNDEF */
		"b __vector_swi\n"
		"b __vector_prefetch_abort \n"
		"b __vector_data_abort \n"       
		"b __vector_reserved;\n"	/* Unused vector */
		"b __vector_irq \n"     /*IRQ*/
		"b __vector_fiq\n"	/* FIQ */
	);
}



void IRQ_handler()
{

  disable_IRQ();
// This function starts on IRQ mode
	
	// Push all registers into the IRQ mode stack (R13)
	asm volatile("push {R0-R12}");
	
	// Put LR register of IRQ mode (PC of interrupted process) on R0
	asm volatile("MOV R0, LR");
	
	
	// Change to system mode
	asm volatile("cps #0x1f");
	
	// Push R0 (interrupted PC) to the system mode stack
	asm volatile("push {R0}");
	
	
	// Return to IRQ mode
	asm volatile("cps #0x12");
	
	// Pop all registers again
	asm volatile("pop {R0-R12}");
	
	
	// Return to system mode
	asm volatile("cps #0x1f");
	
	// Push all registers into the system mode stack
	asm volatile("push {R0-R12}");
	
	// Push the interrupted LR to system mode stack
	asm volatile("push {LR}");
	
	// Copy the processor status to R0
    asm volatile("MRS R0, SPSR");
    
    // Push the processor status to system mode stack
    asm volatile("push {R0}");
    
    
    // Return to IRQ mode
    asm volatile("cps #0x12");
	
	// Copy LR to R0
	asm volatile("MOV R0, LR");
	
	// Back to system mode
	asm volatile("cps #0x1f");
	
	unsigned long pc;

    unsigned long stack_pointer;
	
	// Getting pc and stack just to debug
	asm volatile ("MOV %0, R0\n\t" : "=r" (pc) );
    asm volatile ("MOV %0, SP\n\t" : "=r" (stack_pointer) );
    
	
	// Jump to scheduler to do the context switch
	schedule_timeout(stack_pointer, pc);



}

void timer_reset(void)
{
	*armTimerIRQClear = 0;
}


void IRQ_init()
{	

       	/* Set interrupt base register */
	asm volatile("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (&interrupt_vectors));

       /* Turn on interrupts */
	asm volatile("cpsie i");
        printk("irq init \n");

	/* Use the ARM timer - BCM 2832 peripherals doc, p.196 */
	/* Enable ARM timer IRQ */
	*irqEnableBasic = 0x00000001;

	/* Interrupt every 1024 * 256 (prescaler) timer ticks */
	*armTimerLoad = 0x00000400;

	/* Timer enabled, interrupt enabled, prescale=256, "23 bit" counter
	 * (did they mean 32 bit?)
	 */
	*armTimerControl = 0x000000aa;
}
