---
title: MySQL GTID 簡介
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,gtid,uuid
description: 全局事務 ID (Global Transaction ID, GTID) 是用來強化數據庫在主備複製場景下，可以有效保證主備一致性、提高故障恢復、容錯能力。接下來，看看 GTID 是如何實現的，以及如何使用。
---

全局事務 ID (Global Transaction ID, GTID) 是用來強化數據庫在主備複製場景下，可以有效保證主備一致性、提高故障恢復、容錯能力。

接下來，看看 GTID 是如何實現的，以及如何使用。

<!-- more -->

![gtid]({{ site.url }}/images/databases/mysql/gtid-logo.jpg "gtid"){: .pull-center }

## 簡介

GTID 是一個已提交事務的編號，並且是一個全局唯一的編號，在 MySQL 中，GTID 實際上是由 UUID+TID 組成的。其中 UUID 是一個 MySQL 實例的唯一標識；TID 代表了該實例上已經提交的事務數量，並且隨著事務提交單調遞增。

使用 GTID 功能具體可以歸納為以下兩點：

* 可以確認事務最初是在哪個實例上提交的；
* 方便了 Replication 的 Failover 。

第一條顯而易見，對於第二點稍微介紹下。

在 GTID 出現之前，在配置主備複製的時候，首先需要確認 event 在那個 binlog 文件，及其偏移量；假設有 A(Master)、B(Slave)、C(Slave) 三個實例，如果主庫宕機後，需要通過 ```CHANGE MASTER TO MASTER_HOST='xxx', MASTER_LOG_FILE='xxx', MASTER_LOG_POS=nnnn``` 指向新庫。

這裡的難點在於，同一個事務在每臺機器上所在的 binlog 文件名和偏移都不同，這也就意味著需要知道新主庫的文件以及偏移量，對於有一個主庫+多個備庫的場景，如果主庫宕機，那麼需要手動從備庫中選出最新的備庫，升級為主，然後重新配置備庫。

這就導致操作特別複雜，不方便實施，這也就是為什麼需要 MHA、MMM 這樣的管理工具。

之所以會出現上述的問題，主要是由於各個實例 binlog 中的 event 以及 event 順序是一致的，但是 binlog+position 是不同的；而通過 GTID 則提供了對於事物的全局一致 ID，主備複製時，只需要知道這個 ID 即可。

另外，利用 GTID，MySQL 會記錄那些事物已經執行，從而也就知道接下來要執行那些事務。當有了 GTID 之後，就顯得非常的簡單；因為同一事務的 GTID 在所有節點上的值一致，那麼就可以直接根據 GTID 就可以完成 failover 操作。


### UUID

MySQL 5.6 用 128 位的 server_uuid 代替了原本的 32 位 server_id 的大部分功能；主要是擔心手動設置配置文件中的 server_id 時，可能會產生衝突，通過 UUID(128bits) 避免衝突。

首次啟動時會調用 generate_server_uuid() 函數，自動生成一個 server_uuid，並保存到 auto.cnf 文件，目前該文件的唯一目的就是保存 server_uuid；下次啟動時會自動讀取 auto.cnf 文件，繼續使用上次生成的 UUID 。

可以通過如下命令查看當前服務器的 UUID 值。

{% highlight text %}
mysql> SHOW GLOBAL VARIABLES LIKE 'server_uuid';
c133fbac-e07b-11e6-a219-286ed488dd40
{% endhighlight %}

在 Slave 向 Master 申請 binlog 時，會先發送 Slave 自己的 server_uuid，Master 會使用該值作為 kill_zombie_dump_threads 的參數，來終止衝突或者僵死的 BINLOG_DUMP 線程。

### GTID

MySQL 在 5.6 版本加入了 GTID 功能，GTID 也就是事務提交時創建分配的唯一標識符，所有事務均與 GTID 一一映射，其格式類似於：

{% highlight text %}
5882bfb0-c936-11e4-a843-000c292dc103:1
{% endhighlight %}

這個字符串，用 ```:``` 分開，前面表示這個服務器的 server_uuid，後面是事務在該服務器上的序號。

GTID 模式實例和非 GTID 模式實例是不能進行復制的，要求非常嚴格；而且 gtid_mode 是隻讀的，要改變狀態必須 1) 關閉實例、2) 修改配置文件、3) 重啟實例。

與 GTID 相關的參數可以參考如下：

{% highlight text %}
mysql> SHOW GLOBAL VARIABLES LIKE '%gtid%';
+----------------------------------+-------+
| Variable_name                    | Value |
+----------------------------------+-------+
| binlog_gtid_simple_recovery      | ON    |
| enforce_gtid_consistency         | ON    |
| gtid_executed                    |       |    已經在該實例上執行過的事務
| gtid_executed_compression_period | 1000  |
| gtid_mode                        | ON    |
| gtid_owned                       |       |    正在執行的事務的gtid以及對應的線程ID
| gtid_purged                      |       |    本機已經執行，且被PURGE BINARY LOG刪除
| session_track_gtids              | OFF   |
+----------------------------------+-------+
8 rows in set (0.00 sec)

