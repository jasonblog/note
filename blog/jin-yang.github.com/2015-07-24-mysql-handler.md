---
title: MySQL Handler 監控
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,database,handler
description: 在 MySQL 中有很多的 handler_* 類型的監控，接下來我們就看看這些監控項。
---

在 MySQL 中有很多的 handler_* 類型的監控，接下來我們就看看這些監控項。

<!-- more -->

<!--
<style type="text/css">
.highlight pre { font-size: 0.58em; }
</style>
-->

## 簡介

通過 ```SHOW GLOBAL STATUS``` 命令查看時，其中有一部分是 ```Handler_*``` 類型的監控，顯示了數據庫處理 SQL 語句的狀態，對於調試 SQL 語句有很大意義。

{% highlight text %}
mysql> SHOW GLOBAL STATUS LIKE 'handler_%';
+----------------------------+-------+
| Variable_name              | Value |
+----------------------------+-------+
| Handler_commit             | 65    |
| Handler_delete             | 0     |
| Handler_discover           | 0     |
| Handler_external_lock      | 347   |
| Handler_mrr_init           | 0     |
| Handler_prepare            | 0     |
| Handler_read_first         | 22    |
| Handler_read_key           | 50    |
| Handler_read_last          | 0     |
| Handler_read_next          | 4     |
| Handler_read_prev          | 0     |
| Handler_read_rnd           | 0     |
| Handler_read_rnd_next      | 3620  |
| Handler_rollback           | 0     |
| Handler_savepoint          | 0     |
| Handler_savepoint_rollback | 0     |
| Handler_update             | 0     |
| Handler_write              | 2402  |
+----------------------------+-------+
18 rows in set (0.02 sec)
{% endhighlight %}

不同的發行版本可能會有些區別，其中在 MySQL 上的查詢如下，而在 MariaDB 的發行版本中，實際上還有 ```Handler_read_rnd_deleted``` 一項的；在源碼 sql/mysqld.cc 中有如下內容。

