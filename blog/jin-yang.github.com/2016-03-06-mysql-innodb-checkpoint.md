---
title: InnoDB Checkpoint
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,checkpoint
description: 如果 redo log 可以无限地增大，同时缓冲池也足够大，是不是就意味着可以不将缓冲池中的脏页刷新回磁盘上？宕机时，完全可以通过 redo log 来恢复整个数据库系统中的数据。显然，上述的前提条件是不满足的，这也就引入了 checkpoint 技术。在这篇文章里，就简单介绍下 MySQL 中的实现。
---

如果 redo log 可以无限地增大，同时缓冲池也足够大，是不是就意味着可以不将缓冲池中的脏页刷新回磁盘上？宕机时，完全可以通过 redo log 来恢复整个数据库系统中的数据。

显然，上述的前提条件是不满足的，这也就引入了 checkpoint 技术。

在这篇文章里，就简单介绍下 MySQL 中的实现。

<!-- more -->

## 简介

Checkpoint (检查点) 的目的是为了解决以下几个问题：1、缩短数据库的恢复时间；2、缓冲池不够用时，将脏页刷新到磁盘；3、重做日志不可用时，刷新脏页。

* 数据库宕机时，不需要重做所有的日志，因为 Checkpoint 之前的脏页都已经刷新回磁盘，只需对 Checkpoint 后的 redo log 进行恢复即可，这样就大大缩短了恢复的时间。

* 当缓冲池不够用时，会根据 LRU 算法淘汰最近最少使用的页，若此页为脏页，那么需要强制执行 Checkpoint，将脏页刷回磁盘。

* 当前数据库对 redo log 的设计都是循环使用的，为了防止被覆盖，必须强制 Checkpoint，将缓冲池中的页至少刷新到当前 redo log 的位置。

InnoDB 通过 Log Sequence Number, LSN 来标记版本，这是 8 字节的数字，每个页有 LSN，重做日志中也有 LSN，Checkpoint 也有 LSN，这个是联系三者的关键变量。

LSN 当前状态可以通过如下命令查看。

{% highlight text %}
mysql> SHOW ENGINE INNODB STATUS\G
---
LOG
---
Log sequence number 293590838           LSN1事务创建时一条日志
Log flushed up to   293590838
Pages flushed up to 293590838
Last checkpoint at  293590829
0 pending log flushes, 0 pending chkp writes
1139 log i/o's done, 0.00 log i/o's/second
{% endhighlight %}

### 分类

通常有两种 Checkpoint，分别为：Sharp Checkpoint、Fuzzy Checkpoint；前者在正常关闭数据库时使用，会将所有脏页刷回磁盘；后者，会在运行时使用，用于部分脏页的刷新。

Checkpoint 所做的事情无外乎是将缓冲池中的脏页刷回到磁盘，不同之处在于每次刷新多少页到磁盘，每次从哪里取脏页，以及什么时间触发 Checkpoint。

#### Master Thread Checkpoint

InnoDB 的主线程以每秒或每十秒的速度从缓冲池的脏页列表中刷新一定比例的页回磁盘，这个过程是异步的，此时 InnoDB 可以进行其他的操作，用户查询线程不会阻塞。

#### FLUSH_LRU_LIST Checkpoint

InnoDB 要保证 BP 中有足够空闲页，在 1.1.x 之前，该操作发生在用户查询线程中，显然这会阻塞用户的查询。如果没有足够空闲页，需要将 LRU 列表尾端的页移除，如果有脏页，那么就需要进行 Checkpoint，因为这些页来自 LRU 列表，所以称为 FLUSH_LRU_LIST Checkpoint 。

MySQL-5.6 (InnoDB-1.2.x) 版本开始，这个检查被放在了一个单独的 Page Cleaner 线程中进行，而且用户可以通过参数 ```innodb_lru_scan_depth``` 控制 LRU 列表中可用页的数量。

{% highlight text %}
mysql>  SHOW GLOBAL VARIABLES LIKE 'innodb_lru_scan_depth';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| innodb_lru_scan_depth | 1024  |
+-----------------------+-------+
1 row in set (0.01 sec)
{% endhighlight %}

#### Async/Sync Flush Checkpoint

是指重做日志文件不可用时，需要强制将脏页列表中的一些页刷新回磁盘，而此时脏页是从脏页列表中选取的，这可以保证重做日志文件可循环使用。