mysql> SHOW SESSION VARIABLES LIKE 'gtid_next';
+---------------+-----------+
| Variable_name | Value     |
+---------------+-----------+
| gtid_next     | AUTOMATIC |        session級別變量，表示下一個將被使用的gtid
+---------------+-----------+
1 row in set (0.02 sec)

{% endhighlight %}

對於 gtid_executed 變量，執行 ```reset master``` 會將該變量置空；而且還可以通過設置 gtid_next 執行一個空事務，來影響 gtid_executed 。


### 生命週期

一個 GTID 的生命週期包括：

1. 事務在主庫上執行並提交，此時會給事務分配一個 gtid，該值會被寫入到 binlog 中；
2. 備庫讀取 relaylog 中的 gtid，並設置 session 級別的 gtid_next 值，以告訴備庫下一個事務必須使用這個值；
3. 備庫檢查該 gtid 是否已經被使用並記錄到他自己的 binlog 中；
4. 由於 gtid_next 非空，備庫不會生成一個新的 gtid，而是使用從主庫獲得的 gtid 。

由於 GTID 在全局唯一性，通過 GTID 可以在自動切換時對一些複雜的複製拓撲很方便的提升新主庫及新備庫。


### 通訊協議

開啟 GTID 之後，除了將原有的 file+position 替換為 GTID 之外，實際上還實現了一套新的複製協議，簡單來說，GTID 的目的就是保證所有節點執行了相同的事務。

老協議很簡單，備庫鏈接到主庫時會帶有 file+position 信息，用來確認從那個文件開始複製；而新協議則是在鏈接到主庫時會發送當前備庫已經執行的 GTID Sets，主庫將所有缺失的事務發送給備庫。

<!--
TODODO:
LINKKK: https://www.percona.com/blog/2014/05/09/gtids-in-mysql-5-6-new-replication-protocol-new-ways-to-break-replication/
-->


## 源碼實現

在 binlog 中，與 GTID 相關的事件類型包括了：

* GTID_LOG_EVENT 隨後事務的 GTID；
* ANONYMOUS_GTID_LOG_EVENT 匿名 GTID 事件類型；
* PREVIOUS_GTIDS_LOG_EVENT 當前 binlog 文件之前已經執行過的 GTID 集合，會記錄在 binlog 文件頭。

如下是一個示例：

{% highlight text %}
# at 120
# 130502 23:23:27 server id 119821  end_log_pos 231 CRC32 0x4f33bb48     Previous-GTIDs
# 10a27632-a909-11e2-8bc7-0010184e9e08:1,
# 7a07cd08-ac1b-11e2-9fcf-0010184e9e08:1-1129
{% endhighlight %}

除 gtid 之外，還有 gtid set 的概念，類似 ```7a07cd08-ac1b-11e2-9fcf-0010184e9e08:1-31```，其中變量 gtid_executed 和 gtid_purged 都是典型的 gtid set 類型變量；在一個複製拓撲結構中，gtid_executed 可能包含好幾組數據。


### 結構體

在內存中通過 ```Gtid_state *gtid_state``` 全局變量維護了三個集合。

{% highlight cpp %}
class Gtid_state
{
private:
    Gtid_set lost_gtids;          // 對應gtid_purged
    Gtid_set executed_gtids;      // 對應gtid_executed
    Owned_gtids owned_gtids;      // 對應gtid_owned
};

Gtid_state *gtid_state= NULL;
{% endhighlight %}


<!--

在主庫執行一個事務的過程中，關於Gtid主要涉及到以下幾個部分：

事務開始，執行第一條SQL時，在寫入第一個“BEGIN” 的QUERY EVENT 之前， 為binlog cache 的Group_cache中分配一個group(Group_cache::add_logged_group)，並寫入一個Gtid_log_event，此時並未為其分配事務id,backtrace 如下：

{% highlight text %}
handler::ha_write_row()
 |-binlog_log_row()
   |-write_locked_table_maps()
     |-THD::binlog_write_table_map()
       |-binlog_start_trans_and_stmt()
         |-binlog_cache_data::write_event()
           |-Group_cache::add_logged_group()
{% endhighlight %}

暫時還不清楚什麼時候一個事務裡會有多個gtid的group_cache.




在 binlog group commit 的 flush 階段：



第一步，調用Group_cache::generate_automatic_gno來為當前線程生成一個gtid，分配給thd->owned_gtid，並加入到owned_gtids中，backtrace如下：

