---
title: HAProxy 簡介
layout: post
comments: true
language: chinese
category: [linux,network,misc]
keywords: haproxy
description: HAProxy 是一個免費的負載均衡軟件，可以運行於大部分主流的 Linux 操作系統上，提供了 L4 和 L7 兩種負載均衡能力，可媲美商用負載均衡器的性能和穩定性，使其不僅僅是免費負載均衡軟件的首選，更幾乎成為了唯一選擇。這裡簡單介紹下 HAProxy 的使用方式。
---

HAProxy 是一個免費的負載均衡軟件，可以運行於大部分主流的 Linux 操作系統上，提供了 L4 和 L7 兩種負載均衡能力，可媲美商用負載均衡器的性能和穩定性，使其不僅僅是免費負載均衡軟件的首選，更幾乎成為了唯一選擇。

這裡簡單介紹下 HAProxy 的使用方式。

<!-- more -->

## 簡介

相比 Nginx 來說，HAProxy 支持自定義 URL 健康監測；會話保持除了可以使用 `IP_HASH` 外，還可以使用 `URL_HASH` 算法；支持多種負載均衡等等，如下是 HAProxy 提供的主要功能：

* 負載均衡。提供 L4 和 L7 兩種模式，支持 `RoundRobin`、`Static RoundRobin`、`LeastConnection`、`Source IP Hash`、`URI Hash`、`URL_PARAM Hash`、`HTTP_HEADER Hash` 等豐富的負載均衡算法；
* 健康檢查。支持 `TCP`、`HTTP`、`SSL`、`MySQL`、`Redis` 等多種健康檢查模式；
* 會話保持。對於未實現會話共享的應用集群，可通過 `Insert Cookie`、`Rewrite Cookie`、`Prefix Cookie` 以及上述的多種 `Hash` 方式實現會話保持；
* SSL 支持。可以解析 `HTTPS` 協議，並能夠將請求解密為 `HTTP` 後向後端傳輸；
* HTTP。可以對請求進行重寫與重定向，支持多種 `HTTP` 模式；
* 監控與統計。提供了基於 Web 的統計頁面，展現健康狀態和流量數據。

在配置文件中可以通過 `balance` 參數指定算法，如下僅介紹一些常用的算法：

{% highlight text %}
roundrobin:
  輪詢，適用於短鏈接，會根據權重比選擇服務器，支持4096個後端服務器；注意，這裡的權重信息可以動態修改。
static-rr:
  與上類似，只是這裡的服務器權重不能動態修改，而且對於服務器的數量沒有限制，消耗CPU相對也較少。
leastconn:
  選擇鏈接數最少的服務器建立鏈接，通常在使用長連接時，例如MySQL、LDAP等，對於短鏈接HTTP不建議使用；
  這裡的服務器權重可以動態調整。
first:
  忽略權重信息，選擇第一個可用服務器，需要設置服務器的最大連接數，適用於長連接；常用於動態擴容的場景，
  一般有工具用於動態監測，提供動態伸縮服務。
source
  源地址算法，通過客戶端的原IP地址進行hash，只要沒有服務器宕機，那麼一個客戶端的請求都會由同一臺服務器處理。
uri
  算法，需要使用HTTP服務，通過 HTTP URI 地址來選擇對應服務器。
hdr
  通過 HTTP eder 內容來選擇對應服務器。
{% endhighlight %}

