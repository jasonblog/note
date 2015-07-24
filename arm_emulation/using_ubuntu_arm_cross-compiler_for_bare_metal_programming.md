# Using Ubuntu ARM cross-compiler for bare metal programming

The newest Ubuntu release 10.10, codenamed Maverick Meerkat, includes an ARM cross-compiler in the package gcc-arm-linux-gnueabi. This compiler is a welcome addition (thanks to the maintainer Marcin Juszkiewicz and to the Linaro team) to the default repositories, and it is in line both with the Ubuntu roadmap to appear also on ARM-based devices and with the ARM roadmap to appear also on server solutions.


The main purpose of the package is to allow cross-compiling of Linux programs for Ubuntu ARM distributions, but it can be used also for bare metal programming. One thing to be aware is the fact that the compiler by default tries to link the Linux standard C
libraries that in bare metal programming have no use.


I’m writing a simple “Hello World” program just like the one I used for my previous post: Hello world for bare metal ARM using QEMU, only this time for the Versatile Platform Baseboard for Cortex-A8, that can be emulated by QEMU too. The requisites are Ubuntu 10.10, the cross-compiler and the QEMU arm emulator that can be installed with the command:

```
$ sudo apt-get install gcc-arm-linux-gnueabi qemu-kvm-extras
```

The memory map for the board is specified in its manual; in particular, the first UART is at address 0x10009000. I modified the  “test.c” code that sends the string “Hello World!” to the UART, using the correct address:
```c
volatile unsigned int * const UART0DR = (unsigned int *)0x10009000;

void print_uart0(const char *s) {
    while(*s != '\0') { /* Loop until end of string */
        *UART0DR = *s; /* Transmit char */
        s++; /* Next char */
    }
}

void c_entry() {
    print_uart0("Hello world!\n");
}
```
The first code to be executed is written in “startup.s” like in the last example.

```c
.global _Reset
_Reset:
    LDR sp, =stack_top
    BL c_entry
    B .```

It initializes the stack pointer to a value that is given inside the linker script, and then jumps to the c_entry function. The linker script is the following:

```c
SECTIONS
{
  . = 0x10000;
  .ro : {
    startup.o (.text)
    *(.text)
    *(.rodata)
  }
  .rw : {
    *(.data)
    *(.bss)
    *(COMMON)
  }
  . = ALIGN(8);
  . = . + 0x1000; /* 4kB of stack memory */
  stack_top = .;
}```


The linker scripts separates two sections of the program: a read-only section containing the code and the constants, and a read-write section containing the data. The 0x10000 address is the place where QEMU puts the code and starts executing. To create the ELF executable, the commands are:
```sh
 arm-linux-gnueabi-gcc -mcpu=cortex-a8   -c -o test.o test.c
$ arm-linux-gnueabi-as -mcpu=cortex-a8  -o startup.o startup.s
$ arm-linux-gnueabi-gcc -nostdlib -T test.ld -Xlinker "--build-id=none"  test.o startup.o   -o test```


The linker phase options are the most important. The “-nostdlib” option tells the linker to avoid linking the startup files and standard C libraries. I needed to specify the “--build-id=none” options  because the compiler placed a section of notes just at the beginning of the code, but since I needed the _Reset code to be exactly at 0x10000, it ruined the program execution. Instead of using GCC as a frontend, the linker can be invoked directly with the following command, with the same results:

```
$ arm-linux-gnueabi-ld -T test.ld test.o startup.o -o test
```
To generate a binary suitable for QEMU and emulate the platform, the commands are the following:
```
$ arm-linux-gnueabi-objcopy -O binary test test.bin
$ qemu-system-arm -M realview-pb-a8 -serial stdio -kernel test.bin
Hello world!
```
The cross-compiler can be used in this simple example but, with respect to the CodeSourcery bare metal toolchain, it does not contain the newlib library or the startup code. For more elaborate examples that use the standard C libraries, an implementation of these libraries must be compiled and the correct include directories must be pointed.
