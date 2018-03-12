---
title: ZeroMQ 架构
layout: post
comments: true
language: chinese
category: [network]
keywords: zeromq,架构
description: 之前介绍了 ZeroMQ 的基本使用方法，接下来介绍一下其架构。
---

之前介绍了 ZeroMQ 的基本使用方法，接下来介绍一下其架构。

<!-- more -->

## 设计架构

ZMQ 是通过 C++ 实现，相比来说 ZMQ 的实现非常复杂，这里的复杂并不是说代码量很大，实际上目前即使包括注释以及空白行在内，也就 3W+ 行；之所以复杂，是由于它现在支持包括 X86、ARM 在内的多平台，而且支持二十多种语言的 binding 。

### 全局变量

在 ZMQ 中是没有全局变量的，而是通过 `zmq::ctx_t` 保存了相关的上下文信息，之所以这么做，按照作者的意思，如果一个应用程序的库之间存在如下的依赖关系，也就是存在两个库 libx、liby 同时依赖 libzmq，那么就会导致全局变量被初始化两次，从而可能会导致不可预期的错误。

![ZeroMQ Architecture Libraries Depdents]({{ site.url }}/images/network/zmq-arch-global-state.png "ZeroMQ Architecture Libraries Depdents"){: .pull-center}

从 libzmq 看来，`ctx_t` 就相当于一个全局变量，老版本是通过 `zmq_init()` 进行初始化，而最新的版本则是通过 `zmq_ctx_new()` 初始化。

## 源码解析

<!--
{% highlight text %}
class ctx_t {
    //  Array of pointers to mailboxes for both application and I/O threads.
    uint32_t slot_count;
    mailbox_t **slots;
};
{% endhighlight %}
-->


{% highlight text %}
context_t()                      # 构造函数，3.2之后保存在了cppzmq项目中
 |-zmq_ctx_new()                 # 基本是新建zmq::ctx_t，如果有OpenPGM还会作相应的初始化
 |-zmq_ctx_set()                 # 设置上下文的IO线程、最大sockets属性

socket_t()                       # 构造函数，3.2之后保存在了cppzmq项目中
 |-socket_t::init()              # 作初始化，实际主要调用如下的函数
   |-zmq_socket()
     |-ctx_t::create_socket()
       |-new reaper_t()          # 构造函数，只有一个
       |-reaper->start()         # 启动线程
       |
       |-new io_thread_t()       # 创建IO线程，默认是1，可以通过ctx_t::set()进行设置
       |-io_thread->start()      # 启动线程
       |
       |-socket_base_t::create() # 根据不同的类型创建不同的对象，如pair_t、pub_t

zmq_bind()
  |-socket_base_t::bind()
    |-process_commands()

socket_t::connect()
 |-zmq_connect()
   |-socket_base_t::connect()
     |-process_commands()        # 如果存在未处理的命令，则处理之
     |-parse_uri()               # 解析传入的参数
     |-check_protocol()          # 并校验是否支持


zmq_bind()
  |-socket_base_t::bind()
    |-process_commands()


zmq_recv()
  |-zmq_msg_init()
  |-s_recvmsg()
  |-memcpy()
  |-zmq_msg_close()

zmq_send()
  |-zmq_msg_init_size()
  |-zmq_msg_data()
  |-memcpy()
  |-s_sendmsg()
{% endhighlight %}





## 其它

关于 ZMQ 架构的设计可以参考 [Internal Architecture of libzmq](http://zeromq.org/whitepapers:architecture) 以及 [The Architecture of Open Souce Applications - ZeroMQ](http://www.aosabook.org/en/zeromq.html) 这两篇文章。

<!--
http://www.cnblogs.com/zengzy/category/777608.html
http://www.cnblogs.com/rainbowzc/p/3357594.html
http://blog.csdn.net/kaka11/article/category/619873
http://blog.csdn.net/yangbutao/article/details/8498790
-->



{% highlight text %}
{% endhighlight %}
