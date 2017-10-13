
#Linux kernel v4.0 code for <奔跑吧Linux內核>

微信號：runninglinuxkernel
微博/微信公眾號：奔跑吧Linux內核

《奔跑吧Linux內核》經過了2個多月的等待，已經在幾大網店上開啟預售啦！
京東：http://item.jd.com/12152745.html?dist=jd

亞馬遜：https://www.amazon.cn/dp/B074JV6WGN/ref=sr_1_1?ie=UTF8&qid=1502099486&sr=8-1

異步社區：http://www.epubit.com.cn/book/details/4835

噹噹：http://product.dangdang.com/25138842.htm

最新樣章下載： http://www.epubit.com.cn/book/details/4835， 點擊“下載PDF樣章”

###1. 目的：
* 《奔跑吧linux內核》第6章裡提供了一個使用QEMU來單步調試ARM32/ARM64的Linux內核的實驗平臺，但是默認內核編譯的時候默認使用“-02”的GCC編譯優化選項，
那麼導致在QUMU+GDB調試環境中出現光標亂跳以及無法打印有些變量的值“<optimized out>”等問題。本git tree就是為瞭解決該問題，在Makefile中修改GCC的優化選項為“-O0”。
本git tree僅僅是為了提供一個好用的單步調試環境，不適用於其他用途。
* 另外還內置了編譯好的busybox小文件系統，方便讀者快速編譯和調試內核。
* git tree中上傳了《奔跑吧Linux內核》第6章關於QEMU+GDB+eclipse調試內核的樣章。
* 集成了SystemTap工具和例子.


###2. 使用方法：
本實驗是在Ubuntu 16.04上完成的，使用其他版本的Linux發行版遇到編譯問題請自行解決了。

```sh
$ sudo apt-get install qemu libncurses5-dev gcc-arm-linux-gnueabi build-essential gdb-arm-none-eabi gcc-aarch64-linux-gnu eclipse-cdt libdw-dev systemtap systemtap-runtime
```

編譯ARM32內核：
```sh
$ export ARCH=arm
$ export CROSS_COMPILE=arm-linux-gnueabi-
$ cd _install_arm32/dev/
$ sudo mknod console c 5 1  (注意，不要遺漏該步驟)
$ cd runninglinuxkernel_4.0
$ make vexpress_defconfig (在runninglinuxkernel_4.0目錄下輸入make命令)
$ make bzImage –j4 
$ make dtbs
```
運行ARM32內核：

```sh
$ qemu-system-arm -M vexpress-a9 -smp 4 -m 1024M -kernel arch/arm/boot/zImage  -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" 
   -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic
```

編譯ARM64內核：
```sh
$ export ARCH=arm64
$ export CROSS_COMPILE= aarch64-linux-gnu-
$ cd _install_arm64/dev
$ sudo mknod console c 5 1  (注意，不要遺漏該步驟)
$ cd runninglinuxkernel_4.0 
$ make defconfig (在runninglinuxkernel_4.0目錄下輸入make命令)
$ make –j4 
```

注意: 由於ubuntu 16.04裡沒有aarch64的gdb, 建議下載和安裝 linaro官方網站上編譯好的工具鏈:
https://releases.linaro.org/components/toolchain/binaries/5.4-2017.01/aarch64-linux-gnu/gcc-linaro-5.4.1-2017.01-i686_aarch64-linux-gnu.tar.xz


運行ARM64內核：
```sh
$ qemu-system-aarch64 -machine virt -cpu cortex-a57 -machine type=virt -nographic -m 2048 –smp 2 -kernel arch/arm64/boot/Image --append "rdinit=/linuxrc console=ttyAMA0"
```

- 使用 gdb

```sh
 qemu-system-aarch64 -machine virt -cpu cortex-a57 -machine type=virt -nographic -m 2048  -kernel arch/arm64/boot/Image --append "rdinit=/linuxrc console=ttyAMA0" -S -s
```

```sh
cgdb ./vmlinux                      <= 自己build gdb 支援所有平臺
(gdb) target remote localhost:1234  <= 通過 1234 端口遠程連接到 QEMU 平臺
(gdb) b start_kernel                <= 在內核的 start_kernel 處設置斷點
(gdb) c
```



調試內核請見pdf文檔。

###3. SystemTap實驗
   內置編譯好SystemTap工具. ARM版本的內置在_install_arm32目錄下, host工具內置在 tools/systemtap-host目錄下.
另外, modules_tools/systemtap目錄下面有一些例子.

編譯hello-world.stp

```sh
$ sudo /home/figo/work/runninglinuxkernel_4.0/runninglinuxkernel_4.0/tools/systemtap_host/bin/stap -gv -a arm -r /home/figo/work/runninglinuxkernel_4.0/runninglinuxkernel_4.0/ -B CROSS_COMPILE=arm-linux-gnueabi- -m hello-world.ko hello-world.stp
(注意,上述有兩個地方需要使用絕對路徑)
$ cp hello-world.ko _install_arm32
$ make
$ qemu-system-arm -M vexpress-a9 -m 1024M -kernel arch/arm/boot/zImage  -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic
#在qemu裡運行 staprun hello-world.ko
```
 
---

補充：
《奔跑吧Linux內核》是基於ARM32/ARM64為藍本的，有的小夥伴也希望能以“-O0”來編譯x86的kernel image，笨叔叔修改了一下以滿足這些小夥伴的需求。

###編譯i386 kernel image：

```sh
$ export ARCH=x86
$ cd _install_x86/dev/
$ sudo mknod console c 5 1  (注意，不要遺漏該步驟, 只需要一次即可)
$ cd runninglinuxkernel_4.0
$ make i386_defconfig (在runninglinuxkernel_4.0目錄下輸入make命令)
$ make bzImage –j4 
```

###運行i386內核：

```sh
$ qemu-system-i386 -kernel arch/x86/boot/bzImage -append "rdinit=/linuxrc console=ttyS0" -nographic
```


###編譯x86_64 kernel image：

```sh
$ export ARCH=x86_64
$ cd _install_x86/dev/
$ sudo mknod console c 5 1  (注意，不要遺漏該步驟, 只需要一次即可)
$ cd runninglinuxkernel_4.0
$ make x86_64_defconfig (在runninglinuxkernel_4.0目錄下輸入make命令)
$ make bzImage –j4 
```

###運行x86_64內核：

```sh
$ qemu-system-x86_64 -kernel arch/x86/boot/bzImage -append "rdinit=/linuxrc console=ttyS0" -nographic
```

另外在使用gdb+qemu調試x86_64內核時，出現“Remote 'g' packet reply is too long”的錯誤，解決辦法如下：

```c
下載gdb-7.8源代碼，修改./gdb/remote.c +6066去掉如下幾行：重新編譯gdb
    /* Further sanity checks, with knowledge of the architecture.  */
    //  if (buf_len > 2 * rsa->sizeof_g_packet)
       //  error (_("Remote 'g' packet reply is too long: %s"), rs->buf);

```






