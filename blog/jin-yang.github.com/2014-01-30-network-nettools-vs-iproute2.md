---
title: netstat VS. ss
layout: post
comments: true
language: chinese
category: [linux]
keywords: netstat,ss,网络,监控
description: 现在很多系统管理员仍在使用 ifconfig、route、arp 和 netstat 等命令行工具，来管理和排查各种网络配置。这套工具也就是 net-tools 包，源于 BSD TCP/IP 工具箱，但是自 2001 年以后基本就停滞不前。 iproute 作为一套新的网络配置工具，旨在替换原有的 net-tools 工具包，而且，越来越多的发行版本开始使用 iproute 包替换 net-tools。 netstat 和 ss 命令只是这两个包中的代表，以此大概解析下两者的区别。
---

现在很多系统管理员仍在使用 ifconfig、route、arp 和 netstat 等命令行工具，来管理和排查各种网络配置。这套工具也就是 net-tools 包，源于 BSD TCP/IP 工具箱，但是自 2001 年以后基本就停滞不前。

iproute 作为一套新的网络配置工具，旨在替换原有的 net-tools 工具包，而且，越来越多的发行版本开始使用 iproute 包替换 net-tools。

netstat 和 ss 命令只是这两个包中的代表，以此大概解析下两者的区别。

<!-- more -->

## iproute VS. net-tools

两套包都可以完成网络的配置、查看，其中 net-tools 主要通过 procfs 和 ioctl 系统调用，访问和更改内核网络配置；而 iproute 则通过网络链路套接字接口与内核进行联系。

net-tools 采用的 proc 接口与 iproute 采用的 netlink 相比要笨拙的多；另外，ioctl 采用的是同步系统调用，而 netlink 内部采用的是 sock 队列，也就是异步通讯。

除了性能只外，iproute 的用户界面比 net-tools 要来得直观，而且有很多 net-tools 所不支持的功能。

![nettools-vs-iproute]{: .pull-center width="600"}

具体信息可以参考图片中的网站，仅列举如下部分。

### 路由表设置

查看路由表，在 net-tools 中包括了两种方式，而 iproute2 只有一种。

{% highlight text %}
----- 使用net-tools
# route -n
# netstat -rn

----- 使用iproute2
# ip route show
{% endhighlight %}

接下来看看如何修改路由，包括如何添加或改动内核 IP 路由表中的默认路由，添加或者删除静态路由。

{% highlight text %}
----- 使用net-tools，添加默认路由
# route add default gw 192.168.1.2 eth0
# route del default gw 192.168.1.1 eth0

----- 使用iproute2，添加默认路由
# ip route add default via 192.168.1.2 dev eth0
# ip route replace default via 192.168.1.2 dev eth0


----- 使用net-tools，添加或者删除静态路由
# route add -net 172.16.32.0/24 gw 192.168.1.1 dev eth0
# route del -net 172.16.32.0/24

----- 使用iproute2，添加或者删除静态路由
# ip route add 172.16.32.0/24 via 192.168.1.1 dev eth0
# ip route del 172.16.32.0/24
{% endhighlight %}

### 源码实现

ip 命令的实现在源码的 ip 目录下，在 ip.c 中根据不同的子命令调用不同的函数，而子命令与相应程序的映射关系在 cmds[] 数组中。

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

对于不同的命令可以直接查看对应的函数，如 do_iproute()，而与内核的交互仍然通过 netlink 执行。

关于两者的使用对比，详细可以参考 [Linux TCP/IP networking: net-tools vs. iproute2][iproute2-vs-nettools] 或者中文翻译版 [net-tools 与 iproute2 大比较][iproute2-vs-nettools-cn] 。


## Netstat VS. SS(Socket Statistics)

ss 用于显示 socket 状态，包括了 TCP sockets, UDP sockets, DCCP sockets, RAW sockets, Unix domain sockets 等统计。

直接通过 man 8 netstat 查看帮助时就可以发现这样的一句话 "This program is obsolete.  Replacement for netstat is ss."，而且后面还有几个相关的等价示例介绍。

明摆着 Netstat 这是要退居二线的节奏啊。

ss 与内核之间是通过 netlink 机制进行通讯的，相比 Netstat 来说要快速的多，据说即使都使用 procfs，ss 也要快一些，不过没验证过。如果要使用 netlink 机制，需要内核中相关模块的支持，如 tcp_diag、udp_diag、inet_diag、sock_diag 等，也就是 socket 相关的诊断模块。

