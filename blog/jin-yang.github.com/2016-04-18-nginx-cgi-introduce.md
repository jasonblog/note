---
title: Nginx 通用网关
layout: post
comments: true
category: [linux, network, webserver]
language: chinese
keywords: nginx,网关,cgi,fastcgi
description: CGI (Common Gateway Interface，通用网关接口) 描述了客户端和服务器程序之间传输数据的一种标准，可以让一个客户端，从网页浏览器向执行在网络服务器上的程序请求数据。在此我们看下具体的实现以及应用。
---

CGI (Common Gateway Interface，通用网关接口) 描述了客户端和服务器程序之间传输数据的一种标准，可以让一个客户端，从网页浏览器向执行在网络服务器上的程序请求数据。

在此我们看下具体的实现以及应用。

<!-- more -->

## 简介

CGI 定义了一种标准，实际是独立于任何语言的，CGI 程序可以用任何语言实现。最初，CGI 是 1993 年由美国国家超级电脑应用中心（NCSA）为 NCSA HTTPd Web 服务器开发的。

这个 Web 服务器使用了 UNIX shell 环境变量来保存从 Web 服务器传递出去的参数，然后生成一个运行 CGI 的独立的进程。

如下为 CGI 请求流程图。

![cgi fork and execute]({{ site.url }}/images/network/cgi-fork-and-execute.png "cgi fork and execute"){: .pull-center}

执行过程如下：

1. web 服务器收到客户端（浏览器）的请求 Http Request，启动 CGI 程序，并通过环境变量、标准输入传递数据。

2. CGI 进程启动解析器、加载配置（如业务相关配置）、连接其它服务器（如数据库服务器）、逻辑处理等。

3. CGI 程将处理结果通过标准输出、标准错误，传递给 web 服务器。

4. web 服务器收到 CGI 返回的结果，构建 Http Response 返回给客户端，并杀死 CGI 进程。

在遇到请求时，一般先要创建 CGI 的子进程，然后 CGI 子进程处理请求，处理完之后结束这个子进程，这也就是 fork-and-execute 模式。

对于这种方式的服务器，有多少连接请求就会有多少 CGI 子进程，每个子进程都需要重起 CGI 解析器、加载配置、连接其它服务器等初始化工作，从而导致 CGI 性能低下。

### FastCGI，快速通用网关接口

FastCGI 是通用 CGI 的改进，减少了 Web 服务器与 CGI 程式之间互动的开销。与为每个请求创建一个新的进程不同，FastCGI 使用持续的进程来处理一连串的请求，这些进程由 FastCGI 进程管理器管理，而不是 web 服务器。

![FastCGI process examples]({{ site.url }}/images/network/fastcgi-process-ex1.png "FastCGI process examples"){: .pull-center}

当进来一个请求时，Web 服务器把环境变量和这个页面请求通过一个 unix domain socket (都位于同一物理服务器) 或者一个 IP Socket (FastCGI 部署在其它物理服务器) 传递给 FastCGI 进程。

![FastCGI process examples]({{ site.url }}/images/network/fastcgi-process-ex2.png "FastCGI process examples"){: .pull-center}

FastCGI 执行的主要流程如下：

1. Web 服务器启动时载入初始化 FastCGI 执行环境，例如 IIS ISAPI、apache mod_fastcgi、nginx ngx_http_fastcgi_module、lighttpd mod_fastcgi。

2. FastCGI 进程管理器自身初始化，启动多个 CGI 解释器进程并等待来自 Web 服务器的连接。

3. 当客户端请求到达 Web 服务器时，Web 服务器将请求采用 socket 方式转发到 FastCGI 主进程，FastCGI 主进程选择并连接到一个 CGI 解释器。Web 服务器将 CGI 环境变量和标准输入发送到 FastCGI 子进程。

4. FastCGI 子进程完成处理后将标准输出和错误信息从同一 socket 连接返回 Web 服务器，当 FastCGI 子进程关闭连接时，请求便处理完成。

5. FastCGI 子进程接着等待并处理来自 Web 服务器的下一个连接。

由于 FastCGI 程序并不需要不断的产生新进程，可以大大降低服务器的压力并且产生较高的应用效率，它的速度效率最少要比 CGI 技术提高 5 倍以上，它还支持分布式的部署， 即 FastCGI 程序可以在 web 服务器以外的主机上执行。

