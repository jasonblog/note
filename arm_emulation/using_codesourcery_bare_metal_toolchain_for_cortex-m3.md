# Using CodeSourcery bare metal toolchain for Cortex-M3
CodeSourcery provides toolchains for many architectures, and among them there are “bare metal” toolchains that produce programs that can be run natively on the processor.
In the past I already explored some possibilities about developing bare metal ARM programs:

- Simplest bare metal program for ARM
- Hello world for bare metal ARM using QEMU
- Using Ubuntu ARM cross-compiler for bare metal programming
- Using Newlib in ARM bare metal programs
-
In my opinion the most interesting purpose of the bare metal toolchains is development on small microcontrollers, such as the ones based on Cortex-M processors.

My beloved QEMU is able to make me try a system without buying the hardware first; in particular, mainline QEMU can emulate the Texas Instruments Stellaris lm3s6965 microcontroller, which is a Cortex-M3 based chip that has 256KiB of Flash, 64KiB of RAM,
some serial ports, ADConverters, timers and many other peripherals.

I wanted to be able to write a small “Hello World” program that prints on the serial port with as little effort as possible (in terms of code to write), and emulate it on QEMU. Basically I want to be able to compile and run this “main.c“:
```c
#include <stdio.h>

int main() {
  printf("Hello World!\n");
  return 0;
}
```
For this purpose, I installed the “qemu-system” package on my Debian machine, and
installed the “Sourcery G++ Lite 2011.03-42 for ARM EABI” toolchain.

The “arm-none-eabi” toolchain includes Newlib as an implementation of the C library, but it also contains a particular library that is linked by default in compiled programs, which is called CS3™: the CodeSourcery Common Startup Code Sequence. This library is associated with linker scripts for generic and specific platforms. All this specific low-level code is present in many version inside the installation package, because there are many ARM architectures. In my case, since I wanted to develop for Cortex-M3, I need to use the “thumb2” versions of the library, as stated in the “Getting Started” guide of the toolchain. To do so, I must supply some flags to the compiler: “-mthumb -march=armv7
-mfix-cortex-m3-ldrd“.


A correct linker script must be supplied, to indicate the memory map and where to put the code and data. The arm-none-eabi toolchain package contains a generic linker script in the path “.../Sourcery_G++_Lite/arm-none-eabi/lib/thumb2/generic-m.ld” that I modified in
the following way into a file called “lm3s6965.ld“:

