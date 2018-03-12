---
title: Linux 网络设置
layout: post
comments: true
category: [linux, network]
language: chinese
keywords: linux,网络设置
description: 主要记录下在 Linux 中，一些常见的网络配置，例如 IP 地址、路由、MAC 地址、主机名等设置方式。
---

主要记录下在 Linux 中，一些常见的网络配置，例如 IP 地址、路由、MAC 地址、主机名等设置方式。

<!-- more -->

## 常见配置

简单列举一些常见的网络配置。

{% highlight text %}
# ifconfig eth0 down                                      ← 关闭网络设备
# ifconfig eth0 hw ether xx:xx:xx:xx:xx:xx                ← 设置MAC地址
# ifconfig eht0 up
# ifconfig eth0 210.72.137.115 netmask 255.255.255.0      ← 配置IP
# route add default gw 192.168.0.100 dev eth0             ← 添加默认路由

# ifconfig eth0:0 210.72.137.115 netmask 255.255.255.0 up ← 设置VIP
# ifconfig eth0:0 down                                    ← 关闭VIP配置
{% endhighlight %}

如果 IP 和 GW 不在同一网段会出现 SIOCADDRT:No such process. 的错误，可以在上一条命令之前添加如下命令。

{% highlight text %}
# route add -host 192.168.0.100 [netmask 0.0.0.0] dev eth0
{% endhighlight %}

对于 ARP 可以通过如下方式查看。

{% highlight text %}
# arp -n                                               ← 查看当前系统的ARP表
# arp -s 10.15.72.25 408:d0:9f:e2:4d:c3                ← 网关绑定ip和网关的mac
# arp -d 10.15.72.73                                   ← 删除arp记录
{% endhighlight %}

另外，是和无线网络相关的命令。

{% highlight text %}
# ifconfig eth0 promisc                                ← 设置混杂模式
# ifconfig eth0 -promisc                               ← 取消混杂模式
{% endhighlight %}

### 常见文件

在 Linux 中，包括了一些常见的系统文件，简单列举如下：

* /etc/protocols 主机使用的协议以及各个协议的协议号；
* /etc/services 主机的不同端口的网络服务。

#### /etc/networks

另外这个文件主要完成域名与网络地址的映射，其内容如下，在此简单介绍下。

{% highlight text %}
default 0.0.0.0
loopback 127.0.0.0

foobarname 192.168.1.0
{% endhighlight %}

其中的网络地址是以 .0 结尾，而且只支持 Class A, B, C 网络，不能直接指定 IP 地址，对于 IP 应该保存在 /etc/hosts 文件中。

例如，上述的最后一条记录，假设有 IP 地址为 192.168.1.5，那么查看路由时会显示如下内容。

{% highlight text %}
$ route
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
default         192.168.1.1     0.0.0.0         UG    0      0        0 eth0
foobarname      *               255.255.255.0   U     0      0        0 eth0
{% endhighlight %}







## hostname 修改

hostname 是 Linux 系统下的一个内核参数，可以通过 /proc/sys/kernel/hostname 访问该参数当前值，不过其值一般是在系统启动时通过初始化脚本设置的。当然，这也就导致不同的发行版本，甚至不同的版本，其设置方式也各有区别。

常见的一些操作可以通过如下方式查看。

{% highlight text %}
----- 查看(三者值相同)
$ hostname                                     ← 查看主机名
$ hostname -i                                  ← 查看主机的IP
$ cat /proc/sys/kernel/hostname
$ sysctl kernel.hostname

----- 临时修改(重启后会失效)，立即生效
$ echo foobar > /proc/sys/kernel/hostname
$ sysctl kernel.hostname=foobar
$ hostname foobar
$ export HOSTNAME=foobar                       ← 解决bash的PS显示问题

----- 永久修改 (需要重启网络)   
$ cat /etc/sysconfig/network                   ← CentOS/RedHat
NETWORKING=yes
HOSTNAME=foobar.localdomain
$ cat /etc/hostname                            ← Debian/Ubuntu
foobar

----- 重启网络
$ /etc/init.d/network restart
{% endhighlight %}

其中，有几点需要注意的：

* 在 CentOS 中，如果在上述的配置文件中没有配置，实际也会读取 /etc/hostname 文件。查看 man hostname 帮助时，也需要同时设置 /etc/hostname 。
* 通过 strace 命令查看系统调用时，实际是调用的 uname() 这个内核 API 。

以 CentOS 为例，在系统启动时，会执行 /etc/rc.d/init.d/network 脚本，该脚本会根据不同的配置文件尝试获取 hostname 。