{% highlight c %}
SHOW_VAR status_vars[]= {
  {"Handler_commit",           (char*) offsetof(STATUS_VAR, ha_commit_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_delete",           (char*) offsetof(STATUS_VAR, ha_delete_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_discover",         (char*) offsetof(STATUS_VAR, ha_discover_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_external_lock",    (char*) offsetof(STATUS_VAR, ha_external_lock_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_mrr_init",         (char*) offsetof(STATUS_VAR, ha_multi_range_read_init_count),
         SHOW_LONGLONG_STATUS, SHOW_SCOPE_ALL},
  {"Handler_prepare",          (char*) offsetof(STATUS_VAR, ha_prepare_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_read_first",       (char*) offsetof(STATUS_VAR, ha_read_first_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_read_key",         (char*) offsetof(STATUS_VAR, ha_read_key_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_read_last",        (char*) offsetof(STATUS_VAR, ha_read_last_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_read_next",        (char*) offsetof(STATUS_VAR, ha_read_next_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_read_prev",        (char*) offsetof(STATUS_VAR, ha_read_prev_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_read_rnd",         (char*) offsetof(STATUS_VAR, ha_read_rnd_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_read_rnd_next",    (char*) offsetof(STATUS_VAR, ha_read_rnd_next_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_rollback",         (char*) offsetof(STATUS_VAR, ha_rollback_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_savepoint",        (char*) offsetof(STATUS_VAR, ha_savepoint_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_savepoint_rollback",(char*) offsetof(STATUS_VAR, ha_savepoint_rollback_count),
         SHOW_LONGLONG_STATUS, SHOW_SCOPE_ALL},
  {"Handler_update",           (char*) offsetof(STATUS_VAR, ha_update_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
  {"Handler_write",            (char*) offsetof(STATUS_VAR, ha_write_count),
         SHOW_LONGLONG_STATUS,    SHOW_SCOPE_ALL},
};
{% endhighlight %}

如下，僅介紹下與 ```Handler_read_*``` 相關的內容。

## Handler_read_*

這裡的監控項是處理 SELECT 語句相關。

{% highlight text %}
  {"Handler_read_first",       (char*) offsetof(STATUS_VAR, ha_read_first_count),
  {"Handler_read_key",         (char*) offsetof(STATUS_VAR, ha_read_key_count),
  {"Handler_read_last",        (char*) offsetof(STATUS_VAR, ha_read_last_count),
  {"Handler_read_next",        (char*) offsetof(STATUS_VAR, ha_read_next_count),
  {"Handler_read_prev",        (char*) offsetof(STATUS_VAR, ha_read_prev_count),
  {"Handler_read_rnd",         (char*) offsetof(STATUS_VAR, ha_read_rnd_count),
  {"Handler_read_rnd_next",    (char*) offsetof(STATUS_VAR, ha_read_rnd_next_count),
{% endhighlight %}

其中，前五個統計項的含義如下所示。

{% highlight text %}
Handler_read_first    : 讀索引的第一項
Handler_read_key      : 讀索引的某一項
Handler_read_next     : 讀索引的下一項
Handler_read_last     : 讀索引的最後第一項
Handler_read_prev     : 讀索引的前一項

## 有四種組合
1. Handler_read_first、Handler_read_next 組合應該是索引覆蓋掃描
2. Handler_read_key 基於索引取值
3. Handler_read_key、Handler_read_next 組合應該是索引範圍掃描
4. Handler_read_last、Handler_read_prev 組合應該是索引範圍掃描（orde by desc）
{% endhighlight %}

索引項之間都是有順序的，所以才有 first, last, next, prev 等等；而後兩個是對數據文件的計數器。


<!--
測試發現與顯示內容有所出入。
CREATE TABLE IF NOT EXISTS `foo` (
   `id` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
   `name` VARCHAR(10) NOT NULL,
   `ts` TIMESTAMP,
   PRIMARY KEY (`id`),
   KEY `idx_name` (`name`)
);

INSERT INTO `foo` VALUES
   (1, 'Schmitt', now()),
   (2, 'King', now()),
   (3, 'Ferguson', now()),
   (4, 'Labrune', now()),
   (5, 'Nelson', now()),
   (6, 'Lee', now()),
   (7, 'Freyre', now()),
   (8, 'Young', now()),
   (9, 'Leong', now());
-->

為了進行測試，假設，有如下的數據。

{% highlight text %}
CREATE TABLE test (
    id    INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    data  VARCHAR(32),
    ts    TIMESTAMP,
    KEY idx_data (data)
);
INSERT INTO test VALUES
    (NULL, 'abc', NOW()), (NULL, 'abc', NOW()),
    (NULL, 'abd', NOW()), (NULL, 'acd', NOW()),
    (NULL, 'def', NOW()), (NULL, 'pqr', NOW()),
    (NULL, 'stu', NOW()), (NULL, 'vwx', NOW()),
    (NULL, 'yza', NOW()), (NULL, 'def', NOW());
{% endhighlight %}

在下面的測試裡，每次執行SQL時按照如下過程執行：

{% highlight text %}
FLUSH STATUS;
SELECT ...;
SHOW SESSION STATUS LIKE 'Handler_read%';
EXPLAIN SELECT ...;
{% endhighlight %}

### handler_read_first

{% highlight text %}
The number of times the first entry was read from an index. If this value
is high, it suggests that the server is doing a lot of full index scans;
for example, SELECT col1 FROM foo, assuming that col1 is indexed.
{% endhighlight %}

說明 SQL 是在做一個全索引掃描，注意是全索引(也可以是主健)，而不是部分，所以說如果存在 WHERE 語句，這個參數是不會變的。如果這個監控的數值很大，說明查詢都是在索引裡完成的，而不是數據文件裡；不過仍是做一次完整的掃描。

{% highlight text %}
+----------+     +----+----+
| Table    |     | Index   |
|          |     |    |    |
|          |     |    |    |
|          |     |    |    |
|          |     |    |    |
|          |     |    v    |
|          |     |         |
|          |     |         |
+----------+     +---------+

mysql> FLUSH STATUS;
Query OK, 0 rows affected (0.01 sec)

mysql> SELECT data FROM test;
10 rows in set (0.00 sec)

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| Handler_read_first    | 1     |  會從索引的第一條記錄開始讀取
| Handler_read_key      | 1     |  也就是對應了第一條
| Handler_read_next     | 10    |  加上通過key讀取的，實際讀取了11條記錄
+-----------------------+-------+
7 rows in set (0.00 sec)

mysql> EXPLAIN SELECT data FROM test;
+------+-------------+-------+-------+---------------+----------+---------+------+------+-------------+
| id   | select_type | table | type  | possible_keys | key      | key_len | ref  | rows | Extra       |
+------+-------------+-------+-------+---------------+----------+---------+------+------+-------------+
|    1 | SIMPLE      | test  | index | NULL          | idx_data | 35      | NULL |   10 | Using index |
+------+-------------+-------+-------+---------------+----------+---------+------+------+-------------+
1 row in set (0.00 sec)
{% endhighlight %}

也就是說，在上述的查詢中，是一次全索引查詢，也就是執行了 10 次索引查詢。

{% highlight text %}
mysql> SELECT data FROM test WHERE data BETWEEN 'A' AND 'O';
6 rows in set (0.00 sec)

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| Handler_read_first       | 0     |  不會讀取索引的第一條記錄
| Handler_read_key         | 1     |  但是會通過索引讀取'A'所在位置
| Handler_read_next        | 6     |  加上通過key讀取的，實際讀取了7條記錄
+--------------------------+-------+
7 rows in set (0.00 sec)

mysql> EXPLAIN SELECT data FROM test WHERE data BETWEEN 'A' AND 'O';
+------+-------------+-------+-------+----------+---------+------+------+--------------------------+
| id   | select_type | table | type  | key      | key_len | ref  | rows | Extra                    |
+------+-------------+-------+-------+----------+---------+------+------+--------------------------+
|    1 | SIMPLE      | test  | range | idx_data | 35      | NULL |    6 | Using where; Using index |
+------+-------------+-------+-------+----------+---------+------+------+--------------------------+
1 row in set (0.00 sec)
{% endhighlight %}

上述的查詢實際上沒有從 Handler_read_first 開始；另外，執行如下的 SQL，均未讀取 first 。

{% highlight text %}
mysql> SELECT data FROM test WHERE data < 'O';
mysql> SELECT data FROM test WHERE data LIKE 'a%';
mysql> SELECT data FROM test WHERE data IN ('abc', 'abd', 'acd');
{% endhighlight %}

通常來說，如果不是完全的全索引掃描，一般 read_first 不會增加。

### handler_read_key

{% highlight text %}
The number of requests to read a row based on a key. If this value is high,
it is a good indication that your tables are properly indexed for your queries.
{% endhighlight %}

此選項數值如果很高，那麼恭喜你，你的系統高效的使用了索引，一切運轉良好。

{% highlight text %}
+-------------+          +-------+
| Table       |          | Index |
|             | <------  |       | <--+
|             |          |       |
|             |          |       |
|             |          |       |
|             |          |       |
|             |          |       |
|             |          |       |
+-------------+          +-------+

mysql> FLUSH STATUS;
Query OK, 0 rows affected (0.01 sec)

mysql> SELECT data FROM test WHERE data = 'abc';
2 rows in set (0.00 sec)

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| Handler_read_first    | 0     |  不會讀取第一條記錄
| Handler_read_key      | 1     |  通過索引找到第一條記錄
| Handler_read_next     | 2     |  遍厲，會讀取最後一條記錄以判斷是否滿足條件
+-----------------------+-------+
7 rows in set (0.00 sec)

mysql> EXPLAIN SELECT data FROM test WHERE data = 'abc';
+----+-------------+-------+------+---------------+----------+---------+-------+------+----------+-------------+
| id | select_type | table | type | possible_keys | key      | key_len | ref   | rows | filtered | Extra       |
+----+-------------+-------+------+---------------+----------+---------+-------+------+----------+-------------+
|  1 | SIMPLE      | test  | ref  | idx_data      | idx_data | 35      | const |    2 |   100.00 | Using index |
+----+-------------+-------+------+---------------+----------+---------+-------+------+----------+-------------+
1 row in set, 1 warning (0.00 sec)
{% endhighlight %}

### handler_read_next

{% highlight text %}
The number of requests to read the next row in key order. This value
is incremented if you are querying an index column with a range
constraint or if you are doing an index scan.
{% endhighlight %}

此選項表明在進行索引掃描時，按照索引從數據文件裡取數據的次數，示例的話可以參考上面的。

{% highlight text %}
+-------------+          +-------+
| Table       |          | Index |
|             |          |       |
|             | <------  |   +   |
|             | <------  |   |   |
|             | <------  |   v   |
|             |          |       |
|             |          |       |
|             |          |       |
+-------------+          +-------+
{% endhighlight %}

### handler_read_prev

{% highlight text %}
The number of requests to read the previous row in key order. This read
method is mainly used to optimize ORDER BY ... DESC.
{% endhighlight %}

此選項表明在進行索引掃描時，按照索引倒序從數據文件裡取數據的次數，一般就是 ORDER BY ... DESC。

{% highlight text %}
+-------------+          +-------+
| Table       |          | Index |
|             |          |       |
|             | <------  |   ^   |
|             | <------  |   |   |
|             | <------  |   +   |
|             |          |       |
|             |          |       |
|             |          |       |
+-------------+          +-------+

mysql> FLUSH STATUS;
Query OK, 0 rows affected (0.01 sec)

mysql> SELECT data FROM test ORDER BY data DESC;
10 rows in set (0.00 sec)

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| Handler_read_first    | 0     |
| Handler_read_key      | 1     |  通過索引讀取
| Handler_read_last     | 1     |  讀取最後一條記錄
| Handler_read_next     | 0     |
| Handler_read_prev     | 10    |  利用索引逆序讀，加上讀取最後一條記錄實際讀取了11條
+-----------------------+-------+
7 rows in set (0.02 sec)

mysql> EXPLAIN SELECT data FROM test ORDER BY data DESC;
+----+-------------+-------+-------+---------------+----------+---------+------+------+----------+-------------+
| id | select_type | table | type  | possible_keys | key      | key_len | ref  | rows | filtered | Extra       |
+----+-------------+-------+-------+---------------+----------+---------+------+------+----------+-------------+
|  1 | SIMPLE      | test  | index | NULL          | idx_data | 35      | NULL |   10 |   100.00 | Using index |
+----+-------------+-------+-------+---------------+----------+---------+------+------+----------+-------------+
1 row in set, 1 warning (0.01 sec)
{% endhighlight %}

### handler_read_last

與 handler_read_first 類似，例如逆序讀取最後一條記錄，在此不做過多介紹了。

### handler_read_rnd

{% highlight text %}
The number of requests to read a row based on a fixed position. This value
is high if you are doing a lot of queries that require sorting of the result.
You probably have a lot of queries that require MySQL to scan entire tables
or you have joins that don't use keys properly.
{% endhighlight %}

簡單的說，就是查詢直接操作了數據文件，很多時候表現為沒有使用索引(可能時全表掃描)、文件排序、JOIN 使用不當。

<!--
TODODO:
This status comes into account if the old file_sort mechanism is used [ 2 ].
-->

測試時，需要修改下上述的表結構。

{% highlight text %}
mysql> ALTER TABLE test ADD COLUMN file_sort text;
Query OK, 0 rows affected (0.13 sec)
Records: 0  Duplicates: 0  Warnings: 0

UPDATE test SET file_sort = 'abcdefghijklmnopqrstuvwxyz' WHERE id = 1;
UPDATE test SET file_sort = 'bcdefghijklmnopqrstuvwxyza' WHERE id = 2;
UPDATE test SET file_sort = 'cdefghijklmnopqrstuvwxyzab' WHERE id = 3;
UPDATE test SET file_sort = 'defghijklmnopqrstuvwxyzabc' WHERE id = 4;
UPDATE test SET file_sort = 'efghijklmnopqrstuvwxyzabcd' WHERE id = 5;
UPDATE test SET file_sort = 'fghijklmnopqrstuvwxyzabcde' WHERE id = 6;
UPDATE test SET file_sort = 'ghijklmnopqrstuvwxyzabcdef' WHERE id = 7;
UPDATE test SET file_sort = 'hijklmnopqrstuvwxyzabcdefg' WHERE id = 8;
UPDATE test SET file_sort = 'ijklmnopqrstuvwxyzabcdefgh' WHERE id = 9;
UPDATE test SET file_sort = 'jklmnopqrstuvwxyzabcdefghi' WHERE id = 10;

mysql> FLUSH STATUS;
Query OK, 0 rows affected (0.01 sec)

mysql> SELECT * FROM test ORDER BY file_sort ASC;
10 rows in set (0.00 sec)

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| Handler_read_first    | 1     |
| Handler_read_key      | 11    |
| Handler_read_last     | 0     |
| Handler_read_next     | 0     |
| Handler_read_prev     | 0     |
| Handler_read_rnd      | 10    |
| Handler_read_rnd_next | 11    |
+-----------------------+-------+
7 rows in set (0.01 sec)

mysql> EXPLAIN SELECT * FROM test ORDER BY file_sort ASC;
+----+-------------+-------+------+---------------+------+---------+------+------+----------+----------------+
| id | select_type | table | type | possible_keys | key  | key_len | ref  | rows | filtered | Extra          |
+----+-------------+-------+------+---------------+------+---------+------+------+----------+----------------+
|  1 | SIMPLE      | test  | ALL  | NULL          | NULL | NULL    | NULL |   10 |   100.00 | Using filesort |
+----+-------------+-------+------+---------------+------+---------+------+------+----------+----------------+
1 row in set, 1 warning (0.00 sec)
{% endhighlight %}

**注意**：這裡的性能會變得非常差，所以一定要儘量避免。

### handler_read_rnd_next

{% highlight text %}
The number of requests to read the next row in the data file. This value
is high if you are doing a lot of table scans. Generally this suggests
that your tables are not properly indexed or that your queries are not
written to take advantage of the indexes you have.
{% endhighlight %}

此選項表明在進行數據文件掃描時，從數據文件裡取數據的次數。

{% highlight text %}
+------+------+          +-------+
| Table|      |          | Index |
|      |      |          |       |
|      |      |          |       |
|      |      |          |       |
|      |      |          |       |
|      v      |          |       |
|             |          |       |
|             |          |       |
+-------------+          +-------+

mysql> SELECT * FROM test;
10 rows in set

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| Handler_read_first    | 1     |
| Handler_read_key      | 1     |
| Handler_read_prev     | 0     |
| Handler_read_rnd      | 0     |
| Handler_read_rnd_next | 11    |
+-----------------------+-------+
7 rows in set (0.02 sec)

mysql> EXPLAIN SELECT * FROM test;
+----+-------------+-------+------+---------------+------+---------+------+------+----------+-------+
| id | select_type | table | type | possible_keys | key  | key_len | ref  | rows | filtered | Extra |
+----+-------------+-------+------+---------------+------+---------+------+------+----------+-------+
|  1 | SIMPLE      | test  | ALL  | NULL          | NULL | NULL    | NULL |   10 |   100.00 | NULL  |
+----+-------------+-------+------+---------------+------+---------+------+------+----------+-------+
1 row in set, 1 warning (0.00 sec)
{% endhighlight %}

另外，再看如下的一個示例。

{% highlight text %}
mysql> SELECT * FROM test WHERE ts = '2008-01-18 17:33:39';
Empty set (0.01 sec)

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| Handler_read_first    | 1     |
| Handler_read_key      | 1     |
| Handler_read_rnd      | 0     |
| Handler_read_rnd_next | 11    |
+-----------------------+-------+
7 rows in set (0.01 sec)

mysql> EXPLAIN SELECT * FROM test WHERE ts = '2008-01-18 17:33:39';
+----+-------------+-------+------+---------------+------+---------+------+------+----------+-------------+
| id | select_type | table | type | possible_keys | key  | key_len | ref  | rows | filtered | Extra       |
+----+-------------+-------+------+---------------+------+---------+------+------+----------+-------------+
|  1 | SIMPLE      | test  | ALL  | NULL          | NULL | NULL    | NULL |   10 |    10.00 | Using where |
+----+-------------+-------+------+---------------+------+---------+------+------+----------+-------------+
1 row in set, 1 warning (0.00 sec)
{% endhighlight %}

### 總結

不同平臺，不同版本的 MySQL，在運行上面例子的時候，Handler_read_* 的數值可能會有所不同，這並不要緊，關鍵是你要意識到 Handler_read_* 可以協助你理解 MySQL 處理查詢的過程。

其中，Handler_read_rnd 和 Handler_read_rnd_next 是越低越好，如果很高，應該進行索引相關的調優；而 Handler_read_key 的數值是越高越好，越高代表使用索引的讀很高；其它的計數器，要具體情況具體分析。

另外需要注意的是，不只有 SELECT 語句，UPDATE、DELETE 等語句也需要先查詢，所以也會影響上述參數。

<!--
說到判斷查詢方式優劣這個問題，就再順便提提show profile語法，在新版MySQL裡提供了這個功能：
mysql> set profiling=on;
mysql> use mysql;
mysql> select * from user;
mysql> show profile;
+--------------------+----------+
| Status             | Duration |
+--------------------+----------+
| starting           | 0.000078 |
| Opening tables     | 0.000022 |
| System lock        | 0.000010 |
| Table lock         | 0.000014 |
| init               | 0.000054 |
| optimizing         | 0.000008 |
| statistics         | 0.000015 |
| preparing          | 0.000014 |
| executing          | 0.000007 |
| Sending data       | 0.000139 |
| end                | 0.000007 |
| query end          | 0.000007 |
| freeing items      | 0.000044 |
| logging slow query | 0.000004 |
| cleaning up        | 0.000005 |
+--------------------+----------+
15 rows in set (0.00 sec)
mysql> show profiles;
+----------+------------+--------------------+
| Query_ID | Duration   | Query              |
+----------+------------+--------------------+
|        1 | 0.00017725 | SELECT DATABASE(). |
|        2 | 0.00042675 | select * from user |
+----------+------------+--------------------+
2 rows in set (0.00 sec)
mysql> flush status;
mysql> flush tables;
-->


## 參考

上述的內容其中很大一部分是參考 [The handler_read_* status variables](http://www.fromdual.ch/mysql-handler-read-status-variables)，也可以查看 [本地文檔](/reference/mysql/the handler_read status variables.mht) 。

<!--
http://blog.itpub.net/26250550/viewspace-1076292/
-->

{% highlight text %}
{% endhighlight %}
