---
title: MySQL 复制方式
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,复制
description: MySQL 的复制包括了多种方式，一种是基于 Binlog 的原生复制方式，除此之外，还包括了通过插件实现的 semi-sync 复制。另外，针对 InnoDB 实现的 xtrabackup 也可以作为一种复制方式。在本文中，简单介绍下 MySQL 中的复制方式。
---

MySQL 的复制包括了多种方式，一种是基于 Binlog 的原生复制方式，除此之外，还包括了通过插件实现的 semi-sync 复制。另外，针对 InnoDB 实现的 xtrabackup 也可以作为一种复制方式。

在本文中，简单介绍下 MySQL 中的复制方式。

<!-- more -->

![mysql replication logo]({{ site.url }}/images/databases/mysql/replication-logo.jpg){: .pull-center width="60%"}

## 简介

MySQL 在复制时，可以指定要忽略的数据库、需要复制的数据库甚至具体那些表；支持异步复制、半同步复制、同步复制 (NDB Cluster, Group Replication)、延迟复制等模式。

在进行复制的时候，有两种格式： Statement Based、Row Based，也可以是两者的组合，在配置文件中通过 binlog_format 参数进行设置；后面再介绍与格式相关的内容。

其中，MySQL 的复制原理如下图所示。

![mysql basic replication]({{ site.url }}/images/databases/mysql/replication-basic.png){: .pull-center width="50%"}

在主服务器上，会将数据的更新写入到 binary log 中，而备服务器会从该文件中读取对数据的更改；每次备服务器链接到主时，都会分配一个单独的线程进行处理；该线程会将 binlog 产生的事件发送到备服务器。

通常来说，主服务器会直接从缓存中读取 binlog ，所以不会对磁盘造成压力；但是，如果读取的数据是半小时，甚至更长事件之前的数据，那么就会不可避免的发生磁盘 IO 。

### 备服务器

在备服务器上有两个线程，分别是 IO Thread 以及 SQL Thread 。

IO Thread 线程会从主服务器读取数据，然后保存到本地的日志文件 relay log，该线程当前的状态可以通过 show slave status 查看。

SQL thread 会读取本地的 relay log，然后将相应的语句写入到数据库。


### 延迟复制

当发生延迟复制 (Replication Lag) 时，通常是由于 SQL 线程延迟导致的，当然，最好是通过 show slave status 查看两个线程的状态。如果是 IO 线程导致，最好是打开压缩协议，减小网络 IO 的消耗量。

如果是 SQL 线程导致的，那么会比较复杂一些，需要根据具体的情况排查。

### reset 命令

简单介绍下常用的相关的命令。

#### RESET MASTER

用于删除所有的 binglog 日志文件，并将日志索引文件清空，重新开始所有新的日志文件；通常用于第一次进行搭建主从库时，进行 binlog 初始化工作。
 
#### RESET SLAVE

用于删除 SLAVE 数据库的 relaylog 日志文件，并重新启用新的 relaylog 文件；如果使用 ALL 参数，同时会清理相关的主库信息。

通常用于在主备切换时，为了防止备库异常链接原主库导致数据不一致，需要清理与主库的链接信息，保证新主库不会再链接到原主库，为此，提供了 ```RESET SLAVE``` 命令。

但是不同的 MySQL 版本，对应的命令可能会有所区别，简述如下：

{% highlight text %}
MySQL 5.0+5.1 执行 STOP SLAVE; CHANGE MASTER TO MASTER_HOST=''; RESET SLAVE；
MySQL 5.5+5.6 执行 STOP SLAVE; RESET SLAVE ALL；
{% endhighlight %}

对于所有版本都严禁设置 ```master-user```、```master-host```、```master-password``` 参数，当然在 MySQL 5.5 之后的版本不再支持这些参数了。

<!--
LINKKK: https://www.percona.com/blog/2013/04/17/reset-slave-vs-reset-slave-all-disconnecting-a-replication-slave-is-easier-with-mysql-5-5/
-->

### 相关文件

除了上述的二进制日志和中继日志文件外，还有其它一些与复制相关的文件。

#### *.index

服务器一旦开启二进制日志，会产生一个与二日志文件同名，但是以 .index 结尾的文件；该文件用于跟踪磁盘上存在哪些二进制日志文件，MySQL 用它来定位二进制日志文件。

与 binlog 相似，对于中继日志，同样存在一个索引文件。

#### *.info

一般为 master.info 以及 relay-log.info，前者保存 master 的相关信息，slave 重启后会通过该信息连接 master；后者包含备中当前二进制日志和中继日志的信息。

注意，上述设置需要保证在配置文件中添加如下配置。

{% highlight text %}
[mysqld]
master_info_repository     = FILE
relay_log_info_repository  = FILE
{% endhighlight %}

