---
title: MySQL 半同步复制
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,log,binlog,错误日志,日志
description: MySQL 中的日志主要包括了：错误日志、查询日志、慢查询日志、事务日志、二进制日志。在此，介绍下一些常见的配置。
---


<!-- more -->

MySQL 提供了原生的异步复制，也就是主库的数据落地之后，并不关心备库的日志是否落库，从而可能导致较多的数据丢失。

从 MySQL5.5 开始引入了一种半同步复制功能，该功能可以确保主服务器和访问链中至少一台从服务器之间的数据一致性和冗余，从而可以减少数据的丢失。

接下来，我们就简单介绍下 MySQL 中的半同步复制。

## 简介

半同步复制时，通常是一台主库并配置多个备库，在这样的复制拓扑中，只有在至少一台从服务器确认更新已经收到并写入了其中继日志 (Relay Log) 之后，主库才完成提交。

当然，如果网络出现故障，导致复制超时，主库会暂时切换到原生的异步复制模式；那么，此时备库也可能会丢失事务。

### 半同步复制配置

semisync 采用 MySQL Plugin 方式实现，可以在主库和备库上分别安装不同的插件，但是一般线上会将主备插件都安装上，谁知道哪天会做个主备切换呢！！！

{% highlight text %}
mysql> INSTALL PLUGIN rpl_semi_sync_slave  SONAME 'semisync_slave.so';
mysql> INSTALL PLUGIN rpl_semi_sync_master SONAME 'semisync_master.so';
{% endhighlight %}

安装完插件后，可以通过如下命令查看半同步复制的参数。

{% highlight text %}
mysql> SHOW GLOBAL VARIABLES LIKE '%semi%';
+-------------------------------------------+------------+
| Variable_name                             | Value      |
+-------------------------------------------+------------+
| rpl_semi_sync_master_enabled              | ON         |
| rpl_semi_sync_master_timeout              | 2000       |
| rpl_semi_sync_master_trace_level          | 32         |
| rpl_semi_sync_master_wait_for_slave_count | 1          |
| rpl_semi_sync_master_wait_no_slave        | ON         |
| rpl_semi_sync_master_wait_point           | AFTER_SYNC |
| rpl_semi_sync_slave_enabled               | ON         |
| rpl_semi_sync_slave_trace_level           | 32         |
+-------------------------------------------+------------+
8 rows in set (0.02 sec)
{% endhighlight %}

使用时，可以根据拓扑结构来定义主库和备库的配置，各个配置项的详细解释如下：

* rpl_semi_sync_master_enabled<br>是否开启主库的 semisync，立刻生效；
* rpl_semi_sync_slave_enabled<br>是否开启备库 semisync，立即生效；
* rpl_semi_sync_master_timeout<br>主库上客户端的等待时间(毫秒)，当超时后，客户端返回，同步复制退化成原生的异步复制；
* rpl_semi_sync_master_wait_no_slave<br>当开启时，当备库起来并跟上主库时，自动切换到同步模式；如果关闭，即使备库跟上主库，也不会启用半同步；
* rpl_semi_sync_master_trace_level/rpl_semi_sync_slave_trace_level<br>输出监控信息的级别，不同的级别输出信息不同，用于调试；

当主库打开半同步复制时，必须至少有一个链接的备库是打开半同步复制的，否则主库每次都会等待到超时；因此，如果想关闭半同步复制必须要先关闭主库配置，再关闭备库配置。

当前的半同步复制状态，可以通过如下命令查看。

{% highlight text %}
mysql> SHOW STATUS LIKE '%semi%';
+--------------------------------------------+-------+
| Variable_name                              | Value |
+--------------------------------------------+-------+
| Rpl_semi_sync_master_clients               | 1     | 半同步复制客户端的个数
| Rpl_semi_sync_master_net_avg_wait_time     | 0     | 平均等待时间，毫秒
| Rpl_semi_sync_master_net_wait_time         | 0     | 总共等待时间
| Rpl_semi_sync_master_net_waits             | 0     | 等待次数
| Rpl_semi_sync_master_no_times              | 1     | 关闭半同步复制的次数
| Rpl_semi_sync_master_no_tx                 | 1     | 没有成功接收slave提交的次数
| Rpl_semi_sync_master_status                | ON    | 异步模式还是半同步模式，ON为半同步
| Rpl_semi_sync_master_timefunc_failures     | 0     | 调用时间函数失败的次数
| Rpl_semi_sync_master_tx_avg_wait_time      | 0     | 事务的平均传输时间
| Rpl_semi_sync_master_tx_wait_time          | 0     | 事务的总共传输时间
| Rpl_semi_sync_master_tx_waits              | 0     | 事物等待次数
| Rpl_semi_sync_master_wait_pos_backtraverse | 0     | 后来的先到了，而先来的还没有到的次数
| Rpl_semi_sync_master_wait_sessions         | 0     | 有多少个session因为slave的回复而造成等待
| Rpl_semi_sync_master_yes_tx                | 0     | 成功接受到slave事务回复的次数
| Rpl_semi_sync_slave_status                 | ON    |
+--------------------------------------------+-------+
15 rows in set (0.00 sec)
{% endhighlight %}

