---
title: TCP/IP 協議之 TIME_WAIT
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: 網絡,linux,network,time wait
description: TIME_WAIT 是 TCP 協議棧中比較特殊的狀態，其主要目的是保證不同的鏈接不會相互幹擾，但是對於一些高性能的場景，就可能由於較多的 TIME_WAIT 狀態最終導致鏈接不可用。如下簡單介紹如何充分利用該狀態。
---

```TIME_WAIT``` 是 TCP 協議棧中比較特殊的狀態，其主要目的是保證不同的鏈接不會相互幹擾，但是對於一些高性能的場景，就可能由於較多的 ```TIME_WAIT``` 狀態最終導致鏈接不可用。

如下簡單介紹如何充分利用該狀態。

<!-- more -->

## 簡介

如下是 TCP 的狀態轉換圖，主動關閉鏈接的一端會進入 ```TIME_WAIT``` 狀態。

![TCP/IP Finite State Machine FSM]({{ site.url }}/images/network/tcpip_The-TCP-Finite-State-MachineFSM.png "TCP/IP Finite State Machine FSM"){: .pull-center}

可以通過如下命令統計當前不同狀態的鏈接數。

{% highlight text %}
----- 建議使用後者，要快很多
# netstat -ant | awk '/^tcp/ {++state[$NF]} END {for(key in state) print key,state[key]}' | sort -rnk2
# ss -ant | awk '!/^State/ {++state[$1]} END {for(key in state) print key,state[key]}' | sort -rnk2
{% endhighlight %}

<!--
netstat -atunp > /tmp/netstat.log
cat /tmp/netstat.log | grep 'TIME_WAIT' | awk '{ print $4 }' | sort | uniq -c | sort -nr | head -5
cat /tmp/netstat.log | grep '192.30.16.133:10051' | awk '{ print $5 }' | cut -d: -f1 | sort -u | wc -l
-->

如上，```TIME_WAIT``` 是在發起主動關閉一方，完成四次揮手後 TCP 狀態轉換為 ```TIME_WAIT``` 狀態，並且該狀態會保持兩個 MSL 。

在 Linux 裡一個 MSL 為 30s，不可配置，在 ```include/net/tcp.h``` 中定義如下。

{% highlight c %}
#define TCP_TIMEWAIT_LEN (60*HZ) /* how long to wait to destroy TIME-WAIT
                                  * state, about 60 seconds */
{% endhighlight %}

### 原因

之所以採用兩個 MSL 主要是為了可靠安全的關閉 TCP 連接。

#### 報文延遲

最常見的是為了防止前一個鏈接的延遲報文被下一個鏈接接收，當然這兩個鏈接的四元組 (source address, source port, destination address, destination port) 是相同的，而且序列號 sequence 也是在特定範圍內的。

