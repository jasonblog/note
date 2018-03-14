---
title: MySQL 組提交
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,binlog,innodb,組提交
description: 組提交 (group commit) 是為了優化寫日誌時的刷磁盤問題，從最初只支持 InnoDB redo log 組提交，到 5.6 官方版本同時支持 redo log 和 binlog 組提交，大大提高了 MySQL 的事務處理性能。下面將以 InnoDB 存儲引擎為例，詳細介紹組提交在各個階段的實現原理。
---

組提交 (group commit) 是為了優化寫日誌時的刷磁盤問題，從最初只支持 InnoDB redo log 組提交，到 5.6 官方版本同時支持 redo log 和 binlog 組提交，大大提高了 MySQL 的事務處理性能。

下面將以 InnoDB 存儲引擎為例，詳細介紹組提交在各個階段的實現原理。

<!-- more -->

## 簡介

自 5.1 之後，binlog 和 innodb 採用類似兩階段提交的方式，不過不支持 group commit；在 5.6 中，將 binlog 的 commit 階段分為三個階段：flush stage、sync stage 以及 commit stage。

這三個階段中，每個階段都會去維護一個隊列，各個列表的定義如下。

{% highlight cpp %}
Mutex_queue m_queue[STAGE_COUNTER];
{% endhighlight %}

如上，每個階段都在維護一個隊列，第一個進入該隊列的作為 leader 線程，否則作為 follower 線程；leader 線程會收集 follower 的事務，並負責做 sync，follower 線程等待 leader 通知操作完成。

儘管維護了三個隊列，但隊列中所有的 THD 實際上都是通過 next_to_commit 連接起來。binlog 在事務提交階段，也就是在 MYSQL_BIN_LOG::ordered_commit() 函數中，開始 3 個階段的流程。

接下來，看看 MySQL 中事務是如何提交的。



## 事務提交

接下來，看看 InnoDB 和 binlog 提交的流程。

### 二階段提交

詳細介紹下二階段提交的過程。

#### 未開啟binlog時

InnoDB 通過 redo 和 undo 日誌來恢復數據庫 (safe crash recovery)，當數據恢復時，通過 redo 日誌將所有已經在存儲引擎內部提交的事務應用 redo log 恢復，所有已經 prepared 但是沒有 commit 的事務則會通過 undo log 做回滾。

然後客戶端連接時就能看到已經提交的數據存在數據庫內，未提交被回滾地數據需要重新執行。

#### 開啟binlog時

為了保證存儲引擎和 MySQL 的 binlog 保持一致，引入二階段提交 (two phase commit, 2pc) 。

因為備庫通過 binlog 重放主庫提交的事務，假設主庫存儲引擎已經提交而 binlog 沒有保持一致，則會使備庫數據丟失造成主備數據不一致。

#### 二階段提交

如下是二階段提交流程。

![group commit 2pc]({{ site.url }}/images/databases/mysql/groupcommit_2pc.png "group commit 2pc"){: .pull-center }

詳細執行流程為：

1. InnoDB 的事務 Prepare 階段，即 SQL 已經成功執行並生成 redo 和 undo 的內存日誌；

2. binlog 提交，通過 write() 將 binlog 內存日誌數據寫入文件系統緩存；

3. fsync() 將 binlog 文件系統緩存日誌數據永久寫入磁盤；

4. InnoDB 內部提交，commit 階段在存儲引擎內提交，通過 innodb_flush_log_at_trx_commit 參數控制，使 undo 和 redo 永久寫入磁盤。

開啟 binlog 的 MySQL 在崩潰恢復 (crash recovery) 時：

* 在 prepare 階段崩潰，恢復時該事務未寫入 binlog 且 InnoDB 未提交，該事務直接回滾；

* 在 binlog 已經 fsync() 永久寫入 binlog，但 InnoDB 未來得及 commit 時崩潰；恢復時，將會從 binlog 中獲取提交的信息，重做該事務並提交，使 InnoDB 和 binlog 始終保持一致。

以上提到單個事務的二階段提交過程，能夠保證 InnoDB 和 binlog 保持一致，但是在併發的情況下怎麼保證存儲引擎和 binlog 提交的順序一致？當併發提交的時，如果兩者不一致會造成什麼影響？

### 組提交異常

首先看看，對於上述的問題，當併發提交的時，如果兩者不一致會造成什麼影響？

![group commit 2pc concurrency bug]({{ site.url }}/images/databases/mysql/groupcommit_2pc_concurrency_bug.png "group commit 2pc"){: .pull-center }

如上所示，事務按照 T1、T2、T3 順序開始執行，並依相同次序按照寫入 binlog 日誌文件系統緩存，調用 fsync() 進行一次組提交，將日誌文件永久寫入磁盤。

