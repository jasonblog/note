---
title: MySQL 自带工具
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,tools,工具
description: 简单介绍下 MySQL 中自带的工具集。
---

简单介绍下 MySQL 中自带的工具集。

<!-- more -->

## 简介

MySQL 提供了许多命令行工具，这些工具可以用来管理 MySQL 服务器、进行数据库备份和恢复、对数据库进行访问控制、对数据库进行压测等等。

首先，简单介绍下各个命令行。

{% highlight text %}
----- MySQL安装、初始化
mysql_install_db
    初始化 MySQL 数据目录程序，主要是新建数据库，初始化用户；5.7 已使用 mysqld 替换；
mysql_secure_installation
    安装完之后进行一些安全性配置，例如设置root用户、删除test数据库等；
mysql_safe
    一个shell脚本程序，用来安全启动mysqld程序；
mysql_tzinfo_to_sql
    将时区信息转换为SQL语句，可以直接加载到MySQL的mysql.time_zone%文件中；
mysql_upgrade
    用于升级时检查MySQL表；

----- MySQL客户端程序
mysql
    MySQL命令行程序，用于链接数据库；
mysqlslap
    压测程序，可以用来简单的对数据库进行性能压测；
mysqldump
    逻辑备份程序；
mysqlimport
    数据导入工具；

----- MySQL管理和实用程序
mysqlbinlog
    用于处理二进制日志文件(binlog)，可以用于打印数据；
mysqldumpslow
    用于处理慢查询日志文件；
mysqladmin
    用于管理MySQL服务器；

----- 其它
my_print_defaults
    读取配置文件中的配置选项；
{% endhighlight %}

<!--
mysqlcheck：一个表检查维护工具
mysqlshow：显示数据库，表，列信息

mysqlhotcopy：数据备份工具
-->

<!--

下面先介绍一些现在需要用到的MySQL程序，更多的程序工具在后面哪里用到就会讲到。
连接MySQL

连接MySQL操作是一个连接进程和MySQL数据库实例进行通信，从程序设计上来说，本质上是进程通信。如果对进程通信比较了解，可以知道常用的进程通信方式有管道、命令管道、TCP/IP套接字、UNIX域套接字。

TCP/IP

TCP/IP套接字方式是MySQL数据库在任何平台下都提供的连接方式，也是网络中使用得最多的一种方式。这种方式在TCP/IP连接上建立一个基于网络的连接请求，一般情况下客户端在一台服务器上，而MySQL实例在另一台服务器上，这两台服务器通过一个TCP/IP网络连接。例如在Windows下使用Sqlyog工具进行连接远程MySQL实例就是通过TCP/IP连接。（需要输入MySQL的IP地址及端口号，也称为一个套接字。同时MySQL实例必须要允许远程的IP地址和用户名进程连接访问，默认无法连接。）

共享内存

MySQL提供共享内存的连接方式看着是通过在配置文件中添加–shared-memory实现的，如果想使用共享内存（属于进程间通信）的方式，在MySQL命令行客户端连接时还必须使用–protocol=memory选项即可。

UNIX域套接字

在Linux和Unix环境下，还可以使用Unix域套接字。Unix域套接字不是一个网络协议，所以只能在MySQL客户端和数据库实例在一台服务器上的情况下使用。可以再配置文件中指定套接字文件的路径，如–socket=/tmp/mysql.sock，一般在数据库安装时就指定好了路径。当数据库实例启动后，用户就可以通过Uuix域套接字进行连接。
Mysql：MySQL命令行程序

MySQL命令行是MySQL提供的一个命令行客户端工具，MySQL客户端工具并不具体指某个客户端软件，事实上MySQL客户端是一种复合概念。包含不同程序语言编写的前端应用程序，如Sqlyog是一个非常好用的图形化界面客户端连接工具，MySQL自带的基于命令行的客户端，phpmyadmin是一个web界面的客户端，以及调用API接口等都称为MySQL客户端。

mysql命令行连接服务器
[root@localhost ~]# mysql [OPTIONS] [database]
[OPTIONS]
--help：显示帮助信息
-uroot：指定登录用户
-predhat：指定登录密码（密码与-p之间不能有空格，否则无法识别）
-h localhost：指定要登录的主机（这里登录的主机名必须在Mysql库中有授权才可以）
-e "select * from mysql.user;"：在Linux命令行中执行SQL语句
-s -e ""：可以去掉MySQL显示边框
-N -s -e ""：可以去掉MySQL显示边框和标题信息
--socke=/tmp/mysql.sock：指定套接字文件（不指定会去默认位置查找）
--port：指定端口
--protocol=memory：指定连接协议{tcp|socket|pipe|memory}
--html：以HTML格式显示数据内容
--xml：以XML格式显示数据内容
1
2
3
4
5
6
7
8
9
10
11
12
13
14

[root@localhost ~]# mysql [OPTIONS] [database]
[OPTIONS]
--help：显示帮助信息
-uroot：指定登录用户
-predhat：指定登录密码（密码与-p之间不能有空格，否则无法识别）
-h localhost：指定要登录的主机（这里登录的主机名必须在Mysql库中有授权才可以）
-e "select * from mysql.user;"：在Linux命令行中执行SQL语句
-s -e ""：可以去掉MySQL显示边框
-N -s -e ""：可以去掉MySQL显示边框和标题信息
--socke=/tmp/mysql.sock：指定套接字文件（不指定会去默认位置查找）
--port：指定端口
--protocol=memory：指定连接协议{tcp|socket|pipe|memory}
--html：以HTML格式显示数据内容
--xml：以XML格式显示数据内容

