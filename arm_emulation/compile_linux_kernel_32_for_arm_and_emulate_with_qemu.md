# Compile Linux kernel 3.2 for ARM and emulate with QEMU


This tutorial is an updated version of this old post, with newer software and less obsolete emulated hardware.

Every year the market produces tons of new products that run on ARM cores, and are able to run operating systems such as Linux. While most of these products are quite expensive (think about smartphones, development kits or evaluation boards) it’s possible to explore the world of Linux on ARM freely, thanks to software emulators like QEMU.


I am going to show how to compile the kernel and emulate the boot. To simplify things, the boot will not include a complete filesystem but uses a minimal ramdisk to show the kernel executing just one program.


I chose to emulate the Versatile Express product because it’s well supported both by the mainline Linux kernel and by mainline QEMU. Moreover, this hardware platform runs on the Cortex-A9 core, which is an ARM CPU that is included in many smartphones today.

#Requirements
In order to follow the same steps that I did, you need some tools.

First of all, anything I do is performed on a Linux machine, specifically a Debian
testing distribution, in a bash shell.


To manage the kernel compilation, GNU make should be installed (it is usually in
build-essential package)


To compile the kernel for ARM architecture, a cross-compiler must be installed. The difference between a traditional compiler and a cross-compiler is that the traditional compiler runs on an architecture (for example x86_64) and produces binaries for the same architecture. A cross-compiler produces binaries for a different architecture (in our case ARMv7). Depending on your distribution and what works for your setup, you can choose
from different toolchains:


- Emdebian; here some instructions on how to install,
- Linaro; if you run a newish version of Ubuntu you can install it directly with “sudo apt-get install gcc-arm-linux-gnueabi“,
- Fedora ARM cross-toolchain,
- Sourcery Codebench (was CodeSourcery); available for free only under registration,
- other toolchain suggestions by eLinux.

Cross-compilers offer a set of programs, mainly GCC and binutils, that start with a prefix indicating the architecture, the operating system of the libraries and the binary interface of the compiled programs. In my case I use Emdebian toochain, which has the “**arm-linux-gnueabi-**” prefix.


Finally the emulator that I use is QEMU, in particular the program to emulate ARM hardware is “qemu-system-arm“. You must install the correct package depending on your distribution; sometimes distributions split the QEMU programs into different packages, for example Ubuntu packs it into the “qemu-extras” package.
The short story
Create a clean directory, then create a file called “init.c“, which contains the following simple C code:
```c
#include <stdio.h>

void main() {
  printf("Hello World!\n");
  while(1);
}
```
Then in the same directory execute the following commands in order:
```sh
wget http://www.kernel.org/pub/linux/kernel/v3.0/linux-3.2.tar.bz2
tar xjf linux-3.2.tar.bz2
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
cd linux-3.2
make vexpress_defconfig
make all
cd ..
arm-linux-gnueabi-gcc -static init.c -o init
echo init|cpio -o --format=newc > initramfs
qemu-system-arm -M vexpress-a9 -kernel linux-3.2/arch/arm/boot/zImage -initrd initramfs -serial stdio -append "console=tty1"
```

The kernel compilation (the “make all” command) could take some minutes or hours depending on your host machine power.


The last command opens a QEMU window, that shows a black background and many boot messages, and towards the end the “Hello World!” string is displayed.


#The long story
The steps are:
- Get Linux kernel source code
- Prepare for compilation
- Configure and compile
- Prepare and create ramdisk
- Emulate kernel boot and ramdisk execution


#Get Linux kernel source code
The official site for mainline Linux kernel is at www.kernel.org. The kernel version that I will use is the 3.2, be aware that if you want to use a different version you may have different results, even though most of the functionality used here is simple enough that it should not change between versions.

Download linux-3.2.tar.bz2 from the FTP site, or simply run from the command line:
```sh
wget http://www.kernel.org/pub/linux/kernel/v3.0/linux-3.2.tar.bz2
```
Then extract the kernel source. One way to do it is by running in the same directory:
```
tar xjf linux-3.2.tar.bz2
```
This will create a new subdirectory called linux-3.2 containing the full source of the Linux kernel.

