---
title: InnoDB 隔離級別
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,隔離級別,鎖
description: 我們知道，數據庫的事務存在著 ACID 四個屬性，而在實際應用場景中，當面臨著性能需求時，隔離性往往成為最容易妥協的一個。為了獲取更高的隔離等級，數據庫系統的鎖機制或者多版本併發控制機制都會影響併發；而低的隔離級別可以增加事務的併發，但同時會存在著風險。在本文章中，詳細介紹下 InnoDB 中關於隔離級別的實現。
---

我們知道，數據庫的事務存在著 ACID 四個屬性，而在實際應用場景中，當面臨著性能需求時，隔離性往往成為最容易妥協的一個。

為了獲取更高的隔離等級，數據庫系統的鎖機制或者多版本併發控制機制都會影響併發；而低的隔離級別可以增加事務的併發，但同時會存在著風險。

在本文章中，詳細介紹下 InnoDB 中關於隔離級別的實現。

<!-- more -->

## 簡介

事務隔離級別 (transaction isolation levels)，隔離級別就是對對事務併發控制的等級。

很多 DBMS 定義了不同的 "事務隔離等級" 來控制鎖的程度，多數的數據庫事務都避免高等級的隔離等級 (如可序列化) 從而減少對系統的鎖的開銷，高的隔離級別往往會增加死鎖發生的機率。

同時，當降低事務的隔離級別時，程序員需要小心的分析數據庫訪問部分的代碼，以保證不會造成難以發現的代碼 bug。

<!--
ANSI/ISO SQL 將其分為四個等級，為了實現隔離級別通常數據庫採用鎖 (Lock)，一般在編程的時候只需要設置隔離等級，至於具體採用什麼鎖則由數據庫來設置。

通常在數據庫中會出現如下的錯誤，在講解時採用的是讀寫鎖，加讀鎖(shared-lock)後其它事務可以讀取，但是無法寫入；加寫鎖(exclusive-lock)後其它事務無法讀取，也無法寫入。
-->

### 常用命令

InnoDB 默認是可重複讀的 (REPEATABLE READ)，提供 SQL-92 標準所描述的所有四個事務隔離級別，可以在啟動時用 \-\-transaction-isolation 選項設置，也可以配置文件中設置。

{% highlight text %}
$ cat /etc/my.cnf
[mysqld]
transaction-isolation = {READ-UNCOMMITTED | READ-COMMITTED | REPEATABLE-READ | SERIALIZABLE}
{% endhighlight %}

用戶可以用 ```SET TRANSACTION``` 語句改變單個會話或者所有新進連接的隔離級別，語法如下：

{% highlight text %}
mysql> SET autocommit=0;
mysql> SET [SESSION | GLOBAL] TRANSACTION ISOLATION LEVEL
       {READ UNCOMMITTED | READ COMMITTED | REPEATABLE READ | SERIALIZABLE}
{% endhighlight %}

如果使用 GLOBAL 關鍵字，當然需要 SUPER 權限，則從設置時間點開始創建的所有新連接均採用該默認事務級別，不過原有鏈接事務隔離級別不變。

可以用下列語句查詢全局和會話事務隔離級別。

{% highlight text %}
mysql> SHOW VARIABLES LIKE 'tx_isolation';
mysql> SELECT @@global.tx_isolation;
mysql> SELECT @@session.tx_isolation;
mysql> SELECT @@tx_isolation;
{% endhighlight %}

<!--
接下來，介紹下 InnoDB 中與事務相關的命令。
{% highlight text %}
mysql> create table user (id int primary key, name varchar(20), age int unsigned) engine=innodb;
mysql> insert into user values (1, 'andy', 28);
{% endhighlight %}
-->


### 讀取異常

