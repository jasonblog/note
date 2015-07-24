# Using Newlib in ARM bare metal programs

When dealing with embedded software, I often find useful to have the standard C functions we all learn during our first programming course: printf, malloc, getchar, strncpy, …
A common way to have them is using Newlib. Newlib is an implementation of the standard C library that is specifically thought to run somewhere with low resources and undefined hardware. The idea of Newlib is to implement the hardware-independent parts of the standard C library and rely on few low-level system calls that must be implemented with the target hardware in mind.

I compiled Newlib with the CodeSourcery ARM compiler for bare-metal targets: Sourcery G++ Lite 2010.09-51. Once the toolchain is installed, the commands I used to download compile Newlib are:

```
$ wget ftp://sources.redhat.com/pub/newlib/newlib-1.18.0.tar.gz
$ tar xzf newlib-1.18.0.tar.gz
$ cd newlib-1.18.0/
$ ./configure --target arm-none-eabi --disable-newlib-supplied-syscalls
$ make
$ cd ..
```

The “--disable-newlib-supplied-syscalls” option is necessary because otherwise Newlib compiles some pre-defined libraries for ARM that are useful in conjunction with debug features such as the RDI monitor. Many different outputs are compiled inside the arm-none-eabi subdirectory, but in particular the “libc.a” archive in the newlib directory is the one I use.
As an example, I want to compile the following “test.c” program:

```c
#include
#include <stdlib.h>

extern char* heap_end; /* Defined in syscalls.c */

void c_entry()
{
    char c;
    char* ptr = NULL;
    size_t alloc_size = 1;

    do {
        c = getchar();
        printf("%d: %c\n", c, c);

        ptr = realloc(ptr, alloc_size);

        if (ptr == NULL) {
            puts("Out of memory!\nProgram halting.");

            for (;;);
        } else {
            printf("new alloc of %d bytes at address 0x%X\n", alloc_size,
                   (unsigned int)ptr);
            alloc_size <<= 1;
            printf("Heap end = 0x%X\n", (unsigned int)heap_end);
        }
    } while (1);
}

```

The program uses standard input/output functions and allocates more and more memory with realloc, printing some information to understand what’s going on. The idea is to use the UART serial port as the input/output, and monitor how the memory is managed. The heap_end variable is something that I will implement and use later to manage the memory allocation.
The program needs at least some basic starting code “startup.s” that I borrow from a previous example of mine:


```
LDR sp, =stack_top
BL c_entry
B .```

And the linker script “test.ld” again is a modified version from this post:

```
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
 heap_low = .; /* for _sbrk */
 . = . + 0x10000; /* 64kB of heap memory */
 heap_top = .; /* for _sbrk */
 . = . + 0x10000; /* 64kB of stack memory */
 stack_top = .; /* for startup.s */
}```

One thing that the program needs is the libgcc library: it provides some basic hardware functionalities, such as the division, that are used by the compiler. If the linking phase complains about some undefined reference to “__aeabi_...” symbol, it probably means that there’s some problem regarding the libgcc library. The CodeSourcery toolchain version 2010.09-51 comes with its own version of this library inside the folder “~/CodeSourcery/Sourcery_G++_Lite/lib/gcc/arm-none-eabi/4.5.1/libgcc.a“; the folder may slightly change depending on the toolchain version and installation path.
If I try to compile the program with the Newlib, this is the result:
```

$ arm-none-eabi-gcc -mcpu=cortex-a8 -I ./newlib-1.18.0/newlib/libc/include   -c -o test.o test.c
$ arm-none-eabi-as -mcpu=cortex-a8  -o startup.o startup.s
$ arm-none-eabi-gcc -nostdlib  -T test.ld  test.o startup.o  ./newlib-1.18.0/arm-none-eabi/newlib/libc.a /home/francesco/CodeSourcery/Sourcery_G++_Lite/lib/gcc/arm-none-eabi/4.5.1/libgcc.a -o test
test.o: In function `c_entry':
test.c:(.text+0x10c): undefined reference to `heap_end'
test.c:(.text+0x110): undefined reference to `heap_end'
./newlib-1.18.0/arm-none-eabi/newlib/libc.a(lib_a-sbrkr.o): In function `_sbrk_r':
/home/francesco/src/arm/newlib/newlib-1.18.0/arm-none-eabi/newlib/libc/reent/../../../.././newlib/libc/reent/sbrkr.c:60: undefined reference to `_sbrk'
./newlib-1.18.0/arm-none-eabi/newlib/libc.a(lib_a-writer.o): In function `_write_r':
/home/francesco/src/arm/newlib/newlib-1.18.0/arm-none-eabi/newlib/libc/reent/../../../.././newlib/libc/reent/writer.c:58: undefined reference to `_write'
./newlib-1.18.0/arm-none-eabi/newlib/libc.a(lib_a-closer.o): In function `_close_r':
/home/francesco/src/arm/newlib/newlib-1.18.0/arm-none-eabi/newlib/libc/reent/../../../.././newlib/libc/reent/closer.c:53: undefined reference to `_close'
./newlib-1.18.0/arm-none-eabi/newlib/libc.a(lib_a-fstatr.o): In function `_fstat_r':
/home/francesco/src/arm/newlib/newlib-1.18.0/arm-none-eabi/newlib/libc/reent/../../../.././newlib/libc/reent/fstatr.c:62: undefined reference to `_fstat'
./newlib-1.18.0/arm-none-eabi/newlib/libc.a(lib_a-isattyr.o): In function `_isatty_r':
/home/francesco/src/arm/newlib/newlib-1.18.0/arm-none-eabi/newlib/libc/reent/../../../.././newlib/libc/reent/isattyr.c:58: undefined reference to `_isatty'
./newlib-1.18.0/arm-none-eabi/newlib/libc.a(lib_a-lseekr.o): In function `_lseek_r':
/home/francesco/src/arm/newlib/newlib-1.18.0/arm-none-eabi/newlib/libc/reent/../../../.././newlib/libc/reent/lseekr.c:58: undefined reference to `_lseek'
./newlib-1.18.0/arm-none-eabi/newlib/libc.a(lib_a-readr.o): In function `_read_r':
/home/francesco/src/arm/newlib/newlib-1.18.0/arm-none-eabi/newlib/libc/reent/../../../.././newlib/libc/reent/readr.c:58: undefined reference to `_read'```


