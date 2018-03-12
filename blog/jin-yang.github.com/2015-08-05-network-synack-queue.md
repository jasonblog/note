---
title: Linux 网络半链接、链接队列
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: linux,network,网络,链接,半链接,队列
description: 我们知道在建立链接阶段，需要完成三次握手，而这一阶段是在内核中完成，为此保存了两个队列：半链接队列 (SYN队列)、链接队列 (Accepted队列)。 那么，这两个队列分别对应到内核中的那些成员？队列的大小由那些参数决定？当队列满了之后，对应用的影响是什么？其报错的内容是什么？ 在这篇文章里大致介绍下。
---

我们知道在建立链接阶段，需要完成三次握手，而这一阶段是在内核中完成，为此保存了两个队列：半链接队列 (SYN队列)、链接队列 (Accepted队列)。

那么，这两个队列分别对应到内核中的那些成员？队列的大小由那些参数决定？当队列满了之后，对应用的影响是什么？其报错的内容是什么？

在这篇文章里大致介绍下。

<!-- more -->

## 简介

Linux 内核中的协议栈，在管理 TCP 链接时使用了两个队列，包括：半链接队列，用来保存处于 SYN_SENT 和 SYN_RECV 状态的请求；Accepted 队列，用来保存已经处于 established 状态，但是应用层还没来得及调用 accept() 取走的请求。

对这两个队列比较关心的，也就是这两队列在内核中的对象是什么，其长度又是怎么定义的。

注意，这两个队列都是在服务端的，而我们监听端口的建立是通过 listen() 完成，基本可以判断出来，两个队列的初始化操作也基本在该接口中调用完成。

所以首先看的是 listen() 系统调用。

## listen() - 队列的初始化

首先开始讨论的是 backlog 参数，英语中的原意是 "积压未办之事；没交付的订货" 。

讨论 backlog 时，容易想到 socket 编程中 `listen()` 接口的 backlog 参数，而该参数与 Linux 内核中的 backlog 是否一样？通过 `man 2 listen` 可以看到，对应该接口中 backlog 的解释如下。

{% highlight text %}
int listen(int sockfd, int backlog);

The backlog argument defines the maximum length to which the queue
of pending connections for sockfd may grow. If a connection request
arrives when the queue is full, the client may receive an error with
an indication of ECONNREFUSED or, if the underlying protocol supports
retransmission, the request may be ignored so that a later reattempt
at connection succeeds.
{% endhighlight %}

这里的解释非常简短，对于我们的帮助实际并不是很大，还是那句话 "Show me the CODE!!!" 。

首先是 listen() 系统调用的入口，按照上述的说法，可以想像的到，listen() 系统调用必然要分配一个 listen_sock 结构体。

{% highlight c %}
SYSCALL_DEFINE2(listen, int, fd, int, backlog)
{
    struct socket *sock;
    int err, fput_needed;
    int somaxconn;

    sock = sockfd_lookup_light(fd, &err, &fput_needed);
    if (sock) {
        /* backlog 会选择 listen() 传入的参数与 net.core.somaxconn 的最小值 */
        somaxconn = sock_net(sock->sk)->core.sysctl_somaxconn;
        if ((unsigned int)backlog > somaxconn)
            backlog = somaxconn;

        err = security_socket_listen(sock, backlog);
        if (!err)
            /* 实际会调用 inet_listen() */
            err = sock->ops->listen(sock, backlog);

        fput_light(sock->file, fput_needed);
    }
    return err;
}
{% endhighlight %}

也就是取得入参与 somaxconn 的最小值后，调用 inet_listen() 函数，在该函数中会初始化半链接队列，并设置 accepted 队列的最大值。

{% highlight c %}
int inet_listen(struct socket *sock, int backlog)
{
    ... ...
    /* 如果已经处于 listen 状态了，那么之修改 backlog */
    if (old_state != TCP_LISTEN) {
        ... ...                        // fastopen相关暂时忽略
        err = inet_csk_listen_start(sk, backlog);
        if (err)
            goto out;
    }
    sk->sk_max_ack_backlog = backlog;  // 设置accept队列的最大值
    ... ...
}
{% endhighlight %}

接着来看 inet_csk_listen_start() 函数的实现，其主要工作是调用 reqsk_queue_alloc() 新分配一个 listen socket 以及相应的 accept 队列；然后对申请的端口进行判断。

