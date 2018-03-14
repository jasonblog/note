---
title: Linux 網絡半鏈接、鏈接隊列
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: linux,network,網絡,鏈接,半鏈接,隊列
description: 我們知道在建立鏈接階段，需要完成三次握手，而這一階段是在內核中完成，為此保存了兩個隊列：半鏈接隊列 (SYN隊列)、鏈接隊列 (Accepted隊列)。 那麼，這兩個隊列分別對應到內核中的那些成員？隊列的大小由那些參數決定？當隊列滿了之後，對應用的影響是什麼？其報錯的內容是什麼？ 在這篇文章裡大致介紹下。
---

我們知道在建立鏈接階段，需要完成三次握手，而這一階段是在內核中完成，為此保存了兩個隊列：半鏈接隊列 (SYN隊列)、鏈接隊列 (Accepted隊列)。

那麼，這兩個隊列分別對應到內核中的那些成員？隊列的大小由那些參數決定？當隊列滿了之後，對應用的影響是什麼？其報錯的內容是什麼？

在這篇文章裡大致介紹下。

<!-- more -->

## 簡介

Linux 內核中的協議棧，在管理 TCP 鏈接時使用了兩個隊列，包括：半鏈接隊列，用來保存處於 SYN_SENT 和 SYN_RECV 狀態的請求；Accepted 隊列，用來保存已經處於 established 狀態，但是應用層還沒來得及調用 accept() 取走的請求。

對這兩個隊列比較關心的，也就是這兩隊列在內核中的對象是什麼，其長度又是怎麼定義的。

注意，這兩個隊列都是在服務端的，而我們監聽端口的建立是通過 listen() 完成，基本可以判斷出來，兩個隊列的初始化操作也基本在該接口中調用完成。

所以首先看的是 listen() 系統調用。

## listen() - 隊列的初始化

首先開始討論的是 backlog 參數，英語中的原意是 "積壓未辦之事；沒交付的訂貨" 。

討論 backlog 時，容易想到 socket 編程中 `listen()` 接口的 backlog 參數，而該參數與 Linux 內核中的 backlog 是否一樣？通過 `man 2 listen` 可以看到，對應該接口中 backlog 的解釋如下。

{% highlight text %}
int listen(int sockfd, int backlog);

The backlog argument defines the maximum length to which the queue
of pending connections for sockfd may grow. If a connection request
arrives when the queue is full, the client may receive an error with
an indication of ECONNREFUSED or, if the underlying protocol supports
retransmission, the request may be ignored so that a later reattempt
at connection succeeds.
{% endhighlight %}

這裡的解釋非常簡短，對於我們的幫助實際並不是很大，還是那句話 "Show me the CODE!!!" 。

首先是 listen() 系統調用的入口，按照上述的說法，可以想像的到，listen() 系統調用必然要分配一個 listen_sock 結構體。

{% highlight c %}
SYSCALL_DEFINE2(listen, int, fd, int, backlog)
{
    struct socket *sock;
    int err, fput_needed;
    int somaxconn;

    sock = sockfd_lookup_light(fd, &err, &fput_needed);
    if (sock) {
        /* backlog 會選擇 listen() 傳入的參數與 net.core.somaxconn 的最小值 */
        somaxconn = sock_net(sock->sk)->core.sysctl_somaxconn;
        if ((unsigned int)backlog > somaxconn)
            backlog = somaxconn;

        err = security_socket_listen(sock, backlog);
        if (!err)
            /* 實際會調用 inet_listen() */
            err = sock->ops->listen(sock, backlog);

        fput_light(sock->file, fput_needed);
    }
    return err;
}
{% endhighlight %}

也就是取得入參與 somaxconn 的最小值後，調用 inet_listen() 函數，在該函數中會初始化半鏈接隊列，並設置 accepted 隊列的最大值。

{% highlight c %}
int inet_listen(struct socket *sock, int backlog)
{
    ... ...
    /* 如果已經處於 listen 狀態了，那麼之修改 backlog */
    if (old_state != TCP_LISTEN) {
        ... ...                        // fastopen相關暫時忽略
        err = inet_csk_listen_start(sk, backlog);
        if (err)
            goto out;
    }
    sk->sk_max_ack_backlog = backlog;  // 設置accept隊列的最大值
    ... ...
}
{% endhighlight %}

接著來看 inet_csk_listen_start() 函數的實現，其主要工作是調用 reqsk_queue_alloc() 新分配一個 listen socket 以及相應的 accept 隊列；然後對申請的端口進行判斷。

