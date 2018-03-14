---
title: MySQL 組複製
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,組複製,group replication
description: MySQL 當前存在異步複製、半同步複製，在 5.7.17 引入了組複製模式，這是基於分佈式一致性算法 (Paxos 協議的變體) 實現。一個組允許部分節點掛掉，只要保證多數節點仍然存活並且相互之間可以正常通訊，那麼這個組仍然可以對外提供服務，是目前一種被分佈式系統廣泛使用的技術。
---

MySQL 當前存在異步複製、半同步複製，在 5.7.17 引入了組複製模式，這是基於分佈式一致性算法 (Paxos 協議的變體) 實現。

一個組允許部分節點掛掉，只要保證多數節點仍然存活並且相互之間可以正常通訊，那麼這個組仍然可以對外提供服務，是目前一種被分佈式系統廣泛使用的技術。

如下，僅簡單介紹如何使用。

<!-- more -->

## 介紹


MySQL 組複製實現基於插件，建立在現有的 MySQL 複製基礎結構上，利用了 binlog、GTID、row-based logging、InnoDB、Corosync(三方)；注意最新版本的已經將 Corosync 替換掉。

<!--
僅支持InnoDB表，並且每張表一定要有一個主鍵，用於做write set的衝突檢測；
必須打開GTID特性，二進制日誌格式必須設置為ROW，用於選主與write set
COMMIT可能會導致失敗，類似於快照事務隔離級別的失敗場景
目前一個MGR集群最多支持9個節點
不支持外鍵於save point特性，無法做全局間的約束檢測與部分部分回滾
二進制日誌不支持binlog event checksum
-->

## 搭建集群

首先，需要確認當前 MySQL 版本是否支持組複製；實際上，可以簡單查看 plugin_dir 目錄下是否存在 group_replication.so 動態庫即可。

如下是一個簡單的配置文件，需要注意的是，默認將 ```group_replication_start_on_boot``` 參數關閉，在第一個服務器啟動時打開該參數。

{% highlight text %}
# only the last two sub-sections are directly related to Group Replication
[mysqld]
server-id      = 2
datadir        = /var/lib/mysql
socket         = /var/lib/mysql/mysql.sock
pid-file       = /tmp/mysql.pid
log-error      = mysqld.log

# replication and binlog related options
binlog-row-image          = MINIMAL           # 取消後鏡像，減小binlog大小
relay-log-recovery        = ON                # 備庫恢復時從relaylog獲取位點信息
sync-relay-log            = 1                 # 每個事務同時將位點信息持久化
sync-master-info          = 1000
slave-parallel-workers    = 4                 # 設置備庫的併發
slave-parallel-type       = LOGICAL_CLOCK     # 利用組提交的邏輯值做併發
binlog-rows-query-log-events    = ON          # ROW模式binlog添加SQL信息，方便排錯
log-bin-trust-function-creators = ON          # 同時複製主庫創建的函數
slave-preserve-commit-order     = ON          # 備庫的事務提交時，保持與主庫相同順序
log-slave-updates               = ON          # 備庫同時生成binlog

slave-rows-search-algorithms = 'INDEX_SCAN,HASH_SCAN'
slave-type-conversions       = ALL_NON_LOSSY

# group replication pre-requisites &amp; recommendations
log-bin                   = mysql-bin         # 開啟binlog
binlog-format             = ROW               # 目前只支持ROW模式
gtid-mode                 = ON                # 設置如下兩個參數啟動GTID
enforce-gtid-consistency  = ON
master-info-repository    = TABLE             # 主庫信息保存在表中
relay-log-info-repository = TABLE             # relaylog信息保存在表中
binlog-checksum           = NONE              # 取消checksum
transaction-isolation     = 'READ-COMMITTED'  # 間隙鎖可能會存在問題

# prevent use of non-transactional storage engines
disabled_storage_engines  = "MyISAM,BLACKHOLE,FEDERATED,ARCHIVE"

# group replication specific options
plugin-load         = group_replication.so    # 加載組複製的插件
group_replication   = FORCE_PLUS_PERMANENT
transaction-write-set-extraction  = XXHASH64  # MURMUR32,NONE
group_replication_start_on_boot   = OFF
group_replication_bootstrap_group = OFF
group_replication_group_name      = 550fa9ee-a1f8-4b6d-9bfe-c03c12cd1c72
group_replication_local_address   = '127.0.0.1:3307'
group_replication_group_seeds     = '127.0.0.1:3307,127.0.0.1:3308,127.0.0.1:3309'
{% endhighlight %}

InnoDB 的間隙鎖在進行多主的衝突檢測時存在問題，因此最好不要使用 REPEATABLE-READ 隔離級別，而是使用 READ-COMMITTED 隔離級別。

<!--
# InnoDB gap locks are problematic for multi-primary conflict detection; none are used with READ-COMMITTED
# So if you don't rely on REPEATABLE-READ semantics and/or wish to use multi-primary mode then this
# isolation level is recommended
-->

如下提供了一個自動搭建集群的腳本，可以在 /tmp 目錄下搭建三個 MySQL 服務，詳見腳本 [build-group-replication.sh](/reference/databases/mysql/build-group-replication.sh) 。

搭建完之後，可以通過如下命令查看當前組的成員。

{% highlight text %}
mysql> SELECT * FROM performance_schema.replication_group_members;
{% endhighlight %}


## 參考

關於 Group Replication 的基本介紹可以參考 [High Availability Using MySQL Group Replication (PPT)](https://downloads.mysql.com/presentations/innovation-day-2016/Session_7_MySQL_Group_Replication_for_High_Availability.pdf)，或者 [本地文檔](/reference/databases/mysql/MySQL_Group_Replication_for_High_Availability.pdf) 。對於 MySQL 的 Group Replication 介紹，演進過程，設計文檔等，可以參考 [mysqlhighavailability.com](http://mysqlhighavailability.com/tag/mysql-group-replication/)、[MySQL Group Replication’s documentation!](http://mysqlhighavailability.com/mysqlha/gr/doc/index.html)

MySQL Group Replication 的實現可以參考論文 [The Database State Machine Approach (PDF)](http://www.inf.usi.ch/faculty/pedone/Paper/2002/2002DPDT.pdf) 或者 [本地文檔](/reference/databases/mysql/The_Database_State_Machine_Approach.pdf) 。


<!--
http://siddontang.com/2015/02/02/mysql-replication-protocol/
http://wangwei007.blog.51cto.com/68019/1893703


http://mysqlhighavailability.com/tag/mysql-group-replication/page/4/
http://mysqlhighavailability.com/mysql-group-replication-a-quick-start-guide/
http://mysqlhighavailability.com/getting-started-with-mysql-group-replication/
https://github.com/sjmudd/mysql-x-protocol-specification/blob/master/mysql-x-protocol-specification-draft.txt

-->

{% highlight text %}
{% endhighlight %}
