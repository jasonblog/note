---
title: Linux LVS
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,lvs
description: LVS 通常用于四层的网络服务器做负载均衡，其工作在内核层，而且效率非常高；部署时还会使用 heartbeat 以及 keepalived 做高可用。在本文中，我们对其做些介绍。
---

LVS 通常用于四层的网络服务器做负载均衡，其工作在内核层，而且效率非常高；部署时还会使用 heartbeat 以及 keepalived 做高可用。

在本文中，我们对其做些介绍。

<!-- more -->

## Linux Virtual Server Project

负载均衡设备通常作为应用服务器的入口，简单来说就是对于一个请求，如何选择其中的一个服务器去处理该请求，如下是 Google SRE 中的定义。

{% highlight text %}
Traffic load balancing is how we decide which of the many, many machines
in our datacenters will serve a particular request.
{% endhighlight %}

LVS 工作在内核层，可以处理 TCP 以及 UDP 连接，也就是是说，只要是基于这两个协议的都支持，包括了 email、http、https 等服务，甚至包括 X-Windows 。


仍然以 Amazon 为例。

![network interface]({{ site.url }}/images/network/lb-overview.png){: .pull-center }

性能好的是商用硬件负载设备，但是成本通常很高，一台能达到几十万甚至上百万，所以通常使用的是软负载均衡，而软负载的两个核心问题是：选谁、转发，其中最著名的是 LVS (Linux Virtual Server Project)。

## 简介

实际上很多层都可以做负载均衡，例如网卡的 Bond、ECMP、DNS 等等，而针对 VIP 的通常工作在网络模型的第四层以及第七层。

![network interface]({{ site.url }}/images/network/network-introduce-OSI-TCP-IP.png){: .pull-center }

软负载均衡面向的是一个已经建立连接的会话，而非单独的 IP 报文，也就是说用户访问需要 TCP 协议与服务器建立连接传输数据，而 TCP 是在第四层实现的，如果在第三层做负载均衡，那么将会丢失四层的 "连接"，导致出错。

LVS 工作在网络模型中的第四层，不同于 HAProxy、Nginx 等七层软负载均衡可以面向 HTTP 包，从而七层可以做 URL 的解析工作，而 LVS 无法完成。

## 报文转发

首先负载均衡中包括了 Director (提供 LVS 服务的机器)、Real Server (真正提供服务的机器) 。LVS 支持三种报文的转发方式 NAT、tunneling、direct routing，分别介绍如下：

### Network Address Translation (NAT)

NAT 也就是修改报文的源或者目的地址以及端口号，是一种外网和内网的地址映射技术，对于这种模式，报文的进出都要经过 LVS 服务器，也就意味这 RS 需要将 RS 作为网关。

![network interface]({{ site.url }}/images/network/lvs-nat-example-2.png){: .pull-center }

一个报文的处理过程大致经过了如下的三个步骤：

1. Director 收到报文后会做 DNAT，也就是修改目的地址以及端口号为真正的服务器，然后发送到 RS；

2. RS 接收到包后，就像是客户端直接发给它的一样，不会感知到 LVS 的存在。响应报文的源 IP 是 RS IP，目标 IP 是客户端的 IP。

3. 响应报文通过 LVS 中转，会做 SNAT，将包的源地址改为 VIP，这样对客户端来说，看起来就像是 LVS 直接返回给它的，客户端无法感知到后端 RS 的存在。

下图是数据流的流动过程。

![network interface]({{ site.url }}/images/network/lvs-nat.png){: .pull-center }

这是最见的配置方式了，假设有如下环境。

![network interface]({{ site.url }}/images/network/lvs-nat-example.png){: .pull-center }

只需要做如下的配置。

{% highlight text %}
----- 首先配置 Linux Director，其中VIP为172.17.60.201
---++ 在/etc/sysctl.conf中设置IP转发
net.ipv4.ip_forward = 1
---++ 设置虚拟IP地址
# ifconfig eth0:0 172.17.60.201 netmask 255.255.0.0 broadcast 172.17.255.255
---++ 配置LVS
# ipvsadm -A -t 172.17.60.201:80
# ipvsadm -a -t 172.17.60.201:80 -r 192.168.6.4:80 -m
# ipvsadm -a -t 172.17.60.201:80 -r 192.168.6.5:80 -m

