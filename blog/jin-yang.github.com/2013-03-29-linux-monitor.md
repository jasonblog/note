---
title: Linux 监控
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,monitor,监控
description: Linux 中的系统监控，包括了常见的计算、内存、网络等监控指标，以及相应的工具。
---

Linux 中的系统监控，包括了常见的计算、内存、网络等监控指标，以及相应的工具。

<!-- more -->

很多的性能监控都是通过 proc 文件系统中的文件获得，相应的内核实现保存在 fs/proc/ 目录下。对于日志的监控，除了 tail -f logfile 之外，还可以使用 tailf 。

如下是记录的些工具，不过部分还没有仔细研究过：

* nmon<br>需要下载。

* ftools<br>可以通过  hg clone https://code.google.com/p/linux-ftools/ 下载源码 (hg 在 mercurial 包中)，当然需要翻墙才可以，也可以查看 [本地压缩包](/reference/linux/monitor/linux-ftools.tar.bz2)，据说是一个很牛掰的工具。

* slabtop<br>实时显示 /proc/slabinfo 中的内容。

* Netdata<br>一个很牛掰的高度优化的监控工具，通过 bootstrap框架 做展示，可以参考 [官方网站](https://github.com/firehol/netdata) 。


<!--
首先通过 lscpu 查看 CPU 的基本配置，然后通过 mpstat -P ALL 1 查看多核的性能，查看是否负载均衡。如果不均衡可能是由于：设置了 NUMA、网卡绑定不均衡。<br><br>
BIOS设置  sudo ./idracadm7  -r 10.212.199.160 -u root -p calvin get bios.memsettings.nodeinterleave
/home/tops/bin/python   /usr/alisys/dragoon/libexec/hwqc/hwqc.py bios          查看BIOS设置
上面错误显示，目前BIOS层存在三个问题：
1. BIOS层的NUMA设置。OS层的NUMA已经设置好了，但是BIOS层没有进行修改，这个之前进行过测试，如果出现内存跨CPU访问，会对数据库的性能有较大影响。
2. BIOS层的功耗设置。OS层的功耗目前还是节能模式，在访问IO、内存等处理时，CPU会有一些延迟处理，对性能有较大影响。
3. BIOS层的Turbo Boost设置。Turbo Boost在BIOS层没有关闭。

https://linux.cn/article-6924-1.html   Linux 性能分析的前 60 秒
-->

常用工具介绍。

| 工具     | 简单介绍                                                    |
| :----:   | :--------                                                   |
| top      | 查看进程活动状态以及一些系统状况。                          |
| mpstat   | 查看多处理器状况                                            |
| vmstat   | 查看系统状态、硬件和系统信息等。                            |
| iostat   | 查看CPU 负载，硬盘状况                                      |
| pidstat  | 监控全部或指定进程，包括CPU、内存、IO、任务切换、线程等     |
| netstat  | 查看网络状况                                                |
| iptraf   | 实时网络状况监测                                            |
| tcpdump  | 抓取网络数据包，详细分析                                    |
| mpstat   | 查看多处理器状况                                            |
| tcptrace | 数据包分析工具                                              |
| netperf  | 网络带宽工具                                                |
| sar      | 综合工具，查看系统状况，可以记录历史状态                    |
| dstat    | 综合工具，综合了 vmstat, iostat, ifstat, netstat 等多个信息 |

在 CentOS 中，sysstat 包中包括了 sar、iostat、mpstat、pidstat 等。

## 经典

一些比较经典的参考文档，其实很多的性能监控会和调优绑定到一块。

* [18 Command Line Tools to Monitor Linux Performance](http://www.tecmint.com/command-line-tools-to-monitor-linux-performance/) 也可以参考 [中文版](http://os.51cto.com/art/201402/429890.htm) 。

* [Linux Performance Analysis and Tools.pdf](/reference/linux/monitor/Linux_Performance_Analysis_and_Tools.pdf) ，非常非常经典的调试图；也可以查看作者 [Brendan D. Gregg](http://www.brendangregg.com/) 的 blog 。

* 另外一些常见的经典文档， [Linux System and Performance Monitoring](/reference/linux/monitor/linuxcon2010-linux-monitoring.pdf)、[Linux Performance and Tuning Guidelines](/reference/linux/monitor/redp4285.pdf)。


{% highlight text %}
{% endhighlight %}