{% highlight c %}
int inet_csk_listen_start(struct sock *sk, const int nr_table_entries)
{
    ... ...
    // 在下面的函數中分配listen_sock以及相應的accept隊列
    int rc = reqsk_queue_alloc(&icsk->icsk_accept_queue, nr_table_entries);
    ... ...
    // 這裡之所以還要再次判斷一下端口，是為了防止另一個進程 程在我們調用listen之前改變了這個端口的信息.
    sk->sk_state = TCP_LISTEN;
    if (!sk->sk_prot->get_port(sk, inet->inet_num)) {
        // 將端口賦值給sport，並加入到inet_hashinfo(上面已經分析過)的listening_hash hash鏈表中.
        inet->inet_sport = htons(inet->inet_num);

        sk_dst_reset(sk);
        sk->sk_prot->hash(sk);  // 這裡調用__inet_hash實現的

        return 0;
    }
    ... ...
}
{% endhighlight %}

對於半鏈接隊列，實際是在 reqsk_queue_alloc() 中完成，在該函數中同樣會完成 listen_sock 新建。

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

實際上，如果 listen(..., 20)，且內核參數值如下，那麼此時 nr_table_entries = 16、max_qlen_log = 4 。
{% highlight text %}
$ sysctl -w net.ipv4.tcp_max_syn_backlog=2
$ sysctl -w net.core.somaxconn=3
{% endhighlight %}

但是，對於一個 `listen sock`，我們真的能保存 16 個 `SYN_RCV` 狀態的鏈接嗎？答案是 "不能" 。

### 數據結構

上面介紹了兩個隊列的創建過程，接下來再看下相關的數據結構。擦，有點本末倒置 ^_^

每當一個 SYN 請求到來時，都會新建一個 request_sock 結構體，並加入到 listen_sock.request_sock 的 hash 表中。完成握手後，將它放入到 inet_connection_sock.request_sock_queue 隊列中；這樣當 accept() 函數調用時，就會直接從這個隊列中讀取。

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
    unsigned long           expires;     // 這個請求在隊列中存活的時間
    const struct request_sock_ops   *rsk_ops;
    struct sock         *sk;             // 鏈接成功的socket數目???
    u32             secid;
    u32             peer_secid;
};

struct listen_sock {
    u8          max_qlen_log;
    u8          synflood_warned;
    /* 2 bytes hole, try to use */
    int         qlen;                               // 當前的半連接隊列的長度
    int         qlen_young;                         // 隊列新成員的個數
    int         clock_hand;
    u32         hash_rnd;
    u32                 nr_table_entries;                   // 當前的syn_backlog(半開連接隊列)的最大值
    struct request_sock *syn_table[0];              // 存放SYN隊列，也即半鏈接隊列
};

struct request_sock_queue {
    struct request_sock       *rskq_accept_head;
    struct request_sock       *rskq_accept_tail;
    rwlock_t                  syn_wait_lock;
    u8                        rskq_defer_accept;
    struct listen_sock        *listen_opt;          // 對應的listen_sock結構體
    struct fastopen_queue     *fastopenq;
};

struct inet_connection_sock {
    struct inet_sock          icsk_inet;
    struct request_sock_queue icsk_accept_queue;    // 已經建立鏈接的FIFO隊列
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
    /*qlen最大長度取對數log，即log2 (max_qlen)，這個值在進入SYN/ACK定時器時有用*/
        int          qlen_young;
    /*也是指當前的半開連接隊列長度,不過這個值會當重傳syn/ack的時候(這裡要注意是這個syn/ack第一次重傳的時候才會減一)自動減一，也就是重傳了SYN/ACK的request_sock不在是新的request_sock，在SYN/ACK定時器時有用*/

        int          clock_hand;
    /*每次SYN-ACK定時器超時時，我們需要遍歷SYN隊列哈希表，但表太大了，所以每次都只遍歷部分哈希表，而每次遍歷完，將哈希索引值放在clock_hand這裡，下次遍歷時直接從clock_hand開始，而不用從頭開始*/
         u32          hash_rnd;
    };
-->

上述的結構體中，只包括了半鏈接的隊列長度，而對於 accept 隊列的長度，實際保存在 sock 結構體中。
{% highlight c %}
struct sock {
    unsigned short      sk_ack_backlog;     // 隊列長度
    unsigned short      sk_max_ack_backlog; // 最大的隊列長度
};
{% endhighlight %}
該 sock 是 server 端的 listen 隊列長度，而 client 只保留成 request_sock 。

