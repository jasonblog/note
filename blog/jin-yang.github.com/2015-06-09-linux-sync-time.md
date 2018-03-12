---
title: Linux 时间同步
layout: post
comments: true
language: chinese
category: [linux,program,misc]
keywords: linux,时间,ntp,ntpdate
description: 简单介绍在 Linux 中部分与时间相关的概念，例如时区、闰秒、夏令时、ntp 等。
---

简单介绍在 Linux 中部分与时间相关的概念，例如时区、闰秒、夏令时、ntp 等。

<!-- more -->

## 简介

"现在几点了？" 或者 "离过年还有多久？" 这恐怕连小学生也觉得是再简单不过的问题了；但是如果问及 "时间是什么？" 恐怕绝大多数人都会顿觉茫然。

在中学生数学、物理课本中，时间被表述成一条有起点、有单位、有指向、无始无终的直线，这就是符合人们常识的牛顿 "绝对时间"，也是直到本世纪初被普遍接受的科学的时间概念。

进入二十世纪后，物理学、天文学的新成果、新发现向 "绝对时间" 的基本观念提出了挑战。爱因斯坦狭义相对论指出，时间不能脱离宇宙及其事件的观察者而独立存在，时间是宇宙与其观察者之间的联系的一个方面。爱因斯坦广义相对论的一个直接推论是，由于引力场的原因处于地球表面不同高度的时钟走速不一样，海拔越高钟速越快，差值约为 1.09×10-16 秒/米（海拔），即每升高100米，时钟变快百万亿分之一秒。


"时间是什么？" 这一个问题实质上是探索时间的本质，这只是极少数科学家、哲学家热心研究的课题，而且远没有得出一个令人满意的结果，看来还需要长期探索下去。

> 关于世界时的介绍
>
> 地球自转运动是个相当不错的天然时钟，以它为基础可以建立一个很好的时间计量系统。地球自转的角度可用地方子午线相对于天球上的基本参考点的运动来度量。为了测定地球自转，人们在天球上选取了两个基本参考点：春分点和平太阳，以此确定的时间分别称为恒星时和平太阳时。恒星时虽然与地球自转的角度相对应，符合以地球自转运动为基础的时间计量标准的要求，但不能满足日常生活和应用的需要。人们习惯上是以太阳在天球上的位置来确定时间的，但因为地球绕太阳公转运动的轨道是椭圆，所以真太阳周日视运动的速度是不均匀的（即真太阳时是不均匀的）。为了得到以真太阳周日视运动为基础而又克服其不均匀性的时间计量系统，人们引进了一个假想的参考点─平太阳。它在天赤道上作匀速运动，其速度与真太阳的平均速度相一致。
>
> 平太阳时的基本单位是平太阳日，1平太阳日等于24平太阳小时，86400平太阳秒。以平子夜作为0时开始的格林威治平太阳时，就称为世界时，简称UT。世界时与恒星时有严格的转换关系，人们是通过观测恒星得到世界时的。后来发现，由于地极移动和地球自转的不均匀性，最初得到的世界时，记为UT0，也是不均匀的，人们对UT0 加上极移改正得到UT1，如果再加上地球自转速率季节性变化的经验改正就得到UT2。
>
> 六十年代以前，世界时作为基本时间计量系统被广泛应用，因为它与地球自转的角度有关，所以即使出现了更为均匀的原子时系统，世界时对于日常生活、大地测量、天文导航及其它有关地球的科学仍是必需的。

![ntp rigui logo]({{ site.url }}/images/misc/ntp-rigui-ex.jpg "ntp rigui logo"){: .pull-center width="60%"}


日晷名称是由“日”和“晷”两字组成。“日”指“太阳”，“晷”表示“影子”，“日晷”的意思为“太阳的影子”。因此，所谓日晷，就是白天通过测日影定时间的仪器。日晷计时的原理是这样。在一天中，被太阳照射到的物体投下的影子在不断地改变着，第一是影子的长短在改变，早晨的影子最长，随着时间的推移，影子逐渐变短，一过中午它又重新变长；第二是影子的方向在改变，因为我们在北半球，早晨的影子在西方，中午的影子在北方，傍晚的影子在东方。从原理上来说，根据影子的长度或方向都可以计时，但根据影子的方向来计时更方便一些。故通常都是以影子的方位计时。由于日晷必须依赖日照，不能用于阴天和黑夜。因此，单用日晷来计时是不够的，还需要其他种类的计时器，如水钟，来与之相配。


