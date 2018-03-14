---
title: Webserver 簡介
layout: post
comments: true
language: chinese
category: [webserver,network]
keywords: webserver,micro_httpd,mini_httpd,lighttpd,thttpd,shttpd,miniweb
description: 簡單介紹一下 webserver，其中很大一部分是關於一些小型軟件的介紹。
---

簡單介紹一下 webserver，其中很大一部分是關於一些小型軟件的介紹。

<!-- more -->

## 簡介

一個應用服務器很大一部分操作是處理 http 協議相關的內容，通常一個 Web 應用的基本操作是：

1. 瀏覽器發送一個 HTTP 請求；
2. 服務器收到請求，生成一個 HTML 文檔；
3. 服務器把 HTML 文檔作為 HTTP 響應的 Body 發送給瀏覽器；
4. 瀏覽器收到 HTTP 響應，從 HTTP Body 取出 HTML 文檔並解析顯示。

最簡單的方式就是事先生成 HTML，並用文件保存；然後通過一個現成的 HTTP 服務器接收用戶請求，並從文件中讀取 HTML 返回給客戶端。這也就是常見的靜態服務器的處理邏輯，如 Apache、Nginx、Lighttpd 等。

如果要動態生成 HTML 就需要有相關的軟件實現，而對於接受、解析、發送 HTTP 請求，可以直接通過現有的程序處理，而不是還沒開始寫動態 HTML，就需要用很長時間讀 HTTP 規範，然後再實現業務邏輯。

## 常見 Web Server

常見的 Web 服務器有 Apache、Microsoft 的 Internet Information Server，IIS ；以及一些其它的小型服務器，包括了 lighttpd、thttpd、shttpd、micro_httpd、mini_httpd、miniweb。

簡單介紹一下一些常見的小型服務器。

### micro_httpd

{% highlight text %}
really small HTTP server
{% endhighlight %}

這個 httpd 適合簡單的 Web Server 編寫學習，它只有一個簡單的框架，只能夠處理簡單的靜態頁，可以考慮用來放靜態頁。

* 支持安全的 .. 上級目錄過濾
* 支持通用的 MIME 類型
* 支持簡單的目錄
* 支持目錄列表
* 支持使用 index.html 作為首頁
* Trailing-slash redirection
* 程序總共代碼才 200 多行

