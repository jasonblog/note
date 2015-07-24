# Compiling Linux kernel for QEMU ARM emulator

Last time I experimented on compiling bare-metal ARM programs and U-Boot; now I want to compile a Linux kernel for an ARM architecture from scratch. I don’t have a physical ARM device handy, so I’m using QEMU instead, as I’ve already done before.

Both the mainline kernel and QEMU support the VersatilePB platform, so that’s the target I chose for my tests. The toolchain I’ll be using is the CodeSourcery ARM EABI toolchain. [edit] From version 2010q1 of the toolchain, the manual explicitly says that the compiler is not intended for Linux kernel development; it is anyway possible to use the GNU/Linux toolchain for the same scope.

The vanilla kernel can be downloaded from kernel.org; I took the latest at the moment (version 2.6.33) and extracted it in a folder. From that folder I ran:
```sh
make ARCH=arm versatile_defconfig
```

This command sets a predefined configuration, used in compilation, that is capable of building a kernel targeted to run inside the VersatilePB board. I wanted to tweak it a little bit, so I ran:
```sh
make ARCH=arm menuconfig
```

I removed module support (for simplicity) and enabled EABI support as a binary format (allowing also old ABI). This is necessary to run software compiled with the CodeSourcery toolchain. I exited the menu and saved the configuration, then I ran:

```sh
make ARCH=arm CROSS_COMPILE=arm-none-eabi- all
```

If using the GNU/Linux toolchain, the command that must be used is, instead:
```
make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- all
```

This will start the building of the kernel using the correct ARM compiler; the build will create, among other binaries, a compressed kernel in a file called zImage located in “arch/arm/boot“. This image can be run in QEMU using the following command (assuming that you are in the “arch/arm/boot” directory):
```
qemu-system-arm -M versatilepb -m 128M -kernel zImage
```

QEMU will execute the Linux image: the kernel will display many boot messages and then it will complain that it didn’t find a root filesystem. Let’s then create the simplest filesystem we can do: it consists of a single “Hello World” executable, that can be built using the CodeSourcery GNU/Linux toolchain.
```c
#include <stdio.h>

void main() {
  printf("Hello World!\n");
  while(1);
}
```

Note: an infinite loop is introduced because when Linux executes the first program in the root filesystem, it expects that the program does not exit.
Having the GNU/Linux ARM toolchain installed (be aware that it is different from the bare EABI toolchain) I ran:
```
arm-none-linux-gnueabi-gcc -static    test.c   -o test
```

This creates an executable ELF program for ARM, statically linked (all the libraries that it needs are linked together in a single binary). We can now create a simple filesystem using the cpio tool as follows:
```
echo test | cpio -o --format=newc > rootfs
```

The cpio tool takes a list of files and outputs an archive; the newc format is the format of the initramfs filesystem, that the Linux kernel recognizes. The rootfs file in our case is a binary image of a filesystem containing a single file, that is the test ELF program. QEMU can pass the filesystem binary image to the kernel using the initrd parameter; the kernel must also know that the root filesystem will be located in RAM (because that’s where QEMU writes the initrd binary) and that the program to launch is our test executable, so the command line becomes:
```
qemu-system-arm -M versatilepb -m 128M -kernel zImage -initrd rootfs -append
"root=/dev/ram rdinit=/test"
```
The QEMU window will show the boot messages we saw before, but at the end of the execution a “Hello World!” will be displayed. The next step would be to create a working filesystem with a command shell and at least basic functionality.
