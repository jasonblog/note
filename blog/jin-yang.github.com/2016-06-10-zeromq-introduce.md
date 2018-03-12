---
title: ZeroMQ 简介
layout: post
comments: true
language: chinese
category: [network]
keywords: zeromq
description: ZMQ (Zero Message Queue) 是一个 C++ 编写的高性能分布式消息队列，是一个非常简单好用的传输层，使得 Socket 编程更加简单、简洁和性能更高效。目前比较常用的消息中间件产品包括了 RabbitMQ、ZeroMQ、ActiveMQ，三者分别通过　Erlang、C++、Java 实现，相比而言，ZMQ 是一个简单的库，而非单独的中间件产品。接下来简单介绍一下 ZMQ 的相关内容。
---

ZMQ (Zero Message Queue) 是一个 C++ 编写的高性能分布式消息队列，是一个非常简单好用的传输层，使得 Socket 编程更加简单、简洁和性能更高效。

目前比较常用的消息中间件产品包括了 RabbitMQ、ZeroMQ、ActiveMQ，三者分别通过　Erlang、C++、Java 实现，相比而言，ZMQ 是一个简单的库，而非单独的中间件产品。

接下来简单介绍一下 ZMQ 的相关内容。

<!-- more -->

## 简介

[ZMQ (Zero Message Queue)](http://zeromq.org/) 的资料非常全，相关的资料基本都保存在 [zeromq.org - community](http://zeromq.org/community) 中，例如一些经常使用的链接包括了示例 [ZMQ - The Guide](http://zguide.zeromq.org/page:all)、相关的设计文档 [Interesting Whitepapers](http://zeromq.org/area:whitepapers)、及各种语言的示例程序 [Github Zguide Examples](https://github.com/imatix/zguide/tree/master/examples/) 。

ZMQ 的作者是 Martin Sustrik，其中的绝大多数的代码是由他来维护的，可以看下源码中的 MAINTAINERS 文件。另外，他还维护了：[nanomsg](http://nanomsg.org ) 封装了网络编程中常见的模型、[libmill](http://libmill.org) 基于 C 的类 Go 库，据说可以支持 2KW 个协程，每秒 5KW 次上下文切换 (没有给出硬件配置，不过如果属实，性能确实有点恐怖)。

### Advanced Message Queuing Protocol

[AMQP](http://www.amqp.org/)，高级消息队列协议，一个应用层的开放标准协议，为面向消息的中间件而设计，主要用于各个组件之间的解耦，基于该协议的客户端与消息中间件可以自由传递消息，消息的发送者无需知道消息使用者的存在，反之亦然。

AMQP 协议的主要特征是面向消息、队列、路由（包括点对点和发布/订阅）、可靠性、安全，是一种二进制协议，提供客户端应用与消息中间件之间异步、安全、高效地交互。目前，AMQP 1.0 已经成了国际标准，可以直接从官网下载相关的文档。

RabbitMQ 是一个开源的 AMQP 实现，服务器端使用 Erlang 语言编写，支持多种客户端，例如：Python、Ruby、.NET、Java、JMS、C、PHP、ActionScript、XMPP、STOMP 等。

不过 ZMQ 并非基于 AMQP 协议的实现，两者的区别可以参考 [Welcome from AMQP](http://zeromq.org/docs:welcome-from-amqp) 。

### ZMQ 安装

在 CentOS7 中可以直接通过如下命令安装，依赖 EPEL；另外 czmq 是对 ZMQ 的封装。目前 ZMQ 已经到了 Version 4，可以不指定版本号直接安装最新的，或者安装指定的版本。

{% highlight text %}
----- 安装最新的V4版本
# yum --enablerepo=epel install zeromq zeromq-devel czmq

----- 安装V3版本
# yum --enablerepo=epel install zeromq3 zeromq3-devel czmq
{% endhighlight %}

当然可以通过源码进行编译安装，ZMQ 依赖一个加密库 libsodium ，可以直接通过 yum 进行安装。

{% highlight text %}
# yum --enablerepo=epel install libsodium-devel
{% endhighlight %}

源码可以直接从 [Github ZeroMQ](https://github.com/zeromq) 上下载，包括了 libzmq 在内的各种源码。编译也非常简单，直接通过如下方式编译即可。

{% highlight text %}
----- 最简单的编译方法，默认安装在/usr/local目录下
$ ./configure && make
# make install

----- 也可以指定不依赖于sodium库
$ ./configure --prefix=/usr --without-libsodium && make
# make install
{% endhighlight %}

ZMQ 提供了三个基本的通信模型，分别是 "Request-Reply"、"Publisher-Subscriber"、"Parallel Pipeline"，接下来通过示例看看应该如何使用 ZMQ 。

## 示例

相关的文档可以直接参考 [ZMQ - The Guide](http://zguide.zeromq.org/page:all)，而示例程序保存在 [Github Zguide Examples](https://github.com/imatix/zguide/tree/master/examples/)，在此我们将使用 C/C++ 为例，主要以 C++ 为例。

对于 C++ 程序，从 ZMQ-3.2 后，zmq.hpp 被移到了 [cppzmq github](https://github.com/zeromq/cppzmq) 中，可以直接下载 zmq.hpp 文件，然后保存到 /usr/include 目录下即可，或者通过 yum 安装。

{% highlight text %}
# yum --enablerepo=epel install cppzmq-devel
{% endhighlight %}

而对于 C 程序，会依赖于 [czmq github](https://github.com/zeromq/czmq)，在 CentOS 中可以直接通过如下命令安装。

{% highlight text %}
# yum --enablerepo=epel install czmq czmq-devel
{% endhighlight %}

然后可以直接在 zguide 目录下通过如下方式编译。

{% highlight text %}
----- 调转到示例目录下
$ cd examples/C++

----- 编译单个示例程序hwclient
$ ./build hwclient

----- 编译所有的示例程序
$ ./build all
{% endhighlight %}

接下来看看一些示例。

### Request-Reply

一个非常简单的 Hello World 程序，服务端接收客户端请求 "Hello" 字符串，并将 "World" 字符串返回给客户端。

![ZeroMQ Examples Request Reply Mode]({{ site.url }}/images/network/zmq-examples-request-reply.svg "ZeroMQ Examples Request Reply Mode"){: .pull-center}

服务端程序 hwserver.cpp 。

{% highlight cpp %}
#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>

int main (int argc, char *argv[]) {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv (&request);
        std::cout << "Received Hello" << std::endl;

        //  Do some 'work'
    	sleep(1);

        //  Send reply back to client
        zmq::message_t reply (5);
        memcpy (reply.data (), "World", 5);
        socket.send (reply);
    }
    return 0;
}
{% endhighlight %}

客户端程序 hwclient.cpp 。

{% highlight cpp %}
#include <zmq.hpp>
#include <string>
#include <iostream>

int main (int argc, char *argv[]) {
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to hello world server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    //  Do 10 requests, waiting each time for a response
    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        zmq::message_t request (5);
        memcpy (request.data (), "Hello", 5);
        std::cout << "Sending Hello " << request_nbr << "..." << std::endl;
        socket.send (request);

        //  Get the reply.
        zmq::message_t reply;
        socket.recv (&reply);
        std::cout << "Received World " << request_nbr << std::endl;
    }
    return 0;
}
{% endhighlight %}

在这里可以先启动 client，然后再启动 server，效果是相同的，这与传统的 socket 编程不同。不过如果中途 server 宕掉了，重新启动 server 后 client 是不会自动恢复的，对于这种场景处理会比较麻烦，后面再讨论。

另外，ZMQ 通信单元是消息，而且它只知道消息的 bytes 大小，并不关心消息格式，所以，可以自由选择消息格式，例如 XML、Thrift、Msgpack 等。

###  Publish-Subscribe

在此采用的是一个天气预报的订阅模式，由一个节点提供信息源，由其他的节点，接受信息源的信息。

![ZeroMQ Examples Publish Subscribe Mode]({{ site.url }}/images/network/zmq-examples-publish-subscribe.svg "ZeroMQ Examples Publish Subscribe Mode"){: .pull-center}

如下是服务端的代码 wuserver.cpp 。

{% highlight cpp %}
#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

int main (int argc, char *argv[]) {
    //  Prepare our context and publisher
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    publisher.bind("tcp://*:5556");
    publisher.bind("ipc://weather.ipc");             // Not usable on Windows.

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));
    while (1) {
        int zipcode, temperature, relhumidity;

        //  Get values that will fool the boss
        zipcode     = within (100000);
        temperature = within (215) - 80;
        relhumidity = within (50) + 10;

        //  Send message to all subscribers
        zmq::message_t message(20);
        snprintf ((char *) message.data(), 20, "%05d %d %d", zipcode, temperature, relhumidity);
        publisher.send(message);
    }
    return 0;
}
{% endhighlight %}

以及客户端 wuclient.cpp 。

{% highlight cpp %}
#include <zmq.hpp>
#include <iostream>
#include <sstream>

int main (int argc, char *argv[]) {
    zmq::context_t context (1);

    //  Socket to talk to server
    std::cout << "Collecting updates from weather server...\n" << std::endl;
    zmq::socket_t subscriber (context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5556");

    //  Subscribe to zipcode, default is NYC, 10001
    const char *filter = (argc > 1)? argv [1]: "10001 ";
    subscriber.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

    //  Process 100 updates
    int update_nbr;
    long total_temp = 0;
    for (update_nbr = 0; update_nbr < 100; update_nbr++) {
        zmq::message_t update;
        int zipcode, temperature, relhumidity;

        subscriber.recv(&update);
        std::istringstream iss(static_cast<char*>(update.data()));
        iss >> zipcode >> temperature >> relhumidity ;
        total_temp += temperature;
    }
    std::cout << "Average temperature for zipcode '"<< filter
              <<"' was "<<(int) (total_temp / update_nbr) <<"F"
              << std::endl;
    return 0;
}
{% endhighlight %}

简单来说，这里的代码会在服务器端生成随机的 zipcode、temperature、relhumidity，分别代表城市代码、温度值和湿度值，该值服务端会不断的广播消息。客户端会设置过滤参数，只接受特定城市代码的信息，当收集完了以后，也就是 100 个，那么会做一个平均值，并输出。

需要注意的是，这里必须通过 zmq_setsockopt() 设置 ZMQ_SUBSCRIBE，也就是设置一个过滤值，相当于设定一个订阅频道，否则收不到任何消息。

服务端一直在不断的广播，如果中途有 Subscriber 退出，并不会影响服务端的广播，如果 Subscriber 再连接上来时，收到的就是服务端后来发送的信息了。也就是说，对于比较晚加入的，或者是中途离开的订阅者，必然会丢失掉一部分信息，这是这个模式的一个问题。对于 Slow Joiner 的问题，稍后会解决。

如果 Publisher 中途离开，所有的 Subscriber 会 hold 住，待 Publisher 再上线时，会继续接收信息。

### Parallel Pipeline

这一模型通常用在类似于 Map-Reduce 的场景，通常有三部分组成，包括了一个生成可并行处理任务的节点 (ventilator)、多个对任务进行并行处理的节点 (worker)、一个收集处理结果的节点 (sink) 。

![ZeroMQ Examples Parallel Pipeline Mode]({{ site.url }}/images/network/zmq-examples-parallel-pipeline.svg "ZeroMQ Examples Parallel Pipeline Mode"){: .pull-center}

例如，需要统计各个机器的日志，我们可以将统计任务分发到多个节点，最后收集统计结果，做个汇总。

在如下的示例中，taskvent 生成了 100 个任务，然后启动各个 work 之后由各个 work 进行处理，最后由 tasksink 作统计。其中，work 就是接收任务，然后 sleep 一段时间。

生成多个任务 taskvent.cpp 。

{% highlight cpp %}
#include <zmq.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

int main (int argc, char *argv[]) {
    zmq::context_t context (1);

    //  Socket to send messages on
    zmq::socket_t  sender(context, ZMQ_PUSH);
    sender.bind("tcp://*:5557");

    std::cout << "Press Enter when the workers are ready: " << std::endl;
    getchar ();
    std::cout << "Sending tasks to workers...\n" << std::endl;

    //  The first message is "0" and signals start of batch
    zmq::socket_t sink(context, ZMQ_PUSH);
    sink.connect("tcp://localhost:5558");
    zmq::message_t message(2);
    memcpy(message.data(), "0", 1);
    sink.send(message);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));

    //  Send 100 tasks
    int task_nbr;
    int total_msec = 0;     //  Total expected cost in msecs
    for (task_nbr = 0; task_nbr < 100; task_nbr++) {
        int workload;
        //  Random workload from 1 to 100msecs
        workload = within (100) + 1;
        total_msec += workload;

        message.rebuild(10);
        memset(message.data(), '\0', 10);
        sprintf ((char *) message.data(), "%d", workload);
        sender.send(message);
    }
    std::cout << "Total expected cost: " << total_msec << " msec" << std::endl;
    sleep (1);              //  Give 0MQ time to deliver

    return 0;
}
{% endhighlight %}

任务处理程序 taskwork.cpp 。

{% highlight cpp %}
#include "zhelpers.hpp"
#include <string>

int main (int argc, char *argv[]) {
    zmq::context_t context(1);

    //  Socket to receive messages on
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect("tcp://localhost:5557");

    //  Socket to send messages to
    zmq::socket_t sender(context, ZMQ_PUSH);
    sender.connect("tcp://localhost:5558");

    //  Process tasks forever
    while (1) {
        zmq::message_t message;
        int workload;           //  Workload in msecs

        receiver.recv(&message);
        std::string smessage(static_cast<char*>(message.data()), message.size());

        std::istringstream iss(smessage);
        iss >> workload;

        //  Do the work
        s_sleep(workload);

        //  Send results to sink
        message.rebuild();
        sender.send(message);

        //  Simple progress indicator for the viewer
        std::cout << "." << std::flush;
    }
    return 0;
}
{% endhighlight %}

处理结果 tasksink.cpp 。

{% highlight cpp %}
#include <zmq.hpp>
#include <time.h>
#include <sys/time.h>
#include <iostream>

int main (int argc, char *argv[]) {
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t receiver(context,ZMQ_PULL);
    receiver.bind("tcp://*:5558");

    //  Wait for start of batch
    zmq::message_t message;
    receiver.recv(&message);

    //  Start our clock now
    struct timeval tstart;
    gettimeofday (&tstart, NULL);

    //  Process 100 confirmations
    int task_nbr;
    int total_msec = 0;     //  Total calculated cost in msecs
    for (task_nbr = 0; task_nbr < 100; task_nbr++) {
        receiver.recv(&message);
        if ((task_nbr / 10) * 10 == task_nbr)
            std::cout << ":" << std::flush;
        else
            std::cout << "." << std::flush;
    }
    //  Calculate and report duration of batch
    struct timeval tend, tdiff;
    gettimeofday (&tend, NULL);

    if (tend.tv_usec < tstart.tv_usec) {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec - 1;
        tdiff.tv_usec = 1000000 + tend.tv_usec - tstart.tv_usec;
    } else {
        tdiff.tv_sec = tend.tv_sec - tstart.tv_sec;
        tdiff.tv_usec = tend.tv_usec - tstart.tv_usec;
    }
    total_msec = tdiff.tv_sec * 1000 + tdiff.tv_usec / 1000;
    std::cout << "\nTotal elapsed time: " << total_msec << " msec\n" << std::endl;
    return 0;
}
{% endhighlight %}

如上，ventilator 使用的是 SOCKET_PUSH，将任务分发到 Worker；而 Worker 使用 SOCKET_PULL 从上游接受任务，并使用 SOCKET_PUSH 将结果汇集到 Sink。需要注意的是，任务分发时同样有一个负载均衡的路由功能，worker 可以随时自由加入，ventilator 可以均衡将任务分发出去。

## 其它

PGM 是一个可靠且可伸缩的多播协议，通过该协议，接收方可以检测丢失、请求重新传输丢失的数据或者通知应用程序无法恢复的丢失情形；OpenPGM 是一个基于 PGM 协议的实现。


{% highlight text %}
{% endhighlight %}