### 5.7 增强

MySQL 5.7 版本修复了 semi sync 的一些 bug 并且增强了功能，主要包括了入下两个。

* 支持发送binlog和接受ack的异步化;
* 支持在事务commit前等待ACK;

<!--
* 在server层判断备库是否要求半同步以减少Plugin锁冲突;
* 解除binlog dump线程和lock_log的冲突等等。
-->

新的异步模式可以提高半同步模式下的系统事务处理能力。

#### binlog发送和接收ack异步

旧版本的 semi sync 受限于主库的 dump thread ，原因是该线程承担了两份不同且又十分频繁的任务：A) 发送 binlog 给备库；B) 等待备库反馈信息；而且这两个任务是串行的，dump thread 必须等待备库返回之后才会传送下一个 events 事务。

大体的实现思路是主库分为了两个线程，也就是原来的 dump 线程，以及 ack reciver 线程。


#### 事务commit前等待ACK

新版本的 semi sync 增加了 ```rpl_semi_sync_master_wait_point``` 参数控制半同步模式下，主库在返回给客户端事务成功的时间点。该参数有两个值：AFTER_SYNC(默认值)、AFTER_COMMIT 。

##### after commit

这也是最初版本的同步方式，主库将每事务写入 binlog，发送给备库并刷新到磁盘 (relaylog)，然后在主库提交事务，并等待备库的响应；一旦接到备库的反馈，主库将结果反馈给客户端。

<!--
   在AFTER_COMMIT模式下,如果slave 没有应用日志,此时master crash，系统failover到slave，app将发现数据出现不一致，在master提交而slave 没有应用。
-->

由于主库是在三段提交的最后 commit 阶段完成后才等待，所以主库的其它会话是可以看到这个事务的，所以这时候可能会导致主备库数据不一致。

##### after sync

主库将事务写入 binlog，发送给备库并刷新到磁盘，主库等待备库的响应；一旦接到备库的反馈，主库会提交事务并且返回结果给客户端。

在该模式下，所有的客户端在同一时刻查看已经提交的数据。假如发生主库 crash，所有在主库上已经提交的事务已经同步到备库并记录到 relay log，切到从库可以减小数据损失。







## 源码实现简介

半同步复制采用 plugin+HOOK 的方式实现，也就是通过 HOOK 回调 plugin 中定义的函数，可以参考如下的示例：

{% highlight cpp %}
// sql/binlog.cc
RUN_HOOK(transaction, after_commit, (head, all));

