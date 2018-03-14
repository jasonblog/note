---
title: Kafka 簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: collectd,monitor,linux
description: 介紹下 Collectd 中源碼的實現。
---


<!-- more -->

## 簡介

Kafka 中可以將 Topic 從物理上劃分成一個或多個分區 (Partition)，每個分區在物理上對應一個文件夾，以 ```topicName_partitionIndex``` 的命名方式命名，該文件夾下存儲這個分區的所有消息 (```.log```) 和索引文件 (```.index```)，這使得 Kafka 的吞吐率可以水平擴展。

在通過命令行創建 Topic 時，可以使用 ```--partitions <numPartitions>``` 參數指定分區數；也可以在 ```server.properties``` 配置文件中配置參數 ```num.partitions``` 來指定默認的分區數。需要注意的是，在為 Topic 創建分區時，分區數最好是 broker 數量的整數倍。

## Kafka 安裝

直接從 [kafka.apache.org](http://kafka.apache.org/downloads.html) 下載二進制安裝包，然後解壓。

### 單機測試

{% highlight text %}
----- 1. 通過腳本啟動單節點Zookeeper實例
$ bin/zookeeper-server-start.sh -daemon config/zookeeper.properties

----- 2. 啟動Kafka服務
$ bin/kafka-server-start.sh config/server.properties >logs/kafka-start.logs 2>&1 &

----- 3. 創建單分區單副本的topic
$ bin/kafka-topics.sh --create --zookeeper localhost:2181 --replication-factor 1 --partitions 1 --topic test
Created topic "test".

----- 4. 查看topic
$ bin/kafka-topics.sh --list --zookeeper localhost:2181
test

----- 5. 發送消息
$ bin/kafka-console-producer.sh --broker-list localhost:9092 --topic test
Hello world！
Hello Kafka！

----- 6. 消費消息，接收消息並在終端打印，--zookeeper參數在逐漸取消，建議使用後者
$ bin/kafka-console-consumer.sh --zookeeper localhost:2181 --topic test --from-beginning
$ bin/kafka-console-consumer.sh --bootstrap-server localhost:9092 --topic test --from-beginning
Hello world!
Hello Kafka!
{% endhighlight %}

### 單機多broker配置

利用單節點部署多個 broker，不同的 broker 設置不同的 id 、監聽端口、日誌目錄。

{% highlight text %}
----- 1. 新增配置文件，修改上述的三個主要配置
$ cp config/server.properties config/server-1.properties
$ cat config/server-1.properties:
    broker.id=1
    port=9093
    log.dir=/tmp/kafka-logs-1

----- 2. 啟動Kafka服務
$ bin/kafka-server-start.sh config/server-1.properties &
{% endhighlight %}

## librdkafka


<!--
Consumer Group
  多個consumer可以組成一個組，每個消息只能被組中的一個consumer消費，如果想一個消息可以被多個consumer消費的話，那麼這些consumer必須在不同的組。
消息狀態
  消息的狀態被保存在consumer中，broker不會關心哪個消息被消費了被誰消費了，只記錄一個offset值（指向partition中下一個要被消費的消息位置），這就意味著如果consumer處理不好的話，broker上的一個消息可能會被消費多次。
    消息有效期：Kafka會長久保留其中的消息，以便consumer可以多次消費，當然其中很多細節是可配置的。
    批量發送：Kafka支持以消息集合為單位進行批量發送，以提高push效率。
    push-and-pull : Kafka中的Producer和consumer採用的是push-and-pull模式，即Producer只管向broker push消息，consumer只管從broker pull消息，兩者對消息的生產和消費是異步的。
    負載均衡方面： Kafka提供了一個 metadata API來管理broker之間的負載（對Kafka0.8.x而言，對於0.7.x主要靠zookeeper來實現負載均衡）。
    同步異步：Producer採用異步push方式，極大提高Kafka系統的吞吐率（可以通過參數控制是採用同步還是異步方式）。
Partition (分區)
  一個Topic下可以有多個Partition，Kafka的broker端支持消息分區，Producer可以決定把消息發到哪個分區，在一個分區中消息的順序就是Producer發送消息的順序。

librdkafka 用 mklove 編譯。

代碼運行流程如下

rd_kafka_conf_set()
  設置全局配置，配置文件中通過Property配置
rd_kafka_topic_conf_set()
  設置topic配置
rd_kafka_brokers_add()
  設置broker地址，也就是bootstrap broker，啟動向broker發送消息的線程
rd_kafka_new()
  將上述的conf作為參數，啟動kafka主線程，也就是rd_kafka_thread_main()函數

rd_kafka_topic_new建topic

rd_kafka_produce使用本函數發送消息

rd_kafka_poll調用回調函數

還是看發送一條消息的過程

#define HAVE_LIBRDKAFKA_LOG_CB 1
#undef HAVE_LIBRDKAFKA_LOGGER

新版本使用 rd_kafka_conf_set_log_cb() 替換了 rd_kafka_set_logger() 接口。

簡單來說，應用線程向隊列扔消息，librdkafka啟動的線程負責從隊列裡取消息並向kafka broker發送消息。

src/rdkafka_transport.c 調用操作系統的 poll() 接口。
-->

<!--
rd_kafka_replyq_enq()
 |-rd_kafka_q_enq()
-->

{% highlight text %}
rd_kafka_produce()                      ← 發送消息
 |-rd_kafka_msg_new()
   |-rd_kafka_msg_new0() 創建消息並初始化，包括消息長度、標記位、timeout等內容
   |-rd_kafka_msg_partitioner() 分區並添加到隊列
   | |-rd_kafka_toppar_get() 獲取partition
   | |-rd_kafka_toppar_enq_msg() 加入隊列，等待broker線程取出
   |   |-rd_kafka_msgq_enq() 真正添加到topic partition隊列中，注意需要進行加鎖
   |     |-TAILQ_INSERT_TAIL() 添加到隊列末尾
   |-rd_kafka_msg_destroy()
rd_kafka_broker_add()

rd_kafka_new() 創建新的實例，會返回rd_kafka_t結構體
 |-rd_kafka_cgrp_new() 如果是消費者會創建分組
 |-rd_kafka_thread_main() 為生產者、消費者啟動單獨線程處理 rd_kafka_thread_main()
 | |-rd_snprintf() 將線程名稱設置為main
 | |-rd_kafka_q_serve() 從rk_ops隊列中讀取消息
 |   |-rd_kafka_op_handle() 處理完成後調用回調函數
 |-rd_kafka_broker_add() 創建內部broker線程，也就是 rd_kafka_broker_thread_main()

rd_kafka_broker_thread_main()
 |-rd_kafka_broker_terminating() 如果沒有關閉
 |-rd_kafka_broker_connect() 在初始化以及STATE_DOWN時，不斷重試鏈接
 | |-rd_kafka_broker_resolve()
 | | |-rd_getaddrinfo()
 | |-rd_kafka_transport_connect() 設置網絡異步通訊
 | | |-rd_fd_set_nonblocking()
 | | |-rd_kafka_transport_poll_set()
 | |-rd_kafka_broker_set_state()
 |
 |-rd_kafka_broker_producer_serve() 處於STATE_UP時根據不同的類型調用不同函數
 | |-rd_kafka_broker_terminating()
 | |-rd_kafka_toppar_producer_serve() 會通過TAILQ_FOREACH()從隊列中讀取，然後調用該函數
     |-rd_kafka_msgq_concat() 如果有需要發送的消息，則直接將消息從rktp->rktp_msgq移動到rktp->rktp_xmit_msgq，並清空前者
     |-rd_kafka_msgq_age_scan()
  |-rd_kafka_broker_produce_toppar()
    |-rd_kafka_buf_new() 新建發送緩衝區
 | |-rd_kafka_broker_serve() 從隊列中讀取消息，並將數據發送到網絡
 |   |-rd_kafka_q_pop() 會返回具體的操作類型
 |   | |-rd_kafka_q_pop_serve()
 |   |-rd_kafka_broker_op_serve() 根據不同的操作類型調用不同函數接口
 |   |
 |   |-rd_kafka_transport_io_serve()
 |     |-rd_kafka_transport_poll() 調用操作系統的poll()系統調用接口
 |     |-rd_kafka_transport_poll_clear()
 |     |-rd_kafka_transport_io_event() 處理返回的IO事件
 |       |-rd_kafka_send() 例如發送
 |       |-rd_kafka_recv() 以及接收
 |-rd_kafka_broker_consumer_serve()
{% endhighlight %}

這也就意味著應用程序將消息發送給 rdkafka，然後 rdkafka 會直接將消息保存到隊列中，並由其它線程異步發送給 broker 。

在調用 ```rd_kafka_new()``` 函數時，會根據入參是消費者還是生產者返回 ```rd_kafka_t``` 對象，此時會同時創建一個主處理線程，也就是 ```rd_kafka_thread_main()``` 。

在主線程中會從 ```rd_kafka_t.rk_ops``` 隊列中逐一讀取消息，然後根據不同的操作類型 (通過rd_kafka_op_type_t定義) 分別進行處理，其中操作通過 rd_kafka_op_t 定義，操作類型對應了其中的 rko_type 成員，操作執行完後調用 rd_kafka_op_handle() 回調。

<!--
可以通過 rd_kafka_broker_add() 創建 broker 線程，有如下三種類型：
    RD_KAFKA_CONFIGURED
    根據用戶配置，生成的broker線程
    RD_KAFKA_LEARNED
    內部使用的broker線程，主要針對Client Group使用
    RD_KAFKA_INTERNAL
    內部使用的broker線程
-->

## 參考

<!--
http://leaver.me/2015/09/03/kafka%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B/
Kafka介紹   http://blog.csdn.net/suifeng3051/article/details/48053965
ZeroCPY  https://www.ibm.com/developerworks/linux/library/j-zerocopy/
Kafka failover 機制詳解 http://www.cnblogs.com/fxjwind/p/4972244.html
很多不錯的圖片 http://zqhxuyuan.github.io/2016/01/13/2016-01-13-Kafka-Picture/

http://blog.csdn.net/auwzb/article/details/9665729
http://www.cnblogs.com/xhcqwl/p/3905412.html
http://codingeek.me/2017/04/16/librdkafka%E6%BA%90%E7%A0%81%E5%88%86%E6%9E%90/
-->


{% highlight text %}
{% endhighlight %}
