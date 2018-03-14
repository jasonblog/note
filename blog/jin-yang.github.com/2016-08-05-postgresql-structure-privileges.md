---
title: PostgreSQL 結構及權限
layout: post
comments: true
language: chinese
category: [database,postgresql,linux]
keywords: postgresql,權限管理,表空間
description: 在 PostgreSQL 中，有各種各樣的概念，例如，一個實例中允許創建多個數據庫，每個數據庫中可以創建多個 schema，每個 schema 下面可以創建多個對象，包括了表、物化視圖、操作符、索引、視圖、序列、函數 ... ... 等等。另外，PG 中還有表空間的概念，這就導致上面的概念越來越混亂。本文主要是理解 PostgreSQL 的邏輯結構和權限體系，快速理解和管理數據庫的權限。
---

在 PostgreSQL 中，有各種各樣的概念，例如，一個實例中允許創建多個數據庫，每個數據庫中可以創建多個 schema，每個 schema 下面可以創建多個對象，包括了表、物化視圖、操作符、索引、視圖、序列、函數 ... ... 等等。

另外，PG 中還有表空間的概念，這就導致上面的概念越來越混亂。

本文主要是理解 PostgreSQL 的邏輯結構和權限體系，快速理解和管理數據庫的權限。

<!-- more -->

## PG 結構簡介

首先，介紹下 PG 中常見的幾個概念，包括了 Cluster、Database、Schemas 。

* Cluster：抽象概念，一系列 DBs 的集合，但是不像 DBs 可以直接通過 SQL 在內部表中查看。一個集群在一個目錄樹下 (一般通過 `$PGDATA` 指定目錄根)，通過一個 postmaster 進程提供服務。
* Databases：一系列 Schemas 的集合，在一個 Cluster 中名字必須唯一。
* Schemas：一系列 tables、functions、indexs、data types、operators 的集合，名稱需要在一個 DB 中唯一。主要是提供了一個邏輯的命名空間，在一個 DB 中，可以通過 prefix 引用不同的 schema 。

另外，需要注意的是，在一個 cluster 中，有四個系統表是在各個 DBs 中共享的：

* pg_group 顯示用戶組列表，只是用於兼容 8.1 之前的版本。
* pg_database 該 cluster 中的數據庫列表，默認會有 postgres、template0、template1 三個數據庫。
* pg_shadow 列出有效的用戶，兼容 8.1 之前的表，是 pg_authid 中 rolcanlogin 為 true 的值；另外，pg_user 是 pg_shadow 的視圖，只是將口令設置成了空白。
* pg_tablespace 關於 tablespaces 的列表。

這也就意味著一個 Cluster 中有多個 DB，用戶和用戶組跨 DB 在整個 Cluster 中共享，但是數據不能共享 DB 。每次用戶連接時，只能連接到一個 cluster 的某個 database，可以直接通過 ps 命令查看進程。

### Schema

Schema 是 PG-7.3 引入的特性，在 PG 中是一個邏輯概念，一個用戶可以訪問他所連接數據庫中的任意 schema 中的對象，只要他有權限即可。之所以又添加了 schema，主要有如下原因：

1. 允許多個用戶互不干擾地使用同一個數據庫，從邏輯上將 DB 分組以方便管理。
2. 提供 database 下的命名空間隔離，這樣的好處是，靈活。例如，可以創建月份的 schema，每個裡面的表結構都相同。
3. PG 的限制是一個連接只能連接到一個數據庫，這樣一個連接就可以操作不同 schema 下的對象。

每個客戶端的連接都會存在一個搜索路徑 (search path)，如果沒有指定 schema，那麼 PG 就會按照該路徑進行搜索。

{% highlight text %}
----- 查看搜索路徑，如果$user的schema不存在，則會直接忽略
postgres=# SHOW SEARCH_PATH;
   search_path
-----------------
 "$user", public
(1 row)

----- 可以修改搜索路徑
postgres=# SET SEARCH_PATH TO 'foo','bar','public';
SET
{% endhighlight %}

另外的一些常見操作可以參考如下：

{% highlight text %}
----- 查詢schema
postgres=# \dn+

----- 創建schema以及其中的表
postgres=# CREATE SCHEMA foo;
CREATE SCHEMA
postgres=# CREATE TABLE foo.foobar(id INTEGER);
CREATE TABLE

----- 刪除schema，如果非空，則通過cascade強制刪除
postgres=# DROP SCHEMA foo CASCADE;
{% endhighlight %}