雖然滿足上述場景的概率很小，但是對於 fast connections 以及滑動窗口很大時，會增加出現這種場景的概率。關於 ```TIME_WAIT``` 詳細討論可以參考 [rfc1337](https://tools.ietf.org/html/rfc1337) 。

<!-- TODO: net.ipv4.tcp_rfc1337 -->

下圖是一個延遲延遲報文，在原鏈接已經正常關閉，並使用相同的端口建立了新鏈接，那麼上個鏈接發送的報文可能混入新的鏈接中。

![timewait duplicate segment]({{ site.url }}/images/network/tcpip_timewait_duplicate_segment.png "timewait duplicate segment"){: .pull-center }

<!-- 主動關閉最後一個 FIN 發送延遲，那麼新創建而且複用的 fd 開始就會收到 FIN 報文，直接進入關閉流程；或者由於 SEQ 不對，可能直接收到 RST 報文，影響新鏈接的創建。 -->

#### 確保遠端以關閉

當發送的最後一個 ACK 丟失後，遠端處於 LAST_ACK 狀態，在此狀態時，如果沒有收到最後的 ACK 報文，那麼就會重發 FIN 。

如下圖，當 FIN 丟失，那麼被動關閉方會處於 LAST_ACK 狀態，那麼嘗試重新建立鏈接時，會直接發送一個 RST 關閉鏈接，影響新鏈接創建。

![timewait last ack]({{ site.url }}/images/network/tcpip_timewait_last_ack.png "timewait last ack"){: .pull-center }

注意，處於 ```LAST_ACK``` 狀態的鏈接，如果沒有收到最後一個 ACK 報文，那麼就會一致重發 FIN 報文，直到如下條件：

1. 由於超時自動關閉鏈接；
2. 收到了 ACK 報文然後關閉鏈接；
3. 收到了 RST 報文並關閉鏈接。

簡言之，通過 2MSL 等待時間，保證前一個鏈接的報文已經在網絡上消失，保證雙方都已經正常關閉鏈接。


## 產生影響

如果有較多的鏈接處於 ```TIME_WAIT``` 狀態 (可以通過 ```ss -tan state time-wait | wc -l``` 查看)，那麼一般會有如下幾個方面的影響：

1. 佔用文件描述符 (fd) ，會導致無法創建相同類型的鏈接；
2. 內核中 socket 結構體佔用的內存資源；
3. 額外的 CPU 消耗。

接下來，一個一個看看其影響。

### 文件描述符

一個鏈接在 ```TIME_WAIT``` 保存一分鐘，那麼相同四元組 (saddr, sport, daddr, dport) 的鏈接就無法創建；實際上，如果從內核角度看，實際上根據配置項，還可能包含了 sequence 以及 timestamp 。

如果服務器是掛載在一個 L7 Load-Balancer 之後的，那麼源地址是相同的，而 Linux 中的 Port 分配範圍是通過 ```net.ipv4.ip_local_port_range``` 參數配置的，默認是 3W 左右的可用端口，那麼平均是每秒 500 個鏈接。

在客戶端，那麼就會報 ```EADDRNOTAVAIL (Cannot assign requested address)``` 錯誤，而服務端的排查相比來說要複雜的多，如下可以查看當前與客戶端的鏈接數。

{% highlight text %}
----- sport就是Local Address列對應的端口
# ss -tan 'sport = :80' | awk '/^TIME-WAIT/ {print $(NF)" "$(NF-1)}' | \
    sed 's/:[^ ]*//g' | sort | uniq -c | sort -rn
{% endhighlight %}

針對這一場景，那麼就是如何增加四元組，按照難度依次列舉如下：

1. 客戶端添加更多的端口範圍，設置 ```net.ipv4.ip_local_port_range```；
2. 服務端增加監聽端口，例如額外增加 81、82、83 ...；

<!--
3.
use more client IP by configuring additional IP on the load balancer and use them in a round-robin fashion6,
use more server IP by configuring additional IP on the web server7.
-->

注意，在客戶端，不同版本的內核其行為也略有區別，老版本內核會查找空閒的本地二元組 (source address, source port)，此時增加服務端的 IP 以及 port 時不會增大鏈接數；而 Linux 3.2 之後，對於不同的四元組那麼可以複用本地的二元組。

<!-- http://marc.info/?l=haproxy&m=139315382127339&w=2 -->

<!-- TODO: To avoid EADDRINUSE errors, the load balancer needs to use the SO_REUSEADDR option before calling bind(), then connect(). -->

最後就是調整 ```net.ipv4.tcp_tw_reuse``` 和 ```net.ipv4.tcp_tw_recycle``` 參數，不過在一些場景下可能會有問題，所以儘量不要使用，下面再詳細介紹。

### 內存

假設每秒要處理 1W 的鏈接，那麼在 Linux 中處於 ```TIME_WAIT``` 狀態的鏈接數就有 60W ，這樣的話是否會消耗很多的內存資源？

首先，應用端的 ```TIME_WAIT``` 狀態鏈接已經關閉，所以不會消耗資源；主要資源的消耗在內核中。內核中保存了三個不同的結構體：

#### HASH TABLE

```connection hash table``` 用於快速定位一個現有的鏈接，例如接收到一個報文快速定位到鏈接結構體。

{% highlight text %}
$ dmesg | grep "TCP established hash table"
[    0.292951] TCP established hash table entries: 65536 (order: 7, 524288 bytes)
{% endhighlight %}

可以在內核啟動時通過 ```thash_entries``` 參數設置，系統啟動時會調用 ```alloc_large_system_hash()``` 函數初始化，並打印上述的啟動日誌信息。

在內核中，處於 ```TIME_WAIT``` 狀態的鏈接使用的是 ```struct tcp_timewait_sock``` 結構體，而其它狀態則使用的是 ```struct tcp_sock``` 。

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

用於判斷處於 ```TIME_WAIT``` 狀態的 socket 還有多長時間過期，與上述的 hash 表使用相同的結構體，也就是對應了 ```struct inet_timewait_sock``` 結構體中的 ```struct hlist_node tw_death_node``` 。

#### HASH TABLE

```bind hash table``` 用於保存本地已經保存的端口以及相關參數，用於綁定 listen 端口以及查找可用端口。

{% highlight text %}
$ dmesg | grep "TCP bind hash table"
[    0.293146] TCP bind hash table entries: 65536 (order: 8, 1048576 bytes)
{% endhighlight %}

這裡每個元素使用 ```struct inet_bind_socket``` 結構體，大小與 connection 相同。

重點關注兩個 hash 表使用的內存即可。

{% highlight text %}
# slabtop -o | grep -E '(^  OBJS|tw_sock_TCP|tcp_bind_bucket)'
  OBJS ACTIVE  USE OBJ SIZE  SLABS OBJ/SLAB CACHE SIZE NAME                   
 50955  49725  97%    0.25K   3397       15     13588K tw_sock_TCP            
 44840  36556  81%    0.06K    760       59      3040K tcp_bind_bucket
{% endhighlight %}

## 內核參數調優

相關參數可以查看內核文檔 [ip-sysctl.txt]({{ site.kernel_docs_url }}/Documentation/networking/ip-sysctl.txt) ，僅摘取與此相關的參數介紹：

{% highlight text %}
tcp_tw_reuse - BOOLEAN
    Allow to reuse TIME-WAIT sockets for new connections when it is
    safe from protocol viewpoint. Default value is 0.
    It should not be changed without advice/request of technical experts.

tcp_tw_recycle - BOOLEAN
    Enable fast recycling TIME-WAIT sockets. Default value is 0.
    It should not be changed without advice/request of technical experts.
{% endhighlight %}

注意，在使用上述參數時，需要先開啟 tcp_timestamps 參數，否則這裡的配置時無效的，也就是需要開啟如下參數。

{% highlight text %}
----- 查看當前狀態
# sysctl net.ipv4.tcp_tw_reuse
# sysctl net.ipv4.tcp_tw_recycle
# sysctl net.ipv4.tcp_timestamps

----- 將對應的參數開啟
# sysctl net.ipv4.tcp_tw_reuse=1
# sysctl net.ipv4.tcp_tw_recycle=1
# sysctl net.ipv4.tcp_timestamps=1

----- 寫入到配置文件，持久化，並使配置立即生效
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

net.ipv4.tcp_syncookies = 1 表示開啟SYN Cookies。當出現SYN等待隊列溢出時，啟用cookies來處理，可防範少量SYN攻擊，默認為0，表示關閉；
net.ipv4.tcp_tw_reuse = 1 表示開啟重用。允許將TIME-WAIT sockets重新用於新的TCP連接，默認為0，表示關閉；
net.ipv4.tcp_tw_recycle = 1 表示開啟TCP連接中TIME-WAIT sockets的快速回收，默認為0，表示關閉。
net.ipv4.tcp_fin_timeout = 30 表示如果套接字由本端要求關閉，這個參數決定了它保持在FIN-WAIT-2狀態的時間。
net.ipv4.tcp_keepalive_time = 1200 表示當keepalive起用的時候，TCP發送keepalive消息的頻度。缺省是2小時，改為20分鐘。
net.ipv4.tcp_max_syn_backlog = 8192 表示SYN隊列的長度，默認為1024，加大隊列長度為8192，可以容納更多等待連接的網絡連接數。
net.ipv4.tcp_max_tw_buckets = 5000 表示系統同時保持TIME_WAIT套接字的最大數量，如果超過這個數字，TIME_WAIT套接字將立刻被清除並打印警告信息。
默  認為180000，改為5000。對於Apache、Nginx等服務器，上幾行的參數可以很好地減少TIME_WAIT套接字數量，但是對於Squid，效果卻不大。此項參數可以控制TIME_WAIT套接字的最大數量，避免Squid服務器被大量的TIME_WAIT套接字拖死。
-->



### tcp_tw_reuse

如上所述，```TIME_WAIT``` 狀態主要是為了防止延遲報文影響新鏈接，不過在有些場景下可以確保不會出現這種情況。

[RFC 1323](https://tools.ietf.org/html/rfc1323) 提供了一個 TCP 擴展項，其中定義了兩個四字節的時間戳選項，第一個是發送時的發送端時間戳，第二個是從遠端接收報文的最近時間戳。

{% highlight text %}
TCP Timestamps Option (TSopt):
    +-------+-------+---------------------+---------------------+
    |Kind=8 |  10   |   TS Value (TSval)  |TS Echo Reply (TSecr)|
    +-------+-------+---------------------+---------------------+
        1       1              4                     4
{% endhighlight %}

在開啟了 ```net.ipv4.tcp_tw_reuse``` 參數後，對於 **outgoing** 的鏈接，Linux 會複用處於 ```TIME_WAIT``` 狀態的資源，當然時間戳要大於前一個鏈接的最近時間戳，這也就意味著一般一秒之後就可以重新使用該鏈接。

對於上面說的第一種異常場景，也就是延遲報文被新鏈接接收，這一場景通過 timestamp 就可以判斷上個鏈接的報文已經過期，從而會直接丟棄。

對於第二種場景，也即防止遠端處於 ```LAST_ACK``` 狀態，此時當複用處於 ```TIME_WAIT``` 狀態的鏈接時，第一個發送的 SYN 報文，由於對方校驗時間戳不通過，會直接丟棄，並重發一個 ```FIN``` 報文，由於此時處於 ```SYN_SENT``` 狀態，則會發送一個 RST 報文關閉對端的鏈接。

接下來，就可以正常創建新鏈接，只是時間會略有增加。

![tcpip timewait last ack reuse]({{ site.url }}/images/network/tcpip_timewait_last_ack_reuse.png "tcpip timewait last ack reuse"){: .pull-center}

### tcp_tw_recycle

同樣需要開啟 timestamp 參數，不過會影響到 **incoming 和 outcoming** 。

正常來說，處於 ```TIME_WAIT``` 狀態的超時時間為 ```TCP_TIMEWAIT_LEN``` ，也即 60s；當開啟了該參數後，該 socket 的釋放時間與 RTO (通過RTT計算) 相關。

關於當前鏈接的 RTO 可以通過 ```ss --info  sport = :2112 dport = :4057``` 命令查看。

這一過程的處理可以查看源碼中的 ```tcp_time_wait()``` 函數，內容摘取如下。

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


在內核源碼可以發現 ```tcp_tw_recycle``` 和 ```tcp_timestamps``` 都開啟的條件下，如果 60s 內同一源 IP 主機的 socket connect 請求中的 timestamp 必須是遞增的。

可以從 TCP 的三次握手時的 SYN 包的處理函數中，也就是 tcp_conn_request() 函數中。

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
            // 判斷表示該源ip的上次tcp通訊發生在60s內
            (u32)get_seconds() - tm->tcpm_ts_stamp < TCP_PAWS_MSL &&
            // 該條件判斷表示該源ip的上次tcp通訊的timestamp 大於本次tcp
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
        if (tcp_death_row.sysctl_tw_recycle) { // 本機系統開啟tcp_tw_recycle選項
            bool strict;

            dst = af_ops->route_req(sk, &fl, req, &strict);

            if (dst && strict &&
                !tcp_peer_is_proven(req, dst, true,
                        tmp_opt.saw_tstamp)) { // 該socket支持tcp_timestamp
                NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_PAWSPASSIVEREJECTED);
                goto drop_and_release;
            }
        }
    ... ...
}
{% endhighlight %}

