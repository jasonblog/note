---
title: Nginx 日誌解析
layout: post
comments: true
language: chinese
category: [webserver, network]
keywords: hello world,示例,sample,markdown
description: 簡單記錄一下一些與 Markdown 相關的內容，包括了一些使用模版。
---

在 Nginx 的統計排錯過程中，通常要依賴日誌，Nginx 有非常靈活的日誌記錄模式，每個級別的配置可以有各自獨立的訪問日誌。

<!-- more -->

## 簡介

其中與日誌模塊相關的參數如下，在 CentOS 中，日誌默認保存在 `/usr/local/nginx/logs` 目錄下。

### 訪問日誌

實際上這裡的配置項是通過 [ngx_http_log_module](http://nginx.org/en/docs/http/ngx_http_log_module.html) 實現的，記錄了哪些用戶，哪些頁面以及用戶瀏覽器、IP 和其他的訪問信息。

常見的配置項包括了 `access_log`、`log_format`、`open_log_file_cache` 。

其中 `log_format` 記錄了日誌的記錄格式；`access_log` 用於設置保存的路徑、是否壓縮、緩衝的大小等信息；而最後一個其實是對寫入日誌的優化。

#### access_log

可以將 path 設置為 syslog 將日誌保存到系統日誌中，不過其默認是保存到單獨的日誌文件中，默認的配置如下：

{% highlight text %}
----- 語法格式如下
access_log path format gzip[=level] [buffer=size] [flush=time];
access_log syslog:server=address[,parameter=value] [format];
access_log off;

----- 默認配置為
access_log logs/access.log combined;
{% endhighlight %}

其中 gzip 表示壓縮等級；buffer 用來設置內存緩存區大小；flush 為保存在緩存區中的最長時間。

#### log_format

其配置格式為：

{% highlight text %}
log_format name string;
{% endhighlight %}

其中，name 表示該配置格式的名稱；string 表示定義格式的內容。其中該模塊中有一個默認的無需配置名稱為 combined 的日誌格式，相當於 apache 的 combined 日誌格式。

{% highlight text %}
log_format combined '$remote_addr - $remote_user  [$time_local]  '
                    ' "$request"  $status  $body_bytes_sent  '
                    ' "$http_referer"  "$http_user_agent" ';

{% endhighlight %}

如果 Nginx 位於負載均衡器 squid，或者 Nginx 反向代理之後，那麼 web 服務器中的 $remote_addr 實際上獲取的是反向代理的 IP 地址，將無法直接獲取到客戶端真實的 IP 地址。

為此，反向代理服務器在轉發請求的 http 頭信息中，可以增加 X-Forwarded-For 信息，用來記錄客戶端 IP 地址和客戶端請求的服務器地址。

{% highlight text %}
log_format  porxy  '$http_x_forwarded_for - $remote_user  [$time_local]  '
                   ' "$request"  $status $body_bytes_sent '
                   ' "$http_referer"  "$http_user_agent" ';
{% endhighlight %}

其中對應各個參數分別代表的值可以參考 [ngx_http_log_module](http://nginx.org/en/docs/http/ngx_http_log_module.html) 中的介紹。

#### open_log_file_cache

對於每一條日誌記錄，都將是先打開文件，再寫入日誌，然後關閉；這樣就會導致文件被頻繁的打開關閉，可以使用 open_log_file_cache 設置日誌文件緩存。

{% highlight text %}
open_log_file_cache max=N [inactive=time] [min_uses=N] [valid=time];
open_log_file_cache off;

----- 示例
open_log_file_cache max=1000 inactive=20s valid=1m min_uses=2;
{% endhighlight %}

### 錯誤日誌

`error_log` 則是記錄服務器錯誤日誌。


## 源碼解析

在 nginx 中，會通過 `ngx_errlog_module` 模塊用於處理 nginx 日誌信息，這也是 nginx 的 core 模塊之一。

在配置文件中，會通過解析 `error_log` 配置項將不同等級的日誌信息輸出到指定的文件中，當遇到該配置項是，就調用 errlog 模塊的 `ngx_error_log()` 來解析。

{% highlight text %}
ngx_error_log()
{% endhighlight %}

上述函數會將 `error_log` 配置項的值保存在 `ngx_cycle->new_log` 成員中，當配置文件中有多條 `error_log` 配置項生效時，會通過 `ngx_cycle->new_log.next` 成員將它們組織起來。

如果配置文件中沒有 `error_log` 配置項，在配置文件解析完後調用 `errlog` 模塊的 `ngx_log_open_default()` 將日誌等級默認置為 `NGX_LOG_ERR`，日誌文件設置為 `NGX_ERROR_LOG_PATH` (編譯時指定)。

<!--
由此可看無論配置文件中是否有error_log配置項始終會有日誌輸出，nginx中禁止輸出日誌唯一的辦法：
將error_log配置項的輸出日誌文件名指定為/dev/null。
-->

### 日誌結構體

{% highlight c %}
struct ngx_log_s {
    ngx_uint_t           log_level;          // 日誌等級
    ngx_open_file_t     *file;               // 記錄日誌文件信息
    ngx_atomic_uint_t    connection;
    time_t               disk_full_time;
    ngx_log_handler_pt   handler;
    void                *data;
    ngx_log_writer_pt    writer;
    void                *wdata;
    char                *action;
    ngx_log_t           *next;
};
{% endhighlight %}

一般通過 `ngx_log_error()` 函數輸出日誌信息。

{% highlight c %}
void ngx_cdecl ngx_log_error(ngx_uint_t level,
	ngx_log_t *log, ngx_err_t err, const char *fmt, ...)
{
    va_list  args;

    if (log->log_level >= level) { // 根據日誌等級判斷是否需要輸出
        va_start(args, fmt);
        ngx_log_error_core(level, log, err, fmt, args);
        va_end(args);
    }
}

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, va_list args)
{
	/* ... ... */

	/* 格式化日誌內容 */
	last = errstr + NGX_MAX_ERROR_STR;
	p = ngx_cpymem(errstr, ngx_cached_err_log_time.data,
		       ngx_cached_err_log_time.len);
	p = ngx_slprintf(p, last, " [%V] ", &err_levels[level]);

	/* 寫入日誌 */
	while(log) {
		if (log->log_level < level && !debug_connection) {
			break;
		}

		if (log->writer) {
			log->writer(log, level, errstr, p - errstr);
			goto next;
		}

		if (ngx_time() == log->disk_full_time) {

			/*
			 * on FreeBSD writing to a full filesystem with enabled softupdates
			 * may block process for much longer time than writing to non-full
			 * filesystem, so we skip writing to a log for one second
			 */

			goto next;
		}

		n = ngx_write_fd(log->file->fd, errstr, p - errstr);

		if (n == -1 && ngx_errno == NGX_ENOSPC) {
			log->disk_full_time = ngx_time();
		}

		if (log->file->fd == ngx_stderr) {
			wrote_stderr = 1;
		}

next:

		log = log->next;
	}

}
{% endhighlight %}


## 原子寫入

實際上，這裡會存在一個疑問，多進程的 nginx 模型是怎麼保證多個進程同時寫入一個文件不發生數據交錯呢？

一般來說，主要有如下的幾種解決方案：

* 通過鎖保證只有一個進程寫入，其他進程等待，但是這種情況效率太低。

* 寫日誌前查看鎖狀態，如果已經鎖定，則寫入進程自己的緩衝區中，等待下次調用時同步緩衝區，這樣做的好處是無需阻塞，提高了效率，但是就無法做到 log 的實時了，這樣做工程中也是絕對無法接受的，一旦發生問題，將無法保證 log 是否已經被寫入，因此很難定位。

* 一個進程專門負責寫 log，其他進程通過套接字或者管道將 log 內容發送給他，持續阻塞在 epoll_wait 上，直到收到信息，立即寫入，但是眾所周知，nginx 是調用同一個函數啟動所有進程的，並沒有專門調用函數啟動所謂的 log 進程，除了 master 和 worker，nginx 也確實沒有 log 進程存在。

* 進程啟動後，全部去競爭某個鎖，競爭到該鎖的 worker 執行 log worker 的代碼，其餘的 worker 繼續運行相應程序，這個方案看上去是一個不錯的方案，如果是單 worker 的話，那麼就無需去使用該鎖即可。

閱讀源碼可以發現 nginx 只有一把互斥鎖，即用來避免驚群現象的 `ngx_accept_mutex` 鎖，其餘地方完全沒有用到鎖機制，這麼做原因很簡單，在工程化的代碼中，盲目使用鎖會造成性能的下降，這是不可以接受的。

如上，最終寫入日誌的是通過 `ngx_log_error_core()` 函數，這個函數正是用來打印錯誤日誌的，而最終調用的實際上就是：

{% highlight c %}
static ngx_inline ssize_t ngx_write_fd(ngx_fd_t fd, void *buf, size_t n)
{
    return write(fd, buf, n);
}
{% endhighlight %}

### 寫入原理

如果多個進程都需要將數據添加到某一文件，那麼為了保證定位和寫數據這兩步是一個原子操作，需要在打開文件時設置 `O_APPEND` 標誌。

Single UNIX Specification 標準對此進行了詳細的說明，內核在調用 write 前會對文件進行加鎖，在調用 write 後會對文件進行解鎖，這樣保證了文件寫入的原子性，也就無需擔心數據交錯的發生了。

實際上，對於不同類型的文件實現方式會有所區別。

#### 普通文件

有三種情況可能導致文件寫入失敗：

* 磁盤已滿
* 寫入文件大小超出系統限制
* 內核高速緩存區已滿

遇到這三種情況怎麼處理呢？

如果是使用 `O_NONBLOCK` 標識打開文件的話，write 會立即返回，返回值小於寫入字符數這個參數，雖然寫入了不完整數據，但是內核保證其寫入過程的原子性，否則內核會讓調用進程睡眠，直到文件重新可寫，這樣內核保證了寫入數據的完整性，但是不保證寫入的原子性。

也即，如果在打開文件時設置了 `O_NONBLOCK` 標識，則雖然可能寫入部分數據，但是寫入過程是原子性的。

Linux 系統默認使用 `O_NONBLOCK` 標識打開文件，而 BSD 等 unix 系統則恰恰相反。


<!--

管道

SUS 標準對管道寫入有著明確的說明，只要一次性寫入數據小於管道緩衝區長度（PIPE_BUF），那麼不論 O_NONBLOCK 標識是否開啟，管道寫入都是原子性的，多個進程同時寫入同一管道是一定不會出現數據交錯的，否則，依然可能出現數據交錯


socket

linux 2.6.14 內核對 tcp socket 寫操作進行了說明，他並不是原子的

也許操作系統設計者認為，socket 是有可能永久阻塞的，所以如果保證這樣的 IO 具備原子性是十分荒唐的一件事吧

因此，對於 UNIX 日誌系統服務器的操作，必須每個線程都單獨進行一次 connect，保證每個線程使用不同的 fd 進行寫入，這樣才能防止數據交錯的發生

當然了，對於 udp socket 則無需擔心這一問題
-->

### 總結

對於寫入普通文件，只要文件是使用 `O_NONBLOCK` 標識打開的，那麼就可以保證其寫入的原子性，也就是說這樣寫入是可以接受的，但是這並不意味著這樣做是靠譜的，這樣做依然可能無法成功寫入全部數據。

然而，nginx 並沒有對返回結果進行判斷，他並不關心是否寫入成功，這顯然是不嚴謹的，但是作為一個工程化項目，這是不得不進行的妥協。


<!--
Linux系統環境下關於多進程併發寫同一個文件的討論
http://blog.chinaunix.net/uid-26548237-id-3046664.html

linux 系統調用 write 的原子性
http://techlog.cn/article/list/10182720
-->


{% highlight text %}
{% endhighlight %}