// sql/rpl_handler.h
#define RUN_HOOK(group, hook, args)             \
  (group ##_delegate->is_empty() ?              \
   0 : group ##_delegate->hook args)

// 因此上例被转化成
transaction_delegate->is_empty() ? 0 : transaction_delegate->after_commit(head, all);
{% endhighlight %}

具体的回调接口实例以及函数在 sql/rpl_hander.cc 文件中定义，主要有如下的五种类型，而其中的 server_state_delegate 变量，只与服务器状态有关，可以忽略。

{% highlight cpp %}
Trans_delegate *transaction_delegate;
Binlog_storage_delegate *binlog_storage_delegate;
Server_state_delegate *server_state_delegate;

#ifdef HAVE_REPLICATION
Binlog_transmit_delegate *binlog_transmit_delegate;
Binlog_relay_IO_delegate *binlog_relay_io_delegate;
#endif /* HAVE_REPLICATION */
{% endhighlight %}

也就是说，主要有四类 XXX_delegate 对象；首先看下主库的初始化过程。

### 主库初始化

在主库半同步复制初始化时，会调用 semi_sync_master_plugin_init() 函数，这三个 obeserver 定义了各自的函数接口，详细如下：

{% highlight cpp %}
Trans_observer trans_observer = {
  sizeof(Trans_observer),           // len
  repl_semi_report_before_dml,      // before_dml
  repl_semi_report_before_commit,   // before_commit
  repl_semi_report_before_rollback, // before_rollback
  repl_semi_report_commit,          // after_commit
  repl_semi_report_rollback,        // after_rollback
};
Binlog_storage_observer storage_observer = {
  sizeof(Binlog_storage_observer),  // len
  repl_semi_report_binlog_update,   // after_flush
  repl_semi_report_binlog_sync,     // after_sync
};
Binlog_transmit_observer transmit_observer = {
  sizeof(Binlog_transmit_observer), // len
  repl_semi_binlog_dump_start,      // transmit_start
  repl_semi_binlog_dump_end,        // transmit_stop
  repl_semi_reserve_header,         // reserve_header
  repl_semi_before_send_event,      // before_send_event
  repl_semi_after_send_event,       // after_send_event
  repl_semi_reset_master,           // after_reset_master
};

static int semi_sync_master_plugin_init(void *p) {
  my_create_thread_local_key(&THR_RPL_SEMI_SYNC_DUMP, NULL);

  if (repl_semisync.initObject())
    return 1;
  if (ack_receiver.init())
    return 1;

  // 为transaction_delegate增加transmit_observer
  if (register_trans_observer(&trans_observer, p))
    return 1;
  // 为binlog_transmit_delegate增加storage_observer
  if (register_binlog_storage_observer(&storage_observer, p))
    return 1;
  // 为binlog_transmit_delegate增加transmit_observer
  if (register_binlog_transmit_observer(&transmit_observer, p))
    return 1;
  return 0;
}

mysql_declare_plugin(semi_sync_master)
{
  MYSQL_REPLICATION_PLUGIN,
  &semi_sync_master_plugin,
  "rpl_semi_sync_master",
  "He Zhenxing",
  "Semi-synchronous replication master",
  PLUGIN_LICENSE_GPL,
  semi_sync_master_plugin_init,    /* Plugin Init */
  semi_sync_master_plugin_deinit,  /* Plugin Deinit */
  0x0100 /* 1.0 */,
  semi_sync_master_status_vars,    /* status variables */
  semi_sync_master_system_vars,    /* system variables */
  NULL,                            /* config options */
  0,                               /* flags */
}
mysql_declare_plugin_end;
{% endhighlight %}

在插件初始化时，会注册各种类型的 observer，然后在 RUN_HOOK() 宏时，就会直接调用上述的函数，可以直接通过类似 ```transmit_start``` 成员变量查看。

所有从 server 层向 plugin 的函数调用，都是通过函数指针来完成的，因此我们只需要搞清楚上述几个函数的调用逻辑，基本就可以清楚 semisync plugin 在 MySQL 里的处理逻辑。

#### 初始化过程

如下是半同步插件主库的执行步骤。

{% highlight text %}
semi_sync_master_plugin_init()            ← 主库插件初始化
 |-ReplSemiSyncMaster::initObject()       ← 一系列系统初始化，如超时时间等
 | |-setWaitTimeout()
 | |-setTraceLevel()
 | |-setWaitSlaveCount()
 |
 |-Ack_receiver::init()                   ← 通过线程处理备库返回的响应
 | |-start()
 |   |-ack_receive_handler()              ← 新建单独线程接收响应，run()函数的包装
 |     |-run()
 |
 |-register_trans_observer()
 |-register_binlog_storage_observer()
 |-register_binlog_transmit_observer()
{% endhighlight %}

### 备库初始化

备库会注册 binlog_relay_io_delegate 对象，其它操作与主库类似，在此就不做过多介绍了。


## 详细操作

接下来，一步步看看半同步复制是如何执行的。

### 主库DUMP

在备库中，通过 ```START SLAVE``` 命令启动后，会向主库发送 DUMP 命令。

{% highlight text %}
dispatch_command()
 |-com_binlog_dump_gtid()           ← COM_BINLOG_DUMP_GTID
 |-com_binlog_dump()                ← COM_BINLOG_DUMP
   |-mysql_binlog_send()            ← 上述的两个命令都会到此函数
     |-Binlog_sender::run()         ← 新建Binlog_sender对象并执行run()函数
       |-init()
       | |-transmit_start()         ← RUN_HOOK()，binlog_transmit_delegate
       | |                          ← 实际调用repl_semi_binlog_dump_start()
       |
       |-###BEGIN while()循环，只要没有错误，线程未被杀死，则一直执行
       |-open_binlog_file()
       |-send_binlog()              ← 发送二进制日志
       | |-send_events()
       |   |-after_send_hook()
       |     |-RUN_HOOK()           ← 调用binlog_transmit->after_send_event()钩子函数
       |-###END
{% endhighlight %}

MySQL 会通过 mysql_binlog_send() 处理每个备库发送的 dump 请求，在开始 dump 之前，会调用如上的 HOOK 函数，而对于半同步复制，实际会调用 repl_semi_binlog_dump_start() 。

{% highlight cpp %}
void Binlog_sender::init()
{
  //... ...
  if (check_start_file())
    DBUG_VOID_RETURN;

  sql_print_information("Start binlog_dump to master_thread_id(%u) "
                        "slave_server(%u), pos(%s, %llu)",
                        thd->thread_id(), thd->server_id,
                        m_start_file, m_start_pos);

  if (RUN_HOOK(binlog_transmit, transmit_start,
               (thd, m_flag, m_start_file, m_start_pos,
                &m_observe_transmission)))
  {
    set_unknow_error("Failed to run hook 'transmit_start'");
    DBUG_VOID_RETURN;
  }
  m_transmit_started=true;
  //... ...
}
{% endhighlight %}

看看半同步复制中的 repl_semi_binlog_dump_start() 函数，是如何处理的。

{% highlight text %}
repl_semi_binlog_dump_start()
 |-get_user_var_int()               ← 获取备库参数rpl_semi_sync_slave
 |-ack_receiver.add_slave()         ← 增加备库计数，通过
 |-repl_semisync.handleAck()
   |-reportReplyBinlog()
{% endhighlight %}

对于 repl_semi_binlog_dump_start() 函数，主要做以下几件事情：

1. 判断连接的备库是否开启半同步复制，也即查看备库是否设置 rpl_semi_sync_slave 变量；
2. 如果备库开启了半同步，则增加连接的备库计数，可查看 rpl_semi_sync_master_clients；
3. 最后会调用 reportReplyBinlog() 函数，该函数在后面详述。


<!--
当备库从主库上断开时会调用 repl_semi_binlog_dump_end() 函数，该函数会将计数器rpl_semi_sync_master_clients减1
-->

### 执行DML

以执行一条简单的 DML 操作为例，例如 ```INSERT INTO t VALUES (1)``` 。

在事务提交时，也就是在 ordered_commit() 函数中，当主库将 binlog 写入到文件中后，且在尚未调用 fsync 之前，会调用如下内容。

{% highlight cpp %}
int MYSQL_BIN_LOG::ordered_commit(THD *thd, bool all, bool skip_commit)
{
  ... ...
  /*
    If the flush finished successfully, we can call the after_flush
    hook. Being invoked here, we have the guarantee that the hook is
    executed before the before/after_send_hooks on the dump thread
    preventing race conditions among these plug-ins.
  */
  if (flush_error == 0)
  {
    const char *file_name_ptr= log_file_name + dirname_length(log_file_name);
    DBUG_ASSERT(flush_end_pos != 0);
    if (RUN_HOOK(binlog_storage, after_flush,
                 (thd, file_name_ptr, flush_end_pos)))
    {
      sql_print_error("Failed to run 'after_flush' hooks");
      flush_error= ER_ERROR_ON_WRITE;
    }

    if (!update_binlog_end_pos_after_sync)
      update_binlog_end_pos();
    DBUG_EXECUTE_IF("crash_commit_after_log", DBUG_SUICIDE(););
  }
  ... ...
}
{% endhighlight %}

在上述源码中的 RUN_HOOK() 宏中，对于半同步复制，实际调用的也就是 ```repl_semi_report_binlog_update()``` 函数；另外，在源码会创建 ```ReplSemiSyncMaster repl_semisync;``` 全局对象。

{% highlight text %}
repl_semi_report_binlog_update()
 |-repl_semisync.getMasterEnabled()         ← 判断是否启动了主库
 |-repl_semisync.writeTranxInBinlog()       ← 保存最大事务的binlog位置
{% endhighlight %}


writeTranxInBinlog() 会存储当前的 binlog 文件名和偏移量，更新当前最大的事务 binlog 位置；

<!--存储在 repl_semisync对象的commit_file_name_和commit_file_pos_中(commit_file_name_inited_被设置为TRUE)；然后将该事务的位点信息存储到active_tranxs中（active_tranxs_->insert_tranx_node(log_file_name, log_file_pos)），这是一个链表，用来存储所有活跃的事务的位点信息，每个新加的节点都能保证位点在已有节点之后；另外还维持了一个key->value的数组，数组下标即为事务binlog坐标计算的hash，值为相同hash值的链表

这些操作都在锁LOCK_binlog_的保护下进行的, 即使semisync退化成同步状态，也会继续更新位点（但不写事务节点），主要是为了监控后续SLAVE时候能够跟上当前的事务Bilog状态；

事实上，有两个变量来控制SEMISYNC是否开启：
state_  为true表示同步，为false表示异步状态，semi sync退化时会修改改变量（ReplSemiSyncMaster::switch_off）

另外一个变量是master_enabled_，这个是由参数rpl_semi_sync_master_enabled来控制的。
-->

### 事务提交后

在事务 commit 之后，也就是在 sql/binlog.cc 文件中，会调用如下的函数，可以从代码中看到，实际会调用 after_commit 钩子函数，也就是 repl_semi_report_commit() 函数。

{% highlight cpp %}
void MYSQL_BIN_LOG::process_after_commit_stage_queue(THD *thd, THD *first)
{
  Thread_excursion excursion(thd);
  for (THD *head= first; head; head= head->next_to_commit)
  {
    if (head->get_transaction()->m_flags.run_hooks &&
        head->commit_error != THD::CE_COMMIT_ERROR)
    {

      /*
        TODO: This hook here should probably move outside/below this
              if and be the only after_commit invocation left in the
              code.
      */
      excursion.try_to_attach_to(head);
      bool all= head->get_transaction()->m_flags.real_commit;
      (void) RUN_HOOK(transaction, after_commit, (head, all));
      /*
        When after_commit finished for the transaction, clear the run_hooks flag.
        This allow other parts of the system to check if after_commit was called.
      */
      head->get_transaction()->m_flags.run_hooks= false;
    }
  }
}
{% endhighlight %}

其中，上述的函数会在 Group Commit 的第三个阶段 (也即 commit 阶段) 执行，也就是通过 leader 线程依次为其他线程调用 repl_semi_report_commit() 函数。

{% highlight text %}
repl_semi_report_commit()
 |-repl_semisync.commitTrx()      ← 入参为binlog文件名+位置
   |-lock()                       ← 获取mutex锁
   |-THD_ENTER_COND()             ← 线程进入新状态
{% endhighlight %}

commitTrx() 是实现客户端同步等待的主要部分，主要做以下事情。

##### 1. 线程进入新状态

用户线程进入新的状态，通过 ```SHOW PROCESSLIST``` 可看到线程状态为 ```"Waiting for semi-sync ACK from slave"``` 。

##### 2. 检查当前线程状态

线程中会执行如下的判断，也就是判断是否已经启用了 semisync 主，而且还在等待 binlog 文件。

{% highlight cpp %}
/* This is the real check inside the mutex. */
if (getMasterEnabled() && trx_wait_binlog_name)
{% endhighlight %}

注意，上述操作是在持有锁的状态下进行的检查。

##### 3. 设置超时时间

会根据 wait_timeout_ 设置超时时间变量，随后进入如下的 while 循环。

{% highlight cpp %}
while (is_on())
{
    ... ...
}
{% endhighlight %}

只要 semisync 没有退化到异步状态，就会一直在 while 循环中等待，直到超时或者获得备库反馈；

<!--
while循环内的工作包括：

(1)判断：
当reply_file_name_inited_为true时，如果reply_file_name_及reply_file_pos_大于当前事务等待的位置，表示备库已经收到了比当前位置更后的事务，这时候无需等待，直接返回；

当wait_file_name_inited_为true时，比较当前事务位置和（wait_file_name_，wait_file_pos_）的大小，如果当期事务更小，则将wait_file_pos_和wait_file_name_设置为当前事务的值；
否则，若wait_file_name_inited_为false，将wait_file_name_inited_设置为TRUE，同样将上述两个变量设置为当前事务的值；
这么做的目的是为了维持当前需要等待的最小binlog位置

(2)增加等待线程计数rpl_semi_sync_master_wait_sessions++
wait_result = cond_timewait(&abstime);  线程进入condition wait
在唤醒或超时后rpl_semi_sync_master_wait_sessions–

如果是等待超时的，rpl_semi_sync_master_wait_timeouts++，并关闭semisync (switch_off()，将state_/wait_file_name_inited_/reply_file_name_inited_设置为false，rpl_semi_sync_master_off_times++，同时唤醒其他等待的线程(COND_binlog_send_))

如果是被唤醒的，则增加计数：rpl_semi_sync_master_trx_wait_num++、rpl_semi_sync_master_trx_wait_time += wait_time， 然后回到1)，去检查相关变量；