示例：
[root@localhost ~]# mysql -uroot -predhat -h localhost --port 3306 --socket=/tmp/mysql.sock -s -N -e "select @@VERSION"
5.5.28
1
2

[root@localhost ~]# mysql -uroot -predhat -h localhost --port 3306 --socket=/tmp/mysql.sock -s -N -e "select @@VERSION"
5.5.28

#这里-p指定的密码，必须要提前使用mysqladmin工具设定好，默认root是没有密码的。另外上面的很多选项都可以省略，因为MySQL启动时会根据配置文件中设定的参数来自行指定，如执行ps命令会看到如下进程信息：
[root@localhost ~]# ps aux | grep mysqld
/bin/sh /usr/local/mysql/bin/mysqld_safe --datadir=/mydata --pid-file=/mydata/localhost.localdomain.pid
/usr/local/mysql/bin/mysqld --basedir=/usr/local/mysql --datadir=/mydata --plugin-dir=/usr/local/mysql/lib/plugin --user=mysql --log-error=/mydata/error.log --pid-file=/mydata/localhost.localdomain.pid --socket=/tmp/mysql.sock --port=3306
1
2
3

[root@localhost ~]# ps aux | grep mysqld
/bin/sh /usr/local/mysql/bin/mysqld_safe --datadir=/mydata --pid-file=/mydata/localhost.localdomain.pid
/usr/local/mysql/bin/mysqld --basedir=/usr/local/mysql --datadir=/mydata --plugin-dir=/usr/local/mysql/lib/plugin --user=mysql --log-error=/mydata/error.log --pid-file=/mydata/localhost.localdomain.pid --socket=/tmp/mysql.sock --port=3306

mysql命令

MySQL命令行客户端命令执行有两个模式：交互式模式和批处理模式

交互式模式

客户端命令(不需要在服务器上执行而是由客户端从服务端获取信息)
mysql> help
  #查看命令帮助
mysql> charset utf8
  #改变默认字符集
mysql> status
  #获取MYSQL的工作状态，如当前默认数据，字符集等
mysql> use DATABASE_NAME
  #设置默认数据库
mysql> delimiter \\
  #设置SQL语句结束符,存储过程中常用（默认是;号）
mysql> quit
  #退出MySQL
mysql> system
  #在Mysql登录提示符下操作Linux主机如"system ls /root"命令
mysql> prompt MY>
  #修改MySQL的登录提示符为“my>”
mysql> select * from mysql.user
  -> \c
  #\c是结束命令，一般用于退出当忘记使用SQL结束符进入的模式
mysql> select * from mysql.user\G
  #\G做SQL语句的结束符并以纵向显示输出结果
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21

mysql> help
  #查看命令帮助
mysql> charset utf8
  #改变默认字符集
mysql> status
  #获取MYSQL的工作状态，如当前默认数据，字符集等
mysql> use DATABASE_NAME
  #设置默认数据库
mysql> delimiter \\
  #设置SQL语句结束符,存储过程中常用（默认是;号）
mysql> quit
  #退出MySQL
mysql> system
  #在Mysql登录提示符下操作Linux主机如"system ls /root"命令
mysql> prompt MY>
  #修改MySQL的登录提示符为“my>”
mysql> select * from mysql.user
  -> \c
  #\c是结束命令，一般用于退出当忘记使用SQL结束符进入的模式
mysql> select * from mysql.user\G
  #\G做SQL语句的结束符并以纵向显示输出结果

批处理模式
一般用于导入以.sql结尾的命令集，有以下三种方式：
1.[root@localhost ~]# mysql -u root -h localhost < /root/mysql.sql
2.mysql> source /root/mysql.sql
3.mysql> \. /root/mysql.sql

命令行编辑功能
mysql> ctrl+a：快速移动光标至行首
mysql> ctrl+e：快速移动光标至行尾
mysql> ctrl+w：删除光标之前的单词
mysql> ctrl+u：删除行首至光标处的所有内容
mysql> ctrl+y：粘贴使用ctrl+w或ctrl+u删除的内容
1
2
3
4
5
6
7
8
9
10
11

一般用于导入以.sql结尾的命令集，有以下三种方式：
1.[root@localhost ~]# mysql -u root -h localhost < /root/mysql.sql
2.mysql> source /root/mysql.sql
3.mysql> \. /root/mysql.sql

命令行编辑功能
mysql> ctrl+a：快速移动光标至行首
mysql> ctrl+e：快速移动光标至行尾
mysql> ctrl+w：删除光标之前的单词
mysql> ctrl+u：删除行首至光标处的所有内容
mysql> ctrl+y：粘贴使用ctrl+w或ctrl+u删除的内容

Mysqladmin：管理MySQL服务器客户端工具

语法如下:
[root@localhost ~]# mysqladmin -u root -h localhost -p [Command] [arguments]
-u root：指定登录用户
-h localhost：指定要登录的主机（本机可省略）
-p：指定密码，密码可以直接跟在-p后面

[Command]
passwd 'new password'  [-p 'old password']
  #设置密码，其中“-p ’old password’”只有更换密码时才需要执行指定旧密码，第一次指定密码时不需要
create DATABASE_NAME
  #创建数据库
drop DATABASE_NAME
  #删除数据库
processlist
  #显示当前主机正在执行的任务列表
