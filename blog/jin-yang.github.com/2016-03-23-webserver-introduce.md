---
title: Webserver 简介
layout: post
comments: true
language: chinese
category: [webserver,network]
keywords: webserver,micro_httpd,mini_httpd,lighttpd,thttpd,shttpd,miniweb
description: 简单介绍一下 webserver，其中很大一部分是关于一些小型软件的介绍。
---

简单介绍一下 webserver，其中很大一部分是关于一些小型软件的介绍。

<!-- more -->

## 简介

一个应用服务器很大一部分操作是处理 http 协议相关的内容，通常一个 Web 应用的基本操作是：

1. 浏览器发送一个 HTTP 请求；
2. 服务器收到请求，生成一个 HTML 文档；
3. 服务器把 HTML 文档作为 HTTP 响应的 Body 发送给浏览器；
4. 浏览器收到 HTTP 响应，从 HTTP Body 取出 HTML 文档并解析显示。

最简单的方式就是事先生成 HTML，并用文件保存；然后通过一个现成的 HTTP 服务器接收用户请求，并从文件中读取 HTML 返回给客户端。这也就是常见的静态服务器的处理逻辑，如 Apache、Nginx、Lighttpd 等。

如果要动态生成 HTML 就需要有相关的软件实现，而对于接受、解析、发送 HTTP 请求，可以直接通过现有的程序处理，而不是还没开始写动态 HTML，就需要用很长时间读 HTTP 规范，然后再实现业务逻辑。

## 常见 Web Server

常见的 Web 服务器有 Apache、Microsoft 的 Internet Information Server，IIS ；以及一些其它的小型服务器，包括了 lighttpd、thttpd、shttpd、micro_httpd、mini_httpd、miniweb。

简单介绍一下一些常见的小型服务器。

### micro_httpd

{% highlight text %}
really small HTTP server
{% endhighlight %}

这个 httpd 适合简单的 Web Server 编写学习，它只有一个简单的框架，只能够处理简单的静态页，可以考虑用来放静态页。

* 支持安全的 .. 上级目录过滤
* 支持通用的 MIME 类型
* 支持简单的目录
* 支持目录列表
* 支持使用 index.html 作为首页
* Trailing-slash redirection
* 程序总共代码才 200 多行

