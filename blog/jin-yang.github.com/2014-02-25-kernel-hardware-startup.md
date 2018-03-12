---
title: Linux 硬件启动
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,内核,kernel,编译,启动流程
description: 从内核加电之后，简单介绍如何从硬件加载启动。
---

从内核加电之后，简单介绍如何从硬件加载启动。

<!-- more -->

## Bootstrap

这里的 [Bootstrap](http://en.wiktionary.org/wiki/bootstrap) 通常是指 "鞋子后边的背带"，通常也表示不需要别人的 帮助就能够自己处理事情的情形。这来自于一句常用的谚语 ```"pull oneself up by one's bootstraps"``` 。

这句话最早出现于 《The Surprising Adventures of Baron Munchausen》，这本书里的一个故事：主人公 Baron Munchausen 不小心掉进了一片沼泽，他通过自己的 bootstraps 将自己拉了出来，Magic :-)

其实在 19 世纪初美国也有 ```"pull oneself over a fence by one's bootstraps"``` 的说法，意思是 "做荒谬不可能完成的事情" 。可参考 [Bootstrapping](http://en.wikipedia.org/wiki/Bootstrapping) 。

最早的时候，工程师们用它来比喻计算机启动是一个很矛盾的过程：必须先运行程序，然后计算机才能启动，但是计算机不启动就无法运行程序！

早期真的是这样，必须想尽各种办法，把一小段程序装进内存，然后计算机才能正常运行。所以，工程师们把这个过程叫做"拉鞋带"，久而久之就简称为 boot 了。

接下来主要介绍的是现在最常用的 PC 启动方法，也即采用 BIOS 启动，通常还有其它的启动方式，如 EFI、UEFI 等。

> 可扩展固件接口 (Extensible Firmware Interface，EFI) 是 Intel 为全新类型的 PC 固件的体系结构、接口和服务提出的建议标准。主要目的是为了提供一组在 OS 加载之前（启动前）在所有平台上一致的、正确指定的启动服务，被看做是有近 20 多年历史的 PC BIOS 的继任者。
>
> 统一的可扩展固件接口 (Unified Extensible Firmware Interface，UEFI)，是一种详细描述全新类型接口的标准，由 EFI 1.10 为基础发展起来的，它的所有者已不再是 Intel，而是一个称作 Unified EFI Form 的国际组织，是 open source 的。这种接口用于操作系统自动从预启动的操作环境，加载到一种操作系统上，从而使开机程序化繁为简，节省时间。

## 主板介绍

在介绍之前首先了解一下主板的情况。现在主板大致如下所示，在主板上主要有两大主要部分：北桥 (North Bridge 或 Host Bridge) 和南桥 (South Bridge) 。

北桥主要负责 CPU 和内存、显卡这些部件的数据传送，而南桥主要负责 I/O 设备、外部存储设备以及 BIOS 之间的通信。现在有些芯片厂商为了加快数据的传输速度，已经把北桥所负责的功能直接集成到 CPU 中了，因此有些主板已经没有北桥了。

![hardware materboard diagram]({{ site.url }}/images/linux/kernel/hardware_motherboard_diagram.png "hardware materboard diagram"){: .pull-center }

CPU 通过管脚与外界相连，但是它不知道与之相连的是什么，可能是主板、路由、测试工具等。 CPU 主要通过三种方式与外界通讯：内存地址空间、I/O地址空间和中断。在主板中， CPU 通过前端总线 (Front Side Bus) 与北桥相连，其中包括了地址总线和数据总线。对于 Intel Core 2 QX6600 而言，有 33 针地址总线，64 针数据总线，因此 CPU 可访问的地址空间有 64GB = 2^33 * 8B ，实际使用只是其中一部分。

在访问上述的地址空间时，需要通过北桥与其他设备进行通讯，这种通讯方式称为内存映射 ( [Memory Mapped I/O, MMIO](http://en.wikipedia.org/wiki/Memory-mapped_IO) ) 。若要使 MMIO 空间可用于 32 位操作系统，MMIO 空间必须驻留在前 4GB 地址空间内。这些地址空间一部分会映射到 RAM ，还有其他的设备，如显卡、PCI 设备、BIOS 等。在 Linux 中，可以通过 /proc/iomem 查看，其分布大致如下。

![hardware materboard diagram]({{ site.url }}/images/linux/kernel/hardware_memory_layout.png "hardware materboard diagram"){: .pull-center }

### 启动流程

操作系统启动大致过程如下图所示，总共经过了四步：BIOS、MBR、Bootloader 和 操作系统初始化。

![hardware boot step]({{ site.url }}/images/linux/kernel/hardware_boot_steps.png "hardware boot step"){: .pull-center }

## 第一阶段：BIOS

当按下开关时主板会按照一定的顺序启动各个芯片，当然不同的主板启动的顺序会不一样。刚开始时电压还不太稳定，主板上的控制芯片组会向 CPU 发出并保持一个 RESET 信号，让 CPU 内部自动恢复到初始状态，此时 CPU 在此刻不会马上执行指令。当芯片组检测到电源已经开始稳定供电了，它便撤去 RESET 信号（如果是手工按下计算机面板上的 Reset 按钮来重启机器，那么松开该按钮时芯片组就会撤去 RESET 信号），然后 CPU 开始运行。

![hardware bios hardware]({{ site.url }}/images/linux/kernel/hardware_bios_hardware.jpg "hardware bios hardware"){: .pull-center }

> 上个世纪70年代初，"只读内存" (read-only memory，ROM) 发明，开机程序被刷入 ROM 芯片，计算机通电后，第一件事就是读取它。这块芯片里的程序叫做 "基本输入输出系統" (Basic Input/Output System) ，简称为 [BIOS](http://en.wikipedia.org/wiki/BIOS) 。

如果是多处理器，通常会动态选择一个 Bootstrap Processors(BSP) ，该 CPU 会执行所有的 BIOS 和内核初始化函数。其他的处理器 Application Processors (AP) 会一直等到内核明确激活之后才会运行。

> 在 8086 的那个时代，采用的是实模式(Real Mode)， CPU 寻址时采用 Segment:Offset 模式，物理地址为 Address = Segment * 16 + Offset ，此时段的大小固定为 2^16=64K ，最大的寻址为 0x00000 ~ 0xFFFFF ，即 1MBytes 。
>
> 80386 之后产生了保护模式 (Protect Mode) ，虚拟地址到物理地址转换时需要通过两步进行，即段机制 + 页机制，其中段转换机制是必须的。段机制在转换时采用段描述符，使用 64bits 而不是 16bits 来描述一个段，包括了段基址、段长、属性等，此时段的大小是可变的。
>
> 在保护模式中，可以单独使用段机制；也可以使用页机制，此是会分为两步，首先经过段机制然后是分页机制。
>
> DMI/QPI (Quick Path Interconnect，快速通道互联)，是 Intel 用来取代 FSB 的新一代高速总线，对于 CPU 与 CPU 之间或者 CPU 与北桥芯片之间的通讯都可以使用 QPI 相连。

在 8086 初始化时 ```CS:IP=F000h:FFF0h``` ， 因此将会从 ```FFFF0h``` 开始执行。 80386 之后采用了一种 hack 的方式 (与实模式和保护模式都不太一样) ，此时 ```CR0:PE``` 没有置 1 ，因此不是保护模式，该模式被称为 Big Real Mode。

在初始化时 EIP 仍然为 ```0000FFF0h```， CS 的 ```Selector=F000h```， ```Base=FFFF0000h```， ```Limit=FFFFh```， ```AR=Present, R/W, Accessed``` ，此时 GTD 还没有建立，Selector 没有使用，相应的读取地址为 ```CS:Base+EIP=FFFFFFF0h``` 。

接下来不同的厂商做法就不太一样了，主要是 Intel 设计的 EFI(Extensible Firmware Interface) 的做法和传统的 Legacy BIOS 。

* Legacy BIOS<br>放在 0xFFFFFFF0 的第一条指令一般是一个远跳转指令 (Far Jump) ，也就是说 CPU 在执行 Legacy BIOS 时，会直接从 0xFFFFFFF0 跳回 F Segment ，回到 1MB 以下这个 Legacy BIOS 的老巢里去。此时重装 CS 后，上述的 hack 将无效，也就是到了 8086 时的实模式。此时可以访问的地址范围还和 A0 有关，没有详细了解 :(

* EFI BIOS<br>第一条指令是 wbinvd(清除 CPU 高速缓存)，之后做一些设定之后，会直接进入保护模式。所以 EFI BIOS 是从南桥 Region 4G 通过，并不需要 Legacy Region 。

下面仅对 Legacy BIOS 进行介绍。

对于传统的 “CPU + 北桥 + 南桥” 类型的主板来说，CPU 的地址请求通过 FSB (Front Side BUS, 前端总线) 到达北桥，北桥通过判断这个地址范围在南桥，因此将这个请求送到南桥。

对于最新的主板芯片组来说，北桥和 CPU 封装在一颗芯片里面，所以会看到这个请求通过 DMI/QPI 被送到南桥。请求到达南桥后，南桥根据目前的地址映射表的设置决定是否将请求转发到 SPI(Serial Peripheral Interface) 或者 LPC(Low Pin Count)。

简单说就是南桥芯片拥有一张地址映射表，当有地址解析的请求到来时，南桥查看这张表决定将地址解析到何处去。这张表里有两个特殊区域，一个是从地址空间 4G 向下，大小从 4MB 到 16MB 不等的一个区域，我们以 4MB 为例，地址空间从 ```FFFC00000h~FFFFFFFFh``` 。称之为 Range 4G 。第二个区域一般是是从 1MB 向下 128KB 的范围，即 E Segment 和 F Segment ，从 ```E0000~FFFFF``` ，称之为 Legacy Range ，也就是说， ```FFFC00000h~FFFFFFFFh``` 之间和 ```E0000~FFFFF``` 之间的寻址请求都会被导向到 SPI/LPC ，最终指向了 BIOS 。

### 硬件自检 POST (Power On Self Test)

BIOS 程序首先检查，计算机硬件能否满足运行的基本条件，这叫做"硬件自检"（Power On Self Test），缩写为 [POST](http://en.wikipedia.org/wiki/Power-on_self-test) 。

POST 的主要任务是检测系统中的一些关键设备 (电源、CPU 芯片、BIOS 芯片、定时器芯片、数据收发逻辑电路、DMA 控制 器、中断控制器以及基本的 640K 内存和内存刷新电路等) 是否存在和能否正常工作。

由于 POST 是最早进行的检测过程，此时显卡还没有初始化，如果系统 BIOS 在进行 POST 的过程中发现了一些致命错误，例如没有找到内存或者内存有问题 （此时只会检查640K常规内存），那么系统 BIOS 就会直接控制喇叭发声来报告错误，声音的长短和次数代表了错误的类型。

<!--
接着系统 BIOS 将查找显示卡的 BIOS ，由显卡 BIOS 来完成显示卡的初始化，存放显卡 BIOS 的 ROM 芯片的起始地址通常设在 C0000H 处，系统 BIOS 在这个地方找到显卡 BIOS 之后就调用它的初始化代码，由显卡 BIOS 来初始化显卡，此时多数显卡都会在屏幕上显示出一些初始化信息，介绍生产厂 商、图形芯片类型等内容，不过这个画面几乎是一闪而过。自此，系统就具备了最基本的运行条件，可以对主板上的其它部分进行诊断和测试。再发现故障时，屏幕上会有提示，但一般不死机。

系统 BIOS 接着会查找其它设备的 BIOS 程序，找到之后同样要调用这些 BIOS 内部的初始化代码来初始化相关的设备。 查找完所有其它设备的 BIOS 之后，系统 BIOS 将显示出它自己的启动画面，其中包括有系统 BIOS 的类型、序列号和版本号等内容。

接着，系统 BIOS 将检测 CPU 的类型和工作频率，然后开始测试主机所有的内存容量，并同时在屏幕上显示内存测试的进度，我们可以在 CMOS 设置中自行决定使用简单耗时少或者详细耗时多的测试方式。

内存测试通过之后，系统 BIOS 将开始检测系统中安装的一些标准硬件设备，这些设备包括：硬盘、CD－ROM、软驱、串行接口和并行接口等连接的设备，大多数新版本的系统 BIOS 在这一过程中还要自动检测和设置内存的相关参数、硬盘参数和访问模式等。

标准设备检测完毕后，系统 BIOS 内部的“支持即插即用的代码”将开始检测和配置系统中已安装的即插即用设备。每找到一个设备之后，系统 BIOS 都会在屏幕上显示出设备的名称和型号等信息，同时为该设备分配中断、DMA 通道和 I/O 端口等资源。

到这一步为止，所有硬件都已经检测配置完毕了，多数系统 BIOS 会重新清屏并在屏幕上方显示出一个表格，其中概略地列出了系统中安装的各种标准硬件设备，以及它们使用的资源和一些相关工作参数。
-->

接下来系统 BIOS 将更新 ESCD（Extended System Configuration Data，扩展系统配置数据）。ESCD 是系统 BIOS 用来与操作系统交换硬件配置信息的一种手段，这些数据被存放在 CMOS (一小块特殊的RAM，由主板上的电池来供电) 之中。

<!--
通常 ESCD 数据只在系统硬件配置发生改变后才会更新，所以不是每次启动机器时我们都能够看到 “Update ESCD… Success” 这样的信息，不过，某些主板的系统 BIOS 在保存 ESCD 数据时使用了与 Windows 9x 不相同的数据格式，于是 Windows 9x 在它自己的启动过程中会把 ESCD 数据修改成自己的格式，但在下一次启动机器时，即使硬件配置没有发生改变，系统 BIOS 也会把 ESCD 的数据格式改 回来，如此循环，将会导致在每次启动机器时，系统 BIOS 都要更新一遍 ESCD ，这就是为什么有些机器在每次启动时都会显示出相关信息的原因。
-->

ESCD 数据更新完毕后，系统 BIOS 的启动代码将进行它的最后一项工作，即根据用户指定的启动顺序从软盘、硬盘或光驱启动。

<!--
上面介绍的便是计算机在打开电源开关 (或按 Reset 键) 进行冷启动时所要完成的各种初始化工作，如果我们在 DOS 下按 Ctrl＋Alt＋Del 组合键 (或从 Windows 中选择重新启动计算机) 来进行热启动，那么 POST 过程将被跳过去，直接从第三步开始，另外第五步的检测 CPU 和内存测试也不会再进行。
-->

![hardware bios post]({{ site.url }}/images/linux/kernel/hardware_bios_post.png "hardware bios post"){: .pull-center }


### 启动设备顺序

硬件自检完成后，BIOS 把控制权转交给下一阶段的启动程序。

这时，BIOS 需要知道，"下一阶段的启动程序" 具体存放在哪一个设备。也就是说，BIOS需要有一个外部储存设备的排序，排在前面的设备就是优先转交控制权的设备。这种排序叫做"启动顺序" (Boot Sequence)。

打开 BIOS 的操作界面，里面有一项就是"设定启动顺序"。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_bios_bootseq.jpg "hardware bios bootseq"){: .pull-center }

此时， POST 完成了一系列测试和初始化，并将所有的资源整理，包括了中断、RAM 范围、I/O 端口等。现在的很多 BIOS 会根据 <a href="http://en.wikipedia.org/wiki/ACPI">Advanced Configuration and Power Interface (ACPI)</a> 保存一系列设备列表，这些列表会被 Kernel 使用。


## 第二阶段：主引导记录

BIOS 按照"启动顺序"，把控制权转交给排在第一位的储存设备。这时，计算机读取该设备的 ["主引导记录"](http://en.wikipedia.org/wiki/Master_boot_record) (Master boot record，缩写为 MBR) ，对于硬盘是第一个扇区 (0头0道1扇区，也就是 Boot Sector)，也即读取存储设备最前面的 512 个字节，读入内存地址 ```0000:7c00~0000:7e00(+200)``` 处。

如果这 512 个字节的最后两个字节是否为 0x55 和 0xAA (即检查 (WORD)0000:7dfe 是否等于 0xaa55) ，表明这个设备可以用于启动，于是跳转到 0x7C00 并执行；如果不是，表明设备不能用于启动，控制权于是被转交给"启动顺序"中的下一个设备。

> 硬盘有很多盘片组成，每个盘片的每个面都有一个读写磁头，如果有 N 个盘片，则有 2N 个面，对应 2N 个磁头 (Heads) 。每个盘片以电机为圆心，半径为固定值 R 的同心圆，在逻辑上形成了一个柱面 (Cylinders) 。每个盘片上的每个磁道又分为几个扇区 (Sector) ，通常每个扇区的容量为 512Bytes 。因此总共 Heads×Cylinders×Sector 个扇区，其编号均从 0 开始。
>
> 在读取第一个扇区的时候， BIOS 是通过 INT 13 读取。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_linux_boot_hard.jpg "hardware bios bootseq"){: .pull-center }

最终的结果是， POST 之后会跳转到 **0000:7c00** 处执行 MBR 中的程序，将控制权交给 MBR 中的 Bootloader 程序(通常为grub)；以上是由 BIOS 的引导程序完成；下面是由 MBR 中的引导程序完成。

### 主引导记录

每个操作系统的 Bootloader 通常不一样，因此，在硬盘中，每个文件系统或者分区都会保留一个引导扇区 (Boot Sector) 来为操作系统安装 Bootloader 。其格式大致如下。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_mbr_boot_sector.gif "hardware bios bootseq"){: .pull-center }

> 每个操作系统在安装时会将 Bootloader 安装到该分区的 Boot Sector 中去。在 Linux 中可以选择是否安装到 MBR 中，如果是，会在 MBR 和分区的 Boot Sector 中都保留一份 Bootloader 。而 Windows 则会主动将 MBR 和 Boot Sector 安装上 Bootloader 。
>
> Linux 的 Bootloader 在安装时可以自动搜索其它的 Bootloader ，而 Windows 的却不可以，因此在安装双系统时，最好先安装 Windows 然后安 Linux 。

主引导扇区 MBR 和操作系统无关，单就功能来说可以将其设置为一个通用的模块。其主要功能是用来查找活动分区，并将控制权交给相应分区的引导扇区 (第一个扇区) ，也即告诉计算机到硬盘的哪一个位置去找操作系统。

该扇区的是和操作系统有关的，操作系统的引导是由它来完成的，而 MBR 并不负责，MBR 和操作系统无关。引导扇区的任务是把控制权转交给操作系统的引导程序。

如果使用多系统那么 MBR 通常也就是 Grub 或这 Lilo 的一部分程序，在安装时存放在 MBR。Bootloader 中的 MBR 主要操作步骤为：

1. MBR 先将自己的程序代码由 **0000:7C00h 移动到 0000:0600h** ，然后继续执行。(注，BIOS 把 MBR 放在 0000:7C00h处)。

2. 扫描分区表，找到一个激活(可引导)分区。在主分区表中搜索标志为活动的分区，即 80h 标志(详见分区表，通常为boot分区)。如果发现没有活动分区或者不止一个活动分区，则停止，通常为失败：跳入 ROM BASIC、无效分区表：```goto 5 <1>```。

3. 找到激活分区的起始扇区；读引导扇区(Boot Sector)，将活动分区的第一个扇区读入内存地址 0000:7c00 处。失败：goto 5。注意: 此时读取的是此扇区的第一个 Sector ，而非 MBR 。

4. 验证引导扇区最后是否为 55AAh，即检查(WORD)0000:7dfe 是否等于 0xaa55，若不等于则 ```goto 5 <2/4>``` 。

5. 打印错误进入无穷循环，通常的错误有：
   1. No active partition.
   1. Invalid partition table.
   1. Error loading operating system.
   1. Missing operating system.

6. 跳转到 0000:7c00 处继续执行特定系统的启动程序，将控制权交给引导扇区代码，启动系统。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_mster_boot_record_0.png "hardware bios bootseq"){: .pull-center }

另一种表示方法，

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_master_boot_record_1.png "hardware bios bootseq"){: .pull-center }

如上图所示主引导记录由三个部分组成。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_marster_records.png "hardware bios bootseq"){: .pull-center }

<!--
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="2" cellpadding="3" cellspacing="0" width="70%"><tbody>
<tr bgcolor="lightblue"><td align="center">地址</td><td align="center">注释</td><td align="center">格式</td></tr>
<tr><td align="center">0000-01BD</td><td>Master Boot Record，Bootloader 机器码，446 个字节</td><td>程序</td></tr>　
<tr><td align="center">01BE-01CD</td><td>分区项1，16 个字节</td><td rowspan="4">分区表(Partition Table)</td></tr>　
<tr><td align="center">01CE-01DD</td><td>分区项2，16 个字节</td></tr>　
<tr><td align="center">01DE-01ED</td><td>分区项3，16 个字节</td></tr>　
<tr><td align="center">01EE-01FD</td><td>分区项4，16 个字节</td></tr>
<tr><td align="center">01FE</td><td>0x55</td><td rowspan="2">结束标志</td></tr>　
<tr><td align="center">01FF</td><td>0xAA</td></tr>
</tbody></table></center><br>
-->

其中，第二部分"分区表"的作用，是将硬盘分成若干个区。

### 分区表

硬盘分区有很多 [分区](http://en.wikipedia.org/wiki/Disk_partitioning) ，考虑到每个分区可以安装不同的操作系统，因此 "主引导记录MBR" 必须知道将控制权转交给哪个区。

分区表的长度只有64个字节，里面又分成四项，每项 16 个字节。所以，一个硬盘最多只能分四个一级分区，又叫做"主分区"。

每个主分区的 16 个字节，由 6 个部分组成：

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_marster_records_details.png "hardware bios bootseq"){: .pull-center }

<!--
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="2" cellpadding="3" cellspacing="0" width="70%"><tbody>
<tr bgcolor="lightblue"><td align="center" width="75">地址</td><td align="center">注释</td></tr>
<tr><td align="center">0</td><td>0x80 - 活动分区，0x00 - 非活动分区；控制权会交给活动分区。四个主分区里面有且只只能有一个是活动分区。</td></tr>　
<tr><td align="center">1-3</td><td>主分区第一个扇区的物理位置，磁头(第1个字节)、扇区号(第2字节低6位)、柱面(第2字节高2位+第3字节)。</td></tr>　
<tr><td align="center">4</td><td><a href="http://en.wikipedia.org/wiki/Partition_type"> 主分区类型 </a>：
<ul><li>
    00h 该分区未使用。</li><li>
    05h 扩展分区。</li><li>
    06h FAT16 基本分区。</li><li>
    07h NTFS 分区。</li><li>
    0Bh FAT32 基本分区。</li><li>
    0Fh 扩展分区。
</li></ul></td></tr>　
<tr><td align="center">5-7</td><td>主分区最后一个扇区的物理位置，磁头(第1个字节)、扇区号(第2字节低6位)、柱面(第2字节+第3字节)。</td></tr>　
<tr><td align="center">8-11</td><td>该分区起始扇区号。</td></tr>
<tr><td align="center">12-15</td><td>该分区的扇区总数。</td></tr>　
</tbody></table></center><br>
-->

最后的四个字节 ("主分区的扇区总数")，决定了这个主分区的长度。也就是说，一个主分区的扇区总数最多不超过 2 的 32 次方。

如果每个扇区为 512 个字节，就意味着单个分区最大不超过 2TB 。再考虑到扇区的逻辑地址也是 32 位，所以单个硬盘可利用的空间最大也不超过 2TB 。如果想使用更大的硬盘，只有 2 个方法：一是提高每个扇区的字节数，二是增加 [扇区总数](http://en.wikipedia.org/wiki/GUID_Partition_Table) 。

> 硬盘的分区规则：一个分区的所有扇区必须连续，硬盘可以有最多 4 个物理上的分区，这 4 个物理分区可以时个主分区或者 3 个主分区加 1 个扩展分区，只能由一个扩展分区。在扩展分区里，可以而且必须再继续划分逻辑分区(逻辑盘)，可以有任意多个。
>
> 扩展分区的信息位于硬盘分区表(DPT)中，而逻辑驱动器的信息则位于扩展分区的起始扇区，即该分区的起始地址(面/扇区/磁道)所对应的扇区，该扇区中的信息与硬盘主引导扇区的区别是不包含 MBR ，而 16 字节的分区信息则表示的是逻辑驱动器的起始和结束地址等。

对于主引导扇区最多可以识别 4 个主分区，为了可以使用更多的分区，引入了扩展分区。扩展分区中的逻辑驱动器引导记录采用链式结构。每一个逻辑分区都有一个和 MBR 分区表类似的扩展引导记录 EBR 。该分区表的第一项指向该逻辑分区本身的引导扇区，第二项指向下一个逻辑驱动器的 EBR ，其格式如下所示。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_ebr_1.gif "hardware bios bootseq"){: .pull-center } ![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_ebr_2.gif "hardware bios bootseq"){: .pull-center }

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_ebr_3.png "hardware bios bootseq"){: .pull-center }

