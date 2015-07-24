# ARM emulation

This is a series of examples about emulating ARM systems, mainly with QEMU, and building programs that run on the emulated platforms. The advantages of using an emulated platform for learning is that it’s possible to control and debug many aspects of the system, and also that it’s safer to experiment on a virtual machine without the risk of bricking or breaking a physical board.

The following posts are about running ARM Linux in QEMU, building the kernel, the filesystem and the boot loader:

- Compiling Linux kernel for QEMU ARM emulator
- Compiling Linux kernel 3.0 with Emdebian ARM toolchain
- Compile Linux kernel 3.2 for ARM and emulate with QEMU
- Busybox for ARM on QEMU
- Linux NFS Root under QEMU ARM emulator
- Trying Debian for ARM on QEMU
- Debugging ARM programs inside QEMU
- U-boot for ARM on QEMU
- Booting Linux with U-Boot on QEMU ARM

The following posts are about bare metal programming on emulated ARM systems:

- Simplest bare metal program for ARM
- Hello world for bare metal ARM using QEMU
- Emulating ARM PL011 serial ports
- ARM926 interrupts in QEMU
- Using Ubuntu ARM cross-compiler for bare metal programming
- Using Newlib in ARM bare metal programs
- Using CodeSourcery bare metal toolchain for Cortex-M3
- QEMU ARM semihosting
- Simulating AT91 with Skyeye


Some source code about the experiments is present in [balau/arm-sandbox](https://github.com/balau/arm-sandbox) repository on GitHub.


[Some source code about the experiments](./file/arm-sandbox.tar.bz2)

Reference https://balau82.wordpress.com/arm-emulation/