在 [SQL 92](http://www.contrib.andrew.cmu.edu/~shadow/sql/sql1992.txt) 規範的定義中，規定了四種隔離級別，同時對可能出現的三種現象進行了說明（不包含如下的丟失更新）。

#### Lost Update

丟失更新，當兩個事務讀取相同數據，然後都嘗試更新原來的數據成新的值，此時，第二個事務可能完全覆蓋掉第一個所完成的更新。

丟失更新是唯一一個用戶可能在所有情況下都想避免的行為，在 SQL 92 中甚至沒有提及。

#### Dirty Read

髒讀，一個事務中讀取到另一個事務未提交的數據。例如，事務 T1 讀取到另一個事務 T2 **未提交的數據**，如果 T2 回滾，則 T1 相當於讀取到了一個被認為不可能出現的值。

#### Non-Repeatable Read

不可重複讀，在一個事務中，當重複讀取同一條記錄時，發現該記錄的結果不同或者已經被刪除了；如在事務 T1 中讀取了一行，接著 T2 修改或者刪除了該行 **並提交**，那麼當 T1 嘗試讀取新的值時，就會發現改行的值已經修改或者被刪除。

<!--
此時，寫鎖會一直保持到事務結束，讀鎖在select之後立即釋放掉。
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
<tr><td><small>/* Query 1 */SELECT * FROM users WHERE id = 1;讀鎖-寫鎖-範圍鎖</small></td><td></td></tr>
<tr><td></td><td><small>/* Query 2 */UPDATE users SET age = 21 WHERE id = 1; COMMIT;<br>/* in multiversion concurrency control, or lock-based READ COMMITTED */</small></td></tr>　
<tr><td><small>/* Query 1 */SELECT * FROM users WHERE id = 1; COMMIT;<br>/* lock-based REPEATABLE READ */</small></td><td></td></tr>　
</tbody></table></center></li><br><li>
-->

#### Phantom

幻讀，通常是指在一個事務中，當重複查詢一個結果集時，返回的兩個不同的結果集，可能是由於另一個事務插入或者刪除了一些記錄。

例如，事務 T1 讀取一個結果集，T2 修改了該結果集中的部分記錄 (例如插入一條記錄)，T1 再次讀取時發現與之前的結果不同 (多出來一條記錄)，就像產生幻覺一樣。

<!--
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
<tr><td><small>/* Query 1 */讀鎖+寫鎖-範圍鎖 只鎖定了1，2 SELECT * FROM users WHERE age BETWEEN 10 AND 30;</small></td><td></td></tr>
<tr><td></td><td><small>/* Query 2 */ INSERT INTO users VALUES ( 3, 'Bob', 27 ); COMMIT;</small></td></tr>　
<tr><td><small>/* Query 1 */SELECT * FROM users WHERE age BETWEEN 10 AND 30;</small></td><td></td></tr>　
</tbody></table></center></li></ul>
-->

#### 其它

隔離級別與讀現象。

| 隔離級別 | 髒讀 | 不可重複讀取 | 幻影數據行 |
|:--------:|:----:|:------------:|:----------:|
|READ UNCOMMITTED|YES|YES|YES|
|READ COMMITTED|NO|YES|YES|
|REPEATABLE READ|NO|NO|YES|
|SERIALIZABLE|NO|NO|NO|

### 事務超時

與事務超時相關的變量可以參考。

{% highlight text %}
----- 設置鎖超時時間，單位為秒，默認50s
mysql> SHOW VARIABLES LIKE 'innodb_lock_wait_timeout';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| innodb_lock_wait_timeout | 50    |
+--------------------------+-------+
1 row in set (0.00 sec)

----- 超時後的行為，默認OFF，詳見如下介紹
mysql> SHOW VARIABLES LIKE 'innodb_rollback_on_timeout';
+----------------------------+-------+
| Variable_name              | Value |
+----------------------------+-------+
| innodb_rollback_on_timeout | OFF   |
+----------------------------+-------+
1 row in set (0.02 sec)
{% endhighlight %}

[innodb_rollback_on_timeout ](https://dev.mysql.com/doc/refman/5.7/en/innodb-parameters.html#sysvar_innodb_rollback_on_timeout) 變量默認值為 OFF，如果事務因為加鎖超時，會回滾上一條語句執行的操作；如果設置 ON，則整個事務都會回滾。

當上述變量為 OFF 時，也就是事務會回滾到上一個保存點，這是因為 InnoDB 在執行每條 SQL 語句之前，都會創建一個保存點，可以參見 row_insert_for_mysql() 函數中的代碼。

{% highlight text %}
row_insert_for_mysql()
 |-row_insert_for_mysql_using_ins_graph()
   |-trx_savept_take()
{% endhighlight %}

如果事務因為加鎖超時，相當於回滾到上一條語句，但是報錯後，事務還沒有完成，用戶可以選擇是繼續提交，或者回滾之前的操作，由用戶選擇是否進一步提交或者回滾事務。

上述參數為 ON 時，整個事務都回滾；詳細的內容可以從 row_mysql_handle_errors() 中驗證。

<!--
問題：innodb_rollback_on_timeout為OFF，事務的原子性被破壞了嗎？
答：NO，從示例中可以看到，事務只是回退上一條語句的狀態，而整個事務實際上沒有完成(提交或者回滾)，而作為應用程序在檢測這個錯誤時，應該選擇是提交或者回滾事務。如果嚴格要求事務的原子性，當然是執行ROLLBACK，回滾事務。
-->

### 其它

如何判斷當前會話已經開啟了一個事務？

{% highlight text %}
----- 可以直接使用在事務中會報錯的語句。
mysql> SELECT @@TX_ISOLATION;
mysql> SET TRANSACTION ISOLATION LEVEL REPEATABLE READ;

----- 或者通過如下SQL查看，如果在事務中則會返回當前的事務ID，否則返回為空。
mysql> SELECT trx_id FROM information_schema.innodb_trx WHERE trx_mysql_thread_id = connection_id();
{% endhighlight %}


## 隔離級別

先準備下環境，隔離級別在測試時，會針對不同的場景分別進行設置；另外，將 autocommit 設置為 0 ，此時 commit/rollback 後的一條語句會自動開啟一個新事務。

{% highlight text %}
----- 新建表並寫入數據
mysql> SET GLOBAL autocommit=0;
mysql> CREATE TABLE user (id INT PRIMARY KEY, name VARCHAR(20), age INT UNSIGNED) engine=InnoDB;
mysql> INSERT INTO user VALUES (1, 'andy', 28);

----- 設置隔離級別
mysql> SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;
{% endhighlight %}

一般來說，也顯然，高一級的級別可以提供更強的隔離性。

### READ UNCOMMITTED

也就是讀未提交/未授權讀，在此場景下 **允許髒讀**，也就是允許某個事務看到其它事務尚未提交的數據行改動，這是最低的隔離等級。

{% highlight text %}
### 準備數據
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
### 讀到的age為28
SELECT * FROM user WHERE id=1;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             ### 開啟一個事務，隔離級別任意，更新age
                                                             START TRANSACTION;
                                                             UPDATE user SET age=30 WHERE id=1;
### 讀到未提交數據，age為30
SELECT * FROM user WHERE id=1;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   30 |
+----+------+------+
1 row in set (0.00 sec)
                                                             ### 回滾事務
                                                             ROLLBACK;
### 讀到的age為28
SELECT * FROM user WHERE id=1;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
{% endhighlight %}

如上所示，在事務 A 中，會讀取到不同的 age 值。

### READ COMMITTED

也就是讀已提交/授權讀，此時不允許上述的髒讀，**允許不可重複讀**，也就是 Fuzzy Read (也被稱之為 Non-Repeatable Read)，也指一個事務內的兩次讀同一行看到的數據不一樣，"不可重複" 。

{% highlight text %}
### 準備數據
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             START TRANSACTION;
                                                             UPDATE user SET age=30 WHERE id=1;
                                                             SELECT * FROM user;
                                                             +----+------+------+
                                                             | id | name | age  |
                                                             +----+------+------+
                                                             |  1 | andy |   30 |
                                                             +----+------+------+
                                                             1 row in set (0.00 sec)
### 讀到的age仍然為28，沒有幻讀
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             COMMIT;
### 此時在事務B提交之後，讀到的age為30
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   30 |
+----+------+------+
1 row in set (0.00 sec)
COMMIT;
{% endhighlight %}

如上，事務 A 中同一條 SQL 會讀取到不同的數值，即使事務 B 中是刪除操作。

<!--
基於鎖機制併發控制的DBMS需要對選定對象的寫鎖(write locks)一直保持到事務結束，但是讀鎖(read locks)在SELECT操作完成後馬上釋放，可能會發生“不可重複讀”現象。

簡而言之，授權讀這種隔離級別保證了讀到的任何數據都是提交的數據，避免讀到中間的未提交的數據，髒讀(dirty reads)。但是在每次數據讀完之後其他事務可以修改剛才讀到的數據，從而導致不可重複讀。

也就是說，事務的讀取數據的時候獲取讀鎖，但是讀完之後立即釋放 (不需要等到事務結束)，而寫鎖則是事務提交之後才釋放。釋放讀鎖之後，就可能被其他事物修改數據。
-->


### REPEATABLE READ

也即可重複讀，InnoDB 默認的隔離級別，此時不允許髒讀、不可重複讀，但是允許 **幻讀** 。

<!--
基於鎖機制併發控制需要對選定對象的讀鎖(read locks)和寫鎖(write locks)一直保持到事務結束，但不要求“範圍鎖(range-locks)”，因此可能會發生“幻影讀(phantom reads)”。如果某個事務兩次執行同一個 SELECT 語句，其結果是可重複的。所有被 Select 獲取的數據都不能被修改，這樣就可以避免一個事務前後讀取數據不一致的情況。但是卻沒有辦法控制幻讀，因為這個時候其他事務不能更改所選的數據，但是可以增加數據，因為前一個事務沒有範圍鎖。
-->

{% highlight text %}
### 準備數據
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             START TRANSACTION;
                                                             UPDATE user SET age=30 WHERE id=1;
                                                             SELECT * FROM user;
                                                             +----+------+------+
                                                             | id | name | age  |
                                                             +----+------+------+
                                                             |  1 | andy |   30 |
                                                             +----+------+------+
                                                             1 row in set (0.00 sec)
### 讀到的age仍然為28，沒有幻讀
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             COMMIT;
### 讀到的age仍然為28，沒有不可重複讀
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
COMMIT;

### 此時讀到的age為30
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   30 |
+----+------+------+
1 row in set (0.00 sec)
{% endhighlight %}

再看一個出現幻讀的情況。

{% highlight text %}
### 準備數據
DELETE FROM user;
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user;
Empty set (0.00 sec)
                                                             START TRANSACTION;
                                                             INSERT INTO user VALUES (1, 'andy', 28);
                                                             COMMIT;
### 此時查詢仍然為空
SELECT * FROM user;
Empty set (0.00 sec)
### 嘗試插入數據時報錯，TMD明明說沒有這條記錄的
INSERT INTO user VALUES (1, 'andy', 28);
ERROR 1062 (23000): Duplicate entry '1' for key 'PRIMARY'
COMMIT;


START TRANSACTION;
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             START TRANSACTION;
                                                             INSERT INTO user VALUES (2, 'cassie', 25);
                                                             COMMIT;
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
### 不是說有一條記錄嗎，怎麼多出來一條啊！！！
UPDATE user SET age=10;
Query OK, 2 rows affected (0.00 sec)
Rows matched: 2  Changed: 2  Warnings: 0
{% endhighlight %}


<!--
----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
select * from user where id > 0 and id < 5;
                                                              START TRANSACTION;
                                                              INSERT INTO user VALUES(3, 'bob', 22);
### 可以正常查詢
SELECT * FROM USER WHERE ID > 0 AND ID < 5;
### 如下查詢，會導致鎖超時
SELECT * FROM USER WHERE ID > 0 AND ID < 5 LOCK IN SHARE MODE;
SELECT * FROM USER WHERE ID > 0 AND ID < 5 FOR UPDATE;
UPDATE USER SET AGE = 50 WHERE ID = 3;
COMMIT;

InnoDB 提供了這樣的機制，在可重複讀的隔離級別裡，可以使用加鎖讀去查詢最新的數據。此時，已經存在 id=3 的數據，如果常是插入，則會報錯。
-->

#### Snapshot Read

另外，在 Repeatable Read 隔離級別下，如果使用 Snapshot Read 實現時，允許某些 Phantom 現象，簡單來說就是第二次讀到了第一次沒有的行數據。

{% highlight text %}
### 準備數據
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             START TRANSACTION;
                                                             UPDATE user SET age=30 WHERE id=1;
                                                             SELECT * FROM user;
                                                             +----+------+------+
                                                             | id | name | age  |
                                                             +----+------+------+
                                                             |  1 | andy |   30 |
                                                             +----+------+------+
                                                             1 row in set (0.00 sec)
                                                             COMMIT;
UPDATE user SET name='andrew' WHERE id=1;
### 對於快照讀，事務有了修改之後，就可以讀到age為30
SELECT * FROM user;
+----+--------+------+
| id | name   | age  |
+----+--------+------+
|  1 | andrew |   30 |
+----+--------+------+
1 row in set (0.00 sec)
                                                             ### 不過此時讀取到的仍然是andy
                                                             SELECT * FROM user;
                                                             +----+------+------+
                                                             | id | name | age  |
                                                             +----+------+------+
                                                             |  1 | andy |   30 |
                                                             +----+------+------+
                                                             1 row in set (0.00 sec)
COMMIT;
                                                             ### OK，終於讀取到了最新的數據
                                                             SELECT * FROM user;
                                                             +----+--------+------+
                                                             | id | name   | age  |
                                                             +----+--------+------+
                                                             |  1 | andrew |   30 |
                                                             +----+--------+------+
                                                             1 row in set (0.00 sec)
{% endhighlight %}

#### Write Skew

Repeatable Read 允許 Write Skew，該異常主要是針對多行事務。

簡單來說，就是如果事務 A 讀取了行 X，並因此修改了 Y，然後提交；同時事務 B 讀取了行 Y，並修改了 X ；那麼此時的最終結果可能違反 X 和 Y 相關的某些約束。

{% highlight text %}
### 準備數據
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28),(2, 'cassie', 25);
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user WHERE id=1;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             START TRANSACTION;
                                                             SELECT * FROM user WHERE id=2;
                                                             +----+--------+------+
                                                             | id | name   | age  |
                                                             +----+--------+------+
                                                             |  2 | cassie |   25 |
                                                             +----+--------+------+
                                                             1 row in set (0.00 sec)
UPDATE user SET age=15 WHERE id=2;
SELECT * FROM user;
+----+--------+------+
| id | name   | age  |
+----+--------+------+
|  1 | andy   |   28 |
|  2 | cassie |   15 |
+----+--------+------+
2 rows in set (0.00 sec)
                                                             UPDATE user SET age=18 WHERE id=1;
                                                             SELECT * FROM user;
                                                             +----+--------+------+
                                                             | id | name   | age  |
                                                             +----+--------+------+
                                                             |  1 | andy   |   18 |
                                                             |  2 | cassie |   25 |
                                                             +----+--------+------+
                                                             2 rows in set (0.00 sec)
                                                             COMMIT;
COMMIT;
SELECT * FROM user;
+----+--------+------+
| id | name   | age  |
+----+--------+------+
|  1 | andy   |   18 |
|  2 | cassie |   15 |
+----+--------+------+
2 rows in set (0.00 sec)
{% endhighlight %}



<!--
#### 其它


當隔離級別是可重複讀，且禁用innodb_locks_unsafe_for_binlog的情況下，在搜索和掃描index的時候使用的next-key locks可以避免幻讀。

再看一個實驗，要注意，表t_bitfly裡的id為主鍵字段。
複製代碼

實驗三：
t Session A                 Session B
|
| START TRANSACTION;        START TRANSACTION;
|
| SELECT * FROM t_bitfly
| WHERE id<=1
| FOR UPDATE;
| +------+-------+
| | id   | value |
| +------+-------+
| |    1 | a     |
| +------+-------+
|                           INSERT INTO t_bitfly
|                           VALUES (2, 'b');
|                           Query OK, 1 row affected
|
| SELECT * FROM t_bitfly;
| +------+-------+
| | id   | value |
| +------+-------+
| |    1 | a     |
| +------+-------+
|                           INSERT INTO t_bitfly
|                           VALUES (0, '0');
|                           (waiting for lock ...then timeout)
|                           ERROR 1205 (HY000):
|                           Lock wait timeout exceeded;
|                           try restarting transaction
|
| SELECT * FROM t_bitfly;
| +------+-------+
| | id   | value |
| +------+-------+
| |    1 | a     |
| +------+-------+
|                           COMMIT;
|
| SELECT * FROM t_bitfly;

| +------+-------+
| | id   | value |
| +------+-------+
| |    1 | a     |
| +------+-------+

複製代碼

可以看到，用id<=1加的鎖，只鎖住了id<=1的範圍，可以成功添加id為2的記錄，添加id為0的記錄時就會等待鎖的釋放。
複製代碼

實驗四：一致性讀和提交讀
t Session A                      Session B
|
| START TRANSACTION;             START TRANSACTION;
|
| SELECT * FROM t_bitfly;
| +----+-------+
| | id | value |
| +----+-------+
| |  1 | a     |
| +----+-------+
|                                INSERT INTO t_bitfly
|                                VALUES (2, 'b');
|                                COMMIT;
|
| SELECT * FROM t_bitfly;
| +----+-------+
| | id | value |
| +----+-------+
| |  1 | a     |
| +----+-------+
|
| SELECT * FROM t_bitfly LOCK IN SHARE MODE;
| +----+-------+
| | id | value |
| +----+-------+
| |  1 | a     |
| |  2 | b     |
| +----+-------+
|
| SELECT * FROM t_bitfly FOR UPDATE;
| +----+-------+
| | id | value |
| +----+-------+
| |  1 | a     |
| |  2 | b     |
| +----+-------+
|
| SELECT * FROM t_bitfly;
| +----+-------+
| | id | value |
| +----+-------+
| |  1 | a     |
| +----+-------+

複製代碼

如果使用普通的讀，會得到一致性的結果，如果使用了加鎖的讀，就會讀到“最新的”“提交”讀的結果。

本身，可重複讀和提交讀是矛盾的。在同一個事務裡，如果保證了可重複讀，就會看不到其他事務的提交，違背了提交讀；如果保證了提交讀，就會導致前後兩次讀到的結果不一致，違背了可重複讀。

可以這麼講，InnoDB提供了這樣的機制，在默認的可重複讀的隔離級別裡，可以使用加鎖讀去查詢最新的數據（提交讀）。
MySQL InnoDB的可重複讀並不保證避免幻讀，需要應用使用加鎖讀來保證。而這個加鎖度使用到的機制就是next-key locks。
-->












### SERIALIZABLE

也就是串行化/可序列化，不允許上述的異常情況，包括 Phantom 和 Write Skew 以及任何不可串行化的反常情況。

{% highlight text %}
### 準備數據
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28),(2, 'cassie', 25);
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user;
+----+--------+------+
| id | name   | age  |
+----+--------+------+
|  1 | andy   |   28 |
|  2 | cassie |   25 |
+----+--------+------+
2 rows in set (0.00 sec)
                                                             START TRANSACTION;
                                                             SELECT * FROM user;
                                                             +----+--------+------+
                                                             | id | name   | age  |
                                                             +----+--------+------+
                                                             |  1 | andy   |   28 |
                                                             |  2 | cassie |   25 |
                                                             +----+--------+------+
                                                             2 rows in set (0.00 sec)
