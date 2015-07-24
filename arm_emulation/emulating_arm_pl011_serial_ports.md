# Emulating ARM PL011 serial ports


QEMU is able to emulate ARM systems with multiple serial ports, for example the Versatile PB. It has the ability to direct them to many types of host resources such as standard I/O, a pseudo-terminal or a telnet port. I started from my hello world example for bare-metal ARM programs to control three different serial ports. In the Versatile PB manual there’s a section, called Memory Map, that includes the absolute addresses of the mapped peripherals. For example, the UART 0, 1 and 2 interfaces are placed at addresses **0x101F1000**, **0x101F2000** and **0x101F3000** respectively. Inside the manual, the programmer’s model for the UART peripherals indicates the ARM PrimeCell UART (PL011) Technical Reference Manual as reference. In the PL011 manual we can find a detailed description of the UART memory mapped registers. From that decription I implemented a C struct that renders easy to use the serial ports. The complete program is the following:


```c
#include <stdint.h>

typedef volatile struct {
 uint32_t DR;
 uint32_t RSR_ECR;
 uint8_t reserved1[0x10];
 const uint32_t FR;
 uint8_t reserved2[0x4];
 uint32_t LPR;
 uint32_t IBRD;
 uint32_t FBRD;
 uint32_t LCR_H;
 uint32_t CR;
 uint32_t IFLS;
 uint32_t IMSC;
 const uint32_t RIS;
 const uint32_t MIS;
 uint32_t ICR;
 uint32_t DMACR;
} pl011_T;

enum {
 RXFE = 0x10,
 TXFF = 0x20,
};

pl011_T * const UART0 = (pl011_T *)0x101f1000;
pl011_T * const UART1 = (pl011_T *)0x101f2000;
pl011_T * const UART2 = (pl011_T *)0x101f3000;

static inline char upperchar(char c) {
 if((c >= 'a') && (c <= 'z')) {
  return c - 'a' + 'A';
 } else {
  return c;
 }
}

static void uart_echo(pl011_T *uart) {
 if ((uart->FR & RXFE) == 0) {
  while(uart->FR & TXFF);
  uart->DR = upperchar(uart->DR);
 }
}

void c_entry() {
 for(;;) {
  uart_echo(UART0);
  uart_echo(UART1);
  uart_echo(UART2);
 }
}
```

The pl011_T structure implements the memory map of the PL011 serial port. I used the standard sized types that are defined inside “stdint.h” because the important part of the definition is to put the register at the right offset from the start of the struct. The purpose of the reserved byte arrays is to put the subsequent register at the right offset as specified in the PL011 manual. The definition is marked as volatile because I don’t want that the compiler assumes something about the values of the registers, because they can be changed by the hardware. Some registers are marked const because they are read-only, but the volatile specifier indicates that they can actually change outside the execution of the program. At last, the three pointers UART0, UART1 and UART2 are pointing to the address where the peripheral is mapped in the Versatile platform. The code itself does nothing more than echoing the received character, and if the character is a letter it converts it to uppercase. The echo is done polling the RXFE flag of the Flag Register (FR) until the receive FIFO is not empty. Then, we wait until the transmit FIFO is not full (TXFF flag) and use the Data Register to read and write the byte.

To try the program I used the “startup.s” and “test.ld” files that I prepared in my previous post. I’m assuming the main C code is a file called “test.c“. Then I compiled the program using the CodeSourcery bare metal ARM toolchain, and I emulated it with qemu-system-arm emulator:

```sh
$ arm-none-eabi-gcc -c -mcpu=arm926ej-s test.c -o test.o
$ arm-none-eabi-as -mcpu=arm926ej-s startup.s -o startup.o
$ arm-none-eabi-ld -T test.ld test.o startup.o -o test.elf
$ arm-none-eabi-objcopy -O binary test.elf test.bin
$ qemu-system-arm -M versatilepb -m 128M -kernel test.bin -serial stdio -serial telnet:localhost:1235,server -serial telnet:localhost:1236,server
QEMU waiting for connection on: telnet:127.0.0.1:1235,server
QEMU waiting for connection on: telnet:127.0.0.1:1236,server
```

The QEMU serial options open two telnet ports, and the execution stops until there’s a telnet client connected to each of the ports. So, in another terminal, the commands:
```
$ telnet 127.0.0.1 1235
```
and:
```
$ telnet 127.0.0.1 1236
```
open the connections and the program starts. All three UARTs are active and respond to characters sent by the host.
