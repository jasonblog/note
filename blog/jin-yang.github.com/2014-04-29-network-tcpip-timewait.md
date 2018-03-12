---
title: TCP/IP 协议之 TIME_WAIT
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: 网络,linux,network,time wait
description: TIME_WAIT 是 TCP 协议栈中比较特殊的状态，其主要目的是保证不同的链接不会相互干扰，但是对于一些高性能的场景，就可能由于较多的 TIME_WAIT 状态最终导致链接不可用。如下简单介绍如何充分利用该状态。
---

```TIME_WAIT``` 是 TCP 协议栈中比较特殊的状态，其主要目的是保证不同的链接不会相互干扰，但是对于一些高性能的场景，就可能由于较多的 ```TIME_WAIT``` 状态最终导致链接不可用。

如下简单介绍如何充分利用该状态。

<!-- more -->

## 简介

如下是 TCP 的状态转换图，主动关闭链接的一端会进入 ```TIME_WAIT``` 状态。

![TCP/IP Finite State Machine FSM]({{ site.url }}/images/network/tcpip_The-TCP-Finite-State-MachineFSM.png "TCP/IP Finite State Machine FSM"){: .pull-center}

可以通过如下命令统计当前不同状态的链接数。

{% highlight text %}
----- 建议使用后者，要快很多
# netstat -ant | awk '/^tcp/ {++state[$NF]} END {for(key in state) print key,state[key]}' | sort -rnk2
# ss -ant | awk '!/^State/ {++state[$1]} END {for(key in state) print key,state[key]}' | sort -rnk2
{% endhighlight %}

<!--
netstat -atunp > /tmp/netstat.log
cat /tmp/netstat.log | grep 'TIME_WAIT' | awk '{ print $4 }' | sort | uniq -c | sort -nr | head -5
cat /tmp/netstat.log | grep '192.30.16.133:10051' | awk '{ print $5 }' | cut -d: -f1 | sort -u | wc -l
-->

如上，```TIME_WAIT``` 是在发起主动关闭一方，完成四次挥手后 TCP 状态转换为 ```TIME_WAIT``` 状态，并且该状态会保持两个 MSL 。

在 Linux 里一个 MSL 为 30s，不可配置，在 ```include/net/tcp.h``` 中定义如下。

{% highlight c %}
#define TCP_TIMEWAIT_LEN (60*HZ) /* how long to wait to destroy TIME-WAIT
                                  * state, about 60 seconds */
{% endhighlight %}

### 原因

之所以采用两个 MSL 主要是为了可靠安全的关闭 TCP 连接。

#### 报文延迟

最常见的是为了防止前一个链接的延迟报文被下一个链接接收，当然这两个链接的四元组 (source address, source port, destination address, destination port) 是相同的，而且序列号 sequence 也是在特定范围内的。

