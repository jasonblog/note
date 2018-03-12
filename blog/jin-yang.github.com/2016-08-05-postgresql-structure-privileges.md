---
title: PostgreSQL 结构及权限
layout: post
comments: true
language: chinese
category: [database,postgresql,linux]
keywords: postgresql,权限管理,表空间
description: 在 PostgreSQL 中，有各种各样的概念，例如，一个实例中允许创建多个数据库，每个数据库中可以创建多个 schema，每个 schema 下面可以创建多个对象，包括了表、物化视图、操作符、索引、视图、序列、函数 ... ... 等等。另外，PG 中还有表空间的概念，这就导致上面的概念越来越混乱。本文主要是理解 PostgreSQL 的逻辑结构和权限体系，快速理解和管理数据库的权限。
---

在 PostgreSQL 中，有各种各样的概念，例如，一个实例中允许创建多个数据库，每个数据库中可以创建多个 schema，每个 schema 下面可以创建多个对象，包括了表、物化视图、操作符、索引、视图、序列、函数 ... ... 等等。

另外，PG 中还有表空间的概念，这就导致上面的概念越来越混乱。

本文主要是理解 PostgreSQL 的逻辑结构和权限体系，快速理解和管理数据库的权限。

<!-- more -->

## PG 结构简介

首先，介绍下 PG 中常见的几个概念，包括了 Cluster、Database、Schemas 。

* Cluster：抽象概念，一系列 DBs 的集合，但是不像 DBs 可以直接通过 SQL 在内部表中查看。一个集群在一个目录树下 (一般通过 `$PGDATA` 指定目录根)，通过一个 postmaster 进程提供服务。
* Databases：一系列 Schemas 的集合，在一个 Cluster 中名字必须唯一。
* Schemas：一系列 tables、functions、indexs、data types、operators 的集合，名称需要在一个 DB 中唯一。主要是提供了一个逻辑的命名空间，在一个 DB 中，可以通过 prefix 引用不同的 schema 。

另外，需要注意的是，在一个 cluster 中，有四个系统表是在各个 DBs 中共享的：

* pg_group 显示用户组列表，只是用于兼容 8.1 之前的版本。
* pg_database 该 cluster 中的数据库列表，默认会有 postgres、template0、template1 三个数据库。
* pg_shadow 列出有效的用戶，兼容 8.1 之前的表，是 pg_authid 中 rolcanlogin 为 true 的值；另外，pg_user 是 pg_shadow 的视图，只是将口令设置成了空白。
* pg_tablespace 关于 tablespaces 的列表。

这也就意味着一个 Cluster 中有多个 DB，用户和用户组跨 DB 在整个 Cluster 中共享，但是数据不能共享 DB 。每次用户连接时，只能连接到一个 cluster 的某个 database，可以直接通过 ps 命令查看进程。

### Schema

Schema 是 PG-7.3 引入的特性，在 PG 中是一个逻辑概念，一个用户可以访问他所连接数据库中的任意 schema 中的对象，只要他有权限即可。之所以又添加了 schema，主要有如下原因：

1. 允许多个用户互不干扰地使用同一个数据库，从逻辑上将 DB 分组以方便管理。
2. 提供 database 下的命名空间隔离，这样的好处是，灵活。例如，可以创建月份的 schema，每个里面的表结构都相同。
3. PG 的限制是一个连接只能连接到一个数据库，这样一个连接就可以操作不同 schema 下的对象。

每个客户端的连接都会存在一个搜索路径 (search path)，如果没有指定 schema，那么 PG 就会按照该路径进行搜索。

{% highlight text %}
----- 查看搜索路径，如果$user的schema不存在，则会直接忽略
postgres=# SHOW SEARCH_PATH;
   search_path
-----------------
 "$user", public
(1 row)

----- 可以修改搜索路径
postgres=# SET SEARCH_PATH TO 'foo','bar','public';
SET
{% endhighlight %}

另外的一些常见操作可以参考如下：

{% highlight text %}
----- 查询schema
postgres=# \dn+

----- 创建schema以及其中的表
postgres=# CREATE SCHEMA foo;
CREATE SCHEMA
postgres=# CREATE TABLE foo.foobar(id INTEGER);
CREATE TABLE

----- 删除schema，如果非空，则通过cascade强制删除
postgres=# DROP SCHEMA foo CASCADE;
{% endhighlight %}

除了 public 和用户创建的 schema 之外，还存在一个 `pg_catalog`，它包含了系统表所有的内建数据类型、函数、操作符，它总是搜索路径的一部分，只是没有在搜索路径中显示。

### Databases

DB 的创建语句如下，可以指定 owner 以及 tablespace 。

{% highlight text %}
CREATE DATABASE db_name
[
    [ WITH]
    [ OWNER [ = ] user_name ]                     # 所属用户，默认属于创建的用户
    [ TEMPLDATE [ =] templdate ]                  # 创建数据库需要的模板，默认template1
    [ ENCODING [ = ] encoding ]                   # 数据库使用的编码，如utf8
    [ LC_COLLATE [ = ] lc_collate ]               # 排序类型
    [ LC_CTYPE [ = ] lc_ctype ]                   # 同上
    [ TABLESPACE [ = ] tablespace ]               # 数据库的默认表空间
    [ CONNECTION LIMIT [ = ] connection limit ]   # 数据库的最大连接数，默认为-1，禁用连接
]

