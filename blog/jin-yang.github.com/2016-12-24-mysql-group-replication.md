---
title: MySQL 组复制
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,组复制,group replication
description: MySQL 当前存在异步复制、半同步复制，在 5.7.17 引入了组复制模式，这是基于分布式一致性算法 (Paxos 协议的变体) 实现。一个组允许部分节点挂掉，只要保证多数节点仍然存活并且相互之间可以正常通讯，那么这个组仍然可以对外提供服务，是目前一种被分布式系统广泛使用的技术。
---

MySQL 当前存在异步复制、半同步复制，在 5.7.17 引入了组复制模式，这是基于分布式一致性算法 (Paxos 协议的变体) 实现。

一个组允许部分节点挂掉，只要保证多数节点仍然存活并且相互之间可以正常通讯，那么这个组仍然可以对外提供服务，是目前一种被分布式系统广泛使用的技术。

如下，仅简单介绍如何使用。

<!-- more -->

## 介绍


MySQL 组复制实现基于插件，建立在现有的 MySQL 复制基础结构上，利用了 binlog、GTID、row-based logging、InnoDB、Corosync(三方)；注意最新版本的已经将 Corosync 替换掉。

<!--
仅支持InnoDB表，并且每张表一定要有一个主键，用于做write set的冲突检测；
必须打开GTID特性，二进制日志格式必须设置为ROW，用于选主与write set
COMMIT可能会导致失败，类似于快照事务隔离级别的失败场景
目前一个MGR集群最多支持9个节点
不支持外键于save point特性，无法做全局间的约束检测与部分部分回滚
二进制日志不支持binlog event checksum
-->

## 搭建集群

首先，需要确认当前 MySQL 版本是否支持组复制；实际上，可以简单查看 plugin_dir 目录下是否存在 group_replication.so 动态库即可。

如下是一个简单的配置文件，需要注意的是，默认将 ```group_replication_start_on_boot``` 参数关闭，在第一个服务器启动时打开该参数。

{% highlight text %}
# only the last two sub-sections are directly related to Group Replication
[mysqld]
server-id      = 2
datadir        = /var/lib/mysql
socket         = /var/lib/mysql/mysql.sock
pid-file       = /tmp/mysql.pid
log-error      = mysqld.log

# replication and binlog related options
binlog-row-image          = MINIMAL           # 取消后镜像，减小binlog大小
relay-log-recovery        = ON                # 备库恢复时从relaylog获取位点信息
sync-relay-log            = 1                 # 每个事务同时将位点信息持久化
sync-master-info          = 1000
slave-parallel-workers    = 4                 # 设置备库的并发
slave-parallel-type       = LOGICAL_CLOCK     # 利用组提交的逻辑值做并发
binlog-rows-query-log-events    = ON          # ROW模式binlog添加SQL信息，方便排错
log-bin-trust-function-creators = ON          # 同时复制主库创建的函数
slave-preserve-commit-order     = ON          # 备库的事务提交时，保持与主库相同顺序
log-slave-updates               = ON          # 备库同时生成binlog

slave-rows-search-algorithms = 'INDEX_SCAN,HASH_SCAN'
slave-type-conversions       = ALL_NON_LOSSY

# group replication pre-requisites &amp; recommendations
log-bin                   = mysql-bin         # 开启binlog
binlog-format             = ROW               # 目前只支持ROW模式
gtid-mode                 = ON                # 设置如下两个参数启动GTID
enforce-gtid-consistency  = ON
master-info-repository    = TABLE             # 主库信息保存在表中
relay-log-info-repository = TABLE             # relaylog信息保存在表中
binlog-checksum           = NONE              # 取消checksum
transaction-isolation     = 'READ-COMMITTED'  # 间隙锁可能会存在问题

# prevent use of non-transactional storage engines
disabled_storage_engines  = "MyISAM,BLACKHOLE,FEDERATED,ARCHIVE"

# group replication specific options
plugin-load         = group_replication.so    # 加载组复制的插件
group_replication   = FORCE_PLUS_PERMANENT
transaction-write-set-extraction  = XXHASH64  # MURMUR32,NONE
group_replication_start_on_boot   = OFF
group_replication_bootstrap_group = OFF
group_replication_group_name      = 550fa9ee-a1f8-4b6d-9bfe-c03c12cd1c72
group_replication_local_address   = '127.0.0.1:3307'
group_replication_group_seeds     = '127.0.0.1:3307,127.0.0.1:3308,127.0.0.1:3309'
{% endhighlight %}

InnoDB 的间隙锁在进行多主的冲突检测时存在问题，因此最好不要使用 REPEATABLE-READ 隔离级别，而是使用 READ-COMMITTED 隔离级别。

<!--
# InnoDB gap locks are problematic for multi-primary conflict detection; none are used with READ-COMMITTED
# So if you don't rely on REPEATABLE-READ semantics and/or wish to use multi-primary mode then this
# isolation level is recommended
-->

如下提供了一个自动搭建集群的脚本，可以在 /tmp 目录下搭建三个 MySQL 服务，详见脚本 [build-group-replication.sh](/reference/databases/mysql/build-group-replication.sh) 。

搭建完之后，可以通过如下命令查看当前组的成员。

{% highlight text %}
mysql> SELECT * FROM performance_schema.replication_group_members;
{% endhighlight %}


## 参考

关于 Group Replication 的基本介绍可以参考 [High Availability Using MySQL Group Replication (PPT)](https://downloads.mysql.com/presentations/innovation-day-2016/Session_7_MySQL_Group_Replication_for_High_Availability.pdf)，或者 [本地文档](/reference/databases/mysql/MySQL_Group_Replication_for_High_Availability.pdf) 。对于 MySQL 的 Group Replication 介绍，演进过程，设计文档等，可以参考 [mysqlhighavailability.com](http://mysqlhighavailability.com/tag/mysql-group-replication/)、[MySQL Group Replication’s documentation!](http://mysqlhighavailability.com/mysqlha/gr/doc/index.html)

MySQL Group Replication 的实现可以参考论文 [The Database State Machine Approach (PDF)](http://www.inf.usi.ch/faculty/pedone/Paper/2002/2002DPDT.pdf) 或者 [本地文档](/reference/databases/mysql/The_Database_State_Machine_Approach.pdf) 。


<!--
http://siddontang.com/2015/02/02/mysql-replication-protocol/
http://wangwei007.blog.51cto.com/68019/1893703


http://mysqlhighavailability.com/tag/mysql-group-replication/page/4/
http://mysqlhighavailability.com/mysql-group-replication-a-quick-start-guide/
http://mysqlhighavailability.com/getting-started-with-mysql-group-replication/
https://github.com/sjmudd/mysql-x-protocol-specification/blob/master/mysql-x-protocol-specification-draft.txt

-->

{% highlight text %}
{% endhighlight %}
