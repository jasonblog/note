---
title: MySQL 複製方式
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,複製
description: MySQL 的複製包括了多種方式，一種是基於 Binlog 的原生複製方式，除此之外，還包括了通過插件實現的 semi-sync 複製。另外，針對 InnoDB 實現的 xtrabackup 也可以作為一種複製方式。在本文中，簡單介紹下 MySQL 中的複製方式。
---

MySQL 的複製包括了多種方式，一種是基於 Binlog 的原生複製方式，除此之外，還包括了通過插件實現的 semi-sync 複製。另外，針對 InnoDB 實現的 xtrabackup 也可以作為一種複製方式。

在本文中，簡單介紹下 MySQL 中的複製方式。

<!-- more -->

![mysql replication logo]({{ site.url }}/images/databases/mysql/replication-logo.jpg){: .pull-center width="60%"}

## 簡介

MySQL 在複製時，可以指定要忽略的數據庫、需要複製的數據庫甚至具體那些表；支持異步複製、半同步複製、同步複製 (NDB Cluster, Group Replication)、延遲複製等模式。

在進行復制的時候，有兩種格式： Statement Based、Row Based，也可以是兩者的組合，在配置文件中通過 binlog_format 參數進行設置；後面再介紹與格式相關的內容。

其中，MySQL 的複製原理如下圖所示。

![mysql basic replication]({{ site.url }}/images/databases/mysql/replication-basic.png){: .pull-center width="50%"}

在主服務器上，會將數據的更新寫入到 binary log 中，而備服務器會從該文件中讀取對數據的更改；每次備服務器鏈接到主時，都會分配一個單獨的線程進行處理；該線程會將 binlog 產生的事件發送到備服務器。

通常來說，主服務器會直接從緩存中讀取 binlog ，所以不會對磁盤造成壓力；但是，如果讀取的數據是半小時，甚至更長事件之前的數據，那麼就會不可避免的發生磁盤 IO 。

### 備服務器

在備服務器上有兩個線程，分別是 IO Thread 以及 SQL Thread 。

IO Thread 線程會從主服務器讀取數據，然後保存到本地的日誌文件 relay log，該線程當前的狀態可以通過 show slave status 查看。

SQL thread 會讀取本地的 relay log，然後將相應的語句寫入到數據庫。


### 延遲複製

當發生延遲複製 (Replication Lag) 時，通常是由於 SQL 線程延遲導致的，當然，最好是通過 show slave status 查看兩個線程的狀態。如果是 IO 線程導致，最好是打開壓縮協議，減小網絡 IO 的消耗量。

如果是 SQL 線程導致的，那麼會比較複雜一些，需要根據具體的情況排查。

### reset 命令

簡單介紹下常用的相關的命令。

#### RESET MASTER

用於刪除所有的 binglog 日誌文件，並將日誌索引文件清空，重新開始所有新的日誌文件；通常用於第一次進行搭建主從庫時，進行 binlog 初始化工作。
 
#### RESET SLAVE

用於刪除 SLAVE 數據庫的 relaylog 日誌文件，並重新啟用新的 relaylog 文件；如果使用 ALL 參數，同時會清理相關的主庫信息。

通常用於在主備切換時，為了防止備庫異常鏈接原主庫導致數據不一致，需要清理與主庫的鏈接信息，保證新主庫不會再鏈接到原主庫，為此，提供了 ```RESET SLAVE``` 命令。

但是不同的 MySQL 版本，對應的命令可能會有所區別，簡述如下：

{% highlight text %}
MySQL 5.0+5.1 執行 STOP SLAVE; CHANGE MASTER TO MASTER_HOST=''; RESET SLAVE；
MySQL 5.5+5.6 執行 STOP SLAVE; RESET SLAVE ALL；
{% endhighlight %}

對於所有版本都嚴禁設置 ```master-user```、```master-host```、```master-password``` 參數，當然在 MySQL 5.5 之後的版本不再支持這些參數了。

<!--
LINKKK: https://www.percona.com/blog/2013/04/17/reset-slave-vs-reset-slave-all-disconnecting-a-replication-slave-is-easier-with-mysql-5-5/
-->

### 相關文件

除了上述的二進制日誌和中繼日誌文件外，還有其它一些與複製相關的文件。

#### *.index

服務器一旦開啟二進制日誌，會產生一個與二日誌文件同名，但是以 .index 結尾的文件；該文件用於跟蹤磁盤上存在哪些二進制日誌文件，MySQL 用它來定位二進制日誌文件。

與 binlog 相似，對於中繼日誌，同樣存在一個索引文件。

#### *.info

一般為 master.info 以及 relay-log.info，前者保存 master 的相關信息，slave 重啟後會通過該信息連接 master；後者包含備中當前二進制日誌和中繼日誌的信息。

注意，上述設置需要保證在配置文件中添加如下配置。

{% highlight text %}
[mysqld]
master_info_repository     = FILE
relay_log_info_repository  = FILE
{% endhighlight %}