----- 示例
POSTGRES=# CREATE DATABASE demodb TEMPLATE template0 ENCODING 'UTF8' TABLESPACE ts_demo01 CONNECTION LIMIT 200;
CREATE DATABASE
{% endhighlight %}

如果是 superuser 可以指定不同的 OWNER ，否则使用当前的用户。TEMPLATE 则指定了复制的模版，会将该模版中定义的所有 tables、views、data types、functions、operators 复制到新的数据库目录下，默认采用 template1 。

{% highlight text %}
----- 查看详细信息
postgres=# \l+
{% endhighlight %}

在 PG 中，数据库、表、索引的关系如下：

* 数据库：一个 PG 数据库下可以管理多个数据库，当应用连接的时候，只能连接到其中的一个数据库，而一个数据库只能属于一个实例。

* 表、索引：一个数据库里可以有多个表与索引，PG 称为 "Relation"。

* 数据行：在每张表中可以有很多数据行，PG 称为 "Tuple"。


<!--
## Table

{% highlight text %}
\d+
\dS+
{% endhighlight %}

然后，数据库与模式的关系

看文档了解到：模式（schema）是对数据库（database）逻辑分割，相当于名称空间。
在数据库创建的同时，就已经默认为数据库创建了一个模式--public，这也是该数据库的默认模式。所有为此数据库创建的对象（表、函数、试图、索引、序列等）都是常见在这个模式中的。
 实验如下：
 1.创建一个数据库dbtt----CREATE DATABASE dbtt;
 2.用kanon角色登录到dbtt数据库,查看dbtt数据库中的所有模式：\dn; 显示结果是只有public一个模式。
 3.创建一张测试表----CREATE TABLE test(id integer not null);
 4.查看当前数据库的列表： \d; 显示结果是表test属于模式public.也就是test表被默认创建在了public模式中。
 5.创建一个新模式kanon，对应于登录用户kanon：CREATE SCHEMA kanon OWNER kanon；
 6.再次创建一张test表，这次这张表要指明模式----CREATE TABLE kanon.test (id integer not null);
 7.查看当前数据库的列表： \d; 显示结果是表test属于模式kanon.也就是这个test表被创建在了kanon模式中。
   得出结论是：数据库是被模式(schema)来切分的，一个数据库至少有一个模式，所有数据库内部的对象(object)是被创建于模式的。用户登录到系 统，连接到一个数据库后，是通过该数据库的search_path来寻找schema的搜索顺序,可以通过命令SHOW search_path；具体的顺序，也可以通过SET search_path TO 'schema_name'来修改顺序。
   官方建议是这样的：在管理员创建一个具体数据库后，应该为所有可以连接到该数据库的用户分别创建一个与用户名相同的模式，然后，将search_path设置为"$user"，
   这样，任何当某个用户连接上来后，会默认将查找或者定义的对象都定位到与之同名的模式中。这是一个好的设计架构。


接下来，再来研究下表空间与数据库的关系
    数据库创建语句CREATE DATABASE dbname 默认的数据库所有者是当前创建数据库的角色，默认的表空间是系统的默认表空间--pg_default。
    为什么是这样的呢？因为在PostgreSQL中，数据的创建是通过克隆数据库模板来实现的，这与SQL SERVER是同样的机制。
    由于CREATE DATABASE dbname并没有指明数据库模板，所以系统将默认克隆template1数据库，得到新的数据库dbname。（By default, the new database will be created by cloning the standard system database template1）.

    而template1数据库的默认表空间是pg_default，这个表空间是在数据库初始化时创建的，所以所有template1中的对象将被同步克隆到新的数据库中。
    相对完整的语法应该是这样的：CREATE DATABASE dbname OWNER kanon TEMPLATE template1 TABLESPACE tablespacename;
    下面我们来做个实验验证一下：
 1.连接到template1数据库，创建一个表作为标记：CREATE TABLE tbl_flag(id integer not null);向表中插入数据INSERT INTO tbl_flag VALUES (1);
 2.创建一个表空间:CREATE TABLESPACE tskanon OWNER kanon LOCATION '/tmp/data/tskanon';在此之前应该确保目录/tmp/data/tskanon存在，并且目录为空。
 3.创建一个数据库，指明该数据库的表空间是刚刚创建的tskanon：CREATE DATABASE dbkanon TEMPLATE template1 OWNERE kanon TABLESPACE tskanon;
 4.查看系统中所有数据库的信息：\l；可以发现，dbkanon数据库的表空间是tskanon,拥有者是kanon;
 5.连接到dbkanon数据库，查看所有表结构:\d;可以发现，在刚创建的数据库中居然有了一个表tbl_flag,查看该表数据，输出结果一行一列，其值为1，说明，该数据库的确是从template1克隆而来。

 仔细分析后，不难得出结论：在PostgreSQL中，表空间是一个目录，里面存储的是它所包含的数据库的各种物理文件。