虽然满足上述场景的概率很小，但是对于 fast connections 以及滑动窗口很大时，会增加出现这种场景的概率。关于 ```TIME_WAIT``` 详细讨论可以参考 [rfc1337](https://tools.ietf.org/html/rfc1337) 。

<!-- TODO: net.ipv4.tcp_rfc1337 -->

下图是一个延迟延迟报文，在原链接已经正常关闭，并使用相同的端口建立了新链接，那么上个链接发送的报文可能混入新的链接中。

![timewait duplicate segment]({{ site.url }}/images/network/tcpip_timewait_duplicate_segment.png "timewait duplicate segment"){: .pull-center }

<!-- 主动关闭最后一个 FIN 发送延迟，那么新创建而且复用的 fd 开始就会收到 FIN 报文，直接进入关闭流程；或者由于 SEQ 不对，可能直接收到 RST 报文，影响新链接的创建。 -->

#### 确保远端以关闭

当发送的最后一个 ACK 丢失后，远端处于 LAST_ACK 状态，在此状态时，如果没有收到最后的 ACK 报文，那么就会重发 FIN 。

如下图，当 FIN 丢失，那么被动关闭方会处于 LAST_ACK 状态，那么尝试重新建立链接时，会直接发送一个 RST 关闭链接，影响新链接创建。

![timewait last ack]({{ site.url }}/images/network/tcpip_timewait_last_ack.png "timewait last ack"){: .pull-center }

注意，处于 ```LAST_ACK``` 状态的链接，如果没有收到最后一个 ACK 报文，那么就会一致重发 FIN 报文，直到如下条件：

1. 由于超时自动关闭链接；
2. 收到了 ACK 报文然后关闭链接；
3. 收到了 RST 报文并关闭链接。

简言之，通过 2MSL 等待时间，保证前一个链接的报文已经在网络上消失，保证双方都已经正常关闭链接。


## 产生影响

如果有较多的链接处于 ```TIME_WAIT``` 状态 (可以通过 ```ss -tan state time-wait | wc -l``` 查看)，那么一般会有如下几个方面的影响：

1. 占用文件描述符 (fd) ，会导致无法创建相同类型的链接；
2. 内核中 socket 结构体占用的内存资源；
3. 额外的 CPU 消耗。

接下来，一个一个看看其影响。

### 文件描述符

一个链接在 ```TIME_WAIT``` 保存一分钟，那么相同四元组 (saddr, sport, daddr, dport) 的链接就无法创建；实际上，如果从内核角度看，实际上根据配置项，还可能包含了 sequence 以及 timestamp 。

如果服务器是挂载在一个 L7 Load-Balancer 之后的，那么源地址是相同的，而 Linux 中的 Port 分配范围是通过 ```net.ipv4.ip_local_port_range``` 参数配置的，默认是 3W 左右的可用端口，那么平均是每秒 500 个链接。

在客户端，那么就会报 ```EADDRNOTAVAIL (Cannot assign requested address)``` 错误，而服务端的排查相比来说要复杂的多，如下可以查看当前与客户端的链接数。

{% highlight text %}
----- sport就是Local Address列对应的端口
# ss -tan 'sport = :80' | awk '/^TIME-WAIT/ {print $(NF)" "$(NF-1)}' | \
    sed 's/:[^ ]*//g' | sort | uniq -c | sort -rn
{% endhighlight %}

针对这一场景，那么就是如何增加四元组，按照难度依次列举如下：

1. 客户端添加更多的端口范围，设置 ```net.ipv4.ip_local_port_range```；
2. 服务端增加监听端口，例如额外增加 81、82、83 ...；

<!--
3.
use more client IP by configuring additional IP on the load balancer and use them in a round-robin fashion6,
use more server IP by configuring additional IP on the web server7.
-->

注意，在客户端，不同版本的内核其行为也略有区别，老版本内核会查找空闲的本地二元组 (source address, source port)，此时增加服务端的 IP 以及 port 时不会增大链接数；而 Linux 3.2 之后，对于不同的四元组那么可以复用本地的二元组。

<!-- http://marc.info/?l=haproxy&m=139315382127339&w=2 -->

<!-- TODO: To avoid EADDRINUSE errors, the load balancer needs to use the SO_REUSEADDR option before calling bind(), then connect(). -->

最后就是调整 ```net.ipv4.tcp_tw_reuse``` 和 ```net.ipv4.tcp_tw_recycle``` 参数，不过在一些场景下可能会有问题，所以尽量不要使用，下面再详细介绍。

### 内存

假设每秒要处理 1W 的链接，那么在 Linux 中处于 ```TIME_WAIT``` 状态的链接数就有 60W ，这样的话是否会消耗很多的内存资源？

首先，应用端的 ```TIME_WAIT``` 状态链接已经关闭，所以不会消耗资源；主要资源的消耗在内核中。内核中保存了三个不同的结构体：

#### HASH TABLE

```connection hash table``` 用于快速定位一个现有的链接，例如接收到一个报文快速定位到链接结构体。

{% highlight text %}
$ dmesg | grep "TCP established hash table"
[    0.292951] TCP established hash table entries: 65536 (order: 7, 524288 bytes)
{% endhighlight %}

可以在内核启动时通过 ```thash_entries``` 参数设置，系统启动时会调用 ```alloc_large_system_hash()``` 函数初始化，并打印上述的启动日志信息。

在内核中，处于 ```TIME_WAIT``` 状态的链接使用的是 ```struct tcp_timewait_sock``` 结构体，而其它状态则使用的是 ```struct tcp_sock``` 。

{% highlight c %}
struct inet_timewait_sock {
    struct sock_common  __tw_common;

    int                     tw_timeout;
    volatile unsigned char  tw_substate;
    unsigned char           tw_rcv_wscale;
    __be16 tw_sport;
    unsigned int tw_ipv6only     : 1,
                 tw_transparent  : 1,
                 tw_pad          : 6,
                 tw_tos          : 8,
                 tw_ipv6_offset  : 16;
    unsigned long            tw_ttd;
    struct inet_bind_bucket *tw_tb;
    struct hlist_node        tw_death_node;
};

struct tcp_timewait_sock {
    struct inet_timewait_sock tw_sk;
    u32    tw_rcv_nxt;
    u32    tw_snd_nxt;
    u32    tw_rcv_wnd;
    u32    tw_ts_offset;
    u32    tw_ts_recent;
    long   tw_ts_recent_stamp;
};
{% endhighlight %}


<!--
Each element of the list of connections in the TIME-WAIT state is a struct tcp_timewait_sock, while the type for other states is struct tcp_sock8:
-->

#### TIME_WAIT

用于判断处于 ```TIME_WAIT``` 状态的 socket 还有多长时间过期，与上述的 hash 表使用相同的结构体，也就是对应了 ```struct inet_timewait_sock``` 结构体中的 ```struct hlist_node tw_death_node``` 。

#### HASH TABLE

```bind hash table``` 用于保存本地已经保存的端口以及相关参数，用于绑定 listen 端口以及查找可用端口。

{% highlight text %}
$ dmesg | grep "TCP bind hash table"
[    0.293146] TCP bind hash table entries: 65536 (order: 8, 1048576 bytes)
{% endhighlight %}

这里每个元素使用 ```struct inet_bind_socket``` 结构体，大小与 connection 相同。

重点关注两个 hash 表使用的内存即可。

{% highlight text %}
# slabtop -o | grep -E '(^  OBJS|tw_sock_TCP|tcp_bind_bucket)'
  OBJS ACTIVE  USE OBJ SIZE  SLABS OBJ/SLAB CACHE SIZE NAME                   
 50955  49725  97%    0.25K   3397       15     13588K tw_sock_TCP            
 44840  36556  81%    0.06K    760       59      3040K tcp_bind_bucket
{% endhighlight %}

## 内核参数调优

相关参数可以查看内核文档 [ip-sysctl.txt]({{ site.kernel_docs_url }}/Documentation/networking/ip-sysctl.txt) ，仅摘取与此相关的参数介绍：

{% highlight text %}
tcp_tw_reuse - BOOLEAN
    Allow to reuse TIME-WAIT sockets for new connections when it is
    safe from protocol viewpoint. Default value is 0.
    It should not be changed without advice/request of technical experts.

tcp_tw_recycle - BOOLEAN
    Enable fast recycling TIME-WAIT sockets. Default value is 0.
    It should not be changed without advice/request of technical experts.
{% endhighlight %}

注意，在使用上述参数时，需要先开启 tcp_timestamps 参数，否则这里的配置时无效的，也就是需要开启如下参数。

{% highlight text %}
----- 查看当前状态
# sysctl net.ipv4.tcp_tw_reuse
# sysctl net.ipv4.tcp_tw_recycle
# sysctl net.ipv4.tcp_timestamps

----- 将对应的参数开启
# sysctl net.ipv4.tcp_tw_reuse=1
# sysctl net.ipv4.tcp_tw_recycle=1
# sysctl net.ipv4.tcp_timestamps=1

----- 写入到配置文件，持久化，并使配置立即生效
# cat << EOF >> /etc/sysctl.conf
net.ipv4.tcp_tw_reuse = 1
net.ipv4.tcp_tw_recycle = 1
net.ipv4.tcp_timestamps = 1
EOF
# /sbin/sysctl -p
{% endhighlight %}

<!--
net.ipv4.tcp_fin_timeout = 30
net.ipv4.tcp_syn_retries = 5
net.ipv4.tcp_synack_retries = 5
net.ipv4.tcp_keepalive_time = 1200
net.ipv4.tcp_max_tw_buckets = 10

net.ipv4.tcp_syncookies = 1 表示开启SYN Cookies。当出现SYN等待队列溢出时，启用cookies来处理，可防范少量SYN攻击，默认为0，表示关闭；
net.ipv4.tcp_tw_reuse = 1 表示开启重用。允许将TIME-WAIT sockets重新用于新的TCP连接，默认为0，表示关闭；
net.ipv4.tcp_tw_recycle = 1 表示开启TCP连接中TIME-WAIT sockets的快速回收，默认为0，表示关闭。
net.ipv4.tcp_fin_timeout = 30 表示如果套接字由本端要求关闭，这个参数决定了它保持在FIN-WAIT-2状态的时间。
net.ipv4.tcp_keepalive_time = 1200 表示当keepalive起用的时候，TCP发送keepalive消息的频度。缺省是2小时，改为20分钟。
net.ipv4.tcp_max_syn_backlog = 8192 表示SYN队列的长度，默认为1024，加大队列长度为8192，可以容纳更多等待连接的网络连接数。
net.ipv4.tcp_max_tw_buckets = 5000 表示系统同时保持TIME_WAIT套接字的最大数量，如果超过这个数字，TIME_WAIT套接字将立刻被清除并打印警告信息。
默  认为180000，改为5000。对于Apache、Nginx等服务器，上几行的参数可以很好地减少TIME_WAIT套接字数量，但是对于Squid，效果却不大。此项参数可以控制TIME_WAIT套接字的最大数量，避免Squid服务器被大量的TIME_WAIT套接字拖死。
-->



### tcp_tw_reuse

如上所述，```TIME_WAIT``` 状态主要是为了防止延迟报文影响新链接，不过在有些场景下可以确保不会出现这种情况。

[RFC 1323](https://tools.ietf.org/html/rfc1323) 提供了一个 TCP 扩展项，其中定义了两个四字节的时间戳选项，第一个是发送时的发送端时间戳，第二个是从远端接收报文的最近时间戳。

{% highlight text %}
TCP Timestamps Option (TSopt):
    +-------+-------+---------------------+---------------------+
    |Kind=8 |  10   |   TS Value (TSval)  |TS Echo Reply (TSecr)|
    +-------+-------+---------------------+---------------------+
        1       1              4                     4
{% endhighlight %}

在开启了 ```net.ipv4.tcp_tw_reuse``` 参数后，对于 **outgoing** 的链接，Linux 会复用处于 ```TIME_WAIT``` 状态的资源，当然时间戳要大于前一个链接的最近时间戳，这也就意味着一般一秒之后就可以重新使用该链接。

对于上面说的第一种异常场景，也就是延迟报文被新链接接收，这一场景通过 timestamp 就可以判断上个链接的报文已经过期，从而会直接丢弃。

对于第二种场景，也即防止远端处于 ```LAST_ACK``` 状态，此时当复用处于 ```TIME_WAIT``` 状态的链接时，第一个发送的 SYN 报文，由于对方校验时间戳不通过，会直接丢弃，并重发一个 ```FIN``` 报文，由于此时处于 ```SYN_SENT``` 状态，则会发送一个 RST 报文关闭对端的链接。

接下来，就可以正常创建新链接，只是时间会略有增加。

![tcpip timewait last ack reuse]({{ site.url }}/images/network/tcpip_timewait_last_ack_reuse.png "tcpip timewait last ack reuse"){: .pull-center}

### tcp_tw_recycle

同样需要开启 timestamp 参数，不过会影响到 **incoming 和 outcoming** 。

正常来说，处于 ```TIME_WAIT``` 状态的超时时间为 ```TCP_TIMEWAIT_LEN``` ，也即 60s；当开启了该参数后，该 socket 的释放时间与 RTO (通过RTT计算) 相关。

关于当前链接的 RTO 可以通过 ```ss --info  sport = :2112 dport = :4057``` 命令查看。

这一过程的处理可以查看源码中的 ```tcp_time_wait()``` 函数，内容摘取如下。

{% highlight c %}
if (tcp_death_row.sysctl_tw_recycle && tp->rx_opt.ts_recent_stamp)
    recycle_ok = icsk->icsk_af_ops->remember_stamp(sk);
......

if (timeo < rto)
    timeo = rto;

if (recycle_ok) {
    tw->tw_timeout = rto;
} else {
    tw->tw_timeout = TCP_TIMEWAIT_LEN;
    if (state == TCP_TIME_WAIT)
        timeo = TCP_TIMEWAIT_LEN;
}

inet_twsk_schedule(tw, &tcp_death_row, timeo,
           TCP_TIMEWAIT_LEN);
{% endhighlight %}


在内核源码可以发现 ```tcp_tw_recycle``` 和 ```tcp_timestamps``` 都开启的条件下，如果 60s 内同一源 IP 主机的 socket connect 请求中的 timestamp 必须是递增的。

可以从 TCP 的三次握手时的 SYN 包的处理函数中，也就是 tcp_conn_request() 函数中。

{% highlight c %}
bool tcp_peer_is_proven(struct request_sock *req, struct dst_entry *dst,
            bool paws_check, bool timestamps)
{
    struct tcp_metrics_block *tm;
    bool ret;

    if (!dst)
        return false;

    rcu_read_lock();
    tm = __tcp_get_metrics_req(req, dst);
    if (paws_check) {
        if (tm &&
            // 判断表示该源ip的上次tcp通讯发生在60s内
            (u32)get_seconds() - tm->tcpm_ts_stamp < TCP_PAWS_MSL &&
            // 该条件判断表示该源ip的上次tcp通讯的timestamp 大于本次tcp
            ((s32)(tm->tcpm_ts - req->ts_recent) > TCP_PAWS_WINDOW ||
             !timestamps))
            ret = false;
        else
            ret = true;
    } else {
        if (tm && tcp_metric_get(tm, TCP_METRIC_RTT) && tm->tcpm_ts_stamp)
            ret = true;
        else
            ret = false;
    }
    rcu_read_unlock();

    return ret;
}

int tcp_conn_request(struct request_sock_ops *rsk_ops,
             const struct tcp_request_sock_ops *af_ops,
             struct sock *sk, struct sk_buff *skb)
{
    ... ...
        if (tcp_death_row.sysctl_tw_recycle) { // 本机系统开启tcp_tw_recycle选项
            bool strict;

            dst = af_ops->route_req(sk, &fl, req, &strict);

            if (dst && strict &&
                !tcp_peer_is_proven(req, dst, true,
                        tmp_opt.saw_tstamp)) { // 该socket支持tcp_timestamp
                NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_PAWSPASSIVEREJECTED);
                goto drop_and_release;
            }
        }
    ... ...
}
{% endhighlight %}

<!--
在 TCP 的三次握手时的 SYN 包的处理函数中，也就是 tcp_v4_conn_request() 函数中。
       if (tmp_opt.saw_tstamp &&
            tcp_death_row.sysctl_tw_recycle &&
            (dst = inet_csk_route_req(sk, req)) != NULL &&
            (peer = rt_get_peer((struct rtable *)dst)) != NULL &&
            peer->v4daddr == saddr) {
            if (get_seconds() < peer->tcp_ts_stamp + TCP_PAWS_MSL &&
                (s32)(peer->tcp_ts - req->ts_recent) >
                            TCP_PAWS_WINDOW) {
                NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_PAWSPASSIVEREJECTED);
                goto drop_and_release;
            }
        }
