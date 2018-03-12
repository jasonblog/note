---
title: HTTP 协议简介
layout: post
comments: true
category: [linux, network]
language: chinese
keywords: http,协议,简介
description: HTTP 协议在 TCP/IP 协议之上，目前已经成为了互联网的基础协议，也是网页开发的必备知识，这里简单介绍下 HTTP 协议的历史演变、设计思路，以及相关的解析实现。
---

HTTP 协议在 TCP/IP 协议之上，目前已经成为了互联网的基础协议，也是网页开发的必备知识，这里简单介绍下 HTTP 协议的历史演变、设计思路，以及相关的解析实现。

<!-- more -->

![http introduce]({{ site.url }}/images/network/http-introduce.jpg "http introduce"){: .pull-center }

## 简介

HTTP 协议到现在为止总共经历了 3 个版本的演化，第一个 HTTP 协议诞生于 1989.3 。

### 换行符

在 Web 的发展过程中，UNIX 系统从一开始到现在一直是主要的开发和运行平台，但是在 HTTP 协议中，各个 header 之间用的却是 `CRLF (\r\n)` 这样的 Windows/DOS 换行方式，而不是 UNIX 普遍的的 `LF (\n)` 换行方式。

实际上 ASCII 沿袭了传统打字机的设计，也就是说在某种意义上来说 `CR-LF` 才是正统。

![http introduce]({{ site.url }}/images/network/http-introduce-printer.jpg "http introduce"){: .pull-center width="70%" }

左侧墨带盒上边的金属手柄了就是换行手柄。

打完一行字之后，滚筒已经基本移出了打字机主体，需要推左手边的换行手柄，然后滚筒会被推到最右边，打字头对准了行首，这就是 CR 。

推到头之后，继续用力，换行手柄大约会被扳动 30 度左右，这时候你会发现纸往上卷了一行，这就是 LF 。

所以遵循传统，ASCII 设计了 CR-LF 顺序。

## HTTP/0.9

规定了客户端和服务器之间的通信格式，默认使用 80 端口，最早版本是 1991 年发布的 0.9 版。

该版本极简单，只允许客户端发送 `GET` 这一种请求，不支持请求头。由于没有协议头，造成了 HTTP 0.9 协议只支持一种内容，即纯文本，网页仍支持 HTML 格式化，不过无法插入图片。

HTTP 0.9 具有典型的无状态性，每个事务独立进行处理，事务结束时就释放这个连接，也就是说，HTTP 协议的无状态特点在其第一个版本 0.9 中已经成型。

该版本只有一个命令 GET 。

{% highlight text %}
GET /index.html
{% endhighlight %}

上面命令表示，TCP 连接建立后，客户端向服务器请求网页`index.html` ；协议规定，服务器只能回应 HTML 格式的字符串，不能回应别的格式。

{% highlight c %}
<html>
    <body>Hello World</body>
</html>
{% endhighlight %}

服务器发送完毕，就关闭 TCP 连接。

## HTTP/1.0

1996.05 该版本发布，内容大大增加，主要包括了如下特性：

1. 除了数据部分，请求与响应支持头信息 (HTTP Header)，用来描述一些元数据。
1. 响应对象以一个响应状态行开始，而且支持图像、视频、二进制文件等多种格式。
2. 还引入了 POST、HEAD 方法，丰富了浏览器与服务器的互动手段。
2. 默认仍然使用短连接，但是支持长连接。

其它新增功能还包括状态码 (Status Code)、多字符集支持、多部分发送 (Multi-part Type)、权限 (Authorization)、缓存 (Cache)、内容编码 (Content Encoding) 等。

### 请求格式

下面是一个 1.0 版的 HTTP 请求的例子。

