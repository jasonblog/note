---
title: MySQL GTID 简介
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,gtid,uuid
description: 全局事务 ID (Global Transaction ID, GTID) 是用来强化数据库在主备复制场景下，可以有效保证主备一致性、提高故障恢复、容错能力。接下来，看看 GTID 是如何实现的，以及如何使用。
---

全局事务 ID (Global Transaction ID, GTID) 是用来强化数据库在主备复制场景下，可以有效保证主备一致性、提高故障恢复、容错能力。

接下来，看看 GTID 是如何实现的，以及如何使用。

<!-- more -->

![gtid]({{ site.url }}/images/databases/mysql/gtid-logo.jpg "gtid"){: .pull-center }

## 简介

GTID 是一个已提交事务的编号，并且是一个全局唯一的编号，在 MySQL 中，GTID 实际上是由 UUID+TID 组成的。其中 UUID 是一个 MySQL 实例的唯一标识；TID 代表了该实例上已经提交的事务数量，并且随着事务提交单调递增。

使用 GTID 功能具体可以归纳为以下两点：

* 可以确认事务最初是在哪个实例上提交的；
* 方便了 Replication 的 Failover 。

第一条显而易见，对于第二点稍微介绍下。

在 GTID 出现之前，在配置主备复制的时候，首先需要确认 event 在那个 binlog 文件，及其偏移量；假设有 A(Master)、B(Slave)、C(Slave) 三个实例，如果主库宕机后，需要通过 ```CHANGE MASTER TO MASTER_HOST='xxx', MASTER_LOG_FILE='xxx', MASTER_LOG_POS=nnnn``` 指向新库。

这里的难点在于，同一个事务在每台机器上所在的 binlog 文件名和偏移都不同，这也就意味着需要知道新主库的文件以及偏移量，对于有一个主库+多个备库的场景，如果主库宕机，那么需要手动从备库中选出最新的备库，升级为主，然后重新配置备库。

这就导致操作特别复杂，不方便实施，这也就是为什么需要 MHA、MMM 这样的管理工具。

之所以会出现上述的问题，主要是由于各个实例 binlog 中的 event 以及 event 顺序是一致的，但是 binlog+position 是不同的；而通过 GTID 则提供了对于事物的全局一致 ID，主备复制时，只需要知道这个 ID 即可。

另外，利用 GTID，MySQL 会记录那些事物已经执行，从而也就知道接下来要执行那些事务。当有了 GTID 之后，就显得非常的简单；因为同一事务的 GTID 在所有节点上的值一致，那么就可以直接根据 GTID 就可以完成 failover 操作。


### UUID

MySQL 5.6 用 128 位的 server_uuid 代替了原本的 32 位 server_id 的大部分功能；主要是担心手动设置配置文件中的 server_id 时，可能会产生冲突，通过 UUID(128bits) 避免冲突。

首次启动时会调用 generate_server_uuid() 函数，自动生成一个 server_uuid，并保存到 auto.cnf 文件，目前该文件的唯一目的就是保存 server_uuid；下次启动时会自动读取 auto.cnf 文件，继续使用上次生成的 UUID 。

可以通过如下命令查看当前服务器的 UUID 值。

{% highlight text %}
mysql> SHOW GLOBAL VARIABLES LIKE 'server_uuid';
c133fbac-e07b-11e6-a219-286ed488dd40
{% endhighlight %}

在 Slave 向 Master 申请 binlog 时，会先发送 Slave 自己的 server_uuid，Master 会使用该值作为 kill_zombie_dump_threads 的参数，来终止冲突或者僵死的 BINLOG_DUMP 线程。

### GTID

MySQL 在 5.6 版本加入了 GTID 功能，GTID 也就是事务提交时创建分配的唯一标识符，所有事务均与 GTID 一一映射，其格式类似于：

{% highlight text %}
5882bfb0-c936-11e4-a843-000c292dc103:1
{% endhighlight %}

这个字符串，用 ```:``` 分开，前面表示这个服务器的 server_uuid，后面是事务在该服务器上的序号。

GTID 模式实例和非 GTID 模式实例是不能进行复制的，要求非常严格；而且 gtid_mode 是只读的，要改变状态必须 1) 关闭实例、2) 修改配置文件、3) 重启实例。

与 GTID 相关的参数可以参考如下：

{% highlight text %}
mysql> SHOW GLOBAL VARIABLES LIKE '%gtid%';
+----------------------------------+-------+
| Variable_name                    | Value |
+----------------------------------+-------+
| binlog_gtid_simple_recovery      | ON    |
| enforce_gtid_consistency         | ON    |
| gtid_executed                    |       |    已经在该实例上执行过的事务
| gtid_executed_compression_period | 1000  |
| gtid_mode                        | ON    |
| gtid_owned                       |       |    正在执行的事务的gtid以及对应的线程ID
| gtid_purged                      |       |    本机已经执行，且被PURGE BINARY LOG删除
| session_track_gtids              | OFF   |
+----------------------------------+-------+
8 rows in set (0.00 sec)

mysql> SHOW SESSION VARIABLES LIKE 'gtid_next';
+---------------+-----------+
| Variable_name | Value     |
+---------------+-----------+
| gtid_next     | AUTOMATIC |        session级别变量，表示下一个将被使用的gtid
+---------------+-----------+
1 row in set (0.02 sec)

{% endhighlight %}

对于 gtid_executed 变量，执行 ```reset master``` 会将该变量置空；而且还可以通过设置 gtid_next 执行一个空事务，来影响 gtid_executed 。


### 生命周期

一个 GTID 的生命周期包括：

1. 事务在主库上执行并提交，此时会给事务分配一个 gtid，该值会被写入到 binlog 中；
2. 备库读取 relaylog 中的 gtid，并设置 session 级别的 gtid_next 值，以告诉备库下一个事务必须使用这个值；
3. 备库检查该 gtid 是否已经被使用并记录到他自己的 binlog 中；
4. 由于 gtid_next 非空，备库不会生成一个新的 gtid，而是使用从主库获得的 gtid 。

由于 GTID 在全局唯一性，通过 GTID 可以在自动切换时对一些复杂的复制拓扑很方便的提升新主库及新备库。