```c
/* Linker script for lm3s6965 ARM Cortex-M3 microcontroller
*
* Modified from generic-m.ld file shipped in:
*   Sourcery G++ Lite 2011.03-42
*
* Version: Sourcery G++ Lite 2011.03-42
* Support: https://support.codesourcery.com/GNUToolchain/
*
* Copyright (c) 2007, 2008, 2009, 2010 CodeSourcery, Inc.
*
* The authors hereby grant permission to use, copy, modify, distribute,
* and license this software and its documentation for any purpose, provided
* that existing copyright notices are retained in all copies and that this
* notice is included verbatim in any distributions.  No written agreement,
* license, or royalty fee is required for any of the authorized uses.
* Modifications to this software may be copyrighted by their authors
* and need not follow the licensing terms described here, provided that
* the new terms are clearly indicated on the first page of each file where
* they apply.
*/
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(__cs3_reset)
SEARCH_DIR(.)
GROUP(-lgcc -lc -lcs3 -lcs3unhosted -lcs3micro)

MEMORY
{
flash (rx) : ORIGIN = 0x00000000, LENGTH = 256K
ram  (rwx) : ORIGIN = 0x20000000, LENGTH =  64K
}

/* These force the linker to search for particular symbols from
* the start of the link process and thus ensure the user's
* overrides are picked up
*/
EXTERN(__cs3_reset __cs3_reset_generic_m)
EXTERN(__cs3_start_asm __cs3_start_asm_sim)
/* Bring in the interrupt routines & vector */
INCLUDE micro-names.inc
EXTERN(__cs3_interrupt_vector_micro)
EXTERN(__cs3_start_c main __cs3_stack __cs3_heap_end)

/* Provide fall-back values */
PROVIDE(__cs3_heap_start = _end);
PROVIDE(__cs3_heap_end = __cs3_region_start_ram + __cs3_region_size_ram);
PROVIDE(__cs3_region_num = (__cs3_regions_end - __cs3_regions) / 20);
PROVIDE(__cs3_stack = __cs3_region_start_ram + __cs3_region_size_ram);

SECTIONS
{
.text :
{
CREATE_OBJECT_SYMBOLS
__cs3_region_start_flash = .;
_ftext = .;
*(.cs3.region-head.flash)
ASSERT (. == __cs3_region_start_flash, ".cs3.region-head.flash not permitted");
__cs3_interrupt_vector = __cs3_interrupt_vector_micro;
*(.cs3.interrupt_vector)
/* Make sure we pulled in an interrupt vector.  */
ASSERT (. != __cs3_interrupt_vector_micro, "No interrupt vector");

PROVIDE(__cs3_reset = __cs3_reset_generic_m);
*(.cs3.reset)
__cs3_start_asm_sim = DEFINED(__cs3_start_asm) ? __cs3_start_asm : __cs3_start_asm_sim;

*(.text.cs3.init)
*(.text .text.* .gnu.linkonce.t.*)
*(.plt)
*(.gnu.warning)
*(.glue_7t) *(.glue_7) *(.vfp11_veneer)

*(.ARM.extab* .gnu.linkonce.armextab.*)
*(.gcc_except_table)
} >flash
.eh_frame_hdr : ALIGN (4)
{
KEEP (*(.eh_frame_hdr))
} >flash
.eh_frame : ALIGN (4)
{
KEEP (*(.eh_frame))
} >flash
/* .ARM.exidx is sorted, so has to go in its own output section.  */
PROVIDE_HIDDEN (__exidx_start = .);
.ARM.exidx :
{
*(.ARM.exidx* .gnu.linkonce.armexidx.*)
} >flash
PROVIDE_HIDDEN (__exidx_end = .);
.rodata : ALIGN (4)
{
*(.rodata .rodata.* .gnu.linkonce.r.*)

. = ALIGN(4);
KEEP(*(.init))

. = ALIGN(4);
__preinit_array_start = .;
KEEP (*(.preinit_array))
__preinit_array_end = .;

. = ALIGN(4);
__init_array_start = .;
KEEP (*(SORT(.init_array.*)))
KEEP (*(.init_array))
__init_array_end = .;

. = ALIGN(4);
KEEP(*(.fini))

. = ALIGN(4);
__fini_array_start = .;
KEEP (*(.fini_array))
KEEP (*(SORT(.fini_array.*)))
__fini_array_end = .;

. = ALIGN(0x4);
KEEP (*crtbegin.o(.ctors))
KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
KEEP (*(SORT(.ctors.*)))
KEEP (*crtend.o(.ctors))

. = ALIGN(0x4);
KEEP (*crtbegin.o(.dtors))
KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
KEEP (*(SORT(.dtors.*)))
KEEP (*crtend.o(.dtors))

. = ALIGN(4);
__cs3_regions = .;
LONG (0)
LONG (__cs3_region_init_ram)
LONG (__cs3_region_start_ram)
LONG (__cs3_region_init_size_ram)
LONG (__cs3_region_zero_size_ram)
__cs3_regions_end = .;
. = ALIGN (8);
_etext = .;
} >flash

ASSERT (!(__cs3_region_init_ram & 7), "__cs3_region_init_ram not aligned")
ASSERT (!(__cs3_region_start_ram & 7), "__cs3_region_start_ram not aligned")
ASSERT (!(__cs3_region_init_size_ram & 7), "__cs3_region_init_size_ram not aligned")
ASSERT (!(__cs3_region_zero_size_ram & 7), "__cs3_region_zero_size_ram not aligned")
.data : ALIGN (8)
{
__cs3_region_start_ram = .;
KEEP(*(.jcr))
*(.got.plt) *(.got)
*(.shdata)
*(.data .data.* .gnu.linkonce.d.*)
. = ALIGN (8);
*(.ram)
. = ALIGN (8);
_edata = .;
} >ram AT>flash
.bss : ALIGN (8)
{
*(.shbss)
*(.bss .bss.* .gnu.linkonce.b.*)
*(COMMON)
. = ALIGN (8);
*(.ram.b .bss.ram)
. = ALIGN (8);
_end = .;
__end = .;
} >ram
__cs3_region_init_ram = LOADADDR(.data);
__cs3_region_init_size_ram = _edata - __cs3_region_start_ram;
__cs3_region_zero_size_ram = _end - _edata;
__cs3_region_size_ram = LENGTH(ram);

.stab 0 (NOLOAD) : { *(.stab) }
.stabstr 0 (NOLOAD) : { *(.stabstr) }
/* DWARF debug sections.
* Symbols in the DWARF debugging sections are relative to
* the beginning of the section so we begin them at 0.
*/
/* DWARF 1 */
.debug          0 : { *(.debug) }
.line           0 : { *(.line) }
/* GNU DWARF 1 extensions */
.debug_srcinfo  0 : { *(.debug_srcinfo) }
.debug_sfnames  0 : { *(.debug_sfnames) }
/* DWARF 1.1 and DWARF 2 */
.debug_aranges  0 : { *(.debug_aranges) }
.debug_pubnames 0 : { *(.debug_pubnames) }
/* DWARF 2 */
.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
.debug_abbrev   0 : { *(.debug_abbrev) }
.debug_line     0 : { *(.debug_line) }
.debug_frame    0 : { *(.debug_frame) }
.debug_str      0 : { *(.debug_str) }
.debug_loc      0 : { *(.debug_loc) }
.debug_macinfo  0 : { *(.debug_macinfo) }
/* DWARF 2.1 */
.debug_ranges   0 : { *(.debug_ranges) }
/* SGI/MIPS DWARF 2 extensions */
.debug_weaknames 0 : { *(.debug_weaknames) }
.debug_funcnames 0 : { *(.debug_funcnames) }
.debug_typenames 0 : { *(.debug_typenames) }
.debug_varnames  0 : { *(.debug_varnames) }

.note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
.ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
/DISCARD/ : { *(.note.GNU-stack) }
}
```
The lm3s6965 manual indicates that the 256KiB Flash is placed at address 0x00000000, while the 64KiB RAM starts at address 0x20000000, and it is reflected in the “MEMORY” part of the linker script. One other important modifications with respect to the original linker script is putting the “.data” and “.bss” section into RAM, because it is a read-write memory, while the Flash memory is read-only in normal situations. The Flash memory will be filled by QEMU with a binary file that we provide, and the CS3 code will take care of initializing the RAM at boot time.

