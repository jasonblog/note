---
title: Collectd 简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: collectd,monitor,linux
description: collectd 是一个后台监控程序，用来采集其所运行系统上的系统信息，并提供各种存储方式来存储为不同值的格式，例如 RRD 文件形式、文本格式、MongoDB 等等。在此，简单介绍下 collectd 。
---

collectd 是一个后台监控程序，用来采集其所运行系统上的系统信息，并提供各种存储方式来存储为不同值的格式，例如 RRD 文件形式、文本格式、MongoDB 等等。

在此，简单介绍下 collectd 。

<!-- more -->

![collectd logo]({{ site.url }}/images/linux/collectd_logo.png "collectd logo"){: .pull-center width="30%" }

## 简介

Collectd 完全由 C 语言编写，故性能很高，可移植性好，它允许运行在系统没有脚本语言支持或者 cron daemon 的系统上，比如嵌入式系统；同时，它包含优化以及处理成百上千种数据集的新特性，目前包含了几十种插件。

其中数据采集和采集数据的写入都可以通过插件进行定义，目前支持的数据采集插件包括了几十种，写入包括了 Graphite、RRDtool、Riemann、MongoDB、HTTP 等。

![collectd architecture]({{ site.url }}/images/linux/collectd_architecture.png "collectd architecture"){: .pull-center width="70%" }

简单来说，其特点如下：

1. C语言、插件、多线程(读写线程池)，支持 Python、Shell、Perl 等脚本语言；
2. 数据采集插件包括了 OS(CPU、Memory、Network、Disk等)、通用组件(Nginx、MySQL、PostgreSQL、Redis、HAProxy等)；
3. 写入插件支持 RRDTool、Kafka、MongoDB、Redis、Riemann、Sensu、TSDB等；
4. 支持级联方式，数据发送采用 UDP 报文；

如下是将采集的数据保存为 rrd 格式文件，并通过 rrdtool 工具绘制出来。

![collectd cpu rrd]({{ site.url }}/images/linux/collectd_cpu_example.png "collectd cpu rrd"){: .pull-center }

### 安装

对于 CentOS 可以直接安装 collectd 对应的 RPM 包，不同的插件可以安装不同的包。

{% highlight text %}
----- 查看当前版本，以及所支持的插件
$ yum --enablerepo=epel list all | grep collectd
----- 安装
# yum --enablerepo=epel install collectd
{% endhighlight %}

### 源码编译

编译选项可以直接通过 ```./configure --help``` 命令查看，例如可以通过如下配置开启调试选项，也就是 COLLECT_DEBUG 宏；对于插件会自动检查是否存在头文件，例如 rrdtool 需要 rrd.h 文件，也就是需要安装 rrdtool-devel 包。

{% highlight text %}
$ ./configure --enable-debug --enable-all-plugins
{% endhighlight %}

注意，在配置完之后会打印配置项信息，包括了编译、链接参数，以及支持哪些插件等，也可以查看 config.log 文件。

编译完成之后，可以直接通过 ```make check``` 进行检查；对于 RPM 包生成，在 contrib 中有相应的 spec 文件；对于帮助文档可以查看 ```man -l src/collectd.1```；关于支持哪些组件，可以查看源码中的 README 文件，例如测试时，可以直接开启 write_log 写入插件。

另外，编译后的动态库保存在 src/.libs 目录下，如果不想安装，可以直接调整配置文件即可。


### 常见操作

{% highlight text %}
----- 启动服务
$ collectd -C collectd.conf
----- 检查配置文件
$ collectd -C collectd.conf -t
----- 测试插件，会调用一次插件
$ collectd -C collectd.conf -T
----- 刷新，会生成单独线程执行do_flush()操作
$ kill -SIGUSR1 `pidof collectd`
----- 关闭服务，也可以使用SIGTERM信号
$ kill -SIGINT `pidof collectd`
----- 采用多线程，可以通过该命令查看当前的线程数
$ ps -Lp `pidof collectd`
{% endhighlight %}

#### 状态查看

Collectd 可以通过 unixsock 插件进行通讯、查看状态等，也可以使用 collectdctl 命令；通过如下配置打开 unixsock 。

