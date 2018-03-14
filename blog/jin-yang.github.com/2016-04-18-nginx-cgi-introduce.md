---
title: Nginx 通用網關
layout: post
comments: true
category: [linux, network, webserver]
language: chinese
keywords: nginx,網關,cgi,fastcgi
description: CGI (Common Gateway Interface，通用網關接口) 描述了客戶端和服務器程序之間傳輸數據的一種標準，可以讓一個客戶端，從網頁瀏覽器向執行在網絡服務器上的程序請求數據。在此我們看下具體的實現以及應用。
---

CGI (Common Gateway Interface，通用網關接口) 描述了客戶端和服務器程序之間傳輸數據的一種標準，可以讓一個客戶端，從網頁瀏覽器向執行在網絡服務器上的程序請求數據。

在此我們看下具體的實現以及應用。

<!-- more -->

## 簡介

CGI 定義了一種標準，實際是獨立於任何語言的，CGI 程序可以用任何語言實現。最初，CGI 是 1993 年由美國國家超級電腦應用中心（NCSA）為 NCSA HTTPd Web 服務器開發的。

這個 Web 服務器使用了 UNIX shell 環境變量來保存從 Web 服務器傳遞出去的參數，然後生成一個運行 CGI 的獨立的進程。

如下為 CGI 請求流程圖。

![cgi fork and execute]({{ site.url }}/images/network/cgi-fork-and-execute.png "cgi fork and execute"){: .pull-center}

執行過程如下：

1. web 服務器收到客戶端（瀏覽器）的請求 Http Request，啟動 CGI 程序，並通過環境變量、標準輸入傳遞數據。

2. CGI 進程啟動解析器、加載配置（如業務相關配置）、連接其它服務器（如數據庫服務器）、邏輯處理等。

3. CGI 程將處理結果通過標準輸出、標準錯誤，傳遞給 web 服務器。

4. web 服務器收到 CGI 返回的結果，構建 Http Response 返回給客戶端，並殺死 CGI 進程。

在遇到請求時，一般先要創建 CGI 的子進程，然後 CGI 子進程處理請求，處理完之後結束這個子進程，這也就是 fork-and-execute 模式。

對於這種方式的服務器，有多少連接請求就會有多少 CGI 子進程，每個子進程都需要重起 CGI 解析器、加載配置、連接其它服務器等初始化工作，從而導致 CGI 性能低下。

### FastCGI，快速通用網關接口

FastCGI 是通用 CGI 的改進，減少了 Web 服務器與 CGI 程式之間互動的開銷。與為每個請求創建一個新的進程不同，FastCGI 使用持續的進程來處理一連串的請求，這些進程由 FastCGI 進程管理器管理，而不是 web 服務器。

![FastCGI process examples]({{ site.url }}/images/network/fastcgi-process-ex1.png "FastCGI process examples"){: .pull-center}

當進來一個請求時，Web 服務器把環境變量和這個頁面請求通過一個 unix domain socket (都位於同一物理服務器) 或者一個 IP Socket (FastCGI 部署在其它物理服務器) 傳遞給 FastCGI 進程。

![FastCGI process examples]({{ site.url }}/images/network/fastcgi-process-ex2.png "FastCGI process examples"){: .pull-center}

FastCGI 執行的主要流程如下：

1. Web 服務器啟動時載入初始化 FastCGI 執行環境，例如 IIS ISAPI、apache mod_fastcgi、nginx ngx_http_fastcgi_module、lighttpd mod_fastcgi。

2. FastCGI 進程管理器自身初始化，啟動多個 CGI 解釋器進程並等待來自 Web 服務器的連接。

3. 當客戶端請求到達 Web 服務器時，Web 服務器將請求採用 socket 方式轉發到 FastCGI 主進程，FastCGI 主進程選擇並連接到一個 CGI 解釋器。Web 服務器將 CGI 環境變量和標準輸入發送到 FastCGI 子進程。

4. FastCGI 子進程完成處理後將標準輸出和錯誤信息從同一 socket 連接返回 Web 服務器，當 FastCGI 子進程關閉連接時，請求便處理完成。

5. FastCGI 子進程接著等待並處理來自 Web 服務器的下一個連接。

由於 FastCGI 程序並不需要不斷的產生新進程，可以大大降低服務器的壓力並且產生較高的應用效率，它的速度效率最少要比 CGI 技術提高 5 倍以上，它還支持分佈式的部署， 即 FastCGI 程序可以在 web 服務器以外的主機上執行。

總結：CGI 就是所謂的短生存期應用程序，FastCGI 就是所謂的長生存期應用程序。FastCGI 像是一個常駐 (long-live) 型的 CGI，它可以一直執行著，不會每次都要花費時間去 fork 一次 (這是 CGI 最為人詬病的 fork-and-execute 模式)。

## Nginx