<!--
在 TCP 的三次握手時的 SYN 包的處理函數中，也就是 tcp_v4_conn_request() 函數中。
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

上述種的狀態統計可以查看 ```/proc/net/netstat``` 文件中的 ```PAWSPassive``` 。

#### NAT

<!--
PAWS (Protect Against Wrapped Sequence numbers) 是一種簡單的防止重複報文的機制，TCP 會緩存每個主機 (IP) 發送最新的 timestamp 值，用於丟棄當前鏈接中可能舊的重複報文，在 Linux 就是通過同時啟用 tcp_timestamps 和 tcp_tw_recycle 兩個選項。

這種機制在 C/S 一對一的時候沒有任何問題，但當使用負載均衡後，由於不會修改 timestamp 值，而互聯網上的機器很難保證時間的一致性，而負載均衡通過某個端口向內部的某臺服務器發起連接，源地址為負載均衡的內部地址同一 IP 。

假如恰巧先後兩次連接源端口相同，這臺服務器先後收到兩個包，第一個包的 timestamp 被服務器保存著，第二個包又來了，一對比，發現第二個包的 timestamp 比第一個還老，從而判斷第二個包是重複報文，丟棄。

反映出來的情況就是在服務器上抓包，發現有 SYN 包，但服務器就是不回 ACK 包，因為 SYN 包已經被丟棄了。可以通過 netstat -s 裡面 passive connections rejected by timestamp 一項的數字變化。
-->


