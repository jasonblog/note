---
title: ETCD 简介
layout: post
comments: true
language: chinese
category: [program,golang,linux]
keywords: golang,go,etcd
description: Etcd 是一个分布式可靠的键值存储系统，提供了与 ZooKeeper 相似的功能，通过 GoLang 开发而非 Java ，采用 RAFT 算法而非 PAXOS 算法。相比来所，etcd 的安装使用更加简单有效。
---

Etcd 是一个分布式可靠的键值存储系统，提供了与 ZooKeeper 相似的功能，通过 GoLang 开发而非 Java ，采用 RAFT 算法而非 PAXOS 算法。

相比来所，etcd 的安装使用更加简单有效。

<!-- more -->

## 简介

A distributed, reliable key-value store for the most critical data of a distributed system.

严格来说，ETCD 主要用于保存一些元数据信息，一般小于 1GB 对大于 1GB 的可以使用新型的分布式数据库，例如 TiDB 等，通常适用于 CP 场景。

## 安装

可以直接从 [github release](https://github.com/coreos/etcd/releases) 下载非源码包，也就是已经编译好的二进制包，一般包括了 etcd + etcdctl 。

#### 单机单进程测试

启动单进程服务，并进行测试。

{% highlight text %}
----- 启动单个本地进程，会监听127.1:2379端口
$ ./etcd

----- 使用API v3版本，并测试添加获取参数
$ export ETCDCTL_API=3
$ ./etcdctl put foo bar
OK
$ ./etcdctl get foo
foo
bar

$ ./etcdctl --write-out=table --endpoints=localhost:2379 member list

----- 只打印值信息，不打印key
$ ./etcdctl get foo --print-value-only
bar
----- 打印十六进制格式
$ ./etcdctl get foo --hex
\x66\x6f\x6f
\x62\x61\x72
----- 指定范围为foo~foo3
$ ./etcdctl get foo foo3
foo
foo1
foo2
foo3
----- 指定前缀，且只显示前两个
$ ./etcdctl get --prefix --limit=2 foo
foo
foo1
{% endhighlight %}



<!--
--name
  集群中节点名，可区分且不重复即可；
--listen-peer-urls
监听的用于节点之间通信的url，可监听多个，集群内部将通过这些url进行数据交互(如选举，数据同步等)
--initial-advertise-peer-urls
建议用于节点之间通信的url，节点间将以该值进行通信。
--listen-client-urls
监听的用于客户端通信的url,同样可以监听多个。
--advertise-client-urls
建议使用的客户端通信url,该值用于etcd代理或etcd成员与etcd节点通信。
--initial-cluster-token etcd-cluster-1
节点的token值，设置该值后集群将生成唯一id,并为每个节点也生成唯一id,当使用相同配置文件再启动一个集群时，只要该token值不一样，etcd集群就不会相互影响。
--initial-cluster
也就是集群中所有的initial-advertise-peer-urls 的合集
--initial-cluster-state new
新建集群的标志

./etcd --name infra0 --initial-advertise-peer-urls http://10.0.1.111:2380 \
  --listen-peer-urls http://10.0.1.111:2380 \
  --listen-client-urls http://10.0.1.111:2379,http://127.0.0.1:2379 \
  --advertise-client-urls http://10.0.1.111:2379 \
  --initial-cluster-token etcd-cluster-1 \
  --initial-cluster infra0=http://10.0.1.111:2380,infra1=http://10.0.1.109:2380,infra2=http://10.0.1.110:2380 \
  --initial-cluster-state new

./etcd --name infra1 --initial-advertise-peer-urls http://10.0.1.109:2380 \
  --listen-peer-urls http://10.0.1.109:2380 \
  --listen-client-urls http://10.0.1.109:2379,http://127.0.0.1:2379 \
  --advertise-client-urls http://10.0.1.109:2379 \
  --initial-cluster-token etcd-cluster-1 \
  --initial-cluster infra0=http://10.0.1.111:2380,infra1=http://10.0.1.109:2380,infra2=http://10.0.1.110:2380 \
  --initial-cluster-state new

./etcd --name infra2 --initial-advertise-peer-urls http://10.0.1.110:2380 \
  --listen-peer-urls http://10.0.1.110:2380 \
  --listen-client-urls http://10.0.1.110:2379,http://127.0.0.1:2379 \
  --advertise-client-urls http://10.0.1.110:2379 \
  --initial-cluster-token etcd-cluster-1 \
  --initial-cluster infra0=http://10.0.1.111:2380,infra1=http://10.0.1.109:2380,infra2=http://10.0.1.110:2380 \
  --initial-cluster-state new

按如上配置分别启动集群，启动集群后，将会进入集群选举状态，若出现大量超时，则需要检查主机的防火墙是否关闭，或主机之间是否能通过2380端口通信，集群建立后通过以下命令检查集群状态。

数据持久化基于多版本并发控制，


性能压测的核心指标包括了：延迟(Latency)、吞吐量(Throughput)；延迟表示完成一次请求的处理时间；吞吐量表示某个时间段内完成的请求数。

一般来说，当吞吐量增加增加时，对应的延迟也会同样增加；低负载时 1ms 以内可以完成一次请求，高负载时可以完成 3W/s 的请求。对于 ETCD 来说，会涉及到一致性以及持久化的问题，所以其性能与网络带宽、磁盘IO的性能紧密相关。

其底层的存储基于 BoltDB，一个基于 GoLang 的 KV 数据库，支持 ACID 特性，与 lmdb 相似。

https://coreos.com/etcd/docs/latest/op-guide/performance.html
-->


<!--
configuration management, service discovery, and coordinating distributed work. Many organizations use etcd to implement production systems such as container schedulers, service discovery services, and distributed data storage. Common distributed patterns using etcd include leader election, distributed locks, and monitoring machine liveness.
-->


## API

实际上 API 基本上决定了 etcd 提供了哪些服务，通过 HTTP API 对外提供服务，这种接口更方便各种语言对接，命令行可以使用 httpie 或者 curl 调用。

数据按照树形的结构组织，类似于 Linux 的文件系统，也有目录和文件的区别，不过一般被称为 nodes，其中数据相关的 endpoint 都是以 `/v2/keys` 开头 (v2 表示当前 API 的版本)，比如 `/v2/keys/names/cizixs` 。

要创建一个值，只要使用 PUT 方法在对应的 url endpoint 设置就行。如果对应的 key 已经存在， PUT 也会对 key 进行更新。

### CURD

{% highlight text %}
----- 不存在则创建，否则修改，当超过TTL后，会自动删除
http PUT http://127.0.0.1:2379/v2/keys/message value=="hello, etcd" ttl==5
http GET http://127.0.0.1:2379/v2/keys/message
http DELETE http://127.0.0.1:2379/v2/keys/message
{% endhighlight %}

在创建 key 的时候，如果它所在路径的目录不存在，会自动被创建，所以在多数情况下我们不需要关心目录的创建，如果要单独创建一个目录可以指定参数 `dir=true`。

{% highlight text %}
http PUT http://127.0.0.1:2379/v2/keys/anotherdir dir==true
{% endhighlight %}

注意，ECTD 提供了类似 Linux 中 `.` 开头的隐藏机制，以 `_` 开头的节点也是默认隐藏的，不会在列出目录的时候显示，只有知道隐藏节点的完整路径，才能够访问它的信息。


### 监听机制

通过监听机制，可以在某个 key 发生变化时，通知对应的客户端，主要用于服务发现，集群中有信息更新时可以被及时发现并作出相应处理。

{% highlight text %}
http http://127.0.0.1:2379/v2/keys/foo wait==true
{% endhighlight %}

使用 `recursive=true` 参数，可以用来监听某个目录。

### 比较更新

在分布式环境中，需要解决多个客户端的竞争问题，通过 etcd 提供的原子操作 CompareAndSwap (CAS)，可以很容易实现分布式锁。简单来说，这个命令只有在客户端提供的条件成立的情况下才会更新对应的值。

{% highlight text %}
http PUT http://127.0.0.1:2379/v2/keys/foo prevValue==bar value==changed
{% endhighlight %}

只有当之前的值为 bar 时，才会将其更新成 changed 。

### 比较删除

同样是原子操作，只有在客户端提供的条件成立的情况下，才会执行删除操作；支持 prevValue 和 prevIndex 两种条件检查，没有 prevExist，因为删除不存在的值本身就会报错。

{% highlight text %}
http DELETE http://127.0.0.1:2379/v2/keys/foo prevValue==bar
{% endhighlight %}

### 监控集群

Etcd 还保存了集群的数据信息，包括节点之间的网络信息，操作的统计信息。

<!--
/v2/stats/leader  会返回集群中 leader 的信息，以及 followers 的基本信息
/v2/stats/self 会返回当前节点的信息
/v2/state/store：会返回各种命令的统计信息
-->

### 成员管理

在 `/v2/members` 下保存着集群中各个成员的信息。

<!--
/version   获取版本服务器以及集群的版本号
-->


## 常见操作


### etcdctl

这个实际上是封装了 HTTP 请求的一个客户端，用于更方便的与服务端进行交互。

{% highlight text %}
----- 设置一个key的值
$ etcdctl set /message "hello, etcd"
hello, etcd

----- 获取key的值
$ etcdctl get /message
hello, etcd

----- 获取key值的同时，显示更详细的元数据信息
$ etcdctl -o extended get /message
Key: /message
Created-Index: 1073
Modified-Index: 1073
TTL: 0
Index: 1073

hello, etcd

----- 如果获取的key不存在，则会直接报错
$ etcdctl get /notexist
Error:  100: Key not found (/notexist) [1048]

----- 设置key的ttl，过期后会被自动删除
$ etcdctl set /tempkey "gone with wind" --ttl 5
gone with wind

----- 如果key的值是"hello, etcd"，就把它替换为"goodbye, etcd"
$ etcdctl set --swap-with-value "hello, world" /message "goodbye, etcd"
Error:  101: Compare failed ([hello, world != hello, etcd]) [1050]
$ etcdctl set --swap-with-value "hello, etcd" /message "goodbye, etcd"
goodbye, etcd

----- 仅当key不存在时创建
$ etcdctl mk /foo bar
bar
$ etcdctl mk /foo bar
Error:  105: Key already exists (/foo) [1052]

----- 自动创建排序的key
$ etcdctl mk --in-order /queue job1
job1
$ etcdctl mk --in-order /queue job2
job2
$ etcdctl ls --sort /queue
/queue/00000000000000001053
/queue/00000000000000001054

----- 更新key的值或者ttl，只有当key已经存在的时候才会生效，否则报错
$ etcdctl update /message "I'am changed"
I'am changed
$ etcdctl get /message
I'am changed
$ etcdctl update /notexist "I'am changed"
Error:  100: Key not found (/notexist) [1055]
$ etcdctl update --ttl 3 /message "I'am changed"
I'am changed
$ etcdctl get /message
Error:  100: Key not found (/message) [1057]

----- 删除某个key
$ etcdctl mk /foo bar
bar
$ etcdctl rm /foo
PrevNode.Value: bar
$ etcdctl get /foo
Error:  100: Key not found (/foo) [1062]

----- 只有当key的值匹配的时候，才进行删除
$ etcdctl mk /foo bar
bar
$ etcdctl rm --with-value wrong /foo
Error:  101: Compare failed ([wrong != bar]) [1063]
$ etcdctl rm --with-value bar /foo

----- 创建一个目录
$ etcdctl mkdir /dir

----- 删除空目录
$ etcdctl mkdir /dir/subdir/
$ etcdctl rmdir /dir/subdir/

----- 删除非空目录
$ etcdctl rmdir /dir
Error:  108: Directory not empty (/dir) [1071]
$ etcdctl rm --recursive /dir

----- 列出目录的内容
$ etcdctl ls /
/queue
/anotherdir
/message

----- 递归列出目录的内容
$ etcdctl ls --recursive /
/anotherdir
/message
/queue
/queue/00000000000000001053
/queue/00000000000000001054

----- 监听某个key，当key改变的时候会打印出变化
$ etcdctl watch /message
changed

----- 监听某个目录，当目录中任何node改变的时候，都会打印出来
$ etcdctl watch --recursive /
[set] /message
changed

----- 一直监听，除非CTRL + C导致退出监听
$ etcdctl watch --forever /message
new value
chaned again
Wola

----- 监听目录，并在发生变化的时候执行一个命令
$ etcdctl exec-watch --recursive / -- sh -c "echo change detected."
change detected.
change detected.
{% endhighlight %}

**注意** 默认只保存了 1000 个历史事件，所以不适合有大量更新操作的场景，这样会导致数据的丢失，其使用的典型应用场景是配置管理和服务发现，这些场景都是读多写少的。

## 参考


<!--
https://tonydeng.github.io/2015/10/19/etcd-application-scenarios/
http://jolestar.com/etcd-architecture/
http://www.infoq.com/cn/articles/etcd-interpretation-application-scenario-implement-principle

场景一：服务发现（Service Discovery）
场景二：消息发布与订阅
场景三：负载均衡
场景四：分布式通知与协调
场景五：分布式锁、分布式队列
场景六：集群监控与Leader竞选
-->

{% highlight text %}
{% endhighlight %}