在 InnoDB 1.2.X 版本之前，Async Flush Checkpoint 会阻塞发现问题的用户查询线程，Sync Flush Checkpoint 会阻塞所有查询线程；InnoDB 1.2.X 之后放到单独的 Page Cleaner Thread。



<!--
若将已经写入到重做日志的 LSN 记为 redo_lsn，将已经刷新回磁盘最新页的 LSN 记为 checkpoint_lsn，则可定义：

checkpoint_age = redo_lsn - checkpoint_lsn

再定义以下的变量：

async_water_mark = 75% * total_redo_log_file_size

sync_water_mark = 90% * total_redo_log_file_size

若每个重做日志文件的大小为1GB，并且定义了两个重做日志文件，则重做日志文件的总大小为2GB。那么async_water_mark=1.5GB，sync_water_mark=1.8GB。则：

当checkpoint_age<async_water_mark时，不需要刷新任何脏页到磁盘；

当async_water_mark<checkpoint_age<sync_water_mark时触发Async Flush，从Flush列表中刷新足够的脏页回磁盘，使得刷新后满足checkpoint_age<async_water_mark；

checkpoint_age>sync_water_mark这种情况一般很少发生，除非设置的重做日志文件太小，并且在进行类似LOAD DATA的BULK INSERT操作。此时触发Sync Flush操作，从Flush列表中刷新足够的脏页回磁盘，使得刷新后满足checkpoint_age<async_water_mark。

可见，Async/Sync Flush Checkpoint是为了保证重做日志的循环使用的可用性。在InnoDB 1.2.x版本之前，Async Flush Checkpoint会阻塞发现问题的用户查询线程，而Sync Flush Checkpoint会阻塞所有的用户查询线程，并且等待脏页刷新完成。从InnoDB 1.2.x版本开始——也就是MySQL 5.6版本，这部分的刷新操作同样放入到了单独的Page Cleaner Thread中，故不会阻塞用户查询线程。

MySQL官方版本并不能查看刷新页是从Flush列表中还是从LRU列表中进行Checkpoint的，也不知道因为重做日志而产生的Async/Sync Flush的次数。但是InnoSQL版本提供了方法，可以通过命令SHOW ENGINE INNODB STATUS来观察，如：

{% highlight text %}
mysql> show engine innodb status \G
BUFFER POOL AND MEMORY
----------------------
Total memory allocated 2058485760; in additional pool allocated 0
Dictionary memory allocated 913470
Buffer pool size   122879
Free buffers       79668
Database pages     41957
Old database pages 15468
Modified db pages  0
Pending reads 0
Pending writes: LRU 0, flush list 0, single page 0
Pages made young 15032929, not young 0
0.00 youngs/s, 0.00 non-youngs/s
Pages read 15075936, created 366872, written 36656423
0.00 reads/s, 0.00 creates/s, 0.90 writes/s
Buffer pool hit rate 1000 / 1000, young-making rate 0 / 1000 not 0 / 1000
Pages read ahead 0.00/s, evicted without access 0.00/s, Random read ahead 0.00/s
LRU len: 41957, unzip_LRU len: 0
I/O sum[39]:cur[0], unzip sum[0]:cur[0]
{% endhighlight %}


https://www.percona.com/blog/2011/04/04/innodb-flushing-theory-and-solutions/
-->


#### Dirty Page too much Checkpoint

即脏页数量太多时，InnoDB 会强制进行 Checkpoint 。

{% highlight text %}
mysql> SHOW GLOBAL VARIABLES LIKE 'innodb_max_dirty_pages_pct';
+----------------------------+-----------+
| Variable_name              | Value     |
+----------------------------+-----------+
| innodb_max_dirty_pages_pct | 75.000000 |
+----------------------------+-----------+
1 row in set (0.03 sec)
{% endhighlight %}

也即当缓冲池中脏页的数量占据 75% 时，强制进行 Checkpoint，刷新一部分的脏页到磁盘，其目的还是为了保证缓冲池中有足够可用的空闲页。

### CheckPoint 机制

在 Innodb 每次都取最老的 modified page 对应的 LSN，并将此脏页的 LSN 作为 Checkpoint 点记录到日志文件，意思就是 "此 LSN 之前对应的日志和数据都已经刷新到磁盘" 。

当 MySQL 启动做崩溃恢复时，会从 last checkpoint 对应的 LSN 开始扫描 redo log ，并将其应用到 buffer pool，直到 last checkpoint 对应的 LSN 等于 log flushed up to 对应的 LSN，则恢复完成。

如下是整个 redo log 的生命周期。