内核中是否有相关的模块可以通过 lsmod \| grep diag 命令查看。

ss 使用的就是 TCP 协议栈中 tcp_diag 模块，一个用于分析统计的模块，可以获得 Linux 内核中第一手的信息。如上所述，如果系统中没有 tcp_diag，实际上 ss 也可以正常运行，只是效率会变得稍慢。

关于 ss 的帮助信息，可以直接通过 ss -h 查看，或者 man 8 ss 查看。

{% highlight text %}
-h, --help                          帮助信息
-V, --version                       程序版本信息
-n, --numeric                       不解析服务名称
-r, --resolve                       解析主机名

-a, --all                           显示所有套接字(sockets)
-l, --listening                     显示监听状态的套接字(sockets)

-o, --options                       显示计时器信息
-e, --extended                      显示详细的套接字(sockets)信息
-m, --memory                        显示套接字(socket)的内存使用情况
-p, --processes                     显示使用套接字(socket)的进程
-i, --info                          显示TCP内部信息
-s, --summary                       显示套接字(socket)使用概况

-4, --ipv4                          仅显示IPv4的套接字(sockets)
-6, --ipv6                          仅显示IPv6的套接字(sockets)
-0, --packet                        显示PACKET套接字（socket）
-t, --tcp                           仅显示TCP套接字(sockets)
-u, --udp                           仅显示UCP套接字(sockets)
-d, --dccp                          仅显示DCCP套接字(sockets)
-w, --raw                           仅显示RAW套接字(sockets)
-x, --unix                          仅显示Unix套接字(sockets)
-f, --family=FAMILY                 显示FAMILY类型的套接字(sockets)，支持unix,inet,inet6,link,netlink

-A, --query=QUERY, --socket=QUERY   指定scoket类型
      QUERY := {all|inet|tcp|udp|raw|unix|packet|netlink}[,QUERY]
-D, --diag=FILE                     将原始TCP套接字(sockets)信息转储到文件
-F, --filter=FILE                   从文件中都去过滤器信息
{% endhighlight %}
接下来，我们看些 ss 经常会使用到的示例。

### ss 使用示例

通过 ss 命令，可以查看网络的统计信息，查看相关类型的接口等。如下的示例很多可以从 man 8 ss 中查看，在此仅标记下。

#### 显示统计信息

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

#### 查看监听端口

查看所有打开监听的网络连接端口，可以通过 -p 选项同时显示相应的进程

{% highlight text %}
# ss -pl
State      Recv-Q Send-Q    Local Address:Port     Peer Address:Port
LISTEN     0      80        :::mysql               :::*                 users:(("mysqld",pid=7241,fd=20))
{% endhighlight %}

#### TCP链接信息

显示所有的TCP链接信息，其中 Recv-Q 表示收到的请求，Send-Q 表示可以接收的请求队列大小。

{% highlight text %}
$ ss -ta
State      Recv-Q Send-Q     Local Address:Port     Peer Address:Port
LISTEN     0      0              127.0.0.1:smux                *:*
LISTEN     0      0                      *:3690                *:*
LISTEN     0      0                      *:ssh                 *:*
ESTAB      0      0        192.168.120.204:ssh         10.2.0.68:49368
{% endhighlight %}

#### TCP状态

通过TCP状态进行过滤

{% highlight text %}
----- 所有状态为established的SMTP连接
$ ss -o state established '( dport = :smtp or sport = :smtp )'

----- 处于 FIN-WAIT-1状态的源端口为80或者443，目标网络为193.233.7/24所有tcp套接字
$ ss -o state fin-wait-1 '( sport = :http or sport = :https )' dst 193.233.7/24

----- 通过TCP状态过滤，其中的过滤状态可以通过ss -h查看
$ ss -4 state FILTER-NAME-HERE
{% endhighlight %}

#### 查看UNIX socket信息

{% highlight text %}
----- 列出本地哪个进程连接到X server
# ss -x src /tmp/.X11-unix/*

----- 查看本地MySQL socket链接信息
# ss -x src /tmp/mysql.sock
Netid  State      Recv-Q Send-Q    Local Address:Port          Peer Address:Port
u_str  ESTAB      0      0         /tmp/mysql.sock 14595374    * 14594660
{% endhighlight %}

#### 匹配进程、远程地址和端口号

匹配进程、远程地址和端口号，基本语法为 ```ss [dst|src] ADDRESS:PORT``` 。