4).退出循环后，更新计数

    /* Update the status counter. */
    if (is_on())
      rpl_semi_sync_master_yes_transactions++;
    else
      rpl_semi_sync_master_no_transactions++;



然后返回；

可以看到，上述关键的一步是对（reply_file_name_，reply_file_pos_）的判断，以决定是否需要继续等待；该变量的更新由dump线程来触发
-->

### dump线程的处理

接着，看看在执行一条事务后，dump 线程会有哪些调用逻辑呢？

<!--
1.开始发送binlog之前需要重置packet（reset_transmit_packet）

调用函数repl_semi_reserve_header，用于在Packet的头部预留字节，以维护和备库的交互信息，目前共预留3个字节

这里在packet的头部拷贝两个字节，值为ReplSemiSyncBase::kSyncHeader，固定值，作为MAGIC NUMBER

const unsigned char  ReplSemiSyncBase::kSyncHeader[2] =
  {ReplSemiSyncBase::kPacketMagicNum, 0};

只有备库开启了semisync的情况下，才会去保留额外的packet头部比特位，不管主库是否开启了semisync

2.在发送binlog事件之前调用repl_semi_before_send_event，确认是否需要备库反馈，通过设置之前预留的三个字节的第3个字节

HOOK位置（函数mysql_binlog_send）

