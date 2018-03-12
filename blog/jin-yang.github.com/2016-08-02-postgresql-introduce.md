---
title: PostgreSQL 简单介绍
layout: post
comments: true
language: chinese
category: [database,postgresql,linux]
keywords: postgresql,简介
description: PostgreSQL 可以说是目前功能最强大、特性最丰富和结构最复杂的开源数据库管理系统，其中有些特性甚至连商业数据库都不具备。这里简单介绍 PG 的常见操作。
---

PostgreSQL 可以说是目前功能最强大、特性最丰富和结构最复杂的开源数据库管理系统，其中有些特性甚至连商业数据库都不具备。

这里简单介绍 PG 的常见操作。

<!-- more -->

![PostgreSQL Logo]({{ site.url }}/images/databases/postgresql/postgresql-logo.jpg "PostgreSQL Logo"){: .pull-center width="30%" }

## 简介

PostgreSQL 可以说是目前功能最强大、特性最丰富和结构最复杂的开源数据库管理系统，其中有些特性甚至连商业数据库都不具备。这个起源于加州大学伯克利分校的数据库，现已成为一项国际开发项目，并且拥有广泛的用户群，尤其是在海外，目前国内使用者也越来越多。

PostgreSQL 基本上见证了数据库理论和技术的发展历程，由 UCB 计算机教授 Michael Stonebraker 于 1986 年创建。在此之前，Stonebraker 教授主导了关系数据库 Ingres 研究项目，88 年，提出了 Postgres 的第一个原型设计。

MySQL 号称是使用最广泛的开源数据库，而 PG 则被称为功能最强大的开源数据库。

### 安装

主要有两种方式：yum 安装和源码安装，其中前者相比要简单很多，后者通常在调试源码时使用。

#### 通过 YUM 安装

在 CentOS 中，可以直接通过 YUM 进行安装，不过默认的版本可能会比较低，在 CentOS 7 上默认安装的是 9.2，可以通过 yum info 查看当前的版本。

{% highlight text %}
# yum install postgresql postgresql-server
{% endhighlight %}

