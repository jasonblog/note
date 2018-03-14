---
title: MySQL 主備數據校驗
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,主備複製,一致性校驗
description: 由於各種原因，MySQL 主從架構可能會出現數據不一致的情況出現，為此需要對主備複製的數據進行校驗。在此，簡單介紹 Percona-Toolkits 提供的數據校驗方式。
---

由於各種原因，MySQL 主從架構可能會出現數據不一致的情況出現，為此需要對主備複製的數據進行校驗。

在此，簡單介紹 Percona-Toolkits 提供的數據校驗方式。

<!-- more -->

## 簡介

由於各種原因，MySQL 主從架構可能會出現數據不一致的情況出現，大致歸結為如下幾類：

* 主備方式，且在備庫寫數據；
* 執行了 non-deterministic query，如用戶自定義函數、rand() 等；
* 回滾摻雜事務表和非事務表的事務；
* binlog 或者 relaylog 數據損壞。

在主庫執行基於 statement 的 SQL 語句生成主庫數據塊的 checksum，把相同的 SQL 語句傳遞到從庫，並在從庫上計算相同數據塊的 checksum，最後，比較主從庫上相同數據塊的 checksum 值，由此判斷主從數據是否一致。

這種校驗是分表進行的，在每個表內部又是分塊進行的，而且提供了非常多的限流選項，因此對線上服務的衝擊較小。

### 環境搭建

在 CentOS 中可以通過如下方式安裝依賴 RPM 包。

{% highlight text %}
# rpm -ivh perl-DBD-MySQL perl-TermReadKey
# rpm -ivh percona-toolkit-x.x.x.el7.x86_64.rpm
{% endhighlight %}

三個包分別為 MySQL 驅動、交互密碼輸入、主備數據校驗工具。如果是雙主配置，只需要在一個節點部署即可。如果要測試，可以通過 ```PTDEBUG=1 pt-table-checksum > /tmp/checksum.log 2>&1``` 測試。

可以通過 ```pt-table-checksum --help``` 查看幫助信息，或者 ```perldoc /usr/local/bin/pt-table-checksum``` 查看詳細幫助。

### 新建校驗結果保存表

分別在主備新建數據校驗表。

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

### 主機用戶創建

首先需要創建如下用戶。

#### 腳本部署主機

{% highlight sql %}
CREATE USER 'checker' IDENTIFIED BY 'password';
GRANT SUPER, PROCESS, SHOW DATABASES ON *.* TO 'checker'@'localhost';
GRANT DELETE, INSERT, UPDATE ON zabbix.checksums TO 'checker'@'localhost';
{% endhighlight %}

權限配置解析：

* SUPER: SET @@binlog_format:="STATEMENT" 設置會話級的 binlog 格式；
* PROCESS: SHOW GRANTS FOR 查看當前用戶的權限信息；
* SHOW DATABASES: 查看是否存在數據庫；
* DELETE,INSERT,UPDATE: 用於清理、REPLACE INTO、UPDATE checksums 的歷史數據；

#### 另外一臺主機

{% highlight sql %}
CREATE USER 'checker' IDENTIFIED BY 'password';
GRANT PROCESS, SUPER, REPLICATION CLIENT ON *.* TO 'checker'@'IP-MASTER';
GRANT SELECT ON zabbix.* TO 'checker'@'IP-MASTER';
{% endhighlight %}

權限配置解析：

* SUPER, REPLICATION CLIENT: SHOW SLAVE STATUS 查看是否已經同步完成；
* SELECT: EXPLAIN 評估備庫數據量；


### 數據校驗命令

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

### 數據不一致檢查

可以直接通過如下命令查看數據不一致的表，需要在備庫執行。

{% highlight sql %}
SELECT db, tbl, SUM(this_cnt) AS total_rows, COUNT(*) AS chunks
  FROM checksums
  WHERE (
    master_cnt <> this_cnt
    OR master_crc <> this_crc
    OR ISNULL(master_crc) <> ISNULL(this_crc))
  GROUP BY db, tbl;
{% endhighlight %}


## 執行流程

