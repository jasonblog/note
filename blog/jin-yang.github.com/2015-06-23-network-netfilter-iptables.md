---
title: Linux 的防火墙
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: linux,防火墙,netfilter,iptables
description: Linux 防火墙是由 Netfilter 和 iptables 两个组件组成，其中前者在内核空间中；后者是一个用户程序，用于插入、修改和删除相应的防火墙规则。通过 Netfilter+iptables 可以实现数据包过滤、网络地址转换和数据报文处理等功能。
---

Linux 防火墙是由 Netfilter 和 iptables 两个组件组成，其中前者在内核空间中；后者是一个用户程序，用于插入、修改和删除相应的防火墙规则。

通过 Netfilter+iptables 可以实现数据包过滤、网络地址转换和数据报文处理等功能。

<!-- more -->

## Netfilter 简介

Netfilter 框架用来在网络协议栈的内核路径上过滤数据包，就像一条路上的关卡，一个数据包在被处理的路径上在经过这些关卡时会被检查，如果符合条件就进行处理，结果就是若干个动作，包括接受、丢弃、排队、导入其它路径等。

框架只需针对一个数据包得出一个结果即可，关卡内部提供什么服务在Netfilter框架中并没有任何规定。

Netfilter 是一个设计良好的框架，之所以说它是一个框架是因为它提供了最基本的底层支撑，而对于实现的关注度却没有那么高，这种底层支撑实际上就是其 5 个 HOOK 点。

各个 hook 点其实就是固定的“检查点”，这些检查点是内嵌于网络协议栈的，它将检查点无条件的安插在协议栈中，这些检查点的检查是无条件执行的。

![netfilter]({{ site.url }}/images/linux/netfilter_packet_flow.png "netfiler"){: .pull-center }

如上图所示，Netfilter 包括了五个检查节点。

1. PREROUTING：报文进入网络层但是还未到达路由前
2. FORWARD：报文经过路由后，确定需要转发之后
3. INPUT：报文经过路由之后，确定要本地接收之后
4. OUTPUT：本地数据包发送
5. POSTROUTING：数据包通过本机马上发出去之前

在内核中，通过如下的宏定义。

{% highlight text %}
#define NF_IP_PRE_ROUTING   0
#define NF_IP_LOCAL_IN      1
#define NF_IP_FORWARD       2
#define NF_IP_LOCAL_OUT     3
#define NF_IP_POST_ROUTING  4
{% endhighlight %}

## iptables

该工具在 CentOS 中是默认安装，如果没有可以通过 ```yum install iptables``` 安装。

### 示例

关于添加、除去、编辑规则的命令的一般语法如下。

{% highlight text %}
iptables [-t table] command [chain] [match] [target]

----- command，指定管理规则的方式，如插入、增加、删除以及查看等
-A, --append  该命令将一条规则附加到链的末尾
-L, --list    列出指定链中的所有规则，默认查看filter表，要查看NAT表，可加上-t NAT参数

----- match，条件匹配分为基本匹配和扩展匹配，拓展匹配又分为隐式扩展和显示扩展
-p  指定规则协议，如tcp, udp,icmp等，可以使用all来指定所有协议
-s  指定数据包的源地址参数，可以使IP地址、网络地址、主机名
-d  指定目的地址
-i  输入接口
-o  输出接口
{% endhighlight %}

### 常用操作

首先是查看当前的记录，默认查看的是 filter 表，如果要查看 NAT 表，可以加上 -t nat 参数。

{% highlight text %}
# iptables --line-number -nvL POSTROUTING -t nat

如上参数包括：
   -n    不对IP进行反查，显示速度会快很多
   -v    输出详细信息，包括通过该规则的数据包数量、总字节数、相应的网络接口
   --line-number   显示规则的序列号，这个参数在删除或修改规则时会用到
{% endhighlight %}

添加规则有两个参数：1) -A 添加到规则的末尾；2) -I 插入到指定位置，默认插入到规则的首部。

{% highlight text %}
----- 添加一条规则到尾部
# iptables -A INPUT -s 192.168.1.5 -j DROP

----- 插入一条规则到第三行
# iptables -I INPUT 3 -s 192.168.1.3 -j DROP
{% endhighlight %}

结下来可以通过如下方式删除。

{% highlight text %}
----- 删除之前添加的规则
# iptables -D INPUT -s 192.168.1.5 -j DROP

----- 如果规则太长，直接通过行号删除，如下删除第2行
# iptables -D INPUT 2
{% endhighlight %}

接下来看看如何修改规则。