{% highlight text %}
MYSQL_BIN_LOG::process_flush_stage_queue()
  |-assign_automatic_gtids_to_flush_group()
    |-Gtid_state::generate_automatic_gtid()     gtid_next為AUTOMATIC時，生成新的GTID
      |-lock_sidno()                            為當前sidno加鎖，分配過程互斥
      |-get_server_sidno()
      |-get_automatic_gno()                     獲取事務ID
      | |-get_server_sidno()                    初始化候選值
      |-acquire_ownership()
      | |-Owned_gtids::add_gtid_owner()         添加到owned_gtids集合中
      |-unlock_sidno()                          解鎖
{% endhighlight %}

也就是說，直到事務完成，準備把 binlog 刷到 binlog cache 時，才會去為其分配 gtid 。


當gtid_next的類型為AUTOMATIC時，調用generate_automatic_gno生成事務id(gno)，分配流程大概如下：

2.gtid_state->get_automatic_gno(automatic_gtid.sidno);
        |–>初始化候選(candidate)gno為1
        |–>從logged_gtids[$sidno]中掃描，獲取每個gno區間(iv)：
               |–>當candidate < iv->start（或者MAX_GNO，如果iv為NULL）時，判斷candidate是否有被佔用，如果沒有的話，則使用該candidate，從函數返回，否則candidate++，繼續本步驟
        |–>將candidate設置為iv->end，iv指向下一個區間，繼續第2步
        從該過程可以看出，這裡兼顧了區間存在碎片的場景，有可能分配的gno並不是全局最大的gno. 不過在主庫不手動設置gtid_next的情況下，我們可以認為主庫上的gno總是遞增的。



第二步， 調用Gtid_state::update_on_flush將當前事務的gtid加入到logged_gtids中,backtrace如下：
MYSQL_BIN_LOG::process_flush_stage_queue->MYSQL_BIN_LOG::flush_thread_caches->binlog_cache_mngr::flush->binlog_cache_data::flush->MYSQL_BIN_LOG::write_cache->Gtid_state::update_on_flush
在bin log group commit的commit階段

調用Gtid_state::update_owned_gtids_impl 從owned_gtids中將當前事務的gtid移除,backtrace 如下：
MYSQL_BIN_LOG::ordered_commit->MYSQL_BIN_LOG::finish_commit->Gtid_state::update_owned_gtids_impl

上述步驟涉及到的是對logged_gtids和owned_gtids的修改。而lost_gtids除了啟動時維護外，就是在執行Purge操作時維護。

例如，當我們執行purge binary logs to ‘mysql-bin.000205’ 時， mysql-bin.index先被更新掉，然後再根據index文件找到第一個binlog文件的PREVIOUS_GTIDS_LOG_EVENT事件，更新lost_gtids集合，backtrace如下：
purge_master_logs->MYSQL_BIN_LOG::purge_logs->MYSQL_BIN_LOG::init_gtid_sets->read_gtids_from_binlog->Previous_gtids_log_event::add_to_set->Gtid_set::add_gtid_encoding->Gtid_set::add_gno_interval

關於binlog group commit，參見之前寫的博客：http://mysqllover.com/?p=581
c.如何持久化GTID
當重啟MySQL後，我們看到GTID_EXECUTED和GTID_PURGED和重啟前是一致的。

持久化GTID，是通過全局對象gtid_state來管理的。gtid_state在系統啟動時調用函數gtid_server_init分配內存；如果打開了binlog，則會做進一步的初始化工作：

quoted code:

5419       if (mysql_bin_log.init_gtid_sets(
5420             const_cast<Gtid_set *>(gtid_state->get_logged_gtids()),
5421             const_cast<Gtid_set *>(gtid_state->get_lost_gtids()),
5422             opt_master_verify_checksum,
5423             true/*true=need lock*/))
5424         unireg_abort(1);

gtid_state 包含3個gtid集合：logged_gtids， lost_gtids， owned_gtids，前兩個都是gtid_set類型, owned_gtids類型為Owned_gtids

MYSQL_BIN_LOG::init_gtid_sets 主要用於初始化logged_gtids和lost_gtids,該函數的邏輯簡單描述下：