{% highlight c %}
int inet_csk_listen_start(struct sock *sk, const int nr_table_entries)
{
    ... ...
    // 在下面的函数中分配listen_sock以及相应的accept队列
    int rc = reqsk_queue_alloc(&icsk->icsk_accept_queue, nr_table_entries);
    ... ...
    // 这里之所以还要再次判断一下端口，是为了防止另一个进程 程在我们调用listen之前改变了这个端口的信息.
    sk->sk_state = TCP_LISTEN;
    if (!sk->sk_prot->get_port(sk, inet->inet_num)) {
        // 将端口赋值给sport，并加入到inet_hashinfo(上面已经分析过)的listening_hash hash链表中.
        inet->inet_sport = htons(inet->inet_num);

        sk_dst_reset(sk);
        sk->sk_prot->hash(sk);  // 这里调用__inet_hash实现的

        return 0;
    }
    ... ...
}
{% endhighlight %}

对于半链接队列，实际是在 reqsk_queue_alloc() 中完成，在该函数中同样会完成 listen_sock 新建。

{% highlight c %}
int reqsk_queue_alloc(struct request_sock_queue *queue,
              unsigned int nr_table_entries)
{
    size_t lopt_size = sizeof(struct listen_sock);
    struct listen_sock *lopt = NULL;

    nr_table_entries = min_t(u32, nr_table_entries, sysctl_max_syn_backlog);
    nr_table_entries = max_t(u32, nr_table_entries, 8);
    nr_table_entries = roundup_pow_of_two(nr_table_entries + 1);
    lopt_size += nr_table_entries * sizeof(struct request_sock *);

    if (lopt_size <= (PAGE_SIZE << PAGE_ALLOC_COSTLY_ORDER))
        lopt = kzalloc(lopt_size, GFP_KERNEL |
                      __GFP_NOWARN |
                      __GFP_NORETRY);
    if (!lopt)
        lopt = vzalloc(lopt_size);
    if (!lopt)
        return -ENOMEM;

    get_random_bytes(&lopt->hash_rnd, sizeof(lopt->hash_rnd));
    rwlock_init(&queue->syn_wait_lock);
    queue->rskq_accept_head = NULL;
    lopt->nr_table_entries = nr_table_entries;
    lopt->max_qlen_log = ilog2(nr_table_entries);

    write_lock_bh(&queue->syn_wait_lock);
    queue->listen_opt = lopt;
    write_unlock_bh(&queue->syn_wait_lock);

    return 0;
}
{% endhighlight %}

实际上，如果 listen(..., 20)，且内核参数值如下，那么此时 nr_table_entries = 16、max_qlen_log = 4 。
{% highlight text %}
$ sysctl -w net.ipv4.tcp_max_syn_backlog=2
$ sysctl -w net.core.somaxconn=3
{% endhighlight %}

但是，对于一个 `listen sock`，我们真的能保存 16 个 `SYN_RCV` 状态的链接吗？答案是 "不能" 。

### 数据结构

上面介绍了两个队列的创建过程，接下来再看下相关的数据结构。擦，有点本末倒置 ^_^

每当一个 SYN 请求到来时，都会新建一个 request_sock 结构体，并加入到 listen_sock.request_sock 的 hash 表中。完成握手后，将它放入到 inet_connection_sock.request_sock_queue 队列中；这样当 accept() 函数调用时，就会直接从这个队列中读取。

{% highlight c %}
struct request_sock {
    struct sock_common      __req_common;
    struct request_sock     *dl_next;
    u16             mss;
    u8              num_retrans; /* number of retransmits */
    u8              cookie_ts:1; /* syncookie: encode tcpopts in timestamp */
    u8              num_timeout:7; /* number of timeouts */
    /* The following two fields can be easily recomputed I think -AK */
    u32             window_clamp; /* window clamp at creation time */
    u32             rcv_wnd;      /* rcv_wnd offered first time */
    u32             ts_recent;
    unsigned long           expires;     // 这个请求在队列中存活的时间
    const struct request_sock_ops   *rsk_ops;
    struct sock         *sk;             // 链接成功的socket数目???
    u32             secid;
    u32             peer_secid;
};

struct listen_sock {
    u8          max_qlen_log;
    u8          synflood_warned;
    /* 2 bytes hole, try to use */
    int         qlen;                               // 当前的半连接队列的长度
    int         qlen_young;                         // 队列新成员的个数
    int         clock_hand;
    u32         hash_rnd;
    u32                 nr_table_entries;                   // 当前的syn_backlog(半开连接队列)的最大值
    struct request_sock *syn_table[0];              // 存放SYN队列，也即半链接队列
};