1659             if (RUN_HOOK(binlog_transmit, before_send_event,
1660                          (thd, 0/*flags*/, packet, log_file_name, pos)))
1661             {
1662               my_errno= ER_UNKNOWN_ERROR;
1663               errmsg= "run 'before_send_event' hook failed";
1664               GOTO_ERR;
1665             }

repl_semi_before_send_event->

   repl_semisync.updateSyncHeader(packet,
                                        log_file,
                                        log_pos,
                                        param->server_id);

该函数执行以下步骤，目的是填充上一步保留的头部字节：

1）检查主库和备库是否同时打开了semisync，如果没有，直接返回

2）加锁LOCK_binlog_，再次检查主库是否开启semisync

设置sync为false；

3）如果当前semisync是同步状态（即state_为TRUE）

同样的先检查当前的binlog位点是否有其他备库已经接受到（reply_file_name_inited_为true，并且<reply_file_name_, reply_file_pos_>比当前dump线程的位点要大）；则sync为false，goto l_end

如果当前正在等待的事务最小位点（wait_file_name_，wait_file_pos_）比当前dump线程的位点要小（或者wait_file_name_inited_为false,只有当前dump线程），再次检查当前dump线程的bin log位点是否是事务的最后一个事件(通过遍历由函数repl_semisync.writeTranxInBinlog产生的事务节点Hash链表来检查)，如果是的话，则设置sync为true