另外，一个比较容易混淆的文件是 /etc/hosts，该文件实际上就只提供了一个本地设置的 DNS 映射关系，只是有些启动脚本会通过该文件读取 hostname，这也就导致会认为 hostname 的修改是通过该文件配置的。 

当然，最好也设置好该文件内容。

{% highlight text %}
$ cat /etc/hosts
127.0.0.1 foobar localhost
{% endhighlight %}


## 路由配置

关于 IP 路由相关命令，详细可以查看 [IP Route Management](http://linux-ip.net/html/tools-ip-route.html) 。

{% highlight text %}
# route -n
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.145.12.254  0.0.0.0         UG    0      0        0 eth0
100.125.0.0     0.0.0.0         255.255.248.0   U     0      0        0 eth1

输出：
  Flags 路由标志，标记当前网络节点的状态，各个标志简述如下：
    U Up              此路由当前为启动状态
    G Gateway         此网关为一路由器
    ! Reject Route    表示此路由当前为关闭状态，常用于抵挡不安全规则
    H Host            表示此网关为一主机
    R Reinstate Route 使用动态路由重新初始化的路由
    D Dynamically     此路由是动态性地写入
    M Modified        此路由是由路由守护程序或导向器动态修改

----- 增加一条到达244.0.0.0的路由
# route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
----- 增加一条屏蔽的路由，目的地址为224.x.x.x将被拒绝
# route add -net 224.0.0.0 netmask 240.0.0.0 reject
----- 删除路由记录
# route del -net 224.0.0.0 netmask 240.0.0.0
# route del -net 224.0.0.0 netmask 240.0.0.0 reject
----- 删除和添加设置默认网关
# route del default gw 192.168.120.240
# route add default gw 192.168.120.240
{% endhighlight %}

<!--

ip route list

Destination、Gateway、Genmask，目标地址、网管、掩码

第一行表示主机所在网络的地址为192.168.120.0，若数据传送目标是在本局域网内通信，则可直接通过eth0转发数据包;

第四行表示数据传送目的是访问Internet，则由接口eth0，将数据包发送到网关192.168.120.240

Metric

metric Metric 为路由指定一个整数成本值标（从 1 至 9999），当在路由表(与转发的数据包目标地址最匹配)的多个路由中进行选择时可以使用。

http://www.cnblogs.com/vamei/archive/2012/10/07/2713023.html

ip route show cache
ip route flush cache
ip route get
-->

一般路由匹配的流程是：

1. 先匹配掩码，掩码最精确匹配的路由优先 (longest prefix match)；
2. 如果有多条路由，则匹配 [管理距离](https://en.wikipedia.org/wiki/Administrative_distance)，管理距离小的路由优先；
3. 如果管理距离相同，在匹配度量值 (lowest metric)，度量值小的优先
4. 如果度量值相同，则选择负载均衡，具体的方式看采用哪种路由协议和相关的配置了。

Routing Cache 也称为 Forwarding Information Base, FIB，通过 Hash 保存了最近使用路由信息，如果在 Cache 找到了对应的路由信息，那么会直接使用该规则进行转发，而不再查找路由表。

注意，Routing Cache 和 Route Table 是不相关的，所以在修改路由表之后最后手动清空下 Cache ，可以通过 ```/proc/net/rt_cache``` 文件查看或者使用如下命令：

<!-- http://linux-ip.net/html/routing-selection.html -->

## 常用命令

### TCP 状态排查

可以通过如下命令查看 TCP 状态。

{% highlight text %}
----- 查看链接状态，并对其进行统计，如下的两种方法相同
$ netstat -atn | awk '/^tcp/ {++s[$NF]} END {for(key in s) print s[key], "\t", key}' | sort -nr
$ ss -ant | awk ' {++s[$1]} END {for(key in s) print s[key], "\t", key}' | sort -nr

----- 查找较多time_wait连接
$ netstat -n|grep TIME_WAIT|awk '{print $5}'|sort|uniq -c|sort -rn|head -n20

----- 对接的IP进行排序
$ netstat -ntu | awk '/^tcp/ {print $5}' | cut -d: -f1 | sort | uniq -c | sort -n

----- 查看80端口连接数最多的20个IP
$ netstat -ant |awk '/:80/{split($5,ip,":");++A[ip[1]]}END{for(i in A) print A[i],i}' |sort -rn|head -n20

----- 80端口的各个TCP链接状态
$ netstat -n | grep `hostname -i`:80 |awk '/^tcp/{++S[$NF]}END{for (key in S) print key,S[key]}'
{% endhighlight %}



{% highlight text %}
{% endhighlight %}