{% highlight text %}
$ ss dst 192.168.1.5
$ ss dst 192.168.119.113:http
$ ss src 192.168.119.113:443
{% endhighlight %}

#### 本地或者远程端口

将本地或者远程端口和一个数比较，选择一个端口的范围，语法为 ```ss [dport|sport] OP PORT``` 。

{% highlight text %}
# ss  sport = :http
# ss  dport \> :1024

----- 其中OP操作可以为如下其中的一个比较操作
<= or le : 小于或等于端口号
>= or ge : 大于或等于端口号
== or eq : 等于端口号
!= or ne : 不等于端口号
<  or gt : 小于端口号
>  or lt : 大于端口号
{% endhighlight %}

上述的示例基本上可以满足常见的需求了。


### 查看内核参数

通过 ss 并结合 crash 命令可以用来查看与 socket 相关的内核参数信息，详细的信息可以参考 [Inspecting Internal TCP State on Linux][inspect-internal-tcp] 中的介绍，在此仅简单说明下。

默认 crash 命令是不会安装的，可以通过如下的命令行进行安装。
{% highlight text %}
# yum install crash
{% endhighlight %}
该程序可以用来分析内核的 dump 文件，或者对正在运行中的内核进行分析。在此的话，我们可以先通过 ss 命令获取到链接对应的 sock 地址信息，然后再利用 crash 命令查看当前链接的参数信息。

{% highlight text %}
----- 查看目的端口号是MySQL的链接详细信息，会显示该链接在内核中的struct sock地址信息
# ss -eipn '( dport = :mysql )'
Netid   State    Recv-Q   Send-Q  Local Address:Port    Peer Address:Port
tcp     ESTAB    0        0       127.0.0.1:57334       127.0.0.1:3306
    users:(("mysql",pid=7268,fd=3)) timer:(keepalive,118min,0) uid:1000 ino:13435670 sk:ffff88015e39e180 <->
    ... ...

----- 直接启动crash命令，查看sock相关信息，帮助直接查看help struct
# crash
... ...
crash> struct tcp_sock.rcv_nxt,snd_una,reordering ffff88015e39e180
  rcv_nxt = 1218355271
  snd_una = 725754512
  reordering = 3 '\003'
{% endhighlight %}

如上，通过 crash 中的 struct 命令就可以显示当时 sock 的相关结构体信息。

每个链接在内核中都会对应一个 struct sock 信息，包括了所有相关的状态信息。实际上，包括 struct sock 在内，TCP 协议中有四个结构体 tcp_sock inet_connection_sock inet_sock sock，而内核代码中经常可以看到对其指针进行强制类型转换，之所以这样，主要是根其成员变量的分布相关。
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
而上述的四个结构体，基本上就涵盖了该 socket 链接的所有信息。

### 源码实现

我们可以直接从 [www.kernel.org][iproute2-download] 下载 iproute2 的源码包，或者在 CentOS 中通过 yumdownloader 下载源码并解压。

其中 ss 相关的源码在 misc/ss.c 中，实现也比较简单，主要是解析选项、通过 netlink 获取内核中的状态数据、展示数据。

{% highlight text %}
main()
 |-netlink_show()                     # 根据不同的选项，对应不同的函数
 |-... ...
 |-tcp_show()
   |-inet_show_netlink()              # 主要的通过netlink通讯处理函数
     |-socket()                       # 建立套接字
     |-sockdiag_send()
     | |-tcpdiag_send()               # 如果family==PF_UNSPEC
     | | |-sendmsg()                  # 发送消息到内核
     | |-sendmsg()                    # 功能同上，只是不同的分支
     |-recvmsg()                      # 接收内核返回的消息
     |-inet_show_sock()               # 显示最终获取的结果
{% endhighlight %}

在 inet_show_netlink() 函数中基本就是新建 socket，构建报文，通过 netlink 进行通讯。

以 ss -nti 命令为例，也就是查看 TCP 链接相关的信息，如 RTO、RTT、CWND 等信息。

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
       case 'n':                // 不解析端口号
           resolve_services = 0;
           break;
       case 'i':                // 显示TCP相关信息
           show_tcpinfo = 1;
           break;
       case 't':                // 只包括TCP链接
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

而与内核中对应的就是 tcp_diag 模块，以 ss -atnn 为例，其显示的内容如下：

