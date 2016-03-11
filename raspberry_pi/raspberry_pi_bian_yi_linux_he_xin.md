# Raspberry Pi 編譯 Linux 核心

這裡介紹如何自己編譯 Raspberry Pi 的 Linux 核心，打造自己所需要的系統。

###編譯 Raspberry Pi 的 Linux 核心

要編譯 Raspberry Pi 用的 Linux 核心有兩種方式，一種是在 Raspberry Pi 中直接編譯，另外一種是在一般的個人電腦中交叉編譯（cross compilation），兩種方式各有利弊，以下分別是這兩種編譯方式的步驟教學。

###在個人電腦中交叉編譯

在個人電腦中交叉編譯可以讓編譯的速度加快，一般我是建議使用這樣的方式，會節省很多時間。


###STEP 1
從 GitHub 下載最新的 Raspberry Pi Linux 核心原始碼：
```sh
git clone --depth=1 https://github.com/raspberrypi/linux
```

###STEP 2
要進行編譯之前，先要安裝好一些交叉編譯所需要的編譯器：
```sh
apt-get install gcc-arm-linux-gnueabihf
```

因為每個人的系統會有些差異，如果您在編譯的過程缺少什麼工具的話，可以再利用 apt 來安裝。

###STEP 3
首先進入 linux 目錄
```sh
cd linux
```

進行編譯之前，要先設定編譯的各種選項，這裏我們使用預設的編譯設定：
```sh
make -j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcmrpi_defconfig
```

載入預設的設定之後，您可以再使用 menuconfig 來微調：
```sh
make -j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```

在這個選單中，您可以自行依照需求新增或移除各種功能。


###STEP 4
進行編譯核心：
```sh
make -j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
```

編譯模組：

```sh
make -j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules
```

這裡的 `-j4` 是讓 `make `可以同時執行` 4 `個 jobs，利用多核心的 CPU 來加速編譯，您可以依照您的 PC 的 CPU 核心數來調整這個值。

###STEP 5
因為我們是在另外一台 PC 中編譯的，所以編譯完之後，要再把編譯好的核心複製到 Raspberry Pi 上面，您也可以直接將 Raspberry Pi 的 MicroSD 卡直接插在 PC 上，這樣就可以將新的核心直接安裝上去，我這裡是用手動複製的方式，兩種做法其實差不多。

將編譯好的 Linux 核心複製出來：
```sh
cp arch/arm/boot/Image ../kernel-new.img
```

將模組安裝至一個暫時的目錄中（../modules）：
```sh
make -j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=../modules modules_install
```

切換至上一層目錄
```sh
cd ..
```

因為 Linux 的核心模組有點大，我先壓縮一下，方便等一下複製的動作：
```sh
tar jcvf modules.tar.bz2 modules/
```

這時候在目前的目錄中，我們應該已經有兩個檔案：
- 一個編譯好的核心（kernel-new.img）
- 一個核心模組的壓縮檔（modules.tar.bz2）
這樣在 PC 上面的編譯工作就算完成了。

###STEP 6
接著把 kernel-new.img 與 modules.tar.bz2 複製到 Raspberry Pi 中，並且將 kernel-new.img 放進 /boot 中：
```sh
sudo cp kernel-new.img /boot/
```

將 modules.tar.bz2 解壓縮
```sh
tar jxf modules.tar.bz2
```

把 modules/lib/modules/ 裡面的東西複製到 /lib/modules/
```sh
sudo -r cp modules/lib/modules/* /lib/modules/
```

將 modules/lib/firmware/ 裡面的東西複製到 /lib/firmware/
```sh
sudo -r cp modules/lib/firmware/* /lib/firmware/
```
###STEP 7
編輯 `/boot/config.txt`，加入一行
`kernel=kernel-new.img`
這行是用來指定系統要使用的核心，您可以透過這樣的方式同時在系統上放置多個不同的核心，以便隨時切換。

這樣新編譯好的核心就安裝好了。


##在 Raspberry Pi 中編譯核心

如果您想要在 Raspberry Pi 中直接編譯新的核心也是可以，不過這樣的方式會非常慢（通常都要好幾個小時以上），除非特殊情況，不然一般都不會用這樣的方式。

###STEP 1
在 Raspberry Pi 上編譯的步驟跟 PC 上差不多，首先下載 Linux 核心原始碼：
```sh
git clone --depth=1 https://github.com/raspberrypi/linux
```

###STEP 2
安裝一些必要的工具：
```sh
apt-get install bc libncurses5-dev
```
###STEP 3
進入 linux 目錄，開始進行編譯
```sh
cd linux
make bcmrpi_defconfig
make menuconfig
make
make modules
```

這個部分跟 PC 交叉編譯的都相同，因為是在 Raspberry Pi 中直接編譯，所以不需要指定交叉編譯的參數，指令比較簡單，但是因為 Raspberry Pi 的處理速度很慢（比起 PC 來說），因此編譯的過程會需要非常久的時間。

###STEP 4
接著安裝新編譯好的核心與模組：
```sh
sudo make modules_install
sudo cp arch/arm/boot/Image /boot/kernel-new.img
```
###STEP 5
編輯 /boot/config.txt，加入一行
```sh
kernel=kernel-new.img
```
這樣就完成了。


##更新韌體（Firmware）

如果您原本的 Linux 核心版本跟新編譯的核心版本相差很多，就要連同韌體一起更新。

###STEP 1
最新的韌體可以從 GitHub 下載：
```sh
git clone https://github.com/raspberrypi/firmware.git
```

這部分由於檔案很多，而且也很大，所以下載的過程需要一段時間。

###STEP 2
下載下來後，將 firmware/boot 目錄中的 bootcode.bin、fixup.dat、start.elf 這三個檔案複製到 /boot 中：
```sh
cp firmware/boot/bootcode.bin /boot/
cp firmware/boot/fixup.dat /boot/
cp firmware/boot/start.elf /boot/
```
將 firmware/hardfp/opt/ 目錄中所有的東西複製到 /opt/

```sh
cd firmware/hardfp/opt/* /opt/
```

這樣就可以了。

##重新開機與測試

安裝好新的核心之後，就可以重新開機測試了。重新開機進入系統之後，可以使用 uname 來查看目前的核心版本：
```sh
uname -a
```

正常的話應該可以看到剛剛編譯好的核心版本：

```sh
Linux raspberrypi 3.12.34+ #1 PREEMPT Thu Dec 11 08:51:55 CST 2014 armv6l GNU/Linux
```