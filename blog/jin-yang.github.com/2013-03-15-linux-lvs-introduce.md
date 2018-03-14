---
title: Linux LVS
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,lvs
description: LVS 通常用於四層的網絡服務器做負載均衡，其工作在內核層，而且效率非常高；部署時還會使用 heartbeat 以及 keepalived 做高可用。在本文中，我們對其做些介紹。
---

LVS 通常用於四層的網絡服務器做負載均衡，其工作在內核層，而且效率非常高；部署時還會使用 heartbeat 以及 keepalived 做高可用。

在本文中，我們對其做些介紹。

<!-- more -->

## Linux Virtual Server Project

負載均衡設備通常作為應用服務器的入口，簡單來說就是對於一個請求，如何選擇其中的一個服務器去處理該請求，如下是 Google SRE 中的定義。

{% highlight text %}
Traffic load balancing is how we decide which of the many, many machines
in our datacenters will serve a particular request.
{% endhighlight %}

LVS 工作在內核層，可以處理 TCP 以及 UDP 連接，也就是是說，只要是基於這兩個協議的都支持，包括了 email、http、https 等服務，甚至包括 X-Windows 。


仍然以 Amazon 為例。

![network interface]({{ site.url }}/images/network/lb-overview.png){: .pull-center }

性能好的是商用硬件負載設備，但是成本通常很高，一臺能達到幾十萬甚至上百萬，所以通常使用的是軟負載均衡，而軟負載的兩個核心問題是：選誰、轉發，其中最著名的是 LVS (Linux Virtual Server Project)。

## 簡介

實際上很多層都可以做負載均衡，例如網卡的 Bond、ECMP、DNS 等等，而針對 VIP 的通常工作在網絡模型的第四層以及第七層。

![network interface]({{ site.url }}/images/network/network-introduce-OSI-TCP-IP.png){: .pull-center }

軟負載均衡面向的是一個已經建立連接的會話，而非單獨的 IP 報文，也就是說用戶訪問需要 TCP 協議與服務器建立連接傳輸數據，而 TCP 是在第四層實現的，如果在第三層做負載均衡，那麼將會丟失四層的 "連接"，導致出錯。

LVS 工作在網絡模型中的第四層，不同於 HAProxy、Nginx 等七層軟負載均衡可以面向 HTTP 包，從而七層可以做 URL 的解析工作，而 LVS 無法完成。

## 報文轉發

首先負載均衡中包括了 Director (提供 LVS 服務的機器)、Real Server (真正提供服務的機器) 。LVS 支持三種報文的轉發方式 NAT、tunneling、direct routing，分別介紹如下：

### Network Address Translation (NAT)

NAT 也就是修改報文的源或者目的地址以及端口號，是一種外網和內網的地址映射技術，對於這種模式，報文的進出都要經過 LVS 服務器，也就意味這 RS 需要將 RS 作為網關。

![network interface]({{ site.url }}/images/network/lvs-nat-example-2.png){: .pull-center }

一個報文的處理過程大致經過瞭如下的三個步驟：

1. Director 收到報文後會做 DNAT，也就是修改目的地址以及端口號為真正的服務器，然後發送到 RS；

2. RS 接收到包後，就像是客戶端直接發給它的一樣，不會感知到 LVS 的存在。響應報文的源 IP 是 RS IP，目標 IP 是客戶端的 IP。

3. 響應報文通過 LVS 中轉，會做 SNAT，將包的源地址改為 VIP，這樣對客戶端來說，看起來就像是 LVS 直接返回給它的，客戶端無法感知到後端 RS 的存在。

下圖是數據流的流動過程。

![network interface]({{ site.url }}/images/network/lvs-nat.png){: .pull-center }

這是最見的配置方式了，假設有如下環境。

![network interface]({{ site.url }}/images/network/lvs-nat-example.png){: .pull-center }

只需要做如下的配置。