{% highlight text %}
GET / HTTP/1.0
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5)
Accept: */*
{% endhighlight %}

第一行是请求命令，尾部添加协议版本 `HTTP/1.0`，后面就是多行头信息，描述客户端的情况。

### 响应格式

服务器的回应如下。

{% highlight text %}
HTTP/1.0 200 OK
Content-Type: text/plain
Content-Length: 137582
Expires: Thu, 05 Dec 1997 16:00:00 GMT
Last-Modified: Wed, 5 August 1996 15:55:28 GMT
Server: Apache 0.84

<html>
	<body>Hello World</body>
</html>
{% endhighlight %}

格式是 `头信息 + 一个空行 (\r\n) + 数据`，第一行是 `协议版本 + 状态码 (status code) + 状态描述` 。

### Content-Type 字段

关于字符的编码，`1.0` 版规定，头信息必须是 ASCII 码，后面的数据可以是任何格式，因此，服务器响应时候，必须告诉客户端，数据是什么格式，这就是 `Content-Type` 字段的作用。

下面是一些常见的 `Content-Type` 字段的值。

{% highlight text %}
text/plain
text/html
text/css
image/jpeg
image/svg+xml
audio/mp4
video/mp4
application/pdf
application/atom+xml
{% endhighlight %}

这些数据类型总称为 `MIME Type`，每个值包括一级类型和二级类型，之间用斜杠分隔。

<!--
除了预定义的类型，厂商也可以自定义类型。

    application/vnd.debian.binary-package

上面的类型表明，发送的是Debian系统的二进制数据包。

MIME type还可以在尾部使用分号，添加参数。

    Content-Type: text/html; charset=utf-8

上面的类型表明，发送的是网页，而且编码是UTF-8。

客户端请求的时候，可以使用Accept字段声明自己可以接受哪些数据格式。

    Accept: */*

上面代码中，客户端声明自己可以接受任何格式的数据。
-->

`MIME Type` 不仅用在 HTTP 协议，还可以用在其它地方，比如 `HTML` 网页。

{% highlight text %}
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<!-- 等同于 -->
<meta charset="utf-8" />
{% endhighlight %}

### Content-Encoding 字段

由于发送的数据可以是任何格式，因此可以把数据压缩后再发送，`Content-Encoding` 字段说明数据的压缩方法。

{% highlight text %}
Content-Encoding: gzip
Content-Encoding: compress
Content-Encoding: deflate
{% endhighlight %}

客户端在请求时，用 `Accept-Encoding` 字段说明自己可以接受哪些压缩方法，例如：

{% highlight text %}
Accept-Encoding: gzip, deflate
{% endhighlight %}

### Content-Range 字段

一般断点下载时才会用到 `Range` 和 `Content-Range` 实体头；其中 `Range` 用于请求头，指定第一个字节的位置和最后一个字节的位置，如 `Range: 200-300`；而 `Content-Range` 用于响应头。

其示例请求头如下：

{% highlight text %}
GET /test.rar HTTP/1.1
Connection: close
Host: 116.1.219.219
Range: bytes=0-100
{% endhighlight %}

其中，Range 头可以请求实体的一个或者多个子范围，第一个字节是 0 ，常见的表示如下：

{% highlight text %}
bytes=0-499      头500个字节
bytes=500-999    第二个500字节
bytes=-500       最后500个字节
bytes=500-       500字节以后的范围
bytes=0-0,-1     第一个和最后一个字节
bytes=500-600,601-999  同时指定几个范围
{% endhighlight %}

一般正常响应如下：

{% highlight text %}
HTTP/1.1 206 OK
Content-Length:  801
Content-Type:  application/octet-stream
Content-Location: http://www.onlinedown.net/hj_index.htm
Content-Range: bytes 0-100/2350
Last-Modified: Mon, 16 Feb 2009 16:10:12 GMT
Accept-Ranges: bytes
{% endhighlight %}

上述的响应头中，其中的 `2350` 表示文件总大小，如果用户的请求中含有 `Range` 且服务器支持断点续传，那么服务器的相应代码为 `206` 。

### 缺点

`HTTP/1.0` 版的主要缺点是，每个 `TCP` 连接只能发送一个请求，发送数据完毕，连接就关闭，如果还要请求其它资源，就必须再新建一个连接。

TCP 连接的新建成本很高，因为需要客户端和服务器三次握手，并且开始时发送速率较慢 (slow start)。所以，`HTTP 1.0` 版本的性能比较差，尤其随着网页加载的外部资源越来越多，这个问题就愈发突出了。

为了解决这个问题，有些浏览器在请求时，用了一个非标准的 `Connection` 字段。

{% highlight text %}
Connection: keep-alive
{% endhighlight %}

这个字段要求服务器不要关闭 `TCP` 连接，以便其他请求复用，服务器同样回应这个字段。

{% highlight text %}
Connection: keep-alive
{% endhighlight %}

