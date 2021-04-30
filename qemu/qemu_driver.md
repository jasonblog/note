# 基于QEMU的Linux Driver开发环境的配置



# 1 工具集合

- QEMU 5.2.0
- Linux Kernel 4.18
- Busybox 1.32.1

# 2 配置Linux Kernel

笔者用`Linux 4.18`, 配置文件选用`vexpress_defconfig`， 如果关心Linux Kernel编译流程，请移步[Linux Kernel 编译流程(1)](https://blog.csdn.net/jun_8018/article/details/112792967)

```shell
#Linux kernel
wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.18.tar.xz
tar xvf linux-4.18.tar.xz
cd linux-4.18
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- vexpress_defconfig
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- dtbs 
```

编译完成后会在arch/arm/boot/目录下生成`zImage`

# 3 配置QEMU

编译`QEMU`过程中，可能会出现错误，请移步[Install QEMU 5.1.50](https://blog.csdn.net/jun_8018/article/details/109123670)

```shell
wget https://download.qemu.org/qemu-5.2.0.tar.xz
tar xvJf qemu-5.2.0.tar.xz
cd qemu-5.2.0
mkdir build && cd build
../configure
make
```

# 4 启动Linux Kernel

用`QEMU`启动`LinuxKernel`，笔者将命令放在`run_linuxkernel.sh`文件中，笔者目录如图所示



```shell
sudo qemu/build/qemu-system-arm -M vexpress-a9 -nographic -smp 4 -m 512 -kernel linux-4.18/arch/arm/boot/zImage -a      ppend "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb linux-4.18/arch/arm/boot/dts/vexpress-v2p-ca9.dtb
```

