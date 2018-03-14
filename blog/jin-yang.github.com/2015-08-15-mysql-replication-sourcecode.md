---
title: MySQL 複製源碼解析
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,複製,源碼
description: MySQL 的主從複製是通過 binlog 完成的，從庫通過 dump 協議來交互數據的，binlog 複製的基本邏輯處理單元為 event 。在本文中，我們看看源碼是如何執行的。
---

MySQL 的主從複製是通過 binlog 完成的，從庫通過 dump 協議來交互數據的，binlog 複製的基本邏輯處理單元為 event 。

在本文中，我們看看源碼是如何執行的。

<!-- more -->

![gtid source code]({{ site.url }}/images/databases/mysql/replicatioin-sourcecode-logo.jpg "gtid source code"){: .pull-center }

<!--
## 簡介

如下，是與複製相關的代碼文件的簡介。

{% highlight text %}
slave.(h|cc)
    IO/SQL 線程的實現，主要是 slave 的管理邏輯實現，不含從網絡 dump 數據、解析 relay 日誌等底層處理邏輯。
log.(h|cc)
    對event的排序，以及創建、寫入、刪除 binlog 的高級日誌機制，也包含 binlog 的部分回調函數。
log_event.(h|cc)
    包含 log_event 類及其子類，用於所有 event 的創建、寫入、輸出等底層操作，基本是對數據的序列化。
rpl_rli.(h|cc)
    relay_log_info 數據結構的實現，主要用於 SQL 線程，同時也包含部分 SQL 線程的方法，另一部分在 slave.cc 中。
rpl_mi.(h|cc)
    master_info 數據結構的實現，主要用於 IO 線程。
sql_repl.cc
    主的 dump 線程的實現，主要用於將 binlog 發送到備，同時也包含大部分命令的實現。
sql_binlog.cc
    binlog 語句的執行實現。
rpl_record.(h|cc)
    編碼以及解碼 row 模式下的 row event 格式的工具方法。
rpl_failsafe.(h|cc)
    備機初始化以及註冊到主的實現。
rpl_constants.h
    複製事件的定義。
{% endhighlight %}
-->

## 執行命令

配置主備複製時，都是通過各種的 SQL 指令配置的；所以呢，首先看看這些命令的入口，以及命令是如何執行的。

其中，與複製相關的命令主要包括瞭如下幾個：```change master```、```show slave stat```、```show master stat```、```start slave```、```stop slave``` 等命令。

如下是上述命令在源碼中的實際入口，可以根據不同命令入口查看。

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

注意下面的命令 COM_REGISTER_SLAVE 會直接在 dispatch_command() 函數中執行。

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

## 啟動備庫

首先，在會在備庫執行 ```change master``` 函數，將主庫信息寫入到備庫。

{% highlight text %}
mysql_execute_command()
 |-check_global_access()          ← 查看用戶是否有權限
 |-change_master_cmd()            ← 判斷是否創建channel，調用如下函數
   |-change_master()              ← 命令的實際入口函數
{% endhighlight %}

在編譯源碼的時候需要開啟 HAVE_REPLICATION 的宏定義，在備庫上執行 ```start slave``` 命令後，會在 sql_parse.cc 中執行 SQLCOM_SLAVE_START 分支，實際執行的是 start_slave() 函數。

{% highlight text %}
mysql_execute_command()
 |-start_slave_cmd()
   |-start_slave()
     |-start_slave_threads()
       |-start_slave_thread()                ← 先啟動IO線程，無誤再啟動SQL線程
       | |-handle_slave_io()                 ← IO線程處理函數
       |
       |-start_slave_thread()
         |-handle_slave_sql()                ← SQL線程處理函數
{% endhighlight %}

備庫中有兩類處理線程，也就對應了兩個函數 handle_slave_sql() 和 handle_slave_io()，分別用於從主庫讀取數據，以及將主庫數據寫入到備庫。

接下來依次查看這兩類處理線程。

## 連接主庫

首先，查看下 Slave 如何註冊並請求 Master 的 binlog，也就是對應備庫的 IO 線程。其中 Slave IO 線程對應的入口函數為 handle_slave_io()，大致處理流程為：

