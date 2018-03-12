---
title: Nginx 监控
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,monitor,监控
description: Nginx 服务器在生产环境中运行时，可以通过类似 Nagios、Zabbix、Munin 之类的网络监控软件。然而，很多时候处理问题时，只需要快速简单的查看当前 Nginx 服务器的状态请求。可以使用一个 Python 编写的工具 ngxtop，可以直接从 [www.github.com](https://github.com/lebinh/ngxtop) 上下载相应的源码。接下来介绍一下该工具的使用方法。
---

Nginx 服务器在生产环境中运行时，可以通过类似 Nagios、Zabbix、Munin 之类的网络监控软件。然而，很多时候处理问题时，只需要快速简单的查看当前 Nginx 服务器的状态请求。

可以使用一个 Python 编写的工具 ngxtop，可以直接从 [www.github.com](https://github.com/lebinh/ngxtop) 上下载相应的源码。

接下来介绍一下该工具的使用方法。

<!-- more -->

## ngxtop

ngxtop 会实时解析 nginx 的访问日志 (access log)，并且会将处理结果输出到终端，其功能类似于系统命令 top，通过 ngxtop 再也不需要 tail 日志看屏幕刷新了。

### 安装

ngxtop 是 Python 源码编写的程序，可以直接通过源码安装，也可以通过 pip 安装。

{% highlight text %}
$ wget https://github.com/lebinh/ngxtop/archive/master.zip -O ngxtop-master.zip
$ unzip ngxtop-master.zip
$ cd ngxtop-master
# python setup.py install
... ...
Finished processing dependencies for ngxtop==0.0.1
{% endhighlight %}

或者通过 pip 安装。

{% highlight text %}
# pip install ngxtop
{% endhighlight %}

其中详细的使用方法可以参考 [https://github.com/lebinh/ngxtop](https://github.com/lebinh/ngxtop) 中的帮助文档。

### 使用

ngxtop 默认会从其配置文件 ```/etc/nginx/nginx.conf``` 中查找 Nginx 日志的地址，如果使用的是该文件，那么可以直接运行 ngxtop 命令，这将会列出 10 个 Nginx 服务，按请求数量排序。

首先会解析配置文件的内容，解析后的结果可以通过 info 子命令查看。

{% highlight text %}
----- 查看解析配置文件的基本信息
$ ngxtop info

----- 显示前 20 个最频繁的请求，按照顺序排列
$ ngxtop -n 20

----- 显示自定义的请求变量
$ ngxtop print request http_user_agent remote_addr

----- 显示请求最多的客户端IP地址
$ ngxtop top remote_addr

----- 显示状态码是404的请求
$ ngxtop -i 'status == 404' print request status

----- 404前十的请求
$ ngxtop top request_path --filter 'status == 404'

----- 总流量前十的请求
$ ngxtop --order-by 'avg(bytes_sent) * count'

----- 输出400以上状态吗的请求以及请求来源
$ ngxtop -i 'status >= 400' print request status http_referer

----- 返回为200的请求，且请求地址为foo，打印消息体的平均长度
$ ngxtop avg bytes_sent --filter 'status == 200 and request_path.startswith("foo")'
{% endhighlight %}


## http_stub_status_module

另外，可以通过 Nginx 模块监控其状态，不过需要在编译时添加 ```--with-http_stub_status_module``` 选项，默认该选项不会包含在内。

然后在配置文件中添加如下内容。

{% highlight text %}
http {
    ... ...
    server {
        ... ...
        location /status {
            stub_status on;
            access_log off;
            allow 127.0.0.1;
            deny all;
        }
    }
}
{% endhighlight %}

修改完之后重新加载配置即可，然后可以通过 [http://localhost/status](http://localhost/status) 查看，此时会显示如下信息。

{% highlight text %}
Active connections: 557             # 对后端发起的活动连接数
server accepts handled requests     # 总共处理的链接数；成功握手次数；总共处理的请求
35 35 65
Reading: 3 Writing: 16 Waiting: 8   # 读取到客户端Header信息数；返回给客户端的Header信息数；等待下次请求的驻留连接
{% endhighlight %}

注意，对于最后一行 Waiting 在开启 keep-alive 时，该值等于 ```active-(reading+writing)```，意思就是说已经处理完正在等候下一次请求指令的驻留连接。

<!--
https://linux.cn/article-5970-1.html
http://www.ttlsa.com/zabbix/zabbix-monitor-nginx-performance/
-->


{% highlight text %}
{% endhighlight %}