{% highlight text %}
----- 接下来先查找对应的规则号
# iptables -nL --line-number
Chain INPUT (policy ACCEPT)
num  target     prot opt source               destination
1    DROP       all  --  192.168.1.1          0.0.0.0/0

----- 然后将第一条规则改为ACCEPT
# iptables -R INPUT 1 -j ACCEPT
{% endhighlight %}

#### 持久化保存

设置完成之后，可以直接通过 ```iptables-save``` 保存为文件，默认打印到 stdout 中。

{% highlight text %}
----- RHEL/CentOS
# iptables-save > /etc/sysconfig/iptables
# iptables-restore < /etc/sysconfig/iptables
----- Debian/Ubuntu
# iptables-save > /etc/iptables/rules.v4
# iptables-restore < /etc/iptables/rules.v4
{% endhighlight %}

注意，如果使用了 IPv6 则对应 ```ip6tables-save``` 命令以及 ```/etc/iptables/rules.v6```、```/etc/sysconfig/ip6tables``` 。


### 常用实例

下面是常用的示例。

{% highlight text %}
----- 允许访问22端口，如果需要拒绝访问，则将ACCEPT改为DROP即可
# iptables -A INPUT -p tcp --dport 22 -j ACCEPT
----- 允许192.168.1.123访问本机的1521端口
# iptables -I INPUT -s 192.168.1.123 -p tcp --dport 1521 -j ACCEPT
-A INPUT -s 192.30.19.82/32 -p tcp -m tcp --dport 21005 -j ACCEPT
----- 在指定位置插入，如果时-A参数则是在末尾添加
# iptables -I INPUT 3 -p tcp -m tcp --dport 20 -j ACCEPT

----- 添加NAT规则，对192.168.10.0/24进行地址伪装
# iptables -t nat -A POSTROUTING -s 192.168.10.0/24 -j MASQUERADE

----- 查看filter表规则，显示编号，常用于删除
# iptables -L -n --line-number
----- 查看NAT表规则
# iptables -t nat -vnL POSTROUTING --line-number

----- 修改规则，改为DROP
# iptables -R INPUT 3 -j DROP

----- 删除input的第3条规则
# iptables -D INPUT 3
----- 删除nat表中postrouting的第一条规则
# iptables -t nat -D POSTROUTING 1
----- 清空filter表INPUT所有规则
# iptables -F INPUT
----- 清空所有规则
# iptables -F
----- 清空nat表POSTROUTING所有规则
# iptables -t nat -F POSTROUTING

---- 设置filter表INPUT默认规则是DROP
# iptables -P INPUT DROP
{% endhighlight %}


## 其它