<!--
时间计量系统    时间测量误差（秒/天）   频率准确度
世界时(UT)  10-3    10-8
历书时(ET)  10-4    10-9
原子时(TA)  10-14   10-16
-->

## 时区 TimeZone

网上聊天时，有人问你说现在几点? 你看了看表回答他说晚上 8 点了，但是这个在欧洲的哥们有点纳闷了，因为他那里还太阳当空呢！

![ntp time zone]({{ site.url }}/images/misc/ntp-time-zone-logo.jpg "ntp time zone"){: .pull-center width="70%"}

实际上，人们习惯于按照太阳的位置对当地的时间进行划分，通常是太阳在正中时作为 12 点。因为地球是圆的，在环绕太阳旋转的 24 个小时中，世界各地日出日落的时间是不一样的，所以我们才有划分时区的必要。

在真正使用时，为了照顾到行政上的方便，时区并不严格按南北直线来划分，而是按自然条件来划分。例如，我国差不多跨 5 个时区，但实际上在只用东八时区的标准时即北京时间为准。

![ntp time zone]({{ site.url }}/images/misc/ntp-global-time-zone.jpg "ntp time zone"){: .pull-center width="100%"}

1884 年在华盛顿召开的一次国际经度会议，又称国际子午线会议，规定将全球划分为 24 个时区，其中东、西各 12 个时区，而且规定英国 (格林尼治天文台旧址) 为中时区 (零时区)，每个时区横跨经度 15 度，时间正好是1小时；最后的东、西第 12 区各跨经度 7.5 度，以东、西经 180 度为界。


<!--
地理课上我们都学过格林威治时间(GMT), 它也就是0时区时间. 但是我们在计算机中经常看到的是UTC. 它是Coordinated Universal Time的简写. 虽然可以认为UTC和GMT的值相等(误差相当之小),但是UTC已经被认定为是国际标准,所以我们都应该遵守标准只使用UTC

那么假如现在中国当地的时间是晚上8点的话,我们可以有下面两种表示方式
20:00 CST
12:00 UTC

这里的CST是Chinese Standard Time,也就是我们通常所说的北京时间了. 因为中国处在UTC+8时区,依次类推那么也就是12:00 UTC了.
-->

### Linux 下时区设置

在 Linux 中的 glibc 提供很多编译好的 timezone 文件，存放在 /usr/share/zoneinfo 目录下，基本涵盖了大部分的国家和城市。

{% highlight text %}
# ls -F /usr/share/zoneinfo/
Africa/      Chile/   Factory    Iceland      Mexico/   posix/      Universal
America/     CST6CDT  GB         Indian/      Mideast/  posixrules  US/
Antarctica/  Cuba     GB-Eire    Iran         MST       PRC         UTC
Arctic/      EET      GMT        iso3166.tab  MST7MDT   PST8PDT     WET
Asia/        Egypt    GMT0       Israel       Navajo    right/      W-SU
Atlantic/    Eire     GMT-0      Jamaica      NZ        ROC         zone.tab
Australia/   EST      GMT+0      Japan        NZ-CHAT   ROK         Zulu
Brazil/      EST5EDT  Greenwich  Kwajalein    Pacific/  Singapore
Canada/      Etc/     Hongkong   Libya        Poland    Turkey
CET          Europe/  HST        MET          Portugal  UCT
{% endhighlight %}

在这里面就可以找到自己所在城市的 time zone 文件，文件的内容可以通过 zdump 查看，

{% highlight text %}
# zdump /usr/share/zoneinfo/Asia/Shanghai
Shanghai  Sun Jan  1 07:46:01 2013 Shanghai
{% endhighlight %}

可以通过设置 /etc/localtime 符号链接，或者设置 TZ 环境变量设置本地的时区，其中后者优先。

{% highlight text %}
----- 1. 直接添加符号链接
# cd /etc
# ln -sf ../usr/share/zoneinfo/Asia/Shanghai localtime

----- 2. 首先根据提示选择时区，然后设置环境变量
# tzselect
# export TZ='America/Los_Angeles'
{% endhighlight %}

注意，第二种方法需要添加到 profile 文件中。

## 闰秒

首先，看下我们当前对秒的定义。