{% highlight text %}
$ ss -atnn
State      Recv-Q Send-Q    Local Address:Port       Peer Address:Port
LISTEN     0      128                   *:4000                  *:*
ESTAB      0      0         192.168.1.173:48372   101.201.173.115:80
... ...
{% endhighlight %}

如上的命令会输出所有的 socket 相关信息：A) 第一列表示 tcp socket 的状态；B) 第二列和第三列的内容与 socket 所处的状态有关。于此相关的在内核中 tcp_diag 代码如下：

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

如以上中的代码，不同状态的 socket 所返回的数值是不同的。

#### 处于 LISTEN 状态的 socket

* Recv-Q：已经完成三次握手，现存放于 accept 队列中，等待应用通过 accept() 系统调用获取 socket 。

* Send-Q：实际上就是 accetp 队列的最大值，也就是能容纳的最大完成三次握手的链接。

<!--
#### 非 LISTEN 状态的 socket
Recv-Q：代表了 receive queue中的字节数目（等待接收的下一个tcp段的序号-尚未从内核空间copy到用户空间的段最前面的一个序号）；Send-Q表示发送queue中容纳的字节数（已加入发送队列中最后一个序号-输出段中最早一个未确认的序号）
-->


### ss 命令的 timer 输出

单独拉出来系统介绍一下。

通过 ss 命令查看 timer 选项时，只需要添加一个 -o 选项即可。

{% highlight text %}
$ ss -atonn
State      Recv-Q Send-Q   Local Address:Port       Peer Address:Port
LISTEN     0      128                  *:4000                  *:*
ESTAB      0      0        192.168.1.173:46445   101.201.173.115:80       timer:(keepalive,6.427ms,0)
SYN-SENT   0      1        192.168.1.173:9983    101.201.174.210:80       timer:(on,355ms,0)
TIME-WAIT  0      0            127.0.0.1:4000          127.0.0.1:22173    timer:(timewait,43sec,0)
{% endhighlight %}

与上次的输出相比，多了一个 timer 输出，也就是 tcp socket 上的定时器。

在 tcp 内核中，共有 7 个定时器，通过 4 个 timer 实现；分别是：
通过icsk_retransmit_timer实现的重传定时器、零窗口探测定时器；通过sk_timer实现的连接建立定时器、保活定时器和FIN_WAIT_2定时器；通过icsk_delack_timer实现的延时ack定时器；以及TIME_WAIT定时器。

首先看一下 ss 中是如何进行处理的。

{% highlight c %}
static const char *tmr_name[] = {
	"off",        // 当前socket没有timer
	"on",         // 重传timer
	"keepalive",  // fin_wait_2或保活定时器，可根据连接的状态确定
	"timewait",   // TIME_WAIT 定时器
	"persist",    // 零窗口探测定时器
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
                tmr_name[r->idiag_timer],         // 对应定时器的类型
                print_ms_timer(r->idiag_expires), // 超时时间
                r->idiag_retrans);                // 重传次数
    }
    }
    ... ...
}
{% endhighlight %}

ss 的代码很简单，其中非 TIME_WAIT 类型的时钟在 int inet_sk_diag_fill() 函数中完成，对于 TIME_WAIT 定时器信息在内核的 inet_twsk_diag_fill() 函数中完成。

在此，只介绍下前者。

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

如上代码所示，ss 命令输出 timer 的含义为类型、过期时间、重试次数。





介绍先到此为止，详细内容后面再添加。


[iproute2-download]:        https://www.kernel.org/pub/linux/utils/net/iproute2/                         "iproute2 下载地址"
[iproute2-offical-site]:    http://www.linuxfoundation.org/collaborate/workgroups/networking/iproute2/   "iproute2 官方网站"
[iproute2-vs-nettools]:     http://xmodulo.com/linux-tcpip-networking-net-tools-iproute2.html            "iproute2 和 net-tools 的简单对比"
[iproute2-vs-nettools-cn]:  http://os.51cto.com/art/201409/450886.htm                                    "iproute2 和 net-tools 对比的中文版"
[inspect-internal-tcp]:     https://blogs.janestreet.com/inspecting-internal-tcp-state-on-linux/         "介绍如何通过ss+crash来查看内部TCP的状态"

[nettools-vs-iproute]:      /images/linux/nettools-vs-iproute.jpg                                        "nettools 与 iproute 的对比图"
<!--
http://kristrev.github.io/2013/07/26/passive-monitoring-of-sockets-on-linux/     一个类似与ss的程序
-->