也可以將上述的值設置為 TABLE ，此時會將上述的數據保存在 mysql 庫的 slave_master_info 以及 slave_relay_log_info 兩個表中；存放表裡有兩個好處：

1. 明文存儲在表中相比文件存儲要安全很多；
2. 可避免 relay.info 更新不及時，SLAVE 重啟後導致的主從複製出錯。

上述保存的表默認採用 MyISAM 存儲引擎，官方建議改為 InnoDB 引擎，防止表損壞後自行修復。

{% highlight text %}
mysql> ALTER TABLE slave_master_info    Engine=InnoDB;
mysql> ALTER TABLE slave_relay_log_info Engine=InnoDB;
mysql> ALTER TABLE slave_worker_info    Engine=InnoDB;
{% endhighlight %}

另外，可以開啟如下兩個參數，這兩個是啟用 relaylog 的自動修復功能，避免由於網絡之類的外因造成日誌損壞，導致主從停止。

{% highlight text %}
relay_log_purge = ON
relay_log_recovery = ON
{% endhighlight %}













<!--
在談這個特性之前，我們先來看看MySQL的複製架構衍生史。MySQL的複製分為四種：

1、普通的replication，異步。

搭建簡單，使用非常廣泛，從MySQL誕生之初，就產生了這種架構，性能非常好，可謂非常成熟。但是這種架構數據是異步的，所以有丟失數據庫的風險。

2、semi-sync replication，半同步。

性能，功能都介於異步和全同步中間。從mysql5.5開始誕生，目的是為了折中上述兩種架構的性能以及優缺點。

3、sync replication，同步。

目前官方MySQL 5.7.17基於Group replication的全同步技術已經問世，全同步技術帶來了更多的數據一致性保障。相信是未來同步技術一個重要方向，值得期待。MySQL 5.7 Group Replication
https://dev.mysql.com/doc/refman/5.7/en/group-replication.html

4、mysql cluster，同步。

基於NDB引擎，搭建也簡單，本身也比較穩定，是MySQL裡面對數據保護最靠譜的架構，也是目前唯一一個數據完全同步的架構，數據零丟失。不過對業務比較挑剔，限制也較多。MySQL NDB CLUSTER
https://dev.mysql.com/doc/refman/5.7/en/mysql-cluster.html

根據這幾種複製協議，分別對應MySQL幾種複製類型，分別是異步、半同步、同步。
-->


## 測試實例

在此介紹下一些常見的數據複製方式，包括了執行的步驟，以及相關示例。

### 搭建步驟

整體步驟如下。

##### 1. 增加配置

如果要使用 MySQL 的複製，則必須要有如下的兩個配置項。

{% highlight text %}
[mysqld]
log-bin=mysql-bin             # 開啟binlog日誌
server-id=1                   # 範圍1~(2^32-1)
{% endhighlight %}

##### 2. 創建用戶

備庫鏈接到主庫時，需要通過指定帳號鏈接，且有 REPLICATION SLAVE 權限；在使用時，可以多個備庫使用不同帳號密碼，也可以使用相同的帳號密碼。

{% highlight text %}
mysql> CREATE USER 'replication'@'%.foobar.com' IDENTIFIED BY 'slave-password';
mysql> GRANT REPLICATION SLAVE ON *.* TO 'replication'@'%.foobar.com';
{% endhighlight %}

##### 3. 獲取位置

{% highlight text %}
----- 在一個會話中鎖表，對與InnoDB會阻塞commit
mysql> FLUSH TABLES WITH READ LOCK;
----- 新建另外一個會話，查看當前的File+Position
mysql> SHOW MASTER STATUS;
{% endhighlight %}

如果已存在數據，對於 InnoDB 可以通過 mysqldump 或者 mysqlbackup 進行歷史備份。

{% highlight text %}
----- 其中--master-data會自動添加CHANGE MASTER TO命令
$ mysqldump --all-databases --master-data > dbdump.db
{% endhighlight %}

<!--
https://dev.mysql.com/doc/refman/5.7/en/replication-snapshot-method.html
-->

##### 4. 配置主庫信息

在備庫上執行如下命令，相關信息會保存在 master.info 文件中。

{% highlight text %}
mysql> CHANGE MASTER TO
           MASTER_HOST='master_host_name',
           MASTER_USER='replication_user_name',
           MASTER_PASSWORD='replication_password',
           MASTER_LOG_FILE='recorded_log_file_name',
           MASTER_LOG_POS=recorded_log_position;
{% endhighlight %}

### 常用命令

簡單記錄下常用的命令。

{% highlight text %}
mysql> SHOW SLAVE STATUS\G                    ← 查看slave詳細信息
mysql> HELP CHANGE MASTER TO;                 ← 查看幫助
mysql> CHANGE MASTER TO
       master_host='localhost',               ← 主服務器地址
       master_port=3307,                      ← 主服務器端口，無引號
       master_user='mysync',                  ← 連接到主服務器的用戶
       master_password='kidding',             ← 連接到主服務器的用戶密碼
       master_log_file='mysql-bin.000003',    ← 指定從主那個binlog文件開始複製
       master_log_pos=496;                    ← 從主binlog的指定位置開始複製，無引號
