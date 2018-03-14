---
title: MySQL 監控指標
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,monitor,監控
description: 為了排查問題，對數據庫的監控是必不可少的，在此介紹下 MySQL 中的常用監控指標。
---

為了排查問題，對數據庫的監控是必不可少的，在此介紹下 MySQL 中的常用監控指標。

<!-- more -->

![Monitor Logo]({{ site.url }}/images/databases/mysql/monitor-logo.png "Monitor Logo"){: .pull-center }

## 簡介

MySQL 有多個分支版本，常見的有 MySQL、Percona、MariaDB，各個版本所對應的監控項也會有些區別，在此僅介紹一些通用的監控項。

通常，監控項的源碼是在 mysql/mysqld.cc 文件中定義，其內容如下所示。

{% highlight c %}
SHOW_VAR status_vars[]= {
    {"Aborted_clients",          (char*) &aborted_threads,        SHOW_LONG},
    {"Aborted_connects",         (char*) &aborted_connects,       SHOW_LONG},
    {"Acl",                      (char*) acl_statistics,          SHOW_ARRAY},
    // ... ...
};
{% endhighlight %}


## 監控

對於數據庫，通常可以主動監控以下四個與性能及資源利用率相關的指標：

* 查詢吞吐量
* 查詢執行性能
* 連接情況
* 緩衝池使用情況

### 吞吐量

在 MySQL 中有各種針對不同命令的統計，其監控項指標以 Com_XXX 方式命名，其中比較常用的統計項包括了 TPS/QPS。

而 MySQL 與 QPS 相關的包括了三個監控項，分別為 Queries、Questions、Com_select，一般會採用 Com_select 作為採集項；對於 TPS，一般認為是 Com_insert + Com_update + Com_delete 三個統計項的和。


#### Queries 和 Questioins 區別