----- 然后配置Real Servers，只需要将默认的gateway设置为VIP即可
{% endhighlight %}

然后可以通过如下方法查看当前的统计值。

{% highlight text %}
# ipvsadm -L -n              # 查看当前活动的连接
# ipvsadm -L -n --stats      # 每秒收发的报文数
# ipvsadm -L -n --rate       # 每秒的报文的收发速度
# ipvsadm -L --zero          # 清空报文内容
{% endhighlight %}


### Direct Routing

此时请求由 LVS 接受，然后报文会直接通过修改目的 MAC 发送给 RealServer，其中 IP 报文不会修改，而响应报文则会由 RS 直接返回给用户，不会经过 LVS，这是性能最好的一种模式。

这种方式同时这也就意味着 RS 必须也配置为 VIP，通常是通过 loopback 设备绑定。

简单来说，一个请求总共分为如下三步：

1.  LVS 接收请求，将接收到报文的 MAC 地址修改为某台 RS 的 MAC，该包就会在 LAN 中被转发到相应的 RS 处理，而此时的源 IP 和目标 IP 都没变。

2. RS 收到 LVS 转发来的包，链路层通过 MAC 识别到是发送给自己的，然后传递到上面的网络层，发现 IP 同样是自己的，那么报文就继续向上传递，这个包被合法地接受，RS 并没有感知到 LVS 的存在。

3. RS 返回响应报文时，直接向源 IP 返回即可，此时不再经过 LVS，从而降低 LVS 服务器的压力。

基本操作如下图所示。

![network interface]({{ site.url }}/images/network/lvs-direct-routing.png){: .pull-center }


对于 DR 模式，需要注意如下几点：

1. LVS 的 VIP 和 realserver 必须在同一个网段。

2. 所有的 realserver 都必须绑定 VIP 的 IP 地址，否则 realserver 收到报文后发现目的 IP 不是自己，将会把报文丢掉。

3. realserver 处理完包后直接把报文发送给客户端，从而不需要再经过 LVS 服务器，进而提高效率，不过同时会将 realserver 的 IP 暴漏给外界。

假设有如下的网络结构。

![network interface]({{ site.url }}/images/network/lvs-direct-routing-example.png){: .pull-center }

![network interface]({{ site.url }}/images/network/lvs-direct-routing-example-2.png){: .pull-center }

配置方法如下：

{% highlight text %}
----- 首先配置 Linux Director，其中VIP为172.17.60.201
---++ 在/etc/sysctl.conf中设置IP转发
net.ipv4.ip_forward = 1
---++ 设置虚拟IP地址
# ifconfig eth0:0 172.17.60.201 netmask 255.255.0.0 broadcast 172.17.255.255
---++ 配置LVS
# ipvsadm -A -t 172.17.60.201:80
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.199:80 -g
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.200:80 -g

----- 在真正服务器上执行如下命令
---++ 需要注意netmask
# ifconfig lo:0 172.17.60.201 netmask 255.255.255.255
---++ 隐藏loopback，从而防止相应对VIP的ARP请求
# Enable configuration of hidden devices
net.ipv4.conf.all.hidden = 1
# Make the loopback interface hidden
net.ipv4.conf.lo.hidden = 1
{% endhighlight %}


### IP-IP Encapsulation (Tunnelling)

这种方法可以将报文发送到不同的子网内，同 DR 类似，返回的报文不需要经过 director 。示例仍然采用与 DR 相同的结构。

![network interface]({{ site.url }}/images/network/lvs-direct-routing-example.png){: .pull-center }

然后通过如下方式进行配置。

{% highlight text %}
----- 首先配置Linux Director
---++ 在/etc/sysctl.conf中设置IP转发
net.ipv4.ip_forward = 1
---++ 设置虚拟IP地址
# ifconfig eth0:0 172.17.60.201 netmask 255.255.0.0 broadcast 172.17.255.255
---++ 配置LVS
# ipvsadm -A -t 172.17.60.201:80
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.199:80 -i
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.200:80 -i

