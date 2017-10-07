# 奔跑吧-linux內核-qemu調試內核


本章主要是介紹一些內核調試的工具和技巧。對於初學者來說,單步調試 Linux 內核應該
是比較酷的一件事情,特別是有類似 Virtual C++的圖形化調試環境。另外還要介紹內核開發者
常用的調試工具,比如 ftrace 和 systemtap 等,它們都是內核開發者的最愛。對於編寫內核代
碼和驅動的讀者來說,內存檢測和死鎖檢測也是不可避免的,特別是做產品開發,這就像飛機
起飛前都要做例行安全檢測一樣,產品最終發佈時可不能有越界訪問等內存問題。最後介紹一
些內核調試的小技巧。本章介紹的調試工具和方法大部分都在 Ubuntu 16.04 + QEMU + ARM
Vexpress 平臺上實驗過。

##6.1 QEMU 調試 Linux 內核


ARM 公司為了加速開發過程提供了 Versatile Express 開發平臺,客戶可以基於 Versatile
Express 平臺進行產品原型開發。做為個人學習者,沒有必要浪費金錢去購買 Versatile Express
開發平臺或者其他 ARM 開發板,完全可以通過 QEMU 來模擬這個開發平臺,同樣可以達到學
習的效果。

##6.1.1 QEMU 運行 ARM Linux 內核

### 1 準備工具

首先在 Untuntu 16.04 中安裝如下工具。

```sh
$ sudo apt-get install qemu libncurses5-dev gcc-arm-linux-gnueabi build-essential
```

下載如下代碼包

-  linux-4.0 內核。https://www.kernel.org/pub/linux/kernel/v4.x/linux-4.0.tar.gz
- usybox 工具包。https://busybox.net/downloads/busybox-1.24.0.tar.bz2

###2 編譯最小文件系統
首先利用 busybox 手工編譯一個最小文件系統。

```sh
cd busybox
export ARCH=arm
export CROSS_COMPILE= arm-linux-gnueabi-
make menuconfig
```

進入 menuconfig 之後,配置成靜態編譯。

```sh
Busybox Settings --->
    Build Options --->
        [*] Build BusyBox as a static binary (no shared libs)
```

然後 make install 就可以編譯完成了。編譯完成之後,在 busybox 根目錄下面會有一
個”_install”的目錄,這個目錄就是編譯好的文件系統需要的一些命令集合。
把_install 目錄拷貝到 linux-4.0 目錄下。進入_install 目錄,先創建 etc、dev 等目錄。

```sh
#mkdir etc
```

```sh
#mkdir dev
#mkdir mnt
#mkdir –p etc/init.d/
```

在_install /etc/init.d/目錄下新創建一個叫 rcS 的文件,並且寫入如下內容:

```sh
mkdir –p /proc
mkdir –p /tmp
mkdir -p /sys
mkdir –p /mnt
/bin/mount -a
mkdir -p /dev/pts
mount -t devpts devpts /dev/pts
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev –s
```
在_install /etc 目錄新創建一個叫 fstab 的文件,並寫入如下內容。

```sh
proc /proc proc defaults 0 0
tmpfs /tmp tmpfs defaults 0 0
sysfs /sys sysfs defaults 0 0
tmpfs /dev tmpfs defaults 0 0
debugfs /sys/kernel/debug debugfs defaults 0 0
```

在_install /etc 目錄新創建一個叫 inittab 的文件,並寫入如下內容。

```sh
::sysinit:/etc/init.d/rcS
::respawn:-/bin/sh
::askfirst:-/bin/sh
::ctrlaltdel:/bin/umount -a –r
```

在_install/dev 目錄下創建如下設備節點,需要 root 權限。

```sh
$ cd _install/dev/
$ sudo mknod console c 5 1
$ sudo mknod null c 1 3
```

###3 編譯內核

```sh
cd linux-4.0
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabi-
make vexpress_defconfig
make menuconfig
```

配置 initramfs,在 initramfs source file 中填入_install。另外需要把 Default kernel command
string 清空。

```sh
General setup --->
    [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
        (_install) Initramfs source file(s)
Boot options -->
    ()Default kernel command string
```

