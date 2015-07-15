# QEMU + KGDB + Ftrace

### cross comiler
```
http://releases.linaro.org/14.05/components/toolchain/binaries

arm-linux-gnueabi-linaro_4.9.1-2014.07
https://github.com/Christopher83/linaro_toolchains_2014

https://launchpad.net/linaro-toolchain-binaries/
https://launchpad.net/linaro-toolchain-binaries/trunk/2013.10/+download/gcc-linaro-arm-linux-gnueabihf-4.8-2013.10_linux.tar.bz2
```

###gdb-7.9 build  gdbserver and gdb

```
./configure --target=arm-linux --prefix=/usr/local/arm-gdb
# --target=arm-linux表示生成的gdb調試的目標是在arm核心Linux系統中運行的程序

make
sudo make install
```

- Toolchain 無法使用 android 的 arm 編譯器目前原因不清楚

```
https://launchpad.net/linaro-toolchain-binaries/trunk/2013.10/+download/gcc-linaro-arm-linux-gnueabihf-4.8-2013.10_linux.tar.bz2
```

- build.env

```
# -*- shell-script -*-
TOOLCHAIN=gcc-linaro-arm-linux-gnueabihf-4.8-2013.10_linux
DIR=$(pushd $(dirname $BASH_SOURCE) > /dev/null; pwd; popd > /dev/null)
echo $DIR
export PATH=${PATH}:${DIR}/${TOOLCHAIN}/bin
export CC=arm-linux-gnueabihf-gcc
```
### gdbserver , 要注意gdb和gdbserver版本一致

```
source build.env
./configure --target=arm-linux --host=arm-linux LDFLAGS="-static"
# 這裡的--host指定這個程序的目標平台。這一步中會檢查系統中是否有交叉編譯器的。
# We must add the above LDFLAGS to let gdb statically linked, otherwise it cannot
run on Android.

time make -j8 2>&1 | tee build.log
file ./gdbserver

./gdbserver: ELF 32-bit LSB  executable, ARM, EABI5 version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 3.1.1, BuildID[sha1]=4eec8a5a6893ed8ce65eea5d0741a55cc621236a, not stripped
```

# linaro qemu

```
git clone git://git.linaro.org/qemu/qemu-linaro.git

$ cd qemu-linaro
$ mkdir build
$ cd build
$ ../configure --prefix=/usr/local/qemu-linaro
$ make -j8
$ sudo make install
```

# ~/.bashrc

```
export PATH=/usr/local/qemu-linaro/bin:$PATH
```




# busybox

```
make -j8 ARCH=arm CROSS_COMPILE=arm-eabi- defconfig  // generation .config
```
### 打開 Busybox 選項
	CONFIG_STATIC=y
	CONFIG_INSTALL_NO_USR=y
	CONFIG_FEATURE_PREFER_APPLETS=y
	CONFIG_FEATURE_SH_STANDALONE=y

### build busybox
	make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
	make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- install  // 產生 _install 資料夾

# 建立root filesystem image
	// arm-linux-gnueabihf-gcc hello.c -o hello
	#include <stdio.h>

	int main(int argc, char *argv[])
	{
	    printf("Hello world\n");
	    return 0;
	}

### 查詢  hello 所需要的　share library
	arm-linux-gnueabihf-readelf -a  hello | ag '\.so'

	[Requesting program interpreter: /lib/ld-linux-armhf.so.3]
	 0x00000001 (NEEDED)                     Shared library: [libc.so.6]
	0001055c  00000216 R_ARM_JUMP_SLOT   00000000   __libc_start_main
	     1: 00000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.4 (2)
	     2: 00000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@GLIBC_2.4 (2)
	     4: 00000000     0 FUNC    GLOBAL DEFAULT  UND abort@GLIBC_2.4 (2)
	    84: 00008431     2 FUNC    GLOBAL DEFAULT   13 __libc_csu_fini
	    93: 00000000     0 FUNC    GLOBAL DEFAULT  UND puts@@GLIBC_2.4
	    94: 00000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@@GLIBC_
	    98: 000083f1    64 FUNC    GLOBAL DEFAULT   13 __libc_csu_init
	   107: 00000000     0 FUNC    GLOBAL DEFAULT  UND abort@@GLIBC_2.4
	  000:   0 (*local*)       2 (GLIBC_2.4)     2 (GLIBC_2.4)     0 (*local*)
	  004:   2 (GLIBC_2.4)
	  000000: Version: 1  File: libc.so.6  Cnt: 1
	  0x0010:   Name: GLIBC_2.4  Flags: none  Version: 2


-  需要　ld-linux-armhf.so.3　& libc.so.6
- 如果編譯成 static 應該就不需要 share libray

```
[Requesting program interpreter: /lib/ld-linux-armhf.so.3]
0x00000001 (NEEDED)                     Shared library: [libc.so.6]
000000: Version: 1  File: libc.so.6  Cnt: 1
```



