---
title: Linux 内核编译
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,内核,kernel,编译
description: 现在安装时，一般是发行版本已经编译创建好的通用内核，对于一些特定的需求，如嵌入式设置，那么可能就需要手动编译内核镜像。这里简单介绍如何手动编译内核。
---

现在安装时，一般是发行版本已经编译创建好的通用内核，对于一些特定的需求，如嵌入式设置，那么可能就需要手动编译内核镜像。

这里简单介绍如何手动编译内核。

<!-- more -->

## 内核镜像介绍

通常 Unix 平台下的内核镜像称为 unix，Linux 也与之相似，不过随着虚拟内存 (Virtual Memory) 的发展，于是支持虚拟内存的内核在前面添加了 vm，其中 vmlinux 源自 vmunix 。现在 Linux 支持虚拟内存，不再像老的操作系统比如 DOS 会有 640KB 内存的限制。

> 在 DOS 的时代，采用的是 16 位段寻址方式，因此只能访问 1M 的空间。在这 1M 的 RAM 空间里，段址从 A000 起，被分配给了显示缓存、外部设备 ROM、BIOS ROM 等等。因此，一般情况下， dos 可供自己分配的也就是段 A000 之前的那部分了，这部分的大小为 640K 。

一般来说，一个可启动的内核镜像 (bootable kernel image) 是经过 zlib 算法压缩的，2.6.30 之后采用 LZMA 或者 BZIP2，vmlinuz 最后的 z 表示内核是压缩的，这也意味着内核中会有一段解压程序。

内核中包含了各种内核镜像的格式，如 vmlinux、zImage、bzImage、uImage 等，首先介绍一下内核中的常见内核文件。

* vmlinux 是静态编译出来的最原始的 ELF 文件，包括了内核镜像、调试信息、符号表等内容；其中 "vm" 代表 "Virtual Memory"，现在一般都是虚拟内存模式，这个是相对于 8086 的实地址而言。

* bzImage 是 vmlinux 经过 gzip 压缩后的文件，适用于大内核，"bz" 表示 "big zImage"。

* uImage 是 uboot 专用的镜像文件，它是在 zImage 之前加上一个长度为 0x40 的头信息，包括了该镜像文件的类型、加载位置、生成时间、大小等信息。

在发行版本中，通常使用 vmlinuz 标示，而实际上是 bzImage 格式，可以通过 file 命令查看。

zImage、bzImage 中均包含一个微型的 gzip 用于解压缩内核并引导，两者的不同之处在于： zImage 解压缩内核到低端内存 (第一个640K)，bzImage 解压缩内核到高端内存 (1M以上)。也就是，它们之间最大的差别是对于内核体积大小的限制。

由于 zImage 内核需要放在实模式 1MB 的内存之内，所以其体积受到了限制，目前采用的内核格式大多采用的是 bzImage ，这种格式没有 1MB 内存限制。arm 中常用的是 zImage，而 x86 中常用的是 bzImage 。

本文以下部分主要以 bzImage 为例进行分析。

## 内核镜像生成