不過上述的修改，在 NAT 場景下可能會引起更加複雜的問題，在 [rfc1323](https://tools.ietf.org/html/rfc1323) 中，有如下描述。

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

大概意思是說 TCP 可以緩存從每個主機收到報文的最新時間戳，後續請求的時間戳如果小於緩存時間戳，則認為該報文無效，數據包會被丟棄。

如上所述，這一行為在 Linux 中在 ```tcp_timestamps``` 和 ```tcp_tw_recycle``` 都開啟時使用，而前者默認是開啟的，所以當 ```tcp_tw_recycle``` 被開啟後，實際上這種行為就被激活了，那麼當客戶端或服務端以 NAT 方式構建的時候就可能出現問題。




## 問題排查

針對上述討論的部分異常，簡單討論下常見的場景。

### EADDRNOTAVAIL

在客戶端嘗試建立鏈接時，如果報 ```EADDRNOTAVAIL (Cannot assign requested address)``` 錯誤，而且查看可能是已經達到了最大的端口可用數量。

{% highlight text %}
----- 查看內核端口可用範圍
$ sysctl net.ipv4.ip_local_port_range
net.ipv4.ip_local_port_range = 32768 60999
----- 上述範圍是閉區間，實際可用的端口數量是
$ echo $((60999-32768+1))
28232
{% endhighlight %}

針對這一故障場景，可以通過如下步驟修復：

{% highlight text %}
----- 增加本地可用端口數量，這裡是臨時修改
$ sysctl net.ipv4.ip_local_port_range="10240 61000"
----- 減少TIME_WAIT連接狀態
$ sysctl net.ipv4.tcp_tw_reuse=1
{% endhighlight %}

### NAT

當進行 SNAT 轉換時，也即是在服務端看到的都是同一個 IP，那麼對於服務端而言這些客戶端實際上等同於一個，而客戶端的時間戳會存在差異，那麼服務端就會出現時間戳錯亂的現象，進而直接丟棄時間戳小的數據包。

這類問題的現象是，客戶端明發送的 SYN 報文，但服務端沒有響應 ACK，可以通過下面命令來確認數據包不斷被丟棄的現象：

{% highlight text %}
$ netstat -s | grep timestamp
... packets rejects in established connections because of timestamp
{% endhighlight %}

所以，如果無法確定沒有使用 NAT ，那麼為了安全起見，需要禁止 tcp_tw_recycle 。

### tcp_max_tw_buckets

該參數用於設置 ```TIME_WAIT``` 狀態的 socket 數，超過該限制後就會刪除掉，此時系統日誌裡會顯示： ```TCP: time wait bucket table overflow``` 。

{% highlight text %}
# sysctl net.ipv4.tcp_max_tw_buckets=100000
{% endhighlight %}

如果是 NAT 網絡環境又存在大量訪問，會產生各種連接不穩定斷開的情況。

## 總結

處於 ```TIME_WAIT``` 狀態的鏈接，最大的消耗不是內存或者 CPU，而是四元組對應的資源，所以最有效的方式是增加端口範圍、增加地址等。

在 **服務端**，除非確定沒有使用 NAT ，否則不要配置 ```tcp_tw_recycle``` 參數；另外，```tcp_tw_reuse``` 參數對於 incoming 的報文無效。

在 **客戶端**，可以直接開啟 ```tcp_tw_reuse``` 參數，而 ```tcp_tw_recycle``` 參數的作用不是很大。

另外，在設計協議時，儘量不要讓客戶端先關閉鏈接，最好讓服務端去處理這種場景。


<!--


/proc/net/sockstat


　　說明：
　　net.ipv4.tcp_syncookies = 1 表示開啟SYN Cookies。當出現SYN等待隊列溢出時，啟用cookies來處理，可防範少量SYN攻擊，默認為0，表示關閉；
　　net.ipv4.tcp_tw_reuse = 1 表示開啟重用。允許將TIME-WAIT sockets重新用於新的TCP連接，默認為0，表示關閉；
　　net.ipv4.tcp_tw_recycle = 1 表示開啟TCP連接中TIME-WAIT sockets的快速回收，默認為0，表示關閉。

　　再執行以下命令，讓修改結果立即生效：

#### net.ipv4.tcp_tw_recycle

該參數在內核文檔中有如下內容，主要是為了可以快速回收處於 TIME-WAIT 狀態的鏈接。

Enable fast recycling TIME-WAIT sockets. Default value is 0. It should not be changed without advice/request of technical experts.

之所以不建議修改，主要是針對 NAT 的場景。

另外，需要注意的是，該參數雖然針對的是 ipv4 ，實際上也會影響到 ipv6 ，而且 ```net.netfilter.nf_conntrack_tcp_timeout_time_wait``` 參數不會影響到 tcp/ip 協議棧。

https://vincent.bernat.im/en/blog/2014-tcp-time-wait-state-linux
https://www.isi.edu/touch/pubs/infocomm99/infocomm99-web/
https://huoding.com/2013/12/31/316
http://zhangziqiang.blog.51cto.com/698396/500204
http://www.cnblogs.com/mydomain/archive/2011/08/23/2150567.html
http://blog.csdn.net/yusiguyuan/article/details/12612395


TCP 應當是以太網協定族中被運用最為遍及的協定之一，在此主要介紹下 TCP 協定中的 TimeStamp 選項。該選項是由 RFC 1323 協議引入，在 1992 年提交。

在此簡單說明下為什麼有這個選項。


# 簡介

首先說明下 TCP 協議的幾個設計初衷，以及引發的問題：

* 接收端不需要響應每一個收到的數據報文，只需要收到 N 個報文後，向發端回覆一個 ACK 報文即可，之所以這樣，主要是為了提高通訊效率。

    不過同時也引入了幾個問題： A) 報文發送之後，不能確定多久可以收到 ACK 報文；B) 如果 ACK 報文丟失，很難確定需要重發的超時時間。