1.掃描mysql-index文件，蒐集binlog文件名，並加入到filename_list中
2.從最後一個文件開始往前讀，依次調用函數read_gtids_from_binlog：
      |–>打開binlog文件，如果讀取到PREVIOUS_GTIDS_LOG_EVENT事件
          (1)無論如何，將其加入到logged_gtids（prev_gtids_ev->add_to_set(all_gtids)）
          (2)如果該文件是第一個binlog文件，將其加入到lost_gtids（prev_gtids_ev->add_to_set(prev_gtids)）中.

      |–>獲取GTID_LOG_EVENT事件
          (1) 讀取該事件對應的sidno，sidno= gtid_ev->get_sidno(false);
               這是一個32位的整型，用sidno來代表一個server_uuid，從1開始計算，這主要處於節省內存的考慮。維護在全局對象global_sid_map中。
               當sidno還沒加入到map時，調用global_sid_map->add_sid(sid)，sidno從1開始遞增。

          (2) all_gtids->ensure_sidno(sidno)
               all_gtids是gtid_set類型，可以理解為一個集合，ensure_sidno就是要確保這個集合至少可以容納sidno個元素

          (3) all_gtids->_add_gtid(sidno, gtid_ev->get_gno()
               將該事件中記錄的gtid加到all_gtids[sidno]中(最終調用Gtid_set::add_gno_interval，這裡實際上是把(gno, gno+1)這樣一個區間加入到其中，這裡
               面涉及到區間合併，交集等操作    )
當第一個文件中既沒有PREVIOUS_GTIDS_LOG_EVENT， 也沒有GTID_LOG_EVENT時，就繼續讀上一個文件
如果只存在PREVIOUS_GTIDS_LOG_EVENT事件，函數read_gtids_from_binlog返回GOT_PREVIOUS_GTIDS
如果還存在GTID_LOG_EVENT事件，返回GOT_GTIDS

這裡很顯然存在一個問題，即如果在重啟前，我們並沒有使用gtid_mode，並且產生了大量的binlog，在這次重啟後，我們就可能需要掃描大量的binlog文件。這是一個非常明顯的Bug， 後面再集中討論。

3.如果第二部掃描，沒有到達第一個文件，那麼就從第一個文件開始掃描，和第2步流程類似，讀取到第一個PREVIOUS_GTIDS_LOG_EVENT事件，並加入到lost_gtids中。

簡單的講，如果我們一直打開的gtid_mode，那麼只需要讀取第一個binlog文件和最後一個binlog文件，就可以確定logged_gtids和lost_gtids這兩個GTID SET了。

二、備庫上的GTID
a.如何保持主備GTID一致
由於在binlog中記錄了每個事務的GTID，因此備庫的複製線程可以通過設置線程級別GTID_NEXT來保證主庫和備庫的GTID一致。

默認情況下，主庫上的thd->variables.gtid_next.type為AUTOMATIC_GROUP，而備庫為GTID_GROUP

備庫SQL線程gtid_next輸出：
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

這些變量在執行Gtid_log_event時被賦值：Gtid_log_event::do_apply_event，大體流程為：
1.rpl_sidno sidno= get_sidno(true);  獲取sidno
2.thd->variables.gtid_next.set(sidno, spec.gtid.gno);  設置gtid_next
3.gtid_acquire_ownership_single(thd);

     |–>檢查該gtid是否在logged_gtids集合中，如果在的話，則返回（gtid_pre_statement_checks會忽略該事務）
     |–>如果該gtid已經被其他線程擁有，則等待(gtid_state->wait_for_gtid(thd, gtid_next))，否則將當前線程設置為owner(gtid_state->acquire_ownership(thd, gtid_next))

在上面提到，有可能當前事務的GTID已經在logged_gtids中，因此在執行Rows_log_event::do_apply_event或者mysql_execute_command函數中，都會去調用函數gtid_pre_statement_checks
該函數也會在每個SQL執行前，檢查gtid是否合法，主要流程包括：
1.當打開選項enforce_gtid_consistency時，檢查DDL是否被允許執行（thd->is_ddl_gtid_compatible()），若不允許，返回GTID_STATEMENT_CANCEL
2.檢查當前SQL是否會產生隱式提交併且gtid_next被設置（gtid_next->type != AUTOMATIC_GROUP），如果是的話，則會拋出錯誤ER_CANT_DO_IMPLICIT_COMMIT_IN_TRX_WHEN_GTID_NEXT_IS_SET 並返回GTID_STATEMENT_CANCEL，注意這裡會導致bug#69045
3.對於BEGIN/COMMIT/ROLLBACK/(SET OPTION 或者 SELECT )且沒有使用存儲過程/ 這幾種類型的SQL，總是允許執行，返回GTID_STATEMENT_EXECUTE
4.gtid_next->type為UNDEFINED_GROUP，拋出錯誤ER_GTID_NEXT_TYPE_UNDEFINED_GROUP，返回GTID_STATEMENT_CANCEL
5.gtid_next->type == GTID_GROUP且thd->owned_gtid.sidno == 0時， 返回GTID_STATEMENT_SKIP

其中第五步中處理了函數gtid_acquire_ownership_single的特殊情況

b.備庫如何發起DUMP請求

引入GTID，最大的好處當然是我們可以隨心所欲的切換主備拓撲結構了。在一個正常運行的複製結構中，我們可以在備庫簡單的執行如下SQL：

CHANGE MASTER TO MASTER_USER=’$USERNAME’, MASTER_HOST=’ ‘, MASTER_PORT=’ ‘, MASTER_AUTO_POSITION=1;

打開GTID後，我們就無需指定binlog文件或者位置，MySQL會自動為我們做這些事情。這裡的關鍵就是MASTER_AUTO_POSITION。IO線程連接主庫，可以大概分為以下幾步：
1.IO線程在和主庫建立TCP鏈接後，會去獲取主庫的uuid（get_master_uuid），然後在主庫上設置一個用戶變量@slave_uuid(io_thread_init_commands)

2.之後，在主庫上註冊SLAVE（register_slave_on_master）

在主庫上調用register_slave來註冊備庫，將備庫的host,user,password,port,server_id等信息記錄到slave_list哈希中。

3.調用request_dump，開始向主庫請求數據，這裡分兩種情況：
MASTER_AUTO_POSITION=0時，向主庫發送命令的類型為COM_BINLOG_DUMP，這是傳統的請求BINLOG的模式
MASTER_AUTO_POSITION=1時，命令類型為COM_BINLOG_DUMP_GTID，這是新的方式。
這裡我們只討論第二種。第二種情況下，會先去讀取備庫已經執行的gtid集合
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
構建完成發送包後，發送給主庫。

在主庫上接受到命令後，調用入口函數com_binlog_dump_gtid，流程如下：
1.slave_gtid_executed.add_gtid_encoding(packet_position, data_size) ;讀取備庫傳來的GTID SET
2.讀取備庫的uuid(get_slave_uuid)，被根據uuid來kill殭屍線程(kill_zombie_dump_threads)
這也是之前SLAVE IO線程執行SET @SLAVE_UUID的用處。
3.進入mysql_binlog_send函數：
         |–>調用MYSQL_BIN_LOG::find_first_log_not_in_gtid_set，從最後一個Binlog開始掃描，獲取文件頭部的PREVIOUS_GTIDS_LOG_EVENT，如果它是slave_gtid_executed的子集，保存當前binlog文件名，否則繼續向前掃描。
         這一步的目的就是為了找出備庫執行到的最後一個Binlog文件。

         |–>從這個文件頭部開始掃描，遇到GTID_EVENT時，會去判斷該GTID是否包含在slave_gtid_executed中：
                         Gtid_log_event gtid_ev(packet->ptr() + ev_offset,
                                 packet->length() – checksum_size,
                                 p_fdle);
                          skip_group= slave_gtid_executed->contains_gtid(gtid_ev.get_sidno(sid_map),
                                                     gtid_ev.get_gno());
         主庫通過GTID決定是否可以忽略事務，從而決定執行開始的位置


注意，在使用MASTER_LOG_POSITION後，就不要指定binlog的位置，否則會報錯。
三、運維操作
a.如何忽略複製錯誤

當備庫複製出錯時，傳統的跳過錯誤的方法是設置sql_slave_skip_counter,然後再START SLAVE。
但如果打開了GTID，就會設置失敗：

mysql> set global sql_slave_skip_counter = 1;

ERROR 1858 (HY000): sql_slave_skip_counter can not be set when the server is running with @@GLOBAL.GTID_MODE = ON. Instead, for each transaction that you want to skip, generate an empty transaction with the same GTID as the transaction

提示的錯誤信息告訴我們，可以通過生成一個空事務來跳過錯誤的事務。

我們手動產生一個備庫複製錯誤：

Last_SQL_Error: Error ‘Unknown table ‘test.t1” on query. Default database: ‘test’. Query: ‘DROP TABLE `t1` /* generated by server */’

查看binlog中，該DDL對應的GTID為7a07cd08-ac1b-11e2-9fcf-0010184e9e08:1131

在備庫上執行：

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

再查看show slave status，就會發現錯誤事務已經被跳過了。這種方法的原理很簡單，空事務產生的GTID加入到GTID_EXECUTED中，這相當於告訴備庫，這個GTID對應的事務已經執行了。
b.重指主庫
使用change master to …. , MASTER_AUTO_POSITION=1；
注意在整個複製拓撲中，都需要打開gtid_mode

c.新的until條件
5.6提供了新的util condition，可以根據GTID來決定備庫複製執行到的位置
SQL_BEFORE_GTIDS：在指定的GTID之前停止複製
SQL_AFTER_GTIDS ：在指定的GTID之後停止複製

判斷函數為Relay_log_info::is_until_satisfied

詳細文檔見：http://dev.mysql.com/doc/refman/5.6/en/start-slave.html

d.適當減小binlog文件的大小
如果開啟GTID，理論上最好調小每個binlog文件的最大值，以縮小掃描文件的時間。
四、存在的bug
bug#69097， 即使關閉了gtid_mode，也會在啟動時去掃描binlog文件。
當在重啟前沒有使用gtid_mode，重啟後可能會去掃描所有的binlog文件，如果Binlog文件很多的話，這顯然是不可接受的。

bug#69096，無法通過GTID_NEXT_LIST來跳過複製錯誤，因為默認編譯下，GTID_NEXT_LIST未被編譯進去。
TODO:GTID_NEXT_LIST的邏輯上面均未提到，有空再看。

bug#69095，將備庫的複製模式設置為STATEMENT/MIXED。 主庫設置為ROW模式，執行DML 會導致備庫複製中斷

Last_SQL_Error: Error executing row event: ‘Cannot execute statement: impossible to write to binary log since statement is in row format and BINLOG_FORMAT = STATEMENT.’

判斷報錯的backtrace：
handle_slave_worker->slave_worker_exec_job->Rows_log_event::do_apply_event->open_and_lock_tables->open_and_lock_tables->lock_tables->THD::decide_logging_format


解決辦法：將備庫的複製模式設置為’ROW’ ，保持主備一致
該bug和GTID無關

bug#69045, 當主庫執行類似 FLUSH PRIVILEGES這樣的動作時，如果主庫和備庫都開啟了gtid_mode，會導致複製中斷
Last_SQL_Error: Error ‘Cannot execute statements with implicit commit inside a transaction when @@SESSION.GTID_NEXT != AUTOMATIC or @@SESSION.GTID_NEXT_LIST != NULL.’ on query. Default database: ”. Query: ‘flush privileges’

也是一個很低級的bug，在MySQL5.6.11版本中，如果有可能導致隱式提交的事務， 則gtid_next必須等於AUTOMATIC，對備庫複製線程而言，很容易就中斷了，判斷邏輯在函數gtid_pre_statement_checks中





對於 GTID 的實現，MySQL 和 MariaDB 的實現不同。





Multi-threaded Slave 備庫的並行複製， slave_parallel_workers > 1 。


Crash-safe Slave 也就是說備庫崩潰時，可以自動恢復，需要注意的是，只對 InnoDB 有效，而且需要設置 relay_log_info_repository=TABLE and relay_log_recovery=1 。


組提交，包括了 binlog 以及 InnoDB 的組提交。





???假設現在已經搭建了主備服務器，然後可以通過 mysqlreplicate 命令配置主從複製 (master slave rpl-user 參數必須) 。
???mysqlreplicate --master=root:new-password@127.0.0.1:3307 --slave=root:new-password@127.0.0.1:3308 --rpl-user=mysync:kidding


搭建配置完成之後可以通過 mysqlrplcheck 檢查主備複製的狀態。
???mysqlrplcheck --master=root:new-password@127.0.0.1:3307 --slave=root:new-password@127.0.0.1:3308


mysqlrplshow 查看主從架構，備庫信息通過 SHOW SLAVE HOSTS 命令獲取。
???mysqlrplshow --master=root:new-password@127.0.0.1:3307 --discover-slaves-login=root:new-password


mysqlfailover 監視主從健康狀態，需要保證配置文件中添加 gtid-mode=on+enforce-gtid-consistency=on 。
???mysqlfailover --master=root:new-password@127.0.0.1:3307 --discover-slaves-login=root:new-password



1. 創建package.json文件，執行npm init -yes創建該初始文件。

2. 創建main.js文件，內容如下。
var React = require('react');
var ReactDOM = require('react-dom');
ReactDOM.render(
  <h1>Hello, world!</h1>,
  document.getElementById('example')
);

3. 安裝browserify，並生成瀏覽器可用的javascript文件。
npm install -g browserify
npm install --save-dev react react-dom babelify babel-preset-react babel-preset-es2015
其中babelify(Browserify的babel轉換器)，babel-preset-react(針對React的babel轉碼規則包)。
cat .babelrc
{
  "presets": ["react", "es2015"]
}

4. 使用Browserify對main.js處理及打包。
如下的只需要包含bundle.js即可，無需react.js+react-dom.js。
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
阮一峰的blog關於Reactjs介紹。
http://www.ruanyifeng.com/blog/2015/03/react.html
http://www.ruanyifeng.com/blog/2016/02/react-testing-tutorial.html
官方關於react的教程。
https://github.com/reactjs/react-tutorial/

1.C語言：PC-Lint、codedex
2.Java：findbugs、PMD、checkstyle（不強制要求）
3.python：flake8
4.JavaScript：JSHint
https://addons.mozilla.org/zh-CN/firefox/addon/react-devtools/


JSX基本語法規則：
    遇到以 < 開頭的 HTML 標籤，就用 HTML 規則解析；遇到以 { 開頭的代碼塊，就用 JavaScript 規則解析。
什麼是JSX？
把 HTML 模板直接嵌入到 JS 代碼裡面，從而做到模板和組件關聯。但 JS 不支持這種包含 HTML 的語法，所以需要通過工具將 JSX 編譯輸出成 JS 代碼才能使用。
為了把 JSX 轉成標準的 JavaScript，有如下兩種轉換方法：
    1. 用 <script type="text/babel"> 標籤，並引入 Babel 來完成在瀏覽器裡的代碼轉換。
    2. 在瀏覽器裡打開這個html，你應該可以看到成功的消息！
為什麼要使用JSX？
    JSX 執行更快，因為它在編譯為 JavaScript 代碼後進行了優化。
    它是類型安全的，在編譯過程中就能發現錯誤。
    使用 JSX 編寫模板更加簡單快速。

-->

























## GTID 限制

開啟 GTID 之後，會由部分的限制，內容如下。

### 更新非事務引擎表

GTID 同步複製是基於事務的，所以 MyISAM 存儲引擎不支持，這可能導致多個 GTID 分配給同一個事務。

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

上述的語句不支持，因為該語句會被拆分成 ```CREATE TABLE``` 和 ```INSERT ``` 兩個事務，並且這個兩個事務被分配了同一個 GTID，這會導致 ```INSERT``` 被備庫忽略掉。

{% highlight text %}
mysql> CREATE TABLE hello ENGINE=InnoDB AS SELECT * FROM hello;
ERROR 1786 (HY000): Statement violates GTID consistency: CREATE TABLE ... SELECT.
{% endhighlight %}

### 臨時表

事務內部不能執行創建刪除臨時表語句，但可以在事務外執行，但必須設置 ```set autocommit=1``` 。

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

與臨時表相關的包括了 ```CREATE/DROP TEMPORARY TABLE``` 臨時表操作。


### 總結

實際上，一般啟動 GTID 時，可以啟用 enforce-gtid-consistency 選項，從而在執行上述不支持的語句時，將會返回錯誤。



## 運維相關

簡單介紹一些常見的運維操作。

當備庫配置為 GTID 複製時，可以通過 ```SHOW SLAVE STATUS``` 命令查看其中的 ```Retrieved_Gtid_Set``` 和 ```Executed_Gtid_Set```，分別表示已經從主庫獲取，以及已經執行的事務。


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
               Relay_Log_File: mysqld-relay-bin.000009      # 備庫中的relaylog文件
                Relay_Log_Pos: 736                          # 備庫執行的偏移量
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

一般來說是已經從主庫複製過來，只是在執行的時候報錯，可以從上述的狀態中查看，然後通過命令 ```show relaylog events in 'mysqld-relay-bin.000009' from 736\G``` 確認。

### 忽略複製錯誤

當備庫複製出錯時，如果仍採用傳統的跳過錯誤方法，也就是設置 ```sql_slave_skip_counter```，然後再 ```START SLAVE```；但如果打開了 GTID，在設置上述參數時，就會報錯。

提示的錯誤信息告訴我們，可以通過生成一個空事務來跳過錯誤的事務，示例如下。

{% highlight text %}
mysql> CREATE DATABASE test;
Query OK, 1 row affected (0.00 sec)
mysql> USE test;
Database changed
mysql> CREATE TABLE foobar(id INT PRIMARY KEY);
Query OK, 0 rows affected (0.01 sec)

----- 備庫執行如下SQL
mysql> INSERT INTO foobar VALUES(1),(2),(3);
Query OK, 3 rows affected (0.00 sec)
Records: 3  Duplicates: 0  Warnings: 0
----- 主庫執行如下SQL
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

----- 備庫執行如下SQL
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

再查看 ```SHOW SLAVE STATUS``` 時，就會發現錯誤事務已經被跳過了；這種方法的原理很簡單，空事務產生的 GTID 加入到 GTID_EXECUTED 中，相當於告訴備庫，這個 GTID 對應的事務已經執行了。

注意，此時主從會導致數據不一致，需要進行修復。


### 主庫事件被清除

變量 gtid_purged 記錄了本機已經執行過，且已被 ```PURGE BINARY LOGS TO``` 命令清理的 gtid_set ；在此，看看如果主庫上把某些備庫還沒有獲取到的 gtid event 清理後會有什麼樣的結果。

{% highlight text %}
----- 主庫執行如下SQL
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

----- 在備庫上執行如下SQL
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

接下來我們在備庫忽略 purged 的部分，然後強行同步，在備庫同樣設置 gtid_purged 變量。

{% highlight text %}
----- 備庫上清除gtid_executed，然後設置gtid_purged，忽略主庫的事務
mysql> RESET MASTER;
Query OK, 0 rows affected (0.05 sec)
mysql> SET GLOBAL gtid_purged = "91116597-016c-11e7-94db-ac2b6e8b4228:1-5";
Query OK, 0 rows affected (0.05 sec)

----- 備庫上執行上述的SQL，需要根據具體情況修復
mysql> CREATE TABLE foobar1 (id INT) ENGINE=InnoDB;
Query OK, 0 rows affected (0.01 sec)
mysql> CREATE TABLE foobar2 (id INT) ENGINE=InnoDB;
Query OK, 0 rows affected (0.02 sec)

----- 啟動備庫即可
mysql> START SLAVE;
Query OK, 0 rows affected (0.02 sec)
{% endhighlight %}

實際生產應用中，當遇到上述的情況後，需要 DBA 人為保證該備庫數據和主庫一致；或者即使不一致，這些差異也不會導致今後的主從異常，例如，所有主庫上只有 insert 沒有 update 。

### Errant-Transaction

簡單來說，就是沒有在主庫執行，而是直接在備庫執行的事務，通常可能是在修復備庫的問題或者應用異常寫入了備庫導致。

如果發生 ET 的備庫被提升為主庫，那麼根據 GTID 協議，新主庫就會發現備庫沒有執行 ET 中的事務，接下來就可能會發生如下兩種情況：

1. 備庫中 ET 對應的 binlog 仍然存在，那麼會將相應的事件發送給新的備庫，此時則會導致數據不一致或者發生其它異常；
2. 備庫中 ET 對應的 binlog 已經被刪除，由於無法發送給備庫，那麼會導致複製異常。

對於有些需要修復備庫的任務可以通過 ``` SET sql_log_bin=0``` 命令，設置會話參數，防止生成 ET，當然，此時需要保證數據一致性。在修復時有兩種方案：

1. 在 GTID 的執行歷史中刪除 ET，這樣即使備庫被提升為主庫，也不會發生異常；
2. 在其它 MySQL 服務中執行空白的事務，使其它庫認為已經執行了 ET，那麼 Failover 之後也不會嘗試獲取相應的事件。

接下來看個示例。

{% highlight text %}
----- 在主庫執行如下SQL，查看主庫已執行事務對應的GTID Sets
mysql> SHOW MASTER STATUS\G
*************************** 1. row ***************************
... ...
Executed_Gtid_Set: 8e349184-bc14-11e3-8d4c-0800272864ba:1-30,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-7

----- 同上，在備庫執行
mysql> SHOW SLAVE STATUS\G
... ...
Executed_Gtid_Set: 8e349184-bc14-11e3-8d4c-0800272864ba:1-29,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-9

----- 比較兩個GTID Sets
mysql> SELECT gtid_subset('8e349184-bc14-11e3-8d4c-0800272864ba:1-29,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-9','8e349184-bc14-11e3-8d4c-0800272864ba:1-30,
8e3648e4-bc14-11e3-8d4c-0800272864ba:1-7') AS slave_is_subset;
+-----------------+
| slave_is_subset |
+-----------------+
|               0 |
+-----------------+
1 row in set (0.00 sec)

----- 獲取對應的差值
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

接下來，看看如何修復，假設有 3 個服務，A (主庫)、B (備庫的異常 XXX:3) 以及 C (備庫的異常 YYY:18-19)，那麼，接下來可以在不同的服務器上寫入空白事務。

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

當然，也可以使用 MySQL-Utilities 中的 mysqlslavetrx 腳本寫入空白事務。

{% highlight text %}
$ mysqlslavetrx --gtid-set='457e7d57-1da2-11e7-9c71-286ed488dd40:5' --verbose \
    --slaves='root:new-password@127.0.0.1:3308,root:new-password@127.0.0.1:3309'
{% endhighlight %}




## 參考

[MySQL Reference Manual - Replication with Global Transaction Identifiers](https://dev.mysql.com/doc/refman/en/replication-gtids.html) 。

關於 GTID 的介紹可以參考 [MySQL Replication for High Availability - Tutorial](https://severalnines.com/resources/tutorials/mysql-replication-high-availability-tutorial) 中的內容，一篇不錯的介紹，也可以直接參考 [本地](/reference/databases/mysql/MySQL_Replication_for_High_Availability.mht) 。

<!--
Database Daily Ops Series: GTID Replication
https://www.percona.com/blog/2016/11/10/database-daily-ops-series-gtid-replication/

Database Daily Ops Series: GTID Replication and Binary Logs Purge
https://www.percona.com/blog/2016/12/01/database-daily-ops-series-gtid-replication-binary-logs-purge/

http://mysqllover.com/?p=594

在使用GTID之前需要考慮的內容
http://www.fromdual.ch/things-you-should-consider-before-using-gtid

[MySQL 5.6] GTID內部實現、運維變化及存在的bug
http://mysqllover.com/?p=594
-->

{% highlight text %}
{% endhighlight %}
