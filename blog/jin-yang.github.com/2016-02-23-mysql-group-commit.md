---
title: MySQL 组提交
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,binlog,innodb,组提交
description: 组提交 (group commit) 是为了优化写日志时的刷磁盘问题，从最初只支持 InnoDB redo log 组提交，到 5.6 官方版本同时支持 redo log 和 binlog 组提交，大大提高了 MySQL 的事务处理性能。下面将以 InnoDB 存储引擎为例，详细介绍组提交在各个阶段的实现原理。
---

组提交 (group commit) 是为了优化写日志时的刷磁盘问题，从最初只支持 InnoDB redo log 组提交，到 5.6 官方版本同时支持 redo log 和 binlog 组提交，大大提高了 MySQL 的事务处理性能。

下面将以 InnoDB 存储引擎为例，详细介绍组提交在各个阶段的实现原理。

<!-- more -->

## 简介

自 5.1 之后，binlog 和 innodb 采用类似两阶段提交的方式，不过不支持 group commit；在 5.6 中，将 binlog 的 commit 阶段分为三个阶段：flush stage、sync stage 以及 commit stage。

这三个阶段中，每个阶段都会去维护一个队列，各个列表的定义如下。

{% highlight cpp %}
Mutex_queue m_queue[STAGE_COUNTER];
{% endhighlight %}

如上，每个阶段都在维护一个队列，第一个进入该队列的作为 leader 线程，否则作为 follower 线程；leader 线程会收集 follower 的事务，并负责做 sync，follower 线程等待 leader 通知操作完成。

尽管维护了三个队列，但队列中所有的 THD 实际上都是通过 next_to_commit 连接起来。binlog 在事务提交阶段，也就是在 MYSQL_BIN_LOG::ordered_commit() 函数中，开始 3 个阶段的流程。

接下来，看看 MySQL 中事务是如何提交的。



## 事务提交

接下来，看看 InnoDB 和 binlog 提交的流程。

### 二阶段提交

详细介绍下二阶段提交的过程。

#### 未开启binlog时

InnoDB 通过 redo 和 undo 日志来恢复数据库 (safe crash recovery)，当数据恢复时，通过 redo 日志将所有已经在存储引擎内部提交的事务应用 redo log 恢复，所有已经 prepared 但是没有 commit 的事务则会通过 undo log 做回滚。

然后客户端连接时就能看到已经提交的数据存在数据库内，未提交被回滚地数据需要重新执行。

#### 开启binlog时

为了保证存储引擎和 MySQL 的 binlog 保持一致，引入二阶段提交 (two phase commit, 2pc) 。

因为备库通过 binlog 重放主库提交的事务，假设主库存储引擎已经提交而 binlog 没有保持一致，则会使备库数据丢失造成主备数据不一致。

#### 二阶段提交

如下是二阶段提交流程。

![group commit 2pc]({{ site.url }}/images/databases/mysql/groupcommit_2pc.png "group commit 2pc"){: .pull-center }

详细执行流程为：

1. InnoDB 的事务 Prepare 阶段，即 SQL 已经成功执行并生成 redo 和 undo 的内存日志；

2. binlog 提交，通过 write() 将 binlog 内存日志数据写入文件系统缓存；

3. fsync() 将 binlog 文件系统缓存日志数据永久写入磁盘；

4. InnoDB 内部提交，commit 阶段在存储引擎内提交，通过 innodb_flush_log_at_trx_commit 参数控制，使 undo 和 redo 永久写入磁盘。

开启 binlog 的 MySQL 在崩溃恢复 (crash recovery) 时：

* 在 prepare 阶段崩溃，恢复时该事务未写入 binlog 且 InnoDB 未提交，该事务直接回滚；

* 在 binlog 已经 fsync() 永久写入 binlog，但 InnoDB 未来得及 commit 时崩溃；恢复时，将会从 binlog 中获取提交的信息，重做该事务并提交，使 InnoDB 和 binlog 始终保持一致。

以上提到单个事务的二阶段提交过程，能够保证 InnoDB 和 binlog 保持一致，但是在并发的情况下怎么保证存储引擎和 binlog 提交的顺序一致？当并发提交的时，如果两者不一致会造成什么影响？

### 组提交异常

首先看看，对于上述的问题，当并发提交的时，如果两者不一致会造成什么影响？

![group commit 2pc concurrency bug]({{ site.url }}/images/databases/mysql/groupcommit_2pc_concurrency_bug.png "group commit 2pc"){: .pull-center }

如上所示，事务按照 T1、T2、T3 顺序开始执行，并依相同次序按照写入 binlog 日志文件系统缓存，调用 fsync() 进行一次组提交，将日志文件永久写入磁盘。

