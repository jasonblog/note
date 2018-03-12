---
title: InnoDB 隔离级别
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,隔离级别,锁
description: 我们知道，数据库的事务存在着 ACID 四个属性，而在实际应用场景中，当面临着性能需求时，隔离性往往成为最容易妥协的一个。为了获取更高的隔离等级，数据库系统的锁机制或者多版本并发控制机制都会影响并发；而低的隔离级别可以增加事务的并发，但同时会存在着风险。在本文章中，详细介绍下 InnoDB 中关于隔离级别的实现。
---

我们知道，数据库的事务存在着 ACID 四个属性，而在实际应用场景中，当面临着性能需求时，隔离性往往成为最容易妥协的一个。

为了获取更高的隔离等级，数据库系统的锁机制或者多版本并发控制机制都会影响并发；而低的隔离级别可以增加事务的并发，但同时会存在着风险。

在本文章中，详细介绍下 InnoDB 中关于隔离级别的实现。

<!-- more -->

## 简介

事务隔离级别 (transaction isolation levels)，隔离级别就是对对事务并发控制的等级。

很多 DBMS 定义了不同的 "事务隔离等级" 来控制锁的程度，多数的数据库事务都避免高等级的隔离等级 (如可序列化) 从而减少对系统的锁的开销，高的隔离级别往往会增加死锁发生的几率。

同时，当降低事务的隔离级别时，程序员需要小心的分析数据库访问部分的代码，以保证不会造成难以发现的代码 bug。

<!--
ANSI/ISO SQL 将其分为四个等级，为了实现隔离级别通常数据库采用锁 (Lock)，一般在编程的时候只需要设置隔离等级，至于具体采用什么锁则由数据库来设置。

通常在数据库中会出现如下的错误，在讲解时采用的是读写锁，加读锁(shared-lock)后其它事务可以读取，但是无法写入；加写锁(exclusive-lock)后其它事务无法读取，也无法写入。
-->

### 常用命令

InnoDB 默认是可重复读的 (REPEATABLE READ)，提供 SQL-92 标准所描述的所有四个事务隔离级别，可以在启动时用 \-\-transaction-isolation 选项设置，也可以配置文件中设置。

{% highlight text %}
$ cat /etc/my.cnf
[mysqld]
transaction-isolation = {READ-UNCOMMITTED | READ-COMMITTED | REPEATABLE-READ | SERIALIZABLE}
{% endhighlight %}

用户可以用 ```SET TRANSACTION``` 语句改变单个会话或者所有新进连接的隔离级别，语法如下：

{% highlight text %}
mysql> SET autocommit=0;
mysql> SET [SESSION | GLOBAL] TRANSACTION ISOLATION LEVEL
       {READ UNCOMMITTED | READ COMMITTED | REPEATABLE READ | SERIALIZABLE}
{% endhighlight %}

如果使用 GLOBAL 关键字，当然需要 SUPER 权限，则从设置时间点开始创建的所有新连接均采用该默认事务级别，不过原有链接事务隔离级别不变。

可以用下列语句查询全局和会话事务隔离级别。

{% highlight text %}
mysql> SHOW VARIABLES LIKE 'tx_isolation';
mysql> SELECT @@global.tx_isolation;
mysql> SELECT @@session.tx_isolation;
mysql> SELECT @@tx_isolation;
{% endhighlight %}

<!--
接下来，介绍下 InnoDB 中与事务相关的命令。
{% highlight text %}
mysql> create table user (id int primary key, name varchar(20), age int unsigned) engine=innodb;
mysql> insert into user values (1, 'andy', 28);
{% endhighlight %}
-->


### 读取异常