* 1956 年，秒定义为平均太阳日的 1/86400；
* 1960 至 1967 年之间，定义为 1960 年地球自转一周时间的 1/86400；
* 1967 年，国际计量大会决定，以铯原子基态的两个超精细能级间在零磁场下跃迁辐射 9,192,631,770 周期所持续的时间；
* 1977 年，人类认识到了引力时间膨胀会导致在不同高度的原子钟时间不同，重新定义为 "水平面的铯原子" 基态的两个超精细能级间在零磁场下跃迁辐射 9,192,631,770 周期所持续的时间；
* 2019 年，国际时间频率咨询委员会将讨论“秒”的重新定义问题。

随着人类认知的进步，对秒的定义逐渐发生了变化，从 1967 年开始，时间的计量标准便正式由天文学的宏观领域过渡到了物理学的微观领域。

### 常见概念

接着看下与闰秒相关的概念。

#### UT (Universal Time，世界时)

一种以格林尼治子夜起算的平太阳时，由于以地球自转为基准，观测精度受限于地球的自转速度的稳定，地球体积不均匀、潮汐引力以及其他星球的扰动的原因，导致地球转速不稳定，每日误差达数毫秒。

#### TAI (International Atomic Time) 国际原子时

1971 年建立，利用某些元素(如铯、氢、铷)的原子能级跃迁频率有极高稳定性的特性定义时间标准，现为国际计量局 (BIPM) 的时间部门维持，综合全球约 60 个实验室中的大约 240 台各种自由运转的原子钟提供的数据进行处理，得出 "国际时间标准" 称为国际原子时 (TAI)，每日误差为数纳秒。

TAI 时间原点为 UT 1958.01.01 00:00:00，在此之后 TAI 就沿着原子秒的节拍一直走下去，和 UT 误差也越来越大。

#### UTC (协调世界时)

如上所述，现在有两种时间计量系统：基于天文测量而得出的 "世界时" 和以物理学发展发现的原子振荡周期固定这一特性而来的 "原子时" 。

UTC 就是用于将世界时 (UT，天文时间) 和国际原子时 (TAI，原子时间) 协调起来另外一套计量系统。1971 年国际计量大会通过决议，使用 UTC 来计量时间，协调的原则就是 UTC 以原子秒长节拍，在时刻上尽量接近于世界时 (UT)。

目前 UTC 是事实上的时间标准，比如所有计算机中的时间就是 UTC 时间，通过时区换算为本地时间，而闰秒实际上就是 UTC 特有的。

### 闰秒

闰秒简单来说，就是为了让 UTC 尽量靠近 UT 时间做出的人为调整。

详细来说，世界时依靠天文观测因此误差较大 (每日数毫秒)，而原子时依靠原子的物理特性，相对精度较高 (每日误差几个纳秒)，UTC 是以原子秒长为基础，在时刻上尽量接近于世界时的一种时间计量系统。

为了确保 UTC 与 UT 尽量接近，保证误差小于 0.9 秒，当 UTC 与 UT 之间的误差超过 0.6 秒时，国际地球自转服务组织 (IERS) 就会决定在最快到来的闰秒调整日期 (目前基本都是 6.30 或 12.31) 的最后一分钟，对 UTC 增加或减少一秒，也就是这一分钟将变为 61 秒或 59 秒， 这个增加或减少的一秒即为闰秒。

#### 闰秒操作

如果增加一秒，称为正闰秒，会在调整日的 23:59:59 后额外插入一个 23:59:60，然后再到次日的 00:00:00，这样当日最后一分钟就有 61 秒。

如果减少一秒，则为负闰秒，会在调整日的 23:59:58 秒后跳过 23:59:59 这一秒，直接到达次日 00:00:00， 这样当日最后一分钟就只有 59 秒。

当前为止所有的闰秒调整都是正闰秒，也就是说地球转的越来越慢了。

### 系统状态

注意，闰秒的插入是在 UTC 时间，所以不同的时区闰秒写入时间会有所区别。