最後，當 accept() 從 icsk_accept_queue 隊列中取得 request_sock 之後，將該變量從這個隊列中釋放，然後在 BSD 層新建一個 socket 結構，並將它和接收端新建的 sock 結構關聯起來。

### accept() 系統調用

最終調用的是 inet_csk_accept() 函數，該函數會調用 reqsk_queue_get_child() 從 accept 隊列中取一個 request_sock。

{% highlight c %}
struct sock *inet_csk_accept(struct sock *sk, int flags, int *err)
{
    struct request_sock_queue *queue = &icsk->icsk_accept_queue;
    // 查找已經建立的鏈接
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

## 源碼解析

內核中與隊列相關的設置參數如下：

* net.ipv4.tcp_max_syn_backlog[256]：用於保存半鏈接隊列。注意，如果開啟了 syncookies，那麼基本上沒有限制，所以在如下的實驗中要關閉 cookies 。

* net.core.somaxconn[128]：Accepted 隊列最大長度，表示最多有 129 個 established 鏈接等待 accept()。

### 握手過程

如上所述，握手鍊接過程包括瞭如下的步驟：

1. 收到客戶端的 SYN 請求，並將這個請求放入 syn_table 中去；
2. 服務器端回覆 SYNACK；
3. 收到客戶端的 ACK；
4. 放入 accept queue 中。

對於 IPv4 來說，對應的操作函數通過如下結構體定義：

{% highlight c %}
const struct inet_connection_sock_af_ops ipv4_specific = {
    ... ...
    .conn_request      = tcp_v4_conn_request,   // 對應收到SYN請求
    .syn_recv_sock     = tcp_v4_syn_recv_sock,  // 對應收到ACK請求
    ... ...
};
{% endhighlight %}

接下來查看各個階段的內容。

### 客戶端發送 SYN 請求報文到服務端

此時，客戶端和服務端分別處於 SYN_SENT 和 SYN_RECV 狀態。

其中服務端的處理流程如下：

{% highlight text %}
tcp_v4_do_rcv()
 |-tcp_v4_hnd_req()                         # 被動打開時的處理，包括收到SYN或ACK
 |-sock_rps_save_rxhash()                   # 如果返回值nsk!=sk，則表示是收到了ACK報文
 |-tcp_child_process()                      # 處理新的sock
   |-tcp_rcv_state_process()
     |-icsk->icsk_af_ops->conn_request()    # 實際調用tcp_v4_conn_request()
       |-tcp_conn_request()
{% endhighlight %}

tcp_conn_request() 函數是主要的處理流程，其處理流程如下，在此重點關注一下 drop 的處理邏輯。

{% highlight c %}
int tcp_conn_request(struct request_sock_ops *rsk_ops,
             const struct tcp_request_sock_ops *af_ops,
             struct sock *sk, struct sk_buff *skb)
{
    ... ...
    /* 1. 判斷半鏈接隊列是否已滿，listen_sock.qlen >> listen_sock.max_qlen_log */
    if ((sysctl_tcp_syncookies == 2 ||         // 為2表示用來測試
         inet_csk_reqsk_queue_is_full(sk)) && !isn) {
        want_cookie = tcp_syn_flood_action(sk, skb, rsk_ops->slab_name);
        if (!want_cookie)
            goto drop;
    }

    /* 2. 判斷accept隊列是否已滿，sock.sk_ack_backlog > sock.sk_max_ack_backlog；
     * 以及 listen_sock.qlen_young 的值是否大於 1 。
     * 也就意味著，如果全連接隊列滿了，且有未重傳過的半連接，則直接丟棄SYN請求
     */
    if (sk_acceptq_is_full(sk) && inet_csk_reqsk_queue_young(sk) > 1) {
        NET_INC_STATS_BH(sock_net(sk), LINUX_MIB_LISTENOVERFLOWS);
        goto drop;
    }

    /* 鏈接未滿，申請一個struct request_sock變量 */
    req = inet_reqsk_alloc(rsk_ops);
    if (!req)
        goto drop;

    ... ...

    /* OK, 一切正常，發送 SYNACK */
    err = af_ops->send_synack(sk, dst, &fl, req,
                  skb_get_queue_mapping(skb), &foc);
    if (!fastopen) {
        ... ...
        /* 實際調用 inet_csk_reqsk_queue_hash_add() 函數將該請求添加到半鏈接隊列中 */
        af_ops->queue_hash_add(sk, req, TCP_TIMEOUT_INIT);
    }