該工具每次只檢測一個表，對於表中的數據會分割成 chunk，一般是 1000 條左右數據(默認會動態調整)；這個系統的調用流程可以闡述如下：

#### 設置超時時間

為了減小對線上事務的影響，在主庫建立鏈接後，會先設置會話變量 ```innodb_lock_wait_timeout=1``` 和 ```wait_timeout=10000```，也可以通過 ```--set-vars wait_timeout=500``` 參數覆蓋默認值。

#### 檢查 sql_mode

通過 ```SELECT @@SQL_MODE``` 命令，確認該變量中不包含 ```ONLY_FULL_GROUP_BY```，因為對於一些正常的函數，該模式仍可能會出錯，因此需要通過 ```SET SQL_MODE``` 命令重新設置。

#### 設置 binlog_mode

查看當前 ```SELECT @@binlog_mode``` 模式，如果不是 STATEMENT，默認會報錯直接退出；當然，可以通過 ```--[no]check-binlog-format``` 參數不檢查該參數。

然後，仍會設置會話變量 ```/*!50108 SET @@binlog_format := 'STATEMENT'*/``` 。

#### 設置隔離級別

設置會話的隔離級別為 ```SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ```，主要因為在將 binog_format 設置為 STATEMENT 後，且隔離級別為 RR，那麼會報如下的錯誤 ```Transaction level 'READ-COMMITTED' in InnoDB is not safe for binlog mode 'STATEMENT'```，當然，這正是我們需要的，所以直接忽略該報錯信息。

#### 查找備庫

通過 ```get_slaves()``` 函數查找備庫，當然可以在啟動時設置不同的備庫查找方式，詳見 ```find_slave_hosts()``` 函數。

#### 拼接 SQL

接下來就是拼接主庫執行的 SQL 語句，主要的格式如下：

{% highlight sql %}
REPLACE INTO checksum_table(db, tbl, chunk, ...)
  SELECT ..., COUNT(*) AS cnt, COALESCE(LOWER(CONV(BIT_XOR(
    CAST(CRC32(CONCAT_WS('#',...)) AS UNSIGNED)), 10, 16)), 0) AS crc
FROM tables FORCE INDEX(`PRIMARY`)
WHERE ((col >= ?)) AND ((col <= ?));
{% endhighlight %}

簡單介紹下拼接上述 SQL 時的注意內容：

* 拼接各個列時，會根據不同的數據類型進行處理，例如以 ```TIMESTAMP``` 類型使用 ```UNIX_TIMESTAMP()``` 函數；可能為 ```NULL``` 時，使用 ```CONCAT(ISNULL())``` 等等；
* 每次執行查詢前會通過 ```EXPLAIN``` 生成執行計劃，主要關注評估數據量、索引使用情況，執行時會強制使用索引；
* 通過 ```WHERE``` 進行分區，也就是 chunk 塊，默認會使執行結果儘量接近 ```--chunk-time``` 參數指定的時間，所以每次 chunk 的大小會動態調整。

#### 等待備庫執行完成

在執行完上述的 SQL 後，需要等待備庫執行完成上述的 SQL 語句，也即是計算備庫的 CRC 值。

#### 更新主庫校驗值

校驗完成後在主庫更新校驗值，SQL 如下，該語句會將主庫校驗值同步到備庫：

{% highlight sql %}
UPDATE checksum_table SET chunk_time = ?, master_crc = ?, master_cnt = ?
  WHERE db = ? AND tbl = ? AND chunk = ?;
{% endhighlight %}

#### 等待同步完成

接下來就是等待備庫執行完成上述的 SQL 語句，也即將主庫的校驗值同步到備庫。在按照 chunk 執行完成之後，接下來只需要去備庫的 checksums 表中找 ```master_cnt <> this_cnt or OR master_crc <> this_crc``` 的記錄即可。

### 校驗函數

簡單介紹下上述校驗命令中的函數。

#### COALESCE()

類似於 Oracle 中的 NVL() 函數，語義略有區別，會返回參數中第一個非 NULL 表達式的值，從左到右：

{% highlight text %}
----- 如下函數返回 1
mysql> SELECT COALESCE(NULL, NULL, 1);
{% endhighlight %}

