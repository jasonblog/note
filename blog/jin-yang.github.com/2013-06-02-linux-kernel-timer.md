---
title: Kernel 系统时钟
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux
description:
---


<!-- more -->

## CONFIG_HZ VS. USER_HZ

在内核中维护了两个 HZ 参数，CONFIG_HZ 和 USER_HZ ，其中前者可以在 ```make menuconfig``` 时进行配置，默认 250, 现在有些设置为了 1000，而后者固定为 100 。

#### CONFIG_HZ / HZ

内核中一般使用 HZ，实际两者等价。该参数决定了内核的时钟频率，由硬件中断产生，每次中断是一次 tick ，也就是内核的一次时钟滴答，同时 jiffies++ 。

可以通过提高该参数增加系统的实时性，但同时会导致过多的时间花费在中断处理中。

#### USER_HZ

在用户空间使用 clock_t 表示时钟的滴答数。由于内核的 CONFIG_HZ 是变化的，那么如果类似于 glibc 或用户应用程序，在需要获得 clock_t 时，要么通过 sysctl 系统调用，要么通过 /proc 或 /sys 文件系统去获得。

与其采用上面的方法，不如内核定死用户空间看到的 HZ 就是 100 ，然后内核在往用户空间提供 tick 的时候都以这个为单位，这个也就是 USER_HZ 的作用。

### 使用方式

假设你要设计一个内核模块，该模块提供时间信息给用户程序，且选择用 tick 为单位，那么应使用 ```jiffies_64_to_clock_t(jiffiex)``` 将 CONFIG_HZ 转换成 USER_HZ；通过 ```clock_t_to_jiffies(tick)``` 将 USER_HZ 转换成 CONFIG_HZ 。

也就是说，在用户程序中，只能看到 USER_HZ ，除非是内核的 bug ，否则以 tick 或 jiffies 为单位的用户空间接口，实际指的都是 USER_HZ 。

### 查看验证

{% highlight text %}
----- 关于USER_HZ可以在shell中通过如下命令获取
$ getconf CLK_TCK
100

----- 获取CONFIG_HZ
$ grep ^CONFIG_HZ /boot/config-`uname -r`
CONFIG_HZ_1000=y
CONFIG_HZ=1000
{% endhighlight %}

对于 USER_HZ 也可以在 C 语言中通过 ```sysconf(_SC_CLK_TCK)``` 获得。

<!--
有意思的是，systemtap tutorial有个比较好玩的实验，也可以确定CONFIG_HZ的大小。

global count_jiffies, count_ms
probe timer.jiffies(100) { count_jiffies ++ }
probe timer.ms(100) { count_ms ++ }
probe timer.ms(543210)
{
    hz=(1000*count_jiffies) / count_ms
    printf ("jiffies:ms ratio %d:%d => CONFIG_HZ=%d\n",
    count_jiffies, count_ms, hz)
    exit ()
}

输出如下：

jiffies:ms ratio 1358:5420 => CONFIG_HZ=250

实验等待的时间有点久，需要等待543210ms，9分钟左右，时间越久，误差越小，如果等待的时间段一些，会出现误差。
-->

### 其它

2.6.12 以后引入了 NO_HZ 和 high_res timer ，最新的内核使用 NO_HZ_FULL_ALL；也就是当系统空闲时，不再定时产生时钟中断，系统可以进入 default_idle() 节省系统 power 。

通常硬件可以提供多个时钟源，而且可能精度达到 ns 级，这些时钟源可以供 high_res timer 使用，从而不受 CONFIG_HZ 的限制。




{% highlight text %}
{% endhighlight %}