mysql> START SLAVE;
mysql> START SLAVE IO_THREAD;                 ← 也可以使用sql_thread
mysql> STOP SLAVE;                            ← 也可以停止

----- 查看binlog日誌信息
$ mysqlbinlog -v --base64-output=decode-rows mysql-bin.000019
{% endhighlight %}

<!--
master_connect_retry      #連接重試時間;
master_SSL_Allowed        #是否開啟基於SSL的複製;
master_SSL_CA_File        #指定CA文件;
master_SSL_CA_Path        #指定CA文件路徑;
master_SSL_Cert           #指定證書;
master_SSL_Cipher         #加密算法;
master_SSL_Key            #私鑰文件;
-->

在執行 `CHANGE MASTER TO` 命令時，可以只修改部分命令參數，例如只修改同步的位置信息。

如果不指定 `master_log_file` 和 `master_log_pos` 參數，則會從頭開始複製；但是如果已經有很多數據了，可以通過 `mysqldump` 導出，並記錄二進制文件以及位置。



#### 重置複製

在如下的測試中，可以通過下面的方式重置複製。

{% highlight text %}
----- 備庫上執行，停止複製，等待SQL線程執行完成之後都停止
mysql> STOP SLAVE IO_THREAD;
Query OK, 0 rows affected, 1 warning (0.00 sec)
mysql> STOP SLAVE;
Query OK, 0 rows affected, 1 warning (0.00 sec)

----- 備庫上執行，重置備庫
mysql> RESET SLAVE;              # 刪除master.info和relay-log.info，但會保留同步信息
Query OK, 0 rows affected, 1 warning (0.00 sec)
mysql> RESET SLAVE ALL;          # 會徹底清除備庫所有信息，包括同步信息
Query OK, 0 rows affected, 1 warning (0.00 sec)

----- 主庫上執行，重置主庫
mysql> RESET MASTER ALL;
Query OK, 0 rows affected, 1 warning (0.00 sec)
{% endhighlight %}

然後，根據不同的複製方式重新建立鏈接。

{% highlight text %}
----- 對於常規的異步方式或者半同步方式
mysql> SHOW MASTER STATUS;

mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_log_file='mysql-bin.000003',master_log_pos=496;

----- 對於GTID模式
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_auto_position = 1;
{% endhighlight %}





<!--

RESET MASTER

刪除所有index file中記錄的所有binlog文件，將日誌索引文件清空，創建一個新的日誌文件，這個命令通常僅僅用於第一次用於搭建主從關係的時的主庫，

注意

reset master不同於purge binary log的兩處地方

1）reset master將刪除日誌索引文件中記錄的所有binlog文件，創建一個新的日誌文件起始值從000001開始，然而purge binary log命令並不會修改記錄binlog的順序的數值。

2）reset master不能用於有任何slave正在運行的主從關係的主庫，因為在slave運行時刻reset master命令不被支持，reset master將master的binlog從000001開始記錄，slave記錄的master log則是reset master時主庫的最新的binlog，從庫會報錯無法找的指定的binlog文件。

In MySQL 5.6.5 and later, RESET MASTER also clears the values of the gtid_purged system variable (known as gtid_lost in MySQL 5.6.8 and earlier) as well as the global value of the gtid_executed (gtid_done, prior to MySQL 5.6.9) system variable (but not its session value); that is, executing this statement sets each of these values to an empty string (”)

RESET SLAVE

RESET SLAVE將使slave忘記主從複製關係的位置信息。該語句將被用於乾淨的啟動，它刪除master.info文件和relay-log.info文件以及所有的relay log文件並重新啟用一個新的relaylog文件。

使用reset slave之前必須使用stop slave 命令將複製進程停止。

注：所有的relay log將被刪除不管他們是否被SQL thread進程完全應用（這種情況發生於備庫延遲以及在備庫執行了stop slave 命令），存儲複製鏈接信息的master.info文件將被立即清除，如果SQL thread 正在複製臨時表的過程中，執行了stop slave ，並且執行了reset slave，這些被複制的臨時表將被刪除。

RESET SLAVE ALL

在5.6版本中reset slave並不會清理存儲於內存中的複製信息，比如 master host，master port，master user，or master password，也就是說如果沒有使用change master命令做重新定向，執行start slave還是會指向舊的master上面。

當從庫執行reset slave之後,將mysqld shutdown 複製參數將被重置。