{% highlight text %}
----- 首先配置 Linux Director，其中VIP為172.17.60.201
---++ 在/etc/sysctl.conf中設置IP轉發
net.ipv4.ip_forward = 1
---++ 設置虛擬IP地址
# ifconfig eth0:0 172.17.60.201 netmask 255.255.0.0 broadcast 172.17.255.255
---++ 配置LVS
# ipvsadm -A -t 172.17.60.201:80
# ipvsadm -a -t 172.17.60.201:80 -r 192.168.6.4:80 -m
# ipvsadm -a -t 172.17.60.201:80 -r 192.168.6.5:80 -m

----- 然後配置Real Servers，只需要將默認的gateway設置為VIP即可
{% endhighlight %}

然後可以通過如下方法查看當前的統計值。

{% highlight text %}
# ipvsadm -L -n              # 查看當前活動的連接
# ipvsadm -L -n --stats      # 每秒收發的報文數
# ipvsadm -L -n --rate       # 每秒的報文的收發速度
# ipvsadm -L --zero          # 清空報文內容
{% endhighlight %}


### Direct Routing

此時請求由 LVS 接受，然後報文會直接通過修改目的 MAC 發送給 RealServer，其中 IP 報文不會修改，而響應報文則會由 RS 直接返回給用戶，不會經過 LVS，這是性能最好的一種模式。

這種方式同時這也就意味著 RS 必須也配置為 VIP，通常是通過 loopback 設備綁定。

簡單來說，一個請求總共分為如下三步：

1.  LVS 接收請求，將接收到報文的 MAC 地址修改為某臺 RS 的 MAC，該包就會在 LAN 中被轉發到相應的 RS 處理，而此時的源 IP 和目標 IP 都沒變。

2. RS 收到 LVS 轉發來的包，鏈路層通過 MAC 識別到是發送給自己的，然後傳遞到上面的網絡層，發現 IP 同樣是自己的，那麼報文就繼續向上傳遞，這個包被合法地接受，RS 並沒有感知到 LVS 的存在。

3. RS 返回響應報文時，直接向源 IP 返回即可，此時不再經過 LVS，從而降低 LVS 服務器的壓力。

基本操作如下圖所示。

![network interface]({{ site.url }}/images/network/lvs-direct-routing.png){: .pull-center }


對於 DR 模式，需要注意如下幾點：

1. LVS 的 VIP 和 realserver 必須在同一個網段。

2. 所有的 realserver 都必須綁定 VIP 的 IP 地址，否則 realserver 收到報文後發現目的 IP 不是自己，將會把報文丟掉。

3. realserver 處理完包後直接把報文發送給客戶端，從而不需要再經過 LVS 服務器，進而提高效率，不過同時會將 realserver 的 IP 暴漏給外界。

假設有如下的網絡結構。

![network interface]({{ site.url }}/images/network/lvs-direct-routing-example.png){: .pull-center }

![network interface]({{ site.url }}/images/network/lvs-direct-routing-example-2.png){: .pull-center }

配置方法如下：

{% highlight text %}
----- 首先配置 Linux Director，其中VIP為172.17.60.201
---++ 在/etc/sysctl.conf中設置IP轉發
net.ipv4.ip_forward = 1
---++ 設置虛擬IP地址
# ifconfig eth0:0 172.17.60.201 netmask 255.255.0.0 broadcast 172.17.255.255
---++ 配置LVS
# ipvsadm -A -t 172.17.60.201:80
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.199:80 -g
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.200:80 -g

----- 在真正服務器上執行如下命令
---++ 需要注意netmask
# ifconfig lo:0 172.17.60.201 netmask 255.255.255.255
---++ 隱藏loopback，從而防止相應對VIP的ARP請求
# Enable configuration of hidden devices
net.ipv4.conf.all.hidden = 1
# Make the loopback interface hidden
net.ipv4.conf.lo.hidden = 1
{% endhighlight %}


### IP-IP Encapsulation (Tunnelling)

這種方法可以將報文發送到不同的子網內，同 DR 類似，返回的報文不需要經過 director 。示例仍然採用與 DR 相同的結構。

![network interface]({{ site.url }}/images/network/lvs-direct-routing-example.png){: .pull-center }

然後通過如下方式進行配置。