![innodb checkpoint lsn]({{ site.url }}/images/databases/mysql/checkpoint-lsn.png "innodb checkpoint lsn"){: .pull-center }

InnoDB 的一条事务日志共经历 4 个阶段：

1. 创建阶段 (log sequence number, LSN1)：事务创建一条日志，当前系统 LSN 最大值，新的事务日志 LSN 将在此基础上生成，也就是 LSN1+新日志的大小；

2. 日志刷盘 (log flushed up to, LSN2)：当前已经写入日志文件做持久化的 LSN；

3. 数据刷盘 (oldest modified data log, LSN3)：当前最旧的脏页数据对应的 LSN，写 Checkpoint 的时候直接将此 LSN 写入到日志文件；

4. 写CKP (last checkpoint at, LSN4)：当前已经写入 Checkpoint 的 LSN，也就是上次的写入；

对于系统来说，以上 4 个 LSN 是递减的，即： LSN1>=LSN2>=LSN3>=LSN4 。如上所述，LSN 当前状态可以通过如下命令查看。

{% highlight text %}
mysql> SHOW ENGINE INNODB STATUS\G
---
LOG
---
Log sequence number 293590838           LSN1事务创建时一条日志
Log flushed up to   293590838
Pages flushed up to 293590838
Last checkpoint at  293590829
0 pending log flushes, 0 pending chkp writes
1139 log i/o's done, 0.00 log i/o's/second
{% endhighlight %}

如上的信息是在 log_print() 函数中打印。

{% highlight cpp %}
void log_print( FILE* file)
{
    ... ...
    fprintf(file,
        "Log sequence number " LSN_PF "\n"
        "Log flushed up to   " LSN_PF "\n"
        "Pages flushed up to " LSN_PF "\n"
        "Last checkpoint at  " LSN_PF "\n",
        log_sys->lsn,
        log_sys->flushed_to_disk_lsn,
        log_buf_pool_get_oldest_modification(),
        log_sys->last_checkpoint_lsn);
    ... ...
}
{% endhighlight %}

### 日志保护机制

InnoDB 中 LSN 是单调递增的，而日志文件大小却是固定的，所以在写入的时候通过取余来计算偏移量，这样存在两个 LSN 写入到同一位置的可能，如果日志被覆盖，而数据也没有刷盘，一旦宕机，数据就丢失了。

为此，InnoDB 实现了一套日志保护机制，详细实现如下。

![checkpoint redo buffer protect]({{ site.url }}/images/databases/mysql/checkpoint-redo-buffer-protect.png "checkpoint redo buffer protect"){: .pull-center }

首先，明确下概念，上述的 buf 是指 redo log buffer，而 ckp 实际上与 buffer pool 相关，也就是脏页的刷脏。上述直线表示 redo log 的空间，会乘 0.9 的安全系数。

* Ckp age (LSN1- LSN4) 还没有做 Checkpoint 的日志范围，若超过日志空间，说明被覆盖的日志可能还没有刷到磁盘，而其 BP 中对应的数据 (脏页) 肯定没有刷到磁盘上；
* Buf age (LSN1- LSN3) 脏页对应的日志还没有刷盘的范围，若超过日志空间，说明被覆盖的日志及其 BP 中对应数据肯定还没有刷到磁盘；

<!--
* Buf async (日志空间 * 7/8) 强制将 Buf age-Buf async 的脏页刷盘，此时事务还可以继续执行，所以为async，对事务的执行速度没有直接影响（有间接影响，例如CPU和磁盘更忙了，事务的执行速度可能受到影响）
* Buf sync    日志空间大小 * 15/16    强制将2*(Buf age-Buf async)的脏页刷盘，此时事务停止执行，所以为sync，由于有大量的脏页刷盘，因此阻塞的时间比Ckp sync要长。
* Ckp async   日志空间大小 * 31/32    强制写Checkpoint，此时事务还可以继续执行，所以为async，对事务的执行速度没有影响（间接影响也不大，因为写Checkpoint的操作比较简单）
* Ckp sync    日志空间大小 * 64/64    强制写Checkpoint，此时事务停止执行，所以为sync，但由于写Checkpoint的操作比较简单，即使阻塞，时间也很短
-->

当事务执行速度大于刷脏速度时，Ckp age和Buf age (innodb_flush_log_at_trx_commit!=1时) 都会逐步增长，当达到 async 点的时候，强制进行写 redo-log 或者写 Checkpoint，如果这样做还是赶不上事务执行的速度，则为了避免数据丢失，到达 sync 点的时候，会阻塞其它所有的事务，专门进行 redo-log 刷盘或者写 Checkpoint。

