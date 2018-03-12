---
title: Linux 网络协议栈简介
layout: post
comments: true
language: chinese
category: [network]
keywords: linux,network,网络,协议栈
description: Linux 的 TCP/IP 协议栈估计是目前最灵活、应用最广的网络协议栈了，具有清晰的层次结构以及清晰定义的原语和接口，不仅使得上层应用开发者可以无需关心下层架构或者内部机制，从而提供相对透明的操作网络。在本文中简单介绍一下与协议栈相关的基本内容，如标准模型、数据传输等。
---

Linux 的 TCP/IP 协议栈估计是目前最灵活、应用最广的网络协议栈了，具有清晰的层次结构以及清晰定义的原语和接口，不仅使得上层应用开发者可以无需关心下层架构或者内部机制，从而提供相对透明的操作网络。

在本文中简单介绍一下与协议栈相关的内容。

<!-- more -->

## 标准模型

实际上有两套模型：七层 OSI 和 四层 TCP/IP 模型，由于种种原因，前者只存在于理论中，而实际应用的是后者，也就是 Linux 协议栈的实现，两者的对应关系如下：

![OSI and TCP/IP Models]({{ site.url }}/images/network/network-introduce-OSI-TCP-IP.png "OSI and TCP/IP Models"){: .pull-center}

在 TCP/IP 模型中，不同的协议层对应数据的名称也有所区别，而且每层都有不同的协议栈，详细的可以查看图片 [OSI 七层模型.png]({{ site.url }}/images/network/network-introduce-OSI-models.gif) 。


## 数据传输

接下来看看数据是如何通过协议栈在网络中进行传输的。假设有如下的网络，其中 Host X 尝试从 Server X 上下载 Html 文件，也就是现在我们浏览器经常办的事。

![How the Network Stack Operates]({{ site.url }}/images/network/network-introduce-how-works.png "How the Network Stack Operates"){: .pull-center}

那么有如下的疑问：

* Host X 和 Server Y 在不同的局域网中，它们是如何进行通讯的呢？

* Host X 不知道 Server Y 的物理地址，那么 X 如何发送请求呢？

首先在应用层 (Application Layer)，也就是用户在浏览器中输入 www.baidu.com 然后按下回车后，此时首先会通过 DNS 解析出该 URL 中主机的 IP 地址，假设是上图中的 208.201.237.37，那么接下来两台机器间就是通过 IP 来进行通讯。

在 TCP 传输层 (Transport Layer) 中，如果消息过长会将要发送的数据拆分成多个请求，并且会将源 IP、目的 IP、源 Port、目的 Port 包在消息头中，其中目的 Port 对于 HTTP 请求来说默认是 80，而源 Port 则是 OS 动态分配的。此时，TCP 并不知道如何将报文发送到目的地，实际上这是 IP 层所作的事情。

IP 网络层 (Network Layer) 尝试将报文直接发送出去，但是路由表中没有找到目的 IP，那么就会发送给系统指定的默认网关，也就是 Router RT1。

然后就是链路层 (Link Layer)，不同的链路层实现的机制有所区别，对于以太网来说，是通过 MAC 地址标示地址的，而 IP 地址和 MAC 的映射是通过 ARP 查询，有了路由器 RT1 的 MAC 地址之后，那么在局域网中就可以直接传输报文了。

接下来，就是在 RT1、RT2 ... ... 中的传输了，实际上除了最后一个路由器之外，其操作相同：

1. 移除链路层头部。

2. 找到对应的 IP 地址。

3. 发现该 IP 仍然不在该局域网中，找到下跳 MAC，新建一个链路层头。

4. 然后发送到下一个路由中。

实际上打包的过程，以及报文的内容如下：

![messages travel the stack]({{ site.url }}/images/network/netwok-introduce-travels.png "messages travel the stack"){: .pull-center}

其中 a~d 为在主机 Host X 上各层的打包过程，而 e 是在 RT1-RT2 的报文。



## Linux 协议栈

Linux 协议栈的内容大致如下图所示。

![the linux TCP/IP stack]({{ site.url }}/images/network/introduce-linux-TCP-IP-stack.jpg "the linux TCP/IP stack"){: .pull-center}

其中应用层通过 glibc 封装的系统调用与内核交换程序，例如 socket()、bind()、recv()、send() 等函数，在内核中实际实现的一个与虚拟文件系统类似的接口，也就是 socketfs 。

接下来就是对不同协议栈的实现，最常用的就是 TCP/IP 协议栈了，包括了 TCP、UDP、IP、ICMP、ARP 等等，下面我们会逐步讲解内核中各个模块的实现。

## 杂项

### Request For Comments, RFC

RFC 是最重要，也是最常用的资料之一。通常，当某家机构或团体开发出了一套标准或提出对某种标准的设想，想要征询外界的意见时，就会在 Internet 上发放一份 RFC。

一个 RFC 文件在成为官方标准前一般至少要经历 4 个阶段 【RFC2026】：因特网草案、建议标准、草案标准、因特网标准。

如下是一些常见的协议，可以选择 txt、pdf 等版本：

* [RFC-6176 Secure Sockets Layer (SSL) V2.0](https://tools.ietf.org/html/rfc6176)
* [RFC-6101 Secure Sockets Layer (SSL) V3.0](https://tools.ietf.org/html/rfc6101)



## 参考

其中与 Linux 协议栈相关的两本很不错的书籍，包括 《Understanding Linux Network Internals》 以及 《The Linux Networking Architecture: Design and Implementation of Network Protocols in the Linux Kernel》。

{% highlight c %}
{% endhighlight %}
