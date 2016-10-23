# How Other People Use QEMU


## Viller

```sh
qemu-system-arm \
-M vexpress-a15 \
-kernel buildroot/output/images/zImage \
-dtb buildroot/output/images/vexpress-v2p-ca15_a7.dtb \
-drive file=buildroot/output/images/rootfs.ext2,if=sd \
-smp 2 \
-s \
-serial stdio \
-append "root=/dev/mmcblk0 console=ttyAMA0,115200n8" \
-net nic,vlan=1 -net user,vlan=1,hostfwd=udp:127.0.0.1:6669-:69
```


﻿利用 buildroot 與 Qemu 建構簡易 Embedded Linux 環境
===
# Embedded Linux development

[Building Embedded Linux Full Tutorial for ARM](http://www.slideshare.net/sherif_mosa/building-embedded-linux-full-tutorial-for-arm)

## Components
* cross toolchain
	* libc: glibc / uclibc / musl
* bootloader
	* uboot / barebox 
* kernel
* rootfs
	* busybox
	* applications
	* development toolkits

## Some limitations
* Different sources download path
	* url
	* local
	* ... and so on, one for each package
* Different download tools
	* tar ball / zip
	* git/svn/mercurial
	* copy (for local)
* Some patches might required for embedded environment
	* non-glibc
	* busybox
	* rootfs path
	* and so on
* Package dependency

# builtroot
* (wiki) A set of Makefiles and patches that simplifies and automates the process of building a complete and bootable Linux environment for an embedded system
* menuconfig to select alternatives and versions of
	* board
	* bootloaders
	* compiler, binutils and libc
	* kernel version
	* supported packages
* How to support one package, e.g. foo
	* foo.mk (Makefile DSL). Commands for
		* download
		* configure
		* build
		* install
	* Config.in (menuconfig selection)
	![buildroot](./images/buildroot-diagram.png)
 * Example mk for customized package, e.g. bpftest.mk
   ~~~
    BPFTEST_VERSION = 1.0
    BPFTEST_SITE = $(TOPDIR)/../bpftest
    BPFTEST_SOURCE = "apps modules"
    BPFTEST_SITE_METHOD = local
    BPFTEST_LICENSE = GPLv3+
    BPFTEST_LICENSE_FILES = COPYING

    BPFTEST_MODULE_SUBDIRS = modules
    BPF_MODULE_MAKE_OPTS = KVERSION=$(LINUX_VERSION_PROBED) KERNEL_DIR=$(LINUX_DIR)

    define BPFTEST_EXTRACT_CMDS
    	cp -a $(TOPDIR)/$(BPFTEST_SOURCE) $(@D)
    endef

    define BPFTEST_BUILD_CMDS
	   $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)/apps all
    endef

    define BPFTEST_INSTALL_TARGET_CMDS
    	$(INSTALL) -D -m 0755 $(@D)/apps/hello $(TARGET_DIR)/usr/bin
    endef

    $(eval $(kernel-module))
    $(eval $(generic-package))
    ~~~
 
# Qemu
* My target: arm vexpress
	* Supported by vanilla kernel
	* Supported by Qemu

## Steps
* Build
~~~ sh
$ configure --prefix=/path/to/install --target-list=arm-softmmu
$ make
$ make install
~~~

* Run
	* https://wiki.linaro.org/PeterMaydell/QemuVersatileExpress
		* target: `-M vexpress-a15`
		(Feedback) Can try virtio (?)
		* images:
			* kernel: `-kernel /path/to/zImage`
			* dtb: `-dtb /path/to/dtb`
			* disk: `-drive ...`
		* serial output to tty: `-serial stdio`
		* virtual NAT and port forwarding:`-net nic,vlan=1 -net user,vlan=1,hostfwd=udp:127.0.0.1:6669-:69`
		(Feedback) Can try tap
		* boot args: `-append "root=/dev/mmcblk0 console=ttyAMA0,115200n8"`
		* debug:`-s` or `-gdb tcp::1234` / `-S`
		* CPU logging: `-d op -D /path/to/qemu.log`

    * example command
	  ~~~ sh
	  $ /path/to/install/bin/qemu-system-arm \
      -M vexpress-a15 \
      -kernel buildroot/output/images/zImage \
      -dtb buildroot/output/images/vexpress-v2p-ca15_a7.dtb \
      -drive file=buildroot/output/images/rootfs.ext2,if=sd \
      -smp 2 \
      -s \
      -serial stdio \
      -append "root=/dev/mmcblk0 console=ttyAMA0,115200n8" \
      -net nic,vlan=1 -net user,vlan=1,hostfwd=udp:127.0.0.1:6669-:69
      ~~~



---

## Wen

```sh
qemu-system-arm \
-M versatilepb \
-kernel /tmp/kernel/linux-stable/arch/arm/boot/zImage \-drive file=output/images/rootfs.ext2,if=scsi,format=raw \
-append "root=/dev/sda console=ttyAMA0,115200" \
-serial stdio \
-net nic,model=rtl8139 \
-net user
```


---


##Virt-2.7

```sh
qemu-system-arm \
-M virt-2.7 \
-S \
-m 1024 \
-cpu cortex-a15 \
-nographic \
-device virtio-9p-device,fsdev=host_fs,mount_tag=/dev/root \
-fsdev local,id=host_fs,security_model=none,path=$(pwd)/../sysroot \
-kernel ./arch/arm/boot/zImage \
-append 'root=/dev/root rootfstype=9p rootflags=trans=virtio rw \
-netdev user,id=unet -device virtio-net-device,netdev=unet
```
