---
title: Linux 內核編譯
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,內核,kernel,編譯
description: 現在安裝時，一般是發行版本已經編譯創建好的通用內核，對於一些特定的需求，如嵌入式設置，那麼可能就需要手動編譯內核鏡像。這裡簡單介紹如何手動編譯內核。
---

現在安裝時，一般是發行版本已經編譯創建好的通用內核，對於一些特定的需求，如嵌入式設置，那麼可能就需要手動編譯內核鏡像。

這裡簡單介紹如何手動編譯內核。

<!-- more -->

## 內核鏡像介紹

通常 Unix 平臺下的內核鏡像稱為 unix，Linux 也與之相似，不過隨著虛擬內存 (Virtual Memory) 的發展，於是支持虛擬內存的內核在前面添加了 vm，其中 vmlinux 源自 vmunix 。現在 Linux 支持虛擬內存，不再像老的操作系統比如 DOS 會有 640KB 內存的限制。

> 在 DOS 的時代，採用的是 16 位段尋址方式，因此只能訪問 1M 的空間。在這 1M 的 RAM 空間裡，段址從 A000 起，被分配給了顯示緩存、外部設備 ROM、BIOS ROM 等等。因此，一般情況下， dos 可供自己分配的也就是段 A000 之前的那部分了，這部分的大小為 640K 。

一般來說，一個可啟動的內核鏡像 (bootable kernel image) 是經過 zlib 算法壓縮的，2.6.30 之後採用 LZMA 或者 BZIP2，vmlinuz 最後的 z 表示內核是壓縮的，這也意味著內核中會有一段解壓程序。

內核中包含了各種內核鏡像的格式，如 vmlinux、zImage、bzImage、uImage 等，首先介紹一下內核中的常見內核文件。

* vmlinux 是靜態編譯出來的最原始的 ELF 文件，包括了內核鏡像、調試信息、符號表等內容；其中 "vm" 代表 "Virtual Memory"，現在一般都是虛擬內存模式，這個是相對於 8086 的實地址而言。

* bzImage 是 vmlinux 經過 gzip 壓縮後的文件，適用於大內核，"bz" 表示 "big zImage"。

* uImage 是 uboot 專用的鏡像文件，它是在 zImage 之前加上一個長度為 0x40 的頭信息，包括了該鏡像文件的類型、加載位置、生成時間、大小等信息。

在發行版本中，通常使用 vmlinuz 標示，而實際上是 bzImage 格式，可以通過 file 命令查看。

zImage、bzImage 中均包含一個微型的 gzip 用於解壓縮內核並引導，兩者的不同之處在於： zImage 解壓縮內核到低端內存 (第一個640K)，bzImage 解壓縮內核到高端內存 (1M以上)。也就是，它們之間最大的差別是對於內核體積大小的限制。

由於 zImage 內核需要放在實模式 1MB 的內存之內，所以其體積受到了限制，目前採用的內核格式大多采用的是 bzImage ，這種格式沒有 1MB 內存限制。arm 中常用的是 zImage，而 x86 中常用的是 bzImage 。

本文以下部分主要以 bzImage 為例進行分析。

## 內核鏡像生成