除了 public 和用戶創建的 schema 之外，還存在一個 `pg_catalog`，它包含了系統表所有的內建數據類型、函數、操作符，它總是搜索路徑的一部分，只是沒有在搜索路徑中顯示。

### Databases

DB 的創建語句如下，可以指定 owner 以及 tablespace 。

{% highlight text %}
CREATE DATABASE db_name
[
    [ WITH]
    [ OWNER [ = ] user_name ]                     # 所屬用戶，默認屬於創建的用戶
    [ TEMPLDATE [ =] templdate ]                  # 創建數據庫需要的模板，默認template1
    [ ENCODING [ = ] encoding ]                   # 數據庫使用的編碼，如utf8
    [ LC_COLLATE [ = ] lc_collate ]               # 排序類型
    [ LC_CTYPE [ = ] lc_ctype ]                   # 同上
    [ TABLESPACE [ = ] tablespace ]               # 數據庫的默認表空間
    [ CONNECTION LIMIT [ = ] connection limit ]   # 數據庫的最大連接數，默認為-1，禁用連接
]

----- 示例
POSTGRES=# CREATE DATABASE demodb TEMPLATE template0 ENCODING 'UTF8' TABLESPACE ts_demo01 CONNECTION LIMIT 200;
CREATE DATABASE
{% endhighlight %}

如果是 superuser 可以指定不同的 OWNER ，否則使用當前的用戶。TEMPLATE 則指定了複製的模版，會將該模版中定義的所有 tables、views、data types、functions、operators 複製到新的數據庫目錄下，默認採用 template1 。

{% highlight text %}
----- 查看詳細信息
postgres=# \l+
{% endhighlight %}

在 PG 中，數據庫、表、索引的關係如下：

* 數據庫：一個 PG 數據庫下可以管理多個數據庫，當應用連接的時候，只能連接到其中的一個數據庫，而一個數據庫只能屬於一個實例。

* 表、索引：一個數據庫裡可以有多個表與索引，PG 稱為 "Relation"。

* 數據行：在每張表中可以有很多數據行，PG 稱為 "Tuple"。


<!--
## Table

{% highlight text %}
\d+
\dS+
{% endhighlight %}

然後，數據庫與模式的關係

看文檔瞭解到：模式（schema）是對數據庫（database）邏輯分割，相當於名稱空間。
在數據庫創建的同時，就已經默認為數據庫創建了一個模式--public，這也是該數據庫的默認模式。所有為此數據庫創建的對象（表、函數、試圖、索引、序列等）都是常見在這個模式中的。
 實驗如下：
 1.創建一個數據庫dbtt----CREATE DATABASE dbtt;
 2.用kanon角色登錄到dbtt數據庫,查看dbtt數據庫中的所有模式：\dn; 顯示結果是隻有public一個模式。
 3.創建一張測試表----CREATE TABLE test(id integer not null);
 4.查看當前數據庫的列表： \d; 顯示結果是表test屬於模式public.也就是test表被默認創建在了public模式中。
 5.創建一個新模式kanon，對應於登錄用戶kanon：CREATE SCHEMA kanon OWNER kanon；
 6.再次創建一張test表，這次這張表要指明模式----CREATE TABLE kanon.test (id integer not null);
 7.查看當前數據庫的列表： \d; 顯示結果是表test屬於模式kanon.也就是這個test表被創建在了kanon模式中。
   得出結論是：數據庫是被模式(schema)來切分的，一個數據庫至少有一個模式，所有數據庫內部的對象(object)是被創建於模式的。用戶登錄到系 統，連接到一個數據庫後，是通過該數據庫的search_path來尋找schema的搜索順序,可以通過命令SHOW search_path；具體的順序，也可以通過SET search_path TO 'schema_name'來修改順序。
   官方建議是這樣的：在管理員創建一個具體數據庫後，應該為所有可以連接到該數據庫的用戶分別創建一個與用戶名相同的模式，然後，將search_path設置為"$user"，
   這樣，任何當某個用戶連接上來後，會默認將查找或者定義的對象都定位到與之同名的模式中。這是一個好的設計架構。


