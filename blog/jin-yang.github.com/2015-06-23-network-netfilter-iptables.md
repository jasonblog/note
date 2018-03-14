---
title: Linux 的防火牆
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: linux,防火牆,netfilter,iptables
description: Linux 防火牆是由 Netfilter 和 iptables 兩個組件組成，其中前者在內核空間中；後者是一個用戶程序，用於插入、修改和刪除相應的防火牆規則。通過 Netfilter+iptables 可以實現數據包過濾、網絡地址轉換和數據報文處理等功能。
---

Linux 防火牆是由 Netfilter 和 iptables 兩個組件組成，其中前者在內核空間中；後者是一個用戶程序，用於插入、修改和刪除相應的防火牆規則。

通過 Netfilter+iptables 可以實現數據包過濾、網絡地址轉換和數據報文處理等功能。

<!-- more -->

## Netfilter 簡介

Netfilter 框架用來在網絡協議棧的內核路徑上過濾數據包，就像一條路上的關卡，一個數據包在被處理的路徑上在經過這些關卡時會被檢查，如果符合條件就進行處理，結果就是若干個動作，包括接受、丟棄、排隊、導入其它路徑等。

框架只需針對一個數據包得出一個結果即可，關卡內部提供什麼服務在Netfilter框架中並沒有任何規定。

Netfilter 是一個設計良好的框架，之所以說它是一個框架是因為它提供了最基本的底層支撐，而對於實現的關注度卻沒有那麼高，這種底層支撐實際上就是其 5 個 HOOK 點。

各個 hook 點其實就是固定的“檢查點”，這些檢查點是內嵌於網絡協議棧的，它將檢查點無條件的安插在協議棧中，這些檢查點的檢查是無條件執行的。

![netfilter]({{ site.url }}/images/linux/netfilter_packet_flow.png "netfiler"){: .pull-center }

如上圖所示，Netfilter 包括了五個檢查節點。

1. PREROUTING：報文進入網絡層但是還未到達路由前
2. FORWARD：報文經過路由後，確定需要轉發之後
3. INPUT：報文經過路由之後，確定要本地接收之後
4. OUTPUT：本地數據包發送
5. POSTROUTING：數據包通過本機馬上發出去之前

在內核中，通過如下的宏定義。

{% highlight text %}
#define NF_IP_PRE_ROUTING   0
#define NF_IP_LOCAL_IN      1
#define NF_IP_FORWARD       2
#define NF_IP_LOCAL_OUT     3
#define NF_IP_POST_ROUTING  4
{% endhighlight %}

## iptables

該工具在 CentOS 中是默認安裝，如果沒有可以通過 ```yum install iptables``` 安裝。

### 示例

關於添加、除去、編輯規則的命令的一般語法如下。

{% highlight text %}
iptables [-t table] command [chain] [match] [target]

----- command，指定管理規則的方式，如插入、增加、刪除以及查看等
-A, --append  該命令將一條規則附加到鏈的末尾
-L, --list    列出指定鏈中的所有規則，默認查看filter表，要查看NAT表，可加上-t NAT參數

----- match，條件匹配分為基本匹配和擴展匹配，拓展匹配又分為隱式擴展和顯示擴展
-p  指定規則協議，如tcp, udp,icmp等，可以使用all來指定所有協議
-s  指定數據包的源地址參數，可以使IP地址、網絡地址、主機名
-d  指定目的地址
-i  輸入接口
-o  輸出接口
{% endhighlight %}

### 常用操作

首先是查看當前的記錄，默認查看的是 filter 表，如果要查看 NAT 表，可以加上 -t nat 參數。

{% highlight text %}
# iptables --line-number -nvL POSTROUTING -t nat

如上參數包括：
   -n    不對IP進行反查，顯示速度會快很多
   -v    輸出詳細信息，包括通過該規則的數據包數量、總字節數、相應的網絡接口
   --line-number   顯示規則的序列號，這個參數在刪除或修改規則時會用到
{% endhighlight %}

添加規則有兩個參數：1) -A 添加到規則的末尾；2) -I 插入到指定位置，默認插入到規則的首部。

{% highlight text %}
----- 添加一條規則到尾部
# iptables -A INPUT -s 192.168.1.5 -j DROP

----- 插入一條規則到第三行
# iptables -I INPUT 3 -s 192.168.1.3 -j DROP
{% endhighlight %}

結下來可以通過如下方式刪除。

{% highlight text %}
----- 刪除之前添加的規則
# iptables -D INPUT -s 192.168.1.5 -j DROP

----- 如果規則太長，直接通過行號刪除，如下刪除第2行
# iptables -D INPUT 2
{% endhighlight %}

接下來看看如何修改規則。

