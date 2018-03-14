---
title: PostgreSQL 簡單介紹
layout: post
comments: true
language: chinese
category: [database,postgresql,linux]
keywords: postgresql,簡介
description: PostgreSQL 可以說是目前功能最強大、特性最豐富和結構最複雜的開源數據庫管理系統，其中有些特性甚至連商業數據庫都不具備。這裡簡單介紹 PG 的常見操作。
---

PostgreSQL 可以說是目前功能最強大、特性最豐富和結構最複雜的開源數據庫管理系統，其中有些特性甚至連商業數據庫都不具備。

這裡簡單介紹 PG 的常見操作。

<!-- more -->

![PostgreSQL Logo]({{ site.url }}/images/databases/postgresql/postgresql-logo.jpg "PostgreSQL Logo"){: .pull-center width="30%" }

## 簡介

PostgreSQL 可以說是目前功能最強大、特性最豐富和結構最複雜的開源數據庫管理系統，其中有些特性甚至連商業數據庫都不具備。這個起源於加州大學伯克利分校的數據庫，現已成為一項國際開發項目，並且擁有廣泛的用戶群，尤其是在海外，目前國內使用者也越來越多。

PostgreSQL 基本上見證了數據庫理論和技術的發展歷程，由 UCB 計算機教授 Michael Stonebraker 於 1986 年創建。在此之前，Stonebraker 教授主導了關係數據庫 Ingres 研究項目，88 年，提出了 Postgres 的第一個原型設計。

MySQL 號稱是使用最廣泛的開源數據庫，而 PG 則被稱為功能最強大的開源數據庫。

### 安裝

主要有兩種方式：yum 安裝和源碼安裝，其中前者相比要簡單很多，後者通常在調試源碼時使用。

#### 通過 YUM 安裝

在 CentOS 中，可以直接通過 YUM 進行安裝，不過默認的版本可能會比較低，在 CentOS 7 上默認安裝的是 9.2，可以通過 yum info 查看當前的版本。

{% highlight text %}
# yum install postgresql postgresql-server
{% endhighlight %}

