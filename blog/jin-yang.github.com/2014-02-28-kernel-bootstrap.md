---
title: Linux 啟動過程
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,內核,kernel,編譯,啟動流程
description: 現在的 Linux 啟動過程一般分成了兩步，也就是首先調用 GRUB 作為通用的啟動服務，然後可以選擇 Windows 或者 Linux 加載。下來，看看 Linux 的加載過程。
---

現在的 Linux 啟動過程一般分成了兩步，也就是首先調用 GRUB 作為通用的啟動服務，然後可以選擇 Windows 或者 Linux 加載。

接下來，看看 Linux 的加載過程。

<!-- more -->

## 啟動加載

Bootloader 執行完之後，將內核加載到物理地址 ```0x100000(1MB)``` 處，並將執行權限交給了內核。然後執行的是 ```setup.bin``` 的入口函數 ```_start()``` ( ```arch/x86/boot/header.S```，鏈接文件為 ```setup.lds``` ) 。

第一條是一個調轉指令，調轉到 ```start_of_setup``` (```start_of_setup-1f```是setup頭部的長度)，在這之間的代碼是一個龐大的數據結構，與 ```bootparam.h``` 中的 ```struct setup_header``` 一一對應。這個數據結構定義了啟動時所需的默認參數，其中一些參數可以通過命令選項 overwrite。很多值是在形成 bzImage 時，由 ```arch/x86/boot/tools/build``` 程序賦值。

接著實際是設置 C 語言環境，主要是設置堆棧，然後調轉到 ```main()@arch/x86/boot/main.c```，現在仍然運行在實模式。```setup.bin``` 主要作用就是完成一些系統檢測和環境準備的工作，其函數調用順序為：

{% highlight text %}
_start()@arch/x86/boot/header.S
  |-start_of_setup()@arch/x86/boot/header.S
    |-main()@arch/x86/boot/main.c
      |-copy_boot_params()                        // 將進行參數複製
      |-detect_memory()                           // 探測物理內存，第一次與內存相關
      |-go_to_protect_mode
        |-realmode_switch_hook()                  // 如果boot_params.hdr.realmode_swtch有非空的hook函數地址則執行之
        |-enable_a20()
        |-reset_coprecessor()                     // 重啟協處理器
        |-protected_mode_jump()@arch/x86/boot/pmjump.S   // 進入 32-bit 模式
{% endhighlight %}

main.c 主要功能為檢測系統參數如: ```Detect memory layout```，```set video mode``` 等，後面會詳細分析，最後調用 ```goto_protect_mode```，設置 32-bit 或 64-bit 保護段式尋址模式，注意: ```main.o``` 編譯為16位實模式程序。

> 內核的日誌信息可以通過 printk() 打印，不過只有信息等級小於 console loglevel 的值時，這些信息才會被打印到 console 上，可以通過如下方法修改日誌等級。
>
> 1. 修改內核啟動時的參數選項，loglevel=level。
> 2. 運行時執行如下命令 dmesg -n level 或者 echo $level > /proc/sys/kernel/printk 。
> 3. 寫程序使用 syslog 系統調用。

```goto_protect_modea()``` 則會調用 ```protected_mode_jump()```，進入 32-bit 的兼容模式，也就是 IA32-e 模式，在最後會執行 ```jmpl *%eax``` 調轉到 32-bits 的入口，也就是 ```0x100000``` 處。

{% highlight text %}
startup_32()
{% endhighlight %}

可以看到 ```arch/x86/boot/compressed/head_64.S``` 的入口函數 ```startup_32```，該函數會解壓縮 Kernel、設置內存地址轉換、進入 64bit 模式。

關於轉換的具體步驟，可以參考 ```《Intel 64 and IA-32 Architectures Software Developer’s Manual》``` 中的 ```9.8.5 Initializing IA-32e Mode``` 部分 。

然後會調轉到 ```arch/x86/kernel/head_64.S``` 的 ```startup_64``` 。


## 準備環境

這個函數主要是為第一個 Linux 進程 (進程0) 建立執行環境，該函數主要執行以下操作：

