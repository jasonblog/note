---
title: MySQL 复制源码解析
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,复制,源码
description: MySQL 的主从复制是通过 binlog 完成的，从库通过 dump 协议来交互数据的，binlog 复制的基本逻辑处理单元为 event 。在本文中，我们看看源码是如何执行的。
---

MySQL 的主从复制是通过 binlog 完成的，从库通过 dump 协议来交互数据的，binlog 复制的基本逻辑处理单元为 event 。

在本文中，我们看看源码是如何执行的。

<!-- more -->

![gtid source code]({{ site.url }}/images/databases/mysql/replicatioin-sourcecode-logo.jpg "gtid source code"){: .pull-center }

<!--
## 简介

如下，是与复制相关的代码文件的简介。

{% highlight text %}
slave.(h|cc)
    IO/SQL 线程的实现，主要是 slave 的管理逻辑实现，不含从网络 dump 数据、解析 relay 日志等底层处理逻辑。
log.(h|cc)
    对event的排序，以及创建、写入、删除 binlog 的高级日志机制，也包含 binlog 的部分回调函数。
log_event.(h|cc)
    包含 log_event 类及其子类，用于所有 event 的创建、写入、输出等底层操作，基本是对数据的序列化。
rpl_rli.(h|cc)
    relay_log_info 数据结构的实现，主要用于 SQL 线程，同时也包含部分 SQL 线程的方法，另一部分在 slave.cc 中。
rpl_mi.(h|cc)
    master_info 数据结构的实现，主要用于 IO 线程。
sql_repl.cc
    主的 dump 线程的实现，主要用于将 binlog 发送到备，同时也包含大部分命令的实现。
sql_binlog.cc
    binlog 语句的执行实现。
rpl_record.(h|cc)
    编码以及解码 row 模式下的 row event 格式的工具方法。
rpl_failsafe.(h|cc)
    备机初始化以及注册到主的实现。
rpl_constants.h
    复制事件的定义。
{% endhighlight %}
-->

## 执行命令

配置主备复制时，都是通过各种的 SQL 指令配置的；所以呢，首先看看这些命令的入口，以及命令是如何执行的。

其中，与复制相关的命令主要包括了如下几个：```change master```、```show slave stat```、```show master stat```、```start slave```、```stop slave``` 等命令。

如下是上述命令在源码中的实际入口，可以根据不同命令入口查看。

{% highlight cpp %}
int mysql_execute_command(THD *thd, bool first_level)
{
  ... ...
#ifdef HAVE_REPLICATION
  case SQLCOM_CHANGE_MASTER:
  {

    if (check_global_access(thd, SUPER_ACL))
      goto error;
    res= change_master_cmd(thd);
    break;
  }
  case SQLCOM_SHOW_SLAVE_STAT:
  {
    /* Accept one of two privileges */
    if (check_global_access(thd, SUPER_ACL | REPL_CLIENT_ACL))
      goto error;
    res= show_slave_status_cmd(thd);
    break;
  }
  case SQLCOM_SHOW_MASTER_STAT:
  {
    /* Accept one of two privileges */
    if (check_global_access(thd, SUPER_ACL | REPL_CLIENT_ACL))
      goto error;
    res = show_master_status(thd);
    break;
  }

  case SQLCOM_SLAVE_START:
  {
    res= start_slave_cmd(thd);
    break;
  }
  case SQLCOM_SLAVE_STOP:
  {
  /*
    If the client thread has locked tables, a deadlock is possible.
    Assume that
    - the client thread does LOCK TABLE t READ.
    - then the master updates t.
    - then the SQL slave thread wants to update t,
      so it waits for the client thread because t is locked by it.
    - then the client thread does SLAVE STOP.
      SLAVE STOP waits for the SQL slave thread to terminate its
      update t, which waits for the client thread because t is locked by it.
    To prevent that, refuse SLAVE STOP if the
    client thread has locked tables
  */
  if (thd->locked_tables_mode ||
      thd->in_active_multi_stmt_transaction() || thd->global_read_lock.is_acquired())
  {
    my_message(ER_LOCK_OR_ACTIVE_TRANSACTION,
               ER(ER_LOCK_OR_ACTIVE_TRANSACTION), MYF(0));
    goto error;
  }

  res= stop_slave_cmd(thd);
  break;
  }
#endif /* HAVE_REPLICATION */
  ... ...
}
{% endhighlight %}

