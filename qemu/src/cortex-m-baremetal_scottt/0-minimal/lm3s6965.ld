/* Tested on "qemu-system-arm -M lm3s6965evb" */

MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
	RAM  (rwx) : ORIGIN = 0x20000000, LENGTH =  64K
}

INCLUDE gcc.ld