但是存储引擎提交的顺序为 T2、T3、T1，当 T2、T3 提交事务之后做了一个 On-line 的备份程序新建一个 slave 来做复制；而搭建备库时，```CHANGE MASTER TO``` 的日志偏移量在 T3 事务之后。

那么事务 T1 在备机恢复 MySQL 数据库时，发现 T1 未在存储引擎内提交，那么在恢复时，T1 事务就会被回滚，此时就会导致主备数据不一致。

**结论**：需要保证 binlog 的写入顺序和 InnoDB 事务提交顺序一致，用于 xtrabackup 备份恢复。

### 早期解决方案

早期，使用 prepare_commit_mutex 保证顺序，只有当上一个事务 commit 后释放锁，下个事务才可以进行 prepara 操作，并且在每个事务过程中 binlog 没有 fsync() 的调用。

![group commit 2pc concurrency mutex]({{ site.url }}/images/databases/mysql/groupcommit_2pc_concurrency_mutex.png "group commit 2pc concurrency mutex"){: .pull-center }

由于内存数据写入磁盘的开销很大，如果频繁 fsync() 把日志数据永久写入磁盘，数据库的性能将会急剧下降。为此提供 sync_binlog 参数来设置多少个 binlog 日志产生的时候调用一次 fsync() 把二进制日志刷入磁盘来提高整体性能，该参数的设置作用为：

* sync_binlog=0，二进制日志 fsync() 的操作基于系统自动执行。

* sync_binlog=1，每次事务提交都会调用 fsync()，最大限度保证数据安全，但影响性能。

* sync_binlog=N，当数据库崩溃时，可能会丢失 N-1 个事务。

prepare_commit_mutex 的锁机制会严重影响高并发时的性能，而且 binlog 也无法执行组提交。

### 改进方案

接下来，看看如何保证 binlog 写入顺序和存储引擎提交顺序是一致的，并且能够进行 binlog 的组提交？5.6 引入了组提交，并将提交过程分成 Flush stage、Sync stage、Commit stage 三个阶段。

这样，事务提交时分为了如下的阶段：

{% highlight text %}
InnoDB, Prepare
    SQL已经成功执行并生成了相应的redo和undo内存日志；
Binlog, Flush Stage
    所有已经注册线程都将写入binlog缓存；
Binlog, Sync Stage
    binlog缓存将sync到磁盘，sync_binlog=1时该队列中所有事务的binlog将永久写入磁盘；
InnoDB, Commit stage
    leader根据顺序调用存储引擎提交事务；
{% endhighlight %}

每个 Stage 阶段都有各自的队列，从而使每个会话的事务进行排队，提高并发性能。

如果当一个线程注册到一个空队列时，该线程就做为该队列的 leader，后注册到该队列的线程均为 follower，后续的操作，都由 leader 控制队列中 follower 行为。

leader 同时会带领当前队列的所有 follower 到下一个 stage 去执行，当遇到下一个 stage 为非空队列时，leader 会变成 follower 注册到此队列中；**注意**：follower 线程绝不可能变成 leader 。


<!--
当一组事务在进行Commit阶段时，其他新的事务可以进行Flush阶段，从而使group commit不断生效。那么为了提高group commit中一组队列的事务数量，MySQL用binlog_max_flush_queue_time来控制在Flush stage中的等待时间，让Flush队列在此阶段多等待一些时间来增加这一组事务队列的数量使该队列到Sync阶段可以一次fysn()更多的事务。

MySQL 5.7 Parallel replication实现主备多线程复制基于主库Binary Log Group Commit, 并在Binary log日志中标识同一组事务的last_commited=N和该组事务内所有的事务提交顺序。为了增加一组事务内的事务数量提高备库组提交时的并发量引入了binlog_group_commit_sync_delay=N 和binlog_group_commit_sync_no_delay_count=N (注：binlog_max_flush_queue_time 在MySQL的5.7.9及之后版本不再生效)参数，MySQL等待binlog_group_commit_sync_delay毫秒直到达到binlog_group_commit_sync_no_delay_count事务个数时，将进行一次组提交。
-->



### 配置参数

与 binlog 组提交相关的参数主要包括了如下两个参数。

#### binlog_max_flush_queue_time

单位为微妙，用于从 flush 队列中取事务的超时时间，这主要是防止并发事务过高，导致某些事务的 RT 上升，详细的内容可以查看函数 ```MYSQL_BIN_LOG::process_flush_stage_queue()``` 。

**注意**：该参数在 5.7 之后已经取消了。

