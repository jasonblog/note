---
title: Linux 網絡設置
layout: post
comments: true
category: [linux, network]
language: chinese
keywords: linux,網絡設置
description: 主要記錄下在 Linux 中，一些常見的網絡配置，例如 IP 地址、路由、MAC 地址、主機名等設置方式。
---

主要記錄下在 Linux 中，一些常見的網絡配置，例如 IP 地址、路由、MAC 地址、主機名等設置方式。

<!-- more -->

## 常見配置

簡單列舉一些常見的網絡配置。

{% highlight text %}
# ifconfig eth0 down                                      ← 關閉網絡設備
# ifconfig eth0 hw ether xx:xx:xx:xx:xx:xx                ← 設置MAC地址
# ifconfig eht0 up
# ifconfig eth0 210.72.137.115 netmask 255.255.255.0      ← 配置IP
# route add default gw 192.168.0.100 dev eth0             ← 添加默認路由

# ifconfig eth0:0 210.72.137.115 netmask 255.255.255.0 up ← 設置VIP
# ifconfig eth0:0 down                                    ← 關閉VIP配置
{% endhighlight %}

如果 IP 和 GW 不在同一網段會出現 SIOCADDRT:No such process. 的錯誤，可以在上一條命令之前添加如下命令。

{% highlight text %}
# route add -host 192.168.0.100 [netmask 0.0.0.0] dev eth0
{% endhighlight %}

對於 ARP 可以通過如下方式查看。

{% highlight text %}
# arp -n                                               ← 查看當前系統的ARP表
# arp -s 10.15.72.25 408:d0:9f:e2:4d:c3                ← 網關綁定ip和網關的mac
# arp -d 10.15.72.73                                   ← 刪除arp記錄
{% endhighlight %}

另外，是和無線網絡相關的命令。

{% highlight text %}
# ifconfig eth0 promisc                                ← 設置混雜模式
# ifconfig eth0 -promisc                               ← 取消混雜模式
{% endhighlight %}

### 常見文件

在 Linux 中，包括了一些常見的系統文件，簡單列舉如下：

* /etc/protocols 主機使用的協議以及各個協議的協議號；
* /etc/services 主機的不同端口的網絡服務。

#### /etc/networks

另外這個文件主要完成域名與網絡地址的映射，其內容如下，在此簡單介紹下。

{% highlight text %}
default 0.0.0.0
loopback 127.0.0.0

foobarname 192.168.1.0
{% endhighlight %}

其中的網絡地址是以 .0 結尾，而且只支持 Class A, B, C 網絡，不能直接指定 IP 地址，對於 IP 應該保存在 /etc/hosts 文件中。

例如，上述的最後一條記錄，假設有 IP 地址為 192.168.1.5，那麼查看路由時會顯示如下內容。

{% highlight text %}
$ route
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
default         192.168.1.1     0.0.0.0         UG    0      0        0 eth0
foobarname      *               255.255.255.0   U     0      0        0 eth0
{% endhighlight %}







## hostname 修改

hostname 是 Linux 系統下的一個內核參數，可以通過 /proc/sys/kernel/hostname 訪問該參數當前值，不過其值一般是在系統啟動時通過初始化腳本設置的。當然，這也就導致不同的發行版本，甚至不同的版本，其設置方式也各有區別。

常見的一些操作可以通過如下方式查看。

{% highlight text %}
----- 查看(三者值相同)
$ hostname                                     ← 查看主機名
$ hostname -i                                  ← 查看主機的IP
$ cat /proc/sys/kernel/hostname
$ sysctl kernel.hostname

----- 臨時修改(重啟後會失效)，立即生效
$ echo foobar > /proc/sys/kernel/hostname
$ sysctl kernel.hostname=foobar
$ hostname foobar
$ export HOSTNAME=foobar                       ← 解決bash的PS顯示問題

----- 永久修改 (需要重啟網絡)   
$ cat /etc/sysconfig/network                   ← CentOS/RedHat
NETWORKING=yes
HOSTNAME=foobar.localdomain
$ cat /etc/hostname                            ← Debian/Ubuntu
foobar

----- 重啟網絡
$ /etc/init.d/network restart
{% endhighlight %}

其中，有幾點需要注意的：

* 在 CentOS 中，如果在上述的配置文件中沒有配置，實際也會讀取 /etc/hostname 文件。查看 man hostname 幫助時，也需要同時設置 /etc/hostname 。
* 通過 strace 命令查看系統調用時，實際是調用的 uname() 這個內核 API 。

以 CentOS 為例，在系統啟動時，會執行 /etc/rc.d/init.d/network 腳本，該腳本會根據不同的配置文件嘗試獲取 hostname 。

