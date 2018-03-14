---
title: Linux 網絡常見監控項以及報錯
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: 網絡,監控,linux,network
description: 本來是想將報錯和監控拆開的，但是發現兩者幾乎是耦合的，通過監控項才能發現錯誤，定為錯誤的原因時也要依賴監控項，索性就將兩者合併到了一起。 對於很多的報錯，實際上你即使看到報錯的信息也不清楚到底是那裡報錯了，或者模稜兩可，甚至是誤導。最好的方式是 "Show me the code" 。 在此，簡單介紹一下與網絡相關的調試、查看方法，當然也包括了報錯相關的內容。
---

本來是想將報錯和監控拆開的，但是發現兩者幾乎是耦合的，通過監控項才能發現錯誤，定為錯誤的原因時也要依賴監控項，索性就將兩者合併到了一起。

對於很多的報錯，實際上你即使看到報錯的信息也不清楚到底是那裡報錯了，或者模稜兩可，甚至是誤導。最好的方式是 "Show me the code" 。

在此，簡單介紹一下與網絡相關的調試、查看方法，當然也包括了報錯相關的內容。

<!-- more -->

## 簡介

查看網絡的狀態有很多中方法，如之前介紹的 [網絡狀態查看命令 netstat VS. ss](/post/network-nettools-vs-iproute2.html)，也即使用 netlink 機制；還有 procfs 。相比來說，前者效率更高，但是後者查看會更加方便。

在此僅介紹 procfs 的相關實現，內核的相關實現在 net/core/net-procfs.c 中，所以如果對 /proc/net 下的文件內容不熟悉，可以直接查看內核中的統計項。


## 查看丟包

網絡丟包會有多種可能，例如，交換機，上連和下連端口的流量跑滿或鏈路有問題，那麼數據包就有可能會被交換機丟掉；負載均衡設備，包括了硬件設備以及軟件的負載均衡。

在此，我們僅查看本機可能導致的掉包。

### 操作系統處理不過來，丟棄數據

有兩種情況，一是網卡發現操作系統處理不過來，丟數據包，可以讀取下面的文件：

{% highlight text %}
$ cat /proc/net/dev
{% endhighlight %}

每個網絡接口一行統計數據，第 4 列（errs）是接收出錯的數據包數量，第 5 列（drop）是接收不過來丟棄的數量。

第二部分是傳統非 NAPI 接口實現的網卡驅動，每個 CPU 有一個隊列，當在隊列中緩存的數據包數量超過 net.core.netdev_max_backlog 時，網卡驅動程序會丟掉數據包，這個見下面的文件：

{% highlight text %}
$ cat /proc/net/softnet_stat
{% endhighlight %}

每個 CPU 有一行統計數據，第二列是對應 CPU 丟棄的數據包數量。

### 應用程序處理不過來，操作系統丟棄

內核中記錄了兩個計數器：

* ListenOverflows：當 socket 的 listen queue 已滿，當新增一個連接請求時，應用程序來不及處理；

* ListenDrops：包含上面的情況，除此之外，當內存不夠無法為新的連接分配 socket 相關的數據結構時，也會加 1，當然還有別的異常情況下會增加 1。

分別對應下面文件中的第 21 列（ListenOverflows）和第 22 列（ListenDrops），可以通過如下命令查看：

{% highlight text %}
$ cat /proc/net/netstat | awk '/TcpExt/ { print $21,$22 }'
{% endhighlight %}

如果使用 netstat 命令，有丟包時會看到 "times the listen queue of a socket overflowed" 以及 "SYNs to LISTEN sockets ignored" 對應行前面的數字；如果值為 0 則不會輸出對應的行。


## Out of memory

直接看代碼，在內核中，對應的代碼如下。

{% highlight c %}
bool tcp_check_oom(struct sock *sk, int shift)
{
    bool too_many_orphans, out_of_socket_memory;

    too_many_orphans = tcp_too_many_orphans(sk, shift);
    out_of_socket_memory = tcp_out_of_memory(sk);

    if (too_many_orphans)
        net_info_ratelimited("too many orphaned sockets\n");
    if (out_of_socket_memory)
        net_info_ratelimited("out of memory -- consider tuning tcp_mem\n");
    return too_many_orphans || out_of_socket_memory;
}

