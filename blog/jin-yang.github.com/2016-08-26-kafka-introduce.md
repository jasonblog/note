---
title: Kafka 简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: collectd,monitor,linux
description: 介绍下 Collectd 中源码的实现。
---


<!-- more -->

## 简介

Kafka 中可以将 Topic 从物理上划分成一个或多个分区 (Partition)，每个分区在物理上对应一个文件夹，以 ```topicName_partitionIndex``` 的命名方式命名，该文件夹下存储这个分区的所有消息 (```.log```) 和索引文件 (```.index```)，这使得 Kafka 的吞吐率可以水平扩展。

在通过命令行创建 Topic 时，可以使用 ```--partitions <numPartitions>``` 参数指定分区数；也可以在 ```server.properties``` 配置文件中配置参数 ```num.partitions``` 来指定默认的分区数。需要注意的是，在为 Topic 创建分区时，分区数最好是 broker 数量的整数倍。

## Kafka 安装

直接从 [kafka.apache.org](http://kafka.apache.org/downloads.html) 下载二进制安装包，然后解压。

### 单机测试

{% highlight text %}
----- 1. 通过脚本启动单节点Zookeeper实例
$ bin/zookeeper-server-start.sh -daemon config/zookeeper.properties

----- 2. 启动Kafka服务
$ bin/kafka-server-start.sh config/server.properties >logs/kafka-start.logs 2>&1 &

----- 3. 创建单分区单副本的topic
$ bin/kafka-topics.sh --create --zookeeper localhost:2181 --replication-factor 1 --partitions 1 --topic test
Created topic "test".

----- 4. 查看topic
$ bin/kafka-topics.sh --list --zookeeper localhost:2181
test

----- 5. 发送消息
$ bin/kafka-console-producer.sh --broker-list localhost:9092 --topic test
Hello world！
Hello Kafka！

----- 6. 消费消息，接收消息并在终端打印，--zookeeper参数在逐渐取消，建议使用后者
$ bin/kafka-console-consumer.sh --zookeeper localhost:2181 --topic test --from-beginning
$ bin/kafka-console-consumer.sh --bootstrap-server localhost:9092 --topic test --from-beginning
Hello world!
Hello Kafka!
{% endhighlight %}

### 单机多broker配置

利用单节点部署多个 broker，不同的 broker 设置不同的 id 、监听端口、日志目录。

{% highlight text %}
----- 1. 新增配置文件，修改上述的三个主要配置
$ cp config/server.properties config/server-1.properties
$ cat config/server-1.properties:
    broker.id=1
    port=9093
    log.dir=/tmp/kafka-logs-1

----- 2. 启动Kafka服务
$ bin/kafka-server-start.sh config/server-1.properties &
{% endhighlight %}

## librdkafka


<!--
Consumer Group
  多个consumer可以组成一个组，每个消息只能被组中的一个consumer消费，如果想一个消息可以被多个consumer消费的话，那么这些consumer必须在不同的组。
消息状态
  消息的状态被保存在consumer中，broker不会关心哪个消息被消费了被谁消费了，只记录一个offset值（指向partition中下一个要被消费的消息位置），这就意味着如果consumer处理不好的话，broker上的一个消息可能会被消费多次。
    消息有效期：Kafka会长久保留其中的消息，以便consumer可以多次消费，当然其中很多细节是可配置的。
    批量发送：Kafka支持以消息集合为单位进行批量发送，以提高push效率。
    push-and-pull : Kafka中的Producer和consumer采用的是push-and-pull模式，即Producer只管向broker push消息，consumer只管从broker pull消息，两者对消息的生产和消费是异步的。
    负载均衡方面： Kafka提供了一个 metadata API来管理broker之间的负载（对Kafka0.8.x而言，对于0.7.x主要靠zookeeper来实现负载均衡）。
    同步异步：Producer采用异步push方式，极大提高Kafka系统的吞吐率（可以通过参数控制是采用同步还是异步方式）。
Partition (分区)
  一个Topic下可以有多个Partition，Kafka的broker端支持消息分区，Producer可以决定把消息发到哪个分区，在一个分区中消息的顺序就是Producer发送消息的顺序。

librdkafka 用 mklove 编译。

代码运行流程如下

rd_kafka_conf_set()
  设置全局配置，配置文件中通过Property配置
rd_kafka_topic_conf_set()
  设置topic配置
rd_kafka_brokers_add()
  设置broker地址，也就是bootstrap broker，启动向broker发送消息的线程
rd_kafka_new()
  将上述的conf作为参数，启动kafka主线程，也就是rd_kafka_thread_main()函数

rd_kafka_topic_new建topic

rd_kafka_produce使用本函数发送消息

rd_kafka_poll调用回调函数

还是看发送一条消息的过程

#define HAVE_LIBRDKAFKA_LOG_CB 1
#undef HAVE_LIBRDKAFKA_LOGGER

新版本使用 rd_kafka_conf_set_log_cb() 替换了 rd_kafka_set_logger() 接口。

简单来说，应用线程向队列扔消息，librdkafka启动的线程负责从队列里取消息并向kafka broker发送消息。

src/rdkafka_transport.c 调用操作系统的 poll() 接口。
-->

<!--
rd_kafka_replyq_enq()
 |-rd_kafka_q_enq()
-->

{% highlight text %}
rd_kafka_produce()                      ← 发送消息
 |-rd_kafka_msg_new()
   |-rd_kafka_msg_new0() 创建消息并初始化，包括消息长度、标记位、timeout等内容
   |-rd_kafka_msg_partitioner() 分区并添加到队列
   | |-rd_kafka_toppar_get() 获取partition
   | |-rd_kafka_toppar_enq_msg() 加入队列，等待broker线程取出
   |   |-rd_kafka_msgq_enq() 真正添加到topic partition队列中，注意需要进行加锁
   |     |-TAILQ_INSERT_TAIL() 添加到队列末尾
   |-rd_kafka_msg_destroy()
rd_kafka_broker_add()

rd_kafka_new() 创建新的实例，会返回rd_kafka_t结构体
 |-rd_kafka_cgrp_new() 如果是消费者会创建分组
 |-rd_kafka_thread_main() 为生产者、消费者启动单独线程处理 rd_kafka_thread_main()
 | |-rd_snprintf() 将线程名称设置为main
 | |-rd_kafka_q_serve() 从rk_ops队列中读取消息
 |   |-rd_kafka_op_handle() 处理完成后调用回调函数
 |-rd_kafka_broker_add() 创建内部broker线程，也就是 rd_kafka_broker_thread_main()

rd_kafka_broker_thread_main()
 |-rd_kafka_broker_terminating() 如果没有关闭
 |-rd_kafka_broker_connect() 在初始化以及STATE_DOWN时，不断重试链接
 | |-rd_kafka_broker_resolve()
 | | |-rd_getaddrinfo()
 | |-rd_kafka_transport_connect() 设置网络异步通讯
 | | |-rd_fd_set_nonblocking()
 | | |-rd_kafka_transport_poll_set()
 | |-rd_kafka_broker_set_state()
 |
 |-rd_kafka_broker_producer_serve() 处于STATE_UP时根据不同的类型调用不同函数
 | |-rd_kafka_broker_terminating()
 | |-rd_kafka_toppar_producer_serve() 会通过TAILQ_FOREACH()从队列中读取，然后调用该函数
     |-rd_kafka_msgq_concat() 如果有需要发送的消息，则直接将消息从rktp->rktp_msgq移动到rktp->rktp_xmit_msgq，并清空前者
     |-rd_kafka_msgq_age_scan()
  |-rd_kafka_broker_produce_toppar()
    |-rd_kafka_buf_new() 新建发送缓冲区
 | |-rd_kafka_broker_serve() 从队列中读取消息，并将数据发送到网络
 |   |-rd_kafka_q_pop() 会返回具体的操作类型
 |   | |-rd_kafka_q_pop_serve()
 |   |-rd_kafka_broker_op_serve() 根据不同的操作类型调用不同函数接口
 |   |
 |   |-rd_kafka_transport_io_serve()
 |     |-rd_kafka_transport_poll() 调用操作系统的poll()系统调用接口
 |     |-rd_kafka_transport_poll_clear()
 |     |-rd_kafka_transport_io_event() 处理返回的IO事件
 |       |-rd_kafka_send() 例如发送
 |       |-rd_kafka_recv() 以及接收
 |-rd_kafka_broker_consumer_serve()
{% endhighlight %}

这也就意味着应用程序将消息发送给 rdkafka，然后 rdkafka 会直接将消息保存到队列中，并由其它线程异步发送给 broker 。

在调用 ```rd_kafka_new()``` 函数时，会根据入参是消费者还是生产者返回 ```rd_kafka_t``` 对象，此时会同时创建一个主处理线程，也就是 ```rd_kafka_thread_main()``` 。

在主线程中会从 ```rd_kafka_t.rk_ops``` 队列中逐一读取消息，然后根据不同的操作类型 (通过rd_kafka_op_type_t定义) 分别进行处理，其中操作通过 rd_kafka_op_t 定义，操作类型对应了其中的 rko_type 成员，操作执行完后调用 rd_kafka_op_handle() 回调。

<!--
可以通过 rd_kafka_broker_add() 创建 broker 线程，有如下三种类型：
    RD_KAFKA_CONFIGURED
    根据用户配置，生成的broker线程
    RD_KAFKA_LEARNED
    内部使用的broker线程，主要针对Client Group使用
    RD_KAFKA_INTERNAL
    内部使用的broker线程
-->

## 参考

<!--
http://leaver.me/2015/09/03/kafka%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B/
Kafka介绍   http://blog.csdn.net/suifeng3051/article/details/48053965
ZeroCPY  https://www.ibm.com/developerworks/linux/library/j-zerocopy/
Kafka failover 机制详解 http://www.cnblogs.com/fxjwind/p/4972244.html
很多不错的图片 http://zqhxuyuan.github.io/2016/01/13/2016-01-13-Kafka-Picture/

http://blog.csdn.net/auwzb/article/details/9665729
http://www.cnblogs.com/xhcqwl/p/3905412.html
http://codingeek.me/2017/04/16/librdkafka%E6%BA%90%E7%A0%81%E5%88%86%E6%9E%90/
-->


{% highlight text %}
{% endhighlight %}