----- 配置Real Servers
---++ 配置tunl0
# ifconfig tunl0 172.17.60.201 netmask 255.255.255.255
---++ 同样打开转发，并隐藏loopback
net.ipv4.ip_forward = 1
# Enable configuration of hidden devices
net.ipv4.conf.all.hidden = 1
# Make the tunl0 interface hidden
net.ipv4.conf.tunl0.hidden = 1
{% endhighlight %}



### Full NAT

除了上述的 IP-IP Tunnelling 方式，LVS 和 RS 必须在同一个 LAN 或者 VLAN 下，否则 LVS 无法作为 RS 的网关。这将导致：A) 运维不方便，跨 VLAN 的 RS 无法接入；B) 水平扩展受限制。

Full-NAT 是为了解决此问题，他在 SNAT/DNAT 的基础上，加上另一种转换，过程如下。

![network interface]({{ site.url }}/images/network/lvs-full-nat-example.png){: .pull-center }

大致的执行过程如下：

1. 包从 LVS 转到 RS 时，源地址从客户端 IP 被替换成了 LVS 的内网 IP；利用内网 IP，可以通过多个交换机跨 VLAN 通信。

2. 当 RS 返回响应包时，会将这个包返回给 LVS 的内网 IP，这一步也不受限于 VLAN。

3. LVS 收到包后，在修改源地址的基础上，再把 RS 发来的包中的目标地址从 LVS 内网 IP 改为客户端的 IP。

采用这种方式，LVS 和 RS 的部署在 VLAN 上将不再有任何限制，大大提高了运维部署的便利性。





## 负载均衡算法

针对不同的网络服务需求和服务器配置，IPVS 调度器实现了如下八种负载调度算法：

以及八种负载均衡的算法 (round robin、weighted round robin、least-connection、weighted least-connection、locality-based least-connection、locality-based least-connection with replication、destination-hashing、source-hashing)。

#### 轮询 (Round Robin)

调度器将外部请求按顺序轮流分配到集群中的真实服务器上，该算法平等地对待每一台服务器，而不管服务器上实际的连接数和系统负载。

#### 加权轮询 (Weighted Round Robin)

该算法可以根据真实服务器的不同处理能力来调度访问请求，从而可以保证处理能力强的服务器处理更多的访问流量，而调度器可以自动查询真实服务器的负载情况，并动态地调整其权值。

#### 最少链接 (Least Connections)

调度器动态地将网络请求调度到已建立的链接数最少的服务器上，如果集群系统的真实服务器具有相近的系统性能，采用 "最小连接" 调度算法可以较好地均衡负载。

#### 加权最少链接 (Weighted Least Connections)

在集群系统中的服务器性能差异较大的情况下，调度器采用 "加权最少链接" 调度算法优化负载均衡性能，具有较高权值的服务器将承受较大比例的活动连接负载。调度器可以自动问询真实服务器的负载情况，并动态地调整其权值。

#### 基于局部性的最少链接 (Locality-Based Least Connections)

针对目标 IP 地址的负载均衡，目前主要用于 Cache 集群系统。该算法根据请求的目标IP地址找出该目标IP地址最近使用的服务器，若该服务器是可用的且没有超载，将请求发送到该服务器；若服务器不存在，或者该服务器超载且有服务器处于一半的工作负载，则用"最少链接"的原则选出一个可用的服务器，将请求发送到该服务器。

#### 带复制的基于局部性最少链接 (Locality-Based Least Connections with Replication)

"带复制的基于局部性最少链接"调度算法也是针对目标IP地址的负载均衡，目前主要用于Cache集群系统。它与LBLC算法的不同之处是它要维护从一个目标IP地址到一组服务器的映射，而LBLC算法维护从一个目标IP地址到一台服务器的映射。该算法根据请求的目标IP地址找出该目标IP地址对应的服务器组，按"最小连接"原则从服务器组中选出一台服务器，若服务器没有超载，将请求发送到该服务器，若服务器超载；则按"最小连接"原则从这个集群中选出一台服务器，将该服务器加入到服务器组中，将请求发送到该服务器。同时，当该服务器组有一段时间没有被修改，将最忙的服务器从服务器组中删除，以降低复制的程度。 7.目标地址散列（Destination Hashing）