接下來，再來研究下表空間與數據庫的關係
    數據庫創建語句CREATE DATABASE dbname 默認的數據庫所有者是當前創建數據庫的角色，默認的表空間是系統的默認表空間--pg_default。
    為什麼是這樣的呢？因為在PostgreSQL中，數據的創建是通過克隆數據庫模板來實現的，這與SQL SERVER是同樣的機制。
    由於CREATE DATABASE dbname並沒有指明數據庫模板，所以系統將默認克隆template1數據庫，得到新的數據庫dbname。（By default, the new database will be created by cloning the standard system database template1）.

    而template1數據庫的默認表空間是pg_default，這個表空間是在數據庫初始化時創建的，所以所有template1中的對象將被同步克隆到新的數據庫中。
    相對完整的語法應該是這樣的：CREATE DATABASE dbname OWNER kanon TEMPLATE template1 TABLESPACE tablespacename;
    下面我們來做個實驗驗證一下：
 1.連接到template1數據庫，創建一個表作為標記：CREATE TABLE tbl_flag(id integer not null);向表中插入數據INSERT INTO tbl_flag VALUES (1);
 2.創建一個表空間:CREATE TABLESPACE tskanon OWNER kanon LOCATION '/tmp/data/tskanon';在此之前應該確保目錄/tmp/data/tskanon存在，並且目錄為空。
 3.創建一個數據庫，指明該數據庫的表空間是剛剛創建的tskanon：CREATE DATABASE dbkanon TEMPLATE template1 OWNERE kanon TABLESPACE tskanon;
 4.查看系統中所有數據庫的信息：\l；可以發現，dbkanon數據庫的表空間是tskanon,擁有者是kanon;
 5.連接到dbkanon數據庫，查看所有表結構:\d;可以發現，在剛創建的數據庫中居然有了一個表tbl_flag,查看該表數據，輸出結果一行一列，其值為1，說明，該數據庫的確是從template1克隆而來。

 仔細分析後，不難得出結論：在PostgreSQL中，表空間是一個目錄，裡面存儲的是它所包含的數據庫的各種物理文件。

最後，我們回頭來總結一下這張關係網
    表空間是一個存儲區域，在一個表空間中可以存儲多個數據庫，儘管PostgreSQL不建議這麼做，但我們這麼做完全可行。
    一個數據庫並不知直接存儲表結構等對象的，而是在數據庫中邏輯創建了至少一個模式，在模式中創建了表等對象，將不同的模式指派該不同的角色，可以實現權限 分離，又可以通過授權，實現模式間對象的共享，並且，還有一個特點就是：public模式可以存儲大家都需要訪問的對象。
    這樣，我們的網就形成了。可是，既然一個表在創建的時候可以指定表空間，那麼，是否可以給一個表指定它所在的數據庫表空間之外的表空間呢？
    答案是肯定的!這麼做完全可以：那這不是違背了表屬於模式，而模式屬於數據庫，數據庫最終存在於指定表空間這個網的模型了嗎？！
    是的，看上去這確實是不合常理的，但這麼做又是有它的道理的，而且現實中，我們往往需要這麼做：將表的數據存在一個較慢的磁盤上的表空間，而將表的索引存在於一個快速的磁盤上的表空間。
    但我們再查看錶所屬的模式還是沒變的，它依然屬於指定的模式。所以這並不違反常理。實際上，PostgreSQL並沒有限制一張表必須屬於某個特定的表空間，我們之所以會這麼認為，是因為在關係遞進時，偷換了一個概念：模式是邏輯存在的，它不受表空間的限制。




https://www.postgresql.org/docs/9.5/static/sql-grant.html

如何在PostgreSQL中建只讀賬號

在PostgreSQL中並沒有CREATE TABLE權限名稱，這是與其它數據庫不同的一個地方，PostgreSQL是通過控制是否在模式schema中上有CREATE控制用戶的能否創建表的權限的，默認安裝下，任何用戶都有在模式public中CREATE的權限，所以要創建只讀賬號的第一步，我們要先去除在模式public中的CREATE權限：

REVOKE  CREATE  ON SCHEMA public from public;

下面的SQL創建了一個名為“readonly”的用戶：

CREATE USER readonly with password 'query';

然後把現有的所有在public這個schema下的表的SELECT權限賦給用戶readonly，執行下面的SQL命令：

GRANT SELECT ON  ALL TABLES IN SCHEMA public TO readonly;

上面的SQL命令只把現有的表的權限給了用戶readonly，但如果這之後創建的表，readonly用戶還是不能讀，需要使用下面的SQL把以後創建的表的SELECT權限也給用戶readonly：

ALTER DEFAULT PRIVILEGES IN SCHEMA public grant select on tables to readonly;

注意：上面的過程只是把名稱為public的schema下的表賦了只讀權限，如果想讓這個用戶能訪問其它schema下的表，需要重複執行：