如果要安装最新版本，可以从 [yum.postgresql.org](http://yum.postgresql.org/) 上下载 repos 安装包，安装数据源。这也是我们推荐的一种方式，一下中的 XXX 标记，可能会根据不同的版本有所区别，不过操作大致相同。

{% highlight text %}
----- 安装相应PG版本的repository，并更新库
# rpm -ivh pgdg-centos-XXX.rpm
# yum update

----- 安装相应的版本，包括常用的pgadmin工具
# yum --enablerepo=pgdgXX install postgresqlXX-server pgadmin3_XX postgresqlXX-contrib

----- 另外，安装完之后需要配置PATH环境变量，并使之生效
# cat /etc/profile
pathmunge /usr/pgsql-X.X/bin
# source /etc/profile
{% endhighlight %}

通过 YUM 方式安装后，同时会创建一个 postgres 用户，以及同名用户组，直接切换到该用户即可，执行相关的数据库操作即可。

{% highlight text %}
# su - postgres

----- 初始化db，如下是安装之后默认的数据存储目录
$ initdb -D /var/lib/pgsql/X.X/data
{% endhighlight %}

在 CentOS 7 中，对于会同时安装如下的启动文件，其它的 systemctl 操作基本就相同了。

{% highlight text %}
# ls /usr/lib/systemd/system/postgresql-X.X.service
# systemctl start postgresql-X.X
{% endhighlight %}

#### 通过源码安装

直接从官网 [www.postgresql.org](https://www.postgresql.org) 下载相应版本的源码包。

{% highlight text %}
----- 可以选择新建PG专用的用户
# groupadd postgres
# useradd -g postgres postgres

----- 解压、编译
$ tar -xf postgresql-X.X.X.tar.bz2 && cd postgresql-X.X.X
$ ./configure --prefix=/opt/postgre
$ make
# make install

----- 同样可以编译contrib目录下的一些工具
$ cd contrib && make
# make install

----- 绑定数据库文件存储目录
$ export PGDATA=/home/andy/Workspace/databases/postgre/data
$ /opt/postgre/bin/initdb -D $PGDATA

----- 常见操作，如启动、停止数据库
$ /opt/postgre/bin/pg_ctl -D $PGDATA -l logfile start
$ /opt/postgre/bin/pg_ctl -D $PGDATA -l logfile stop
{% endhighlight %}

### 日志查看

PG 的日志分为三类，分别是 `pg_log`、`pg_xlog` 和 `pg_clog`，一般保存在 `$PGDATA` 对应的目录下。

1. pg_log  数据库运行日志，默认开启，可以通过配置 `$PGDATA/postgresql.conf` 。
2. pg_xlog WAL日志，强制开启。
3. pg_clog 事务提交日志，记录事务的元数据，强制开启。

### 防火墙、SELinux 设置

PG 默认使用 `5432` 端口，也可以在 `postgresql.conf` 文件中设置，可以使用如下命令开启防火墙端口。

{% highlight text %}
# firewall-cmd --add-port=5432/tcp                          # 暂时有效
# firewall-cmd --permanent --add-port=5432/tcp              # 永久生效
----- 在iptables中开启
# iptables -A INPUT -p tcp --dport 5432 -m state --state NEW,ESTABLISHED -j ACCEPT
{% endhighlight %}

如果开启了 SELinux 服务，可能会在使用中遇到各种各样的权限问题。

{% highlight text %}
----- 修改数据库的存放位置，必须添加一些新上下文来匹配新位置
# semanage fcontext -a -t postgresql_db_t "/new/location(/.*)?"
----- 默认端口不起作用，需要匹配postgre的端口类型为你想要的端口
# semanage port -a -t postgresql_port_t -p tcp 5433
----- 如果APP需要通过TCP/IP与PG交互，你需要告诉SELinux允许这个操作
# setsebool -P httpd_can_network_connect_db on
{% endhighlight %}

### 设置用户和数据库

创建一个用户，并为用户创建一个数据库。
{% highlight text %}
$ psql -U postgres -W
psql (9.3.2)
Type "help" for help.

postgres=# \password postgres         # 设置一下密码

----- 可以通过如下方式创建用户、数据库
postgres=# CREATE USER foobar WITH PASSWORD 'justkidding';
postgres=# CREATE DATABASE test OWNER foobar;

----- 也可以直接在shell中创建
$ createuser foobar
$ createdb --owner=foobar test
{% endhighlight %}

### 配置

PG 主要使用了两个配置文件 `/var/lib/pgsql/data/{postgresql.conf, pg_hba.conf}`，其中一些配置参数可以通过命令行选项传给守护进程，此时会覆盖配置文件中的设置。

例如，如果想要修改服务的端口为 5433，创建一个名为 `/etc/systemd/system/postgresql.service` 的文件，包含以下内容。

{% highlight text %}
.include /lib/systemd/system/postgresql.service
[Service]
Environment=PGPORT=5433
{% endhighlight %}

当数据库安装后，可以通过编辑 `/var/lib/pgsql/data/pg_hba.conf` 文件来实现权限管理。

{% highlight text %}
# TYPE    DATABASE        USER            ADDRESS                 METHOD
  host    all             all             127.0.0.1/32            md5
  local   all             postgres                                peer
{% endhighlight %}






## 常见操作

接下来，我们使用默认的用户 postgres 登陆，执行一些 CURD (Create, Update, Read, Delete) 操作。

{% highlight text %}
$ /opt/postgre/bin/createdb test             # 创建test数据库
$ /opt/postgre/bin/psql test                 # 链接到test数据库
test=# create table test1(id integer);       # 建表
test=# insert into test1 values(1);          # 插入数据
test=# select * from test1;                  # 查询
{% endhighlight %}

也可以直接登陆创建。

{% highlight text %}
$ psql -U postgres -W                        # 直接登陆，默认无密码
postgres=# CREATE USER foobar WITH PASSWORD 'password';          # 创建用户
postgres=# CREATE DATABASE test OWNER foobar ENCODING 'UTF8';    # 以及数据库

$ psql -U foobar -W test
Password for user foobar: password
test=> \q
{% endhighlight %}

可以直接修改配置文件，允许远程访问。

{% highlight text %}
----- 修改/var/lib/pgsql/data/postgresql.conf
listen_addresses ='*'

----- 修改/var/lib/pgsql/data/pg_hba.conf
host    all             all             0.0.0.0/0            trust
{% endhighlight %}

### 状态查看

{% highlight text %}
----- 查看PG的当前进程
# ps auxww | grep ^postgres
{% endhighlight %}
主要包括了两类进程，分别是启动时的后台进程，以及用户连接的进程；对于前者，其中 stats collector 和 autovacuum launcher 两个进程是可选启动的，各个进程作用如下。
{% highlight text %}
postgres: 主进程，接收客户端连接，创建服务进程；启动时拉起和管理后台进程。
achiever process: 事务日志归档进程。
{% endhighlight %}

而用户进程通常格式如下：

{% highlight text %}
postgres: user database host activity
{% endhighlight %}

其中 activity 的状态为 idle (正在等待用户输入命令)、idle in transaction、waiting (等待锁) 等。

### CTID

在 PG 中的 ctid 表示数据记录的物理行信息，用于标示一条记录位于那个数据块的那个位移上面，类似于 Oracle 中的 rowid 。

{% highlight text %}
postgres=# CREATE TABLE test(x int, y varchar(30), z date) TABLESPACE ts_demo;
CREATE TABLE
postgres=# INSERT INTO test VALUES(1, 'ShangHai', now()), (2, 'NanJing', now()), (3, 'HangZhou', now());
INSERT 0 3
postgres=# INSERT INTO test SELECT generate_series(4, 1000), 'JiNan '||generate_series(4, 1000), now();
postgres=# SELECT ctid, * FROM test;
postgres=# ANALYZE test;
postgres=# SELECT relpages, reltuples FROM pg_class WHERE relname = 't';
{% endhighlight %}

当删除了数据之后，原有的空间不会自动释放，可以通过 vacuum tbl 回收。

<!-- generate_series(0, 100, 2); -->

### 索引

{% highlight text %}
----- 创建唯一约束
postgres=# ALTER TABLE tbl ADD CONSTRAINT UK_tbl_col UNIQUE(x, y);

----- 创建函数索引
postgres=# CREATE INDEX idx_tbl_col ON tbl USING btree(UPPER(x), y);

----- 创建部分索引
postgres=# CREATE INDEX idx_tbl_col ON tbl USING btree(UPPER(x), y) WHERE z IS NULL;
{% endhighlight %}

### 常用命令

pgsql 常见的命令。

{% highlight text %}
----- 查看帮助
postgre=# \h SELECT            # 查看SQL命令的帮助
postgre=# \?                   # 查看psql命令的帮助

----- 导入/导出数据
postgre=# \i file.sql          # 从某个文件导入
postgre=# COPY weather FROM 'file.txt';

----- 切换数据库，相当于MySQL的use dbname
postgre=# \c dbname username IP PORT

----- 查看数据库，相当于MySQL的show databases
postgre=# \l    \list

----- 查看表，相当于MySQL的show tables
postgre=# \dt

----- 查看表结构，相当于desc
postgre=# \d tblname           # \d+ tblname查看详细信息

----- 查看索引
postgre=# \di

----- 查看用户
postgre=# \du

----- 在文本编辑器中编译，退出后执行
postgre=# \e

----- 查看当前连接的信息
postgre=# \conninfo

----- 其它杂项
postgre=# \! shell-command                   # 执行终端的命令
postgre=# \set COMP_KEYWORD_CASE upper       # 设置自动提示关键字大写显示
postgre=# \x auto                            # 如果列较多时，通过行显示，其中\x表示直接行显示
postgre=# \pset null ¤                       # 当值为NULL时显示如下的字符，以区分空格
{% endhighlight %}

PG 中通过 `::` 进行类型转换；另外，支持一些常见的字符串匹配函数，如 `ilike`、`~*` 等，详细可以参考 [Pattern Matching](https://www.postgresql.org/docs/9.6/static/functions-matching.html) 。

## 常用概念

在 PostgreSQL 中，有各种各样的概念，常见的有表空间、数据库、模式、表、用户、角色等。

### 角色 VS. 用户

这两个可以理解为相同，只是两者在创建时默认行为的区别，其它基本一致。文档中，对两者进行了简单的说明 `CREATE USER is the same as CREATE ROLE except that it implies LOGIN.` ，也就是说如下的命令是等价的。

{% highlight text %}
CREATE ROLE foobar PASSWORD 'foobar' LOGIN;
CREATE USER foobar PASSWORD 'foobar';
{% endhighlight %}

### 数据库 VS. 模式

简单来说模式 (Schema) 就是对数据库 (Database) 的逻辑分割，而且在数据库创建的时候，已经默认创建了一个 public 模式，在此数据库中创建的对象，如表、函数、试图、索引、序列等都保存在这个模式中。

{% highlight text %}
----- 1. 创建一个数据库
CREATE DATABASE tsdb;
----- 2. 链接到新建的数据库，并查看其中的模式
\c tsdb
\dn
----- 3. 新建一张测试表
CREATE TABLE test(id INTEGER NOT NULL);
\d
----- 4. 创建新的模式，同时设置属主为默认用户，并查看当前库所有的表
CREATE SCHEMA foobar AUTHORIZATION postgres;
CREATE TABLE foobar.test (id INTEGER NOT NULL);
SELECT * FROM pg_tables WHERE schemaname NOT IN('pg_catalog', 'information_schema');
----- 5. 如果通过\d查看时，需要设置搜索路径
SHOW search_path;
SET search_path TO 'foobar,public'
\d
{% endhighlight %}

也就是说，数据库通过模式做逻辑区分，而且一个数据库至少包含一个模式，接到一个数据库后，可以通过 `search_path` 设置搜索顺序。

### 表空间 VS. 数据库

在通过 `CREATE DATABASE dbname` 语句创建数据库时，默认的数据库所有者是当前创建数据库的角色，默认表空间是系统的默认表空间 pg_default ，其主要原因是创建是通过克隆数据库模板实现的。

如上创建数据库时，如果没有指明数据库模板，系统将默认克隆 template1 数据库，其默认表空间是 pg_default ，其完整的语句如下。

{% highlight text %}
CREATE DATABASE dbname OWNER foobar TEMPLATE template1 TABLESPACE tablespacename;
{% endhighlight %}

实际上可以通过如下的步骤进行测试：

{% highlight text %}
----- 1. 切换到template1数据库并新建一个表进行测试
\c template1
CREATE TABLE test(id INTEGER NOT NULL);
INSERT INTO test VALUES (1);
----- 2. 创建一个表空间，需要注意对应的目录存在且为空
CREATE TABLESPACE tsfoobar OWNER postgres LOCATION '/tmp/foobar';
----- 3. 创建一个数据库
CREATE DATABASE dbfoobar TEMPLATE template1 OWNERE postgres TABLESPACE tsfoobar;
{% endhighlight %}

链接查看新数据库时，实际上存在一个表，而且有上述写入的数据。表空间是一个存储区域，在一个表空间中可以存储多个数据库，尽管 PostgreSQL 不建议这么做，例如将索引保存到 SSD 中，而数据保存到 SATA 中。


## 常用特性

### sequece

序列对象，也被称为序列生成器，实际上就是用 `CREATE SEQUENCE` 创建的特殊的单行表，通常用来表生成唯一的标识符。

{% highlight text %}
----- 直接在建表时使用serial类型，默认生成为tblname+colname+'seq'
postgres=# CREATE TABLE tbl_seq(id SERIAL, name TEXT);
CREATE TABLE
postgres=# \d tbl_seq
                         Table "public.tbl_seq"
 Column |  Type   |                      Modifiers
--------+---------+------------------------------------------------------
 id     | integer | not null default nextval('tbl_seq_id_seq'::regclass)
 name   | text    |
postgres=# \d tbl_seq_id_seq
       Sequence "public.tbl_seq_id_seq"
    Column     |  Type   |        Value
---------------+---------+---------------------
 sequence_name | name    | tbl_seq_id_seq
 last_value    | bigint  | 1
 start_value   | bigint  | 1
 increment_by  | bigint  | 1
 max_value     | bigint  | 9223372036854775807
 min_value     | bigint  | 1
 cache_value   | bigint  | 1
 log_cnt       | bigint  | 0
 is_cycled     | boolean | f
 is_called     | boolean | f
Owned by: public.tbl_seq.id
postgres=# SELECT * FROM tbl_seq_id_seq;
 sequence_name  | last_value | start_value | increment_by | ...
----------------+------------+-------------+--------------+ ...
 tbl_seq_id_seq |          1 |           1 |            1 | ...
(1 row)

----- 单独创建序列，建表时指定，不过该列需要为int类型
postgres=# CREATE SEQUENCE seq_tblseq2 INCREMENT BY 1 MINVALUE 1 NO MAXVALUE START WITH 1;
CREATE SEQUENCE
postgres=# CREATE TABLE tbl_seq2(id INT NOT NULL DEFAULT nextval('seq_tblseq2'), name TEXT);
CREATE TABLE
{% endhighlight %}

其中 `CREATE SEQUECE` 的语法可以参考 [www.postgresql.com](https://www.postgresql.com/docs/current/static/sql-createsequence.html)，接下来看看如何使用序列。

{% highlight text %}
postgres=# INSERT INTO tbl_seq VALUES(nextval('tbl_seq_id_seq'), 'Lucy');
INSERT 0 1
postgres=# INSERT INTO tbl_seq(name) VALUES('Andy');
INSERT 0 1
{% endhighlight %}

还有一个问题是在数据迁移之后，如何设置 sequence 。

{% highlight text %}
postgres=# TRUNCATE tbl_seq;
TRUNCATE TABLE
postgres=# INSERT INTO tbl_seq(name) VALUES('Sanndy'), ('David'), ('Simon'), ('Peter');
INSERT 0 4
postgres=# select * from tbl_seq;
 id |  name
----+--------
  8 | Sanndy
  9 | David
 10 | Simon
 11 | Peter
(4 rows)
postgres=# COPY tbl_seq TO '/tmp/tbl_seq.sql';
COPY 4
postgres=# TRUNCATE tbl_seq;
TRUNCATE TABLE

------ 尝试恢复
postgres=# BEGIN
BEGIN
postgres=# COPY tbl_seq FROM '/tmp/tbl_seq.sql';
COPY 4
postgres=# SELECT setval('tbl_seq_id_seq', max(id)) from tbl_seq;
 setval
--------
     11
(1 row)
postgres=# END;
COMMIT

postgres=# INSERT INTO tbl_seq(name) VALUES('Monica');
INSERT 0 1
postgres=# select * from tbl_seq;
 id |  name
----+--------
  8 | Sanndy
  9 | David
 10 | Simon
 11 | Peter
 12 | Monica
(5 rows)
{% endhighlight %}

其它的一些常用函数还包括了：

1. `nextval(seq)`：递增到下一个值，并返回当前值，即使多个会话并发执行该函数，每个进程也会安全的收到一个唯一的序列值。
2. `currval(seq)`：当前会话返回的最近一次的值，如果从没有执行过 `nextval()` 则会返回错误。
3. `setval(seq, bigint, boolean)`：重置序列对象的值，如果最后一个参数为 false，那么 nextval 首先返回该值，然后才开始递增。

删除非常简单。

{% highlight text %}
postgres=# DROP SEQUENCE seq;
{% endhighlight %}

### cursor

在一个查询中，可以使用游标 (cursor)，防止一个大查询超过了内容的容量，而对于 PL/pgSQL 来说，FOR 语句则默认使用了游标。


{% highlight text %}
postgre=# CREATE TABLE foobar(id INT);
CREATE TABLE
postgre=# INSERT INTO foobar VALUES(generate_series(1, 1000));
INSERT 0 1000

postgre=# BEGIN;
BEGIN
postgre=# DECLARE cur CURSOR FOR SELECT * FROM foobar;
DECLARE CURSOR
postgre=# FETCH FIRST FROM cur;           # 移动到第一行，并返回第一行数据
 id
----
  1
(1 row)
postgre=# FETCH NEXT FROM cur;            # 获取下一行
 id
----
  2
(1 row)
postgre=# MOVE LAST IN cur;               # 移动到最后
MOVE 1
postgre=# CLOSE cur;                      # 关闭游标
CLOSE CURSOR
{% endhighlight %}

另外，PG 允许在函数中返回对 cursor 的引用，这样就可以直接通过函数返回一个大的对象。

### 常用程序

简单介绍下 PG 相关的程序，部分比较复杂的只是在此简单记录下，详细介绍可以参考后面的文章。

#### pg_config

用来显示当前版本的一些配置参数，如 bin 目录、版本信息、cpp flags 等。

#### pg_controldata

用来显示当前 cluster 的一些详细信息。

{% highlight text %}
$ pg_controldata -D $PGDATA
{% endhighlight %}

#### pg_test_fsync

用于测试不同的 wal_sync_method 参数所具有的性能。

#### pg_test_timing

用于时间测试的程序，详细可以参考文档。



<!--
# PL/pgSQL

{% highlight sql %}
CREATE FUNCTION somefunc() RETURNS integer AS $$
<< outerblock >>
DECLARE
quantity integer := 30;
BEGIN
RAISE NOTICE 'Quantity here is %', quantity; -- Prints 30
quantity := 50;
--
-- Create a subblock
--
DECLARE
quantity integer := 80;
BEGIN
RAISE NOTICE 'Quantity here is %', quantity; -- Prints 80
RAISE NOTICE 'Outer quantity here is %', outerblock.quantity;
END;
RAISE NOTICE 'Quantity here is %', quantity;
-- Prints 50
-- Prints 50
RETURN quantity;
END;
$$ LANGUAGE plpgsql;
{% endhighlight %}


## 多实例部署

有些常见的操作，如主备，需要在同一台机器上部署多个实例。

## WAL 日志解析

9.3 之后提供了 pg_xlogdump 工具用来解析 xlog 中的内容，对于之前的版本可以考虑使用 [xlogdump-github](https://github.com/snaga/xlogdump) 这个开源的软件。

通过该工具找到精准的误操作XID。

pg_ctl reload 重新加载即可。
-->




## 参考

可以参考官方网站 [www.postgresql.org](http://www.postgresql.org/)，以及 [PostgreSQL 中文社区](http://www.postgres.cn/) 。国内 PG 研究比较多的人，可以查看 [PostgreSQL Research](http://blog.163.com/digoal@126/)。

源码可以参考 [Github Postgres](https://github.com/postgres/postgres) 。

<!--
pgcli.com
http://www.rails365.net/groups/postgresql

[xxxx](http://blog.itpub.net/30088583) 。

http://www.postgres.cn/news/viewone/1/99                 参考资料
https://github.com/ty4z2008/Qix/blob/master/pg.md

http://www.2cto.com/database/201608/541209.html
https://pan.baidu.com/share/home?uk=1982970774#category/type=0

pgadmin3  管理工具

PostgreSQL 9 Administration CookBook    一本不错的书籍
-->


{% highlight text %}
{% endhighlight %}