SELECT * FROM user WHERE id=2;
+----+--------+------+
| id | name   | age  |
+----+--------+------+
|  2 | cassie |   25 |
+----+--------+------+
1 row in set (0.00 sec)
                                                             ### 此時嘗試更新時會等待直到鎖超時
                                                             UPDATE user SET age=15 WHERE id=2;
COMMIT;
                                                             ### 重新嘗試提交
                                                             UPDATE user SET age=15 WHERE id=2;
                                                             SELECT * FROM user;
                                                             +----+--------+------+
                                                             | id | name   | age  |
                                                             +----+--------+------+
                                                             |  1 | andy   |   28 |
                                                             |  2 | cassie |   15 |
                                                             +----+--------+------+
                                                             2 rows in set (0.00 sec)
                                                             COMMIT;
{% endhighlight %}

接下來，再看個示例。

{% highlight text %}
### 準備數據
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同時設置兩個事務的隔離級別
SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user;
+----+--------+------+
| id | name   | age  |
+----+--------+------+
|  1 | andy   |   28 |
+----+--------+------+
1 rows in set (0.00 sec)
                                                             START TRANSACTION;
                                                             SELECT * FROM user;
                                                             +----+--------+------+
                                                             | id | name   | age  |
                                                             +----+--------+------+
                                                             |  1 | andy   |   28 |
                                                             |  2 | cassie |   25 |
                                                             +----+--------+------+
                                                             2 rows in set (0.00 sec)
                                                             ### 此時嘗試更新時會等待直到鎖超時
                                                             UPDATE user SET age=15 WHERE id=2;
                                                             ### 同上，仍然鎖等待超時
                                                             INSERT INTO user VALUES (2, 'cassie', 25);
{% endhighlight %}



