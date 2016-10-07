# raspberrypi2_kernel_debug


# QEMU를 이용한 라즈베리파이2 커널 디버깅
## QEMU 컴파일
* 참고 URL [Emulate Rapberry Pi 2 in QEMU](http://blog.3mdeb.com/2015/12/30/emulate-rapberry-pi-2-in-qemu/)

* QEMU 다운로드
```sh
$ mkdir -p ~/git/pi2
$ cd ~/git/pi2

$ git clone https://github.com/0xabu/qemu.git -b raspi
$ cd qemu
$ git submodule update --init dtc
```

* QEMU 커널 BASE 주소 수정
 * Qemu 소스 hw/arm/boot.c 파일 수정.
```c
#define KERNEL_LOAD_ADDR 0x00010000
```
이것을 아래처럼 수정.
```c
#define KERNEL_LOAD_ADDR 0x00008000
```

* compile 및 설치
```sh
$ cd ~/git/pi2/qemu

$ ./configure --target-list=arm-softmmu
$ make -j$(nproc)
$ sudo make install
```

## 라즈베리파이2용 커널 컴파일
* 참고 URL [Raspberry Pi Kernel Building](https://www.raspberrypi.org/documentation/linux/kernel/building.md)
* 라즈베리파이2 이미지 다운로드 [Raspberry PI 2 Image download](https://downloads.raspberrypi.org/raspbian_latest)
* 압축을 푼뒤 raspbian-jessie.img 파일명으로 변경한다

* 라즈베리파이2 컴파일러 및 툴체인 설치
```sh
$ mkdir ~/git/pi2
$ cd ~/git/pi2
$ git clone https://github.com/raspberrypi/tools
```

* 컴파일 환경 설정 파일 만들기
```sh
$ vi env.sh
```

```sh
#!/bin/sh
export PATH=~/git/pi2/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin:$PATH
export KERNEL=kernel7

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
```

* 환경 설정 적용.
```sh
$ source env.sh
```

* 커널 컴파일시 추가 셋팅
```sh
$ cd ~/git/pi2/
$ git clone --depth=1 https://github.com/raspberrypi/linux

$ cd linux
$ make bcm2709_defconfig
$ make menuconfig

Kernel hacking --> Compile-time checks and compiler option --> Compile the kernel with debug info --> Enable
```
* 최적화 옵션 변경
```sh
$ vim Makefile
```
```make
ifdef CONFIG_CC_OPTIMIZE_FOR_SIZE
KBUILD_CFLAGS	+= -Os $(call cc-disable-warning,maybe-uninitialized,)
else
KBUILD_CFLAGS	+= -O2 # --> 이것을 -O1 으로 변경.. 
endif
```

  * 최적화 옵션을 설정 하지 않을려면 -O0으로 해야 하나 -O0으로 컴파일 하면 에러가 발생한다..(혹시 수정 방법을 아시는 분은 메일로 좀 알려주시길..)

* 커널 빌드
```sh
$ make -j$(nproc) zImage modules dtbs
```

## 커널 디버깅
* 컴파일 된 커널과 DTB파일 추출
```sh
$ scripts/mkknlimg arch/arm/boot/zImage ~/git/pi2/kernel7.img
$ cp arch/arm/boot/dts/bcm2709-rpi-2-b.dtb ~/git/pi2
```

* QEMNU 실행 스크립트 작성.

```sh
$ vi ~/git/pi2/kernel_debug.sh
```

```sh
#!/bin/sh
if [ -z "${KERNEL}" ] ; then
	export PATH=~/git/pi2/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/bin:$PATH
	export KERNEL=kernel7

	export ARCH=arm
	export CROSS_COMPILE=arm-linux-gnueabihf-
fi

BOOT_CMDLINE="rw earlyprintk loglevel=8 console=ttyAMA0,115200 console=tty1 dwc_otg.lpm_enable=0 root=/dev/mmcblk0p2"
DTB_FILE="bcm2709-rpi-2-b.dtb"
KERNEL_IMG="kernel7.img"
SD_IMG="raspbian-jessie.img"

echo "target remote localhost:1234"
qemu-system-arm -s -S -M raspi2 -kernel ${KERNEL_IMG} \
    -sd ${SD_IMG} \
    -append "${BOOT_CMDLINE}" \
    -dtb ${DTB_FILE} -serial stdio &

QEMU_PID=$!
sleep 1
arm-linux-gnueabihf-gdb -ex "target remote localhost:1234" ~/git/pi2/linux/vmlinux

kill -9 ${QEMU_PID}
```

* QEMU 실행 및 디버깅

```sh
$ chmod a+x kernel_debug.sh

$ ./kernel_debug.sh 
target remote localhost:1234
VNC server running on '::1;5900'
Reading symbols from ./vmlinux...done.
Remote debugging using localhost:1234
__vectors_start () at arch/arm/kernel/entry-armv.S:1219
1219		W(b)	vector_rst

# start_kernel에 브레이크 포인트 설정
(gdb) b start_kernel
Breakpoint 1 at 0x8079d9c0: file init/main.c, line 546.

# 디버깅 시작 continue
(gdb) c
Continuing.
Uncompressing Linux... done, booting the kernel.

Breakpoint 1, start_kernel () at init/main.c:546
546	{

# 소스보기.
(gdb) list
541		vmalloc_init();
542		ioremap_huge_init();
543	}
544	
545	asmlinkage __visible void __init start_kernel(void)
546	{
547		char *command_line;
548		char *after_dashes;
549	
550		/*
(gdb) 

```

## Writer
 * [fehead](http://github.com/fehead)