{% highlight text %}
----- 首先配置Linux Director
---++ 在/etc/sysctl.conf中設置IP轉發
net.ipv4.ip_forward = 1
---++ 設置虛擬IP地址
# ifconfig eth0:0 172.17.60.201 netmask 255.255.0.0 broadcast 172.17.255.255
---++ 配置LVS
# ipvsadm -A -t 172.17.60.201:80
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.199:80 -i
# ipvsadm -a -t 172.17.60.201:80 -r 172.17.60.200:80 -i

----- 配置Real Servers
---++ 配置tunl0
# ifconfig tunl0 172.17.60.201 netmask 255.255.255.255
---++ 同樣打開轉發，並隱藏loopback
net.ipv4.ip_forward = 1
# Enable configuration of hidden devices
net.ipv4.conf.all.hidden = 1
# Make the tunl0 interface hidden
net.ipv4.conf.tunl0.hidden = 1
{% endhighlight %}



### Full NAT

除了上述的 IP-IP Tunnelling 方式，LVS 和 RS 必須在同一個 LAN 或者 VLAN 下，否則 LVS 無法作為 RS 的網關。這將導致：A) 運維不方便，跨 VLAN 的 RS 無法接入；B) 水平擴展受限制。

Full-NAT 是為瞭解決此問題，他在 SNAT/DNAT 的基礎上，加上另一種轉換，過程如下。

![network interface]({{ site.url }}/images/network/lvs-full-nat-example.png){: .pull-center }

大致的執行過程如下：

1. 包從 LVS 轉到 RS 時，源地址從客戶端 IP 被替換成了 LVS 的內網 IP；利用內網 IP，可以通過多個交換機跨 VLAN 通信。

2. 當 RS 返回響應包時，會將這個包返回給 LVS 的內網 IP，這一步也不受限於 VLAN。

3. LVS 收到包後，在修改源地址的基礎上，再把 RS 發來的包中的目標地址從 LVS 內網 IP 改為客戶端的 IP。

採用這種方式，LVS 和 RS 的部署在 VLAN 上將不再有任何限制，大大提高了運維部署的便利性。





## 負載均衡算法

針對不同的網絡服務需求和服務器配置，IPVS 調度器實現瞭如下八種負載調度算法：

以及八種負載均衡的算法 (round robin、weighted round robin、least-connection、weighted least-connection、locality-based least-connection、locality-based least-connection with replication、destination-hashing、source-hashing)。

#### 輪詢 (Round Robin)

調度器將外部請求按順序輪流分配到集群中的真實服務器上，該算法平等地對待每一臺服務器，而不管服務器上實際的連接數和系統負載。

#### 加權輪詢 (Weighted Round Robin)

該算法可以根據真實服務器的不同處理能力來調度訪問請求，從而可以保證處理能力強的服務器處理更多的訪問流量，而調度器可以自動查詢真實服務器的負載情況，並動態地調整其權值。

#### 最少鏈接 (Least Connections)

調度器動態地將網絡請求調度到已建立的鏈接數最少的服務器上，如果集群系統的真實服務器具有相近的系統性能，採用 "最小連接" 調度算法可以較好地均衡負載。

#### 加權最少鏈接 (Weighted Least Connections)

在集群系統中的服務器性能差異較大的情況下，調度器採用 "加權最少鏈接" 調度算法優化負載均衡性能，具有較高權值的服務器將承受較大比例的活動連接負載。調度器可以自動問詢真實服務器的負載情況，並動態地調整其權值。

#### 基於局部性的最少鏈接 (Locality-Based Least Connections)

針對目標 IP 地址的負載均衡，目前主要用於 Cache 集群系統。該算法根據請求的目標IP地址找出該目標IP地址最近使用的服務器，若該服務器是可用的且沒有超載，將請求發送到該服務器；若服務器不存在，或者該服務器超載且有服務器處於一半的工作負載，則用"最少鏈接"的原則選出一個可用的服務器，將請求發送到該服務器。

#### 帶複製的基於局部性最少鏈接 (Locality-Based Least Connections with Replication)