<!--
某個事物正在查看的數據行不允許其他事務修改，直到該事務完成為止，也就是說需要同時獲得讀鎖+寫鎖，當select查詢使用where字句時需要獲得一個範圍鎖。所有事務都一個接一個地串行執行，這樣可以避免幻讀 (phantom reads)。如果不是基於鎖實現併發控制的數據庫，則檢查到有違反串行操作的事務時(如寫衝突)，需要回滾該事務，此時只有一個事務可以提交成功，詳見"快照隔離"。
-->



<!--
在基於鎖的併發控制中，隔離級別決定了鎖的持有時間。"C"-表示鎖會持續到事務提交。 "S" –表示鎖持續到當前語句執行完畢。如果鎖在語句執行完畢就釋放則另外一個事務就可以在這個事務提交前修改鎖定的數據，從而造成混亂。
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1">
<tbody><tr bgcolor="lightblue" align="center"><td>隔離級別</td><td>讀操作</td><td>寫操作</td><td>範圍操作</td></tr>
<tr align="center"><td>READ UNCOMMITTED</td><td>S</td><td>S</td><td>S</td></tr>
<tr align="center"><td>READ COMMITTED</td><td>C</td><td>S</td><td>S</td></tr>
<tr align="center"><td>REPEATABLE READ</td><td>C</td><td>C</td><td>S</td></tr>
<tr align="center"><td>SERIALIZABLE</td><td>C</td><td>C</td><td>C</td></tr>
</tbody></table></center><br>



