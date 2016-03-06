# [Raspberry Pi Project] Compile and update Raspbian kernel


想要用RPi研究linux kernel或者要幫RPi新增module的話,學習Compile and update Raspbian kernel應該是免不了的.
你可以使用RPi本來進行compile,或者在你自己的linux system上執行cross-compiler.這篇文章是以後者的方式(使用Ubuntu PC)進行

## 步驟

- 以下所寫RPi上的路徑(/home/me/RPi),請記得依你實際的環境修改 

- Get the latest raspberrypi compiler:
可以從Git Hub上下載pre-build  bmc2708 compiler

```sh
git clone git://github.com/raspberrypi/tools.git
```

- Get the raspberrypi kernel source

git hub上有很多branch版本,你可以特別指定想要的branch,我下載時master版本是rpi-3.10.y,因此以此為例

```sh
git clone git://github.com/raspberrypi/linux.git rpi-3.10.y
```

-  取得一個能正常使用的kernel configuration

可以取出你手邊正在運行的RPi的把config

```sh
zcat /proc/config.gz > .config
```
或者可以從raspberrypi kernel source(arch/arm/configs/)取出一個來(例如bcmrpi_defconfig)修改使用 


- 設定環境變數 CCPREFIX 指向cross-compile tool 

這步可做可不做,不做就是之後的指令打長一點 

```sh
export CCPREFIX=/home/me/RPi/tools-master/arm-bcm2708/arm-bcm2708-linux-gnueabi/bin/arm-bcm2708-linux-gnueabi-
```

- 清除所有 generated files、config 和 various backup files

```sh
make mrproper
```

- 從 RPi 上複製 .config 至 第三步下載的kernel source 的root

- 進行config


```sh
make ARCH=arm CROSS_COMPILE=${CCPREFIX} oldconfig
```

- start building kernel


```sh
make ARCH=arm CROSS_COMPILE=${CCPREFIX}
```

如果電腦有多核心的話,可以下-j (-j<N> where <N> is the number of cores on your system plus one (i.e. -j3 for 2 cores).)
編譯完成後，會產生 arch/arm/boot/Image 檔案，這支Image就是kernel

- 重編Kernel Module


```sh
mkdir /home/me/RPi/new_kernel/modules
make ARCH=arm CROSS_COMPILE=${CCPREFIX} INSTALL_MOD_PATH=/home/me/RPi/new_kernel/modules modules_install
```

編譯完成後的檔案會在/home/me/RPi/new_kernel/modules/lib 下

- 更新kernel跟module

kernel: 複製arch/arm/boot/Image檔案到SD卡的boot partition並且rename成kernel.img<br>
module: 把/home/me/RPi/new_kernel/modules下的lib目錄,複製到SD卡的root<br> partition(直接蓋掉原本的lib內的檔案)<br>


- 如果你build的kernel跟你原本SD卡所使用的kernel版本差異太大,建議下載firmware並更新

```sh
git://github.com/raspberrypi/firmware.git
```

- 重新啟動RPi並確定系統正常運作

##[參考資料]
http://elinux.org/RPi_Kernel_Compilation





