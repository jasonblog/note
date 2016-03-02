# Linux内核kernel panic机制浅析

1.简介：

内核错误(Kernel panic)是指操作系统在监测到内部的致命错误，并无法安全处理此错误时采取的动作。一旦到这个情况，kernel就尽可能把它此时能获取的全部信息都打印出来，至于能打印出多少信息，那就看是哪种情况导致它panic了。`这个概念主要被限定在Unix以及类Unix系统中；对于Microsoft Windows系统，等同的概念通常被称为蓝屏死机`。


操作系统内核中处理Kernel panic的子程序（在AT&T派生类以及BSD类Unix中，通常称为panic()）通常被设计用来向控制台输出错误信息，向磁盘保存一份内核内存的转储，以便事后的调试，然后等待系统被手动重新引导，或自动重新引导。该程序提供的技术性信息通常是用来帮助系统管理员或者软件开发者诊断问题的。

操作系统试图读写无效或不允许的内存地址是导致内核错误的一个常见原因。内核错误也有可能在遇到硬件错误或操作系统BUG时发生。在许多情况中，操作系统可以在内存访问违例发生时继续运行。然而，系统处于不稳定状态时，操作系统通常会停止工作以避免造成破坏安全和数据损坏的风险，并提供错误的诊断信息。内核错误在早期的Unix系统中被引入，显示了在Unix与Multics在设计哲学上的主要差异之一。Multics的开发者Tom van Vleck曾引述了一段在这个问题上与Unix开发者Dennis Ritchie的讨论：

我提醒Dennis说，我在Multics中写的近半数代码都是错误恢复代码。 他说：“我们不需要这些。我们有称为panic的子程序，如果发生了错误就可以调用这个函数，使得系统崩溃， 然后你可以在大厅里面大叫：‘嘿，重启机器’。”

原始的`panic()`函数从UNIX第五版开始到基于VAX的UNIX 32V期间几乎没有变化，只是输出一条错误信息，然后就使系统进入NOP的死循环中。当改进UNIX的基础代码的时候，panic()函数也有所改进，可以向控制台输出多种格式的调试信息。



kernel panic 主要有以下几个出错提示：

```sh
Kernel panic - not syncing fatal exception in interrupt
kernel panic - not syncing: Attempted to kill the idle task!
kernel panic - not syncing: killing interrupt handler!
Kernel Panic - not syncing：Attempted to kill init!
```

2.什么能导致kernel panic：

一般出现下面的情况，就认为是发生了kernel panic:

- 机器彻底被锁定，不能使用

- 数字键(Num Lock)，大写锁定键(Caps Lock)，滚动锁定键(Scroll Lock)不停闪烁。

- 如果在终端下，应该可以看到内核dump出来的信息（包括一段”Aieee”信息或者”Oops”信息）

- 和Windows蓝屏相似

有两种主要类型kernel panic：

```sh
hard panic(也就是Aieee信息输出)

soft panic (也就是Oops信息输出)
```

只有`加载到内核空间的驱动模块`才能`直接`导致kernel panic，你可以在系统正常的情况下，使用lsmod查看当前系统加载了哪些模块。 除此之外，`内建在内核里的组件`（比如memory map等）也能导致panic。因为hard panic和soft panic本质上不同，因此我们分别讨论。


### 2.1 hard panic

原因

`对于hard panic而言，最大的可能性是驱动模块的中断处理(interrupt handler)导致的`，`一般是因为驱动模块在中断处理程序中访问一个空指针(null pointre)`。一旦发生这种情况，驱动模块就无法处理新的中断请求，最终导致系统崩溃。

例如：在多核系统中，包括AP应用处理器、mcu微控制器和modem处理器等系统中，mcu控制器用于系统的低功耗控制，mcu微控制器由于某种原因超时向AP应用处理器发送一个超时中断，AP接受中断后调用中断处理函数读取mcu的状态寄存器，发现是mcu的超时中断，就在中断处理程序中主动引用一个空指针，迫使AP处理器打印堆栈信息然后重启linux系统。这就是一个典型的hard panic。

信息收集

根据panic的状态不同，内核将记录所有在系统锁定之前的信息。因为kenrel panic是一种很严重的错误，不能确定系统能记录多少信息，下面是一些需要收集的关键信息，他们非常重要，因此尽可能收集全，当然如果系统启动的时候就kernel panic，那就无法知道能收集到多少有用的信息了。

- `/var/log/messages`: 幸运的时候，整个kernel panic栈跟踪信息都能记录在这里。

- 应用程序/库日志: 可能可以从这些日志信息里能看到发生panic之前发生了什么。

- 其他发生panic之前的信息，或者知道如何重现panic那一刻的状态

- 终端屏幕dump信息，一般OS被锁定后，复制，粘贴肯定是没戏了，因此这类信息，你可以需要借助数码相机或者原始的纸笔工具了。

实际上，当内核发生panic时，linux系统会默认立即重启系统，当然这只是默认情况，除非你修改了产生panic时重启定时时间，这个值默认情况下是0，即立刻重启系统。所以当panic时没有把kernel信息导入文件的话，那么可能你很难再找到panic产生的地方。