最后，我们回头来总结一下这张关系网
    表空间是一个存储区域，在一个表空间中可以存储多个数据库，尽管PostgreSQL不建议这么做，但我们这么做完全可行。
    一个数据库并不知直接存储表结构等对象的，而是在数据库中逻辑创建了至少一个模式，在模式中创建了表等对象，将不同的模式指派该不同的角色，可以实现权限 分离，又可以通过授权，实现模式间对象的共享，并且，还有一个特点就是：public模式可以存储大家都需要访问的对象。
    这样，我们的网就形成了。可是，既然一个表在创建的时候可以指定表空间，那么，是否可以给一个表指定它所在的数据库表空间之外的表空间呢？
    答案是肯定的!这么做完全可以：那这不是违背了表属于模式，而模式属于数据库，数据库最终存在于指定表空间这个网的模型了吗？！
    是的，看上去这确实是不合常理的，但这么做又是有它的道理的，而且现实中，我们往往需要这么做：将表的数据存在一个较慢的磁盘上的表空间，而将表的索引存在于一个快速的磁盘上的表空间。
    但我们再查看表所属的模式还是没变的，它依然属于指定的模式。所以这并不违反常理。实际上，PostgreSQL并没有限制一张表必须属于某个特定的表空间，我们之所以会这么认为，是因为在关系递进时，偷换了一个概念：模式是逻辑存在的，它不受表空间的限制。




https://www.postgresql.org/docs/9.5/static/sql-grant.html

如何在PostgreSQL中建只读账号

在PostgreSQL中并没有CREATE TABLE权限名称，这是与其它数据库不同的一个地方，PostgreSQL是通过控制是否在模式schema中上有CREATE控制用户的能否创建表的权限的，默认安装下，任何用户都有在模式public中CREATE的权限，所以要创建只读账号的第一步，我们要先去除在模式public中的CREATE权限：

REVOKE  CREATE  ON SCHEMA public from public;

下面的SQL创建了一个名为“readonly”的用户：

CREATE USER readonly with password 'query';

然后把现有的所有在public这个schema下的表的SELECT权限赋给用户readonly，执行下面的SQL命令：

GRANT SELECT ON  ALL TABLES IN SCHEMA public TO readonly;

上面的SQL命令只把现有的表的权限给了用户readonly，但如果这之后创建的表，readonly用户还是不能读，需要使用下面的SQL把以后创建的表的SELECT权限也给用户readonly：

ALTER DEFAULT PRIVILEGES IN SCHEMA public grant select on tables to readonly;

注意：上面的过程只是把名称为public的schema下的表赋了只读权限，如果想让这个用户能访问其它schema下的表，需要重复执行：

GRANT SELECT ON  ALL TABLES IN SCHEMA other_schema TO readonly;
ALTER DEFAULT PRIVILEGES IN SCHEMA other_schema grant select on tables to readonly;




pg 里面如何实现一个用户的只读访问呢？

oracle 里 我们通过对每个对象的单独的授予 select 权限，解决。

整个数据库级别的只读怎么做？ oracle 里应该是对应到schema 级别的，设置表空间只读。

pg 下面怎么做呢？

PG 里有个参数  default_transaction_read_only[code]


postgres=# create user lsl password 'lsl' ;
CREATE ROLE
postgres=# alter user lsl set default_transaction_read_only=on;
ALTER ROLE
postgres=# grant all on database cms_pcbaby_app to lsl;
GRANT
cms_pcbaby_app=> create table t(id int) ;
ERROR:  cannot execute CREATE TABLE in a read-only transaction
cms_pcbaby_app=>
[/code]数据库级别也可以设置这个参数的。[code]
cms_pcbaby_app=> \c postgres postgres
You are now connected to database "postgres" as user "postgres".
postgres=# create database  p with wner=postgres ;
CREATE DATABASE

postgres=# alter database p set default_transaction_read_only= on ;
ALTER DATABASE
postgres=# \c p
You are now connected to database "p" as user "postgres".
p=# create table abc (id int) ;
ERROR:  cannot execute CREATE TABLE in a read-only transaction
p=#
[/code]PG  很好，很强大！！


--->



## 表空间，tablespaces

这是 PG-8.0 引入的特性，将 DBs、tables、indexs 保存到指定的 tablespace 中，会在创建时指定数据的保存目录，也就是说表空间就是一个简单的目录，其主要用途分两个：

1. 单独扩展表空间用，一旦磁盘或分区被耗尽，可以创建一个表空间到其他磁盘或分区上面。

2. 区分不同对象的存储位置，比如可以将冷热数据进行分别存放。

与 Oracle 中的表空间被独占不同，PG 的表空间是可以被共享的，当创建了一个表空间后，这个表空间可以被多个数据库、表、索引等数据库对象使用。

{% highlight text %}
CREATE TABLESPACE tablespacename [ OWNER username ] LOCATION 'directory';
{% endhighlight %}

创建时必须要满足如下的条件：1) superuser；2) OS 必须支持符号连接，也就是说 Windows 不支持；3) 目录必须已经存在且为空。

创建时，PG 会在后台执行一系列操作，基本流程为：1) 将目录权限修改为 700；2) 创建一个 `PG_VERSION` 文件；3) 在 `pg_tablespace` 添加一行，并新建一个 OID(object-id)；4) 在 `$PGDATA/pg_tblspc` 目录下创建一个 OID 指向的符号连接。

当在 tablespace 中创建对象时，例如 database，会先创建一个目录，然后才会在该目录下创建对象，之所以这样，是为了避免 OID 冲突。

{% highlight text %}
----- 查看表空间
postgres=# SELECT spcname, pg_tablespace_location(oid) FROM pg_tablespace;
postgres=# \db+

