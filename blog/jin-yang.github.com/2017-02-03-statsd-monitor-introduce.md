---
title: StatsD 監控簡介
layout: post
comments: true
language: chinese
category: [misc]
keywords: linux,statsd,monitor
description:
---

StatsD 的使用非常簡單，其中 Etsy 改寫後使用的是 Node.js 編寫的，所以需要先安裝好 node 環境，然後修改配置文件，直接啟動即可。

當然，也可以將其理解為一個協議，也就對應了多種語言的實現。

這裡簡單介紹與之相關的概念。

<!-- more -->

## 簡介

StatsD 狹義上來講，其實就是一個監聽 UDP(Default)/TCP 的守護程序。

基本分為了三步，A) 根據簡單的協議收集 StatsD 客戶端發送來的數據；B) 解析數據並進行聚合；C) 然後定時推送給後端，如 Graphite 和 InfluxDB 等，並通過 Grafana 等展示。

### 協議格式

StatsD 的協議其實非常簡單，每一行就是一條數據，通過換行符 `'\n'` 分割。

{% highlight text %}
<metric_name/bucket>:<value>|<type>[|@sample_rate]
{% endhighlight %}

以監控系統負載為例，假如某一時刻系統 1 分鐘的負載是 0.5，通過以下命令就可以寫入 StatsD 。

{% highlight text %}
echo "system.load.1min:0.5|g" | nc --wait 1 --udp 127.0.0.1 8125
echo "system.load.1min:0.5|g" | nc -w 1 -u 127.0.0.1 8125
{% endhighlight %}

其中指標命名沒有特別的限制，但是一般的約定是使用點號分割的命名空間；指標的值一般是大於等於 0 的浮點數。

StatD 通過指標類型 gauge、counting、timing、set，封裝了一些最常用的操作，例如週期內指標值的累加、統計執行時間等。

## 指標類型

### Counting

也就是計數類型，每次會對 `countor` 遞加，當刷新時會將計數值發送，並重置為 0 。

{% highlight text %}
countor:1|c
{% endhighlight %}

注意，只有 counting 和 timing 類型，可以設置採樣。

#### 關於sample_rate

這個參數的主要作用是降低網絡的傳輸帶寬，例如 0.5 表示 UDP 包減少一半，其代價是降低了精確度。對於客戶端，如果設置了 0.5 ，也就意味著只有原先 50% 的時間發送統計值；在服務端，會根據採樣值進行一些修正，簡單來說就是乘以 2 。

實際上，在客戶端每次調用發送接口會計算其發送概率。

### Timing

計時器的一大好處在於，可以得到平均值、總值、計數值、百分位數(Percentile)和上下限值，最終生成如下類似的值：

{% highlight text %}
stats.timers.$KEY.mean_$PCT
stats.timers.$KEY.upper_$PCT
stats.timers.$KEY.sum_$PCT
{% endhighlight %}

接下來從一個例子可能下上述值的具體含義，假設有如下 20 個採集值：

{% highlight text %}
0 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95
{% endhighlight %}

其中 `90` 中位數表示會忽略最大 `10%` 的數據，也就是忽略 `20*10%=2` 個值，就是取消 `90`、`95` 這兩個值。那麼 `lower_90=0`，`upper_90=85`，`sum_90=sum(0,5,10,...,85)=765`，`mean_90=765/(20-2)=42.5` 。

注意，其中 `lower_90` 總是最小值，也就是跟 `lower` 的含義相同，有些冗餘。

### Gauges

標量是任何可測量的一維變量，如內存、人的身高、體重等。

{% highlight text %}
gaugor:333|g
{% endhighlight %}

如果該值在下次刷新前沒有更新，那麼就會發送上次的值。也可以在值前添加正負號，用於表示對值的修改，而非設置。這也就意味著，不能直接設置負值，需要先設置為 0 才可以。

{% highlight text %}
gaugor:333|g
gaugor:-10|g
gaugor:+4|g
{% endhighlight %}

如上，最終的值是 `333-10+4=327` 。

### Sets

可以針對某一個集合進行統計，統計總共出現了多少種類的值。

{% highlight text %}
seter:765|s
{% endhighlight %}


## 源碼解析

其主代碼比較少，詳細的可以查看 [github stats.js](https://github.com/etsy/statsd/blob/master/stats.js) 中的內容，而後端的刷新代碼，如對於 graphite 使用的是 `graphite_flush()` 函數。


## 參考

兩個 C 語言的實現可以參考 [statsite](https://github.com/statsite/statsite)、[brubeck](https://github.com/github/brubeck) 。

<!--
https://github.com/etsy/statsd/blob/master/docs/metric_types.md
https://blog.pkhamre.com/understanding-statsd-and-graphite/

https://github.com/statsite/statsite
https://github.com/github/brubeck
https://github.com/jbuchbinder/statsd-c


StatsD 的 APM 實現
https://docs.datadoghq.com/tracing/
Datadog StatsD 的消息格式
https://docs.datadoghq.com/developers/dogstatsd/


單行最大為 1024 字節，超過將會返回 `-ENOSPC(28)` 錯誤。
-->


{% highlight text %}
{% endhighlight %}