{% highlight text %}
# ntpq -crv
associd=0 status=4519 **leap_add_sec**(leap_none), sync_ntp, 1 event, leap_armed(clock_sync),
version="ntpd 4.2.6p5@1.2349-o Mon Feb  6 07:34:43 UTC 2017 (1)",
processor="x86_64", system="Linux/3.10.0-514.10.2.el7.x86_64", leap=00,
stratum=3, precision=-18, rootdelay=70.647, rootdisp=142.974,
refid=59.46.44.253, reftime=dcb86ce1.cc4a9654  **Sat, May  6 2017 23:40:17.798**,
clock=dcb86dd0.e933c60d  Sat, May  6 2016 23:44:16.910, peer=41148, tc=6,
mintc=3, offset=-18.414, frequency=16.806, sys_jitter=83.733,
clk_jitter=49.584, clk_wander=6.979, tai=35, leapsec=201507010000,
expire=201512280000
{% endhighlight %}

由于闰秒是整数加入，因此 UTC 与 TAI 之间会出现若干整数秒差别，从 1958.01.01 00:00:00 这一 TAI 时间开始，到 1970.01.01 00:00:00 (epoch time，UNIX 0 second)，UTC 已经比 TAI 慢了 10 秒，后经过多次闰秒调整，2015.07.01 00:00:00 开始 (闰秒调整后)，UTC 将比 TAI 慢 36 秒。

在内核中，可以通过 ```"inserting leap second"``` 关键词查看。

另外，需要注意，内核日志显示的是 23:59:60 这一闰秒，实际验证时发现不一定会出现 23:59:60 这一时间点，而是和本机的 tzdata 版本有关，如果未升级到包含闰秒信息的最新版本的话，结果会是重复一次 23:59:59 。

可以通过 ```while true; do date; sleep 1; done``` 命令查看。

## Linux 时间设置

首先，需要明确两个概念，在一台计算机上我们有两个时钟：A) 硬件时间时钟 (Real Time Clock, RTC)；B) 系统时钟 (System Clock) 。

硬件时钟是主板上的一特殊电路，由单独的电池供电，从而保证关机之后还可以计算时间。系统时钟就是内核用来计算时间的时钟，是从 1970-01-01 00:00:00 UTC 到目前为止秒数总和。

在 Linux 下系统时间在开机的时候会和硬件时间同步，之后也就各自独立运行了；当然，时间久了之后必然就会产生误差。可以通过如下命令查看系统时间以及硬件时间。

{% highlight text %}
----- 1. 查看系统时间和硬件时间
# date
Fri Jul  6 00:27:13 CST 2007
# hwclock --show
Fri 06 Jul 2007 12:27:17 AM CST  -0.968931 seconds

----- 2. 时间同步
# hwclock --hctosys               # 把硬件时间设置成系统时间
# hwclock --systohc               # 把系统时间设置成硬件时间

----- 3. 时间设置
# hwclock --set --date="mm/dd/yy hh:mm:ss"
# date -s "dd/mm/yyyy hh:mm:ss"
{% endhighlight %}

实际上服务器的时钟与标准时间随着时间偏移都会有些偏差，因为现在使用的 "铯/铷原子钟" 成本过高，为此我们就需要在互联网上找到一个可以提供准确时间的服务器，然后通过一种协议来同步我们的系统时间。

这个协议就是 NTP 了。

## NTP

