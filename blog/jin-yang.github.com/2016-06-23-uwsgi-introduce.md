---
title: uWSGI 簡介
layout: post
comments: true
language: chinese
category: [webserver,network]
keywords: uwsgi,簡介
description: 這是一個 C 語言編寫的 web 容器，實現了 wsgi 和 uwsig 協議，速度很快。
---

這是一個 C 語言編寫的 web 容器，實現了 wsgi 和 uwsig 協議，速度很快。

<!-- more -->


## 簡介

### 安裝

可以直接參考 [Github - unbit/uwsgi](https://github.com/unbit/uwsgi) 源碼，可以通過 yum 進行安裝，不過安裝的版本比較低，後面測試的時候會有很多的問題。

{% highlight text %}
# yum install python-pip
# pip install uwsgi
{% endhighlight %}

然後新建一個很簡單的腳本，如下。

{% highlight text %}
$ cat foobar.py
def application(env, start-response):
    start-reponse('200 OK','[('Content_Type','text/html')]')
    return [b"Congraduations!!"]

$ uwsgi --http :8001 --wsgi-file foobar.py
{% endhighlight %}

當然，啟動的時候可以通過如下方式啟動。

{% highlight text %}
$ cat foobar.ini
[uwsgi]
chdir=/tmp/test/
wsgi-file=foobar.py
http-socket = 127.0.0.1:8001

$ uwsgi --ini foobar.ini
{% endhighlight %}

然後瀏覽器訪問 [http://localhost:8001](http://localhost:8001) 或者 `curl http://127.1:8001` 即可，最終返回如下內容。

![flask uwsgi]({{ site.url }}/images/webserver/flask_uwsgi.png "flask uwsgi"){: .pull-center}

上述通過 uwsgi 啟動時，如果啟動報錯，無法識別 `--wsgi-file` 時，實際是由於版本過低導致的。

## 調優

調優uwsgi，後臺啟動，熱更改PY，杜絕502

經典配置需要看下

uwsgi-docs-cn.readthedocs.io/zh_CN/latest/Snippets.html

## 監控

除了可以通過 SNMP 監控之外，uWSGI 可以將當前的狀態以 json 格式輸出，可以通過如下的方式設置，其中 stats-http 用於提供 HTTP 服務。

{% highlight text %}
--stats 127.1:1717
--stats /tmp/stats.sock
--stats :1717
--stats @foobar
# Any of the above socket types can also return stats using HTTP
--stats 127.1:1717 --stats-http
{% endhighlight %}

然後可以通過如下的方式進行查看，返回的是一個 json 結果。

{% highlight text %}
$ nc -U /tmp/stats.sock
$ nc 127.1 1717
$ uwsgi --connect-and-read /tmp/stats.sock
$ uwsgi --connect-and-read 127.1 1717
{% endhighlight %}

另外，還有一個類似 top 的工具，可以實時查看。

{% highlight text %}
# pip install uwsgitop
$ uwsgitop /tmp/stats.sock
{% endhighlight %}

## 配置文件

{% highlight ini %}
[uwsgi]
base = /var/www/uop/
chdir=/tmp/test/
wsgi-file=foobar.py
#http-socket = 127.0.0.1:8001
chmod-socket = 664
socket = /tmp/%n.sock
daemonize = %(base)/log/uwsgi.log

# reload uWSGI if the specified file is modified/touched.
touch-reload = %(base)/conf/uwsgi.conf

# fork a thread to scan python file per N seconds.
py-autoreload = 2

# start the uWSGI stats server by a valid socket address.
stats = 127.1:1717
stats-http = true            # served over HTTP protocol


# add concurrency, by default, uWSGI starts with a single process and a single thread.
processes = 4
threads = 2

# set uid/gid, MUST start with root.
uid = foobar
gid = foobar

# try to remove all of the generated file/sockets.
vacuum = true


# optimize,
{% endhighlight %}



<!--
uwsgi基礎 最佳實踐和問題


# 常見問題

入口為 main@core/uwsgi.c 。

{% highlight text %}
|-uwsgi_setup()
  |-uwsgi_start()
|-uwsgi_run()
{% endhighlight %}
-->


{% highlight text %}
{% endhighlight %}