在 [SQL 92](http://www.contrib.andrew.cmu.edu/~shadow/sql/sql1992.txt) 规范的定义中，规定了四种隔离级别，同时对可能出现的三种现象进行了说明（不包含如下的丢失更新）。

#### Lost Update

丢失更新，当两个事务读取相同数据，然后都尝试更新原来的数据成新的值，此时，第二个事务可能完全覆盖掉第一个所完成的更新。

丢失更新是唯一一个用户可能在所有情况下都想避免的行为，在 SQL 92 中甚至没有提及。

#### Dirty Read

脏读，一个事务中读取到另一个事务未提交的数据。例如，事务 T1 读取到另一个事务 T2 **未提交的数据**，如果 T2 回滚，则 T1 相当于读取到了一个被认为不可能出现的值。

#### Non-Repeatable Read

不可重复读，在一个事务中，当重复读取同一条记录时，发现该记录的结果不同或者已经被删除了；如在事务 T1 中读取了一行，接着 T2 修改或者删除了该行 **并提交**，那么当 T1 尝试读取新的值时，就会发现改行的值已经修改或者被删除。

<!--
此时，写锁会一直保持到事务结束，读锁在select之后立即释放掉。
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
<tr><td><small>/* Query 1 */SELECT * FROM users WHERE id = 1;读锁-写锁-范围锁</small></td><td></td></tr>
<tr><td></td><td><small>/* Query 2 */UPDATE users SET age = 21 WHERE id = 1; COMMIT;<br>/* in multiversion concurrency control, or lock-based READ COMMITTED */</small></td></tr>　
<tr><td><small>/* Query 1 */SELECT * FROM users WHERE id = 1; COMMIT;<br>/* lock-based REPEATABLE READ */</small></td><td></td></tr>　
</tbody></table></center></li><br><li>
-->

#### Phantom

幻读，通常是指在一个事务中，当重复查询一个结果集时，返回的两个不同的结果集，可能是由于另一个事务插入或者删除了一些记录。

例如，事务 T1 读取一个结果集，T2 修改了该结果集中的部分记录 (例如插入一条记录)，T1 再次读取时发现与之前的结果不同 (多出来一条记录)，就像产生幻觉一样。

<!--
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
<tr><td><small>/* Query 1 */读锁+写锁-范围锁 只锁定了1，2 SELECT * FROM users WHERE age BETWEEN 10 AND 30;</small></td><td></td></tr>
<tr><td></td><td><small>/* Query 2 */ INSERT INTO users VALUES ( 3, 'Bob', 27 ); COMMIT;</small></td></tr>　
<tr><td><small>/* Query 1 */SELECT * FROM users WHERE age BETWEEN 10 AND 30;</small></td><td></td></tr>　
</tbody></table></center></li></ul>
-->

#### 其它

隔离级别与读现象。

| 隔离级别 | 脏读 | 不可重复读取 | 幻影数据行 |
|:--------:|:----:|:------------:|:----------:|
|READ UNCOMMITTED|YES|YES|YES|
|READ COMMITTED|NO|YES|YES|
|REPEATABLE READ|NO|NO|YES|
|SERIALIZABLE|NO|NO|NO|

### 事务超时

与事务超时相关的变量可以参考。

{% highlight text %}
----- 设置锁超时时间，单位为秒，默认50s
mysql> SHOW VARIABLES LIKE 'innodb_lock_wait_timeout';
+--------------------------+-------+
| Variable_name            | Value |
+--------------------------+-------+
| innodb_lock_wait_timeout | 50    |
+--------------------------+-------+
1 row in set (0.00 sec)

----- 超时后的行为，默认OFF，详见如下介绍
mysql> SHOW VARIABLES LIKE 'innodb_rollback_on_timeout';
+----------------------------+-------+
| Variable_name              | Value |
+----------------------------+-------+
| innodb_rollback_on_timeout | OFF   |
+----------------------------+-------+
1 row in set (0.02 sec)
{% endhighlight %}

[innodb_rollback_on_timeout ](https://dev.mysql.com/doc/refman/5.7/en/innodb-parameters.html#sysvar_innodb_rollback_on_timeout) 变量默认值为 OFF，如果事务因为加锁超时，会回滚上一条语句执行的操作；如果设置 ON，则整个事务都会回滚。

当上述变量为 OFF 时，也就是事务会回滚到上一个保存点，这是因为 InnoDB 在执行每条 SQL 语句之前，都会创建一个保存点，可以参见 row_insert_for_mysql() 函数中的代码。

{% highlight text %}
row_insert_for_mysql()
 |-row_insert_for_mysql_using_ins_graph()
   |-trx_savept_take()
{% endhighlight %}

如果事务因为加锁超时，相当于回滚到上一条语句，但是报错后，事务还没有完成，用户可以选择是继续提交，或者回滚之前的操作，由用户选择是否进一步提交或者回滚事务。

上述参数为 ON 时，整个事务都回滚；详细的内容可以从 row_mysql_handle_errors() 中验证。

<!--
问题：innodb_rollback_on_timeout为OFF，事务的原子性被破坏了吗？
答：NO，从示例中可以看到，事务只是回退上一条语句的状态，而整个事务实际上没有完成(提交或者回滚)，而作为应用程序在检测这个错误时，应该选择是提交或者回滚事务。如果严格要求事务的原子性，当然是执行ROLLBACK，回滚事务。
-->

### 其它

如何判断当前会话已经开启了一个事务？

{% highlight text %}
----- 可以直接使用在事务中会报错的语句。
mysql> SELECT @@TX_ISOLATION;
mysql> SET TRANSACTION ISOLATION LEVEL REPEATABLE READ;

----- 或者通过如下SQL查看，如果在事务中则会返回当前的事务ID，否则返回为空。
mysql> SELECT trx_id FROM information_schema.innodb_trx WHERE trx_mysql_thread_id = connection_id();
{% endhighlight %}


## 隔离级别

先准备下环境，隔离级别在测试时，会针对不同的场景分别进行设置；另外，将 autocommit 设置为 0 ，此时 commit/rollback 后的一条语句会自动开启一个新事务。

{% highlight text %}
----- 新建表并写入数据
mysql> SET GLOBAL autocommit=0;
mysql> CREATE TABLE user (id INT PRIMARY KEY, name VARCHAR(20), age INT UNSIGNED) engine=InnoDB;
mysql> INSERT INTO user VALUES (1, 'andy', 28);

----- 设置隔离级别
mysql> SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;
{% endhighlight %}

一般来说，也显然，高一级的级别可以提供更强的隔离性。

### READ UNCOMMITTED

也就是读未提交/未授权读，在此场景下 **允许脏读**，也就是允许某个事务看到其它事务尚未提交的数据行改动，这是最低的隔离等级。

{% highlight text %}
### 准备数据
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同时设置两个事务的隔离级别
SET SESSION TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
### 读到的age为28
SELECT * FROM user WHERE id=1;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             ### 开启一个事务，隔离级别任意，更新age
                                                             START TRANSACTION;
                                                             UPDATE user SET age=30 WHERE id=1;
### 读到未提交数据，age为30
SELECT * FROM user WHERE id=1;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   30 |
+----+------+------+
1 row in set (0.00 sec)
                                                             ### 回滚事务
                                                             ROLLBACK;
### 读到的age为28
SELECT * FROM user WHERE id=1;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
{% endhighlight %}

如上所示，在事务 A 中，会读取到不同的 age 值。

### READ COMMITTED

也就是读已提交/授权读，此时不允许上述的脏读，**允许不可重复读**，也就是 Fuzzy Read (也被称之为 Non-Repeatable Read)，也指一个事务内的两次读同一行看到的数据不一样，"不可重复" 。

{% highlight text %}
### 准备数据
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同时设置两个事务的隔离级别
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
### 读到的age仍然为28，没有幻读
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             COMMIT;
### 此时在事务B提交之后，读到的age为30
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   30 |
+----+------+------+
1 row in set (0.00 sec)
COMMIT;
{% endhighlight %}

如上，事务 A 中同一条 SQL 会读取到不同的数值，即使事务 B 中是删除操作。

<!--
基于锁机制并发控制的DBMS需要对选定对象的写锁(write locks)一直保持到事务结束，但是读锁(read locks)在SELECT操作完成后马上释放，可能会发生“不可重复读”现象。

简而言之，授权读这种隔离级别保证了读到的任何数据都是提交的数据，避免读到中间的未提交的数据，脏读(dirty reads)。但是在每次数据读完之后其他事务可以修改刚才读到的数据，从而导致不可重复读。

也就是说，事务的读取数据的时候获取读锁，但是读完之后立即释放 (不需要等到事务结束)，而写锁则是事务提交之后才释放。释放读锁之后，就可能被其他事物修改数据。
-->


### REPEATABLE READ

也即可重复读，InnoDB 默认的隔离级别，此时不允许脏读、不可重复读，但是允许 **幻读** 。

<!--
基于锁机制并发控制需要对选定对象的读锁(read locks)和写锁(write locks)一直保持到事务结束，但不要求“范围锁(range-locks)”，因此可能会发生“幻影读(phantom reads)”。如果某个事务两次执行同一个 SELECT 语句，其结果是可重复的。所有被 Select 获取的数据都不能被修改，这样就可以避免一个事务前后读取数据不一致的情况。但是却没有办法控制幻读，因为这个时候其他事务不能更改所选的数据，但是可以增加数据，因为前一个事务没有范围锁。
-->

{% highlight text %}
### 准备数据
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同时设置两个事务的隔离级别
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
### 读到的age仍然为28，没有幻读
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
                                                             COMMIT;
### 读到的age仍然为28，没有不可重复读
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   28 |
+----+------+------+
1 row in set (0.00 sec)
COMMIT;

### 此时读到的age为30
SELECT * FROM user;
+----+------+------+
| id | name | age  |
+----+------+------+
|  1 | andy |   30 |
+----+------+------+
1 row in set (0.00 sec)
{% endhighlight %}

再看一个出现幻读的情况。

{% highlight text %}
### 准备数据
DELETE FROM user;
### 同时设置两个事务的隔离级别
SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ;

----- TRANS A ---------------------------------------------+----- TRANS B -------------------------
START TRANSACTION;
SELECT * FROM user;
Empty set (0.00 sec)
                                                             START TRANSACTION;
                                                             INSERT INTO user VALUES (1, 'andy', 28);
                                                             COMMIT;
### 此时查询仍然为空
SELECT * FROM user;
Empty set (0.00 sec)
### 尝试插入数据时报错，TMD明明说没有这条记录的
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
### 不是说有一条记录吗，怎么多出来一条啊！！！
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
### 可以正常查询
SELECT * FROM USER WHERE ID > 0 AND ID < 5;
### 如下查询，会导致锁超时
SELECT * FROM USER WHERE ID > 0 AND ID < 5 LOCK IN SHARE MODE;
SELECT * FROM USER WHERE ID > 0 AND ID < 5 FOR UPDATE;
UPDATE USER SET AGE = 50 WHERE ID = 3;
COMMIT;

InnoDB 提供了这样的机制，在可重复读的隔离级别里，可以使用加锁读去查询最新的数据。此时，已经存在 id=3 的数据，如果常是插入，则会报错。
-->

#### Snapshot Read

另外，在 Repeatable Read 隔离级别下，如果使用 Snapshot Read 实现时，允许某些 Phantom 现象，简单来说就是第二次读到了第一次没有的行数据。

{% highlight text %}
### 准备数据
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同时设置两个事务的隔离级别
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
### 对于快照读，事务有了修改之后，就可以读到age为30
SELECT * FROM user;
+----+--------+------+
| id | name   | age  |
+----+--------+------+
|  1 | andrew |   30 |
+----+--------+------+
1 row in set (0.00 sec)
                                                             ### 不过此时读取到的仍然是andy
                                                             SELECT * FROM user;
                                                             +----+------+------+
                                                             | id | name | age  |
                                                             +----+------+------+
                                                             |  1 | andy |   30 |
                                                             +----+------+------+
                                                             1 row in set (0.00 sec)
COMMIT;
                                                             ### OK，终于读取到了最新的数据
                                                             SELECT * FROM user;
                                                             +----+--------+------+
                                                             | id | name   | age  |
                                                             +----+--------+------+
                                                             |  1 | andrew |   30 |
                                                             +----+--------+------+
                                                             1 row in set (0.00 sec)
{% endhighlight %}

#### Write Skew

Repeatable Read 允许 Write Skew，该异常主要是针对多行事务。

简单来说，就是如果事务 A 读取了行 X，并因此修改了 Y，然后提交；同时事务 B 读取了行 Y，并修改了 X ；那么此时的最终结果可能违反 X 和 Y 相关的某些约束。

{% highlight text %}
### 准备数据
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28),(2, 'cassie', 25);
### 同时设置两个事务的隔离级别
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


当隔离级别是可重复读，且禁用innodb_locks_unsafe_for_binlog的情况下，在搜索和扫描index的时候使用的next-key locks可以避免幻读。

再看一个实验，要注意，表t_bitfly里的id为主键字段。
复制代码

实验三：
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

复制代码

可以看到，用id<=1加的锁，只锁住了id<=1的范围，可以成功添加id为2的记录，添加id为0的记录时就会等待锁的释放。
复制代码

实验四：一致性读和提交读
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

复制代码

如果使用普通的读，会得到一致性的结果，如果使用了加锁的读，就会读到“最新的”“提交”读的结果。

本身，可重复读和提交读是矛盾的。在同一个事务里，如果保证了可重复读，就会看不到其他事务的提交，违背了提交读；如果保证了提交读，就会导致前后两次读到的结果不一致，违背了可重复读。

可以这么讲，InnoDB提供了这样的机制，在默认的可重复读的隔离级别里，可以使用加锁读去查询最新的数据（提交读）。
MySQL InnoDB的可重复读并不保证避免幻读，需要应用使用加锁读来保证。而这个加锁度使用到的机制就是next-key locks。
-->












### SERIALIZABLE

也就是串行化/可序列化，不允许上述的异常情况，包括 Phantom 和 Write Skew 以及任何不可串行化的反常情况。

{% highlight text %}
### 准备数据
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28),(2, 'cassie', 25);
### 同时设置两个事务的隔离级别
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
                                                             ### 此时尝试更新时会等待直到锁超时
                                                             UPDATE user SET age=15 WHERE id=2;