通常使用場景為，如果某個字段默認是 NULL，如果不想返回 NULL，而是 0 或其它值，可以使用該函數：

{% highlight text %}
mysql> SELECT COALESCE(field_name, 0) AS value FROM table;
{% endhighlight %}

#### BIT_XOR()

用於 Binary String 的檢測，會字段的值逐位進行檢測，如果 bit 相同則返回 0，否則返回 1；比較時會按照行逐行進行檢測，示例如下：

{% highlight text %}
mysql> CREATE TABLE foobar (id INT);
mysql> INSERT INTO foobar VALUES(100), (100), (120);
mysql> SELECT BIT_XOR(id) FROM foobar;
{% endhighlight %}

首先是第一行與第二行比較，結果是 0；然後結果 0 與第三行比較，結果是 120。

#### CAST()/CONVERT()

CAST() 和 CONVERT() 用來進行類型轉換，只是語法不同，可用類型包括了 BINARY、CHAR(N)、DATE、TIME、DATETIME、DECIMAL、SIGNED、UNSIGNED 等：

{% highlight text %}
CAST(value AS type);
CONVERT(value, type);
{% endhighlight %}

#### CONV()

語法為 ```CONV(N,from_base,to_base)```，該函數用於數字的基數轉換，最小基數值是2，最大值為36，如果任一參數為NULL，則該函數返回NULL。

{% highlight text %}
----- 將數字5從基數16轉為基數2
SELECT CONV(5,16,2);
{% endhighlight %}


## 配置選項

接下來，看下執行命令時的常用參數。

### 安全選項

* \-\-[no]check-replication-filters (可選)

是否檢查複製過濾規則，默認會檢查，如果檢查到有設置過濾規則會直接退出，主要防治檢查的表在主庫存在，而備庫不存在導致 SQL 異常，主備複製出錯；

* \-\-[no]check-slave-tables (設置)

默認檢查，查看是否所有從庫都有被檢查的表和列，防止由於改程序導致主備複製異常；每次同時會通過 EXPLAIN 檢查備庫的執行計劃，評估行數；

* \-\-chunk-size-limit=2.0 (設置，儘量確保有主鍵或者唯一索引)

每次執行校驗 SQL 前，會通過 EXPLAIN 檢查本次執行SQL的數據量，對於沒有唯一索引的表，通過 EXPLAIN 查看的行數差異會比較大，該參數就用於設置最大可以超過 chunk-size 幾倍；如果超過 chunk-size*chunk-size-limit，那麼就不會對本次的 chunk 做校驗。

如果是 1.0，就以 chunk-size 為準；是 0.0 則表示不對 chunk 大小做校驗。

### 限速選項

* \-\-check-interval=1 (可選)

需要睡眠多久，再檢查一次主從延遲是否超過 max-lag，默認是 1 秒；

* \-\-max-load=Threads_running=25 (建議設置)

設置最大負載，當超過這個值時就等待，防止由於工具導致過載，默認是 ```Threads_running=25``` 等價於 ```Threads_running:25```，可以逗號分隔設置多個值；如果不指定值則會在開始檢查變量對應的值，並將上限設置為 120%；

每次執行完一個 chunk 之後，會通過 ```SHOW GLOBAL STATUS``` 查看當前服務器的狀態，如果超過了上述值，就會等待；如果校驗 SQL 導致鎖等待，則會導致隊列增加，進而 Threads_running 增加，通過該參數可以減小系統負載。

* \-\-check-slave-lag="h=slave1" (可選)

是否只檢查這個從庫的複製延遲，如果有些場景通過 pt-slave-delay 特意設置了延遲複製，那麼可以通過該參數指定需要關注哪些複製延遲。

* \-\-max-lag=1s (設置)

檢查備庫的最大延遲，也就是 ```Seconds_Behind_Master``` 參數，當超過這個就等待，而且如果備庫複製線程異常，同樣會等待；默認會檢查所有庫，也可以通過 check-slave-lag 指定需要檢查哪些庫。

### 過濾選項

可以指定只檢查哪些數據庫、表、列、存儲引擎等，也可以使用 Perl 正則表達式；除此之外，也可以指定忽略哪些。

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