但是存儲引擎提交的順序為 T2、T3、T1，當 T2、T3 提交事務之後做了一個 On-line 的備份程序新建一個 slave 來做複製；而搭建備庫時，```CHANGE MASTER TO``` 的日誌偏移量在 T3 事務之後。

那麼事務 T1 在備機恢復 MySQL 數據庫時，發現 T1 未在存儲引擎內提交，那麼在恢復時，T1 事務就會被回滾，此時就會導致主備數據不一致。

**結論**：需要保證 binlog 的寫入順序和 InnoDB 事務提交順序一致，用於 xtrabackup 備份恢復。

### 早期解決方案

早期，使用 prepare_commit_mutex 保證順序，只有當上一個事務 commit 後釋放鎖，下個事務才可以進行 prepara 操作，並且在每個事務過程中 binlog 沒有 fsync() 的調用。

![group commit 2pc concurrency mutex]({{ site.url }}/images/databases/mysql/groupcommit_2pc_concurrency_mutex.png "group commit 2pc concurrency mutex"){: .pull-center }

由於內存數據寫入磁盤的開銷很大，如果頻繁 fsync() 把日誌數據永久寫入磁盤，數據庫的性能將會急劇下降。為此提供 sync_binlog 參數來設置多少個 binlog 日誌產生的時候調用一次 fsync() 把二進制日誌刷入磁盤來提高整體性能，該參數的設置作用為：

* sync_binlog=0，二進制日誌 fsync() 的操作基於系統自動執行。

* sync_binlog=1，每次事務提交都會調用 fsync()，最大限度保證數據安全，但影響性能。

* sync_binlog=N，當數據庫崩潰時，可能會丟失 N-1 個事務。

prepare_commit_mutex 的鎖機制會嚴重影響高併發時的性能，而且 binlog 也無法執行組提交。

### 改進方案

接下來，看看如何保證 binlog 寫入順序和存儲引擎提交順序是一致的，並且能夠進行 binlog 的組提交？5.6 引入了組提交，並將提交過程分成 Flush stage、Sync stage、Commit stage 三個階段。

這樣，事務提交時分為瞭如下的階段：

{% highlight text %}
InnoDB, Prepare
    SQL已經成功執行並生成了相應的redo和undo內存日誌；
Binlog, Flush Stage
    所有已經註冊線程都將寫入binlog緩存；
Binlog, Sync Stage
    binlog緩存將sync到磁盤，sync_binlog=1時該隊列中所有事務的binlog將永久寫入磁盤；
InnoDB, Commit stage
    leader根據順序調用存儲引擎提交事務；
{% endhighlight %}

每個 Stage 階段都有各自的隊列，從而使每個會話的事務進行排隊，提高併發性能。

如果當一個線程註冊到一個空隊列時，該線程就做為該隊列的 leader，後註冊到該隊列的線程均為 follower，後續的操作，都由 leader 控制隊列中 follower 行為。

leader 同時會帶領當前隊列的所有 follower 到下一個 stage 去執行，當遇到下一個 stage 為非空隊列時，leader 會變成 follower 註冊到此隊列中；**注意**：follower 線程絕不可能變成 leader 。


<!--
當一組事務在進行Commit階段時，其他新的事務可以進行Flush階段，從而使group commit不斷生效。那麼為了提高group commit中一組隊列的事務數量，MySQL用binlog_max_flush_queue_time來控制在Flush stage中的等待時間，讓Flush隊列在此階段多等待一些時間來增加這一組事務隊列的數量使該隊列到Sync階段可以一次fysn()更多的事務。

MySQL 5.7 Parallel replication實現主備多線程複製基於主庫Binary Log Group Commit, 並在Binary log日誌中標識同一組事務的last_commited=N和該組事務內所有的事務提交順序。為了增加一組事務內的事務數量提高備庫組提交時的併發量引入了binlog_group_commit_sync_delay=N 和binlog_group_commit_sync_no_delay_count=N (注：binlog_max_flush_queue_time 在MySQL的5.7.9及之後版本不再生效)參數，MySQL等待binlog_group_commit_sync_delay毫秒直到達到binlog_group_commit_sync_no_delay_count事務個數時，將進行一次組提交。
-->



### 配置參數

與 binlog 組提交相關的參數主要包括瞭如下兩個參數。

#### binlog_max_flush_queue_time

單位為微妙，用於從 flush 隊列中取事務的超時時間，這主要是防止併發事務過高，導致某些事務的 RT 上升，詳細的內容可以查看函數 ```MYSQL_BIN_LOG::process_flush_stage_queue()``` 。

