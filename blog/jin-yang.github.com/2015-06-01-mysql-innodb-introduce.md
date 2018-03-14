---
title: InnoDB 簡單介紹
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: innodb,mysql,簡介
description: 我們知道，在 MySQL 中，存儲引擎是通過插件實現的，從而使得添加存儲引擎時相對來說比較簡單，而且目前支持多種類型的存儲引擎。InnoDB 目前基本上已經是實時上的沒人存儲引擎了，現在可以支持外鍵、行鎖、MVCC、支持標準的 4 種隔離級別等等。實際上，這也就意味著 InnoDB 是相當複雜的一個存儲引擎。
---

我們知道，在 MySQL 中，存儲引擎是通過插件實現的，從而使得添加存儲引擎時相對來說比較簡單，而且目前支持多種類型的存儲引擎。

InnoDB 目前基本上已經是實時上的沒人存儲引擎了，現在可以支持外鍵、行鎖、MVCC、支持標準的 4 種隔離級別等等。實際上，這也就意味著 InnoDB 是相當複雜的一個存儲引擎。

在此，僅簡單介紹下。

<!-- more -->

## 簡介

在 MySQL 中，現仍然使用 InnoDB，而在 MariaDB 中，使用 XtraDB 替換了 InnoDB，但是仍然顯示為 InnoDB，可以通過 show engines 查看，不知道後面會不會給換掉。

另一個比較不錯的存儲引擎是 Percona 開發的 TokuDB，還沒有仔細研究過，暫時標記下。

### InnoDB 特性

InnoDB 採用的 **索引組織表**，也就是採用唯一且非空的主鍵進行組織，主健可以為多個字段；如果沒有定義主鍵，則會選擇第一個非空的唯一索引；如果沒有符合條件的索引則會使用一個隱含的 6-bytes 遞增字段。

> 堆組織表(Heap Organized Table, HOT)，數據會以堆的方式進行管理，插入數據時，會使用第一個能放下此數據的空閒空間。索引組織表(Index Organized Table, IOT)，數據按主鍵進行存儲和排序。
>
> Oracle 支持堆表以及索引組織表；PostgreSQL 只支持堆表；InnoDB 只支持索引組織表。

上述的索引 (主健) 稱為聚集索引 (Clustered Index)，除了聚集索引之外的其它索引都被稱為二級索引 (Secondary Index)，二級索引還會保存對應的聚集索引，因此通常來說聚集索引不要太長。

索引是以 B+Tree 組織，對磁盤讀取的最小單位為頁 (Page)，默認大小是 16KB。頁的大小可以在創建實例時通過 innodb_page_size 參數設置，創建之後不能修改，不同頁大小之間的實例不兼容。

當新記錄插入時，會預留 1KB 為以後插入時使用。如果是順序插入(升序或者降序)，則一般會佔用 15/16，對於隨機插入則會在 1/2~15/16 之間，小於 1/2 則嘗試合併刪除。

### 常用操作

InnoDB 相關的操作。

{% highlight text %}
mysql> SHOW ENGINES;                                         ← 查看現在支持的存儲引擎。
mysql> SELECT * FROM information_schema.engines;             ← 功能同上
mysql> SHOW VARIABLES LIKE 'innodb_version';                 ← innodb的版本
mysql> SHOW VARIABLES LIKE 'innodb_%_io_threads';            ← io線程
mysql> SHOW VARIABLES LIKE 'innodb_purge_threads';           ← purge線程
mysql> SHOW VARIABLES LIKE 'innodb_buffer_pool_size';        ← 緩衝池大小
mysql> SHOW VARIABLES LIKE 'innodb_buffer_pool_instances';   ← 緩衝池實例數目
mysql> SHOW ENGINE INNODB STATUS\G                           ← 詳細狀態
{% endhighlight %}


## 文件管理

MySQL 的各種數據保存在 datadir 變量指定的目錄下，使用 OS 的文件系統；其中，表結構保存在數據庫的目錄下，文件為 *.frm，但是 InnoDB 同時會在表空間中保存表的元數據，顯然不能通過刪除 *.frm 來刪除表，暫沒有嘗試過對其它的存儲引擎直接刪除 *.frm 。

![innodb files](/images/databases/mysql/innodb-files.png){: .pull-center }

各個文件的詳細介紹如下：

* ibdata1，系統表空間<br>包括了 Undo log、Double Write Buffer、Insert Buffer 等，可以通過 innodb_data_file_path 變量查看文件名稱和大小。

* ib_logfile0/1，redo-log日誌<br>InnoDB 的 redo-log ，順序寫入，循環利用。

