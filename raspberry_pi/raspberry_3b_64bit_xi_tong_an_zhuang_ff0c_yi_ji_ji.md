# raspberry 3b 64bit 系統安裝，以及交叉編譯環境、調試環境的搭建


##一、系統安裝


##二、交叉環境的搭建，以及內核替換

我用的Linux版本是 ubun16.04-64bit

1、安裝交叉編譯器

```sh
sudo apt-get install gcc-aarch64-linux-gnu
```

2、下載Linux 內核

3、編譯

```sh
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcmrpi3_defconfig
make-j 3 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
mkdir modules
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu INSTALL_MOD_PATH=modules modules_install
```

4、替換Linux內核，在raspberry 上面輸入下面命令,(當然必須保證網絡和ssh是可用的，否則就用u盤搬運吧)

```sh
kernel=4.11.11-v8
scp hostname@xx.xx.xx.xx:/yourpath/arch/arm64/boot/Image /boot/Image-${kernel}
scp hostname@xx.xx.xx.xx:/yourpath/arch/arm64/boot/Image.gz /boot/Image-${kernel}.gz
scp -r hostname@xx.xx.xx.xx:/yourpath/modules/lib/modules/4.11.11-v8/ /lib/modules
mkinitrd /boot/initrd.img 4.11.11-v8
```

5、重啟

6、輸入"uname -r" 命令， 發現內核已經更新到最新內核，如果不太自信，可以在某些肯定運行到的代碼裡面加一句log。


編譯過程遇到的一些小問題
解決：內核編譯 fatal error: curses.h: 沒有那個文件或目錄
```sh
apt-get install libncurses5-dev
```
##三、搭建調試環境，並用 kgdb 調試Linux內核

首先聲明一下調試過程中我用的是usb轉串口，所以我的設備是ttyUSB0,,網上的絕大數資料都是ttyS0, 但我想現在用串口的電腦少之又少了吧

3.1. 安裝arm-linux-gdb

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

3.2. 內核開啟kgdb

重新編譯內核，並替換內核

改啟動參數，修改`grub.cfg`配置文件

在 啟動參數裡面加入 `"kgdboc=ttyS0,115200 kgdbwait"`

3.3. 調試內核

首先保證串口是正常工作的

安裝kermit

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

當時我連接完成後，總是沒有顯示數據，不知道為啥，重啟了一下系統結果正常了，不知道為啥。


確保串口連接正常，就可以調試內核了，

先重啟被調試板，會看到下面消息

```sh
aarch64-linux-gdb vmlinux\
set serial baud 115200
target remote /dev/ttyUSB0
```

出現下面信息，說明可以調試了

```sh
Remote debugging using /dev/ttyUSB0
arch_kgdb_breakpoint () at ./arch/arm64/include/asm/kgdb.h:32
32 asm ("brk %0" : : "I" (KGDB_COMPILED_DBG_BRK_IMM));
```