### 通讯协议

开启 GTID 之后，除了将原有的 file+position 替换为 GTID 之外，实际上还实现了一套新的复制协议，简单来说，GTID 的目的就是保证所有节点执行了相同的事务。

老协议很简单，备库链接到主库时会带有 file+position 信息，用来确认从那个文件开始复制；而新协议则是在链接到主库时会发送当前备库已经执行的 GTID Sets，主库将所有缺失的事务发送给备库。

<!--
TODODO:
LINKKK: https://www.percona.com/blog/2014/05/09/gtids-in-mysql-5-6-new-replication-protocol-new-ways-to-break-replication/
-->


## 源码实现

在 binlog 中，与 GTID 相关的事件类型包括了：

* GTID_LOG_EVENT 随后事务的 GTID；
* ANONYMOUS_GTID_LOG_EVENT 匿名 GTID 事件类型；
* PREVIOUS_GTIDS_LOG_EVENT 当前 binlog 文件之前已经执行过的 GTID 集合，会记录在 binlog 文件头。

如下是一个示例：

{% highlight text %}
# at 120
# 130502 23:23:27 server id 119821  end_log_pos 231 CRC32 0x4f33bb48     Previous-GTIDs
# 10a27632-a909-11e2-8bc7-0010184e9e08:1,
# 7a07cd08-ac1b-11e2-9fcf-0010184e9e08:1-1129
{% endhighlight %}

除 gtid 之外，还有 gtid set 的概念，类似 ```7a07cd08-ac1b-11e2-9fcf-0010184e9e08:1-31```，其中变量 gtid_executed 和 gtid_purged 都是典型的 gtid set 类型变量；在一个复制拓扑结构中，gtid_executed 可能包含好几组数据。


### 结构体

在内存中通过 ```Gtid_state *gtid_state``` 全局变量维护了三个集合。

{% highlight cpp %}
class Gtid_state
{
private:
    Gtid_set lost_gtids;          // 对应gtid_purged
    Gtid_set executed_gtids;      // 对应gtid_executed
    Owned_gtids owned_gtids;      // 对应gtid_owned
};

Gtid_state *gtid_state= NULL;
{% endhighlight %}


<!--

在主库执行一个事务的过程中，关于Gtid主要涉及到以下几个部分：

事务开始，执行第一条SQL时，在写入第一个“BEGIN” 的QUERY EVENT 之前， 为binlog cache 的Group_cache中分配一个group(Group_cache::add_logged_group)，并写入一个Gtid_log_event，此时并未为其分配事务id,backtrace 如下：

{% highlight text %}
handler::ha_write_row()
 |-binlog_log_row()
   |-write_locked_table_maps()
     |-THD::binlog_write_table_map()
       |-binlog_start_trans_and_stmt()
         |-binlog_cache_data::write_event()
           |-Group_cache::add_logged_group()
{% endhighlight %}

暂时还不清楚什么时候一个事务里会有多个gtid的group_cache.




在 binlog group commit 的 flush 阶段：



第一步，调用Group_cache::generate_automatic_gno来为当前线程生成一个gtid，分配给thd->owned_gtid，并加入到owned_gtids中，backtrace如下：

{% highlight text %}
MYSQL_BIN_LOG::process_flush_stage_queue()
  |-assign_automatic_gtids_to_flush_group()
    |-Gtid_state::generate_automatic_gtid()     gtid_next为AUTOMATIC时，生成新的GTID
      |-lock_sidno()                            为当前sidno加锁，分配过程互斥
      |-get_server_sidno()
      |-get_automatic_gno()                     获取事务ID
      | |-get_server_sidno()                    初始化候选值
      |-acquire_ownership()
      | |-Owned_gtids::add_gtid_owner()         添加到owned_gtids集合中
      |-unlock_sidno()                          解锁
{% endhighlight %}

也就是说，直到事务完成，准备把 binlog 刷到 binlog cache 时，才会去为其分配 gtid 。


当gtid_next的类型为AUTOMATIC时，调用generate_automatic_gno生成事务id(gno)，分配流程大概如下：

2.gtid_state->get_automatic_gno(automatic_gtid.sidno);
        |–>初始化候选(candidate)gno为1
        |–>从logged_gtids[$sidno]中扫描，获取每个gno区间(iv)：
               |–>当candidate < iv->start（或者MAX_GNO，如果iv为NULL）时，判断candidate是否有被占用，如果没有的话，则使用该candidate，从函数返回，否则candidate++，继续本步骤
        |–>将candidate设置为iv->end，iv指向下一个区间，继续第2步
        从该过程可以看出，这里兼顾了区间存在碎片的场景，有可能分配的gno并不是全局最大的gno. 不过在主库不手动设置gtid_next的情况下，我们可以认为主库上的gno总是递增的。



第二步， 调用Gtid_state::update_on_flush将当前事务的gtid加入到logged_gtids中,backtrace如下：
MYSQL_BIN_LOG::process_flush_stage_queue->MYSQL_BIN_LOG::flush_thread_caches->binlog_cache_mngr::flush->binlog_cache_data::flush->MYSQL_BIN_LOG::write_cache->Gtid_state::update_on_flush
在bin log group commit的commit阶段

调用Gtid_state::update_owned_gtids_impl 从owned_gtids中将当前事务的gtid移除,backtrace 如下：
MYSQL_BIN_LOG::ordered_commit->MYSQL_BIN_LOG::finish_commit->Gtid_state::update_owned_gtids_impl

上述步骤涉及到的是对logged_gtids和owned_gtids的修改。而lost_gtids除了启动时维护外，就是在执行Purge操作时维护。

例如，当我们执行purge binary logs to ‘mysql-bin.000205’ 时， mysql-bin.index先被更新掉，然后再根据index文件找到第一个binlog文件的PREVIOUS_GTIDS_LOG_EVENT事件，更新lost_gtids集合，backtrace如下：
purge_master_logs->MYSQL_BIN_LOG::purge_logs->MYSQL_BIN_LOG::init_gtid_sets->read_gtids_from_binlog->Previous_gtids_log_event::add_to_set->Gtid_set::add_gtid_encoding->Gtid_set::add_gno_interval

