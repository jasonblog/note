---
title: uWSGI 简介
layout: post
comments: true
language: chinese
category: [webserver,network]
keywords: uwsgi,简介
description: 这是一个 C 语言编写的 web 容器，实现了 wsgi 和 uwsig 协议，速度很快。
---

这是一个 C 语言编写的 web 容器，实现了 wsgi 和 uwsig 协议，速度很快。

<!-- more -->


## 简介

### 安装

可以直接参考 [Github - unbit/uwsgi](https://github.com/unbit/uwsgi) 源码，可以通过 yum 进行安装，不过安装的版本比较低，后面测试的时候会有很多的问题。

{% highlight text %}
# yum install python-pip
# pip install uwsgi
{% endhighlight %}

然后新建一个很简单的脚本，如下。

{% highlight text %}
$ cat foobar.py
def application(env, start-response):
    start-reponse('200 OK','[('Content_Type','text/html')]')
    return [b"Congraduations!!"]

$ uwsgi --http :8001 --wsgi-file foobar.py
{% endhighlight %}

当然，启动的时候可以通过如下方式启动。

{% highlight text %}
$ cat foobar.ini
[uwsgi]
chdir=/tmp/test/
wsgi-file=foobar.py
http-socket = 127.0.0.1:8001

$ uwsgi --ini foobar.ini
{% endhighlight %}

然后浏览器访问 [http://localhost:8001](http://localhost:8001) 或者 `curl http://127.1:8001` 即可，最终返回如下内容。

![flask uwsgi]({{ site.url }}/images/webserver/flask_uwsgi.png "flask uwsgi"){: .pull-center}

上述通过 uwsgi 启动时，如果启动报错，无法识别 `--wsgi-file` 时，实际是由于版本过低导致的。

## 调优

调优uwsgi，后台启动，热更改PY，杜绝502

经典配置需要看下

uwsgi-docs-cn.readthedocs.io/zh_CN/latest/Snippets.html

## 监控

除了可以通过 SNMP 监控之外，uWSGI 可以将当前的状态以 json 格式输出，可以通过如下的方式设置，其中 stats-http 用于提供 HTTP 服务。

{% highlight text %}
--stats 127.1:1717
--stats /tmp/stats.sock
--stats :1717
--stats @foobar
# Any of the above socket types can also return stats using HTTP
--stats 127.1:1717 --stats-http
{% endhighlight %}

然后可以通过如下的方式进行查看，返回的是一个 json 结果。

{% highlight text %}
$ nc -U /tmp/stats.sock
$ nc 127.1 1717
$ uwsgi --connect-and-read /tmp/stats.sock
$ uwsgi --connect-and-read 127.1 1717
{% endhighlight %}

另外，还有一个类似 top 的工具，可以实时查看。

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
uwsgi基础 最佳实践和问题


# 常见问题

入口为 main@core/uwsgi.c 。

{% highlight text %}
|-uwsgi_setup()
  |-uwsgi_start()
|-uwsgi_run()
{% endhighlight %}
-->


{% highlight text %}
{% endhighlight %}
