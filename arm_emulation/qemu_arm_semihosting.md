# QEMU ARM semihosting

QEMU is able to emulate ARM architectures with many details. One thing that the qemu-system-arm program implements is the Angel interface, that enables semihosting. Semihosting involves two machines: a target (the embedded device), and a host (such as a PC) that is connected to the target through a debug interface. An embedded program running in semihosted mode is executing on the target architecture, but is able to see some resources of the host machine.

I created a simple bare metal program to test the semihosting functionality. The C code of the program is the following:

```c
#include <stdio.h>

void main()
{
    FILE* fp;
    char line[100];
    printf("Hello World!\n");
    fp = fopen("log.txt", "w");

    if (fp == NULL) {
        return;
    }

    while (fgets(line, 100, stdin) != NULL) {
        fprintf(fp, "%s", line);

        if (line[0] == 'q') {
            break;
        }
    }

    close(fp);
}
```
The purpose is to try to access the standard output, standard input and filesystem of the host. To compile this example on my Linux machine I used the toolchain Sourcery G++ Lite 2010q1-188, that comes with a series of libraries and linker scripts that allow to create programs running in a hosted environment. In my case (depending on the installation path) the directory “~/CodeSourcery/Sourcery_G++_Lite/arm-none-eabi/lib/” contains the linker script “generic-hosted.ld” that is almost OK to use: it needs just a little change in the memory description because of QEMU behavior.

I copied “generic-hosted.ld” into a local file that I called “qemu-hosted.ld” and then I opened to modify it. The problem is that QEMU loads the program binary into address 0x10000, but the linker script assumes that the program is executed at address 0x0. I decided to change the memory section of the script to tell the linker that the memory starts at 0x10000; in practice I changed the following lines:
```
MEMORY
{
    ram (rwx) : ORIGIN = 0x0, LENGTH = 128M
}
```
…into these:
```
MEMORY
{
    ram (rwx) : ORIGIN = 0x10000, LENGTH = 127M
}
```

To compile the program and run it inside QEMU the commands are (assuming the C source file has been called “main.c“):
```
$ arm-none-eabi-gcc   -T qemu-hosted.ld main.c -o main.elf
$ arm-none-eabi-objcopy -O binary main.elf main.bin
$ qemu-system-arm -semihosting -kernel main.bin
```
The terminal will display the “Hello World!” line and then accept user input. The input is written inside the file “log.txt” and when the standard input is closed (using CTRL-D) or the line starts with the character ‘q‘ the program ends. To start QEMU without the graphic window, use the following command:
```
$ qemu-system-arm -nographic -monitor null -serial null -semihosting -kernel main.bin
```
The “-monitor null -serial null” options are necessary to avoid conflicts on the standard streams.
What happens underneath this emulation? To discover it, it’s interesting to watch the assembly code that is run. The following command disassembles the program into the file main.code:
```
$ arm-none-eabi-objdump -d main.elf > main.code
```

Examining the code, it can be traced that the main function calls fopen, which eventually calls a low-level function called _open, that contains a SuperVisor Call (SVC) instruction.
```
00013a9c <_open>:
 [...]
 13b18:    e3a00001     mov    r0, #1
 13b1c:    ef123456     svc    0x00123456
 [...]
```

The SVC instruction, when using 0x123456 as parameter, tells QEMU to use its implementation of the Angel interface. The r0 register contains the code of the function to emulate, according to this table. Other registers can be used as other parameters to pass, like a function call.

The semihosting functionality is useful in prototyping, especially to emulate some parts of the systems to speed up development; but the possibilities are various. For example:

- when data coming from a sensor is difficult to control, it can be emulated by getting data from a file on the host machine instead.
- the program can dump intermediate data to file, without problems of non-volatile memory space.
- the embedded device can get/send data with the Internet through the host PC.