[vmlinux](http://en.wikipedia.org/wiki/Vmlinux) 是一個靜態鏈接的可執行文件，是 Linux 支持的目標文件格式 (Object File Format) ，如 ELF、COFF 和 a.out ，一般為 ELF ，也是編譯後最原始的文件。通常用於調試，產生符號表等，如果要做成可啟動的內核，需要添加 multiboot header、bootsector、setup routines 等模塊。

可啟動的鏡像文件常被稱為 vmlinuz 或 zImage ，對於 x86 平臺而言，其生成的鏡像默認會保存在 ```arch/x86/boot/bzImage``` 路徑下。

鏡像的創建過程如下。

![bootstrap anatomy of bzimage]({{ site.url }}/images/linux/kernel/bootstap_anatomy_of_bzimage.png "bootstrap anatomy of bzimage"){: .pull-center width="60%" }

bzImage 從前向後分為 3 個部分，前 512 字節為 bootsect ，這就是軟盤引導 Linux 時用到的 bootloader，現在不在用軟盤了，所以這部分就沒有用，其中存儲了一些編譯時生成的內核啟動選項的默認值。

從 512 個字節開始的 512xN 個字節稱為 setup部 分，這是 Linux 內核的實模式部分，這部分在實模式下運行，主要功能是為保護模式的 Linux 內核啟動準備環境。這個部分最後會切換進入保護模式，跳轉到保護模式的內核執行。最後的部分就是保護模式的內核，也就是真正意義上的 Linux 內核。其中 N 的大小可以從 bootsect 後半部得到，詳細地址可以參閱 linux boot protocol。

bzImage (big zImage) 包含了 bootsect.o + setup.o + misc.o + piggy.o ，上述的源碼都在 arch/i386/boot/ 目錄下。需要注意的是，bzImage 不是用 bzip2 壓縮的，bz 表示 “big zImage” 。

zImage 是 vmlinux 經過壓縮後的文件，適用於小內核，小於 512KB ，bzImage 適用於大內核，大於 512KB 。兩者的不同之處在於，老的 zImage 解壓縮內核到低端內存 (第一個640K) ， bzImage 解壓縮內核到高端內存 (1M以上) 。

下面介紹一下 zImage 的加載過程。

![bootstrap vmlinux]({{ site.url }}/images/linux/kernel/bootstrap_vmlinux.png "bootstrap vmlinux"){: .pull-center width="60%" }

現在的加載器 (例如grub)，在加載的時候會直接跳過 bootsect 部分，然後加載 setup 對應的部分到 ```9020H:0(0x90200)``` 內存處，並將保護模式的內核拷貝到 1MB 開始的地方。

啟動時，會根據 [Linux Boot Protocol 2.03]( {{ site.kernel_docs_url }}/Documentation/x86/boot.txt ) 的內容設定參數區的內容，基地址就是 ```9000H:0```，最後使用一條 ```ljmp $0x9020,$0``` 跳轉到 setup 段，剩下的事情就是 Linux 自己的了。

也就是上述會直接跳過 bootsect 部分,在 setup 中,會做一些初始化，如設置 Video (video.S) 等。

### bzImage 分析

在源碼的根目錄下，可以通過 ```cat .vmlinux.cmd``` 查看 vmlinux 的鏈接文件。同樣， bzImage 可以查看 ```arch/x86/boot/.bzImage.cmd``` ，實際是通過 build 將 ```setup.bin``` 和 ```vmlinux.bin``` 的內容直接寫入一個文件；build 的源碼為 ```arch/x86/boot/tools/build.c``` 。

對於 ```setup.bin``` 和 ```vmlinux.bin``` 同樣可以在 ```arch/x86/boot/``` 目錄下查看 ```.setup.bin.cmd``` 和 ```.vmlinux.bin.cmd``` 。可知，兩者分別由 ```setup.elf``` 和 ```vmlinux``` 生成。

在生成 ```setup.elf``` 時是使用 ```setup.ld``` 鏈接腳本文件進行鏈接。通過該腳本可以發現，最早的是 ```.bstext``` 段，位於 ```arch/x86/boot/head.S``` 。

最初執行時是將內核的前 512bytes 加載到 0x7c00 ，然後將控制權交給 Kernel 。為了兼容，現在只會輸出 ```"Direct booting from floppy is no longer supported. Please use a boot loader program instead. Remove disk and press any key to reboot..."``` 。在前 512bytes 中還會包含一個 Header ，同樣也是為了兼容設置的。

現在 Bootloader 會直接跳轉到第二個 512bytes ，也即鏈接腳本指定的入口 ```_start()@head.S```；第一條指令是跳轉到 ```start_of_setup``` ，然後緊跟著是 63Bytes 的 Header ，詳見 [boot.txt]( {{ site.kernel_docs_url }}/Documentation/x86/boot.txt ) 。

## KBuild，Kernel Build System

從 2.6 開始，內核編譯時採用 Kbuild 系統，會兩次掃描 Linux 的 Makefile：首先編譯系統會讀取 Linux 內核頂層的 Makefile；然後根據讀到的內容第二次讀取 Kbuild 的 Makefile 來編譯 Linux 內核。

Kbuild 是建立在 GUN make 機制上的一種編譯體系，可以參考 [kbuild.txt]( {{ site.kernel_docs_url }}/Documentation/kbuild.txt) 文檔，分為 5 步：

1. 頂層 Makefile，主要用於指定編譯 Linux Kernel 目標文件 vmlinux 和模塊，編譯時該文件會被首先讀取，並根據讀到的內容配置編譯環境變量，如內核版本、include路徑、編譯參數。

2. ```.config```，內核配置文件，通過 ```make xxxxconfig``` 生成。

3. ```arch/$(ARCH)/Makefile```，根據具體架構的 Makefile，頂層 Makefile 會包含這個文件來指定平臺相關信息。

4. ```scripts/{Makefile.*,Kbuild.include}```，通用的規則等，面向所有的 Kbuild Makefiles，包含了所有的定義、規則等信息。

5. ```kbuild Makefiles```，一般是每個目錄有一個 ```Makefile/Kconfig```，內核源碼中大約有 500 個這樣的文件，用來執行從上層傳遞下來的命令。

其執行過程大致可以分為如下的幾個步驟：

1. ```make menuconfig```，根據內核配置生成文件 ```.config``` 。

2. 將內核的版本號存儲在 ```include/linux/version.h``` 。

3. 生成指向 ```include/asm-$(ARCH)``` 的符號鏈接 。

4. 更新所有編譯所需的文件： 附加的文件由 ```arch/$(ARCH)/Makefile``` 指定。

5. 遞歸向下訪問所有在下列變量中列出的目錄： ```init-*``` ```core*``` ```drivers-*``` ```net-*``` ```libs-*```，並編譯生成目標文件。這些變量的值可以在 ```arch/$(ARCH)/Makefile``` 中擴充。

6. 聯接所有的目標文件，在源代碼樹頂層目錄中生成 ```vmlinux```。最先聯接是在 ```head-y``` 中列出的文件，該變量由 ```arch/$(ARCH)/Makefile``` 賦值。

7. 最後完成具體架構的特殊要求，並壓縮 vmlinux 生成最終的內核鏡像。

對於 KBuild 可以參考本地文檔 [KBUILD系統原理分析](/reference/linux/kernel/KBUILD_system.pdf) 。

### make menuconfig

當執行 ```make xxxconfig``` 時，實際會匹配頂層 ```Makefile``` 的 ```%config``` 規則，% 採用的是模式規則，所有以 config 結尾的目標都採用這個規則，如：```menuconfig``` ```xconfig``` ```gconfig``` 。

{% highlight makefile %}
%config: scripts_basic outputmakefile FORCE
    $(Q)mkdir -p include/linux include/config
    $(Q)$(MAKE) $(build)=scripts/kconfig $@
{% endhighlight %}

```FORCE``` 實際用來確保，```%config``` 規則下的命令總能執行。首先看 ```scripts_basic``` 這個依賴的規則。

{% highlight makefile %}
scripts_basic:
    $(Q)$(MAKE) $(build)=scripts/basic
    $(Q)rm -f .tmp_quiet_recordmcount
{% endhighlight %}

其中 build 這個變量的定義在 include 的 ```scripts/kbuild.include``` 文件中。

{% highlight makefile %}
build := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.build obj
{% endhighlight %}

KBUILD_SRC 是當前目錄，所以 srctree 參數也是當前目錄，上面的規則可寫成如下形式。

{% highlight makefile %}
scripts_basic:
    @make -f scripts/Makefile.build obj=scripts/basic
{% endhighlight %}

這個規則的命令最終會進入 scripts 目錄，執行 Makefile.build 文件，也就是傳說中的 Kbuild 腳本，並傳遞參數 ```obj=scripts/basic``` 。

對於 outputmakefile 規則。

{% highlight makefile %}
outputmakefile:
ifneq ($(KBUILD_SRC),)
       $(Q)ln -fsn $(srctree) source
       $(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile \
           $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif
{% endhighlight %}

這個規則的命令實際運行一個 shell 腳本 ```scripts/mkmakefile```，並傳遞四個參數，這個腳本主要是在 ```$(objtree)``` 參數指定的目錄中生成一個 Makefile 文件。由於這裡 ```KBUILD_SRC``` 為空，所以這個腳本並不會被執行。

```$(Q)``` 根據 ```KBUILD_VERBOSE``` 是否設置而定義，可以為空或 ```'@'```，當為 ```'@'``` 時，```$(Q)$(MAKE)``` 就相當於 ```@make```，就是不把命令詳細信息打印出來。

所以 ```make menuconfig``` 最終會運行 ```@make $(build)=scripts/kconfig menuconfig``` 命令，```$@``` 就是指目標 ```menuconfig``` 。

```$(build)``` 為 ```-f scripts/Makefile.build obj```，因此最後擴展為 ```@make -f scripts/Makefile.build obj=scripts/kconfig menuconfig``` 。這個命令執行 ```scripts/Makefile.build``` 這個 ```makefile``` 文件，並傳遞參數 ```obj=scripts/kconfig``` 和 ```menuconfig``` 。

因此實際調用的是 ```scripts/kconfig/Makefile``` 中的 ```menuconfig``` 。

{% highlight makefile %}
menuconfig: $(obj)/mconf
    $< $(Kconfig)
{% endhighlight %}

也就是，最終會以 ```arch/x86/Kconfig``` 為參數運行 ```scripts/kconfig/mconf``` 腳本，出現配置界面。

其中 ```mconf``` 是個 ```ncurse``` 的 C 程序，提供圖形配置界面；```arch/x86/Kconfig``` 記錄了各個 x86 的內核配置選項，在 ```make menuconfig``` 或者 ```make xconfig``` 時顯示的菜單項和幫助信息，都是從這個文件中讀出來的。

配置完成之後會生成一個 ```.config``` 文件，接下來通過 ```make bzImage``` 編譯內核。

### 生成 vmlinux

默認會直接選擇第一個目標，不過發現該依賴下面為空。

{% highlight makefile %}
PHONY := _all
_all:
{% endhighlight %}

實際上在 ```make``` 中，如果有重複的目標，則會執行最後的目標和依賴。在 ```Makefile``` 中，有一行 ```include $(srctree)/arch/$(SRCARCH)/Makefile```，也就是實際包含的是 ```arch/x86/Makefile```，在該文件中定義瞭如下的依賴。

{% highlight makefile %}
all: bzImage

bzImage: vmlinux
    $(Q)$(MAKE) $(build)=$(boot) $(KBUILD_IMAGE)
    $(Q)mkdir -p $(objtree)/arch/$(UTS_MACHINE)/boot
    $(Q)ln -fsn ../../x86/boot/bzImage $(objtree)/arch/$(UTS_MACHINE)/boot/$@

    @make -f ./scripts/Makefile.build obj=arch/x86/boot arch/x86/boot/bzImage   # 等價於
{% endhighlight %}

也就是最後會依賴於 vmlinux，實際上該依賴位於頂層 Makefile，對於可執行文件會查看是否存在該文件，而不會執行。

{% highlight makefile %}
vmlinux: scripts/link-vmlinux.sh $(vmlinux-deps) FORCE
    +$(call if_changed,link-vmlinux)

vmlinux-deps := $(KBUILD_LDS) $(KBUILD_VMLINUX_INIT) $(KBUILD_VMLINUX_MAIN)

export KBUILD_VMLINUX_INIT := $(head-y) $(init-y)
export KBUILD_VMLINUX_MAIN := $(core-y) $(libs-y) $(drivers-y) $(net-y)
export KBUILD_LDS          := arch/$(SRCARCH)/kernel/vmlinux.lds
{% endhighlight %}

```head-y``` 的依賴保存在```$(srctree)/arch/$(SRCARCH)/Makefile``` 中，剩餘的保存在頂層的```Makefile``` 中，有些變量也會在 arch 目錄下添加，如 ```init-y := $(patsubst %/, %/built-in.o, $(init-y))``` ，此時會將 ```init/``` 轉換為 ```init/build-in.o``` ，其它類似。

{% highlight makefile %}
head-y := arch/x86/kernel/head_$(BITS).o
head-y += arch/x86/kernel/head$(BITS).o
head-y += arch/x86/kernel/head.o
{% endhighlight %}

此時會在目錄下 **生成了一系列的 build-in.o 文件**，這些目錄實際是通過 ```${vmlinux-deps}``` 定義，然後根據 vmlinux 的生成規則，會執行 ```+$(call if_changed,link-vmlinux)``` 。

```if_changed``` 定義在 ```scripts/Kbuild.include``` 中，與 ```if_changed_rule``` 的作用相似既檢查依賴的更新也檢查命令行參數的更新，如果有更新則會執行後面的命令，在此執行的是 ```cmd_linux-vmlinux``` 命令，具體的命令會保存在 ```.xxx.cmd``` 文件中，在此為 ```.vmlinux.cmd``` 。

也就是調用 ```scripts/link-vmlinux.sh```，最後會生成 ```vmlinux``` 以及 ```System.map``` 等文件。

<!-- 輸出打印變量值的方法是$(warning  $(XXX))，http://coolshell.cn/articles/3790.html -->

```link-vmlinux.sh``` 文件執行流程為。

1. 通過 ```modpost_link()``` 函數鏈接生成```vmlinux.o``` 文件，就是將各個目錄下的 ```build-in.o``` 鏈接在一起，該文件為 ELF 格式。

2. 通過 ```kallsyms()``` 生成內核的符號，用於調試。

3. 通過 ```vmlinux_link()``` 生成靜態文件 ```vmlinux```，與 ```vmlinux.o``` 的區別是指定了鏈接腳本 ```arch/x86/kernel/vmlinux.lds```。

4. 通過 ```mksysmap()``` 生成 ```System.map``` 文件。

```vmlinux``` 是 64 位的程序，其中入口為 ```arch/x86/kernel/{head_64.o,head64.o,head.o}```，這個文件可以視為 C 程序，查看函數可以通過 ```nm vmlinux | grep " _text"``` 。

vmlinux 程序的入口通過 ```ENTRY(phys_startup_64)``` 指定，實際為 ```startup_64```，隨後 ```.=0xxxxxx``` 指明代碼段起始虛擬地址； ```.text : { ....}``` 表示從該位置開始放置所有目標文件的代碼段，裡面又分幾個小區段。

現在已經生成了第一個位於頂層的 vmlinux。

### 生成 bzImage

**vmlinux 的入口函數是 startup_64()，而該函數工作在 64-bit 段尋址的保護模式**，但問題是系統自加電那一刻起，就運行於 16-bit 實模式，因此需要一些輔助程序從 16-bit 實模式轉到 64-bit 保護模式。

設置好必須的參數後才能開啟分頁模式轉到 64-bit 分頁保護模式，其中前半部分是由 ```arch/x86/boot/setup.bin``` 實現的，後半部分則是由 ```arch/x86/kernel/head.o``` 實現的。

接著查看 bzImage 是如何生成的，在 ```arch/x86/Makefile``` 中，實際有效的命令，以及擴展後的命令如下。

{% highlight makefile %}
bzImage: vmlinux
    $(Q)$(MAKE) $(build)=$(boot) $(KBUILD_IMAGE)
    @make -f scripts/Makefile.build obj=arch/x86/boot arch/x86/boot/bzImage    // 等價於
{% endhighlight %}

在 ```Makefile.build``` 中，```include $(kbuild-file)``` 實際會包含 ```arch/x86/boot/Makefile```，也就是實際執行的是。

{% highlight makefile %}
$(obj)/bzImage: $(obj)/setup.bin $(obj)/vmlinux.bin $(obj)/tools/build FORCE
{% endhighlight %}

實際上需要依賴 ```setup.bin```、```vmlinux.bin```，首先查看一下 ```setup.bin``` 的生成。在 ```arch/x86/boot/Makefile``` 中，可以看到 ```setup.bin``` 目標的生成規則鏈為。

{% highlight makefile %}
$(obj)/setup.bin: $(obj)/setup.elf FORCE            # 通過objcopy -O binary生成
    $(call if_changed,objcopy)

LDFLAGS_setup.elf   := -T
$(obj)/setup.elf: $(src)/setup.ld $(SETUP_OBJS) FORCE
    $(call if_changed,ld)
{% endhighlight %}

也就是會根據 ```arch/x86/boot/setup.ld``` 編譯生成 ```setup.elf```，包含了那些文件可以參考 ```.setup.elf.cmd``` 文件，也就是 **bootloader 的入口函數為 _start()``` 。

{% highlight makefile %}
$(obj)/vmlinux.bin: $(obj)/compressed/vmlinux FORCE
    $(call if_changed,objcopy)
{% endhighlight %}

接下來看看 ```vmlinux.bin```，其生成規則鏈為。

{% highlight makefile %}
OBJCOPYFLAGS_vmlinux.bin := -O binary -R .note -R .comment -S
$(obj)/vmlinux.bin: $(obj)/compressed/vmlinux FORCE
    $(call if_changed,objcopy)

$(obj)/compressed/vmlinux: FORCE
    $(Q)$(MAKE) $(build)=$(obj)/compressed $@
{% endhighlight %}

而 ```arch/x86/boot/compressed/Makefile``` 中 vmlinux 規則為。

{% highlight makefile %}
$(obj)/vmlinux: $(vmlinux-objs-y) FORCE
    $(call if_changed,ld)
    @:                                             # 無操作，相當於nop

targets += piggy.S
$(obj)/piggy.S: $(obj)/vmlinux.bin.$(suffix-y) $(obj)/mkpiggy FORCE
    $(call if_changed,mkpiggy)

$(obj)/vmlinux.bin.gz: $(vmlinux.bin.all-y) FORCE  # 將vmlinux.bin壓縮
    $(call if_changed,gzip)

vmlinux.bin.all-y := $(obj)/vmlinux.bin            # 將頂層的vmliunux複製，去除註釋
OBJCOPYFLAGS_vmlinux.bin :=  -R .comment -S
$(obj)/vmlinux.bin: vmlinux FORCE
    $(call if_changed,objcopy)
{% endhighlight %}

此時會在 ```arch/x86/boot/``` 目錄下生成 ```vmlinux.bin``` 文件，大致步驟是：

1. 通過編譯 ```vmlinux.lds.S``` 生成鏈接腳本 ```vmlinux.lds``` 。

2. 使用 objcopy 從頂層目錄拷貝剛剛生成的 vmlinux 到 ```arch/x86/boot/compressed/vmlinux.bin```，並刪除其中的 ```.comment``` 段，此時大小為 20M。

3. 根據編譯選項，選擇一個壓縮程序，對上一步的 ```vmlinux.bin``` 進行壓縮，默認配置是 ```CONFIG_KERNEL_GZIP```，也就是生成 ```vmlinux.bin.gz```，大小為 5.6M。

4. 通過 ```mkpiggyy``` 生成 ```piggy.S```，其中通過 ```.incbin``` 包含 ```vmlinux.bin.gz``` 文件。

5. 編譯生成 ```head_64.o```、```misc.o```、```string.o```、```cmdline.o```、```piggy.o```、```cpuflags.o``` 等。

6. 使用 ```arch/x86/boot/compressed/vmlinux.lds``` 鏈接腳本將上述的文件鏈接生成 ```vmlinux``` 。

7. 去除 ```vmlinux``` 中的調試信息、註釋、符號表等內容，生成 ```arch/x86/boot/vmlinux.bin``` 。

目前已經 **生成了 vmlinux.bin 和 setup.bin** ，接下來就開始鏈接 bzImage 了。

其中 ```arch/x86/boot/tools/build``` 腳本是用於構建最終 bzImage 的程序，他的作用是把 ```setup.bin``` 和 ```vmlinux.bin``` 連接到一起，可以簡單查看 ```.bzImage.cmd``` 命令。

```if_changed``` 定義在 ```scripts/Kbuild.include``` 中，與 ```if_changed_rule``` 的作用相似既檢查依賴的更新也檢查命令行參數的更新，實際執行的命令可以查看 ```.setup.elf.cmd``` 文件。

## 總結
總結一下執行的流程為：

1. 在根目錄生成的第一個 vmlinux，大小約為 24M<br>
在 ```scripts/link-vmlinux.sh``` 腳本中，根據 ```arch/x86/kernel/vmlinux.lds``` 鏈接腳本文件，由 LD 鏈接而成 ELF 格式的靜態文件，該文件屬於未壓縮、帶調試信息、符號表的最初的內核。<br>
該文件包括了 ```arch/x86/kernel/{head_64.o,head64.o,head.o}```，以及相關目錄下的 ```build-in.o``` 文件。

2. 去除註釋信息，生成 vmlinux.bin，大小約為 20M<br>
objcopy 將 vmlinux 轉成二進制的 ```arch/x86/boot/compressed/vmlinux.bin```，刪除 ```.comment``` 字段。

3. 壓縮為 vmlinux.bin.gz，大小約為 5.6M<br>
通過一個壓縮程序壓縮內核，默認使用 gzip，將 ```vmlinux.bin``` 壓縮成 ```arch/x86/boot/compressed/vmlinux.bin.gz``` 。

4. 通過 mkpiggy 生成 piggy.S<br>
通過一定的規則生成 ```piggy.S```，該文件最後通過 ```.incbin``` 包含了 ```vmlinux.bin.gz```，然後編譯生成 piggy.o 後實際包括了上述的壓縮包。

5. 在 arch/x86/boot/compressed 目錄下生成第二個 vmlinux，大小約為 7.6M<br>
將 ```head_64.o```、```misc.o```、```string.o```、```piggy.o``` 等文件編譯成 ```vmlinux```，詳細命令可以查看 ```.vmlinux.cmd```，此時包含了解壓的程序，以及壓縮的內核。

6. 生成 bootloader 的入口引導程序，setup.bin，大小約為 16k<br>
```arch/x86/boot/``` 目錄下是 bootloader 執行後的入口函數，首先生成 ```setup.elf```，然後生成 ```setup.bin``` 文件。

7. 生成 vmlinux.bin，大小約為 5.6M<br>
將 ```arch/x86/boot/compressed/vmlinux``` 去除 ELF header、note 等信息生成 ```arch/x86/boot/vmlinux.bin``` 。

8. 生成最終的 bzImage，大小約為 5.6M<br>
當 ```setup.bin``` 和 ```vmlinux.bin``` 都生成之後，通過 ```arch/x86/boot/tools/build``` 生成最終的 bzImage，該文件位於 ```arch/x86/boot``` 目錄下。

因此，bzImage 組成結構：

{% highlight text %}
bzImage       : bootsect+setup+vmlinux.bin
vmlinux.bin   : head.S+misc.c+vmlinux.bin.gz
vmlinux.bin.gz: gzip 壓縮的 vmlinux.bin 即最終的內核
{% endhighlight %}



## 參考

關於 KBuild 可以參考 [你知道 Linux 內核是如何構建的嗎？](http://www.open-open.com/news/view/dfa811) 或者參考 [本地文檔](/reference/linux/kernel/how_linux_kernel_build.maff)；以及介紹 ARM bzImage 生成過程 [zImage 的生成和加載](/reference/linux/kernel/how_zimage_generate.maff) 。

對於 KBuild 可以參考本地文檔 [KBUILD系統原理分析](/reference/linux/kernel/KBUILD_system.pdf) 。

<!--
內核映像的形成 —— KBuild體系，介紹 Linux 的 Blog ，一系列文章，雲鬆的文章。
http://blog.csdn.net/yunsongice/article/category/759408

Linux 內核 Makefile 體系簡單分析
http://blog.chinaunix.net/uid-20543672-id-3065852.html
-->

{% highlight text %}
{% endhighlight %}
