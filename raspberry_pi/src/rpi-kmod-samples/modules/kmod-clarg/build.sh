#!/bin/sh

export PATH=/opt/rpi/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin:$PATH

# build modules
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

# install to sysroot
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_install

# do a "cross-depmod" in sysroot
depmod -b ${PWD}/../sysroot 3.6.11

echo ""
echo "*** modinfo ***"
echo ""
modinfo clargmod.ko