{% highlight text %}
LoadPlugin unixsock
<Plugin unixsock>
  SocketFile "/tmp/collectd.sock"
  SocketGroup "collectd"
  SocketPerms "0660"
  DeleteSocket true                  # 启动时如果存在sock，是否尝试删除
</Plugin>
{% endhighlight %}

在开启了 UnixSock 之后，可以通过如下命令与 Collectd 进行交互；对于 collectdctl 命令，可直接通过 ```man -l src/collectdctl.1``` 查看帮助。

{% highlight text %}
$ collectdctl -s /tmp/collectd.sock listval
$ echo "PUTNOTIF severity=okay time=$(date +%s) message=hello" | \
    socat - UNIX-CLIENT:/path/to/socket
{% endhighlight %}


### 日志配置

collectd 支持多种日志格式，包括了 syslog、logstash、本地日志文件等；如果调试，也可以通过 write_log 插件将采集数据写入到日志文件中。

<!--
另外，可以使用 logrotate 管理日志数据，配置如下。TODO:如何确认logrotate配置有效%%%%%无法自动重新打开,logrotate失效!!!

cat <<- EOF > /etc/logrotate.d/collectd.conf
var/log/nginx/*.log /var/log/tomcat/*log {   # 可以指定多个路径
    daily                      # 每天切换一次日志
    rotate 15                  # 默认保存15天数据，超过的则删除
    compress                   # 切割后压缩
    delaycompress              # 切割时对上次的日志文件进行压缩
    dateext                    # 日志文件切割时添加日期后缀
    missingok                  # 如果没有日志文件也不报错
    notifempty                 # 日志为空时不进行切换，默认为ifempty
    create 644 monitor monitor # 使用该模式创建日志文件
    postrotate # TODO
        if [ -f /var/run/nginx.pid ]; then
            kill -USR1 `cat /var/run/nginx.pid`
        fi
    endscript
}

FIXME: 如果启用logfile，则尝试打开默认文件，没有权限则报错。用于打印 [2017-04-27 13:57:32] plugin_load: plugin "logfile" successfully loaded. 信息。
-->

## 推荐配置

源码中有一个 src/collectd.conf 参考配置文件，如下是一些常见的配置项。

{% highlight text %}
# 指定上报的主机名，可以为IP或者按照规则定义主机名称(service_name.componet_name)
Hostname    "foobar"
# 是否允许主机名查找，如果DNS配置可能有误，建议不要开启
FQDNLookup   false

# 各种文件、目录的设置
BaseDir     "/var/lib/collectd"
PIDFile     "/var/run/collectd.pid"
PluginDir   "/home/andy/Workspace/packages/collectd/collectd-5.7.1/src/.libs"
TypesDB     "/home/andy/Workspace/packages/collectd/collectd-5.7.1/src/types.db"

# 设置为true时，对于<Plugin XXX>可以自动加载插件，无需指定LoadPlugin，详见dispatch_block_plugin()
AutoLoadPlugin false

# 是否同时上报collectd自身的状态，会通过plugin_register_read()注册读取函数，也即会通过
# 执行plugin_update_internal_statistics()函数上报当前collectd的状态
CollectInternalStats false

# 设置全局的数据采集时间间隔，单位是秒，可以在插件中进行覆盖；关于最大采集时间间隔，详见后面的介绍
Interval                 1
MaxReadInterval        180

# 注意这里单位不是秒，实际超时时间是timeout*interval
Timeout                  2

# 用于配置读写线程数
WriteThreads             2
ReadThreads              5

# 配置缓存的上下限
WriteQueueLimitLow    8000
WriteQueueLimitHigh  12000

# 创建一个Unix Socket用于一些命令发送，状态查看等
LoadPlugin unixsock
<Plugin unixsock>
  SocketFile "/tmp/collectd.sock"
  SocketGroup "collectd"
  SocketPerms "0660"
  DeleteSocket true                  # 启动时如果存在sock，是否尝试删除
</Plugin>

# 设置日志文件，保存到本地文件中，可以通过logrotate管理
LoadPlugin logfile
<Plugin logfile>
    LogLevel info
    File "/var/log/collectd/collectd.log"  # 也可以配置为STDOUT，作为标准输出
    Timestamp true
    PrintSeverity true
</Plugin>

# 通过write_log插件将采集数据保存到日志中，默认采用的是Graphite格式
LoadPlugin write_log
<Plugin write_log>
  Format JSON
</Plugin>
{% endhighlight %}

如下简单列举一些常用的配置。

1\. 数据缓存上下限设置。

可以通过 ```WriteQueueLimitHigh```、```WriteQueueLimitLow``` 参数可以配置缓存队列的大小，在源码中对应了 ```write_limit_high```、```write_limit_low``` 变量。

当小于 ```WriteQueueLimitLow``` 时不会丢弃所采集的数据，当大于上述值而小于 ```WriteQueueLimitHigh``` 时会概率性丢弃，而大于 High 则直接丢弃。

2\. 插件采集时间间隔。

如果调用插件读取数据时失败，则会将下次采集时间 double，但是最大值为配置文件中的设置值；当一次读取成功时，则会直接恢复到正常的采集时间间隔。

3\. 关于Timeout参数

主线程会检查是否有数据采集超时，注意这里真正的超时时间是 timeout*interval ，如果超时则会调用注册的 missing 回调函数，而且会在 global cache 中删除这个对象。

4\. types.db 的使用

type.db 每行由两个字段组成，由空格或者 tab 分隔，分别表示：A) 数据集名称；B) 数据源说明的列表，对列表单元都以逗号分隔。

每个数据源通过冒号分为 4 部分：数据源名、类型、最小值和最大值 ```ds-name:ds-type:min:max```；其中 ds-type 包含 4 种类型 (```ABSOULUTE```, ```COUNTER```, ```DERIVE```, ```GAUSE```)，其中，mix 和 max 定义了固定值范围，U 表示范围未知或者不限定。

每行数据对应了一个 ```value_list_t```，可以通过 ```format_values()``` 函数进行格式化。

5\. 如何查看 collectd 的状态？

自身状态查看，可以在配置文件中添加 ```CollectInternalStats true``` 选项，或者开启 unixsocket，然后通过 collectdctl 命令进行查看。

显然，一个是可以远程采集数据的，一个是只能本地访问的。

## Filter-Chain

从 4.6 版本开始支持 filter-chain 功能，可用于将某个值发送给特定的输出，其工作模式类似于 ip_tables，包括了 matches 和 targets，前者用于匹配一个规则，后者用于执行某个操作。

如下是处理流程图。

{% highlight text %}
 +---------+
 ! Chain   !
 +---------+
      !
      V
 +---------+  +---------+  +---------+  +---------+
 ! Rule    !->! Match   !->! Match   !->! Target  !
 +---------+  +---------+  +---------+  +---------+
      !
      V
 +---------+  +---------+  +---------+
 ! Rule    !->! Target  !->! Target  !
 +---------+  +---------+  +---------+
      !
      V
      :
      :
      !
      V
 +---------+  +---------+  +---------+
 ! Rule    !->! Match   !->! Target  !
 +---------+  +---------+  +---------+
      !
      V
 +---------+
 ! Default !
 ! Target  !
 +---------+
{% endhighlight %}

简单介绍下如上的常见概念：

* Match，用于匹配规则，例如采集指标名称以及当前采集的值，通常规则使用 match_ 插件，不过在使用前需要提前加载。
* Target，就是将要执行的一些操作，插件以 target_ 开头。
* Rule，零到多个 match 规则以及至少一个 target 组成 rule ，如果没有规则对所有的值匹配。
* Chain，包括了一系列的 rule 以及可能的默认目标，会按照规则进行匹配，如果满足则指定 target 执行。

Chain 规则，目前只支持 Pre-Chain 和 Post-Chain 两种，在配置文件中可通过 PreCacheChain、PostCacheChain 用于配置 filter-chain 功能。


<!--
There are four ways to control which way a value takes through the filter mechanism:

jump
    The built-in jump target can be used to ``call'' another chain, i. e. process the value with another chain. When the called chain finishes, usually the next target or rule after the jump is executed.
stop
    The stop condition, signaled for example by the built-in target stop, causes all processing of the value to be stopped immediately.
return
    Causes processing in the current chain to be aborted, but processing of the value generally will continue. This means that if the chain was called via Jump, the next target or rule after the jump will be executed. If the chain was not called by another chain, control will be returned to the daemon and it may pass the value to another chain.
continue
    Most targets will signal the continue condition, meaning that processing should continue normally. There is no special built-in target for this condition.
-->

### 示例

一个简单的示例如下。

{% highlight text %}
PostCacheChain "PostCache"
<Chain "PostCache">
  <Rule "ignore_mysql_show">
    <Match "regex">
      Plugin "^mysql$"
      Type "^mysql_command$"
      TypeInstance "^show_"
    </Match>
    <Target "stop">
    </Target>
  </Rule>
  <Target "write">
    Plugin "rrdtool"
  </Target>
</Chain>
{% endhighlight %}

上述的配置中，会忽略 ```plugin=mysql + type=mysql_command + type_instance=show_``` 采集的值，不过需要注意的是，这个操作是在 ```PostCache``` 链表中，所以仍然可以通过 unixsock 插件查看采集值。




<!--
## 优化建议




通过cdtime()函数获取当前时间，实际上调用clock_gettime()系统函数，统计从1970.1.1开始时间；然后通过nanosleep()休眠。

struct timespec {
    time_t tv_sec; /* seconds */
    long tv_nsec;  /* nanoseconds */
};
typedef uint64_t cdtime_t;
#define NS_TO_CDTIME_T(ns)                                                     \
  (cdtime_t) {                                                                 \
    ((((cdtime_t)(ns)) / 1000000000) << 30) |                                  \
        ((((((cdtime_t)(ns)) % 1000000000) << 30) + 500000000) / 1000000000)   \
  }