GRANT SELECT ON  ALL TABLES IN SCHEMA other_schema TO readonly;
ALTER DEFAULT PRIVILEGES IN SCHEMA other_schema grant select on tables to readonly;




pg 裡面如何實現一個用戶的只讀訪問呢？

oracle 裡 我們通過對每個對象的單獨的授予 select 權限，解決。

整個數據庫級別的只讀怎麼做？ oracle 裡應該是對應到schema 級別的，設置表空間只讀。

pg 下面怎麼做呢？

PG 裡有個參數  default_transaction_read_only[code]


postgres=# create user lsl password 'lsl' ;
CREATE ROLE
postgres=# alter user lsl set default_transaction_read_only=on;
ALTER ROLE
postgres=# grant all on database cms_pcbaby_app to lsl;
GRANT
cms_pcbaby_app=> create table t(id int) ;
ERROR:  cannot execute CREATE TABLE in a read-only transaction
cms_pcbaby_app=>
[/code]數據庫級別也可以設置這個參數的。[code]
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
[/code]PG  很好，很強大！！


--->



## 表空間，tablespaces

這是 PG-8.0 引入的特性，將 DBs、tables、indexs 保存到指定的 tablespace 中，會在創建時指定數據的保存目錄，也就是說表空間就是一個簡單的目錄，其主要用途分兩個：

1. 單獨擴展表空間用，一旦磁盤或分區被耗盡，可以創建一個表空間到其他磁盤或分區上面。

2. 區分不同對象的存儲位置，比如可以將冷熱數據進行分別存放。

與 Oracle 中的表空間被獨佔不同，PG 的表空間是可以被共享的，當創建了一個表空間後，這個表空間可以被多個數據庫、表、索引等數據庫對象使用。

{% highlight text %}
CREATE TABLESPACE tablespacename [ OWNER username ] LOCATION 'directory';
{% endhighlight %}

創建時必須要滿足如下的條件：1) superuser；2) OS 必須支持符號連接，也就是說 Windows 不支持；3) 目錄必須已經存在且為空。

創建時，PG 會在後臺執行一系列操作，基本流程為：1) 將目錄權限修改為 700；2) 創建一個 `PG_VERSION` 文件；3) 在 `pg_tablespace` 添加一行，並新建一個 OID(object-id)；4) 在 `$PGDATA/pg_tblspc` 目錄下創建一個 OID 指向的符號連接。

當在 tablespace 中創建對象時，例如 database，會先創建一個目錄，然後才會在該目錄下創建對象，之所以這樣，是為了避免 OID 衝突。

{% highlight text %}
----- 查看錶空間
postgres=# SELECT spcname, pg_tablespace_location(oid) FROM pg_tablespace;
postgres=# \db+

----- 創建表空間，目錄需要先創建
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

----- 在上述創建的表空間中添加表
mydb=# CREATE TABLE foobar(x int, y varchar(30), z date) TABLESPACE ts_foobar;
CREATE TABLE
mydb=# INSERT INTO foobar VALUES(1, 'ShangHai', now()), (2, 'NanJing', now()), (3, 'HangZhou', now());
INSERT 0 3
mydb=# CREATE INDEX idx_foobar_x on foobar(x) TABLESPACE ts_foobar;
CREATE INDEX

----- 添加表的主鍵約束或是唯一鍵約束的時候指定表空間
mydb=# ALTER TABLE foobar ADD CONSTRAINT uk_foobar_y UNIQUE(y) USING INDEX TABLESPACE ts_foobar;
ALTER TABLE

------ 查看對應的OID
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

----- 查看錶所在文件，可以直接在目錄下查找對應文件
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

----- 也可以修改表空間名稱
postgres=# ALTER TABLESPACE ts_foobar RENAME TO ts_foobar01;
ALTER TABLESPACE

----- 此時表T的表空間名稱會相應發生變化
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

----- 也可以把表、數據庫在表空間上的移動
postgres=# CREATE TABLESPACE ts_foobar02 LOCATION '/tmp/foobar02';
CREATE TABLESPACE
postgres=# ALTER DATABASE mydb SET TABLESPACE ts_foobar02;
ALTER TABLE
postgres=# ALTER TABLE t SET TABLESPACE ts_foobar02;
ALTER TABLE

----- 修改表空間的默認用戶
postgres=# ALTER TABLESPACE ts_foobar OWNER TO 'foobar';
postgres=# ALTER USER foobar SET default_tablespace = 'ts_foobar';
{% endhighlight %}