<!--
<center><a href="http://en.wikipedia.org/wiki/Extended_boot_record">扩展引导记录</a><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="2" cellpadding="3" cellspacing="0" width="70%"><tbody>
<tr bgcolor="lightblue"><td align="center">地址</td><td align="center">注释</td><td align="center">格式</td></tr>
	<tr><td align="center">0000-01BD</td><td>通常没有使用，一般为 0 。</td><td>空闲</td></tr>　
	<tr><td align="center">01BE-01CD</td><td>分区项1，16 个字节。本扩展分区。</td><td rowspan="4">分区表(Partition Table)</td></tr>　
	<tr><td align="center">01CE-01DD</td><td>分区项2，16 个字节。下一个扩展分区。</td></tr>　
	<tr><td align="center">01DE-01ED</td><td>分区项3，16 个字节。未使用。</td></tr>　
	<tr><td align="center">01EE-01FD</td><td>分区项4，16 个字节。未使用。</td></tr>
	<tr><td align="center">01FE</td><td>0x55</td><td rowspan="2">结束标志</td></tr>　
	<tr><td align="center">01FF</td><td>0xAA</td></tr>
</tbody></table></center>
-->

### MBR 操作

在 Linux 中可以通过如下的命令对 MBR 进行操作，包括备份、删除等。