We need to override the reset code of the CS3 library because the default behavior is implemented to be run inside a generic simulator. Instead we can simply create an assembly file that implements the very first code to be executed and then calls the CS3 functions (“__cs3_start_c” to be precise). I created the following “reset.S” assembly file:

```c
.text
.thumb

.global __cs3_reset

.thumb_func
__cs3_reset:
  # add peripherals and memory initialization here
  LDR r0, =__cs3_start_asm
  BX r0

.thumb_func
__cs3_start_asm:
  # add assembly initializations here
  LDR r0, =__cs3_start_c
  BX r0

.end
```

The last thing needed to compile is low-level input/output functions that connect the “stdio” functions to the microcontroller’s UART. From the microcontroller’s manual it seems that the UART in question is the ARM PrimeCell PL011 that I already explored in a previous post; also, the start address of the UART registers is 0x4000C000. I can use the following “syscalls.c” file that I already created for Newlib tests, and that I took mainly from this tutorial:

```c
#include <sys/stat.h>

enum {
    UART_FR_RXFE = 0x10,
    UART_FR_TXFF = 0x20,
    UART0_ADDR = 0x4000C000,
};

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

int _close(int file)
{
    return 0;
}

int _fstat(int file, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _open(const char* name, int flags, int mode)
{
    return -1;
}

int _read(int file, char* ptr, int len)
{
    int todo;

    if (len == 0) {
        return 0;
    }

    while (UART_FR(UART0_ADDR) & UART_FR_RXFE);

    *ptr++ = UART_DR(UART0_ADDR);

    for (todo = 1; todo < len; todo++) {
        if (UART_FR(UART0_ADDR) & UART_FR_RXFE) {
            break;
        }

        *ptr++ = UART_DR(UART0_ADDR);
    }

    return todo;
}

static char* heap_end = 0;
caddr_t _sbrk(int incr)
{
    extern char __cs3_heap_start; /* Defined by the linker */
    extern char __cs3_heap_end; /* Defined by the linker */
    char* prev_heap_end;

    if (heap_end == 0) {
        heap_end = &__cs3_heap_start;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > &__cs3_heap_end) {
        /* Heap and stack collision */
        return (caddr_t)0;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int _write(int file, char* ptr, int len)
{
    int todo;

    for (todo = 0; todo < len; todo++) {
        UART_DR(UART0_ADDR) = *ptr++;
    }

    return len;
}

```

Now it’s time to compile the program, create the binary code that must be written in the Flash, and call QEMU to emulate it:

```
$ arm-none-eabi-gcc -mthumb -march=armv7 -mfix-cortex-m3-ldrd -T lm3s6965.ld main.c reset.S syscalls.c -o main
$ arm-none-eabi-objcopy -O binary main main.bin
$ qemu-system-arm -M lm3s6965evb --kernel main.bin --serial stdio
Hello World!
```

From this point, it is possible to create a more complex application using the C standard library as needed. This application should work also on real hardware with some modifications on the UART code, that currently does not check if the write buffer is full and does not initialize the port speed, for example. Other than that, the embedded Flash must be written with the binary file using the tools that the microcontroller vendor provides.