也可以将上述的值设置为 TABLE ，此时会将上述的数据保存在 mysql 库的 slave_master_info 以及 slave_relay_log_info 两个表中；存放表里有两个好处：

1. 明文存储在表中相比文件存储要安全很多；
2. 可避免 relay.info 更新不及时，SLAVE 重启后导致的主从复制出错。

上述保存的表默认采用 MyISAM 存储引擎，官方建议改为 InnoDB 引擎，防止表损坏后自行修复。

{% highlight text %}
mysql> ALTER TABLE slave_master_info    Engine=InnoDB;
mysql> ALTER TABLE slave_relay_log_info Engine=InnoDB;
mysql> ALTER TABLE slave_worker_info    Engine=InnoDB;
{% endhighlight %}

另外，可以开启如下两个参数，这两个是启用 relaylog 的自动修复功能，避免由于网络之类的外因造成日志损坏，导致主从停止。

{% highlight text %}
relay_log_purge = ON
relay_log_recovery = ON
{% endhighlight %}













<!--
在谈这个特性之前，我们先来看看MySQL的复制架构衍生史。MySQL的复制分为四种：

1、普通的replication，异步。

搭建简单，使用非常广泛，从MySQL诞生之初，就产生了这种架构，性能非常好，可谓非常成熟。但是这种架构数据是异步的，所以有丢失数据库的风险。

2、semi-sync replication，半同步。

性能，功能都介于异步和全同步中间。从mysql5.5开始诞生，目的是为了折中上述两种架构的性能以及优缺点。

3、sync replication，同步。

目前官方MySQL 5.7.17基于Group replication的全同步技术已经问世，全同步技术带来了更多的数据一致性保障。相信是未来同步技术一个重要方向，值得期待。MySQL 5.7 Group Replication
https://dev.mysql.com/doc/refman/5.7/en/group-replication.html

4、mysql cluster，同步。

基于NDB引擎，搭建也简单，本身也比较稳定，是MySQL里面对数据保护最靠谱的架构，也是目前唯一一个数据完全同步的架构，数据零丢失。不过对业务比较挑剔，限制也较多。MySQL NDB CLUSTER
https://dev.mysql.com/doc/refman/5.7/en/mysql-cluster.html

根据这几种复制协议，分别对应MySQL几种复制类型，分别是异步、半同步、同步。
-->


## 测试实例

在此介绍下一些常见的数据复制方式，包括了执行的步骤，以及相关示例。

### 搭建步骤

整体步骤如下。

##### 1. 增加配置

如果要使用 MySQL 的复制，则必须要有如下的两个配置项。

{% highlight text %}
[mysqld]
log-bin=mysql-bin             # 开启binlog日志
server-id=1                   # 范围1~(2^32-1)
{% endhighlight %}

##### 2. 创建用户

备库链接到主库时，需要通过指定帐号链接，且有 REPLICATION SLAVE 权限；在使用时，可以多个备库使用不同帐号密码，也可以使用相同的帐号密码。

{% highlight text %}
mysql> CREATE USER 'replication'@'%.foobar.com' IDENTIFIED BY 'slave-password';
mysql> GRANT REPLICATION SLAVE ON *.* TO 'replication'@'%.foobar.com';
{% endhighlight %}

##### 3. 获取位置

{% highlight text %}
----- 在一个会话中锁表，对与InnoDB会阻塞commit
mysql> FLUSH TABLES WITH READ LOCK;
----- 新建另外一个会话，查看当前的File+Position
mysql> SHOW MASTER STATUS;
{% endhighlight %}

如果已存在数据，对于 InnoDB 可以通过 mysqldump 或者 mysqlbackup 进行历史备份。

{% highlight text %}
----- 其中--master-data会自动添加CHANGE MASTER TO命令
$ mysqldump --all-databases --master-data > dbdump.db
{% endhighlight %}

<!--
https://dev.mysql.com/doc/refman/5.7/en/replication-snapshot-method.html
-->

##### 4. 配置主库信息

在备库上执行如下命令，相关信息会保存在 master.info 文件中。

{% highlight text %}
mysql> CHANGE MASTER TO
           MASTER_HOST='master_host_name',
           MASTER_USER='replication_user_name',
           MASTER_PASSWORD='replication_password',
           MASTER_LOG_FILE='recorded_log_file_name',
           MASTER_LOG_POS=recorded_log_position;
{% endhighlight %}

### 常用命令

简单记录下常用的命令。

{% highlight text %}
mysql> SHOW SLAVE STATUS\G                    ← 查看slave详细信息
mysql> HELP CHANGE MASTER TO;                 ← 查看帮助
mysql> CHANGE MASTER TO
       master_host='localhost',               ← 主服务器地址
       master_port=3307,                      ← 主服务器端口，无引号
       master_user='mysync',                  ← 连接到主服务器的用户
       master_password='kidding',             ← 连接到主服务器的用户密码
       master_log_file='mysql-bin.000003',    ← 指定从主那个binlog文件开始复制
       master_log_pos=496;                    ← 从主binlog的指定位置开始复制，无引号