#### binlog_order_commits

当设置为 0 时，事务可能以和 binlog 不同的顺序提交，其性能会有稍微提升，但并不是特别明显.


<!--
The configuration variables for artificial delay are binlog-group-commit-sync-delay (delay in microseconds) and binlog-group-commit-sync-no-delay-count (number of transactions to wait for before deciding to abort waiting).
-->





## 源码解析

binlog 的组提交是通过 Stage_manager 管理，其中比较核心内容如下。

{% highlight cpp %}
class Stage_manager {
  public:
    enum StageID {         // binlog的组提交包括了三个阶段
      FLUSH_STAGE,
      SYNC_STAGE,
      COMMIT_STAGE,
      STAGE_COUNTER
    };
  private:
    Mutex_queue m_queue[STAGE_COUNTER];
};
{% endhighlight %}

组提交 (Group Commit) 三阶段流程，详细实现如下。

{% highlight text %}
MYSQL_BIN_LOG::ordered_commit()           ← 执行事务顺序提交，binlog group commit的主流程
 |
 |-#########>>>>>>>>>                     ← 进入Stage_manager::FLUSH_STAGE阶段
 |-change_stage(..., &LOCK_log)
 | |-stage_manager.enroll_for()           ← 将当前线程加入到m_queue[FLUSH_STAGE]中
 | |
 | |                                      ← (follower)返回true
 | |-mysql_mutex_lock()                   ← (leader)对LOCK_log加锁，并返回false
 |
 |-finish_commit()                        ← (follower)对于follower则直接返回
 | |-ha_commit_low()
 |
 |-process_flush_stage_queue()            ← (leader)对于follower则直接返回
 | |-fetch_queue_for()                    ← 通过stage_manager获取队列中的成员
 | | |-fetch_and_empty()                  ← 获取元素并清空队列
 | |-ha_flush_log()
 | |-flush_thread_caches()                ← 对于每个线程做该操作
 | |-my_b_tell()                          ← 判断是否超过了max_bin_log_size，如果是则切换binlog文件
 |
 |-flush_cache_to_file()                  ← (follower)将I/O Cache中的内容写到文件中
 |-RUN_HOOK()                             ← 调用HOOK函数，也就是binlog_storage->after_flush()
 |
 |-#########>>>>>>>>>                     ← 进入Stage_manager::SYNC_STAGE阶段
 |-change_stage()
 |-sync_binlog_file()
 | |-mysql_file_sync()
 |   |-my_sync()
 |     |-fdatasync()                      ← 调用系统API写入磁盘，也可以是fsync()
 |
 |-#########>>>>>>>>>                     ← 进入Stage_manager::COMMIT_STAGE阶段
 |-change_stage()                         ← 该阶段会受到binlog_order_commits参数限制
 |-process_commit_stage_queue()           ← 会遍厉所有线程，然后调用如下存储引擎接口
 | |-ha_commit_low()
 |   |-ht->commit()                       ← 调用存储引擎handlerton->commit()
 |   |                                    ← ### 注意，实际调用如下的两个函数
 |   |-binlog_commit()
 |   |-innobase_commit()
 |-process_after_commit_stage_queue()     ← 提交之后的后续处理，例如semisync
 | |-RUN_HOOK()                           ← 调用transaction->after_commit
 |
 |-stage_manager.signal_done()            ← 通知其它线程事务已经提交
 |
 |-finish_commit()
{% endhighlight %}

在 enroll_for() 函数中，刚添加的线程如果是队列的第一个线程，就将其设置为 leader 线程；否则就是 follower 线程，此时线程会睡眠，直到被 leader 唤醒 (m_cond_done) 。

注意，binlog_max_flush_queue_time 参数已经取消。

### commit stage

如上所述，commit 阶段会受到参数 binlog_order_commits 的影响，当该参数关闭时，会直接释放 LOCK_sync ，各个 session 自行进入 InnoDB commit 阶段，这样不会保证 binlog 和事务 commit 的顺序一致。

当然，如果你不关注两者的一致性，那么可以关闭这个选项来稍微提高点性能；当打开了上述的参数，才会进入 commit stage 。






<!--

## 小结

MySQL 的 group commit 只有两次 sync，即在 prepare 阶段的 redo-log sync，以及 sync binlog 文件（双一配置），为了保证rotate时，所有前一个binlog的事件的redo log都被刷到磁盘，会在函数new_file_impl中调用如下代码段：

## 参考

http://blog.csdn.net/woqutechteam/article/details/51178803
http://www.cnblogs.com/cchust/p/4439107.html
-->

{% highlight text %}
{% endhighlight %}