{% highlight text %}
----- 接下來先查找對應的規則號
# iptables -nL --line-number
Chain INPUT (policy ACCEPT)
num  target     prot opt source               destination
1    DROP       all  --  192.168.1.1          0.0.0.0/0

----- 然後將第一條規則改為ACCEPT
# iptables -R INPUT 1 -j ACCEPT
{% endhighlight %}

#### 持久化保存

設置完成之後，可以直接通過 ```iptables-save``` 保存為文件，默認打印到 stdout 中。

{% highlight text %}
----- RHEL/CentOS
# iptables-save > /etc/sysconfig/iptables
# iptables-restore < /etc/sysconfig/iptables
----- Debian/Ubuntu
# iptables-save > /etc/iptables/rules.v4
# iptables-restore < /etc/iptables/rules.v4
{% endhighlight %}

注意，如果使用了 IPv6 則對應 ```ip6tables-save``` 命令以及 ```/etc/iptables/rules.v6```、```/etc/sysconfig/ip6tables``` 。


### 常用實例

下面是常用的示例。

{% highlight text %}
----- 允許訪問22端口，如果需要拒絕訪問，則將ACCEPT改為DROP即可
# iptables -A INPUT -p tcp --dport 22 -j ACCEPT
----- 允許192.168.1.123訪問本機的1521端口
# iptables -I INPUT -s 192.168.1.123 -p tcp --dport 1521 -j ACCEPT
-A INPUT -s 192.30.19.82/32 -p tcp -m tcp --dport 21005 -j ACCEPT
----- 在指定位置插入，如果時-A參數則是在末尾添加
# iptables -I INPUT 3 -p tcp -m tcp --dport 20 -j ACCEPT

----- 添加NAT規則，對192.168.10.0/24進行地址偽裝
# iptables -t nat -A POSTROUTING -s 192.168.10.0/24 -j MASQUERADE

----- 查看filter表規則，顯示編號，常用於刪除
# iptables -L -n --line-number
----- 查看NAT表規則
# iptables -t nat -vnL POSTROUTING --line-number

----- 修改規則，改為DROP
# iptables -R INPUT 3 -j DROP

----- 刪除input的第3條規則
# iptables -D INPUT 3
----- 刪除nat表中postrouting的第一條規則
# iptables -t nat -D POSTROUTING 1
----- 清空filter表INPUT所有規則
# iptables -F INPUT
----- 清空所有規則
# iptables -F
----- 清空nat表POSTROUTING所有規則
# iptables -t nat -F POSTROUTING

---- 設置filter表INPUT默認規則是DROP
# iptables -P INPUT DROP
{% endhighlight %}


## 其它