一个可以复用的TCP连接就建立了，直到客户端或服务器主动关闭连接。但是，这不是标准字段，不同实现的行为可能不一致，因此不是根本的解决办法。

## HTTP/1.1

1997.01 `HTTP/1.1` 版本发布，只比 `1.0` 版本晚了半年。它进一步完善了 HTTP 协议，一直用到了 20 年后的今天，直到现在还是最流行的版本。

### 持久连接

`1.1` 版的最大变化，就是引入了持久连接 (Persistent Connection)，即 TCP 连接默认不关闭，可以被多个请求复用，不用声明 `Connection: keep-alive` 。

客户端和服务器发现对方一段时间没有活动，就可以主动关闭连接。不过，规范的做法是，客户端在最后一个请求时，发送 `Connection: close` 明确要求服务器关闭 TCP 连接。

目前，对于同一个域名，大多数浏览器允许同时建立 6 个持久连接。

### 管道机制

`1.1` 版还引入了管道机制 (Pipelining)，即在同一个 TCP 连接里面，客户端可以同时发送多个请求，这样就进一步改进了 HTTP 协议的效率。

举例来说，客户端需要请求两个资源。

以前的做法是，在同一个 TCP 连接里面，先发送 A 请求，然后等待服务器做出回应，收到后再发出 B 请求。管道机制则是允许浏览器同时发出 A 请求和 B 请求，但是服务器还是按照顺序，先回应 A 请求，完成后再回应 B 请求。

### Content-Length 字段

一个 TCP 连接现在可以传送多个回应，势必就要有一种机制，区分数据包是属于哪一个回应的。这就是 `Content-length` 字段的作用，声明本次回应的数据长度。

{% highlight text %}
Content-Length: 3495
{% endhighlight %}

上面代码告诉浏览器，本次回应的长度是 3495 个字节，后面的字节就属于下一个回应了。

在 1.0 版中，`Content-Length` 字段不是必需的，因为浏览器发现服务器关闭了 TCP 连接，就表明收到的数据包已经全了。

### 分块传输编码

使用 `Content-Length` 字段的前提条件是，服务器发送回应之前，必须知道回应的数据长度。

对于一些很耗时的动态操作来说，这意味着，服务器要等到所有操作完成，才能发送数据，显然这样的效率不高。更好的处理方法是，产生一块数据，就发送一块，采用"流模式" (stream) 取代"缓存模式" (buffer)。

<!--
因此，1.1版规定可以不使用Content-Length字段，而使用"分块传输编码"（chunked transfer encoding）。只要请求或回应的头信息有Transfer-Encoding字段，就表明回应将由数量未定的数据块组成。

    Transfer-Encoding: chunked

每个非空的数据块之前，会有一个16进制的数值，表示这个块的长度。最后是一个大小为0的块，就表示本次回应的数据发送完了。下面是一个例子。


    HTTP/1.1 200 OK
    Content-Type: text/plain
    Transfer-Encoding: chunked

    25
    This is the data in the first chunk

    1C
    and this is the second one

    3
    con

    8
    sequence

    0

3.5 其他功能

1.1版还新增了许多动词方法：PUT、PATCH、HEAD、 OPTIONS、DELETE。

另外，客户端请求的头信息新增了Host字段，用来指定服务器的域名。


    Host: www.example.com

有了Host字段，就可以将请求发往同一台服务器上的不同网站，为虚拟主机的兴起打下了基础。
3.6 缺点

虽然1.1版允许复用TCP连接，但是同一个TCP连接里面，所有的数据通信是按次序进行的。服务器只有处理完一个回应，才会进行下一个回应。要是前面的回应特别慢，后面就会有许多请求排队等着。这称为"队头堵塞"（Head-of-line blocking）。

为了避免这个问题，只有两种方法：一是减少请求数，二是同时多开持久连接。这导致了很多的网页优化技巧，比如合并脚本和样式表、将图片嵌入CSS代码、域名分片（domain sharding）等等。如果HTTP协议设计得更好一些，这些额外的工作是可以避免的。
-->


## 参考

[The Original HTTP as defined in 1991](https://www.w3.org/Protocols/HTTP/AsImplemented.html)

<!--
http://www.ruanyifeng.com/blog/2016/08/http.html
-->

{% highlight text %}
{% endhighlight %}