### 主庫登陸選項

* \-\-host
* \-\-user
* \-\-password
* \-\-port
* \-\-ask-pass
* \-\-socket

### 其它參數：

* \-\-[no]empty-replicate-table

默認yes，會在每次執行校驗前清理要校驗表對應的記錄，注意該操作不會TRUNCATE TABLE，如果需要，則要手動執行；

* \-\-progress=time,30

輸出當前的進展，有三種輸出方式，包括了兩個參數值，默認是 time,30，還可以設置為 percentage,time,iterations；

* \-\-explain

打印要執行的命令，但是不執行 (dry run)；

* \-\-[no]create-replicate-table

檢查 replicate 參數指定的表，如果不存在是否創建；

* \-\-replicate=percona.checksums

將校驗結果寫入到那個表中，默認是 percona.checksums，可以通過該參數指定；

* \-\-binary-index (默認不開啟測試)

默認通過 replicate 指定的表的 lower_boundary、upper_boundary 字段格式為 text，有些情況下可能會存在不兼容，通過該參數將定義上述字段為 blob 格式；

* \-\-[no]replicate-check

默認 yes，也就是每次校驗完一個表之後，會等待各個備庫同步完成，從而進行數據校驗，如果關閉則需要通過 replicate-check-only 參數再檢查一遍；

* \-\-replicate-check-only

不執行數據校驗的 SQL，只查詢保存的結果；

* \-\-run-time=NUM

要執行多長時間，每個循環會檢測是否達到該參數設置值，可以指定單位，默認是秒，可以指定參數 s=seconds,m=minutes,h=hours,d=days；如果使用 ```--resume``` 那麼之前的時間也會計算在內；

* \-\-recurse=NUM

在查找複製庫時指定遞歸層次，可以查看 ```recurse_to_slaves()``` 函數；

* \-\-recursion-method=processlist

遞歸查找時的方法，包括了 processlist(```SHOW PROCESSLIST```)、hosts(```SHOW SLAVE HOSTS```)、none，詳細查看 ```recurse_to_slaves()``` 函數；默認在啟動時會檢查所有的級聯備庫，可以通過該參數確認遞歸方法，通過 recurse 設置檢查級聯數；

* \-\-[no]check-binlog-format

檢查各個節點的格式是否相同，一般需要保證 binlog 格式為 STATEMENT 。

* \-\-[no]check-plan

每次在執行正式查詢前，默認會通過 EXPLAIN 查看執行計劃，然後判斷執行計劃是否有問題，有可能會跳過本 chunk；

* \-\-replicate-check-retries

當出現校驗異常時，會嘗試多少次；

* \-\-replicate-database

默認會在執行過程中通過 USE 切換數據庫，如果設置了 ```binlog_ignore_db```、```replicate_ignore_db``` 類型的參數，那麼可能會導致錯誤，通過該參數可以防止切換數據庫；

* \-\-chunk-index

優先選擇使用的索引，如果索引不存在則自動選擇合適的索引，然後在執行 SQL 時會通過 ```FORCE INDEX``` 指定索引，當然如果指定錯的話可能會有問題，建議只針對單表設置；

* \-\-chunk-index-columns

只使用索引最左匹配的幾個列，通常可以在 MySQL 優化器有問題時進行設置；

* \-\-chunk-size

每次校驗查詢時的查詢數據行數，可以使用 k,M,G 後綴，不建議直接使用，最好設置 chunk-time；默認會在每次查詢時動態調整儘量接近 chunk-time 值，不指定時採用默認值作為起始值，後續的查詢則動態調整，如果指定則嚴格按照該參數進行查詢；

* \-\-chunk-time

如上，每次查詢會動態調整 checksum 查詢的記錄數，使執行時間儘量接近該值，單位是秒；

* \-\-config

指定配置文件，如果指定則需要設置為命令行的第一項；

* \-\-defaults-file

指定 MySQL 配置文件，需要使用絕對路徑；

* \-\-float-precision

用於浮點數四捨五入，會通過 ROUND() 函數處理，防止在不同平臺上由於精度不同導致校驗失敗，例如為2時對於 1.008 和 1.009 會四捨五入為 1.01；