注意下面的命令 COM_REGISTER_SLAVE 会直接在 dispatch_command() 函数中执行。

{% highlight cpp %}
bool dispatch_command(THD *thd, const COM_DATA *com_data,
                      enum enum_server_command command)
{
  ... ...
  switch (command) {
  ... ...
  case COM_REGISTER_SLAVE:
  {
    // TODO: access of protocol_classic should be removed
    if (!register_slave(thd,
      thd->get_protocol_classic()->get_raw_packet(),
      thd->get_protocol_classic()->get_packet_length()))
      my_ok(thd);
    break;
  }
  ... ...
}
{% endhighlight %}

## 启动备库

首先，在会在备库执行 ```change master``` 函数，将主库信息写入到备库。

{% highlight text %}
mysql_execute_command()
 |-check_global_access()          ← 查看用户是否有权限
 |-change_master_cmd()            ← 判断是否创建channel，调用如下函数
   |-change_master()              ← 命令的实际入口函数
{% endhighlight %}

在编译源码的时候需要开启 HAVE_REPLICATION 的宏定义，在备库上执行 ```start slave``` 命令后，会在 sql_parse.cc 中执行 SQLCOM_SLAVE_START 分支，实际执行的是 start_slave() 函数。

{% highlight text %}
mysql_execute_command()
 |-start_slave_cmd()
   |-start_slave()
     |-start_slave_threads()
       |-start_slave_thread()                ← 先启动IO线程，无误再启动SQL线程
       | |-handle_slave_io()                 ← IO线程处理函数
       |
       |-start_slave_thread()
         |-handle_slave_sql()                ← SQL线程处理函数
{% endhighlight %}

备库中有两类处理线程，也就对应了两个函数 handle_slave_sql() 和 handle_slave_io()，分别用于从主库读取数据，以及将主库数据写入到备库。

接下来依次查看这两类处理线程。

## 连接主库

首先，查看下 Slave 如何注册并请求 Master 的 binlog，也就是对应备库的 IO 线程。其中 Slave IO 线程对应的入口函数为 handle_slave_io()，大致处理流程为：

{% highlight text %}
handle_slave_io()
  |-my_thread_init()                ← 0) 线程初始化
  |-init_slave_thread()
  |-RUN_HOOK()                      ←    调用relay_io->thread_start钩子函数
  |
  |-safe_connect()                  ← 1) 以标准的连接方式连上master
  |-get_master_version_and_clock()       并获取主库的所需信息
  |-get_master_uuid()
  |-io_thread_init_commands()
  |
  |-register_slave_on_master()      ← 2) 把自己注册到master上去
  | |-net_store_data()              ←    设置数据包
  | |-simple_command()              ←    S把自己的ID、IP、端口、用户名提交给M，用于注册
  | |                               ←    **上述会发送COM_REGISTER_SLAVE命令**
  |
  |                                 ###1BEGIN while循环中检测io_slave_killed()
  |
  |-request_dump()                  ← 3) 开始请求数据，向master请求binlog数据
  | |-RUN_HOOK()                    ←    调用relay_io->before_request_transmit()
  | |-int2store()                   ←    会根据是否为GTID作区分
  | |-simple_command()              ←    发送dump数据请求
  | |                               ←    **执行COM_BINLOG_DUMP_GTID/COM_BINLOG_DUMP命令**
  |
  |                                 ###2BEGIN while循环中检测io_slave_killed()
  |
  |-read_event()                    ← 4) 读取event并存放到本地relay log中
  | |-cli_safe_read()               ←    等待主库将binlog数据发过来
  |   |-my_net_read()
  |-RUN_HOOK()                      ←    调用relay_io->after_read_event()
  |
  |-queue_event()                   ← 5) 将接收到的event保存在relaylog中
  |-RUN_HOOK()                      ←    调用relay_io->after_queue_event()
  |-flush_master_info()
  |
  |                                 ###2END
  |                                 ###1END
{% endhighlight %}