配置 memory split 為“3G/1G user/kernel split”以及打開高端內存。

```sh
Kernel Features --->
Memory split (3G/1G user/kernel split) --->
[ *] High Memory Support
```

開始編譯 kernel。

```sh
$ make bzImage –j4 ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
$ make dtbs
```

運行 QEMU 來模擬 4 核 Cortex-A9 的 Versatile Express 開發平臺。

```sh
$ qemu-system-arm -M vexpress-a9 -smp 4 -m 1024M -kernel arch/arm/boot/zImage
-append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb
arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic
```

運行結果如下:

```sh
figo@figo-OptiPlex-9020:~/work/linux-4.0$ qemu-system-arm -M vexpress-a9 -smp 4 -
m 1024M -kernel arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0
loglevel=8" -dtb arch/arm/boot/dts/vexpress-v2p-ca9.dtb -nographic
Booting Linux on physical CPU 0x0
Initializing cgroup subsys cpuset
Linux version 4.0.0 (figo@figo-OptiPlex-9020) (gcc version 4.6.3 (Ubuntu/Linaro 4.6.3-
1ubuntu5) ) #9 SMP Wed Jun 22 04:23:19 CST 2016
CPU: ARMv7 Processor [410fc090] revision 0 (ARMv7), cr=10c5387d
CPU: PIPT / VIPT nonaliasing data cache, VIPT nonaliasing instruction cache
Machine model: V2P-CA9
Memory policy: Data cache writealloc
On node 0 totalpages: 262144
free_area_init_node: node 0, pgdat c074c600, node_mem_map eeffa000
Normal zone: 1520 pages used for memmap
Normal zone: 0 pages reserved
Normal zone: 194560 pages, LIFO batch:31
HighMem zone: 67584 pages, LIFO batch:15
PERCPU: Embedded 10 pages/cpu @eefc1000 s11712 r8192 d21056 u40960
pcpu-alloc: s11712 r8192 d21056 u40960 alloc=10*4096
pcpu-alloc: [0] 0 [0] 1 [0] 2 [0] 3
Built 1 zonelists in Zone order, mobility grouping on. Total pages: 260624
Kernel command line: rdinit=/linuxrc console=ttyAMA0 loglevel=8
log_buf_len individual max cpu contribution: 4096 bytes
log_buf_len total cpu_extra contributions: 12288 bytes
log_buf_len min size: 16384 bytes
log_buf_len: 32768 bytes
early log buf free: 14908(90%)
PID hash table entries: 4096 (order: 2, 16384 bytes)
Dentry cache hash table entries: 131072 (order: 7, 524288 bytes)
Inode-cache hash table entries: 65536 (order: 6, 262144 bytes)
Memory: 1031644K/1048576K available (4745K kernel code, 157K rwdata, 1364K
rodata, 1176K init, 166K bss, 16932K reserved, 0K cma-reserved, 270336K highmem)
Virtual kernel memory layout:
vector : 0xffff0000 - 0xffff1000 ( 4 kB)
fixmap : 0xffc00000 - 0xfff00000 (3072 kB)
vmalloc : 0xf0000000 - 0xff000000 ( 240 MB)
lowmem : 0xc0000000 - 0xef800000 ( 760 MB)
pkmap : 0xbfe00000 - 0xc0000000 (
2 MB)
modules : 0xbf000000 - 0xbfe00000 ( 14 MB)
.text : 0xc0008000 - 0xc05ff80c (6111 kB)
.init : 0xc0600000 - 0xc0726000 (1176 kB)
.data : 0xc0726000 - 0xc074d540 ( 158 kB)
.bss : 0xc074d540 - 0xc0776f38 ( 167 kB)
SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=4, Nodes=1
Hierarchical RCU implementation.
Additional per-CPU info printed with stalls.
RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=4.
RCU: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=4
NR_IRQS:16 nr_irqs:16 16
smp_twd: clock not found -2
sched_clock: 32 bits at 24MHz, resolution 41ns, wraps every 178956969942ns
CPU: Testing write buffer coherency: ok
CPU0: thread -1, cpu 0, socket 0, mpidr 80000000
Setting up static identity map for 0x604804f8 - 0x60480550
CPU1: thread -1, cpu 1, socket 0, mpidr 80000001
CPU2: thread -1, cpu 2, socket 0, mpidr 80000002
CPU3: thread -1, cpu 3, socket 0, mpidr 80000003
Brought up 4 CPUs
SMP: Total of 4 processors activated (1648.43 BogoMIPS).
Advanced Linux Sound Architecture Driver Initialized.
Switched to clocksource arm,sp804
Freeing unused kernel memory: 1176K (c0600000 - c0726000)
Please press Enter to activate this console.
/ # ls
sbin sys tmp usr bin dev etc linuxrc proc
/ #
```
在 Ubuntu 另外一個超級終端中輸入 killall qemu-system-arm 就可以關閉 QEMU 平臺。