在源碼中，其實現在 `lb_XXX.c` 文件中，關於 HAProxy 的健康檢查可以參考 [Health checking](https://www.haproxy.com/doc/aloha/7.0/haproxy/healthchecks.html) 。

## 配置文件

詳細可以查看官方的文檔 [doc/configuration.txt](http://www.haproxy.org/download/1.7/doc/configuration.txt) ，這裡簡單介紹常見的概念。

如下是常見的運維操作。

{% highlight text %}
----- 測試配置文件是否有語法錯誤
# haproxy -c -f /etc/haproxy/haproxy.cfg
----- 沒有問題則嘗試啟動
# haproxy -f /etc/haproxy/haproxy.cfg
----- 重新加載
# haproxy -f /etc/haproxy/haproxy.cfg -sf `cat /var/run/haproxy.pid`
{% endhighlight %}

在通過 `systemctl status haproxy` 啟動時，也就是使用 systemd 時，實際會有一個 wrap 程序啟動，所以會看到有多個進程啟動。

HAProxy 配置文件由全局配置+代理配置兩部分組成，又分為五段：global、defaults、frontend、backend、listen。

{% highlight text %}
global:
  全局配置內容，用於定義全局參數，屬於進程級的配置，通常和操作系統配置有關。
default:
  作為frontend、backend、listen的默認配置參數。
frontend:
  接收請求的前端虛擬節點，在1.3版本引入，用於簡化haproxy配置文件複雜度，可以通過ACL規則指定要使用的後端backend。
backend:
  後端服務器配置。
listen:
  frontend+backend結合體，1.3版本之前使用，主要為了保持兼容性。
{% endhighlight %}

### 日誌配置

在 `global` 中設置日誌的級別，然後通過 syslog 進行保存。

{% highlight text %}
$ cat /etc/haproxy/haproxy.conf
global
  log 127.0.0.1 local2 info
{% endhighlight %}

使用 `rsyslog` 保存。

{% highlight text %}
----- 確認添加了include子目錄，因為如下的HAProxy配置會放置到子目錄下。
cat /etc/rsyslog.conf | grep "IncludeConfig"
----- 修改rsyslog的主配置文件，開啟遠程日誌，查看是否開啟UDP端口
cat /etc/sysconfig/rsyslog | grep "SYSLOGD_OPTIONS"
SYSLOGD_OPTIONS="-c 2 -r -m 0"
#-c 2 使用兼容模式，默認是 -c 5
#-r 開啟遠程日誌
#-m 0 標記時間戳。單位是分鐘，為0時，表示禁用該功能

----- 添加HAProxy配置文件，需要注意最後一行，否則會同時寫入haproxy.log和message文件
cat /etc/rsyslog.d/haproxy.conf
$ModLoad imudp
$UDPServerRun 514
local2.*     /var/log/haproxy.log
&~
{% endhighlight %}

### 配置文件


{% highlight text %}
#---------------------------------------------------------------------
# Global settings
#---------------------------------------------------------------------
global
    # to have these messages end up in /var/log/haproxy.log you will
    # need to:
    # 1) configure syslog to accept network log events.  This is done
    #    by adding the '-r' option to the SYSLOGD_OPTIONS in
    #    /etc/sysconfig/[r]syslog
    # 2) configure local2 events to go to the /var/log/haproxy.log
    #   file. A line like the following can be added to
    #   /etc/sysconfig/syslog
    #    local2.*                       /var/log/haproxy.log
    log         127.0.0.1 local2 debug


    chroot      /var/lib/haproxy
    pidfile     /var/run/haproxy.pid
    maxconn     4000
    user        haproxy
    group       haproxy
    daemon

    # turn on stats unix socket
    stats socket /var/run/haproxy.sock level admin process 1
    stats timeout 5m                   # 設置等待輸入超時時間為5min

#---------------------------------------------------------------------
# common defaults that all the 'listen' and 'backend' sections will
# use if not designated in their block
#---------------------------------------------------------------------
defaults
    mode                    http
    log                     global
    option                  tcplog                 # 默認只打印很少的信息，指定打印詳細信息
    option                  httplog
    option                  dontlognull            # 默認會將保活等信息記錄到日誌，可通過該選項關閉
    option http-server-close
    option                  redispatch             # 使用cookies後會發送到指定服務器，如果服務器不可用則重新調度
    retries                 3                      # 3次連接失敗則認為服務不可用
    timeout http-request    10s                    # 默認http請求超時時間
    timeout queue           1m                     # 默認隊列超時時間
    timeout connect         10s                    # 默認連接超時時間
    timeout client          1m                     # 默認客戶端超時時間
    timeout server          1m                     # 默認服務器超時時間
    timeout http-keep-alive 10s                    # 默認持久連接超時時間
    timeout check           10s                    # 默認檢查時間間隔
    maxconn                 3000                   # 最大連接數

#---------------------------------------------------------------------
# main frontend which proxys to the backends
#---------------------------------------------------------------------
frontend  main *:5000
    # 新建acl策略path_beg以/static /images等開頭的訪問路徑，-i忽略大小寫 
    acl url_static       path_beg       -i /static /images /javascript /stylesheets
    # 新建acl策略path_end以.jpg .gif等結尾的訪問路徑，-i忽略大小寫 
    acl url_static       path_end       -i .jpg .gif .png .css .js

    # 如果匹配url_static這個acl策略，則使用static這個後端 
    use_backend static          if url_static
    # 沒有任何配置的情況下，使用默認的後端app 
    default_backend             app

#---------------------------------------------------------------------
# static backend for serving up images, stylesheets and such
#---------------------------------------------------------------------
backend static
    balance     roundrobin
    server      static 127.0.0.1:4331 check

#---------------------------------------------------------------------
# round robin balancing between the various backends
#---------------------------------------------------------------------
backend app
    balance     roundrobin
    server  app1 127.0.0.1:5001 check
    server  app2 127.0.0.1:5002 check
    server  app3 127.0.0.1:5003 check
    server  app4 127.0.0.1:5004 check

#---------------------------------------------------------------------
# 1.3之前使用，為了保持向後兼容，建議使用frontend+backend代替
# 關於server的詳細配置選項可以參考Server and default-server options
#  * maxconn NUM 每個後端服務器的最大鏈接數，超過該值則保存在隊列中
#  * maxqueue NUM 超過該隊列後會重新選擇後端服務器
#---------------------------------------------------------------------
listen http-proxy 192.168.1.201:80
    mode http
    option httpchk GET /test
    balance roundrobin
    timeout server 30s
    timeout client 30s
    server server-01 192.168.1.101:80 check inter 2s
    server server-02 192.168.1.102:80 check inter 2s
    server server-03 192.168.1.103:80 check inter 2s
{% endhighlight %}


<!--
### HTTPS 配置 (SSL Termination With HAProxy)

對 http://www.foobar.com 域名的訪問會自動跳轉為 https://www.foobar.com ，而對 http://haproxy.foobar.com 訪問走 http 協議。

http://virtuallyhyper.com/2013/05/configure-haproxy-to-load-balance-sites-with-ssl/
http://www.ilanni.com/?p=10641
http://www.oschina.net/translate/getting-the-most-of-haproxy?print

另外，源碼目錄下有很多不錯的文檔可以參考，例如架構(architecture.txt)、配置文件(configuration.txt)、管理(management.txt)等，還有很多內部設計文檔。

/etc/haproxy
/etc/haproxy/haproxy.cfg #配置文件
/etc/logrotate.d/haproxy #日誌輪轉
/etc/rc.d/init.d/haproxy #運行腳本
/usr/bin/halog           #日誌分析工具
/usr/sbin/haproxy

可以通過haproxy -vv查看詳細的編譯參數。


## 監控信息

關於監控項的詳細介紹可以查看源碼 doc/management.txt 文件中 Statistics and monitoring 的內容，目前主要分為了三部分：1) Frontend；2) Backend(Servers)；3) HAProxy Health Check 。

**注意** 按照 HAProxy 的模型來說，是單進程的，如果為了優化多核場景採用多進程，那麼需要為每個進程配置一個訪問地址。

### 監控頁面

可以通過如下配置打開一個 HTTP 監控頁面，然後可以直接通過瀏覽器訪問，如果要查看 csv 格式，可以通過 ```curl --silent "http://192.144.35.35:85/status;csv"``` 命令查看。