----- 创建表空间，目录需要先创建
postgres=# CREATE TABLESPACE ts_foobar LOCATION '/tmp/foobar';
CREATE TABLESPACE

postgres=# SELECT oid, * FROM pg_tablespace WHERE spcname = 'ts_foobar';
  oid  |  spcname  | spcowner | spcacl | spcoptions
-------+-----------+----------+--------+------------
 16435 | ts_foobar |       10 |        |
(1 row)
postgres=# \db+
                                    List of tablespaces
    Name    |  Owner   |  Location   | Access privileges | Options |  Size   | Description
------------+----------+-------------+-------------------+---------+---------+-------------
 pg_default | postgres |             |                   |         | 35 MB   |
 pg_global  | postgres |             |                   |         | 464 kB  |
 ts_foobar  | postgres | /tmp/foobar |                   |         | 0 bytes |
(3 rows)

postgres=# CREATE DATABASE mydb OWNER postgres TABLESPACE ts_foobar;
CREATE DATABASE
postgres=# SELECT oid, datname FROM pg_database;
  oid  |  datname
-------+-----------
     1 | template1
 12918 | template0
 12923 | postgres
 16439 | mydb
(4 rows)
postgres=# \connect mydb;
You are now connected to database "mydb" as user "postgres".

----- 在上述创建的表空间中添加表
mydb=# CREATE TABLE foobar(x int, y varchar(30), z date) TABLESPACE ts_foobar;
CREATE TABLE
mydb=# INSERT INTO foobar VALUES(1, 'ShangHai', now()), (2, 'NanJing', now()), (3, 'HangZhou', now());
INSERT 0 3
mydb=# CREATE INDEX idx_foobar_x on foobar(x) TABLESPACE ts_foobar;
CREATE INDEX

----- 添加表的主键约束或是唯一键约束的时候指定表空间
mydb=# ALTER TABLE foobar ADD CONSTRAINT uk_foobar_y UNIQUE(y) USING INDEX TABLESPACE ts_foobar;
ALTER TABLE

------ 查看对应的OID
mydb=# SELECT oid FROM pg_class WHERE relname = 'foobar';
  oid
-------
 16440
(1 row)
mydb=# SELECT oid FROM pg_database WHERE datname = 'mydb';
  oid
-------
 16439
(1 row)

----- 查看表所在文件，可以直接在目录下查找对应文件
mydb=# SELECT pg_relation_filepath('foobar');
             pg_relation_filepath
----------------------------------------------
 pg_tblspc/16435/PG_9.5_201510051/16439/16440
(1 row)

$ ls -l $PGDATA/pg_tblspc
total 0
lrwxrwxrwx 1 postgres postgres 11 9月  27 23:04 16435 -> /tmp/foobar
total 0
-bash-4.2$ ls /tmp/foobar/PG_9.5_201510051/ -l
total 0
drwx------ 2 postgres postgres   40 9月  27 23:13 13294
drwx------ 2 postgres postgres 5400 9月  27 23:20 16439

----- 也可以修改表空间名称
postgres=# ALTER TABLESPACE ts_foobar RENAME TO ts_foobar01;
ALTER TABLESPACE

----- 此时表T的表空间名称会相应发生变化
postgres=# \d t
              Table "public.t"
 Column |         Type          | Modifiers
--------+-----------------------+-----------
 x      | integer               |
 y      | character varying(30) |
 z      | date                  |
Indexes:
    "uk_t_y" UNIQUE CONSTRAINT, btree (y), tablespace "ts_foobar01"
    "idx_t_x" btree (x), tablespace "ts_foobar01"
Tablespace: "ts_foobar01"

----- 也可以把表、数据库在表空间上的移动
postgres=# CREATE TABLESPACE ts_foobar02 LOCATION '/tmp/foobar02';
CREATE TABLESPACE
postgres=# ALTER DATABASE mydb SET TABLESPACE ts_foobar02;
ALTER TABLE
postgres=# ALTER TABLE t SET TABLESPACE ts_foobar02;
ALTER TABLE

----- 修改表空间的默认用户
postgres=# ALTER TABLESPACE ts_foobar OWNER TO 'foobar';
postgres=# ALTER USER foobar SET default_tablespace = 'ts_foobar';
{% endhighlight %}

当创建 cluster 时，默认会创建 `pg_global`、`pg_default` 两个 tablespace，其中前者保存了 cluster-wide 相关的数据，如 `pg_database`、`pg_group` 等，当然你不能在该 tablespace 下创建对象。

后者，则保存在 `$PGDATA/base` 目录下，是系统默认表空间，可通过 `set default tablespace=ts-name` 指定为其他表空间。

要注意的是，如果创建对象时没有指定 tablespace，它会按照上一级的对象所在 tablespace 创建相应的对象。如，创建 index 时默认与 table 相同；创建 table 时默认与 schema 相同，以此类推。

### 删除表空间

如果删除时表空间中仍然有数据库，那么会报错，此时可以通过如下方式查看。

{% highlight text %}
postgres=# DROP TABLESPACE ts_foobar;
ERROR:  tablespace "ts_foobar" is not empty

----- 查看那些DBs保存在了该表空间中
SELECT d.datname, t.spcname FROM pg_database d JOIN pg_tablespace t ON d.dattablespace = t.oid
    WHERE t.spcname = 'ts_foobar';