##6.1.2 QEMU 調試 ARM-Linux 內核
安裝 ARM GDB 工具。

```sh
$ sudo apt-get install gdb-arm-none-eabi
```

首先要確保編譯的內核包含調試信息。

```sh
Kernel hacking --->
    Compile-time checks and compiler options --->
        [*] Compile the kernel with debug info
```

重新編譯內核。在超級終端中輸入:

```sh
$ qemu-system-arm -nographic -M vexpress-a9 -m 1024M -kernel
arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb
arch/arm/boot/dts/vexpress-v2p-ca9.dtb -S -s
```

- -S:表示 QEMU 虛擬機會凍結 CPU 直到遠程的 GDB 輸入相應控制命令
- -s:表示在 1234 端口接受 GDB 的調試連接


圖 6.1 gdb 調試內核
然後在另外一個超級終端中啟動 ARM GDB

![](./images/qemu_gdb.png)


```sh
$ cd linux-4.0
$ arm-none-eabi-gdb --tui vmlinux
(gdb) target remote localhost:1234  <= 通過 1234 端口遠程連接到 QEMU 平臺
(gdb) b start_kernel                <= 在內核的 start_kernel 處設置斷點
(gdb) c
```





如上圖所示,GDB 開始接管 ARM-Linux 內核運行,並且到斷點中暫停,這時候就可以使用
GDB 命令來調試內核了。


##6.1.3 QEMU 運行 ARMv8 開發平臺
Ubuntu16.04 版本的 qemu 包含了 qemu-system-aarch64 工具,但是 Ubunut14.04 版本則需
要自己編譯了。下載 qemu2.6 軟件包 1 ,按照如下步驟編譯 qemu。

```sh
sudo apt-get build-dep qemu
tar –jxf qemu-2.6.0.tar.bz2
cd qemu-2.6.0
./configure –target-list=aarch64-softmmu
make
sudo make install
```

安裝如下工具包。

