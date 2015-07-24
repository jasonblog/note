# Simulating AT91 with Skyeye

Atmel AT91 is a series of common microcontrollers that contain ARM cores. The AT91SAM7 series in particular runs the ubiquitous ARM7TDMI. I found out that there is an open source program that emulates these microcontrollers (among many others): it is Skyeye.

Skyeye is a project that emulates big and small microcontrollers, and for this reason it complements QEMU, which is the current state of the art of open source emulators for cores devoted to applications. The old version 1.2.5 can be installed from Debian and Ubuntu repositories (package name: skyeye), while on sourceforge the latest version (currently 1.3.1) can be downloaded from source and then compiled. I noted that version 1.3.1 can only be built and installed without overriding the default prefix (that is “/opt/skyeye“), in order for it to work properly.

When simulating, Skyeye wants two files as input: an ELF executable and a configuration file. A simple executable for ARM7TDMI can be created with the EABI CodeSourcery toolchain as I explain in a previous post. Almost everything is identical except for the CPU type (the compiler option should be “-mcpu=arm7tdmi“) and the UART address (0xfffd001c). Once rewritten the “test.c“, “startup.s” and “test.ld” files, the commands to create the ELF executable are:
```
$ arm-none-eabi-as -mcpu=arm7tdmi -g startup.s -o startup.o
$ arm-none-eabi-gcc -c -mcpu=arm7tdmi  -g test.c -o test.o
$ arm-none-eabi-ld -T test.ld test.o startup.o -o test.elf
```

Then the configuration file (I called it “skyeye.conf“) must be:

```
arch:arm
cpu: arm7tdmi
mach: at91
mem_bank: map=M, type=RW, addr=0x00000000, size=0x00400000
mem_bank: map=I, type=RW, addr=0xf0000000, size=0x10000000
uart:mod=term
```
The “uart:mod” line is for printing what’s sent to the serial port to the terminal. To launch the emulator:
```
$ skyeye -e test.elf -c skyeye.conf
```
To end the emulation, press CTRL-C.