如上所述，当备库注册到主库时，会发送 COM_REGISTER_SLAVE 命令；请求 binlog 日志时则会根据是否使用 GTID，发送 COM_BINLOG_DUMP_GTID/COM_BINLOG_DUMP 命令。

接下来看下主库是如何处理备库的 binlog 请求，这里相关的命令都会在 dispatch_command() 函数中进行处理，其核心的内容为。

{% highlight c %}
bool dispatch_command(THD *thd, const COM_DATA *com_data,
                      enum enum_server_command command)
{
  ... ...
  switch (command) {
      ... ...
#ifdef HAVE_REPLICATION
      case COM_REGISTER_SLAVE:    // 注册slave
          if (!register_slave(thd, (uchar*)packet, packet_length))
              my_ok(thd);
          break;
#endif
#ifdef EMBEDDED_LIBRARY
      case COM_BINLOG_DUMP_GTID:
          error= com_binlog_dump_gtid(thd, packet, packet_length);
          break;
      case COM_BINLOG_DUMP:
          error= com_binlog_dump(thd, packet, packet_length);
          break;
#endif
      ... ...
  }
  ... ...
}
{% endhighlight %}

下面以 COM_BINLOG_DUMP 为例介绍 master 是怎么发送 binlog event 给 slave 的。

{% highlight text %}
dispatch_command()
 |-com_binlog_dump_gtid()           ← COM_BINLOG_DUMP_GTID
 |-com_binlog_dump()                ← COM_BINLOG_DUMP
   |-kill_zombie_dump_threads()     ← 如果同一个备库注册，会移除跟该备库匹配的binlog dump线程
   |-mysql_binlog_send()            ← 上述两个命令都会执行到此处
     |                              ← 会打开文件，在指定位置读取文件，将event按照顺序发给备库
     |-Binlog_sender::run()         ← 调用rpl_binlog_sender.cc中的发送
       |-init()
       | |-init_heartbeat_period()  ← 启动心跳
       | |-transmit_start()         ← RUN_HOOK()，binlog_transmit_delegate
       |
       |-###BEGIN while()循环，只要没有错误，线程未被杀死，则一直执行
       |-open_binlog_file()
       |-send_binlog()              ← 发送二进制日志
       | |-send_events()
       |   |-after_send_hook()
       |     |-RUN_HOOK()           ← 调用binlog_transmit->after_send_event()钩子函数
       |
       |-set_last_file()
       |-end_io_cache()
       |-mysql_file_close()
       |-###END
{% endhighlight %}

综上所述，MySQL 复制需要 Slave 先注册到 Master，再向 Master 提交 binlog 和 POS，请求发送 binlog；Master 接收到请求后，先做一系列验证，打开本地 binlog 文件，按照内部 event 的顺序，依序发给 slave。

## 备库应用日志

我们知道，在主库中会利用多线程机制并发执行，而复制时，在 5.6 之前只会有一个 SQL 线程，就导致复制延迟成为 MySQL 最为诟病的问题之一。

在 5.6 版本中，MySQL 提供了基于 schema 或者说是数据库的并行复制功能，不过对于很多单库的应用场景下，性能提升有限；5.7 开始，提供了基于组提交的并行复制技术，基本可以被认为是真正意义上的并行复制技术。

### 源码解析

与 MySQL 备库中提供的多线程并发机制相关的，有两个函数的入口：handle_slave_worker() 和 handle_slave_sql()；前者为真正的工作函数，后者作为协调器会启动和分配 worker 线程。

其中，handle_slave_sql() 主要调用了 slave_worker_exec_job_group() ，该函数会利用 C++ 的多态性，调用相应 event 的 do_apply_event() 虚函数，以便将不同的 event 操作在备机上重做一遍。

