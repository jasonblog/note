---
title: MySQL Crash-Safe 複製
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,crash-safe,replication,複製
description: 簡單來說，crash-safe replication 是指當主備庫任何一個節點發生宕機等意外情況下，服務器重啟後主備的數據依然能夠保證一致性。接下來看看 MySQL 中是如何實現的。
---

簡單來說，crash-safe replication 是指當主備庫任何一個節點發生宕機等意外情況下，服務器重啟後主備的數據依然能夠保證一致性。

接下來看看 MySQL 中是如何實現的。

<!-- more -->

## 簡介

對於主庫來說，crash-safe master 比較簡單，只要使用事務的存儲引擎，並且正確的配置就能達到 crash safe 的效果，對於 InnoDB 可以在配置文件中添加如下的設置，也即 ```"雙1" 模式``` 。

{% highlight text %}
sync_binlog                    = 1
innodb-flush-log-at-trx-commit = 1
{% endhighlight %}

在 MySQL 5.6 版本之前存在一個 bug，即當啟用上述兩個參數時，會使得 InnoDB 存儲引擎的 group commit 失效，從而導致在寫密集的環境中性能的急劇下降。

MariaDB 真正解決了該問題，因此很多分支版本，比如 Percona、InnoSQL 都將 MariaDB 的 group commit 方案移植到了自己的分支中，解決 group commit 失效的問題。

<!--
因此，DBA在性能和數據一致性中做了妥協，通常將參數innodb-flush-log-at-trx-commit設置為2，而這就導致了master不再是crash safe的，主從數據可能會不一致。

MariaDB真正解決了該問題，因此很多分支版本，比如Percona，Facebook MySQL，InnoSQL都將MariaDB的group commit方案移植到了自己的分支中，從而解決group commit失效的問題。
-->

### crash-safe slave

這種情況要複雜的多，而且是 DBA 常見的問題，例如備庫不斷的報 1062 錯誤 (主健衝突)，或者發現主從數據不一致，特別是表沒有主鍵的情況；這時，沒有太好的辦法，基本就是全庫重建了。

首先，說明下在這裡用到的概念，我們將 event 在 binlog 所在的文件名以及其在文件中的偏移量稱為位點。
 
備庫分別有兩個線程負責複製 (IO-thread) + 回放 (SQL-thread)，前者會讀取主庫 binlog 並寫入到備庫的 relay-log 中，該線程會記錄需要讀取主庫下個位點的信息，並將該信息記錄到 master.info 文件中(包含如下的兩個信息)；而後者則負責從 relay-log 中讀取日誌，並將位點信息更新到 relay-log.info 文件中(包含如下的四個信息)。

當前備庫的信息可以通過如下方式查看。

{% highlight text %}
mysql> SHOW SLAVE STATUS;
*************************** 1. row ***************************
                          ... ...
              Master_Log_File: mysql-bin.000002       IO-thread讀取主庫binlog的文件名
          Read_Master_Log_Pos: 447                    IO-thread讀取主庫binlog的文件偏移
               Relay_Log_File: relay-bin.000002       SQL-thread讀取relay-log的文件名
                Relay_Log_Pos: 320                    SQL-thread讀取relay-log的文件偏移
        Relay_Master_Log_File: mysql-bin.000002       SQL-thread讀取的事件對應主庫的文件名
                          ... ...
          Exec_Master_Log_Pos: 447                    SQL-thread讀取的事件對應主庫的文件偏移
                          ... ...
1 row in set (0.00 sec)
{% endhighlight %}

這樣，如果 IO-thread 崩潰後，而位點信息沒有同步寫入到文件中，那麼就可能導致 event 被重複添加到 relay-log 中；而對於 SQL-thread 則會將 event 重複執行！！！

對於 INSERT 來說，在重啟備庫時就可能會報 "Duplicate entry for key PRIMARY" 的錯誤，而對於 DELETE、UPDATE 等場景會更加複雜，而且可能更加難識別。

