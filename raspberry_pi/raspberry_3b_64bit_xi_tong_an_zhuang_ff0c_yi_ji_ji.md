# raspberry 3b 64bit 系统安装，以及交叉编译环境、调试环境的搭建


##一、系统安装


##二、交叉环境的搭建，以及内核替换

我用的Linux版本是 ubun16.04-64bit

1、安装交叉编译器

```sh
sudo apt-get install gcc-aarch64-linux-gnu
```

2、下载Linux 内核

3、编译

```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcmrpi3_defconfig
make-j 3 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
mkdir modules
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu INSTALL_MOD_PATH=modules modules_install
```

4、替换Linux内核，在raspberry 上面输入下面命令,(当然必须保证网络和ssh是可用的，否则就用u盘搬运吧)

```sh
kernel=4.11.11-v8
scp hostname@xx.xx.xx.xx:/yourpath/arch/arm64/boot/Image /boot/Image-${kernel}
scp hostname@xx.xx.xx.xx:/yourpath/arch/arm64/boot/Image.gz /boot/Image-${kernel}.gz
scp -r hostname@xx.xx.xx.xx:/yourpath/modules/lib/modules/4.11.11-v8/ /lib/modules
mkinitrd /boot/initrd.img 4.11.11-v8
```

5、重启

6、输入"uname -r" 命令， 发现内核已经更新到最新内核，如果不太自信，可以在某些肯定运行到的代码里面加一句log。


编译过程遇到的一些小问题
解决：内核编译 fatal error: curses.h: 没有那个文件或目录
```sh
apt-get install libncurses5-dev
```
##三、搭建调试环境，并用 kgdb 调试Linux内核

首先声明一下调试过程中我用的是usb转串口，所以我的设备是ttyUSB0,,网上的绝大数资料都是ttyS0, 但我想现在用串口的电脑少之又少了吧

3.1. 安装arm-linux-gdb

```sh
wget http://ftp.gnu.org/gnu/gdb/gdb-8.0.tar.gz
tar -zxvf gdb-8.0.tar.gz 
mkdir build
../gdb-8.0/configure --target=aarch64-linux --prefix=/usr/local/aarch64-linux
sudo make
sudo make install
makeinfo -version
sudo apt-get install texinfo
sudo make install
export PATH=$PATH:/usr/local/aarch64-linux/bin
```

3.2. 内核开启kgdb

重新编译内核，并替换内核

改启动参数，修改`grub.cfg`配置文件

在 启动参数里面加入 `"kgdboc=ttyS0,115200 kgdbwait"`

3.3. 调试内核

首先保证串口是正常工作的

安装kermit

```sh
sudo apt-get install ckermit
vi ~/.kermit

    set line            /dev/ttyUSB0
    set speed           115200
    set carrier-watch   off
    set handshake       none
    set flow-control    none
    robust
    set file type       bin
    set file name       lit
    set rec pack        1000
    set send pack       1000
    set window          5
    
sudo kermit -c
```

当时我连接完成后，总是没有显示数据，不知道为啥，重启了一下系统结果正常了，不知道为啥。


确保串口连接正常，就可以调试内核了，

先重启被调试板，会看到下面消息

```sh
aarch64-linux-gdb vmlinux\
set serial baud 115200
target remote /dev/ttyUSB0
```

出现下面信息，说明可以调试了

```sh
Remote debugging using /dev/ttyUSB0
arch_kgdb_breakpoint () at ./arch/arm64/include/asm/kgdb.h:32
32 asm ("brk %0" : : "I" (KGDB_COMPILED_DBG_BRK_IMM));
```