struct request_sock_queue {
    struct request_sock       *rskq_accept_head;
    struct request_sock       *rskq_accept_tail;
    rwlock_t                  syn_wait_lock;
    u8                        rskq_defer_accept;
    struct listen_sock        *listen_opt;          // 对应的listen_sock结构体
    struct fastopen_queue     *fastopenq;
};

struct inet_connection_sock {
    struct inet_sock          icsk_inet;
    struct request_sock_queue icsk_accept_queue;    // 已经建立链接的FIFO队列
    ... ...
    __u8                      icsk_syn_retries;
    unsigned long             icsk_timeout;
    struct timer_list         icsk_retransmit_timer;
    struct timer_list         icsk_delack_timer;
    u32                       icsk_user_timeout;
    ... ...
};
{% endhighlight %}

<!--
    struct listen_sock {
         u8           max_qlen_log;
    /*qlen最大长度取对数log，即log2 (max_qlen)，这个值在进入SYN/ACK定时器时有用*/
        int          qlen_young;
    /*也是指当前的半开连接队列长度,不过这个值会当重传syn/ack的时候(这里要注意是这个syn/ack第一次重传的时候才会减一)自动减一，也就是重传了SYN/ACK的request_sock不在是新的request_sock，在SYN/ACK定时器时有用*/

        int          clock_hand;
    /*每次SYN-ACK定时器超时时，我们需要遍历SYN队列哈希表，但表太大了，所以每次都只遍历部分哈希表，而每次遍历完，将哈希索引值放在clock_hand这里，下次遍历时直接从clock_hand开始，而不用从头开始*/
         u32          hash_rnd;
    };
-->

上述的结构体中，只包括了半链接的队列长度，而对于 accept 队列的长度，实际保存在 sock 结构体中。
{% highlight c %}
struct sock {
    unsigned short      sk_ack_backlog;     // 队列长度
    unsigned short      sk_max_ack_backlog; // 最大的队列长度
};
{% endhighlight %}
该 sock 是 server 端的 listen 队列长度，而 client 只保留成 request_sock 。

最后，当 accept() 从 icsk_accept_queue 队列中取得 request_sock 之后，将该变量从这个队列中释放，然后在 BSD 层新建一个 socket 结构，并将它和接收端新建的 sock 结构关联起来。

### accept() 系统调用

最终调用的是 inet_csk_accept() 函数，该函数会调用 reqsk_queue_get_child() 从 accept 队列中取一个 request_sock。

{% highlight c %}
struct sock *inet_csk_accept(struct sock *sk, int flags, int *err)
{
    struct request_sock_queue *queue = &icsk->icsk_accept_queue;
    // 查找已经建立的链接
    if (reqsk_queue_empty(queue)) {
        long timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);

        /* If this is a non blocking socket don't sleep */
        error = -EAGAIN;
        if (!timeo)
            goto out_err;

        error = inet_csk_wait_for_connect(sk, timeo);
        if (error)
            goto out_err;
    }
    req = reqsk_queue_remove(queue);
    newsk = req->sk;

    sk_acceptq_removed(sk);
}
{% endhighlight %}

## 源码解析

内核中与队列相关的设置参数如下：

* net.ipv4.tcp_max_syn_backlog[256]：用于保存半链接队列。注意，如果开启了 syncookies，那么基本上没有限制，所以在如下的实验中要关闭 cookies 。

* net.core.somaxconn[128]：Accepted 队列最大长度，表示最多有 129 个 established 链接等待 accept()。

### 握手过程

如上所述，握手链接过程包括了如下的步骤：

1. 收到客户端的 SYN 请求，并将这个请求放入 syn_table 中去；
2. 服务器端回复 SYNACK；
3. 收到客户端的 ACK；
4. 放入 accept queue 中。

对于 IPv4 来说，对应的操作函数通过如下结构体定义：

{% highlight c %}
const struct inet_connection_sock_af_ops ipv4_specific = {
    ... ...
    .conn_request      = tcp_v4_conn_request,   // 对应收到SYN请求
    .syn_recv_sock     = tcp_v4_syn_recv_sock,  // 对应收到ACK请求
    ... ...
};
{% endhighlight %}

接下来查看各个阶段的内容。

### 客户端发送 SYN 请求报文到服务端

此时，客户端和服务端分别处于 SYN_SENT 和 SYN_RECV 状态。

其中服务端的处理流程如下：