status
  #显示Mysql的服务器状态信息，如启动时间
status --sleep 2 --count 2
  #2秒显示一次一共显示2此
extended-status
  #显示MYSQL状态变量及其值
variables
  #显示MYSQL服务器变量及其值
shutdown
  #终止MySQL服务
flush-privileges
  #刷新授权表
flush-threads
  #清空线程缓存
flush-status
  #重置大多数的服务器状态变量
flush-logs
  #刷新日志
start-slave
stop-slave
  #启动和关闭Slave
version
  #查看MYSQL的版本和状态信息
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37

[root@localhost ~]# mysqladmin -u root -h localhost -p [Command] [arguments]
-u root：指定登录用户
-h localhost：指定要登录的主机（本机可省略）
-p：指定密码，密码可以直接跟在-p后面

[Command]
passwd 'new password'  [-p 'old password']
  #设置密码，其中“-p ’old password’”只有更换密码时才需要执行指定旧密码，第一次指定密码时不需要
create DATABASE_NAME
  #创建数据库
drop DATABASE_NAME
  #删除数据库
processlist
  #显示当前主机正在执行的任务列表
status
  #显示Mysql的服务器状态信息，如启动时间
status --sleep 2 --count 2
  #2秒显示一次一共显示2此
extended-status
  #显示MYSQL状态变量及其值
variables
  #显示MYSQL服务器变量及其值
shutdown
  #终止MySQL服务
flush-privileges
  #刷新授权表
flush-threads
  #清空线程缓存
flush-status
  #重置大多数的服务器状态变量
flush-logs
  #刷新日志
start-slave
stop-slave
  #启动和关闭Slave
version
  #查看MYSQL的版本和状态信息

Mysqlshow：显示数据库，表，列信息

[root@localhost ~]# mysqlshow [options] [db_name [tbl_name [col_name]]]
如果没有给出数据库，显示所有匹配的数据库。
如果没有给出表，显示数据库中所有匹配的表。
如果没有给出列，显示表中所有匹配的列和列类型。
[options]
--help：详细帮助信息
-uroot：指定用户
-predhat：指定密码

[root@localhost ~]# mysqlshow -uroot -predhat
  #查看所有数据库
[root@localhost ~]# mysqlshow -uroot -predhat mysql
  #查看mysql库的所有表
[root@localhost ~]# mysqlshow -uroot -predhat mysql user
  #查看mysql库的user表所有记录
[root@localhost ~]# mysqlshow -uroot -predhat mysql user show-table-type
  #查看表的类型
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17

[root@localhost ~]# mysqlshow [options] [db_name [tbl_name [col_name]]]
如果没有给出数据库，显示所有匹配的数据库。
如果没有给出表，显示数据库中所有匹配的表。
如果没有给出列，显示表中所有匹配的列和列类型。
[options]
--help：详细帮助信息
-uroot：指定用户
-predhat：指定密码

[root@localhost ~]# mysqlshow -uroot -predhat
  #查看所有数据库
[root@localhost ~]# mysqlshow -uroot -predhat mysql
  #查看mysql库的所有表
[root@localhost ~]# mysqlshow -uroot -predhat mysql user
  #查看mysql库的user表所有记录
[root@localhost ~]# mysqlshow -uroot -predhat mysql user show-table-type
  #查看表的类型

Mysql_safe：这是一个脚本程序，用来安全启动Mysqld服务

在上面我们查看mysql进程的时候发现，不光有mysqld还有一个使用bash执行的mysqld_safe程序。mysqld_safe是用来安全启动mysqld程序的，一般直接运行mysqld程序来启动MySQL服务的方法很少见，mysqld_safe增加了一些安全特性，例如当出现错误时重启服务器并向错误日志文件写入运行时间信息。

在类UNIX系统中推荐使用mysqld_safe来启动mysqld服务器。mysqld_safe从选项配置文件的[mysqld]、[server]和 [mysqld_safe]部分读取所有选项。其实我们平时使用mysql.server脚本其实也是调用mysqld_safe脚本去启动MySQL服务器的，所以如果你直接去启动mysqld程序而不是用脚本的话一般都会有问题。

mysqld_safe通常做如下事情：

1.检查系统和选项。

2.检查MyISAM表。

3.保持MySQL服务器窗口。

4.启动并监视mysqld，如果因错误终止则重启。

5.将mysqld的错误消息发送到数据目录中的host_name.err 文件。

6.将mysqld_safe的屏幕输出发送到数据目录中的host_name.safe文件。
-->


## mysql_tzinfo_to_sql

该命令用于加载时区表，首先看下 MySQL 中时区的设置。

{% highlight text %}
----- 查看MySQL当前时间以及当前时区
mysql> SELECT curtime();                      # 也可以使用now()
mysql> SHOW VARIABLES LIKE "%time_zone%";
+------------------+--------+
|   Variable_name  | Value  |
+------------------+--------+
| system_time_zone | CST    |                 # system使用CST时区
| time_zone        | SYSTEM |                 # 使用system时区，也就是上述的参数
+------------------+--------+
2 rows in set (0.00 sec)

----- 修改MySQL时区为北京时间，也即东8区；并立即生效
mysql> SET [GLOBAL | SESSION] time_zone='+8:00';
mysql> FLUSH PRIVILEGES;

----- 也可以直接修改配置文件，需要重启服务器
$ cat /etc/my.cnf
[mysqld]
default-time_zone = '+8:00'
{% endhighlight %}