{% highlight text %}
handle_slave_io()
  |-my_thread_init()                ← 0) 線程初始化
  |-init_slave_thread()
  |-RUN_HOOK()                      ←    調用relay_io->thread_start鉤子函數
  |
  |-safe_connect()                  ← 1) 以標準的連接方式連上master
  |-get_master_version_and_clock()       並獲取主庫的所需信息
  |-get_master_uuid()
  |-io_thread_init_commands()
  |
  |-register_slave_on_master()      ← 2) 把自己註冊到master上去
  | |-net_store_data()              ←    設置數據包
  | |-simple_command()              ←    S把自己的ID、IP、端口、用戶名提交給M，用於註冊
  | |                               ←    **上述會發送COM_REGISTER_SLAVE命令**
  |
  |                                 ###1BEGIN while循環中檢測io_slave_killed()
  |
  |-request_dump()                  ← 3) 開始請求數據，向master請求binlog數據
  | |-RUN_HOOK()                    ←    調用relay_io->before_request_transmit()
  | |-int2store()                   ←    會根據是否為GTID作區分
  | |-simple_command()              ←    發送dump數據請求
  | |                               ←    **執行COM_BINLOG_DUMP_GTID/COM_BINLOG_DUMP命令**
  |
  |                                 ###2BEGIN while循環中檢測io_slave_killed()
  |
  |-read_event()                    ← 4) 讀取event並存放到本地relay log中
  | |-cli_safe_read()               ←    等待主庫將binlog數據發過來
  |   |-my_net_read()
  |-RUN_HOOK()                      ←    調用relay_io->after_read_event()
  |
  |-queue_event()                   ← 5) 將接收到的event保存在relaylog中
  |-RUN_HOOK()                      ←    調用relay_io->after_queue_event()
  |-flush_master_info()
  |
  |                                 ###2END
  |                                 ###1END
{% endhighlight %}

如上所述，當備庫註冊到主庫時，會發送 COM_REGISTER_SLAVE 命令；請求 binlog 日誌時則會根據是否使用 GTID，發送 COM_BINLOG_DUMP_GTID/COM_BINLOG_DUMP 命令。

接下來看下主庫是如何處理備庫的 binlog 請求，這裡相關的命令都會在 dispatch_command() 函數中進行處理，其核心的內容為。