早在 MySQL 4.0 時 Google 就發佈過 [補丁](https://code.google.com/p/google-mysql-tools/wiki/TransactionalReplication) 解決該問題，也就是在每次 InnoDB 提交時，將 binlog 的位置信息到事務系統段的段頭；當備庫重啟時，將保存的這部分的信息重新生成 relay-info.log 文件。


## 解決方案

在 5.5 中，增加了 ```sync_master_info=1``` 以及 ```sync_relay_log_info=1``` 兩個參數，用來控制每次更新位點文件時，同時進行一次 fdatasync 操作，確保每個事務都會同時將位點刷新到日誌中。<!--(這種對於write-back cache比較有效)-->

不過，即使這樣，仍然存在小概率發生崩潰，因為一般是在事務提交之後，將位點信息寫入文件；這樣如果恰好在事務提交之後，但未寫入文件之前崩潰，同樣會導致最後一個事務的位點異常。

為此，5.6 提供了兩個參數 ```relay_log_info_repository=TABLE``` 和 ```master_info_repository=TABLE```，用於將相關參數分別保存在 mysql 庫的 slave_relay_log_info 和 slave_master_info 兩個表中；而且，之前的數據提交方式將做如下的修改。

{% highlight text %}
START TRANSACTION;
-- Statement 1
-- ...
-- Statement N
COMMIT;
-- Update replication info files
{% endhighlight %}

修改為：

{% highlight text %}
START TRANSACTION;
-- Statement 1
-- ...
-- Statement N
-- Update replication info
COMMIT;
{% endhighlight %}

實際上，對於 SQL-thread 來說是有效的，因為在提交事務的時候同時將位點信息寫入到了表中，保證了操作的原子性，進而可以保持一致性。

### IO-thread 線程

但對於 IO-thread 仍然會有問題，因為寫文件+寫表仍然是兩個操作，這兩個操作不是在一個事務中，一個是數據庫操作，一個是文件操作，因此不能達到原子的效果，從而還是無法保證一致性。

為此，5.6 給出的解決方案是：

1. 將 IO-thread 的初始位點設置為與 SQL-thread 相同；
2. SQL-thread 順序執行到 relay-log 文件末尾。

上述功能需要開啟 ```relay_log_recovery=ON``` 配置，此時 IO 線程啟動時，會從 ```slave_relay_log_info``` 表中讀取需要讀取的位點信息，然後從主庫拉取數據，從而無需設置 ```master_info_repository=TABLE``` 。

注意，實際上 IO-thread 是從 SQL-thread 保存的位點開始重新從主庫獲取 event 。

## 更多討論

通過上述的方式，雖然可以解決更新 relay-info.log 的原子性問題，但是還存在一些缺陷。

由於最後表 slave_relay_log_info 的更新會鎖住記錄，從而導致備庫上的事務提交都是串行的。雖然 MySQL 5.6 之後支持並行複製，但由於串行更新表 slave_relay_log_info，再次導致 group commit 失效。

再通過 \-\-log-slave-updates 級聯複製的話，性能又會受限。


### 關於relay_log_recovery

當設置該參數之後，將會存在如下的問題：

1. 每次啟動備庫時，都會從備庫拉取數據，如果 SQL 線程已經落後很多，那麼就會導致從主庫複製大量的數據；
2. 如果同時主庫也宕機了，那麼即使備庫有大量的 binlog ，SQL 線程也不會去處理；
3. 如果主庫中的 binlog 已經被清理了，備庫同樣也無法進行恢復；
4. 如果想要在備庫備份 relay-log 的話，也即將 relay-log-purge 設置為 0，那麼重啟或者崩潰恢復將會導致備份的 relay-log 文件被損壞。

對於第一個問題，當設置了延遲複製時，對於網絡的壓力將會更加明顯。

### DDL

對於 DDL 語句，即使設置了 binlog 為 ROW 模式，實際上仍保存為 SQL 語句，而且不會觸發刷盤操作，這樣就會導致 DDL 操作，仍然有可能會重複執行。

例如，當你修改了表結構以後，突然宕機，slave_relay_log_info 表沒刷進磁盤，下次重啟服務後，會再次執行一次修改表結構，此時同步就掛了，只能手工去跳過這個錯誤。

![replication sync-relay-log-info]({{ site.url }}/images/databases/mysql/replication-sync-relay-log-info.png "replication sync-relay-log-info"){: .pull-center }

此時，可以在備庫中添加如下的參數。

{% highlight text %}
slave_skip_errors = ddl_exist_errors
{% endhighlight %}

## 總結

簡言之，需要設置如下兩個參數。

{% highlight text %}
relay_log_info_repository = TABLE
relay_log_recovery = ON
{% endhighlight %}

表默認為 MyISAM 存儲引擎，為了保證事務，需要將表設置為 InnoDB 。

{% highlight text %}
mysql> ALTER TABLE mysql.slave_master_info    ENGINE = InnoDB;
mysql> ALTER TABLE mysql.slave_relay_log_info ENGINE = InnoDB;
mysql> ALTER TABLE mysql.slave_worker_info    ENGINE = InnoDB;
{% endhighlight %}


<!--
注意，當主備配置好之後，重啟備庫時不需要通過 change master 重新配置。

從庫上執行
    STOP SLAVE IO_THREAD;
    SHOW PROCESSLIST;
其中，確保SQL-thread狀體為"Slave has read all relay log"，然後再執行：
    STOP SLAVE;    停SQL_THREAD
    RESET MASTER;  清空主庫的binlog，包括index文件，從頭開始記錄，show master status
    RESET SLAVE;   清空備庫relaylog、index，以及主備關係(master.info、relay-log.info)，並從頭開始記錄show slave status\G
    show master status;

11.3 主庫上執行
    RESETMASTER;
    RESETSLAVE;

    CHANGEMASTERTO
    MASTER_HOST='192.168.119.129',
    MASTER_USER='repl',
    MASTER_PASSWORD='1234',
    MASTER_LOG_FILE='mysql-bin.000001',
    MASTER_LOG_POS=106;
    startslave

{% highlight text %}
init_recovery()
{% endhighlight %}
-->

## 參考

關於主備複製中備庫的常用配置項可以參考 [Reference Manual - Replication Slave Options and Variables](https://dev.mysql.com/doc/refman/en/replication-options-slave.html) 。

關於 Crash-safe replication 的最佳實踐，可以參考 [Better Crash-safe replication for MySQL](https://blog.booking.com/better_crash_safe_replication_for_mysql.html) 中的介紹，或者 [本地文檔](/reference/databases/mysql/Better Crash-safe replication for MySQL.mht) 。


<!--
https://www.percona.com/blog/2013/09/13/enabling-crash-safe-slaves-with-mysql-5-6/

在主庫中，binlog的事件如何發送給dump線程？備庫中IO線程如何通知SQL線程執行？
binlog的文件格式是什麼樣的？
-->






 
 
 


{% highlight text %}
{% endhighlight %}
