# 自己動手編譯 Raspberry Pi 的 Kernel

Raspberry Pi 除了是個很方便的小玩具之外，同時也非常適合用他來學習和 Linux 以及 Linux Kernel 的相關知識，本篇將講解如何自己編譯最新的 Linux Kernel 給你的 Raspberry Pi 使用。

## 取得編譯用的 toolchain 與工具


若要進行跨平台編譯，相對應的 toolchain 是必要的，讓我們來說說如何取得 toolchain。

若你是 Gentoo Linux 的使用者，你可以透過 crossdev 建立專用的 toolchain，更完整的資訊，請參考 [Raspberry Pi - Gentoo Wiki](http://wiki.gentoo.org/wiki/Raspberry_Pi) 。


```c
Rosia rpi # crossdev -S -v -t armv6j-hardfloat-linux-gnueabi
```

除此之外，在 GitHub 也有開發 Raspberry Pi 所需要的工具，裡面也包含了預先編譯好的 `arm-bcm2708hardfp-linux-gnueabi` 這個 toolchain，因此我們也把他下載到我們的電腦裡。

```sh
git clone git://github.com/raspberrypi/tools.git
```

注意到在 GitHub 上面的 kernel 有許多不同的 branch，請選擇你想要使用的 branch 來使用。

```sh
coldnew@Rosia ~/rpi/linux $
git branch -a
* rpi-3.10.y
  remotes/origin/HEAD -> origin/rpi-3.10.y
  remotes/origin/master
  remotes/origin/rpi-3.10.y
  remotes/origin/rpi-3.11.y
  remotes/origin/rpi-3.12.y
  remotes/origin/rpi-3.2.27
  remotes/origin/rpi-3.6.y
  remotes/origin/rpi-3.8.y
  remotes/origin/rpi-3.9.y
  remotes/origin/rpi-patches
 ```
## 編譯 Raspberry Pi 的 Linux Kernel

- 修改當前 shell 的環境變數


因為我們是進行跨平台編譯，因此編譯的時候要加入 `ARCH` 以及 `CROSS_COMPILE` 的參數，這邊採用偷懶的做法，直接將資訊覆蓋到目前的 shell 上，這樣在目前的 shell 就是用於編譯 arm 平台的開發環境了。

**註：若是在 Raspberry Pi 上進行編譯，此步驟可以省略。**


```sh
export ARCH=arm
export CROSS_COMPILE=armv6j-hardfloat-linux-gnueabi-
```

```
若你是使用預先編譯好的 toolchain，請將 armv6j-hardfloat-linux-gnueabi- 更改成你的 toolchain 的資訊。
```
- 取得 Raspberry Pi 的 kernel config

如果你手邊有 Raspberry Pi，可以登入他並使用以下方式取得目前 Raspberry Pi 的 Kernel Config。

```sh
pi@raspberrypi:/home/pi$ zcat /proc/config.gz > rpi-config
```

Rapsberry Pi 的 Linux Kernel config 可以在`arch/arm/configs/` 下面找到，共有以下幾個

```sh
coldnew@Rosia ~/rpi/linux $
(cd arch/arm/configs/ && ls bcmrpi*)
bcmrpi_cutdown_defconfig bcmrpi_emergency_defconfig bcmrpi_defconfig bcmrpi_quick_defconfig
```

選擇你想要用的 kernel config，在這邊我選擇 bcmrpi_defconfig

```sh
coldnew@Rosia ~/rpi/linux $ make bcmrpi_defconfig
```

- 設定自己想要的 kernel config

選擇好預設的 config 後，接著就可以進行設定並加上自己需要的 kernel 功能。

```sh
coldnew@Rosia ~/rpi/linux $ make menuconfig
```

- 編譯 kernel

由於我們在前面已經修改了 `ARCH` 和 `CROSS_COMPILE` 這兩個環境變數，因此編譯 Kernel 只需要用 make 就夠囉~

```sh
coldnew@Rosia ~/rpi/linux $
make
```

編譯完成後，會產生 `arch/arm/boot/Image` 檔案，你可以使用 `imagetool-uncompressed.py` 將他產生成 kernel.img，若你沒有這個程式，請到 [GitHub](https://github.com/raspberrypi/tools) 下載。

```sh
coldnew@Rosia ~/rpi/tools/mkimage $ ./imagetool-uncompressed.py ../../linux/arch/arm/boot/Image
```

完成後可以在當前目錄下看到 kernel.img 這個檔案

```sh
coldnew@Rosia ~/rpi/tools/mkimage $ ls
args-uncompressed.txt  boot-uncompressed.txt  first32k.bin  imagetool-uncompressed.py  kernel.img
```
- 安裝 kernel module 到臨時目錄

為了方便將 kernel modules 放置到 SD 卡裡面，這邊我們將 kernel modules 安裝到 ../modules_build 去。

```sh
coldnew@Rosia ~/rpi/linux $ INSTALL_MOD_PATH="../modules_build" make modules_install
```

完成後，在 ../modules_build 資料夾裡面可以看到如下的目錄結構

```sh
coldnew@Rosia ~/rpi/modules_build $
tree -L 2
.
└── lib
    ├── firmware
    └── modules

3 directories, 0 files
```


後面會提到如何將這裡面的 firmware 和 modules 資料夾安裝到你的 SD 卡裡面。

##取得最新的 firmware
Raspberry Pi 開機時，會預先讀取 SD 卡上面的 firmware，很可惜的是，這些 firmware 並未公開原始碼，所以若你有更新你的 kernel，建議是取得最新的 firmware binaries。


```sh
git clone https://github.com/raspberrypi/firmware.git
```

## 更換 SD 卡上的 Kernel 以及 Kernel Module

- 更換 firmware
要更換 SD 卡上面的遇編譯好的 firmware，要進行以下兩個步驟
    - 將 firmware/boot 資料夾內的以下幾個檔案複製到你的 SD 卡 (boot 資料夾或是第一個分割區)
        - bootcode.bin
        - fixup.dat
        - start.elf
    - 將 `firmware/hardfp/opt` 複製到 SD 卡上面的 /opt

- 更換 kernel
更換 kernel 只需要將你剛剛產生的 kernel.img 蓋掉 SD 卡上面的 kernel.img 即可。

- 更換 kernel modules

將前面你使用 `make modules_install` 安裝到 modules_build 裡面的 `firmware` 以及 `modules` 複製到你的 Raspberry Pi SD 卡的根目錄下面的 /lib 資料夾下，替換原本的 `/lib/firmware` 以及 `/lib/modules` 這兩個資料夾。

## 參考連結

[1] [RPi Kernel Compilation - eLinux.org](http://elinux.org/RPi_Kernel_Compilation) <br>
[2] [BrokenDragon's Notes: Raspberry Pi Kernel Compilation](http://bkdragonker.blogspot.tw/2013/03/dvb-module-for-raspberry-pi.html)<br>
[3] [Raspberry Pi - Gentoo Wiki](http://wiki.gentoo.org/wiki/Raspberry_Pi)