{% highlight c %}
bool dispatch_command(THD *thd, const COM_DATA *com_data,
                      enum enum_server_command command)
{
  ... ...
  switch (command) {
      ... ...
#ifdef HAVE_REPLICATION
      case COM_REGISTER_SLAVE:    // 註冊slave
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

下面以 COM_BINLOG_DUMP 為例介紹 master 是怎麼發送 binlog event 給 slave 的。

{% highlight text %}
dispatch_command()
 |-com_binlog_dump_gtid()           ← COM_BINLOG_DUMP_GTID
 |-com_binlog_dump()                ← COM_BINLOG_DUMP
   |-kill_zombie_dump_threads()     ← 如果同一個備庫註冊，會移除跟該備庫匹配的binlog dump線程
   |-mysql_binlog_send()            ← 上述兩個命令都會執行到此處
     |                              ← 會打開文件，在指定位置讀取文件，將event按照順序發給備庫
     |-Binlog_sender::run()         ← 調用rpl_binlog_sender.cc中的發送
       |-init()
       | |-init_heartbeat_period()  ← 啟動心跳
       | |-transmit_start()         ← RUN_HOOK()，binlog_transmit_delegate
       |
       |-###BEGIN while()循環，只要沒有錯誤，線程未被殺死，則一直執行
       |-open_binlog_file()
       |-send_binlog()              ← 發送二進制日誌
       | |-send_events()
       |   |-after_send_hook()
       |     |-RUN_HOOK()           ← 調用binlog_transmit->after_send_event()鉤子函數
       |
       |-set_last_file()
       |-end_io_cache()
       |-mysql_file_close()
       |-###END
{% endhighlight %}

綜上所述，MySQL 複製需要 Slave 先註冊到 Master，再向 Master 提交 binlog 和 POS，請求發送 binlog；Master 接收到請求後，先做一系列驗證，打開本地 binlog 文件，按照內部 event 的順序，依序發給 slave。

## 備庫應用日誌

我們知道，在主庫中會利用多線程機制併發執行，而複製時，在 5.6 之前只會有一個 SQL 線程，就導致複製延遲成為 MySQL 最為詬病的問題之一。

在 5.6 版本中，MySQL 提供了基於 schema 或者說是數據庫的並行複製功能，不過對於很多單庫的應用場景下，性能提升有限；5.7 開始，提供了基於組提交的並行複製技術，基本可以被認為是真正意義上的並行複製技術。

### 源碼解析

與 MySQL 備庫中提供的多線程併發機制相關的，有兩個函數的入口：handle_slave_worker() 和 handle_slave_sql()；前者為真正的工作函數，後者作為協調器會啟動和分配 worker 線程。

其中，handle_slave_sql() 主要調用了 slave_worker_exec_job_group() ，該函數會利用 C++ 的多態性，調用相應 event 的 do_apply_event() 虛函數，以便將不同的 event 操作在備機上重做一遍。

如下是 handle_slave_sql() 函數的調用邏輯。

{% highlight text %}
handle_slave_sql()                              ← ###作為協調線程
 |-my_thread_init()
 |-init_slave_thread()
 |-slave_start_workers()                          MTS(Multi-Threaded Slave)
 | |-init_hash_workers()
 | |-slave_start_single_worker()
 |   |-Rpl_info_factory::create_worker()
 |   |-###                                      ← 如下操作是在一個線程裡
 |   |-handle_slave_worker()                    ← ###對於複製的並行執行線程
 |     |-my_thread_init()
 |     |-init_slave_thread()
 |     |                                        ← 在while循環中執行
 |     |-slave_worker_exec_job_group()
 |       |-pop_jobs_item()                      ← 獲取具體的event(ev)，會阻塞等待==<<<==
 |       |                                      ← 在while循環中執行
 |       |-is_gtid_event()
 |       |-worker->slave_worker_exec_event(ev)
 |         |-ev->do_apply_event_worker()        ← 調用該函數應用event
 |           |-do_apply_event()                 ← 利用C++多態性執行對應的event
 |
 |-### 如下從IO線程中讀取數據
 |-sql_slave_killed()                           ← 只要線程未kill則一直執行
   |-exec_relay_log_event()
     |-next_event()                             ← 從cache或者relaylog中讀取event
     | |-sql_slave_killed()                     ← 只要線程未kill則一直執行
     | |-Log_event::read_log_event()            ← 讀取記錄，第一參數為IO_CACHE
     |   |-my_b_read()                          ← 從磁盤讀取頭部，並檢查頭部信息是否合法
     |   |-Log_event::read_log_event()          ← 處理讀取到緩存中的數據，第一個參數為char*
     |     | ... ...
     |     |-Write_rows_log_event()             ← 根據不同的event類型，創建ev對象
     |     |-Update_rows_log_event()
     |     |-Delete_rows_log_event()
     |     | ... ...
     |
     |-apply_event_and_update_pos()             ← 執行event並修改當前讀的位置
       |-append_item_to_jobs()                  ← 發送給workers線程==>>>==
{% endhighlight %}

首先，看下 SQL 線程的處理流程，也就是從 read_log_event() 函數中讀取處理；這裡會根據事件的類型來調用相應的構造函數，這裡只關心 ROW 模式的事件處理：

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

如下是 do_apply_event_worker() 函數的代碼，event 是 binlog 的最小單元，所有的 event 的父類是 Log_event(抽象基類)，它定義了一系列虛函數，其中就包括我們這裡調用的函數。

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

對於 do_apply_event() 實現可以查看 sql/log_event.cc 文件內容。

## 事件簡介

在 INSERT、UPDATE、DELETE 事件中，實際執行父類 ```Rows_log_event::do_apply_event()```，接下來，簡單說明下數據是怎麼應用到備庫。

{% highlight text %}
Write_rows_log_event
Update_rows_log_event
Delete_rows_log_event
    如上三個事件都是調用其基類Rows_log_event::do_apply_event()；
{% endhighlight %}

其中 Rows_log_event::do_apply_event() 函數的部分調用代碼邏輯如下。

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

首先，通過 get_table() 調用，從 table map 中獲得對應的 table 信息，然後根據不同的類型通過 do_apply_row_ptr 函數指針指向的函數，將事件對應操作應用到備庫。

如上代碼所示，do_apply_row_ptr 函數指針可能指向以下幾種不同的函數：

{% highlight text %}
do_hash_scan_and_update
do_index_scan_and_update
do_table_scan_and_update
do_apply_row
{% endhighlight %}

對於 insert 操作，不用查找數據，會直接調用 do_apply_row() ，調用邏輯如下。

{% highlight text %}
do_apply_row()
 |-do_exec_row()
   |-write_row()
     |-ha_start_bulk_insert()
{% endhighlight %}

也就是說，它直接把這一行數據交給了存儲引擎，讓存儲引擎把數據給插進去。

<!--
{% highlight text %}
Log_event::print_base64()                        下面的調用只有這一個路徑
Rows_log_event::print_verbose()
Rows_log_event::print_verbose_one_row()
 |-log_event_print_value()
{% endhighlight %}
-->


## 參考

MySQL 中與複製相關的內容，可以參考官方文檔 [MySQL Reference Manual - Replication](http://dev.mysql.com/doc/refman/en/replication.html)。


{% highlight text %}
{% endhighlight %}
