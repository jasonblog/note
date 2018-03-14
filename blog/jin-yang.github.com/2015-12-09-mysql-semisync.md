---
title: MySQL 半同步複製
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,log,binlog,錯誤日誌,日誌
description: MySQL 中的日誌主要包括了：錯誤日誌、查詢日誌、慢查詢日誌、事務日誌、二進制日誌。在此，介紹下一些常見的配置。
---


<!-- more -->

MySQL 提供了原生的異步複製，也就是主庫的數據落地之後，並不關心備庫的日誌是否落庫，從而可能導致較多的數據丟失。

從 MySQL5.5 開始引入了一種半同步複製功能，該功能可以確保主服務器和訪問鏈中至少一臺從服務器之間的數據一致性和冗餘，從而可以減少數據的丟失。

接下來，我們就簡單介紹下 MySQL 中的半同步複製。

## 簡介

半同步複製時，通常是一臺主庫並配置多個備庫，在這樣的複製拓撲中，只有在至少一臺從服務器確認更新已經收到並寫入了其中繼日誌 (Relay Log) 之後，主庫才完成提交。

當然，如果網絡出現故障，導致複製超時，主庫會暫時切換到原生的異步複製模式；那麼，此時備庫也可能會丟失事務。

### 半同步複製配置

semisync 採用 MySQL Plugin 方式實現，可以在主庫和備庫上分別安裝不同的插件，但是一般線上會將主備插件都安裝上，誰知道哪天會做個主備切換呢！！！

{% highlight text %}
mysql> INSTALL PLUGIN rpl_semi_sync_slave  SONAME 'semisync_slave.so';
mysql> INSTALL PLUGIN rpl_semi_sync_master SONAME 'semisync_master.so';
{% endhighlight %}

安裝完插件後，可以通過如下命令查看半同步複製的參數。

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

使用時，可以根據拓撲結構來定義主庫和備庫的配置，各個配置項的詳細解釋如下：

* rpl_semi_sync_master_enabled<br>是否開啟主庫的 semisync，立刻生效；
* rpl_semi_sync_slave_enabled<br>是否開啟備庫 semisync，立即生效；
* rpl_semi_sync_master_timeout<br>主庫上客戶端的等待時間(毫秒)，當超時後，客戶端返回，同步複製退化成原生的異步複製；
* rpl_semi_sync_master_wait_no_slave<br>當開啟時，當備庫起來並跟上主庫時，自動切換到同步模式；如果關閉，即使備庫跟上主庫，也不會啟用半同步；
* rpl_semi_sync_master_trace_level/rpl_semi_sync_slave_trace_level<br>輸出監控信息的級別，不同的級別輸出信息不同，用於調試；

當主庫打開半同步複製時，必須至少有一個鏈接的備庫是打開半同步複製的，否則主庫每次都會等待到超時；因此，如果想關閉半同步複製必須要先關閉主庫配置，再關閉備庫配置。

當前的半同步複製狀態，可以通過如下命令查看。

{% highlight text %}
mysql> SHOW STATUS LIKE '%semi%';
+--------------------------------------------+-------+
| Variable_name                              | Value |
+--------------------------------------------+-------+
| Rpl_semi_sync_master_clients               | 1     | 半同步複製客戶端的個數
| Rpl_semi_sync_master_net_avg_wait_time     | 0     | 平均等待時間，毫秒
| Rpl_semi_sync_master_net_wait_time         | 0     | 總共等待時間
| Rpl_semi_sync_master_net_waits             | 0     | 等待次數
| Rpl_semi_sync_master_no_times              | 1     | 關閉半同步複製的次數
| Rpl_semi_sync_master_no_tx                 | 1     | 沒有成功接收slave提交的次數
| Rpl_semi_sync_master_status                | ON    | 異步模式還是半同步模式，ON為半同步
| Rpl_semi_sync_master_timefunc_failures     | 0     | 調用時間函數失敗的次數
| Rpl_semi_sync_master_tx_avg_wait_time      | 0     | 事務的平均傳輸時間
| Rpl_semi_sync_master_tx_wait_time          | 0     | 事務的總共傳輸時間
| Rpl_semi_sync_master_tx_waits              | 0     | 事物等待次數
| Rpl_semi_sync_master_wait_pos_backtraverse | 0     | 後來的先到了，而先來的還沒有到的次數
| Rpl_semi_sync_master_wait_sessions         | 0     | 有多少個session因為slave的回覆而造成等待
| Rpl_semi_sync_master_yes_tx                | 0     | 成功接受到slave事務回覆的次數
| Rpl_semi_sync_slave_status                 | ON    |
+--------------------------------------------+-------+
15 rows in set (0.00 sec)
{% endhighlight %}

