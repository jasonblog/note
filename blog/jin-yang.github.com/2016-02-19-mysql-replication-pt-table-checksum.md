---
title: MySQL 主备数据校验
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,主备复制,一致性校验
description: 由于各种原因，MySQL 主从架构可能会出现数据不一致的情况出现，为此需要对主备复制的数据进行校验。在此，简单介绍 Percona-Toolkits 提供的数据校验方式。
---

由于各种原因，MySQL 主从架构可能会出现数据不一致的情况出现，为此需要对主备复制的数据进行校验。

在此，简单介绍 Percona-Toolkits 提供的数据校验方式。

<!-- more -->

## 简介

由于各种原因，MySQL 主从架构可能会出现数据不一致的情况出现，大致归结为如下几类：

* 主备方式，且在备库写数据；
* 执行了 non-deterministic query，如用户自定义函数、rand() 等；
* 回滚掺杂事务表和非事务表的事务；
* binlog 或者 relaylog 数据损坏。

在主库执行基于 statement 的 SQL 语句生成主库数据块的 checksum，把相同的 SQL 语句传递到从库，并在从库上计算相同数据块的 checksum，最后，比较主从库上相同数据块的 checksum 值，由此判断主从数据是否一致。

这种校验是分表进行的，在每个表内部又是分块进行的，而且提供了非常多的限流选项，因此对线上服务的冲击较小。

### 环境搭建

在 CentOS 中可以通过如下方式安装依赖 RPM 包。

{% highlight text %}
# rpm -ivh perl-DBD-MySQL perl-TermReadKey
# rpm -ivh percona-toolkit-x.x.x.el7.x86_64.rpm
{% endhighlight %}

三个包分别为 MySQL 驱动、交互密码输入、主备数据校验工具。如果是双主配置，只需要在一个节点部署即可。如果要测试，可以通过 ```PTDEBUG=1 pt-table-checksum > /tmp/checksum.log 2>&1``` 测试。

可以通过 ```pt-table-checksum --help``` 查看帮助信息，或者 ```perldoc /usr/local/bin/pt-table-checksum``` 查看详细帮助。

### 新建校验结果保存表

分别在主备新建数据校验表。