总结：CGI 就是所谓的短生存期应用程序，FastCGI 就是所谓的长生存期应用程序。FastCGI 像是一个常驻 (long-live) 型的 CGI，它可以一直执行着，不会每次都要花费时间去 fork 一次 (这是 CGI 最为人诟病的 fork-and-execute 模式)。

## Nginx

nginx 不像 apache 那样可以直接执行外部可执行程序，但 nginx 可以作为代理服务器，将请求转发给后端服务器。其中 nginx 就支持 FastCGI 代理，接收客户端的请求，然后将请求转发给后端 fastcgi 进程。

### Nginx+FastCGI

fastcgi 进程由 FastCGI 进程管理器管理，而不是 nginx，这样就需要一个程序管理我们编写的 fastcgi 程序，在此使用 spawn-fcgi。

[spawn-fcgi](http://redmine.lighttpd.net/projects/spawn-fcgi) 是一个通用的 FastCGI 进程管理器，简单小巧，原属于 [lighttpd](http://redmine.lighttpd.net/) 的一部分，后来由于使用比较广泛，所以就迁移出来作为独立项目了。

spawn-fcgi 使用 pre-fork 模型，功能主要是打开监听端口，绑定地址，然后 fork-and-exec 创建我们编写的 fastcgi 应用程序进程，退出完成工作。可以通过如下方法测试 C 的 FastCGI 程序。

在 CentOS 中可以通过如下命令直接安装 (依赖 EPEL 源)，当然也可以通过源码安装。

{% highlight text %}
# yum install spawn-fcgi fcgi fcgi-devel
{% endhighlight %}

新建 FastCGI 应用程序。

{% highlight text %}
$ cat examples/foobar.c                                               # 查看源码
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
$ gcc -o foobar.cgi foobar.c -L /usr/local/lib/ -lfcgi                # 直接编译
{% endhighlight %}

启动 spawn-fcgi 。

{% highlight text %}
$ spawn-fcgi -a 127.0.0.1 -p 7000 -u nginx -f /tmp/examples/foobar/foobar.cgi
{% endhighlight %}

修改 nginx 配置文件，添加如下内容，然后通过 kill -HUP PID 重新加载配置文件。

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

然后可以通过 [http://localhost/hello.cgi](http://localhost/hello.cgi) 查看。

如果报错可以查看日志，最好通过 setenforce 0 将 selinux 关闭，否则可能会有 502 Bad Gateway 报错。


### Nginx+CGI

nginx 不能直接执行外部可执行程序，并且 cgi 是接收到请求时才会启动 cgi 进程，不像 fastcgi 会在一开就启动好，这样 nginx 天生是不支持 cgi 的。

而 naginx 支持 FastCGI，我们可以考虑使用 fastcgi 包装来支持 cgi，原理大致如下图所示：pre-fork 几个通用的代理 fastcgi 程序 fastcgi-wrapper，fastcgi-wrapper 启动执行 cgi 然后将 cgi 的执行结果返回给 nginx（fork-and-exec）。

![Nginx FastCGI process examples]({{ site.url }}/images/network/cgi-process-ex.png "Nginx FastCGI process examples"){: .pull-center}

明白原理之后，编写一个 fastcgi-warpper 也比较简单，网上流传比较多的一个解决方案是，来自 [nginx wiki](http://wiki.nginx.org/SimpleCGI) 上的使用 perl 的 fastcgi 包装脚本 cgiwrap-fcgi.pl。

其实编写 C 的 fastcgi-wrapper 就类似 C 的 fastcgi 程序，可以参考 [fastcgi-wrapper](https://github.com/gnosek/fcgiwrap) 。

下载源码之后通过如下命令安装，默认安装在 /usr/local/sbin/fcgiwrap 。

{% highlight text %}
$ autoreconf -i
$ ./configure && make
# make install
{% endhighlight %}

仍以上述的示例为例，通过如下命令启动。

{% highlight text %}
# spawn-fcgi -a 127.0.0.1 -p 7001 -u nginx -f /usr/local/sbin/fcgiwrap
{% endhighlight %}

配置文件中添加如下内容。
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

简单介绍下如何在 PHP 中使用。

{% highlight text %}
# yum install php php-fpm                   # 安装PHP以及PHP-FPM
# systemctl start php-fpm                   # 启动服务，默认使用9000
{% endhighlight %}

在 Nginx 中添加配置内容。

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

WSGI 是 Python 中的一个规范协议，定义了一套接口，用来实现服务器端与应用端通信的规范化，定义为：

{% highlight text %}
WSGI is the Web Server Gateway Interface. It is a specification for web servers and
application servers to communicate with web applications (though it can also be used
for more than that).
{% endhighlight %}

Python 中的 SimpleHTTPServer 模块提供了简单创建 http 服务的示例，不过 WEB 领域中却很少这么做，而是使用了另外一个 WSGI (Web Server Gateway Interface) 协议。

如下是一个简单的示例，可以直接启动并访问 http://127.1:8000：

{% highlight python %}
from wsgiref.simple_server import make_server

# 可以做为单独的模块，通过 from hello import foobar 导入
def foobar(environ, start_response):
    start_response('200 OK', [('Content-Type', 'text/html')])
    return '&lt;h1&gt;Hello, web!&lt;/h1&gt;'

# 创建一个服务器，IP地址为空，端口是8000，处理函数是foobar:
httpd = make_server('', 8000, foobar)
print "Serving HTTP on port 8000..."
# 开始监听HTTP请求
httpd.serve_forever()
{% endhighlight %}

WSGI 接口定义非常简单，如上所示，只需要实现一个函数，就可以响应 HTTP 请求；对于上述的 WSGI 标准处理函数，接收两个参数：

* environ：一个包含所有 HTTP 请求信息的 dict 对象；

* start_response：一个发送 HTTP 响应的函数。

在 foobar() 中，通过调用 start_response() 发送 HTTP 响应的 Header，注意 Header 只能发送一次；包括了两个参数：A) HTTP 响应码，B) 一组 list 表示的 HTTP Header。

然后，函数的返回值将作为 HTTP 响应的 Body 发送给浏览器。

也就是说，通过 WSGI 定义的标准接口，只需要关心如何从 environ 这个字典中拿到 HTTP 请求信息，然后动态构造 HTML，通过 start_response() 发送 Header，最后返回 Body。

Python 内置了一个 WSGI 服务器，这个模块为 wsgiref，是一个用 Python 编写的 WSGI 服务器的参考实现。这也就意味这该实现完全符合 WSGI 标准，但是不考虑任何运行效率，仅供开发和测试使用。


### uwsgi

目前生产环境下最受推崇的方式是 uwsgi 服务器，其作用类似于 java 里的 Tomcat 服务器，可参考官方网站 [The uWSGI project](http://uwsgi-docs.readthedocs.org/) 。

{% highlight text %}
# pip install uwsgi                                         # 安装uwsgi服务器
# yum --enablerepo=epel install uwsgi uwsgi-plugin-python   # 作用同上

# cat foobar.py                                             # 实现一个简单的应用
def application(env, start_response):
    start_response('200 OK', [('Content-Type','text/html')])
    return "Hello World"

/usr/bin/uwsgi_python -s 127.0.0.1:9090 --file hello.py --daemonize uwsgi.log

# uwsgi --http :8001 --wsgi-file foobar.py    # 简单启动
{% endhighlight %}

此时可以通过 [http://127.1:8001](http://127.1:8001/) 访问，此时会在网页上显示 Hello World 。

<!-- server {
  location / {
    #使用动态端口
    uwsgi_pass unix:///tmp/uwsgi_vhosts.sock;
    #uwsgi_pass 127.0.0.1:9031;

    include uwsgi_params;
    uwsgi_param UWSGI_SCRIPT uwsgi;
    uwsgi_param UWSGI_PYHOME $document_root;
    uwsgi_param UWSGI_CHDIR  $document_root;
  }
} -->


## 参考

[FastCGI.com Archives](https://fastcgi-archives.github.io/)，包括了 FastCGI 标准，以及 C 的实现 fcgi library。


<!--
http://www.cnblogs.com/skynet/p/4173450.html
-->

{% highlight text %}
{% endhighlight %}
