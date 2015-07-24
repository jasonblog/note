# ARM926 interrupts in QEMU
In this post I prepared what I think is the simplest example on how to manage interrupts for the widespread ARM926 core. From this example one can expand the complexity of the interrupt management at will. I’m going to test the functionality with QEMU, emulating the Versatile Platform Baseboard. I based this example on my old post Hello world for bare metal ARM using QEMU.

The ARM926 is able to detect and manage a certain number of exceptions, for example:

- “undefined” exception: when the core tries to execute an instruction that is not mapped on the instruction set
- data abort: when the system bus reports an error in trying to access one of the peripherals
- IRQ: the most common exception is the arrival of an interrupt request (IRQ) from a peripheral.


When an exception occurs, the ARM926 core changes its operating “mode” and jumps to the beginning of the memory, with an offset from address 0 that depends on the exception. For example, when an “undefined” exception happens, the core jumps to address 4. By placing instruction at those addresses, it is possible to manage these exceptions with custom functions. The common way is to place a “jump” at each of those addresses, creating an
“exception vector table” (where the vector is the jump instruction).

The following is an assembly file called “vectors.S” that shows an example of vector table and reset handler, which contains the minimal initialization that needs to be done before jumping to C code.
```c
.text
 .code 32

 .global vectors_start
 .global vectors_end

vectors_start:
 LDR PC, reset_handler_addr
 LDR PC, undef_handler_addr
 LDR PC, swi_handler_addr
 LDR PC, prefetch_abort_handler_addr
 LDR PC, data_abort_handler_addr
 B .
 LDR PC, irq_handler_addr
 LDR PC, fiq_handler_addr

reset_handler_addr: .word reset_handler
undef_handler_addr: .word undef_handler
swi_handler_addr: .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr: .word data_abort_handler
irq_handler_addr: .word irq_handler
fiq_handler_addr: .word fiq_handler

vectors_end:

reset_handler:
 /* set Supervisor stack */
 LDR sp, =stack_top
 /* copy vector table to address 0 */
 BL copy_vectors
 /* get Program Status Register */
 MRS r0, cpsr
 /* go in IRQ mode */
 BIC r1, r0, #0x1F
 ORR r1, r1, #0x12
 MSR cpsr, r1
 /* set IRQ stack */
 LDR sp, =irq_stack_top
 /* Enable IRQs */
 BIC r0, r0, #0x80
 /* go back in Supervisor mode */
 MSR cpsr, r0
 /* jump to main */
 BL main
 B .

.end
```

Some details about the assembly code:

The vector table must be placed at address 0, but when the program is executed, it is possible that it is not loaded at the beginning of the memory. For example QEMU loads the binary code at address 0x10000. For this reason the vector table needs to be copied before being useful. This is done by using global symbols to mark the beginning (vectors_start) and end (vectors_end) of the vectors area, and then using a function (copy_vectors, implemented below in the C code) to copy it to the correct location.

When the core receives an exception it changes operating mode, and this means (among other things) that it uses a different stack pointer. For this reason before enabling interrupts it is necessary to configure the stack for the modes that we intend to use.

The operating mode can be changed manually by accessing the Program Status Register (cpsr), which must also be used to enable IRQs. More information on the ARM9EJ-S Technical Reference Manual.

I want to use the UART as a simple peripheral that uses IRQ to function. The Versatile manual indicates that an Interrupt Controller is used to manage the various IRQs. The following manuals are needed to understand what to do:


- RealView Platform Baseboard for ARM926EJ–S User Guide
- PrimeCell UART (PL011) Technical Reference Manual
- PrimeCell Vectored Interrupt Controller (PL190) Technical Reference Manual


In order to enable IRQ for UART, the interrupt must be enabled in three places:

- The IRQs must be enabled in the ARM cpsr
- The UART IRQ must be enabled in the Interrupt Controller
- The interrupt generation must be enabled in UART registers for the chosen events

I will use the “RX” event to fire an interrupt that manages the arrival of a byte from the UART, and then send it back as a sort of “echo”. The following “test.c” file shows
how to do it:
```c
#include <stdint.h>

#define UART0_BASE_ADDR 0x101f1000
#define UART0_DR (*((volatile uint32_t *)(UART0_BASE_ADDR + 0x000)))
#define UART0_IMSC (*((volatile uint32_t *)(UART0_BASE_ADDR + 0x038)))

#define VIC_BASE_ADDR 0x10140000
#define VIC_INTENABLE (*((volatile uint32_t *)(VIC_BASE_ADDR + 0x010)))

void __attribute__((interrupt)) irq_handler() {
 /* echo the received character + 1 */
 UART0_DR = UART0_DR + 1;
}

/* all other handlers are infinite loops */
void __attribute__((interrupt)) undef_handler(void) { for(;;); }
void __attribute__((interrupt)) swi_handler(void) { for(;;); }
void __attribute__((interrupt)) prefetch_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) data_abort_handler(void) { for(;;); }
void __attribute__((interrupt)) fiq_handler(void) { for(;;); }

void copy_vectors(void) {
 extern uint32_t vectors_start;
 extern uint32_t vectors_end;
 uint32_t *vectors_src = &vectors_start;
 uint32_t *vectors_dst = (uint32_t *)0;

while(vectors_src < &vectors_end)
 *vectors_dst++ = *vectors_src++;
}

void main(void) {
 /* enable UART0 IRQ */
 VIC_INTENABLE = 1<<12;
 /* enable RXIM interrupt */
 UART0_IMSC = 1<<4;
 for(;;);
}
```
The main code enables the interrupt and then waits forever. When a character is received from the UART, the IRQ is fired and the irq_handler function is called, transmitting back the modified character.

In order to create the complete binary code, we need a linker script that is aware of the memory map of the system. In our case QEMU loads the code to address 0x10000. The following is the linker script “test.ld” that is used to link the complete program:

```c
ENTRY(vectors_start)
SECTIONS
{
 . = 0x10000;
 .text : {
 vectors.o
 *(.text .rodata)
 }
 .data : { *(.data) }
 .bss : { *(.bss) }
 . = ALIGN(8);
 . = . + 0x1000; /* 4kB of stack memory */
 stack_top = .;
 . = . + 0x1000; /* 4kB of irq stack memory */
 irq_stack_top = .;
}
```
To compile the program I used CodeSourcery bare metal toolchain, but the commands can be adapted to work with other GCC toolchains such as the Emdebian ones. The commands are the following:
```
arm-none-eabi-gcc -mcpu=arm926ej-s -c -o test.o test.c
arm-none-eabi-gcc -mcpu=arm926ej-s -c -o vectors.o vectors.S
arm-none-eabi-gcc -T test.ld test.o vectors.o -o test
arm-none-eabi-objcopy -O binary test test.bin
```

This creates a “test.bin” binary file that contains our code. To simulate the program, the command to launch QEMU is the following:
```
qemu-system-arm -M versatilepb -serial stdio -kernel test.bin
```

The **“-serial stdio”** option will redirect the terminal input/output to the emulated UART that we want to test. If you type some letters in the terminal where you launched the command, you will see them echoed back to you, modified by the interrupt handler.

Possible next steps from here are:

- Managing different sources of interrupt from the same peripheral
- Managing IRQs from different peripherals
- Dynamically remapping the exception handlers
- Fully using the features of the Vectored Interrupt Controller
- Enabling nested interrupts
Here is a guide that contains much information about it: [Building bare metal ARM with GNU [html]](http://www.embedded.com/200000632)