Linux 系统中，与时区相关的内容保存在 /usr/share/zoneinfo 目录下；MySQL 中与时区相关的信息保存在 mysql.time_zone% 表中，可以通过该命令将时区信息导入到 MySQL 中。

{% highlight text %}
----- 将全部时区信息，或者指定时区信息转换为SQL，并导入到MySQL中
$ mysql_tzinfo_to_sql /usr/share/zoneinfo | mysql -u root mysql
$ mysql_tzinfo_to_sql tz_file tz_name | mysql -u root mysql

----- 如果有闰秒，则同时设置
$ mysql_tzinfo_to_sql --leap tz_file | mysql -u root mysql
{% endhighlight %}


## mysqlslap

这是一个压测工具，源码在 client/mysqlslap.c，测试的步骤为：A) 创建数据库+表；B) 插入测试数据；C) 执行压测；D) 删除创建的数据库。

常见参数为：

{% highlight text %}
--delimiter
    分隔符，用于分割命令行或文件指定的SQL；
--auto-generate-sql, -a
    自动生成测试表和数据，并自动生成 SQL 脚本来测试并发压力；
--create-schema (mysqlslap)
    测试 schema 名称，也就是 database；
--only-print
    只打印测试语句而不实际执行；
--concurrency=N, -c N
    控制并发，用于模拟多少个客户端同时执行select，可以指定多个数值，用于不同迭代时的并发；
--iterations=N, -i N
    执行的迭代次数，代表要在不同并发环境下，各自运行测试多少次；
--number-of-queries=N
    总的测试查询次数，其值 = 并发客户数 x 每次查询数；
--debug-info, -T
    最后执行结果打印内存和 CPU 的相关信息；
--auto-generate-sql-load-type=type
    测试语句的类型，代表要测试的环境是读操作还是写操作还是两者混合的，取值包括：read，key，write，update和mixed(默认)；
--auto-generate-sql-add-auto-increment
    代表对生成的表自动添加auto_increment列，从5.1.18版本开始支持；
--number-char-cols=N, -x N
    自动生成的测试表中包含多少个字符类型的列，默认1；
--number-int-cols=N, -y N
    自动生成的测试表中包含多少个数字类型的列，默认1；
--query=name,-q
    使用自定义脚本执行测试，例如可以调用自定义的一个存储过程或者sql语句来执行测试；
--commint=N
    多少条DML后提交一次；
--compress, -C
    如果服务器和客户端支持都压缩，则压缩信息传递；
--engine=engine_name, -e engine_name
    代表要测试的引擎，可以有多个，用分隔符隔开。例如：--engines=myisam,innodb；
--detach=N
    执行N条语句后断开重连。
{% endhighlight %}

如下是常见的测试用例。

{% highlight text %}
----- 单线程测试，打印执行的SQL，查看具体做了什么
$ mysqlslap --auto-generate-sql --only-print -uroot -pYourPassword

----- 设置并发以及循环次数，也就是说在并发为10,20,50时，均执行4次，返回10,20,50时的统计结果
$ mysqlslap --auto-generate-sql --concurrency=10,20,50 --iterations=4 --number-of-queries 1000 \
    -uroot -pYourPassword

----- 测试不同类型存储引擎的性能指标
$ mysqlslap --auto-generate-sql --concurrency=10,20,50 --iterations=4 --number-of-queries 1000 \
    --engine=myisam,innodb -uroot -pYourPassword
{% endhighlight %}

## mysqldump

这是一个逻辑备份工具，其处理流程基本如下。

{% highlight text %}
01 Connect     root@localhost on
02 Query       /*!40100 SET @@SQL_MODE='' */
03 Init DB     foobar
04 Query       SHOW TABLES LIKE 'foobar'
05 Query       LOCK TABLES `foobar` READ /*!32311 LOCAL */
06 Query       SET OPTION SQL_QUOTE_SHOW_CREATE=1
07 Query       SHOW CREATE TABLE `foobar`
08 Query       SHOW FIELDS FROM `foobar`
09 Query       SHOW TABLE STATUS LIKE 'foobar'
11 Query       SELECT * FROM `foobar`
12 Query       UNLOCK TABLES
13 Quit
{% endhighlight %}

接下来，主要查看一些常见的参数。

{% highlight text %}
-q
    导出数据时加了一个SQL_NO_CACHE来确保不会读取缓存里的数据，第11行修改如下；
    Query       SELECT /*!40001 SQL_NO_CACHE */ * FROM `foobar`

--lock-tables
    跟上面类似，不过多加了一个READ LOCAL LOCK，该锁不会阻止读，也不会阻止新的数据插入；

--lock-all-tables
    备份前会发起一个全局的读锁，会阻止对所有表的写入，以此确保数据的一致性，备份完成后会话断开，会自动解锁；
    会在开头增加如下命令；
    Query       FLUSH TABLES
    Query       FLUSH TABLES WITH READ LOCK

--master-data
    和--lock-all-tables参数相同，同时多了个SHOW MASTER STATUS命令；
    Query       FLUSH TABLES
    Query       FLUSH TABLES WITH READ LOCK
    Query       SHOW MASTER STATUS

--single-transaction
    InnoDB表在备份时，通常会启用参数来保证备份的一致性，其工作原理是设定本次会话的隔离级别为REPEATABLE READ，
    以确保本次会话(dump)时，不会看到其他会话已经提交了的数据；同样开始增加如下命令：
    Query       SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ
    Query       BEGIN
    Query       UNLOCK TABLES
{% endhighlight %}