### 5.7 增強

MySQL 5.7 版本修復了 semi sync 的一些 bug 並且增強了功能，主要包括了入下兩個。

* 支持發送binlog和接受ack的異步化;
* 支持在事務commit前等待ACK;

<!--
* 在server層判斷備庫是否要求半同步以減少Plugin鎖衝突;
* 解除binlog dump線程和lock_log的衝突等等。
-->

新的異步模式可以提高半同步模式下的系統事務處理能力。

#### binlog發送和接收ack異步

舊版本的 semi sync 受限於主庫的 dump thread ，原因是該線程承擔了兩份不同且又十分頻繁的任務：A) 發送 binlog 給備庫；B) 等待備庫反饋信息；而且這兩個任務是串行的，dump thread 必須等待備庫返回之後才會傳送下一個 events 事務。

大體的實現思路是主庫分為了兩個線程，也就是原來的 dump 線程，以及 ack reciver 線程。


#### 事務commit前等待ACK

新版本的 semi sync 增加了 ```rpl_semi_sync_master_wait_point``` 參數控制半同步模式下，主庫在返回給客戶端事務成功的時間點。該參數有兩個值：AFTER_SYNC(默認值)、AFTER_COMMIT 。

##### after commit

這也是最初版本的同步方式，主庫將每事務寫入 binlog，發送給備庫並刷新到磁盤 (relaylog)，然後在主庫提交事務，並等待備庫的響應；一旦接到備庫的反饋，主庫將結果反饋給客戶端。

<!--
   在AFTER_COMMIT模式下,如果slave 沒有應用日誌,此時master crash，系統failover到slave，app將發現數據出現不一致，在master提交而slave 沒有應用。
-->

由於主庫是在三段提交的最後 commit 階段完成後才等待，所以主庫的其它會話是可以看到這個事務的，所以這時候可能會導致主備庫數據不一致。

##### after sync

主庫將事務寫入 binlog，發送給備庫並刷新到磁盤，主庫等待備庫的響應；一旦接到備庫的反饋，主庫會提交事務並且返回結果給客戶端。

在該模式下，所有的客戶端在同一時刻查看已經提交的數據。假如發生主庫 crash，所有在主庫上已經提交的事務已經同步到備庫並記錄到 relay log，切到從庫可以減小數據損失。







## 源碼實現簡介

半同步複製採用 plugin+HOOK 的方式實現，也就是通過 HOOK 回調 plugin 中定義的函數，可以參考如下的示例：

{% highlight cpp %}
// sql/binlog.cc
RUN_HOOK(transaction, after_commit, (head, all));