"目标地址散列"调度算法根据请求的目标IP地址，作为散列键（Hash Key）从静态分配的散列表找出对应的服务器，若该服务器是可用的且未超载，将请求发送到该服务器，否则返回空。

#### 源地址散列 (Source Hashing)

"源地址散列"调度算法根据请求的源IP地址，作为散列键（Hash Key）从静态分配的散列表找出对应的服务器，若该服务器是可用的且未超载，将请求发送到该服务器，否则返回空。



## ipvsadm

ipvsadm 是用户层管理 LVS 的程序，

{% highlight text %}
ipvsadm -A|E -t|u|f service-address [-s scheduler] [-p [timeout]] [-M netmask]
ipvsadm -D -t|u|f service-address
ipvsadm -C
ipvsadm -R
ipvsadm -S [-n]
ipvsadm -a|e -t|u|f service-address -r RS-address [-g|i|m] [-w weight] [-x upper] [-y lower]
ipvsadm -d -t|u|f service-address -r RS-address
ipvsadm -L|l [options]
ipvsadm -Z [-t|u|f service-address]

常见选项如下：
    -A    添加一个新的集群服务；
    -E    修改一个己有的集群服务；
    -D    删除指定的集群服务；
    -a    向指定的集群服务中添加real server及属性；
    -e    修改real server属性；
    -r    指定real server的地址；

    -t    指定为tcp协议；
    -u    指定为udp协议；

    -g    Gateway, DR模型；
    -i    ipip, TUN模型；
    -m    masquerade, NAT模型；


    -f    指定防火墙标记码，通常用于将两个或以上的服务绑定为一个服务进行处理时使用；

    -s    调度方法，默认为wlc；



    -w    指定权重，默认为1；
    -p timeout   persistent connection, 持久连接超时时长；
    -S    保存ipvsadm设定的规则策略，默认保存在/etc/sysconfig/ipvsadm中；
    -R    载入己保存的规则策略，默认加载/etc/sysconfig/ipvsadm；
    -C    清除所有集群服务；
    -Z    清除所有记数器；
    -L    显示当前己有集群服务，能通过相应的options查看不同状态信息；
        -n: 数字格式显示IP地址；
        -c: 显示连接数相关信息；
        --stats: 显示统计数据；
        --rate: 速率；
        --exact：显示统计数据的精确值；


ipvsadm -A -t 192.168.132.254:80 -s rr -p 120
ipvsadm -a -t 192.168.132.254:80 -r 192.168.132.64:80 -g
ipvsadm -a -t 192.168.132.254:80 -r 192.168.132.68:80 -g

{% endhighlight %}

<!--
我们可以看到，此规则为一个VS ： 192.168.132.254：80， 两个real server 分别是 192.168.132.64：80 与 192.168.132.68：80， LVS模式为DR，调度算法为Round Robin。
-->


## 参考

有两篇不错的文章 [Linux Virtual Server Tutorial](http://www.ultramonkey.org/papers/lvs_tutorial/html/)、[LVS - HOWTO](http://www.austintek.com/LVS/LVS-HOWTO/HOWTO/index.html)，比较老的文章，前者相比简单些。另外 [ja.ssi.bg](http://ja.ssi.bg/) 中很多不错的相关补丁，以及文章。

在 [LVS 中文网站](http://zh.linuxvirtualserver.org/) 中有几篇比较经典的文章 [Linux服务器集群系统](http://www.linuxvirtualserver.org/zh/lvs1.html) 。

<!--
http://blog.csdn.net/raintungli/article/details/39051435
http://junwang.blog.51cto.com/5050337/1439428/
http://blog.csdn.net/pi9nc/article/details/23380589
http://www.it165.net/admin/html/201401/2248.html
http://blog.chinaunix.net/uid-11207493-id-4617479.html


真正零停机 HAProxy 重载
http://www.oschina.net/translate/zero-downtime-haproxy-reloads
-->

{% highlight text %}
{% endhighlight %}
