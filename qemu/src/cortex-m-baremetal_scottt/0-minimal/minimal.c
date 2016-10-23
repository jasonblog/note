/* Minimal Cortex-M program that:
 * 1. boots
 * 2. writes to a global variable
 */

__asm__(".word 0x20000000 + 0x10000"); /* Initial value of the stack pointer  */
__asm__(".word Reset_Handler");        /* Address of function called at reset */

/* Declaring 'x' volatile so the compiler doesn't optimize it away */
volatile int x;

int Reset_Handler(void)
{
	x = 17;

	for (;;)
		;
}