也就是说，只要事务执行速度大于脏页刷盘速度，最终都会触发日志保护机制，进而将事务阻塞，导致 MySQL 操作挂起。

<!--
由于写Checkpoint本身的操作相比写脏页要简单，耗费时间也要少得多，且Ckp sync点在Buf sync点之后，因此绝大部分的阻塞都是阻塞在了Buf sync点，这也是当事务阻塞的时候，IO很高的原因，因为这个时候在不断的刷脏页数据到磁盘。例如如下截图的日志显示了很多事务阻塞在了Buf sync点：
-->

## 源码解析

### 临界范围计算

如下是相关的变量以及临界值的计算函数。

{% highlight text %}
----- 相关变量
innodb_log_buffer_size         = 16777216 = 16M
innodb_log_file_size           = 50331648 = 48M
innodb_log_files_in_group      = 2
innodb_flush_log_at_trx_commit = 1
innodb_thread_concurrency      = 0

----- 计算临界函数调用栈
innobase_start_or_create_for_mysql()
 |-log_group_init()
   |-log_calc_max_ages()                计算临界范围
{% endhighlight %}

接下来看看临界值是如何计算的。

{% highlight cpp %}
bool log_calc_max_ages(void)
{
    log_mutex_enter();
    group = UT_LIST_GET_FIRST(log_sys->log_groups);

    // 设置redo-log的最大磁盘空间，也就是64-bits正整数的最大值
    smallest_capacity = LSN_MAX;

    // 5.7实际只支持一个分组，获取的是除了头(LOG_FILE_HDR_SIZE)之外的总redo-log空间大小
    while (group) {
        if (log_group_get_capacity(group) < smallest_capacity) {
            smallest_capacity = log_group_get_capacity(group);
        }
        group = UT_LIST_GET_NEXT(log_groups, group);
    }

    // 实际真正可以使用的空间需要乘以一个安全系数0.9
    smallest_capacity = smallest_capacity - smallest_capacity / 10;

    // 为每个OS线程预留一部分存储空间
    free = LOG_CHECKPOINT_FREE_PER_THREAD * (10 + srv_thread_concurrency)
        + LOG_CHECKPOINT_EXTRA_FREE;
    if (free >= smallest_capacity / 2) {  // 需要预留足够的内存空间
        success = false;
        goto failure;
    } else {
        margin = smallest_capacity - free;
    }

    // 好吧，再预留一部分内存空间
    margin = margin - margin / 10;  /* Add still some extra safety */
    log_sys->log_group_capacity = smallest_capacity;

    // 1-1/8=7/8=0.875
    log_sys->max_modified_age_async = margin
        - margin / LOG_POOL_PREFLUSH_RATIO_ASYNC;
    // 1-1/16=15/16=0.9375
    log_sys->max_modified_age_sync = margin
        - margin / LOG_POOL_PREFLUSH_RATIO_SYNC;

    // 1-1/32=31/32=0.96875
    log_sys->max_checkpoint_age_async = margin - margin
        / LOG_POOL_CHECKPOINT_RATIO_ASYNC;
    log_sys->max_checkpoint_age = margin;

failure:
    log_mutex_exit();
    return(success);
}
{% endhighlight %}

关于边界划分可以简单查看下图。

![checkpoint max ages]({{ site.url }}/images/databases/mysql/innodb-checkpoint-max-ages.png "checkpoint max ages"){: .pull-center width="90%"}


### 边界检查

对于 ```max_modified_age_async``` 变量，也就是异步刷新，会在 page cleaner 线程中检查<!--，在此暂时就不做过多介绍了，在后面会做详细介绍 -->。

{% highlight text %}
#define PCT_IO(p) ((ulong) (srv_io_capacity * ((double) (p) / 100.0)))

buf_flush_page_cleaner_coordinator()            ← 该函数基本上每秒调用一次
 |-buf_flush_page_cleaner_coordinator()
   |-page_cleaner_flush_pages_recommendation()
     |-log_get_lsn()                            ← 获取当前lsn，也就是log_sys->lsn
     |-af_get_pct_for_dirty()                   ← 是否需要刷新多个页，返回IO-Capacity的百分比
     | |-buf_get_modified_ratio_pct()
     |   |-buf_get_total_list_len()
     |
     |-af_get_pct_for_lsn()                     ← 计算是否需要进行异步刷redo-log，返回IO-Capacity的百分比
     | |-log_get_max_modified_age_async()       ← 获取max_modified_age_async
     |
     |-ut_max()                                 ← 获取上述两个返回值的最大值
{% endhighlight %}