<!--
 6. --single-transaction and --master-data
 本例中，由于增加了选项 --master-data，因此还需要提交一个快速的全局读锁。在这里，可以看到和上面的不同之处在于少了发起 BEGIN 来显式声明事务的开始。这里采用 START TRANSACTION WITH CONSISTENT SNAPSHOT 来代替 BEGIN 的做法的缘故不是太了解，可以看看源代码来分析下。

 081022 17:27:07       2 Connect     root@localhost on
 2 Query       /*!40100 SET @@SQL_MODE='' */
 2 Query       FLUSH TABLES
 2 Query       FLUSH TABLES WITH READ LOCK
 2 Query       SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ
 2 Query       START TRANSACTION WITH CONSISTENT SNAPSHOT
 2 Query       SHOW MASTER STATUS
 2 Query       UNLOCK TABLES
 2 Init DB     foobar
 2 Query       SHOW TABLES LIKE 'foobar'
 2 Query       SET OPTION SQL_QUOTE_SHOW_CREATE=1
 2 Query       show create table `foobar`
 2 Query       show fields from `foobar`
 2 Query       show table status like 'foobar'
 2 Query       SELECT /*!40001 SQL_NO_CACHE */ * FROM `foobar`
 2 Quit

 关于隔离级别可以看手册 13.2.10.3. InnoDB and TRANSACTION ISOLATION LEVEL，或者本站之前的文章：[InnoDB系列] - 实例解析Innodb的隔离级别以及锁模式。

 关于 START TRANSACTION WITH CONSISTENT SNAPSHOT 的说明可以看下手册描述：

 The WITH CONSISTENT SNAPSHOT clause starts a consistent read for storage engines that are capable of it. This applies only to InnoDB. The effect is the same as issuing a START TRANSACTION followed by a SELECT from any InnoDB  table. See Section 13.2.10.4, “Consistent Non-Locking Read”. The WITH CONSISTENT SNAPSHOT clause does not change the current transaction isolation level, so it provides a consistent snapshot only if the current isolation level is one that allows consistent read (REPEATABLE READ or SERIALIZABLE).

 12.4.1. START TRANSACTION, COMMIT, and ROLLBACK Syntax

mysqldump -u root -p --all-databases –flush-privileges --single-transaction --flush-logs --triggers --routines --events –hex-blob > /path/to/backupdir/full_backup-$TIMESTAMP.sql
-->











## mysqldumpslow

该命令用于处理慢查询日志，这是一个 Perl 脚本程序。首先，需要设置好配置文件。

{% highlight text %}
$ cat /etc/my.cnf
[mysqld]
long_query_time=2                    # 设置慢查询的超时时间，单位为秒
log-slow-queries=slow-query.log
{% endhighlight %}

另外，可以通过 log-queries-not-using-indexes 参数设置没有使用索引的 SQL 。

可以使用该命令查看慢查询日志，常用的命令可以通过如下方式查看。

{% highlight text %}
常用参数：
   -h    : 查看帮助信息；
   -t NUM: 显示头NUM条记录；
   -s ARG: 排序参数，有如下的选项，其中前面添加a表示倒序，如ac、at、al、ar；
        c: 记录的次数；
        t: 查询时的时间戳；
        l: 查询使用的时间；
        r: 返回记录数目；
   -g REG: 根据正则表达式进行匹配，大小写不敏感；

实例：
----- 返回访问次数最多的20个SQL语句
$ mysqldumpslow -s c -t 20 slow-query.log
----- 按照时间返回前10条里面含有左连接的SQL语句
$ mysqldumpslow -s t -t 10 -g "left join" slow-query.log
{% endhighlight %}

通过这个工具可以查询出来那些 SQL 语句是性能的瓶颈，如下是上述命令的输出：

{% highlight text %}
Count: 2  Time=2.31s (5s)  Lock=0.00s (0s)  Rows=1000.0 (2000), root[root]@[localhost]
    SELECT * FROM sms_send WHERE service_id<=N GROUP BY content LIMIT N, N;

输出解析：
    出现次数(Count) 2；
    最大耗时时间(Time) 2.31s；
    累计总耗费时间(Time) 5s；
    等待锁的时间(Lock) 0.00s；
    等待锁的总耗时(Lock) 0s；
    发送给客户端的行总数(Rows) 1000；
    扫描的行总数(Rows) 2000；
    用户以及SQL语句本身，其中的数字会被替换为N。
{% endhighlight %}


## mysqlcheck

数据库经常可能遇到错误，譬如数据写入磁盘时发生错误、索引没有同步更新、数据库宕机等；从而可能会导致数据库异常。

mysqlcheck 的功能类似 myisamchk，但其工作不同，前者需要在服务器运行的时候执行，而后者需要停服务。

实际上，mysqlcheck 只是提供了一种方便的使用 SQL 语句的方式，会根据不同类型拼接 SQL 语句，真正调用的还是 ```CHECK TABLE```、```REPAIR TABLE```、```ANALYZE TABLE``` 和 ```OPTIMIZE TABLE``` 命令。

可以通过 3 种方式来调用 mysqlcheck 。

{% highlight text %}
----- 检查表
$ mysqlcheck [options] db_name [tables]

----- 检查多个数据库
$ mysqlcheck [options] ---database DB1 [DB2 DB3...]

