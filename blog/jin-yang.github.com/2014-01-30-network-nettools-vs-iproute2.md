---
title: netstat VS. ss
layout: post
comments: true
language: chinese
category: [linux]
keywords: netstat,ss,網絡,監控
description: 現在很多系統管理員仍在使用 ifconfig、route、arp 和 netstat 等命令行工具，來管理和排查各種網絡配置。這套工具也就是 net-tools 包，源於 BSD TCP/IP 工具箱，但是自 2001 年以後基本就停滯不前。 iproute 作為一套新的網絡配置工具，旨在替換原有的 net-tools 工具包，而且，越來越多的發行版本開始使用 iproute 包替換 net-tools。 netstat 和 ss 命令只是這兩個包中的代表，以此大概解析下兩者的區別。
---

現在很多系統管理員仍在使用 ifconfig、route、arp 和 netstat 等命令行工具，來管理和排查各種網絡配置。這套工具也就是 net-tools 包，源於 BSD TCP/IP 工具箱，但是自 2001 年以後基本就停滯不前。

iproute 作為一套新的網絡配置工具，旨在替換原有的 net-tools 工具包，而且，越來越多的發行版本開始使用 iproute 包替換 net-tools。

netstat 和 ss 命令只是這兩個包中的代表，以此大概解析下兩者的區別。

<!-- more -->

## iproute VS. net-tools

兩套包都可以完成網絡的配置、查看，其中 net-tools 主要通過 procfs 和 ioctl 系統調用，訪問和更改內核網絡配置；而 iproute 則通過網絡鏈路套接字接口與內核進行聯繫。

net-tools 採用的 proc 接口與 iproute 採用的 netlink 相比要笨拙的多；另外，ioctl 採用的是同步系統調用，而 netlink 內部採用的是 sock 隊列，也就是異步通訊。

除了性能只外，iproute 的用戶界面比 net-tools 要來得直觀，而且有很多 net-tools 所不支持的功能。

![nettools-vs-iproute]{: .pull-center width="600"}

具體信息可以參考圖片中的網站，僅列舉如下部分。

### 路由表設置

查看路由表，在 net-tools 中包括了兩種方式，而 iproute2 只有一種。

{% highlight text %}
----- 使用net-tools
# route -n
# netstat -rn

----- 使用iproute2
# ip route show
{% endhighlight %}

接下來看看如何修改路由，包括如何添加或改動內核 IP 路由表中的默認路由，添加或者刪除靜態路由。

{% highlight text %}
----- 使用net-tools，添加默認路由
# route add default gw 192.168.1.2 eth0
# route del default gw 192.168.1.1 eth0

----- 使用iproute2，添加默認路由
# ip route add default via 192.168.1.2 dev eth0
# ip route replace default via 192.168.1.2 dev eth0


----- 使用net-tools，添加或者刪除靜態路由
# route add -net 172.16.32.0/24 gw 192.168.1.1 dev eth0
# route del -net 172.16.32.0/24

----- 使用iproute2，添加或者刪除靜態路由
# ip route add 172.16.32.0/24 via 192.168.1.1 dev eth0
# ip route del 172.16.32.0/24
{% endhighlight %}

### 源碼實現

ip 命令的實現在源碼的 ip 目錄下，在 ip.c 中根據不同的子命令調用不同的函數，而子命令與相應程序的映射關係在 cmds[] 數組中。

