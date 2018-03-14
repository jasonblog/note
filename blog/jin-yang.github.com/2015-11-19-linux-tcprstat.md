---
title: 通過 tcprstat 工具統計應答時間
layout: post
comments: true
category: [linux, database, mysql]
language: chinese
keywords: tcprstat,mysql,linux,響應時間
description: 一個不錯的工具，由 Percona 開發，可以用來統計服務端的數據處理時間，包括了最大最小值等統計信息。只要是使用 TCP 協議，採用服務器-客戶端的模式，都可以支持，如 MySQL 。
---

一個不錯的工具，可以用來統計服務端的數據處理時間，包括了最大最小值等統計信息。

本文簡單介紹該工具的使用方法及其實現原理。

<!-- more -->

![tcprstat logo]({{ site.url }}/images/databases/mysql/tcprstat-logo.jpg "tcprstat logo"){: .pull-center width="600px" }

## 簡介

該工具是 Percona 提供的一個工具，通常用來監測 MySQL 響應時間的，或者說是請求在服務器端的處理時間，其輸出結果包括了響應的很多統計值，用來診斷服務器端。

tcprstat 是一個基於 libpcap 的工具，通過提取 TCP libpcap 的捕獲時間 (struct pcap_pkthd.ts) 用來計算統計值。也就是通過測量 TCP 的 request 和 response 所需的時間間隔，適用於一問一答式協議類型的處理。

這也就意味著，該工具不僅適用於 MySQL 協議，對於 HTTP 也同樣適用。

### 源碼安裝