----- 检查所有的数据库
$ mysqlcheck [options] --all--database


选项：
  --database，-B
    指定数据库名，可以为多个；
  --all--database，-A
    检查所有数据库；

{% endhighlight %}

<!--
·         --analyze，-a
分析表。

·         --all-in-1，-1
不是为每个表发出一个语句，而是为命名数据库中待处理的所有表的每个数据库执行一个语句。

·         --auto-repair
如果某个被检查的表破坏了，自动修复它。检查完所有表后自动进行所有需要的修复。

·         --character-sets-dir=path
字符集的安装目录。参见5.10.1节，“数据和排序用字符集”。

·         --check，-c
检查表的错误。

·         --check-only-changed，-C
只检查上次检查以来已经更改的或没有正确关闭的表。

·         --compress
压缩在客户端和服务器之间发送的所有信息（如果二者均支持压缩）。

·         ---debug[=debug_options]，-# [debug_options]

写调试日志。debug_options字符串通常为'd:t:o,file_name'。

·         --default-character-set=charset

使用charsetas默认字符集。参见5.10.1节，“数据和排序用字符集”。

·         --extended，-e

如果你正使用该选项来检查表，可以确保它们100%地一致，但需要很长的时间。

如果你正使用该选项来修复表，则运行扩展修复，不但执行的时间很长，而且还会产生大量的垃圾行!

·         --fast，-F

只检查没有正确关闭的表。

·         --force，-f

即使出现SQL错误也继续。

·         --host=host_name，-h host_name

连接给定主机上的MySQL服务器。

·         --medium-check，-m

执行比--extended操作更快的检查。只能发现99.99%的错误，在大多数情况下这已经足够了。

·         --optimize，-o

优化表。

·         --password[=password]，-p[password]

当连接服务器时使用的密码。如果使用短选项形式(-p)，选项和 密码之间不能有空格。如果在命令行中--password或-p选项后面没有 密码值，则提示输入一个密码。

·         --port=port_num，-P port_num

用于连接的TCP/IP端口号。

·         --protocol={TCP | SOCKET | PIPE | MEMORY}

使用的连接协议。

·         --quick，-q

如果你正使用该选项在检查表，它防止扫描行以检查错误链接的检查。这是最快的检查方法。

如果你正使用该选项在修复表，它尝试只修复索引树。这是最快的修复方法。

·         --repair，-r

执行可以修复大部分问题的修复，只是唯一值不唯一时不能修复。

·         --silent，-s

沉默模式。只打印错误消息。

·         --socket=path，-S path

用于连接的套接字文件。

·         --tables

覆盖---database或-B选项。选项后面的所有参量被视为表名。

·         --user=user_name，-u user_name

当连接服务器时使用的MySQL用户名。

·         --verbose，-v

冗长模式。打印关于各阶段程序操作的信息。

·         --version，-V

显示版本信息并退出。





同其它客户端比较，mysqlcheck有一个特殊特性。重新命名二进制可以更改检查表的默认行为(--check)。如果你想要一个工具默认可以修复表的工具，只需要将mysqlcheck重新复制为mysqlrepair，或者使用一个符号链接mysqlrepair链接mysqlcheck。如果调用mysqlrepair，可按照命令修复表。

下面的名可用来更改mysqlcheck的默认行为：
mysqlrepair
默认选项为--repair

mysqlanalyze
默认选项为--analyze

mysqloptimize
默认选项为--optimize

-->

源码在 client/check 目录下，处理过程简单介绍如下。


{% highlight text %}
main()
 |-get_options()                    ← 加载配置文件默认配置
 | |-load_defaults()                ← 通过load_default_groups指定配置文件加载的groups
 |
 |-mysql_check()
   |-disable_binlog()               ← 根据参数设置SET SQL_LOG_BIN=0
   |-process_all_databases()        ← 处理所有数据库
   | |-process_one_db()
   |   |-process_all_tables_in_db()
   |     |-process_selected_tables()
   |
   |-process_selected_tables()
   | |-handle_request_for_tables()  ← 真正的拼接命令处
   |
   |-process_databases()
{% endhighlight %}

其中 handle_request_for_tables() 函数的处理流程如下。

{% highlight cpp %}
static int handle_request_for_tables(string tables)
{
  string operation, options;

  switch (what_to_do) {
  case DO_CHECK:
    operation = "CHECK";
    if (opt_quick)              options+= " QUICK";
    if (opt_fast)               options+= " FAST";
    if (opt_medium_check)       options+= " MEDIUM"; /* Default */
    if (opt_extended)           options+= " EXTENDED";
    if (opt_check_only_changed) options+= " CHANGED";
    if (opt_upgrade)            options+= " FOR UPGRADE";
    break;
  case DO_REPAIR:
    operation= (opt_write_binlog) ? "REPAIR" : "REPAIR NO_WRITE_TO_BINLOG";
    if (opt_quick)              options+= " QUICK";
    if (opt_extended)           options+= " EXTENDED";
    if (opt_frm)                options+= " USE_FRM";
    break;
  case DO_ANALYZE:
    operation= (opt_write_binlog) ? "ANALYZE" : "ANALYZE NO_WRITE_TO_BINLOG";
    break;
  case DO_OPTIMIZE:
    operation= (opt_write_binlog) ? "OPTIMIZE" : "OPTIMIZE NO_WRITE_TO_BINLOG";
    break;
  case DO_UPGRADE:
    return fix_table_storage_name(tables);
  }

  string query= operation + " TABLE " + tables + " " + options;

  if (mysql_real_query(sock, query.c_str(), (ulong)query.length()))
  {
    DBError(sock,
      "when executing '" + operation + " TABLE ... " + options + "'");
    return 1;
  }
  print_result();
  return 0;
}
{% endhighlight %}