**注意**：該參數在 5.7 之後已經取消了。

#### binlog_order_commits

當設置為 0 時，事務可能以和 binlog 不同的順序提交，其性能會有稍微提升，但並不是特別明顯.


<!--
The configuration variables for artificial delay are binlog-group-commit-sync-delay (delay in microseconds) and binlog-group-commit-sync-no-delay-count (number of transactions to wait for before deciding to abort waiting).
-->





## 源碼解析

binlog 的組提交是通過 Stage_manager 管理，其中比較核心內容如下。

{% highlight cpp %}
class Stage_manager {
  public:
    enum StageID {         // binlog的組提交包括了三個階段
      FLUSH_STAGE,
      SYNC_STAGE,
      COMMIT_STAGE,
      STAGE_COUNTER
    };
  private:
    Mutex_queue m_queue[STAGE_COUNTER];
};
{% endhighlight %}

組提交 (Group Commit) 三階段流程，詳細實現如下。

{% highlight text %}
MYSQL_BIN_LOG::ordered_commit()           ← 執行事務順序提交，binlog group commit的主流程
 |
 |-#########>>>>>>>>>                     ← 進入Stage_manager::FLUSH_STAGE階段
 |-change_stage(..., &LOCK_log)
 | |-stage_manager.enroll_for()           ← 將當前線程加入到m_queue[FLUSH_STAGE]中
 | |
 | |                                      ← (follower)返回true
 | |-mysql_mutex_lock()                   ← (leader)對LOCK_log加鎖，並返回false
 |
 |-finish_commit()                        ← (follower)對於follower則直接返回
 | |-ha_commit_low()
 |
 |-process_flush_stage_queue()            ← (leader)對於follower則直接返回
 | |-fetch_queue_for()                    ← 通過stage_manager獲取隊列中的成員
 | | |-fetch_and_empty()                  ← 獲取元素並清空隊列
 | |-ha_flush_log()
 | |-flush_thread_caches()                ← 對於每個線程做該操作
 | |-my_b_tell()                          ← 判斷是否超過了max_bin_log_size，如果是則切換binlog文件
 |
 |-flush_cache_to_file()                  ← (follower)將I/O Cache中的內容寫到文件中
 |-RUN_HOOK()                             ← 調用HOOK函數，也就是binlog_storage->after_flush()
 |
 |-#########>>>>>>>>>                     ← 進入Stage_manager::SYNC_STAGE階段
 |-change_stage()
 |-sync_binlog_file()
 | |-mysql_file_sync()
 |   |-my_sync()
 |     |-fdatasync()                      ← 調用系統API寫入磁盤，也可以是fsync()
 |
 |-#########>>>>>>>>>                     ← 進入Stage_manager::COMMIT_STAGE階段
 |-change_stage()                         ← 該階段會受到binlog_order_commits參數限制
 |-process_commit_stage_queue()           ← 會遍厲所有線程，然後調用如下存儲引擎接口
 | |-ha_commit_low()
 |   |-ht->commit()                       ← 調用存儲引擎handlerton->commit()
 |   |                                    ← ### 注意，實際調用如下的兩個函數
 |   |-binlog_commit()
 |   |-innobase_commit()
 |-process_after_commit_stage_queue()     ← 提交之後的後續處理，例如semisync
 | |-RUN_HOOK()                           ← 調用transaction->after_commit
 |
 |-stage_manager.signal_done()            ← 通知其它線程事務已經提交
 |
 |-finish_commit()
{% endhighlight %}

在 enroll_for() 函數中，剛添加的線程如果是隊列的第一個線程，就將其設置為 leader 線程；否則就是 follower 線程，此時線程會睡眠，直到被 leader 喚醒 (m_cond_done) 。

注意，binlog_max_flush_queue_time 參數已經取消。

### commit stage

如上所述，commit 階段會受到參數 binlog_order_commits 的影響，當該參數關閉時，會直接釋放 LOCK_sync ，各個 session 自行進入 InnoDB commit 階段，這樣不會保證 binlog 和事務 commit 的順序一致。

當然，如果你不關注兩者的一致性，那麼可以關閉這個選項來稍微提高點性能；當打開了上述的參數，才會進入 commit stage 。






<!--

## 小結

MySQL 的 group commit 只有兩次 sync，即在 prepare 階段的 redo-log sync，以及 sync binlog 文件（雙一配置），為了保證rotate時，所有前一個binlog的事件的redo log都被刷到磁盤，會在函數new_file_impl中調用如下代碼段：

## 參考

http://blog.csdn.net/woqutechteam/article/details/51178803
http://www.cnblogs.com/cchust/p/4439107.html
-->

{% highlight text %}
{% endhighlight %}
