---
title: StatsD 监控简介
layout: post
comments: true
language: chinese
category: [misc]
keywords: linux,statsd,monitor
description:
---

StatsD 的使用非常简单，其中 Etsy 改写后使用的是 Node.js 编写的，所以需要先安装好 node 环境，然后修改配置文件，直接启动即可。

当然，也可以将其理解为一个协议，也就对应了多种语言的实现。

这里简单介绍与之相关的概念。

<!-- more -->

## 简介

StatsD 狭义上来讲，其实就是一个监听 UDP(Default)/TCP 的守护程序。

基本分为了三步，A) 根据简单的协议收集 StatsD 客户端发送来的数据；B) 解析数据并进行聚合；C) 然后定时推送给后端，如 Graphite 和 InfluxDB 等，并通过 Grafana 等展示。

### 协议格式

StatsD 的协议其实非常简单，每一行就是一条数据，通过换行符 `'\n'` 分割。

{% highlight text %}
<metric_name/bucket>:<value>|<type>[|@sample_rate]
{% endhighlight %}

以监控系统负载为例，假如某一时刻系统 1 分钟的负载是 0.5，通过以下命令就可以写入 StatsD 。

{% highlight text %}
echo "system.load.1min:0.5|g" | nc --wait 1 --udp 127.0.0.1 8125
echo "system.load.1min:0.5|g" | nc -w 1 -u 127.0.0.1 8125
{% endhighlight %}

其中指标命名没有特别的限制，但是一般的约定是使用点号分割的命名空间；指标的值一般是大于等于 0 的浮点数。

StatD 通过指标类型 gauge、counting、timing、set，封装了一些最常用的操作，例如周期内指标值的累加、统计执行时间等。

## 指标类型

### Counting

也就是计数类型，每次会对 `countor` 递加，当刷新时会将计数值发送，并重置为 0 。

{% highlight text %}
countor:1|c
{% endhighlight %}

注意，只有 counting 和 timing 类型，可以设置采样。

#### 关于sample_rate

这个参数的主要作用是降低网络的传输带宽，例如 0.5 表示 UDP 包减少一半，其代价是降低了精确度。对于客户端，如果设置了 0.5 ，也就意味着只有原先 50% 的时间发送统计值；在服务端，会根据采样值进行一些修正，简单来说就是乘以 2 。

实际上，在客户端每次调用发送接口会计算其发送概率。

### Timing

计时器的一大好处在于，可以得到平均值、总值、计数值、百分位数(Percentile)和上下限值，最终生成如下类似的值：

{% highlight text %}
stats.timers.$KEY.mean_$PCT
stats.timers.$KEY.upper_$PCT
stats.timers.$KEY.sum_$PCT
{% endhighlight %}

接下来从一个例子可能下上述值的具体含义，假设有如下 20 个采集值：

{% highlight text %}
0 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95
{% endhighlight %}

其中 `90` 中位数表示会忽略最大 `10%` 的数据，也就是忽略 `20*10%=2` 个值，就是取消 `90`、`95` 这两个值。那么 `lower_90=0`，`upper_90=85`，`sum_90=sum(0,5,10,...,85)=765`，`mean_90=765/(20-2)=42.5` 。

注意，其中 `lower_90` 总是最小值，也就是跟 `lower` 的含义相同，有些冗余。

### Gauges

标量是任何可测量的一维变量，如内存、人的身高、体重等。

{% highlight text %}
gaugor:333|g
{% endhighlight %}

如果该值在下次刷新前没有更新，那么就会发送上次的值。也可以在值前添加正负号，用于表示对值的修改，而非设置。这也就意味着，不能直接设置负值，需要先设置为 0 才可以。

{% highlight text %}
gaugor:333|g
gaugor:-10|g
gaugor:+4|g
{% endhighlight %}

如上，最终的值是 `333-10+4=327` 。

### Sets

可以针对某一个集合进行统计，统计总共出现了多少种类的值。

{% highlight text %}
seter:765|s
{% endhighlight %}


## 源码解析

其主代码比较少，详细的可以查看 [github stats.js](https://github.com/etsy/statsd/blob/master/stats.js) 中的内容，而后端的刷新代码，如对于 graphite 使用的是 `graphite_flush()` 函数。


## 参考

两个 C 语言的实现可以参考 [statsite](https://github.com/statsite/statsite)、[brubeck](https://github.com/github/brubeck) 。

<!--
https://github.com/etsy/statsd/blob/master/docs/metric_types.md
https://blog.pkhamre.com/understanding-statsd-and-graphite/

https://github.com/statsite/statsite
https://github.com/github/brubeck
https://github.com/jbuchbinder/statsd-c


StatsD 的 APM 实现
https://docs.datadoghq.com/tracing/
Datadog StatsD 的消息格式
https://docs.datadoghq.com/developers/dogstatsd/


单行最大为 1024 字节，超过将会返回 `-ENOSPC(28)` 错误。
-->


{% highlight text %}
{% endhighlight %}