mysql> START SLAVE;
mysql> START SLAVE IO_THREAD;                 ← 也可以使用sql_thread
mysql> STOP SLAVE;                            ← 也可以停止

----- 查看binlog日志信息
$ mysqlbinlog -v --base64-output=decode-rows mysql-bin.000019
{% endhighlight %}

<!--
master_connect_retry      #连接重试时间;
master_SSL_Allowed        #是否开启基于SSL的复制;
master_SSL_CA_File        #指定CA文件;
master_SSL_CA_Path        #指定CA文件路径;
master_SSL_Cert           #指定证书;
master_SSL_Cipher         #加密算法;
master_SSL_Key            #私钥文件;
-->

在执行 `CHANGE MASTER TO` 命令时，可以只修改部分命令参数，例如只修改同步的位置信息。

如果不指定 `master_log_file` 和 `master_log_pos` 参数，则会从头开始复制；但是如果已经有很多数据了，可以通过 `mysqldump` 导出，并记录二进制文件以及位置。



#### 重置复制

在如下的测试中，可以通过下面的方式重置复制。

{% highlight text %}
----- 备库上执行，停止复制，等待SQL线程执行完成之后都停止
mysql> STOP SLAVE IO_THREAD;
Query OK, 0 rows affected, 1 warning (0.00 sec)
mysql> STOP SLAVE;
Query OK, 0 rows affected, 1 warning (0.00 sec)

----- 备库上执行，重置备库
mysql> RESET SLAVE;              # 删除master.info和relay-log.info，但会保留同步信息
Query OK, 0 rows affected, 1 warning (0.00 sec)
mysql> RESET SLAVE ALL;          # 会彻底清除备库所有信息，包括同步信息
Query OK, 0 rows affected, 1 warning (0.00 sec)

----- 主库上执行，重置主库
mysql> RESET MASTER ALL;
Query OK, 0 rows affected, 1 warning (0.00 sec)
{% endhighlight %}

然后，根据不同的复制方式重新建立链接。

{% highlight text %}
----- 对于常规的异步方式或者半同步方式
mysql> SHOW MASTER STATUS;

mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_log_file='mysql-bin.000003',master_log_pos=496;

----- 对于GTID模式
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_auto_position = 1;
{% endhighlight %}





<!--

RESET MASTER

删除所有index file中记录的所有binlog文件，将日志索引文件清空，创建一个新的日志文件，这个命令通常仅仅用于第一次用于搭建主从关系的时的主库，

注意

reset master不同于purge binary log的两处地方

1）reset master将删除日志索引文件中记录的所有binlog文件，创建一个新的日志文件起始值从000001开始，然而purge binary log命令并不会修改记录binlog的顺序的数值。

2）reset master不能用于有任何slave正在运行的主从关系的主库，因为在slave运行时刻reset master命令不被支持，reset master将master的binlog从000001开始记录，slave记录的master log则是reset master时主库的最新的binlog，从库会报错无法找的指定的binlog文件。

In MySQL 5.6.5 and later, RESET MASTER also clears the values of the gtid_purged system variable (known as gtid_lost in MySQL 5.6.8 and earlier) as well as the global value of the gtid_executed (gtid_done, prior to MySQL 5.6.9) system variable (but not its session value); that is, executing this statement sets each of these values to an empty string (”)

RESET SLAVE

RESET SLAVE将使slave忘记主从复制关系的位置信息。该语句将被用于干净的启动，它删除master.info文件和relay-log.info文件以及所有的relay log文件并重新启用一个新的relaylog文件。

使用reset slave之前必须使用stop slave 命令将复制进程停止。

注：所有的relay log将被删除不管他们是否被SQL thread进程完全应用（这种情况发生于备库延迟以及在备库执行了stop slave 命令），存储复制链接信息的master.info文件将被立即清除，如果SQL thread 正在复制临时表的过程中，执行了stop slave ，并且执行了reset slave，这些被复制的临时表将被删除。

RESET SLAVE ALL

在5.6版本中reset slave并不会清理存储于内存中的复制信息，比如 master host，master port，master user，or master password，也就是说如果没有使用change master命令做重新定向，执行start slave还是会指向旧的master上面。

当从库执行reset slave之后,将mysqld shutdown 复制参数将被重置。