#define TIMESPEC_TO_CDTIME_T(ts)                                               \
  NS_TO_CDTIME_T(1000000000ULL * (ts)->tv_sec + (ts)->tv_nsec)

在内部维护了list以及heap两个结构，分别用于保存当前的插件，以及下个周期需要调用那个插件。


LoadPlugin rrdtool
对于rrdtool插件，会在DataDir目录下创建 {hostname}/{measurement} 目录保存采集数据。
-->

## 通讯协议

Collectd 提供了两种协议 [Plain text protocol](https://collectd.org/wiki/index.php/Plain_text_protocol) 以及 [Binary protocol](https://collectd.org/wiki/index.php/Binary_protocol) 。

### Plain text protocol

这一协议目前可以在使用 Exec 执行脚本时使用，或者通过 UnixSock 查看当前 Collectd 服务的状态时使用。注意：一行的请求需要小于 1024 字节，否则报 "Parsing options failed" 错误。

#### LISTVAL

语法是 ```LISTVAL``` ，用于查看当前监控项，会打印出上次采集的时间点。

{% highlight text %}
$ collectdctl -s /tmp/collectd.sock listval
$ echo "LISTVAL" | nc -U /tmp/collectd.sock
$ echo "LISTVAL" | socat - UNIX-CLIENT:/tmp/collectd.sock
{% endhighlight %}

#### GETVAL

语法是 ```GETVAL Identifier``` ，获取某个指标的采集值，指标通过上述的标示符 (Identifier) 定义。

{% highlight text %}
$ echo 'GETVAL "localhost/memory/memory-buffered"' | nc -U /tmp/collectd.sock
{% endhighlight %}

#### PUTVAL

语法是 ```PUTVAL Identifier [OptionList] Valuelist``` ，用于将某个监控项提交给 Collectd 服务，会自动转给写插件。

* OptionList: 选项列表，每项通过 KV 表示，目前只支持 interval=seconds 这一模式。
* Valuelist: 通过冒号分割的时间戳以及值，支持的数据类型有 uint(COUNTER+ABSOLUTE), int(DERIVE), double(GAUGE)；对于 GAUGE 可以使用 "U" 标示未定义值，但是不能对 COUNTER 使用 "U"；时间戳采用 UNIX epoch，也可以使用 "N" 表示使用当前时间。

{% highlight text %}
$ echo 'PUTVAL "localhost/load/load" interval=10 N:13:456:5000' | nc -U /tmp/collectd.sock
{% endhighlight %}

<!--
##### PUTNOTIF

echo "PUTNOTIF severity=okay time=$(date +%s) message=hello" | \
  socat - UNIX-CLIENT:/path/to/socket
-->

## Thresholds

4.3.0 版本之后，开始支持监控，实际上也就是说不仅支持数据的采集上报，而且会对采集数据进行判断，如果定义的事件发生则会上报。

这一操作主要是通过 threshold 插件完成，然后发送 notification 消息；而其它的插件则可以注册接收该消息并作进一步的处理。

注意，这里没有对采集的数据进行处理，也就是没有做类似移动平均线 (Moving Average) 之类的处理，也就意味着比较敏感，可能会有误报。

如下是一个简单的配置文件。

{% highlight text %}
LoadPlugin "threshold"
<Plugin "threshold">
  <Type "foo">
    WarningMin    0.00
    WarningMax 1000.00
    FailureMin    0.00
    FailureMax 1200.00
    Invert false
    Instance "bar"
  </Type>

  <Plugin "interface">
    Instance "eth0"
    <Type "if_octets">
      FailureMax 10000000
      DataSource "rx"
    </Type>
  </Plugin>

  <Host "hostname">
    <Type "cpu">
      Instance "idle"
      FailureMin 10
    </Type>

    <Plugin "memory">
      <Type "memory">
        Instance "cached"
        WarningMin 100000000
      </Type>
    </Plugin>

    <Type "load">
       DataSource "midterm"
       FailureMax 4
       Hits 3
       Hysteresis 3
    </Type>
  </Host>
</Plugin>
{% endhighlight %}

在插件中可以通过 Host Plugin Type 配置块决定需要监控的指标 (注意需要按顺序嵌套)，而且 Plugin Type 还可以通过 Instance 决定哪个示例。

<!--
(WarningMax, FailureMax] + [FailureMin, WarningMin) Warning信息
(FailureMax, ) + ( , FailureMin) Failure信息

DataSource DSName
  某些监控项包括了多个数据源，例如 if_octets 包括了 rx 和 tx、load 包括了 shortterm、midterm、longterm 等等；默认会检查所有的采集项，或者通过该参数指定需要检查的采集项。
Invert true|false(default)
  监控时间策略默认如上所示，当然可以通过该参数取反，也就是在 FailureMin~FailureMax 范围内是异常的。
Persist true|false(default)
  设置Notify通知的频率，true 每个采集点都会发送一个通知；false 越过边界时发送通知，也就是上次正常，本次异常。
PersistOK true|false

    Sets how OKAY notifications act. If set to true one notification will be generated for each value that is in the acceptable range. If set to false (the default) then a notification is only generated if a value is in range but the previous value was not.
Percentage true|false

    If set to true, the minimum and maximum values given are interpreted as percentage value, relative to the other data sources. This is helpful for example for the "df" type, where you may want to issue a warning when less than 5 % of the total space is available. Defaults to false.
Hits Value

    Sets the number of occurrences which the threshold must be raised before to dispatch any notification or, in other words, the number of Intervals that the threshold must be match before dispatch any notification.
Hysteresis Value

    Sets the hysteresis value for threshold. The hysteresis is a method to prevent flapping between states, until a new received value for a previously matched threshold down below the threshold condition (WarningMax, FailureMin or everything else) minus the hysteresis value, the failure (respectively warning) state will be keep.
Interesting true|false

    If set to true (the default), a notification with severity FAILURE will be created when a matching value list is no longer updated and purged from the internal cache. When this happens depends on the interval of the value list and the global Timeout setting. See the Interval and Timeout settings in collectd.conf(5) for details. If set to false, this event will be ignored.

https://collectd.org/wiki/index.php/Notifications_and_thresholds
-->



## RRDTool

Round Robin Database, RRD 适用于存储和时间序列相关的数据，是一种循环使用存储空间的数据库，也就是创建之后其大小已经固定，不再需要维护。

可以通过 rrdtool 保存数据，此时磁盘 IO 可能会成为瓶颈，为此可以使用 RRDCacheD 后台进程。


### 简介

首先介绍下常见概念。

Primary Data Point, PDP：正常情况下每个周期都会收到一个采集值，然后根据不同的类型计算出一个值，这个计算出的值就是 PDP ；注意，除非是 GAUGE，否则改值不是收到的值。

Consolidation Data Point, CPD：通过定义的 CF 函数，使用多个 PDP 计算出一个 CDP 值并保存在 RRA 中，绘图时就使用这些数据。

### 创建文件

其中 filename 、DS 部分和 RRA 部分是必须的，其它两个参数可选。

{% highlight text %}
rrdtool create filename [--start|-b start time] [--step|-s step]
    [DS:variable_name:DST:heartbeat:min:max]
    [RRA:CF:xff:step:rows]

参数解析：
  --step NUM
    预计多久收到一个值，默认是5分钟，与MRTG中的interval相同；
  --start TIMESTAMP
    给出第一个记录的起始时间，小于等于该时间的数据不保存，例如指定为一天前--start $(date -d '1 days ago' +%s)；
  DS:variable_name:DST:heartbeat:min:max
    用于定义 Data Soure Name，常见有eth0_in, eth0_out，一般为1-19个字符，必须是0-9,a-z,A-Z；
    以及数据类型(Data Source Type, DST)，包括了COUNTER、GUAGE、DERIVE、ABSOLUTE、COMPUTE 5 种：
      COUNTER
        必须是递增的，除非是计数器溢出 (overflows) 此时会自动修改收到的值；这也是最常见的类型，例如网络接口流量。
      DERIVE
        和 COUNTER 类似，但可以是递增，也可以递减，例如数据库的链接数。
      ABSOLUTE
        会计算斜率，每次都会假设其前一个值是 0，类似于每次读取监控数据后统计值自动清零。
      GAUGE
        没有上述的平均值概念，收到后原样保存。
      COMPUTE
        这一类型比较特殊，它并不接受输入，而是定义一个表达式，通过引用其它 DS 并自动计算出值。
  RRA:CF:xff:step:rows
    RRA定义了数据如何存放，可以把一个 RRA 看成一个表，各保存不同 interval 的统计结果。
    Consolidation Function, CF 也就是指定的合并功能函数，有 AVERAGE、MAX、MIN、LAST 四种，分别表
    示对多个PDP取平均、最大值、最小值、当前值四种类型。
    xfile factor 取值范围是 0~1 ，表示一个范围内采样值为 UNKNOWN 值比率超过多少就会将聚合值设置为UNKNOWN；
    例如 RRA:AVERAGE:0.5:24:600 ，当 PDP 中有超过 12 值为 UNKNOWN 时该聚合的值就是 UNKNOWN 。

----- 创建文件
$ rrdtool create eth0.rrd \
    --start $(date –d '1 days ago' +%s) \    # 保存一天的数据
    --step 300 \                             # 采集时间间隔为5min
    DS:eth0_in:COUNTER:600:0:12500000 \      # 600 是 heartbeat；0 是最小值；12500000 表示最大值；
    DS:eth0_out:COUNER:600:0:12500000 \      # 如果没有最小值/最大值，可以用 U 代替，例如 U:U
    RRA:AVERAGE:0.5:1:600 \                  # 1表示对1个PDP取平均，实际上就等于PDP的值
    RRA:AVERAGE:0.5:4:600 \                  # 4表示每4个PDP合成为一个CDP，也就是20分钟，方法是对4个PDP取平均
    RRA:AVERAGE:0.5:24:600 \                 # 同上，也就是24*5=120分钟=2小时
    RRA:AVERAGE:0.5:288:730                  # 同上，也就是 288*5=1440分钟=1天

----- 查看文件信息，主要包括了创建时的信息，例如DS、RRA等
$ rrdtool info eth0.rrd

----- 查看最近一次，第一次更新时间
$ rrdtool last eth0.rrd |xargs -i date -d '1970-01-01 {} sec utc'
$ rrdtool first eth0.rrd |xargs -i date -d '1970-01-01 {} sec utc'

{% endhighlight %}

关于 heartbeat ，如上，如果在 300 秒内没有收到数据，那么就再等待 ```heartbeat-step=300``` 秒，如果还没有收到则设置为 UNKNOWN 。

### 更新数据

{% highlight text %}
$ rrdtool {update | updatev} filename
    [--template|-t ds-name[:ds-name]...]
    N|timestamp:value[:value...]
    at-timestamp@value[:value...]
    [timestamp:value[:value...] ...]

----- 更新一个值，需要注意时间戳要大于最近更新的时间，DS数目要保持一致
$ rrdtool update eth0.rrd 1479477014:1:15

----- 两个命令类似，不过会回显写入的结果，类似upate verbose
$ rrdtool updatev eth0.rrd 1479477014:1:15
{% endhighlight %}

时间定义比较灵活，可以是 ```N(date +%s)``` ```AT(man 1 at)``` 两种格式，可以通过 ```date -d '2016-11-18 21:50:14' +%s``` 获取时间戳。

### 获取数据

{% highlight text %}
rrdtool fetch filename CF [--resolution|-r resolution] [--start|-s start] [--end|-e end]

----- 最简单方式
$ rrdtool fetch eth0.rrd AVERAGE
----- 指定范围，可以用AT方式
$ rrdtool fetch eth0.rrd AVERAGE --start end-1day --end 1164553800
----- 指定resolution，如果不存在则使用>=该值的最小值，不过需要保证改值为resolution的整数倍
$ rrdtool fetch eth0.rrd AVERAGE -r 1200 --end $((($(date +%s)/1200)*1200))
{% endhighlight %}

默认 ```--end``` 是 now ，```--start``` 是 end-1day ，也就是 1 天前；指定的 CF 类型需要保证建库时有该 CF 类型的 RRA 才可以。通常一个文件中包含了多个 RRA ，那么其选择条件如下：

1. 该 RRA 必须要满足所请求时间范围，例如，查看两天的数据，如果一个 RRA 保存了一天，那么肯定不会选择该 RRA 。
2. 如果多个 RRA 满足，除非指定 resolution ，否者会选择 resolution 最小的一个。

<!--
http://bbs.chinaunix.net/thread-864861-1-1.html

http://www.yunweipai.com/archives/4220.html
-->

### RRDTool 插件

在 RRDTool 插件中，比较核心的配置分为了两部分：A) 如何创建文件；B) 如何写文件。