多個事務在同時運行時可能會發生各種各樣的問題，如下：<ul><li>
髒讀(dirty read)。某個事務所做的修改在它尚未提交時就可以被其它事務看到，修改的數據也有可能回滾，因此會造成數據的不一致。</li><br><li>

不可重複讀取(nonrepeatable read)。同一個事務使用同一條 SELECT 語句每次讀取到的結果不一樣。</li><br><li>

幻影數據行(phantom row)。一個事務突然看到了一個它以前沒有見過數據行。
</li></ul>




<ul><li>
髒讀：如果一個事務A對數據進行了更改，但是還沒有提交，而另一個事務B就可以讀到事務A尚未提交的更新結果。這樣，當事務A進行回滾時，那麼事務B開始讀到的數據就是一筆髒數據。
</li></ul>





	<ul><li>
		可重複讀，可產生幻讀 (phantom reads)，也可以歸於不可重複讀<br><br>
		一個事務中先後各執行一次同一個查詢，但是返回的結果集卻不一樣。發生這種情況是因為在執行 Select 操作的時候沒有獲取範圍鎖 (Range Lock)，導致其他事務仍然可以插入或者刪除新的數據。
		<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
		<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
		<tr><td><small>/* Query 1 */</small><br>SELECT * FROM users WHERE age BETWEEN 10 AND 30;</td><td></td></tr>
		<tr><td></td><td><small>/* Query 2 */</small><br>INSERT INTO users VALUES ( 3, 'Bob', 27 );<br>COMMIT;</td></tr>　
		<tr><td><small>/* Query 3 */</small><br>SELECT * FROM users WHERE age BETWEEN 10 AND 30;</td><td></td></tr>　
		</tbody></table></center><br>

		注意 transaction 1 對同一個查詢語句（Query 1）執行了兩次。 如果採用更高級別的隔離等級（即串行化）的話，那麼前後兩次查詢應該返回同樣的結果集。但是在可重複讀隔離等級中卻前後兩次結果集不一樣。但是為什麼叫做可重複讀等級呢？那是因為該等級解決了下面的不可重複讀問題。</li><br><li>



		讀已提交，可產生不可重複讀 (Non-repeatable reads)<br><br>
		在採用鎖來實現併發控制的數據庫系統中，不可重複讀是因為在執行 Select 操作的時候沒有加讀鎖 (read lock)。

		<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
		<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
		<tr><td><small>/* Query 1 */</small><br>SELECT * FROM users WHERE id = 1;</td><td></td></tr>
		<tr><td></td><td><small>/* Query 2 */</small><br>UPDATE users SET age = 21 WHERE id = 1;<br>COMMIT;</td></tr>　
		<tr><td><small>/* Query 3 */</small><br>SELECT * FROM users WHERE id = 1;</td><td></td></tr>　
		</tbody></table></center><br>

		在這個例子當中，Transaction 2提交成功,所以Transaction 1第二次將獲取一個不同的age 值.在SERIALIZABLE和REPEATABLE READ隔離級別中,數據庫應該返回同一個值。而在READ COMMITTED和READ UNCOMMITTED級別中數據庫返回更新的值。這樣就出現了不可重複讀。



		讀未提交，可造成髒讀 (dirty reads)<br><br>
		如果一個事務 2 讀取了另一個事務 1 修改的值，但是最後事務 1 回滾了，那麼事務 2 就讀取了一個髒數據，這也就是所謂的髒讀。發生這種情況就是允許事務讀取未提交的更新。
		<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
		<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
		<tr><td></td><td><small>/* Query 1 */</small><br>UPDATE users SET age = 21 WHERE id = 1;</td></tr>　
		<tr><td><small>/* Query 2 */</small><br>SELECT * FROM users WHERE id = 1;</td><td></td></tr>
		<tr><td></td><td><small>/* Query 1 */</small><br>Roll Back</td></tr>　
		</tbody></table></center></li><br><li>

		第一類更新丟失<br><br>
		這是事物隔離級別最低時出現的問題，既不發出共享鎖，也不接受排它鎖。兩個事務更新相同的數據資源時，如果一個事務被提交，一個事務被撤銷，則提交的事務所做的修改也將被撤銷；或者兩個事務同時提交，導致第一次的修改被覆蓋。
	</li></ul>
-->






## 參考

關於數據庫的事務隔離級別可以參考 [WikiPedia - Isolation (database systems)](https://en.wikipedia.org/wiki/Isolation_%28database_systems%29) 中的介紹。


<!--
[深入理解數據庫事務隔離級別](/reference/databases/mysql/mysql_isolation_details.pptx)

http://docs.oracle.com/cd/B19306_01/server.102/b14220/consist.htm
<a href="http://book.51cto.com/art/200710/58623.htm">http://book.51cto.com/art/200710/58623.htm</a></li><br><li>
-->

{% highlight text %}
{% endhighlight %}