### 開始建立 filesystem
	cd busybox-1.23.2/_install/
	mkdir -p proc sys dev etc etc/init.d lib
	cd gcc-linaro-arm-linux-gnueabihf-4.9-2014.05_linux
	find . -name  'libc.so.6'
	cp -a gcc-linaro-arm-linux-gnueabihf-4.9-2014.05_linux/arm-linux-gnueabihf/libc/* busybox-1.23.2/_install/lib


	cat << EOF > etc/fstab
	proc  /proc  proc  defaults  0  0
	sysfs  /sys  sysfs defaults  0  0
	tmpfs  /tmp  tmpfs defaults  0  0
	EOF


	cat << EOF > etc/inittab
	::sysinit:/etc/init.d/rcS
	::respawn:-/bin/sh
	tty2::askfirst:-/bin/sh
	::ctrlaltdel:/bin/umount -a -r
	EOF

	// etc/init.d/rcS
	#! /bin/sh

	MAC=08:90:90:59:62:21
	IP=192.168.100.2
	Mask=255.255.255.0
	Gateway=192.168.100.1

- chmod 755 etc/init.d/rcS
- 產生 rootfs, 可在temp 下新增修改檔案

```
find ./ | cpio -o -H newc | gzip > ../rootfs.img
```


# build kernel
- linux-3.18.16  , 目前4.1版本好像會有問題  , qemu 無法使用

```
make ARCH=arm  vexpress_defconfig
make ARCH=arm  menuconfig  or  make ARCH=arm  vexpress_defconfig menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j12
```

#安裝 tftp

```
sudo apt-get install tftp-hpa tftpd-hpa
/etc/default/tftpd-hpa
# /etc/default/tftpd-hpa
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/tftpboot"
TFTP_ADDRESS="0.0.0.0:69"
TFTP_OPTIONS="-l -c -s"
```

#建立 tftpboot

```
sudo mkdir /tftpboot
sudo chmod 777 /tftpboot
// restart service
sudo service tftpd-hpa restart
// test tftp
tftp host-ip  // ex tftp localhost
get or put
ex: tftp -g -r hello.c  192.168.100.1
```

# 啟動 tap :
```
sudo apt-get install uml-utilities   // 需要安裝
sudo ./nettap.sh  // 必須加上 sudo
```
- nettap.sh

```
#! /bin/bash
tunctl -u jason-yao -t tap0
ifconfig tap0 192.168.100.1 up
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -o eth1 -j MASQUERADE
iptables -I FORWARD 1 -i tap0 -j ACCEPT
iptables -I FORWARD 1 -o tap0 -m state --state RELATED,ESTABLISHED -j ACCEPT
```

# 啟動 QEMU
```
qemu-system-arm -M vexpress-a9 \
                -kernel zImage \
                -initrd rootfs.img \
                -serial stdio \
                -append "root=/dev/ram rdinit=/sbin/init console=ttyAMA0" \
                -k en-us \
                -net nic \
                -net tap,ifname=tap0,script=no


qemu-system-arm -M vexpress-a9 \
                -kernel zImage \
                -initrd rootfs.img \
                -serial stdio \
                -append "root=/dev/ram rdinit=/sbin/init console=ttyAMA0" \
                -k en-us \
                -net nic \
                -net tap,ifname=tap0,script=no \
                -gdb tcp::1234 -S


```

# 啟動 GDB

```
arm-linux-gnueabihf-gdb  ./vmlinux
target remote localhost:1234
```

# 使用Ftrace
```
Kernel hacking -> Tracers -> enable/disable ftrace tracepoints dynamically
Kernel hacking -> Tracers -> Kernel Function Tracer

mount -t debugfs none /sys/kernel/debug
cd /sys/kernel/debug/tracing
```


```
qemu-system-arm -m 1024M -sd armdisk.img \
                -M vexpress-a9 -cpu cortex-a9 \
                -kernel vmlinuz-3.2.0-4-vexpress -initrd initrd.gz \
                -append "root=/dev/ram"  -no-reboot
```


- ftrace_on.sh

```
#!/bin/bash

mount -t debugfs none /sys/kernel/debug
echo 16384 > /sys/kernel/debug/tracing/buffer_size_kb
echo nop > /sys/kernel/debug/tracing/current_tracer

echo 1 > /sys/kernel/debug/tracing/events/kmem/kmalloc/enable
echo 1 > /sys/kernel/debug/tracing/events/kmem/kmalloc_node/enable
echo 1 > /sys/kernel/debug/tracing/events/kmem/kfree/enable

echo 0 > /sys/kernel/debug/tracing/tracing_on
echo  > /sys/kernel/debug/tracing/trace
echo 1 > /sys/kernel/debug/tracing/tracing_on
```