* \-\-function

指定數據校驗的函數，如果採用 CRC32 可能會有 hash 衝突，而 SHA1+MD5 對 CPU 消耗比較大，也可以使用 FNV1A_64()、MURMUR_HASH() 自定義函數進行處理；

* \-\-pid

指定 PID 文件，一般只允許一個實例部署；

* \-\-plugin

指定 Perl 插件；

* \-\-quiet

使用一次時只打印 Errors、Warnings、數據不一致；使用兩次則只輸出錯誤信息；

* \-\-resume

繼續上次中斷；

* \-\-retries

發生非致命問題時(鎖等待超時、查詢被 kill 等)，每個 chunk 的重試執行次數；

* \-\-separator

執行 CONCAT_WS() 函數時指定的分隔符，默認是 #；

* \-\-set-vars

設置會話變量，默認會設置 ```wait_timeout=10000```、```innodb_lock_wait_timeout=1``` 兩個參數；

* \-\-version

查看當前版本；

* \-\-[no]version-check

自動檢查版本，用於更新，除了檢查 Percona-toolkit 版本之外，還會檢查 Perl 以及 MySQL 的版本，並打印已知問題；

* \-\-chunk-size=1000

每次 checksum 校驗時查詢的記錄數，如果指定了該參數，則將 chunk-time 設置為 0；

* \-\-chunk-time=0.5

每次動態調整 checksum 記錄數，使執行時間儘量接近該值，單位是秒；

* \-\-slave-skip-tolerance

如果設置主庫一個 chunk 校驗完，而備庫可能超過限制大小，那麼就會跳過備庫的查詢；

* \-\-trim

主要是針對 VARCHAR 類型的數據列，主要用於 4.1 和 >=5.0 的數據對比，5.0 之後會保存 VARCHAR 類型開始與結尾的空格，而 4.1 則會自動去除；

* \-\-where

用於過濾部分數據，與 mysqldump 的 -w 選項類似；


## FAQs

##### 1. binlog在row模式下是否支持？
工具在執行時會將會話變量binlog格式設置為statement；如果級聯開啟了--log-slave-updates選項，而binlog_format=ROW，那麼此時仍有可能在會導致下一層的級聯校驗失敗。

##### 2. 如何做到流量控制？
提供了多個參數進行流量控制，每次執行完一個chunk之後，會檢查參數是否滿足限流要求，如果超過限制則睡眠等待，直到滿足要求。詳見參數配置中的 "限速選項" 。


<!--
### 校驗數據表確認

導出當前表結構，過濾表數量：

```
mysqldump --no-data -S /var/lib/mysql/mysql.sock -u omadmin -p'Opsmonitordb@2015' zabbix > opsmonitor.sql
cat opsmonitor.sql | grep "CREATE TABLE" | wc -l
cat opsmonitor.sql | grep "PRIMARY KEY" | wc -l
```

總計114個表，109有Primary Key，監控表包含了除dbversion、history、history_str、history_uint、tmp_cpustat、history_text、history_log外所有表，包括了


pt-table-checksum --nocheck-binlog-format \
  --user=root --password=new-password --port=3307 --host=127.1 --databases=sakila --tables=payment \
  --replicate=sakila.checksums


get_replication_filters() 如果設置check-replication-filters參數，則檢查是否設置了過濾選項

check_slave_tables() 默認check-slave-tables


|-make_chunk_checksum() 生成需要進行校驗的列，只有在沒有索引或者chunk過大時才會報錯
| |-get_crc_args() 獲取CRC參數，測試各種HASH函數測試是否可用
| |-make_row_checksum()
|-OobNibbleIterator() 新建一個循環
  |-

can_nibble()
 |-get_row_estimate() 評估當前數據量，主要通過EXPLAIN測試
 |-_find_best_index() 查找最優的索引

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

## 參考

[DSN (DATA SOURCE NAME) SPECIFICATIONS](https://www.percona.com/doc/percona-toolkit/2.1/dsn_data_source_name_specifications.html)

{% highlight text %}
{% endhighlight %}