// sql/rpl_handler.h
#define RUN_HOOK(group, hook, args)             \
  (group ##_delegate->is_empty() ?              \
   0 : group ##_delegate->hook args)

// 因此上例被轉化成
transaction_delegate->is_empty() ? 0 : transaction_delegate->after_commit(head, all);
{% endhighlight %}

具體的回調接口實例以及函數在 sql/rpl_hander.cc 文件中定義，主要有如下的五種類型，而其中的 server_state_delegate 變量，只與服務器狀態有關，可以忽略。

{% highlight cpp %}
Trans_delegate *transaction_delegate;
Binlog_storage_delegate *binlog_storage_delegate;
Server_state_delegate *server_state_delegate;

#ifdef HAVE_REPLICATION
Binlog_transmit_delegate *binlog_transmit_delegate;
Binlog_relay_IO_delegate *binlog_relay_io_delegate;
#endif /* HAVE_REPLICATION */
{% endhighlight %}

也就是說，主要有四類 XXX_delegate 對象；首先看下主庫的初始化過程。

### 主庫初始化

在主庫半同步複製初始化時，會調用 semi_sync_master_plugin_init() 函數，這三個 obeserver 定義了各自的函數接口，詳細如下：

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

  // 為transaction_delegate增加transmit_observer
  if (register_trans_observer(&trans_observer, p))
    return 1;
  // 為binlog_transmit_delegate增加storage_observer
  if (register_binlog_storage_observer(&storage_observer, p))
    return 1;
  // 為binlog_transmit_delegate增加transmit_observer
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

在插件初始化時，會註冊各種類型的 observer，然後在 RUN_HOOK() 宏時，就會直接調用上述的函數，可以直接通過類似 ```transmit_start``` 成員變量查看。

所有從 server 層向 plugin 的函數調用，都是通過函數指針來完成的，因此我們只需要搞清楚上述幾個函數的調用邏輯，基本就可以清楚 semisync plugin 在 MySQL 裡的處理邏輯。

#### 初始化過程

如下是半同步插件主庫的執行步驟。

{% highlight text %}
semi_sync_master_plugin_init()            ← 主庫插件初始化
 |-ReplSemiSyncMaster::initObject()       ← 一系列系統初始化，如超時時間等
 | |-setWaitTimeout()
 | |-setTraceLevel()
 | |-setWaitSlaveCount()
 |
 |-Ack_receiver::init()                   ← 通過線程處理備庫返回的響應
 | |-start()
 |   |-ack_receive_handler()              ← 新建單獨線程接收響應，run()函數的包裝
 |     |-run()
 |
 |-register_trans_observer()
 |-register_binlog_storage_observer()
 |-register_binlog_transmit_observer()
{% endhighlight %}

### 備庫初始化

備庫會註冊 binlog_relay_io_delegate 對象，其它操作與主庫類似，在此就不做過多介紹了。


## 詳細操作

接下來，一步步看看半同步複製是如何執行的。

### 主庫DUMP

在備庫中，通過 ```START SLAVE``` 命令啟動後，會向主庫發送 DUMP 命令。

{% highlight text %}
dispatch_command()
 |-com_binlog_dump_gtid()           ← COM_BINLOG_DUMP_GTID
 |-com_binlog_dump()                ← COM_BINLOG_DUMP
   |-mysql_binlog_send()            ← 上述的兩個命令都會到此函數
     |-Binlog_sender::run()         ← 新建Binlog_sender對象並執行run()函數
       |-init()
       | |-transmit_start()         ← RUN_HOOK()，binlog_transmit_delegate
       | |                          ← 實際調用repl_semi_binlog_dump_start()
       |
       |-###BEGIN while()循環，只要沒有錯誤，線程未被殺死，則一直執行
       |-open_binlog_file()
       |-send_binlog()              ← 發送二進制日誌
       | |-send_events()
       |   |-after_send_hook()
       |     |-RUN_HOOK()           ← 調用binlog_transmit->after_send_event()鉤子函數
       |-###END
{% endhighlight %}

MySQL 會通過 mysql_binlog_send() 處理每個備庫發送的 dump 請求，在開始 dump 之前，會調用如上的 HOOK 函數，而對於半同步複製，實際會調用 repl_semi_binlog_dump_start() 。

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

看看半同步複製中的 repl_semi_binlog_dump_start() 函數，是如何處理的。

{% highlight text %}
repl_semi_binlog_dump_start()
 |-get_user_var_int()               ← 獲取備庫參數rpl_semi_sync_slave
 |-ack_receiver.add_slave()         ← 增加備庫計數，通過
 |-repl_semisync.handleAck()
   |-reportReplyBinlog()
{% endhighlight %}

對於 repl_semi_binlog_dump_start() 函數，主要做以下幾件事情：

1. 判斷連接的備庫是否開啟半同步複製，也即查看備庫是否設置 rpl_semi_sync_slave 變量；
2. 如果備庫開啟了半同步，則增加連接的備庫計數，可查看 rpl_semi_sync_master_clients；
3. 最後會調用 reportReplyBinlog() 函數，該函數在後面詳述。


<!--
當備庫從主庫上斷開時會調用 repl_semi_binlog_dump_end() 函數，該函數會將計數器rpl_semi_sync_master_clients減1
-->

### 執行DML

以執行一條簡單的 DML 操作為例，例如 ```INSERT INTO t VALUES (1)``` 。

在事務提交時，也就是在 ordered_commit() 函數中，當主庫將 binlog 寫入到文件中後，且在尚未調用 fsync 之前，會調用如下內容。

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

在上述源碼中的 RUN_HOOK() 宏中，對於半同步複製，實際調用的也就是 ```repl_semi_report_binlog_update()``` 函數；另外，在源碼會創建 ```ReplSemiSyncMaster repl_semisync;``` 全局對象。

{% highlight text %}
repl_semi_report_binlog_update()
 |-repl_semisync.getMasterEnabled()         ← 判斷是否啟動了主庫
 |-repl_semisync.writeTranxInBinlog()       ← 保存最大事務的binlog位置
{% endhighlight %}


writeTranxInBinlog() 會存儲當前的 binlog 文件名和偏移量，更新當前最大的事務 binlog 位置；

<!--存儲在 repl_semisync對象的commit_file_name_和commit_file_pos_中(commit_file_name_inited_被設置為TRUE)；然後將該事務的位點信息存儲到active_tranxs中（active_tranxs_->insert_tranx_node(log_file_name, log_file_pos)），這是一個鏈表，用來存儲所有活躍的事務的位點信息，每個新加的節點都能保證位點在已有節點之後；另外還維持了一個key->value的數組，數組下標即為事務binlog座標計算的hash，值為相同hash值的鏈表

這些操作都在鎖LOCK_binlog_的保護下進行的, 即使semisync退化成同步狀態，也會繼續更新位點（但不寫事務節點），主要是為了監控後續SLAVE時候能夠跟上當前的事務Bilog狀態；

事實上，有兩個變量來控制SEMISYNC是否開啟：
state_  為true表示同步，為false表示異步狀態，semi sync退化時會修改改變量（ReplSemiSyncMaster::switch_off）

另外一個變量是master_enabled_，這個是由參數rpl_semi_sync_master_enabled來控制的。
-->

### 事務提交後

在事務 commit 之後，也就是在 sql/binlog.cc 文件中，會調用如下的函數，可以從代碼中看到，實際會調用 after_commit 鉤子函數，也就是 repl_semi_report_commit() 函數。

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

其中，上述的函數會在 Group Commit 的第三個階段 (也即 commit 階段) 執行，也就是通過 leader 線程依次為其他線程調用 repl_semi_report_commit() 函數。

{% highlight text %}
repl_semi_report_commit()
 |-repl_semisync.commitTrx()      ← 入參為binlog文件名+位置
   |-lock()                       ← 獲取mutex鎖
   |-THD_ENTER_COND()             ← 線程進入新狀態
{% endhighlight %}

commitTrx() 是實現客戶端同步等待的主要部分，主要做以下事情。

##### 1. 線程進入新狀態

用戶線程進入新的狀態，通過 ```SHOW PROCESSLIST``` 可看到線程狀態為 ```"Waiting for semi-sync ACK from slave"``` 。

##### 2. 檢查當前線程狀態

線程中會執行如下的判斷，也就是判斷是否已經啟用了 semisync 主，而且還在等待 binlog 文件。

{% highlight cpp %}
/* This is the real check inside the mutex. */
if (getMasterEnabled() && trx_wait_binlog_name)
{% endhighlight %}

注意，上述操作是在持有鎖的狀態下進行的檢查。

##### 3. 設置超時時間

會根據 wait_timeout_ 設置超時時間變量，隨後進入如下的 while 循環。

{% highlight cpp %}
while (is_on())
{
    ... ...
}
{% endhighlight %}

只要 semisync 沒有退化到異步狀態，就會一直在 while 循環中等待，直到超時或者獲得備庫反饋；

<!--
while循環內的工作包括：

(1)判斷：
當reply_file_name_inited_為true時，如果reply_file_name_及reply_file_pos_大於當前事務等待的位置，表示備庫已經收到了比當前位置更後的事務，這時候無需等待，直接返回；

當wait_file_name_inited_為true時，比較當前事務位置和（wait_file_name_，wait_file_pos_）的大小，如果當期事務更小，則將wait_file_pos_和wait_file_name_設置為當前事務的值；
否則，若wait_file_name_inited_為false，將wait_file_name_inited_設置為TRUE，同樣將上述兩個變量設置為當前事務的值；
這麼做的目的是為了維持當前需要等待的最小binlog位置

(2)增加等待線程計數rpl_semi_sync_master_wait_sessions++
wait_result = cond_timewait(&abstime);  線程進入condition wait
在喚醒或超時後rpl_semi_sync_master_wait_sessions–

如果是等待超時的，rpl_semi_sync_master_wait_timeouts++，並關閉semisync (switch_off()，將state_/wait_file_name_inited_/reply_file_name_inited_設置為false，rpl_semi_sync_master_off_times++，同時喚醒其他等待的線程(COND_binlog_send_))

如果是被喚醒的，則增加計數：rpl_semi_sync_master_trx_wait_num++、rpl_semi_sync_master_trx_wait_time += wait_time， 然後回到1)，去檢查相關變量；