网络时间协议 (Network Time Protocol, NTP) 是通过 [RFC-1305](https://tools.ietf.org/html/rfc1305) 定义的时间同步协议，该协议基于 UDP 协议，使用端口号为 123；该协议用于在时间服务器和客户端之间同步时间，从而使网络内所有设备的时钟保持一致。

对于运行NTP的本地系统，既可以接收来自其他时钟源的同步，又可以作为时钟源同步其他的时钟，并且可以和其他设备互相同步。


### 时钟层级 Stratum Levels

NTP 时间同步是按照层级分发时间的。

![ntp stratum levels]({{ site.url }}/images/misc/ntp-stratum-levels.png "ntp stratum levels"){: .pull-center width="60%"}

时钟层级定义了到参考时钟的距离，如上图所示。

* stratum-0 也即参考时钟，有极高的精度，提供可靠的 UTC 时间，通常为铯/铷原子钟(Atomic Cesium/Rubidium Clocks)、GPS 授时或者无线电波授时 (如 CDMA)，这类的时钟源不会直接为互联网提供服务。

* stratum-1 直接（非通过网络）链接到 stratum-0 提供服务，通常通过 RS-232、IRIG-B 链接，为互联网提供标准时间。

* stratum-2+ 通过网络向上一级请求服务，通常利用 NTP 协议。

通常距离顶层的时钟源越远时钟的精度也就越差。

### 安装

在 CentOS 中，可以通过如下方式进行安装。

{% highlight text %}
# yum install ntp ntpdate              # 安装
# systemctl [enable|start] ntpd        # 设置开机启动，或者直接启动
{% endhighlight %}

在 Linux 下有两种时钟同步方式，分别为直接同步和平滑同步：

* 通过 ntpdate 直接同步时间。但异常情况时，若机器有个 12 点的定时任务，当前时间认为是 13 点，而实际的标准时间为 11 点，直接使用该命令会造成任务重复执行；因此该命令多用于时钟同步服务的第一次同步时间时使用。

* 通过 ntpd 可以平滑同步时钟，通过每次同步较小的时间的偏移量，慢慢进行调整，也因此会耗费较长的时间，但可以保证一个时间不经历两次。

[www.pool.ntp.org](http://www.pool.ntp.org/zone/cn) 提供了全球的标准时钟同步服务，对于中国可以参考其中的建议 ntp 配置文件，NTP 建议我们为了保障时间的准确性，最少找两个个 NTP Server 。

{% highlight text %}
server 0.asia.pool.ntp.org
server 1.asia.pool.ntp.org
server 2.asia.pool.ntp.org
server 3.asia.pool.ntp.org
{% endhighlight %}

它的一般格式都是 number.country.pool.ntp.org ；当然，也可以使用阿里的公共服务。

![ntp alibaba basic service]({{ site.url }}/images/misc/ntp-alibaba-basic-service.png "ntp alibaba basic service"){: .pull-center width="60%"}

接下来可以使用如下命令。

{% highlight text %}
{% endhighlight %}


### 工作原理

NTP 授时有以下三种工作模式。

#### broadcast/multicast

适用于局域网的环境，时间服务器周期性以广播的方式，将时间信息传送给其他网路中的时间服务器，配置简单 (可做到客户端免配置)，但是精确度不高，对时间精确度要求不是很高的情况下可以采用。

#### client/server

最主流的 ntp 授时方式，精度较高，对网络没有特殊要求；同时可以指定多个 ntp server，因而也可避免网络路径失效带来的时钟不能同步。

#### symmetric

一台服务器可以从远端时间服务器获取时钟，如果需要也可提供时间信息给远端的时间服务器，服务器之间的 peer 关系即为这种工作方式。

接下来，简单介绍下 Client/Server 模式。

如下图所示，Device A 和 Device B 通过网络相连，且都有自己独立的系统时钟，需要通过 NTP 实现各自系统时钟的自动同步。假设在系统时钟同步前，Device A 的时钟设定为 10:00:00am，Device B 的时钟设定为 11:00:00am。

其中 Device B 作为 NTP 时间服务器，即 Device A 将使自己的时钟与 Device B 的时钟同步，另外，假设 NTP 报文在 Device A 和 Device B 之间单向传输所需要的时间为 1 秒。

![how ntp works]({{ site.url }}/images/misc/ntp-how-ntp-works.png "how ntp works"){: .pull-center }

系统时钟同步的工作过程如下：

1. Device A 发送一个 NTP 报文给 Device B，该报文带有它离开 Device A 时的时间戳，该时间戳为 10:00:00am(T1)。

2. 当 NTP 报文到达 Device B 时，Device B 会加上自己的时间戳，该时间戳为 11:00:01am(T2)。

3. 此 NTP 报文离开 Device B 时，Device B 再加上自己的时间戳，该时间戳为 11:00:02am(T3)。

4. 当 Device A 接收到该响应报文时，Device A 的本地时间为 10:00:03am(T4)。

至此，Device A 已经拥有足够的信息来计算两个重要的参数：A）报文的往返时延 Delay=(T4-T1)-(T3-T2)=2s；B) Device A 相对 Device B 的时间差 Offset=((T2-T1)+(T3-T4))/2=1h。这样，Device A 就能够根据这些信息来设定自己的时钟，使之与 Device B 的时钟同步。

### NTP 的报文格式

NTP 有两种不同类型的报文，一种是时钟同步报文，另一种是控制报文。控制报文仅用于需要网络管理的场合，它对于时钟同步功能来说并不是必需的，在此只介绍时钟同步报文。

![ntp packets format]({{ site.url }}/images/misc/ntp-packets-format.jpg "ntp packets format"){: .pull-center }

<!--
主要字段的解释如下：
LI（Leap Indicator）：长度为2比特，值为“11”时表示告警状态，时钟未被同步。为其他值时NTP本身不做处理。
VN（Version Number）：长度为3比特，表示NTP的版本号，目前的最新版本为3。
Mode：长度为3比特，表示NTP的工作模式。不同的值所表示的含义分别是：0未定义、1表示主动对等体模式、2表示被动对等体模式、3表示客户模式、4表示服务器模式、5表示广播模式或组播模式、6表示此报文为NTP控制报文、7预留给内部使用。
Stratum：系统时钟的层数，取值范围为1～16，它定义了时钟的准确度。层数为1的时钟准确度最高，准确度从1到16依次递减，层数为16的时钟处于未同步状态，不能作为参考时钟。
Poll：轮询时间，即两个连续NTP报文之间的时间间隔。
Precision：系统时钟的精度。
Root Delay：本地到主参考时钟源的往返时间。
Root Dispersion：系统时钟相对于主参考时钟的最大误差。
Reference Identifier：参考时钟源的标识。
Reference Timestamp：系统时钟最后一次被设定或更新的时间。
Originate Timestamp：NTP请求报文离开发送端时发送端的本地时间。
Receive Timestamp：NTP请求报文到达接收端时接收端的本地时间。
Transmit Timestamp：应答报文离开应答者时应答者的本地时间。
Authenticator：验证信息。
-->

NTP 提供了多种工作模式进行时间同步，包括了：客户端/服务器模式、对等体模式、广播模式、组播模式。

在不能确定服务器或对等体IP地址、网络中需要同步的设备很多等情况下，可以通过广播或组播模式实现时钟同步；客户端/服务器和对等体模式中，设备从指定的服务器或对等体获得时钟同步，增加了时钟的可靠性。

### 启动 NTP 服务

接下来，看看如何在 Linux 中配置并启动 ntp 服务器。

#### 配置文件

先修改配置文件 /etc/ntp.conf ，只需要加入上面的 NTP Server 和一个 driftfile 就可以了。

{% highlight text %}
# vi /etc/ntp.conf
driftfile /var/lib/ntp/drift
server 0.asia.pool.ntp.org
server 1.asia.pool.ntp.org
server 2.asia.pool.ntp.org
server 3.asia.pool.ntp.org
{% endhighlight %}

fudge 127.127.1.0 stratum 0  stratum  这行是时间服务器的层次。设为0则为顶级，如果要向别的NTP服务器更新时间，请不要把它设为0


<!--
那么这里一个很简单的思路就是第一我们只允许局域网内一部分的用户连接到我们的服务器. 第二个就是这些client不能修改我们服务器上的时间

权限的设定主要以 restrict 这个参数来设定，主要的语法为：
restrict IP地址 mask 子网掩码 参数
其中 IP 可以是IP地址，也可以是 default ，default 就是指所有的IP
参数有以下几个：
ignore　：关闭所有的 NTP 联机服务
nomodify：客户端不能更改服务端的时间参数，但是客户端可以通过服务端进行网络校时。
notrust ：客户端除非通过认证，否则该客户端来源将被视为不信任子网
noquery ：不提供客户端的时间查询
注意：如果参数没有设定，那就表示该 IP (或子网)没有任何限制！

在/etc/ntp.conf文件中我们可以用restrict关键字来配置上面的要求

首先我们对于默认的client拒绝所有的操作
代码:
restrict default kod nomodify notrap nopeer noquery

然后允许本机地址一切的操作
代码:
restrict 127.0.0.1

最后我们允许局域网内所有client连接到这台服务器同步时间.但是拒绝让他们修改服务器上的时间
代码:
restrict 192.168.1.0 mask 255.255.255.0 nomodify

把这三条加入到/etc/ntp.conf中就完成了我们的简单配置. NTP还可以用key来做authentication,这里就不详细介绍了
-->




#### 更新时间

如上，在正式启动 NTP 服务器之前，需要先通过 ntpdate 命令同步系统时间。需要注意的是，此时最好不要启动服务，否则可能由于时间跳变导致服务异常。

{% highlight text %}
# ntpdate 0.asia.pool.ntp.org
# ntpdate 1.asia.pool.ntp.org
{% endhighlight %}

需要注意的是，通过 ntpdate 更新时间时，不能开启 NTP 服务，否则会提示端口被占用。

{% highlight text %}
1 Jan 22:35:08 ntpdate[12481]: no servers can be used, exiting
{% endhighlight %}

当然，可以通过 cron 定期更新时间，

{% highlight text %}
30 8 * * * root /usr/sbin/ntpdate 192.168.0.1; /sbin/hwclock -w
{% endhighlight %}

这样，每天 8:30 Linux 系统就会自动的进行网络时间校准，并写入硬件时钟。


#### 查看状态

首先，通过 ntpstat 查看 NTP 服务的整体状态。

{% highlight text %}
# ntpstat
synchronised to NTP server (202.112.29.82) at stratum 3
   time correct to within 44 ms
   polling server every 256 s
{% endhighlight %}

然后，可以通过 ntpq (NTP query) 来查看当前服务器的状态，到底和那些服务器做时间同步。

{% highlight text %}
# watch ntpq -pn
Every 2.0s: ntpq -p                                  Sat Jul  7 00:41:45 2007

     remote           refid      st t when poll reach   delay   offset  jitter
===========================================================
+193.60.199.75   193.62.22.98     2 u   52   64  377    8.578   10.203 289.032
*mozart.musicbox 192.5.41.41      2 u   54   64  377   19.301  -60.218 292.411
{% endhighlight %}

<!--ntpdc -np -c peer 127.0.0.1-->

现在我就来解释一下其中各个字段的含义。

{% highlight text %}
remote: 本地机器所连接的远程NTP服务器；
 refid: 给远程服务器提供时间同步的服务器，也就是上级服务器；
    st: 远程服务器的层级别，级别从1~16，原则上不会与1直接链接；
     t: 也就是type，表示该时间服务器的工作模式(local, unicast, multicast or broadcast)；

  when: 多少秒之前与服务器成功做过时间同步；
  poll: 和远程服务器多少秒进行一次同步，开始较小，后面会逐渐增加，可在配置文件中通过minpoll+maxpoll设置；
 reach: 成功连接服务器的次数，这是八进值；
 delay: 从本地机发送同步要求到服务器的网络延迟 (round trip time)；
offset: 本地和服务器之间的时间差别，该值越接近于0，说明和服务器的时间越接近；
jitter: 统计值，统计了offset的分布情况，这个数值的绝对值越小则说明和服务器的时间就越精确；
{% endhighlight %}

对于上述输出，有几个问题，第一我们在配置文件中设置与 remote 不同？

因为 NTP 提供的是一个集群，所以每次连接的得到的服务器都有可能是不一样，也就意味着，配置文件中最好指定 hostname 而非 IP 。

第二问题是，那个最前面的 + 和 * 都是什么意思呢？如上，提供了多个服务器同步时间，这些标志就是用来标识服务器的状态。

{% highlight text %}
  (space) reject，节点网络不可达，或者 ACL 有限制；
x falsetick 通过算法确定远程服务器不可用；
. excess 不在前10个同步距离较短的节点所以被排除，如果前10个节点故障，该节点可进一步被选中；
- outlyer 远程服务器被clustering algorithm认为是不合格的NTP Server；
+ candidate 将作为辅助的服务器，作为组合算法的一个候选者，当上述的服务器不可用时，该服务器会接管；
* sys.peer 服务器被确认为我们的主NTP Server，系统时间将由这台机器所提供；
o pps.peer 该节点为选中的时钟源，系统时钟以其为参考；
{% endhighlight %}

<!--对于上述的 o ，实际上，系统时钟无论是间接地通过PPS参考时钟驱动器或直接通过内核接口同步，原则上都是是从秒脉冲信号（PPS）进行同步的。-->

也即是说，输出内容中第一个域标记符号是 * 即可认为系统通过该时间源同步时间。

了解这些之后我们就可以实时监测我们系统的时间同步状况了；注意，部分虚拟机使用的是容器，无独立 kernel，使用的就是宿主机的时间源，所以这时就需要到对应宿主机器排查。

{% highlight text %}
# systemctl start ntpd
$ ps aux | grep ntp
{% endhighlight %}



#### 其它

记录下常见的问题。

##### 1. 配置文件中的driftfile是什么?

每个 system clock 的频率都有误差，NTP 会监测我们时钟的误差值并予以调整，但是 NTP 不会立即调整时间，这样会导致时钟跳变，所以采用的是渐进方式，进而导致这是一个冗长的过程。

为此，它会把记录下来的误差先写入 driftfile，这样即使重启之前的计算结果也就不会丢失了。

另外，记录的单为是 PPM (Part Per Million)，是指百万分之一秒，也就是微秒，

##### 2. 如何同步硬件时钟?

NTP 一般只会同步 system clock，如果我们也要同步 RTC (hwclock) 的话那么只需要把下面的选项打开就可以了。

{% highlight text %}
# vi /etc/sysconfig/ntpd
SYNC_HWCLOCK=yes
{% endhighlight %}

<!--
2、fudge 127.127.1.0 stratum 10 如果是LINUX做为NTP服务器，stratum(层级)的值不能太大，如果要向上级NTP更新可以设成2

ntpd -gq


 frequency error -506 PPM exceeds tolerance 500 PPM


-g
    Normally, ntpd exits if the offset exceeds the sanity limit, which is 1000 s by default. If the sanity limit is set to zero, no sanity checking is performed and any offset is acceptable. This option overrides the limit and allows the time to be set to any value without restriction; however, this can happen only once. After that, ntpd will exit if the limit is exceeded.
-q
Exit the ntpd just after the first time the clock is set. This behavior mimics that of the ntpdate program, which is to be retired. The -g and -x options can be used with this option.



-->


## 夏令时

夏令时 (Daylight Saving Time, DST) 是一种为节约能源而人为规定地方时间的制度，在这一制度实行期间所采用的统一时间称为 "夏令时间"。

一般在天亮早的夏季人为将时间调快一小时，可以使人早起早睡，减少照明量，以充分利用光照资源，从而节约照明用电。各个采纳夏时制的国家具体规定不同，目前全世界有近 110 个国家每年要实行夏令时。

北京夏令时是我国在 1986~1991 年间实施的一种夏季时间制度，北京夏令时比标准的北京时间早一个小时。


## 参考

另外，有个 adjtimex 命令，可以用来调整系统和硬件时间；Linux 中通过 tzdata 保存了与时区相关的信息，包括夏令时、闰秒等信息，不过对于新发布的闰秒则需要最近的更新。

### NTP

[standard NTP query program](https://www.eecis.udel.edu/~mills/ntp/html/ntpq.html) ntpq 的实现官网，而且包括了很多相关的资料 [The Network Time Protocol (NTP) Distribution](https://www.eecis.udel.edu/~mills/ntp/html/index.html) 。

<!-- https://www.eecis.udel.edu/~mills/ntp/html/warp.html -->

关于时间的简单介绍，包括了时区、NTP 等 [Managing Accurate Date and Time](http://www.tldp.org/HOWTO/TimePrecision-HOWTO/index.html)，或者 [本地文档](/reference/misc/ntp/HOWTO/TimePrecision-HOWTO/index.html) ；其它的一些关于 GPS 相关的信息可以参考 [GPS时间同步原理及其应用](/reference/misc/ntp/GPS_NTP.doc) ，一篇十分不错的文章。

[Stratum Levels Defined](/reference/misc/ntp/sync_an02-StratumLevelDefined.pdf) 关于时钟层级的定义；另外一篇关于 ntpq 的介绍可以参考 [网络时间的那些事及 ntpq 详解](https://linux.cn/article-4664-1.html?pr) ，或者参考 [本地文档](/reference/misc/ntp/ntpq_details.mht)<!--；还有一篇是关于 [ntp反射式攻击原理](/reference/misc/ntp/ntp_attack.mht)--> 。

实际可以通过 GPS 和 Raspberry Pi 搭建 stratum-1 服务器，可以参考 [Building a Stratum 1 NTP Server with a Raspberry Pi](https://xmission.com/blog/2014/05/28/building-a-stratum-1-ntp-server-with-a-raspberry-pi) ，也可参考 [本地文档](/reference/misc/ntp/Building a Stratum 1 NTP Server with a Raspberry Pi.mht)；或者另一篇 [The Raspberry Pi as a Stratum-1 NTP Server](http://www.satsignal.eu/ntp/Raspberry-Pi-NTP.html)，也可查看 [本地文档](/reference/misc/ntp/Building a Raspberry-Pi Stratum-1 NTP Server.mht) 。

<!--
另外两篇包括了 常见服务器时钟不准的解决方案 、 [NTP alibaba](/reference/misc/ntp/ntp.tar.gz) 。


-->

{% highlight text %}
{% endhighlight %}