{% highlight text %}
tcp_v4_do_rcv()
 |-tcp_v4_hnd_req()                         # 被动打开时的处理，包括收到SYN或ACK
 |-sock_rps_save_rxhash()                   # 如果返回值nsk!=sk，则表示是收到了ACK报文
 |-tcp_child_process()                      # 处理新的sock
   |-tcp_rcv_state_process()
     |-icsk->icsk_af_ops->conn_request()    # 实际调用tcp_v4_conn_request()
       |-tcp_conn_request()
{% endhighlight %}

tcp_conn_request() 函数是主要的处理流程，其处理流程如下，在此重点关注一下 drop 的处理逻辑。

{% highlight c %}
int tcp_conn_request(struct request_sock_ops *rsk_ops,
             const struct tcp_request_sock_ops *af_ops,
             struct sock *sk, struct sk_buff *skb)
{
    ... ...
    /* 1. 判断半链接队列是否已满，listen_sock.qlen >> listen_sock.max_qlen_log */
    if ((sysctl_tcp_syncookies == 2 ||         // 为2表示用来测试
         inet_csk_reqsk_queue_is_full(sk)) && !isn) {
        want_cookie = tcp_syn_flood_action(sk, skb, rsk_ops->slab_name);
        if (!want_cookie)
            goto drop;
    }

    /* 2. 判断accept队列是否已满，sock.sk_ack_backlog > sock.sk_max_ack_backlog；
     * 以及 listen_sock.qlen_young 的值是否大于 1 。
     * 也就意味着，如果全连接队列满了，且有未重传过的半连接，则直接丢弃SYN请求
     */
    if (sk_acceptq_is_full(sk) && inet_csk_reqsk_queue_young(sk) > 1) {
        NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_LISTENOVERFLOWS);
        goto drop;
    }

    /* 链接未满，申请一个struct request_sock变量 */
    req = inet_reqsk_alloc(rsk_ops);
    if (!req)
        goto drop;

    ... ...

    /* OK, 一切正常，发送 SYNACK */
    err = af_ops->send_synack(sk, dst, &fl, req,
                  skb_get_queue_mapping(skb), &foc);
    if (!fastopen) {
        ... ...
        /* 实际调用 inet_csk_reqsk_queue_hash_add() 函数将该请求添加到半链接队列中 */
        af_ops->queue_hash_add(sk, req, TCP_TIMEOUT_INIT);
    }

    ... ...
}
{% endhighlight %}

#### 判断半链接队列是否已满

该判断通过 inet_csk_reqsk_queue_is_full(sk) 实现，也即比较如下的值。
{% highlight text %}
listen_sock.qlen >> listen_sock.max_qlen_log 。
{% endhighlight %}

其中的 qlen 代表的是 listen_sock 中的 syn_table 的长度，那什么是 max_qlen_log 呢？可以直接参考上面的介绍。

#### 判断accept队列已满

在上述的第二步中，在通过 sk_acceptq_is_full() 判断 accept 队列已满的情况下，同是也会判断 listen_sock 结构体中的 qlen_young 变量是否大于 1 。

而 qlen_young 是对 syn_table 的计数，当进入 syn_table 时加 1，出了syn_table 时减 1 。

首先介绍一下什么是 qlen_young，下面是摘抄自网络上的注释：

{% highlight text %}
qlen_young
sum of new open_request on syn_table. after TCP_TIMEOUT_INIT, if the open_request
hadn't be acked, then it can retrans corresponding syn/ack and it becoms old , the
qlen_young will dec 1. when open_request migrate to acceptq the qlen_yong will also
dec 1 except it had been retransmitted.

see tcp_synack_timer() for more details about qlen_young and clock_hand.
{% endhighlight %}

这也就意味着，qlen_young 记录了半链接队列里面第一次接收到的 SYN 报文数量，当然，其中不含已经超时重传的链接。

这也就意味着，如果 accept 队列满了，那么 qlen_young 就一直增加，而新来的请求就会直接被 drop 掉。但是，真实情况是，我们在客户端却很少发现这种情况，Why？？？

原因有两个：A) 客户端会不断重试，通过 tcp_syn_retries 设置；B) 在 inet_csk_reqsk_queue_prune() 函数中，会不断尝试发送 SYN+ACK 报文，此时 qlen_young 会减小。

这也就是说，在第一次的时候，之前的握手过程都没有重传过，所以这个 syn 包 server 端会直接 drop 掉，之后 client 会重传 syn，当 inet_csk_reqsk_queue_yong(sk) < 1 时，那么这个 syn 会被 server 端接受。server 会回复 synack 给 client，这样一来两边的状态就变为 client(ESTABLISHED)+server(SYN_SENT) 。