4).退出循環後，更新計數

    /* Update the status counter. */
    if (is_on())
      rpl_semi_sync_master_yes_transactions++;
    else
      rpl_semi_sync_master_no_transactions++;



然後返回；

可以看到，上述關鍵的一步是對（reply_file_name_，reply_file_pos_）的判斷，以決定是否需要繼續等待；該變量的更新由dump線程來觸發
-->

### dump線程的處理

接著，看看在執行一條事務後，dump 線程會有哪些調用邏輯呢？

<!--
1.開始發送binlog之前需要重置packet（reset_transmit_packet）

調用函數repl_semi_reserve_header，用於在Packet的頭部預留字節，以維護和備庫的交互信息，目前共預留3個字節

這裡在packet的頭部拷貝兩個字節，值為ReplSemiSyncBase::kSyncHeader，固定值，作為MAGIC NUMBER

const unsigned char  ReplSemiSyncBase::kSyncHeader[2] =
  {ReplSemiSyncBase::kPacketMagicNum, 0};

只有備庫開啟了semisync的情況下，才會去保留額外的packet頭部比特位，不管主庫是否開啟了semisync

2.在發送binlog事件之前調用repl_semi_before_send_event，確認是否需要備庫反饋，通過設置之前預留的三個字節的第3個字節