[vmlinux](http://en.wikipedia.org/wiki/Vmlinux) 是一个静态链接的可执行文件，是 Linux 支持的目标文件格式 (Object File Format) ，如 ELF、COFF 和 a.out ，一般为 ELF ，也是编译后最原始的文件。通常用于调试，产生符号表等，如果要做成可启动的内核，需要添加 multiboot header、bootsector、setup routines 等模块。

可启动的镜像文件常被称为 vmlinuz 或 zImage ，对于 x86 平台而言，其生成的镜像默认会保存在 ```arch/x86/boot/bzImage``` 路径下。

镜像的创建过程如下。

![bootstrap anatomy of bzimage]({{ site.url }}/images/linux/kernel/bootstap_anatomy_of_bzimage.png "bootstrap anatomy of bzimage"){: .pull-center width="60%" }

bzImage 从前向后分为 3 个部分，前 512 字节为 bootsect ，这就是软盘引导 Linux 时用到的 bootloader，现在不在用软盘了，所以这部分就没有用，其中存储了一些编译时生成的内核启动选项的默认值。

从 512 个字节开始的 512xN 个字节称为 setup部 分，这是 Linux 内核的实模式部分，这部分在实模式下运行，主要功能是为保护模式的 Linux 内核启动准备环境。这个部分最后会切换进入保护模式，跳转到保护模式的内核执行。最后的部分就是保护模式的内核，也就是真正意义上的 Linux 内核。其中 N 的大小可以从 bootsect 后半部得到，详细地址可以参阅 linux boot protocol。

bzImage (big zImage) 包含了 bootsect.o + setup.o + misc.o + piggy.o ，上述的源码都在 arch/i386/boot/ 目录下。需要注意的是，bzImage 不是用 bzip2 压缩的，bz 表示 “big zImage” 。

zImage 是 vmlinux 经过压缩后的文件，适用于小内核，小于 512KB ，bzImage 适用于大内核，大于 512KB 。两者的不同之处在于，老的 zImage 解压缩内核到低端内存 (第一个640K) ， bzImage 解压缩内核到高端内存 (1M以上) 。

下面介绍一下 zImage 的加载过程。

![bootstrap vmlinux]({{ site.url }}/images/linux/kernel/bootstrap_vmlinux.png "bootstrap vmlinux"){: .pull-center width="60%" }

现在的加载器 (例如grub)，在加载的时候会直接跳过 bootsect 部分，然后加载 setup 对应的部分到 ```9020H:0(0x90200)``` 内存处，并将保护模式的内核拷贝到 1MB 开始的地方。

启动时，会根据 [Linux Boot Protocol 2.03]( {{ site.kernel_docs_url }}/Documentation/x86/boot.txt ) 的内容设定参数区的内容，基地址就是 ```9000H:0```，最后使用一条 ```ljmp $0x9020,$0``` 跳转到 setup 段，剩下的事情就是 Linux 自己的了。

也就是上述会直接跳过 bootsect 部分,在 setup 中,会做一些初始化，如设置 Video (video.S) 等。

### bzImage 分析

在源码的根目录下，可以通过 ```cat .vmlinux.cmd``` 查看 vmlinux 的链接文件。同样， bzImage 可以查看 ```arch/x86/boot/.bzImage.cmd``` ，实际是通过 build 将 ```setup.bin``` 和 ```vmlinux.bin``` 的内容直接写入一个文件；build 的源码为 ```arch/x86/boot/tools/build.c``` 。

对于 ```setup.bin``` 和 ```vmlinux.bin``` 同样可以在 ```arch/x86/boot/``` 目录下查看 ```.setup.bin.cmd``` 和 ```.vmlinux.bin.cmd``` 。可知，两者分别由 ```setup.elf``` 和 ```vmlinux``` 生成。

在生成 ```setup.elf``` 时是使用 ```setup.ld``` 链接脚本文件进行链接。通过该脚本可以发现，最早的是 ```.bstext``` 段，位于 ```arch/x86/boot/head.S``` 。

最初执行时是将内核的前 512bytes 加载到 0x7c00 ，然后将控制权交给 Kernel 。为了兼容，现在只会输出 ```"Direct booting from floppy is no longer supported. Please use a boot loader program instead. Remove disk and press any key to reboot..."``` 。在前 512bytes 中还会包含一个 Header ，同样也是为了兼容设置的。

现在 Bootloader 会直接跳转到第二个 512bytes ，也即链接脚本指定的入口 ```_start()@head.S```；第一条指令是跳转到 ```start_of_setup``` ，然后紧跟着是 63Bytes 的 Header ，详见 [boot.txt]( {{ site.kernel_docs_url }}/Documentation/x86/boot.txt ) 。

## KBuild，Kernel Build System

从 2.6 开始，内核编译时采用 Kbuild 系统，会两次扫描 Linux 的 Makefile：首先编译系统会读取 Linux 内核顶层的 Makefile；然后根据读到的内容第二次读取 Kbuild 的 Makefile 来编译 Linux 内核。

Kbuild 是建立在 GUN make 机制上的一种编译体系，可以参考 [kbuild.txt]( {{ site.kernel_docs_url }}/Documentation/kbuild.txt) 文档，分为 5 步：

1. 顶层 Makefile，主要用于指定编译 Linux Kernel 目标文件 vmlinux 和模块，编译时该文件会被首先读取，并根据读到的内容配置编译环境变量，如内核版本、include路径、编译参数。

2. ```.config```，内核配置文件，通过 ```make xxxxconfig``` 生成。

3. ```arch/$(ARCH)/Makefile```，根据具体架构的 Makefile，顶层 Makefile 会包含这个文件来指定平台相关信息。

4. ```scripts/{Makefile.*,Kbuild.include}```，通用的规则等，面向所有的 Kbuild Makefiles，包含了所有的定义、规则等信息。

5. ```kbuild Makefiles```，一般是每个目录有一个 ```Makefile/Kconfig```，内核源码中大约有 500 个这样的文件，用来执行从上层传递下来的命令。

其执行过程大致可以分为如下的几个步骤：

1. ```make menuconfig```，根据内核配置生成文件 ```.config``` 。

2. 将内核的版本号存储在 ```include/linux/version.h``` 。

3. 生成指向 ```include/asm-$(ARCH)``` 的符号链接 。

4. 更新所有编译所需的文件： 附加的文件由 ```arch/$(ARCH)/Makefile``` 指定。

5. 递归向下访问所有在下列变量中列出的目录： ```init-*``` ```core*``` ```drivers-*``` ```net-*``` ```libs-*```，并编译生成目标文件。这些变量的值可以在 ```arch/$(ARCH)/Makefile``` 中扩充。

6. 联接所有的目标文件，在源代码树顶层目录中生成 ```vmlinux```。最先联接是在 ```head-y``` 中列出的文件，该变量由 ```arch/$(ARCH)/Makefile``` 赋值。

7. 最后完成具体架构的特殊要求，并压缩 vmlinux 生成最终的内核镜像。

对于 KBuild 可以参考本地文档 [KBUILD系统原理分析](/reference/linux/kernel/KBUILD_system.pdf) 。

### make menuconfig

当执行 ```make xxxconfig``` 时，实际会匹配顶层 ```Makefile``` 的 ```%config``` 规则，% 采用的是模式规则，所有以 config 结尾的目标都采用这个规则，如：```menuconfig``` ```xconfig``` ```gconfig``` 。

{% highlight makefile %}
%config: scripts_basic outputmakefile FORCE
    $(Q)mkdir -p include/linux include/config
    $(Q)$(MAKE) $(build)=scripts/kconfig $@
{% endhighlight %}

```FORCE``` 实际用来确保，```%config``` 规则下的命令总能执行。首先看 ```scripts_basic``` 这个依赖的规则。

{% highlight makefile %}
scripts_basic:
    $(Q)$(MAKE) $(build)=scripts/basic
    $(Q)rm -f .tmp_quiet_recordmcount
{% endhighlight %}

其中 build 这个变量的定义在 include 的 ```scripts/kbuild.include``` 文件中。

{% highlight makefile %}
build := -f $(if $(KBUILD_SRC),$(srctree)/)scripts/Makefile.build obj
{% endhighlight %}

KBUILD_SRC 是当前目录，所以 srctree 参数也是当前目录，上面的规则可写成如下形式。

{% highlight makefile %}
scripts_basic:
    @make -f scripts/Makefile.build obj=scripts/basic
{% endhighlight %}

这个规则的命令最终会进入 scripts 目录，执行 Makefile.build 文件，也就是传说中的 Kbuild 脚本，并传递参数 ```obj=scripts/basic``` 。

对于 outputmakefile 规则。

{% highlight makefile %}
outputmakefile:
ifneq ($(KBUILD_SRC),)
       $(Q)ln -fsn $(srctree) source
       $(Q)$(CONFIG_SHELL) $(srctree)/scripts/mkmakefile \
           $(srctree) $(objtree) $(VERSION) $(PATCHLEVEL)
endif
{% endhighlight %}

这个规则的命令实际运行一个 shell 脚本 ```scripts/mkmakefile```，并传递四个参数，这个脚本主要是在 ```$(objtree)``` 参数指定的目录中生成一个 Makefile 文件。由于这里 ```KBUILD_SRC``` 为空，所以这个脚本并不会被执行。

```$(Q)``` 根据 ```KBUILD_VERBOSE``` 是否设置而定义，可以为空或 ```'@'```，当为 ```'@'``` 时，```$(Q)$(MAKE)``` 就相当于 ```@make```，就是不把命令详细信息打印出来。

所以 ```make menuconfig``` 最终会运行 ```@make $(build)=scripts/kconfig menuconfig``` 命令，```$@``` 就是指目标 ```menuconfig``` 。

```$(build)``` 为 ```-f scripts/Makefile.build obj```，因此最后扩展为 ```@make -f scripts/Makefile.build obj=scripts/kconfig menuconfig``` 。这个命令执行 ```scripts/Makefile.build``` 这个 ```makefile``` 文件，并传递参数 ```obj=scripts/kconfig``` 和 ```menuconfig``` 。

因此实际调用的是 ```scripts/kconfig/Makefile``` 中的 ```menuconfig``` 。

{% highlight makefile %}
menuconfig: $(obj)/mconf
    $< $(Kconfig)
{% endhighlight %}

也就是，最终会以 ```arch/x86/Kconfig``` 为参数运行 ```scripts/kconfig/mconf``` 脚本，出现配置界面。

其中 ```mconf``` 是个 ```ncurse``` 的 C 程序，提供图形配置界面；```arch/x86/Kconfig``` 记录了各个 x86 的内核配置选项，在 ```make menuconfig``` 或者 ```make xconfig``` 时显示的菜单项和帮助信息，都是从这个文件中读出来的。

配置完成之后会生成一个 ```.config``` 文件，接下来通过 ```make bzImage``` 编译内核。

### 生成 vmlinux

默认会直接选择第一个目标，不过发现该依赖下面为空。

{% highlight makefile %}
PHONY := _all
_all:
{% endhighlight %}

实际上在 ```make``` 中，如果有重复的目标，则会执行最后的目标和依赖。在 ```Makefile``` 中，有一行 ```include $(srctree)/arch/$(SRCARCH)/Makefile```，也就是实际包含的是 ```arch/x86/Makefile```，在该文件中定义了如下的依赖。

{% highlight makefile %}
all: bzImage

bzImage: vmlinux
    $(Q)$(MAKE) $(build)=$(boot) $(KBUILD_IMAGE)
    $(Q)mkdir -p $(objtree)/arch/$(UTS_MACHINE)/boot
    $(Q)ln -fsn ../../x86/boot/bzImage $(objtree)/arch/$(UTS_MACHINE)/boot/$@

    @make -f ./scripts/Makefile.build obj=arch/x86/boot arch/x86/boot/bzImage   # 等价于
{% endhighlight %}

也就是最后会依赖于 vmlinux，实际上该依赖位于顶层 Makefile，对于可执行文件会查看是否存在该文件，而不会执行。

{% highlight makefile %}
vmlinux: scripts/link-vmlinux.sh $(vmlinux-deps) FORCE
    +$(call if_changed,link-vmlinux)

vmlinux-deps := $(KBUILD_LDS) $(KBUILD_VMLINUX_INIT) $(KBUILD_VMLINUX_MAIN)

export KBUILD_VMLINUX_INIT := $(head-y) $(init-y)
export KBUILD_VMLINUX_MAIN := $(core-y) $(libs-y) $(drivers-y) $(net-y)
export KBUILD_LDS          := arch/$(SRCARCH)/kernel/vmlinux.lds
{% endhighlight %}

```head-y``` 的依赖保存在```$(srctree)/arch/$(SRCARCH)/Makefile``` 中，剩余的保存在顶层的```Makefile``` 中，有些变量也会在 arch 目录下添加，如 ```init-y := $(patsubst %/, %/built-in.o, $(init-y))``` ，此时会将 ```init/``` 转换为 ```init/build-in.o``` ，其它类似。

{% highlight makefile %}
head-y := arch/x86/kernel/head_$(BITS).o
head-y += arch/x86/kernel/head$(BITS).o
head-y += arch/x86/kernel/head.o
{% endhighlight %}

此时会在目录下 **生成了一系列的 build-in.o 文件**，这些目录实际是通过 ```${vmlinux-deps}``` 定义，然后根据 vmlinux 的生成规则，会执行 ```+$(call if_changed,link-vmlinux)``` 。

```if_changed``` 定义在 ```scripts/Kbuild.include``` 中，与 ```if_changed_rule``` 的作用相似既检查依赖的更新也检查命令行参数的更新，如果有更新则会执行后面的命令，在此执行的是 ```cmd_linux-vmlinux``` 命令，具体的命令会保存在 ```.xxx.cmd``` 文件中，在此为 ```.vmlinux.cmd``` 。

也就是调用 ```scripts/link-vmlinux.sh```，最后会生成 ```vmlinux``` 以及 ```System.map``` 等文件。

<!-- 输出打印变量值的方法是$(warning  $(XXX))，http://coolshell.cn/articles/3790.html -->

```link-vmlinux.sh``` 文件执行流程为。

1. 通过 ```modpost_link()``` 函数链接生成```vmlinux.o``` 文件，就是将各个目录下的 ```build-in.o``` 链接在一起，该文件为 ELF 格式。

2. 通过 ```kallsyms()``` 生成内核的符号，用于调试。

3. 通过 ```vmlinux_link()``` 生成静态文件 ```vmlinux```，与 ```vmlinux.o``` 的区别是指定了链接脚本 ```arch/x86/kernel/vmlinux.lds```。

4. 通过 ```mksysmap()``` 生成 ```System.map``` 文件。

```vmlinux``` 是 64 位的程序，其中入口为 ```arch/x86/kernel/{head_64.o,head64.o,head.o}```，这个文件可以视为 C 程序，查看函数可以通过 ```nm vmlinux | grep " _text"``` 。

vmlinux 程序的入口通过 ```ENTRY(phys_startup_64)``` 指定，实际为 ```startup_64```，随后 ```.=0xxxxxx``` 指明代码段起始虚拟地址； ```.text : { ....}``` 表示从该位置开始放置所有目标文件的代码段，里面又分几个小区段。

现在已经生成了第一个位于顶层的 vmlinux。

### 生成 bzImage

**vmlinux 的入口函数是 startup_64()，而该函数工作在 64-bit 段寻址的保护模式**，但问题是系统自加电那一刻起，就运行于 16-bit 实模式，因此需要一些辅助程序从 16-bit 实模式转到 64-bit 保护模式。

设置好必须的参数后才能开启分页模式转到 64-bit 分页保护模式，其中前半部分是由 ```arch/x86/boot/setup.bin``` 实现的，后半部分则是由 ```arch/x86/kernel/head.o``` 实现的。

接着查看 bzImage 是如何生成的，在 ```arch/x86/Makefile``` 中，实际有效的命令，以及扩展后的命令如下。

{% highlight makefile %}
bzImage: vmlinux
    $(Q)$(MAKE) $(build)=$(boot) $(KBUILD_IMAGE)
    @make -f scripts/Makefile.build obj=arch/x86/boot arch/x86/boot/bzImage    // 等价于
{% endhighlight %}

在 ```Makefile.build``` 中，```include $(kbuild-file)``` 实际会包含 ```arch/x86/boot/Makefile```，也就是实际执行的是。

{% highlight makefile %}
$(obj)/bzImage: $(obj)/setup.bin $(obj)/vmlinux.bin $(obj)/tools/build FORCE
{% endhighlight %}

实际上需要依赖 ```setup.bin```、```vmlinux.bin```，首先查看一下 ```setup.bin``` 的生成。在 ```arch/x86/boot/Makefile``` 中，可以看到 ```setup.bin``` 目标的生成规则链为。

{% highlight makefile %}
$(obj)/setup.bin: $(obj)/setup.elf FORCE            # 通过objcopy -O binary生成
    $(call if_changed,objcopy)

LDFLAGS_setup.elf   := -T
$(obj)/setup.elf: $(src)/setup.ld $(SETUP_OBJS) FORCE
    $(call if_changed,ld)
{% endhighlight %}

也就是会根据 ```arch/x86/boot/setup.ld``` 编译生成 ```setup.elf```，包含了那些文件可以参考 ```.setup.elf.cmd``` 文件，也就是 **bootloader 的入口函数为 _start()``` 。

{% highlight makefile %}
$(obj)/vmlinux.bin: $(obj)/compressed/vmlinux FORCE
    $(call if_changed,objcopy)
{% endhighlight %}

接下来看看 ```vmlinux.bin```，其生成规则链为。

{% highlight makefile %}
OBJCOPYFLAGS_vmlinux.bin := -O binary -R .note -R .comment -S
$(obj)/vmlinux.bin: $(obj)/compressed/vmlinux FORCE
    $(call if_changed,objcopy)

$(obj)/compressed/vmlinux: FORCE
    $(Q)$(MAKE) $(build)=$(obj)/compressed $@
{% endhighlight %}

而 ```arch/x86/boot/compressed/Makefile``` 中 vmlinux 规则为。

{% highlight makefile %}
$(obj)/vmlinux: $(vmlinux-objs-y) FORCE
    $(call if_changed,ld)
    @:                                             # 无操作，相当于nop

targets += piggy.S
$(obj)/piggy.S: $(obj)/vmlinux.bin.$(suffix-y) $(obj)/mkpiggy FORCE
    $(call if_changed,mkpiggy)

$(obj)/vmlinux.bin.gz: $(vmlinux.bin.all-y) FORCE  # 将vmlinux.bin压缩
    $(call if_changed,gzip)

vmlinux.bin.all-y := $(obj)/vmlinux.bin            # 将顶层的vmliunux复制，去除注释
OBJCOPYFLAGS_vmlinux.bin :=  -R .comment -S
$(obj)/vmlinux.bin: vmlinux FORCE
    $(call if_changed,objcopy)
{% endhighlight %}

此时会在 ```arch/x86/boot/``` 目录下生成 ```vmlinux.bin``` 文件，大致步骤是：

1. 通过编译 ```vmlinux.lds.S``` 生成链接脚本 ```vmlinux.lds``` 。

2. 使用 objcopy 从顶层目录拷贝刚刚生成的 vmlinux 到 ```arch/x86/boot/compressed/vmlinux.bin```，并删除其中的 ```.comment``` 段，此时大小为 20M。

3. 根据编译选项，选择一个压缩程序，对上一步的 ```vmlinux.bin``` 进行压缩，默认配置是 ```CONFIG_KERNEL_GZIP```，也就是生成 ```vmlinux.bin.gz```，大小为 5.6M。

4. 通过 ```mkpiggyy``` 生成 ```piggy.S```，其中通过 ```.incbin``` 包含 ```vmlinux.bin.gz``` 文件。

5. 编译生成 ```head_64.o```、```misc.o```、```string.o```、```cmdline.o```、```piggy.o```、```cpuflags.o``` 等。

6. 使用 ```arch/x86/boot/compressed/vmlinux.lds``` 链接脚本将上述的文件链接生成 ```vmlinux``` 。

7. 去除 ```vmlinux``` 中的调试信息、注释、符号表等内容，生成 ```arch/x86/boot/vmlinux.bin``` 。

目前已经 **生成了 vmlinux.bin 和 setup.bin** ，接下来就开始链接 bzImage 了。

其中 ```arch/x86/boot/tools/build``` 脚本是用于构建最终 bzImage 的程序，他的作用是把 ```setup.bin``` 和 ```vmlinux.bin``` 连接到一起，可以简单查看 ```.bzImage.cmd``` 命令。

```if_changed``` 定义在 ```scripts/Kbuild.include``` 中，与 ```if_changed_rule``` 的作用相似既检查依赖的更新也检查命令行参数的更新，实际执行的命令可以查看 ```.setup.elf.cmd``` 文件。

## 总结
总结一下执行的流程为：

1. 在根目录生成的第一个 vmlinux，大小约为 24M<br>
在 ```scripts/link-vmlinux.sh``` 脚本中，根据 ```arch/x86/kernel/vmlinux.lds``` 链接脚本文件，由 LD 链接而成 ELF 格式的静态文件，该文件属于未压缩、带调试信息、符号表的最初的内核。<br>
该文件包括了 ```arch/x86/kernel/{head_64.o,head64.o,head.o}```，以及相关目录下的 ```build-in.o``` 文件。

2. 去除注释信息，生成 vmlinux.bin，大小约为 20M<br>
objcopy 将 vmlinux 转成二进制的 ```arch/x86/boot/compressed/vmlinux.bin```，删除 ```.comment``` 字段。

3. 压缩为 vmlinux.bin.gz，大小约为 5.6M<br>
通过一个压缩程序压缩内核，默认使用 gzip，将 ```vmlinux.bin``` 压缩成 ```arch/x86/boot/compressed/vmlinux.bin.gz``` 。

4. 通过 mkpiggy 生成 piggy.S<br>
通过一定的规则生成 ```piggy.S```，该文件最后通过 ```.incbin``` 包含了 ```vmlinux.bin.gz```，然后编译生成 piggy.o 后实际包括了上述的压缩包。

5. 在 arch/x86/boot/compressed 目录下生成第二个 vmlinux，大小约为 7.6M<br>
将 ```head_64.o```、```misc.o```、```string.o```、```piggy.o``` 等文件编译成 ```vmlinux```，详细命令可以查看 ```.vmlinux.cmd```，此时包含了解压的程序，以及压缩的内核。

6. 生成 bootloader 的入口引导程序，setup.bin，大小约为 16k<br>
```arch/x86/boot/``` 目录下是 bootloader 执行后的入口函数，首先生成 ```setup.elf```，然后生成 ```setup.bin``` 文件。

7. 生成 vmlinux.bin，大小约为 5.6M<br>
将 ```arch/x86/boot/compressed/vmlinux``` 去除 ELF header、note 等信息生成 ```arch/x86/boot/vmlinux.bin``` 。

8. 生成最终的 bzImage，大小约为 5.6M<br>
当 ```setup.bin``` 和 ```vmlinux.bin``` 都生成之后，通过 ```arch/x86/boot/tools/build``` 生成最终的 bzImage，该文件位于 ```arch/x86/boot``` 目录下。

因此，bzImage 组成结构：

{% highlight text %}
bzImage       : bootsect+setup+vmlinux.bin
vmlinux.bin   : head.S+misc.c+vmlinux.bin.gz
vmlinux.bin.gz: gzip 压缩的 vmlinux.bin 即最终的内核
{% endhighlight %}



## 参考

关于 KBuild 可以参考 [你知道 Linux 内核是如何构建的吗？](http://www.open-open.com/news/view/dfa811) 或者参考 [本地文档](/reference/linux/kernel/how_linux_kernel_build.maff)；以及介绍 ARM bzImage 生成过程 [zImage 的生成和加载](/reference/linux/kernel/how_zimage_generate.maff) 。

对于 KBuild 可以参考本地文档 [KBUILD系统原理分析](/reference/linux/kernel/KBUILD_system.pdf) 。

<!--
内核映像的形成 —— KBuild体系，介绍 Linux 的 Blog ，一系列文章，云松的文章。
http://blog.csdn.net/yunsongice/article/category/759408

Linux 内核 Makefile 体系简单分析
http://blog.chinaunix.net/uid-20543672-id-3065852.html
-->

{% highlight text %}
{% endhighlight %}