也即是实际上会生成如下的命令。

{% highlight text %}
   CHECK TABLE table_name {QUICK|FAST|MEDIUM|EXTENDED|CHANGED|FOR UPGRADE}
  REPAIR NO_WRITE_TO_BINLOG TABLE table_name {QUICK|EXTENDED|USE_FRM}
 ANALYZE NO_WRITE_TO_BINLOG TABLE table_name
OPTIMIZE NO_WRITE_TO_BINLOG TABLE table_name
{% endhighlight %}

每个表会记录最近的一次检查时间，可以通过如下命令查看。

{% highlight text %}
mysql> SELECT table_name, check_time FROM information_schema.tables
          WHERE table_name = 'tbl-name' AND table_schema = 'db-name';
{% endhighlight %}

<!--
create databasename             创建一个新数据库
drop databasename               删除一个数据库及其所有表
flush-hosts                     洗掉所有缓存的主机
flush-logs                      洗掉所有日志
flush-tables                    洗掉所有表
flush-privileges                再次装载授权表(同reload)
kill id,id,...                  杀死mysql线程
reload                          重载授权表
refresh                         洗掉所有表并关闭和打开日志文件
-->

## mysqladmin

该工具最常见的是用来关闭数据库，还可以查看 MySQL 运行状态、进程信息、关闭进程等，如下是常用的子命令；所有命令可以通过 ```--help``` 查看帮助文档。

{% highlight text %}
mysqladmin [option] command [command-option] command ......
参数如下：
  extended-status
    可获得所有MySQL性能指标，即SHOW GLOBAL STATUS的输出
  status
    获取当前MySQL的几个基本的状态值，包括线程数、查询量、慢查询等
  variables
    打印出可用变量
  ping
    查看服务器是否存活
  shutdown
    关掉服务器
  processlist
    显示服务其中活跃线程列表
  version
    得到服务器的版本信息
  password 'new-password'
    新口令，将老口令改为新口令
{% endhighlight %}


### extended-status

默认输出的都是累计值，可以通过 ```-r/--relative``` 查看各个指标的差值；然后再配合 ```-i/--sleep``` 指定刷新的频率。

{% highlight text %}
$ mysqladmin -uroot -pnew-password -h127.1 -P3307 -r -i 1 extended-status |\
   grep "Questions\|Queries\|Innodb_rows\|Com_select \|Com_insert \|Com_update \|Com_delete"

$ mysqladmin -uroot -pnew-password -h127.1 -P3307 -r -i 1 extended-status |\
   awk -F"|" '{\
     if($2 ~ /Variable_name/){\
       print " <-------------    "  strftime("%H:%M:%S") "    ------------->";\
     }\
     if($2 ~ /Questions|Queries|Innodb_rows|Com_select |Com_insert |Com_update |Com_delete/)\
       print $2 $3;\
   }'

$ mysqladmin -uroot -pnew-password -h127.1 -P3307 -r -i 1 extended-status |\
   awk 'BEGIN{ FS="|"; count=0; } { \
   if($2 ~ /Variable_name/ && ((++count)%20 == 1)){ \
       print "----------|---------|--- MySQL Command Status --|----- Innodb row operation ----";\
       print "---Time---|---QPS---|select insert update delete|  read inserted updated deleted";\
   } \
   else if ($2 ~ /Queries/){queries=$3;}\
   else if ($2 ~ /Com_select /){com_select=$3;}\
   else if ($2 ~ /Com_insert /){com_insert=$3;}\
   else if ($2 ~ /Com_update /){com_update=$3;}\
   else if ($2 ~ /Com_delete /){com_delete=$3;}\
   else if ($2 ~ /Innodb_rows_read/){innodb_rows_read=$3;}\
   else if ($2 ~ /Innodb_rows_deleted/){innodb_rows_deleted=$3;}\
   else if ($2 ~ /Innodb_rows_inserted/){innodb_rows_inserted=$3;}\
   else if ($2 ~ /Innodb_rows_updated/){innodb_rows_updated=$3;}\
   else if ($2 ~ /Uptime / && count >= 2){\
     printf(" %s |%9d",strftime("%H:%M:%S"),queries);\
     printf("|%6d %6d %6d %6d",com_select,com_insert,com_update,com_delete);\
     printf("|%6d %8d %7d %7d\n",innodb_rows_read,innodb_rows_inserted,innodb_rows_updated,innodb_rows_deleted);\
   }}'
{% endhighlight %}

## mysqlbinlog

二进制日志 (Binary Log) 是由多个事件 (events) 组成，用于描述对于数据库的修改内容，MySQL 服务器以二进制的形式写入，可以通过该工具显示文件中具体事件的内容。

{% highlight text %}
----- 备份时指定多个binlog文件
$ mysqlbinlog --stop-date="2015-04-20 9:59:59" mysql-bin.[0-9]* | \
    mysql -u root -pyour-password

----- 只恢复单个库example
$ mysqlbinlog --stop-date="2015-04-20 9:59:59" mysql-bin.000001 | \
    mysql -u root -pyour-password --one-database example