HOOK位置（函數mysql_binlog_send）

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

該函數執行以下步驟，目的是填充上一步保留的頭部字節：

1）檢查主庫和備庫是否同時打開了semisync，如果沒有，直接返回

2）加鎖LOCK_binlog_，再次檢查主庫是否開啟semisync

設置sync為false；

3）如果當前semisync是同步狀態（即state_為TRUE）

同樣的先檢查當前的binlog位點是否有其他備庫已經接受到（reply_file_name_inited_為true，並且<reply_file_name_, reply_file_pos_>比當前dump線程的位點要大）；則sync為false，goto l_end

如果當前正在等待的事務最小位點（wait_file_name_，wait_file_pos_）比當前dump線程的位點要小（或者wait_file_name_inited_為false,只有當前dump線程），再次檢查當前dump線程的bin log位點是否是事務的最後一個事件(通過遍歷由函數repl_semisync.writeTranxInBinlog產生的事務節點Hash鏈表來檢查)，如果是的話，則設置sync為true

4)如果當前semisync為異步狀態(state_為FALSE)

當commit_file_name_inited_為TRUE時（事務提交位點信息被更新過，在函數repl_semisync.writeTranxInBinlog中），如果dump線程的位點大於等於上次事務提交的位點（commit_file_name_, commit_file_pos_），表示當前dump線程已經追趕上了主庫位點，因此sync被設置為TRUE，

當commit_file_name_inited_為false時，表示還沒有事務提交位點信息，同樣設置sync為TRUE；

5)當sync為TRUE時，設置packet頭部，告訴備庫需要其提供反饋

  if (sync)
  {
    (packet)[2] = kPacketFlagSync;
  }


plugin/semisync/semisync.cc:

const unsigned char ReplSemiSyncBase::kPacketFlagSync = 0x01;
-->


### 發送事件後

該 HOOK 在 mysql_binlog_send() 函數中調用，也就是 binlog_transmit->after_send_event() 函數，對於半同步複製，實際調用函數 repl_semi_after_send_event() 來讀取備庫的反饋。

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

