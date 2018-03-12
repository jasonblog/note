---
Date: October 19, 2013
title: MySQL 基本介绍
layout: post
comments: true
language: chinese
category: [mysql,database]
---

在此主要介绍下与数据库相关的一些操作，其中有一部分是与 MySQL 相关的功能。

<!-- more -->


## 简介

![sql logo]({{ site.url }}/images/databases/mysql/sql-logo.jpg "sql logo"){: .pull-right }

首先简单介绍一下 SQL (Structured Query Language) 相关的标准；SQL-92 是最常用的 SQL 标准，其定义了一些常见的语法等，详细的可参考 [SQL-92](http://www.contrib.andrew.cmu.edu/~shadow/sql/sql1992.txt) ，也可查看 [本地文档](/reference/mysql/SQL1992.txt) 。

其中 SQL 常见的操作包括如下：

* DDL：数据库模式定义语言，create 。
* DML：数据操纵语言，insert、delete、update 。
* DCL：数据库控制语言 ，grant、remove 。
* DQL：数据库查询语言，select 。

不过不同的数据库对 SQL-92 标准会有所扩展，这也就造成了一些数据库操作的不兼容，或者是一些细节上的差异。

对于 MySQL，在 Linux 主机上的默认存储位置为 /var/lib/mysql/ ； FreeBSD 在 /var/db/mysql；如果想要修改可以通过符号链接即可。

如果是单独进行安装的，可以通过如下命令查看 MySQL 是否安装成功。

{% highlight text %}
$ mysqladmin --version
{% endhighlight %}

### 示例数据库

通常最基本的是熟悉简单的 join 操作，以及基本的 CRUD(Create, Retrieve, Update, Delete) 操作，如下包括了很多教程示例，可以参考 [MySQL Sample Databases](https://www3.ntu.edu.sg/home/ehchua/programming/sql/SampleDatabases.html) 中的介绍，在此简单列举几个。

下面的示例可以参考 [MySQLTutorial](http://www.mysqltutorial.org/)，数据库的数据也可以从 [本地下载](/reference/mysql/from_mysqltutorial.sql) ，然后通过 source 导入。下图是对应的 ER 图，其中关于 ER-Diagram 相关的介绍可以参考 [ER Diagram Symbols and Meaning (LucidChart)](https://www.lucidchart.com/pages/ER-diagram-symbols-and-meaning) 。

![sample sql]({{ site.url }}/images/databases/mysql/sample_database_store.png "sample sql"){: .pull-center width="60%" }

上述是一个销售汽车的模型，包括了常见的模型以及使用数据。

<!--
Customers，保存了用户的信息。
Products，产品信息，也就是车的信息。
ProductLines: stores a list of product line categories.
Orders: stores  sales orders placed by customers.
OrderDetails: stores sales order line items for each sales order.
Payments: stores payments made by customers based on their accounts.
Employees: stores all employee information as well as the organization structure such as who reports to whom.
Offices: stores sales office data.
-->


另外，可以参考 [employee](https://launchpad.net/test-db/) 的示例数据库，现在已经迁移到 [github](https://github.com/datacharmer/test_db)，其文档位于 [Employees Sample Database](http://dev.mysql.com/doc/employee/en/) ，可以直接从 [本地下载](/reference/mysql/test_db-master.zip) 。

![employees schema]({{ site.url }}/images/databases/mysql/employees-schema.png "employees schema"){: .pull-center width="50%"}

其中 README.md 中包括了如何进行安装。

还有一个经常使用的示例数据库 [Sakila Sample Database](https://dev.mysql.com/doc/sakila/en/)。

![sample sakila schema]({{ site.url }}/images/databases/mysql/sample-sakila-schema.png "sample sakila schema"){: .pull-center width="90%"}





## 时间

MySQL 中有三种时间类型，官方的解释如下：

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
发现DATETIME类型只支持mysql 5.6.5+

|:----:|:--------------------|:--------------------|
| 特性 |DATETIME|TIMESTAMP |
| 显示 |YYYY-MM-DD HH:MM:SS|YYYY-MM-DD HH:MM:SS|
| 范围 |1000-01-01 00:00:00 ~ 9999-12-31 23:59:59|1970-01-01 00:00:00 ~ 2038-01-19 03:14:07|
| 存储 |8字节存储，采用数值格式|4字节存储，以UTC格式存储|
| 时区 |无时区概念|时区转换，存储时对当前的时区进行转换，检索时再转换回当前的时区 |
| 特性 |无|在insert, update时可以设置为自动更新时间|
-->

### 特殊属性

timestamp 有两个比较特殊的属性，分别是 ```ON UPDATE CURRENT_TIMESTAMP``` 和 ```CURRENT_TIMESTAMP``` 两种，使用情况分别如下：

#### CURRENT_TIMESTAMP

当要向数据库执行 INSERT 操作时，如果有 timestamp 字段属性设为 CURRENT_TIMESTAMP，则无论这个字段有没有值都会插入当前系统时间。

#### ON UPDATE CURRENT_TIMESTAMP

当执行 update 操作时，并且字段有上述的属性时，则字段无论值有没有变化，它的值也会跟着更新为当前 UPDATE 操作时的时间。

简单测试如下。

{% highlight text %}
----- 新建测试表
mysql> CREATE TABLE foobar (
  id INT(10) UNSIGNED NOT NULL PRIMARY KEY,
  gmt_create TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  gmt_modify TIMESTAMP NOT NULL ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB;
Query OK, 0 rows affected (0.06 sec)

----- 新写入数据时，会自动更新DEFAULT CURRENT_TIMESTAMP对应字段
mysql> INSERT INTO foobar(id, gmt_modify) VALUES(1, now());
Query OK, 1 row affected (0.01 sec)
mysql> SELECT * FROM foobar;
+----+---------------------+---------------------+
| id | gmt_create          | gmt_modify          |
+----+---------------------+---------------------+
|  1 | 2016-04-23 13:04:59 | 2016-04-23 13:04:59 |
+----+---------------------+---------------------+
1 row in set (0.00 sec)

----- 在执行UPDATE语句时，会更新ON UPDATE CURRENT_TIMESTAMP对应字段
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

### 默认值

从 MySQL 5.6.6 之后，可能会发现如下的 Warning 日志，```TIMESTAMP with implicit DEFAULT value is deprecated. Please use --explicit_defaults_for_timestamp server option (see documentation for more details).``` 这是因为这一版本之后，timestamp 类型的默认行为已经取消。

在此之前，TIMESTAMP 类型的默认行为是：

* TIMESTAMP 列如果没有明确声明 NULL 属性，则默认为 NOT NULL (而其它数据类型，未显示声明 NOT NULL，则允许 NULL值)，当设置 TIMESTAMP 的列值为 NULL，会自动存储为当前 timestamp。
* 表中的第一个 TIMESTAMP 列，如果没有声明 NULL 属性、DEFAULT 或者 ON UPDATE，会自动分配 ```DEFAULT CURRENT_TIMESTAMP``` 和 ```ON UPDATE CURRENT_TIMESTAMP``` 属性。
* 表中第二个 TIMESTAMP 列，如果没有声明 NULL 或 DEFAULT 值，则默认设置 ```'0000-00-00 00:00:00'```；插入行时没有指明改列的值，该列默认分配 ```'0000-00-00 00:00:00'```，且无警告。

如果要关闭警告，如上所述，可以在启动时添加 ```--explicit_defaults_for_timestamp``` 参数，或者在下面的配置文件中添加如下的参数：

{% highlight text %}
[mysqld]
explicit_defaults_for_timestamp=true
{% endhighlight %}

重启 MySQL 后，此时 TIMESTAMP 的行为如下：

* 如果没有显示声明 ```NOT NULL``` 属性，也就是是允许 NULL 值的，可以直接设置改列为 NULL，而没有默认的填充行为。
* 不会默认分配 ```DEFAULT CURRENT_TIMESTAMP``` 和 ```ON UPDATE CURRENT_TIMESTAMP``` 属性。
* 声明为 ```NOT NULL``` 且没有默认子句的 TIMESTAMP 列是没有默认值的；往数据表中插入列，又没有给 TIMESTAMP 列赋值时，如果是严格 SQL 模式，会抛出一个错误，如果严格 SQL 模式没有启用，该列会赋值为 ```'0000-00-00 00:00:00'``` ，同时出现一个警告(和 MySQL 处理其他时间类型数据一样，如 DATETIME)。

显然，通过该参数关闭了 timestamp 类型字段所拥有的一些会让人感到奇怪的默认行为，如果仍需要默认行为，则需要在建表的时候指定。

## 外键

<!--
在使用 foreign key 时需要遵守以下几点规则：

1，有外键约束的表，必须是innodb型
2，外键约束的二个表，本来就相关系的表，并且要有索引关系，如果没有，创建外键时也可以创建索引。
3，不支持对外键列的索引前缀。这样的后果之一是BLOB和TEXT列不被包括在一个外键中，这是因为对这些列的索引必须总是包含一个前缀长度。
4，mysql外键的名子在数据库内要是唯一的
-->

创建外键的语法规则如下：

{% highlight text %}
[CONSTRAINT symbol] FOREIGN KEY [id] (index_col_name, ...)
    REFERENCES tbl_name (index_col_name, ...)
    [ON DELETE {RESTRICT | CASCADE | SET NULL | NO ACTION}]
    [ON UPDATE {RESTRICT | CASCADE | SET NULL | NO ACTION}]
{% endhighlight %}

外键维护数据完整性的 5 种方式：

1. CASCADE: 从父表删除或更新且自动删除或更新子表中匹配的行，可以为 `ON DELETE CASCADE` 和 `ON UPDATE CASCADE` ；
2. SET NULL: 从父表删除或更新行，设置子表中的外键列为 NULL，需要对应列也可为 NULL 。
3. NO ACTION: SQL-92 标准中，如果有外键的，那么删除或更新主键时会报错；
4. RESTRICT: 拒绝对父表的删除或更新操作。一些数据库有延期检查，一般 NO ACTION 是一个延期检查；而在 MySQL 中，外键约束是被立即检查的，所以 RESTRICT 与上述同样。
5. SET DEFAULT: 这个动作被解析程序识别，但 InnoDB 不支持包含 `ON DELETE SET DEFAULT` 或 `ON UPDATE SET DEFAULT` 子句的表定义。

如下是测试用例：

{% highlight text %}
----- 创建用户表
mysql> CREATE TABLE `user` (
  `id` int(11) NOT NULL auto_increment COMMENT '用户ID',
  `name` varchar(50) NOT NULL default '' COMMENT '名字',
  `sex` int(1) NOT NULL default '0' COMMENT '0为男，1为女',
  PRIMARY KEY  (`id`)
) ENGINE=innodb DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
----- 写入测试数据
mysql> INSERT INTO user(name, sex) VALUES("andy", 0),("lily", 1);

----- 创建订单表
mysql> CREATE TABLE `order` (
  `order_id` INT(11) NOT NULL AUTO_INCREMENT COMMENT '订单ID',
  `u_id` INT(11) NOT NULL DEFAULT '0' COMMENT '用户ID',
  `username` VARCHAR(50) NOT NULL DEFAULT '' COMMENT '用户名',
  `money` INT(11) NOT NULL DEFAULT '0' COMMENT '钱数',
  `datetime` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '生成时间',
  PRIMARY KEY(`order_id`),
  INDEX (`u_id`),
  FOREIGN KEY order_f_key (u_id) REFERENCES user(id)
) ENGINE=innodb DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;
mysql> INSERT INTO `order`(`u_id`, `username`, `money`, `datetime`)
       VALUES('1', 'andy','10', CURRENT_TIMESTAMP);

----- 由于order表中有依赖，导致删除user表时失败
mysql> DELETE FROM user WHERE id =1;
----- 写入数据时由于user表中不存在，导致写入失败
mysql> INSERT INTO `order`(`u_id`, `username`, `money`, `datetime`)
       VALUES('5', 'foobar', '123', CURRENT_TIMESTAMP);

----- MySQL中没有修改外键操作，需要先删除再重新添加
mysql> ALTER TABLE `order` DROP FOREIGN KEY order_ibfk_1;
mysql> ALTER TABLE `order` ADD FOREIGN KEY(u_id) REFERENCES user(id)
       ON DELETE CASCADE ON UPDATE CASCADE;

mysql> REPLACE INTO `order`(`u_id`, `username`, `money`, `datetime`)
       VALUES('2', 'andy','10', CURRENT_TIMESTAMP);
{% endhighlight %}

## 其它

### 注释

在 MySQL 中可以通过 ```#``` (从开始到结束)、```/* ... */``` (可以多行) 表示注释内容；从 MySQL-3.23 版本开始，可以通过 ```/*! ... */``` 这种特有的注释方式标示 MySQL 特有特性。

MySQL 会解析其中的关键字，而其它数据库则会视为注释，从而保证兼容性。另外，从 3.23 开始支持 ```"-- "``` 格式的注释，功能与 ```#``` 类似；需要注意的是，双短划线之后有一个空格。

接下来，重点说下 ```/*! ... */``` 这个注释，简言之，MySQL 会执行其中的内容，例如，MySQL 支持 STRAIGHT_JOIN 写法，而其它数据库可能不支持，那么就可以写成 ```SELECT /*! STRAIGHT_JOIN */ col1 FROM table1,table2 WHERE...``` 这种方式。

另外，可以在叹号后面添加版本号，表示只有当 MySQL 的版本大于等于该版本时才会执行，例如只有 MySQL-3.23.02 之后才支持 TEMPORARY，那么可以写为 ```CREATE /*!32302 TEMPORARY */ TABLE t (a INT);``` 。

其它的示例 ```CREATE DATABASE `blog` /*!40100 DEFAULT CHARACTER SET latin1 */;``` 。

<!--
### 字符串操作函数

CONCAT(str1,str2,…)返回连接参数产生的字符串，返回值的规则如下：

1. 如有任何一个参数为NULL，则返回值为NULL

mysql> select concat("foobar", null);
+------------------------+
| concat("foobar", null) |
+------------------------+
| NULL                   |
+------------------------+
1 row in set (0.00 sec)
2. 如果所有参数均为非二进制字符串，则结果为非二进制字符串；
3. 如果自变量中含有任一二进制字符串，则结果为一个二进制字符串；
4. 一个数字参数被转化为与之相等的二进制字符串格式；若要避免这种情况，可使用显式类型 cast, 例如： SELECT CONCAT(CAST(int_col AS CHAR), char_col)。

CONCAT_WS(separator,str1,str2,…)
CONCAT_WS()是CONCAT With Separator的简写，第一个参数是其它参数的分隔符，可以是字符串或者其它类型，如果为NULL，则结果为NULL；另外，函数会忽略任何分隔符参数后的NULL值。
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

sql_mode 是个很容易被忽视的变量，在生产环境必须将这个值设置为严格模式，所以开发、测试环境的数据库也必须要设置，这样在开发测试阶段就可以发现问题

{% highlight text %}
----- 查看变量
mysql> SHOW VARIABLES LIKE 'sql_mode%'\G
 *************************** 1. row ***************************
 Variable_name: sql_mode
         Value: ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,
         NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION
         1 row in set (0.01 sec)

----- 设置变量
mysql> SET GLOBAL sql_mode='';
{% endhighlight %}

简单列举如下常用的，详细可以参考官方手册 [MySQL Reference Manual - Server SQL Modes](https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html) 。 <!--https://segmentfault.com/a/1190000005936172-->

* NO_ZERO_IN_DATE<br>在严格模式下，不允许日期和月份为零。
* NO_ZERO_DATE<br>不允许插入零日期，插入零日期会抛出错误而不是警告。
* NO_AUTO_CREATE_USER<br>禁止 GRANT 创建密码为空的用户。
* ONLY_FULL_GROUP_BY<br>参考如下。

#### ONLY_FULL_GROUP_BY

对于 ```GROUP BY``` 聚合操作，如果在 SELECT 中的列没有在 ```GROUP BY``` 中出现 (可以是聚合函数)，那么认为这个 SQL 是不合法的。

例如 ```select language_id, count(1), length from film group by language_id;``` 是不合法的，原因是 MySQL 允许在 SELECT 列表中存在除聚集函数或 ```GROUP BY``` 以外的表达式，但是这些表达式的结果并不确定，通常也就没有太多意义，反而可能会导致错误。

实际上 SQLServer、Oracle、PostgreSQL 基本都不支持这种语法，为此，MySQL 5.7 修订了这一语义。另外，需要注意 ```SELECT id+1 FROM tt GROUP BY 1+id;``` 这种是不允许的，而 ```SELECT id+1 FROM tt GROUP BY id+1;``` 则可以。

### show tables

默认在使用 LIKE 过滤时，不能使用 AND/OR 等条件，可以通过如下方式添加过滤条件。

{% highlight text %}
----- 只能使用单个like语句
mysql> SHOW TABLES LIKE 'host%';

----- 如果要使用多个，需要使用WHERE子句，其中FROM子句可以省略
mysql> SHOW TABLES FROM `sys` WHERE `Tables_in_sys` LIKE 'user%' OR `Tables_in_sys` LIKE 'host%';
{% endhighlight %}

<!--
select * from mysql.user where user like 'root%' and host like 'local%';
%匹配多个任意字符；_匹配单个任意字符；
也可以使用正则表达式，也即[NOT] REGEXP操作符或者[NOT] RLIKE。
-->

<!---
http://blog.csdn.net/imzoer/article/details/8277797
-->

## 参考

[MySQL Reference Manual - Server SQL Modes](https://dev.mysql.com/doc/refman/5.7/en/sql-mode.html)


{% highlight text %}
{% endhighlight %}