### 客户端回复 ACK 报文到服务端

当 client 收到 synack 后回复 ack 给 server，此时 server 端的处理流程为：

{% highlight text %}
tcp_v4_do_rcv()
 |-tcp_rcv_state_process()
   |-tcp_check_req()
   | |-inet_csk(sk)->icsk_af_ops->syn_recv_sock()     # 实际调用tcp_v4_syn_recv_sock()
   |   |-sk_acceptq_is_full()                         # 判断接收队列是否已满
   |   |-tcp_create_openreq_child()                   # 完成三次握手之后，才正式创建新的socket
   |
   |-req->rsk_ops->send_reset()                       # 如果设置了overflow，则会返回reset报文
{% endhighlight %}

如果 server 端设置了 sysctl_tcp_abort_on_overflow 那么 server 会发送 rst 给 client，并删除掉这个链接；否则 server 端只是记录一下 LINUX_MIB_LISTENOVERFLOWS，然后返回。

默认情况下是不会设置的，server 端只是标记连接请求块的 acked 标志，之后连接建立定时器，会遍历半连接表，重新发送 synack，重复上面的过程，具体的过程可以查看 inet_csk_reqsk_queue_prune()，重传会直到超过 synack 重传的阀值，才会把该连接从半连接链表中删除。

<br>

在函数 tcpp_create_openreq_child() 中才真正 clone 出一个新的 socket，也就是只有通过了 3 次握手后，Linux 才会产生新的 socket。而在 3 次握手中所传的 socket 实际上是 server 处于 listen 状态的 socket，那也就是说这个 socket 只有一个 TCP_LISTEN 状态。

## 实验

为了方便我们观察，需要修改一下 Linux 内核的配置参数。首先查看下操作系统默认的内核参数值，为了后面恢复。下面是自己笔记本上的默认配置参数。
{% highlight text %}
$ sysctl net.ipv4.tcp_max_syn_backlog
net.ipv4.tcp_max_syn_backlog = 256

$ sysctl net.core.somaxconn
net.core.somaxconn = 128

$ sysctl net.ipv4.tcp_syncookies
net.ipv4.tcp_syncookies = 1

$ sysctl net.ipv4.tcp_abort_on_overflow
net.ipv4.tcp_abort_on_overflow = 0

$ sysctl net.ipv4.tcp_syn_retries
net.ipv4.tcp_syn_retries = 6

$ sysctl net.ipv4.tcp_synack_retries
net.ipv4.tcp_synack_retries = 5
{% endhighlight %}

为了方便查看，我们先修改几个参数。

{% highlight text %}
$ sysctl -w net.ipv4.tcp_max_syn_backlog=2        # 将半链接队列设置较小的值
$ sysctl -w net.core.somaxconn=3                  # 监听队列的长度同样设小
$ sysctl -w net.ipv4.tcp_syncookies=0             # 关闭sync cookies
$ sysctl -w net.ipv4.tcp_syn_retries=2            # 当客户端无法链接时，重发SYN报文的次数，设置较小值
$ sysctl -w net.ipv4.tcp_synack_retries=20        # 服务端没有收到ACK时，重发SYN+ACK的次数，同样设置较大值
{% endhighlight %}

在代码中使用的是 listen(..., 20)，那么在设置完内核参数之后，我们首先计算一下两个队列的长度，其实上面已经计算过了。
{% highlight text %}
----- accept队列长度
entries = min(backlog/*from listen()*/, net.core.somaxconn) = 3

----- 半链接队列长度
entries = min(entries, net.ipv4.sysctl_max_syn_backlog) = 2
entries = max(entries, 8) = 8
entries = roundup_pow_of_two( 8 + 1) = 16
{% endhighlight %}

也就是说，半链接队列大小是 16；accept 队列大小是 3 。

### 测试程序

测试程序直接用 C 写的一个简单回显程序，源码可以从 [本地][syn_ack_source] 直接下载。

这个程序非常简单，服务端每次最多只会处理一个请求，只有当第一个客户端关闭之后，才会接着响应其它的请求，否则就一直保存在内核的队列中。

上述的服务器默认使用 8080 端口，为了观察方便，我们直接通过 ss 监控链接数，命令如下：

{% highlight text %}
# watch -n 1 "ss -tann '( sport = 8080 or dport = 8080 )'"
{% endhighlight %}