```
listen stats_auth 192.144.35.35:85
    mode http
    stats enable
    bind-process 1                   # 監控的HAProxy進程
    stats uri /status                # 監控URI地址
    stats admin if LOCALHOST         # 默認監控頁面是隻讀，可以通過該選項開啟啟停服務等操作
    stats auth monitor:foobar        # 指定登陸用戶密碼，可以配置多個
    stats realm "Haproxy Statistics" # 如果沒有登陸，瀏覽器的用戶名密碼彈出窗口
    stats refresh 5s                 # 自動刷新，通常用於一直打開的瀏覽器頁面
    stats hide-version               # 隱藏版本信息
    stats scope .                    # 指定監控顯示的範圍
    stats show-desc Node for Europe  # 後續的都是描述信息
    stats show-legends
    stats show-node
```

Starting frontend GLOBAL: cannot bind UNIX socket [/var/run/haproxy1.sock]

### Unix Socket

相比來說這是更好的方式，方便自動化、可以在線修改配置、安全，可以通過如下配置進行修改。

```
global # Make sure you add it to the global section
    stats socket /var/run/haproxy.sock mode 600 level admin process 1  # uid 0 gid 0
    stats timeout 2m # Wait up to 2 minutes for input
```

timeout 選項通常是用在交互場景下；而 mode 600 level admin 分別表示 socket 的權限，以及是否開啟管理模式，默認是隻讀的；然後就可以通過 netcat 或者 socat 進行訪問，當然官方推薦使用後者，而實際上前者更加通用些；Socket 方式提供了非交互式以及交互式兩種。

非交互方式比較適合腳本採集監控數據，可以一次發送多個命令，如下分別返回 HAProxy 的進程信息，以及監控數據(頁面 URI 返回的數據)。

echo "show info;show stat" | nc -U /var/run/haproxy.sock

交互模式實際上也就是單行，比較適合登陸查看，如下通過 prompt 命令，表示進入交互模式；更多命令可以查看源碼 doc/management.txt 文件中 Unix Socket commands 的內容。

$ nc -U /var/run/haproxy.sock
$ prompt
> show info

cannot bind socket
cat /etc/sysctl.conf
net.ipv4.ip_nonlocal_bind=1
sysctl -p


### 監控指標

列舉常用的監控指標，以及源碼中對應的函數。

#### show info

對應源碼中的 stats_fill_info() 函數，主要包括了當前 HAProxy 的配置信息，例如資源限制、資源使用率等信息。

Name: HAProxy
Version: 1.5.18
Release_date: 2016/05/10
Nbproc: 4                  配置的進程數，在配置文件中通過nbproc配置
Process_num: 1             當前的進程號，編號從1開始
Pid: 96699                 當前的進程ID
Uptime: 1d 20h10m06s       服務啟動時間，格式化後
Uptime_sec: 159006         服務啟動時間，按秒計算
Memmax_MB: 0               當前進程可以使用的最大內存，對應了global.rlimit_memmax變量，會在啟動時通過global.rlimit_memmax_all計算
Ulimit-n: 8036             單個進程最大的描述符數global.rlimit_nofile，可以在配置文件中通過ulimit-n設置，否則與maxsock相同
Maxsock: 8036              最大sock數目，在程序中單獨計算，與maxconn、pipe數量相關，詳見global.maxsock變量
Maxconn: 4000              可以在配置文件中通過maxconn設置
Hard_maxconn: 4000
CurrConns: 0
CumConns: 3
CumReq: 3
Maxpipes: 0
PipesUsed: 0
PipesFree: 0
ConnRate: 0
ConnRateLimit: 0
MaxConnRate: 0
SessRate: 0
SessRateLimit: 0
MaxSessRate: 0
ZlibMemUsage: 0
MaxZlibMemUsage: 0
Tasks: 11                  內存中保存的任務數，通過task_new()函數分配，注意可能部分任務只是緩存，並沒有使用
Run_queue: 1               當前運行隊列的大小，可以參考run_queue_cur變量，包含了當前查詢的鏈接，詳見process_runnable_tasks()函數
Idle_pct: 100              空閒比率
node: hamonitor1           節點信息

si_attach_conn()
 |-conn_attach()


cfg_register_section() 通過該函數將配置文件解析函數添加到sections中，會調用section_parser()函數解析。

整個HAProxy配置文件解析完成後，listener相關也已初始化完畢，如下簡單梳理一下幾個accept方法的設計邏輯：
* listener_accept(): 對應不同協議的accept()方法，IPv4對應該方法，負責接收並新建TCP連接；該函數中通過accept()系統調用建立鏈接後，接著會觸發listener自己的accept方法也就是session_accept_fd()；
* session_accept_fd(): 負責創建+初始化session，並在stream_new()函數中調用frontend的accept方法frontend_accept()；
* frontend_accept(): 該函數主要負責session前端的TCP連接的初始化，包括socket設置，log設置，以及session部分成員的初始化。
接下來 TCP 新建連接處理過程，基本上都是與這三個函數相關。


建鏈的處理都是通過_do_poll()處理，只是不同的多路複用方式實現不同，而入口的函數名相同，位於不同的文件中。

session_accept()


tcp_bind_listener()

stats_parse_global()
 |-str2listener() 監聽地址解析

上述客戶端的處理在cli.c文件中，
cli_io_handler()
cli_parse_request()

uxst_bind_listener() 報錯


### tips

#### 多路複用
http://www.cnblogs.com/Anker/p/3263780.html
HAProxy支持多種多路複用機制，包括了select、poll、epoll、kqueue等，簡單介紹下ev_epoll.c中實現的epoll機制。


int fd_nbupdt = 0;             // number of updates in the list
unsigned int *fd_updt = NULL;  // FD updates list
這兩個全局變量用來記錄狀態需要更新的 fd 的數量及具體的 fd ，在 _do_poll() 函數中會根據這些信息修改對應 fd 的 epoll 設置。

