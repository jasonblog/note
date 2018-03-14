---
title: ZeroMQ 簡介
layout: post
comments: true
language: chinese
category: [network]
keywords: zeromq
description: ZMQ (Zero Message Queue) 是一個 C++ 編寫的高性能分佈式消息隊列，是一個非常簡單好用的傳輸層，使得 Socket 編程更加簡單、簡潔和性能更高效。目前比較常用的消息中間件產品包括了 RabbitMQ、ZeroMQ、ActiveMQ，三者分別通過　Erlang、C++、Java 實現，相比而言，ZMQ 是一個簡單的庫，而非單獨的中間件產品。接下來簡單介紹一下 ZMQ 的相關內容。
---

ZMQ (Zero Message Queue) 是一個 C++ 編寫的高性能分佈式消息隊列，是一個非常簡單好用的傳輸層，使得 Socket 編程更加簡單、簡潔和性能更高效。

目前比較常用的消息中間件產品包括了 RabbitMQ、ZeroMQ、ActiveMQ，三者分別通過　Erlang、C++、Java 實現，相比而言，ZMQ 是一個簡單的庫，而非單獨的中間件產品。

接下來簡單介紹一下 ZMQ 的相關內容。

<!-- more -->

## 簡介

[ZMQ (Zero Message Queue)](http://zeromq.org/) 的資料非常全，相關的資料基本都保存在 [zeromq.org - community](http://zeromq.org/community) 中，例如一些經常使用的鏈接包括了示例 [ZMQ - The Guide](http://zguide.zeromq.org/page:all)、相關的設計文檔 [Interesting Whitepapers](http://zeromq.org/area:whitepapers)、及各種語言的示例程序 [Github Zguide Examples](https://github.com/imatix/zguide/tree/master/examples/) 。

ZMQ 的作者是 Martin Sustrik，其中的絕大多數的代碼是由他來維護的，可以看下源碼中的 MAINTAINERS 文件。另外，他還維護了：[nanomsg](http://nanomsg.org ) 封裝了網絡編程中常見的模型、[libmill](http://libmill.org) 基於 C 的類 Go 庫，據說可以支持 2KW 個協程，每秒 5KW 次上下文切換 (沒有給出硬件配置，不過如果屬實，性能確實有點恐怖)。

### Advanced Message Queuing Protocol

[AMQP](http://www.amqp.org/)，高級消息隊列協議，一個應用層的開放標準協議，為面向消息的中間件而設計，主要用於各個組件之間的解耦，基於該協議的客戶端與消息中間件可以自由傳遞消息，消息的發送者無需知道消息使用者的存在，反之亦然。

AMQP 協議的主要特徵是面向消息、隊列、路由（包括點對點和發佈/訂閱）、可靠性、安全，是一種二進制協議，提供客戶端應用與消息中間件之間異步、安全、高效地交互。目前，AMQP 1.0 已經成了國際標準，可以直接從官網下載相關的文檔。

RabbitMQ 是一個開源的 AMQP 實現，服務器端使用 Erlang 語言編寫，支持多種客戶端，例如：Python、Ruby、.NET、Java、JMS、C、PHP、ActionScript、XMPP、STOMP 等。

不過 ZMQ 並非基於 AMQP 協議的實現，兩者的區別可以參考 [Welcome from AMQP](http://zeromq.org/docs:welcome-from-amqp) 。

### ZMQ 安裝

在 CentOS7 中可以直接通過如下命令安裝，依賴 EPEL；另外 czmq 是對 ZMQ 的封裝。目前 ZMQ 已經到了 Version 4，可以不指定版本號直接安裝最新的，或者安裝指定的版本。

{% highlight text %}
----- 安裝最新的V4版本
# yum --enablerepo=epel install zeromq zeromq-devel czmq

----- 安裝V3版本
# yum --enablerepo=epel install zeromq3 zeromq3-devel czmq
{% endhighlight %}

當然可以通過源碼進行編譯安裝，ZMQ 依賴一個加密庫 libsodium ，可以直接通過 yum 進行安裝。

{% highlight text %}
# yum --enablerepo=epel install libsodium-devel
{% endhighlight %}

源碼可以直接從 [Github ZeroMQ](https://github.com/zeromq) 上下載，包括了 libzmq 在內的各種源碼。編譯也非常簡單，直接通過如下方式編譯即可。

{% highlight text %}
----- 最簡單的編譯方法，默認安裝在/usr/local目錄下
$ ./configure && make
# make install

----- 也可以指定不依賴於sodium庫
$ ./configure --prefix=/usr --without-libsodium && make
# make install
{% endhighlight %}

ZMQ 提供了三個基本的通信模型，分別是 "Request-Reply"、"Publisher-Subscriber"、"Parallel Pipeline"，接下來通過示例看看應該如何使用 ZMQ 。

## 示例

相關的文檔可以直接參考 [ZMQ - The Guide](http://zguide.zeromq.org/page:all)，而示例程序保存在 [Github Zguide Examples](https://github.com/imatix/zguide/tree/master/examples/)，在此我們將使用 C/C++ 為例，主要以 C++ 為例。

對於 C++ 程序，從 ZMQ-3.2 後，zmq.hpp 被移到了 [cppzmq github](https://github.com/zeromq/cppzmq) 中，可以直接下載 zmq.hpp 文件，然後保存到 /usr/include 目錄下即可，或者通過 yum 安裝。

{% highlight text %}
# yum --enablerepo=epel install cppzmq-devel
{% endhighlight %}

而對於 C 程序，會依賴於 [czmq github](https://github.com/zeromq/czmq)，在 CentOS 中可以直接通過如下命令安裝。

{% highlight text %}
# yum --enablerepo=epel install czmq czmq-devel
{% endhighlight %}

然後可以直接在 zguide 目錄下通過如下方式編譯。

{% highlight text %}
----- 調轉到示例目錄下
$ cd examples/C++

----- 編譯單個示例程序hwclient
$ ./build hwclient

----- 編譯所有的示例程序
$ ./build all
{% endhighlight %}

接下來看看一些示例。

### Request-Reply

一個非常簡單的 Hello World 程序，服務端接收客戶端請求 "Hello" 字符串，並將 "World" 字符串返回給客戶端。

![ZeroMQ Examples Request Reply Mode]({{ site.url }}/images/network/zmq-examples-request-reply.svg "ZeroMQ Examples Request Reply Mode"){: .pull-center}

服務端程序 hwserver.cpp 。

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

客戶端程序 hwclient.cpp 。

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

在這裡可以先啟動 client，然後再啟動 server，效果是相同的，這與傳統的 socket 編程不同。不過如果中途 server 宕掉了，重新啟動 server 後 client 是不會自動恢復的，對於這種場景處理會比較麻煩，後面再討論。

另外，ZMQ 通信單元是消息，而且它只知道消息的 bytes 大小，並不關心消息格式，所以，可以自由選擇消息格式，例如 XML、Thrift、Msgpack 等。

###  Publish-Subscribe

在此採用的是一個天氣預報的訂閱模式，由一個節點提供信息源，由其他的節點，接受信息源的信息。

![ZeroMQ Examples Publish Subscribe Mode]({{ site.url }}/images/network/zmq-examples-publish-subscribe.svg "ZeroMQ Examples Publish Subscribe Mode"){: .pull-center}

如下是服務端的代碼 wuserver.cpp 。

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

以及客戶端 wuclient.cpp 。

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

簡單來說，這裡的代碼會在服務器端生成隨機的 zipcode、temperature、relhumidity，分別代表城市代碼、溫度值和溼度值，該值服務端會不斷的廣播消息。客戶端會設置過濾參數，只接受特定城市代碼的信息，當收集完了以後，也就是 100 個，那麼會做一個平均值，並輸出。

需要注意的是，這裡必須通過 zmq_setsockopt() 設置 ZMQ_SUBSCRIBE，也就是設置一個過濾值，相當於設定一個訂閱頻道，否則收不到任何消息。

服務端一直在不斷的廣播，如果中途有 Subscriber 退出，並不會影響服務端的廣播，如果 Subscriber 再連接上來時，收到的就是服務端後來發送的信息了。也就是說，對於比較晚加入的，或者是中途離開的訂閱者，必然會丟失掉一部分信息，這是這個模式的一個問題。對於 Slow Joiner 的問題，稍後會解決。

如果 Publisher 中途離開，所有的 Subscriber 會 hold 住，待 Publisher 再上線時，會繼續接收信息。

### Parallel Pipeline

這一模型通常用在類似於 Map-Reduce 的場景，通常有三部分組成，包括了一個生成可並行處理任務的節點 (ventilator)、多個對任務進行並行處理的節點 (worker)、一個收集處理結果的節點 (sink) 。

![ZeroMQ Examples Parallel Pipeline Mode]({{ site.url }}/images/network/zmq-examples-parallel-pipeline.svg "ZeroMQ Examples Parallel Pipeline Mode"){: .pull-center}

例如，需要統計各個機器的日誌，我們可以將統計任務分發到多個節點，最後收集統計結果，做個彙總。

在如下的示例中，taskvent 生成了 100 個任務，然後啟動各個 work 之後由各個 work 進行處理，最後由 tasksink 作統計。其中，work 就是接收任務，然後 sleep 一段時間。

生成多個任務 taskvent.cpp 。

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

任務處理程序 taskwork.cpp 。

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

處理結果 tasksink.cpp 。

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

如上，ventilator 使用的是 SOCKET_PUSH，將任務分發到 Worker；而 Worker 使用 SOCKET_PULL 從上游接受任務，並使用 SOCKET_PUSH 將結果彙集到 Sink。需要注意的是，任務分發時同樣有一個負載均衡的路由功能，worker 可以隨時自由加入，ventilator 可以均衡將任務分發出去。

## 其它

PGM 是一個可靠且可伸縮的多播協議，通過該協議，接收方可以檢測丟失、請求重新傳輸丟失的數據或者通知應用程序無法恢復的丟失情形；OpenPGM 是一個基於 PGM 協議的實現。


{% highlight text %}
{% endhighlight %}
