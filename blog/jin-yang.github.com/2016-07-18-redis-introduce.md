---
title: Redis 简介
layout: post
comments: true
language: chinese
category: [misc]
keywords: hello world,示例,sample,markdown
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---

REmote DIctionary Server (Redis) 是一个由 Salvatore Sanfilippo 写的 KV 存储系统，采用 C 语言实现，同时提供了多种语言的 API 。

Redis 是完全开源免费的高性能 KV 数据库，相比其它的 KV 数据库来说，Redis 还支持持久化，可以将内存中的数据保存到磁盘中，重启时再次加载；除了 KV 接口，还支持 list、set、hash 等多种数据库结构；同时支持备份以及集群模式。


<!-- more -->

## 简介

Redis 是一个高性能的内存非关系型数据库，如下是一些特性：

* 提供多种持久化方式，可以将内存中的数据保存在磁盘中，并在重启时进行数据恢复。
* 提供了 5 种数据结构类型分别为 STRING、LIST、SET、HASH 和 ZSET (有序集合)，同时提供了原子性操作。
* 支持主备模式的数据备份。
* 支持集群模式。

常用于缓存、队列等场景。

## 常用命令

可以使用命令行直接获取。

{% highlight text %}
----- 通过YUM直接安装
# yum --enablerepo=epel install redis

# redis-cli shutdown

# redis-cli
127.0.0.1:6379> set foo bar
OK
127.0.0.1:6379> get foo
"bar"

----- 查看服务器信息
# redis-cli info

------ 通过管道批量执行
# cat cmds.txt | redis-cli > cmds_out.txt
{% endhighlight %}


### Hash 类型操作

{% highlight text %}
----- 设置单个或者多个值
hset key field value
hmset key field value [field value ...]

----- 获取值
hget key field
hmget key field [key field ...]
hgetall key                             # 获取字段的名称及其值
hvals key                               # 只获取字段对应的值

----- 删除字段，会返回删除字段的个数
hdel key field [field ...]

----- 判断是否存在
hexists key field

----- 当字段不存在时赋值
hsetnx key field value

----- 增加int类型的字段值，每次增加NUM
hincrby key field NUM

----- 其它
hkeys key               # 获取KEY字段名
hlen key                # 获取KEY字段数量
{% endhighlight %}

### List 类型操作

{% highlight text %}
lpush key value [value ...]
lpushx key value                # 插入到头部
lrange key start stop
lpop key
llen key                        # 查看链表中的数量
lrem key count value
lset key index value
lindex key index

ltrim key start stop
{% endhighlight %}

### Set 类型操作

Set 是 string 类型对象的无序集合，对对象操作的时间复杂度是 `O(1)`，最多可以包含 `2^32 - 1` 个对象，使用 SET 操作时，原有的值会被覆盖，即使是不同的类型。

{% highlight text %}
----- 添加成员，返回增加元素的个数，对相同的add，只保存最早的一份
sadd key member [member ...]

----- 获取key的所有成员
smembers key

----- 计算多个set的并集
sunion key [key ...]

----- 计算多个set的交集
sinter key [key ...]
{% endhighlight %}

<!--
sismemeber key member            # 判断是否是set的成员
scard key                        # 返回成员个数
srandmember key                  # 随机返回成员
spop key                         # 移出并返回随机的成员
srem key member [member ...]
smove src dst member             # 将成员从src移动到dst
sunionstore dest key [key ...]   多个set的并集，并保存到dest
-->

另外一种类型就是有序的 SET，也就是 zset，在 set 的基础上增加了顺序，形成一个有序的集合。

## 持久化

Redis 可以将数据同步保存到磁盘做数据持久化，用于重启后的数据恢复，目前主要由两种方式：A) Snapshot(RDB) 默认方式；B) Append Only File(AOF) 方式。

### RDB

默认持久化方式，将当前数据的快照保存成一个数据文件，可以按照一定的策略周期性的将数据保存到磁盘，其对应产生的数据文件为 dump.rdb ，通过配置文件中的 save 参数来定义快照的周期，也可以通过客户端 save/bgsave 手动做快照持久化。

实际上 Redis 借助了 fork 命令的 Copy On Write 机制，在生成快照时，将当前进程 fork 出一个子进程，然后在子进程中循环所有的数据，将数据写成为 RDB 文件。

另外需要注意，save 操作是在主线程中保存快照，而且 Redis 是通过主线程来处理所有客户端请求，因此这种方式会阻塞所有客户端的请求，因此不推荐使用。而且，每次快照持久化都是将内存数据完整写入到磁盘一次，而非增量同步，当数据量大时，会引起大量的磁盘 IO ，从而严重影响性能。

