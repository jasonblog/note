---
title: 通过 tcprstat 工具统计应答时间
layout: post
comments: true
category: [linux, database, mysql]
language: chinese
keywords: tcprstat,mysql,linux,响应时间
description: 一个不错的工具，由 Percona 开发，可以用来统计服务端的数据处理时间，包括了最大最小值等统计信息。只要是使用 TCP 协议，采用服务器-客户端的模式，都可以支持，如 MySQL 。
---

一个不错的工具，可以用来统计服务端的数据处理时间，包括了最大最小值等统计信息。

本文简单介绍该工具的使用方法及其实现原理。

<!-- more -->

![tcprstat logo]({{ site.url }}/images/databases/mysql/tcprstat-logo.jpg "tcprstat logo"){: .pull-center width="600px" }

## 简介

该工具是 Percona 提供的一个工具，通常用来监测 MySQL 响应时间的，或者说是请求在服务器端的处理时间，其输出结果包括了响应的很多统计值，用来诊断服务器端。

tcprstat 是一个基于 libpcap 的工具，通过提取 TCP libpcap 的捕获时间 (struct pcap_pkthd.ts) 用来计算统计值。也就是通过测量 TCP 的 request 和 response 所需的时间间隔，适用于一问一答式协议类型的处理。

这也就意味着，该工具不仅适用于 MySQL 协议，对于 HTTP 也同样适用。

### 源码安装

