---
title: Linux 启动过程
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,内核,kernel,编译,启动流程
description: 现在的 Linux 启动过程一般分成了两步，也就是首先调用 GRUB 作为通用的启动服务，然后可以选择 Windows 或者 Linux 加载。下来，看看 Linux 的加载过程。
---

现在的 Linux 启动过程一般分成了两步，也就是首先调用 GRUB 作为通用的启动服务，然后可以选择 Windows 或者 Linux 加载。

接下来，看看 Linux 的加载过程。

<!-- more -->

## 启动加载

Bootloader 执行完之后，将内核加载到物理地址 ```0x100000(1MB)``` 处，并将执行权限交给了内核。然后执行的是 ```setup.bin``` 的入口函数 ```_start()``` ( ```arch/x86/boot/header.S```，链接文件为 ```setup.lds``` ) 。

第一条是一个调转指令，调转到 ```start_of_setup``` (```start_of_setup-1f```是setup头部的长度)，在这之间的代码是一个庞大的数据结构，与 ```bootparam.h``` 中的 ```struct setup_header``` 一一对应。这个数据结构定义了启动时所需的默认参数，其中一些参数可以通过命令选项 overwrite。很多值是在形成 bzImage 时，由 ```arch/x86/boot/tools/build``` 程序赋值。

接着实际是设置 C 语言环境，主要是设置堆栈，然后调转到 ```main()@arch/x86/boot/main.c```，现在仍然运行在实模式。```setup.bin``` 主要作用就是完成一些系统检测和环境准备的工作，其函数调用顺序为：

{% highlight text %}
_start()@arch/x86/boot/header.S
  |-start_of_setup()@arch/x86/boot/header.S
    |-main()@arch/x86/boot/main.c
      |-copy_boot_params()                        // 将进行参数复制
      |-detect_memory()                           // 探测物理内存，第一次与内存相关
      |-go_to_protect_mode
        |-realmode_switch_hook()                  // 如果boot_params.hdr.realmode_swtch有非空的hook函数地址则执行之
        |-enable_a20()
        |-reset_coprecessor()                     // 重启协处理器
        |-protected_mode_jump()@arch/x86/boot/pmjump.S   // 进入 32-bit 模式
{% endhighlight %}

main.c 主要功能为检测系统参数如: ```Detect memory layout```，```set video mode``` 等，后面会详细分析，最后调用 ```goto_protect_mode```，设置 32-bit 或 64-bit 保护段式寻址模式，注意: ```main.o``` 编译为16位实模式程序。

> 内核的日志信息可以通过 printk() 打印，不过只有信息等级小于 console loglevel 的值时，这些信息才会被打印到 console 上，可以通过如下方法修改日志等级。
>
> 1. 修改内核启动时的参数选项，loglevel=level。
> 2. 运行时执行如下命令 dmesg -n level 或者 echo $level > /proc/sys/kernel/printk 。
> 3. 写程序使用 syslog 系统调用。

```goto_protect_modea()``` 则会调用 ```protected_mode_jump()```，进入 32-bit 的兼容模式，也就是 IA32-e 模式，在最后会执行 ```jmpl *%eax``` 调转到 32-bits 的入口，也就是 ```0x100000``` 处。

{% highlight text %}
startup_32()
{% endhighlight %}

可以看到 ```arch/x86/boot/compressed/head_64.S``` 的入口函数 ```startup_32```，该函数会解压缩 Kernel、设置内存地址转换、进入 64bit 模式。

关于转换的具体步骤，可以参考 ```《Intel 64 and IA-32 Architectures Software Developer’s Manual》``` 中的 ```9.8.5 Initializing IA-32e Mode``` 部分 。

然后会调转到 ```arch/x86/kernel/head_64.S``` 的 ```startup_64``` 。


## 准备环境

这个函数主要是为第一个 Linux 进程 (进程0) 建立执行环境，该函数主要执行以下操作：