static int tcp_out_of_resources(struct sock *sk, bool do_reset)
{
    struct tcp_sock *tp = tcp_sk(sk);
    int shift = 0;

    /* If peer does not open window for long time, or did not transmit
     * anything for long time, penalize it. */
    if ((s32)(tcp_time_stamp - tp->lsndtime) > 2*TCP_RTO_MAX || !do_reset)
        shift++;

    /* If some dubious ICMP arrived, penalize even more. */
    if (sk->sk_err_soft)
        shift++;

    if (tcp_check_oom(sk, shift)) {
        /* Catch exceptional cases, when connection requires reset.
         *      1. Last segment was sent recently. */
        if ((s32)(tcp_time_stamp - tp->lsndtime) <= TCP_TIMEWAIT_LEN ||
            /*  2. Window is closed. */
            (!tp->snd_wnd && !tp->packets_out))
            do_reset = true;
        if (do_reset)
            tcp_send_active_reset(sk, GFP_ATOMIC);
        tcp_done(sk);
        NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_TCPABORTONMEMORY);
        return 1;
    }
    return 0;
}
{% endhighlight %}

如上所示，出現內存不足可能會有兩種情況：

* 有太多的 orphan sockets，通常對於一些前端的服務器經常會出現這種情況。

* 分配給 TCP 的內存確實較少，從而導致內存不足。

### 內存不足

這個比較好排查，只需要查看一下實際分配給 TCP 多少內存，現在時用了多少內存即可。需要注意的是，通常的配置項使用的單位是 Bytes，在此用的是 Pages，通常為 4K 。

先查看下給 TCP 分配了多少內存。

{% highlight text %}
$ cat /proc/sys/net/ipv4/tcp_mem
183474  244633  366948
{% endhighlight %}

簡單來說，三個值分別表示進入 無壓力、壓力模式、內存上限的值，當到達最後一個值的時候就會報錯。

接下來查看一下當前使用的內存。

{% highlight text %}
$ cat /proc/net/sockstat
sockets: used 855
TCP: inuse 17 orphan 1 tw 0 alloc 19 mem 3
UDP: inuse 16 mem 10
UDPLITE: inuse 0
RAW: inuse 1
FRAG: inuse 0 memory 0
{% endhighlight %}

其中的 mem 表示使用了多少 Pages，如果相比 tcp_mem 的配置來說還很小，那麼就有可能是由於 orphan sockets 導致的。


### orphan sockets

首先介紹一下什麼是 orphan sockets，簡單來說就是該 socket 不與任何一個文件描述符相關聯。例如，當應用調用 close() 關閉一個鏈接時，此時該 socket 就成為了 orphan，但是該 sock 仍然會保留一段時間，直到最後根據 TCP 協議結束。

實際上 orphan socket 對於應用來說是無用的，因此內核希望儘可能減小 orphan 的數量。對於像 http 這樣的短請求來說，出現 orphan 的概率會比較大。

對於系統允許的最大 orphan 數量，以及當前的 orphan 數量可以通過如下方式查看：

{% highlight text %}
$ cat /proc/sys/net/ipv4/tcp_max_orphans
32768
$ cat /proc/net/sockstat
... ...
TCP: inuse 37 orphan 14 tw 8 alloc 39 mem 9
... ...
{% endhighlight %}

你可能會發現，sockstat 中的 orphan 數量要遠小於 tcp_max_orphans 的數目。

實際上，可以從代碼中看到，實際會有個偏移量 shift，該值範圍為 [0, 2] 。

{% highlight c %}
static inline bool tcp_too_many_orphans(struct sock *sk, int shift)
{
    struct percpu_counter *ocp = sk->sk_prot->orphan_count;
    int orphans = percpu_counter_read_positive(ocp);

    if (orphans << shift > sysctl_tcp_max_orphans) {
        orphans = percpu_counter_sum_positive(ocp);
        if (orphans << shift > sysctl_tcp_max_orphans)
            return true;
    }
    return false;
}
{% endhighlight %}

也就是說，在某些場景下會對 orphan 做些懲罰，將 orphan 的數量 2x 甚至 4x，這也就解釋了上述的問題。

如果是這樣，那麼就可以根據具體的情況，將 tcp_max_orphans 值適當調大。

### 總結

除了可能真正出現內存不足的情況之外，還有可能是由於內核的懲罰措施，導致 orphan 的誤報。


## Procfs 文件系統

* /proc/net/tcp：記錄 TCP 的狀態信息。


{% highlight text %}
{% endhighlight %}