nginx 不像 apache 那樣可以直接執行外部可執行程序，但 nginx 可以作為代理服務器，將請求轉發給後端服務器。其中 nginx 就支持 FastCGI 代理，接收客戶端的請求，然後將請求轉發給後端 fastcgi 進程。

### Nginx+FastCGI

fastcgi 進程由 FastCGI 進程管理器管理，而不是 nginx，這樣就需要一個程序管理我們編寫的 fastcgi 程序，在此使用 spawn-fcgi。

[spawn-fcgi](http://redmine.lighttpd.net/projects/spawn-fcgi) 是一個通用的 FastCGI 進程管理器，簡單小巧，原屬於 [lighttpd](http://redmine.lighttpd.net/) 的一部分，後來由於使用比較廣泛，所以就遷移出來作為獨立項目了。

spawn-fcgi 使用 pre-fork 模型，功能主要是打開監聽端口，綁定地址，然後 fork-and-exec 創建我們編寫的 fastcgi 應用程序進程，退出完成工作。可以通過如下方法測試 C 的 FastCGI 程序。

在 CentOS 中可以通過如下命令直接安裝 (依賴 EPEL 源)，當然也可以通過源碼安裝。

{% highlight text %}
# yum install spawn-fcgi fcgi fcgi-devel
{% endhighlight %}

新建 FastCGI 應用程序。

{% highlight text %}
$ cat examples/foobar.c                                               # 查看源碼
#include &lt;fcgi_stdio.h&gt;
int main( int argc, char *argv[] )
{
    int count = 0;
    while( FCGI_Accept() &gt;= 0 )
        FCGI_printf( "Status: 200 OK\r\n" );
        FCGI_printf( "Content-Type: text/html\r\n\r\n" );
        FCGI_printf( "Hello world in C, #%d running on host %s\n",
                    ++count, getenv("SERVER_NAME"));
    }
    return 0;
}
$ gcc -o foobar.cgi foobar.c -L /usr/local/lib/ -lfcgi                # 直接編譯
{% endhighlight %}

啟動 spawn-fcgi 。

{% highlight text %}
$ spawn-fcgi -a 127.0.0.1 -p 7000 -u nginx -f /tmp/examples/foobar/foobar.cgi
{% endhighlight %}

修改 nginx 配置文件，添加如下內容，然後通過 kill -HUP PID 重新加載配置文件。

{% highlight text %}
... ...
        location = /hello.cgi {
            fastcgi_pass 127.0.0.1:7000;
            fastcgi_index index.cgi;
            fastcgi_param SCRIPT_FILENAME fcgi$fastcgi_script_name;
            include fastcgi_params;
        }
... ...
{% endhighlight %}