    ... ...
}
{% endhighlight %}

#### 判斷半鏈接隊列是否已滿

該判斷通過 inet_csk_reqsk_queue_is_full(sk) 實現，也即比較如下的值。
{% highlight text %}
listen_sock.qlen >> listen_sock.max_qlen_log 。
{% endhighlight %}

其中的 qlen 代表的是 listen_sock 中的 syn_table 的長度，那什麼是 max_qlen_log 呢？可以直接參考上面的介紹。

#### 判斷accept隊列已滿

在上述的第二步中，在通過 sk_acceptq_is_full() 判斷 accept 隊列已滿的情況下，同是也會判斷 listen_sock 結構體中的 qlen_young 變量是否大於 1 。

而 qlen_young 是對 syn_table 的計數，當進入 syn_table 時加 1，出了syn_table 時減 1 。

首先介紹一下什麼是 qlen_young，下面是摘抄自網絡上的註釋：

{% highlight text %}
qlen_young
sum of new open_request on syn_table. after TCP_TIMEOUT_INIT, if the open_request
hadn't be acked, then it can retrans corresponding syn/ack and it becoms old , the
qlen_young will dec 1. when open_request migrate to acceptq the qlen_yong will also
dec 1 except it had been retransmitted.

see tcp_synack_timer() for more details about qlen_young and clock_hand.
{% endhighlight %}

這也就意味著，qlen_young 記錄了半鏈接隊列裡面第一次接收到的 SYN 報文數量，當然，其中不含已經超時重傳的鏈接。

這也就意味著，如果 accept 隊列滿了，那麼 qlen_young 就一直增加，而新來的請求就會直接被 drop 掉。但是，真實情況是，我們在客戶端卻很少發現這種情況，Why？？？

原因有兩個：A) 客戶端會不斷重試，通過 tcp_syn_retries 設置；B) 在 inet_csk_reqsk_queue_prune() 函數中，會不斷嘗試發送 SYN+ACK 報文，此時 qlen_young 會減小。

這也就是說，在第一次的時候，之前的握手過程都沒有重傳過，所以這個 syn 包 server 端會直接 drop 掉，之後 client 會重傳 syn，當 inet_csk_reqsk_queue_yong(sk) < 1 時，那麼這個 syn 會被 server 端接受。server 會回覆 synack 給 client，這樣一來兩邊的狀態就變為 client(ESTABLISHED)+server(SYN_SENT) 。

### 客戶端回覆 ACK 報文到服務端

當 client 收到 synack 後回覆 ack 給 server，此時 server 端的處理流程為：

{% highlight text %}
tcp_v4_do_rcv()
 |-tcp_rcv_state_process()
   |-tcp_check_req()
   | |-inet_csk(sk)->icsk_af_ops->syn_recv_sock()     # 實際調用tcp_v4_syn_recv_sock()
   |   |-sk_acceptq_is_full()                         # 判斷接收隊列是否已滿
   |   |-tcp_create_openreq_child()                   # 完成三次握手之後，才正式創建新的socket
   |
   |-req->rsk_ops->send_reset()                       # 如果設置了overflow，則會返回reset報文
{% endhighlight %}

如果 server 端設置了 sysctl_tcp_abort_on_overflow 那麼 server 會發送 rst 給 client，並刪除掉這個鏈接；否則 server 端只是記錄一下 LINUX_MIB_LISTENOVERFLOWS，然後返回。

默認情況下是不會設置的，server 端只是標記連接請求塊的 acked 標誌，之後連接建立定時器，會遍歷半連接表，重新發送 synack，重複上面的過程，具體的過程可以查看 inet_csk_reqsk_queue_prune()，重傳會直到超過 synack 重傳的閥值，才會把該連接從半連接鏈表中刪除。

<br>

在函數 tcpp_create_openreq_child() 中才真正 clone 出一個新的 socket，也就是隻有通過了 3 次握手後，Linux 才會產生新的 socket。而在 3 次握手中所傳的 socket 實際上是 server 處於 listen 狀態的 socket，那也就是說這個 socket 只有一個 TCP_LISTEN 狀態。

## 實驗

為了方便我們觀察，需要修改一下 Linux 內核的配置參數。首先查看下操作系統默認的內核參數值，為了後面恢復。下面是自己筆記本上的默認配置參數。
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