----- 据说这个有效的，不过DBs的信息没有保存在pg_class中
SELECT c.relname, t.spcname FROM pg_class c JOIN pg_tablespace t ON c.reltablespace = t.oid
    WHERE t.spcname = 'ts_foobar';
{% endhighlight %}

<!--
If you are creating a database and you don’t specify a tablespace, the database is created in the tablespace of the template database (typically, template1). So, an index inherits its tablespace from the parent table, a table inherits its tablespace from the parent schema, a schema inherits its tablespace from the parent database, and a database inherits its database from the template database.
-->

### tablesapce 与 schema 的区别

tablespace 实际只影响到 cluster 中数据的存储位置；而 schema 则会影响到数据在 database 中的逻辑组织方式，也就是会影响到一个对象名的解析，而 tablespace 则不会。

也就是说，当创建了 tablespace 之后，实际就不需要再关心了，而 schema 则不同。


## 权限管理

PG 中所有的权限都和用户 (或者角色) 挂钩，超级用户是有允许任意操作对象的，而普通用户只能操作自己创建的对象。另外，public 是一个特殊角色，代表所有人，有些对象是给 public 角色赋值默认权限的，也就是默认所有人都有权限的。

除了 cluster 是通过 `pg_hba.conf` 进行权限控制外，其它对象，如 database、schema、tablespace、table 等，都是通过 grant 和 revoke 赋权或者回收的。


### pg_hba.conf (PG Host-Based Authentication)

通过该配置文件确定那些用户可以登陆到那些数据库，并包括了其认证方式。该文件在 `$PGDATA` 目录下，也可以登陆 PG 之后通过 `SHOW hba_file` 查看。

{% highlight text %}
# TYPE   DATABASE  USER    CIDR-ADDRESS      METHOD

TYPE         : 指定连接PG的方式
    local        本地套接字
    host         使用TCP/IP连接，包括SSL以及非SSL
    hostssl      只能只用SSL
    hostnossl    不能使用SSL
DATABASE     : 指定的数据库名，可以指定多个数据库，通过','分割，其中all的优先级最低
USER         : 用于指定那个用户，同样可以使用','分割
CIDR-ADDRESS : local方式可以直接省略，其它可以使用主机或者某个网段
METHOD       : 认证方式
    peer         只用于本地，直接使用当前用户名作为数据库的登陆用户
    ident        本地用户登陆认证方式，只要系统中有该用户即可，通过pg_ident.conf映射
    md5          密码以md5方式传递给数据库
    password     密码以明文方式传递给数据库
    trust        不需要用户名既可以登陆
    reject       拒绝认证
{% endhighlight %}

接下来看看 PG 中的对象权限管理。

### 对象权限管理简介