{% highlight sql %}
CREATE TABLE `zabbix`.`checksums` (
  `db` char(64) NOT NULL,
  `tbl` char(64) NOT NULL,
  `chunk` int(11) NOT NULL,
  `chunk_time` float DEFAULT NULL,
  `chunk_index` varchar(200) DEFAULT NULL,
  `lower_boundary` text,
  `upper_boundary` text,
  `this_crc` char(40) NOT NULL,
  `this_cnt` int(11) NOT NULL,
  `master_crc` char(40) DEFAULT NULL,
  `master_cnt` int(11) DEFAULT NULL,
  `ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`db`,`tbl`,`chunk`),
  KEY `ts_db_tbl` (`ts`,`db`,`tbl`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
{% endhighlight %}

### 主机用户创建

首先需要创建如下用户。

#### 脚本部署主机

{% highlight sql %}
CREATE USER 'checker' IDENTIFIED BY 'password';
GRANT SUPER, PROCESS, SHOW DATABASES ON *.* TO 'checker'@'localhost';
GRANT DELETE, INSERT, UPDATE ON zabbix.checksums TO 'checker'@'localhost';
{% endhighlight %}

权限配置解析：

* SUPER: SET @@binlog_format:="STATEMENT" 设置会话级的 binlog 格式；
* PROCESS: SHOW GRANTS FOR 查看当前用户的权限信息；
* SHOW DATABASES: 查看是否存在数据库；
* DELETE,INSERT,UPDATE: 用于清理、REPLACE INTO、UPDATE checksums 的历史数据；

#### 另外一台主机

{% highlight sql %}
CREATE USER 'checker' IDENTIFIED BY 'password';
GRANT PROCESS, SUPER, REPLICATION CLIENT ON *.* TO 'checker'@'IP-MASTER';
GRANT SELECT ON zabbix.* TO 'checker'@'IP-MASTER';
{% endhighlight %}

权限配置解析：

* SUPER, REPLICATION CLIENT: SHOW SLAVE STATUS 查看是否已经同步完成；
* SELECT: EXPLAIN 评估备库数据量；


### 数据校验命令

{% highlight text %}
$ pt-table-checksum --nocheck-replication-filters --check-slave-tables --chunk-size-limit=1.5 \
  --check-interval=3 --max-lag=1s --nocheck-binlog-format \
  --empty-replicate-table --progress=time,30 \
  --nocreate-replicate-table --replicate=zabbix.checksums \
  --replicate-check --check-plan --replicate-database=zabbix \
  --float-precision=2 --chunk-time=0.5 \
  --user=checker --password=password --port=3306 --host=localhost \
  --databases=somedb --tables=table1,table2,table3
{% endhighlight %}

### 数据不一致检查

可以直接通过如下命令查看数据不一致的表，需要在备库执行。

{% highlight sql %}
SELECT db, tbl, SUM(this_cnt) AS total_rows, COUNT(*) AS chunks
  FROM checksums
  WHERE (
    master_cnt <> this_cnt
    OR master_crc <> this_crc
    OR ISNULL(master_crc) <> ISNULL(this_crc))
  GROUP BY db, tbl;
{% endhighlight %}


## 执行流程

该工具每次只检测一个表，对于表中的数据会分割成 chunk，一般是 1000 条左右数据(默认会动态调整)；这个系统的调用流程可以阐述如下：

#### 设置超时时间

为了减小对线上事务的影响，在主库建立链接后，会先设置会话变量 ```innodb_lock_wait_timeout=1``` 和 ```wait_timeout=10000```，也可以通过 ```--set-vars wait_timeout=500``` 参数覆盖默认值。

#### 检查 sql_mode

通过 ```SELECT @@SQL_MODE``` 命令，确认该变量中不包含 ```ONLY_FULL_GROUP_BY```，因为对于一些正常的函数，该模式仍可能会出错，因此需要通过 ```SET SQL_MODE``` 命令重新设置。

#### 设置 binlog_mode

查看当前 ```SELECT @@binlog_mode``` 模式，如果不是 STATEMENT，默认会报错直接退出；当然，可以通过 ```--[no]check-binlog-format``` 参数不检查该参数。

然后，仍会设置会话变量 ```/*!50108 SET @@binlog_format := 'STATEMENT'*/``` 。

#### 设置隔离级别

设置会话的隔离级别为 ```SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ```，主要因为在将 binog_format 设置为 STATEMENT 后，且隔离级别为 RR，那么会报如下的错误 ```Transaction level 'READ-COMMITTED' in InnoDB is not safe for binlog mode 'STATEMENT'```，当然，这正是我们需要的，所以直接忽略该报错信息。

#### 查找备库

通过 ```get_slaves()``` 函数查找备库，当然可以在启动时设置不同的备库查找方式，详见 ```find_slave_hosts()``` 函数。

#### 拼接 SQL

接下来就是拼接主库执行的 SQL 语句，主要的格式如下：

{% highlight sql %}
REPLACE INTO checksum_table(db, tbl, chunk, ...)
  SELECT ..., COUNT(*) AS cnt, COALESCE(LOWER(CONV(BIT_XOR(
    CAST(CRC32(CONCAT_WS('#',...)) AS UNSIGNED)), 10, 16)), 0) AS crc
FROM tables FORCE INDEX(`PRIMARY`)
WHERE ((col >= ?)) AND ((col <= ?));
{% endhighlight %}

简单介绍下拼接上述 SQL 时的注意内容：

* 拼接各个列时，会根据不同的数据类型进行处理，例如以 ```TIMESTAMP``` 类型使用 ```UNIX_TIMESTAMP()``` 函数；可能为 ```NULL``` 时，使用 ```CONCAT(ISNULL())``` 等等；
* 每次执行查询前会通过 ```EXPLAIN``` 生成执行计划，主要关注评估数据量、索引使用情况，执行时会强制使用索引；
* 通过 ```WHERE``` 进行分区，也就是 chunk 块，默认会使执行结果尽量接近 ```--chunk-time``` 参数指定的时间，所以每次 chunk 的大小会动态调整。

#### 等待备库执行完成

在执行完上述的 SQL 后，需要等待备库执行完成上述的 SQL 语句，也即是计算备库的 CRC 值。

#### 更新主库校验值

校验完成后在主库更新校验值，SQL 如下，该语句会将主库校验值同步到备库：

{% highlight sql %}
UPDATE checksum_table SET chunk_time = ?, master_crc = ?, master_cnt = ?
  WHERE db = ? AND tbl = ? AND chunk = ?;
{% endhighlight %}

#### 等待同步完成

接下来就是等待备库执行完成上述的 SQL 语句，也即将主库的校验值同步到备库。在按照 chunk 执行完成之后，接下来只需要去备库的 checksums 表中找 ```master_cnt <> this_cnt or OR master_crc <> this_crc``` 的记录即可。

### 校验函数

简单介绍下上述校验命令中的函数。

#### COALESCE()

类似于 Oracle 中的 NVL() 函数，语义略有区别，会返回参数中第一个非 NULL 表达式的值，从左到右：

{% highlight text %}
----- 如下函数返回 1
mysql> SELECT COALESCE(NULL, NULL, 1);
{% endhighlight %}

通常使用场景为，如果某个字段默认是 NULL，如果不想返回 NULL，而是 0 或其它值，可以使用该函数：

{% highlight text %}
mysql> SELECT COALESCE(field_name, 0) AS value FROM table;
{% endhighlight %}

#### BIT_XOR()

用于 Binary String 的检测，会字段的值逐位进行检测，如果 bit 相同则返回 0，否则返回 1；比较时会按照行逐行进行检测，示例如下：

{% highlight text %}
mysql> CREATE TABLE foobar (id INT);
mysql> INSERT INTO foobar VALUES(100), (100), (120);
mysql> SELECT BIT_XOR(id) FROM foobar;
{% endhighlight %}

首先是第一行与第二行比较，结果是 0；然后结果 0 与第三行比较，结果是 120。

#### CAST()/CONVERT()

CAST() 和 CONVERT() 用来进行类型转换，只是语法不同，可用类型包括了 BINARY、CHAR(N)、DATE、TIME、DATETIME、DECIMAL、SIGNED、UNSIGNED 等：

{% highlight text %}
CAST(value AS type);
CONVERT(value, type);
{% endhighlight %}

#### CONV()

语法为 ```CONV(N,from_base,to_base)```，该函数用于数字的基数转换，最小基数值是2，最大值为36，如果任一参数为NULL，则该函数返回NULL。

{% highlight text %}
----- 将数字5从基数16转为基数2
SELECT CONV(5,16,2);
{% endhighlight %}


## 配置选项

接下来，看下执行命令时的常用参数。

### 安全选项

* \-\-[no]check-replication-filters (可选)

是否检查复制过滤规则，默认会检查，如果检查到有设置过滤规则会直接退出，主要防治检查的表在主库存在，而备库不存在导致 SQL 异常，主备复制出错；

* \-\-[no]check-slave-tables (设置)

默认检查，查看是否所有从库都有被检查的表和列，防止由于改程序导致主备复制异常；每次同时会通过 EXPLAIN 检查备库的执行计划，评估行数；

* \-\-chunk-size-limit=2.0 (设置，尽量确保有主键或者唯一索引)

每次执行校验 SQL 前，会通过 EXPLAIN 检查本次执行SQL的数据量，对于没有唯一索引的表，通过 EXPLAIN 查看的行数差异会比较大，该参数就用于设置最大可以超过 chunk-size 几倍；如果超过 chunk-size*chunk-size-limit，那么就不会对本次的 chunk 做校验。

如果是 1.0，就以 chunk-size 为准；是 0.0 则表示不对 chunk 大小做校验。

### 限速选项

* \-\-check-interval=1 (可选)

需要睡眠多久，再检查一次主从延迟是否超过 max-lag，默认是 1 秒；

* \-\-max-load=Threads_running=25 (建议设置)

设置最大负载，当超过这个值时就等待，防止由于工具导致过载，默认是 ```Threads_running=25``` 等价于 ```Threads_running:25```，可以逗号分隔设置多个值；如果不指定值则会在开始检查变量对应的值，并将上限设置为 120%；

每次执行完一个 chunk 之后，会通过 ```SHOW GLOBAL STATUS``` 查看当前服务器的状态，如果超过了上述值，就会等待；如果校验 SQL 导致锁等待，则会导致队列增加，进而 Threads_running 增加，通过该参数可以减小系统负载。

* \-\-check-slave-lag="h=slave1" (可选)

是否只检查这个从库的复制延迟，如果有些场景通过 pt-slave-delay 特意设置了延迟复制，那么可以通过该参数指定需要关注哪些复制延迟。

* \-\-max-lag=1s (设置)

检查备库的最大延迟，也就是 ```Seconds_Behind_Master``` 参数，当超过这个就等待，而且如果备库复制线程异常，同样会等待；默认会检查所有库，也可以通过 check-slave-lag 指定需要检查哪些库。

### 过滤选项

可以指定只检查哪些数据库、表、列、存储引擎等，也可以使用 Perl 正则表达式；除此之外，也可以指定忽略哪些。

* \-\-ignore-databases
* \-\-ignore-databases-regex
* \-\-ignore-tables
* \-\-ignore-tables-regex
* \-\-ignore-columns
* \-\-ignore-engines
* \-\-databases
* \-\-databases-regex
* \-\-tables
* \-\-tables-regex
* \-\-columns
* \-\-engines

### 主库登陆选项

* \-\-host
* \-\-user
* \-\-password
* \-\-port
* \-\-ask-pass
* \-\-socket

### 其它参数：

* \-\-[no]empty-replicate-table

默认yes，会在每次执行校验前清理要校验表对应的记录，注意该操作不会TRUNCATE TABLE，如果需要，则要手动执行；

* \-\-progress=time,30

输出当前的进展，有三种输出方式，包括了两个参数值，默认是 time,30，还可以设置为 percentage,time,iterations；

* \-\-explain

打印要执行的命令，但是不执行 (dry run)；

* \-\-[no]create-replicate-table

检查 replicate 参数指定的表，如果不存在是否创建；

* \-\-replicate=percona.checksums

将校验结果写入到那个表中，默认是 percona.checksums，可以通过该参数指定；

* \-\-binary-index (默认不开启测试)

默认通过 replicate 指定的表的 lower_boundary、upper_boundary 字段格式为 text，有些情况下可能会存在不兼容，通过该参数将定义上述字段为 blob 格式；

* \-\-[no]replicate-check

默认 yes，也就是每次校验完一个表之后，会等待各个备库同步完成，从而进行数据校验，如果关闭则需要通过 replicate-check-only 参数再检查一遍；

* \-\-replicate-check-only

不执行数据校验的 SQL，只查询保存的结果；

* \-\-run-time=NUM

要执行多长时间，每个循环会检测是否达到该参数设置值，可以指定单位，默认是秒，可以指定参数 s=seconds,m=minutes,h=hours,d=days；如果使用 ```--resume``` 那么之前的时间也会计算在内；

* \-\-recurse=NUM

在查找复制库时指定递归层次，可以查看 ```recurse_to_slaves()``` 函数；

* \-\-recursion-method=processlist

递归查找时的方法，包括了 processlist(```SHOW PROCESSLIST```)、hosts(```SHOW SLAVE HOSTS```)、none，详细查看 ```recurse_to_slaves()``` 函数；默认在启动时会检查所有的级联备库，可以通过该参数确认递归方法，通过 recurse 设置检查级联数；

* \-\-[no]check-binlog-format

检查各个节点的格式是否相同，一般需要保证 binlog 格式为 STATEMENT 。

* \-\-[no]check-plan

每次在执行正式查询前，默认会通过 EXPLAIN 查看执行计划，然后判断执行计划是否有问题，有可能会跳过本 chunk；

* \-\-replicate-check-retries

当出现校验异常时，会尝试多少次；

* \-\-replicate-database

默认会在执行过程中通过 USE 切换数据库，如果设置了 ```binlog_ignore_db```、```replicate_ignore_db``` 类型的参数，那么可能会导致错误，通过该参数可以防止切换数据库；

* \-\-chunk-index

优先选择使用的索引，如果索引不存在则自动选择合适的索引，然后在执行 SQL 时会通过 ```FORCE INDEX``` 指定索引，当然如果指定错的话可能会有问题，建议只针对单表设置；

* \-\-chunk-index-columns

只使用索引最左匹配的几个列，通常可以在 MySQL 优化器有问题时进行设置；

* \-\-chunk-size

每次校验查询时的查询数据行数，可以使用 k,M,G 后缀，不建议直接使用，最好设置 chunk-time；默认会在每次查询时动态调整尽量接近 chunk-time 值，不指定时采用默认值作为起始值，后续的查询则动态调整，如果指定则严格按照该参数进行查询；

* \-\-chunk-time

如上，每次查询会动态调整 checksum 查询的记录数，使执行时间尽量接近该值，单位是秒；

* \-\-config

指定配置文件，如果指定则需要设置为命令行的第一项；

* \-\-defaults-file

指定 MySQL 配置文件，需要使用绝对路径；

* \-\-float-precision

用于浮点数四舍五入，会通过 ROUND() 函数处理，防止在不同平台上由于精度不同导致校验失败，例如为2时对于 1.008 和 1.009 会四舍五入为 1.01；

* \-\-function

指定数据校验的函数，如果采用 CRC32 可能会有 hash 冲突，而 SHA1+MD5 对 CPU 消耗比较大，也可以使用 FNV1A_64()、MURMUR_HASH() 自定义函数进行处理；

* \-\-pid

指定 PID 文件，一般只允许一个实例部署；

* \-\-plugin

指定 Perl 插件；

* \-\-quiet

使用一次时只打印 Errors、Warnings、数据不一致；使用两次则只输出错误信息；

* \-\-resume

继续上次中断；

* \-\-retries

发生非致命问题时(锁等待超时、查询被 kill 等)，每个 chunk 的重试执行次数；

* \-\-separator

执行 CONCAT_WS() 函数时指定的分隔符，默认是 #；

* \-\-set-vars

设置会话变量，默认会设置 ```wait_timeout=10000```、```innodb_lock_wait_timeout=1``` 两个参数；

* \-\-version

查看当前版本；

* \-\-[no]version-check

自动检查版本，用于更新，除了检查 Percona-toolkit 版本之外，还会检查 Perl 以及 MySQL 的版本，并打印已知问题；

* \-\-chunk-size=1000

每次 checksum 校验时查询的记录数，如果指定了该参数，则将 chunk-time 设置为 0；

* \-\-chunk-time=0.5

每次动态调整 checksum 记录数，使执行时间尽量接近该值，单位是秒；

* \-\-slave-skip-tolerance

如果设置主库一个 chunk 校验完，而备库可能超过限制大小，那么就会跳过备库的查询；

* \-\-trim

主要是针对 VARCHAR 类型的数据列，主要用于 4.1 和 >=5.0 的数据对比，5.0 之后会保存 VARCHAR 类型开始与结尾的空格，而 4.1 则会自动去除；

* \-\-where

用于过滤部分数据，与 mysqldump 的 -w 选项类似；


## FAQs

##### 1. binlog在row模式下是否支持？
工具在执行时会将会话变量binlog格式设置为statement；如果级联开启了--log-slave-updates选项，而binlog_format=ROW，那么此时仍有可能在会导致下一层的级联校验失败。

##### 2. 如何做到流量控制？
提供了多个参数进行流量控制，每次执行完一个chunk之后，会检查参数是否满足限流要求，如果超过限制则睡眠等待，直到满足要求。详见参数配置中的 "限速选项" 。


<!--
### 校验数据表确认

导出当前表结构，过滤表数量：

```
mysqldump --no-data -S /var/lib/mysql/mysql.sock -u omadmin -p'Opsmonitordb@2015' zabbix > opsmonitor.sql
cat opsmonitor.sql | grep "CREATE TABLE" | wc -l
cat opsmonitor.sql | grep "PRIMARY KEY" | wc -l
```

总计114个表，109有Primary Key，监控表包含了除dbversion、history、history_str、history_uint、tmp_cpustat、history_text、history_log外所有表，包括了


pt-table-checksum --nocheck-binlog-format \
  --user=root --password=new-password --port=3307 --host=127.1 --databases=sakila --tables=payment \
  --replicate=sakila.checksums


get_replication_filters() 如果设置check-replication-filters参数，则检查是否设置了过滤选项

check_slave_tables() 默认check-slave-tables


|-make_chunk_checksum() 生成需要进行校验的列，只有在没有索引或者chunk过大时才会报错
| |-get_crc_args() 获取CRC参数，测试各种HASH函数测试是否可用
| |-make_row_checksum()
|-OobNibbleIterator() 新建一个循环
  |-

can_nibble()
 |-get_row_estimate() 评估当前数据量，主要通过EXPLAIN测试
 |-_find_best_index() 查找最优的索引

get_connected_slaves()
wait_for_master()
start_slave()
catchup_to_master()
has_slave_updates()

get_replication_filters()
make_chunk_checksum()
make_row_checksum()
get_checksum_columns()
find_replication_differences()

https://github.com/KredytyChwilowki/MySQLReplicaIntegrityCheck

log-slave-updates

http://www.jet-almost-lover.cn/Article/Detail/45703
http://blog.csdn.net/melody_mr/article/details/45224249
http://www.cnblogs.com/zhoujinyi/archive/2013/05/09/3067045.html
http://keithlan.github.io/2016/05/25/pt_table_checksum/
http://www.cnblogs.com/xuanzhi201111/p/4180638.html
-->

## 参考

[DSN (DATA SOURCE NAME) SPECIFICATIONS](https://www.percona.com/doc/percona-toolkit/2.1/dsn_data_source_name_specifications.html)

{% highlight text %}
{% endhighlight %}