详细的内容可以直接参考官方网站 [http://www.acme.com/software/micro_httpd/](http://www.acme.com/software/micro_httpd/) 。

### mini_httpd

{% highlight text %}
small HTTP server
{% endhighlight %}

mini_httpd 也是相对比较适合学习使用，大体实现了一个 Web Server 的功能，支持静态页和 CGI，能够用来放置一些个人简单的东西，不适宜投入生产使用。

* 支持 GET、HEAD、POST 方法
* 支持 CGI 功能
* 支持基本的验证功能
* 支持安全 .. 上级目录功能
* 支持通用的 MIME 类型
* 支持目录列表功能
* 支持使用 index.html, index.htm, index.cgi 作为首页
* 支持多个根目录的虚拟主机
* 支持标准日志记录
* 支持自定义错误页
* Trailing-slash redirection

详细的内容可以直接参考官方网站 [http://www.acme.com/software/thttpd/](http://www.acme.com/software/thttpd/) 。

### lighttpd

{% highlight text %}
light footprint + httpd = LightTPD
{% endhighlight %}

Lighttpd 是一个德国人领导的开源软件，其根本的目的是提供一个专门针对高性能网站，安全、快速、兼容性好并且灵活的 web server 环境。具有非常低的内存开销，CPU 占用率低，效能好，以及丰富的模块等特点。

这是众多开源轻量级的 web server 中较为优秀的一个，支持 FastCGI、CGI、Auth、输出压缩、URL 重写、Alias 等重要功能。

Lighttpd 采用了 Multiplex，代码经过优化，体积非常小，资源占用很低，而且响应速度相当快。

Lighttpd 适合静态资源类的服务，比如图片、资源文件、静态 HTML 等等的应用，性能应该比较好，同时也适合简单的 CGI 应用的场合。

详细的内容可以直接参考官方网站 [http://www.lighttpd.net/](http://www.lighttpd.net/) 。

### thttpd

{% highlight text %}
tiny/turbo/throttling HTTP server
{% endhighlight %}

thttpd 中是一个简单、小型、轻便、快速和安全的 http 服务器。

thttpd 同样是采用多路复用实现，支持基于 URL 的文件流量限制，这对于下载的流量控制而言是非常方便的。适合静态资源类的服务，比如图片、资源文件、静态 HTML 等等的应用，性能应该比较好，同时也适合简单的 CGI 应用的场合。

详细的内容可以直接参考官方网站 [http://www.acme.com/software/thttpd/](http://www.acme.com/software/thttpd/) 。

### SHTTPD

{% highlight text %}
Simple HTTPD
{% endhighlight %}

Shttpd 是另一个轻量级的 web server，具有比 thttpd 更丰富的功能特性，支持 CGI、SSL、cookie、MD5 认证, 还能嵌入到现有的软件里，不需要配置文件。可以非常容易的开发嵌入式系统的 web server，官方网站上称 shttpd 如果使用 uclibc/dielibc(libc的简化子集) 则开销将非常非常低。

* 小巧、快速、不膨胀、无需安装、简单的 40KB 的可执行文件，随意运行
* 支持 GET, POST, HEAD, PUT, DELETE 等方法
* 支持 CGI, SSL, SSI, MD5 验证, resumed download, aliases, inetd 模式运行
* 标准日志格式
* 非常简单整洁的嵌入式 API
* 容易定制运行在任意平台：Windows, QNX, RTEMS, UNIX, BSD, Solaris, Linux

由于 shttpd 可以轻松嵌入其他程序里，因此 shttpd 是较为理想的 web server 开发原形，开发人员可以基于 shttpd 开发出自己的 webserver！

详细的内容可以直接参考官方网站 [http://shttpd.sourceforge.net/](http://shttpd.sourceforge.net/) 。

### MiniWeb

{% highlight text %}
small and elegent
{% endhighlight %}

通过 C 语言实现的 HTTP 服务器，支持 GET/POST 请求，支持授权，动态内容，支持压缩，在 X86 上最小 20KB，除此之外还支持 VOD (video-on-demand) 。

详细的内容可以直接参考官方网站 [http://miniweb.sourceforge.net/](http://miniweb.sourceforge.net/) 。

## micro_httpd

micro_httpd 是一个轻量级的 http 服务器， micro_httpd 从 xinetd 运行，适用于负载较小的网站，可以从 [www.acme.com](http://www.acme.com/software/micro_httpd/) 下载。

实现了 http 服务器的一些基本特征功能：

* 文件名探测以保证安全；
* 常用 MIME 类型识别；
* Trailing-slash 重定向；
* index.html 首页的处理。

### 安装/配置

#### 1. 下载，安装。
源码可以从 [www.acme.com](http://www.acme.com/software/) 下载，或者 [本地保存版本](/reference/linux/network/micro_httpd_12dec2005.tar.gz)，解压、编译、安装。

{% highlight text %}
$ make
$ make install
{% endhighlight %}

#### 2.  配置/etc/services文件

假设我们的 micro_httpd 将使用 8000 端口，先搜一下有没有使用 8000 端口的，如果有则注释掉并追加 micro_httpd 。

{% highlight text %}
$ cat /etc/services
micro_httpd     8000/tcp
micro_httpd     8000/udp
{% endhighlight %}

#### 3.  建立xinetd守护文件

如果你的系统没有 /var/www/html 目录，自行创建或者改成你期望的已经存在的目录。

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

#### 4. 重启xinetd

通过如下命令重启。

{% highlight text %}
# /etc/init.d/xinetd start
{% endhighlight %}

#### 5. 测试

创建一个测试网页/var/www/html/index.html。

{% highlight text %}
# echo "hello" > /var/www/html/index.html
{% endhighlight %}

在浏览器里输入 http://ip:8000/index.html 。

当然通过 xinetd 可以进行一些配置。

<!--
 要求：配置telnet服务器并通过xinetd实现以下访问控制功能
1. telnet服务服务器的最大连接数是20个
2. 若每秒的telnet请求达到4个，则服务自动停止1分钟
3. 只允许来自trust.com域和192.168.0.0/24这个网络的主机访问你的telnet服务器
4. 拒绝192.168.0.100和bad.trust.com这两台主机访问你的telnet服务器
5. 允许以上的客户端在9：00-11：00 13：00-15：00访问telnet服务器
6. 允许每个客户端最多同时有2个telnet联系到服务器


[root@localhost ~]# cat /etc/hosts
127.0.0.1 localhost.localdomain localhost
192.168.0.1 ns.trust.com ns
192.168.0.2 bad.trust.com bad
解决问题：
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
