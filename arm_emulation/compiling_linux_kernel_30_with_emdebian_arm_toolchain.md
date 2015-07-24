# Compiling Linux kernel 3.0 with Emdebian ARM toolchain
The Emdebian project works to bring Debian on embedded platforms, with repositories of custom distributions and toolchains to cross-compile software. I wanted to try their ARM toolchains, and coincidentally the Linux kernel 3.0 has been released in these days, so I tried to cross-compile it and emulate it on QEMU. These tests have been done on my Debian “wheezy” desktop.
In order to install software from Emdebian repository, their authentication key must be installed first. I ran as root:
```
# aptitude install emdebian-archive-keyring
```
To use the Emdebian repository with Debian “apt” package management,  I added to “/etc/apt/sources.list” the following lines:
```
deb http://www.emdebian.org/debian/ testing main
deb-src http://www.emdebian.org/debian/ testing main
```
Then I updated the repository information and installed all that was needed for my tests:

```
# aptitude update
# aptitude install gcc-4.4-arm-linux-gnueabi libncurses5-dev qemu-system
```

I then downloaded the Linux kernel and extracted it. To keep things simple, I chose to compile the kernel for an architecture that has support both in the kernel and in QEMU, which is the ARM Versatile Platform Baseboard.
These are the commands that I ran (as regular user, not as root) to donwload and compile the kernel:
```
$ wget http://www.kernel.org/pub/linux/kernel/v3.0/testing/linux-3.0.tar.bz2
$ tar xjf linux-3.0.tar.bz2
$ cd linux-3.0
$ export ARCH=arm
$ export CROSS_COMPILE=arm-linux-gnueabi-
$ make versatile_defconfig
```
at this point I changed the configuration to enable EABI support, which is the new “Application Binary Interface” that is used by all modern ARM compilers.

$ make menuconfig
(Kernel features -> enable EABI)
Then I compiled the kernel:

$ make
...
Kernel: arch/arm/boot/zImage is ready
...
To test it, I used “qemu-system-arm”, which is a full system emulator for many platforms. To emulate the Linux boot on a VersatilePB machine, I ran the command:
```
$ qemu-system-arm -M versatilepb -kernel arch/arm/boot/zImage
```
The window that opens emulates the machine’s monitor. At the end of the boot phase, since there’s no root filesystem, an error message is displayed.
```
...
Kernel panic - not syncing
...
```