如下是 [Server Status Variables](http://dev.mysql.com/doc/refman/en/server-status-variables.html) 中對這兩個值的介紹。

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

也就是說，如果不使用 prepared statements ，那麼兩者的區別是 Questions 會將存儲過程作為一個語句；而 Queries 會統計存儲過程中的各個執行的語句。


{% highlight text %}
mysql> SHOW STATUS LIKE "questions";        ← 查看本連接執行的查詢SQL數
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

上述的 questions 是會話級別的，當然可以通過 global 查看全局的變量。

另外，MySQL 會在執行 SQL 之前開始增加上述的統計計數；而當前正在執行的 SQL 數量則可以通過 threads_running 查看。

#### 總結

{% highlight text %}
Questions        已執行的由客戶端發出的語句
Com_select       執行SELECT語句的數量，通常也就是QPS
Writes           Com_insert+Com_update+Com_delete，也就是TPS
{% endhighlight %}



### 執行性能

目前，有幾種方式可以用來查看 MySQL 的執行性能問題，可以參考如下。

#### performance_schema

在 events_statements_summary_by_digest 表中保存了許多關鍵指標，均以微秒為單位，該表會忽略數值、規範化空格與大小寫。

如果要以微秒為單位查看各個 database 的平均運行時間，或者出現的錯誤語句總數，可以通過如下方式查看：

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

sys 存儲引擎默認在 5.7.7 中添加，對於 5.6 可以手動安裝，詳細可以參考 [github - sys schema](https://github.com/mysql/mysql-sys) 。

#### 慢查詢

MySQL 提供了一個 Slow_queries 計數器，當查詢的執行時間超過 long_query_time 參數指定的值之後，該計數器就會增加，默認設置為 10 秒。

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

需要注意的是，需要關閉會話然後重新連接之後，才能使該參數生效。

#### 總結

{% highlight text %}
Slow_queries         慢查詢的數量
{% endhighlight %}

其它的一些比較適合發現性能問題後用於排查。


### 連接情況

監控客戶端連接情況相當重要，因為一旦可用連接耗盡，新的客戶端連接就會遭到拒絕，MySQL 默認的連接數限制為 151，可通過下面的方法在配置文件中進行設置。

{% highlight text %}
[mysqld]
max_connections = 200
{% endhighlight %}

通過如下方法查詢和臨時設置。

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

通常 Linux 可以處理 500~1000 個連接，如果 RAM 資源足夠，可處理 1W+ 個連接，而 Windows 由於採用 Posix 兼容層，能處理的連接數一般不超過 2048 個。


#### 監控連接使用率

如果採用每個連接一個線程的方式，可以通過 Threads_connected 查看，監控該指標與先前設置的連接限制，可以確保服務器擁有足夠的容量處理新的連接。

另外，通過 Threads_running 指標，可以查看正在處理請求的線程，可以用來判斷那些連接被佔用但是卻沒有處理任何請求。

如果達到 max_connections 就會拒絕新的連接請求，Connection_errors_max_connections 指標就會開始增加，同時，追蹤所有失敗連接嘗試的 Aborted_connects 指標也會開始增加。

另外，通過 Connection_errors_internal 這個指標，可以用來監控來自服務器本身導致的錯誤，例如內存不足。

#### 總結

{% highlight text %}
Threads_connected                   已經建立的連接
Threads_running                     正在運行的連接
Connection_errors_internal          由於服務器內部本身導致的錯誤
Aborted_connects                    嘗試與服務器建立連接但是失敗的次數
Connection_errors_max_connections   由於到達最大連接數導致的錯誤
{% endhighlight %}


### 緩衝池使用情況

InnoDB 使用一片內存區域作為緩衝區，用來緩存數據表與索引數據，緩衝區太小可能會導致數據庫性能下滑，磁盤 I/O 攀升。

默認值一般是 128MiB，建議將其設置為物理內存的 80%；不過需要注意的是，InnoDB 可能會使用超過緩衝池 10%，如果耗盡內存，則會使用分頁，從而使數據庫性能受損。

{% highlight text %}
SHOW GLOBAL VARIABLES LIKE "innodb_buffer_pool_chunk_size";
SHOW GLOBAL VARIABLES LIKE "innodb_buffer_pool_instances";
{% endhighlight %}

如果 innodb_buffer_pool_chunk_size 查詢沒有返回結果，則表示在你使用的 MySQL 版本中此參數無法更改，其值為 128 MiB，實際參數為 innodb_buffer_pool_size 。

在服務器啟動時，你可以這樣設置緩衝池的大小以及實例的數量：

{% highlight text %}
$ mysqld --innodb_buffer_pool_size=8G --innodb_buffer_pool_instances=16
{% endhighlight %}

#### 監控指標

Innodb_buffer_pool_read_requests 記錄了讀取請求的數量，而 Innodb_buffer_pool_reads 記錄了緩衝池無法滿足，因而只能從磁盤讀取的請求數量，也就是說，如果 Innodb_buffer_pool_reads 的值開始增加，意味著數據庫性能大有問題。

緩存的使用率和命中率可以通過如下方法計算：

{% highlight text %}
(Innodb_buffer_pool_pages_total - Innodb_buffer_pool_pages_free) /
    Innodb_buffer_pool_pages_total * 100%

(Innodb_buffer_pool_read_requests - Innodb_buffer_pool_reads) /
    Innodb_buffer_pool_read_requests * 100%
{% endhighlight %}

如果數據庫從磁盤進行大量讀取，而緩衝池還有許多閒置空間，這可能是因為緩存最近才清理過，還處於預熱階段。

#### 總結

{% highlight text %}
Innodb_buffer_pool_pages_total          BP中總頁面數
Buffer pool utilization                 BP中頁面的使用率
Innodb_buffer_pool_read_requests        BP的讀請求
Innodb_buffer_pool_reads                需要讀取磁盤的請求數
{% endhighlight %}



## 響應時間

"響應時間" (Response Time, RT) 在數據庫應用中，尤其是 OLTP 的場景，非常重要，但官方版本中一直沒有加上這個統計功能。開始使用的是 tcpdump+mk-query-digest，再後來 tcprstat，很快 Percona 提供了響應時間統計插件。

對於 MariaDB 也存在類似的方式，可以通過如下的方式安裝、測試。

{% highlight text %}
----- 1. 查看是否存在插件
$ ls /opt/mariadb/lib/plugin | grep response
query_response_time.so

----- 2. 安裝插件
mysql> INSTALL PLUGIN query_response_time_audit SONAME 'query_response_time.so';
Query OK, 0 rows affected (0.00 sec)
mysql> INSTALL PLUGIN query_response_time SONAME 'query_response_time.so';
Query OK, 0 rows affected (0.00 sec)

----- 3. 打開統計功能
mysql> SET GLOBAL query_response_time_stats = 'ON';
Query OK, 0 rows affected (0.00 sec)
mysql> SET GLOBAL query_response_time_flush = 'ON';
Query OK, 0 rows affected (0.00 sec)

----- 4. 查看統計值
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


默認的時間統計區間是按照基數 10 增長的，也就是說默認的區間如下：

{% highlight text %}
(0;10^-6], (10^-6;10^-5], (10^-5;10^-4], ..., (10^-1;10^1], (10^1; 10^2], ...
{% endhighlight %}

可以通過修改參數 query_response_time_range_base 來縮小時間區間，默認該是 10，實際的統計時間區間如上，可以修改為 2，則區間如下：

{% highlight text %}
(0;2^-19], (2^-19;2^-18], (2^-18;2^-17], ..., (2^-1; 2^1], (2^1; 2^2], ...
{% endhighlight %}

第一個區間總是最接近 0.000001 的區間開始；最後區間是到最接近且小於 10000000 處結束。

當然，有些比較從網上摘錄的不錯 SQL，可以根據自己需求修改。

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








## 其它監控項

除了上述的監控項，常見的還有如下常用的方法。

#### 1. 是否可用

可以使用如下幾條命令來查看當前 MySQL 服務是否處於運行狀態。

{% highlight text %}
mysqladmin -h 127.1 -u root -pnew-password ping
mysqladmin -h 127.1 -u root -pnew-password status
{% endhighlight %}

#### 2. 用戶管理

嚴禁對用戶的 "host" 部分採用 "%"，除非你想從世界任何一個地方登陸；默認不要使用 GRANT ALL ON *.* 給用戶過度賦權，

{% highlight text %}
mysql> CREATE USER 'user' IDENTIFIED BY 'password';
mysql> GRANT privileges TO 'user'@'host' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;

mysql> GRANT privileges  TO 'user'@'host' IDENTIFIED BY 'password' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;
{% endhighlight %}


#### 3. 連接數是否正常

主要查看客戶是否有由於沒正確關閉連接而死掉的連接，有多少失敗的連接，是否有惡意連接等。



{% highlight text %}
----- 查看連接數，root會看到所有，其它用戶只能看到自己的連接
mysql> SHOW FULL PROCESSLIST;
$ mysqladmin -h host -u user -p processlist

----- 當前失敗連接數
mysql> SHOW GLOBAL STATUS LIKE 'aborted_connects';

----- 由於客戶沒有正確關閉連接而死掉的連接數
mysql> SHOW GLOBAL STATUS LIKE 'aborted_clients';

----- 最大連接數
mysql> SHOW CLOBAL VARIABLES LIKE 'max_connections';
mysql> SHOW GLOBAL STATUS LIKE 'max_connections';
{% endhighlight %}

#### 4. 慢查詢日誌

慢查詢日誌對 SQL 調優來說是非常重要的，它記錄了超過指定時間 long_query_time 的查詢語句；一般只在需要時開啟。

<!--

#### 4. 全表掃描比例

計算方式如下

((Handler_read_rnd_next + Handler_read_rnd) / (Handler_read_rnd_next + Handler_read_rnd + Handler_read_first + Handler_read_next + Handler_read_key + Handler_read_prev))

對於全表掃描對性能的影響我想大家都比我清楚得多！

我們可以通過
SHOW GLOBAL STATUS LIKE 'Handler_read%'

取得相關參數的值再進行計算。

5 Innodb 死鎖
當遇到死鎖時Innodb會回滾事務。瞭解死鎖何時發生對於追溯其發生的根本原因非常重要。我們必須知道產生了什麼樣的死鎖，相關應用是否正確處理或已採取了相關措施。
可以通過
SHOW ENGINE INNODB STATUS
查看死鎖相關狀況
-->


<!--
<br><h2>innodb_data_read 和 innodb_data_reads</h2><p>
下面是 MySQL 官方文檔裡對於這兩個參數的解釋：
<pre style="font-size:0.8em; face:arial;">
innodb_data_read
The amount of data read since the server was started.

innodb_data_reads
The total number of data reads.
</pre>
接下來我們從源代碼來解析這兩個參數。在src/Srv0srv.c中存在對這兩個參數和代碼變量的映射關係的定義代碼，如下：
     export_vars.innodb_data_read = srv_data_read;
     export_vars.innodb_data_reads = os_n_file_reads;
接著讓我們來看下這兩個代碼變量在哪裡進行了累加操作。
srv_data_read 盡有一處代碼進行累加操作，buf0file.c 的 _fil_io 函數中，操作如下：
　　srv_data_read+= len;
從這行可以看出，innodb_data_read中實際存的是從磁盤上進行物理IO的字節數。

os_n_file_reads 也幾乎只有一處代碼進行累加（其他是windows的sync讀這裡忽略），在buf0file.c 的 中，操作如下：os_file_pread

　　os_n_file_reads++;

隨後，代碼即調用os_aio_array_reserve_slot將IO請求推入 simulated array，再根據wake_later標誌位決定是否調用
os_aio_simulated_wake_handler_thread來立即喚醒IO工作隊列。從這裡也可以看到，innodb_data_reads記錄的是innodb對於磁盤發起的讀IO請求次數。
看到這裡我們就很容易產生一個疑問：既然兩個變量都是對磁盤發起的IO的計數器，為什麼不直接放在一個函數的相鄰行裡呢？

回答這個問題，我們就需要對於innodb的simulated-aio和read-ahead算法有一定理解了。

進入到simulated的aio slot array的請求會有兩種，一種是通過buf_read_page 過來的普通頁的讀請求，一種是通過buf_read_ahead_random/linear 過來的預讀請求。從innodb的實現來說普通數據頁的請求是需要立即返回響應的，所以是同步（sync）IO。而對於預讀這樣數據並非SQL所需要，僅是用於提升性能的頁讀取，這樣的IO完全是可以異步的。這兩個差異也是導致simulated aio出現的原因。把IO請求推入slot array後，數據頁同步請求立即通知worker thread去os做同步IO。而預讀IO請求會不斷的推入slot array直到一次預讀所需要的page全部推入slot中，然後再會通知worker thread。此外在worker thread中，也會判斷一個segment的io請求是否相鄰連續，如果連續則把這些請求合併後，作為一次OS IO發到下層存儲中。

明白了這些也就不難理解為什麼計數器要分開在不同的函數中計數了。如果累加都放在 _fil_io中進行，那麼 innodb_data_read = 16K * innodb_data_reads （這裡假設page沒有做壓縮）。然而在有了異步IO合併這個操作後，實際的innodb_data_reads會少於_fil_io中獲得的計數次數。所以，通過 innodb_data_read / innodb_data_reads得到的比值也可以推斷出一個MySQL實例中順序IO或者可順序預讀的IO比例。

我們在production環境的服務器上做一個驗證：

服務器A：在線用戶訪問的數據庫，猜測隨機IO較多

SHOW GLOBAL STATUS LIKE '%innodb_data_read%';

Innodb_data_read 46384854470656
Innodb_data_reads 2812101578

每次IO平均讀取字節數=16494

服務器B：歷史數據統計的數據庫，數據內容和服務器A完全一樣，猜測順序IO較多

SHOW GLOBAL STATUS LIKE '%innodb_data_read%';

Innodb_data_read 54835669766144
Innodb_data_reads 2604758776

每次IO平均讀取字節數=21052

可見順序IO較多的MySQL的單次IO讀取字節數確實要多於一個page的大小，說明IO合併效果明顯。

而隨機IO較多的MySQL的單詞IO讀取字節數幾乎和一個page大小一致，即16K



最後我們再總結下一些結論：
Innodb_data_read   表示Innodb啟動後，從物理磁盤上讀取的字節數總和。
Innodb_data_reads 表示Innodb啟動後，隊伍物理磁盤發起的IO請求次數總和。
Innodb_data_read / Innodb_data_reads 得到的比值，越接近16K說明IO壓力越傾向於隨機IO，越遠離16K說明IO從順序預讀中獲得性能提升越多
http://www.cnblogs.com/cenalulu/archive/2013/10/16/3370626.html
</p>




 6. innodb緩衝池狀態

 innodb_buffer_pool_reads: 平均每秒從物理磁盤讀取頁的次數

 innodb_buffer_pool_read_requests: 平均每秒從innodb緩衝池的讀次數（邏輯讀請求數）

 innodb_buffer_pool_write_requests: 平均每秒向innodb緩衝池的寫次數

 innodb_buffer_pool_pages_dirty: 平均每秒innodb緩存池中髒頁的數目

 innodb_buffer_pool_pages_flushed: 平均每秒innodb緩存池中刷新頁請求的數目

 innodb緩衝池的讀命中率
 innodb_buffer_read_hit_ratio = ( 1 - Innodb_buffer_pool_reads/Innodb_buffer_pool_read_requests) * 100

 Innodb緩衝池的利用率

 Innodb_buffer_usage =  ( 1 - Innodb_buffer_pool_pages_free / Innodb_buffer_pool_pages_total) * 100

 7. innodb日誌

 innodb_os_log_fsyncs: 平均每秒向日志文件完成的fsync()寫數量

 innodb_os_log_written: 平均每秒寫入日誌文件的字節數

 innodb_log_writes: 平均每秒向日志文件的物理寫次數

 innodb_log_write_requests: 平均每秒日誌寫請求數

 8. innodb行

 innodb_rows_deleted: 平均每秒從innodb表刪除的行數

 innodb_rows_inserted: 平均每秒從innodb表插入的行數

 innodb_rows_read: 平均每秒從innodb表讀取的行數

 innodb_rows_updated: 平均每秒從innodb表更新的行數

 innodb_row_lock_waits:  一行鎖定必須等待的時間數

 innodb_row_lock_time: 行鎖定花費的總時間，單位毫秒

 innodb_row_lock_time_avg: 行鎖定的平均時間，單位毫秒

 9. MyISAM讀寫次數

 key_read_requests: MyISAM平均每秒鐘從緩衝池中的讀取次數

 Key_write_requests: MyISAM平均每秒鐘從緩衝池中的寫入次數

 key_reads : MyISAM平均每秒鐘從硬盤上讀取的次數

 key_writes : MyISAM平均每秒鐘從硬盤上寫入的次數

 10. MyISAM緩衝池

 MyISAM平均每秒key buffer利用率

 Key_usage_ratio =Key_blocks_used/(Key_blocks_used+Key_blocks_unused)*100

 MyISAM平均每秒key buffer讀命中率

 Key_read_hit_ratio=(1-Key_reads/Key_read_requests)*100

 MyISAM平均每秒key buffer寫命中率

 Key_write_hit_ratio =(1-Key_writes/Key_write_requests)*100

 11. 其他

 slow_queries: 執行時間超過long_query_time秒的查詢的個數（重要）

 sort_rows: 已經排序的行數

 open_files: 打開的文件的數目

 open_tables: 當前打開的表的數量

 select_scan: 對第一個表進行完全掃描的聯接的數量

 此外，還有一些性能指標不能通過mysqladmin extended-status或show global status直接得到，但是十分重要。

 12. response time: 響應時間

 Percona提供了 tcprstat工具 統計響應時間，此功能默認是關閉的，可以通過設置參數query_response_time_stats=1打開這個功能。

 有兩種方法查看響應時間：

 （1）通過命令SHOW QUERY_RESPONSE_TIME查看響應時間統計；

 （2）通過INFORMATION_SCHEMA裡面的表QUERY_RESPONSE_TIME來查看。

 http://www.orczhou.com/index.php/2011/09/thanks-percona-response-time-distribution/comment-page-1/ （參考文章）



運行中的mysql狀態查看

對正在運行的mysql進行監控，其中一個方式就是查看mysql運行狀態。

(1)QPS(每秒Query量)
QPS = Questions(or Queries) / seconds
mysql > show  global  status like 'Question%';

(2)TPS(每秒事務量)
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

(6)Table Cache狀態量
mysql> show global  status like 'open%';
比較 open_tables  與 opend_tables 值

(7)Thread Cache 命中率
mysql> show global status like 'Thread%';
mysql> show global status like 'Connections';
Thread_cache_hits = (1 - Threads_created / connections ) * 100%

(8)鎖定狀態
mysql> show global  status like '%lock%';
Table_locks_waited/Table_locks_immediate=0.3%  如果這個比值比較大的話，說明表鎖造成的阻塞比較嚴重
Innodb_row_lock_waits innodb行鎖，太大可能是間隙鎖造成的

(10) Tmp Table 狀況(臨時表狀況)
mysql > show status like 'Create_tmp%';
Created_tmp_disk_tables/Created_tmp_tables比值最好不要超過10%，如果Created_tmp_tables值比較大，
可能是排序句子過多或者是連接句子不夠優化

(11) Binlog Cache 使用狀況
mysql > show status like 'Binlog_cache%';
如果Binlog_cache_disk_use值不為0 ，可能需要調大 binlog_cache_size大小

{% highlight text %}

(12) Innodb_log_waits 量
Innodb_log_waits值不等於0的話，表明 innodb log  buffer 因為空間不足而等待
mysql > show status like 'innodb_log_waits';
{% endhighlight %}

 4. innodb文件讀寫次數
 innodb_data_reads：innodb平均每秒從文件中讀取的次數
 innodb_data_writes：innodb平均每秒從文件中寫入的次數
 innodb_data_fsyncs：innodb平均每秒進行fsync()操作的次數


 5. innodb讀寫量
 innodb_data_read：innodb平均每秒鐘讀取的數據量，單位為KB
 innodb_data_written：innodb平均每秒鐘寫入的數據量，單位為KB

-->


## MyISAM

在對 MyISAM 存儲引擎調優時，很多文章推薦使用 Key_read_requests 和 Key_reads 的比例作為調優的參考，來設置 key_buffer_size 參數的值，而實際上這是錯誤的，詳細可以參考 [Why you should ignore MySQL's key cache hit ratio](https://www.percona.com/blog/2010/02/28/why-you-should-ignore-mysqls-key-cache-hit-ratio/) 這篇文章。 <!-- ' -->

簡單介紹如下。

### rate VS. ratio

首先需要注意的是，這裡有兩個重要的概念："miss rate"  一般是每秒 miss 的數目；"miss ratio" 表示從磁盤讀取和從 cache 讀取的比例，該參數沒有單位。

如下的兩個參數可以通過  SHOW GLOBAL STATUS 命令查看，官方文檔的解釋如下。

{% highlight text %}
Key_read_requests
The number of requests to read a key block from the cache.

Key_reads
The number of physical reads of a key block from disk.
{% endhighlight %}
也就是說，兩者分別對應了：A) 從緩存讀取索引的請求次數；B) 從磁盤讀取索引的請求次數。

> NOTE: 實際上，Key_reads 統計的並非嚴格意義上的讀磁盤，嚴格來說應該是發送系統請求的次數。如果文件系統中有緩存的話，實際耗時就是系統調用，並沒有磁盤讀取的耗時。

很多人認為 Key_reads/Key_read_requests 越小越好，否則就應該增大 key_buffer_size 的設置，但通過計數器的比例來調優有兩個問題：

1. 比例並不顯示數量的絕對值大小，並不知道總共的請求有多少；
2. 計數器並沒有考慮時間因素。

假設有兩臺機器，其 miss ratio 分別為 23% 和 0.1% ，因為沒有讀的請求量，很難判斷那臺機器需要進行調優。比如，前者是 23/100，後者則是 10K/10M 。

### 參數指標

雖說 Key_read_requests 大比小好，但是對於系統調優而言，更有意義的應該是單位時間內的 Key_reads，通常可以通過 Key_reads / Uptime 計算；該參數可以通過如下命令得到：

{% highlight text %}
$ mysqladmin ext -ri10 | grep Key_reads
{% endhighlight %}

其中第一行表示的是系統啟動後的總請求，在此可以忽略，下面的每行數值都表示 10 秒內的數據變化，這樣就可以大致評估每秒有多少的磁盤請求，而且可以根據你的磁盤性能進行評估是否合理。

> NOTE: 命令裡的 mysqladmin ext 其實就是 mysqladmin extended-status，你甚至可以簡寫成 mysqladmin e 。

### 結論

通過 Key_reads / Uptime 替換 Key_reads / Key_read_requests 。


<!--

## 緩存監控

MySQL 的服務器級別只提供了 query cache 緩存，對於不同的存儲引擎，分別引入了不同的緩存機制，例如：MyISAM -- key cache；InnoDB -- buffer pool 。


什麼是query cache

Mysql沒有shared_pool緩存執行計劃，但是提供了query cache緩存sql執行結果和文本，如果在生命週期內完全相同的sql再次運行，則連sql解析都免去了；

所謂完全相同，包含如下條件

Sql的大小寫必須完全一樣；

發起sql的客戶端必須使用同樣的字符集和通信協議；

sql查詢同一數據庫下的同一個表(不同數據庫可能有同名表)；

Sql查詢結果必須確定，即不能帶有now()等函數；

當查詢表發生DML或DDL，其緩存即失效；

針對mysql/information_schema/performance_schema的查詢不緩存；

使用臨時表的sql也不能緩存；



開啟緩存後，每個select先檢查是否有可用緩存(必須對這些表有select權限)，而每個寫入操作先執行查詢語句並使相關緩存失效；

5.5起可緩存基於視圖的查詢



Mysql維護一個hash表用來查找緩存，其key為sql text，數據庫名以及客戶端協議的版本等

相應參數

Have_query_cache：服務器是否支持查詢緩存

Query_cache_type：0(OFF)不緩存；1(ON)緩存查詢但不包括使用SQL_NO_CACHE的sql；2(DEMAND)只緩存使用SQL_CACHE的sql

Query_cache_size：字節為單位，即使query_cache_type=0也會為分配該內存，所以應該一併設置為0

Query_cache_limit：允許緩存的最大結果集，大於此的sql不予緩存

Query_cache_min_res_limit：用於限定塊的最小尺寸，默認4K；



緩存的metadata佔有40K內存，其可分為大小不等的多個子塊，各塊之間使用雙向鏈表鏈接；根據其功能分別存儲查詢結果，基表和sql text等；

每個sql至少用到兩個塊：分別存儲sql文本和查詢結果，查詢引用到的表各佔一個塊；

為了減少響應時間，每產生1行數據就發送給客戶端；

數據庫啟動時調用malloc()分配查詢緩存



查詢緩存擁有一個全局鎖，一旦有會話獲取就會阻塞其他訪問緩存的會話，因此當緩存大量sql時，緩存invalidation可能會消耗較長時間；



Innodb也可以使用查詢緩存，每個表在數據字典中都有一個事務ID計數器，ID小於此值的事務不可使用緩存；表如果有鎖(任何鎖)則也不可使用查詢緩存；



 狀態變量

有關query cache的狀態變量都以Qcache打頭

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

Qcache_inserts—被加到緩存中query數目

Qcache_queries_in_cache—註冊到緩存中的query數目

緩存每被命中一次，Qcache_hits就加1；

計算緩存query的平均大小=(query_cache_size-Qcache_free_memory)/Qcache_queries_in_cache

Com_select = Qcache_not_cached + Qcache_inserts + queries with errors found during the column-privileges check

Select = Qcache_hits + queries with errors found by parser





Buffer pool

innodb即緩存表又緩存索引，還有設置多個緩衝池以增加併發，很像oracle

採用LRU算法：

所有buffer塊位於同一列表，其中後3/8為old，每當新讀入一個數據塊時，先從隊尾移除同等塊數然後插入到old子列的頭部，如再次訪問該塊則將其移至new子列的頭部

Innodb_buffer_pool_size:  buffer pool大小

Innodb_buffer_pool_instances: 子buffer pool數量，buffer pool至少為1G時才能生效

Innodb_old_blocks_pct: 範圍5 – 95， 默認為37即3/8，指定old子列的比重

Innodb_old_blocks_time: 以ms為單位，新插入old子列的buffer塊必須等待指定時間後才能移入new列，適用於one-time scan頻繁的操作，以避免經常訪問的數據塊被剔出buffer pool



可通過狀態變量獲知當前buffer pool的運行信息

Innodb_buffer_pool_pages_total：緩存池總頁數

Innodb_buffer_pool_bytes_data：當前buffer pool緩存的數據大小，包括髒數據

Innodb_buffer_pool_pages_data：緩存數據的頁數量

Innodb_buffer_pool_bytes_dirty：緩存的髒數據大小

Innodb_buffer_pool_pages_diry：緩存髒數據頁數量

Innodb_buffer_pool_pages_flush：刷新頁請求數量

Innodb_buffer_pool_pages_free：空閒頁數量

Innodb_buffer_pool_pages_latched：緩存中被latch的頁數量，這些頁此刻正在被讀或寫；然而計算此變量比較消耗資源，只有在UNIV_DEBUG被定義了才可用

相關源代碼如下

#ifdef UNIV_DEBUG
  {"buffer_pool_pages_latched",
  (char*) &export_vars.innodb_buffer_pool_pages_latched,  SHOW_LONG},
#endif /* UNIV_DEBUG */



Innodb_buffer_pool_pages_misc：用於維護諸如行級鎖或自適應hash索引的內存頁=總頁數-空閒頁-使用的頁數量

Innodb_buffer_pool_read_ahead：預讀入緩存的頁數量

Innodb_buffer_pool_read_ahead_evicted：預讀入但是1次都沒用就被剔出緩存的頁

Innodb_buffer_pool_read_requests：InnoDB的邏輯讀請求次數

Innodb_buffer_pool_reads：直接從磁盤讀取數據的邏輯讀次數

Innodb_buffer_pool_wait_free：緩存中沒有空閒頁滿足當前請求，必須等待部分頁回收或刷新，記錄等待次數

Innodb_buffer_pool_write_requests：向緩存的寫數量





可使用innodb standard monitor監控buffer pool的使用情況，主要有如下指標：

Old database pages: old子列中的頁數

Pages made young, not young: 從old子列移到new子列的頁數，old子列中沒有被再次訪問的頁數

Youngs/s  non-youngs/s: 訪問old並導致其移到new列的次數







Key cache

5.5僅支持一個結構化變量，即key cache，其包含4個部件

Key_buffer_size

Key_cache_block_size：單個塊大小，默認1k

Key_cache_division_limit：warm子列的百分比(默認100)，key cache buffer列表的分隔點，用於分隔host和warm子列表

Key_cache_age_threshold：頁在hot子列中的生命週期，值越小則越快的移至warm列表



MyISAM只緩存索引，

可創建多個key buffer—set global hot_cache.key_buffer_size=128*1024

索引指定key buffer—cache index t1 in hot_cache

可在數據庫啟動時load index into key_buffer提前加載緩存，也可通過配置文件自動把索引映射到key cache



key_buffer_size = 4G

hot_cache.key_buffer_size = 2G

cold_cache.key_buffer_size = 2G

init_file=/path/to/data-directory/mysqld_init.sql

mysqld_init.sql內容如下

CACHE INDEX db1.t1, db1.t2, db2.t3 IN hot_cache

CACHE INDEX db1.t4, db2.t5, db2.t6 IN cold_cache



默認採用LRU算法，也支持名為中間點插入機制midpoint insertion strategy

索引頁剛讀入key cache時，被放在warm列的尾部，被訪問3次後則移到hot列尾並循環移動，如果在hot列頭閒置連續N次都沒訪問到，則會被移到warm列頭，成為被剔出cache的首選；

N= block no* key_cache_age_threshold/100
-->



## xtools


{% highlight text %}
----- 標準數據統計，主要是命令次數的統計，transaction
    ins  Com_insert                               (diff)
    upd  Com_update                               (diff)
    del  Com_delete                               (diff)
    sel  Com_select                               (diff)
    tps  Com_insert + Com_update + Com_delete     (diff)

----- 線程處理，threads
    run  Threads_running
    con  Threads_connected
    cre  Threads_created                          (diff)
    cac  Threads_cached

----- 網絡字節數，bytes
   recv  Bytes_received                           (diff)
   sent  Bytes_sent                               (diff)

----- buffer pool的緩存命中率
         Innodb_buffer_pool_read_requests         邏輯讀總次數
         Innodb_buffer_pool_reads                 物理讀總次數
   read  Innodb_buffer_pool_read_requests         每秒讀請求(diff)
   hits  (Innodb_buffer_pool_read_requests-Innodb_buffer_pool_reads)/Innodb_buffer_pool_read_requests

----- buffer pool頁的狀態，innodb bp pages status
   data  Innodb_buffer_pool_pages_data            已經使用緩存頁數
   free  Innodb_buffer_pool_pages_free            空閒緩存頁數
  dirty  Innodb_buffer_pool_pages_dirty           髒頁數目
  flush  Innodb_buffer_pool_pages_flushed         每秒刷新頁數(diff)

----- innoDB相關的操作，innodb rows status
    ins  Innodb_rows_inserted                     (diff)
    upd  Innodb_rows_updated                      (diff)
    del  Innodb_rows_deleted                      (diff)
   read  Innodb_rows_read                         (diff)

----- 數據讀寫請求數，innodb data status
  reads  Innodb_data_reads                        數據讀總次數(diff)
 writes  Innodb_data_writes                       數據寫的總次數(diff)
   read  Innodb_data_read                         至此已經讀的數據量(diff)
written  Innodb_data_written                      至此已經寫的數據量(diff)

----- 日誌寫入磁盤請求，innodb log
 fsyncs  Innodb_os_log_fsyncs                     向日志文件寫的總次數(diff)
written  Innodb_os_log_written                    寫入日誌文件的字節數(diff)
{% endhighlight %}

<!--
$mysql_headline1 .= "  his --log(byte)--  read ---query--- ";
$mysql_headline2 .= " list uflush  uckpt  view inside  que|";
-->


## 參考

可以參考官方文檔 [Reference Manual - Server Status Variables](http://dev.mysql.com/doc/refman/en/server-status-variables.html)，主要介紹各個監控項的含義。

[Monitoring MySQL performance metrics](https://www.datadoghq.com/blog/monitoring-mysql-performance-metrics/)，一篇很不錯的介紹 MySQL 監控項文章，包括上述的吞吐量、執行性能、鏈接情況、緩衝池使用情況等。

[Why you should ignore MySQL's key cache hit ratio](https://www.percona.com/blog/2010/02/28/why-you-should-ignore-mysqls-key-cache-hit-ratio/) 這篇文章介紹了 MyISAM 緩存的調優，其中的思想其它參數也可以考慮，也可以參考 [本地文檔](/reference/mysql/Why you should ignore MySQL's key cache hit ratio.mht) 。

{% highlight text %}
{% endhighlight %}
