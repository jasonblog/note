---
title: Nginx 日志解析
layout: post
comments: true
language: chinese
category: [webserver, network]
keywords: hello world,示例,sample,markdown
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---

在 Nginx 的统计排错过程中，通常要依赖日志，Nginx 有非常灵活的日志记录模式，每个级别的配置可以有各自独立的访问日志。

<!-- more -->

## 简介

其中与日志模块相关的参数如下，在 CentOS 中，日志默认保存在 `/usr/local/nginx/logs` 目录下。

### 访问日志

实际上这里的配置项是通过 [ngx_http_log_module](http://nginx.org/en/docs/http/ngx_http_log_module.html) 实现的，记录了哪些用户，哪些页面以及用户浏览器、IP 和其他的访问信息。

常见的配置项包括了 `access_log`、`log_format`、`open_log_file_cache` 。

其中 `log_format` 记录了日志的记录格式；`access_log` 用于设置保存的路径、是否压缩、缓冲的大小等信息；而最后一个其实是对写入日志的优化。

#### access_log

可以将 path 设置为 syslog 将日志保存到系统日志中，不过其默认是保存到单独的日志文件中，默认的配置如下：

{% highlight text %}
----- 语法格式如下
access_log path format gzip[=level] [buffer=size] [flush=time];
access_log syslog:server=address[,parameter=value] [format];
access_log off;

----- 默认配置为
access_log logs/access.log combined;
{% endhighlight %}

其中 gzip 表示压缩等级；buffer 用来设置内存缓存区大小；flush 为保存在缓存区中的最长时间。

#### log_format

其配置格式为：

{% highlight text %}
log_format name string;
{% endhighlight %}

其中，name 表示该配置格式的名称；string 表示定义格式的内容。其中该模块中有一个默认的无需配置名称为 combined 的日志格式，相当于 apache 的 combined 日志格式。

{% highlight text %}
log_format combined '$remote_addr - $remote_user  [$time_local]  '
                    ' "$request"  $status  $body_bytes_sent  '
                    ' "$http_referer"  "$http_user_agent" ';

{% endhighlight %}

如果 Nginx 位于负载均衡器 squid，或者 Nginx 反向代理之后，那么 web 服务器中的 $remote_addr 实际上获取的是反向代理的 IP 地址，将无法直接获取到客户端真实的 IP 地址。

为此，反向代理服务器在转发请求的 http 头信息中，可以增加 X-Forwarded-For 信息，用来记录客户端 IP 地址和客户端请求的服务器地址。

{% highlight text %}
log_format  porxy  '$http_x_forwarded_for - $remote_user  [$time_local]  '
                   ' "$request"  $status $body_bytes_sent '
                   ' "$http_referer"  "$http_user_agent" ';
{% endhighlight %}

其中对应各个参数分别代表的值可以参考 [ngx_http_log_module](http://nginx.org/en/docs/http/ngx_http_log_module.html) 中的介绍。

#### open_log_file_cache

对于每一条日志记录，都将是先打开文件，再写入日志，然后关闭；这样就会导致文件被频繁的打开关闭，可以使用 open_log_file_cache 设置日志文件缓存。

{% highlight text %}
open_log_file_cache max=N [inactive=time] [min_uses=N] [valid=time];
open_log_file_cache off;

----- 示例
open_log_file_cache max=1000 inactive=20s valid=1m min_uses=2;
{% endhighlight %}

### 错误日志

`error_log` 则是记录服务器错误日志。


## 源码解析

在 nginx 中，会通过 `ngx_errlog_module` 模块用于处理 nginx 日志信息，这也是 nginx 的 core 模块之一。

在配置文件中，会通过解析 `error_log` 配置项将不同等级的日志信息输出到指定的文件中，当遇到该配置项是，就调用 errlog 模块的 `ngx_error_log()` 来解析。

{% highlight text %}
ngx_error_log()
{% endhighlight %}

上述函数会将 `error_log` 配置项的值保存在 `ngx_cycle->new_log` 成员中，当配置文件中有多条 `error_log` 配置项生效时，会通过 `ngx_cycle->new_log.next` 成员将它们组织起来。

如果配置文件中没有 `error_log` 配置项，在配置文件解析完后调用 `errlog` 模块的 `ngx_log_open_default()` 将日志等级默认置为 `NGX_LOG_ERR`，日志文件设置为 `NGX_ERROR_LOG_PATH` (编译时指定)。

<!--
由此可看无论配置文件中是否有error_log配置项始终会有日志输出，nginx中禁止输出日志唯一的办法：
将error_log配置项的输出日志文件名指定为/dev/null。
-->

### 日志结构体

{% highlight c %}
struct ngx_log_s {
    ngx_uint_t           log_level;          // 日志等级
    ngx_open_file_t     *file;               // 记录日志文件信息
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

一般通过 `ngx_log_error()` 函数输出日志信息。

{% highlight c %}
void ngx_cdecl ngx_log_error(ngx_uint_t level,
	ngx_log_t *log, ngx_err_t err, const char *fmt, ...)
{
    va_list  args;

    if (log->log_level >= level) { // 根据日志等级判断是否需要输出
        va_start(args, fmt);
        ngx_log_error_core(level, log, err, fmt, args);
        va_end(args);
    }
}

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
    const char *fmt, va_list args)
{
	/* ... ... */

	/* 格式化日志内容 */
	last = errstr + NGX_MAX_ERROR_STR;
	p = ngx_cpymem(errstr, ngx_cached_err_log_time.data,
		       ngx_cached_err_log_time.len);
	p = ngx_slprintf(p, last, " [%V] ", &err_levels[level]);

	/* 写入日志 */
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


## 原子写入

实际上，这里会存在一个疑问，多进程的 nginx 模型是怎么保证多个进程同时写入一个文件不发生数据交错呢？

一般来说，主要有如下的几种解决方案：

* 通过锁保证只有一个进程写入，其他进程等待，但是这种情况效率太低。

* 写日志前查看锁状态，如果已经锁定，则写入进程自己的缓冲区中，等待下次调用时同步缓冲区，这样做的好处是无需阻塞，提高了效率，但是就无法做到 log 的实时了，这样做工程中也是绝对无法接受的，一旦发生问题，将无法保证 log 是否已经被写入，因此很难定位。

* 一个进程专门负责写 log，其他进程通过套接字或者管道将 log 内容发送给他，持续阻塞在 epoll_wait 上，直到收到信息，立即写入，但是众所周知，nginx 是调用同一个函数启动所有进程的，并没有专门调用函数启动所谓的 log 进程，除了 master 和 worker，nginx 也确实没有 log 进程存在。

* 进程启动后，全部去竞争某个锁，竞争到该锁的 worker 执行 log worker 的代码，其余的 worker 继续运行相应程序，这个方案看上去是一个不错的方案，如果是单 worker 的话，那么就无需去使用该锁即可。

阅读源码可以发现 nginx 只有一把互斥锁，即用来避免惊群现象的 `ngx_accept_mutex` 锁，其余地方完全没有用到锁机制，这么做原因很简单，在工程化的代码中，盲目使用锁会造成性能的下降，这是不可以接受的。

如上，最终写入日志的是通过 `ngx_log_error_core()` 函数，这个函数正是用来打印错误日志的，而最终调用的实际上就是：

{% highlight c %}
static ngx_inline ssize_t ngx_write_fd(ngx_fd_t fd, void *buf, size_t n)
{
    return write(fd, buf, n);
}
{% endhighlight %}

### 写入原理

如果多个进程都需要将数据添加到某一文件，那么为了保证定位和写数据这两步是一个原子操作，需要在打开文件时设置 `O_APPEND` 标志。

Single UNIX Specification 标准对此进行了详细的说明，内核在调用 write 前会对文件进行加锁，在调用 write 后会对文件进行解锁，这样保证了文件写入的原子性，也就无需担心数据交错的发生了。

实际上，对于不同类型的文件实现方式会有所区别。

#### 普通文件

有三种情况可能导致文件写入失败：

* 磁盘已满
* 写入文件大小超出系统限制
* 内核高速缓存区已满

遇到这三种情况怎么处理呢？

如果是使用 `O_NONBLOCK` 标识打开文件的话，write 会立即返回，返回值小于写入字符数这个参数，虽然写入了不完整数据，但是内核保证其写入过程的原子性，否则内核会让调用进程睡眠，直到文件重新可写，这样内核保证了写入数据的完整性，但是不保证写入的原子性。

也即，如果在打开文件时设置了 `O_NONBLOCK` 标识，则虽然可能写入部分数据，但是写入过程是原子性的。

Linux 系统默认使用 `O_NONBLOCK` 标识打开文件，而 BSD 等 unix 系统则恰恰相反。


<!--

管道

SUS 标准对管道写入有着明确的说明，只要一次性写入数据小于管道缓冲区长度（PIPE_BUF），那么不论 O_NONBLOCK 标识是否开启，管道写入都是原子性的，多个进程同时写入同一管道是一定不会出现数据交错的，否则，依然可能出现数据交错


socket

linux 2.6.14 内核对 tcp socket 写操作进行了说明，他并不是原子的

也许操作系统设计者认为，socket 是有可能永久阻塞的，所以如果保证这样的 IO 具备原子性是十分荒唐的一件事吧

因此，对于 UNIX 日志系统服务器的操作，必须每个线程都单独进行一次 connect，保证每个线程使用不同的 fd 进行写入，这样才能防止数据交错的发生

当然了，对于 udp socket 则无需担心这一问题
-->

### 总结

对于写入普通文件，只要文件是使用 `O_NONBLOCK` 标识打开的，那么就可以保证其写入的原子性，也就是说这样写入是可以接受的，但是这并不意味着这样做是靠谱的，这样做依然可能无法成功写入全部数据。

然而，nginx 并没有对返回结果进行判断，他并不关心是否写入成功，这显然是不严谨的，但是作为一个工程化项目，这是不得不进行的妥协。


<!--
Linux系统环境下关于多进程并发写同一个文件的讨论
http://blog.chinaunix.net/uid-26548237-id-3046664.html

linux 系统调用 write 的原子性
http://techlog.cn/article/list/10182720
-->


{% highlight text %}
{% endhighlight %}