{% highlight c %}
static const struct cmd {
    const char *cmd;
    int (*func)(int argc, char **argv);
} cmds[] = {
    { "address",    do_ipaddr },
    { "addrlabel",  do_ipaddrlabel },
    { "maddress",   do_multiaddr },
    { "route",  do_iproute },
    { "rule",   do_iprule },
    { "neighbor",   do_ipneigh },
    { "neighbour",  do_ipneigh },
    { "ntable", do_ipntable },
    { "ntbl",   do_ipntable },
    { "link",   do_iplink },
    { "l2tp",   do_ipl2tp },
    { "tunnel", do_iptunnel },
    { "tunl",   do_iptunnel },
    { "tuntap", do_iptuntap },
    { "tap",    do_iptuntap },
    { "token",  do_iptoken },
    { "tcpmetrics", do_tcp_metrics },
    { "tcp_metrics",do_tcp_metrics },
    { "monitor",    do_ipmonitor },
    { "xfrm",   do_xfrm },
    { "mroute", do_multiroute },
    { "mrule",  do_multirule },
    { "netns",  do_netns },
    { "netconf",    do_ipnetconf },
    { "help",   do_help },
    { 0 }
};
{% endhighlight %}

對於不同的命令可以直接查看對應的函數，如 do_iproute()，而與內核的交互仍然通過 netlink 執行。

關於兩者的使用對比，詳細可以參考 [Linux TCP/IP networking: net-tools vs. iproute2][iproute2-vs-nettools] 或者中文翻譯版 [net-tools 與 iproute2 大比較][iproute2-vs-nettools-cn] 。


## Netstat VS. SS(Socket Statistics)

ss 用於顯示 socket 狀態，包括了 TCP sockets, UDP sockets, DCCP sockets, RAW sockets, Unix domain sockets 等統計。

直接通過 man 8 netstat 查看幫助時就可以發現這樣的一句話 "This program is obsolete.  Replacement for netstat is ss."，而且後面還有幾個相關的等價示例介紹。

明擺著 Netstat 這是要退居二線的節奏啊。

ss 與內核之間是通過 netlink 機制進行通訊的，相比 Netstat 來說要快速的多，據說即使都使用 procfs，ss 也要快一些，不過沒驗證過。如果要使用 netlink 機制，需要內核中相關模塊的支持，如 tcp_diag、udp_diag、inet_diag、sock_diag 等，也就是 socket 相關的診斷模塊。

內核中是否有相關的模塊可以通過 lsmod \| grep diag 命令查看。

ss 使用的就是 TCP 協議棧中 tcp_diag 模塊，一個用於分析統計的模塊，可以獲得 Linux 內核中第一手的信息。如上所述，如果系統中沒有 tcp_diag，實際上 ss 也可以正常運行，只是效率會變得稍慢。

關於 ss 的幫助信息，可以直接通過 ss -h 查看，或者 man 8 ss 查看。

{% highlight text %}
-h, --help                          幫助信息
-V, --version                       程序版本信息
-n, --numeric                       不解析服務名稱
-r, --resolve                       解析主機名

-a, --all                           顯示所有套接字(sockets)
-l, --listening                     顯示監聽狀態的套接字(sockets)

-o, --options                       顯示計時器信息
-e, --extended                      顯示詳細的套接字(sockets)信息
-m, --memory                        顯示套接字(socket)的內存使用情況
-p, --processes                     顯示使用套接字(socket)的進程
-i, --info                          顯示TCP內部信息
-s, --summary                       顯示套接字(socket)使用概況

-4, --ipv4                          僅顯示IPv4的套接字(sockets)
-6, --ipv6                          僅顯示IPv6的套接字(sockets)
-0, --packet                        顯示PACKET套接字（socket）
-t, --tcp                           僅顯示TCP套接字(sockets)
-u, --udp                           僅顯示UCP套接字(sockets)
-d, --dccp                          僅顯示DCCP套接字(sockets)
-w, --raw                           僅顯示RAW套接字(sockets)
-x, --unix                          僅顯示Unix套接字(sockets)
-f, --family=FAMILY                 顯示FAMILY類型的套接字(sockets)，支持unix,inet,inet6,link,netlink

-A, --query=QUERY, --socket=QUERY   指定scoket類型
      QUERY := {all|inet|tcp|udp|raw|unix|packet|netlink}[,QUERY]
-D, --diag=FILE                     將原始TCP套接字(sockets)信息轉儲到文件
-F, --filter=FILE                   從文件中都去過濾器信息
{% endhighlight %}
接下來，我們看些 ss 經常會使用到的示例。