/* info about one given fd */
struct fdtab {
    int (*iocb)(int fd);                 /* I/O handler, returns FD_WAIT_* */
    void *owner;                         /* the connection or listener associated with this fd, NULL if closed */
    unsigned int  cache;                 /* position+1 in the FD cache. 0=not in cache. */
    unsigned char state;                 /* FD state for read and write directions (2*3 bits) */
    unsigned char ev;                    /* event seen in return of poll() : FD_POLL_* */
    unsigned char new:1;                 /* 1 if this fd has just been created */
    unsigned char updated:1;             /* 1 if this fd is already in the update list */
    unsigned char linger_risk:1;         /* 1 if we must kill lingering before closing */
    unsigned char cloned:1;              /* 1 if a cloned socket, requires EPOLL_CTL_DEL on close */
};
struct fdtab *fdtab = NULL;     /* array of all the file descriptors */

全局變量 *fdtab 記錄了 HAProxy 中所有與 fd 相關的信息，數組的 index 是 fd 對應的值，用於快速定位到某個 fd 的信息。

在 fd.h 頭文件中定義了一些操作 fd event 的函數，主要是fd_(want|stop)_(recv|send)類似的函數，用於設置 fd 啟動、停止接收以及發送；簡單來說就是修改fdtabl[fd]中的狀態值，如果有更新則添加到fd_updt[]數組中。