<!--
1.  把段寄存器初始化為最終值。
2.  把內核的bss段填充為0。
3.  初始化包含在swapper_pg_dir的臨時內核頁表，並初始化pg0，以使線性地址一致地映射同一物理地址。
4.  把頁全局目錄的地址存放在cr3寄存器中，並通過設置cr0寄存器的PG位啟用分頁。
5.  把從BIOS中獲得的系統參數和傳遞給操作系統的參數boot_params放入第一個頁框中。
6.  為進程0建立內核態堆棧。
7.  該函數再一次清零eflags寄存器的所有位。
8.  調用setup_idt用空的中斷處理程序填充中斷描述符表IDT。
9.  識別處理器的型號。
10. 用編譯好的GDT和IDT表的地址來填充gdtr和idtr寄存器。
11. 初始化虛擬機監視器Xen。
12. 向start_kernel()函數進發。
-->

![bootstrap]({{ site.url }}/images/linux/kernel/bootstrap_startup_process.png "bootstrap"){: .pull-center }

最先執行的是 ```arch/x86/boot/header.S``` (由最早的bootsect.S和setup.S修改而來)，一般是彙編語言。最早的時候 Linux 可以自己啟動，因此包含了 Boot Sector 的代碼，但是現在如果執行的話只會輸出 ```"bugger_off_msg"``` ，並重啟，該段為 ```.bstext``` 。

現在的 Bootloader 會忽略這段代碼。上述無效的 Boot Sector 之後還有 15Bytes 的 Real-Mode Kernel Header ，這兩部分總計 512Bytes 。

512 字節之後，也就是在偏移 ```0x200``` 處，**是 Linux Kernel 實模式的入口，也即 _start** 。第一條指令是直接用機器碼寫的跳轉指令 ```(0xeb+[start_of_setup-1])```，因此會直接跳轉到 ```start_of_setup``` 。這個函數主要是設置堆棧、清零 bss 段，然後跳轉到 ```arch/x86/boot/main.c:main()``` 。

main() 函數主要完成一些必要的清理工作，例如探測內存的分佈、設置 Video Mode 等，最後調用 ```go_to_protected_ mode()@arch/x86/boot/pm.c``` 。

### 初始化

在進入保護模式之前，還需要一些初始化操作，最主要的兩項是 中斷 和 內存。

* 中斷<br>在實模式中，中斷向量表保存在地址 0 處，而在保護模式中，中斷向量表的地址保存在寄存器 IDTR 中。
* 內存<br>實模式和保護模式的從邏輯地址到實地址的轉換不同。在保護模式中，需要通過 GDTR 定位全局描述符表(Global Descriptor Table) 。

因此，進入保護模式之前，在 ```go_to_protected_mode()``` 中會通過調用 ```setup_idt()``` 和 ```setup_gdt()``` 創建臨時的中斷描述符表和全局描述符表。最後通過 ```protected_mode_jump()``` (同樣為彙編語言) 進入保護模式。

```protected_mode_jump()``` 會設置 CR0 寄存器的 PE 位。此時，不需要分頁功能，因此分頁功能是關閉的。最主要的是現在不會受 640K 的限制， RAM 的訪問空間達到了 4GB 。

然後會調用 32-bit 的內核入口，即 startup_32 。該函數會初始化寄存器，並通過 decompress_kernel() 解壓內核。

```decompress_kernel()``` 輸出 ```"Decompressing Linux..."``` ，然後 in-place 解壓，解壓後的內核鏡像會覆蓋上圖中的壓縮鏡像。因此，解壓後的內容也是從 1MB 開始。解壓完成後將會輸出 ```"done."```，然後輸出 ```"Booting the kernel."``` 。

此時將會跳轉到保護模式下的入口處 ```(0x100000)```，該入口同樣為 ```startup_32``` ，但與上面的在不同的目錄下。

第二個 ```startup_32``` 同樣為彙編語言，包含了 32-bit 的初始化函數。包括清零保護模式下的 Kernel bss 段、建立全局描述符、建立中斷描述符表，然後跳轉到目標相關的啟動入口，```start_kernel()``` ，過程如下所示。

