---
title: Linux IO 调度器
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,io,scheduler
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---

用来决定块设备上 IO 操作提交顺序的方法，主要是用于提高吞吐量、降低响应时间。然而两者是相互矛盾的，为了尽量平衡这两者，Linux 内核提供了多种调度算法来适应不同的 IO 请求场景。

这里简单介绍下 Linux 中的 IO 调度器。

<!-- more -->

## Linux IO 调度算法

Linux 2.6 引入了新的 IO 调度子系统，其总体目标是希望让磁头可以沿着一个方向移动，移动到底了再往反方向走，这恰恰类似于现实生活中的电梯模型，所以对应的调度算法也就叫做电梯算法。

内核中的电梯算法包括了：AS(Anticipatory)、CFQ(Complete Fairness Queueing)、Deadline、NOOP(No Operation)，可以在启动的时候通过内核参数指定，默认使用的是 CFQ 。

可以通过如下方式查看或者设置 IO 的调度算法。

{% highlight text %}
----- 查看当前系统支持的IO调度算法
# dmesg | grep -i scheduler
io scheduler noop registered
io scheduler anticipatory registered
io scheduler deadline registered
io scheduler cfq registered (default)

----- 查看当前系统的IO调度方法
$ cat /sys/block/BLOCK-DEVICE/queue/scheduler
noop anticipatory deadline [cfq]

----- 临时修改IO调度方法
$ echo "noop" > /sys/block/BLOCK-DEVICE/queue/scheduler

----- 永久修改参数
# vim /boot/grub/menu.lst
kernel /boot/vmlinuz-2.6.32-504.el6 ro root=LABEL=/ elevator=deadline quiet
{% endhighlight %}

## NOOP

全称为 No Operation，实际上就是实现了最简单的 FIFO 队列，所有 IO 请求基本会按照先进先出的规则操作，不过对于一些相邻的还是做了 IO 请求合并，而非完全按照 FIFO 规则。

{% highlight text %}
----- 有如下的IO请求序列
100, 500, 101, 10, 56, 1000
----- 经过NOOP算法处理之后会按照如下顺序处理
100(101), 500, 10, 56, 1000
{% endhighlight %}

这一算法是 2.4 之前版本的唯一调度算法，计算比较简单，从而减少了 CPU 的使用，不过容易造成 IO 请求饿死。

<!---
关于IO饿死的描述如下：因为写请求比读请求更容易。写请求通过文件系统cache，不需要等一次写完成，就可以开始下一次写操作，写请求通过合并，堆积到I/O队列中。读请求需要等到它前面所有的读操作完成，才能进行下一次读操作。在读操作之间有几毫秒时间，而写请求在这之间就到来 ，饿死了后面的读请求 。其适用于SSD或Fusion IO环境下。
-->

其应用环境主要有以下两种：一是物理设备中包含了自己的 IO 调度程序，比如 SCSI 的 TCQ；二是寻道时间可以忽略不计的设备，比如 SSD 等。

## CFQ

CFQ (Complete Fair Queuing) 完全公平的排队。

该算法会按照 IO 请求的地址进行排序，而非按照 FIFO 的顺序进行响应。该算法为每个进程/线程单独创建一个队列来管理该进程所产生的请求，各队列之间的调度使用时间片来调度，以此来保证每个进程都能被很好的分配到 IO 带宽。

这是一种 QoS 的 IO 调度算法，为每一个进程分配一个时间窗口，在该时间窗口内，允许进程发出 IO 请求。通过时间窗口在不同进程间的移动，保证了对于所有进程而言都有公平的发出 IO 请求的机会；同时 CFQ 也实现了进程的优先级控制，可保证高优先级进程可以获得更长的时间窗口。

{% highlight text %}
----- 有如下的IO请求序列
100，500，101，10，56，1000
----- 经过CFQ算法处理之后会按照如下顺序处理
100，101，500，1000，10，56
{% endhighlight %}

在传统机械磁盘上，寻道会消耗绝大多数的 IO 响应时间，所以 CFQ 尽量减小磁盘的寻道时间。

CFQ 适用于系统中存在多任务 IO 请求的情况，通过在多进程中轮换，保证了系统 IO 请求整体的低延迟。但是，对于只有少数进程存在大量密集的 IO 请求的情况，会出现明显的 IO 性能下降。

通过 CFQ 算法可以有效提高 SATA 盘的整体吞吐量，但是对于先来的 IO 请求并不一定能被满足，也可能会出现饿死的情况，对于通用的服务器也是比较好的选择。

CFQ 是 Deadline 和 AS 调度器的折中，对于多媒体应用和桌面系统是最好的选择。

### 配置参数

CFQ 调度器主要提供如下参数。

{% highlight text %}
$ ls /sys/block/BLOCK-DEVICE/queue/iosched/

slice_idle
	如果一个进程在自己的时间窗口里，经过 slice_idle 时间都没有发送 IO 请求，则调度选择下一个程序。
quantum
	该参数控制在一个时间窗口内可以发送的 IO 请求的最大数目。
low_latency
	对于 IO 请求延时非常重要的任务，可以打开低延迟模式来降低 IO 请求的延时。
{% endhighlight %}

## DEADLINE

在 CFQ 的基础上，解决了 IO 请求可能会被饿死的极端情况，除了 CFQ 本身具有的 IO 排序队列之外，还分别为读 IO 和写 IO 提供了 FIFO 队列，读 FIFO 队列的最大等待时间为 500ms，写 FIFO 队列的最大等待时间为 5s。