這個工具有點老了，從 [launchpad.net][source-code] 看，最近的一次更新是在 2010-09-01 :-( 不過還好，這個工具本身源碼較少，我們直接通過源碼進行安裝。

首先從 [launchpad.net][source-code] 上下載源碼，或者直接通過如下命令下載。

{% highlight text %}
$ bzr branch lp:tcprstat                       # 下載源碼
{% endhighlight %}

在編譯的時候，會生成兩個程序，分別為 tcprstat、tcprstat-static ，其中後者為靜態鏈接生成。這也就意味著，會依賴於 glibc-static 包；否則，編譯靜態文件時會報 ld: cannot find -lpthread 鏈接錯誤。

對於 CentOS，直接安裝 glibc 靜態庫，然後通過如下方式下載編譯。

{% highlight text %}
$ yum install glibc-static                     # 直接通過yum安裝所需的靜態庫
$ bash bootstrap && bash configure && make     # 編譯
{% endhighlight %}

在源碼中包括了一份 libpcap 的源碼，不過是比較老的版本。編譯完成後，會在 src 目錄下生成 tcprstat 及其靜態版本 tcprstat-static。對於後者，該版本不會依賴其它庫，可以直接複製到其它機器運行。


### 使用

該工具的使用也非常簡單，其幫助文檔可以參考 Percona 中的 [The tcprstat User Manual][offical-manual]。詳細的使用參數可以通過 tcprstat -h 查看，其中常見的參數有：

* \-\-port PORT, -p PORT
    指定監聽的端口號。

* \-\-format FORMAT, -f FORMAT
    指定輸出信息的格式。

* \-\-interval Nseconds, -t Nseconds
    指定監聽的時間間隔，默認是 10s 。

* \-\-iterations Ntimes, -n Ntimes
    指定循環的次數，默認是 1 次，0 表示永遠循環。

* \-\-read FILE, -r FILE
    如果是離線分析，通過該參數指定具體文件，網絡包可以通過 tcpdump -w FILE 獲得。

其輸出結果包括了時間戳，以及響應時間的最大值、均值、方差等信息，輸出信息可以通過 -f 參數進行定製，其中響應時間的單位為微妙。

{% highlight text %}
# tcprstat -p 3306 -t 1 -n 5                      # 監聽3306端口，每1秒輸出一次，共輸出5次
timestamp   count   max     min  avg  med  stddev  95_max  95_avg  95_std  99_max  99_avg  99_std
1283261499  1870    559009  39   883  153  13306   1267    201     150     6792    323     685
1283261500  1865    25704   29   578  142  2755    889     175     107     23630   333     1331
1283261501  1887    26908   33   583  148  2761    714     176     94      23391   339     1340
1283261502  2015    304965  35   624  151  7204    564     171     79      8615    237     507
1283261503  1650    289087  35   462  146  7133    834     184     120     3565    244     358

# tcprstat -l 192.168.52.129 -p 3306 -t 1 -n 0    # 設置本地地址為192.168.52.129，一直輸出
{% endhighlight %}

對於 bonding 的網卡，會報如下的錯誤 pcap: SIOCGIFFLAGS: bonding_masters: No such device ，此時可以使用如下方式啟動。

{% highlight text %}
# tcprstat -p 3306 -t 1 -n 0 -l \
    `/sbin/ifconfig | grep 'addr:[^ ]\+' -o | cut -f 2 -d : | xargs echo | sed -e 's/ /,/g'`
{% endhighlight %}

也就是用 IP 地址方式，而不是通過網絡接口方式指定。




## 源碼解析

與 tcpdump 一樣，tcprstat 也使用 libpcap 庫進行抓包，然後再通過程序對抓取的 tcp 包進行分析。估計之前很多發行版本不包括 libpcap ，所以在源碼包中同時包含了一份 libpcap 的源碼。

另外，需要注意的是，該命令會打開所有的設備，而且沒有提供命令行參數指定監聽的設備；可以通過修改如下源碼指定只監聽特定的設備。

{% highlight c %}
void * capture(void *arg) {
    ... ...
    //pcap = pcap_open_live("any", CAPTURE_LENGTH, 0, READ_TIMEOUT, errbuf);
    pcap = pcap_open_live("eth0", CAPTURE_LENGTH, 0, READ_TIMEOUT, errbuf);
    ... ...
}
{% endhighlight %}

代碼的實現也非常簡單，包括了在線和離線兩種方式，在此只介紹前者。

如果沒有使用 -r file 參數，也即使用在線分析，則會新建兩個線程 output_thread() 和 capture()；其中前者用於顯示統計信息值，後者為主要的抓包以及處理函數。

在 capture() 中會打開設備、設置過濾條件，做初始化操作；然後通過 libcap 進行抓包，而每次捕獲的網絡報文，會調用 process_packet() 函數處理。

![prcodess_ip_func]{: .pull-center width='650'}

如下為函數的調用邏輯。

{% highlight text %}
capture()                            # 捕獲網絡報文的函數
 |-pcap_loop()                       # libcap的循環處理函數
   |-process_packet()                # 主要處理函數
     |-process_ip()                  # 根據IP進行判斷
       |-is_local_address()          # 通過該函數判斷報文的近出
       |-inbound()                   # 判斷是進來的數據包處理
       |-outbound()                  # 出去的數據包

output_thread()                      # 統計信息的輸出線程
{% endhighlight %}

如上所述，process_packet() 是主要處理函數，在此再大致介紹下其處理流程如下：

1. 分析來源 IP 和目標 IP，看那個 IP 是本地的，來判斷是進來的包（請求包）還是出去的包（響應包）。

2. 如果包的數據大小為 0，那麼就跳過，不再處理。數據大小為 0 的視為 TCP 控制包。

3. 如果數據包為進來的包（請求包），則插入一條記錄到哈希表。

4. 如果數據包為出去的包（響應包），則用現在的包和之前插入哈希表中的響應包做時間差計算，並把之前的包在哈希表中刪除。


### 本地報文(無效)

需要注意的是，如果是在本地通過 MySQL 客戶端登陸，則該工具實際是無效的。

問題出在 is_local_address() 函數中，無法判斷是響應包還是請求包，因為報文的 IP 均為 127.0.0.1 。可以在 process_ip() 中添加如下內容查看。

{% highlight c %}
int process_ip(pcap_t *dev, const struct ip *ip, struct timeval tv) {
    ... ...   // 查看每個報文的源和目的地址
    printf("src adddress %s\n", inet_ntoa(ip->ip_src));
    printf("dst adddress %s\n", inet_ntoa(ip->ip_dst));
    ... ...
}
{% endhighlight %}

而獲取本地地址，有兩種方式，分別為通過 get_address() 函數自動獲取；或者通過 -l 參數指定。對於前者的值，可以通過在函數末尾添加如下內容查看。

{% highlight c %}
int get_addresses(void) {
    ... ...
    struct address_list *tmp;
    for (tmp = address_list.next; tmp; tmp = tmp->next) {
        printf("local adddress %s\n", inet_ntoa(tmp->in_addr));
    }
    return 0;
}
{% endhighlight %}

不過對於本地的鏈接，即使通過 -l 127.1 設置了 IP，在進行判斷時都會被判斷為 income，從而也是無效的。

對於這種情況，可以通過 ifconfig 設置浮動 IP 地址 (一般在127.0.0.1/8網段)，然後服務監聽該 IP 地址，啟動 tcprstat 時，使用 -l 參數指定本地 IP 地址。


## 參考

tcprstat 的官方文檔 [The tcprstat User's Manual][offical-manual]，介紹了該工具的使用方法；也可以從 [本地](/reference/mysql/tcprstat.tar.bz2) 下載。

[source-code]:        https://launchpad.net/tcprstat                     "保存在launchpad上的源碼"
[offical-manual]:     https://www.percona.com/docs/wiki/tcprstat:start   "The tcprstat User's Manual"
[prcodess_ip_func]:   /images/linux/tcprstat_process_ip.png              "process_ip()函數的處理流程"
