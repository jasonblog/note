# U-boot for ARM on QEMU


Das U-Boot, the universal bootloader, is a crucial piece of software that runs on embedded platforms: its role is to put in place and boot the linux kernel from a hard drive, a flash memory, network or serial line. Here I explain how to try U-Boot on QEMU, and in particular on the emulation of the VersatilePB platform.

First, install the necessary tools:

- qemu-system-arm: run “apt-get install qemu” on Debian, or “sudo apt-get install qemu-kvm-extras” on Ubuntu.
- mkimage: install the uboot-mkimage package from the Debian or Ubuntu repository.
- CodeSourcery ARM EABI toolchain toolchain: download from their website and install.

Grab the U-Boot source code from the U-Boot FTP site and decompress it. Go inside the created directory and run:
```
make versatilepb_config ARCH=arm CROSS_COMPILE=arm-none-eabi-
```
This command configures U-Boot to be compiled for the VersatilePB board. Then compile and build with:
```
make all ARCH=arm CROSS_COMPILE=arm-none-eabi-
```
The compilation will create a u-boot.bin binary image. To simulate, run:
```
qemu-system-arm -M versatilepb -m 128M -nographic -kernel u-boot.bin
```
The U-Boot prompt should appear:

U-Boot 1.1.6 (Mar 3 2010 - 21:46:06)

DRAM: 0 kB
Flash: 0 kB
*** Warning – bad CRC, using default environment

In: serial
Out: serial
Err: serial
Versatile #

You can have a list of commands by entering help, and then try out various commands (hit “Ctrl-a” and then “x” to exit QEMU). The bootm command in particular is used to boot a program that is loaded in memory as a special U-Boot image, that can be created with the tool mkimage. This program is usually an operating system kernel, but instead of running a full-blown Linux kernel, we can instead run the simple “Hello world” program described in a previous blog post. To do so, we create a single binary that contains both the U-Boot program and our “Hello world” program together. The initial address of the “Hello world” program must be changed with respect to the instructions present in the last blog post, because at 0x10000 (our last initial address) QEMU places the beginning of the U-Boot binary. Since the U-Boot binary is about 100KB, we can place our binary at 0x100000 (that is 1MB) to be safe.

Create test.c, startup.s and test.ld as last time, but change line 4 of test.ld from “. = 0x10000” to “. = 0x100000“. Build the binary with:

```sh
arm-none-eabi-gcc -c -mcpu=arm926ej-s test.c -o test.o
arm-none-eabi-ld -T test.ld -Map=test.map test.o startup.o -o test.elf
arm-none-eabi-objcopy -O binary test.elf test.bin
```
Now create the U-Boot image test.uimg with:
```
mkimage -A arm -C none -O linux -T kernel -d test.bin -a 0x00100000 -e 0x00100000 test.uimg
```

With these options we affirm that the image is for ARM architecture, is not compressed, is meant to be loaded at address 0x100000 and the entry point is at the same address. I use “linux” as operating system and “kernel” as image type because in this way U-Boot cleans the environment before passing the control to our image: this means disabling interrupts, caches and MMU.
`
Now we can create a single binary simply with:
```
cat u-boot.bin test.uimg > flash.bin
```

This binary can be run instead of the U-Boot binary with:
```
qemu-system-arm -M versatilepb -m 128M -nographic -kernel flash.bin
```

at the U-Boot prompt, we can check that the image is inside the memory: it should be exactly after the u-boot.bin code. To calculate the address, we must take the size of u-boot.bin and sum the initial address where flash.bin is mapped. From the bash prompt, the following script prints the command to be written inside U-Boot:
```
printf "bootm 0x%X\n" $(expr $(stat -c%s u-boot.bin) + 65536)
```

in my case it prints “bootm 0x21C68“. In fact, if I run “iminfo 0x21C68” inside U-Boot prompt to check the memory content I get:
```
## Checking Image at 00021c68 ...
Image Name:
Image Type:   ARM U-Boot Standalone Program (uncompressed)
Data Size:    376 Bytes =  0.4 kB
Load Address: 00100000
Entry Point:  00100000
Verifying Checksum ... OK
```

I can then confidently run “bootm 0x21C68” (you should substitute your address in this command). This command copies the content of the image, that is actually test.bin, into the address 0x100000 as specified in the U-Boot image, and then jumps to the entry point. The emulator should print “Hello world!” as last time, and then run indefinitely (hit “Ctrl-a” and then “x” to exit). This is basically the same procedure that is used to boot a Linux kernel, with some modifications: for example, the Linux kernel accepts some parameters that must be received from U-Boot somehow. I plan to write a post about that in the future.

In a real world example, the binary file we created could be placed inside the parallel Flash memory of an embedded platform, and the boot process can be controlled from the serial port.