如果該事件需要 skip 則調用 skipSlaveReply()，否則調用 readSlaveReply()；前者只判斷事件的頭部是否設置了需要 sync，如果是的，則調用 ```handleAck->reportReplyBinlog()```；後者則先讀取備庫傳遞的數據包，從中讀出備庫傳遞的 binlog 座標信息，函數 readSlaveReply() 主要有如下流程：

1. 如果無需等待，直接返回；
2. 通過 net_flush() 將數據發送到備庫，防止數據保存在主的緩存中，然後調用 net_clear()。

到此為止，就已經算將數據發送到了備庫，而響應的處理則是在一個單獨的線程裡處理的。

在 semisync 的主庫啟動之後，會創建一個 ack_receive_handler() 線程，處理備庫的響應報文；實際上會阻塞在 my_net_read() 函數中。

{% highlight text %}
ack_receive_handler()           新建單獨線程接收響應，run()函數的包裝
 |-run()
   |                            ###BEGIN while循環
   |-select()                   等待備庫響應報文
   |-net_clear()
   |-my_net_read()              讀取數據
   |-reportReplyPacket()        獲取備庫返回的文件名以及position
     |-handleAck()
       |-reportReplyBinlog()
         |-getMasterEnabled()   檢查是否為主
         |-try_switch_on()      如果是異步，則嘗試轉換為同步模式
{% endhighlight %}

從 my_net_read() 接收到備庫返回的數據後，從數據包中讀取備庫傳遞過來的 binlog 位點信息，然後調用 reportReplyBinlog()；該函數的主要調用流程如下：

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

  // 1. 檢查主庫 semisync 是否打開，如果沒有則直接結束；
  if (!getMasterEnabled())
    goto l_end;

  // 2. 如果當前 semisync 為異步狀態，嘗試將其切換為同步狀態；
  if (!is_on())
    try_switch_on(log_file_name, log_file_pos);

  // 3. 將dump線程從備庫反饋的位點信息與(reply_file_name_, reply_file_pos_)做對比
  //    如果小於後者，說明已經有別的備庫讀到更新的事務了，此時無需更新
  //    如上所述，semisync只保證全局至少有一個備庫讀到更新的事務
  if (reply_file_name_inited_) {
    cmp = ActiveTranx::compare(log_file_name, log_file_pos,
                               reply_file_name_, reply_file_pos_);
    if (cmp < 0) {
      need_copy_send_pos = false;
    }
  }

  // 4. 如果需要，更新位點，清理當前位點之前的事務節點信息
  if (need_copy_send_pos) {
    strncpy(reply_file_name_, log_file_name, sizeof(reply_file_name_) - 1);
    reply_file_name_[sizeof(reply_file_name_) - 1]= '\0';
    reply_file_pos_ = log_file_pos;
    reply_file_name_inited_ = true;

    if (trace_level_ & kTraceDetail)
      sql_print_information("%s: Got reply at (%s, %lu)", kWho,
                            log_file_name, (unsigned long)log_file_pos);
  }

  // 5. 接收到的備庫反饋位點信息大於等於當前等待的事務的最小位點，則設置更新
  if (rpl_semi_sync_master_wait_sessions > 0) {
    cmp = ActiveTranx::compare(reply_file_name_, reply_file_pos_,
                               wait_file_name_, wait_file_pos_);
    if (cmp >= 0) {
      can_release_threads = true;
      wait_file_name_inited_ = false;
    }
  }

 l_end:

  // 6. 釋放鎖，進行一次 broadcast，喚醒等待的用戶線程
  if (can_release_threads) {
    if (trace_level_ & kTraceDetail)
      sql_print_information("%s: signal all waiting threads.", kWho);
    active_tranxs_->signal_waiting_sessions_up_to(reply_file_name_, reply_file_pos_);
  }

  function_exit(kWho, 0);
}
{% endhighlight %}

## 備庫

當接受到主庫發送的 binlog 後，由於開啟了 semisync 的備庫需要為主庫發送響應；與主庫類似，備庫同樣也是為 Binlog_relay_IO_delegate 增加一個 observer 。

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