{% highlight text %}
LoadPlugin rrdtool
<Plugin rrdtool>
  DataDir Directory       # 指定数据保存目录，默认会保存在BaseDir目录下
  CreateFilesAsync false  # 如果需要创建几百个文件可以设置为异步，此时在文件创建完成前的数据将会丢弃，
                          #     如果同步则会阻塞
  StepSize Seconds        # 设置RRD文件的step，默认会从上报的数据中选择step，建议不要设置
  HeartBeat Seconds       # 一般设置为step的两倍，同样不建议设置
  RRARows NumRows         # 默认是1200，一般保存15个RRA，是MIN, AVERAGE, MAX与hour, day, week, month,
                          #     year的组合，计算方式为PDPs=timespan/(stepsize*rrarows)
  RRATimespan Seconds     # 手动设置范围，可以配置多次，单位是秒；如果没有配置默认是(3600, 86400,
                          #     604800, 2678400, 31622400)
  XFF Factor              # 设置XFiles Factor参数，默认是0.1，范围是[0.0-1.0)

  CacheFlush Seconds      # 当设置了缓存时，每次接收到新值时，如果超过了这里设置的时间，就会检查所有
                          #     的采集值；适用于部分指标由于异常一直没有更新，导致始终保存在缓存中，
                          #     因为消耗资源比较大，建议设置为较大的值，如900、7200秒。
  CacheTimeout Seconds    # 大于0时会将值缓存到内存中，如果缓存的时间差超过改值，则放入到update queue队列中
  WritesPerSecond Updates # 如果采集的指标比较多，可能会一直在刷新，那么如果此时有 IO 操作，那么可能会
                          #     导致阻塞。为此，可以通过该参数限制每秒更新的次数，从而不会造成过大的磁
                          #     盘压力，建议设置为 25~80；此时也可以根据 RRD 文件数，大致评估一次刷新锁
                          #     消耗的时间。
  RandomTimeout Seconds   # 选择随机的时间，其范围是 CacheTimeout-RandomTimeout ~ CacheTimeout+RandomTimeout ，
                          #     用于防止由于同一间隔导致峰值。