4)如果当前semisync为异步状态(state_为FALSE)

当commit_file_name_inited_为TRUE时（事务提交位点信息被更新过，在函数repl_semisync.writeTranxInBinlog中），如果dump线程的位点大于等于上次事务提交的位点（commit_file_name_, commit_file_pos_），表示当前dump线程已经追赶上了主库位点，因此sync被设置为TRUE，

当commit_file_name_inited_为false时，表示还没有事务提交位点信息，同样设置sync为TRUE；

5)当sync为TRUE时，设置packet头部，告诉备库需要其提供反馈

  if (sync)
  {
    (packet)[2] = kPacketFlagSync;
  }


plugin/semisync/semisync.cc:

const unsigned char ReplSemiSyncBase::kPacketFlagSync = 0x01;
-->


### 发送事件后

该 HOOK 在 mysql_binlog_send() 函数中调用，也就是 binlog_transmit->after_send_event() 函数，对于半同步复制，实际调用函数 repl_semi_after_send_event() 来读取备库的反馈。

{% highlight cpp %}
int repl_semi_after_send_event(Binlog_transmit_param *param,
                               const char *event_buf, unsigned long len,
                               const char * skipped_log_file,
                               my_off_t skipped_log_pos)
{
  if (is_semi_sync_dump())
  {
    if(skipped_log_pos>0)
      repl_semisync.skipSlaveReply(event_buf, param->server_id,
                                   skipped_log_file, skipped_log_pos);
    else
    {
      THD *thd= current_thd;
      /*
        Possible errors in reading slave reply are ignored deliberately
        because we do not want dump thread to quit on this. Error
        messages are already reported.
      */
      (void) repl_semisync.readSlaveReply(
        thd->get_protocol_classic()->get_net(),
        param->server_id, event_buf);
      thd->clear_error();
    }
  }
  return 0;
}
{% endhighlight %}

如果该事件需要 skip 则调用 skipSlaveReply()，否则调用 readSlaveReply()；前者只判断事件的头部是否设置了需要 sync，如果是的，则调用 ```handleAck->reportReplyBinlog()```；后者则先读取备库传递的数据包，从中读出备库传递的 binlog 坐标信息，函数 readSlaveReply() 主要有如下流程：

1. 如果无需等待，直接返回；
2. 通过 net_flush() 将数据发送到备库，防止数据保存在主的缓存中，然后调用 net_clear()。

到此为止，就已经算将数据发送到了备库，而响应的处理则是在一个单独的线程里处理的。

在 semisync 的主库启动之后，会创建一个 ack_receive_handler() 线程，处理备库的响应报文；实际上会阻塞在 my_net_read() 函数中。

{% highlight text %}
ack_receive_handler()           新建单独线程接收响应，run()函数的包装
 |-run()
   |                            ###BEGIN while循环
   |-select()                   等待备库响应报文
   |-net_clear()
   |-my_net_read()              读取数据
   |-reportReplyPacket()        获取备库返回的文件名以及position
     |-handleAck()
       |-reportReplyBinlog()
         |-getMasterEnabled()   检查是否为主
         |-try_switch_on()      如果是异步，则尝试转换为同步模式
{% endhighlight %}

从 my_net_read() 接收到备库返回的数据后，从数据包中读取备库传递过来的 binlog 位点信息，然后调用 reportReplyBinlog()；该函数的主要调用流程如下：