这个工具有点老了，从 [launchpad.net][source-code] 看，最近的一次更新是在 2010-09-01 :-( 不过还好，这个工具本身源码较少，我们直接通过源码进行安装。

首先从 [launchpad.net][source-code] 上下载源码，或者直接通过如下命令下载。

{% highlight text %}
$ bzr branch lp:tcprstat                       # 下载源码
{% endhighlight %}

在编译的时候，会生成两个程序，分别为 tcprstat、tcprstat-static ，其中后者为静态链接生成。这也就意味着，会依赖于 glibc-static 包；否则，编译静态文件时会报 ld: cannot find -lpthread 链接错误。

对于 CentOS，直接安装 glibc 静态库，然后通过如下方式下载编译。

{% highlight text %}
$ yum install glibc-static                     # 直接通过yum安装所需的静态库
$ bash bootstrap && bash configure && make     # 编译
{% endhighlight %}

在源码中包括了一份 libpcap 的源码，不过是比较老的版本。编译完成后，会在 src 目录下生成 tcprstat 及其静态版本 tcprstat-static。对于后者，该版本不会依赖其它库，可以直接复制到其它机器运行。


### 使用

该工具的使用也非常简单，其帮助文档可以参考 Percona 中的 [The tcprstat User Manual][offical-manual]。详细的使用参数可以通过 tcprstat -h 查看，其中常见的参数有：

* \-\-port PORT, -p PORT
    指定监听的端口号。

* \-\-format FORMAT, -f FORMAT
    指定输出信息的格式。

* \-\-interval Nseconds, -t Nseconds
    指定监听的时间间隔，默认是 10s 。

* \-\-iterations Ntimes, -n Ntimes
    指定循环的次数，默认是 1 次，0 表示永远循环。

* \-\-read FILE, -r FILE
    如果是离线分析，通过该参数指定具体文件，网络包可以通过 tcpdump -w FILE 获得。

其输出结果包括了时间戳，以及响应时间的最大值、均值、方差等信息，输出信息可以通过 -f 参数进行定制，其中响应时间的单位为微妙。

{% highlight text %}
# tcprstat -p 3306 -t 1 -n 5                      # 监听3306端口，每1秒输出一次，共输出5次
timestamp   count   max     min  avg  med  stddev  95_max  95_avg  95_std  99_max  99_avg  99_std
1283261499  1870    559009  39   883  153  13306   1267    201     150     6792    323     685
1283261500  1865    25704   29   578  142  2755    889     175     107     23630   333     1331
1283261501  1887    26908   33   583  148  2761    714     176     94      23391   339     1340
1283261502  2015    304965  35   624  151  7204    564     171     79      8615    237     507
1283261503  1650    289087  35   462  146  7133    834     184     120     3565    244     358

# tcprstat -l 192.168.52.129 -p 3306 -t 1 -n 0    # 设置本地地址为192.168.52.129，一直输出
{% endhighlight %}

对于 bonding 的网卡，会报如下的错误 pcap: SIOCGIFFLAGS: bonding_masters: No such device ，此时可以使用如下方式启动。

{% highlight text %}
# tcprstat -p 3306 -t 1 -n 0 -l \
    `/sbin/ifconfig | grep 'addr:[^ ]\+' -o | cut -f 2 -d : | xargs echo | sed -e 's/ /,/g'`
{% endhighlight %}

也就是用 IP 地址方式，而不是通过网络接口方式指定。




## 源码解析

与 tcpdump 一样，tcprstat 也使用 libpcap 库进行抓包，然后再通过程序对抓取的 tcp 包进行分析。估计之前很多发行版本不包括 libpcap ，所以在源码包中同时包含了一份 libpcap 的源码。

另外，需要注意的是，该命令会打开所有的设备，而且没有提供命令行参数指定监听的设备；可以通过修改如下源码指定只监听特定的设备。

{% highlight c %}
void * capture(void *arg) {
    ... ...
    //pcap = pcap_open_live("any", CAPTURE_LENGTH, 0, READ_TIMEOUT, errbuf);
    pcap = pcap_open_live("eth0", CAPTURE_LENGTH, 0, READ_TIMEOUT, errbuf);
    ... ...
}
{% endhighlight %}

代码的实现也非常简单，包括了在线和离线两种方式，在此只介绍前者。

如果没有使用 -r file 参数，也即使用在线分析，则会新建两个线程 output_thread() 和 capture()；其中前者用于显示统计信息值，后者为主要的抓包以及处理函数。

在 capture() 中会打开设备、设置过滤条件，做初始化操作；然后通过 libcap 进行抓包，而每次捕获的网络报文，会调用 process_packet() 函数处理。

![prcodess_ip_func]{: .pull-center width='650'}

如下为函数的调用逻辑。

{% highlight text %}
capture()                            # 捕获网络报文的函数
 |-pcap_loop()                       # libcap的循环处理函数
   |-process_packet()                # 主要处理函数
     |-process_ip()                  # 根据IP进行判断
       |-is_local_address()          # 通过该函数判断报文的近出
       |-inbound()                   # 判断是进来的数据包处理
       |-outbound()                  # 出去的数据包

output_thread()                      # 统计信息的输出线程
{% endhighlight %}

如上所述，process_packet() 是主要处理函数，在此再大致介绍下其处理流程如下：

1. 分析来源 IP 和目标 IP，看那个 IP 是本地的，来判断是进来的包（请求包）还是出去的包（响应包）。

2. 如果包的数据大小为 0，那么就跳过，不再处理。数据大小为 0 的视为 TCP 控制包。

3. 如果数据包为进来的包（请求包），则插入一条记录到哈希表。

4. 如果数据包为出去的包（响应包），则用现在的包和之前插入哈希表中的响应包做时间差计算，并把之前的包在哈希表中删除。


### 本地报文(无效)

需要注意的是，如果是在本地通过 MySQL 客户端登陆，则该工具实际是无效的。

问题出在 is_local_address() 函数中，无法判断是响应包还是请求包，因为报文的 IP 均为 127.0.0.1 。可以在 process_ip() 中添加如下内容查看。

{% highlight c %}
int process_ip(pcap_t *dev, const struct ip *ip, struct timeval tv) {
    ... ...   // 查看每个报文的源和目的地址
    printf("src adddress %s\n", inet_ntoa(ip->ip_src));
    printf("dst adddress %s\n", inet_ntoa(ip->ip_dst));
    ... ...
}
{% endhighlight %}

而获取本地地址，有两种方式，分别为通过 get_address() 函数自动获取；或者通过 -l 参数指定。对于前者的值，可以通过在函数末尾添加如下内容查看。

{% highlight c %}
int get_addresses(void) {
    ... ...
    struct address_list *tmp;
    for (tmp = address_list.next; tmp; tmp = tmp->next) {
        printf("local adddress %s\n", inet_ntoa(tmp->in_addr));
    }
    return 0;
}
{% endhighlight %}

不过对于本地的链接，即使通过 -l 127.1 设置了 IP，在进行判断时都会被判断为 income，从而也是无效的。

对于这种情况，可以通过 ifconfig 设置浮动 IP 地址 (一般在127.0.0.1/8网段)，然后服务监听该 IP 地址，启动 tcprstat 时，使用 -l 参数指定本地 IP 地址。


## 参考

tcprstat 的官方文档 [The tcprstat User's Manual][offical-manual]，介绍了该工具的使用方法；也可以从 [本地](/reference/mysql/tcprstat.tar.bz2) 下载。

[source-code]:        https://launchpad.net/tcprstat                     "保存在launchpad上的源码"
[offical-manual]:     https://www.percona.com/docs/wiki/tcprstat:start   "The tcprstat User's Manual"
[prcodess_ip_func]:   /images/linux/tcprstat_process_ip.png              "process_ip()函数的处理流程"
