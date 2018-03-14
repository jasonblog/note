---
Date: October 19, 2013
title: MySQL 基本介紹
layout: post
comments: true
language: chinese
category: [mysql,database]
---

在此主要介紹下與數據庫相關的一些操作，其中有一部分是與 MySQL 相關的功能。

<!-- more -->


## 簡介

![sql logo]({{ site.url }}/images/databases/mysql/sql-logo.jpg "sql logo"){: .pull-right }

首先簡單介紹一下 SQL (Structured Query Language) 相關的標準；SQL-92 是最常用的 SQL 標準，其定義了一些常見的語法等，詳細的可參考 [SQL-92](http://www.contrib.andrew.cmu.edu/~shadow/sql/sql1992.txt) ，也可查看 [本地文檔](/reference/mysql/SQL1992.txt) 。

其中 SQL 常見的操作包括如下：

* DDL：數據庫模式定義語言，create 。
* DML：數據操縱語言，insert、delete、update 。
* DCL：數據庫控制語言 ，grant、remove 。
* DQL：數據庫查詢語言，select 。

不過不同的數據庫對 SQL-92 標準會有所擴展，這也就造成了一些數據庫操作的不兼容，或者是一些細節上的差異。

對於 MySQL，在 Linux 主機上的默認存儲位置為 /var/lib/mysql/ ； FreeBSD 在 /var/db/mysql；如果想要修改可以通過符號鏈接即可。

如果是單獨進行安裝的，可以通過如下命令查看 MySQL 是否安裝成功。

{% highlight text %}
$ mysqladmin --version
{% endhighlight %}

### 示例數據庫

通常最基本的是熟悉簡單的 join 操作，以及基本的 CRUD(Create, Retrieve, Update, Delete) 操作，如下包括了很多教程示例，可以參考 [MySQL Sample Databases](https://www3.ntu.edu.sg/home/ehchua/programming/sql/SampleDatabases.html) 中的介紹，在此簡單列舉幾個。

下面的示例可以參考 [MySQLTutorial](http://www.mysqltutorial.org/)，數據庫的數據也可以從 [本地下載](/reference/mysql/from_mysqltutorial.sql) ，然後通過 source 導入。下圖是對應的 ER 圖，其中關於 ER-Diagram 相關的介紹可以參考 [ER Diagram Symbols and Meaning (LucidChart)](https://www.lucidchart.com/pages/ER-diagram-symbols-and-meaning) 。

![sample sql]({{ site.url }}/images/databases/mysql/sample_database_store.png "sample sql"){: .pull-center width="60%" }

上述是一個銷售汽車的模型，包括了常見的模型以及使用數據。

<!--
Customers，保存了用戶的信息。
Products，產品信息，也就是車的信息。
ProductLines: stores a list of product line categories.
Orders: stores  sales orders placed by customers.
OrderDetails: stores sales order line items for each sales order.
Payments: stores payments made by customers based on their accounts.
Employees: stores all employee information as well as the organization structure such as who reports to whom.
Offices: stores sales office data.
-->


另外，可以參考 [employee](https://launchpad.net/test-db/) 的示例數據庫，現在已經遷移到 [github](https://github.com/datacharmer/test_db)，其文檔位於 [Employees Sample Database](http://dev.mysql.com/doc/employee/en/) ，可以直接從 [本地下載](/reference/mysql/test_db-master.zip) 。

![employees schema]({{ site.url }}/images/databases/mysql/employees-schema.png "employees schema"){: .pull-center width="50%"}

其中 README.md 中包括瞭如何進行安裝。

還有一個經常使用的示例數據庫 [Sakila Sample Database](https://dev.mysql.com/doc/sakila/en/)。

![sample sakila schema]({{ site.url }}/images/databases/mysql/sample-sakila-schema.png "sample sakila schema"){: .pull-center width="90%"}





## 時間

MySQL 中有三種時間類型，官方的解釋如下：

{% highlight text %}
The DATE type is used for values with a date part but no time part. MySQL
retrieves and displays DATE values in 'YYYY-MM-DD' format. The supported
range is '1000-01-01' to '9999-12-31'.

The DATETIME type is used for values that contain both date and time parts.
MySQL retrieves and displays DATETIME values in 'YYYY-MM-DD HH:MM:SS' format.
The supported range is '1000-01-01 00:00:00' to '9999-12-31 23:59:59'.

The TIMESTAMP data type is used for values that contain both date and time
parts. TIMESTAMP has a range of '1970-01-01 00:00:01' UTC to '2038-01-19
03:14:07' UTC.
{% endhighlight %}

<!--
發現DATETIME類型只支持mysql 5.6.5+

|:----:|:--------------------|:--------------------|
| 特性 |DATETIME|TIMESTAMP |
| 顯示 |YYYY-MM-DD HH:MM:SS|YYYY-MM-DD HH:MM:SS|
| 範圍 |1000-01-01 00:00:00 ~ 9999-12-31 23:59:59|1970-01-01 00:00:00 ~ 2038-01-19 03:14:07|
| 存儲 |8字節存儲，採用數值格式|4字節存儲，以UTC格式存儲|
| 時區 |無時區概念|時區轉換，存儲時對當前的時區進行轉換，檢索時再轉換回當前的時區 |
| 特性 |無|在insert, update時可以設置為自動更新時間|
-->

### 特殊屬性

timestamp 有兩個比較特殊的屬性，分別是 ```ON UPDATE CURRENT_TIMESTAMP``` 和 ```CURRENT_TIMESTAMP``` 兩種，使用情況分別如下：

#### CURRENT_TIMESTAMP

當要向數據庫執行 INSERT 操作時，如果有 timestamp 字段屬性設為 CURRENT_TIMESTAMP，則無論這個字段有沒有值都會插入當前系統時間。

#### ON UPDATE CURRENT_TIMESTAMP

當執行 update 操作時，並且字段有上述的屬性時，則字段無論值有沒有變化，它的值也會跟著更新為當前 UPDATE 操作時的時間。

簡單測試如下。

{% highlight text %}
----- 新建測試表
mysql> CREATE TABLE foobar (
  id INT(10) UNSIGNED NOT NULL PRIMARY KEY,
  gmt_create TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  gmt_modify TIMESTAMP NOT NULL ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB;
Query OK, 0 rows affected (0.06 sec)

----- 新寫入數據時，會自動更新DEFAULT CURRENT_TIMESTAMP對應字段
mysql> INSERT INTO foobar(id, gmt_modify) VALUES(1, now());
Query OK, 1 row affected (0.01 sec)
mysql> SELECT * FROM foobar;
+----+---------------------+---------------------+
| id | gmt_create          | gmt_modify          |
+----+---------------------+---------------------+
|  1 | 2016-04-23 13:04:59 | 2016-04-23 13:04:59 |
+----+---------------------+---------------------+
1 row in set (0.00 sec)

----- 在執行UPDATE語句時，會更新ON UPDATE CURRENT_TIMESTAMP對應字段
mysql> UPDATE foobar SET id=2 WHERE id=1;
Query OK, 1 row affected (0.01 sec)
Rows matched: 1  Changed: 1  Warnings: 0
mysql> SELECT * FROM foobar;
+----+---------------------+---------------------+
| id | gmt_create          | gmt_modify          |
+----+---------------------+---------------------+
|  2 | 2016-04-23 13:04:59 | 2016-04-23 13:06:27 |
+----+---------------------+---------------------+
1 row in set (0.01 sec)
{% endhighlight %}

### 默認值

從 MySQL 5.6.6 之後，可能會發現如下的 Warning 日誌，```TIMESTAMP with implicit DEFAULT value is deprecated. Please use --explicit_defaults_for_timestamp server option (see documentation for more details).``` 這是因為這一版本之後，timestamp 類型的默認行為已經取消。

在此之前，TIMESTAMP 類型的默認行為是：

* TIMESTAMP 列如果沒有明確聲明 NULL 屬性，則默認為 NOT NULL (而其它數據類型，未顯示聲明 NOT NULL，則允許 NULL值)，當設置 TIMESTAMP 的列值為 NULL，會自動存儲為當前 timestamp。
* 表中的第一個 TIMESTAMP 列，如果沒有聲明 NULL 屬性、DEFAULT 或者 ON UPDATE，會自動分配 ```DEFAULT CURRENT_TIMESTAMP``` 和 ```ON UPDATE CURRENT_TIMESTAMP``` 屬性。
* 表中第二個 TIMESTAMP 列，如果沒有聲明 NULL 或 DEFAULT 值，則默認設置 ```'0000-00-00 00:00:00'```；插入行時沒有指明改列的值，該列默認分配 ```'0000-00-00 00:00:00'```，且無警告。

如果要關閉警告，如上所述，可以在啟動時添加 ```--explicit_defaults_for_timestamp``` 參數，或者在下面的配置文件中添加如下的參數：

{% highlight text %}
[mysqld]
explicit_defaults_for_timestamp=true
{% endhighlight %}

重啟 MySQL 後，此時 TIMESTAMP 的行為如下：

* 如果沒有顯示聲明 ```NOT NULL``` 屬性，也就是是允許 NULL 值的，可以直接設置改列為 NULL，而沒有默認的填充行為。
* 不會默認分配 ```DEFAULT CURRENT_TIMESTAMP``` 和 ```ON UPDATE CURRENT_TIMESTAMP``` 屬性。
* 聲明為 ```NOT NULL``` 且沒有默認子句的 TIMESTAMP 列是沒有默認值的；往數據表中插入列，又沒有給 TIMESTAMP 列賦值時，如果是嚴格 SQL 模式，會拋出一個錯誤，如果嚴格 SQL 模式沒有啟用，該列會賦值為 ```'0000-00-00 00:00:00'``` ，同時出現一個警告(和 MySQL 處理其他時間類型數據一樣，如 DATETIME)。

顯然，通過該參數關閉了 timestamp 類型字段所擁有的一些會讓人感到奇怪的默認行為，如果仍需要默認行為，則需要在建表的時候指定。

## 外鍵

<!--
在使用 foreign key 時需要遵守以下幾點規則：

1，有外鍵約束的表，必須是innodb型
2，外鍵約束的二個表，本來就相關係的表，並且要有索引關係，如果沒有，創建外鍵時也可以創建索引。
3，不支持對外鍵列的索引前綴。這樣的後果之一是BLOB和TEXT列不被包括在一個外鍵中，這是因為對這些列的索引必須總是包含一個前綴長度。
4，mysql外鍵的名子在數據庫內要是唯一的
-->

創建外鍵的語法規則如下：

{% highlight text %}
[CONSTRAINT symbol] FOREIGN KEY [id] (index_col_name, ...)
    REFERENCES tbl_name (index_col_name, ...)
    [ON DELETE {RESTRICT | CASCADE | SET NULL | NO ACTION}]
    [ON UPDATE {RESTRICT | CASCADE | SET NULL | NO ACTION}]
{% endhighlight %}

外鍵維護數據完整性的 5 種方式：

1. CASCADE: 從父表刪除或更新且自動刪除或更新子表中匹配的行，可以為 `ON DELETE CASCADE` 和 `ON UPDATE CASCADE` ；
2. SET NULL: 從父表刪除或更新行，設置子表中的外鍵列為 NULL，需要對應列也可為 NULL 。
3. NO ACTION: SQL-92 標準中，如果有外鍵的，那麼刪除或更新主鍵時會報錯；
4. RESTRICT: 拒絕對父表的刪除或更新操作。一些數據庫有延期檢查，一般 NO ACTION 是一個延期檢查；而在 MySQL 中，外鍵約束是被立即檢查的，所以 RESTRICT 與上述同樣。
5. SET DEFAULT: 這個動作被解析程序識別，但 InnoDB 不支持包含 `ON DELETE SET DEFAULT` 或 `ON UPDATE SET DEFAULT` 子句的表定義。

如下是測試用例：

{% highlight text %}
----- 創建用戶表
mysql> CREATE TABLE `user` (
  `id` int(11) NOT NULL auto_increment COMMENT '用戶ID',
  `name` varchar(50) NOT NULL default '' COMMENT '名字',
  `sex` int(1) NOT NULL default '0' COMMENT '0為男，1為女',
  PRIMARY KEY  (`id`)
) ENGINE=innodb DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
----- 寫入測試數據
mysql> INSERT INTO user(name, sex) VALUES("andy", 0),("lily", 1);

----- 創建訂單表
mysql> CREATE TABLE `order` (
  `order_id` INT(11) NOT NULL AUTO_INCREMENT COMMENT '訂單ID',
  `u_id` INT(11) NOT NULL DEFAULT '0' COMMENT '用戶ID',
  `username` VARCHAR(50) NOT NULL DEFAULT '' COMMENT '用戶名',
  `money` INT(11) NOT NULL DEFAULT '0' COMMENT '錢數',
  `datetime` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '生成時間',
  PRIMARY KEY(`order_id`),
  INDEX (`u_id`),
  FOREIGN KEY order_f_key (u_id) REFERENCES user(id)
) ENGINE=innodb DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
mysql> INSERT INTO `order`(`u_id`, `username`, `money`, `datetime`)
       VALUES('1', 'andy','10', CURRENT_TIMESTAMP);

----- 由於order表中有依賴，導致刪除user表時失敗
mysql> DELETE FROM user WHERE id =1;
----- 寫入數據時由於user表中不存在，導致寫入失敗
mysql> INSERT INTO `order`(`u_id`, `username`, `money`, `datetime`)
       VALUES('5', 'foobar', '123', CURRENT_TIMESTAMP);

----- MySQL中沒有修改外鍵操作，需要先刪除再重新添加
mysql> ALTER TABLE `order` DROP FOREIGN KEY order_ibfk_1;
mysql> ALTER TABLE `order` ADD FOREIGN KEY(u_id) REFERENCES user(id)
       ON DELETE CASCADE ON UPDATE CASCADE;

mysql> REPLACE INTO `order`(`u_id`, `username`, `money`, `datetime`)
       VALUES('2', 'andy','10', CURRENT_TIMESTAMP);
{% endhighlight %}

## 其它

### 註釋

在 MySQL 中可以通過 ```#``` (從開始到結束)、```/* ... */``` (可以多行) 表示註釋內容；從 MySQL-3.23 版本開始，可以通過 ```/*! ... */``` 這種特有的註釋方式標示 MySQL 特有特性。

MySQL 會解析其中的關鍵字，而其它數據庫則會視為註釋，從而保證兼容性。另外，從 3.23 開始支持 ```"-- "``` 格式的註釋，功能與 ```#``` 類似；需要注意的是，雙短劃線之後有一個空格。

接下來，重點說下 ```/*! ... */``` 這個註釋，簡言之，MySQL 會執行其中的內容，例如，MySQL 支持 STRAIGHT_JOIN 寫法，而其它數據庫可能不支持，那麼就可以寫成 ```SELECT /*! STRAIGHT_JOIN */ col1 FROM table1,table2 WHERE...``` 這種方式。

另外，可以在歎號後面添加版本號，表示只有當 MySQL 的版本大於等於該版本時才會執行，例如只有 MySQL-3.23.02 之後才支持 TEMPORARY，那麼可以寫為 ```CREATE /*!32302 TEMPORARY */ TABLE t (a INT);``` 。

其它的示例 ```CREATE DATABASE `blog` /*!40100 DEFAULT CHARACTER SET latin1 */;``` 。

<!--
### 字符串操作函數

CONCAT(str1,str2,…)返回連接參數產生的字符串，返回值的規則如下：

1. 如有任何一個參數為NULL，則返回值為NULL

mysql> select concat("foobar", null);
+------------------------+
| concat("foobar", null) |
+------------------------+
| NULL                   |
+------------------------+
1 row in set (0.00 sec)
2. 如果所有參數均為非二進制字符串，則結果為非二進制字符串；
3. 如果自變量中含有任一二進制字符串，則結果為一個二進制字符串；
4. 一個數字參數被轉化為與之相等的二進制字符串格式；若要避免這種情況，可使用顯式類型 cast, 例如： SELECT CONCAT(CAST(int_col AS CHAR), char_col)。

CONCAT_WS(separator,str1,str2,…)
CONCAT_WS()是CONCAT With Separator的簡寫，第一個參數是其它參數的分隔符，可以是字符串或者其它類型，如果為NULL，則結果為NULL；另外，函數會忽略任何分隔符參數後的NULL值。
mysql> SELECT CONCAT_WS(',', 'First Name', 'Second Name', 'Last Name') AS name;
+----------------------------------+
| name                             |
+----------------------------------+
| First Name,Second Name,Last Name |
+----------------------------------+
1 row in set (0.00 sec)
mysql> SELECT CONCAT_WS(',', 'First Name', NULL, 'Last Name') AS name;
+----------------------+
| name                 |
+----------------------+
| First Name,Last Name |
+----------------------+
1 row in set (0.00 sec)

group_concat
-->


### sql_mode

sql_mode 是個很容易被忽視的變量，在生產環境必須將這個值設置為嚴格模式，所以開發、測試環境的數據庫也必須要設置，這樣在開發測試階段就可以發現問題

{% highlight text %}
----- 查看變量
mysql> SHOW VARIABLES LIKE 'sql_mode%'\G
 *************************** 1. row ***************************
 Variable_name: sql_mode
         Value: ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,
         NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION
         1 row in set (0.01 sec)

----- 設置變量
mysql> SET GLOBAL sql_mode='';
{% endhighlight %}

簡單列舉如下常用的，詳細可以參考官方手冊 [MySQL Reference Manual - Server SQL Modes](https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html) 。 <!--https://segmentfault.com/a/1190000005936172-->

* NO_ZERO_IN_DATE<br>在嚴格模式下，不允許日期和月份為零。
* NO_ZERO_DATE<br>不允許插入零日期，插入零日期會拋出錯誤而不是警告。
* NO_AUTO_CREATE_USER<br>禁止 GRANT 創建密碼為空的用戶。
* ONLY_FULL_GROUP_BY<br>參考如下。

#### ONLY_FULL_GROUP_BY

對於 ```GROUP BY``` 聚合操作，如果在 SELECT 中的列沒有在 ```GROUP BY``` 中出現 (可以是聚合函數)，那麼認為這個 SQL 是不合法的。

例如 ```select language_id, count(1), length from film group by language_id;``` 是不合法的，原因是 MySQL 允許在 SELECT 列表中存在除聚集函數或 ```GROUP BY``` 以外的表達式，但是這些表達式的結果並不確定，通常也就沒有太多意義，反而可能會導致錯誤。

實際上 SQLServer、Oracle、PostgreSQL 基本都不支持這種語法，為此，MySQL 5.7 修訂了這一語義。另外，需要注意 ```SELECT id+1 FROM tt GROUP BY 1+id;``` 這種是不允許的，而 ```SELECT id+1 FROM tt GROUP BY id+1;``` 則可以。

### show tables

默認在使用 LIKE 過濾時，不能使用 AND/OR 等條件，可以通過如下方式添加過濾條件。

{% highlight text %}
----- 只能使用單個like語句
mysql> SHOW TABLES LIKE 'host%';

----- 如果要使用多個，需要使用WHERE子句，其中FROM子句可以省略
mysql> SHOW TABLES FROM `sys` WHERE `Tables_in_sys` LIKE 'user%' OR `Tables_in_sys` LIKE 'host%';
{% endhighlight %}

<!--
select * from mysql.user where user like 'root%' and host like 'local%';
%匹配多個任意字符；_匹配單個任意字符；
也可以使用正則表達式，也即[NOT] REGEXP操作符或者[NOT] RLIKE。
-->

<!---
http://blog.csdn.net/imzoer/article/details/8277797
-->

## 參考

[MySQL Reference Manual - Server SQL Modes](https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html)


{% highlight text %}
{% endhighlight %}