如下是 handle_slave_sql() 函数的调用逻辑。

{% highlight text %}
handle_slave_sql()                              ← ###作为协调线程
 |-my_thread_init()
 |-init_slave_thread()
 |-slave_start_workers()                          MTS(Multi-Threaded Slave)
 | |-init_hash_workers()
 | |-slave_start_single_worker()
 |   |-Rpl_info_factory::create_worker()
 |   |-###                                      ← 如下操作是在一个线程里
 |   |-handle_slave_worker()                    ← ###对于复制的并行执行线程
 |     |-my_thread_init()
 |     |-init_slave_thread()
 |     |                                        ← 在while循环中执行
 |     |-slave_worker_exec_job_group()
 |       |-pop_jobs_item()                      ← 获取具体的event(ev)，会阻塞等待==<<<==
 |       |                                      ← 在while循环中执行
 |       |-is_gtid_event()
 |       |-worker->slave_worker_exec_event(ev)
 |         |-ev->do_apply_event_worker()        ← 调用该函数应用event
 |           |-do_apply_event()                 ← 利用C++多态性执行对应的event
 |
 |-### 如下从IO线程中读取数据
 |-sql_slave_killed()                           ← 只要线程未kill则一直执行
   |-exec_relay_log_event()
     |-next_event()                             ← 从cache或者relaylog中读取event
     | |-sql_slave_killed()                     ← 只要线程未kill则一直执行
     | |-Log_event::read_log_event()            ← 读取记录，第一参数为IO_CACHE
     |   |-my_b_read()                          ← 从磁盘读取头部，并检查头部信息是否合法
     |   |-Log_event::read_log_event()          ← 处理读取到缓存中的数据，第一个参数为char*
     |     | ... ...
     |     |-Write_rows_log_event()             ← 根据不同的event类型，创建ev对象
     |     |-Update_rows_log_event()
     |     |-Delete_rows_log_event()
     |     | ... ...
     |
     |-apply_event_and_update_pos()             ← 执行event并修改当前读的位置
       |-append_item_to_jobs()                  ← 发送给workers线程==>>>==
{% endhighlight %}

首先，看下 SQL 线程的处理流程，也就是从 read_log_event() 函数中读取处理；这里会根据事件的类型来调用相应的构造函数，这里只关心 ROW 模式的事件处理：

{% highlight cpp %}
Log_event* Log_event::read_log_event(const char* buf, uint event_len, const char **error,
                                     const Format_description_log_event *description_event,
                                     my_bool crc_check)
{
    ... ...
    switch(event_type) {
    ... ...
#if defined(HAVE_REPLICATION)
    case binary_log::WRITE_ROWS_EVENT:
      ev = new Write_rows_log_event(buf, event_len, description_event);
      break;
    case binary_log::UPDATE_ROWS_EVENT:
      ev = new Update_rows_log_event(buf, event_len, description_event);
      break;
    case binary_log::DELETE_ROWS_EVENT:
      ev = new Delete_rows_log_event(buf, event_len, description_event);
      break;
    case binary_log::TRANSACTION_CONTEXT_EVENT:
      ev = new Transaction_context_log_event(buf, event_len, description_event);
      break;
    case binary_log::VIEW_CHANGE_EVENT:
      ev = new View_change_log_event(buf, event_len, description_event);
      break;
#endif
    ... ...
    };
    ... ...
}
{% endhighlight %}

如下是 do_apply_event_worker() 函数的代码，event 是 binlog 的最小单元，所有的 event 的父类是 Log_event(抽象基类)，它定义了一系列虚函数，其中就包括我们这里调用的函数。

{% highlight cpp %}
inline int Log_event::do_apply_event_worker(Slave_worker *w)
{
  DBUG_EXECUTE_IF("crash_in_a_worker",
                  {
                    /* we will crash a worker after waiting for
                    2 seconds to make sure that other transactions are
                    scheduled and completed */
                    if (w->id == 2)
                    {
                      DBUG_SET("-d,crash_in_a_worker");
                      my_sleep(2000000);
                      DBUG_SUICIDE();
                    }
                  });
  return do_apply_event(w);
}
{% endhighlight %}