當創建 cluster 時，默認會創建 `pg_global`、`pg_default` 兩個 tablespace，其中前者保存了 cluster-wide 相關的數據，如 `pg_database`、`pg_group` 等，當然你不能在該 tablespace 下創建對象。

後者，則保存在 `$PGDATA/base` 目錄下，是系統默認表空間，可通過 `set default tablespace=ts-name` 指定為其他表空間。

要注意的是，如果創建對象時沒有指定 tablespace，它會按照上一級的對象所在 tablespace 創建相應的對象。如，創建 index 時默認與 table 相同；創建 table 時默認與 schema 相同，以此類推。

### 刪除表空間

如果刪除時表空間中仍然有數據庫，那麼會報錯，此時可以通過如下方式查看。

{% highlight text %}
postgres=# DROP TABLESPACE ts_foobar;
ERROR:  tablespace "ts_foobar" is not empty

----- 查看那些DBs保存在了該表空間中
SELECT d.datname, t.spcname FROM pg_database d JOIN pg_tablespace t ON d.dattablespace = t.oid
    WHERE t.spcname = 'ts_foobar';

----- 據說這個有效的，不過DBs的信息沒有保存在pg_class中
SELECT c.relname, t.spcname FROM pg_class c JOIN pg_tablespace t ON c.reltablespace = t.oid
    WHERE t.spcname = 'ts_foobar';
{% endhighlight %}

<!--
If you are creating a database and you don’t specify a tablespace, the database is created in the tablespace of the template database (typically, template1). So, an index inherits its tablespace from the parent table, a table inherits its tablespace from the parent schema, a schema inherits its tablespace from the parent database, and a database inherits its database from the template database.
-->

### tablesapce 與 schema 的區別

tablespace 實際隻影響到 cluster 中數據的存儲位置；而 schema 則會影響到數據在 database 中的邏輯組織方式，也就是會影響到一個對象名的解析，而 tablespace 則不會。

也就是說，當創建了 tablespace 之後，實際就不需要再關心了，而 schema 則不同。


## 權限管理

PG 中所有的權限都和用戶 (或者角色) 掛鉤，超級用戶是有允許任意操作對象的，而普通用戶只能操作自己創建的對象。另外，public 是一個特殊角色，代表所有人，有些對象是給 public 角色賦值默認權限的，也就是默認所有人都有權限的。

除了 cluster 是通過 `pg_hba.conf` 進行權限控制外，其它對象，如 database、schema、tablespace、table 等，都是通過 grant 和 revoke 賦權或者回收的。


### pg_hba.conf (PG Host-Based Authentication)

通過該配置文件確定那些用戶可以登陸到那些數據庫，幷包括了其認證方式。該文件在 `$PGDATA` 目錄下，也可以登陸 PG 之後通過 `SHOW hba_file` 查看。

{% highlight text %}
# TYPE   DATABASE  USER    CIDR-ADDRESS      METHOD

TYPE         : 指定連接PG的方式
    local        本地套接字
    host         使用TCP/IP連接，包括SSL以及非SSL
    hostssl      只能只用SSL
    hostnossl    不能使用SSL
DATABASE     : 指定的數據庫名，可以指定多個數據庫，通過','分割，其中all的優先級最低
USER         : 用於指定那個用戶，同樣可以使用','分割
CIDR-ADDRESS : local方式可以直接省略，其它可以使用主機或者某個網段
METHOD       : 認證方式
    peer         只用於本地，直接使用當前用戶名作為數據庫的登陸用戶
    ident        本地用戶登陸認證方式，只要系統中有該用戶即可，通過pg_ident.conf映射
    md5          密碼以md5方式傳遞給數據庫
    password     密碼以明文方式傳遞給數據庫
    trust        不需要用戶名既可以登陸
    reject       拒絕認證
{% endhighlight %}

接下來看看 PG 中的對象權限管理。

### 對象權限管理簡介