{% highlight text %}
# fdisk -l                                   // 查看硬盘，假设为 /dev/sda
# dd if=/dev/sda of=mbr.bak bs=512 count=1   // 备份 MBR
# dd if=mbr.bak of=/dev/sda bs=512 count=1   // 恢复 MBR
# dd if=/dev/zero of=/dev/sda bs=446 count=1 // 删除 MBR

# vim mbr.bak -b                             // 以二进制打开
:%!xxd                                       // 切换为十六进制显示
{% endhighlight %}

## 第三阶段：硬盘启动

这时，计算机的控制权就要转交给硬盘的某个分区了，这里又分成三种情况。

### 情况A：卷引导记录

上一节提到，四个主分区里面，只有一个是激活的。计算机会读取激活分区的第一个扇区，叫做 [卷引导记录](http://en.wikipedia.org/wiki/Volume_Boot_Record) (Volume boot record，缩写为VBR) 。

"卷引导记录"的主要作用是，告诉计算机，操作系统在这个分区里的位置。然后，计算机就会加载操作系统了。

### 情况B：扩展分区和逻辑分区

随着硬盘越来越大，四个主分区已经不够了，需要更多的分区。但是，分区表只有四项，因此规定有且仅有一个区可以被定义成"扩展分区" (Extended partition)。

所谓"扩展分区"，就是指这个区里面又分成多个区。这种分区里面的分区，就叫做"逻辑分区" (logical partition)。

计算机先读取扩展分区的第一个扇区，叫做 [扩展引导记录](http://en.wikipedia.org/wiki/Extended_partition) （Extended boot record，缩写为EBR）。它里面同样也包含一张 64 字节的分区表，但是只有前两项是有效的，分别指向当前分区的磁盘地址，以及下一个逻辑分区的磁盘地址，从而形成了一个有序的链表。直到某个逻辑分区的分区表只包含它自身为止 (即只有一个分区项)。因此，扩展分区可以包含无数个逻辑分区。<br><br>

但是，似乎很少通过这种方式启动操作系统。如果操作系统确实安装在扩展分区，一般采用下一种方式启动。

### 情况C：启动管理器

在这种情况下，计算机读取"主引导记录"前面 446 字节的机器码之后，不再把控制权转交给某一个分区，而是运行事先安装的 [启动管理器](http://en.wikipedia.org/wiki/Boot_loader) (boot loader)，由用户选择启动哪一个操作系统。

Linux环境中，目前最流行的启动管理器是 [Grub](http://en.wikipedia.org/wiki/GNU_GRUB) 。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_boot_grub.png "hardware bios bootseq"){: .pull-center }

### X86 Bootloader 设计

Linux 运行在保护模式下，但是当机器启动复位的时候却处于实模式下，所以 Bootloader 做的工作也是在实模式之下的，使用低端的 640k 内存。

早期的磁盘，每个磁道有 63 个扇区，硬盘在使用中，0 面的 0 磁道的所有扇区即前 63 个扇区做为特殊用途，BIOS 和操作系统不放置任何数据于此，一般做为 bootloader 使用，这也是为什么 GRUB 必须使用 stage1.5 的原因（因为其大小一般为 100KB 左右，超出了 62 个扇区的限制）。

<!--
grub 引导的 linux 的过程不太清楚，有待添加。？？？？？？？？？？？？？？？？
2)       BIOS -> MBR(GRUB2 boot.img)
此场景中MBR引导代码即GRUB2的boot/i386/pc/boot.S编译出来的代码，称为boot.img；代码执行位置即0x7C00，主要行为：设置堆栈指针为0x2000处，探测硬盘CHS/LBA工作模式，并加载第二个扇区最终至0x8000处，并跳至0x8000执行；
-->

### Grub Stage2

当 Grub-Stage2 被载入内存执行时，它首先会去解析 grub 的配置文件 ```/boot/grub/grub.conf``` ，然后加载内核镜像到内存中，并将控制权转交给内核。控制权转交给操作系统后，操作系统的内核首先被载入内存。

bootloader 执行完成后内存的分布如下图所示，详见 [Documentation/i386/boot.txt]({{ site.kernel_docs_url }}/Documentation/i386/boot.txt) Linux-x86 的启动协议。

![hardware bios bootseq]({{ site.url }}/images/linux/kernel/hardware_memory_after_bootloader.png "hardware bios bootseq"){: .pull-center }

现在 Linux 的镜像已经加载到了内存中，主要包括了两部分：A) 运行在实模式中，加载到了 640K 以下的内存；B) 运行在保护模式的内核，加载到了 1M+ 。

<!--
通常从系统上电到运行到linux kenel这部分的任务是由boot loader来完成。Boot loader在跳转到kernel之前要完成一些限制条件:
1、CPU必须处于SVC(supervisor)模式,并且IRQ和FIQ中断必须是禁止的；
2、MMU(内存管理单元)必须是关闭的, 此时虚拟地址对应物理地址；
3、数据cache(Data cache)必须是关闭的；
4、指令cache(Instruction cache)没有强制要求；
5、CPU通用寄存器0(r0)必须是0;
6、CPU通用寄存器1(r1)必须是ARM Linux machine type；
7、CPU通用寄存器2(r2)必须是kernel parameter list的物理地址；
-->

上图所示的 real-mode kernel header 之上的内容在 Linux boot protocol 中定义，主要用来 Bootloader 和 Linux Kernel 之间进行通讯。其中包括了 Bootloader 运行时写入的内容，如果内核版本、内核参数的地址等。当完成了一系列初始化之后，那么就会跳转到 Linux Kernel 中执行。

首先加载的是 setup.bin，其中包括了开始的 512(0x200)bytes 的 bootsector 用于兼容之前的版本，正常应该不会执行的，如果执行会输出错误信息。可以通过 vim+%!xxd 打开 setup.bin 文件查看，0x200 处为 eb 也就是一个调转指令，也就是入口 _start。

其实在 GRUB 等 bootloader 将内核的 setup.bin 读到 0x90000 处，将 vmlinux 读到 0x100000 处(grub拷贝时会打开保护模式,然后关闭)，然后跳转到 0x90200 开始执行，恰好跳过了 512 字节的 bootsector。

那么接下来就将 CPU 的执行权限交给了 Linux 。




## 参考

介绍开关键按下到 BIOS 启动，含有硬件的介绍，可以参考 [CPU Reset](/reference/linux/kernel/CPU_Reset.pdf)；以及 [Big Real Mode 的介绍](/reference/linux/kernel/Introduction_to_Big_Real_Mode.pdf) 。

[All the Details of many versions of both MBR and OS Boot Records](http://thestarman.pcministry.com/asm/mbr/index.html) 对各个版本 MBR 的介绍，含有 Windows 下的工具，十分详细。

<!--
<a href="http://www.ibm.com/developerworks/cn/linux/embed/l-bootloader/"> Linux bootloader 编写方法 [Web] </a> IBM 介绍如何编写 Bootloader </li><li>
<a href="reference/lfs/linux_x86_boot_protocol.org.txt"> THE LINUX/x86 BOOT PROTOCOL [local] </a> 原版，摘自 Linux-3.4.48 源码 Documentation/x86/boot.txt </li><li>
<a href="reference/lfs/linux_x86_boot_protocol.txt"> THE LINUX/x86 BOOT PROTOCOL [local]</a> 中文翻译版</li><li>
<a href="http://blog.csdn.net/yunsongice/article/category/759408">系统初始化 [Web]</a> 内核编译、初始化过程，以及其它的一些操作，介绍比较详细
-->


{% highlight text %}
{% endhighlight %}