<!--
1.  把段寄存器初始化为最终值。
2.  把内核的bss段填充为0。
3.  初始化包含在swapper_pg_dir的临时内核页表，并初始化pg0，以使线性地址一致地映射同一物理地址。
4.  把页全局目录的地址存放在cr3寄存器中，并通过设置cr0寄存器的PG位启用分页。
5.  把从BIOS中获得的系统参数和传递给操作系统的参数boot_params放入第一个页框中。
6.  为进程0建立内核态堆栈。
7.  该函数再一次清零eflags寄存器的所有位。
8.  调用setup_idt用空的中断处理程序填充中断描述符表IDT。
9.  识别处理器的型号。
10. 用编译好的GDT和IDT表的地址来填充gdtr和idtr寄存器。
11. 初始化虚拟机监视器Xen。
12. 向start_kernel()函数进发。
-->

![bootstrap]({{ site.url }}/images/linux/kernel/bootstrap_startup_process.png "bootstrap"){: .pull-center }

最先执行的是 ```arch/x86/boot/header.S``` (由最早的bootsect.S和setup.S修改而来)，一般是汇编语言。最早的时候 Linux 可以自己启动，因此包含了 Boot Sector 的代码，但是现在如果执行的话只会输出 ```"bugger_off_msg"``` ，并重启，该段为 ```.bstext``` 。

现在的 Bootloader 会忽略这段代码。上述无效的 Boot Sector 之后还有 15Bytes 的 Real-Mode Kernel Header ，这两部分总计 512Bytes 。

512 字节之后，也就是在偏移 ```0x200``` 处，**是 Linux Kernel 实模式的入口，也即 _start** 。第一条指令是直接用机器码写的跳转指令 ```(0xeb+[start_of_setup-1])```，因此会直接跳转到 ```start_of_setup``` 。这个函数主要是设置堆栈、清零 bss 段，然后跳转到 ```arch/x86/boot/main.c:main()``` 。

main() 函数主要完成一些必要的清理工作，例如探测内存的分布、设置 Video Mode 等，最后调用 ```go_to_protected_ mode()@arch/x86/boot/pm.c``` 。

### 初始化

在进入保护模式之前，还需要一些初始化操作，最主要的两项是 中断 和 内存。

* 中断<br>在实模式中，中断向量表保存在地址 0 处，而在保护模式中，中断向量表的地址保存在寄存器 IDTR 中。
* 内存<br>实模式和保护模式的从逻辑地址到实地址的转换不同。在保护模式中，需要通过 GDTR 定位全局描述符表(Global Descriptor Table) 。

因此，进入保护模式之前，在 ```go_to_protected_mode()``` 中会通过调用 ```setup_idt()``` 和 ```setup_gdt()``` 创建临时的中断描述符表和全局描述符表。最后通过 ```protected_mode_jump()``` (同样为汇编语言) 进入保护模式。

```protected_mode_jump()``` 会设置 CR0 寄存器的 PE 位。此时，不需要分页功能，因此分页功能是关闭的。最主要的是现在不会受 640K 的限制， RAM 的访问空间达到了 4GB 。

然后会调用 32-bit 的内核入口，即 startup_32 。该函数会初始化寄存器，并通过 decompress_kernel() 解压内核。

```decompress_kernel()``` 输出 ```"Decompressing Linux..."``` ，然后 in-place 解压，解压后的内核镜像会覆盖上图中的压缩镜像。因此，解压后的内容也是从 1MB 开始。解压完成后将会输出 ```"done."```，然后输出 ```"Booting the kernel."``` 。

此时将会跳转到保护模式下的入口处 ```(0x100000)```，该入口同样为 ```startup_32``` ，但与上面的在不同的目录下。

第二个 ```startup_32``` 同样为汇编语言，包含了 32-bit 的初始化函数。包括清零保护模式下的 Kernel bss 段、建立全局描述符、建立中断描述符表，然后跳转到目标相关的启动入口，```start_kernel()``` ，过程如下所示。

![bootstrap protect mode]({{ site.url }}/images/linux/kernel/bootstrap_protected_mode_process.png "bootstrap protect mode"){: .pull-center }

