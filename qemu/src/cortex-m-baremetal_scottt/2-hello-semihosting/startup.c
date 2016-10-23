int main();

/* defined in the linker script gcc.ld */
extern int __etext, __data_start__, __data_end__, __bss_start__, __bss_end__;

/* make gcc not translate the data copy loop into a memcpy() call
 *
 * See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56888
 * Note that just passing optimize("freestanding", "no-builtin")
 * as a function attribute here doesn't work on
 * gcc-arm-embedded 2014 (gcc 4.9.3) */
__attribute__((optimize("freestanding", "no-builtin",
			"no-tree-loop-distribute-patterns")))
void Reset_Handler()
{
	int *restrict src, *restrict dst;

	src = __builtin_assume_aligned(&__etext, 4); 
	dst = __builtin_assume_aligned(&__data_start__, 4);
	while (dst != __builtin_assume_aligned(&__data_end__, 4))
		*dst++ = *src++;

	dst = __builtin_assume_aligned(&__bss_start__, 4);
	while (dst != __builtin_assume_aligned(&__bss_end__, 4))
		*dst++ = 0;

	main();
}


void Default_Handler(void) { for (;;) ; }
void NMI_Handler(void) { for (;;) ; }
void HardFault_Handler(void) { for (;;) ; }
void MemManage_Handler(void) { for (;;) ; }
void BusFault_Handler(void) { for (;;) ; }
void UsageFault_Handler(void) { for (;;) ; }
void SVC_Handler(void) { for (;;) ; }
void DebugMon_Handler(void) { for (;;) ; }
void PendSV_Handler(void) { for (;;) ; }
void SysTick_Handler(void) { for (;;) ; }

extern const char __stack; /* defined by linker script */
const void *isr_vectors[] __attribute__ ((section (".isr_vector"))) = {
	&__stack,
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0, /* Reserved */
	SVC_Handler,
	DebugMon_Handler,
	0, /* Reserved */
	PendSV_Handler,
	SysTick_Handler,

	/* External Interrupts */
	Default_Handler,
};
