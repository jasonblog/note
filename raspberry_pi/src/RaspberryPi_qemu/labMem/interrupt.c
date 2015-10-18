/*
   interrupt.s  
   Carlos Hsu <karose.tzu@gmail.com>

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
  printk("IRQ_handler\n");
  
   /*reset timer*/ 
   *armTimerIRQClear = 0;

  enable_IRQ();
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
