---
title: MySQL Crash-Safe 复制
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,crash-safe,replication,复制
description: 简单来说，crash-safe replication 是指当主备库任何一个节点发生宕机等意外情况下，服务器重启后主备的数据依然能够保证一致性。接下来看看 MySQL 中是如何实现的。
---

简单来说，crash-safe replication 是指当主备库任何一个节点发生宕机等意外情况下，服务器重启后主备的数据依然能够保证一致性。

接下来看看 MySQL 中是如何实现的。

<!-- more -->

## 简介

对于主库来说，crash-safe master 比较简单，只要使用事务的存储引擎，并且正确的配置就能达到 crash safe 的效果，对于 InnoDB 可以在配置文件中添加如下的设置，也即 ```"双1" 模式``` 。

{% highlight text %}
sync_binlog                    = 1
innodb-flush-log-at-trx-commit = 1
{% endhighlight %}

在 MySQL 5.6 版本之前存在一个 bug，即当启用上述两个参数时，会使得 InnoDB 存储引擎的 group commit 失效，从而导致在写密集的环境中性能的急剧下降。

MariaDB 真正解决了该问题，因此很多分支版本，比如 Percona、InnoSQL 都将 MariaDB 的 group commit 方案移植到了自己的分支中，解决 group commit 失效的问题。

<!--
因此，DBA在性能和数据一致性中做了妥协，通常将参数innodb-flush-log-at-trx-commit设置为2，而这就导致了master不再是crash safe的，主从数据可能会不一致。

MariaDB真正解决了该问题，因此很多分支版本，比如Percona，Facebook MySQL，InnoSQL都将MariaDB的group commit方案移植到了自己的分支中，从而解决group commit失效的问题。
-->

### crash-safe slave

这种情况要复杂的多，而且是 DBA 常见的问题，例如备库不断的报 1062 错误 (主健冲突)，或者发现主从数据不一致，特别是表没有主键的情况；这时，没有太好的办法，基本就是全库重建了。

首先，说明下在这里用到的概念，我们将 event 在 binlog 所在的文件名以及其在文件中的偏移量称为位点。
 
备库分别有两个线程负责复制 (IO-thread) + 回放 (SQL-thread)，前者会读取主库 binlog 并写入到备库的 relay-log 中，该线程会记录需要读取主库下个位点的信息，并将该信息记录到 master.info 文件中(包含如下的两个信息)；而后者则负责从 relay-log 中读取日志，并将位点信息更新到 relay-log.info 文件中(包含如下的四个信息)。

当前备库的信息可以通过如下方式查看。

{% highlight text %}
mysql> SHOW SLAVE STATUS;
*************************** 1. row ***************************
                          ... ...
              Master_Log_File: mysql-bin.000002       IO-thread读取主库binlog的文件名
          Read_Master_Log_Pos: 447                    IO-thread读取主库binlog的文件偏移
               Relay_Log_File: relay-bin.000002       SQL-thread读取relay-log的文件名
                Relay_Log_Pos: 320                    SQL-thread读取relay-log的文件偏移
        Relay_Master_Log_File: mysql-bin.000002       SQL-thread读取的事件对应主库的文件名
                          ... ...
          Exec_Master_Log_Pos: 447                    SQL-thread读取的事件对应主库的文件偏移
                          ... ...
1 row in set (0.00 sec)
{% endhighlight %}

这样，如果 IO-thread 崩溃后，而位点信息没有同步写入到文件中，那么就可能导致 event 被重复添加到 relay-log 中；而对于 SQL-thread 则会将 event 重复执行！！！

对于 INSERT 来说，在重启备库时就可能会报 "Duplicate entry for key PRIMARY" 的错误，而对于 DELETE、UPDATE 等场景会更加复杂，而且可能更加难识别。