在 PG 中，對於不同的對象，可以進行配置的權限是不同的，詳細內容查看 [PostgreSQL Documentation - GRANT](https://www.postgresql.org/docs/9.5/static/sql-grant.html) 的定義。例如，數據庫有 `CREATE`、`CONNECT` 等權限，而表有 `SELECT`、`INSERT`、`UPDATE`、`DELETE`、`TRUNCATE` 等權限。

另外，`WITH ADMIN OPTION` 表示被賦予權限的用戶，還可以將對應的權限賦予給其他人。

#### 刪除角色

如果要刪除的用戶還擁有數據庫對象，或者這個用戶在某些數據庫對象上還擁有權限時，不能刪除。

{% highlight text %}
----- 重新將某個用戶的權限賦值給另外的用戶，如果不知道可以賦給postgres
REASSIGN OWNED BY 'olduser' TO 'newuser';

----- 刪除賦給該用戶的所有權限
DROP OWNED BY 'olduser';
{% endhighlight %}

#### 角色與用戶的關係

在 PostgreSQL 中，實際上這是兩個完全相同的對象，唯一的區別是在通過 `CREATE USER` 命令創建時會默認添加 `LOGIN` 權限。

{% highlight text %}
----- 只創建角色，則在嘗試登陸的時候會報錯
postgres=# CREATE ROLE foobar1 PASSWORD 'kidding';
----- 添加登陸的權限
postgres=# ALTER ROLE foobar1 LOGIN;

----- 創建用戶時，默認會添加LOGIN權限
postgres=# CREATE USER foobar2 PASSWORD 'kidding';
postgres=# CREATE ROLE foobar2 PASSWORD 'kidding' LOGIN;
{% endhighlight %}

在 PG 中，默認有一個 public 角色，代表所有人的意思。

#### 查看權限

首先介紹下對象權限的查看方式。

{% highlight text %}
postgres=# SELECT relname, relacl FROM pg_class WHERE relname = 'object-name' AND relkind='r';
postgres=# \dp+ pg_catalog.pg_statistic;
                                      Access privileges
   Schema   |     Name     | Type  |     Access privileges     | Column privileges | Policies
------------+--------------+-------+---------------------------+-------------------+----------
 pg_catalog | pg_statistic | table | postgres=arwdDxt/postgres |                   |
{% endhighlight %}

除了上述方式之外，還有一個比較複雜的 SQL 用於查看，詳細可以查看腳本中指定的內容，在此解釋一下 Access privileges 的含義。

{% highlight text %}
postgres=arwdDxt/postgres
    表示postgres這個角色所擁有的權限，對於public則為空；對應的權限為arwdDxt，相應含義如下；該權限是postgres所賦於的。

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


### 數據庫級別


數據庫級別的權限，包括允許連接數據庫，允許在數據庫中創建schema。

* 默認創建後，允許 public 角色 (任何人) 連接；
* 默認創建後，不允許除了超級用戶和 owner 之外的任何人在數據庫中創建 schema；
* 默認創建後，會自動創建名為 public 的 schema，這個 schema 的 all 權限已經賦予給 public 角色，即允許任何人在裡面創建對象。

這意味著，默認所有角色可以在新建的數據庫中創建對象。

### schema 級別

schema 級別的權限，包括允許查看 schema 中的對象，允許在 schema 中創建對象。默認情況下新建的 schema 的權限不會賦予給 public 角色，因此除了超級用戶和 owner，任何人都沒有權限查看 schema 中的對象或者在 schema 中新建對象。

<!--
## 表級別
## Public

每個新建的數據庫都會包含一個 public 的 schema，也就是如果不指定 schema，則默認保存在 public 中。默認來說，public 都有 USAGE 和 CREATE 權限，可以通過如下方式收回。

{% highlight text %}
postgres=# REVOKE ALL ON SCHEMA sh_foobar FROM PUBLIC;
{% endhighlight %}
-->


## schema 和 database 權限

關於 schema 使用，需要特別注意，同一個 schema 中可能會有其它用戶讀取，也就是說，千萬不要把自己的對象創建到別人的 schema下面，那很危險。

{% highlight text %}
According to the SQL standard, the owner of a schema always owns all objects within it.
PostgreSQL allows schemas to contain objects owned by users other than the schema owner.
This can happen only if the schema owner grants the CREATE privilege on his schema to
someone else, or a superuser chooses to create objects in it.
{% endhighlight %}

如上面手冊中的內容，schema 的 owner 默認是該 schema 下所有對象的 owner；同時 PG 還允許用戶在別人的 schema 下創建對象，所以一個對象可能屬於 "兩個" owner；而且，如果 schema 的 owner 擁有 drop 權限，那麼就悲劇了。

### 示例1

r1 創建了一個 schema r1，並把這個 schema 的所有權限給了 r2；此時，r2 和超級用戶 postgres 分別在 r1 這個 schema 下面創建了一個表；此時，r1 可以把 r2 和 postgres 在 schema r1 下創建的表刪掉。

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

r1 甚至可以直接 `DROP SCHEMA CASCADE` 來刪除整個 schema。

### 示例2

對於 database 的 owner 也存在這個問題，它同樣具有刪除 database 中任何其他用戶創建的對象的權力。

{% highlight text %}
----- 添加創建數據庫的權限
postgres=# ALTER USER r1 WITH CREATEDB;

----- 普通用戶r1創建的數據庫
postgres=# \c postgres r1
You are now connected to database "postgres" as user "r1".
postgres=> CREATE DATABASE db1;
CREATE DATABASE
postgres=> GRANT ALL ON DATABASE db1 TO r2;
GRANT

----- 其他用戶在這個數據庫中創建對象
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

----- 數據庫的OWNER不能直接刪數據庫中的對象
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

----- 但是可以直接刪庫
postgres=> DROP DATABASE db1;
DROP DATABASE
{% endhighlight %}


### 建議

介於此，建議用戶使用超級用戶創建 schema 和 database，然後再把 schema 和 database 的讀寫權限給普通用戶，這樣就不怕被誤刪了，因為超級用戶本來就有所有權限。

還有一種方法是創建事件觸發器，當執行 DROP 命令時，只有 OWNER 和超級用戶能刪對應的對象。


## 只讀用戶設計

在一些企業裡面，通常會在數據庫中創建一些只讀用戶，這些只讀用戶可以查看某些用戶的對象，但是不能修改或刪除這些對象的數據。

這種用戶通常可以給開發人員，運營人員使用，或者數據分析師 等角色的用戶使用。

因為他們可能關注的是數據本身，並且為了防止他們誤操作修改或刪除線上的數據，所以限制他們的用戶只有只讀的權限。

<!--
MySQL這塊的管理應該非常方便。

其實PostgreSQL管理起來也很方便。

用戶可以先參考我前面寫的兩篇文章

PostgreSQL 邏輯結構 和 權限體系 介紹

PostgreSQL 批量權限 管理方法

PostgreSQL schema,database owner 的高危注意事項

建議用戶使用超級用戶創建schema和database，然後再把schema和database的讀寫權限給普通用戶，這樣就不怕被誤刪了。因為超級用戶本來就有所有權限。
-->

為了滿足本文的需求, 創建讀寫用戶的只讀影子用戶

#### 1. 使用超級用戶創建讀寫賬號，創建數據庫, 創建schema

{% highlight text %}
postgres=# CREATE ROLE appuser LOGIN;
CREATE ROLE
postgres=# CREATE DATABASE appdb;
CREATE DATABASE

----- 使用超級用戶創建schema
postgres=# \c appdb postgres
You are now connected to database "appdb" as user "postgres".
appdb=# CREATE SCHEMA appuser;
CREATE SCHEMA

----- 賦權
appdb=# GRANT CONNECT ON DATABASE appdb TO appuser;   -- 只賦予連接權限
GRANT
appdb=# GRANT ALL ON SCHEMA appuser TO appuser;       -- 值賦予讀和寫權限
GRANT
{% endhighlight %}

#### 2. 假設該讀寫賬號已經創建了一些對象

{% highlight text %}
postgres=# \c appdb appuser
appdb=> CREATE TABLE tbl1(id INT);
CREATE TABLE
appdb=> CREATE TABLE tbl2(id INT);
CREATE TABLE
appdb=> CREATE TABLE tbl3(id INT);
CREATE TABLE
{% endhighlight %}

#### 3. 創建只讀影子賬號

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

#### 4. 創建隱藏敏感信息的視圖

假設tbl2是敏感信息表，需要加密後給只讀用戶看

{% highlight text %}
postgres=# \c appdb appuser
appdb=> CREATE VIEW v AS SELECT md5(id::text) FROM tbl2;
CREATE VIEW
{% endhighlight %}

#### 5. 修改已有權限

<!--
創建權限管理函數
\c appuser appuser
appuser=> create or replace function g_or_v
(
  g_or_v text,   -- 輸入 grant or revoke 表示賦予或回收
  own name,      -- 指定用戶 owner
  target name,   -- 賦予給哪個目標用戶 grant privilege to who?
  objtyp text,   --  對象類別: 表, 物化視圖, 視圖 object type 'r', 'v' or 'm', means table,view,materialized view
  exp text[],    --  排除哪些對象, 用數組表示, excluded objects
  priv text      --  權限列表, privileges, ,splits, like 'select,insert,update'
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

另外還提供了一種方法，但是一定要指定 schema，所以用戶自己要注意，如果要對所有 schema 操作，需要把所有的 schema 都寫進去。

{% highlight text %}
grant select on all tables in schema public,schema1,schema2,schema3 to ro;
{% endhighlight %}

並且這種方法還有一個弊端，如果這些schema下面有其他用戶創建的對象，也會被賦予，如果賦權的賬號沒有權限，則會報錯。


#### 6. 回收敏感表的權限

因為前面已經排除賦予了，所以不需要回收。


#### 7. 修改新建對象的默認權限

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

#### 8. 未來如果有新增的敏感表，先創建視圖，同時回收表的權限

{% highlight text %}
appuser=> create table tbl4(id int);
CREATE TABLE
appuser=> create view v2 as select md5(id::text) from tbl4;
CREATE VIEW
appuser=> revoke select on tbl4 from ro;
REVOKE
{% endhighlight %}

<!--
權限檢查

{% highlight text %}
appuser=> \dp+ v2
                               Access privileges
 Schema | Name | Type |    Access privileges    | Column privileges | Policies
--------+------+------+-------------------------+-------------------+----------
 public | v2   | view | appuser=arwdDxt/appuser+|                   |
        |      |      | ro=r/appuser            |                   |
(1 row)
{% endhighlight %}

希望本文對PostgreSQL用戶有所幫助。


## 最佳實踐

建議在線上提供如下的用戶：

* admin 具有 Superuser、Create Role、Create DB、Bypass RLS 權限，沒有默認的 Replication 權限。

* readonly 業務用戶的只讀權限。

* monitor

* appuser

{% highlight text %}
----- 查看public權限，並回收默認的權限UC
\dn+ public
REVOKE ALL ON SCHEMA public FROM PUBLIC;


----- 1. admin權限賦值
CREATE USER admin WITH SUPERUSER CREATEDB CREATEROLE LOGIN BYPASSRLS;

----- 2. monitor權限賦值
CREATE USER monitor WITH LOGIN;

----- 3. readonly權限賦值
CREATE USER readonly WITH LOGIN;



CREATE USER foobar WITH LOGIN;

#CREATE DATABASE foobardb OWNER foobar;
#REVOKE ALL ON DATABASE foobardb FROM foobar;    owner的alter、drop權限無法收回

CREATE DATABASE foobardb OWNER admin;
\c foobardb
REVOKE ALL ON SCHEMA public FROM PUBLIC;

CREATE TABLE dummy(id INT);
ALTER TABLE dummy OWNER TO foobar;   防止誤刪除user，DB的owner仍然可以刪除，刪除DB是高危操作

GRANT USAGE ON SCHEMA public TO foobar;
GRANT USAGE ON SCHEMA public TO readonly;

CREATE TABLE foo(id INT);
GRANT SELECT, UPDATE, DELETE, INSERT ON TABLE foo TO foobar;
GRANT SELECT ON TABLE foo TO readonly;


grant select on all table in schema fffff to xxxx;





----- 查看所有角色的權限
\dg+

----- 1. admin權限賦值
CREATE USER admin WITH SUPERUSER CREATEDB CREATEROLE LOGIN BYPASSRLS;


----- 查看某個用戶下的表
SELECT s.usename, c.relname, c.reltype, c.reltablespace FROM
    pg_class c JOIN pg_shadow s ON c.relowner = s.usesysid
    WHERE s.usename = 'r1';


查看默認權限
\ddp
查看角色
\dg+
\du+
查看table、view、seq的權限
\dp
\z
查看DB的角色設置
\drds

----- 查看各個DB的權限
\l+


----- 修改表、SEQ、FUN等的默認權限
ALTER DEFAULT PRIVILEGES
{% endhighlight %}




# 常見操作

{% highlight text %}
----- 查看所有用戶
\du+
select * from pg_roles;

----- 查看schema的權限
\dn+

----- 用戶相關
CREATE USER foobar;
ALTER USER foobar WITH PASSWORD 'test';
ALTER USER foobar WITH CONNECTION LIMIT 10;

select * from information_schema.usage_privileges where grantee='foobar';
routine
table
{% endhighlight %}

# RLS (Row Level Security)

查看錶空間及其大小
select spcname from pg_tablespace;
select pg_size_pretty(pg_tablespace_size('pg_default');
-->



<!--
PostgreSQL · 特性分析 · 邏輯結構和權限體系
http://mysql.taobao.org/monthly/2016/05/03/
--->

{% highlight text %}
{% endhighlight %}
