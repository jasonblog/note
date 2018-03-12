---
title: MySQL Handler 监控
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,database,handler
description: 在 MySQL 中有很多的 handler_* 类型的监控，接下来我们就看看这些监控项。
---

在 MySQL 中有很多的 handler_* 类型的监控，接下来我们就看看这些监控项。

<!-- more -->

<!--
<style type="text/css">
.highlight pre { font-size: 0.58em; }
</style>
-->

## 简介

通过 ```SHOW GLOBAL STATUS``` 命令查看时，其中有一部分是 ```Handler_*``` 类型的监控，显示了数据库处理 SQL 语句的状态，对于调试 SQL 语句有很大意义。

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

不同的发行版本可能会有些区别，其中在 MySQL 上的查询如下，而在 MariaDB 的发行版本中，实际上还有 ```Handler_read_rnd_deleted``` 一项的；在源码 sql/mysqld.cc 中有如下内容。

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

如下，仅介绍下与 ```Handler_read_*``` 相关的内容。

## Handler_read_*

这里的监控项是处理 SELECT 语句相关。

{% highlight text %}
  {"Handler_read_first",       (char*) offsetof(STATUS_VAR, ha_read_first_count),
  {"Handler_read_key",         (char*) offsetof(STATUS_VAR, ha_read_key_count),
  {"Handler_read_last",        (char*) offsetof(STATUS_VAR, ha_read_last_count),
  {"Handler_read_next",        (char*) offsetof(STATUS_VAR, ha_read_next_count),
  {"Handler_read_prev",        (char*) offsetof(STATUS_VAR, ha_read_prev_count),
  {"Handler_read_rnd",         (char*) offsetof(STATUS_VAR, ha_read_rnd_count),
  {"Handler_read_rnd_next",    (char*) offsetof(STATUS_VAR, ha_read_rnd_next_count),
{% endhighlight %}

其中，前五个统计项的含义如下所示。

{% highlight text %}
Handler_read_first    : 读索引的第一项
Handler_read_key      : 读索引的某一项
Handler_read_next     : 读索引的下一项
Handler_read_last     : 读索引的最后第一项
Handler_read_prev     : 读索引的前一项

## 有四种组合
1. Handler_read_first、Handler_read_next 组合应该是索引覆盖扫描
2. Handler_read_key 基于索引取值
3. Handler_read_key、Handler_read_next 组合应该是索引范围扫描
4. Handler_read_last、Handler_read_prev 组合应该是索引范围扫描（orde by desc）
{% endhighlight %}

索引项之间都是有顺序的，所以才有 first, last, next, prev 等等；而后两个是对数据文件的计数器。


<!--
测试发现与显示内容有所出入。
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

为了进行测试，假设，有如下的数据。

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

在下面的测试里，每次执行SQL时按照如下过程执行：

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

说明 SQL 是在做一个全索引扫描，注意是全索引(也可以是主健)，而不是部分，所以说如果存在 WHERE 语句，这个参数是不会变的。如果这个监控的数值很大，说明查询都是在索引里完成的，而不是数据文件里；不过仍是做一次完整的扫描。

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
| Handler_read_first    | 1     |  会从索引的第一条记录开始读取
| Handler_read_key      | 1     |  也就是对应了第一条
| Handler_read_next     | 10    |  加上通过key读取的，实际读取了11条记录
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

也就是说，在上述的查询中，是一次全索引查询，也就是执行了 10 次索引查询。

{% highlight text %}
mysql> SELECT data FROM test WHERE data BETWEEN 'A' AND 'O';
6 rows in set (0.00 sec)

mysql> SHOW SESSION STATUS LIKE 'Handler_read%';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| Handler_read_first       | 0     |  不会读取索引的第一条记录
| Handler_read_key         | 1     |  但是会通过索引读取'A'所在位置
| Handler_read_next        | 6     |  加上通过key读取的，实际读取了7条记录
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

上述的查询实际上没有从 Handler_read_first 开始；另外，执行如下的 SQL，均未读取 first 。

{% highlight text %}
mysql> SELECT data FROM test WHERE data < 'O';
mysql> SELECT data FROM test WHERE data LIKE 'a%';
mysql> SELECT data FROM test WHERE data IN ('abc', 'abd', 'acd');
{% endhighlight %}

通常来说，如果不是完全的全索引扫描，一般 read_first 不会增加。

### handler_read_key

{% highlight text %}
The number of requests to read a row based on a key. If this value is high,
it is a good indication that your tables are properly indexed for your queries.
{% endhighlight %}

此选项数值如果很高，那么恭喜你，你的系统高效的使用了索引，一切运转良好。

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
| Handler_read_first    | 0     |  不会读取第一条记录
| Handler_read_key      | 1     |  通过索引找到第一条记录
| Handler_read_next     | 2     |  遍厉，会读取最后一条记录以判断是否满足条件
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

此选项表明在进行索引扫描时，按照索引从数据文件里取数据的次数，示例的话可以参考上面的。

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

此选项表明在进行索引扫描时，按照索引倒序从数据文件里取数据的次数，一般就是 ORDER BY ... DESC。

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
| Handler_read_key      | 1     |  通过索引读取
| Handler_read_last     | 1     |  读取最后一条记录
| Handler_read_next     | 0     |
| Handler_read_prev     | 10    |  利用索引逆序读，加上读取最后一条记录实际读取了11条
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

与 handler_read_first 类似，例如逆序读取最后一条记录，在此不做过多介绍了。

### handler_read_rnd

{% highlight text %}
The number of requests to read a row based on a fixed position. This value
is high if you are doing a lot of queries that require sorting of the result.
You probably have a lot of queries that require MySQL to scan entire tables
or you have joins that don't use keys properly.
{% endhighlight %}

简单的说，就是查询直接操作了数据文件，很多时候表现为没有使用索引(可能时全表扫描)、文件排序、JOIN 使用不当。

<!--
TODODO:
This status comes into account if the old file_sort mechanism is used [ 2 ].
-->

测试时，需要修改下上述的表结构。

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

**注意**：这里的性能会变得非常差，所以一定要尽量避免。

### handler_read_rnd_next

{% highlight text %}
The number of requests to read the next row in the data file. This value
is high if you are doing a lot of table scans. Generally this suggests
that your tables are not properly indexed or that your queries are not
written to take advantage of the indexes you have.
{% endhighlight %}

此选项表明在进行数据文件扫描时，从数据文件里取数据的次数。

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

另外，再看如下的一个示例。

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

### 总结

不同平台，不同版本的 MySQL，在运行上面例子的时候，Handler_read_* 的数值可能会有所不同，这并不要紧，关键是你要意识到 Handler_read_* 可以协助你理解 MySQL 处理查询的过程。

其中，Handler_read_rnd 和 Handler_read_rnd_next 是越低越好，如果很高，应该进行索引相关的调优；而 Handler_read_key 的数值是越高越好，越高代表使用索引的读很高；其它的计数器，要具体情况具体分析。

另外需要注意的是，不只有 SELECT 语句，UPDATE、DELETE 等语句也需要先查询，所以也会影响上述参数。

<!--
说到判断查询方式优劣这个问题，就再顺便提提show profile语法，在新版MySQL里提供了这个功能：
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


## 参考

上述的内容其中很大一部分是参考 [The handler_read_* status variables](http://www.fromdual.ch/mysql-handler-read-status-variables)，也可以查看 [本地文档](/reference/mysql/the handler_read status variables.mht) 。

<!--
http://blog.itpub.net/26250550/viewspace-1076292/
-->

{% highlight text %}
{% endhighlight %}