![bootstrap protect mode]({{ site.url }}/images/linux/kernel/bootstrap_protected_mode_process.png "bootstrap protect mode"){: .pull-center }

## 正式啟動

```start_kernel()@init/main.c``` 的大部分代碼為 C ，而且與平臺相關。該函數會初始化內核的各個子系統和數據結構，包括了調度器(Scheduler)、內存、時鐘等。

然後 ```start_kernel()``` 會調用 ```rest_init()``` ，該函數將會創建一個內核線程，並將 ```kernel_init()``` 作為一個入口傳入。

dmesg 中的信息是從 ```start_kernel()``` 之後記錄的。

> 像 USB、ACPI、PCI 這樣的系統，會通過 subsys_initcall() 定義一個入口，當然還有一些其它的類似入口，可以參考 include/linux/init.h ，實際上時在內核鏡像中定義了 .initcall.init 字段 (連接腳本見vmlinux.lds)。
>
> 這些函數會在 do_initcalls() 中調用。

```rest_init()``` 隨後會調用 ```schedule()``` 啟動任務調度器，並通過 ```cpu_idle()``` 進入睡眠，這是 Linux 中的空閒線程。當沒有可運行的進程時，該線程會調用，否則運行可運行的線程。

此時，之前啟動的線程將會替代進程0(process 0) 即空閒線程。```kernel_init()``` 將會初始化其他的 CPUs ，在此之前，這些 CPUs 並沒有運行。負責初始化的 CPU 被稱為 boot processor ，此時啟動的 CPUs 被稱為 application processors ，這些 CPUs 同樣從實模式啟動，因此也需要類似的初始化。

最後 ```kernel_init()``` 調用 ```init_post()``` ，該函數將會嘗試執行一個用戶進程，執行順序如下 ```/sbin/init```，```/etc/init```，```/bin/init``` 和 ```/bin/sh``` 。如果所有的都失敗了，那麼內核將會停止。

此時執行的進程 PID 為 1 ，它將會檢查配置文件，然後運行相應的進程，如 X11 Windows、登錄程序、網絡服務等。

至此，全部啟動過程完成。

![bootstrap all]({{ site.url }}/images/linux/kernel/bootstrap_startup_process_all.png "bootstrap all"){: .pull-center width="90%" }

## 詳細流程

從代碼角度，介紹啟動時的調用流程。