-->

上述种的状态统计可以查看 ```/proc/net/netstat``` 文件中的 ```PAWSPassive``` 。

#### NAT

<!--
PAWS (Protect Against Wrapped Sequence numbers) 是一种简单的防止重复报文的机制，TCP 会缓存每个主机 (IP) 发送最新的 timestamp 值，用于丢弃当前链接中可能旧的重复报文，在 Linux 就是通过同时启用 tcp_timestamps 和 tcp_tw_recycle 两个选项。

这种机制在 C/S 一对一的时候没有任何问题，但当使用负载均衡后，由于不会修改 timestamp 值，而互联网上的机器很难保证时间的一致性，而负载均衡通过某个端口向内部的某台服务器发起连接，源地址为负载均衡的内部地址同一 IP 。

假如恰巧先后两次连接源端口相同，这台服务器先后收到两个包，第一个包的 timestamp 被服务器保存着，第二个包又来了，一对比，发现第二个包的 timestamp 比第一个还老，从而判断第二个包是重复报文，丢弃。

反映出来的情况就是在服务器上抓包，发现有 SYN 包，但服务器就是不回 ACK 包，因为 SYN 包已经被丢弃了。可以通过 netstat -s 里面 passive connections rejected by timestamp 一项的数字变化。
-->


不过上述的修改，在 NAT 场景下可能会引起更加复杂的问题，在 [rfc1323](https://tools.ietf.org/html/rfc1323) 中，有如下描述。

{% highlight text %}
An additional mechanism could be added to the TCP, a per-host
cache of the last timestamp received from any connection.
This value could then be used in the PAWS mechanism to reject
old duplicate segments from earlier incarnations of the
connection, if the timestamp clock can be guaranteed to have
ticked at least once since the old connection was open.  This
would require that the TIME-WAIT delay plus the RTT together
must be at least one tick of the sender's timestamp clock.
Such an extension is not part of the proposal of this RFC.
{% endhighlight %}

大概意思是说 TCP 可以缓存从每个主机收到报文的最新时间戳，后续请求的时间戳如果小于缓存时间戳，则认为该报文无效，数据包会被丢弃。

如上所述，这一行为在 Linux 中在 ```tcp_timestamps``` 和 ```tcp_tw_recycle``` 都开启时使用，而前者默认是开启的，所以当 ```tcp_tw_recycle``` 被开启后，实际上这种行为就被激活了，那么当客户端或服务端以 NAT 方式构建的时候就可能出现问题。




## 问题排查

针对上述讨论的部分异常，简单讨论下常见的场景。

### EADDRNOTAVAIL

在客户端尝试建立链接时，如果报 ```EADDRNOTAVAIL (Cannot assign requested address)``` 错误，而且查看可能是已经达到了最大的端口可用数量。

{% highlight text %}
----- 查看内核端口可用范围
$ sysctl net.ipv4.ip_local_port_range
net.ipv4.ip_local_port_range = 32768 60999
----- 上述范围是闭区间，实际可用的端口数量是
$ echo $((60999-32768+1))
28232
{% endhighlight %}

针对这一故障场景，可以通过如下步骤修复：

{% highlight text %}
----- 增加本地可用端口数量，这里是临时修改
$ sysctl net.ipv4.ip_local_port_range="10240 61000"
----- 减少TIME_WAIT连接状态
$ sysctl net.ipv4.tcp_tw_reuse=1
{% endhighlight %}

### NAT

当进行 SNAT 转换时，也即是在服务端看到的都是同一个 IP，那么对于服务端而言这些客户端实际上等同于一个，而客户端的时间戳会存在差异，那么服务端就会出现时间戳错乱的现象，进而直接丢弃时间戳小的数据包。

这类问题的现象是，客户端明发送的 SYN 报文，但服务端没有响应 ACK，可以通过下面命令来确认数据包不断被丢弃的现象：

{% highlight text %}
$ netstat -s | grep timestamp
... packets rejects in established connections because of timestamp
{% endhighlight %}

所以，如果无法确定没有使用 NAT ，那么为了安全起见，需要禁止 tcp_tw_recycle 。

### tcp_max_tw_buckets

该参数用于设置 ```TIME_WAIT``` 状态的 socket 数，超过该限制后就会删除掉，此时系统日志里会显示： ```TCP: time wait bucket table overflow``` 。

{% highlight text %}
# sysctl net.ipv4.tcp_max_tw_buckets=100000
{% endhighlight %}

如果是 NAT 网络环境又存在大量访问，会产生各种连接不稳定断开的情况。

## 总结

处于 ```TIME_WAIT``` 状态的链接，最大的消耗不是内存或者 CPU，而是四元组对应的资源，所以最有效的方式是增加端口范围、增加地址等。

在 **服务端**，除非确定没有使用 NAT ，否则不要配置 ```tcp_tw_recycle``` 参数；另外，```tcp_tw_reuse``` 参数对于 incoming 的报文无效。

在 **客户端**，可以直接开启 ```tcp_tw_reuse``` 参数，而 ```tcp_tw_recycle``` 参数的作用不是很大。

另外，在设计协议时，尽量不要让客户端先关闭链接，最好让服务端去处理这种场景。


<!--


/proc/net/sockstat


　　说明：
　　net.ipv4.tcp_syncookies = 1 表示开启SYN Cookies。当出现SYN等待队列溢出时，启用cookies来处理，可防范少量SYN攻击，默认为0，表示关闭；
　　net.ipv4.tcp_tw_reuse = 1 表示开启重用。允许将TIME-WAIT sockets重新用于新的TCP连接，默认为0，表示关闭；
　　net.ipv4.tcp_tw_recycle = 1 表示开启TCP连接中TIME-WAIT sockets的快速回收，默认为0，表示关闭。

　　再执行以下命令，让修改结果立即生效：

#### net.ipv4.tcp_tw_recycle

该参数在内核文档中有如下内容，主要是为了可以快速回收处于 TIME-WAIT 状态的链接。

Enable fast recycling TIME-WAIT sockets. Default value is 0. It should not be changed without advice/request of technical experts.

之所以不建议修改，主要是针对 NAT 的场景。

另外，需要注意的是，该参数虽然针对的是 ipv4 ，实际上也会影响到 ipv6 ，而且 ```net.netfilter.nf_conntrack_tcp_timeout_time_wait``` 参数不会影响到 tcp/ip 协议栈。

https://vincent.bernat.im/en/blog/2014-tcp-time-wait-state-linux
https://www.isi.edu/touch/pubs/infocomm99/infocomm99-web/
https://huoding.com/2013/12/31/316
http://zhangziqiang.blog.51cto.com/698396/500204
http://www.cnblogs.com/mydomain/archive/2011/08/23/2150567.html
http://blog.csdn.net/yusiguyuan/article/details/12612395


TCP 应当是以太网协定族中被运用最为遍及的协定之一，在此主要介绍下 TCP 协定中的 TimeStamp 选项。该选项是由 RFC 1323 协议引入，在 1992 年提交。

在此简单说明下为什么有这个选项。


# 简介

首先说明下 TCP 协议的几个设计初衷，以及引发的问题：

* 接收端不需要响应每一个收到的数据报文，只需要收到 N 个报文后，向发端回复一个 ACK 报文即可，之所以这样，主要是为了提高通讯效率。

    不过同时也引入了几个问题： A) 报文发送之后，不能确定多久可以收到 ACK 报文；B) 如果 ACK 报文丢失，很难确定需要重发的超时时间。