### ss 使用示例

通過 ss 命令，可以查看網絡的統計信息，查看相關類型的接口等。如下的示例很多可以從 man 8 ss 中查看，在此僅標記下。

#### 顯示統計信息

{% highlight text %}
$ ss -s
Total: 1009 (kernel 1079)
TCP:   57 (estab 23, closed 16, orphaned 2, synrecv 0, timewait 6/0), ports 0

Transport Total     IP        IPv6
*         1079      -         -
RAW       2         1         1
UDP       29        26        3
TCP       41        34        7
INET      72        61        11
FRAG      0         0         0
{% endhighlight %}

#### 查看監聽端口

查看所有打開監聽的網絡連接端口，可以通過 -p 選項同時顯示相應的進程

{% highlight text %}
# ss -pl
State      Recv-Q Send-Q    Local Address:Port     Peer Address:Port
LISTEN     0      80        :::mysql               :::*                 users:(("mysqld",pid=7241,fd=20))
{% endhighlight %}

#### TCP鏈接信息

顯示所有的TCP鏈接信息，其中 Recv-Q 表示收到的請求，Send-Q 表示可以接收的請求隊列大小。

{% highlight text %}
$ ss -ta
State      Recv-Q Send-Q     Local Address:Port     Peer Address:Port
LISTEN     0      0              127.0.0.1:smux                *:*
LISTEN     0      0                      *:3690                *:*
LISTEN     0      0                      *:ssh                 *:*
ESTAB      0      0        192.168.120.204:ssh         10.2.0.68:49368
{% endhighlight %}

#### TCP狀態

通過TCP狀態進行過濾

{% highlight text %}
----- 所有狀態為established的SMTP連接
$ ss -o state established '( dport = :smtp or sport = :smtp )'

----- 處於 FIN-WAIT-1狀態的源端口為80或者443，目標網絡為193.233.7/24所有tcp套接字
$ ss -o state fin-wait-1 '( sport = :http or sport = :https )' dst 193.233.7/24

----- 通過TCP狀態過濾，其中的過濾狀態可以通過ss -h查看
$ ss -4 state FILTER-NAME-HERE
{% endhighlight %}

#### 查看UNIX socket信息

{% highlight text %}
----- 列出本地哪個進程連接到X server
# ss -x src /tmp/.X11-unix/*

----- 查看本地MySQL socket鏈接信息
# ss -x src /tmp/mysql.sock
Netid  State      Recv-Q Send-Q    Local Address:Port          Peer Address:Port
u_str  ESTAB      0      0         /tmp/mysql.sock 14595374    * 14594660
{% endhighlight %}

#### 匹配進程、遠程地址和端口號

匹配進程、遠程地址和端口號，基本語法為 ```ss [dst|src] ADDRESS:PORT``` 。

{% highlight text %}
$ ss dst 192.168.1.5
$ ss dst 192.168.119.113:http
$ ss src 192.168.119.113:443
{% endhighlight %}

#### 本地或者遠程端口

將本地或者遠程端口和一個數比較，選擇一個端口的範圍，語法為 ```ss [dport|sport] OP PORT``` 。

{% highlight text %}
# ss  sport = :http
# ss  dport \> :1024

----- 其中OP操作可以為如下其中的一個比較操作
<= or le : 小於或等於端口號
>= or ge : 大於或等於端口號
== or eq : 等於端口號
!= or ne : 不等於端口號
<  or gt : 小於端口號
>  or lt : 大於端口號
{% endhighlight %}

上述的示例基本上可以滿足常見的需求了。


### 查看內核參數

通過 ss 並結合 crash 命令可以用來查看與 socket 相關的內核參數信息，詳細的信息可以參考 [Inspecting Internal TCP State on Linux][inspect-internal-tcp] 中的介紹，在此僅簡單說明下。