## 正式启动

```start_kernel()@init/main.c``` 的大部分代码为 C ，而且与平台相关。该函数会初始化内核的各个子系统和数据结构，包括了调度器(Scheduler)、内存、时钟等。

然后 ```start_kernel()``` 会调用 ```rest_init()``` ，该函数将会创建一个内核线程，并将 ```kernel_init()``` 作为一个入口传入。

dmesg 中的信息是从 ```start_kernel()``` 之后记录的。

> 像 USB、ACPI、PCI 这样的系统，会通过 subsys_initcall() 定义一个入口，当然还有一些其它的类似入口，可以参考 include/linux/init.h ，实际上时在内核镜像中定义了 .initcall.init 字段 (连接脚本见vmlinux.lds)。
>
> 这些函数会在 do_initcalls() 中调用。

```rest_init()``` 随后会调用 ```schedule()``` 启动任务调度器，并通过 ```cpu_idle()``` 进入睡眠，这是 Linux 中的空闲线程。当没有可运行的进程时，该线程会调用，否则运行可运行的线程。

此时，之前启动的线程将会替代进程0(process 0) 即空闲线程。```kernel_init()``` 将会初始化其他的 CPUs ，在此之前，这些 CPUs 并没有运行。负责初始化的 CPU 被称为 boot processor ，此时启动的 CPUs 被称为 application processors ，这些 CPUs 同样从实模式启动，因此也需要类似的初始化。

最后 ```kernel_init()``` 调用 ```init_post()``` ，该函数将会尝试执行一个用户进程，执行顺序如下 ```/sbin/init```，```/etc/init```，```/bin/init``` 和 ```/bin/sh``` 。如果所有的都失败了，那么内核将会停止。

此时执行的进程 PID 为 1 ，它将会检查配置文件，然后运行相应的进程，如 X11 Windows、登录程序、网络服务等。

至此，全部启动过程完成。

![bootstrap all]({{ site.url }}/images/linux/kernel/bootstrap_startup_process_all.png "bootstrap all"){: .pull-center width="90%" }

## 详细流程

从代码角度，介绍启动时的调用流程。

{% highlight text %}
start_kernel()
 |-smp_setup_processor_id()                  ← 返回启动时的CPU号
 |-local_irq_disable()                       ← 关闭当前CPU的中断
 |-setup_arch()                              ← 完成与体系结构相关的初始化工作
 | |-setup_memory_map()                      ← 建立内存图
 | |-e820_end_of_ram_pfn()                   ← 找出最大的可用页帧号
 | |-init_mem_mapping()                      ← 初始化内存映射机制
 | |-initmem_init()                          ← 初始化内存分配器
 | |-x86_init.paging.pagetable_init()        ← 建立完整的页表
 |-parse_early_param()
 | |-parse_args()                            ← 调用两次parse_args()处理bootloader传递的参数
 |-parse_args()
 |-init_IRQ()                                ← 硬件中断初始化
 |-softirq_init()                            ← 软中断初始化
 |
 |-vfs_caches_init_early()
 |-vfs_caches_init()                         ← 根据参数计算可以作为缓存的页面数，并建立一个存放文件名称的slab缓存
 | |-kmem_cache_create()                     ← 创建slab缓存
 | |-dcache_init()                           ← 建立dentry和dentry_hashtable的缓存
 | |-inode_init()                            ← 建立inode和inode_hashtable的缓存
 | |-files_init()                            ← 建立filp的slab缓存，设置内核可打开的最大文件数
 | |-mnt_init()                              ← 完成sysfs和rootfs的注册和挂载
 |   |-kernfs_init()
 |   |-sysfs_init()                          ← 注册挂载sysfs
 |   | |-kmem_cache_create()                 ← 创建缓存
 |   | |-register_filesystem()
 |   |-kobject_create_and_add()              ← 创建fs目录
 |   |-init_rootfs()                         ← 注册rootfs文件系统
 |   |-init_mount_tree()                     ← 建立目录树，将init_task的命名空间与之联系起来
 |     |-vfs_kern_mount()                    ← 挂载已经注册的rootfs文件系统
 |     | |-alloc_vfsmnt()
 |     |-create_mnt_ns()                     ← 创建命名空间
 |     |-set_fs_pwd()                        ← 设置init的当前目录
 |     |-set_fs_root()                       ← 以及根目录
 |
 |-rest_init()
   |-kernel_init()                           ← 通过kernel_thread()创建独立内核线程
   | |-kernel_init_freeable()
   | | |-do_basic_setup()
   | | | |-do_initcalls()                    ← 调用子模块的初始化
   | | |   |-do_initcall_level()
   | | |     |-do_one_initcall()             ← 调用一系列初始化函数
   | | |       |-populate_rootfs()
   | | |         |-unpack_to_rootfs()
   | | |
   | | |-prepare_namespace()
   | |   |-wait_for_device_probe()
   | |   |-md_run_setup()
   | |   |-initrd_load()                     ← 加载initrd
   | |   | |-create_dev()
   | |   | |-rd_load_image()
   | |   |   |-identify_ramdisk_image()      ← 检查映像文件的magic确定格式，minux、ext2等；并返回解压方法
   | |   |   | |-decompress_method()
   | |   |   |-crd_load()                    ← 解压
   | |   |     |-deco()
   | |   |
   | |   |-mount_root()
   | |
   | |-run_init_process()                    ← 执行init，会依次查看ramdisk、命令行指定、/sbin/init等
   |
   |-kthreadd()                              ← 同样通过kernel_thread()创建独立内核线程
{% endhighlight %}