為了方便查看，我們先修改幾個參數。

{% highlight text %}
$ sysctl -w net.ipv4.tcp_max_syn_backlog=2        # 將半鏈接隊列設置較小的值
$ sysctl -w net.core.somaxconn=3                  # 監聽隊列的長度同樣設小
$ sysctl -w net.ipv4.tcp_syncookies=0             # 關閉sync cookies
$ sysctl -w net.ipv4.tcp_syn_retries=2            # 當客戶端無法鏈接時，重發SYN報文的次數，設置較小值
$ sysctl -w net.ipv4.tcp_synack_retries=20        # 服務端沒有收到ACK時，重發SYN+ACK的次數，同樣設置較大值
{% endhighlight %}

在代碼中使用的是 listen(..., 20)，那麼在設置完內核參數之後，我們首先計算一下兩個隊列的長度，其實上面已經計算過了。
{% highlight text %}
----- accept隊列長度
entries = min(backlog/*from listen()*/, net.core.somaxconn) = 3

----- 半鏈接隊列長度
entries = min(entries, net.ipv4.sysctl_max_syn_backlog) = 2
entries = max(entries, 8) = 8
entries = roundup_pow_of_two( 8 + 1) = 16
{% endhighlight %}

也就是說，半鏈接隊列大小是 16；accept 隊列大小是 3 。

### 測試程序

測試程序直接用 C 寫的一個簡單回顯程序，源碼可以從 [本地][syn_ack_source] 直接下載。

這個程序非常簡單，服務端每次最多隻會處理一個請求，只有當第一個客戶端關閉之後，才會接著響應其它的請求，否則就一直保存在內核的隊列中。

上述的服務器默認使用 8080 端口，為了觀察方便，我們直接通過 ss 監控鏈接數，命令如下：

{% highlight text %}
# watch -n 1 "ss -tann '( sport = 8080 or dport = 8080 )'"
{% endhighlight %}

也就是查看源端口或者目的端口是 8080 的所有 TCP 鏈接，且每秒刷新一次。

接下來驗證的時候簡單分為兩個場景，是否設置 net.ipv4.tcp_abort_on_overflow 。

###  設置 tcp_abort_on_overflow

如果設置 overflow=1 則在嘗試建立第 6 個鏈接時報錯 Connection reset by peer 。

![overflow1]{: .pull-center}

其中包括了一個 accept 的鏈接 + 四個在 accept 隊列中的鏈接，那麼在嘗試建第六個的時候就會報錯。

這裡有個問題，我們設置的 accept 隊列是 3，為什麼保存了 4 個鏈接，可以參考如下的介紹，忘了從那裡摘抄的了。
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

也就是說，accept 的鏈接數總是 +1 。

###  取消 tcp_abort_on_overflow

將該參數設置為 0 時，在 accept 隊列溢出時就不會發送 rst 報文，而是直接 drop 報文。如果此時通過 tcpdump 之類的工具查看時，可以發現客戶端一直在重試。

![overflow0]{: .pull-center width="700"}

此時，是在嘗試建立第 9 個鏈接時報錯 Connection timed out 。

我們在上述的計算過程中，半鏈接隊列應該是 16 個才對，此時只有 3 個處於 SYN-RECV 的報文，這是為什麼？？？ 還是看一下代碼。

仍然是在 tcp_conn_request() 函數中，在已經嘗試建立鏈接時，會發現如下的代碼。

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

在上述的判斷中，可以從 1 到 3 計算一下，當嘗試建立第 4 個鏈接時，就會為 true ，那麼就直接將報文 drop 掉了。

## 總結

listen() 中的入參 backlog 不僅影響到了鏈接隊列，還影響到了半鏈接隊列。


<!--
何時添加到半鏈接隊列中
.queue_hash_add = inet_csk_reqsk_queue_hash_add()
inet_csk_reqsk_queue_added()

tcp_rcv_synsent_state_process()    設置為TCP_SYN_RECV

擦，為什麼 nr_table_entries 是 16 ，qlen 是 3 ，卻不能再接收 SYN 請求了。
-->




[syn_ack_source]:    /reference/linux/network/syn_ack.tar.bz2        "簡單的回顯測試程序源碼"
[overflow1]:         /images/linux/network-synack-overflow-1.png     "設置overflow時的參數"
[overflow0]:         /images/linux/network-synack-overflow-0.png     "取消overflow時的參數"