在5.6.3 版本以及以后使用使用RESET SLAVE ALL来完全的清理复制连接参数信息。(Bug #11809016)

RESET SLAVE ALL does not clear the IGNORE_SERVER_IDS list set by CHANGE MASTER TO. This issue is fixed in MySQL 5.7. (Bug #18816897)
In MySQL 5.6.7 and later, RESET SLAVE causes an implicit commit of an ongoing transaction. See Section 13.3.3, “Statements That Cause an Implicit Commit”.
-->






### 主备复制

在此，我们会在同一台机器的 /tmp 目录下部署两个主备实例；当然，这只是一个示例，也只是为了测试而已。

<!--
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-master
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-slave
-->

{% highlight text %}
----- 1.1 安装主备两个实例
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-master --user=mysql
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-slave  --user=mysql

----- 1.2 修改配置文件，其中log-bin必须设置，server-id需要不同
$ cat /tmp/mysql-master/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-master.sock
pid-file        = /tmp/mysql-master.pid
datadir         = /tmp/mysql-master
port            = 3307
server-id       = 1
$ cat /tmp/mysql-slave/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-slave.sock
pid-file        = /tmp/mysql-slave.pid
datadir         = /tmp/mysql-slave
port            = 3308
server-id       = 2
relay_log_index = relay-bin.index
relay_log       = relay-bin
report_host     = 127.1
report_port     = 3308

----- 2.1 分别启动主备服务器
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-master/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-master > /dev/null 2>&1 &
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-slave/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-slave  > /dev/null 2>&1 &

----- 2.2.0 登陆
$ mysql -p -P3307 -uroot -S/tmp/mysql-master.sock
$ mysql -p -P3308 -uroot -S/tmp/mysql-slave.sock
----- 2.2.1 修改密码
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new-password';
----- 2.2.2 然后用新密码登陆
$ mysql -p'new-password' -P3307 -uroot -S/tmp/mysql-master.sock
$ mysql -p'new-password' -P3308 -uroot -S/tmp/mysql-slave.sock

----- 3.1 配置主服务器，需要新建一个用户
mysql> GRANT REPLICATION SLAVE ON *.* to 'mysync'@'localhost' IDENTIFIED BY 'kidding';
mysql> SHOW MASTER STATUS;            # 查看相应的File以及Position
----- 3.2 配置从服务器，然后启动
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_log_file='mysql-bin.000003',master_log_pos=496;
mysql> START SLAVE;
mysql> STOP SLAVE;                    # 也可以停止

----- 4. 关闭数据库
$ mysqladmin -uroot -S /tmp/mysql-master.sock shutdown
$ mysqladmin -uroot -S /tmp/mysql-slave.sock shutdown
{% endhighlight %}

现在基本就在同一个服务器上创建了一个主备的测试实例，接下来就可以通过创建数据库、表、写入数据等进行测试，如果正常则可以直接在备上看到主所作的操作。

另外，在主从上分别执行 show processlist 就可以看到，主多一个线程处理 Binlog Dump；而在从上有两个线程，分别在等待主发送 event，另外一个线程则进行回放。

可以参考 [build-master-slave.sh](/reference/databases/mysql/build-master-slave.sh) 脚本在 /tmp 目录下创建主备复制；对于一主多备可以参考 [build-master-multislave.sh](/reference/databases/mysql/build-master-multislave.sh) 。

### 主主复制

同上，会在同一台机器的 /tmp 目录下部署两个主主实例，用于测试。

<!--
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-master
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-slave
-->

{% highlight text %}
----- 1.1 安装主主两个实例
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-master1 --user=mysql
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-master2 --user=mysql

----- 1.2 修改配置文件，其中log-bin必须设置，server-id需要不同
$ cat /tmp/mysql-master1/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-master1.sock
pid-file        = /tmp/mysql-master1.pid
datadir         = /tmp/mysql-master1
port            = 3307
server-id       = 1
relay_log_index = relay-bin.index
relay_log       = relay-bin
report_host     = 127.1
report_port     = 3307
auto-increment-increment = 2
auto-increment-offset    = 1
$ cat /tmp/mysql-master2/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-master2.sock
pid-file        = /tmp/mysql-master2.pid
datadir         = /tmp/mysql-master2
port            = 3308
server-id       = 2
relay_log_index = relay-bin.index
relay_log       = relay-bin
report_host     = 127.1
report_port     = 3308
auto-increment-increment = 2
auto-increment-offset    = 2

----- 2.1 分别启动主主服务器
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-master1/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-master1 > /dev/null 2>&1 &
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-master2/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-master2 > /dev/null 2>&1 &

----- 2.2.0 登陆
$ mysql -p -P3307 -uroot -S/tmp/mysql-master1.sock
$ mysql -p -P3308 -uroot -S/tmp/mysql-master2.sock
----- 2.2.1 修改密码
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new-password';
----- 2.2.2 然后用新密码登陆
$ mysql -p'new-password' -P3307 -uroot -S/tmp/mysql-master1.sock
$ mysql -p'new-password' -P3308 -uroot -S/tmp/mysql-master2.sock

----- 3.1 配置两个主服务器，需要新建一个用户
mysql> GRANT REPLICATION SLAVE ON *.* to 'mysync'@'localhost' IDENTIFIED BY 'kidding';
mysql> SHOW MASTER STATUS;            # 查看相应的File以及Position
----- 3.2 配置两个主服务器，然后启动
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_log_file='mysql-bin.000003',master_log_pos=496;
mysql> START SLAVE;
mysql> STOP SLAVE;                    # 也可以停止

----- 4. 关闭数据库
$ mysqladmin -uroot -S /tmp/mysql-master1.sock shutdown
$ mysqladmin -uroot -S /tmp/mysql-master2.sock shutdown
{% endhighlight %}

两台服务器都需要开启二进制日志和中继日志。

主备设置主键自动增长 auto-increment-increment 步长均为 2，而起始数值则分别为 1 和 2 ，这样两个服务的 auto_increment 值分别为奇数和偶数，从而避免数据同步时出现主键冲突。

可以参考 [build-master-master.sh](/reference/databases/mysql/build-master-master.sh) 脚本在 /tmp 目录下创建主主复制。



### 半同步机制

执行流程为。

1. 当 Master 上开启半同步复制的功能时，至少应该有一个 Slave 开启其功能；此时，一个线程在 Master 上提交事务将受到阻塞，直到得知一个已开启半同步复制功能的 Slave 已收到此事务的所有事件，或等待超时。

2. 当 Slave 主机连接到 Master 时，能够查看其是否处于半同步复制的机制。

3. 当一个事务的事件都已写入其 relay-log 中且已刷新到磁盘上，Slave 才会告知已收到。

4. 如果等待超时，也就是 Master 没被告知已收到，此时 Master 会自动转换为异步复制的机制。当至少一个半同步的 Slave 赶上了，Master 与其 Slave 自动转换为半同步复制的机制。

5.  半同步复制的功能要在 Master，Slave 都开启，半同步复制才会起作用；否则，只开启一边，它依然为异步复制。

如下是搭建的方法。

{% highlight text %}
----- 1. 安装半同步模块并启动，模块保存在lib/plugin目录下
mysql> INSTALL PLUGIN rpl_semi_sync_slave SONAME 'semisync_slave.so';
mysql> INSTALL PLUGIN rpl_semi_sync_master SONAME 'semisync_master.so';
----- 1.1 启动半同步模块，并设置超时时间2s
mysql> SET GLOBAL rpl_semi_sync_slave_enabled = ON;
mysql> SET GLOBAL rpl_semi_sync_master_enabled = ON;
mysql> SET GLOBAL rpl_semi_sync_master_timeout = 2000;
----- 1.2 也可以设置配置文件
[mysqld]
plugin_dir = /opt/mysql-5.7/lib/plugin
plugin_load = "rpl_semi_sync_master=semisync_master.so;rpl_semi_sync_slave=semisync_slave.so"
rpl_semi_sync_master_enabled = ON
rpl_semi_sync_slave_enabled = ON
rpl_semi_sync_master_timeout = 2000

----- 2 如果已经搭建主备复制，则从节点需要重新连接主服务器半同步才会生效
mysql> STOP SLAVE IO_THREAD;
mysql> START SLAVE IO_THREAD;

----- 3. 查看是否已经启动，需要大于等于1
mysql> SHOW GLOBAL STATUS LIKE 'rpl_semi_sync_master_clients';
{% endhighlight %}

现在半同步已正常工作，可以验证一下半同步超时，是否会自动降为异步工作；可在 Slave 上停掉半同步协议，然后在 Master 上创建数据库看一下能不能复制到 Slave 上。

{% highlight text %}
-----  slave ------------------------+----- master -------------------------------
### 关闭备库的IO线程
mysql> STOP SLAVE IO_THREAD;
Query OK, 0 rows affected (0.00 sec)
                                       ### 执行2s超时，主库仍然执行成功，备库失败
                                       mysql> CREATE DATABASE foobar;
                                       Query OK, 1 row affected (2.01 sec)
### 启动备库的IO线程，查看DB已经同步
mysql> START SLAVE IO_THREAD;
Query OK, 0 rows affected (0.00 sec)
mysql> SHOW DATABASES LIKE 'foobar' ;
+-------------------+
| Database (foobar) |
+-------------------+
| foobar            |
+-------------------+
1 row in set (0.00 sec)
                                       ### 可以执行成功
                                       mysql> CREATE DATABASE foobar1;
                                       Query OK, 1 row affected (0.01 sec)
{% endhighlight %}

创建第一个数据库花了 2.01 秒，如前设置的超时时间是 2 秒；而创建第二个数据库只花了 0.01 秒，由此得出结论是超时转换为异步传送。

可以参考 [build-semisync.sh](/reference/databases/mysql/build-semisync.sh) 脚本在 /tmp 目录下创建 semisync 的主主复制。


### GTID 复制

如下是搭建的方法，需要注意的是，执行 ```CHANGE MASTER TO``` 的命令与原生的稍微有些区别。

{% highlight text %}
----- 在配置文件中添加如下内容
[mysqld]
gtid-mode                 = ON
enforce-gtid-consistency  = ON

----- 设置主库信息
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_auto_position = 1;
{% endhighlight %}

可以参考 [build-gtid-replication-mm.sh](/reference/databases/mysql/build-gtid-replication-mm.sh) 脚本，会在 /tmp 目录下创建 gtid 的主主复制；或者主从复制 [build-gtid-replication-ms.sh](/reference/databases/mysql/build-gtid-replication-ms.sh)；或者一主多从复制 [build-gtid-replication-multislaves.sh](/reference/databases/mysql/build-gtid-replication-multislaves.sh)，需要注意的是，在备库链接到主库前，需要执行 ```RESET MASTER;``` 命令清空 GTID_EXECUTED，否则会导致 Errant-Transaction 。

<!--
需要注意开启 GTID 复制模式的约束：

不支持非事务类型的存储引擎复制，如MyISAM；
create table...select语句不支持，因为在基于row的复制中，该语句会备拆分为create和insert两件事务，并且会分配到相同的transaction id，会造成slave跳过相同transaction id而不同步的现象；
不支持临时表的使用；
无法使用sql_slave_skip_counter 跳过复制中出现的错误，可以在slave节点中指定变量gtid_executed来跳过相应的错误；
在mysql5.6.7以前的版本在运行mysql_upgrade的时候，如果开启gtid_mode=on的时候没有指定write_binlog=off会连接不上server。


1.如果master所有的binlog还在，安装slave后，直接change master 到master
原理是直接获取master所有的gtid并执行
优点是简单
缺点是如果binlog太多，数据完全同步需要的时间较长，并且需要master一开始就启用了GTID
总结：适用于master也是新建不久的情况

2.通过master或者其它slave的mysqldump备份搭建新的slave.
原理：备份时获取master的数据和这些数据对应的GTID，在Slave端跳过备份包含的GTID
优点是可以避免第一种方法中的不足
缺点操作相对复杂
总结：适用于拥有较大数据集的情况

3、percona xtrabackup
基于xtrabackup备份文件xtrabackup_binlog_info包含了GTID信息
做从库恢复后，需要手工设置：
set@@GLOBAL.GTID_PURGED='c8d960f1-83ca-11e5-a8eb-000c29ea831c:1-745497';
恢复后，执行change master to
缺点操作相对复杂
总结：适用于拥有较大数据集的情况
-->


### 状态监控

{% highlight text %}
mysql> SHOW SLAVE STATUS\G
*************************** 1. row ***************************
               Slave_IO_State: Waiting for master to send event
                  Master_Host: localhost           # 主服务器地址
                  Master_User: mysync              # 授权用户名
                  Master_Port: 3307                # 主服务器绑定的端口号
                Connect_Retry: 60
              Master_Log_File: mysql-bin.000003
          Read_Master_Log_Pos: 496                 # 主服务器二进制日志位置
               Relay_Log_File: mysql-slave-relay-bin.000001
                Relay_Log_Pos: 4
        Relay_Master_Log_File: mysql-bin.000003
             Slave_IO_Running: Yes                 # 重要
            Slave_SQL_Running: Yes                 # 重要
              Replicate_Do_DB:
          Replicate_Ignore_DB:
           Replicate_Do_Table:
       Replicate_Ignore_Table:
      Replicate_Wild_Do_Table:
  Replicate_Wild_Ignore_Table:
                   Last_Errno: 0
                   Last_Error:
                 Skip_Counter: 0
          Exec_Master_Log_Pos: 447
              Relay_Log_Space: 531
              Until_Condition: None
               Until_Log_File:
                Until_Log_Pos: 0
           Master_SSL_Allowed: No
           Master_SSL_CA_File:
           Master_SSL_CA_Path:
              Master_SSL_Cert:
            Master_SSL_Cipher:
               Master_SSL_Key:
        Seconds_Behind_Master: 0
Master_SSL_Verify_Server_Cert: No
                Last_IO_Errno: 0
                Last_IO_Error:
               Last_SQL_Errno: 0
               Last_SQL_Error:
  Replicate_Ignore_Server_Ids:
             Master_Server_Id: 1
                  Master_UUID: 10de195f-f051-11e6-8362-ac2b6e8b4228
             Master_Info_File: /tmp/mysql-slave/master.info
                    SQL_Delay: 0
          SQL_Remaining_Delay: NULL
      Slave_SQL_Running_State: Slave has read all relay log; waiting for more updates
           Master_Retry_Count: 86400
                  Master_Bind:
      Last_IO_Error_Timestamp:
     Last_SQL_Error_Timestamp:
               Master_SSL_Crl:
           Master_SSL_Crlpath:
           Retrieved_Gtid_Set:
            Executed_Gtid_Set:
                Auto_Position: 0
         Replicate_Rewrite_DB:
                 Channel_Name:
           Master_TLS_Version:
1 row in set (0.00 sec)

{% endhighlight %}




## 常用工具

一些主备复制中常用的工具，主要来自 percona-tools 。

### pt_heartbeat

该工具用来监控主备复制的延迟。

通常，是执行 SHOW SLAVE STATUS 查看 Slave_IO_Running (Yes)、Slave_SQL_Running (Yes)、Seconds_Behind_Master (0)；实际上这个是不可靠的，其原因如下：

1.  延迟时间的计算是服务器当前的时间戳与二进制日志中的事件时间戳相减得到的，也就是只有在执行事件时才能计算延迟；

2. 如果备库复制线程没有运行，或者由于某些错误导致备库无法计算延迟，就会报延迟 NULL。

3. 长事务执行更新达一个小时，最后提交，从而导致这条更新将比它实际发生时间要晚一个小时才记录到二进制日志中；而当备库执行这条语句时，会临时地报告备库延迟为一个小时，然后又很快变成 0 。

4. 库级联时，二级备库已经追赶上了它的备库，此时延迟显示为 0 ；但实际上与源主库是存在延迟的。

而该工具会在主机写入数据，在备机读取，然后计算两者的差值。可以通过如下方式执行：

{% highlight text %}
----- 主机执行如下命令
$ pt-heartbeat -D test -u root -P 3307 -h 127.1 --update --create-table --daemonize
参数：
          --database, -D  需要监控的数据库
              --user, -u  用户
          --password, -p  密码
              --port, -P  端口
              --host, -h  主机
            --socket, -S  主机
                --update  在指定的时间更新一个时间戳；
          --create-table  自动生成heartbeat表
             --daemonize  在后台运行
              --ask-pass  隐式输入MySQL密码
               --charset  字符集设置
              --interval  检查、更新的间隔时间，默认1s，最小单位0.01s
                 --table  指定心跳表名，默认heartbeat
      --master-server-id  指定主的server_id
--print-master-server-id  monitor和check 模式下，打印出主server_id
{% endhighlight %}
<!--
--check
检查从的延迟，检查一次就退出，除非指定了--recurse会递归的检查所有的从服务器。
--check-read-only
如果从服务器开启了只读模式，该工具会跳过任何插入。
--file 【--file=output.txt】
打印--monitor最新的记录到指定的文件，很好的防止满屏幕都是数据的烦恼。
--frames 【--frames=1m,2m,3m】
在--monitor里输出的[]里的记录段，默认是1m,5m,15m。可以指定1个，如：--frames=1s，多个用逗号隔开。可用单位有秒（s）、分钟（m）、小时（h）、天（d）。
--log
开启daemonized模式的所有日志将会被打印到制定的文件中。
--monitor
持续监控从的延迟情况。通过--interval指定的间隔时间，打印出从的延迟信息，通过--file则可以把这些信息打印到指定的文件。
--recurse
多级复制的检查深度。模式M-S-S...不是最后的一个从都需要开启log_slave_updates，这样才能检查到。
--recursion-method
指定复制检查的方式,默认为processlist,hosts。
--update
更新主上的心跳表。
--replace
使用--replace代替--update模式更新心跳表里的时间字段，这样的好处是不用管表里是否有行。
--stop
停止运行该工具（--daemonize），在/tmp/目录下创建一个“pt-heartbeat-sentinel” 文件。后面想重新开启则需要把该临时文件删除，才能开启（--daemonize）。
-->

如上，会在 test 数据库中创建一个 heartbeat 表，然后就可以监控复制在备库上的延迟了。

{% highlight text %}
----- 持续监控slave状态
$ pt-heartbeat -D test -u root -P 3308 -h 127.1 --master-server-id=1 --monitor

----- 只查看一次
$ pt-heartbeat -D test -u root -P 3308 -h 127.1 --master-server-id=1 --check
{% endhighlight %}

通过如下命令可以查看下二进制日志中到底记录了什么。

{% highlight text %}
$ mysqlbinlog --no-defaults /tmp/mysql-master/mysql-bin.000010
BEGIN
/*!*/;
# at 588960
#130822  6:44:01 server id 1  end_log_pos 589202        Query   thread_id=28    exec_time=0     error_code=0
SET TIMESTAMP=1377168241/*!*/;
UPDATE `test`.`heartbeat` SET ts='2013-08-22T06:44:01.005550', file='mysql-bin.000010', position='588555', relay_master_log_file=NULL, exec_master_log_pos=NULL WHERE server_id='1'
/*!*/;
# at 589202
#130822  6:44:01 server id 1  end_log_pos 589229        Xid = 6980
COMMIT/*!*/;
{% endhighlight %}

可以看到 heratbeat 表一直在更改 ts 和 position，而 ts 是我们检查复制延迟的关键。

如果要停止，可以直接执行如下的命令即可。

{% highlight text %}
$ pt-heartbeat --stop
{% endhighlight %}


### pt-slave-find

查看当前主从复制的拓扑结构。

{% highlight text %}
$ pt-slave-find -u root -P 3307 -h 127.1
{% endhighlight %}

### pt-slave-restart

当备服务器报错时，直接重启复制；特别注意，如果使用不当，可能会导致数据不一致，可以通过下面的工具进行校验。

{% highlight text %}
$ pt-slave-restart --socket=/tmp/mysql-slave.sock --ask-pass --error-numbers=1062
{% endhighlight %}

### pt-table-checksum

用来校验主备的数据是否一致，在主上执行校验查询对复制的一致性进行检查，对比主从的校验值，从而产生结果。

{% highlight text %}
# pt-table-checksum --nocheck-replication-filters --no-check-binlog-format \
  --user=root --port=3307 --host=127.1 --databases=test --tables=heartbeat \
  --replicate=test.checksums
{% endhighlight %}

上述的表，如果要检查多个，可以通过逗号分割。

如果 DIFFS 是 1 就说明主从的表数据不一致，如果指定 \-\-replicate=test.checksums 参数，则会将检查信息都写到了 checksums 表中。

#### 测试

可以通过如下方式新建一个表，然后进行测试。

{% highlight text %}
----- 在主库执行如下的SQL
mysql> CREATE TABLE test.foobar (id INT PRIMARY KEY, name VARCHAR(20));
Query OK, 0 rows affected (0.01 sec)
mysql> INSERT INTO test.foobar VALUES(1, "Andy"), (2, "Alan"), (3, "Bernard"), (4, "Christian");
Query OK, 4 rows affected (0.00 sec)
Records: 4  Duplicates: 0  Warnings: 0

----- 在备库执行如下的SQL
mysql> INSERT INTO test.foobar VALUES(5, "Hobart"), (6, "Raymond");
Query OK, 2 rows affected (0.00 sec)
Records: 2  Duplicates: 0  Warnings: 0
mysql> DELETE FROM test.foobar WHERE id = 3;
Query OK, 1 row affected (0.00 sec)
{% endhighlight %}

然后执行如下命令校验该表，然后可以在备库的 test.checksums 查看。

{% highlight text %}
# pt-table-checksum --nocheck-replication-filters --no-check-binlog-format \
  --user=root --port=3307 --host=127.1 --databases=test --tables=foobar \
  --replicate=test.checksums
{% endhighlight %}


### pt-table-sync

用于主备表的数据同步，可以单向、双向同步表的数据，但是不同步表结构、索引或者其它对象，所以需要保证修复数据前保证表存在。

{% highlight text %}
# pt-table-sync --replicate=test.checksums h=127.1,u=root,P=3307 h=127.1,u=root,P=3308 --print
参数：
   --replicate  过pt-table-checksum得到校验表
   --databases  执行同步的数据库，多个用逗号隔开
      --tables  执行同步的表，多个用逗号隔开
       --print  打印，但不执行命令
     --execute  执行命令
{% endhighlight %}

此时，直接在备库执行如上打印的 SQL 即可，也可以通过 \-\-execute 执行命令。


### 其它

常见报错。

#### Cannot connect to P=xxx,h=,u=xxx

也就是无法连接备库，详细的报错日志如下。

{% highlight text %}
Cannot connect to P=xxx,h=,u=xxx
Diffs cannot be detected because no slaves were found.  Please read the --recursion-method
documentation for information.
{% endhighlight %}

默认是通过 show processlist 或者 show slave hosts 找到 host 的值。不过对于后者，需要在配置文件里面已经配置自己的地址和端口。

{% highlight text %}
report_host = 192.168.0.20
report_port = 3306
{% endhighlight %}

<!--
METHOD       USES
===========  =============================================
processlist  SHOW PROCESSLIST
hosts        SHOW SLAVE HOSTS
cluster      SHOW STATUS LIKE 'wsrep\_incoming\_addresses'
dsn=DSN      DSNs from a table
none         Do not find slaves
-->





## 参考

MySQL 中与复制相关的内容，可以参考官方文档 [MySQL Reference Manual - Replication](http://dev.mysql.com/doc/refman/en/replication.html)。

一个基于 PyMySQL，纯 Python 编写的 MySQL 复制协议的实现 [python-mysql-replication](https://github.com/noplay/python-mysql-replication) 。

<!--
mysql 复制相关源代码代码分布
http://www.onlamp.com/2006/04/20/advanced-mysql-replication.html

http://siddontang.com/2015/02/02/mysql-replication-protocol/
-->

{% highlight text %}
{% endhighlight %}