COMMIT;
                                                             ### 重新尝试提交
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

接下来，再看个示例。

{% highlight text %}
### 准备数据
DELETE FROM user;
INSERT INTO user VALUES (1, 'andy', 28);
### 同时设置两个事务的隔离级别
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
                                                             ### 此时尝试更新时会等待直到锁超时
                                                             UPDATE user SET age=15 WHERE id=2;
                                                             ### 同上，仍然锁等待超时
                                                             INSERT INTO user VALUES (2, 'cassie', 25);
{% endhighlight %}



<!--
某个事物正在查看的数据行不允许其他事务修改，直到该事务完成为止，也就是说需要同时获得读锁+写锁，当select查询使用where字句时需要获得一个范围锁。所有事务都一个接一个地串行执行，这样可以避免幻读 (phantom reads)。如果不是基于锁实现并发控制的数据库，则检查到有违反串行操作的事务时(如写冲突)，需要回滚该事务，此时只有一个事务可以提交成功，详见"快照隔离"。
-->



<!--
在基于锁的并发控制中，隔离级别决定了锁的持有时间。"C"-表示锁会持续到事务提交。 "S" –表示锁持续到当前语句执行完毕。如果锁在语句执行完毕就释放则另外一个事务就可以在这个事务提交前修改锁定的数据，从而造成混乱。
<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1">
<tbody><tr bgcolor="lightblue" align="center"><td>隔离级别</td><td>读操作</td><td>写操作</td><td>范围操作</td></tr>
<tr align="center"><td>READ UNCOMMITTED</td><td>S</td><td>S</td><td>S</td></tr>
<tr align="center"><td>READ COMMITTED</td><td>C</td><td>S</td><td>S</td></tr>
<tr align="center"><td>REPEATABLE READ</td><td>C</td><td>C</td><td>S</td></tr>
<tr align="center"><td>SERIALIZABLE</td><td>C</td><td>C</td><td>C</td></tr>
</tbody></table></center><br>