默認 crash 命令是不會安裝的，可以通過如下的命令行進行安裝。
{% highlight text %}
# yum install crash
{% endhighlight %}
該程序可以用來分析內核的 dump 文件，或者對正在運行中的內核進行分析。在此的話，我們可以先通過 ss 命令獲取到鏈接對應的 sock 地址信息，然後再利用 crash 命令查看當前鏈接的參數信息。

{% highlight text %}
----- 查看目的端口號是MySQL的鏈接詳細信息，會顯示該鏈接在內核中的struct sock地址信息
# ss -eipn '( dport = :mysql )'
Netid   State    Recv-Q   Send-Q  Local Address:Port    Peer Address:Port
tcp     ESTAB    0        0       127.0.0.1:57334       127.0.0.1:3306
    users:(("mysql",pid=7268,fd=3)) timer:(keepalive,118min,0) uid:1000 ino:13435670 sk:ffff88015e39e180 <->
    ... ...

----- 直接啟動crash命令，查看sock相關信息，幫助直接查看help struct
# crash
... ...
crash> struct tcp_sock.rcv_nxt,snd_una,reordering ffff88015e39e180
  rcv_nxt = 1218355271
  snd_una = 725754512
  reordering = 3 '\003'
{% endhighlight %}

如上，通過 crash 中的 struct 命令就可以顯示當時 sock 的相關結構體信息。

每個鏈接在內核中都會對應一個 struct sock 信息，包括了所有相關的狀態信息。實際上，包括 struct sock 在內，TCP 協議中有四個結構體 tcp_sock inet_connection_sock inet_sock sock，而內核代碼中經常可以看到對其指針進行強制類型轉換，之所以這樣，主要是根其成員變量的分佈相關。
{% highlight c %}
struct sock {
    ... ...
};
struct inet_sock {
    struct sock       sk;
    ... ...
};
struct inet_connection_sock {
    struct inet_sock  icsk_inet;
    ... ...
};
struct tcp_sock {
    struct inet_connection_sock inet_conn;
    ... ...
};
{% endhighlight %}
而上述的四個結構體，基本上就涵蓋了該 socket 鏈接的所有信息。

### 源碼實現

我們可以直接從 [www.kernel.org][iproute2-download] 下載 iproute2 的源碼包，或者在 CentOS 中通過 yumdownloader 下載源碼並解壓。

其中 ss 相關的源碼在 misc/ss.c 中，實現也比較簡單，主要是解析選項、通過 netlink 獲取內核中的狀態數據、展示數據。

{% highlight text %}
main()
 |-netlink_show()                     # 根據不同的選項，對應不同的函數
 |-... ...
 |-tcp_show()
   |-inet_show_netlink()              # 主要的通過netlink通訊處理函數
     |-socket()                       # 建立套接字
     |-sockdiag_send()
     | |-tcpdiag_send()               # 如果family==PF_UNSPEC
     | | |-sendmsg()                  # 發送消息到內核
     | |-sendmsg()                    # 功能同上，只是不同的分支
     |-recvmsg()                      # 接收內核返回的消息
     |-inet_show_sock()               # 顯示最終獲取的結果
{% endhighlight %}

在 inet_show_netlink() 函數中基本就是新建 socket，構建報文，通過 netlink 進行通訊。

以 ss -nti 命令為例，也就是查看 TCP 鏈接相關的信息，如 RTO、RTT、CWND 等信息。

{% highlight c %}
static int inet_show_netlink(struct filter *f, FILE *dump_fp, int protocol)
{
    ... ...
    if ((fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_INET_DIAG)) < 0)
        return -1;

    family = PF_INET;
again:
    if (sockdiag_send(family, fd, protocol, f))
        return -1;

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;

    iov[0] = (struct iovec){
        .iov_base = buf,
        .iov_len = sizeof(buf)
    };
    ... ...
}