如上，也就是 relay_io_observer，同樣通過 HOOK 方式回調 PLUGIN 函數，主要包括了上述的接口函數。

### 開啟IO線程

在執行 start slave 命令時，也就是在 handle_slave_io() 函數中，會調用如下的鉤子函數，也就是 relay_io 中的 thread_start() 函數，而實際調用的是 repl_semi_slave_io_start() 。

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

在 repl_semi_slave_io_start() 函數中，最終會調用 slaveStart() 函數。

{% highlight cpp %}
int ReplSemiSyncSlave::slaveStart(Binlog_relay_IO_param *param)
{
  bool semi_sync= getSlaveEnabled();

  if (semi_sync && !rpl_semi_sync_slave_status)
    rpl_semi_sync_slave_status= 1;
  return 0;
}
{% endhighlight %}

如上，函數功能很簡單，主要是設置全局變量 rpl_semi_sync_slave_status 。

### 發起dump請求

當與主庫成功建立連接之後，接下來從庫會向主庫發起 dump 請求，同樣是在 handle_slave_io() 函數中，在調用 request_dump() 函數時，會調用鉤子函數 relay_io->thread_start()，而實際調用的是 repl_semi_slave_request_dump() 。

調用的 HOOK 位置為 handle_slave_io->request_dump() ，如下。

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

在 repl_semi_slave_request_dump() 函數中會檢查主庫是否支持 semisync，主要檢查是否存在 rpl_semi_sync_master_enabled 變量，如果支持則在備庫設置用戶變量 rpl_semi_sync_slave 。

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

主庫就是通過 rpl_semi_sync_slave 來判斷一個 dump 請求的 SLAVE 是否是開啟半同步複製。

到此為止，備庫就已經初始化成功。


### 讀取事件

同樣是在 handle_slave_io() 函數中，會調用 relay_io->after_read_event() 鉤子函數，而實際上調用的函數是 repl_semi_slave_read_event() 。

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

由於我們在主庫上是對packet頭部有附加了3個比特的，這裡需要將其讀出來，同時需要更新event_buf及event_len的值；

如果rpl_semi_sync_slave_status為false，表示開啟io線程時未打開semisync，直接使用packet的長度即可，否則調用ReplSemiSyncSlave::slaveReadSyncHeader，去讀取packet的頭部信息，如果需要給主庫一個ack，則設置semi_sync_need_reply為TRUE

,

### 寫入relaylog

d.當將binlog寫入relaylog之後(即完成函數queue_event之後)，調用repl_semi_slave_queue_event
HOOK位置 (handle_slave_io)

4295       if (RUN_HOOK(binlog_relay_io, after_queue_event,
4296                    (thd, mi, event_buf, event_len, synced)))
4297       {
4298         mi->report(ERROR_LEVEL, ER_SLAVE_FATAL_ERROR,
4299                    ER(ER_SLAVE_FATAL_ERROR),
4300                    "Failed to run 'after_queue_event' hook");
4301         goto err;
4302       }

如果備庫開啟了semisync且需要ack時（pl_semi_sync_slave_status && semi_sync_need_reply），調用ReplSemiSyncSlave::slaveReply，向主庫發送數據包，包括當前的binlog文件名及偏移量信息

### 停止IO線程

e.停止IO線程時，調用函數repl_semi_slave_io_end，將rpl_semi_sync_slave_status設置為false，這裡判斷的mysql_reply實際上不會用到;

存在的問題主要集中在主庫上：

1.鎖的粒度太粗，看看能不能細化，或者使用lock-free的算法來優化

2.字符串比較的調用，大部分是對binlog文件名的比較，實際上只要比較後面的那一串數字就足夠了；嘗試下看看能否有性能優化

-->











## 參考

關於半同步參數的介紹可以參考 [Semisynchronous Replication Administrative Interface](https://dev.mysql.com/doc/refman/en/replication-semisync-interface.html)；以及 [Reference Manual - Semisync](https://dev.mysql.com/doc/refman/5.7/en/replication-semisync.html) 。


<!--
http://blog.itpub.net/15480802/viewspace-1430221
http://www.tuicool.com/articles/ERVNrmy
-->


{% highlight text %}
{% endhighlight %}