另外，一個比較容易混淆的文件是 /etc/hosts，該文件實際上就只提供了一個本地設置的 DNS 映射關係，只是有些啟動腳本會通過該文件讀取 hostname，這也就導致會認為 hostname 的修改是通過該文件配置的。 

當然，最好也設置好該文件內容。

{% highlight text %}
$ cat /etc/hosts
127.0.0.1 foobar localhost
{% endhighlight %}


## 路由配置

關於 IP 路由相關命令，詳細可以查看 [IP Route Management](http://linux-ip.net/html/tools-ip-route.html) 。

{% highlight text %}
# route -n
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.145.12.254  0.0.0.0         UG    0      0        0 eth0
100.125.0.0     0.0.0.0         255.255.248.0   U     0      0        0 eth1

輸出：
  Flags 路由標誌，標記當前網絡節點的狀態，各個標誌簡述如下：
    U Up              此路由當前為啟動狀態
    G Gateway         此網關為一路由器
    ! Reject Route    表示此路由當前為關閉狀態，常用於抵擋不安全規則
    H Host            表示此網關為一主機
    R Reinstate Route 使用動態路由重新初始化的路由
    D Dynamically     此路由是動態性地寫入
    M Modified        此路由是由路由守護程序或導向器動態修改

----- 增加一條到達244.0.0.0的路由
# route add -net 224.0.0.0 netmask 240.0.0.0 dev eth0
----- 增加一條屏蔽的路由，目的地址為224.x.x.x將被拒絕
# route add -net 224.0.0.0 netmask 240.0.0.0 reject
----- 刪除路由記錄
# route del -net 224.0.0.0 netmask 240.0.0.0
# route del -net 224.0.0.0 netmask 240.0.0.0 reject
----- 刪除和添加設置默認網關
# route del default gw 192.168.120.240
# route add default gw 192.168.120.240
{% endhighlight %}

<!--

ip route list

Destination、Gateway、Genmask，目標地址、網管、掩碼

第一行表示主機所在網絡的地址為192.168.120.0，若數據傳送目標是在本局域網內通信，則可直接通過eth0轉發數據包;

第四行表示數據傳送目的是訪問Internet，則由接口eth0，將數據包發送到網關192.168.120.240

Metric

metric Metric 為路由指定一個整數成本值標（從 1 至 9999），當在路由表(與轉發的數據包目標地址最匹配)的多個路由中進行選擇時可以使用。

http://www.cnblogs.com/vamei/archive/2012/10/07/2713023.html

ip route show cache
ip route flush cache
ip route get
-->

一般路由匹配的流程是：

1. 先匹配掩碼，掩碼最精確匹配的路由優先 (longest prefix match)；
2. 如果有多條路由，則匹配 [管理距離](https://en.wikipedia.org/wiki/Administrative_distance)，管理距離小的路由優先；
3. 如果管理距離相同，在匹配度量值 (lowest metric)，度量值小的優先
4. 如果度量值相同，則選擇負載均衡，具體的方式看採用哪種路由協議和相關的配置了。

Routing Cache 也稱為 Forwarding Information Base, FIB，通過 Hash 保存了最近使用路由信息，如果在 Cache 找到了對應的路由信息，那麼會直接使用該規則進行轉發，而不再查找路由表。

注意，Routing Cache 和 Route Table 是不相關的，所以在修改路由表之後最後手動清空下 Cache ，可以通過 ```/proc/net/rt_cache``` 文件查看或者使用如下命令：

<!-- http://linux-ip.net/html/routing-selection.html -->

## 常用命令

### TCP 狀態排查

可以通過如下命令查看 TCP 狀態。

{% highlight text %}
----- 查看鏈接狀態，並對其進行統計，如下的兩種方法相同
$ netstat -atn | awk '/^tcp/ {++s[$NF]} END {for(key in s) print s[key], "\t", key}' | sort -nr
$ ss -ant | awk ' {++s[$1]} END {for(key in s) print s[key], "\t", key}' | sort -nr

----- 查找較多time_wait連接
$ netstat -n|grep TIME_WAIT|awk '{print $5}'|sort|uniq -c|sort -rn|head -n20

----- 對接的IP進行排序
$ netstat -ntu | awk '/^tcp/ {print $5}' | cut -d: -f1 | sort | uniq -c | sort -n

----- 查看80端口連接數最多的20個IP
$ netstat -ant |awk '/:80/{split($5,ip,":");++A[ip[1]]}END{for(i in A) print A[i],i}' |sort -rn|head -n20

----- 80端口的各個TCP鏈接狀態
$ netstat -n | grep `hostname -i`:80 |awk '/^tcp/{++S[$NF]}END{for (key in S) print key,S[key]}'
{% endhighlight %}



{% highlight text %}
{% endhighlight %}