static int tcp_show(struct filter *f, int socktype)
{
    ... ...
    if (!getenv("PROC_NET_TCP") && !getenv("PROC_ROOT")
        && inet_show_netlink(f, NULL, socktype) == 0)
        return 0;
    ... ...
}

int main(int argc, char *argv[])
{
    ... ...
    while ((ch = getopt_long(argc, argv, "dhaletuwxnro460spbf:miA:D:F:vV",
                 long_opts, NULL)) != EOF) {
       switch(ch) {
       case 'n':                // 不解析端口號
           resolve_services = 0;
           break;
       case 'i':                // 顯示TCP相關信息
           show_tcpinfo = 1;
           break;
       case 't':                // 只包括TCP鏈接
           current_filter.dbs |= (1<<TCP_DB);
           do_default = 0;
           break;
       }
    }
    ... ...
    if (current_filter.dbs & (1<<TCP_DB))
        tcp_show(&current_filter, IPPROTO_TCP);
    return 0;
}
{% endhighlight %}

而與內核中對應的就是 tcp_diag 模塊，以 ss -atnn 為例，其顯示的內容如下：

{% highlight text %}
$ ss -atnn
State      Recv-Q Send-Q    Local Address:Port       Peer Address:Port
LISTEN     0      128                   *:4000                  *:*
ESTAB      0      0         192.168.1.173:48372   101.201.173.115:80
... ...
{% endhighlight %}

如上的命令會輸出所有的 socket 相關信息：A) 第一列表示 tcp socket 的狀態；B) 第二列和第三列的內容與 socket 所處的狀態有關。於此相關的在內核中 tcp_diag 代碼如下：

{% highlight c %}
static void tcp_diag_get_info(struct sock *sk, struct inet_diag_msg *r,
                  void *_info)
{
    ... ...
    if (sk->sk_state == TCP_LISTEN) {
        r->idiag_rqueue = sk->sk_ack_backlog;
        r->idiag_wqueue = sk->sk_max_ack_backlog;
    } else {
        r->idiag_rqueue = max_t(int, tp->rcv_nxt - tp->copied_seq, 0);
        r->idiag_wqueue = tp->write_seq - tp->snd_una;
    }
    ... ...
}
{% endhighlight %}

如以上中的代碼，不同狀態的 socket 所返回的數值是不同的。

#### 處於 LISTEN 狀態的 socket

* Recv-Q：已經完成三次握手，現存放於 accept 隊列中，等待應用通過 accept() 系統調用獲取 socket 。

* Send-Q：實際上就是 accetp 隊列的最大值，也就是能容納的最大完成三次握手的鏈接。

<!--
#### 非 LISTEN 狀態的 socket
Recv-Q：代表了 receive queue中的字節數目（等待接收的下一個tcp段的序號-尚未從內核空間copy到用戶空間的段最前面的一個序號）；Send-Q表示發送queue中容納的字節數（已加入發送隊列中最後一個序號-輸出段中最早一個未確認的序號）
-->


### ss 命令的 timer 輸出

單獨拉出來系統介紹一下。

通過 ss 命令查看 timer 選項時，只需要添加一個 -o 選項即可。

{% highlight text %}
$ ss -atonn
State      Recv-Q Send-Q   Local Address:Port       Peer Address:Port
LISTEN     0      128                  *:4000                  *:*
ESTAB      0      0        192.168.1.173:46445   101.201.173.115:80       timer:(keepalive,6.427ms,0)
SYN-SENT   0      1        192.168.1.173:9983    101.201.174.210:80       timer:(on,355ms,0)
TIME-WAIT  0      0            127.0.0.1:4000          127.0.0.1:22173    timer:(timewait,43sec,0)
{% endhighlight %}

與上次的輸出相比，多了一個 timer 輸出，也就是 tcp socket 上的定時器。

在 tcp 內核中，共有 7 個定時器，通過 4 個 timer 實現；分別是：
通過icsk_retransmit_timer實現的重傳定時器、零窗口探測定時器；通過sk_timer實現的連接建立定時器、保活定時器和FIN_WAIT_2定時器；通過icsk_delack_timer實現的延時ack定時器；以及TIME_WAIT定時器。