也就是查看源端口或者目的端口是 8080 的所有 TCP 链接，且每秒刷新一次。

接下来验证的时候简单分为两个场景，是否设置 net.ipv4.tcp_abort_on_overflow 。

###  设置 tcp_abort_on_overflow

如果设置 overflow=1 则在尝试建立第 6 个链接时报错 Connection reset by peer 。

![overflow1]{: .pull-center}

其中包括了一个 accept 的链接 + 四个在 accept 队列中的链接，那么在尝试建第六个的时候就会报错。

这里有个问题，我们设置的 accept 队列是 3，为什么保存了 4 个链接，可以参考如下的介绍，忘了从那里摘抄的了。
{% highlight text %}
When I use linux TCP socket, and find there is a bug in function sk_acceptq_is_full():
  When a new SYN comes, TCP module first checks its validation. If valid, send SYN+ACK to
the client and add the sock to the syn hash table.

  Next time if received the valid ACK for SYN+ACK from the client. server will accept this
connection and increase the sk->sk_ack_backlog (which is done in function tcp_check_req()).

  We check wether acceptq is full in function tcp_v4_syn_recv_sock().

Consider an example:
  After listen(sockfd, 1) system call, sk->sk_max_ack_backlog is set to 1. As we know,
sk->sk_ack_backlog is initialized to 0. Assuming accept() system call is not invoked now.

1. 1st connection comes. invoke sk_acceptq_is_full(). sk->sk_ack_backlog=0 sk->sk_max_ack_backlog=1,
   function return 0 accept this connection. Increase the sk->sk_ack_backlog
2. 2nd connection comes. invoke sk_acceptq_is_full(). sk->sk_ack_backlog=1 sk->sk_max_ack_backlog=1,
   function return 0 accept this connection. Increase the sk->sk_ack_backlog
3. 3rd connection comes. invoke sk_acceptq_is_full(). sk->sk_ack_backlog=2 sk->sk_max_ack_backlog=1,
   function return 1. Refuse this connection.

I think it has bugs. after listen system call. sk->sk_max_ack_backlog=1 but now it can accept 2 connections.
{% endhighlight %}

也就是说，accept 的链接数总是 +1 。

###  取消 tcp_abort_on_overflow

将该参数设置为 0 时，在 accept 队列溢出时就不会发送 rst 报文，而是直接 drop 报文。如果此时通过 tcpdump 之类的工具查看时，可以发现客户端一直在重试。

![overflow0]{: .pull-center width="700"}

此时，是在尝试建立第 9 个链接时报错 Connection timed out 。

我们在上述的计算过程中，半链接队列应该是 16 个才对，此时只有 3 个处于 SYN-RECV 的报文，这是为什么？？？ 还是看一下代码。

仍然是在 tcp_conn_request() 函数中，在已经尝试建立链接时，会发现如下的代码。

{% highlight c %}
int tcp_conn_request(struct request_sock_ops *rsk_ops,
             const struct tcp_request_sock_ops *af_ops,
             struct sock *sk, struct sk_buff *skb)
{
    ... ...
    /* Kill the following clause, if you dislike this way. */
    else if (!sysctl_tcp_syncookies &&
         (sysctl_max_syn_backlog - inet_csk_reqsk_queue_len(sk) <
          (sysctl_max_syn_backlog >> 2)) &&
         !tcp_peer_is_proven(req, dst, false,
                     tmp_opt.saw_tstamp)) {
        ... ...
        goto drop_and_release;
    }
}
{% endhighlight %}

在上述的判断中，可以从 1 到 3 计算一下，当尝试建立第 4 个链接时，就会为 true ，那么就直接将报文 drop 掉了。

## 总结

listen() 中的入参 backlog 不仅影响到了链接队列，还影响到了半链接队列。


<!--
何时添加到半链接队列中
.queue_hash_add = inet_csk_reqsk_queue_hash_add()
inet_csk_reqsk_queue_added()

tcp_rcv_synsent_state_process()    设置为TCP_SYN_RECV

擦，为什么 nr_table_entries 是 16 ，qlen 是 3 ，却不能再接收 SYN 请求了。
-->




[syn_ack_source]:    /reference/linux/network/syn_ack.tar.bz2        "简单的回显测试程序源码"
[overflow1]:         /images/linux/network-synack-overflow-1.png     "设置overflow时的参数"
[overflow0]:         /images/linux/network-synack-overflow-0.png     "取消overflow时的参数"