----- 指定起始时间以及库
$ mysqlbinlog --start-datetime='2015-08-05 00:00:00' --stop-datetime='2015-08-05 10:00:00' \
    --database=db_name mysql-bin.000001

----- 也可以指定binlog的position位置
$ mysqlbinlog --start-postion=107 --stop-position=1000 --database=db_name mysql-bin.000001

----- 从远程服务器读取
$ mysqlbinlog -u username -p password -h127.1 -P3306 --read-from-remote-server \
    --start-datetime='2015-08-05 00:00:00' --stop-datetime='2015-08-05 10:00:00' mysql-bin.000001
{% endhighlight %}

### ROW格式解析

首先准备下数据。

{% highlight sql %}
CREATE DATABASE test;
USE test;
CREATE TABLE foobar (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name CHAR(20) NOT NULL,
    sex ENUM('F','M') NOT NULL DEFAULT 'M',
    address VARCHAR(30) NOT NULL
) Engine=InnoDB;
INSERT INTO foobar(name,sex,address) VALUES('Barton','M','Washington'),('Borg','M','New Mexico'),
    ('Steven','M','Colorado');
UPDATE foobar SET address='Texas';
{% endhighlight %}

可以直接通过 mysqlbinlog 解析。

{% highlight text %}
----- 解析ROW格式binlog文件
$ mysqlbinlog --no-defaults -v -v --base64-output=DECODE-ROWS mysql-bin.000003
... ...
### INSERT INTO `test`.`foobar`
### SET
###   @1=1 /* INT meta=0 nullable=0 is_null=0 */
###   @2='Barton' /* STRING(60) meta=65084 nullable=0 is_null=0 */
###   @3=2 /* ENUM(1 byte) meta=63233 nullable=0 is_null=0 */
###   @4='Washington' /* VARSTRING(90) meta=90 nullable=0 is_null=0 */
... ...
### UPDATE `test`.`foobar`
### WHERE
###   @1=3 /* INT meta=0 nullable=0 is_null=0 */
###   @2='Steven' /* STRING(60) meta=65084 nullable=0 is_null=0 */
###   @3=2 /* ENUM(1 byte) meta=63233 nullable=0 is_null=0 */
###   @4='Colorado' /* VARSTRING(90) meta=90 nullable=0 is_null=0 */
### SET
###   @1=3 /* INT meta=0 nullable=0 is_null=0 */
###   @2='Steven' /* STRING(60) meta=65084 nullable=0 is_null=0 */
###   @3=2 /* ENUM(1 byte) meta=63233 nullable=0 is_null=0 */
###   @4='Texas' /* VARSTRING(90) meta=90 nullable=0 is_null=0 */
{% endhighlight %}

@1、@2、@3、@4 分别代表了第 1~4 列。

<!--
MySQL 误操作后数据恢复（update,delete忘加where条件）
http://www.cnblogs.com/gomysql/p/3582058.html
-->

## innochecksum

使用该工具时必须停止 MySQL 服务器，否则会报 "Unable to lock file" 错误，如果是在线的最好使用 ```CHECK TABLE``` 命令。

简单来说，该工具会读取 InnoDB 的表空间，计算每个页的 Checksum 值，然后与页中的值比较，如果不同则会报错。

{% highlight text %}
innochecksum --verbose=FALSE --log=/tmp/innocheck.log
常用参数：
  --help/info
    查看帮助信息；
  --verbose
    打印详细信息，可以通过--verbose=FALSE关闭；
  --count/-c
    只打印页的数量信息；
  --start-page=NUM/-s NUM; --end-page=NUM/-e NUM; --page=NUM/-p NUM
    指定开始、结束页，或者只查看指定的页；
  --strict-check/-C
    指定checksum算法，通常有innodb、crc32、none，默认是从三者中选择，否则强制指定；
  --page-type-summary/-S
    打印文件中页的统计信息，包括了总页类型以及数量；
  --page-type-dump=file-name/-D file-name
    打印各个页的详细信息，将其输出到一个文件中；

常用示例：
  ----- 检查系统表空间，也可以使用table-name.ibd，默认出错时才会输出异常
  innodbchecksum ibdata1
  ----- 保存文件中各个页的信息，并在最后打印统计信息
  innodbchecksum -S -D /tmp/page.info schema/*.ibd
{% endhighlight %}


详细使用文档可以参考 [innochecksum](https://dev.mysql.com/doc/refman/en/innochecksum.html) 。

<!--
innochecksum /tmp/data/*/*.ibd
-->

## my_print_defaults

会按照顺序读取配置文件，并提取相应属组的配置项，可以指定多个属组。

{% highlight text %}
----- 使用示例
$ my_print_defaults mysqlcheck client
--user=myusername
--password=secret
--host=localhost

常见参数如下：
  --config-file=file_name, --defaults-file=file_name, -c file_name
    只读取如上选项指定的配置文件。
  --defaults-extra-file=file_name, --extra-file=file_name, -e file_name
    读取全局配置项且在读取用户配置前的配置文件。
{% endhighlight %}

如果不添加任何参数，可以看到配置文件默认的加载顺序。

{% highlight text %}
Default options are read from the following files in the given order:
/etc/mysql/my.cnf /etc/my.cnf ~/.my.cnf
{% endhighlight %}

## 参考

关于 MySQL 自带的程序，可以直接参考官方网站 [Reference Manual - MySQL Programs](https://dev.mysql.com/doc/refman/en/programs.html) 。

{% highlight text %}
{% endhighlight %}