目前除了 iptables 之外，還有 [nf-hipac](http://www.hipac.org/index.html) 。曾經有網友測試過，nf-hipac 比 iptables 優化的不是一星半點，而且前者在加載 20000 個條目時，使用 iperf 的測試結果基本根裸奔相差無幾。不過該項目基本已經停滯，現在是通過 [ipset](http://ipset.netfilter.org/) 替換之 ^_^

目前有個 [Netfilter](http://www.netfilter.org/projects/nftables/index.html) 的項目，用來取代現有的 {ip,ip6,arp,eb}tables 等工具，不過現在還在開發過程中。

另外，內核中提供了 BPF (BSD Packet Filter) 支持，包括現在的 tcpdump 等程序，都是使用的這一規則。據說，其速度是 iptables 的幾倍，採用綜合狀態機、JIT，可以在很短的時間內完成數據包的匹配判斷，可以參考 [eBPF: One Small Step](http://www.brendangregg.com/blog/2015-05-15/ebpf-one-small-step.html) 。










<!--

## NAT, Network Address Translation

網絡地址轉換包括了兩類典型的應用：SNAT (Source NAT) 和 DNAT (Destination NAT)；另外，還有一種被稱為地址偽裝 (MASQUERADE)，不過其功能與 SNAT 相似。

兩者分別用於實現企業局域網共享上網、在 Internet 中發佈內網的應用服務器。

### 源地址轉換

修改數據包的源地址。源NAT改變第一個數據包的來源地址，它永遠會在數據包發送到網絡之前完成，數據包偽裝就是一具SNAT的例子。

隨著 Internet 網絡在全世界範圍內的快速發展， IPv4 協議支持的可用 IP 地址資源逐漸變得山窮水盡，資源匱乏使得很多企業難以申請更多的公網 IP 地址，或者只能承受一個或者少數幾個公網 IP 地址的費用。而與此同時，大部分企業都面臨著將局域網的主機接入 Internet 的需求。使用 iptables 的 SNAT 策略，可以基本化解這個難題。

{% highlight text %}
+----------------+
| 172.18.0.20/16 |
+----------------+

+----------------+
| 172.18.0.20/16 |
+----------------+
{% endhighlight %}





### 目的地址轉換

(2)目的NAT（Destination NAT，DNAT）：修改數據包的目的地址。Destination NAT剛好與SNAT相反，它是改變第一個數據懈的目的地地址，如平衡負載、端口轉發和透明代理就是屬於DNAT










MASQUERADE


-A, --append






　　SNAT是source network address translation的縮寫，即源地址目標轉換
　　比如，多個PC機使用ADSL路由器共享上網，每個PC機都配置了內網IP
　　PC機訪問外部網絡的時候，路由器將數據包的報頭中的源地址替換成路由器的ip
　　當外部網絡的服務器比如網站web服務器接到訪問請求的時候
　　他的日誌記錄下來的是路由器的ip地址，而不是pc機的內網ip
　　這是因為，這個服務器收到的數據包的報頭裡邊的“源地址”，已經被替換了
　　所以叫做SNAT，基於源地址的地址轉換

　　DNAT是destination network address translation的縮寫
　　即目標網絡地址轉換
　　典型的應用是，有個web服務器放在內網配置內網ip，前端有個防火牆配置公網ip
　　互聯網上的訪問者使用公網ip來訪問這個網站
　　當訪問的時候，客戶端發出一個數據包
　　這個數據包的報頭裡邊，目標地址寫的是防火牆的公網ip
　　防火牆會把這個數據包的報頭改寫一次，將目標地址改寫成web服務器的內網ip
　　然後再把這個數據包發送到內網的web服務器上
　　這樣，數據包就穿透了防火牆，並從公網ip變成了一個對內網地址的訪問了
　　即DNAT，基於目標的網絡地址轉換

　　MASQUERADE，地址偽裝，在iptables中有著和SNAT相近的效果，但也有一些區別
　　但使用SNAT的時候，出口ip的地址範圍可以是一個，也可以是多個，例如：
　　如下命令表示把所有10.8.0.0網段的數據包SNAT成192.168.5.3的ip然後發出去
　　iptables -t nat -A POSTROUTING -s 10.8.0.0/255.255.255.0 -o eth0 -j SNAT --to-source 192.168.5.3
　　如下命令表示把所有10.8.0.0網段的數據包SNAT成192.168.5.3/192.168.5.4/192.168.5.5等幾個ip然後發出去
　　iptables -t nat -A POSTROUTING -s 10.8.0.0/255.255.255.0 -o eth0 -j SNAT --to-source 192.168.5.3-192.168.5.5
　　這就是SNAT的使用方法，即可以NAT成一個地址，也可以NAT成多個地址
但是，對於SNAT，不管是幾個地址，必須明確的指定要SNAT的ip
　　假如當前系統用的是ADSL動態撥號方式，那麼每次撥號，出口ip192.168.5.3都會改變
　　而且改變的幅度很大，不一定是192.168.5.3到192.168.5.5範圍內的地址
　　這個時候如果按照現在的方式來配置iptables就會出現問題了
　　因為每次撥號後，服務器地址都會變化，而iptables規則內的ip是不會隨著自動變化的
　　每次地址變化後都必須手工修改一次iptables，把規則裡邊的固定ip改成新的ip
　　這樣是非常不好用的
　　MASQUERADE就是針對這種場景而設計的，他的作用是，從服務器的網卡上，自動獲取當前ip地址來做NAT
　　比如下邊的命令：
　　iptables -t nat -A POSTROUTING -s 10.8.0.0/255.255.255.0 -o eth0 -j MASQUERADE
　　如此配置的話，不用指定SNAT的目標ip了
　　不管現在eth0的出口獲得了怎樣的動態ip，MASQUERADE會自動讀取eth0現在的ip地址然後做SNAT出去
　　這樣就實現了很好的動態SNAT地址轉換











NAT有三種類型：靜態NAT(Static NAT)、動態地址NAT(Pooled NAT)、網絡地址端口轉換NAPT（Port-Level NAT）
其中，網絡地址端口轉換NAPT（Network Address Port Translation）則是把內部地址映射到外部網絡的一個IP地址的不同端口上。它可以將中小型的網絡隱藏在一個合法的IP地址後面。NAPT與 動態地址NAT不同，它將內部連接映射到外部網絡中的一個單獨的IP地址上，同時在該地址上加上一個由NAT設備選定的端口號.NAPT是使用最普遍的一種轉換方式
,它又細分為snat和dnat.










## Connection Tracking

nf_conntrack 是一個內核模塊，用於跟蹤一個連接的狀態信息，其中連接狀態的跟蹤信息可以供其它模塊使用，最常見的兩個使用場景是 iptables 的 nat 和 state 模塊。

其中，iptables 的 NAT 通過規則來修改目的或者源地址，除了修改地址之外，還要藉助 nf_conntrack 找到原來的那個連接記錄，從而能讓回來的包可以路由到最初的源主機。

另外，iptables 的 state 模塊會直接使用 nf_conntrack 中記錄的連接狀態來匹配用戶定義的相關規則。例如，下面的規則用於放行 80 端口上的狀態為 NEW 的連接。

{% highlight text %}
# iptables -A INPUT -p tcp -m state --state NEW -m tcp --dport 80 -j ACCEPT
{% endhighlight %}

目前使用的較多的是 nf_conntrack，老版本採用的是 ip_conntrack 模塊，所以，之前的 ip_conntrack_XXX 已經被 nf_conntrack_XXX 取代，包括 procfs、內核源碼中的定義。例如，之前的 /proc/net/ip_conntrack，現在為 /proc/net/nf_conntrack 。

nf_conntrack 工作在 3 層，可以同時支持 IPv4 和 IPv6，而 ip_conntrack 只支持 IPv4 。

<br>

除了直接查看 /proc/net/ip_conntrack 之外，用戶層提供了一個 conntrack-tools 工具包。









### nf_conntrack 隊列滿

此時的現象是部分客戶端連不上服務器，可以在 /var/log/{dmesg,messages} 文件中看到類似如下的報錯。

{% highlight text %}
kernel: nf_conntrack: table full, dropping packet
{% endhighlight %}

在 /proc/net/nf_conntrack 中，保存了連接跟蹤數據庫 (conntrack database)，最常見的兩個場景是 iptables 中的 NAT 以及 state 模塊。

通常用來做 NAT 轉換時的連接跟蹤，如果滿了，肯定會有問題。

{% highlight text %}
----- 1. 當前跟蹤的鏈接數
# wc -l /proc/net/nf_conntrack

----- 2. 查看最大值限制，可以適當調大
# cat /proc/sys/net/netfilter/nf_conntrack_max

----- 3. 保留時間，單位時秒，默認保留5天，可以調小
# cat /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established
{% endhighlight %}

nf_conntrack 文件中保存了跟蹤鏈接的相關信息，該文件的輸出在 net/netfilter/nf_conntrack_core.c 中實現，對應 ct_seq_show() 函數。

該文件中的相關信息可以參考 [Connection tracking][connection-tracking]，有比較詳細的介紹。



## 參考


iptables使用
http://blog.51yip.com/linux/1404.html






[connection-tracking]:        http://www.rigacci.org/wiki/lib/exe/fetch.php/doc/appunti/linux/sa/iptables/conntrack.html
[connection-tracking-local]:  /reference/linux/network/conntrack.html




## 源碼解析

對於源碼中的接口，可以直接通過 grep -rne 'NF_HOOK.\*NFPROTO\_IPV4'  net/* 查找。


http://blog.csdn.net/lickylin/article/details/33020731
http://blog.csdn.net/lickylin/article/details/32387217
http://drops.wooyun.org/tips/1424


http://bbs.chinaunix.net/forum.php?mod=viewthread&tid=3749208&fromuid=20171559          linux-2.6.35.6內核netfilter框架
http://blog.csdn.net/godleading/article/details/19972253                                Linux nf_conntrack連接跟蹤的實現
http://bbs.chinaunix.net/thread-2008344-1-1.html                                        Netfilter實現機制分析
http://bbs.chinaunix.net/thread-2005999-1-1.html                                        Netfilter之連接跟蹤實現機制初步分析
http://bbs.chinaunix.net/thread-1750611-1-1.html                                        Netfilter機制分析
http://inai.de/documents/Netfilter_Modules.pdf                                          Writing Netfilter modules

# xxx
在 netfilter.h 頭文件中會對 NF_HOOK() 內聯函數進行定義，在該函數中會調用 nf_hook_thresh()，最終會調用 nf_hook_slow() 函數。
{% highlight c %}
struct nf_hook_ops {
    struct list_head list;       // 保存了鏈表
    nf_hookfn       *hook;       // 以及相應的HOOK函數
    struct module   *owner;
    void            *priv;
    u_int8_t         pf;
    unsigned int     hooknum;
    int              priority;
};
{% endhighlight %}
在 nf_iterate() 函數中，會遞歸搜索 list 並執行 hook 函數。
{% highlight text %}
nf_hook_slow()
 |-nf_iterate()           # 遞歸調用註冊的鉤子函數
   |-elem->hook()
{% endhighlight %}







-->




{% highlight text %}
{% endhighlight %}