关于 RDB 文件一致性的问题，实际上写操作是在一个新进程中完成的，会先将数据写入到一个临时文件中，然后利用文件系统的原子 rename 操作将文件重命名为 RDB 文件，从而可以保证故障是 RDB 文件的完整性。

#### 配置项

{% highlight text %}
save 60 10000                    # 设置备份频率，也就是多少秒内至少要多个key被修改
stop-writes-on-bgsave-error yes  # 当备份出现错误时停止备份
rdbcompression yes               # 在进行镜像备份时是否压缩，yes压缩，也就是时间换空间
rdbchecksum yes                  # 会在文件末尾添加CRC64校验，当保存或者加载时会有部分的性能损耗
dbfilename dump.rdb              # 快照的文件名
dir /var/lib/redis               # 以及存放快照的目录
{% endhighlight %}

### AOF

Append Only File 持久化会将每个收到的写命令都通过 Write 函数追加到文件最后，类似于 MySQL 的 binlog ，当重启时会重新执行文件中保存的写命令来在内存中重建整个数据库的内容。

这种方式会导致 AOF 文件越来越大，为此提供了 bgrewriteaof 命令可以通过类似 RDB 的方式进行数据保存。

**注意** 如果同时配置了 RBD 和 AOF ，那么启动时只加载 AOF 文件恢复数据。

#### 配置项

{% highlight text %}
appendonly yes                   # 是否开启AOF，默认关闭
appendfilename appendonly.aof    # 指定 AOF 文件名
appendfsync everysec             # 刷新模式总共三种，always每条命令都会刷新，everysec每秒钟刷新一次，no依赖OS刷新机制
no-appendfsync-on-rewrite yes    # 日志重写时不做fsync操作，暂时缓存在内存中
auto-aof-rewrite-percentage 100  # 当前AOF文件是上次重写AOF二倍时，自动启动新的日志重写
auto-aof-rewrite-min-size 64mb   # 启动新日志重写的最小值，避免频繁重写
{% endhighlight %}

### 比较

从上面处理逻辑可以看出，两种方式都是顺序 IO 操作，性能都很高；在恢复时一般也是顺序加载，同样只有很少的随机读。

通常，如果想要提供很高的数据保障性，可以同时使用两种持久化方式；如果可以接受丢失几分钟数据，那么可以仅使用 RDB ，因为 RDB 可以经常做数据的完整快照，并提供更快的重启，所以最好还是使用 RDB 。

RDB 的启动时间更短原因主要有两个：A) 文件中每条数据只有一条记录，而 AOF 对于同一条数据可能需要操作多次；B) RDB 中的数据存储格式跟 Redis 内存编码格式相同，无需编码转换。

注意，在执行备份时，如果会有频繁的更新，可能会导致内存 double 。例如，fork 出来进行 dump 操作的子进程会占用与父进程一样的内存，如果在 dump 期间，内存被修改执行了 Copy-On-Write 操作，最大就可能会占用 12G 内存空间，所以在使用时一定要做好容量规划。

目前，一般的设计思路是利用 Replication 机制来弥补 Snapshot、AOF 在性能上的不足，也就是 Master 不做备份保证性能，在 Slave 则可以同时开启 Snapshot 和 AOF 来进行持久化，保证数据的安全性。

















































## 常用概念

### 数据库

Redis 可以划分逻辑数据库，类似于命名空间，不同的数据库中命名不会冲突，只是不能对其进行命名，只能是通过一个整数索引进行标识(从0开始)，默认连接到 0 ，可以通过 SELECT 选择不同的数据库。

可以在配置文件中，通过 `databases 64` 配置项限制支持的最大数量，默认是 16 个。

在持久化时，会将所有的数据库保存在相同的文件中，但是部分命令可以针对单独的数据库，例如 FLUSHDB、SWAPDB、RANDOMKEY ，而 FLUSHALL 会清除所有 DB 。

实际使用时，建议不要将多个应用通过 DB 隔离。另外需要注意，在集群模式中，只有 0 这个数据库。

## 客户端

### Python

Redis-py 是 Python 中的客户端，可以通过模块与 Redis 服务器进行交互。

{% highlight text %}
----- 可以通过PIP安装
# apt-get install python-pip
# pip install --proxy=http://your-proxy-ip:port redis

----- 或者使用easy_install方式安装
# easy_install redis

----- 也可以通过源码安装
# tar -xf redis-2.10.6.tar.gz && cd redis-2.10.6
# python setup.py install
{% endhighlight %}

详细内容可以参考 https://pypi.python.org/pypi/redis 以及 https://github.com/andymccurdy/redis-py 。