"帶複製的基於局部性最少鏈接"調度算法也是針對目標IP地址的負載均衡，目前主要用於Cache集群系統。它與LBLC算法的不同之處是它要維護從一個目標IP地址到一組服務器的映射，而LBLC算法維護從一個目標IP地址到一臺服務器的映射。該算法根據請求的目標IP地址找出該目標IP地址對應的服務器組，按"最小連接"原則從服務器組中選出一臺服務器，若服務器沒有超載，將請求發送到該服務器，若服務器超載；則按"最小連接"原則從這個集群中選出一臺服務器，將該服務器加入到服務器組中，將請求發送到該服務器。同時，當該服務器組有一段時間沒有被修改，將最忙的服務器從服務器組中刪除，以降低複製的程度。 7.目標地址散列（Destination Hashing）

"目標地址散列"調度算法根據請求的目標IP地址，作為散列鍵（Hash Key）從靜態分配的散列表找出對應的服務器，若該服務器是可用的且未超載，將請求發送到該服務器，否則返回空。

#### 源地址散列 (Source Hashing)

"源地址散列"調度算法根據請求的源IP地址，作為散列鍵（Hash Key）從靜態分配的散列表找出對應的服務器，若該服務器是可用的且未超載，將請求發送到該服務器，否則返回空。



## ipvsadm

ipvsadm 是用戶層管理 LVS 的程序，

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

常見選項如下：
    -A    添加一個新的集群服務；
    -E    修改一個己有的集群服務；
    -D    刪除指定的集群服務；
    -a    向指定的集群服務中添加real server及屬性；
    -e    修改real server屬性；
    -r    指定real server的地址；

    -t    指定為tcp協議；
    -u    指定為udp協議；

    -g    Gateway, DR模型；
    -i    ipip, TUN模型；
    -m    masquerade, NAT模型；


    -f    指定防火牆標記碼，通常用於將兩個或以上的服務綁定為一個服務進行處理時使用；

    -s    調度方法，默認為wlc；



    -w    指定權重，默認為1；
    -p timeout   persistent connection, 持久連接超時時長；
    -S    保存ipvsadm設定的規則策略，默認保存在/etc/sysconfig/ipvsadm中；
    -R    載入己保存的規則策略，默認加載/etc/sysconfig/ipvsadm；
    -C    清除所有集群服務；
    -Z    清除所有記數器；
    -L    顯示當前己有集群服務，能通過相應的options查看不同狀態信息；
        -n: 數字格式顯示IP地址；
        -c: 顯示連接數相關信息；
        --stats: 顯示統計數據；
        --rate: 速率；
        --exact：顯示統計數據的精確值；


ipvsadm -A -t 192.168.132.254:80 -s rr -p 120
ipvsadm -a -t 192.168.132.254:80 -r 192.168.132.64:80 -g
ipvsadm -a -t 192.168.132.254:80 -r 192.168.132.68:80 -g

{% endhighlight %}

<!--
我們可以看到，此規則為一個VS ： 192.168.132.254：80， 兩個real server 分別是 192.168.132.64：80 與 192.168.132.68：80， LVS模式為DR，調度算法為Round Robin。
-->


## 參考

有兩篇不錯的文章 [Linux Virtual Server Tutorial](http://www.ultramonkey.org/papers/lvs_tutorial/html/)、[LVS - HOWTO](http://www.austintek.com/LVS/LVS-HOWTO/HOWTO/index.html)，比較老的文章，前者相比簡單些。另外 [ja.ssi.bg](http://ja.ssi.bg/) 中很多不錯的相關補丁，以及文章。

在 [LVS 中文網站](http://zh.linuxvirtualserver.org/) 中有幾篇比較經典的文章 [Linux服務器集群系統](http://www.linuxvirtualserver.org/zh/lvs1.html) 。

<!--
http://blog.csdn.net/raintungli/article/details/39051435
http://junwang.blog.51cto.com/5050337/1439428/
http://blog.csdn.net/pi9nc/article/details/23380589
http://www.it165.net/admin/html/201401/2248.html
http://blog.chinaunix.net/uid-11207493-id-4617479.html


真正零停機 HAProxy 重載
http://www.oschina.net/translate/zero-downtime-haproxy-reloads
-->

{% highlight text %}
{% endhighlight %}