Ignoring heap_end for now, the linker complains that some functions cannot be found. These functions are the system calls that must be implemented depending on the hardware. One example of a minimal implementation of these functions is here.

The “hardware” where I will make the program run is the QEMU emulation of the Versatile Platform Baseboard Cortex-A8 platform. According to the RealView® Platform Baseboard for Cortex™-A8 User Guide, there is a memory-mapped UART serial port at address 0x10009000, and my intention is to write output and read input using this UART.

The _sbrk function is used to increase the memory allocated by the program; modifying slightly the minimal implementation I use the memory space between the heap_low and the heap_top symbols that I defined inside the linker script. This is my implementation of **“syscalls.c“**:

```c
#include <sys/stat.h>

enum {
    UART_FR_RXFE = 0x10,
    UART_FR_TXFF = 0x20,
    UART0_ADDR = 0x10009000,
};

#define UART_DR(baseaddr) (*(unsigned int *)(baseaddr))
#define UART_FR(baseaddr) (*(((unsigned int *)(baseaddr))+6))

int _close(int file)
{
    return -1;
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

char* heap_end = 0;
caddr_t _sbrk(int incr)
{
    extern char heap_low; /* Defined by the linker */
    extern char heap_top; /* Defined by the linker */
    char* prev_heap_end;

    if (heap_end == 0) {
        heap_end = &heap_low;
    }

    prev_heap_end = heap_end;

    if (heap_end + incr > &heap_top) {
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



The compilation and emulation is done with the following commands; QEMU will open a graphic window but the terminal will accept key strokes and pass it to the emulated UART:

```
$ arm-none-eabi-gcc -mcpu=cortex-a8 -I ./newlib-1.18.0/newlib/libc/include   -c -o test.o test.c
$ arm-none-eabi-as -mcpu=cortex-a8  -o startup.o startup.s
$ arm-none-eabi-gcc -mcpu=cortex-a8 -I ./newlib-1.18.0/newlib/libc/include   -c -o syscalls.o syscalls.c
$ arm-none-eabi-gcc -nostdlib  -T test.ld   test.o startup.o syscalls.o  ./newlib-1.18.0/arm-none-eabi/newlib/libc.a   ~/CodeSourcery/Sourcery_G++_Lite/lib/gcc/arm-none-eabi/4.5.1/libgcc.a  -o test
$ arm-none-eabi-objcopy -O binary test test.bin
$ qemu-system-arm -M realview-pb-a8 -serial stdio -kernel test.bin
97: a
new alloc of 1 bytes at address 0x1D280
Heap end = 0x1E000
98: b
new alloc of 2 bytes at address 0x1D280
Heap end = 0x1E000
99: c
new alloc of 4 bytes at address 0x1D280
Heap end = 0x1E000
100: d
new alloc of 8 bytes at address 0x1D280
Heap end = 0x1E000
97: a
new alloc of 16 bytes at address 0x1D280
Heap end = 0x1E000
98: b
new alloc of 32 bytes at address 0x1D280
Heap end = 0x1E000
99: c
new alloc of 64 bytes at address 0x1D280
Heap end = 0x1E000
100: d
new alloc of 128 bytes at address 0x1D280
Heap end = 0x1E000
97: a
new alloc of 256 bytes at address 0x1D280
Heap end = 0x1E000
98: b
new alloc of 512 bytes at address 0x1D280
Heap end = 0x1E000
99: c
new alloc of 1024 bytes at address 0x1D280
Heap end = 0x1E000
100: d
new alloc of 2048 bytes at address 0x1D280
Heap end = 0x1E000
97: a
new alloc of 4096 bytes at address 0x1D280
Heap end = 0x20000
98: b
new alloc of 8192 bytes at address 0x1D280
Heap end = 0x20000
99: c
new alloc of 16384 bytes at address 0x1D280
Heap end = 0x25000
100: d
Out of memory!
Program halting.
```
Each keystroke the programs tries to allocate more and more memory until the heap is consumed.

It’s worth noting that the resulting binary’s size, that includes some relevant standard C functions, is about 52KB.


One big part that is missing from the standard C library is the file I/O functions, but in order to work it needs a filesystem implementation and some storage. One small improvement of my example could be the opening of the other serial ports using the open function and some fixed names such as UART0, UART1 and UART2 to associate a file descriptor with a hardware peripheral, and use it accordingly.

See also:
http://www.eetimes.com/discussion/other/4024637/Embedding-with-GNU-newlib