* db-name/，與數據庫名稱相同的目錄<br>除了 information_schema 之外，每個數據庫都有一個相同名稱的目錄，而 information_schema 實際上是一個視圖，提供了訪問元數據的一個接口，因此沒有保存與之相關的文件。

* db-name/db.opt，文本文件<br>每個目錄下都會保存一個 db.opt 文本文件，用來保存該庫的默認字符集編碼和字符集排序規則。在創建表時，如果沒有指定字符集和排序規則，那麼該新建的表將採用 db.opt 文件中指定的屬性。

* db-name/table-name.frm，MySQL 表的元數據，與 InnoDB 無關<br>同時每個數據庫對應的目錄下都會保存一個 table-name.frm，與存儲引擎無關，用來保存表的元數據。

* db-name/table-name.idb<br>對於 innodb，如果採用共享表空間，則索引、數據等信息會保存在 ibdataN 中；如果採用獨立表空間則會保存在目錄下 table-name.idb。

* db-name/{table-name.MYD,table-name.MYI}<br>.MYD 文件用來存儲 MyISAM 的數據，.MYI 用來存儲索引相關的信息。

對於表的存儲格式，實際上 Google 的 [Jeremy Cole](https://blog.jcole.us) 對此介紹的已經很詳細，可以直接參考其 blog，後面也會有對這方面的解析，僅記錄個人對此的理解。


## 狀態查看

對於 InnoDB 狀態的查看，有如下兩種方法。

### show engine

通過如下命令可以查看當前 InnoDB 的狀態。

{% highlight text %}
mysql> SHOW ENGINE INNODB STATUS\G
*************************** 1. row ***************************
  Type: InnoDB
  Name:
Status:
=====================================
2016-12-11 09:07:11 7fc8552f4b00 INNODB MONITOR OUTPUT
=====================================
Per second averages calculated from the last 3 seconds
-----------------
BACKGROUND THREAD
-----------------
srv_master_thread loops: 1 srv_active, 0 srv_shutdown, 78844 srv_idle
srv_master_thread log flush and writes: 78845
----------
SEMAPHORES
----------
----------
MUTEX INFO
----------
Locked mutex: addr 0x7fc8586c6a40 thread 140498399349504 file handler/ha_innodb.cc line 15729
Locked mutex: addr 0x7fc8586c69c0 thread 140498399349504 file srv/srv0srv.cc line 1397
Total number of mutexes 8370
-------------
RW-LATCH INFO
-------------
Total number of rw-locks 16434
OS WAIT ARRAY INFO: reservation count 5
OS WAIT ARRAY INFO: signal count 5
Mutex spin waits 975, rounds 1472, OS waits 2
RW-shared spins 2, rounds 60, OS waits 2
RW-excl spins 0, rounds 0, OS waits 0
Spin rounds per wait: 1.51 mutex, 30.00 RW-shared, 0.00 RW-excl
------------
TRANSACTIONS
------------
Trx id counter 95236
Purge done for trx's n:o < 94832 undo n:o < 0 state: running but idle
History list length 545
Total number of lock structs in row lock hash table 0
LIST OF TRANSACTIONS FOR EACH SESSION:
---TRANSACTION 0, not started
MySQL thread id 53, OS thread handle 0x7fc8552f4b00, query id 600031 localhost 127.0.0.1 root init
SHOW ENGINE INNODB STATUS
---TRANSACTION 95235, not started
MySQL thread id 1, OS thread handle 0x7fc856e58b00, query id 0 Waiting for background binlog tasks
--------
FILE I/O
--------
I/O thread 0 state: waiting for i/o request (insert buffer thread)
I/O thread 1 state: waiting for i/o request (log thread)
I/O thread 2 state: waiting for i/o request (read thread)
I/O thread 3 state: waiting for i/o request (read thread)
I/O thread 4 state: waiting for i/o request (read thread)
I/O thread 5 state: waiting for i/o request (read thread)
I/O thread 6 state: waiting for i/o request (write thread)
I/O thread 7 state: waiting for i/o request (write thread)
I/O thread 8 state: waiting for i/o request (write thread)
I/O thread 9 state: waiting for i/o request (write thread)
Pending normal aio reads: 0 [0, 0, 0, 0] , aio writes: 0 [0, 0, 0, 0] ,
 ibuf aio reads: 0, log i/o's: 0, sync i/o's: 0
Pending flushes (fsync) log: 0; buffer pool: 0
357 OS file reads, 5 OS file writes, 5 OS fsyncs
0.00 reads/s, 0 avg bytes/read, 0.00 writes/s, 0.00 fsyncs/s
-------------------------------------
INSERT BUFFER AND ADAPTIVE HASH INDEX
-------------------------------------
Ibuf: size 1, free list len 0, seg size 2, 0 merges
merged operations:
 insert 0, delete mark 0, delete 0
discarded operations:
 insert 0, delete mark 0, delete 0
0.00 hash searches/s, 0.00 non-hash searches/s
---
LOG
---
Log sequence number 47392436
Log flushed up to   47392436
Pages flushed up to 47392436
Last checkpoint at  47392436
Max checkpoint age    80826164
Checkpoint age target 78300347
Modified age          0
Checkpoint age        0
0 pending log writes, 0 pending chkp writes
8 log i/o's done, 0.00 log i/o's/second
----------------------
BUFFER POOL AND MEMORY
----------------------
Total memory allocated 141033472; in additional pool allocated 0
Total memory allocated by read views 88
Internal hash tables (constant factor + variable factor)
    Adaptive hash index 2233968         (2213368 + 20600)
    Page hash           139112 (buffer pool 0 only)
    Dictionary cache    618319  (554768 + 63551)
    File system         819088  (812272 + 6816)
    Lock system         333624  (332872 + 752)
    Recovery system     0       (0 + 0)
Dictionary memory allocated 63551
Buffer pool size        8191
Buffer pool size, bytes 134201344
Free buffers            7856
Database pages          334
Old database pages      0
Modified db pages       0
Percent of dirty pages(LRU & free pages): 0.000
Max dirty pages percent: 75.000
Pending reads 0
Pending writes: LRU 0, flush list 0, single page 0
Pages made young 0, not young 0
0.00 youngs/s, 0.00 non-youngs/s
Pages read 334, created 0, written 1
0.00 reads/s, 0.00 creates/s, 0.00 writes/s
No buffer pool page gets since the last printout
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 334, unzip_LRU len: 0
I/O sum[0]:cur[0], unzip sum[0]:cur[0]
--------------
ROW OPERATIONS
--------------
0 queries inside InnoDB, 0 queries in queue
0 read views open inside InnoDB
0 RW transactions active inside InnoDB
0 RO transactions active inside InnoDB
0 out of 1000 descriptors used
Main thread process no. 6432, id 140497645459200, state: sleeping
Number of rows inserted 0, updated 0, deleted 0, read 0
0.00 inserts/s, 0.00 updates/s, 0.00 deletes/s, 0.00 reads/s
Number of system rows inserted 0, updated 0, deleted 0, read 0
0.00 inserts/s, 0.00 updates/s, 0.00 deletes/s, 0.00 reads/s
----------------------------
END OF INNODB MONITOR OUTPUT
============================

1 row in set (0.02 sec)
{% endhighlight %}

<!--
### TRANSACTIONS

{% highlight text %}
------------
TRANSACTIONS
------------
Trx id counter 95236
Purge done for trx's n:o < 94832 undo n:o < 0 state: running but idle
History list length 545
Total number of lock structs in row lock hash table 0
LIST OF TRANSACTIONS FOR EACH SESSION:
---TRANSACTION 0, not started
MySQL thread id 53, OS thread handle 0x7fc8552f4b00, query id 600031 localhost 127.0.0.1 root init
SHOW ENGINE INNODB STATUS
---TRANSACTION 95235, not started
MySQL thread id 1, OS thread handle 0x7fc856e58b00, query id 0 Waiting for background binlog tasks
{% endhighlight %}

{% highlight text %}
----------------------
BUFFER POOL AND MEMORY
----------------------
Total memory allocated 141033472; in additional pool allocated 0
Total memory allocated by read views 88
Internal hash tables (constant factor + variable factor)
    Adaptive hash index 2233968         (2213368 + 20600)
    Page hash           139112 (buffer pool 0 only)
    Dictionary cache    618319  (554768 + 63551)
    File system         819088  (812272 + 6816)
    Lock system         333624  (332872 + 752)
    Recovery system     0       (0 + 0)
Dictionary memory allocated 63551
Buffer pool size        8191                 # 總頁數
Buffer pool size, bytes 134201344            # 總字節數=總頁數*頁大小
Free buffers            7856
Database pages          334
Old database pages      0
Modified db pages       0
Percent of dirty pages(LRU & free pages): 0.000
Max dirty pages percent: 75.000
Pending reads 0
Pending writes: LRU 0, flush list 0, single page 0
Pages made young 0, not young 0
0.00 youngs/s, 0.00 non-youngs/s
Pages read 334, created 0, written 1
0.00 reads/s, 0.00 creates/s, 0.00 writes/s
No buffer pool page gets since the last printout
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 334, unzip_LRU len: 0
I/O sum[0]:cur[0], unzip sum[0]:cur[0]
{% endhighlight %}

對於 ibdata1 文件各個文件所佔 page 數量，可以通過 innodb_page_info.py ibdata1 查看。
-->

#### 源碼實現

上述命令會調用存儲引擎中定義的 show_status() 接口，對於 InnoDB 來說，調用邏輯如下。

{% highlight cpp %}
// 初始化為相應的函數
static int innobase_init(void *p)
{
    ... ...
    innobase_hton->show_status = innobase_show_status;
    ... ...
}
{% endhighlight %}

然後，其調用流程如下，也就是最終調用的是 srv_printf_innodb_monitor() 函數。

{% highlight text %}
innobase_show_status()               ← handler/ha_innodb.cc
  |-innodb_show_status()
    |-srv_printf_innodb_monitor()    ← 實際打印函數入口，srv/srv0srv.cc
      |-lock_print_info_summary()    ← 打印鎖相關信息
      |-log_print()                  ← LOG，redo日誌相關
{% endhighlight %}



### innodb_metrics

從 MySQL 5.6 開始，引入了 ```information_schema.innodb_metrics``` 表，包含了比 ```show global status``` 更詳細的內容，而且相比 ```performance_schema``` 更輕量級。

八卦下，據說 ```innodb_metrics``` 表是在 Oracle-Sun 談判的時候，所以就只實現了這一個表 ^_^

該表中包括了，頁的分裂和合並、Purge 的性能、Adaptive Hash Index 活動、頁的刷新、日誌刷新、Index Condition Pushdown(ICP) 等等；監控那些指標可以分別控制。

InnoDB 中提供瞭如下的變量，可以對錶內的參數進行設置。

{% highlight text %}
----- 查看可以使用的變量
mysql> SHOW GLOBAL VARIABLES LIKE 'innodb_monitor_%';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| innodb_monitor_disable   |       |
| innodb_monitor_enable    |       |
| innodb_monitor_reset     |       |
| innodb_monitor_reset_all |       |
+--------------------------+-------+
4 rows in set (0.00 sec)

----- 查看當前的監控指標
mysql> SELECT name,subsystem,status,type,comment FROM information_schema.innodb_metrics;

----- 開啟一個指標項
mysql> SET GLOBAL innodb_monitor_enable='buffer_pool_reads';
----- 關閉一個指標項
mysql> SET GLOBAL innodb_monitor_disable='buffer_pool_reads';
----- 重置參數，只重置XXX_RESET列參數
mysql> SET GLOBAL innodb_monitor_reset='buffer_pool_reads';
----- 重置所有參數，會重置所有參數
mysql> SET GLOBAL innodb_monitor_reset_all='buffer_pool_reads';
----- 也可以使用通配符
mysql> SET GLOBAL innodb_monitor_enable='buffer_pool_%';
{% endhighlight %}

可以參考 [Reference Manual](https://dev.mysql.com/doc/refman/en/innodb-metrics-table.html) 以及 [Get started with InnoDB Metrics Table](https://blogs.oracle.com/mysqlinnodb/entry/get_started_with_innodb_metrics) 。

#### 源碼解析

監控的源碼實現在 ```storage/innobase/srv/srv0mon.cc``` 文件中，通過如下變量進行統計。

{% highlight cpp %}
static monitor_info_t   innodb_counter_info[] =
{
    /* A dummy item to mark the module start, this is
    to accomodate the default value (0) set for the
    global variables with the control system. */
    {"module_start", "module_start", "module_start",
    MONITOR_MODULE,
    MONITOR_DEFAULT_START, MONITOR_DEFAULT_START},
    ... ...
    {"buffer_flush_background_pages", "buffer",
     "Pages queued as a background batch",
     MONITOR_SET_MEMBER, MONITOR_FLUSH_BACKGROUND_TOTAL_PAGE,
     MONITOR_FLUSH_BACKGROUND_PAGES},
    ... ...
};
{% endhighlight %}

計數器在那裡增加，可以通過最後的一個宏定義查看，如上述 ```MONITOR_FLUSH_BACKGROUND_PAGES``` 。


### 其它

在 MySQL 中設置與 InnoDB 相關的變量時，實際對應到源碼後是將變量中的 innodb 替換為了 srv ，例如 innodb_read_ahead_threshold 對應到源碼中是 srv_read_ahead_threshold 。



{% highlight text %}
{% endhighlight %}