<!--
至于异步刷脏，log_sys->max_modified_age_async被封装在函数log_get_max_modified_age_async中， 被函数af_get_pct_for_lsn。显而易见，异步刷脏是由page cleaner线程来完成的。

在函数af_get_pct_for_lsn中，根据当前的LSN，计算需要以IO capacity的百分之几来刷脏

当当前lsn-buf_pool_get_oldest_modification()超过log_sys->max_modified_age_async时：

age = log_sys->lsn – buf_pool_get_oldest_modification()
lsn_age_factor = (age *100)/log_sys->max_modified_age_async )

返回值为：
pct =   [
                 (innodb_io_capacity_max/innodb_io_capacity)
                 *
                 (lsn_age_factor * sqrt((double)lsn_age_factor))
          ] /7.5


另外，如果参数innodb_adaptive_flushing设置为OFF，且没有超过log_get_max_modified_age_async()的话，直接返回0

Page cleaner线程会同时根据LSN 和脏页比例来获取pct，并取其中的最大值。
-->


除了 ```max_modified_age_async``` 变量之外，其它相关的变量都会在 ```log_checkpoint_margin()``` 函数中进行比较，详细内容可以直接查看如下函数。

{% highlight cpp %}
static void log_checkpoint_margin(void)
{
    log_t*      log     = log_sys;
    lsn_t       age;
    lsn_t       checkpoint_age;
    ib_uint64_t advance;
    lsn_t       oldest_lsn;
    bool        success;
loop:
    advance = 0;

    log_mutex_enter();
    ut_ad(!recv_no_log_write);

    // 判断是否需要执行flush或者checkpoint，不需要则直接返回
    if (!log->check_flush_or_checkpoint) {
        log_mutex_exit();
        return;
    }

    // 找出当前所有buffer pool实例中最老的LSN，实际上直接读取每个flush_list的尾部即可
    oldest_lsn = log_buf_pool_get_oldest_modification();

    // 如果计算的age大于max_modified_age_sync，则需要做一次同步刷新
    age = log->lsn - oldest_lsn;
    if (age > log->max_modified_age_sync) {
        /* A flush is urgent: we have to do a synchronous preflush */
        advance = age - log->max_modified_age_sync;
    }

    // 计算checkpoint_age，并判断是否需要做checkpoint以及是否需要同步
    checkpoint_age = log->lsn - log->last_checkpoint_lsn;
    if (checkpoint_age > log->max_checkpoint_age) {
        /* A checkpoint is urgent: we do it synchronously */
        checkpoint_sync = true;
        do_checkpoint = true;
    } else if (checkpoint_age > log->max_checkpoint_age_async) {
        /* A checkpoint is not urgent: do it asynchronously */
        do_checkpoint = true;
        checkpoint_sync = false;
        log->check_flush_or_checkpoint = false;
    } else {
        do_checkpoint = false;
        checkpoint_sync = false;
        log->check_flush_or_checkpoint = false;
    }
    log_mutex_exit();

    if (advance) {
        lsn_t   new_oldest = oldest_lsn + advance;
        // 需要同步刷新，则将LSN推进到新的LSN位置
        success = log_preflush_pool_modified_pages(new_oldest);

        // 如果失败说明有其它的线程在处理
        /* If the flush succeeded, this thread has done its part
        and can proceed. If it did not succeed, there was another
        thread doing a flush at the same time. */
        if (!success) {
            log_mutex_enter();
            log->check_flush_or_checkpoint = true;
            log_mutex_exit();
            goto loop;
        }
    }

    if (do_checkpoint) {
        log_checkpoint(checkpoint_sync, FALSE);
        if (checkpoint_sync) {
            goto loop;
        }
    }
}
{% endhighlight %}

<!--
另外，这几个变量在函数log_close中会被用到，它会去做一件重要的事情：设置log_sys->check_flush_or_checkpoint。
-->

在用户线程中，会调用 ```log_free_check()``` 函数检查是否需要将日志刷新到磁盘。

{% highlight text %}
log_free_check()
 |-log_check_margins()
   |-log_write_up_to()
{% endhighlight %}

在 ```log_check_margins()``` 函数中，会检查 ```log_sys->buf_free > log->max_buf_free```，如果成立则会执行日志刷盘操作。