#Prepare for compilation
We are going to compile for ARM architecture by using a cross-toolchain, so we need to tell it somehow to the Linux build system. There are two environmental variable for this: ARCH and CROSS_COMPILE. The valid values for ARCH are basically the subdirectories of the “arch” directory. For CROSS_COMPILE we need to provide the prefix of the toolchain, which is the name of the compiler program minus the gcc at the end. For example if we are using arm-linux-gnueabi-gcc, we need to set CROSS_COMPILE to arm-linux-gnueabi-. On the terminal, run:
```sh
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
```
# Configure and compile
We want to compile for the Versatile Express, and for this we can use the prepared configuration file in “arch/arm/configs/vexpress_defconfig” by running:
```
make vexpress_defconfig
```
This will configure the compilation for the desired hardware, by creating a file called “.config” that contains all the relevant options.
 Then, to compile the kernel image, the command is simply:
 ```
make all
```
At the end of compilation it creates a file in “linux-3.2/arch/arm/boot/zImage” that is a compressed kernel image that auto-extracts in RAM. To speed-up compilation on multi-core hosts I suggest trying the parallel compilation, by launching
```
make -j 2 all
```
which, instead of compiling sequentially, will use a parallelism of 2 to create the objects and the final images.

#Prepare and create ramdisk
In order to make the kernel do something, we can create a simple “Hello-World” user-space program. We can use a ramdisk as the first filesystem that Linux uses as root, using the “initramfs” scheme. More information about ramdisks can be found in the kernel source tree, in the file “Documentation/early-userspace/README“. The first program that Linux tries to execute is “/init“, so we can create an executable with that name. The source code is simply:
```c
#include <stdio.h>

void main() {
  printf("Hello World!\n");
  while(1);
}
```

And it will be compiled by our cross-toolchain of choice. In order to make this program work alone, we need to compile it as a static executable, which links in the program also the libraries that it needs. In this way we can have a filesystem with only one executable without worrying about shared libraries. The command for compilation is:
```sh
arm-linux-gnueabi-gcc -static init.c -o init
```
This will create an executable for ARM called init. The format of the file is something like the following:
```
$ file init
init: ELF 32-bit LSB executable, ARM, version 1 (SYSV), statically linked, for GNU/Linux
2.6.18, BuildID[sha1]=0xf78b4955773f3c5dfecdb11c62dae094a99ba8f9, not stripped
```
We can now create our ramdisk with the cpio utility, by adding just the init file to a new archive:
```
echo init|cpio -o --format=newc > initramfs
```
Be aware that the command must be run in the same directory as the init file. The initramfs file is our ramdisk. You can check its content with:
```
$ cpio -t < initramfs
init
1090 blocks
```
#Emulate kernel boot and ramdisk execution
We have all that is needed to execute the Linux boot with QEMU, which is able to emulate the Versatile Express platform using the “-M vexpress A9” option. The zImage kernel and initramfs image are loaded by QEMU in the emulated RAM with the “-kernel” and “-initrd” options, pointing to the corresponding files.

We also want to display the boot messages which are sent on the console. To show them in the graphic window, we need to pass the “console=tty1” kernel parameter.  The kernel parameters will be passed to Linux by QEMU using the “-append” option.

The complete command is then:
```
qemu-system-arm -M vexpress-a9 -kernel linux-3.2/arch/arm/boot/zImage -initrd initramfs -append "console=tty1"
```

The command will launch QEMU and open a black window, with a Tux logo to show the graphic capabilities. The boot messages will be displayed in the graphic window, and at the end of the messages our “Hello World!” string will be printed.

Otherwise, QEMU can redirect the serial port of the emulated system on the host terminal, using the “-serial stdio” option, and Linux can display its messages on the first serial port by passing “console=ttyAMA0” as a kernel parameter. The command becomes:

```
qemu-system-arm -M vexpress-a9 -kernel linux-3.2/arch/arm/boot/zImage -initrd initramfs -serial stdio -append "console=ttyAMA0"
```
The command will launch QEMU and open the black graphical window, and the boot messages will be displayed in the host terminal instead of the black window. Note that “ttyAMA0” is a serial port name that is dependent on the hardware that is emulated, and may not be the same for all systems.
In my tests I used QEMU version 1.0, and the result may vary if you are using a different version.
