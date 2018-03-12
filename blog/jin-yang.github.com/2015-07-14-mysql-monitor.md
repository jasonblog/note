---
title: MySQL 监控指标
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,monitor,监控
description: 为了排查问题，对数据库的监控是必不可少的，在此介绍下 MySQL 中的常用监控指标。
---

为了排查问题，对数据库的监控是必不可少的，在此介绍下 MySQL 中的常用监控指标。

<!-- more -->

![Monitor Logo]({{ site.url }}/images/databases/mysql/monitor-logo.png "Monitor Logo"){: .pull-center }

## 简介

MySQL 有多个分支版本，常见的有 MySQL、Percona、MariaDB，各个版本所对应的监控项也会有些区别，在此仅介绍一些通用的监控项。

通常，监控项的源码是在 mysql/mysqld.cc 文件中定义，其内容如下所示。

{% highlight c %}
SHOW_VAR status_vars[]= {
    {"Aborted_clients",          (char*) &aborted_threads,        SHOW_LONG},
    {"Aborted_connects",         (char*) &aborted_connects,       SHOW_LONG},
    {"Acl",                      (char*) acl_statistics,          SHOW_ARRAY},
    // ... ...
};
{% endhighlight %}


## 监控

对于数据库，通常可以主动监控以下四个与性能及资源利用率相关的指标：

* 查询吞吐量
* 查询执行性能
* 连接情况
* 缓冲池使用情况

### 吞吐量

在 MySQL 中有各种针对不同命令的统计，其监控项指标以 Com_XXX 方式命名，其中比较常用的统计项包括了 TPS/QPS。

而 MySQL 与 QPS 相关的包括了三个监控项，分别为 Queries、Questions、Com_select，一般会采用 Com_select 作为采集项；对于 TPS，一般认为是 Com_insert + Com_update + Com_delete 三个统计项的和。


#### Queries 和 Questioins 区别

