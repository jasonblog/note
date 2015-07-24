# Hello world for bare metal ARM using QEMU

Last time I wrote about writing and debugging bare metal ARM software using the CodeSourcery toolchain. Now I want to exploit QEMU’s emulation of a complete system and create the simplest “Hello world!” example.


The QEMU emulator supports the VersatilePB platform, that contains an ARM926EJ-S core and, among other peripherals, four UART serial ports; the first serial port in particular (UART0) works  as a terminal when using the **-nographic or “-serial stdio” **qemu option. The memory map of the VersatilePB board is implemented in QEMU in this board-specific C source; from that I note the address where the UART0 is mapped: **0x101f1000**. The code that emulates the serial port inside QEMU (here in the source repository) implements a subset of the functionalities of the PL011 Prime Cell UART from ARM; there is a useful technical manual from the ARM info center that describes how to interact with the hardware. In details, there is a register (UARTDR) that is used to transmit (when writing in the register) and receive (when reading) bytes; this register is placed at offset **0x0**, so I need to read and write at the beginning of the memory allocated for the UART0.

To implement the simple “Hello world!” printing, I wrote this test.c file:

```c
volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;

void print_uart0(const char *s) {
    while(*s != '\0') { /* Loop until end of string */
        *UART0DR = (unsigned int)(*s); /* Transmit char */
        s++; /* Next char */
    }
}


void c_entry() {
    print_uart0("Hello world!\n");
}
```
The code is pretty straightforward; a couple of details:

- The volatile keyword is necessary to instruct the compiler that the memory pointed by UART0DR can change or has effects independently of the program.
- The unsigned int type enforces 32-bits read and write access.
- The QEMU model of the PL011 serial port ignores the transmit FIFO capabilities; in a real system on chip the “Transmit FIFO Full” flag must be checked in the UARTFR register before writing on the UARTDR register.

The QEMU emulator is written especially to emulate Linux guest systems; for this reason its startup procedure is implemented specifically: the -kernel option loads a binary file (usually a Linux kernel) inside the system memory starting at address **0x00010000**. The emulator starts the execution at address **0x00000000**, where few instructions (already in place) are used to jump at the beginning of the kernel image. The interrupt table of ARM cores, usually placed at address **0x00000000**, is not present, and the peripheral interrupts are disabled at startup, as needed to boot a Linux kernel. Knowing this, to implement a working emulation I need to considerate a few things:

- The software must be compiled and linked to be placed at **0x00010000**
- I need to create a binary image of our program
- I can ignore interrupt handling for now

This is the startup.s assembler file I wrote, simplified from the one I wrote in the previous blog post:

```
.global _Reset
_Reset:
 LDR sp, =stack_top
 BL c_entry
 B .```

And this is the linker script test.ld, modified from last time to place the program at the right address (thanks to Gnurou for the suggestion to specify only the text section of startup.o):

```
ENTRY(_Reset)
SECTIONS
{
 . = 0x10000;
 .startup . : { startup.o(.text) }
 .text : { *(.text) }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 . = . + 0x1000; /* 4kB of stack memory */
 stack_top = .;
}```

To create the binary file, the CodeSourcery toolchain (arm-none-eabi type) must be installed and the PATH environmental variable must be set accordingly. These are the commands to run:

```sh
$ arm-none-eabi-as -mcpu=arm926ej-s -g startup.s -o startup.o
$ arm-none-eabi-gcc -c -mcpu=arm926ej-s -g test.c -o test.o
$ arm-none-eabi-ld -T test.ld test.o startup.o -o test.elf
$ arm-none-eabi-objcopy -O binary test.elf test.bin```

These commands create a test.elf program and a test.bin binary image that I can use with the QEMU emulator for ARM systems. In Debian, QEMU can be installed running “apt-get install qemu” as root, and the installation includes the qemu-system-arm program that I need in my example. In Ubuntu machines, this program is not present in the qemu package but is placed instead in the qemu-kvm-extras package; for this reason the “sudo apt-get install qemu-kvm-extras” command must be used to install it.

To run my program in the emulator, the command is:
```
$ qemu-system-arm -M versatilepb -m 128M -nographic -kernel test.bin
```

The -M option specifies the emulated system. The program prints “Hello world!” in the
terminal and runs indefinitely; to exit QEMU, press “Ctrl + a” and then “x”.

It is possible also to debug the program using the CodeSourcery version of gdb, because QEMU implements a gdb connector using a TCP connection. To do so, I run the emulator with the correct options as follows:
```
$ qemu-system-arm -M versatilepb -m 128M -nographic -s -S -kernel test.bin
```
This command freezes the system before executing any guest code, and waits for a connection on the TCP port 1234. From another terminal, I run arm-none-eabi-gdb and enter the commands:
```
target remote localhost:1234
file test.elf
```

This connects to the QEMU system and loads the debugging symbols of the test program, whose binary image is already loaded in the system memory. From there, it is possible to run the program with the continue command, single-step the program and debug it in general. The exit command in gdb closes both the debugger and the emulator.

To summarize the necessary steps to create a “Hello world” program:

- Install CodeSourcery toolchain
- Install QEMU (in particular qemu-system-arm)
- Write the test.c, startup.s and test.ld source files
- Build the test ELF and binary image
- Run QEMU ARM emulator using the created binary image as a kernel
- Run the gdb debugger and attach to QEMU