{% highlight cpp %}
void ReplSemiSyncMaster::reportReplyBinlog(const char *log_file_name,
                                           my_off_t log_file_pos)
{
  const char *kWho = "ReplSemiSyncMaster::reportReplyBinlog";
  int   cmp;
  bool  can_release_threads = false;
  bool  need_copy_send_pos = true;

  function_enter(kWho);
  mysql_mutex_assert_owner(&LOCK_binlog_);

  // 1. 检查主库 semisync 是否打开，如果没有则直接结束；
  if (!getMasterEnabled())
    goto l_end;

  // 2. 如果当前 semisync 为异步状态，尝试将其切换为同步状态；
  if (!is_on())
    try_switch_on(log_file_name, log_file_pos);

  // 3. 将dump线程从备库反馈的位点信息与(reply_file_name_, reply_file_pos_)做对比
  //    如果小于后者，说明已经有别的备库读到更新的事务了，此时无需更新
  //    如上所述，semisync只保证全局至少有一个备库读到更新的事务
  if (reply_file_name_inited_) {
    cmp = ActiveTranx::compare(log_file_name, log_file_pos,
                               reply_file_name_, reply_file_pos_);
    if (cmp < 0) {
      need_copy_send_pos = false;
    }
  }

  // 4. 如果需要，更新位点，清理当前位点之前的事务节点信息
  if (need_copy_send_pos) {
    strncpy(reply_file_name_, log_file_name, sizeof(reply_file_name_) - 1);
    reply_file_name_[sizeof(reply_file_name_) - 1]= '\0';
    reply_file_pos_ = log_file_pos;
    reply_file_name_inited_ = true;

    if (trace_level_ & kTraceDetail)
      sql_print_information("%s: Got reply at (%s, %lu)", kWho,
                            log_file_name, (unsigned long)log_file_pos);
  }

  // 5. 接收到的备库反馈位点信息大于等于当前等待的事务的最小位点，则设置更新
  if (rpl_semi_sync_master_wait_sessions > 0) {
    cmp = ActiveTranx::compare(reply_file_name_, reply_file_pos_,
                               wait_file_name_, wait_file_pos_);
    if (cmp >= 0) {
      can_release_threads = true;
      wait_file_name_inited_ = false;
    }
  }

 l_end:

  // 6. 释放锁，进行一次 broadcast，唤醒等待的用户线程
  if (can_release_threads) {
    if (trace_level_ & kTraceDetail)
      sql_print_information("%s: signal all waiting threads.", kWho);
    active_tranxs_->signal_waiting_sessions_up_to(reply_file_name_, reply_file_pos_);
  }

  function_exit(kWho, 0);
}
{% endhighlight %}

## 备库

当接受到主库发送的 binlog 后，由于开启了 semisync 的备库需要为主库发送响应；与主库类似，备库同样也是为 Binlog_relay_IO_delegate 增加一个 observer 。

{% highlight cpp %}
Binlog_relay_IO_observer relay_io_observer = {
  sizeof(Binlog_relay_IO_observer), // len

  repl_semi_slave_io_start,         // thread_start
  repl_semi_slave_io_end,           // thread_stop
  repl_semi_slave_sql_stop,         // applier_stop (stop sql thread)
  repl_semi_slave_request_dump,     // before_request_transmit
  repl_semi_slave_read_event,       // after_read_event
  repl_semi_slave_queue_event,      // after_queue_event
  repl_semi_reset_slave,            // after_reset_slave
};

static int semi_sync_slave_plugin_init(void *p)
{
  if (repl_semisync.initObject())
    return 1;
  if (register_binlog_relay_io_observer(&relay_io_observer, p))
    return 1;
  return 0;
}
{% endhighlight %}

如上，也就是 relay_io_observer，同样通过 HOOK 方式回调 PLUGIN 函数，主要包括了上述的接口函数。

### 开启IO线程

在执行 start slave 命令时，也就是在 handle_slave_io() 函数中，会调用如下的钩子函数，也就是 relay_io 中的 thread_start() 函数，而实际调用的是 repl_semi_slave_io_start() 。

{% highlight cpp %}
extern "C" void *handle_slave_io(void *arg)
{
 ... ...
  /* This must be called before run any binlog_relay_io hooks */
  my_set_thread_local(RPL_MASTER_INFO, mi);

  if (RUN_HOOK(binlog_relay_io, thread_start, (thd, mi)))
  {
    mi->report(ERROR_LEVEL, ER_SLAVE_FATAL_ERROR,
               ER(ER_SLAVE_FATAL_ERROR), "Failed to run 'thread_start' hook");
    goto err;
  }
  ... ...
}
{% endhighlight %}

在 repl_semi_slave_io_start() 函数中，最终会调用 slaveStart() 函数。

{% highlight cpp %}
int ReplSemiSyncSlave::slaveStart(Binlog_relay_IO_param *param)
{
  bool semi_sync= getSlaveEnabled();

  if (semi_sync && !rpl_semi_sync_slave_status)
    rpl_semi_sync_slave_status= 1;
  return 0;
}
{% endhighlight %}

如上，函数功能很简单，主要是设置全局变量 rpl_semi_sync_slave_status 。

### 发起dump请求

当与主库成功建立连接之后，接下来从库会向主库发起 dump 请求，同样是在 handle_slave_io() 函数中，在调用 request_dump() 函数时，会调用钩子函数 relay_io->thread_start()，而实际调用的是 repl_semi_slave_request_dump() 。

调用的 HOOK 位置为 handle_slave_io->request_dump() ，如下。

