# Busybox for ARM on QEMU

Busybox is a solution for embedded Linux designs that need a compact filesystem: the trick is compiling and linking many system utilities into a single binary that behaves differently based on the name it was used to execute it. A working Linux root filesystem then consists in a small directory tree (/bin, /sbin, /usr/bin, …), a single executable binary in /bin/busybox, and many symbolic links to the Busybox binary (/bin/ls, /bin/sh, /sbin/ifconfig, …), and using a typical configuration it can be as small as 2MB (1MB if compressed).

Last time I compiled a Linux kernel and a minimal root filesystem containing just a “Hello World” program. Now we can build a reasonably working root filesystem and test it using QEMU. Note that, in order to follow this example, you need:

- The CodeSourcery Linux toolchain
- A Linux kernel image compiled for the Versatile platform, with EABI support (here is how I created mine)
- QEMU ARM emulator (the package to install is qemu on Debian or qemu-kvm-extras on Ubuntu)
- Developer’s libraries for ncurses (the package to install is libncurses5-dev) to compile the menu configuration

I downloaded version 1.16.0 of the Busybox source and extracted it. Inside the extracted directory I ran:

```
$ make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- defconfig
```
A default configuration file is created. To change it to our needs:
```
$ make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- menuconfig
```
I checked the option to compile Busybox as a static executable, so that we don’t have to copy the dynamic libraries inside the root filesystem. The setting can be found in “Busybox Settings --> Build Options“. Then, the following command builds Busybox and creates a directory called _install containing the root filesystem tree:

```
$ make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- install
```
We can now create a root filesystem image using the cpio tool, and in order to compact the filesystem even more, we can run gzip on it to create a compressed image:
```
$ cd _install
$ find . | cpio -o --format=newc > ../rootfs.img
$ cd ..
$ gzip -c rootfs.img > rootfs.img.gz
```
In my case, the compressed image size is about 1MB. To test Busybox, we can emulate an ARM platform with QEMU using the following command, assuming we copy the Linux kernel zImage in the current directory:
```
$ qemu-system-arm -M versatilepb -m 128M -kernel zImage -initrd rootfs.img.gz -append
"root=/dev/ram rdinit=/bin/sh"
```

The Linux kernel will boot, and the shell /bin/sh will be executed as specified by the boot parameter rdinit, showing the common “#” prompt. The shell can be used normally, for example you can run ls to find the same directory structure of the Busybox _install directory, but using commands like ps and mount we can see that not everything is in place: both programs complain abount the /proc directory. We can create and populate the /proc directory running these commands inside the QEMU emulated system prompt:

```
# mkdir /proc
# mount -t proc none /proc
```

After that, the ps and mount programs work fine. We can also note that the /dev directory is almost empty (the only exception being the console device). To populate it we need to mount also the /sys directory, so that we can use the mdev tool; run inside QEMU:
```
# mkdir /sys
# mount -t sysfs none /sys
# mdev -s
```
The /sys and /dev directory are now populated. To execute these steps every time, we can use /sbin/init functionality: this program is usually the first run by the Linux kernel, and its default behavior is to execute the initialization file with path /etc/init.d/rcS. In the host computer this time, in the folder where we compiled Busybox, we create the missing directories:
```
$ cd _install
$ mkdir proc sys dev etc etc/init.d
$ cd ..
```
Now we create a new _install/etc/init.d/rcS with the following content:
```
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
/sbin/mdev -s
```
Then set it as an executable file:
```
$ chmod +x _install/etc/init.d/rcS
```
We can then recreate an updated root filesystem compressed image as before, and run:
```
$ qemu-system-arm -M versatilepb -m 128M -kernel zImage -initrd rootfs.img.gz -append
"root=/dev/ram rdinit=/sbin/init"
```

The Linux kernel will boot again, but this time it is asking to press Enter, and when we do the usual console prompt will appear. The system directories are fully populated, meaning that the script rcS has been executed; other than that, /sbin/init also spawns
shell terminals using getty.

In our example the Busybox filesystem is accessed using a ram disk, but it can also be read from a physical disk storage or loaded from the network using NFS protocol. Maybe another time I will describe in details how to run Busybox with NFS inside QEMU.