也就是说针对 IO 请求的延时而设计，每个 IO 请求都被附加一个最后执行期限。该算法维护两类队列，一是按照扇区排序的读写请求队列；二是按照过期时间排序的读写请求队列。

如果当前没有 IO 请求过期，则会按照扇区顺序执行 IO 请求；如果发现过期的 IO 请求，则会处理按照过期时间排序的队列，直到所有过期请求都被发送为止；在处理请求时，该算法会优先考虑读请求。

FIFO 队列内的 IO 请求优先级要比 CFQ 队列中的高，而读 FIFO 队列的优先级又比写 FIFO 队列的优先级高，也就是说 `FIFO(Read) > FIFO(Write) > CFQ` 。

<!--Deadline 确保了在一个截止时间内服务请求，这个截止时间是可调整的，为了防止了写操作由于不能读取而饿死，默认读期限短于写期限。 -->

### 参数配置

当系统中存在的 IO 请求进程数量比较少时，与 CFQ 算法相比，DEADLINE 算法可以提供较高的 IO 吞吐率，主要提供如下参数。

{% highlight text %}
$ ls /sys/block/BLOCK-DEVICE/queue/iosched/

writes_starved
	该参数控制当读写队列均不为空时，发送多少个读请求后，允许发射写请求。
read_expire
	参数控制读请求的过期时间，单位毫秒。
write_expire
	参数控制写请求的过期时间，单位毫秒。
{% endhighlight %}

该算法适用于数据库环境，如 MySQL、PostgreSQL、ES 等。

## Anticipatory

CFQ 和 DEADLINE 主要聚焦于一些零散的 IO 请求上，而对于连续的 IO 请求，比如顺序读写，并没有做优化。

该算法在 DEADLINE 的基础上，为每个读 IO 都设置了 6ms 的等待时间窗口，如果在这 6ms 内 OS 收到了相邻位置的读 IO 请求，就可以立即处理。

<!--
本质上与Deadline一样，但在最后一次读操作后，要等待6ms，才能继续进行对其它I/O请求进行调度。可以从应用程序中预订一个新的读请求，改进读操作的执行，但以一些写操作为代价。它会在每个6ms中插入新的I/O操作，而会将一些小写入流合并成一个大写入流，用写入延时换取最大的写入吞吐量。AS适合于写入较多的环境，比如文件服务器，但对对数据库环境表现很差。
-->



<!--
读测试
# time dd if=/dev/sda1 of=/dev/null bs=2M count=300
写测试
# time dd if=/dev/zero of=/tmp/test bs=2M count=300


ionice可以更改任务的类型和优先级，不过只有cfq调度程序可以用ionice。

采用cfq的实时调度，优先级为7
ionice -c1 -n7 -ptime dd if=/dev/sda1 f=/tmp/test bs=2M count=300&
采用缺省的磁盘I/O调度，优先级为3
ionice -c2 -n3 -ptime dd if=/dev/sda1 f=/tmp/test bs=2M count=300&
采用空闲的磁盘调度，优先级为0
ionice -c3 -n0 -ptime dd if=/dev/sda1 f=/tmp/test bs=2M count=300&
ionice的磁盘调度优先级有8种，最高是0，最低是7。注意，磁盘调度的优先级与进程nice的优先级没有关系。一个是针对进程I/O的优先级，一个是针对进程CPU的优先级。

七、总结

CFQ和DEADLINE考虑的焦点在于满足零散IO请求上。对于连续的IO请求，比如顺序读，并没有做优化。为了满足随机IO和顺序IO混合的场景，Linux还支持ANTICIPATORY调度算法。ANTICIPATORY的在DEADLINE的基础上，为每个读IO都设置了6ms的等待时间窗口。如果在这6ms内OS收到了相邻位置的读IO请求，就可以立即满足。

IO调度器算法的选择，既取决于硬件特征，也取决于应用场景。
在传统的SAS盘上，CFQ、DEADLINE、ANTICIPATORY都是不错的选择；对于专属的数据库服务器，DEADLINE的吞吐量和响应时间都表现良好。然而在新兴的固态硬盘比如SSD、Fusion IO上，最简单的NOOP反而可能是最好的算法，因为其他三个算法的优化是基于缩短寻道时间的，而固态硬盘没有所谓的寻道时间且IO响应时间非常短。

https://github.com/torvalds/linux/tree/master/Documentation/block
http://wiki.linuxquestions.org/wiki/IOSched
https://liwei.life/2016/03/14/linux_io_scheduler/
http://www.cnblogs.com/zhenjing/archive/2012/06/20/linux_writeback.html







<br><br><br><h1>IO 调度源码分析</h1><p>
其中调度算法的更改等操作保存在 block/elevator.c 中，相关的调度算法为 block/*-iosched.c 。
<pre style="font-size:0.8em; face:arial;">
.elevator_dispatch_fn    取出
.elevator_add_req_fn     添加
</pre>
</p>
-->



## 参考

[Linux IO Stack Diagram](/reference/linux/linux-io-stack-diagram_v0.1.pdf) 文件系统与 IO 的调用层次。

<!--
https://www.xaprb.com/blog/2010/01/09/how-linux-iostat-computes-its-results/
http://hustcat.github.io/iostats/
http://ykrocku.github.io/blog/2014/04/11/diskstats/
https://www.percona.com/live/mysql-conference-2013/sites/default/files/slides/Monitoring-Linux-IO.pdf
-->

{% highlight text %}
{% endhighlight %}