### 初始化

在初始化时通常可以分为两种：A) 一种是关键而其必须按照特定顺序来完成，通常在 ```start_kernel()``` 中直接调用；B) 以子系统、模块实现，通过 ```do_initcalls()``` 完成。

在 ```do_initcalls()``` 中调用时，会按照等级，从 level0 ~ level7 来初始化，其宏定义在 ```include/linux/init.h``` 中实现，简单分为了两类，内核以及模块的实现。

下面以 inet_init 的初始化为例，末行为最后的展开格式。

{% highlight c %}
fs_initcall(inet_init);                                           // net/ipv4/af_inet.c
#define fs_initcall(fn) __define_initcall(fn, 5)                  // include/linux/init.h
#define __define_initcall(fn, id) \                               // 同上
      static initcall_t __initcall_##fn##id __used \
      __attribute__((__section__(".initcall" #id ".init"))) = fn

static initcall_t __initcall_inet_init5 __used __attribute__((__section__(".initcall5.init"))) = inet_init;
{% endhighlight %}

```do_initcalls()``` 是从特定的内存区域取出初始化函数的指针，然后调用该函数，通过 ```"vmlinux.lds.h"``` 定义的宏。


## 参考

阮一峰的网络日志中有介绍启动流程，可以查看 [Linux 的启动流程](http://www.ruanyifeng.com/blog/2013/08/linux_boot_process.html) 。

另外，一个不错的介绍可以查看 Gustavo Duarte Blog ，[Motherboard Chipsets Mmemory Map](http://duartes.org/gustavo/blog/post/motherboard-chipsets-memory-map) 主要介绍主板的内存；[How Computers Boot Up](http://duartes.org/gustavo/blog/post/how-computers-boot-up) 介绍计算机如何启动；[Kernel Boot Process](http://duartes.org/gustavo/blog/post/kernel-boot-process) 介绍内核启动的过程，含有 Linux 和 Windows 的启动过程 。

<!--
/reference/linux/kernel/bootstrap_motherboard_chipsets_and_the_memory_map.maff
/reference/linux/kernel/bootstrap_how_computers_boot_up.maff
/reference/linux/kernel/bootstrap_the_kernel_boot_process.maff


	<h2>常用工具</h2>
	<p>
		<ul><li>
			读取 BIOS<br>
			在 Linux 中使用 <a href="http://flashrom.org/Flashrom">FlashROM</a> 读取 BIOS 。通过如下命令安装，<pre>$ sudo apt-get install flashrom</pre>

			在读取之前最好看一下 FlashROM 的 <a href="http://flashrom.org/Supported_hardware">硬件支持列表</a> ，读写如下
			<pre># flashrom -r bios_image.bin // 备份 BIOS<br># flashrom -wv new_bios.bin // 刷写新的 BIOS<br># flashrom -E // 某些芯片需要先擦除</pre><br><br>

			在 Windows 中，可以通过 <a href="reference/BIOS_Backup_TooKit.EXE"> BIOS_Backup_TooKit </a> 备份 BIOS 。
		</li></ul>
	</p>

<h1 id="Improvements">Improvements</h1> <p>
Linux内核从2.4升级到2.6之后，内核的引导过程发生了许多变化，现在研究一下Linux内核2.6版本的主要引导过程。

（参考资料：

1、《深入理解Linux内核》附录A；

2、Linux源码自带文档http://lxr.linux.no/linux+v2.6.36/Documentation/x86/boot.txt、

3、Linux源码header.S代码http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/header.S

）



Linux内核2.6版本系统引导步骤：

<ol><li>
    开机执行BIOS代码，主要就是POST过程和初始化硬件设备；</li><br><li>

    BIOS代码运行结束后，BIOS把MBR中的第一部分代码和分区表加载到内存地址0X00007c00，最后跳转到第一部分代码；</li><br><li>

    第一部分代码把自己加载到内存地址0X00096a00，并在内存地址0X00098000处设置实模式堆栈，然后LILO把自己的第二部分代码加载到内存地址0X00096c00，最后跳转到LILO第二部分代码；</li><br><li>

    LILO第二部分代码把header.S代码，一共两个512字节（boot sector和setup）分别加载到内存地址0X00090000和0X0009200，同时把Linux小内核映像加载到内存地址0X00010000或者Linux大内核映像加载到内存地址0X00100000，最后跳转到header.S代码的setup代码（240行）；</li><br><li>

    header.S代码的setup过程调用main函数（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/main.c）（301行），最后跳转到main函数代码；

6、main函数设置临时的IDT表、GDT表、调用go_to_protected_mode()（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/pm.c#L104）函数（177行）进入保护模式，最后跳转到startup_32函数（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/compressed/head_32.S）；

7、startup_32函数调用decompress_kernel()（http://lxr.linux.no/linux+v2.6.36/arch/x86/boot/compressed/misc.c）函数（151行）解压Linux内核映像到内存地址0X00100000，最后跳转到startup_32函数（http://lxr.linux.no/linux+v2.6.36/arch/x86/kernel/head_32.S）；

8、startup_32函数为第一个进程0设置执行环境，包括初始化段寄存器、初始化内核页表、设置内核堆栈、加载GDT表、加载IDT表等，最后跳转到start_kernel()函数（http://lxr.linux.no/linux+v2.6.36/init/main.c#L536）；
<br><br>
<br><br>
start_kernel() 函数完整的初始化了所有 Linux 内核，包括进程调度、内存管理、系统时间等，最后调用 kernel_thread()。<br><br>
</li></ol>


<br><br>
</p>




<br><br><h1>initramfs/initrd镜像</h1><p>
Linux 在自身初始化完成之后，需要能够找到并运行第一个用户程序，通常叫做 "init" 程序。该程序存在于文件系统中，因此，内核必须找到并挂载一个文件系统才可以成功完成系统的引导过程。<br><br>

在 grub 中提供了一个选项 "root=" 用来指定第一个文件系统，但随着硬件的发展，很多情况下这个文件系统也许是存放在 USB 设备、SCSI 设备等等多种多样的设备之上，如果需要正确引导，USB 或者 SCSI 驱动模块首先需要运行起来，可是不巧的是，这些驱动程序也是存放在文件系统里，因此会形成一个悖论。<br><br>

为解决此问题，Linux kernel 提出了一个 RAM disk 的解决方案，把一些启动所必须的用户程序和驱动模块放在 RAM disk 中，这个 RAM disk 看上去和普通的 disk 一样，有文件系统、cache 等。内核启动时，首先把 RAM disk 挂载起来，等到 init 程序和一些必要模块运行起来之后，再切到真正的文件系统之中。initrd 通常在加载 kernel 之前由 Bootloader 初始化。<br /><br />

上面提到的RAM disk的方案实际上就是initrd。 如果仔细考虑一下，initrd虽然解决了问题但并不完美。 比如，disk有cache机制，对于RAM disk来说，这个cache机制就显得很多余且浪费空间；disk需要文件系统，那文件系统（如ext2等）必须被编译进kernel而不能作为模块来使用。<br /><br />

Linux 2.6 kernel提出了一种新的实现机制，即initramfs。顾名思义，initramfs只是一种RAM filesystem而不是disk。initramfs实际是一个cpio归档，启动所需的用户程序和驱动模块被归档成一个文件。因此，不需要cache，也不需要文件系统。<br /><br />


其初始化过程如下。<ol><li>
[boot loader]<br>Bootloader 根据配置文件将 kernel 和 initrd 这两个 image 加载到 RAM 。</li><li>

[boot loader -> kernel]<br>完成必要的操作后，准备将执行权交给 Linux kernel 。</li><li>

[kernel]<br>进行一系列初始化操作，initrd 所在的 RAM 被 kernel 映射到 /dev/initrd ，通过 kernel 內部的 gzip 解压到 /dev/ram0 上。</li><li>

[kernel]<br>Linux 以 R/W (可读写) 模式将 /dev/ram0 挂载在暂时性的 rootfs 上。</li><li>

[kernel-space -> user-space]<br>Kernel 准备执行 /dev/ram0 上的 /linuxrc 程序，並切換執行流程
[user space] /linuxrc 與相關的程式處理特定的操作，比方說準備掛載 rootfs 等
[user-space -> kernel-space] /linuxrc 執行即將完畢，執行權轉交給 kernel
[kernel] Linux 掛載真正的 rootfs 並執行 /sbin/init 程式
[user space] 依據 Linux distribution 規範的流程，執行各式系統與應用程式
</ol>
在 grub 中，通常有两个配置项 linux 和 initrd 。
</p>

<br><h2>制作initramfs</h2><p><ol><li>
查看 initramfs 的内容<br>
<pre style="font-size:0.8em; face:arial;">
# mkdir temp && cd temp && cp /boot/initramfs-`uname -r`.img initrd.img
# file initrd.img                                              // 查看文件类型
# mv initrd.img initrd.img.gz && gunzip initrd.img.gz          // 如果是压缩包
# cpio -i --make-directories &lt; initrd.img                      // 解压cpio文件类型
</li>

<li>制作initramfs<br />

</li> </ol>
</p>
http://www.ibm.com/developerworks/cn/linux/l-initrd.html  Linux 初始 RAM 磁盘（initrd）概述，含有详细介绍

http://www.cnblogs.com/cybertitan/archive/2012/10/10/2718971.html
http://www.linuxsir.org/bbs/thread356982.html

arch/x86/kernel/microcode_intel_early.c:737:load_ucode_intel_bsp(void)
再dmesg中输出第一条信息

startup_64  arch/x86/boot/compressed/head_64.S  内核保护模式的入口

demesg中的信息是从start_kernel()之后记录的。
http://www.linuxjournal.com/content/lock-free-multi-producer-multi-consumer-queue-ring-buffer
http://www.ibm.com/developerworks/cn/linux/l-cn-lockfree/
http://lwn.net/Articles/340400/
http://wenku.baidu.com/view/a6b26372f242336c1eb95e9e.html  Linux内核跟踪之ring buffer的实现
https://www.kernel.org/doc/Documentation/circular-buffers.txt
-->




{% highlight text %}
{% endhighlight %}