#### 连接池

Redis-py 使用连接池维护与 Redis 服务器的连接，默认每次创建都会返回一个连接池实例，也可以在创建时指定一个现有的连接池。

{% highlight python %}
pool = redis.ConnectionPool(host='localhost', port=6379, db=0)
r = redis.Redis(connection_pool=pool)
{% endhighlight %}

Redis 连接实例是线程安全的，可以直接将连接作为全局变量使用，没有实现 SELECT 接口，如果需要链接到另一个 Redis 实例时，就需要重新创建 Redis 连接实例来获取一个新的连接。

Python 链接库的实现接口与 Redis 提供的 API 略有区别，如上诉的 SELECT 接口。

**注意** 对于链接池，如果使用了 SELECT 命令，那么在返还连接的时候该链接除非显示重新调用 SELECT 指令，否则会导致不一致问题。

#### 基本脚本

{% highlight python %}
import redis
r = redis.Redis(host='localhost',port=6379,db=0)
r.set('foobar', 'Hello World!')
r.get('foobar')
r['foobar']
r.keys()
r.dbsize()         # 当前数据库包含多少条数据
r.delete('guo')
r.save()           # 将内存中的数据写回磁盘，会阻塞
r.flushdb()        # 清空当前数据库中的所有数据
{% endhighlight %}

#### 批量执行

通过管道可以将多个命令通过单个请求发送到服务器，可以有效减少服务器和客户端之间反复的 TCP 数据交互，提高执行批量执行命令。

{% highlight python %}
p = r.pipeline()
p.set('hello','redis')
p.sadd('faz','baz')
p.incr('num')
p.execute()
{% endhighlight %}

甚至也可以将多个命令合并到一起。

{% highlight python %}
p.set('hello','redis').sadd('faz','baz').incr('num').execute()
{% endhighlight %}

默认的情况下，管道里执行的命令会保证原子性，可以在初始化的时候进行设置，也就是通过 `pipe = r.pipeline(transaction=False)` 禁用这一特性。


## 监控

在 Redis 中可以通过 `CONFIG SET` 去修改配置，不过这个只会修改内存，不会持久化到配置文件，为了防止出现不一致，需要通过定期巡检检查是否一致。

Redis 提供了很多命令用户查看当前的执行状态，例如当前客户端信息可以通过 `CLIENT LIST` 命令查看，集群信息可以通过 `CLUSTER INFO` 命令查看等等。

其中使用最多的是通过 INFO 查看系统整体的状态信息，在 Python 中可以通过如下方式采集内存相关状态信息。


<!--
{% highlight text %}
----- 查看KEY时可以使用*,?,[]通配符
keys *         # 查看当前数据库所有的key
type key       # 查看KEY对应的数据类型
monitor        # 打印所有发送到服务器的命令

exists foo   查看是否存在
del foo      删除对象
move foo 1   移动到ID为1数据库
select 1     打开ID为1的数据库
rename foo hello  重命名

----- 超时时间设置相关
expire key seconds      # 设置将该健值的超时时间，单位为秒
expireat key timestamp  # 将该健值超时时间设置为具体的时间点
ttl foo                 # 查看剩余时间
persist foo             # 取消超时时间

randomkey       随机返回任意健值
flushdb         清空所有数据
{% endhighlight %}

需要注意的是，当使用 KEYS 命令查看时，Redis 都会遍厉数据库中的所有 KEY，也就意味着该操作会非常耗时，如果有类似的需求的时候，最后将这些值存放到 set 类型变量中。
-->

## 参考

中文社区 [www.redis.cn](http://www.redis.cn/)，[C 客户端hiredis](https://github.com/redis/hiredis)，[Redis.io 命令参考](https://redis.io/commands) 。

[twemproxy](https://github.com/twitter/twemproxy) 一个 Redis 和 Memcached 的代理，由 twitter 开发并开源。

<!--
Redis Cookbook

http://debugo.com/python-redis/

http://redisdoc.com/server/info.html
Redis运维集合
http://www.redisfans.com/?p=55
http://www.ywnds.com/?cat=63
Redis PipeLine 性能优化
https://redis.io/topics/pipelining
CAP 理论
http://blog.csdn.net/dc_726/article/details/42784237
通过集群，Redis 完成了从 CP 到 AP 的转换，详细介绍可以参考，在使用集群之前，一般采用Sentinel方案。
https://redis.io/topics/cluster-tutorial
Redis Cluster设计总览
http://blog.csdn.net/dc_726/article/details/48552531
-->

{% highlight text %}
{% endhighlight %}