如下是 [Server Status Variables](http://dev.mysql.com/doc/refman/en/server-status-variables.html) 中对这两个值的介绍。

{% highlight text %}
Queries : The number of statements executed by the server. This variable
includes statements executed within stored programs, unlike the Questions
variable. It does not count COM_PING or COM_STATISTICS commands.

Questions : The number of statements executed by the server. This includes
only statements sent to the server by clients and not statements executed
within stored programs, unlike the Queries variable. This variable does
not count COM_PING, COM_STATISTICS, COM_STMT_PREPARE, COM_STMT_CLOSE, or
COM_STMT_RESET commands.
{% endhighlight %}

也就是说，如果不使用 prepared statements ，那么两者的区别是 Questions 会将存储过程作为一个语句；而 Queries 会统计存储过程中的各个执行的语句。


{% highlight text %}
mysql> SHOW STATUS LIKE "questions";        ← 查看本连接执行的查询SQL数
+---------------+-------+
| Variable_name | Value |
+---------------+-------+
| Questions     | 2     |
+---------------+-------+
1 row in set (0.00 sec)

mysql> SHOW STATUS LIKE "queries";
+---------------+----------+
| Variable_name | Value    |
+---------------+----------+
| Queries       | 21241470 |
+---------------+----------+
1 row in set (0.00 sec)

{% endhighlight %}

上述的 questions 是会话级别的，当然可以通过 global 查看全局的变量。

另外，MySQL 会在执行 SQL 之前开始增加上述的统计计数；而当前正在执行的 SQL 数量则可以通过 threads_running 查看。

#### 总结

{% highlight text %}
Questions        已执行的由客户端发出的语句
Com_select       执行SELECT语句的数量，通常也就是QPS
Writes           Com_insert+Com_update+Com_delete，也就是TPS
{% endhighlight %}



### 执行性能

目前，有几种方式可以用来查看 MySQL 的执行性能问题，可以参考如下。

#### performance_schema

在 events_statements_summary_by_digest 表中保存了许多关键指标，均以微秒为单位，该表会忽略数值、规范化空格与大小写。

如果要以微秒为单位查看各个 database 的平均运行时间，或者出现的错误语句总数，可以通过如下方式查看：

{% highlight text %}
mysql> SELECT schema_name, SUM(count_star) count, \
       ROUND((SUM(sum_timer_wait)/SUM(count_star))/1000000) AS avg_microsec \
       FROM performance_schema.events_statements_summary_by_digest
       WHERE schema_name IS NOT NULL GROUP BY schema_name;

mysql> SELECT schema_name, SUM(sum_errors) err_count \
       FROM performance_schema.events_statements_summary_by_digest \
       WHERE schema_name IS NOT NULL GROUP BY schema_name;
{% endhighlight %}

#### sys

sys 存储引擎默认在 5.7.7 中添加，对于 5.6 可以手动安装，详细可以参考 [github - sys schema](https://github.com/mysql/mysql-sys) 。

#### 慢查询

MySQL 提供了一个 Slow_queries 计数器，当查询的执行时间超过 long_query_time 参数指定的值之后，该计数器就会增加，默认设置为 10 秒。

{% highlight text %}
mysql> SHOW VARIABLES LIKE 'long_query_time';
+-----------------+-----------+
| Variable_name   | Value     |
+-----------------+-----------+
| long_query_time | 10.000000 |
+-----------------+-----------+
1 row in set (0.00 sec)

mysql> SET GLOBAL long_query_time = 5;
Query OK, 0 rows affected (0.00 sec)
{% endhighlight %}

需要注意的是，需要关闭会话然后重新连接之后，才能使该参数生效。

#### 总结

{% highlight text %}
Slow_queries         慢查询的数量
{% endhighlight %}

其它的一些比较适合发现性能问题后用于排查。


### 连接情况

监控客户端连接情况相当重要，因为一旦可用连接耗尽，新的客户端连接就会遭到拒绝，MySQL 默认的连接数限制为 151，可通过下面的方法在配置文件中进行设置。

{% highlight text %}
[mysqld]
max_connections = 200
{% endhighlight %}

通过如下方法查询和临时设置。

{% highlight text %}
mysql> SHOW VARIABLES LIKE 'max_connections';
+-----------------+-------+
| Variable_name   | Value |
+-----------------+-------+
| max_connections | 151   |
+-----------------+-------+
1 row in set (0.00 sec)

mysql> SET GLOBAL max_connections = 200;
Query OK, 0 rows affected (0.00 sec)
{% endhighlight %}

通常 Linux 可以处理 500~1000 个连接，如果 RAM 资源足够，可处理 1W+ 个连接，而 Windows 由于采用 Posix 兼容层，能处理的连接数一般不超过 2048 个。


#### 监控连接使用率

如果采用每个连接一个线程的方式，可以通过 Threads_connected 查看，监控该指标与先前设置的连接限制，可以确保服务器拥有足够的容量处理新的连接。

另外，通过 Threads_running 指标，可以查看正在处理请求的线程，可以用来判断那些连接被占用但是却没有处理任何请求。

如果达到 max_connections 就会拒绝新的连接请求，Connection_errors_max_connections 指标就会开始增加，同时，追踪所有失败连接尝试的 Aborted_connects 指标也会开始增加。

另外，通过 Connection_errors_internal 这个指标，可以用来监控来自服务器本身导致的错误，例如内存不足。

#### 总结

{% highlight text %}
Threads_connected                   已经建立的连接
Threads_running                     正在运行的连接
Connection_errors_internal          由于服务器内部本身导致的错误
Aborted_connects                    尝试与服务器建立连接但是失败的次数
Connection_errors_max_connections   由于到达最大连接数导致的错误
{% endhighlight %}


### 缓冲池使用情况

InnoDB 使用一片内存区域作为缓冲区，用来缓存数据表与索引数据，缓冲区太小可能会导致数据库性能下滑，磁盘 I/O 攀升。

默认值一般是 128MiB，建议将其设置为物理内存的 80%；不过需要注意的是，InnoDB 可能会使用超过缓冲池 10%，如果耗尽内存，则会使用分页，从而使数据库性能受损。

{% highlight text %}
SHOW GLOBAL VARIABLES LIKE "innodb_buffer_pool_chunk_size";
SHOW GLOBAL VARIABLES LIKE "innodb_buffer_pool_instances";
{% endhighlight %}

如果 innodb_buffer_pool_chunk_size 查询没有返回结果，则表示在你使用的 MySQL 版本中此参数无法更改，其值为 128 MiB，实际参数为 innodb_buffer_pool_size 。

在服务器启动时，你可以这样设置缓冲池的大小以及实例的数量：

{% highlight text %}
$ mysqld --innodb_buffer_pool_size=8G --innodb_buffer_pool_instances=16
{% endhighlight %}

#### 监控指标

Innodb_buffer_pool_read_requests 记录了读取请求的数量，而 Innodb_buffer_pool_reads 记录了缓冲池无法满足，因而只能从磁盘读取的请求数量，也就是说，如果 Innodb_buffer_pool_reads 的值开始增加，意味着数据库性能大有问题。

缓存的使用率和命中率可以通过如下方法计算：

{% highlight text %}
(Innodb_buffer_pool_pages_total - Innodb_buffer_pool_pages_free) /
    Innodb_buffer_pool_pages_total * 100%

(Innodb_buffer_pool_read_requests - Innodb_buffer_pool_reads) /
    Innodb_buffer_pool_read_requests * 100%
{% endhighlight %}

如果数据库从磁盘进行大量读取，而缓冲池还有许多闲置空间，这可能是因为缓存最近才清理过，还处于预热阶段。

#### 总结

{% highlight text %}
Innodb_buffer_pool_pages_total          BP中总页面数
Buffer pool utilization                 BP中页面的使用率
Innodb_buffer_pool_read_requests        BP的读请求
Innodb_buffer_pool_reads                需要读取磁盘的请求数
{% endhighlight %}



## 响应时间

"响应时间" (Response Time, RT) 在数据库应用中，尤其是 OLTP 的场景，非常重要，但官方版本中一直没有加上这个统计功能。开始使用的是 tcpdump+mk-query-digest，再后来 tcprstat，很快 Percona 提供了响应时间统计插件。

对于 MariaDB 也存在类似的方式，可以通过如下的方式安装、测试。

{% highlight text %}
----- 1. 查看是否存在插件
$ ls /opt/mariadb/lib/plugin | grep response
query_response_time.so

----- 2. 安装插件
mysql> INSTALL PLUGIN query_response_time_audit SONAME 'query_response_time.so';
Query OK, 0 rows affected (0.00 sec)
mysql> INSTALL PLUGIN query_response_time SONAME 'query_response_time.so';
Query OK, 0 rows affected (0.00 sec)

----- 3. 打开统计功能
mysql> SET GLOBAL query_response_time_stats = 'ON';
Query OK, 0 rows affected (0.00 sec)
mysql> SET GLOBAL query_response_time_flush = 'ON';
Query OK, 0 rows affected (0.00 sec)

----- 4. 查看统计值
mysql> SHOW QUERY_RESPONSE_TIME;
mysql> SELECT * FROM information_schema.query_response_time;
+----------------+-------+----------------+
| Time           | Count | Total          |
+----------------+-------+----------------+
|       0.000001 |     0 |       0.000000 |
|       0.000010 |     0 |       0.000000 |
|       0.000100 |     1 |       0.000089 |
|       0.001000 |    14 |       0.010173 |
|       0.010000 |     0 |       0.000000 |
|       0.100000 |     0 |       0.000000 |
|       1.000000 |     0 |       0.000000 |
|      10.000000 |     0 |       0.000000 |
|     100.000000 |     0 |       0.000000 |
|    1000.000000 |     0 |       0.000000 |
|   10000.000000 |     0 |       0.000000 |
|  100000.000000 |     0 |       0.000000 |
| 1000000.000000 |     0 |       0.000000 |
| TOO LONG       |     0 | TOO LONG       |
+----------------+-------+----------------+
14 rows in set (0.00 sec)
{% endhighlight %}


默认的时间统计区间是按照基数 10 增长的，也就是说默认的区间如下：

{% highlight text %}
(0;10^-6], (10^-6;10^-5], (10^-5;10^-4], ..., (10^-1;10^1], (10^1; 10^2], ...
{% endhighlight %}

可以通过修改参数 query_response_time_range_base 来缩小时间区间，默认该是 10，实际的统计时间区间如上，可以修改为 2，则区间如下：

{% highlight text %}
(0;2^-19], (2^-19;2^-18], (2^-18;2^-17], ..., (2^-1; 2^1], (2^1; 2^2], ...
{% endhighlight %}

第一个区间总是最接近 0.000001 的区间开始；最后区间是到最接近且小于 10000000 处结束。

当然，有些比较从网上摘录的不错 SQL，可以根据自己需求修改。

{% highlight text %}
SELECT
    case TRIM(time)
        when '0.000001' then '<  1us'
        when '0.000010' then '< 10us'
        when '0.000100' then '<100us'
        when '0.001000' then '<  1ms'
        when '0.010000' then '< 10ms'
        when '0.100000' then '<100ms'
        when '1.000000' then '<   1s'
        when '10.000000' then '<  10s'
        when '100.000000' then '<100s'
        else '>100s'
    END as `RT area`,
    CONCAT(ROUND(100*count/query_count,2),"%") as percent,
    count
FROM(
    SELECT
        c.count,
        c.time,
        (
            SELECT SUM(a.count)
            FROM INFORMATION_SCHEMA.QUERY_RESPONSE_TIME as a
            WHERE a.count != 0
        ) as query_count
    FROM INFORMATION_SCHEMA.QUERY_RESPONSE_TIME as c
    WHERE c.count > 0
) d;
{% endhighlight %}








## 其它监控项

除了上述的监控项，常见的还有如下常用的方法。

#### 1. 是否可用

可以使用如下几条命令来查看当前 MySQL 服务是否处于运行状态。

{% highlight text %}
mysqladmin -h 127.1 -u root -pnew-password ping
mysqladmin -h 127.1 -u root -pnew-password status
{% endhighlight %}

#### 2. 用户管理

严禁对用户的 "host" 部分采用 "%"，除非你想从世界任何一个地方登陆；默认不要使用 GRANT ALL ON *.* 给用户过度赋权，

{% highlight text %}
mysql> CREATE USER 'user' IDENTIFIED BY 'password';
mysql> GRANT privileges TO 'user'@'host' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;

mysql> GRANT privileges  TO 'user'@'host' IDENTIFIED BY 'password' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;
{% endhighlight %}


#### 3. 连接数是否正常

主要查看客户是否有由于没正确关闭连接而死掉的连接，有多少失败的连接，是否有恶意连接等。



{% highlight text %}
----- 查看连接数，root会看到所有，其它用户只能看到自己的连接
mysql> SHOW FULL PROCESSLIST;
$ mysqladmin -h host -u user -p processlist

----- 当前失败连接数
mysql> SHOW GLOBAL STATUS LIKE 'aborted_connects';

----- 由于客户没有正确关闭连接而死掉的连接数
mysql> SHOW GLOBAL STATUS LIKE 'aborted_clients';

----- 最大连接数
mysql> SHOW CLOBAL VARIABLES LIKE 'max_connections';
mysql> SHOW GLOBAL STATUS LIKE 'max_connections';
{% endhighlight %}

#### 4. 慢查询日志

慢查询日志对 SQL 调优来说是非常重要的，它记录了超过指定时间 long_query_time 的查询语句；一般只在需要时开启。

<!--

#### 4. 全表扫描比例

计算方式如下

((Handler_read_rnd_next + Handler_read_rnd) / (Handler_read_rnd_next + Handler_read_rnd + Handler_read_first + Handler_read_next + Handler_read_key + Handler_read_prev))

对于全表扫描对性能的影响我想大家都比我清楚得多！

我们可以通过
SHOW GLOBAL STATUS LIKE 'Handler_read%'

取得相关参数的值再进行计算。

5 Innodb 死锁
当遇到死锁时Innodb会回滚事务。了解死锁何时发生对于追溯其发生的根本原因非常重要。我们必须知道产生了什么样的死锁，相关应用是否正确处理或已采取了相关措施。
可以通过
SHOW ENGINE INNODB STATUS
查看死锁相关状况
-->


<!--
<br><h2>innodb_data_read 和 innodb_data_reads</h2><p>
下面是 MySQL 官方文档里对于这两个参数的解释：
<pre style="font-size:0.8em; face:arial;">
innodb_data_read
The amount of data read since the server was started.

innodb_data_reads
The total number of data reads.
</pre>
接下来我们从源代码来解析这两个参数。在src/Srv0srv.c中存在对这两个参数和代码变量的映射关系的定义代码，如下：
     export_vars.innodb_data_read = srv_data_read;
     export_vars.innodb_data_reads = os_n_file_reads;
接着让我们来看下这两个代码变量在哪里进行了累加操作。
srv_data_read 尽有一处代码进行累加操作，buf0file.c 的 _fil_io 函数中，操作如下：
　　srv_data_read+= len;
从这行可以看出，innodb_data_read中实际存的是从磁盘上进行物理IO的字节数。

os_n_file_reads 也几乎只有一处代码进行累加（其他是windows的sync读这里忽略），在buf0file.c 的 中，操作如下：os_file_pread

　　os_n_file_reads++;

随后，代码即调用os_aio_array_reserve_slot将IO请求推入 simulated array，再根据wake_later标志位决定是否调用
os_aio_simulated_wake_handler_thread来立即唤醒IO工作队列。从这里也可以看到，innodb_data_reads记录的是innodb对于磁盘发起的读IO请求次数。
看到这里我们就很容易产生一个疑问：既然两个变量都是对磁盘发起的IO的计数器，为什么不直接放在一个函数的相邻行里呢？

回答这个问题，我们就需要对于innodb的simulated-aio和read-ahead算法有一定理解了。

进入到simulated的aio slot array的请求会有两种，一种是通过buf_read_page 过来的普通页的读请求，一种是通过buf_read_ahead_random/linear 过来的预读请求。从innodb的实现来说普通数据页的请求是需要立即返回响应的，所以是同步（sync）IO。而对于预读这样数据并非SQL所需要，仅是用于提升性能的页读取，这样的IO完全是可以异步的。这两个差异也是导致simulated aio出现的原因。把IO请求推入slot array后，数据页同步请求立即通知worker thread去os做同步IO。而预读IO请求会不断的推入slot array直到一次预读所需要的page全部推入slot中，然后再会通知worker thread。此外在worker thread中，也会判断一个segment的io请求是否相邻连续，如果连续则把这些请求合并后，作为一次OS IO发到下层存储中。

明白了这些也就不难理解为什么计数器要分开在不同的函数中计数了。如果累加都放在 _fil_io中进行，那么 innodb_data_read = 16K * innodb_data_reads （这里假设page没有做压缩）。然而在有了异步IO合并这个操作后，实际的innodb_data_reads会少于_fil_io中获得的计数次数。所以，通过 innodb_data_read / innodb_data_reads得到的比值也可以推断出一个MySQL实例中顺序IO或者可顺序预读的IO比例。

我们在production环境的服务器上做一个验证：

服务器A：在线用户访问的数据库，猜测随机IO较多

SHOW GLOBAL STATUS LIKE '%innodb_data_read%';

Innodb_data_read 46384854470656
Innodb_data_reads 2812101578

每次IO平均读取字节数=16494

服务器B：历史数据统计的数据库，数据内容和服务器A完全一样，猜测顺序IO较多

SHOW GLOBAL STATUS LIKE '%innodb_data_read%';

Innodb_data_read 54835669766144
Innodb_data_reads 2604758776

每次IO平均读取字节数=21052

可见顺序IO较多的MySQL的单次IO读取字节数确实要多于一个page的大小，说明IO合并效果明显。

而随机IO较多的MySQL的单词IO读取字节数几乎和一个page大小一致，即16K



最后我们再总结下一些结论：
Innodb_data_read   表示Innodb启动后，从物理磁盘上读取的字节数总和。
Innodb_data_reads 表示Innodb启动后，队伍物理磁盘发起的IO请求次数总和。
Innodb_data_read / Innodb_data_reads 得到的比值，越接近16K说明IO压力越倾向于随机IO，越远离16K说明IO从顺序预读中获得性能提升越多
http://www.cnblogs.com/cenalulu/archive/2013/10/16/3370626.html
</p>




 6. innodb缓冲池状态

 innodb_buffer_pool_reads: 平均每秒从物理磁盘读取页的次数

 innodb_buffer_pool_read_requests: 平均每秒从innodb缓冲池的读次数（逻辑读请求数）

 innodb_buffer_pool_write_requests: 平均每秒向innodb缓冲池的写次数

 innodb_buffer_pool_pages_dirty: 平均每秒innodb缓存池中脏页的数目

 innodb_buffer_pool_pages_flushed: 平均每秒innodb缓存池中刷新页请求的数目

 innodb缓冲池的读命中率
 innodb_buffer_read_hit_ratio = ( 1 - Innodb_buffer_pool_reads/Innodb_buffer_pool_read_requests) * 100

 Innodb缓冲池的利用率

 Innodb_buffer_usage =  ( 1 - Innodb_buffer_pool_pages_free / Innodb_buffer_pool_pages_total) * 100

 7. innodb日志

 innodb_os_log_fsyncs: 平均每秒向日志文件完成的fsync()写数量

 innodb_os_log_written: 平均每秒写入日志文件的字节数

 innodb_log_writes: 平均每秒向日志文件的物理写次数

 innodb_log_write_requests: 平均每秒日志写请求数

 8. innodb行

 innodb_rows_deleted: 平均每秒从innodb表删除的行数

 innodb_rows_inserted: 平均每秒从innodb表插入的行数

 innodb_rows_read: 平均每秒从innodb表读取的行数

 innodb_rows_updated: 平均每秒从innodb表更新的行数

 innodb_row_lock_waits:  一行锁定必须等待的时间数

 innodb_row_lock_time: 行锁定花费的总时间，单位毫秒

 innodb_row_lock_time_avg: 行锁定的平均时间，单位毫秒

 9. MyISAM读写次数

 key_read_requests: MyISAM平均每秒钟从缓冲池中的读取次数

 Key_write_requests: MyISAM平均每秒钟从缓冲池中的写入次数

 key_reads : MyISAM平均每秒钟从硬盘上读取的次数

 key_writes : MyISAM平均每秒钟从硬盘上写入的次数

 10. MyISAM缓冲池

 MyISAM平均每秒key buffer利用率

 Key_usage_ratio =Key_blocks_used/(Key_blocks_used+Key_blocks_unused)*100

 MyISAM平均每秒key buffer读命中率

 Key_read_hit_ratio=(1-Key_reads/Key_read_requests)*100

 MyISAM平均每秒key buffer写命中率

 Key_write_hit_ratio =(1-Key_writes/Key_write_requests)*100

 11. 其他

 slow_queries: 执行时间超过long_query_time秒的查询的个数（重要）

 sort_rows: 已经排序的行数

 open_files: 打开的文件的数目

 open_tables: 当前打开的表的数量

 select_scan: 对第一个表进行完全扫描的联接的数量

 此外，还有一些性能指标不能通过mysqladmin extended-status或show global status直接得到，但是十分重要。

 12. response time: 响应时间

 Percona提供了 tcprstat工具 统计响应时间，此功能默认是关闭的，可以通过设置参数query_response_time_stats=1打开这个功能。

 有两种方法查看响应时间：

 （1）通过命令SHOW QUERY_RESPONSE_TIME查看响应时间统计；

 （2）通过INFORMATION_SCHEMA里面的表QUERY_RESPONSE_TIME来查看。

 http://www.orczhou.com/index.php/2011/09/thanks-percona-response-time-distribution/comment-page-1/ （参考文章）



运行中的mysql状态查看

对正在运行的mysql进行监控，其中一个方式就是查看mysql运行状态。

(1)QPS(每秒Query量)
QPS = Questions(or Queries) / seconds
mysql > show  global  status like 'Question%';

(2)TPS(每秒事务量)
TPS = (Com_commit + Com_rollback) / seconds
mysql > show global status like 'Com_commit';
mysql > show global status like 'Com_rollback';



(3)key Buffer 命中率
mysql>show  global   status  like   'key%';
key_buffer_read_hits = (1-key_reads / key_read_requests) * 100%
key_buffer_write_hits = (1-key_writes / key_write_requests) * 100%

(4)InnoDB Buffer命中率
mysql> show status like 'innodb_buffer_pool_read%';
innodb_buffer_read_hits = (1 - innodb_buffer_pool_reads / innodb_buffer_pool_read_requests) * 100%

(5)Query Cache命中率
mysql> show status like 'Qcache%';
Query_cache_hits = (Qcahce_hits / (Qcache_hits + Qcache_inserts )) * 100%;

(6)Table Cache状态量
mysql> show global  status like 'open%';
比较 open_tables  与 opend_tables 值

(7)Thread Cache 命中率
mysql> show global status like 'Thread%';
mysql> show global status like 'Connections';
Thread_cache_hits = (1 - Threads_created / connections ) * 100%

(8)锁定状态
mysql> show global  status like '%lock%';
Table_locks_waited/Table_locks_immediate=0.3%  如果这个比值比较大的话，说明表锁造成的阻塞比较严重
Innodb_row_lock_waits innodb行锁，太大可能是间隙锁造成的

(10) Tmp Table 状况(临时表状况)
mysql > show status like 'Create_tmp%';
Created_tmp_disk_tables/Created_tmp_tables比值最好不要超过10%，如果Created_tmp_tables值比较大，
可能是排序句子过多或者是连接句子不够优化

(11) Binlog Cache 使用状况
mysql > show status like 'Binlog_cache%';
如果Binlog_cache_disk_use值不为0 ，可能需要调大 binlog_cache_size大小

{% highlight text %}

(12) Innodb_log_waits 量
Innodb_log_waits值不等于0的话，表明 innodb log  buffer 因为空间不足而等待
mysql > show status like 'innodb_log_waits';
{% endhighlight %}

 4. innodb文件读写次数
 innodb_data_reads：innodb平均每秒从文件中读取的次数
 innodb_data_writes：innodb平均每秒从文件中写入的次数
 innodb_data_fsyncs：innodb平均每秒进行fsync()操作的次数


 5. innodb读写量
 innodb_data_read：innodb平均每秒钟读取的数据量，单位为KB
 innodb_data_written：innodb平均每秒钟写入的数据量，单位为KB

-->


## MyISAM

在对 MyISAM 存储引擎调优时，很多文章推荐使用 Key_read_requests 和 Key_reads 的比例作为调优的参考，来设置 key_buffer_size 参数的值，而实际上这是错误的，详细可以参考 [Why you should ignore MySQL's key cache hit ratio](https://www.percona.com/blog/2010/02/28/why-you-should-ignore-mysqls-key-cache-hit-ratio/) 这篇文章。 <!-- ' -->

简单介绍如下。

### rate VS. ratio

首先需要注意的是，这里有两个重要的概念："miss rate"  一般是每秒 miss 的数目；"miss ratio" 表示从磁盘读取和从 cache 读取的比例，该参数没有单位。

如下的两个参数可以通过  SHOW GLOBAL STATUS 命令查看，官方文档的解释如下。

{% highlight text %}
Key_read_requests
The number of requests to read a key block from the cache.

Key_reads
The number of physical reads of a key block from disk.
{% endhighlight %}
也就是说，两者分别对应了：A) 从缓存读取索引的请求次数；B) 从磁盘读取索引的请求次数。

> NOTE: 实际上，Key_reads 统计的并非严格意义上的读磁盘，严格来说应该是发送系统请求的次数。如果文件系统中有缓存的话，实际耗时就是系统调用，并没有磁盘读取的耗时。

很多人认为 Key_reads/Key_read_requests 越小越好，否则就应该增大 key_buffer_size 的设置，但通过计数器的比例来调优有两个问题：

1. 比例并不显示数量的绝对值大小，并不知道总共的请求有多少；
2. 计数器并没有考虑时间因素。

假设有两台机器，其 miss ratio 分别为 23% 和 0.1% ，因为没有读的请求量，很难判断那台机器需要进行调优。比如，前者是 23/100，后者则是 10K/10M 。

### 参数指标

虽说 Key_read_requests 大比小好，但是对于系统调优而言，更有意义的应该是单位时间内的 Key_reads，通常可以通过 Key_reads / Uptime 计算；该参数可以通过如下命令得到：

{% highlight text %}
$ mysqladmin ext -ri10 | grep Key_reads
{% endhighlight %}

其中第一行表示的是系统启动后的总请求，在此可以忽略，下面的每行数值都表示 10 秒内的数据变化，这样就可以大致评估每秒有多少的磁盘请求，而且可以根据你的磁盘性能进行评估是否合理。

> NOTE: 命令里的 mysqladmin ext 其实就是 mysqladmin extended-status，你甚至可以简写成 mysqladmin e 。

### 结论

通过 Key_reads / Uptime 替换 Key_reads / Key_read_requests 。


<!--

## 缓存监控

MySQL 的服务器级别只提供了 query cache 缓存，对于不同的存储引擎，分别引入了不同的缓存机制，例如：MyISAM -- key cache；InnoDB -- buffer pool 。


什么是query cache

Mysql没有shared_pool缓存执行计划，但是提供了query cache缓存sql执行结果和文本，如果在生命周期内完全相同的sql再次运行，则连sql解析都免去了；

所谓完全相同，包含如下条件

Sql的大小写必须完全一样；

发起sql的客户端必须使用同样的字符集和通信协议；

sql查询同一数据库下的同一个表(不同数据库可能有同名表)；

Sql查询结果必须确定，即不能带有now()等函数；

当查询表发生DML或DDL，其缓存即失效；

针对mysql/information_schema/performance_schema的查询不缓存；

使用临时表的sql也不能缓存；



开启缓存后，每个select先检查是否有可用缓存(必须对这些表有select权限)，而每个写入操作先执行查询语句并使相关缓存失效；

5.5起可缓存基于视图的查询



Mysql维护一个hash表用来查找缓存，其key为sql text，数据库名以及客户端协议的版本等

相应参数

Have_query_cache：服务器是否支持查询缓存

Query_cache_type：0(OFF)不缓存；1(ON)缓存查询但不包括使用SQL_NO_CACHE的sql；2(DEMAND)只缓存使用SQL_CACHE的sql

Query_cache_size：字节为单位，即使query_cache_type=0也会为分配该内存，所以应该一并设置为0

Query_cache_limit：允许缓存的最大结果集，大于此的sql不予缓存

Query_cache_min_res_limit：用于限定块的最小尺寸，默认4K；



缓存的metadata占有40K内存，其可分为大小不等的多个子块，各块之间使用双向链表链接；根据其功能分别存储查询结果，基表和sql text等；

每个sql至少用到两个块：分别存储sql文本和查询结果，查询引用到的表各占一个块；

为了减少响应时间，每产生1行数据就发送给客户端；

数据库启动时调用malloc()分配查询缓存



查询缓存拥有一个全局锁，一旦有会话获取就会阻塞其他访问缓存的会话，因此当缓存大量sql时，缓存invalidation可能会消耗较长时间；



Innodb也可以使用查询缓存，每个表在数据字典中都有一个事务ID计数器，ID小于此值的事务不可使用缓存；表如果有锁(任何锁)则也不可使用查询缓存；



 状态变量

有关query cache的状态变量都以Qcache打头

mysql> SHOW STATUS LIKE 'Qcache%';

+-------------------------+--------+

| Variable_name           | Value  |

+-------------------------+--------+

| Qcache_free_blocks      | 36     |

| Qcache_free_memory      | 138488 |

| Qcache_hits             | 79570  |

| Qcache_inserts          | 27087  |

| Qcache_lowmem_prunes    | 3114   |

| Qcache_not_cached       | 22989  |

| Qcache_queries_in_cache | 415    |

| Qcache_total_blocks     | 912    |

+-------------------------+--------+

Qcache_inserts—被加到缓存中query数目

Qcache_queries_in_cache—注册到缓存中的query数目

缓存每被命中一次，Qcache_hits就加1；

计算缓存query的平均大小=(query_cache_size-Qcache_free_memory)/Qcache_queries_in_cache

Com_select = Qcache_not_cached + Qcache_inserts + queries with errors found during the column-privileges check

Select = Qcache_hits + queries with errors found by parser





Buffer pool

innodb即缓存表又缓存索引，还有设置多个缓冲池以增加并发，很像oracle

采用LRU算法：

所有buffer块位于同一列表，其中后3/8为old，每当新读入一个数据块时，先从队尾移除同等块数然后插入到old子列的头部，如再次访问该块则将其移至new子列的头部

Innodb_buffer_pool_size:  buffer pool大小

Innodb_buffer_pool_instances: 子buffer pool数量，buffer pool至少为1G时才能生效

Innodb_old_blocks_pct: 范围5 – 95， 默认为37即3/8，指定old子列的比重

Innodb_old_blocks_time: 以ms为单位，新插入old子列的buffer块必须等待指定时间后才能移入new列，适用于one-time scan频繁的操作，以避免经常访问的数据块被剔出buffer pool



可通过状态变量获知当前buffer pool的运行信息

Innodb_buffer_pool_pages_total：缓存池总页数

Innodb_buffer_pool_bytes_data：当前buffer pool缓存的数据大小，包括脏数据

Innodb_buffer_pool_pages_data：缓存数据的页数量

Innodb_buffer_pool_bytes_dirty：缓存的脏数据大小

Innodb_buffer_pool_pages_diry：缓存脏数据页数量

Innodb_buffer_pool_pages_flush：刷新页请求数量

Innodb_buffer_pool_pages_free：空闲页数量

Innodb_buffer_pool_pages_latched：缓存中被latch的页数量，这些页此刻正在被读或写；然而计算此变量比较消耗资源，只有在UNIV_DEBUG被定义了才可用

相关源代码如下

#ifdef UNIV_DEBUG
  {"buffer_pool_pages_latched",
  (char*) &export_vars.innodb_buffer_pool_pages_latched,  SHOW_LONG},
#endif /* UNIV_DEBUG */



Innodb_buffer_pool_pages_misc：用于维护诸如行级锁或自适应hash索引的内存页=总页数-空闲页-使用的页数量

Innodb_buffer_pool_read_ahead：预读入缓存的页数量

Innodb_buffer_pool_read_ahead_evicted：预读入但是1次都没用就被剔出缓存的页

Innodb_buffer_pool_read_requests：InnoDB的逻辑读请求次数

Innodb_buffer_pool_reads：直接从磁盘读取数据的逻辑读次数

Innodb_buffer_pool_wait_free：缓存中没有空闲页满足当前请求，必须等待部分页回收或刷新，记录等待次数

Innodb_buffer_pool_write_requests：向缓存的写数量





可使用innodb standard monitor监控buffer pool的使用情况，主要有如下指标：

Old database pages: old子列中的页数

Pages made young, not young: 从old子列移到new子列的页数，old子列中没有被再次访问的页数

Youngs/s  non-youngs/s: 访问old并导致其移到new列的次数







Key cache

5.5仅支持一个结构化变量，即key cache，其包含4个部件

Key_buffer_size

Key_cache_block_size：单个块大小，默认1k

Key_cache_division_limit：warm子列的百分比(默认100)，key cache buffer列表的分隔点，用于分隔host和warm子列表

Key_cache_age_threshold：页在hot子列中的生命周期，值越小则越快的移至warm列表



MyISAM只缓存索引，

可创建多个key buffer—set global hot_cache.key_buffer_size=128*1024

索引指定key buffer—cache index t1 in hot_cache

可在数据库启动时load index into key_buffer提前加载缓存，也可通过配置文件自动把索引映射到key cache



key_buffer_size = 4G

hot_cache.key_buffer_size = 2G

cold_cache.key_buffer_size = 2G

init_file=/path/to/data-directory/mysqld_init.sql

mysqld_init.sql内容如下

CACHE INDEX db1.t1, db1.t2, db2.t3 IN hot_cache

CACHE INDEX db1.t4, db2.t5, db2.t6 IN cold_cache



默认采用LRU算法，也支持名为中间点插入机制midpoint insertion strategy

索引页刚读入key cache时，被放在warm列的尾部，被访问3次后则移到hot列尾并循环移动，如果在hot列头闲置连续N次都没访问到，则会被移到warm列头，成为被剔出cache的首选；

N= block no* key_cache_age_threshold/100
-->



## xtools


{% highlight text %}
----- 标准数据统计，主要是命令次数的统计，transaction
    ins  Com_insert                               (diff)
    upd  Com_update                               (diff)
    del  Com_delete                               (diff)
    sel  Com_select                               (diff)
    tps  Com_insert + Com_update + Com_delete     (diff)

----- 线程处理，threads
    run  Threads_running
    con  Threads_connected
    cre  Threads_created                          (diff)
    cac  Threads_cached

----- 网络字节数，bytes
   recv  Bytes_received                           (diff)
   sent  Bytes_sent                               (diff)

----- buffer pool的缓存命中率
         Innodb_buffer_pool_read_requests         逻辑读总次数
         Innodb_buffer_pool_reads                 物理读总次数
   read  Innodb_buffer_pool_read_requests         每秒读请求(diff)
   hits  (Innodb_buffer_pool_read_requests-Innodb_buffer_pool_reads)/Innodb_buffer_pool_read_requests

----- buffer pool页的状态，innodb bp pages status
   data  Innodb_buffer_pool_pages_data            已经使用缓存页数
   free  Innodb_buffer_pool_pages_free            空闲缓存页数
  dirty  Innodb_buffer_pool_pages_dirty           脏页数目
  flush  Innodb_buffer_pool_pages_flushed         每秒刷新页数(diff)

----- innoDB相关的操作，innodb rows status
    ins  Innodb_rows_inserted                     (diff)
    upd  Innodb_rows_updated                      (diff)
    del  Innodb_rows_deleted                      (diff)
   read  Innodb_rows_read                         (diff)

----- 数据读写请求数，innodb data status
  reads  Innodb_data_reads                        数据读总次数(diff)
 writes  Innodb_data_writes                       数据写的总次数(diff)
   read  Innodb_data_read                         至此已经读的数据量(diff)
written  Innodb_data_written                      至此已经写的数据量(diff)

----- 日志写入磁盘请求，innodb log
 fsyncs  Innodb_os_log_fsyncs                     向日志文件写的总次数(diff)
written  Innodb_os_log_written                    写入日志文件的字节数(diff)
{% endhighlight %}

<!--
$mysql_headline1 .= "  his --log(byte)--  read ---query--- ";
$mysql_headline2 .= " list uflush  uckpt  view inside  que|";
-->


## 参考

可以参考官方文档 [Reference Manual - Server Status Variables](http://dev.mysql.com/doc/refman/en/server-status-variables.html)，主要介绍各个监控项的含义。

[Monitoring MySQL performance metrics](https://www.datadoghq.com/blog/monitoring-mysql-performance-metrics/)，一篇很不错的介绍 MySQL 监控项文章，包括上述的吞吐量、执行性能、链接情况、缓冲池使用情况等。

[Why you should ignore MySQL's key cache hit ratio](https://www.percona.com/blog/2010/02/28/why-you-should-ignore-mysqls-key-cache-hit-ratio/) 这篇文章介绍了 MyISAM 缓存的调优，其中的思想其它参数也可以考虑，也可以参考 [本地文档](/reference/mysql/Why you should ignore MySQL's key cache hit ratio.mht) 。

{% highlight text %}
{% endhighlight %}