* TCP 报文中，seq 长度为 32-bits，也就限制了发端最多一次发送 2^30 长度的数据，就必须等待 ACK 信号，对于超高速以太网 (>1G)，这样会影响 TCP 连接的转发效率。

为了解决上面提到的问题，通过 TimeStamp 选项主要有两个用途:

* 测量 TCP 连接两端通讯的延迟 (Round Trip Time Measurement)

    通过 RTTM 机制可以很容易的判断出线路上报文的延迟情况，从而制定出一个优化的发包间隔和报文超时时间，从而解决了第一个问题。

* 处理 seq 反转的问题 (Protect Against Wrapped Sequence Numbers)

    TCP 接收到一个数据报文后，会先比较本次收到报文的 TimeStamp 和上次收到报文的 TimeStamp；如果本次的比较新，那么可以直接认为本次收到的报文是新的报文，不需要进行复杂的 Sequence Number Window Scale 计算，从而解决了第二个问题。


## 缺陷

虽然 RFC-1323 可以解决上述的问题，然而还存在一些隐患。

建议中定义TimeStamp增加的间隔可以使1ms-1s。如果设备按照1ms的速度增加TimeStamp，那么只要一个TCP连接连续24.8天（1ms*2^31）没有通讯，再发送报文，收端比较本次报文和上次报文TimeStamp的动作就会出错。（问题1）
（注：TCP协议中并没有定义KeepAlive。如果应用层代码不定义超时机制，TCP连接就永远不会中断，所以连续24.8天不通讯的情况是却有可能发生的。）
引用Linux相关代码：((s32)(tp->rx_opt.rcv_tsval - tp->rx_opt.ts_recent) < 0)
比如 tp->rx_opt.rcv_tsval = 0x80000020，       tp->rx_opt.ts_recent = 0x10
 ((s32)(tp->rx_opt.rcv_tsval - tp->rx_opt.ts_recent) = (s32)0x80000010，是一个负数，必然小于0。

如果解决问题1呢？
已知按照RFC1323的规定，按照最快TimeStamp增加的速度，也需要24.8天TImeStamp才有可能发生反转。
如果((s32)(tp->rx_opt.rcv_tsval - tp->rx_opt.ts_recent) < 0)判断成立，还可以再用本地收到报文的本地TimeStamp减去上一次收到报文的本地TimeStamp。如果时间大于24.8天，那么就是TimeStamp发生了反转；否则就不是反转的情况。这样做是不是就万无一失了呢？不一定！

别忘了本地TimeStamp的计数器也是个32位，也可能会翻转的。（问题2）
举个极端的例子：假设TCP两端设备的TimeStamp增加间隔不一致，A为1ms，B为10ms。TCP连接连续248天没有通讯；这个时候B向A发送了一个数据报文。
此时B发送给A的TCP报文中的TimeStamp，正好发生了翻转。然而由于A的计数器是每1ms加一的，248天时间，A的计数器已经归零过5次了。这时候再用本地TimeStamp做判断还是错的。







- tcp_tw_reuse 和SO_REUSEADDR 是两个完全不同的东西

1. tw_reuse，tw_recycle 必须在客户端和服务端timestamps 开启时才管用（默认打开）

2. tw_reuse 只对客户端起作用，开启后客户端在1s内回收

3. tw_recycle 对客户端和服务器同时起作用，开启后在 3.5*RTO 内回收，RTO 200ms~ 120s 具体时间视网络状况。

　　内网状况比tw_reuse 稍快，公网尤其移动网络大多要比tw_reuse 慢，优点就是能够回收服务端的```TIME_WAIT```数量


对于客户端

1. 作为客户端因为有端口65535问题，TIME_OUT过多直接影响处理能力，打开tw_reuse 即可解决，不建议同时打开tw_recycle，帮助不大。kk

2. tw_reuse 帮助客户端1s完成连接回收，基本可实现单机6w/s请求，需要再高就增加IP数量吧。

3. 如果内网压测场景，且客户端不需要接收连接，同时tw_recycle 会有一点点好处。

4. 业务上也可以设计由服务端主动关闭连接






tw_socket_TCP 占用70M, 因为业务简单服务占用CPU 200% 运行很稳定。
复制代码

slabtop

262230 251461  95%    0.25K  17482       15     69928K tw_sock_TCP

 ss -s
Total: 259 (kernel 494)
TCP:   262419 (estab 113, closed 262143, orphaned 156, synrecv 0, timewait 262143/0), ports 80

Transport Total     IP        IPv6
*         494       -         -
RAW       1         1         0
UDP       0         0         0
TCP       276       276       0
INET      277       277       0
FRAG      0         0         0

-->


## 参考

一篇很不错的文章，可以参考 [Coping with the TCP TIME-WAIT state on busy Linux servers](https://vincent.bernat.im/en/blog/2014-tcp-time-wait-state-linux) 。


{% highlight text %}
{% endhighlight %}