在 PG 中，对于不同的对象，可以进行配置的权限是不同的，详细内容查看 [PostgreSQL Documentation - GRANT](https://www.postgresql.org/docs/9.5/static/sql-grant.html) 的定义。例如，数据库有 `CREATE`、`CONNECT` 等权限，而表有 `SELECT`、`INSERT`、`UPDATE`、`DELETE`、`TRUNCATE` 等权限。

另外，`WITH ADMIN OPTION` 表示被赋予权限的用户，还可以将对应的权限赋予给其他人。

#### 删除角色

如果要删除的用户还拥有数据库对象，或者这个用户在某些数据库对象上还拥有权限时，不能删除。

{% highlight text %}
----- 重新将某个用户的权限赋值给另外的用户，如果不知道可以赋给postgres
REASSIGN OWNED BY 'olduser' TO 'newuser';

----- 删除赋给该用户的所有权限
DROP OWNED BY 'olduser';
{% endhighlight %}

#### 角色与用户的关系

在 PostgreSQL 中，实际上这是两个完全相同的对象，唯一的区别是在通过 `CREATE USER` 命令创建时会默认添加 `LOGIN` 权限。

{% highlight text %}
----- 只创建角色，则在尝试登陆的时候会报错
postgres=# CREATE ROLE foobar1 PASSWORD 'kidding';
----- 添加登陆的权限
postgres=# ALTER ROLE foobar1 LOGIN;

----- 创建用户时，默认会添加LOGIN权限
postgres=# CREATE USER foobar2 PASSWORD 'kidding';
postgres=# CREATE ROLE foobar2 PASSWORD 'kidding' LOGIN;
{% endhighlight %}

在 PG 中，默认有一个 public 角色，代表所有人的意思。

#### 查看权限

首先介绍下对象权限的查看方式。

{% highlight text %}
postgres=# SELECT relname, relacl FROM pg_class WHERE relname = 'object-name' AND relkind='r';
postgres=# \dp+ pg_catalog.pg_statistic;
                                      Access privileges
   Schema   |     Name     | Type  |     Access privileges     | Column privileges | Policies
------------+--------------+-------+---------------------------+-------------------+----------
 pg_catalog | pg_statistic | table | postgres=arwdDxt/postgres |                   |
{% endhighlight %}

除了上述方式之外，还有一个比较复杂的 SQL 用于查看，详细可以查看脚本中指定的内容，在此解释一下 Access privileges 的含义。

{% highlight text %}
postgres=arwdDxt/postgres
    表示postgres这个角色所拥有的权限，对于public则为空；对应的权限为arwdDxt，相应含义如下；该权限是postgres所赋于的。

rolename=xxxx -- privileges granted to a role
        =xxxx -- privileges granted to PUBLIC

            r -- SELECT ("read")
            w -- UPDATE ("write")
            a -- INSERT ("append")
            d -- DELETE
            D -- TRUNCATE
            x -- REFERENCES
            t -- TRIGGER
            X -- EXECUTE
            U -- USAGE
            C -- CREATE
            c -- CONNECT
            T -- TEMPORARY
      arwdDxt -- ALL PRIVILEGES (for tables, varies for other objects)
            * -- grant option for preceding privilege

        /yyyy -- role that granted this privilege
{% endhighlight %}


### 数据库级别


数据库级别的权限，包括允许连接数据库，允许在数据库中创建schema。

* 默认创建后，允许 public 角色 (任何人) 连接；
* 默认创建后，不允许除了超级用户和 owner 之外的任何人在数据库中创建 schema；
* 默认创建后，会自动创建名为 public 的 schema，这个 schema 的 all 权限已经赋予给 public 角色，即允许任何人在里面创建对象。

这意味着，默认所有角色可以在新建的数据库中创建对象。

### schema 级别

schema 级别的权限，包括允许查看 schema 中的对象，允许在 schema 中创建对象。默认情况下新建的 schema 的权限不会赋予给 public 角色，因此除了超级用户和 owner，任何人都没有权限查看 schema 中的对象或者在 schema 中新建对象。

<!--
## 表级别
## Public

每个新建的数据库都会包含一个 public 的 schema，也就是如果不指定 schema，则默认保存在 public 中。默认来说，public 都有 USAGE 和 CREATE 权限，可以通过如下方式收回。

{% highlight text %}
postgres=# REVOKE ALL ON SCHEMA sh_foobar FROM PUBLIC;
{% endhighlight %}
-->


## schema 和 database 权限

关于 schema 使用，需要特别注意，同一个 schema 中可能会有其它用户读取，也就是说，千万不要把自己的对象创建到别人的 schema下面，那很危险。

{% highlight text %}
According to the SQL standard, the owner of a schema always owns all objects within it.
PostgreSQL allows schemas to contain objects owned by users other than the schema owner.
This can happen only if the schema owner grants the CREATE privilege on his schema to
someone else, or a superuser chooses to create objects in it.
{% endhighlight %}

如上面手册中的内容，schema 的 owner 默认是该 schema 下所有对象的 owner；同时 PG 还允许用户在别人的 schema 下创建对象，所以一个对象可能属于 "两个" owner；而且，如果 schema 的 owner 拥有 drop 权限，那么就悲剧了。

### 示例1

r1 创建了一个 schema r1，并把这个 schema 的所有权限给了 r2；此时，r2 和超级用户 postgres 分别在 r1 这个 schema 下面创建了一个表；此时，r1 可以把 r2 和 postgres 在 schema r1 下创建的表删掉。

{% highlight text %}
postgres=# CREATE ROLE r1 LOGIN;
CREATE ROLE
postgres=# CREATE ROLE r2 LOGIN;
CREATE ROLE

postgres=# GRANT ALL ON DATABASE postgres TO r1;
GRANT
postgres=# GRANT ALL ON DATABASE postgres TO r2;
GRANT

postgres=# \c postgres r1;
You are now connected to database "postgres" as user "r1".
postgres=> CREATE SCHEMA r1;
CREATE SCHEMA
postgres=> GRANT ALL ON SCHEMA r1 TO r2;
GRANT

postgres=> \c postgres r2;
You are now connected to database "postgres" as user "r2".
postgres=> CREATE TABLE r1.t(id int);
CREATE TABLE

postgres=> \c postgres postgres
postgres=# CREATE TABLE r1.t1(id int);
CREATE TABLE

postgres=# \c postgres r1
postgres=> DROP TABLE r1.t;
DROP TABLE
postgres=> DROP TABLE r1.t1;
DROP TABLE
{% endhighlight %}

r1 甚至可以直接 `DROP SCHEMA CASCADE` 来删除整个 schema。

### 示例2

对于 database 的 owner 也存在这个问题，它同样具有删除 database 中任何其他用户创建的对象的权力。

{% highlight text %}
----- 添加创建数据库的权限
postgres=# ALTER USER r1 WITH CREATEDB;

----- 普通用户r1创建的数据库
postgres=# \c postgres r1
You are now connected to database "postgres" as user "r1".
postgres=> CREATE DATABASE db1;
CREATE DATABASE
postgres=> GRANT ALL ON DATABASE db1 TO r2;
GRANT

----- 其他用户在这个数据库中创建对象
postgres=> \c db1 r2
You are now connected to database "db1" as user "r2".
db1=> CREATE SCHEMA r2;
CREATE SCHEMA
db1=> CREATE TABLE r2.t(id int);
CREATE TABLE

db1=> \c db1 postgres
You are now connected to database "db1" as user "postgres".
db1=# CREATE TABLE t(id int);
CREATE TABLE

----- 数据库的OWNER不能直接删数据库中的对象
postgres=> \c db1 r1
You are now connected to database "db1" as user "r1".
db1=> DROP TABLE r2.t ;
ERROR:  permission denied for schema r2
db1=> DROP TABLE public.t ;
ERROR:  must be owner of relation t
db1=> DROP SCHEMA r2;
ERROR:  must be owner of schema r2
db1=> DROP SCHEMA public;
ERROR:  must be owner of schema public

----- 但是可以直接删库
postgres=> DROP DATABASE db1;
DROP DATABASE
{% endhighlight %}


### 建议

介于此，建议用户使用超级用户创建 schema 和 database，然后再把 schema 和 database 的读写权限给普通用户，这样就不怕被误删了，因为超级用户本来就有所有权限。

还有一种方法是创建事件触发器，当执行 DROP 命令时，只有 OWNER 和超级用户能删对应的对象。


## 只读用户设计

在一些企业里面，通常会在数据库中创建一些只读用户，这些只读用户可以查看某些用户的对象，但是不能修改或删除这些对象的数据。

这种用户通常可以给开发人员，运营人员使用，或者数据分析师 等角色的用户使用。

因为他们可能关注的是数据本身，并且为了防止他们误操作修改或删除线上的数据，所以限制他们的用户只有只读的权限。

<!--
MySQL这块的管理应该非常方便。

其实PostgreSQL管理起来也很方便。

用户可以先参考我前面写的两篇文章

PostgreSQL 逻辑结构 和 权限体系 介绍

PostgreSQL 批量权限 管理方法

PostgreSQL schema,database owner 的高危注意事项

建议用户使用超级用户创建schema和database，然后再把schema和database的读写权限给普通用户，这样就不怕被误删了。因为超级用户本来就有所有权限。
-->

为了满足本文的需求, 创建读写用户的只读影子用户

#### 1. 使用超级用户创建读写账号，创建数据库, 创建schema

{% highlight text %}
postgres=# CREATE ROLE appuser LOGIN;
CREATE ROLE
postgres=# CREATE DATABASE appdb;
CREATE DATABASE

----- 使用超级用户创建schema
postgres=# \c appdb postgres
You are now connected to database "appdb" as user "postgres".
appdb=# CREATE SCHEMA appuser;
CREATE SCHEMA

----- 赋权
appdb=# GRANT CONNECT ON DATABASE appdb TO appuser;   -- 只赋予连接权限
GRANT
appdb=# GRANT ALL ON SCHEMA appuser TO appuser;       -- 值赋予读和写权限
GRANT
{% endhighlight %}

#### 2. 假设该读写账号已经创建了一些对象

{% highlight text %}
postgres=# \c appdb appuser
appdb=> CREATE TABLE tbl1(id INT);
CREATE TABLE
appdb=> CREATE TABLE tbl2(id INT);
CREATE TABLE
appdb=> CREATE TABLE tbl3(id INT);
CREATE TABLE
{% endhighlight %}

#### 3. 创建只读影子账号

{% highlight text %}
postgres=# CREATE ROLE readonly LOGIN;
CREATE ROLE

postgres=# \c appdb postgres
You are now connected to database "appdb" as user "postgres".
appdb=# GRANT CONNECT ON DATABASE appdb TO readonly;
GRANT
appdb=# GRANT USAGE ON SCHEMA appuser TO readonly;
GRANT
{% endhighlight %}

#### 4. 创建隐藏敏感信息的视图

假设tbl2是敏感信息表，需要加密后给只读用户看

{% highlight text %}
postgres=# \c appdb appuser
appdb=> CREATE VIEW v AS SELECT md5(id::text) FROM tbl2;
CREATE VIEW
{% endhighlight %}

#### 5. 修改已有权限

<!--
创建权限管理函数
\c appuser appuser
appuser=> create or replace function g_or_v
(
  g_or_v text,   -- 输入 grant or revoke 表示赋予或回收
  own name,      -- 指定用户 owner
  target name,   -- 赋予给哪个目标用户 grant privilege to who?
  objtyp text,   --  对象类别: 表, 物化视图, 视图 object type 'r', 'v' or 'm', means table,view,materialized view
  exp text[],    --  排除哪些对象, 用数组表示, excluded objects
  priv text      --  权限列表, privileges, ,splits, like 'select,insert,update'
) returns void as $$
declare
  nsp name;
  rel name;
  sql text;
  tmp_nsp name := '';
begin
  for nsp,rel in select t2.nspname,t1.relname from pg_class t1,pg_namespace t2 where t1.relkind=objtyp and t1.relnamespace=t2.oid and t1.relowner=(select oid from pg_roles where rolname=own)
  loop
    if (tmp_nsp = '' or tmp_nsp <> nsp) and lower(g_or_v)='grant' then
      -- auto grant schema to target user
      sql := 'GRANT usage on schema "'||nsp||'" to '||target;
      execute sql;
      raise notice '%', sql;
    end if;

    tmp_nsp := nsp;

    if (exp is not null and nsp||'.'||rel = any (exp)) then
      raise notice '% excluded % .', g_or_v, nsp||'.'||rel;
    else
      if lower(g_or_v) = 'grant' then
        sql := g_or_v||' '||priv||' on "'||nsp||'"."'||rel||'" to '||target ;
      elsif lower(g_or_v) = 'revoke' then
        sql := g_or_v||' '||priv||' on "'||nsp||'"."'||rel||'" from '||target ;
      else
        raise notice 'you must enter grant or revoke';
      end if;
      raise notice '%', sql;
      execute sql;
    end if;
  end loop;
end;
$$ language plpgsql;

appuser=> select g_or_v('grant', 'appuser', 'ro', 'r', array['public.tbl2'], 'select');
WARNING:  no privileges were granted for "public"
CONTEXT:  SQL statement "GRANT usage on schema "public" to ro"
PL/pgSQL function g_or_v(text,name,name,text,text[],text) line 13 at EXECUTE
NOTICE:  GRANT usage on schema "public" to ro
NOTICE:  grant select on "public"."tbl1" to ro
NOTICE:  grant excluded public.tbl2 .
NOTICE:  grant select on "public"."tbl3" to ro
 g_or_v
--------

(1 row)
-->

另外还提供了一种方法，但是一定要指定 schema，所以用户自己要注意，如果要对所有 schema 操作，需要把所有的 schema 都写进去。

{% highlight text %}
grant select on all tables in schema public,schema1,schema2,schema3 to ro;
{% endhighlight %}

并且这种方法还有一个弊端，如果这些schema下面有其他用户创建的对象，也会被赋予，如果赋权的账号没有权限，则会报错。


#### 6. 回收敏感表的权限

因为前面已经排除赋予了，所以不需要回收。


#### 7. 修改新建对象的默认权限

{% highlight text %}
appdb=> alter default privileges for role appuser grant select on tables to ro;
ALTER DEFAULT PRIVILEGES
appdb=> \ddp+
               Default access privileges
  Owner   | Schema | Type  |     Access privileges
----------+--------+-------+---------------------------
 appuser  |        | table | appuser=arwdDxt/appuser  +
          |        |       | ro=r/appuser
{% endhighlight %}

#### 8. 未来如果有新增的敏感表，先创建视图，同时回收表的权限

{% highlight text %}
appuser=> create table tbl4(id int);
CREATE TABLE
appuser=> create view v2 as select md5(id::text) from tbl4;
CREATE VIEW
appuser=> revoke select on tbl4 from ro;
REVOKE
{% endhighlight %}

<!--
权限检查

{% highlight text %}
appuser=> \dp+ v2
                               Access privileges
 Schema | Name | Type |    Access privileges    | Column privileges | Policies
--------+------+------+-------------------------+-------------------+----------
 public | v2   | view | appuser=arwdDxt/appuser+|                   |
        |      |      | ro=r/appuser            |                   |
(1 row)
{% endhighlight %}

希望本文对PostgreSQL用户有所帮助。


## 最佳实践

建议在线上提供如下的用户：

* admin 具有 Superuser、Create Role、Create DB、Bypass RLS 权限，没有默认的 Replication 权限。

* readonly 业务用户的只读权限。

* monitor

* appuser

{% highlight text %}
----- 查看public权限，并回收默认的权限UC
\dn+ public
REVOKE ALL ON SCHEMA public FROM PUBLIC;


----- 1. admin权限赋值
CREATE USER admin WITH SUPERUSER CREATEDB CREATEROLE LOGIN BYPASSRLS;

----- 2. monitor权限赋值
CREATE USER monitor WITH LOGIN;

----- 3. readonly权限赋值
CREATE USER readonly WITH LOGIN;



CREATE USER foobar WITH LOGIN;

#CREATE DATABASE foobardb OWNER foobar;
#REVOKE ALL ON DATABASE foobardb FROM foobar;    owner的alter、drop权限无法收回

CREATE DATABASE foobardb OWNER admin;
\c foobardb
REVOKE ALL ON SCHEMA public FROM PUBLIC;

CREATE TABLE dummy(id INT);
ALTER TABLE dummy OWNER TO foobar;   防止误删除user，DB的owner仍然可以删除，删除DB是高危操作

GRANT USAGE ON SCHEMA public TO foobar;
GRANT USAGE ON SCHEMA public TO readonly;

CREATE TABLE foo(id INT);
GRANT SELECT, UPDATE, DELETE, INSERT ON TABLE foo TO foobar;
GRANT SELECT ON TABLE foo TO readonly;


grant select on all table in schema fffff to xxxx;





----- 查看所有角色的权限
\dg+

----- 1. admin权限赋值
CREATE USER admin WITH SUPERUSER CREATEDB CREATEROLE LOGIN BYPASSRLS;


----- 查看某个用户下的表
SELECT s.usename, c.relname, c.reltype, c.reltablespace FROM
    pg_class c JOIN pg_shadow s ON c.relowner = s.usesysid
    WHERE s.usename = 'r1';


查看默认权限
\ddp
查看角色
\dg+
\du+
查看table、view、seq的权限
\dp
\z
查看DB的角色设置
\drds

----- 查看各个DB的权限
\l+


----- 修改表、SEQ、FUN等的默认权限
ALTER DEFAULT PRIVILEGES
{% endhighlight %}




# 常见操作

{% highlight text %}
----- 查看所有用户
\du+
select * from pg_roles;

----- 查看schema的权限
\dn+

----- 用户相关
CREATE USER foobar;
ALTER USER foobar WITH PASSWORD 'test';
ALTER USER foobar WITH CONNECTION LIMIT 10;

select * from information_schema.usage_privileges where grantee='foobar';
routine
table
{% endhighlight %}

# RLS (Row Level Security)

查看表空间及其大小
select spcname from pg_tablespace;
select pg_size_pretty(pg_tablespace_size('pg_default');
-->



<!--
PostgreSQL · 特性分析 · 逻辑结构和权限体系
http://mysql.taobao.org/monthly/2016/05/03/
--->

{% highlight text %}
{% endhighlight %}
