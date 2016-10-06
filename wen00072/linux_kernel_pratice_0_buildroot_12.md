# Linux Kernel Pratice 0: Buildroot (1/2)


理論上不應該要邊移動邊開火，延長戰線。不過計劃趕不上變化，既來之則安之。

最近因為特別因素開始學習Linux kernel，看能不能Linux kernel和STM32兩邊都不要漏掉。不管怎樣，學習和實習絕對分不開，所以還是從環境架設開始吧。這次的實習環境架設的目標是：

1. 可以使用ARM 平台。一方面追求流行，一方面我不想再開x86這個副本
2. 可以方便地建立ARM平台的Linux Rootfs和kernel版本
3. 可以方便地更改指定要編譯的Kernel版本
4. 透過Qemu ，使用2的Rootfs和kernel開機
5. 透過Qemu和搭配的工具可以分析Linux kernel的run time 行為

今天只有辦到1, 2和4而已，剩下的還要繼續努力。

## 目錄

* [測試環境](#lk0_env)
* [安裝Buildroot](#lk0_ins)
    * [下載Buildroot](#lk0_ins_dl)
    * [設定ARM 環境](#lk0_ins_set)
    * [編譯及輸出](#lk0_ins_build)
* [測試](#lk0_test)
* [參考資料](#lk0_ref)
    * [下次準備看的資料](#lk0_ref_data)

<a name="lk0_env"></a>
## 測試環境
因為我已經裝過開發相關的套件，因此如果您是新手可能要自行摸索也許有需要另外安裝的套件如`git`。嘛，練習解讀錯誤訊息也是一種學習。

```sh
$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 14.04.5 LTS
Release:	14.04
Codename:	trusty
```

<a name="lk0_ins"></a>
## 安裝Buildroot
主要分成下面三個步驟

* [下載Buildroot](#lk0_ins_dl)
* [設定ARM 環境](#lk0_ins_set)
* [編譯及輸出](#lk0_ins_build)

<a name="lk0_ins_dl"></a>
### 下載Buildroot
直接看例子，剪下貼上就好

```sh
mkdir buildroot
cd buildroot
git clone git://git.buildroot.net/buildroot
```

<a name="lk0_ins_set"></a>
### 設定ARM 環境
網路上查到大部分都是從`make menuconfig`開始。不過我是很**明確地**要用`Qemu`跑ARM的系統。所以就找了一下發現有下面的指令

```text
make qemu_x86_defconfig
```

想說既然有`x86_defconfig`，那應該有`arm_defconfig`吧? 錯！那我就去`buildroot/board/qemu`目錄下找，有看到`arm-versatile`。印象中[以前](blog/2015/02/07/by-qemu-arm-installation-of-debian-systems/)有用過Qemu跑的Debian系統也是`versatile`。所以就很高興地下了

```text
make qemu_arm-versatile_defconfig
```

結果一樣GG，估狗查才知道正確的用法是：

```
make qemu_arm_versatile_defconfig
```

**更新:** 後來看手冊才知道有`make list-defconfigs`可以查詢有哪些default config，果然前輩說要RTFM是對的，唉。

接下來就用`make menuconfig`做細項調整，我主要是改成

1. 使用glibc
2. 使用gcc 4.8
    * 預設5.x，因為我想要編Linux kernel 4.4.2。以前PC經驗使用gcc 5.x極端痛苦，後來還是換回gcc 4.8
3. 一些除錯設定

另外本來想要嘗試設定更動Kernel版本，但是發現需要更進一步的了解buildroot才能夠達成。當作下次目標吧。

<a name="lk0_ins_build"></a>
### 編譯及輸出
編譯只要下`make`就會幫你下載和編譯開機需要的

1. 套件和一些常用工具，並封裝到`output/image/roofs.ext2`
2. Kernel(預設4.7)，編譯成`zImage`，放在`output/image/zImage`

<a name="lk0_test"></a>
## 測試
接下來也不難，可以參考`board/qemu/arm-versatile/readme.txt`
簡單來說就是執行下面指令，開機完使用`root`登入不用密碼，使用`poweroff`後再手動離開qemu。

```sh
qemu-system-arm -M versatilepb -kernel output/images/zImage -dtb output/images/versatile-pb.dtb -drive file=output/images/rootfs.ext2,if=scsi,format=raw -append "root=/dev/sda console=ttyAMA0,115200" -serial stdio -net nic,model=rtl8139 -net user
```

執行畫面如下

```text
$ qemu-system-arm -M versatilepb -kernel output/images/zImage -dtb output/images/versatile-pb.dtb -drive file=output/images/rootfs.ext2,if=scsi,format=raw -append "root=/dev/sda console=ttyAMA0,115200" -serial stdio -net nic,model=rtl8139 -net user
...
Booting Linux on physical CPU 0x0
Linux version 4.7.0 (user@host) (gcc version 4.8.5 (Buildroot 2016.11-git-00439-g14b2472) ) #1 Mon Sep 26 22:36:42 CST 2016
CPU: ARM926EJ-S [41069265] revision 5 (ARMv5TEJ), cr=00093177
CPU: VIVT data cache, VIVT instruction cache
Machine model: ARM Versatile PB
....
EXT4-fs (sda): re-mounted. Opts: block_validity,barrier,user_xattr,errors=remount-ro
Starting logging: OK
Initializing random number generator... random: dd urandom read with 46 bits of entropy available
done.
Starting network: 8139cp 0000:00:0c.0 eth0: link up, 100Mbps, full-duplex, lpa 0x05E1
...
adding dns 10.0.2.3
OK

Welcome to Buildroot
buildroot login: root
#
```

<a name="lk0_ref"></a>
## 參考資料
* [The Buildroot user manual](https://buildroot.org/downloads/manual/manual.html)
    * 只有看部份，不過官方文件本來就是應該放在第一位
* [Buildroot and QEMU – the quickest receipe for your own Linux](http://pressreset.net/2013/09/buildroot-and-qemu-the-quickest-receipe-for-your-own-linux/)
    * 東西弄完才看到的文章，入門好文

<a name="lk0_ref_data"></a>
### 下次準備看的資料
* [Qemu and the Kernel](http://www.linux-magazine.com/Online/Features/Qemu-and-the-Kernel)
    * 使用Qemu debug kernel的資料
* [Stackoverflow: Can virtfs/9p be used as root file system?](http://unix.stackexchange.com/questions/90423/can-virtfs-9p-be-used-as-root-file-system)
    * Qemu和Host主機共享資料，甚至直接把rootfs放host讓qemu去讀取的方式