首先看一下 ss 中是如何進行處理的。

{% highlight c %}
static const char *tmr_name[] = {
	"off",        // 當前socket沒有timer
	"on",         // 重傳timer
	"keepalive",  // fin_wait_2或保活定時器，可根據連接的狀態確定
	"timewait",   // TIME_WAIT 定時器
	"persist",    // 零窗口探測定時器
	"unknown"
};

static int inet_show_sock(struct nlmsghdr *nlh, struct filter *f)
{
    ... ...
    if (show_options) {
        if (r->idiag_timer) {
            if (r->idiag_timer > 4)
                r->idiag_timer = 5;
            printf(" timer:(%s,%s,%d)",
                tmr_name[r->idiag_timer],         // 對應定時器的類型
                print_ms_timer(r->idiag_expires), // 超時時間
                r->idiag_retrans);                // 重傳次數
    }
    }
    ... ...
}
{% endhighlight %}

ss 的代碼很簡單，其中非 TIME_WAIT 類型的時鐘在 int inet_sk_diag_fill() 函數中完成，對於 TIME_WAIT 定時器信息在內核的 inet_twsk_diag_fill() 函數中完成。

在此，只介紹下前者。

{% highlight c %}
int inet_sk_diag_fill(... ...)
{
    ... ...
    #define EXPIRES_IN_MS(tmo)  DIV_ROUND_UP((tmo - jiffies) * 1000, HZ)
    if (icsk->icsk_pending == ICSK_TIME_RETRANS ||
        icsk->icsk_pending == ICSK_TIME_EARLY_RETRANS ||
        icsk->icsk_pending == ICSK_TIME_LOSS_PROBE) {
        r->idiag_timer = 1;
        r->idiag_retrans = icsk->icsk_retransmits;
        r->idiag_expires = EXPIRES_IN_MS(icsk->icsk_timeout);
    } else if (icsk->icsk_pending == ICSK_TIME_PROBE0) {
        r->idiag_timer = 4;
        r->idiag_retrans = icsk->icsk_probes_out;
        r->idiag_expires = EXPIRES_IN_MS(icsk->icsk_timeout);
    } else if (timer_pending(&sk->sk_timer)) {
        r->idiag_timer = 2;
        r->idiag_retrans = icsk->icsk_probes_out;
        r->idiag_expires = EXPIRES_IN_MS(sk->sk_timer.expires);
    } else {
        r->idiag_timer = 0;
        r->idiag_expires = 0;
    }
#undef EXPIRES_IN_MS
    ... ...
}
{% endhighlight %}

如上代碼所示，ss 命令輸出 timer 的含義為類型、過期時間、重試次數。





介紹先到此為止，詳細內容後面再添加。


[iproute2-download]:        https://www.kernel.org/pub/linux/utils/net/iproute2/                         "iproute2 下載地址"
[iproute2-offical-site]:    http://www.linuxfoundation.org/collaborate/workgroups/networking/iproute2/   "iproute2 官方網站"
[iproute2-vs-nettools]:     http://xmodulo.com/linux-tcpip-networking-net-tools-iproute2.html            "iproute2 和 net-tools 的簡單對比"
[iproute2-vs-nettools-cn]:  http://os.51cto.com/art/201409/450886.htm                                    "iproute2 和 net-tools 對比的中文版"
[inspect-internal-tcp]:     https://blogs.janestreet.com/inspecting-internal-tcp-state-on-linux/         "介紹如何通過ss+crash來查看內部TCP的狀態"

[nettools-vs-iproute]:      /images/linux/nettools-vs-iproute.jpg                                        "nettools 與 iproute 的對比圖"
<!--
http://kristrev.github.io/2013/07/26/passive-monitoring-of-sockets-on-linux/     一個類似與ss的程序
-->