{% highlight text %}
start_kernel()
 |-smp_setup_processor_id()                  ← 返回啟動時的CPU號
 |-local_irq_disable()                       ← 關閉當前CPU的中斷
 |-setup_arch()                              ← 完成與體系結構相關的初始化工作
 | |-setup_memory_map()                      ← 建立內存圖
 | |-e820_end_of_ram_pfn()                   ← 找出最大的可用頁幀號
 | |-init_mem_mapping()                      ← 初始化內存映射機制
 | |-initmem_init()                          ← 初始化內存分配器
 | |-x86_init.paging.pagetable_init()        ← 建立完整的頁表
 |-parse_early_param()
 | |-parse_args()                            ← 調用兩次parse_args()處理bootloader傳遞的參數
 |-parse_args()
 |-init_IRQ()                                ← 硬件中斷初始化
 |-softirq_init()                            ← 軟中斷初始化
 |
 |-vfs_caches_init_early()
 |-vfs_caches_init()                         ← 根據參數計算可以作為緩存的頁面數，並建立一個存放文件名稱的slab緩存
 | |-kmem_cache_create()                     ← 創建slab緩存
 | |-dcache_init()                           ← 建立dentry和dentry_hashtable的緩存
 | |-inode_init()                            ← 建立inode和inode_hashtable的緩存
 | |-files_init()                            ← 建立filp的slab緩存，設置內核可打開的最大文件數
 | |-mnt_init()                              ← 完成sysfs和rootfs的註冊和掛載
 |   |-kernfs_init()
 |   |-sysfs_init()                          ← 註冊掛載sysfs
 |   | |-kmem_cache_create()                 ← 創建緩存
 |   | |-register_filesystem()
 |   |-kobject_create_and_add()              ← 創建fs目錄
 |   |-init_rootfs()                         ← 註冊rootfs文件系統
 |   |-init_mount_tree()                     ← 建立目錄樹，將init_task的命名空間與之聯繫起來
 |     |-vfs_kern_mount()                    ← 掛載已經註冊的rootfs文件系統
 |     | |-alloc_vfsmnt()
 |     |-create_mnt_ns()                     ← 創建命名空間
 |     |-set_fs_pwd()                        ← 設置init的當前目錄
 |     |-set_fs_root()                       ← 以及根目錄
 |
 |-rest_init()
   |-kernel_init()                           ← 通過kernel_thread()創建獨立內核線程
   | |-kernel_init_freeable()
   | | |-do_basic_setup()
   | | | |-do_initcalls()                    ← 調用子模塊的初始化
   | | |   |-do_initcall_level()
   | | |     |-do_one_initcall()             ← 調用一系列初始化函數
   | | |       |-populate_rootfs()
   | | |         |-unpack_to_rootfs()
   | | |
   | | |-prepare_namespace()
   | |   |-wait_for_device_probe()
   | |   |-md_run_setup()
   | |   |-initrd_load()                     ← 加載initrd
   | |   | |-create_dev()
   | |   | |-rd_load_image()
   | |   |   |-identify_ramdisk_image()      ← 檢查映像文件的magic確定格式，minux、ext2等；並返回解壓方法
   | |   |   | |-decompress_method()
   | |   |   |-crd_load()                    ← 解壓
   | |   |     |-deco()
   | |   |
   | |   |-mount_root()
   | |
   | |-run_init_process()                    ← 執行init，會依次查看ramdisk、命令行指定、/sbin/init等
   |
   |-kthreadd()                              ← 同樣通過kernel_thread()創建獨立內核線程
{% endhighlight %}


### 初始化

在初始化時通常可以分為兩種：A) 一種是關鍵而其必須按照特定順序來完成，通常在 ```start_kernel()``` 中直接調用；B) 以子系統、模塊實現，通過 ```do_initcalls()``` 完成。

在 ```do_initcalls()``` 中調用時，會按照等級，從 level0 ~ level7 來初始化，其宏定義在 ```include/linux/init.h``` 中實現，簡單分為了兩類，內核以及模塊的實現。

下面以 inet_init 的初始化為例，末行為最後的展開格式。