然後可以通過 [http://localhost/hello.cgi](http://localhost/hello.cgi) 查看。

如果報錯可以查看日誌，最好通過 setenforce 0 將 selinux 關閉，否則可能會有 502 Bad Gateway 報錯。


### Nginx+CGI

nginx 不能直接執行外部可執行程序，並且 cgi 是接收到請求時才會啟動 cgi 進程，不像 fastcgi 會在一開就啟動好，這樣 nginx 天生是不支持 cgi 的。

而 naginx 支持 FastCGI，我們可以考慮使用 fastcgi 包裝來支持 cgi，原理大致如下圖所示：pre-fork 幾個通用的代理 fastcgi 程序 fastcgi-wrapper，fastcgi-wrapper 啟動執行 cgi 然後將 cgi 的執行結果返回給 nginx（fork-and-exec）。

![Nginx FastCGI process examples]({{ site.url }}/images/network/cgi-process-ex.png "Nginx FastCGI process examples"){: .pull-center}

明白原理之後，編寫一個 fastcgi-warpper 也比較簡單，網上流傳比較多的一個解決方案是，來自 [nginx wiki](http://wiki.nginx.org/SimpleCGI) 上的使用 perl 的 fastcgi 包裝腳本 cgiwrap-fcgi.pl。

其實編寫 C 的 fastcgi-wrapper 就類似 C 的 fastcgi 程序，可以參考 [fastcgi-wrapper](https://github.com/gnosek/fcgiwrap) 。

下載源碼之後通過如下命令安裝，默認安裝在 /usr/local/sbin/fcgiwrap 。

{% highlight text %}
$ autoreconf -i
$ ./configure && make
# make install
{% endhighlight %}

仍以上述的示例為例，通過如下命令啟動。

{% highlight text %}
# spawn-fcgi -a 127.0.0.1 -p 7001 -u nginx -f /usr/local/sbin/fcgiwrap
{% endhighlight %}

配置文件中添加如下內容。
{% highlight text %}
        ... ...
        location ~ .*\.cgi$ {
            root /path/to/modules
            fastcgi_pass 127.0.0.1:7001;
            fastcgi_index index.cgi;
            include fastcgi.conf;
        }
        ... ...
{% endhighlight %}
<!-- http://www.cnblogs.com/skynet/p/4173450.html -->


## PHP

簡單介紹下如何在 PHP 中使用。

{% highlight text %}
# yum install php php-fpm                   # 安裝PHP以及PHP-FPM
# systemctl start php-fpm                   # 啟動服務，默認使用9000
{% endhighlight %}

在 Nginx 中添加配置內容。

{% highlight text %}
server {
    location ~ \.php$ {
        fastcgi_pass   127.0.0.1:9000;
        fastcgi_index  index.php;
        #fastcgi_param  SCRIPT_FILENAME  /scripts$fastcgi_script_name;
        fastcgi_param  SCRIPT_FILENAME   $document_root$fastcgi_script_name;
        include        fastcgi_params;
    }
}
{% endhighlight %}

### Web Server Gateway Interface, WSGI

WSGI 是 Python 中的一個規範協議，定義了一套接口，用來實現服務器端與應用端通信的規範化，定義為：

{% highlight text %}
WSGI is the Web Server Gateway Interface. It is a specification for web servers and
application servers to communicate with web applications (though it can also be used
for more than that).
{% endhighlight %}

Python 中的 SimpleHTTPServer 模塊提供了簡單創建 http 服務的示例，不過 WEB 領域中卻很少這麼做，而是使用了另外一個 WSGI (Web Server Gateway Interface) 協議。

如下是一個簡單的示例，可以直接啟動並訪問 http://127.1:8000：

{% highlight python %}
from wsgiref.simple_server import make_server

# 可以做為單獨的模塊，通過 from hello import foobar 導入
def foobar(environ, start_response):
    start_response('200 OK', [('Content-Type', 'text/html')])
    return '&lt;h1&gt;Hello, web!&lt;/h1&gt;'

# 創建一個服務器，IP地址為空，端口是8000，處理函數是foobar:
httpd = make_server('', 8000, foobar)
print "Serving HTTP on port 8000..."
# 開始監聽HTTP請求
httpd.serve_forever()
{% endhighlight %}

WSGI 接口定義非常簡單，如上所示，只需要實現一個函數，就可以響應 HTTP 請求；對於上述的 WSGI 標準處理函數，接收兩個參數：

* environ：一個包含所有 HTTP 請求信息的 dict 對象；

* start_response：一個發送 HTTP 響應的函數。

在 foobar() 中，通過調用 start_response() 發送 HTTP 響應的 Header，注意 Header 只能發送一次；包括了兩個參數：A) HTTP 響應碼，B) 一組 list 表示的 HTTP Header。

然後，函數的返回值將作為 HTTP 響應的 Body 發送給瀏覽器。

也就是說，通過 WSGI 定義的標準接口，只需要關心如何從 environ 這個字典中拿到 HTTP 請求信息，然後動態構造 HTML，通過 start_response() 發送 Header，最後返回 Body。

Python 內置了一個 WSGI 服務器，這個模塊為 wsgiref，是一個用 Python 編寫的 WSGI 服務器的參考實現。這也就意味這該實現完全符合 WSGI 標準，但是不考慮任何運行效率，僅供開發和測試使用。


### uwsgi

目前生產環境下最受推崇的方式是 uwsgi 服務器，其作用類似於 java 裡的 Tomcat 服務器，可參考官方網站 [The uWSGI project](http://uwsgi-docs.readthedocs.org/) 。

{% highlight text %}
# pip install uwsgi                                         # 安裝uwsgi服務器
# yum --enablerepo=epel install uwsgi uwsgi-plugin-python   # 作用同上

# cat foobar.py                                             # 實現一個簡單的應用
def application(env, start_response):
    start_response('200 OK', [('Content-Type','text/html')])
    return "Hello World"

/usr/bin/uwsgi_python -s 127.0.0.1:9090 --file hello.py --daemonize uwsgi.log

# uwsgi --http :8001 --wsgi-file foobar.py    # 簡單啟動
{% endhighlight %}

此時可以通過 [http://127.1:8001](http://127.1:8001/) 訪問，此時會在網頁上顯示 Hello World 。

<!-- server {
  location / {
    #使用動態端口
    uwsgi_pass unix:///tmp/uwsgi_vhosts.sock;
    #uwsgi_pass 127.0.0.1:9031;

    include uwsgi_params;
    uwsgi_param UWSGI_SCRIPT uwsgi;
    uwsgi_param UWSGI_PYHOME $document_root;
    uwsgi_param UWSGI_CHDIR  $document_root;
  }
} -->


## 參考

[FastCGI.com Archives](https://fastcgi-archives.github.io/)，包括了 FastCGI 標準，以及 C 的實現 fcgi library。


<!--
http://www.cnblogs.com/skynet/p/4173450.html
-->

{% highlight text %}
{% endhighlight %}