</Plugin>
{% endhighlight %}

关于 RRDTool 插件，可以参考 [Inside the RRDtool plugin](https://collectd.org/wiki/index.php/Inside_the_RRDtool_plugin) 。

简言之，如果有很多个监控指标，那么直接更新到磁盘时，就会导致大量的小文件+随机写，将严重影响到 HDD 甚至 SSD 的写入性能。

![collectd rrdtool plugin]({{ site.url }}/images/linux/collectd_rrdtool_plugin.png "collectd rrdtool plugin"){: .pull-center width="70%" }

更新一个值时，一般为 8 Bytes，为了更新该值需要读取 512B(HDD)、16KB(SSD) 字节，在内存中修改对应的值，然后再写入内存，显然这将严重影响到磁盘 IO 性能，为此需要将多个指标合并写入磁盘。

通过一个平衡二叉树，每个节点是采集值或者 RRD 文件，每次收到一个采集值后会与最老的值比较时间戳，如果超过了设置的超时时间，那么该节点就被放 update queue 中。如果一个新写入的值已经在 update queue 队列中了，那么新值会直接写入到 update queue 中相应的节点。


### collectd-web

直接从 [github collectd-web](https://github.com/httpdss/collectd-web) 上下载即可，在配置文件 ```/etc/collectd/collection.conf``` 中指定 ```datadir``` 目录，也就是 rrd 保存的目录；例如：

{% highlight text %}
datadir: "/etc/collectd/collectd-web/"
{% endhighlight %}

然后，通过如下方式运行即可。

{% highlight text %}
./runserver.py 0.0.0.0 8989
{% endhighlight %}

#### No DS

对于 ```No DS called 'contextswitches' in``` 之类的报错，其原因是在 ```collection.modified.cgi``` 文件中的 ```load_graph_definitions()``` 写死；解决方法是先通过 ```rrdtool info filename``` 查看是否存在对应的 DS name 是否存在，一般类似：

{% highlight text %}
ds[value].index = 0
ds[value].type = "DERIVE"
ds[value].minimal_heartbeat = 2
ds[value].min = 0.0000000000e+00
ds[value].max = NaN
ds[value].last_ds = "6950152000"
ds[value].value = 0.0000000000e+00
ds[value].unknown_sec = 0
{% endhighlight %}

其中的 value 就是所谓的 DS name 。


## 常用插件

### tail

主要用于日志文件。


## 参考

关于告警可以参考 [Collectd Thresholds and alerting](https://www.netways.de/fileadmin/images/Events_Trainings/Events/OSMC/2015/Slides_2015/collectd_Thresholds_Plugin_and_Icinga_-_Florian_Forster.pdf) 。

<!--
http://www.linuxhowtos.org/manpages/5/collectd.conf.htm
https://collectd.org/wiki/index.php/Chains





配置文件
1. 配置项的值有三种类型：字符串、浮点型、布尔值
{% highlight text %}
A) 字符串，必须通过双引号包裹
Hostname "localhost"                # localhost
Hostname "local'host"               # local'host
Hostname "local\"host"              # local"host
Hostname "local\"host \             # local"host locahost
         localhost"
{% endhighlight %}



插件通过 LoadPlugin 语句加载，有两种方式：
LoadPlugin syslog        # 加载和配置分开，可以配置多次，例如Python脚本
<Plugin syslog>
    config
</Plugin>
<Plugin syslog>
    config
</Plugin>

<LoadPlugin syslog>        # 加载的同时进行配置
    config
</LoadPlugin>
-->






{% highlight text %}
{% endhighlight %}