多个事务在同时运行时可能会发生各种各样的问题，如下：<ul><li>
脏读(dirty read)。某个事务所做的修改在它尚未提交时就可以被其它事务看到，修改的数据也有可能回滚，因此会造成数据的不一致。</li><br><li>

不可重复读取(nonrepeatable read)。同一个事务使用同一条 SELECT 语句每次读取到的结果不一样。</li><br><li>

幻影数据行(phantom row)。一个事务突然看到了一个它以前没有见过数据行。
</li></ul>




<ul><li>
脏读：如果一个事务A对数据进行了更改，但是还没有提交，而另一个事务B就可以读到事务A尚未提交的更新结果。这样，当事务A进行回滚时，那么事务B开始读到的数据就是一笔脏数据。
</li></ul>





	<ul><li>
		可重复读，可产生幻读 (phantom reads)，也可以归于不可重复读<br><br>
		一个事务中先后各执行一次同一个查询，但是返回的结果集却不一样。发生这种情况是因为在执行 Select 操作的时候没有获取范围锁 (Range Lock)，导致其他事务仍然可以插入或者删除新的数据。
		<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
		<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
		<tr><td><small>/* Query 1 */</small><br>SELECT * FROM users WHERE age BETWEEN 10 AND 30;</td><td></td></tr>
		<tr><td></td><td><small>/* Query 2 */</small><br>INSERT INTO users VALUES ( 3, 'Bob', 27 );<br>COMMIT;</td></tr>　
		<tr><td><small>/* Query 3 */</small><br>SELECT * FROM users WHERE age BETWEEN 10 AND 30;</td><td></td></tr>　
		</tbody></table></center><br>

		注意 transaction 1 对同一个查询语句（Query 1）执行了两次。 如果采用更高级别的隔离等级（即串行化）的话，那么前后两次查询应该返回同样的结果集。但是在可重复读隔离等级中却前后两次结果集不一样。但是为什么叫做可重复读等级呢？那是因为该等级解决了下面的不可重复读问题。</li><br><li>



		读已提交，可产生不可重复读 (Non-repeatable reads)<br><br>
		在采用锁来实现并发控制的数据库系统中，不可重复读是因为在执行 Select 操作的时候没有加读锁 (read lock)。

		<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
		<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
		<tr><td><small>/* Query 1 */</small><br>SELECT * FROM users WHERE id = 1;</td><td></td></tr>
		<tr><td></td><td><small>/* Query 2 */</small><br>UPDATE users SET age = 21 WHERE id = 1;<br>COMMIT;</td></tr>　
		<tr><td><small>/* Query 3 */</small><br>SELECT * FROM users WHERE id = 1;</td><td></td></tr>　
		</tbody></table></center><br>

		在这个例子当中，Transaction 2提交成功,所以Transaction 1第二次将获取一个不同的age 值.在SERIALIZABLE和REPEATABLE READ隔离级别中,数据库应该返回同一个值。而在READ COMMITTED和READ UNCOMMITTED级别中数据库返回更新的值。这样就出现了不可重复读。



		读未提交，可造成脏读 (dirty reads)<br><br>
		如果一个事务 2 读取了另一个事务 1 修改的值，但是最后事务 1 回滚了，那么事务 2 就读取了一个脏数据，这也就是所谓的脏读。发生这种情况就是允许事务读取未提交的更新。
		<center><table align="center" style = "font-size: small;" bgcolor="lightyellow" border="1" width="95%">
		<tbody><tr bgcolor="lightblue" align="center"><td width=50%>Transaction 1</td><td>Transaction 2</td></tr>
		<tr><td></td><td><small>/* Query 1 */</small><br>UPDATE users SET age = 21 WHERE id = 1;</td></tr>　
		<tr><td><small>/* Query 2 */</small><br>SELECT * FROM users WHERE id = 1;</td><td></td></tr>
		<tr><td></td><td><small>/* Query 1 */</small><br>Roll Back</td></tr>　
		</tbody></table></center></li><br><li>

		第一类更新丢失<br><br>
		这是事物隔离级别最低时出现的问题，既不发出共享锁，也不接受排它锁。两个事务更新相同的数据资源时，如果一个事务被提交，一个事务被撤销，则提交的事务所做的修改也将被撤销；或者两个事务同时提交，导致第一次的修改被覆盖。
	</li></ul>
-->






## 参考

关于数据库的事务隔离级别可以参考 [WikiPedia - Isolation (database systems)](https://en.wikipedia.org/wiki/Isolation_%28database_systems%29) 中的介绍。


<!--
[深入理解数据库事务隔离级别](/reference/databases/mysql/mysql_isolation_details.pptx)

http://docs.oracle.com/cd/B19306_01/server.102/b14220/consist.htm
<a href="http://book.51cto.com/art/200710/58623.htm">http://book.51cto.com/art/200710/58623.htm</a></li><br><li>
-->

{% highlight text %}
{% endhighlight %}