* TCP 報文中，seq 長度為 32-bits，也就限制了發端最多一次發送 2^30 長度的數據，就必須等待 ACK 信號，對於超高速以太網 (>1G)，這樣會影響 TCP 連接的轉發效率。

為瞭解決上面提到的問題，通過 TimeStamp 選項主要有兩個用途:

* 測量 TCP 連接兩端通訊的延遲 (Round Trip Time Measurement)

    通過 RTTM 機制可以很容易的判斷出線路上報文的延遲情況，從而制定出一個優化的發包間隔和報文超時時間，從而解決了第一個問題。

* 處理 seq 反轉的問題 (Protect Against Wrapped Sequence Numbers)

    TCP 接收到一個數據報文後，會先比較本次收到報文的 TimeStamp 和上次收到報文的 TimeStamp；如果本次的比較新，那麼可以直接認為本次收到的報文是新的報文，不需要進行復雜的 Sequence Number Window Scale 計算，從而解決了第二個問題。


## 缺陷

雖然 RFC-1323 可以解決上述的問題，然而還存在一些隱患。

建議中定義TimeStamp增加的間隔可以使1ms-1s。如果設備按照1ms的速度增加TimeStamp，那麼只要一個TCP連接連續24.8天（1ms*2^31）沒有通訊，再發送報文，收端比較本次報文和上次報文TimeStamp的動作就會出錯。（問題1）
（注：TCP協議中並沒有定義KeepAlive。如果應用層代碼不定義超時機制，TCP連接就永遠不會中斷，所以連續24.8天不通訊的情況是卻有可能發生的。）
引用Linux相關代碼：((s32)(tp->rx_opt.rcv_tsval - tp->rx_opt.ts_recent) < 0)
比如 tp->rx_opt.rcv_tsval = 0x80000020，       tp->rx_opt.ts_recent = 0x10
 ((s32)(tp->rx_opt.rcv_tsval - tp->rx_opt.ts_recent) = (s32)0x80000010，是一個負數，必然小於0。

如果解決問題1呢？
已知按照RFC1323的規定，按照最快TimeStamp增加的速度，也需要24.8天TImeStamp才有可能發生反轉。
如果((s32)(tp->rx_opt.rcv_tsval - tp->rx_opt.ts_recent) < 0)判斷成立，還可以再用本地收到報文的本地TimeStamp減去上一次收到報文的本地TimeStamp。如果時間大於24.8天，那麼就是TimeStamp發生了反轉；否則就不是反轉的情況。這樣做是不是就萬無一失了呢？不一定！

別忘了本地TimeStamp的計數器也是個32位，也可能會翻轉的。（問題2）
舉個極端的例子：假設TCP兩端設備的TimeStamp增加間隔不一致，A為1ms，B為10ms。TCP連接連續248天沒有通訊；這個時候B向A發送了一個數據報文。
此時B發送給A的TCP報文中的TimeStamp，正好發生了翻轉。然而由於A的計數器是每1ms加一的，248天時間，A的計數器已經歸零過5次了。這時候再用本地TimeStamp做判斷還是錯的。







- tcp_tw_reuse 和SO_REUSEADDR 是兩個完全不同的東西

1. tw_reuse，tw_recycle 必須在客戶端和服務端timestamps 開啟時才管用（默認打開）

2. tw_reuse 只對客戶端起作用，開啟後客戶端在1s內回收

3. tw_recycle 對客戶端和服務器同時起作用，開啟後在 3.5*RTO 內回收，RTO 200ms~ 120s 具體時間視網絡狀況。

　　內網狀況比tw_reuse 稍快，公網尤其移動網絡大多要比tw_reuse 慢，優點就是能夠回收服務端的```TIME_WAIT```數量


對於客戶端

1. 作為客戶端因為有端口65535問題，TIME_OUT過多直接影響處理能力，打開tw_reuse 即可解決，不建議同時打開tw_recycle，幫助不大。kk

2. tw_reuse 幫助客戶端1s完成連接回收，基本可實現單機6w/s請求，需要再高就增加IP數量吧。

3. 如果內網壓測場景，且客戶端不需要接收連接，同時tw_recycle 會有一點點好處。

4. 業務上也可以設計由服務端主動關閉連接






tw_socket_TCP 佔用70M, 因為業務簡單服務佔用CPU 200% 運行很穩定。
複製代碼

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


## 參考

一篇很不錯的文章，可以參考 [Coping with the TCP TIME-WAIT state on busy Linux servers](https://vincent.bernat.im/en/blog/2014-tcp-time-wait-state-linux) 。


{% highlight text %}
{% endhighlight %}
