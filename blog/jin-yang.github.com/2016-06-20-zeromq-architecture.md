---
title: ZeroMQ 架構
layout: post
comments: true
language: chinese
category: [network]
keywords: zeromq,架構
description: 之前介紹了 ZeroMQ 的基本使用方法，接下來介紹一下其架構。
---

之前介紹了 ZeroMQ 的基本使用方法，接下來介紹一下其架構。

<!-- more -->

## 設計架構

ZMQ 是通過 C++ 實現，相比來說 ZMQ 的實現非常複雜，這裡的複雜並不是說代碼量很大，實際上目前即使包括註釋以及空白行在內，也就 3W+ 行；之所以複雜，是由於它現在支持包括 X86、ARM 在內的多平臺，而且支持二十多種語言的 binding 。

### 全局變量

在 ZMQ 中是沒有全局變量的，而是通過 `zmq::ctx_t` 保存了相關的上下文信息，之所以這麼做，按照作者的意思，如果一個應用程序的庫之間存在如下的依賴關係，也就是存在兩個庫 libx、liby 同時依賴 libzmq，那麼就會導致全局變量被初始化兩次，從而可能會導致不可預期的錯誤。

![ZeroMQ Architecture Libraries Depdents]({{ site.url }}/images/network/zmq-arch-global-state.png "ZeroMQ Architecture Libraries Depdents"){: .pull-center}

從 libzmq 看來，`ctx_t` 就相當於一個全局變量，老版本是通過 `zmq_init()` 進行初始化，而最新的版本則是通過 `zmq_ctx_new()` 初始化。

## 源碼解析

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
context_t()                      # 構造函數，3.2之後保存在了cppzmq項目中
 |-zmq_ctx_new()                 # 基本是新建zmq::ctx_t，如果有OpenPGM還會作相應的初始化
 |-zmq_ctx_set()                 # 設置上下文的IO線程、最大sockets屬性

socket_t()                       # 構造函數，3.2之後保存在了cppzmq項目中
 |-socket_t::init()              # 作初始化，實際主要調用如下的函數
   |-zmq_socket()
     |-ctx_t::create_socket()
       |-new reaper_t()          # 構造函數，只有一個
       |-reaper->start()         # 啟動線程
       |
       |-new io_thread_t()       # 創建IO線程，默認是1，可以通過ctx_t::set()進行設置
       |-io_thread->start()      # 啟動線程
       |
       |-socket_base_t::create() # 根據不同的類型創建不同的對象，如pair_t、pub_t

zmq_bind()
  |-socket_base_t::bind()
    |-process_commands()

socket_t::connect()
 |-zmq_connect()
   |-socket_base_t::connect()
     |-process_commands()        # 如果存在未處理的命令，則處理之
     |-parse_uri()               # 解析傳入的參數
     |-check_protocol()          # 並校驗是否支持


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

關於 ZMQ 架構的設計可以參考 [Internal Architecture of libzmq](http://zeromq.org/whitepapers:architecture) 以及 [The Architecture of Open Souce Applications - ZeroMQ](http://www.aosabook.org/en/zeromq.html) 這兩篇文章。

<!--
http://www.cnblogs.com/zengzy/category/777608.html
http://www.cnblogs.com/rainbowzc/p/3357594.html
http://blog.csdn.net/kaka11/article/category/619873
http://blog.csdn.net/yangbutao/article/details/8498790
-->



{% highlight text %}
{% endhighlight %}