目前除了 iptables 之外，还有 [nf-hipac](http://www.hipac.org/index.html) 。曾经有网友测试过，nf-hipac 比 iptables 优化的不是一星半点，而且前者在加载 20000 个条目时，使用 iperf 的测试结果基本根裸奔相差无几。不过该项目基本已经停滞，现在是通过 [ipset](http://ipset.netfilter.org/) 替换之 ^_^

目前有个 [Netfilter](http://www.netfilter.org/projects/nftables/index.html) 的项目，用来取代现有的 {ip,ip6,arp,eb}tables 等工具，不过现在还在开发过程中。

另外，内核中提供了 BPF (BSD Packet Filter) 支持，包括现在的 tcpdump 等程序，都是使用的这一规则。据说，其速度是 iptables 的几倍，采用综合状态机、JIT，可以在很短的时间内完成数据包的匹配判断，可以参考 [eBPF: One Small Step](http://www.brendangregg.com/blog/2015-05-15/ebpf-one-small-step.html) 。










<!--

## NAT, Network Address Translation

网络地址转换包括了两类典型的应用：SNAT (Source NAT) 和 DNAT (Destination NAT)；另外，还有一种被称为地址伪装 (MASQUERADE)，不过其功能与 SNAT 相似。

两者分别用于实现企业局域网共享上网、在 Internet 中发布内网的应用服务器。

### 源地址转换

修改数据包的源地址。源NAT改变第一个数据包的来源地址，它永远会在数据包发送到网络之前完成，数据包伪装就是一具SNAT的例子。

随着 Internet 网络在全世界范围内的快速发展， IPv4 协议支持的可用 IP 地址资源逐渐变得山穷水尽，资源匮乏使得很多企业难以申请更多的公网 IP 地址，或者只能承受一个或者少数几个公网 IP 地址的费用。而与此同时，大部分企业都面临着将局域网的主机接入 Internet 的需求。使用 iptables 的 SNAT 策略，可以基本化解这个难题。

{% highlight text %}
+----------------+
| 172.18.0.20/16 |
+----------------+

+----------------+
| 172.18.0.20/16 |
+----------------+
{% endhighlight %}





### 目的地址转换

(2)目的NAT（Destination NAT，DNAT）：修改数据包的目的地址。Destination NAT刚好与SNAT相反，它是改变第一个数据懈的目的地地址，如平衡负载、端口转发和透明代理就是属于DNAT










MASQUERADE


-A, --append






　　SNAT是source network address translation的缩写，即源地址目标转换
　　比如，多个PC机使用ADSL路由器共享上网，每个PC机都配置了内网IP
　　PC机访问外部网络的时候，路由器将数据包的报头中的源地址替换成路由器的ip
　　当外部网络的服务器比如网站web服务器接到访问请求的时候
　　他的日志记录下来的是路由器的ip地址，而不是pc机的内网ip
　　这是因为，这个服务器收到的数据包的报头里边的“源地址”，已经被替换了
　　所以叫做SNAT，基于源地址的地址转换

　　DNAT是destination network address translation的缩写
　　即目标网络地址转换
　　典型的应用是，有个web服务器放在内网配置内网ip，前端有个防火墙配置公网ip
　　互联网上的访问者使用公网ip来访问这个网站
　　当访问的时候，客户端发出一个数据包
　　这个数据包的报头里边，目标地址写的是防火墙的公网ip
　　防火墙会把这个数据包的报头改写一次，将目标地址改写成web服务器的内网ip
　　然后再把这个数据包发送到内网的web服务器上
　　这样，数据包就穿透了防火墙，并从公网ip变成了一个对内网地址的访问了
　　即DNAT，基于目标的网络地址转换

　　MASQUERADE，地址伪装，在iptables中有着和SNAT相近的效果，但也有一些区别
　　但使用SNAT的时候，出口ip的地址范围可以是一个，也可以是多个，例如：
　　如下命令表示把所有10.8.0.0网段的数据包SNAT成192.168.5.3的ip然后发出去
　　iptables -t nat -A POSTROUTING -s 10.8.0.0/255.255.255.0 -o eth0 -j SNAT --to-source 192.168.5.3
　　如下命令表示把所有10.8.0.0网段的数据包SNAT成192.168.5.3/192.168.5.4/192.168.5.5等几个ip然后发出去
　　iptables -t nat -A POSTROUTING -s 10.8.0.0/255.255.255.0 -o eth0 -j SNAT --to-source 192.168.5.3-192.168.5.5
　　这就是SNAT的使用方法，即可以NAT成一个地址，也可以NAT成多个地址
但是，对于SNAT，不管是几个地址，必须明确的指定要SNAT的ip
　　假如当前系统用的是ADSL动态拨号方式，那么每次拨号，出口ip192.168.5.3都会改变
　　而且改变的幅度很大，不一定是192.168.5.3到192.168.5.5范围内的地址
　　这个时候如果按照现在的方式来配置iptables就会出现问题了
　　因为每次拨号后，服务器地址都会变化，而iptables规则内的ip是不会随着自动变化的
　　每次地址变化后都必须手工修改一次iptables，把规则里边的固定ip改成新的ip
　　这样是非常不好用的
　　MASQUERADE就是针对这种场景而设计的，他的作用是，从服务器的网卡上，自动获取当前ip地址来做NAT
　　比如下边的命令：
　　iptables -t nat -A POSTROUTING -s 10.8.0.0/255.255.255.0 -o eth0 -j MASQUERADE
　　如此配置的话，不用指定SNAT的目标ip了
　　不管现在eth0的出口获得了怎样的动态ip，MASQUERADE会自动读取eth0现在的ip地址然后做SNAT出去
　　这样就实现了很好的动态SNAT地址转换











NAT有三种类型：静态NAT(Static NAT)、动态地址NAT(Pooled NAT)、网络地址端口转换NAPT（Port-Level NAT）
其中，网络地址端口转换NAPT（Network Address Port Translation）则是把内部地址映射到外部网络的一个IP地址的不同端口上。它可以将中小型的网络隐藏在一个合法的IP地址后面。NAPT与 动态地址NAT不同，它将内部连接映射到外部网络中的一个单独的IP地址上，同时在该地址上加上一个由NAT设备选定的端口号.NAPT是使用最普遍的一种转换方式
,它又细分为snat和dnat.










## Connection Tracking

nf_conntrack 是一个内核模块，用于跟踪一个连接的状态信息，其中连接状态的跟踪信息可以供其它模块使用，最常见的两个使用场景是 iptables 的 nat 和 state 模块。

其中，iptables 的 NAT 通过规则来修改目的或者源地址，除了修改地址之外，还要借助 nf_conntrack 找到原来的那个连接记录，从而能让回来的包可以路由到最初的源主机。

另外，iptables 的 state 模块会直接使用 nf_conntrack 中记录的连接状态来匹配用户定义的相关规则。例如，下面的规则用于放行 80 端口上的状态为 NEW 的连接。

{% highlight text %}
# iptables -A INPUT -p tcp -m state --state NEW -m tcp --dport 80 -j ACCEPT
{% endhighlight %}

目前使用的较多的是 nf_conntrack，老版本采用的是 ip_conntrack 模块，所以，之前的 ip_conntrack_XXX 已经被 nf_conntrack_XXX 取代，包括 procfs、内核源码中的定义。例如，之前的 /proc/net/ip_conntrack，现在为 /proc/net/nf_conntrack 。

nf_conntrack 工作在 3 层，可以同时支持 IPv4 和 IPv6，而 ip_conntrack 只支持 IPv4 。

<br>

除了直接查看 /proc/net/ip_conntrack 之外，用户层提供了一个 conntrack-tools 工具包。









### nf_conntrack 队列满

此时的现象是部分客户端连不上服务器，可以在 /var/log/{dmesg,messages} 文件中看到类似如下的报错。

{% highlight text %}
kernel: nf_conntrack: table full, dropping packet
{% endhighlight %}

在 /proc/net/nf_conntrack 中，保存了连接跟踪数据库 (conntrack database)，最常见的两个场景是 iptables 中的 NAT 以及 state 模块。

通常用来做 NAT 转换时的连接跟踪，如果满了，肯定会有问题。

{% highlight text %}
----- 1. 当前跟踪的链接数
# wc -l /proc/net/nf_conntrack

----- 2. 查看最大值限制，可以适当调大
# cat /proc/sys/net/netfilter/nf_conntrack_max

----- 3. 保留时间，单位时秒，默认保留5天，可以调小
# cat /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established
{% endhighlight %}

nf_conntrack 文件中保存了跟踪链接的相关信息，该文件的输出在 net/netfilter/nf_conntrack_core.c 中实现，对应 ct_seq_show() 函数。

该文件中的相关信息可以参考 [Connection tracking][connection-tracking]，有比较详细的介绍。



## 参考


iptables使用
http://blog.51yip.com/linux/1404.html






[connection-tracking]:        http://www.rigacci.org/wiki/lib/exe/fetch.php/doc/appunti/linux/sa/iptables/conntrack.html
[connection-tracking-local]:  /reference/linux/network/conntrack.html




## 源码解析

对于源码中的接口，可以直接通过 grep -rne 'NF_HOOK.\*NFPROTO\_IPV4'  net/* 查找。


http://blog.csdn.net/lickylin/article/details/33020731
http://blog.csdn.net/lickylin/article/details/32387217
http://drops.wooyun.org/tips/1424


http://bbs.chinaunix.net/forum.php?mod=viewthread&tid=3749208&fromuid=20171559          linux-2.6.35.6内核netfilter框架
http://blog.csdn.net/godleading/article/details/19972253                                Linux nf_conntrack连接跟踪的实现
http://bbs.chinaunix.net/thread-2008344-1-1.html                                        Netfilter实现机制分析
http://bbs.chinaunix.net/thread-2005999-1-1.html                                        Netfilter之连接跟踪实现机制初步分析
http://bbs.chinaunix.net/thread-1750611-1-1.html                                        Netfilter机制分析
http://inai.de/documents/Netfilter_Modules.pdf                                          Writing Netfilter modules

# xxx
在 netfilter.h 头文件中会对 NF_HOOK() 内联函数进行定义，在该函数中会调用 nf_hook_thresh()，最终会调用 nf_hook_slow() 函数。
{% highlight c %}
struct nf_hook_ops {
    struct list_head list;       // 保存了链表
    nf_hookfn       *hook;       // 以及相应的HOOK函数
    struct module   *owner;
    void            *priv;
    u_int8_t         pf;
    unsigned int     hooknum;
    int              priority;
};
{% endhighlight %}
在 nf_iterate() 函数中，会递归搜索 list 并执行 hook 函数。
{% highlight text %}
nf_hook_slow()
 |-nf_iterate()           # 递归调用注册的钩子函数
   |-elem->hook()
{% endhighlight %}







-->




{% highlight text %}
{% endhighlight %}