Linux的稳定性勿容置疑，但是有些时候一些Kernel的致命错误还是会发生（有些时候甚至是因为硬件的原因或驱动故障），`Kernel Panic会导致系统crash，并且默认的系统会一直hung在那里，直到你去把它重新启动！ 不过你可以在/etc/sysctl.conf文件中加入`

### 2.2 soft panic(oops)

在Linux上，oops即Linux内核的行为不正确，并产生了一份相关的错误日志。`许多类型的oops会导致kernel panic，即令系统立即停止工作，但部分oops也允许继续操作，作为与稳定性的妥协。这个概念只代表一个简单的错误。`

`当内核检测到问题时，它会打印一个oops信息然后终止全部相关进程`。oops信息可以帮助Linux内核工程师调试，检测oops出现的条件，并修复导致oops的程序错误。
Linux官方内核文档中提到的oops信息被放在内核源代码Documentation/oops-tracing.txt中。通常klogd是用来将oops信息从内核缓存中提取出来的，然而，在某些系统上，例如最近的Debian发行版中，rsyslogd代替了klogd，因此，缺少klogd进程并不能说明log文件中缺少oops信息的原因。
若系统遇到了oops，一些内部资源可能不再可用。即使系统看起来工作正常，非预期的副作用可能导致活动进程被终止。oops常常导致kernel panic，若系统试图使用被禁用的资源。Kernelloops提到了一种用于收集和提交oops到 http://www.kerneloops.org/ 的软件 。Kerneloops.org同时也提供oops的统计信息。


症状：
- 1.没有hard panic严重
- 2.`通常导致段错误(segmentation fault)`
- 3.`可以看到一个oops信息，/var/log/messages里可以搜索到’Oops’`
- 4.机器稍微还能用（但是收集信息后，应该重启系统）

原因:
`凡是非中断处理引发的模块崩溃都将导致soft panic。在这种情况下，驱动本身会崩溃，但是还不至于让系统出现致命性失败，因为它没有锁定中断处理例程`。导致hard panic的原因同样对soft panic也有用（比如在运行时访问一个空指针）

信息收集：
`当soft panic发生时，内核将产生一个包含内核符号(kernel symbols)信息的dump数据，这个将记录在/var/log/messages里`。为了开始排查故障，可以使用ksymoops工具来把内核符号信息转成有意义的数据。
为了生成ksymoops文件,需要：
从/var/log/messages里找到的堆栈跟踪文本信息保存为一个新文件。确保删除了时间戳(timestamp)，否则ksymoops会失败。
运行ksymoops程序（如果没有，请安装）
详细的ksymoops执行用法，可以参考ksymoops(8)手册。

## 3.Kernel panic实例:

今天就遇到 一个客户机器内核报错：“Kernel panic-not syncing fatal exception”，重启后正常，几个小时后出现同样报错，系统down了，有时重启后可恢复有时重启后仍然报同样的错误。

`什么是fatal exception?`

`“致命异常（fatal exception）表示一种例外情况，这种情况要求导致其发生的程序关闭。通常，异常（exception）可能是任何意想不到的情况（它不仅仅包括程序错误）。致命异常简单地说就是异常不能被妥善处理以至于程序不能继续运行。`

`软件应用程序通过几个不同的代码层与操作系统及其他应用程序相联系。当异常（exception）在某个代码层发生时，为了查找所有异常处理的代码，各个代码层都会将这个异常发送给下一层，这样就能够处理这种异常。如果在所有层都没有这种异常处理的代码，致命异常（fatal exception）错误信息就会由操作系统显示出来。这个信息可能还包含一些关于该致命异常错误发生位置的秘密信息（比如在程序存储范围中的十六进制的位置）。这些额外的信息对用户而言没有什么价值，但是可以帮助技术支持人员或开发人员调试程序。`



`当致命异常（fatal exception）发生时，操作系统没有其他的求助方式只能关闭应用程序，并且在有些情况下是关闭操作系统本身。当使用一种特殊的应用程序时，如果反复出现致命异常错误的话，应将这个问题报告给软件供应商。 ” 而且此时键盘无任何反应，必然使用reset键硬重启。`

处理panic后的系统自动重启panic.c源文件有个方法，当panic挂起后，指定超时时间，可以重新启动机器，这就是前面说的panic超时重启。


```c
# vi linux-2.6.31.8/kernel/panic.c ...

if (panic_timeout > 0)
{
    /*
    * Delay timeout seconds before rebooting the machine.
    * We can't use the "normal" timers since we just panicked.
    */
    int i;
    printk(KERN_EMERG "Rebooting in %d seconds..", panic_timeout);

    for (i = 0; i < panic_timeout * 1000;) {
        touch_nmi_watchdog();
        i += panic_blink(i);
        mdelay(1);
        i++;
    }

    /*
    * This will not be a clean reboot, with everything
    * shutting down. But if there is a chance of
    * rebooting the system it will be rebooted.
    */
    emergency_restart();
} ...
```