詳細的內容可以直接參考官方網站 [http://www.acme.com/software/micro_httpd/](http://www.acme.com/software/micro_httpd/) 。

### mini_httpd

{% highlight text %}
small HTTP server
{% endhighlight %}

mini_httpd 也是相對比較適合學習使用，大體實現了一個 Web Server 的功能，支持靜態頁和 CGI，能夠用來放置一些個人簡單的東西，不適宜投入生產使用。

* 支持 GET、HEAD、POST 方法
* 支持 CGI 功能
* 支持基本的驗證功能
* 支持安全 .. 上級目錄功能
* 支持通用的 MIME 類型
* 支持目錄列表功能
* 支持使用 index.html, index.htm, index.cgi 作為首頁
* 支持多個根目錄的虛擬主機
* 支持標準日誌記錄
* 支持自定義錯誤頁
* Trailing-slash redirection

詳細的內容可以直接參考官方網站 [http://www.acme.com/software/thttpd/](http://www.acme.com/software/thttpd/) 。

### lighttpd

{% highlight text %}
light footprint + httpd = LightTPD
{% endhighlight %}

Lighttpd 是一個德國人領導的開源軟件，其根本的目的是提供一個專門針對高性能網站，安全、快速、兼容性好並且靈活的 web server 環境。具有非常低的內存開銷，CPU 佔用率低，效能好，以及豐富的模塊等特點。

這是眾多開源輕量級的 web server 中較為優秀的一個，支持 FastCGI、CGI、Auth、輸出壓縮、URL 重寫、Alias 等重要功能。

Lighttpd 採用了 Multiplex，代碼經過優化，體積非常小，資源佔用很低，而且響應速度相當快。

Lighttpd 適合靜態資源類的服務，比如圖片、資源文件、靜態 HTML 等等的應用，性能應該比較好，同時也適合簡單的 CGI 應用的場合。

詳細的內容可以直接參考官方網站 [http://www.lighttpd.net/](http://www.lighttpd.net/) 。

### thttpd

{% highlight text %}
tiny/turbo/throttling HTTP server
{% endhighlight %}

thttpd 中是一個簡單、小型、輕便、快速和安全的 http 服務器。

thttpd 同樣是採用多路複用實現，支持基於 URL 的文件流量限制，這對於下載的流量控制而言是非常方便的。適合靜態資源類的服務，比如圖片、資源文件、靜態 HTML 等等的應用，性能應該比較好，同時也適合簡單的 CGI 應用的場合。

詳細的內容可以直接參考官方網站 [http://www.acme.com/software/thttpd/](http://www.acme.com/software/thttpd/) 。

### SHTTPD

{% highlight text %}
Simple HTTPD
{% endhighlight %}

Shttpd 是另一個輕量級的 web server，具有比 thttpd 更豐富的功能特性，支持 CGI、SSL、cookie、MD5 認證, 還能嵌入到現有的軟件裡，不需要配置文件。可以非常容易的開發嵌入式系統的 web server，官方網站上稱 shttpd 如果使用 uclibc/dielibc(libc的簡化子集) 則開銷將非常非常低。

* 小巧、快速、不膨脹、無需安裝、簡單的 40KB 的可執行文件，隨意運行
* 支持 GET, POST, HEAD, PUT, DELETE 等方法
* 支持 CGI, SSL, SSI, MD5 驗證, resumed download, aliases, inetd 模式運行
* 標準日誌格式
* 非常簡單整潔的嵌入式 API
* 容易定製運行在任意平臺：Windows, QNX, RTEMS, UNIX, BSD, Solaris, Linux

由於 shttpd 可以輕鬆嵌入其他程序裡，因此 shttpd 是較為理想的 web server 開發原形，開發人員可以基於 shttpd 開發出自己的 webserver！

詳細的內容可以直接參考官方網站 [http://shttpd.sourceforge.net/](http://shttpd.sourceforge.net/) 。

### MiniWeb

{% highlight text %}
small and elegent
{% endhighlight %}

通過 C 語言實現的 HTTP 服務器，支持 GET/POST 請求，支持授權，動態內容，支持壓縮，在 X86 上最小 20KB，除此之外還支持 VOD (video-on-demand) 。

詳細的內容可以直接參考官方網站 [http://miniweb.sourceforge.net/](http://miniweb.sourceforge.net/) 。

## micro_httpd

micro_httpd 是一個輕量級的 http 服務器， micro_httpd 從 xinetd 運行，適用於負載較小的網站，可以從 [www.acme.com](http://www.acme.com/software/micro_httpd/) 下載。

實現了 http 服務器的一些基本特徵功能：

* 文件名探測以保證安全；
* 常用 MIME 類型識別；
* Trailing-slash 重定向；
* index.html 首頁的處理。

### 安裝/配置

#### 1. 下載，安裝。
源碼可以從 [www.acme.com](http://www.acme.com/software/) 下載，或者 [本地保存版本](/reference/linux/network/micro_httpd_12dec2005.tar.gz)，解壓、編譯、安裝。

{% highlight text %}
$ make
$ make install
{% endhighlight %}

#### 2.  配置/etc/services文件

假設我們的 micro_httpd 將使用 8000 端口，先搜一下有沒有使用 8000 端口的，如果有則註釋掉並追加 micro_httpd 。

{% highlight text %}
$ cat /etc/services
micro_httpd     8000/tcp
micro_httpd     8000/udp
{% endhighlight %}

#### 3.  建立xinetd守護文件

如果你的系統沒有 /var/www/html 目錄，自行創建或者改成你期望的已經存在的目錄。

{% highlight text %}
$ cat /etc/xinetd.d/micro_httpd
service micro_httpd
{
    socket_type   = stream
    protocol      = tcp
    wait          = no
    user          = root
    server        = /usr/local/sbin/micro_httpd
    server_args   = /var/www/html
    disable       = no
    flags         = IPv4
}
{% endhighlight %}

#### 4. 重啟xinetd

通過如下命令重啟。

{% highlight text %}
# /etc/init.d/xinetd start
{% endhighlight %}

#### 5. 測試

創建一個測試網頁/var/www/html/index.html。

{% highlight text %}
# echo "hello" > /var/www/html/index.html
{% endhighlight %}

在瀏覽器裡輸入 http://ip:8000/index.html 。

當然通過 xinetd 可以進行一些配置。

<!--
 要求：配置telnet服務器並通過xinetd實現以下訪問控制功能
1. telnet服務服務器的最大連接數是20個
2. 若每秒的telnet請求達到4個，則服務自動停止1分鐘
3. 只允許來自trust.com域和192.168.0.0/24這個網絡的主機訪問你的telnet服務器
4. 拒絕192.168.0.100和bad.trust.com這兩臺主機訪問你的telnet服務器
5. 允許以上的客戶端在9：00-11：00 13：00-15：00訪問telnet服務器
6. 允許每個客戶端最多同時有2個telnet聯繫到服務器


[root@localhost ~]# cat /etc/hosts
127.0.0.1 localhost.localdomain localhost
192.168.0.1 ns.trust.com ns
192.168.0.2 bad.trust.com bad
解決問題：
[root@localhost ~]# vi /etc/xinetd.d/telnet
instances = 20
cps = 4 60
only_from = 192.168.0.0/24 .trust.com
no_access = 192.168.0.1 bad.trust.com
access_times = 9：00-11：00 13：00-15：00
per_source = 2
[root@localhost ~]# service xinetd restart
-->

{% highlight text %}
{% endhighlight %}