### 检查点写入

一般会通过调用 ```log_checkpoint()``` 函数完成 checkpoint 的写入，需要注意的是，该函数中只会完成 checkpoint 的写入，并不会刷脏页。

当然也可以调用 ```log_make_checkpoint_at()``` 完成刷脏以及 checkpoint 的写入。

{% highlight text %}
log_make_checkpoint_at()
 |-log_preflush_pool_modified_pages()
 |-log_checkpoint()                         ← 并不从BP中刷脏页，只检查BP中的最大LSN，然后刷新到磁盘
   |-log_mutex_enter()                      ← 持有log_sys->mutex锁
   |-log_buf_pool_get_oldest_modification()
   | |-buf_pool_get_oldest_modification()   ← 遍厉所有BP实例，获取最大lsn，之前都已经写入磁盘
   |
   |-fil_names_clear()
   | |-mtr_t::commit_checkpoint()
   |
   |-log_write_up_to()
   |
   |-log_write_checkpoint_info()
     |-log_group_checkpoint()               ← 将checkpoint信息写入redolog头部，两个写入点轮流写入
{% endhighlight %}

checkpoint 信息分别保存在 ib_logfile0 的 512 字节和 1536(3*512) 字节处，每个 checkpoint 默认大小为 512 字节，当然，其中很大一部分是空白，详细可以参考如下函数。

{% highlight cpp %}
static void log_group_checkpoint(log_group_t* group)
{
    ... ...
    buf = group->checkpoint_buf;
    memset(buf, 0, OS_FILE_LOG_BLOCK_SIZE);

    mach_write_to_8(buf + LOG_CHECKPOINT_NO, log_sys->next_checkpoint_no);
    mach_write_to_8(buf + LOG_CHECKPOINT_LSN, log_sys->next_checkpoint_lsn);

    lsn_offset = log_group_calc_lsn_offset(log_sys->next_checkpoint_lsn,
                           group);
    mach_write_to_8(buf + LOG_CHECKPOINT_OFFSET, lsn_offset);
    mach_write_to_8(buf + LOG_CHECKPOINT_LOG_BUF_SIZE, log_sys->buf_size);

    log_block_set_checksum(buf, log_block_calc_checksum_crc32(buf));

    MONITOR_INC(MONITOR_PENDING_CHECKPOINT_WRITE);

    log_sys->n_log_ios++;

    MONITOR_INC(MONITOR_LOG_IO);

    ut_ad(LOG_CHECKPOINT_1 < univ_page_size.physical());
    ut_ad(LOG_CHECKPOINT_2 < univ_page_size.physical());

    if (log_sys->n_pending_checkpoint_writes++ == 0) {
        rw_lock_x_lock_gen(&log_sys->checkpoint_lock,
                   LOG_CHECKPOINT);
    }

    /* Note: We alternate the physical place of the checkpoint info.
    See the (next_checkpoint_no & 1) below. */

    /* We send as the last parameter the group machine address
    added with 1, as we want to distinguish between a normal log
    file write and a checkpoint field write */

    fil_io(IORequestLogWrite, false,
           page_id_t(group->space_id, 0),
           univ_page_size,
           (log_sys->next_checkpoint_no & 1)
           ? LOG_CHECKPOINT_2 : LOG_CHECKPOINT_1,
           OS_FILE_LOG_BLOCK_SIZE,
           buf, (byte*) group + 1);

    ut_ad(((ulint) group & 0x1UL) == 0);
}
{% endhighlight %}

InnoDB 的 checkpoint 主要有 3 部分信息组成：

* checkpoint no<br>每次写入都会递增，用于轮流写入 redo log 的头部的两部分，可以通过该值判断那个比较新；
* checkpoint lsn<br>记录了产生该 checkpoint 时 log_sys->next_checkpoint_lsn 是 flush 的 LSN，确保在该 LSN 前面的数据页都已经落盘，不再需要通过 redo log 进行恢复；
* checkpoint offset<br>记录了该 checkpoint 产生时，redo log 在 ib_logfile 中的偏移量，通过该值就可以找到需要恢复的 redo log 开始位置。

每次在启动时，都会尝试读取两个值，并比较两者，获取较大的值。

<!--
## 参考

http://tech.uc.cn/?p=716

Checkpoint原理
http://hedengcheng.com/?p=88

关于checkpoint机制
http://www.cnblogs.com/chenpingzhao/p/5107480.html

-->


{% highlight text %}
{% endhighlight %}