对于 do_apply_event() 实现可以查看 sql/log_event.cc 文件内容。

## 事件简介

在 INSERT、UPDATE、DELETE 事件中，实际执行父类 ```Rows_log_event::do_apply_event()```，接下来，简单说明下数据是怎么应用到备库。

{% highlight text %}
Write_rows_log_event
Update_rows_log_event
Delete_rows_log_event
    如上三个事件都是调用其基类Rows_log_event::do_apply_event()；
{% endhighlight %}

其中 Rows_log_event::do_apply_event() 函数的部分调用代码逻辑如下。

{% highlight cpp %}
int Rows_log_event::do_apply_event(Relay_log_info const *rli)
{
  ... ...
  table=
    m_table= const_cast<Relay_log_info*>(rli)->m_table_map.get_table(m_table_id);
  ... ...
  if (table)
  {
    ... ...
    if ((m_rows_lookup_algorithm != ROW_LOOKUP_NOT_NEEDED) &&
        !is_any_column_signaled_for_table(table, &m_cols))
    {
      error= HA_ERR_END_OF_FILE;
      goto AFTER_MAIN_EXEC_ROW_LOOP;
    }
    switch (m_rows_lookup_algorithm)
    {
      case ROW_LOOKUP_HASH_SCAN:
        do_apply_row_ptr= &Rows_log_event::do_hash_scan_and_update;
        break;

      case ROW_LOOKUP_INDEX_SCAN:
        do_apply_row_ptr= &Rows_log_event::do_index_scan_and_update;
        break;

      case ROW_LOOKUP_TABLE_SCAN:
        do_apply_row_ptr= &Rows_log_event::do_table_scan_and_update;
        break;

      case ROW_LOOKUP_NOT_NEEDED:
        DBUG_ASSERT(get_general_type_code() == binary_log::WRITE_ROWS_EVENT);

        /* No need to scan for rows, just apply it */
        do_apply_row_ptr= &Rows_log_event::do_apply_row;
        break;

      default:
        DBUG_ASSERT(0);
        error= 1;
        goto AFTER_MAIN_EXEC_ROW_LOOP;
        break;
    }

    do {

      error= (this->*do_apply_row_ptr)(rli);

      if (handle_idempotent_and_ignored_errors(rli, &error))
        break;

      /* this advances m_curr_row */
      do_post_row_operations(rli, error);

    } while (!error && (m_curr_row != m_rows_end));
    ... ...
  }
  ... ...
}
{% endhighlight %}

首先，通过 get_table() 调用，从 table map 中获得对应的 table 信息，然后根据不同的类型通过 do_apply_row_ptr 函数指针指向的函数，将事件对应操作应用到备库。

如上代码所示，do_apply_row_ptr 函数指针可能指向以下几种不同的函数：

{% highlight text %}
do_hash_scan_and_update
do_index_scan_and_update
do_table_scan_and_update
do_apply_row
{% endhighlight %}

对于 insert 操作，不用查找数据，会直接调用 do_apply_row() ，调用逻辑如下。

{% highlight text %}
do_apply_row()
 |-do_exec_row()
   |-write_row()
     |-ha_start_bulk_insert()
{% endhighlight %}

也就是说，它直接把这一行数据交给了存储引擎，让存储引擎把数据给插进去。

<!--
{% highlight text %}
Log_event::print_base64()                        下面的调用只有这一个路径
Rows_log_event::print_verbose()
Rows_log_event::print_verbose_one_row()
 |-log_event_print_value()
{% endhighlight %}
-->


## 参考

MySQL 中与复制相关的内容，可以参考官方文档 [MySQL Reference Manual - Replication](http://dev.mysql.com/doc/refman/en/replication.html)。


{% highlight text %}
{% endhighlight %}