关于binlog group commit，参见之前写的博客：http://mysqllover.com/?p=581
c.如何持久化GTID
当重启MySQL后，我们看到GTID_EXECUTED和GTID_PURGED和重启前是一致的。

持久化GTID，是通过全局对象gtid_state来管理的。gtid_state在系统启动时调用函数gtid_server_init分配内存；如果打开了binlog，则会做进一步的初始化工作：

quoted code:

5419       if (mysql_bin_log.init_gtid_sets(
5420             const_cast<Gtid_set *>(gtid_state->get_logged_gtids()),
5421             const_cast<Gtid_set *>(gtid_state->get_lost_gtids()),
5422             opt_master_verify_checksum,
5423             true/*true=need lock*/))
5424         unireg_abort(1);

gtid_state 包含3个gtid集合：logged_gtids， lost_gtids， owned_gtids，前两个都是gtid_set类型, owned_gtids类型为Owned_gtids

MYSQL_BIN_LOG::init_gtid_sets 主要用于初始化logged_gtids和lost_gtids,该函数的逻辑简单描述下：

1.扫描mysql-index文件，搜集binlog文件名，并加入到filename_list中
2.从最后一个文件开始往前读，依次调用函数read_gtids_from_binlog：
      |–>打开binlog文件，如果读取到PREVIOUS_GTIDS_LOG_EVENT事件
          (1)无论如何，将其加入到logged_gtids（prev_gtids_ev->add_to_set(all_gtids)）
          (2)如果该文件是第一个binlog文件，将其加入到lost_gtids（prev_gtids_ev->add_to_set(prev_gtids)）中.

      |–>获取GTID_LOG_EVENT事件
          (1) 读取该事件对应的sidno，sidno= gtid_ev->get_sidno(false);
               这是一个32位的整型，用sidno来代表一个server_uuid，从1开始计算，这主要处于节省内存的考虑。维护在全局对象global_sid_map中。
               当sidno还没加入到map时，调用global_sid_map->add_sid(sid)，sidno从1开始递增。

          (2) all_gtids->ensure_sidno(sidno)
               all_gtids是gtid_set类型，可以理解为一个集合，ensure_sidno就是要确保这个集合至少可以容纳sidno个元素

          (3) all_gtids->_add_gtid(sidno, gtid_ev->get_gno()
               将该事件中记录的gtid加到all_gtids[sidno]中(最终调用Gtid_set::add_gno_interval，这里实际上是把(gno, gno+1)这样一个区间加入到其中，这里
               面涉及到区间合并，交集等操作    )
当第一个文件中既没有PREVIOUS_GTIDS_LOG_EVENT， 也没有GTID_LOG_EVENT时，就继续读上一个文件
如果只存在PREVIOUS_GTIDS_LOG_EVENT事件，函数read_gtids_from_binlog返回GOT_PREVIOUS_GTIDS
如果还存在GTID_LOG_EVENT事件，返回GOT_GTIDS

这里很显然存在一个问题，即如果在重启前，我们并没有使用gtid_mode，并且产生了大量的binlog，在这次重启后，我们就可能需要扫描大量的binlog文件。这是一个非常明显的Bug， 后面再集中讨论。

3.如果第二部扫描，没有到达第一个文件，那么就从第一个文件开始扫描，和第2步流程类似，读取到第一个PREVIOUS_GTIDS_LOG_EVENT事件，并加入到lost_gtids中。

简单的讲，如果我们一直打开的gtid_mode，那么只需要读取第一个binlog文件和最后一个binlog文件，就可以确定logged_gtids和lost_gtids这两个GTID SET了。

二、备库上的GTID
a.如何保持主备GTID一致
由于在binlog中记录了每个事务的GTID，因此备库的复制线程可以通过设置线程级别GTID_NEXT来保证主库和备库的GTID一致。

默认情况下，主库上的thd->variables.gtid_next.type为AUTOMATIC_GROUP，而备库为GTID_GROUP

备库SQL线程gtid_next输出：
(gdb) p thd->variables.gtid_next
$2 = {
type = GTID_GROUP,
gtid = {
sidno = 2,
gno = 1127,
static MAX_TEXT_LENGTH = 56
},
static MAX_TEXT_LENGTH = 56
}

这些变量在执行Gtid_log_event时被赋值：Gtid_log_event::do_apply_event，大体流程为：
1.rpl_sidno sidno= get_sidno(true);  获取sidno
2.thd->variables.gtid_next.set(sidno, spec.gtid.gno);  设置gtid_next
3.gtid_acquire_ownership_single(thd);

     |–>检查该gtid是否在logged_gtids集合中，如果在的话，则返回（gtid_pre_statement_checks会忽略该事务）
     |–>如果该gtid已经被其他线程拥有，则等待(gtid_state->wait_for_gtid(thd, gtid_next))，否则将当前线程设置为owner(gtid_state->acquire_ownership(thd, gtid_next))

在上面提到，有可能当前事务的GTID已经在logged_gtids中，因此在执行Rows_log_event::do_apply_event或者mysql_execute_command函数中，都会去调用函数gtid_pre_statement_checks
该函数也会在每个SQL执行前，检查gtid是否合法，主要流程包括：
1.当打开选项enforce_gtid_consistency时，检查DDL是否被允许执行（thd->is_ddl_gtid_compatible()），若不允许，返回GTID_STATEMENT_CANCEL
2.检查当前SQL是否会产生隐式提交并且gtid_next被设置（gtid_next->type != AUTOMATIC_GROUP），如果是的话，则会抛出错误ER_CANT_DO_IMPLICIT_COMMIT_IN_TRX_WHEN_GTID_NEXT_IS_SET 并返回GTID_STATEMENT_CANCEL，注意这里会导致bug#69045
3.对于BEGIN/COMMIT/ROLLBACK/(SET OPTION 或者 SELECT )且没有使用存储过程/ 这几种类型的SQL，总是允许执行，返回GTID_STATEMENT_EXECUTE
4.gtid_next->type为UNDEFINED_GROUP，抛出错误ER_GTID_NEXT_TYPE_UNDEFINED_GROUP，返回GTID_STATEMENT_CANCEL
5.gtid_next->type == GTID_GROUP且thd->owned_gtid.sidno == 0时， 返回GTID_STATEMENT_SKIP

其中第五步中处理了函数gtid_acquire_ownership_single的特殊情况

b.备库如何发起DUMP请求

引入GTID，最大的好处当然是我们可以随心所欲的切换主备拓扑结构了。在一个正常运行的复制结构中，我们可以在备库简单的执行如下SQL：

CHANGE MASTER TO MASTER_USER=’$USERNAME’, MASTER_HOST=’ ‘, MASTER_PORT=’ ‘, MASTER_AUTO_POSITION=1;

打开GTID后，我们就无需指定binlog文件或者位置，MySQL会自动为我们做这些事情。这里的关键就是MASTER_AUTO_POSITION。IO线程连接主库，可以大概分为以下几步：
1.IO线程在和主库建立TCP链接后，会去获取主库的uuid（get_master_uuid），然后在主库上设置一个用户变量@slave_uuid(io_thread_init_commands)

2.之后，在主库上注册SLAVE（register_slave_on_master）

在主库上调用register_slave来注册备库，将备库的host,user,password,port,server_id等信息记录到slave_list哈希中。

3.调用request_dump，开始向主库请求数据，这里分两种情况：
MASTER_AUTO_POSITION=0时，向主库发送命令的类型为COM_BINLOG_DUMP，这是传统的请求BINLOG的模式
MASTER_AUTO_POSITION=1时，命令类型为COM_BINLOG_DUMP_GTID，这是新的方式。
这里我们只讨论第二种。第二种情况下，会先去读取备库已经执行的gtid集合
quoted code in rpl_slave.cc :

2974   if (command == COM_BINLOG_DUMP_GTID)
2975   {
2976     // get set of GTIDs
2977     Sid_map sid_map(NULL/*no lock needed*/);
2978     Gtid_set gtid_executed(&sid_map);
2979     global_sid_lock->wrlock();
2980     gtid_state->dbug_print();
2981     if (gtid_executed.add_gtid_set(mi->rli->get_gtid_set()) != RETURN_STATUS_OK ||
2982         gtid_executed.add_gtid_set(gtid_state->get_logged_gtids()) !=

2983         RETURN_STATUS_OK)
构建完成发送包后，发送给主库。

在主库上接受到命令后，调用入口函数com_binlog_dump_gtid，流程如下：
1.slave_gtid_executed.add_gtid_encoding(packet_position, data_size) ;读取备库传来的GTID SET
2.读取备库的uuid(get_slave_uuid)，被根据uuid来kill僵尸线程(kill_zombie_dump_threads)
这也是之前SLAVE IO线程执行SET @SLAVE_UUID的用处。
3.进入mysql_binlog_send函数：
         |–>调用MYSQL_BIN_LOG::find_first_log_not_in_gtid_set，从最后一个Binlog开始扫描，获取文件头部的PREVIOUS_GTIDS_LOG_EVENT，如果它是slave_gtid_executed的子集，保存当前binlog文件名，否则继续向前扫描。
         这一步的目的就是为了找出备库执行到的最后一个Binlog文件。

         |–>从这个文件头部开始扫描，遇到GTID_EVENT时，会去判断该GTID是否包含在slave_gtid_executed中：
                         Gtid_log_event gtid_ev(packet->ptr() + ev_offset,
                                 packet->length() – checksum_size,
                                 p_fdle);
                          skip_group= slave_gtid_executed->contains_gtid(gtid_ev.get_sidno(sid_map),
                                                     gtid_ev.get_gno());
         主库通过GTID决定是否可以忽略事务，从而决定执行开始的位置


注意，在使用MASTER_LOG_POSITION后，就不要指定binlog的位置，否则会报错。
三、运维操作
a.如何忽略复制错误

当备库复制出错时，传统的跳过错误的方法是设置sql_slave_skip_counter,然后再START SLAVE。
但如果打开了GTID，就会设置失败：

mysql> set global sql_slave_skip_counter = 1;

ERROR 1858 (HY000): sql_slave_skip_counter can not be set when the server is running with @@GLOBAL.GTID_MODE = ON. Instead, for each transaction that you want to skip, generate an empty transaction with the same GTID as the transaction

提示的错误信息告诉我们，可以通过生成一个空事务来跳过错误的事务。

我们手动产生一个备库复制错误：

Last_SQL_Error: Error ‘Unknown table ‘test.t1” on query. Default database: ‘test’. Query: ‘DROP TABLE `t1` /* generated by server */’

查看binlog中，该DDL对应的GTID为7a07cd08-ac1b-11e2-9fcf-0010184e9e08:1131

在备库上执行：

mysql> STOP SLAVE;
Query OK, 0 rows affected (0.00 sec)
mysql> SET SESSION GTID_NEXT = ‘7a07cd08-ac1b-11e2-9fcf-0010184e9e08:1131’;
Query OK, 0 rows affected (0.00 sec)
mysql> BEGIN; COMMIT;
Query OK, 0 rows affected (0.00 sec)

Query OK, 0 rows affected (0.00 sec)



mysql> SET SESSION GTID_NEXT = AUTOMATIC;

Query OK, 0 rows affected (0.00 sec)
mysql> START SLAVE;

再查看show slave status，就会发现错误事务已经被跳过了。这种方法的原理很简单，空事务产生的GTID加入到GTID_EXECUTED中，这相当于告诉备库，这个GTID对应的事务已经执行了。
b.重指主库
使用change master to …. , MASTER_AUTO_POSITION=1；
注意在整个复制拓扑中，都需要打开gtid_mode

c.新的until条件
5.6提供了新的util condition，可以根据GTID来决定备库复制执行到的位置
SQL_BEFORE_GTIDS：在指定的GTID之前停止复制
SQL_AFTER_GTIDS ：在指定的GTID之后停止复制

判断函数为Relay_log_info::is_until_satisfied

详细文档见：http://dev.mysql.com/doc/refman/5.6/en/start-slave.html

d.适当减小binlog文件的大小
如果开启GTID，理论上最好调小每个binlog文件的最大值，以缩小扫描文件的时间。
四、存在的bug
bug#69097， 即使关闭了gtid_mode，也会在启动时去扫描binlog文件。
当在重启前没有使用gtid_mode，重启后可能会去扫描所有的binlog文件，如果Binlog文件很多的话，这显然是不可接受的。

bug#69096，无法通过GTID_NEXT_LIST来跳过复制错误，因为默认编译下，GTID_NEXT_LIST未被编译进去。
TODO:GTID_NEXT_LIST的逻辑上面均未提到，有空再看。

bug#69095，将备库的复制模式设置为STATEMENT/MIXED。 主库设置为ROW模式，执行DML 会导致备库复制中断

Last_SQL_Error: Error executing row event: ‘Cannot execute statement: impossible to write to binary log since statement is in row format and BINLOG_FORMAT = STATEMENT.’

判断报错的backtrace：
handle_slave_worker->slave_worker_exec_job->Rows_log_event::do_apply_event->open_and_lock_tables->open_and_lock_tables->lock_tables->THD::decide_logging_format


解决办法：将备库的复制模式设置为’ROW’ ，保持主备一致
该bug和GTID无关

bug#69045, 当主库执行类似 FLUSH PRIVILEGES这样的动作时，如果主库和备库都开启了gtid_mode，会导致复制中断
Last_SQL_Error: Error ‘Cannot execute statements with implicit commit inside a transaction when @@SESSION.GTID_NEXT != AUTOMATIC or @@SESSION.GTID_NEXT_LIST != NULL.’ on query. Default database: ”. Query: ‘flush privileges’

也是一个很低级的bug，在MySQL5.6.11版本中，如果有可能导致隐式提交的事务， 则gtid_next必须等于AUTOMATIC，对备库复制线程而言，很容易就中断了，判断逻辑在函数gtid_pre_statement_checks中





对于 GTID 的实现，MySQL 和 MariaDB 的实现不同。





Multi-threaded Slave 备库的并行复制， slave_parallel_workers > 1 。


Crash-safe Slave 也就是说备库崩溃时，可以自动恢复，需要注意的是，只对 InnoDB 有效，而且需要设置 relay_log_info_repository=TABLE and relay_log_recovery=1 。


组提交，包括了 binlog 以及 InnoDB 的组提交。





???假设现在已经搭建了主备服务器，然后可以通过 mysqlreplicate 命令配置主从复制 (master slave rpl-user 参数必须) 。
???mysqlreplicate --master=root:new-password@127.0.0.1:3307 --slave=root:new-password@127.0.0.1:3308 --rpl-user=mysync:kidding


搭建配置完成之后可以通过 mysqlrplcheck 检查主备复制的状态。
???mysqlrplcheck --master=root:new-password@127.0.0.1:3307 --slave=root:new-password@127.0.0.1:3308


mysqlrplshow 查看主从架构，备库信息通过 SHOW SLAVE HOSTS 命令获取。
???mysqlrplshow --master=root:new-password@127.0.0.1:3307 --discover-slaves-login=root:new-password


mysqlfailover 监视主从健康状态，需要保证配置文件中添加 gtid-mode=on+enforce-gtid-consistency=on 。
???mysqlfailover --master=root:new-password@127.0.0.1:3307 --discover-slaves-login=root:new-password



1. 创建package.json文件，执行npm init -yes创建该初始文件。

2. 创建main.js文件，内容如下。
var React = require('react');
var ReactDOM = require('react-dom');
ReactDOM.render(
  <h1>Hello, world!</h1>,
  document.getElementById('example')
);

3. 安装browserify，并生成浏览器可用的javascript文件。
npm install -g browserify
npm install --save-dev react react-dom babelify babel-preset-react babel-preset-es2015
其中babelify(Browserify的babel转换器)，babel-preset-react(针对React的babel转码规则包)。
cat .babelrc
{
  "presets": ["react", "es2015"]
}

4. 使用Browserify对main.js处理及打包。
如下的只需要包含bundle.js即可，无需react.js+react-dom.js。
browserify -t babelify --presets react,es2015 src/foobar.js -o bundle.js
如下需要包含react.js+react-dom.js。
babel --presets react src --watch --out-dir build/js

{
  "name": "dbadmin",
  "description": "A Database admin platform",
  "version": "0.1.0",
  "main": "src/index.js",
  "devDependencies": {
    "babel-preset-react": "^6.23.0",
    "babel-preset-es2015": "^6.6.0",
    "babelify": "^7.3.0",
    "grunt": "~0.4.5",
    "grunt-contrib-jshint": "~0.10.0",
    "grunt-contrib-nodeunit": "~0.4.1",
    "grunt-contrib-uglify": "~0.5.0",
    "react": "^15.4.2",
    "browserify": "^13.0.0",
    "watchify": "^3.7.0",
    "react-dom": "^15.4.2"
  },
  "dependencies": {
    "babel-preset-react": "^6.23.0",
    "babelify": "^7.3.0",
    "react": "^15.4.2",
    "react-dom": "^15.4.2"
  },
  "scripts": {
    "build": "browserify src/index.js -t babelify -o bundle.js",
    "start": "watchify ./index.js -v -t babelify -o bundle.js"
  }
}
阮一峰的blog关于Reactjs介绍。
http://www.ruanyifeng.com/blog/2015/03/react.html
http://www.ruanyifeng.com/blog/2016/02/react-testing-tutorial.html
官方关于react的教程。
https://github.com/reactjs/react-tutorial/

1.C语言：PC-Lint、codedex
2.Java：findbugs、PMD、checkstyle（不强制要求）
3.python：flake8
4.JavaScript：JSHint
https://addons.mozilla.org/zh-CN/firefox/addon/react-devtools/


JSX基本语法规则：
    遇到以 < 开头的 HTML 标签，就用 HTML 规则解析；遇到以 { 开头的代码块，就用 JavaScript 规则解析。
什么是JSX？
把 HTML 模板直接嵌入到 JS 代码里面，从而做到模板和组件关联。但 JS 不支持这种包含 HTML 的语法，所以需要通过工具将 JSX 编译输出成 JS 代码才能使用。
为了把 JSX 转成标准的 JavaScript，有如下两种转换方法：
    1. 用 <script type="text/babel"> 标签，并引入 Babel 来完成在浏览器里的代码转换。
    2. 在浏览器里打开这个html，你应该可以看到成功的消息！
为什么要使用JSX？
    JSX 执行更快，因为它在编译为 JavaScript 代码后进行了优化。
    它是类型安全的，在编译过程中就能发现错误。
    使用 JSX 编写模板更加简单快速。

-->

























## GTID 限制

开启 GTID 之后，会由部分的限制，内容如下。

### 更新非事务引擎表

GTID 同步复制是基于事务的，所以 MyISAM 存储引擎不支持，这可能导致多个 GTID 分配给同一个事务。

{% highlight text %}
mysql> CREATE TABLE error (ID INT) ENGINE=MyISAM;
Query OK, 0 rows affected (0.00 sec)
mysql> INSERT INTO error VALUES(1),(2);
Query OK, 2 rows affected (0.00 sec)
Records: 2  Duplicates: 0  Warnings: 0

mysql> CREATE TABLE hello (ID INT) ENGINE=InnoDB;
Query OK, 0 rows affected (0.00 sec)
mysql> INSERT INTO hello VALUES(1),(2);
Query OK, 2 rows affected (0.00 sec)
Records: 2  Duplicates: 0  Warnings: 0

mysql> SET AUTOCOMMIT = 0；
Query OK, 0 rows affected (0.00 sec)
mysql> BEGIN;
Query OK, 0 rows affected (0.00 sec)
mysql> UPDATE hello SET id = 3 WHERE id =2;
Query OK, 1 row affected (0.00 sec)
Rows matched: 1  Changed: 1  Warnings: 0
mysql> UPDATE error SET id = 3 WHERE id =2;
ERROR 1785 (HY000): When @@GLOBAL.ENFORCE_GTID_CONSISTENCY = 1, updates to non-transactional
tables can only be done in either autocommitted statements or single-statement transactions,
and never in the same statement as updates to transactional tables.
{% endhighlight %}

### CREATE TABLE ... SELECT

上述的语句不支持，因为该语句会被拆分成 ```CREATE TABLE``` 和 ```INSERT ``` 两个事务，并且这个两个事务被分配了同一个 GTID，这会导致 ```INSERT``` 被备库忽略掉。

{% highlight text %}
mysql> CREATE TABLE hello ENGINE=InnoDB AS SELECT * FROM hello;
ERROR 1786 (HY000): Statement violates GTID consistency: CREATE TABLE ... SELECT.
{% endhighlight %}

### 临时表

事务内部不能执行创建删除临时表语句，但可以在事务外执行，但必须设置 ```set autocommit=1``` 。

{% highlight text %}
mysql> CREATE TEMPORARY TABLE test(id INT);
ERROR 1787 (HY000): Statement violates GTID consistency: CREATE TEMPORARY TABLE and DROP
TEMPORARY TABLE can only be executed outside transactional context.  These statements are
also not allowed in a function or trigger because functions and triggers are also considered
to be multi-statement transactions.

mysql> SET AUTOCOMMIT = 1;
Query OK, 0 rows affected (0.00 sec)
mysql> CREATE TEMPORARY TABLE test(id INT);
Query OK, 0 rows affected (0.04 sec)
{% endhighlight %}

与临时表相关的包括了 ```CREATE/DROP TEMPORARY TABLE``` 临时表操作。


### 总结

实际上，一般启动 GTID 时，可以启用 enforce-gtid-consistency 选项，从而在执行上述不支持的语句时，将会返回错误。



## 运维相关

简单介绍一些常见的运维操作。

当备库配置为 GTID 复制时，可以通过 ```SHOW SLAVE STATUS``` 命令查看其中的 ```Retrieved_Gtid_Set``` 和 ```Executed_Gtid_Set```，分别表示已经从主库获取，以及已经执行的事务。


{% highlight text %}
mysql> SHOW SLAVE STATUS\G
*************************** 1. row ***************************
               Slave_IO_State: Waiting for master to send event
                  Master_Host: 192.168.0.123
                  Master_User: mysync
                  Master_Port: 3306
                Connect_Retry: 60
              Master_Log_File: mysqld-bin.000005
          Read_Master_Log_Pos: 879
               Relay_Log_File: mysqld-relay-bin.000009      # 备库中的relaylog文件
                Relay_Log_Pos: 736                          # 备库执行的偏移量
        Relay_Master_Log_File: mysqld-bin.000005
             Slave_IO_Running: Yes
            Slave_SQL_Running: No
                          ... ...
                 Skip_Counter: 0
          Exec_Master_Log_Pos: 634
              Relay_Log_Space: 1155
                          ... ...
                Last_IO_Errno: 0
                Last_IO_Error:
               Last_SQL_Errno: 1062
               Last_SQL_Error: Error 'Duplicate entry '1' for key 'PRIMARY'' on query.
                               Default database: ''. Query: 'insert into wb.t1 set i=1'
  Replicate_Ignore_Server_Ids:
             Master_Server_Id: 3
                  Master_UUID: 46fdb7ad-5852-11e6-92c9-0800274fb806
                          ... ...
           Retrieved_Gtid_Set: 46fdb7ad-5852-11e6-92c9-0800274fb806:1-4,
                               4fbe2d57-5843-11e6-9268-0800274fb806:1-3
            Executed_Gtid_Set: 46fdb7ad-5852-11e6-92c9-0800274fb806:1-3,
                               4fbe2d57-5843-11e6-9268-0800274fb806:1-3,
                               81a567a8-5852-11e6-92cb-0800274fb806:1
                Auto_Position: 1
1 row in set (0.00 sec)
{% endhighlight %}

一般来说是已经从主库复制过来，只是在执行的时候报错，可以从上述的状态中查看，然后通过命令 ```show relaylog events in 'mysqld-relay-bin.000009' from 736\G``` 确认。

### 忽略复制错误

当备库复制出错时，如果仍采用传统的跳过错误方法，也就是设置 ```sql_slave_skip_counter```，然后再 ```START SLAVE```；但如果打开了 GTID，在设置上述参数时，就会报错。

提示的错误信息告诉我们，可以通过生成一个空事务来跳过错误的事务，示例如下。

{% highlight text %}
mysql> CREATE DATABASE test;
Query OK, 1 row affected (0.00 sec)
mysql> USE test;
Database changed
mysql> CREATE TABLE foobar(id INT PRIMARY KEY);
Query OK, 0 rows affected (0.01 sec)

----- 备库执行如下SQL
mysql> INSERT INTO foobar VALUES(1),(2),(3);
Query OK, 3 rows affected (0.00 sec)
Records: 3  Duplicates: 0  Warnings: 0
----- 主库执行如下SQL
mysql> INSERT INTO foobar VALUES(1),(4),(5);
Query OK, 3 rows affected (0.00 sec)
Records: 3  Duplicates: 0  Warnings: 0
mysql> SHOW MASTER STATUS;
+------------------+----------+--------------+------------------+------------------------------------------+
| File             | Position | Binlog_Do_DB | Binlog_Ignore_DB | Executed_Gtid_Set                        |
+------------------+----------+--------------+------------------+------------------------------------------+
| mysql-bin.000002 |     1109 |              |                  | ab298681-00f5-11e7-a02a-ac2b6e8b4228:1-5 |
+------------------+----------+--------------+------------------+------------------------------------------+
1 row in set (0.00 sec)

----- 备库执行如下SQL
mysql> SHOW SLAVE STATUS \G
*************************** 1. row ***************************
... ...
             Slave_IO_Running: Yes
            Slave_SQL_Running: No
                   Last_Errno: 1062
                   Last_Error: Error 'Duplicate entry '1' for key 'PRIMARY'' on query.
Default database: 'test'. Query: 'INSERT INTO foobar VALUES(1),(4),(5)'
                 Skip_Counter: 0
           Retrieved_Gtid_Set: ab298681-00f5-11e7-a02a-ac2b6e8b4228:1-5
            Executed_Gtid_Set: ab298681-00f5-11e7-a02a-ac2b6e8b4228:1-4,
                               ad9b6105-00f5-11e7-a114-ac2b6e8b4228:1-2
                Auto_Position: 1
... ...
1 row in set (0.00 sec)

mysql> SET @@SESSION.GTID_NEXT= 'ab298681-00f5-11e7-a02a-ac2b6e8b4228:5';
Query OK, 0 rows affected (0.00 sec)
mysql> BEGIN;
Query OK, 0 rows affected (0.00 sec)
mysql> COMMIT;
Query OK, 0 rows affected (0.00 sec)
mysql> SET SESSION GTID_NEXT = AUTOMATIC;
Query OK, 0 rows affected (0.00 sec)

mysql> START SLAVE;
Query OK, 0 rows affected (0.00 sec)
{% endhighlight %}

再查看 ```SHOW SLAVE STATUS``` 时，就会发现错误事务已经被跳过了；这种方法的原理很简单，空事务产生的 GTID 加入到 GTID_EXECUTED 中，相当于告诉备库，这个 GTID 对应的事务已经执行了。

注意，此时主从会导致数据不一致，需要进行修复。


### 主库事件被清除

变量 gtid_purged 记录了本机已经执行过，且已被 ```PURGE BINARY LOGS TO``` 命令清理的 gtid_set ；在此，看看如果主库上把某些备库还没有获取到的 gtid event 清理后会有什么样的结果。

{% highlight text %}
----- 主库执行如下SQL
mysql> FLUSH LOGS; CREATE TABLE foobar1 (id INT) ENGINE=InnoDB;
Query OK, 0 rows affected (0.01 sec)
Query OK, 0 rows affected (0.02 sec)
mysql> FLUSH LOGS; CREATE TABLE foobar2 (id INT) ENGINE=InnoDB;
Query OK, 0 rows affected (0.01 sec)
Query OK, 0 rows affected (0.02 sec)
mysql> SHOW MASTER STATUS;
+------------------+----------+--------------+------------------+------------------------------------------+
| File             | Position | Binlog_Do_DB | Binlog_Ignore_DB | Executed_Gtid_Set                        |
+------------------+----------+--------------+------------------+------------------------------------------+
| mysql-bin.000004 |      379 |              |                  | 91116597-016c-11e7-94db-ac2b6e8b4228:1-5 |
+------------------+----------+--------------+------------------+------------------------------------------+
1 row in set (0.00 sec)
mysql> SHOW GLOBAL VARIABLES LIKE 'gtid_%';
+----------------------------------+------------------------------------------+
| Variable_name                    | Value                                    |
+----------------------------------+------------------------------------------+
| gtid_executed                    | 91116597-016c-11e7-94db-ac2b6e8b4228:1-5 |
| gtid_executed_compression_period | 1000                                     |
| gtid_mode                        | ON                                       |
| gtid_owned                       |                                          |
| gtid_purged                      |                                          |
+----------------------------------+------------------------------------------+
5 rows in set (0.02 sec)
mysql> PURGE BINARY LOGS TO 'mysql-bin.000004';
Query OK, 0 rows affected (0.00 sec)
mysql> SHOW GLOBAL VARIABLES LIKE 'gtid_%';
+----------------------------------+------------------------------------------+
| Variable_name                    | Value                                    |
+----------------------------------+------------------------------------------+
| gtid_executed                    | 91116597-016c-11e7-94db-ac2b6e8b4228:1-5 |
| gtid_executed_compression_period | 1000                                     |
| gtid_mode                        | ON                                       |
| gtid_owned                       |                                          |
| gtid_purged                      | 91116597-016c-11e7-94db-ac2b6e8b4228:1-4 |
+----------------------------------+------------------------------------------+
5 rows in set (0.01 sec)

----- 在备库上执行如下SQL
mysql> START SLAVE;
Query OK, 0 rows affected (0.01 sec)
mysql> SHOW SLAVE STATUS\G
*************************** 1. row ***************************
                          ......
             Slave_IO_Running: No
            Slave_SQL_Running: Yes
                          ......
                Last_IO_Errno: 1236
                Last_IO_Error: Got fatal error 1236 from master when reading data from
binary log: 'The slave is connecting using CHANGE MASTER TO MASTER_AUTO_POSITION = 1,
but the master has purged binary logs containing GTIDs that the slave requires.'
                          ......
1 row in set (0.00 sec)
{% endhighlight %}

接下来我们在备库忽略 purged 的部分，然后强行同步，在备库同样设置 gtid_purged 变量。

{% highlight text %}
----- 备库上清除gtid_executed，然后设置gtid_purged，忽略主库的事务
mysql> RESET MASTER;
Query OK, 0 rows affected (0.05 sec)
mysql> SET GLOBAL gtid_purged = "91116597-016c-11e7-94db-ac2b6e8b4228:1-5";
Query OK, 0 rows affected (0.05 sec)

----- 备库上执行上述的SQL，需要根据具体情况修复
mysql> CREATE TABLE foobar1 (id INT) ENGINE=InnoDB;
Query OK, 0 rows affected (0.01 sec)
mysql> CREATE TABLE foobar2 (id INT) ENGINE=InnoDB;
Query OK, 0 rows affected (0.02 sec)

----- 启动备库即可
mysql> START SLAVE;
Query OK, 0 rows affected (0.02 sec)
{% endhighlight %}

实际生产应用中，当遇到上述的情况后，需要 DBA 人为保证该备库数据和主库一致；或者即使不一致，这些差异也不会导致今后的主从异常，例如，所有主库上只有 insert 没有 update 。

### Errant-Transaction

简单来说，就是没有在主库执行，而是直接在备库执行的事务，通常可能是在修复备库的问题或者应用异常写入了备库导致。

如果发生 ET 的备库被提升为主库，那么根据 GTID 协议，新主库就会发现备库没有执行 ET 中的事务，接下来就可能会发生如下两种情况：

1. 备库中 ET 对应的 binlog 仍然存在，那么会将相应的事件发送给新的备库，此时则会导致数据不一致或者发生其它异常；
2. 备库中 ET 对应的 binlog 已经被删除，由于无法发送给备库，那么会导致复制异常。

对于有些需要修复备库的任务可以通过 ``` SET sql_log_bin=0``` 命令，设置会话参数，防止生成 ET，当然，此时需要保证数据一致性。在修复时有两种方案：

1. 在 GTID 的执行历史中删除 ET，这样即使备库被提升为主库，也不会发生异常；
2. 在其它 MySQL 服务中执行空白的事务，使其它库认为已经执行了 ET，那么 Failover 之后也不会尝试获取相应的事件。

接下来看个示例。

{% highlight text %}
----- 在主库执行如下SQL，查看主库已执行事务对应的GTID Sets
mysql> SHOW MASTER STATUS\G
*************************** 1. row ***************************
... ...
Executed_Gtid_Set: 8e349184-bc14-11e3-8d4c-0800272864ba:1-30,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-7

----- 同上，在备库执行
mysql> SHOW SLAVE STATUS\G
... ...
Executed_Gtid_Set: 8e349184-bc14-11e3-8d4c-0800272864ba:1-29,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-9

----- 比较两个GTID Sets
mysql> SELECT gtid_subset('8e349184-bc14-11e3-8d4c-0800272864ba:1-29,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-9','8e349184-bc14-11e3-8d4c-0800272864ba:1-30,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-7') AS slave_is_subset;
+-----------------+
| slave_is_subset |
+-----------------+
|               0 |
+-----------------+
1 row in set (0.00 sec)

----- 获取对应的差值
mysql> SELECT gtid_subtract('8e349184-bc14-11e3-8d4c-0800272864ba:1-29,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-9','8e349184-bc14-11e3-8d4c-0800272864ba:1-30,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-7') AS errant_transactions;
+------------------------------------------+
| errant_transactions                      |
+------------------------------------------+
| 8e3648e4-bc14-11e3-8d4c-0800272864ba:8-9 |
+------------------------------------------+
1 row in set (0.00 sec)
{% endhighlight %}

接下来，看看如何修复，假设有 3 个服务，A (主库)、B (备库的异常 XXX:3) 以及 C (备库的异常 YYY:18-19)，那么，接下来可以在不同的服务器上写入空白事务。

{% highlight text %}
# A
- Inject empty trx(XXX:3)
- Inject empty trx(YYY:18)
- Inject empty trx(YYY:19)
# B
- Inject empty trx(YYY:18)
- Inject empty trx(YYY:19)
# C
- Inject empty trx(XXX:3)
{% endhighlight %}

当然，也可以使用 MySQL-Utilities 中的 mysqlslavetrx 脚本写入空白事务。

{% highlight text %}
$ mysqlslavetrx --gtid-set='457e7d57-1da2-11e7-9c71-286ed488dd40:5' --verbose \
    --slaves='root:new-password@127.0.0.1:3308,root:new-password@127.0.0.1:3309'
{% endhighlight %}




## 参考

[MySQL Reference Manual - Replication with Global Transaction Identifiers](https://dev.mysql.com/doc/refman/en/replication-gtids.html) 。

关于 GTID 的介绍可以参考 [MySQL Replication for High Availability - Tutorial](https://severalnines.com/resources/tutorials/mysql-replication-high-availability-tutorial) 中的内容，一篇不错的介绍，也可以直接参考 [本地](/reference/databases/mysql/MySQL_Replication_for_High_Availability.mht) 。

<!--
Database Daily Ops Series: GTID Replication
https://www.percona.com/blog/2016/11/10/database-daily-ops-series-gtid-replication/

Database Daily Ops Series: GTID Replication and Binary Logs Purge
https://www.percona.com/blog/2016/12/01/database-daily-ops-series-gtid-replication-binary-logs-purge/

http://mysqllover.com/?p=594

在使用GTID之前需要考虑的内容
http://www.fromdual.ch/things-you-should-consider-before-using-gtid

[MySQL 5.6] GTID内部实现、运维变化及存在的bug
http://mysqllover.com/?p=594
-->

{% highlight text %}
{% endhighlight %}