{% highlight c %}
fs_initcall(inet_init);                                           // net/ipv4/af_inet.c
#define fs_initcall(fn) __define_initcall(fn, 5)                  // include/linux/init.h
#define __define_initcall(fn, id) \                               // 同上
      static initcall_t __initcall_##fn##id __used \
      __attribute__((__section__(".initcall" #id ".init"))) = fn

static initcall_t __initcall_inet_init5 __used __attribute__((__section__(".initcall5.init"))) = inet_init;
{% endhighlight %}

```do_initcalls()``` 是從特定的內存區域取出初始化函數的指針，然後調用該函數，通過 ```"vmlinux.lds.h"``` 定義的宏。


## 參考

阮一峰的網絡日誌中有介紹啟動流程，可以查看 [Linux 的啟動流程](http://www.ruanyifeng.com/blog/2013/08/linux_boot_process.html) 。

另外，一個不錯的介紹可以查看 Gustavo Duarte Blog ，[Motherboard Chipsets Mmemory Map](http://duartes.org/gustavo/blog/post/motherboard-chipsets-memory-map) 主要介紹主板的內存；[How Computers Boot Up](http://duartes.org/gustavo/blog/post/how-computers-boot-up) 介紹計算機如何啟動；[Kernel Boot Process](http://duartes.org/gustavo/blog/post/kernel-boot-process) 介紹內核啟動的過程，含有 Linux 和 Windows 的啟動過程 。

<!--
/reference/linux/kernel/bootstrap_motherboard_chipsets_and_the_memory_map.maff
/reference/linux/kernel/bootstrap_how_computers_boot_up.maff
/reference/linux/kernel/bootstrap_the_kernel_boot_process.maff


	<h2>常用工具</h2>
	<p>
		<ul><li>
			讀取 BIOS<br>
			在 Linux 中使用 <a href="http://flashrom.org/Flashrom">FlashROM</a> 讀取 BIOS 。通過如下命令安裝，<pre>$ sudo apt-get install flashrom</pre>

			在讀取之前最好看一下 FlashROM 的 <a href="http://flashrom.org/Supported_hardware">硬件支持列表</a> ，讀寫如下
			<pre># flashrom -r bios_image.bin // 備份 BIOS<br># flashrom -wv new_bios.bin // 刷寫新的 BIOS<br># flashrom -E // 某些芯片需要先擦除</pre><br><br>

			在 Windows 中，可以通過 <a href="reference/BIOS_Backup_TooKit.EXE"> BIOS_Backup_TooKit </a> 備份 BIOS 。
		</li></ul>
	</p>

<h1 id="Improvements">Improvements</h1> <p>
Linux內核從2.4升級到2.6之後，內核的引導過程發生了許多變化，現在研究一下Linux內核2.6版本的主要引導過程。

（參考資料：

1、《深入理解Linux內核》附錄A；

2、Linux源碼自帶文檔http://lxr.linux.no/linux+v2.6.36/Documentation/x86/boot.txt、

3、Linux源碼header.S代碼http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/header.S

）



Linux內核2.6版本系統引導步驟：

<ol><li>
    開機執行BIOS代碼，主要就是POST過程和初始化硬件設備；</li><br><li>

    BIOS代碼運行結束後，BIOS把MBR中的第一部分代碼和分區表加載到內存地址0X00007c00，最後跳轉到第一部分代碼；</li><br><li>

    第一部分代碼把自己加載到內存地址0X00096a00，並在內存地址0X00098000處設置實模式堆棧，然後LILO把自己的第二部分代碼加載到內存地址0X00096c00，最後跳轉到LILO第二部分代碼；</li><br><li>

    LILO第二部分代碼把header.S代碼，一共兩個512字節（boot sector和setup）分別加載到內存地址0X00090000和0X0009200，同時把Linux小內核映像加載到內存地址0X00010000或者Linux大內核映像加載到內存地址0X00100000，最後跳轉到header.S代碼的setup代碼（240行）；</li><br><li>

    header.S代碼的setup過程調用main函數（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/main.c）（301行），最後跳轉到main函數代碼；

6、main函數設置臨時的IDT表、GDT表、調用go_to_protected_mode()（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/pm.c#L104）函數（177行）進入保護模式，最後跳轉到startup_32函數（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/compressed/head_32.S）；

7、startup_32函數調用decompress_kernel()（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/compressed/misc.c）函數（151行）解壓Linux內核映像到內存地址0X00100000，最後跳轉到startup_32函數（http://lxr.linux.no/linux+v2.6.36/arch/x86/kernel/head_32.S）；

8、startup_32函數為第一個進程0設置執行環境，包括初始化段寄存器、初始化內核頁表、設置內核堆棧、加載GDT表、加載IDT表等，最後跳轉到start_kernel()函數（http://lxr.linux.no/linux+v2.6.36/init/main.c#L536）；
<br><br>
<br><br>
start_kernel() 函數完整的初始化了所有 Linux 內核，包括進程調度、內存管理、系統時間等，最後調用 kernel_thread()。<br><br>
</li></ol>


<br><br>
</p>




<br><br><h1>initramfs/initrd鏡像</h1><p>
Linux 在自身初始化完成之後，需要能夠找到並運行第一個用戶程序，通常叫做 "init" 程序。該程序存在於文件系統中，因此，內核必須找到並掛載一個文件系統才可以成功完成系統的引導過程。<br><br>

在 grub 中提供了一個選項 "root=" 用來指定第一個文件系統，但隨著硬件的發展，很多情況下這個文件系統也許是存放在 USB 設備、SCSI 設備等等多種多樣的設備之上，如果需要正確引導，USB 或者 SCSI 驅動模塊首先需要運行起來，可是不巧的是，這些驅動程序也是存放在文件系統裡，因此會形成一個悖論。<br><br>

為解決此問題，Linux kernel 提出了一個 RAM disk 的解決方案，把一些啟動所必須的用戶程序和驅動模塊放在 RAM disk 中，這個 RAM disk 看上去和普通的 disk 一樣，有文件系統、cache 等。內核啟動時，首先把 RAM disk 掛載起來，等到 init 程序和一些必要模塊運行起來之後，再切到真正的文件系統之中。initrd 通常在加載 kernel 之前由 Bootloader 初始化。<br /><br />

上面提到的RAM disk的方案實際上就是initrd。 如果仔細考慮一下，initrd雖然解決了問題但並不完美。 比如，disk有cache機制，對於RAM disk來說，這個cache機制就顯得很多餘且浪費空間；disk需要文件系統，那文件系統（如ext2等）必須被編譯進kernel而不能作為模塊來使用。<br /><br />

Linux 2.6 kernel提出了一種新的實現機制，即initramfs。顧名思義，initramfs只是一種RAM filesystem而不是disk。initramfs實際是一個cpio歸檔，啟動所需的用戶程序和驅動模塊被歸檔成一個文件。因此，不需要cache，也不需要文件系統。<br /><br />


其初始化過程如下。<ol><li>
[boot loader]<br>Bootloader 根據配置文件將 kernel 和 initrd 這兩個 image 加載到 RAM 。</li><li>

[boot loader -> kernel]<br>完成必要的操作後，準備將執行權交給 Linux kernel 。</li><li>

[kernel]<br>進行一系列初始化操作，initrd 所在的 RAM 被 kernel 映射到 /dev/initrd ，通過 kernel 內部的 gzip 解壓到 /dev/ram0 上。</li><li>

[kernel]<br>Linux 以 R/W (可讀寫) 模式將 /dev/ram0 掛載在暫時性的 rootfs 上。</li><li>

[kernel-space -> user-space]<br>Kernel 準備執行 /dev/ram0 上的 /linuxrc 程序，並切換執行流程
[user space] /linuxrc 與相關的程式處理特定的操作，比方說準備掛載 rootfs 等
[user-space -> kernel-space] /linuxrc 執行即將完畢，執行權轉交給 kernel
[kernel] Linux 掛載真正的 rootfs 並執行 /sbin/init 程式
[user space] 依據 Linux distribution 規範的流程，執行各式系統與應用程式
</ol>
在 grub 中，通常有兩個配置項 linux 和 initrd 。
</p>

<br><h2>製作initramfs</h2><p><ol><li>
查看 initramfs 的內容<br>
<pre style="font-size:0.8em; face:arial;">
# mkdir temp && cd temp && cp /boot/initramfs-`uname -r`.img initrd.img
# file initrd.img                                              // 查看文件類型
# mv initrd.img initrd.img.gz && gunzip initrd.img.gz          // 如果是壓縮包
# cpio -i --make-directories &lt; initrd.img                      // 解壓cpio文件類型
</li>

<li>製作initramfs<br />

</li> </ol>
</p>
http://www.ibm.com/developerworks/cn/linux/l-initrd.html  Linux 初始 RAM 磁盤（initrd）概述，含有詳細介紹

http://www.cnblogs.com/cybertitan/archive/2012/10/10/2718971.html
http://www.linuxsir.org/bbs/thread356982.html

arch/x86/kernel/microcode_intel_early.c:737:load_ucode_intel_bsp(void)
再dmesg中輸出第一條信息

startup_64  arch/x86/boot/compressed/head_64.S  內核保護模式的入口

demesg中的信息是從start_kernel()之後記錄的。
http://www.linuxjournal.com/content/lock-free-multi-producer-multi-consumer-queue-ring-buffer
http://www.ibm.com/developerworks/cn/linux/l-cn-lockfree/
http://lwn.net/Articles/340400/
http://wenku.baidu.com/view/a6b26372f242336c1eb95e9e.html  Linux內核跟蹤之ring buffer的實現
https://www.kernel.org/doc/Documentation/circular-buffers.txt
-->




{% highlight text %}
{% endhighlight %}
