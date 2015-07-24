# Simplest bare metal program for ARM


Bare metal programs run without an operating system beneath; coding on bare metal is useful to deeply understand how a hardware architecture works and what happens in the lowest levels of an operating system. I wanted to create a simple example of bare metal program for ARM using free open source tools: RealView Development Suite is the state of the art of ARM compilers, but it is expensive for hobbyists; Codesourcery is a company that provides a free version of the GNU gcc toolchain for ARM cores. In particular, the EABI toolchain must be downloaded from their download page; I fetched the IA32 GNU/Linux installer. During the graphical installation, the tools are installed in a sub-folder of the user’s home; this is fine if only a single person wants to use the toolchain on that computer, otherwise it is more efficient to install it system-wide. The path to the toolchain binaries must be added to the PATH environmental variable; usually the installation process does it for you, but if it doesn’t, the standard installation path is “~/CodeSourcery/Sourcery_G++_Lite/bin“.

I created a C file called test.c containing the simplest C code I wanted to compile:

```c
int c_entry() {
  return 0;
}
```

The classic printf(“Hello world!\n”); example is more complex because when coding bare metal the standard input/output must be defined: it could be a physical serial port for example. I called it c_entry instead of main because in this example some things that are usually assumed true when the program reaches the main code are not implemented: for example, variable initialized globally in C code could not be really initialized.

To compile this code into an object file (test.o) run the following command, very similar to compiling code with gcc:


```
arm-none-eabi-gcc -c -mcpu=arm926ej-s -g test.c -o test.o
```


The -mcpu flag indicates the processor for which the code is compiled. I wanted to target the ARM926EJ-S processor in this example for these reasons:

- It’s a widespread core in common products
- I worked on a project that used this core
- The QEMU emulator supports this core in the form of a VersatilePB


In order to create a bare metal program we must understand what does the processor do when it is switched on. The ARM9 architecture begins to execute code at a determined address, that could be 0 (usually allocated to RAM) or 0xFFFF0000 (usually allocated to Read Only Memory). We must put some special code at that particular address: the interrupt vector table. It is a series of 32-bit instructions that are executed when something special happens: for example when the ARM core is reset, or when the memory contains an unknown instruction that doesn’t belong to the ARM instruction set, or when a peripheral generates an interrupt (the serial port received a byte). The instructions in the interrupt vector table usually make the processor jump to the code that handles the event. The jump can be done with a branch instruction (B in ARM assembly) when the destination address is near.

I created an assembly file called startup.s containing the following code:

```sh
.section INTERRUPT_VECTOR, "x"
.global _Reset
_Reset:
  B Reset_Handler /* Reset */
  B . /* Undefined */
  B . /* SWI */
  B . /* Prefetch Abort */
  B . /* Data Abort */
  B . /* reserved */
  B . /* IRQ */
  B . /* FIQ */

Reset_Handler:
  LDR sp, =stack_top
  BL c_entry
  B .
```

A brief explanation:
- Line 1 generates a section named INTERRUPT_VECTOR containing executable (“x”) code.
- Line 2 exports the name _Reset to the linker in order to set the program entry point.
- Line 3 to 11 is the interrupt vector table that contains a series of branches. The notation “B .” means that the code branches on itself and stays there forever like an endless for(;;);
- Line 14 initializes the stack pointer, that is necessary when calling C functions. The top of the stack (stack_top) will be defined during linking.
- Line 15 calls the c_entry function, and saves the return address in the link register (lr).


To compile this code into an object file (startup.o) run the following command:

```
$ arm-none-eabi-as -mcpu=arm926ej-s -g startup.s -o startup.o
```

Now we have test.o and startup.o, that must be linked together to become a program. The linking process also defines the address where the program is going to be executed and declares the placement of its sections. To give this information to the linker, a linker script is used. I wrote this linker script, called test.ld, following a simple example in the linker manual:
```sh
ENTRY(_Reset)
SECTIONS
{
 . = 0x0;
 .text : {
 startup.o (INTERRUPT_VECTOR)
 *(.text)
 }
 .data : { *(.data) }
 .bss : { *(.bss COMMON) }
 . = ALIGN(8);
 . = . + 0x1000; /* 4kB of stack memory */
 stack_top = .;
}
```
The script tells the linker to place the INTERRUPT_VECTOR section at address 0, and then subsequently place the code (.text), initialized data (.data) and zero-initialized and uninitialized data (.bss). Line 11 and 12 tells the linker to move 4kByte from the end of the useful sections and then place the stack_top symbol there. Since the stack grows downwards the stack pointer should not exceed its own zone, otherwise it will corrupt lower sections. The script on line 1 tells the linker also that the entry point is at _Reset. To link the program, execute the following command:
```
$ arm-none-eabi-ld -T test.ld test.o startup.o -o test.elf
```

This will generate an ELF binary for ARM that can be executed with a simulator, or it can be loaded inside a real ARM core on a hardware board; for simplicity we can use the Codesourcery version of the gdb debugger:

```
$ arm-none-eabi-gdb test.elf
```
```
[...]
This GDB was configured as "--host=i686-pc-linux-gnu --target=arm-none-eabi".
[...]
(gdb) target sim
Connected to the simulator.
(gdb) load
Loading section .text, size 0x50 vma 0x0
Start address 0x0
Transfer rate: 640 bits in <1 sec.
(gdb) break c_entry
Breakpoint 1 at 0x3c: file test.c, line 24.
(gdb) run
Starting program: /home/francesco/src/arm-none-eabi/startup/test.elf

Breakpoint 1, c_entry () at test.c:24
24        return 0;
(gdb) set $pc=0
(gdb) stepi
Reset_Handler () at startup.s:34
34        LDR sp, =stack_top
```


- The target sim command tells the debugger to use its internal ARM simulator,
- the load command fills the simulator memory with the binary code,
- the debugger places a breakpoint at the beginning of the c_entry function,
- the program is executed and stops at the breakpoint,
- the program counter (pc register) of the ARM core is set to 0 to emulate a software reset,
- the execution flow can be examined step-by-step in the debugger.
-
An easier way to debug is using the ddd graphical front-end with the following command:

```
$ ddd --debugger arm-none-eabi-gdb test.elf
```

This program is a starting point to begin to develop more elaborate solutions. The next step I want to take is using QEMU as the development target: with it I can interact with some peripherals, even if emulated, and create bare metal embedded programs more useful in the “real world” using only free open source software.

For a deeper look into bare metal programming for ARM see also:

[Building bare metal ARM with GNU [pdf]](http://www.state-machine.com/arm/Building_bare-metal_ARM_with_GNU.pdf)<BR>
[Building bare metal ARM with GNU [html]](http://www.embedded.com/200000632)