{% highlight cpp %}
static int request_dump(THD *thd, MYSQL* mysql, Master_info* mi,
                        bool *suppress_warnings)
{
  ... ...
  if (RUN_HOOK(binlog_relay_io,
               before_request_transmit,
               (thd, mi, binlog_flags)))
    goto err;
  ... ...
}
{% endhighlight %}

在 repl_semi_slave_request_dump() 函数中会检查主库是否支持 semisync，主要检查是否存在 rpl_semi_sync_master_enabled 变量，如果支持则在备库设置用户变量 rpl_semi_sync_slave 。

{% highlight cpp %}
int repl_semi_slave_request_dump(Binlog_relay_IO_param *param,
                 uint32 flags)
{
  ... ...
  if (!repl_semisync.getSlaveEnabled())
    return 0;
  ... ...
  query= "SET @rpl_semi_sync_slave= 1";
  if (mysql_real_query(mysql, query, static_cast<ulong>(strlen(query))))
  {
    sql_print_error("Set 'rpl_semi_sync_slave=1' on master failed");
    return 1;
  }
  mysql_free_result(mysql_store_result(mysql));
  rpl_semi_sync_slave_status= 1;
  return 0;
}
{% endhighlight %}

主库就是通过 rpl_semi_sync_slave 来判断一个 dump 请求的 SLAVE 是否是开启半同步复制。

到此为止，备库就已经初始化成功。


### 读取事件

同样是在 handle_slave_io() 函数中，会调用 relay_io->after_read_event() 钩子函数，而实际上调用的函数是 repl_semi_slave_read_event() 。

{% highlight cpp %}
extern "C" void *handle_slave_io(void *arg)
{
  while (!io_slave_killed(thd,mi))
  {
    ... ...
    while (!io_slave_killed(thd,mi))
    {
      ulong event_len;
      /*
         We say "waiting" because read_event() will wait if there's nothing to
         read. But if there's something to read, it will not wait. The
         important thing is to not confuse users by saying "reading" whereas
         we're in fact receiving nothing.
      */
      THD_STAGE_INFO(thd, stage_waiting_for_master_to_send_event);
      event_len= read_event(mysql, mi, &suppress_warnings);
      ... ...
      if (RUN_HOOK(binlog_relay_io, after_read_event,
                   (thd, mi,(const char*)mysql->net.read_pos + 1,
                    event_len, &event_buf, &event_len)))
      {
        mi->report(ERROR_LEVEL, ER_SLAVE_FATAL_ERROR,
                   ER(ER_SLAVE_FATAL_ERROR),
                   "Failed to run 'after_read_event' hook");
        goto err;
      }
      ... ...
    }
  }
}
{% endhighlight %}


<!--

由于我们在主库上是对packet头部有附加了3个比特的，这里需要将其读出来，同时需要更新event_buf及event_len的值；

如果rpl_semi_sync_slave_status为false，表示开启io线程时未打开semisync，直接使用packet的长度即可，否则调用ReplSemiSyncSlave::slaveReadSyncHeader，去读取packet的头部信息，如果需要给主库一个ack，则设置semi_sync_need_reply为TRUE

,

### 写入relaylog

d.当将binlog写入relaylog之后(即完成函数queue_event之后)，调用repl_semi_slave_queue_event
HOOK位置 (handle_slave_io)

4295       if (RUN_HOOK(binlog_relay_io, after_queue_event,
4296                    (thd, mi, event_buf, event_len, synced)))
4297       {
4298         mi->report(ERROR_LEVEL, ER_SLAVE_FATAL_ERROR,
4299                    ER(ER_SLAVE_FATAL_ERROR),
4300                    "Failed to run 'after_queue_event' hook");
4301         goto err;
4302       }

如果备库开启了semisync且需要ack时（pl_semi_sync_slave_status && semi_sync_need_reply），调用ReplSemiSyncSlave::slaveReply，向主库发送数据包，包括当前的binlog文件名及偏移量信息

### 停止IO线程

e.停止IO线程时，调用函数repl_semi_slave_io_end，将rpl_semi_sync_slave_status设置为false，这里判断的mysql_reply实际上不会用到;

存在的问题主要集中在主库上：

1.锁的粒度太粗，看看能不能细化，或者使用lock-free的算法来优化

2.字符串比较的调用，大部分是对binlog文件名的比较，实际上只要比较后面的那一串数字就足够了；尝试下看看能否有性能优化

-->











## 参考

关于半同步参数的介绍可以参考 [Semisynchronous Replication Administrative Interface](https://dev.mysql.com/doc/refman/en/replication-semisync-interface.html)；以及 [Reference Manual - Semisync](https://dev.mysql.com/doc/refman/5.7/en/replication-semisync.html) 。


<!--
http://blog.itpub.net/15480802/viewspace-1430221
http://www.tuicool.com/articles/ERVNrmy
-->


{% highlight text %}
{% endhighlight %}