早在 MySQL 4.0 时 Google 就发布过 [补丁](https://code.google.com/p/google-mysql-tools/wiki/TransactionalReplication) 解决该问题，也就是在每次 InnoDB 提交时，将 binlog 的位置信息到事务系统段的段头；当备库重启时，将保存的这部分的信息重新生成 relay-info.log 文件。


## 解决方案

在 5.5 中，增加了 ```sync_master_info=1``` 以及 ```sync_relay_log_info=1``` 两个参数，用来控制每次更新位点文件时，同时进行一次 fdatasync 操作，确保每个事务都会同时将位点刷新到日志中。<!--(这种对于write-back cache比较有效)-->

不过，即使这样，仍然存在小概率发生崩溃，因为一般是在事务提交之后，将位点信息写入文件；这样如果恰好在事务提交之后，但未写入文件之前崩溃，同样会导致最后一个事务的位点异常。

为此，5.6 提供了两个参数 ```relay_log_info_repository=TABLE``` 和 ```master_info_repository=TABLE```，用于将相关参数分别保存在 mysql 库的 slave_relay_log_info 和 slave_master_info 两个表中；而且，之前的数据提交方式将做如下的修改。

{% highlight text %}
START TRANSACTION;
-- Statement 1
-- ...
-- Statement N
COMMIT;
-- Update replication info files
{% endhighlight %}

修改为：

{% highlight text %}
START TRANSACTION;
-- Statement 1
-- ...
-- Statement N
-- Update replication info
COMMIT;
{% endhighlight %}

实际上，对于 SQL-thread 来说是有效的，因为在提交事务的时候同时将位点信息写入到了表中，保证了操作的原子性，进而可以保持一致性。

### IO-thread 线程

但对于 IO-thread 仍然会有问题，因为写文件+写表仍然是两个操作，这两个操作不是在一个事务中，一个是数据库操作，一个是文件操作，因此不能达到原子的效果，从而还是无法保证一致性。

为此，5.6 给出的解决方案是：

1. 将 IO-thread 的初始位点设置为与 SQL-thread 相同；
2. SQL-thread 顺序执行到 relay-log 文件末尾。

上述功能需要开启 ```relay_log_recovery=ON``` 配置，此时 IO 线程启动时，会从 ```slave_relay_log_info``` 表中读取需要读取的位点信息，然后从主库拉取数据，从而无需设置 ```master_info_repository=TABLE``` 。

注意，实际上 IO-thread 是从 SQL-thread 保存的位点开始重新从主库获取 event 。

## 更多讨论

通过上述的方式，虽然可以解决更新 relay-info.log 的原子性问题，但是还存在一些缺陷。

由于最后表 slave_relay_log_info 的更新会锁住记录，从而导致备库上的事务提交都是串行的。虽然 MySQL 5.6 之后支持并行复制，但由于串行更新表 slave_relay_log_info，再次导致 group commit 失效。

再通过 \-\-log-slave-updates 级联复制的话，性能又会受限。


### 关于relay_log_recovery

当设置该参数之后，将会存在如下的问题：

1. 每次启动备库时，都会从备库拉取数据，如果 SQL 线程已经落后很多，那么就会导致从主库复制大量的数据；
2. 如果同时主库也宕机了，那么即使备库有大量的 binlog ，SQL 线程也不会去处理；
3. 如果主库中的 binlog 已经被清理了，备库同样也无法进行恢复；
4. 如果想要在备库备份 relay-log 的话，也即将 relay-log-purge 设置为 0，那么重启或者崩溃恢复将会导致备份的 relay-log 文件被损坏。

对于第一个问题，当设置了延迟复制时，对于网络的压力将会更加明显。

### DDL

对于 DDL 语句，即使设置了 binlog 为 ROW 模式，实际上仍保存为 SQL 语句，而且不会触发刷盘操作，这样就会导致 DDL 操作，仍然有可能会重复执行。

例如，当你修改了表结构以后，突然宕机，slave_relay_log_info 表没刷进磁盘，下次重启服务后，会再次执行一次修改表结构，此时同步就挂了，只能手工去跳过这个错误。

![replication sync-relay-log-info]({{ site.url }}/images/databases/mysql/replication-sync-relay-log-info.png "replication sync-relay-log-info"){: .pull-center }

此时，可以在备库中添加如下的参数。

{% highlight text %}
slave_skip_errors = ddl_exist_errors
{% endhighlight %}

## 总结

简言之，需要设置如下两个参数。

{% highlight text %}
relay_log_info_repository = TABLE
relay_log_recovery = ON
{% endhighlight %}

表默认为 MyISAM 存储引擎，为了保证事务，需要将表设置为 InnoDB 。

{% highlight text %}
mysql> ALTER TABLE mysql.slave_master_info    ENGINE = InnoDB;
mysql> ALTER TABLE mysql.slave_relay_log_info ENGINE = InnoDB;
mysql> ALTER TABLE mysql.slave_worker_info    ENGINE = InnoDB;
{% endhighlight %}


<!--
注意，当主备配置好之后，重启备库时不需要通过 change master 重新配置。

从库上执行
    STOP SLAVE IO_THREAD;
    SHOW PROCESSLIST;
其中，确保SQL-thread状体为"Slave has read all relay log"，然后再执行：
    STOP SLAVE;    停SQL_THREAD
    RESET MASTER;  清空主库的binlog，包括index文件，从头开始记录，show master status
    RESET SLAVE;   清空备库relaylog、index，以及主备关系(master.info、relay-log.info)，并从头开始记录show slave status\G
    show master status;

11.3 主库上执行
    RESETMASTER;
    RESETSLAVE;

    CHANGEMASTERTO
    MASTER_HOST='192.168.119.129',
    MASTER_USER='repl',
    MASTER_PASSWORD='1234',
    MASTER_LOG_FILE='mysql-bin.000001',
    MASTER_LOG_POS=106;
    startslave

{% highlight text %}
init_recovery()
{% endhighlight %}
-->

## 参考

关于主备复制中备库的常用配置项可以参考 [Reference Manual - Replication Slave Options and Variables](https://dev.mysql.com/doc/refman/en/replication-options-slave.html) 。

关于 Crash-safe replication 的最佳实践，可以参考 [Better Crash-safe replication for MySQL](https://blog.booking.com/better_crash_safe_replication_for_mysql.html) 中的介绍，或者 [本地文档](/reference/databases/mysql/Better Crash-safe replication for MySQL.mht) 。


<!--
https://www.percona.com/blog/2013/09/13/enabling-crash-safe-slaves-with-mysql-5-6/

在主库中，binlog的事件如何发送给dump线程？备库中IO线程如何通知SQL线程执行？
binlog的文件格式是什么样的？
-->






 
 
 


{% highlight text %}
{% endhighlight %}