如果要安裝最新版本，可以從 [yum.postgresql.org](http://yum.postgresql.org/) 上下載 repos 安裝包，安裝數據源。這也是我們推薦的一種方式，一下中的 XXX 標記，可能會根據不同的版本有所區別，不過操作大致相同。

{% highlight text %}
----- 安裝相應PG版本的repository，並更新庫
# rpm -ivh pgdg-centos-XXX.rpm
# yum update

----- 安裝相應的版本，包括常用的pgadmin工具
# yum --enablerepo=pgdgXX install postgresqlXX-server pgadmin3_XX postgresqlXX-contrib

----- 另外，安裝完之後需要配置PATH環境變量，並使之生效
# cat /etc/profile
pathmunge /usr/pgsql-X.X/bin
# source /etc/profile
{% endhighlight %}

通過 YUM 方式安裝後，同時會創建一個 postgres 用戶，以及同名用戶組，直接切換到該用戶即可，執行相關的數據庫操作即可。

{% highlight text %}
# su - postgres

----- 初始化db，如下是安裝之後默認的數據存儲目錄
$ initdb -D /var/lib/pgsql/X.X/data
{% endhighlight %}

在 CentOS 7 中，對於會同時安裝如下的啟動文件，其它的 systemctl 操作基本就相同了。

{% highlight text %}
# ls /usr/lib/systemd/system/postgresql-X.X.service
# systemctl start postgresql-X.X
{% endhighlight %}

#### 通過源碼安裝

直接從官網 [www.postgresql.org](https://www.postgresql.org) 下載相應版本的源碼包。

{% highlight text %}
----- 可以選擇新建PG專用的用戶
# groupadd postgres
# useradd -g postgres postgres

----- 解壓、編譯
$ tar -xf postgresql-X.X.X.tar.bz2 && cd postgresql-X.X.X
$ ./configure --prefix=/opt/postgre
$ make
# make install

----- 同樣可以編譯contrib目錄下的一些工具
$ cd contrib && make
# make install

----- 綁定數據庫文件存儲目錄
$ export PGDATA=/home/andy/Workspace/databases/postgre/data
$ /opt/postgre/bin/initdb -D $PGDATA

----- 常見操作，如啟動、停止數據庫
$ /opt/postgre/bin/pg_ctl -D $PGDATA -l logfile start
$ /opt/postgre/bin/pg_ctl -D $PGDATA -l logfile stop
{% endhighlight %}

### 日誌查看

PG 的日誌分為三類，分別是 `pg_log`、`pg_xlog` 和 `pg_clog`，一般保存在 `$PGDATA` 對應的目錄下。

1. pg_log  數據庫運行日誌，默認開啟，可以通過配置 `$PGDATA/postgresql.conf` 。
2. pg_xlog WAL日誌，強制開啟。
3. pg_clog 事務提交日誌，記錄事務的元數據，強制開啟。

### 防火牆、SELinux 設置

PG 默認使用 `5432` 端口，也可以在 `postgresql.conf` 文件中設置，可以使用如下命令開啟防火牆端口。

{% highlight text %}
# firewall-cmd --add-port=5432/tcp                          # 暫時有效
# firewall-cmd --permanent --add-port=5432/tcp              # 永久生效
----- 在iptables中開啟
# iptables -A INPUT -p tcp --dport 5432 -m state --state NEW,ESTABLISHED -j ACCEPT
{% endhighlight %}

如果開啟了 SELinux 服務，可能會在使用中遇到各種各樣的權限問題。

{% highlight text %}
----- 修改數據庫的存放位置，必須添加一些新上下文來匹配新位置
# semanage fcontext -a -t postgresql_db_t "/new/location(/.*)?"
----- 默認端口不起作用，需要匹配postgre的端口類型為你想要的端口
# semanage port -a -t postgresql_port_t -p tcp 5433
----- 如果APP需要通過TCP/IP與PG交互，你需要告訴SELinux允許這個操作
# setsebool -P httpd_can_network_connect_db on
{% endhighlight %}

### 設置用戶和數據庫

創建一個用戶，併為用戶創建一個數據庫。
{% highlight text %}
$ psql -U postgres -W
psql (9.3.2)
Type "help" for help.

postgres=# \password postgres         # 設置一下密碼

----- 可以通過如下方式創建用戶、數據庫
postgres=# CREATE USER foobar WITH PASSWORD 'justkidding';
postgres=# CREATE DATABASE test OWNER foobar;

----- 也可以直接在shell中創建
$ createuser foobar
$ createdb --owner=foobar test
{% endhighlight %}

### 配置

PG 主要使用了兩個配置文件 `/var/lib/pgsql/data/{postgresql.conf, pg_hba.conf}`，其中一些配置參數可以通過命令行選項傳給守護進程，此時會覆蓋配置文件中的設置。

例如，如果想要修改服務的端口為 5433，創建一個名為 `/etc/systemd/system/postgresql.service` 的文件，包含以下內容。

{% highlight text %}
.include /lib/systemd/system/postgresql.service
[Service]
Environment=PGPORT=5433
{% endhighlight %}

當數據庫安裝後，可以通過編輯 `/var/lib/pgsql/data/pg_hba.conf` 文件來實現權限管理。

{% highlight text %}
# TYPE    DATABASE        USER            ADDRESS                 METHOD
  host    all             all             127.0.0.1/32            md5
  local   all             postgres                                peer
{% endhighlight %}






## 常見操作

接下來，我們使用默認的用戶 postgres 登陸，執行一些 CURD (Create, Update, Read, Delete) 操作。

{% highlight text %}
$ /opt/postgre/bin/createdb test             # 創建test數據庫
$ /opt/postgre/bin/psql test                 # 鏈接到test數據庫
test=# create table test1(id integer);       # 建表
test=# insert into test1 values(1);          # 插入數據
test=# select * from test1;                  # 查詢
{% endhighlight %}

也可以直接登陸創建。

{% highlight text %}
$ psql -U postgres -W                        # 直接登陸，默認無密碼
postgres=# CREATE USER foobar WITH PASSWORD 'password';          # 創建用戶
postgres=# CREATE DATABASE test OWNER foobar ENCODING 'UTF8';    # 以及數據庫

$ psql -U foobar -W test
Password for user foobar: password
test=> \q
{% endhighlight %}

可以直接修改配置文件，允許遠程訪問。

{% highlight text %}
----- 修改/var/lib/pgsql/data/postgresql.conf
listen_addresses ='*'

----- 修改/var/lib/pgsql/data/pg_hba.conf
host    all             all             0.0.0.0/0            trust
{% endhighlight %}

### 狀態查看

{% highlight text %}
----- 查看PG的當前進程
# ps auxww | grep ^postgres
{% endhighlight %}
主要包括了兩類進程，分別是啟動時的後臺進程，以及用戶連接的進程；對於前者，其中 stats collector 和 autovacuum launcher 兩個進程是可選啟動的，各個進程作用如下。
{% highlight text %}
postgres: 主進程，接收客戶端連接，創建服務進程；啟動時拉起和管理後臺進程。
achiever process: 事務日誌歸檔進程。
{% endhighlight %}

而用戶進程通常格式如下：

{% highlight text %}
postgres: user database host activity
{% endhighlight %}

其中 activity 的狀態為 idle (正在等待用戶輸入命令)、idle in transaction、waiting (等待鎖) 等。

### CTID

在 PG 中的 ctid 表示數據記錄的物理行信息，用於標示一條記錄位於那個數據塊的那個位移上面，類似於 Oracle 中的 rowid 。

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

當刪除了數據之後，原有的空間不會自動釋放，可以通過 vacuum tbl 回收。

<!-- generate_series(0, 100, 2); -->

### 索引

{% highlight text %}
----- 創建唯一約束
postgres=# ALTER TABLE tbl ADD CONSTRAINT UK_tbl_col UNIQUE(x, y);

----- 創建函數索引
postgres=# CREATE INDEX idx_tbl_col ON tbl USING btree(UPPER(x), y);

----- 創建部分索引
postgres=# CREATE INDEX idx_tbl_col ON tbl USING btree(UPPER(x), y) WHERE z IS NULL;
{% endhighlight %}

### 常用命令

pgsql 常見的命令。

{% highlight text %}
----- 查看幫助
postgre=# \h SELECT            # 查看SQL命令的幫助
postgre=# \?                   # 查看psql命令的幫助

----- 導入/導出數據
postgre=# \i file.sql          # 從某個文件導入
postgre=# COPY weather FROM 'file.txt';

----- 切換數據庫，相當於MySQL的use dbname
postgre=# \c dbname username IP PORT

----- 查看數據庫，相當於MySQL的show databases
postgre=# \l    \list

----- 查看錶，相當於MySQL的show tables
postgre=# \dt

----- 查看錶結構，相當於desc
postgre=# \d tblname           # \d+ tblname查看詳細信息

----- 查看索引
postgre=# \di

----- 查看用戶
postgre=# \du

----- 在文本編輯器中編譯，退出後執行
postgre=# \e

----- 查看當前連接的信息
postgre=# \conninfo

----- 其它雜項
postgre=# \! shell-command                   # 執行終端的命令
postgre=# \set COMP_KEYWORD_CASE upper       # 設置自動提示關鍵字大寫顯示
postgre=# \x auto                            # 如果列較多時，通過行顯示，其中\x表示直接行顯示
postgre=# \pset null ¤                       # 當值為NULL時顯示如下的字符，以區分空格
{% endhighlight %}

PG 中通過 `::` 進行類型轉換；另外，支持一些常見的字符串匹配函數，如 `ilike`、`~*` 等，詳細可以參考 [Pattern Matching](https://www.postgresql.org/docs/9.6/static/functions-matching.html) 。

## 常用概念

在 PostgreSQL 中，有各種各樣的概念，常見的有表空間、數據庫、模式、表、用戶、角色等。

### 角色 VS. 用戶

這兩個可以理解為相同，只是兩者在創建時默認行為的區別，其它基本一致。文檔中，對兩者進行了簡單的說明 `CREATE USER is the same as CREATE ROLE except that it implies LOGIN.` ，也就是說如下的命令是等價的。

{% highlight text %}
CREATE ROLE foobar PASSWORD 'foobar' LOGIN;
CREATE USER foobar PASSWORD 'foobar';
{% endhighlight %}

### 數據庫 VS. 模式

簡單來說模式 (Schema) 就是對數據庫 (Database) 的邏輯分割，而且在數據庫創建的時候，已經默認創建了一個 public 模式，在此數據庫中創建的對象，如表、函數、試圖、索引、序列等都保存在這個模式中。

{% highlight text %}
----- 1. 創建一個數據庫
CREATE DATABASE tsdb;
----- 2. 鏈接到新建的數據庫，並查看其中的模式
\c tsdb
\dn
----- 3. 新建一張測試表
CREATE TABLE test(id INTEGER NOT NULL);
\d
----- 4. 創建新的模式，同時設置屬主為默認用戶，並查看當前庫所有的表
CREATE SCHEMA foobar AUTHORIZATION postgres;
CREATE TABLE foobar.test (id INTEGER NOT NULL);
SELECT * FROM pg_tables WHERE schemaname NOT IN('pg_catalog', 'information_schema');
----- 5. 如果通過\d查看時，需要設置搜索路徑
SHOW search_path;
SET search_path TO 'foobar,public'
\d
{% endhighlight %}

也就是說，數據庫通過模式做邏輯區分，而且一個數據庫至少包含一個模式，接到一個數據庫後，可以通過 `search_path` 設置搜索順序。

### 表空間 VS. 數據庫

在通過 `CREATE DATABASE dbname` 語句創建數據庫時，默認的數據庫所有者是當前創建數據庫的角色，默認表空間是系統的默認表空間 pg_default ，其主要原因是創建是通過克隆數據庫模板實現的。

如上創建數據庫時，如果沒有指明數據庫模板，系統將默認克隆 template1 數據庫，其默認表空間是 pg_default ，其完整的語句如下。

{% highlight text %}
CREATE DATABASE dbname OWNER foobar TEMPLATE template1 TABLESPACE tablespacename;
{% endhighlight %}

實際上可以通過如下的步驟進行測試：

{% highlight text %}
----- 1. 切換到template1數據庫並新建一個表進行測試
\c template1
CREATE TABLE test(id INTEGER NOT NULL);
INSERT INTO test VALUES (1);
----- 2. 創建一個表空間，需要注意對應的目錄存在且為空
CREATE TABLESPACE tsfoobar OWNER postgres LOCATION '/tmp/foobar';
----- 3. 創建一個數據庫
CREATE DATABASE dbfoobar TEMPLATE template1 OWNERE postgres TABLESPACE tsfoobar;
{% endhighlight %}

鏈接查看新數據庫時，實際上存在一個表，而且有上述寫入的數據。表空間是一個存儲區域，在一個表空間中可以存儲多個數據庫，儘管 PostgreSQL 不建議這麼做，例如將索引保存到 SSD 中，而數據保存到 SATA 中。


## 常用特性

### sequece

序列對象，也被稱為序列生成器，實際上就是用 `CREATE SEQUENCE` 創建的特殊的單行表，通常用來表生成唯一的標識符。

{% highlight text %}
----- 直接在建表時使用serial類型，默認生成為tblname+colname+'seq'
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

----- 單獨創建序列，建表時指定，不過該列需要為int類型
postgres=# CREATE SEQUENCE seq_tblseq2 INCREMENT BY 1 MINVALUE 1 NO MAXVALUE START WITH 1;
CREATE SEQUENCE
postgres=# CREATE TABLE tbl_seq2(id INT NOT NULL DEFAULT nextval('seq_tblseq2'), name TEXT);
CREATE TABLE
{% endhighlight %}

其中 `CREATE SEQUECE` 的語法可以參考 [www.postgresql.com](https://www.postgresql.com/docs/current/static/sql-createsequence.html)，接下來看看如何使用序列。

{% highlight text %}
postgres=# INSERT INTO tbl_seq VALUES(nextval('tbl_seq_id_seq'), 'Lucy');
INSERT 0 1
postgres=# INSERT INTO tbl_seq(name) VALUES('Andy');
INSERT 0 1
{% endhighlight %}

還有一個問題是在數據遷移之後，如何設置 sequence 。

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

------ 嘗試恢復
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

其它的一些常用函數還包括了：

1. `nextval(seq)`：遞增到下一個值，並返回當前值，即使多個會話併發執行該函數，每個進程也會安全的收到一個唯一的序列值。
2. `currval(seq)`：當前會話返回的最近一次的值，如果從沒有執行過 `nextval()` 則會返回錯誤。
3. `setval(seq, bigint, boolean)`：重置序列對象的值，如果最後一個參數為 false，那麼 nextval 首先返回該值，然後才開始遞增。

刪除非常簡單。

{% highlight text %}
postgres=# DROP SEQUENCE seq;
{% endhighlight %}

### cursor

在一個查詢中，可以使用遊標 (cursor)，防止一個大查詢超過了內容的容量，而對於 PL/pgSQL 來說，FOR 語句則默認使用了遊標。


{% highlight text %}
postgre=# CREATE TABLE foobar(id INT);
CREATE TABLE
postgre=# INSERT INTO foobar VALUES(generate_series(1, 1000));
INSERT 0 1000

postgre=# BEGIN;
BEGIN
postgre=# DECLARE cur CURSOR FOR SELECT * FROM foobar;
DECLARE CURSOR
postgre=# FETCH FIRST FROM cur;           # 移動到第一行，並返回第一行數據
 id
----
  1
(1 row)
postgre=# FETCH NEXT FROM cur;            # 獲取下一行
 id
----
  2
(1 row)
postgre=# MOVE LAST IN cur;               # 移動到最後
MOVE 1
postgre=# CLOSE cur;                      # 關閉遊標
CLOSE CURSOR
{% endhighlight %}

另外，PG 允許在函數中返回對 cursor 的引用，這樣就可以直接通過函數返回一個大的對象。

### 常用程序

簡單介紹下 PG 相關的程序，部分比較複雜的只是在此簡單記錄下，詳細介紹可以參考後面的文章。

#### pg_config

用來顯示當前版本的一些配置參數，如 bin 目錄、版本信息、cpp flags 等。

#### pg_controldata

用來顯示當前 cluster 的一些詳細信息。

{% highlight text %}
$ pg_controldata -D $PGDATA
{% endhighlight %}

#### pg_test_fsync

用於測試不同的 wal_sync_method 參數所具有的性能。

#### pg_test_timing

用於時間測試的程序，詳細可以參考文檔。



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


## 多實例部署

有些常見的操作，如主備，需要在同一臺機器上部署多個實例。

## WAL 日誌解析

9.3 之後提供了 pg_xlogdump 工具用來解析 xlog 中的內容，對於之前的版本可以考慮使用 [xlogdump-github](https://github.com/snaga/xlogdump) 這個開源的軟件。

通過該工具找到精準的誤操作XID。

pg_ctl reload 重新加載即可。
-->




## 參考

可以參考官方網站 [www.postgresql.org](http://www.postgresql.org/)，以及 [PostgreSQL 中文社區](http://www.postgres.cn/) 。國內 PG 研究比較多的人，可以查看 [PostgreSQL Research](http://blog.163.com/digoal@126/)。

源碼可以參考 [Github Postgres](https://github.com/postgres/postgres) 。

<!--
pgcli.com
http://www.rails365.net/groups/postgresql

[xxxx](http://blog.itpub.net/30088583) 。

http://www.postgres.cn/news/viewone/1/99                 參考資料
https://github.com/ty4z2008/Qix/blob/master/pg.md

http://www.2cto.com/database/201608/541209.html
https://pan.baidu.com/share/home?uk=1982970774#category/type=0

pgadmin3  管理工具

PostgreSQL 9 Administration CookBook    一本不錯的書籍
-->


{% highlight text %}
{% endhighlight %}