接下來，看下_do_poll()函數中的主要處理過程，大致可以分為三部分：
1. 從fd_updt[]獲取更新的fd列表，判斷各個fd event的變化情況，並通過epoll_ctl()進行設置；
2. 計算epoll_wait需要等待的最大超時時間，並調用epoll_wait()獲取當前活動的fd；
3. 逐一處理所有有 IO 事件的 fd。

        fdtab[fd].updated = 0; // 只有被修改過(更新、新建)且有owner才會被使用
        eo = fdtab[fd].state;  // 關於狀態的概念、轉換等信息可以查看src/fd.c文件中的註釋
        en = fd_compute_new_polled_status(eo);  // 狀態轉換可以查看fd.c中的註釋
        if ((eo ^ en) & FD_EV_POLLED_RW) { // 1. 按位異或，如果有變更則通過epoll_ctl()修改

#### 位或 VS. 邏輯或
在HAProxy中有一個函數用來判斷buffer是否為空的代碼：
static inline int buffer_not_empty(const struct buffer *buf)
{
 return buf->i | buf->o;
}
其中 buf->i 和 buf->o 都是 unsigned int 類型，這兩個成員任何一個非零就代表著 buffer 不空，正常來說應該採用 logical-OR 但是這裡採用的卻是 bitwise-OR ，其原因是兩者的功能相同，而前者生成的機器代碼更少效率更高。
static inline int buffer_not_empty(const struct buffer *buf)
{
 return buf->i || buf->o;
}

#### 實現文件複製

最簡單的方式就是申請一份內存buffer，通過系統調用 read() 讀取源文件的一段數據到 buffer，然後將此 buffer 再通過 write() 系統調用寫到目標文件，示例代碼如下：

char buf[max_read];
off_t size = stat_buf.st_size, off_in=0;
while ( off_in < size ) {
 int len = size - off_in > max_read ? max_read : size - off_in;
 len = read(f_in, buf, len);
 off_in += len;
 write(f_out, buf, len);
}

還有一種方式是通過 mmap() 系統調用實現，示例代碼如下：

size_t filesize = stat_buf.st_size;
source = mmap(0, filesize, PROT_READ, MAP_SHARED, f_in, 0);
target = mmap(0, filesize, PROT_WRITE, MAP_SHARED, f_out, 0);
memcpy(target, source, filesize);

因為 mmap() 不需要內核態和用戶態的內存拷貝，從而效率大大提高。另外一種是通過 splice() 系統調用實現，這是 Linux 2.6.17 新加入的系統調用，用於在兩個文件間移動數據，而無需內核態和用戶態的內存拷貝，但需要藉助管道 (pipe) 實現。

大概原理就是通過pipe buffer實現一組內核內存頁（pages of kernel memory）的引用計數指針（reference-counted pointers），數據拷貝過程中並不真正拷貝數據，而是創建一個新的指向內存頁的指針。也就是說拷貝過程實質是指針的拷貝。示例代碼如下：

int pipefd[2];
pipe( pipefd );
int max_read = 4096;
off_t size = stat_buf.st_size;
while ( off_in < size ) {
 int len = size - off_in > max_read ? max_read : size - off_in;
 len = splice(f_in, &off_in, pipefd[1], NULL, len, SPLICE_F_MORE |SPLICE_F_MOVE);
 splice(pipefd[0], NULL, f_out, &off_out, len, SPLICE_F_MORE |SPLICE_F_MOVE);
}

使用splice一定要注意，因為其藉助管道實現，而管道有眾所周知的空間限制問題，超過了限制就會hang住，所以每次寫入管道的數據量好嚴格控制，保守的建議值是一個內存頁大小，即4k。另外，off_in和off_out傳遞的是指針，其值splice會做一定變動，使用時應注意。
http://abcdxyzk.github.io/blog/2015/05/07/kernel-mm-splice/
https://www.ibm.com/developerworks/cn/linux/l-cn-zerocopy2/

http://blog.chinaunix.net/uid/10167808/list/1.html?cid=178438
http://tech.uc.cn/?p=1523
http://blog.csdn.net/dylan_csdn/article/details/51261421
https://www.linux.com/blog/how-analyze-haproxy-logs
http://fangpeishi.com/haproxy_best_practice_notes.html
http://abcdxyzk.github.io/blog/2015/07/29/tools-haproxy_src1/
http://tech.uc.cn/?tag=haproxy
http://tech.uc.cn/?p=1738#more-1738
http://www.ttlsa.com/linux/haproxy-study-tutorial/

https://github.com/1u4nx/Exploit-Exercises-Nebula

minconn, maxconn and fullconn參數的對比
http://www.serverphorums.com/read.php?10,292171
運維0宕機時間，使用TC+IPtables技術
https://engineeringblog.yelp.com/2015/04/true-zero-downtime-haproxy-reloads.html
性能調優
https://medium.freecodecamp.com/how-we-fine-tuned-haproxy-to-achieve-2-000-000-concurrent-ssl-connections-d017e61a4d27
常用Python工具
https://github.com/feurix/hatop/blob/master/bin/hatop
https://github.com/unixsurfer/haproxyadmin
https://github.com/breakwang/zabbix-external-script-haproxy_monitor/blob/master/haproxy_monitor.py
HAProxy源碼介紹
http://blog.chinaunix.net/uid/10167808/list/1.html?cid=178438
用HAProxy來檢測MySQL複製的延遲的教程
http://www.jb51.net/article/64615.htm
官方blog
https://blog.haproxy.com/
haproxy的stick-table實際應用探索
http://blog.sina.com.cn/s/blog_704836f40102w243.html
HAProxy的獨門武器：ebtree
http://mp.weixin.qq.com/s?__biz=MzA5NzA0ODA1Nw==&mid=2650513447&idx=1&sn=a44562e281beef32e6e2f59d2d887b9c&scene=5&srcid=05258uXjxyX8evtbriDv0wYn#rd

#### show pools

對應源碼中 dump_pools_to_trash() 函數，用於顯示當前內存池的使用情況。

```
Dumping pools usage. Use SIGQUIT to flush them.
  - Pool pipe (32 bytes) : 5 allocated (160 bytes), 5 used, 3 users [SHARED]
  - Pool capture (64 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool channel (80 bytes) : 2 allocated (160 bytes), 2 used, 1 users [SHARED]
  - Pool task (112 bytes) : 11 allocated (1232 bytes), 11 used, 1 users [SHARED]
  - Pool uniqueid (128 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool connection (320 bytes) : 2 allocated (640 bytes), 2 used, 1 users [SHARED]
  - Pool hdr_idx (416 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool session (864 bytes) : 1 allocated (864 bytes), 1 used, 1 users [SHARED]
  - Pool requri (1024 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool buffer (16416 bytes) : 2 allocated (32832 bytes), 2 used, 1 users [SHARED]
Total: 10 pools, 35888 bytes allocated, 35888 used.
```

#### show stat

對應到源碼中的 cli_io_handler_dump_stat() 函數，這裡實際會打印一個 proxy 對應的信息，frontend 和 backend 略有區別，而 backend 和 server 基本相同。

```
>>>>>frontend<<<<<
Session:
   current(scur)               px->feconn                   session_accept_fd()     前端建立的會話數，可以用於評估當前資源使用量，是否達到了maxconn；如果迅速上漲，說明backend異常或者DDoS攻擊。
   max(smax)                   px->fe_counters.conn_max     session_accept_fd()     計數清零以來，所建立連接的最大值。
   limit(slim)                 px->maxconn                                          在配置文件中設置的最大值
   total(stot)                 px->fe_counters.cum_sess     proxy_inc_fe_sess_ctr() 已處理的請求數，一直遞增，在session_count_new()函數中調用
Session Rate:                 
   Current Connection Rate     px->fe_conn_per_sec          proxy_inc_fe_conn_ctr()
   Current Session Rate(rate)  px->fe_sess_per_sec          proxy_inc_fe_sess_ctr()
   Max connection rate         px->fe_counters.cps_max      proxy_inc_fe_conn_ctr()
   Max session rate            px->fe_counters.sps_max      proxy_inc_fe_sess_ctr()
   (rate_lim)                  px->fe_sps_lim
   (rate_max)                  px->fe_counters.sps_max
Bytes:                        
   In(in)                      px->fe_counters.bytes_in
   Out(out)                    px->fe_counters.bytes_out
   Out(bypassed)               px->fe_counters.comp_byp
Denied:                       
   Req(dreq)                   px->fe_counters.denied_req  tcp_exec_action_silent_drop() 處於安全考慮拒絕的請求數，一般是根據規則丟棄
   Resp(dresp)                 px->fe_counters.denied_resp 同上，拒絕的響應數
Errors:                       
   Req(ereq)                   px->fe_counters.failed_req  在HTTP模式中會有失敗
>>>>>backend<<<<<
Queue:
   current(qcur)               px->nbpend                   pendconn_add()          默認會直接添加到各個後端Server服務器的隊列中，如果已滿則添加到be隊列中。
   max(qmax)                   px->be_counters.nbpend_max   pendconn_add()          計數清零以來，所建立會話的最大值。
Session:
   current(scur)               px->beconn                   stream_set_backend()    默認會直接添加到各個後端Server服務器的隊列中，如果已滿則添加到be隊列中。
   max(smax)                   px->be_counters.nbpend_max   pendconn_add()          計數清零以來，所建立會話的最大值。
   limit(slim)                 px->fullconn                                         在配置文件中設置，可以通過fullconn設置，否則計算所有proxy的累加值
   total(stot)                 px->be_counters.cum_conn     proxy_inc_be_ctr()      已處理的請求數，一直遞增
Session Rate: xxx                
   Current Connection Rate     px->fe_conn_per_sec          proxy_inc_fe_conn_ctr()
   Current Session Rate(rate)  px->fe_sess_per_sec          proxy_inc_fe_sess_ctr()
   Max connection rate         px->fe_counters.cps_max      proxy_inc_fe_conn_ctr()
   Max session rate            px->fe_counters.sps_max      proxy_inc_fe_sess_ctr()
   (rate_lim)                  px->fe_sps_lim
   (rate_max)                  px->fe_counters.sps_max
Bytes:                        
   In(in)                      px->be_counters.bytes_in
   Out(out)                    px->be_counters.bytes_out
Denied:                       
   Req(dreq)                   px->be_counters.denied_req  處於安全考慮拒絕的請求數，一般是根據規則丟棄
   Resp(dresp)                 px->be_counters.denied_resp 同上，拒絕的響應數
Errors:                       
   Conn(econ)                  px->be_counters.failed_conns                         場景的是隊列滿、沒有服務器等，導致請求發送失敗
   Resq(eresp)                 px->be_counters.failed_resp                          只在HTTP模式中存在
Warnings:
   Retr(wretr)                 px->be_counters.retries                              鏈接後端失敗的次數，可以嘗試多次
   Redis(wredis)               px->be_counters.redispatches                         嘗試多次後仍然失敗，則發送給其他服務器，注意需要配置允許redispatch選項
Server:
   Act(act)                    px->srv_act                                          當前有多少服務是正常的
   Bck(bck)                    px->srv_bck                                          backup服務器可用數
```

#### 其它

除此之外，還可以通過 ```show errors```、```show sess [id]```、```show table [id]```、```show acl [id]```、```show map [id]``` 命令查看狀態。

Connection和Session的區別

在建立連接之後，需要一些初始化操作，只有正常完成這些操作之後，才會作為會話。

tcp_connect_server()
 |-conn_ctrl_init()

listener_accept()
 |-accept() 調用系統API建立連接

session_accept_fd
 |-proxy_inc_fe_conn_ctr() 新增connect統計信息
 |-session_count_new() 新增session統計信息
 | |-proxy_inc_fe_sess_ctr() 統計信息更新

HAProxy當前的任務信息保存在static struct eb_root rqueue指定的隊列中，這是一個ebtree結構體，每個任務通過struct task結構體表示。

很多的狀態信息處理在src/stats.c文件中，例如listener的監控數據處理在stats_fill_li_stats()函數中；而一些宏則通過enum stat_field定義，例如ST_F_PXNAME(pxname)值。



### 配置文件解析
cfgparse_init() 會在main函數啟動前添加默認解析配置項

  clear counters : clear max statistics counters (add 'all' for all counters)
  clear table    : remove an entry from a table
  help           : this message
  prompt         : toggle interactive mode with prompt
  quit           : disconnect
 
  show info      : report information about the running process
  show pools     : report information about the memory pools usage
  show stat      : report counters for each proxy and server
  show errors    : report last request and response errors for each proxy
  show sess [id] : report the list of current sessions or dump this session
  show table [id]: report table usage stats or dump this table's contents
  get weight     : report a server's current weight
  set weight     : change a server's weight
  set server     : change a server's state or weight
  set table [id] : update or create a table entry's data
  set timeout    : change a timeout setting
  set maxconn    : change a maxconn setting
  set rate-limit : change a rate limiting value
  disable        : put a server or frontend in maintenance mode
  enable         : re-enable a server or frontend which is in maintenance mode
  shutdown       : kill a session or a frontend (eg:to release listening ports)
  show acl [id]  : report available acls or dump an acl's contents
  get acl        : reports the patterns matching a sample for an ACL
  add acl        : add acl entry
  del acl        : delete acl entry
  clear acl <id> : clear the content of this acl
  show map [id]  : report available maps or dump a map's contents
  get map        : reports the keys and values matching a sample for a map
  set map        : modify map entry
  add map        : add map entry
  del map        : delete map entry
  clear map <id> : clear the content of this map
  set ssl <stmt> : set statement for ssl

TCP(Layer 4)和HTTP(Layer 7)負載均衡，反向代理。HAProxy單點，可以使用Pacemaker或者Keepalived提供高可用

ACLs
  Extract some information, make decision
  Block request, select backend, rewrite headers, etc.
TCP mode (Layer 4)
  Basic TCP services, SSL passthrough
  Some ACLs available
HTTP mode (Layer 7)
  HTTP header inspection ACLs
  Persistence with cookie insertion
Stick Tables
  Record information in table, eg. source address
  Check table for new connections, select backend

後臺服務器只能看到HAProxy，HTTP可以通過添加X-Forwarded-For頭部信息。

curl --silent "http://192.144.35.35:85/status;csv"

info_field_names[] <==> show info
stat_field_names[] <==> show stat

http_stats_io_handler() 內置HTTP服務器處理
 |-stats_dump_stat_to_buffer()     // same as above, but used for CSV or HTML
   |-stats_dump_csv_header()      // emits the CSV headers (same as above)
   |-stats_dump_html_head()       // emits the HTML headers
   |-stats_dump_html_info()       // emits the equivalent of "show info" at the top
   |-stats_dump_proxy_to_buffer() // same as above, valid for CSV and HTML
   | |-stats_dump_html_px_hdr()
   | |-stats_dump_fe_stats() frontend statistics
   | | |-stats_fill_fe_stats()
   | |-stats_dump_li_stats() listener statistics
   | | |-stats_fill_li_stats() 將數據保存在靜態變量stats中
   | |-stats_dump_sv_stats() server statistics
   | | |-stats_fill_sv_stats()
   | |-stats_dump_be_stats() backend statistics
   | |-stats_dump_html_px_end()
   |-stats_dump_html_end()       // emits HTML trailer

 * cli_io_handler()
 *     -> stats_dump_sess_to_buffer()     // "show sess"
 *     -> stats_dump_errors_to_buffer()   // "show errors"
 *     -> stats_dump_info_to_buffer()     // "show info"
 *     -> stats_dump_backend_to_buffer()  // "show backend"
 *     -> stats_dump_servers_state_to_buffer() // "show servers state [<backend name>]"
 *     -> stats_dump_stat_to_buffer()     // "show stat"
 *        -> stats_dump_resolvers_to_buffer() // "show stat resolver <id>"
 *        -> stats_dump_csv_header()
 *        -> stats_dump_proxy_to_buffer()
 *           -> stats_dump_fe_stats()
 *           -> stats_dump_li_stats()
 *           -> stats_dump_sv_stats()
 *           -> stats_dump_be_stats()
 *
 * http_stats_io_handler()
 *     -> stats_dump_stat_to_buffer()     // same as above, but used for CSV or HTML
 *        -> stats_dump_csv_header()      // emits the CSV headers (same as above)
 *        -> stats_dump_html_head()       // emits the HTML headers
 *        -> stats_dump_html_info()       // emits the equivalent of "show info" at the top
 *        -> stats_dump_proxy_to_buffer() // same as above, valid for CSV and HTML
 *           -> stats_dump_html_px_hdr()
 *           -> stats_dump_fe_stats()
 *           -> stats_dump_li_stats()
 *           -> stats_dump_sv_stats()
 *           -> stats_dump_be_stats()
 *           -> stats_dump_html_px_end()
 *        -> stats_dump_html_end()       // emits HTML trailer

$ nc -U /var/run/haproxy.sock
$ prompt
> show info
cli_io_handler() Unix Socket的命令處理函數接口，這裡的處理通過狀態機完成
 |-bo_getline() 讀取命令行
 |-cli_parse_request() 調用定義的回調函數解析命令，並複製給appctx變量
 | |-cli_find_kw() 查找定義的結構體，主要是各種回調函數
 |-appctx->io_handler() 調用各個插件定義的處理函數

struct cli_kw_list {
  struct list list;
  struct cli_kw kw[VAR_ARRAY];
};


show info; 結構體定義<->enum info_field@stats.h  回調函數<->cli_io_handler_dump_info()@stat.c
cli_io_handler_dump_info()
 |-stats_dump_info_to_buffer()
   |-stats_fill_info() 填充數值，各個HAProxy監控信息與內部變量的對應關係

show pools; 通過pools
cli_io_handler_dump_pools()
 |-dump_pools_to_trash()

echo "show stat" | nc -U /var/run/haproxy.sock
cli_io_handler_dump_stat()
 |-stats_dump_stat_to_buffer() HTML和CLI都會調用該函數
   |-stats_dump_csv_header() 打印CSV頭信息
   |-stats_dump_proxy_to_buffer() 會循環調用打印proxy信息，使用csv格式
     |-stats_dump_fe_stats() frontend statistics
     | |-stats_fill_fe_stats()
     |-stats_dump_li_stats() listener statistics
     | |-stats_fill_li_stats() 將數據保存在靜態變量stats中
     |-stats_dump_sv_stats() server statistics
     | |-stats_fill_sv_stats()
     |-stats_dump_be_stats() backend statistics
  | |-stats_fill_be_stats()
     |-stats_dump_html_px_end()
  
echo "show sess" | nc -U /var/run/haproxy.sock
cli_io_handler_dump_sess()

echo "show table" | nc -U /var/run/haproxy.sock
cli_io_handler_table()

echo "show acl" | nc -U /var/run/haproxy.sock
cli_parse_show_map()

echo "show map" | nc -U /var/run/haproxy.sock
cli_parse_show_map()

stick table
一般來說可以通過cookies和hash來映射front+back end，所以就有了stick table。

客戶端支持的命令通過struct cli_kw_list結構體定義，例如：show pools(memory.c)、show stats(stats.c)等，該結構體中包括瞭解析函數、處理函數的定義；而且利用GCC的__attribute__((constructor))特性，在執行main()函數之前調用cli_register_kw()函數執行，而該函數實際就是將其添加到cli_keywords.list中。

/usr/bin/python /usr/local/bin/denyhosts.py --daemon --config=/usr/share/denyhosts/denyhosts.cfg

 
### 服務器狀態檢測
用於檢查後臺的服務器是否正常，可以根據不同服務類型進行檢測。
check: httpchk、smtpchk、mysql-check、pgsql-check、ssl-hello-chk
  默認認為服務器是正常的，通過該參數表示需要進行檢測確認服務器狀態；
rise:
  多少次檢測正常後認為該服務器是正常的；
fall:
  多少次檢測失敗後認為該服務器異常；
inter:
  設置檢測的時間間隔，另外還可以定義fastinter、downinter參數，根據不同的狀態設置檢測時間間隔；



timeout check 通過min("timeout connect", "inter")作為與後臺服務器建立連接的超時時間；該參數對應了建立鏈接後讀取數據的超時時間；如果該參數沒有設置，那麼就直接使用inter作為(connect+read)的超時時間；一般來說檢查的時間要小於正常業務處理時間，所以該值要小於timeout server指定的超時時間。

timeout client                            X          X         X         -
timeout client-fin                        X          X         X         -
timeout connect                           X          -         X         X
timeout http-keep-alive                   X          X         X         X
timeout http-request                      X          X         X         X
timeout queue                             X          -         X         X
timeout server                            X          -         X         X
timeout server-fin                        X          -         X         X
timeout tarpit                            X          X         X         X
timeout tunnel                            X          -         X         X

haproxy 負責處理請求的核心數據結構是 struct session，本文不對該數據結構進行分析。

從業務的處理的角度，簡單介紹一下對 session 的理解：

    haproxy 每接收到 client 的一個連接，便會創建一個 session 結構，該結構一直伴隨著連接的處理，直至連接被關閉，session 才會被釋放
    haproxy 其他的數據結構，大多會通過引用的方式和 session 進行關聯
    一個業務 session 上會存在兩個 TCP 連接，一個是 client 到 haproxy，一個是 haproxy 到後端 server。

此外，一個 session，通常還要對應一個 task，haproxy 最終用來做調度的是通過 task。
struct proxy {
  struct lbprm lbprm;  // 負載均衡參數
};
-->

## 源碼解析

`HAProxy is a single-threaded, event-driven, non-blocking daemon.`

啟動非常簡單，只需要一個可執行文件+配置文件即可，啟動之後基本處理如下的三個操作：

{% highlight text %}
1. 處理鏈接；
   1.1 從frontend指定的監聽端口創建鏈接；
   1.2 應用frontend指定的規則，包括了阻塞、修改頭部信息、統計等；
   1.3 將鏈接傳遞給backend指定的服務器；
   1.4 應用backend指定的規則；
   1.5 根據策略決定將鏈接發送給那個服務器；
   1.6 對響應數據應用backend指定的規則；
   1.7 對響應數據應用frontend指定的規則；
   1.8 發送日誌；
   1.9 如果是HTTP那麼會等待新請求，否則就關閉鏈接；
2. 週期性檢查服務器的狀態；
3. 與其它HAProxy節點交換數據；
{% endhighlight %}

然後，直接看下源碼解析過程。

{% highlight text %}
main()
 |-init()                                  ← 所有的初始化操作，包括各個模塊初始化、命令行解析等
 | |-cfgfiles_expand_directories()         ← 處理配置文件，參數解析會將配置文件保存在cfg_cfgfiles
 | |-init_default_instance()               ← 初始化默認配置
 | |-readcfgfile()                         ← 讀取配置文件，並調用sections->section_parser()對應的函數
 | | |-cfg_parse_listen()                  ← 對於frontend、backend、listen段的參數解析驗證
 | |   |-str2listener()
 | |     |-l->frontend=curproxy
 | |     |-tcpv4_add_listener()            ← 添加到proto_tcpv4對象中的鏈表，真正監聽會在proto_XXX.c文件中
 | |       |-listener->proto=&proto_tcpv4  ← 會設置該變量，後續的接收鏈接也就對應了accept變量
 | |-check_config_validity()               ← 配置文件格式校驗
 | | |-listener->frontend=curproxy         ← 在上面解析，實際上curporxy->accept=frontend_accept
 | | |-listener->accept=session_accept_fd
 | | |-listener->handler=process_stream
 | | |-根據不同的後端服務器選擇算法選擇
 | |
 | | <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<解析完配置文件>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 | |
 | |-start_checks()                        ← 執行後端檢查任務
 | | |-start_check_task()
 | |   |-process_chk()
 | |     |-process_chk_conn()
 | |       |-connect_conn_chk()
 | |         |-event_srv_chk_r()           ← 健康檢查，在check_conn_cb變量中定義，通過conn_attach()綁定
 | |           |-tcpcheck_main()           ← 如果是TCP檢查PR_O2_TCPCHK_CHK
 | |
 | |-init_pollers()                        ← 選擇多路複用方法，也就是設置cur_poller
 |   |-calloc()                            ← 分配資源fd_cache+fd_updt，其大小是global.maxsock
 |   |-bp->init()                          ← 調用各個可用poll方法的初始化函數，選擇第一個可用方法
 |   |-memcpy()                            ← 複製到cur_poller全局變量中
 |
 |-start_proxies()                         ← 開始啟動，調用各協議bind接口，對TCPv4就是tcp_bind_listener()
 |-protocol_bind_all()
 |-protocol_enable_all()                   ← 啟動各個協議，例如ipv4/ipv6/unix等
 |
 | <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<主循環處理流程>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 |
 |-run_poll_loop()
 | |-tv_update_date()
 | | ###WHILE###BEGIN
 | |-process_runnable_tasks()              ← 調用可運行任務
 | | |-process_stream()                    ← 一般是調用該函數，也可執行struct task.process中自定義函數
 | |   |
 | |   | <<PHASE:解析請求>>                ← 會根據定義的各種規則選擇後端
 | |   |-process_switching_rules()         ← 除了默認使用的後端服務器之外，會根據規則再次選擇<<RULES>>
 | |   | |-stream_set_backend()            ← 選擇該後端，<<STAT:be->beconn>>
 | |   |   |-proxy_inc_be_ctr()            ← 後端統計值的更新
 | |   |-http_process_tarpit()
 | |   |
 | |   | <<PHASE:解析響應>>
 | |   |-process_store_rules()             ← 處理規則
 | |   |
 | |   | <<PAHSE:正式處理請求>>
 | |   |-sess_prepare_conn_req()           ← 選擇後端的服務器
 | |     |-srv_redispatch_connect()
 | |       |-assign_server_and_queue()     ← 選擇後端的服務器，並添加到隊列中
 | |         |-assign_server()             ← 根據負載均衡配置選擇後端服務器
 | |         | |-get_server_sh()
 | |         |   |-chash_get_server_hash() ← 使用一致性hash算法
 | |         |-sess_change_server()
 | |         |-pendconn_add()              ← 如果後端服務器已滿，則添加到proxy隊列中<<STAT:px->nbpend++>>
 | |
 | |-signal_process_queue()                ← 信號隊列，如果捕獲了信號則處理
 | |-wake_expired_tasks()                  ← 超時任務
 | |-cur_poller.poll()                     ← 不同平臺的多路複用技術
 | | |-_do_poll()                          ← 以ev_epoll.c中的epoll為例
 | |   |-epoll_wait()
 | |   |-fd_may_recv()
 | |   | |-fd_update_cache()               ← 在處理函數中只添加到cache中，真正的處理過程在後面
 | |   |-fd_may_send()
 | |     |-fd_update_cache()
 | |
 | |-fd_process_cached_events()            ← 真正處理epoll()中觸發的事件
 | | |-fdtab[fd].iocb(fd)                  ← 調用註冊的回調函數，一般是conn_fd_handler()
 | |   |-conn->data->recv(conn)            ← 實際調用si_conn_recv_cb()函數，也就是負責接收的函數
 | |   | |-conn->xprt->rcv_pipe()          ← 如果不啟用SSL則調用raw_sock_to_pipe()，否則調用下面的buff函數
 | |   | |-conn->xprt->rcv_buf()           ← 不啟用SSL則調用raw_sock_to_buf()，否則調用ssl_sock_to_buf()
 | |   |-conn->data->recv(conn)            ← 實際調用si_conn_send_cb()函數，也就是負責發送的函數
 | |
 | |-applet_run_active()
 | | ###WHILE###END
 |-deinit() 清理操作
{% endhighlight %}

## 參考

官方網站為 [www.haproxy.org](http://www.haproxy.org/)，源碼可以直接從 [https://github.com/haproxy/haproxy](https://github.com/haproxy/haproxy) 下載，很多幫助文檔可以直接查看源碼中的 doc 目錄。

關於幫助文檔也可以查看 [haproxy-dconv](http://cbonte.github.io/haproxy-dconv/)，另外有個監控工具 [HATop](http://feurix.org/projects/hatop/) 。

[MySQL Load Balancing with HAProxy - Tutorial](http://severalnines.com/tutorials/mysql-load-balancing-haproxy-tutorial) 同時包含了一個不錯的終端錄製工具 [asciinema.org](https://asciinema.org/) 。


<!--
http://jaminzhang.github.io/lb/HAProxy-Get-Started/
-->


{% highlight text %}
{% endhighlight %}