````h
$sudo apt-get install gcc-aarch64-linux-gnu
```

同樣需要編譯和製作一個基於 aarch64 架構的最小文件系統。可以參照之前的做法,只是
編譯環境變量不一樣了。

```sh
$ export ARCH=arm64
$ export CROSS_COMPILE=aarch64-linux-gnu-
```

下面開始編譯內核,我們依然採用 linux-4.0 內核。
```sh
cd linux-4.0
export ARCH=arm64
export CROSS_COMPILE= aarch64-linux-gnu-
make menuconfig
```

依然採用 initramfs 方式來加載最小文件系統,假設編譯的最小文件系統放在 linux-4.0 根目
錄下面,文件目錄叫_install_arm64,以區別之前編譯的 arm32 的最小文件系統。另外設置頁的
大小為 4KB,系統的總線位寬為 48 位。


```sh
General setup --->
    [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
        (_install_arm64) Initramfs source file(s)
Boot options -->
    ()Default kernel command string
Kernel Features --->
    Page size (4KB) --->
        Virtual address space size (48-bit) --->
```

輸入 make –j4 開始編譯內核。
運行 QEMU 來模擬 2 核 Cortex-A57 開發平臺。

```sh
$ qemu-system-aarch64 -machine virt -cpu cortex-a57 -machine type=virt -nographic -
m 2048 –smp 2 -kernel arch/arm64/boot/Image --append "rdinit=/linuxrc
console=ttyAMA0"
```

運行結果如下(刪掉部分信息)

```sh
Booting Linux on physical CPU 0x0
Initializing cgroup subsys cpu
Linux version 4.0.0 (figo@figo-OptiPlex-9020) (gcc version 4.9.1 20140529 (prerelease)
(crosstool-NG linaro-1.13.1-4.9-2014.08 - Linaro GCC 4.9-2014.08) ) #3 SMP PREEMPT
Mon Jun 27 02:44:27 CST 2016
CPU: AArch64 Processor [411fd070] revision 0
Detected PIPT I-cache on CPU0
efi: Getting EFI parameters from FDT:
efi: UEFI not found.
cma: Reserved 16 MiB at 0x00000000bf000000
On node 0 totalpages: 524288
DMA zone: 8192 pages used for memmap
DMA zone: 0 pages reserved
DMA zone: 524288 pages, LIFO batch:31
psci: probing for conduit method from DT.
psci: PSCIv0.2 detected in firmware.
psci: Using standard PSCI v0.2 function IDs
PERCPU: Embedded 14 pages/cpu @ffff80007efcb000 s19456 r8192 d29696 u57344
pcpu-alloc: s19456 r8192 d29696 u57344 alloc=14*4096
pcpu-alloc: [0] 0 [0] 1
Built 1 zonelists in Zone order, mobility grouping on. Total pages: 516096
Kernel command line: rdinit=/linuxrc console=ttyAMA0 debug
PID hash table entries: 4096 (order: 3, 32768 bytes)
Dentry cache hash table entries: 262144 (order: 9, 2097152 bytes)
Inode-cache hash table entries: 131072 (order: 8, 1048576 bytes)
software IO TLB [mem 0xb8a00000-0xbca00000] (64MB) mapped at
[ffff800078a00000-ffff80007c9fffff]
Memory: 1969604K/2097152K available (5125K kernel code, 381K rwdata, 1984K
rodata, 1312K init, 205K bss, 111164K reserved, 16384K cma-reserved)
Virtual kernel memory layout:
vmalloc : 0xffff000000000000 - 0xffff7bffbfff0000 (126974 GB)
vmemmap : 0xffff7bffc0000000 - 0xffff7fffc0000000 ( 4096 GB maximum)
0xffff7bffc1000000 - 0xffff7bffc3000000 (
32 MB actual)
fixed : 0xffff7ffffabfe000 - 0xffff7ffffac00000 (
8 KB)
PCI I/O : 0xffff7ffffae00000 - 0xffff7ffffbe00000 (
16 MB)
modules : 0xffff7ffffc000000 - 0xffff800000000000 (
64 MB)
memory : 0xffff800000000000 - 0xffff800080000000 ( 2048 MB)
.init : 0xffff800000774000 - 0xffff8000008bc000 ( 1312 KB)
.text : 0xffff800000080000 - 0xffff8000007734e4 ( 7118 KB)
.data : 0xffff8000008c0000 - 0xffff80000091f400 ( 381 KB)
SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=2, Nodes=1
Preemptible hierarchical RCU implementation.
Additional per-CPU info printed with stalls.
RCU restricting CPUs from NR_CPUS=64 to nr_cpu_ids=2.
RCU: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=2
NR_IRQS:64 nr_irqs:64 0
GICv2m: Node v2m: range[0x8020000:0x8020fff], SPI[80:144]
Architected cp15 timer(s) running at 62.50MHz (virt).
sched_clock: 56 bits at 62MHz, resolution 16ns, wraps every 2199023255552ns
Console: colour dummy device 80x25
Calibrating delay loop (skipped), value calculated using timer frequency.. 125.00
BogoMIPS (lpj=625000)
pid_max: default: 32768 minimum: 301
Security Framework initialized
Mount-cache hash table entries: 4096 (order: 3, 32768 bytes)
Mountpoint-cache hash table entries: 4096 (order: 3, 32768 bytes)
Initializing cgroup subsys memory
Initializing cgroup subsys hugetlb
hw perfevents: no hardware support available
EFI services will not be available.
CPU1: Booted secondary processor
Detected PIPT I-cache on CPU1
Brought up 2 CPUs
SMP: Total of 2 processors activated.
devtmpfs: initialized
DMI not present or invalid.
NET: Registered protocol family 16
cpuidle: using governor ladder
cpuidle: using governor menu
vdso: 2 pages (1 code @ ffff8000008c5000, 1 data @ ffff8000008c4000)
hw-breakpoint: found 6 breakpoint and 4 watchpoint registers.
DMA: preallocated 256 KiB pool for atomic allocations
Freeing unused kernel memory: 1312K (ffff800000774000 - ffff8000008bc000)
Freeing alternatives memory: 8K (ffff8000008bc000 - ffff8000008be000)
Please press Enter to activate this console.
/ #
```

##6.1.4 文件系統支持
本書內存管理中會講述頁面回收相關內容,頁面回收代碼相當複雜,在 QEMU 上建立一個
可以調試的環境顯得相當必要了。這裡介紹如何添加了一個 swap 分區。

在 Ubuntu 中創建一個 64MB 的 image。

```sh
$ dd if=/dev/zero of=swap.img bs=512 count=131072 <=這裡使用 DD 命令
```

然後通過 SD 卡的方式加載 swap.img 到 QEMU 中。

```sh
$ qemu-system-arm -nographic -M vexpress-a9 -m 64M -kernel
arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb
arch/arm/boot/dts/vexpress-v2p-ca9.dtb -sd swap.img
[...]
# mkswap /dev/mmcblk0 <=第一次需要格式化 swap 分區
# swapon /dev/mmcblk0 <= 使能 swap 分區
# free


total used free shared buffers 
Mem: 1026368 9844 1016524 1360 4
-/+ buffers: 9840 1016528
Swap: 65532 0 65532 <= 可以看到 swap 分區已經工作了
```

如果需要調試頁面回收方面的代碼,那可以在 kswapd()函數裡設置斷點,但是需要在編寫
一個應用程序模擬吃掉內存來觸發 kswapd 內核線程工作。QEMU 裡的“-m 64M”設置了 64MB
內存是為了方便觸發 kswapd 內核線程工作。

```sh
$ dd if=/dev/zero of=ext4.img bs=512 count=131072 <=創建一個 img 鏡像
$ mkfs.ext4 ext4.img <=格式化 ext4.img 成 ext4 格式
```

另外掛載 ext4 文件系統需要打開如下配置選項。

```sh
[arch/arm/configs/vexpress_defconfig]
CONFIG_LBDAF=y
CONFIG_EXT4_FS=y
```

重新編譯內核,make vexpress_defconfig && make。

```sh
$ qemu-system-arm -nographic -M vexpress-a9 -m 1024M -kernel
arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb
arch/arm/boot/dts/vexpress-v2p-ca9.dtb -sd ext4.img
[...]
# mount -t ext4 /dev/mmcblk0 /mnt/ <=掛載 SD 卡到/mnt 目錄
```

##6.1.5 圖形化調試

之前介紹瞭如何使用 gdb 和 QEMU 來調試 Linux 內核源代碼。由於 gdb 是命令行的方式,
可能有不少讀者希望在 Linux 中能有類似 Virtual C++這樣的圖形化的開發工具。這裡介紹使用
Eclipse 這個工具來調試內核。Eclipse 是著名的跨平臺的開源集成開發環境(IDE),最初主要用
於 JAVA 語言開發,目前可以支持 C/C++、Python 等多種開發語言。Eclipse 最初由 IBM 公司開
發,2001 年貢獻給開源社區,有很多開發環境都基於 Eclipse 來完成。
首先安裝 Eclipse-CDT 這個軟件。Eclipse-CDT 是 Eclipse 的一個插件,提供強大的 C/C++編譯
和編輯功能。

```sh
$ sudo apt-get install eclipse-cdt
```

打開 Eclipse 菜單選擇“Window->Open Perspective->C/C++”。新創建一個 C/C++的 Makefile
工程,在“File->New->Project”中選擇“Makefile Project with Exiting Code”來創建一個新的工
程。

接下來配置 Debug 選項。打開 Eclipse 菜單的“Run->Debug Configurations...”選項,創建一
個“C/C++ Attach to Application”調試選項。


-  Project:選擇剛才創建的工程。
-  C/C++ Appliction:選擇編譯 Linux 內核帶符號表信息的 vmlinux。
-  Debugger:選擇 gdbserver
-  GDB debugger:填入 arm-none-eabi-gdb
-  Host name or IP addrss: 填入 localhost
-  Port number: 填入 1234


調試選項設置完成之後,點擊“Debug”按鈕。

![](images/eclipse_gdb1.png)

圖 6.2 eclipse 調試選項設置



在 Ubuntu 的一個終端中先打開 QEMU。為了調試方便,這裡沒有指定多個 CPU 只是單個
CPU。

```sh
$ qemu-system-arm -nographic -M vexpress-a9 -m 1024M -kernel
arch/arm/boot/zImage -append "rdinit=/linuxrc console=ttyAMA0 loglevel=8" -dtb
arch/arm/boot/dts/vexpress-v2p-ca9.dtb -S -s
```

在 Eclipse 菜單中選擇“Run->Debug History”中選擇剛才創建的調試選項,或者在快捷菜
單中點擊“小昆蟲”圖標。

![](./images/debug_icon.png)


在 Eclipse 的 Console 控制檯裡,輸入“file vmlinux”命令來導入調試文件的符號表。

![](images/eclipse_gdb2.png)

圖 6.3 console 控制檯


輸入“b do_fork”在 do_fork 函數裡設置一個斷點了。輸入“c”命令就可以開始運行
QEMU 裡的 Linux 內核了,它會停在 do_fork 函數裡。

![](images/eclipse_gdb3.png)

圖 6.4 Eclipse 調試內核

Eclipse 調試內核比使用 gdb 命令要直觀很多,比如參數、局部變量以及數據結構的值都會
自動顯示在“Variables”標籤卡上,不需要每次都使用 gdb 的打印命令才能看到變量的值。讀
者可以單步調試並且直觀的調試內核了。


最後說明一點,讀者可能發現 gdb 在單步調試內核時會出現光標亂跳以及無法打印有些變
量的值等問題,其實這個不能怪罪 gdb 或者 QEMU。那是因為內核編譯的默認優化選項是 O2,
因此如果希望光標不會亂跳,那可以嘗試把 Linux-4.0 根目錄的 Makefile 中 O2 改成 O0,但是這
樣會編譯有問題,所以希望讀者自行修改。


##6.1.6 實驗進階
上述的實驗是利用 QEMU 和 Initramfs 來搭建一個簡單實用的內核調試環境,但是依然存在
一些缺點,比如所有的步驟都需要手工輸入和製作,對於有些剛接觸 Linux 的同學來說可能會
產生各種各樣的問題,另外有很多熱門的開源組件也沒包括進來比如 u-boot、Buildroot 等。為
此,社區大牛吳章金同學非常熱心中國高校的嵌入式系統的教學工作,利用業餘時間搭建了一
個非常好用的實驗平臺 – Cloud Lab 1 。Cloud Lab 利用 Docker 容器化技術可以快速創建好多好用
的實驗環境:

- CS630 Qemu Lab:x86 Linux 彙編語言實驗
- Linux 0.11 Lab:Linux 0.11 內核實驗
- Linux lab:Linux 內核和嵌入式 Linux 實驗

Cloud Lab 具有以下幾個新特性:
- 通過 Docker 加速環境的安裝和重複構建。
- 提供 Docker 鏡像庫,方便直接下載實驗環境。
- 便利化的腳本管理,方便環境使用和部署。
 
因此有精力同學可以嘗試利用 Cloud Lab 來進一步學習和研究。


http://tinylab.org/how-to-deploy-cloud-labs/<br>
http://tinylab.org/docker-qemu-linux-lab/<br>
http://tinylab.org/using-linux-lab-to-do-embedded-linux-development/