在5.6.3 版本以及以後使用使用RESET SLAVE ALL來完全的清理複製連接參數信息。(Bug #11809016)

RESET SLAVE ALL does not clear the IGNORE_SERVER_IDS list set by CHANGE MASTER TO. This issue is fixed in MySQL 5.7. (Bug #18816897)
In MySQL 5.6.7 and later, RESET SLAVE causes an implicit commit of an ongoing transaction. See Section 13.3.3, “Statements That Cause an Implicit Commit”.
-->






### 主備複製

在此，我們會在同一臺機器的 /tmp 目錄下部署兩個主備實例；當然，這只是一個示例，也只是為了測試而已。

<!--
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-master
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-slave
-->

{% highlight text %}
----- 1.1 安裝主備兩個實例
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-master --user=mysql
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-slave  --user=mysql

----- 1.2 修改配置文件，其中log-bin必須設置，server-id需要不同
$ cat /tmp/mysql-master/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-master.sock
pid-file        = /tmp/mysql-master.pid
datadir         = /tmp/mysql-master
port            = 3307
server-id       = 1
$ cat /tmp/mysql-slave/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-slave.sock
pid-file        = /tmp/mysql-slave.pid
datadir         = /tmp/mysql-slave
port            = 3308
server-id       = 2
relay_log_index = relay-bin.index
relay_log       = relay-bin
report_host     = 127.1
report_port     = 3308

----- 2.1 分別啟動主備服務器
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-master/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-master > /dev/null 2>&1 &
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-slave/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-slave  > /dev/null 2>&1 &

----- 2.2.0 登陸
$ mysql -p -P3307 -uroot -S/tmp/mysql-master.sock
$ mysql -p -P3308 -uroot -S/tmp/mysql-slave.sock
----- 2.2.1 修改密碼
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new-password';
----- 2.2.2 然後用新密碼登陸
$ mysql -p'new-password' -P3307 -uroot -S/tmp/mysql-master.sock
$ mysql -p'new-password' -P3308 -uroot -S/tmp/mysql-slave.sock

----- 3.1 配置主服務器，需要新建一個用戶
mysql> GRANT REPLICATION SLAVE ON *.* to 'mysync'@'localhost' IDENTIFIED BY 'kidding';
mysql> SHOW MASTER STATUS;            # 查看相應的File以及Position
----- 3.2 配置從服務器，然後啟動
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_log_file='mysql-bin.000003',master_log_pos=496;
mysql> START SLAVE;
mysql> STOP SLAVE;                    # 也可以停止

----- 4. 關閉數據庫
$ mysqladmin -uroot -S /tmp/mysql-master.sock shutdown
$ mysqladmin -uroot -S /tmp/mysql-slave.sock shutdown
{% endhighlight %}

現在基本就在同一個服務器上創建了一個主備的測試實例，接下來就可以通過創建數據庫、表、寫入數據等進行測試，如果正常則可以直接在備上看到主所作的操作。

另外，在主從上分別執行 show processlist 就可以看到，主多一個線程處理 Binlog Dump；而在從上有兩個線程，分別在等待主發送 event，另外一個線程則進行回放。

可以參考 [build-master-slave.sh](/reference/databases/mysql/build-master-slave.sh) 腳本在 /tmp 目錄下創建主備複製；對於一主多備可以參考 [build-master-multislave.sh](/reference/databases/mysql/build-master-multislave.sh) 。

### 主主複製

同上，會在同一臺機器的 /tmp 目錄下部署兩個主主實例，用於測試。

<!--
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-master
$ /opt/mysql-5.7/scripts/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
       --datadir=/tmp/mysql-slave
-->

{% highlight text %}
----- 1.1 安裝主主兩個實例
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-master1 --user=mysql
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/tmp/mysql-master2 --user=mysql

----- 1.2 修改配置文件，其中log-bin必須設置，server-id需要不同
$ cat /tmp/mysql-master1/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-master1.sock
pid-file        = /tmp/mysql-master1.pid
datadir         = /tmp/mysql-master1
port            = 3307
server-id       = 1
relay_log_index = relay-bin.index
relay_log       = relay-bin
report_host     = 127.1
report_port     = 3307
auto-increment-increment = 2
auto-increment-offset    = 1
$ cat /tmp/mysql-master2/my.cnf
[mysqld]
binlog_format   = mixed
log_warnings    = 1
log_error       = localhost.err
log-bin         = mysql-bin
basedir         = /opt/mysql-5.7
socket          = /tmp/mysql-master2.sock
pid-file        = /tmp/mysql-master2.pid
datadir         = /tmp/mysql-master2
port            = 3308
server-id       = 2
relay_log_index = relay-bin.index
relay_log       = relay-bin
report_host     = 127.1
report_port     = 3308
auto-increment-increment = 2
auto-increment-offset    = 2

----- 2.1 分別啟動主主服務器
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-master1/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-master1 > /dev/null 2>&1 &
$ /opt/mysql-5.7/bin/mysqld --defaults-file=/tmp/mysql-master2/my.cnf  \
    --basedir=/opt/mysql-5.7 --datadir=/tmp/mysql-master2 > /dev/null 2>&1 &

----- 2.2.0 登陸
$ mysql -p -P3307 -uroot -S/tmp/mysql-master1.sock
$ mysql -p -P3308 -uroot -S/tmp/mysql-master2.sock
----- 2.2.1 修改密碼
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new-password';
----- 2.2.2 然後用新密碼登陸
$ mysql -p'new-password' -P3307 -uroot -S/tmp/mysql-master1.sock
$ mysql -p'new-password' -P3308 -uroot -S/tmp/mysql-master2.sock

----- 3.1 配置兩個主服務器，需要新建一個用戶
mysql> GRANT REPLICATION SLAVE ON *.* to 'mysync'@'localhost' IDENTIFIED BY 'kidding';
mysql> SHOW MASTER STATUS;            # 查看相應的File以及Position
----- 3.2 配置兩個主服務器，然後啟動
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_log_file='mysql-bin.000003',master_log_pos=496;
mysql> START SLAVE;
mysql> STOP SLAVE;                    # 也可以停止

----- 4. 關閉數據庫
$ mysqladmin -uroot -S /tmp/mysql-master1.sock shutdown
$ mysqladmin -uroot -S /tmp/mysql-master2.sock shutdown
{% endhighlight %}

兩臺服務器都需要開啟二進制日誌和中繼日誌。

主備設置主鍵自動增長 auto-increment-increment 步長均為 2，而起始數值則分別為 1 和 2 ，這樣兩個服務的 auto_increment 值分別為奇數和偶數，從而避免數據同步時出現主鍵衝突。

可以參考 [build-master-master.sh](/reference/databases/mysql/build-master-master.sh) 腳本在 /tmp 目錄下創建主主複製。



### 半同步機制

執行流程為。

1. 當 Master 上開啟半同步複製的功能時，至少應該有一個 Slave 開啟其功能；此時，一個線程在 Master 上提交事務將受到阻塞，直到得知一個已開啟半同步複製功能的 Slave 已收到此事務的所有事件，或等待超時。

2. 當 Slave 主機連接到 Master 時，能夠查看其是否處於半同步複製的機制。

3. 當一個事務的事件都已寫入其 relay-log 中且已刷新到磁盤上，Slave 才會告知已收到。

4. 如果等待超時，也就是 Master 沒被告知已收到，此時 Master 會自動轉換為異步複製的機制。當至少一個半同步的 Slave 趕上了，Master 與其 Slave 自動轉換為半同步複製的機制。

5.  半同步複製的功能要在 Master，Slave 都開啟，半同步複製才會起作用；否則，只開啟一邊，它依然為異步複製。

如下是搭建的方法。

{% highlight text %}
----- 1. 安裝半同步模塊並啟動，模塊保存在lib/plugin目錄下
mysql> INSTALL PLUGIN rpl_semi_sync_slave SONAME 'semisync_slave.so';
mysql> INSTALL PLUGIN rpl_semi_sync_master SONAME 'semisync_master.so';
----- 1.1 啟動半同步模塊，並設置超時時間2s
mysql> SET GLOBAL rpl_semi_sync_slave_enabled = ON;
mysql> SET GLOBAL rpl_semi_sync_master_enabled = ON;
mysql> SET GLOBAL rpl_semi_sync_master_timeout = 2000;
----- 1.2 也可以設置配置文件
[mysqld]
plugin_dir = /opt/mysql-5.7/lib/plugin
plugin_load = "rpl_semi_sync_master=semisync_master.so;rpl_semi_sync_slave=semisync_slave.so"
rpl_semi_sync_master_enabled = ON
rpl_semi_sync_slave_enabled = ON
rpl_semi_sync_master_timeout = 2000

----- 2 如果已經搭建主備複製，則從節點需要重新連接主服務器半同步才會生效
mysql> STOP SLAVE IO_THREAD;
mysql> START SLAVE IO_THREAD;

----- 3. 查看是否已經啟動，需要大於等於1
mysql> SHOW GLOBAL STATUS LIKE 'rpl_semi_sync_master_clients';
{% endhighlight %}

現在半同步已正常工作，可以驗證一下半同步超時，是否會自動降為異步工作；可在 Slave 上停掉半同步協議，然後在 Master 上創建數據庫看一下能不能複製到 Slave 上。

{% highlight text %}
-----  slave ------------------------+----- master -------------------------------
### 關閉備庫的IO線程
mysql> STOP SLAVE IO_THREAD;
Query OK, 0 rows affected (0.00 sec)
                                       ### 執行2s超時，主庫仍然執行成功，備庫失敗
                                       mysql> CREATE DATABASE foobar;
                                       Query OK, 1 row affected (2.01 sec)
### 啟動備庫的IO線程，查看DB已經同步
mysql> START SLAVE IO_THREAD;
Query OK, 0 rows affected (0.00 sec)
mysql> SHOW DATABASES LIKE 'foobar' ;
+-------------------+
| Database (foobar) |
+-------------------+
| foobar            |
+-------------------+
1 row in set (0.00 sec)
                                       ### 可以執行成功
                                       mysql> CREATE DATABASE foobar1;
                                       Query OK, 1 row affected (0.01 sec)
{% endhighlight %}

創建第一個數據庫花了 2.01 秒，如前設置的超時時間是 2 秒；而創建第二個數據庫只花了 0.01 秒，由此得出結論是超時轉換為異步傳送。

可以參考 [build-semisync.sh](/reference/databases/mysql/build-semisync.sh) 腳本在 /tmp 目錄下創建 semisync 的主主複製。


### GTID 複製

如下是搭建的方法，需要注意的是，執行 ```CHANGE MASTER TO``` 的命令與原生的稍微有些區別。

{% highlight text %}
----- 在配置文件中添加如下內容
[mysqld]
gtid-mode                 = ON
enforce-gtid-consistency  = ON

----- 設置主庫信息
mysql> CHANGE MASTER TO master_host='localhost',master_port=3307,
       master_user='mysync',master_password='kidding',
       master_auto_position = 1;
{% endhighlight %}

可以參考 [build-gtid-replication-mm.sh](/reference/databases/mysql/build-gtid-replication-mm.sh) 腳本，會在 /tmp 目錄下創建 gtid 的主主複製；或者主從複製 [build-gtid-replication-ms.sh](/reference/databases/mysql/build-gtid-replication-ms.sh)；或者一主多從複製 [build-gtid-replication-multislaves.sh](/reference/databases/mysql/build-gtid-replication-multislaves.sh)，需要注意的是，在備庫鏈接到主庫前，需要執行 ```RESET MASTER;``` 命令清空 GTID_EXECUTED，否則會導致 Errant-Transaction 。

<!--
需要注意開啟 GTID 複製模式的約束：

不支持非事務類型的存儲引擎複製，如MyISAM；
create table...select語句不支持，因為在基於row的複製中，該語句會備拆分為create和insert兩件事務，並且會分配到相同的transaction id，會造成slave跳過相同transaction id而不同步的現象；
不支持臨時表的使用；
無法使用sql_slave_skip_counter 跳過複製中出現的錯誤，可以在slave節點中指定變量gtid_executed來跳過相應的錯誤；
在mysql5.6.7以前的版本在運行mysql_upgrade的時候，如果開啟gtid_mode=on的時候沒有指定write_binlog=off會連接不上server。


1.如果master所有的binlog還在，安裝slave後，直接change master 到master
原理是直接獲取master所有的gtid並執行
優點是簡單
缺點是如果binlog太多，數據完全同步需要的時間較長，並且需要master一開始就啟用了GTID
總結：適用於master也是新建不久的情況

2.通過master或者其它slave的mysqldump備份搭建新的slave.
原理：備份時獲取master的數據和這些數據對應的GTID，在Slave端跳過備份包含的GTID
優點是可以避免第一種方法中的不足
缺點操作相對複雜
總結：適用於擁有較大數據集的情況

3、percona xtrabackup
基於xtrabackup備份文件xtrabackup_binlog_info包含了GTID信息
做從庫恢復後，需要手工設置：
set@@GLOBAL.GTID_PURGED='c8d960f1-83ca-11e5-a8eb-000c29ea831c:1-745497';
恢復後，執行change master to
缺點操作相對複雜
總結：適用於擁有較大數據集的情況
-->


### 狀態監控

{% highlight text %}
mysql> SHOW SLAVE STATUS\G
*************************** 1. row ***************************
               Slave_IO_State: Waiting for master to send event
                  Master_Host: localhost           # 主服務器地址
                  Master_User: mysync              # 授權用戶名
                  Master_Port: 3307                # 主服務器綁定的端口號
                Connect_Retry: 60
              Master_Log_File: mysql-bin.000003
          Read_Master_Log_Pos: 496                 # 主服務器二進制日誌位置
               Relay_Log_File: mysql-slave-relay-bin.000001
                Relay_Log_Pos: 4
        Relay_Master_Log_File: mysql-bin.000003
             Slave_IO_Running: Yes                 # 重要
            Slave_SQL_Running: Yes                 # 重要
              Replicate_Do_DB:
          Replicate_Ignore_DB:
           Replicate_Do_Table:
       Replicate_Ignore_Table:
      Replicate_Wild_Do_Table:
  Replicate_Wild_Ignore_Table:
                   Last_Errno: 0
                   Last_Error:
                 Skip_Counter: 0
          Exec_Master_Log_Pos: 447
              Relay_Log_Space: 531
              Until_Condition: None
               Until_Log_File:
                Until_Log_Pos: 0
           Master_SSL_Allowed: No
           Master_SSL_CA_File:
           Master_SSL_CA_Path:
              Master_SSL_Cert:
            Master_SSL_Cipher:
               Master_SSL_Key:
        Seconds_Behind_Master: 0
Master_SSL_Verify_Server_Cert: No
                Last_IO_Errno: 0
                Last_IO_Error:
               Last_SQL_Errno: 0
               Last_SQL_Error:
  Replicate_Ignore_Server_Ids:
             Master_Server_Id: 1
                  Master_UUID: 10de195f-f051-11e6-8362-ac2b6e8b4228
             Master_Info_File: /tmp/mysql-slave/master.info
                    SQL_Delay: 0
          SQL_Remaining_Delay: NULL
      Slave_SQL_Running_State: Slave has read all relay log; waiting for more updates
           Master_Retry_Count: 86400
                  Master_Bind:
      Last_IO_Error_Timestamp:
     Last_SQL_Error_Timestamp:
               Master_SSL_Crl:
           Master_SSL_Crlpath:
           Retrieved_Gtid_Set:
            Executed_Gtid_Set:
                Auto_Position: 0
         Replicate_Rewrite_DB:
                 Channel_Name:
           Master_TLS_Version:
1 row in set (0.00 sec)

{% endhighlight %}




## 常用工具

一些主備複製中常用的工具，主要來自 percona-tools 。

### pt_heartbeat

該工具用來監控主備複製的延遲。

通常，是執行 SHOW SLAVE STATUS 查看 Slave_IO_Running (Yes)、Slave_SQL_Running (Yes)、Seconds_Behind_Master (0)；實際上這個是不可靠的，其原因如下：

1.  延遲時間的計算是服務器當前的時間戳與二進制日誌中的事件時間戳相減得到的，也就是隻有在執行事件時才能計算延遲；

2. 如果備庫複製線程沒有運行，或者由於某些錯誤導致備庫無法計算延遲，就會報延遲 NULL。

3. 長事務執行更新達一個小時，最後提交，從而導致這條更新將比它實際發生時間要晚一個小時才記錄到二進制日誌中；而當備庫執行這條語句時，會臨時地報告備庫延遲為一個小時，然後又很快變成 0 。

4. 庫級聯時，二級備庫已經追趕上了它的備庫，此時延遲顯示為 0 ；但實際上與源主庫是存在延遲的。

而該工具會在主機寫入數據，在備機讀取，然後計算兩者的差值。可以通過如下方式執行：

{% highlight text %}
----- 主機執行如下命令
$ pt-heartbeat -D test -u root -P 3307 -h 127.1 --update --create-table --daemonize
參數：
          --database, -D  需要監控的數據庫
              --user, -u  用戶
          --password, -p  密碼
              --port, -P  端口
              --host, -h  主機
            --socket, -S  主機
                --update  在指定的時間更新一個時間戳；
          --create-table  自動生成heartbeat表
             --daemonize  在後臺運行
              --ask-pass  隱式輸入MySQL密碼
               --charset  字符集設置
              --interval  檢查、更新的間隔時間，默認1s，最小單位0.01s
                 --table  指定心跳錶名，默認heartbeat
      --master-server-id  指定主的server_id
--print-master-server-id  monitor和check 模式下，打印出主server_id
{% endhighlight %}
<!--
--check
檢查從的延遲，檢查一次就退出，除非指定了--recurse會遞歸的檢查所有的從服務器。
--check-read-only
如果從服務器開啟了只讀模式，該工具會跳過任何插入。
--file 【--file=output.txt】
打印--monitor最新的記錄到指定的文件，很好的防止滿屏幕都是數據的煩惱。
--frames 【--frames=1m,2m,3m】
在--monitor裡輸出的[]裡的記錄段，默認是1m,5m,15m。可以指定1個，如：--frames=1s，多個用逗號隔開。可用單位有秒（s）、分鐘（m）、小時（h）、天（d）。
--log
開啟daemonized模式的所有日誌將會被打印到制定的文件中。
--monitor
持續監控從的延遲情況。通過--interval指定的間隔時間，打印出從的延遲信息，通過--file則可以把這些信息打印到指定的文件。
--recurse
多級複製的檢查深度。模式M-S-S...不是最後的一個從都需要開啟log_slave_updates，這樣才能檢查到。
--recursion-method
指定複製檢查的方式,默認為processlist,hosts。
--update
更新主上的心跳錶。
--replace
使用--replace代替--update模式更新心跳錶裡的時間字段，這樣的好處是不用管表裡是否有行。
--stop
停止運行該工具（--daemonize），在/tmp/目錄下創建一個“pt-heartbeat-sentinel” 文件。後面想重新開啟則需要把該臨時文件刪除，才能開啟（--daemonize）。
-->

如上，會在 test 數據庫中創建一個 heartbeat 表，然後就可以監控複製在備庫上的延遲了。

{% highlight text %}
----- 持續監控slave狀態
$ pt-heartbeat -D test -u root -P 3308 -h 127.1 --master-server-id=1 --monitor

----- 只查看一次
$ pt-heartbeat -D test -u root -P 3308 -h 127.1 --master-server-id=1 --check
{% endhighlight %}

通過如下命令可以查看下二進制日誌中到底記錄了什麼。

{% highlight text %}
$ mysqlbinlog --no-defaults /tmp/mysql-master/mysql-bin.000010
BEGIN
/*!*/;
# at 588960
#130822  6:44:01 server id 1  end_log_pos 589202        Query   thread_id=28    exec_time=0     error_code=0
SET TIMESTAMP=1377168241/*!*/;
UPDATE `test`.`heartbeat` SET ts='2013-08-22T06:44:01.005550', file='mysql-bin.000010', position='588555', relay_master_log_file=NULL, exec_master_log_pos=NULL WHERE server_id='1'
/*!*/;
# at 589202
#130822  6:44:01 server id 1  end_log_pos 589229        Xid = 6980
COMMIT/*!*/;
{% endhighlight %}

可以看到 heratbeat 表一直在更改 ts 和 position，而 ts 是我們檢查複製延遲的關鍵。

如果要停止，可以直接執行如下的命令即可。

{% highlight text %}
$ pt-heartbeat --stop
{% endhighlight %}


### pt-slave-find

查看當前主從複製的拓撲結構。

{% highlight text %}
$ pt-slave-find -u root -P 3307 -h 127.1
{% endhighlight %}

### pt-slave-restart

當備服務器報錯時，直接重啟複製；特別注意，如果使用不當，可能會導致數據不一致，可以通過下面的工具進行校驗。

{% highlight text %}
$ pt-slave-restart --socket=/tmp/mysql-slave.sock --ask-pass --error-numbers=1062
{% endhighlight %}

### pt-table-checksum

用來校驗主備的數據是否一致，在主上執行校驗查詢對複製的一致性進行檢查，對比主從的校驗值，從而產生結果。

{% highlight text %}
# pt-table-checksum --nocheck-replication-filters --no-check-binlog-format \
  --user=root --port=3307 --host=127.1 --databases=test --tables=heartbeat \
  --replicate=test.checksums
{% endhighlight %}

上述的表，如果要檢查多個，可以通過逗號分割。

如果 DIFFS 是 1 就說明主從的表數據不一致，如果指定 \-\-replicate=test.checksums 參數，則會將檢查信息都寫到了 checksums 表中。

#### 測試

可以通過如下方式新建一個表，然後進行測試。

{% highlight text %}
----- 在主庫執行如下的SQL
mysql> CREATE TABLE test.foobar (id INT PRIMARY KEY, name VARCHAR(20));
Query OK, 0 rows affected (0.01 sec)
mysql> INSERT INTO test.foobar VALUES(1, "Andy"), (2, "Alan"), (3, "Bernard"), (4, "Christian");
Query OK, 4 rows affected (0.00 sec)
Records: 4  Duplicates: 0  Warnings: 0

----- 在備庫執行如下的SQL
mysql> INSERT INTO test.foobar VALUES(5, "Hobart"), (6, "Raymond");
Query OK, 2 rows affected (0.00 sec)
Records: 2  Duplicates: 0  Warnings: 0
mysql> DELETE FROM test.foobar WHERE id = 3;
Query OK, 1 row affected (0.00 sec)
{% endhighlight %}

然後執行如下命令校驗該表，然後可以在備庫的 test.checksums 查看。

{% highlight text %}
# pt-table-checksum --nocheck-replication-filters --no-check-binlog-format \
  --user=root --port=3307 --host=127.1 --databases=test --tables=foobar \
  --replicate=test.checksums
{% endhighlight %}


### pt-table-sync

用於主備表的數據同步，可以單向、雙向同步表的數據，但是不同步表結構、索引或者其它對象，所以需要保證修復數據前保證表存在。

{% highlight text %}
# pt-table-sync --replicate=test.checksums h=127.1,u=root,P=3307 h=127.1,u=root,P=3308 --print
參數：
   --replicate  過pt-table-checksum得到校驗表
   --databases  執行同步的數據庫，多個用逗號隔開
      --tables  執行同步的表，多個用逗號隔開
       --print  打印，但不執行命令
     --execute  執行命令
{% endhighlight %}

此時，直接在備庫執行如上打印的 SQL 即可，也可以通過 \-\-execute 執行命令。


### 其它

常見報錯。

#### Cannot connect to P=xxx,h=,u=xxx

也就是無法連接備庫，詳細的報錯日誌如下。

{% highlight text %}
Cannot connect to P=xxx,h=,u=xxx
Diffs cannot be detected because no slaves were found.  Please read the --recursion-method
documentation for information.
{% endhighlight %}

默認是通過 show processlist 或者 show slave hosts 找到 host 的值。不過對於後者，需要在配置文件裡面已經配置自己的地址和端口。

{% highlight text %}
report_host = 192.168.0.20
report_port = 3306
{% endhighlight %}

<!--
METHOD       USES
===========  =============================================
processlist  SHOW PROCESSLIST
hosts        SHOW SLAVE HOSTS
cluster      SHOW STATUS LIKE 'wsrep\_incoming\_addresses'
dsn=DSN      DSNs from a table
none         Do not find slaves
-->





## 參考

MySQL 中與複製相關的內容，可以參考官方文檔 [MySQL Reference Manual - Replication](http://dev.mysql.com/doc/refman/en/replication.html)。

一個基於 PyMySQL，純 Python 編寫的 MySQL 複製協議的實現 [python-mysql-replication](https://github.com/noplay/python-mysql-replication) 。

<!--
mysql 複製相關源代碼代碼分佈
http://www.onlamp.com/2006/04/20/advanced-mysql-replication.html

http://siddontang.com/2015/02/02/mysql-replication-protocol/
-->

{% highlight text %}
{% endhighlight %}
