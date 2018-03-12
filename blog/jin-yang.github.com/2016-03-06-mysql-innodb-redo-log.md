---
title: InnoDB Redo Log
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,redo log,重做日志
description:
---

当事务需要修改某条记录时，会先记录到 redo log，在此介绍下其实现。

<!-- more -->

## 简介

InnoDB 有 Buffer Pool 也就是数据库的页面缓存，对数据页的任何修改都会先在 BP 上修改，然后这样的页面将被标记为 dirty 并被放到专门的 flush list 上，后续将由 master thread 或专门的刷脏线程阶段性的将这些页面写入磁盘。

这样带来的好处是避免每次写操作都会导致大量的随机 IO，阶段性的刷脏可以将多次对页面的修改合并成一次 IO 操作，同时异步写入也降低了访问的时延。

然而，如果在脏页未刷入磁盘时，服务器或者进程非正常关闭，将会导致这些修改操作丢失，如果写入操作正在进行，甚至会由于损坏数据文件导致数据库不可用。

为了避免上述问题，InnoDB 会将所有对页面的修改操作写入一个特定的文件，也就是 redolog，并在数据库启动时从此文件进行恢复操作。这样推迟了 BP 页面的刷脏，提升了数据库的吞吐，有效的降低了访问时延；带来的问题是额外的写 redo log 操作的开销 (顺序 IO 速度较快)，以及数据库启动时恢复操作所需的时间。

接下来将结合 MySQL 代码看下 Log 文件的结构、生成过程以及数据库启动时的恢复流程。

### LSN

LSN(Log Sequence Number，日志序列号，ib_uint64_t) 保存在 log_sys.lsn 中，在 log_init() 中初始化，初始值为 LOG_START_LSN(8192) 。改值实际上对应日志文件的偏移量，新的 LSN＝旧的LSN + 写入的日志大小，在调用日志写入函数时，LSN 就一直随着写入的日志长度增加。

{% highlight text %}
#define OS_FILE_LOG_BLOCK_SIZE      512
#define LOG_START_LSN       ((lsn_t) (16 * OS_FILE_LOG_BLOCK_SIZE))
{% endhighlight %}

日志通过 log_write_low()@log/log0log.c 函数写入。

{% highlight cpp %}
void log_write_low(byte* str, ulint str_len)
{
    log_t* log = log_sys;
part_loop:
    ... ...                                        // 计算写入日志的长度
    ut_memcpy(log->buf + log->buf_free, str, len); // 将日志内容拷贝到log buffer
    ... ...
    log->lsn += len;
}
{% endhighlight %}

如上所述，LSN 是不会减小的，它是日志位置的唯一标记，在重做日志写入、checkpoint 构建和 PAGE 头里面都有 LSN。

例如当前重做日志的 LSN=2048，这时候调用 log_write_low() 写入一个长度为 700 的日志，2048 刚好是 4 个 block 长度，那么需要存储 700 长度的日志，需要两个 block(单个block只能存496个字节)，那么很容易得出新的 LSN 为。

{% highlight text %}
LSN=2048+700+2*LOG_BLOCK_HDR_SIZE(12)+LOG_BLOCK_TRL_SIZE(4)=2776
{% endhighlight %}

<!--
lsn是联系dirty page、redo log record和redo log file的纽带，每个redo log record被拷贝到内存的log buffer时会产生一个相关联的lsn，而每个页面修改时会产生一个log record，从而每个数据库的page也会有一个相关联的lsn，这个lsn记录在每个page的header字段中。为了保证WAL（Write-Ahead-Logging）要求的逻辑，dirty page要求其关联lsn的log record已经被写入log file才允许执行flush操作。<br><br>

从上面的结构定义可以看出有很多LSN相关的定义，那么这些LSN直接的关系是怎么样的呢？理解这些LSN之间的关系对理解整个重做日志系统的运作机理会有极大的信心。以下各种LSN的解释：
<ul><li>
lsn<br>
当前log系统最后写入日志时的LSN。</li><br><li>

flush_lsn<br>
redo-log buffer最后一次数据刷盘数据末尾的LSN，作为下次刷盘的起始LSN。</li><br><li>

written_to_some_lsn<br>
单个日志组最后一次日志刷盘时的起始LSN。</li><br><li>

written_to_all_lsn<br>
所有日志组最后一次日志刷盘是的起始LSN。

 last_checkpoint_lsn        最后一次建立checkpoint日志数据起始的LSN

 next_checkpoint_lsn        下一次建立checkpoint的日志    数据起始的LSN,用log_buf_pool_get_oldest_modification获得的

 archived_lsn               最后一次归档日志数据起始的LSN
 next_archived_lsn          下一次归档日志数据的其实LSN
</li></ul>



b)        文件为顺序写入，当达到最后一个文件末尾时，会从第一个文件开始顺序复用。

c)        lsn: Log Sequence Number，是一个递增的整数。 Ib_logfile中的每次写入操作都包含至少1个log，每个log都带有一个lsn。在内存page修复过程中，只有大于page_lsn的log才会被使用。

d)        lsn的保存在全局变量log_sys中。递增数值等于每个log的实际内容长度。即如果新增的一个log长度是len，则log_sys->lsn += len.

e)        ib_logfile每次写入以512（OS_FILE_LOG_BLOCK_SIZE）字节为单位。实际写入函数 log_group_write_buf (log/log0log.c)

f)         每次写盘后是否flush，由参数innodb_flush_log_at_trx_commit控制。

在 innodb 中，维护了一个全局变量 struct log_struct log_sys ，
written_to_all_lsn
n表示实际已经写盘的lsn。需要这个值是因为并非每次生成log后就写盘。
flushed_to_disk_lsn
表示刷到磁盘的lsn。需要这个值是因为并非每次写盘后就flush。
buf
待写入的内容保存在buf中
buf_size
buf的大小。由配置中innodb_log_buffer_size决定，实际大小为innodb_log_buffer_size /16k * 16k。
buf_next_to_write
buf中下一个要写入磁盘的位置
buf_free
buf中实际内容的最后位置。当buf_free> buf_next_to_write时，说明内存中还有数据未写盘。



log 采用循环写法，
有三种情况――overflow， checkpoint和commit――可以导致写盘操作
如果data buffer满了，InnoDB将最近使用的buffer写入到数据库中，但是不可能足够的快。这种情况下，页头的LSN就起作用了。第一，InnoDB检查它的LSN是否比log文件中最近的log记录的LSN大，只有当log赶上了data的时候，才会将数据写到磁盘。换句话说，数据页不会写盘，直到相应的log记录需要写盘的时候。这就是先写日志策略。
-->

### 变量设置

简单来说 InnoDB 通过两个核心参数 ```innodb_buffer_pool_size```、```innodb_log_file_size```，分别定义了数据缓存和 redolog 的大小，而后者的大小也决定了 BP 中可以有多少脏页。当然，也不能因此就增大 redolog 文件的大小，这样，可能会导致系统启动时 Crash Recovery 时间增大。

redolog 保存在 ```innodb_log_group_home_dir``` 参数指定的目录下，文件名为 ib_logfile\*；undolog 保存在共享表空间 ibdata\* 文件中。

redolog 由一组固定大小的文件组成，顺序写入，而且文件循环使用，文件名为 ib_logfileN (其中N为从0开始的数字)，可以通过 ```innodb_log_file_size``` 和 ```innodb_log_files_in_group``` 参数控制文件的大小和数目，日志总大小为两者之积。

#### innodb_log_file_size

简单来说，该变量设置时至少要保证 redo log 在峰值的时候可以容纳 1 小时的日志，当前的写入值可以通过如下方式查看。

{% highlight text %}
mysql> pager grep sequence
PAGER set to 'grep sequence'
mysql> SHOW ENGINE INNODB STATUS\G select sleep(60); SHOW ENGINE INNODB STATUS\G
Log sequence number 3836410803
1 row in set (0.06 sec)
1 row in set (1 min 0.00 sec)
Log sequence number 3838334638
1 row in set (0.05 sec)

mysql> SHOW GLOBAL STATUS LIKE 'Innodb_os_log_written';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| Innodb_os_log_written | 1024  |
+-----------------------+-------+
1 row in set (0.00 sec)
{% endhighlight %}

其实，通过上述的两个变量查看的值会有些区别，一般后者的计算值会偏大，严格来说：LSN 是在写入 log buffer 时递增；而 ```Innodb_os_log_written``` 则是在写入文件时递增的。

而将 redo log buffer 中的内容写入到文件的时候是以 512 字节为单位来写的，而且会覆盖写。

简单举例来说：

1. 事务 A 写了 100B 日志，此时 LSN 的值增加 100，而刷到磁盘时，会从某个偏移开始写入 512B，也就是说 innodb_os_log_written 增加 512 ，只是这 512B 本次有效的只有 100B 。

2. B 写了 200B 日志，此时 LSN 的值增加 200，刷磁盘时 innodb_os_log_written 又增加 512 。

也就是说最好参考 ```SHOW ENGINE INNODB STATUS``` 变量中的 ```Log sequence number``` 计算值。

<!-- https://www.percona.com/blog/2012/10/08/measuring-the-amount-of-writes-in-innodb-redo-logs/ -->

#### innodb_log_buffer_size

该参数就是用来设置 InnoDB 的 Log Buffer 大小，系统默认值为 16MB，主要作用就是缓冲 redo log 数据，增加缓存可以使大事务在提交前不用写入磁盘，从而提高写 IO 性能。

可以通过系统状态参数，查看性能统计数据来分析 Log 的使用情况：

{% highlight text %}
mysql> SHOW STATUS LIKE 'innodb_log%';
+---------------------------+-------+
| Variable_name             | Value |
+---------------------------+-------+
| Innodb_log_waits          | 0     |  由于缓存过小，导致事务必须等待的次数
| Innodb_log_write_requests | 30    |  日志写请求数
| Innodb_log_writes         | 21    |  向日志文件的物理写次数
+---------------------------+-------+
3 rows in set (0.03 sec)
{% endhighlight %}

<!--
通过这三个状态参数我们可以很清楚的看到 Log Buffer 的等待次数等性能状态。

当然，如果完全从 Log Buffer 本身来说，自然是大一些会减少更多的磁盘 IO。但是由于 Log 本身是 为了保护数据安全而产生的，而 Log 从 Buffer 到磁盘的刷新频率和控制数据安全一致的事务直接相关， 并且也有相关参数来控制（innodb_flush_log_at_trx_commit），所以关于 Log 相关的更详细的实现机 制和优化在后面的“事务优化”中再做更详细的分析，这里就不展开了。

innodb_additional_mem_pool_size 参数理解

innodb_additional_mem_pool_size 所设置的是用于存放 Innodb 的字典信息和其他一些内部结构所 需要的内存空间。所以我们的 Innodb 表越多，所需要的空间自然也就越大，系统默认值仅有 1MB。当 然，如果 Innodb 实际运行过程中出现了实际需要的内存比设置值更大的时候，Innodb 也会继续通过 OS 来申请内存空间，并且会在 MySQL 的错误日志中记录一条相应的警告信息让我们知晓。

从我个人的经验来看，一个常规的几百个 Innodb 表的 MySQL，如果不是每个表都是上百个字段的 话，20MB 内存已经足够了。当然，如果你有足够多的内存，完全可以继续增大这个值的设置。实际上， innodb_additional_mem_pool_size 参数对系统整体性能并无太大的影响，所以只要能存放需要的数据即 可，设置超过实际所需的内存并没有太大意义，只是浪费内存而已。
-->


## 文件结构

先明确下常用概念，每个日志组都会在第一个文件中有头部信息，通过 LOG_FILE_HDR_SIZE 宏定义，也就是 4*OS_FILE_LOG_BLOCK_SIZE(512) 。

redolog 写入通常是以 ```OS_FILE_LOG_BLOCK_SIZE``` (512字节，编译时设置) 为单位顺序写入文件，每个 LogBlock (512B) 包含了一个 header 段、一个 tailer 段、一组 LogRecords (多条记录组成)；每条记录都有自己的 LSN，表示从日志记录创建开始到特定的日志记录已经写入的字节数。

接着看看各个部分的定义。

### 头部信息

头部信息，在 log0log.h 文件中定义，简单看下常用的保存字段。

{% highlight text %}
LOG_HEADER_FORMAT
  之前为LOG_GROUP_ID，用于标示redolog的分组ID，但是现在只支持一个分组；
LOG_HEADER_START_LSN
  日志文件记录的初始LSN，占用8字节，注意其偏移量与老版本有所区别；
LOG_HEADER_CREATOR
  备份程序会将填写备份程序和创建时间，通常是使用xtrabackup、mysqlbackup时会填充；
LOG_CHECKPOINT_1/2
  在头文件中定义的checkpoint位置，注意只有日志组了的第一文件会有该记录，每次checkpoint都会更新这两个值；
{% endhighlight %}

块的宏偏移量同样在 log0log.h 文件中定义，其中头部包括了 LOG_BLOCK_HDR_SIZE(12) 个字节，详细如下：

{% highlight text %}
LOG_BLOCK_HDR_NO
  四字节标示这是第几个block块，该值是通过LSN计算得来，详见log_block_convert_lsn_to_no()函数；
LOG_BLOCK_HDR_DATA_LEN
  两字节表示该block中已经有多少字节被使用；
LOG_BLOCK_FIRST_REC_GROUP
  两个字节表示该block中作为一个新的MTR开始log
LOG_BLOCK_CHECKPOINT_NO
  四个字节表示该block的checkpoint，也就是log_sys->next_checkpoint_no；
LOG_BLOCK_CHECKSUM
  四个字节记录了该block的校验值，通过innodb_checksum_algorithm变量指定算法；
{% endhighlight %}


<!--
innodb_fast_shutdown

innodb_force_recovery
innodb_force_recovery_crash

????LSN对应了日志文件的偏移量，为了减小故障恢复时间，引入了Checkpoint机制，

InnoDB在启动时会自动检测InnoDB数据和事务日志是否一致，是否需要执行相应的操作？？？保证数据一致性；当然，故障恢复时间与事务日志的大小相关。
-->

## 源码解析

在此涉及到两块内存缓冲，包括了 mtr_t 、log_sys 等内部结构，接下来一一介绍。

### 数据结构

首先看下 InnoDB 在内存中保存的一个全局变量，也就是 ```log_t* log_sys=NULL;``` 定义，其维护了一块称为 log buffer 的全局内存区域，也就是 ```log_sys->buff```，同时维护有若干 lsn 值等信息表示 logging 进行的状态；会在 ```log_init()``` 中对所有的内部区域进行分配并对各个变量进行初始化。

变量 log_sys 是 InnoDB 日志系统的中枢及核心对象，控制着日志的拷贝、写入、checkpoint 等核心功能，是连接 InnoDB 日志文件及 log buffer 的枢纽。

接下来，简单介绍下 log_t 中比较重要的字段值：

{% highlight cpp %}
struct log_t{
  char        pad1[CACHE_LINE_SIZE];
  lsn_t       lsn;                    // 接下来将要生成的log record使用此lsn的值；
  ulint       buf_size;               // buf大小，通过innodb-log-buffer-size变量指定
  ulint       buf_free;               // 下条log record写入的位置
  ulint       max_buf_free;           // 当buf_free超过该值后，需要进行刷新，可以查看log_free_check()函数
  byte*       buf;                    // 全局的log buffer
  ulint       buf_next_to_write;      // 下条记录写入到磁盘的偏移量
  lsn_t       write_lsn;              // 已经写入的LSN
  lsn_t       flushed_to_disk_lsn;    // 已经刷新到磁盘的LSN值，小于该值的日志已经被安全地记录到了磁盘上

  UT_LIST_BASE_NODE_T(log_group_t)    // 日志组，当前版本仅支持一组日志
          log_groups;                 // 包含了当前日志组的文件个数、每个文件的大小、space id等信息

  lsn_t       log_group_capacity;     // 当前日志文件的总容量在log_calc_max_ages()中计算，
                                      // (redo-log文件大小-头部)*0.9，其中乘0.9是一个安全系数

  lsn_t       max_modified_age_async;
                  /*!< when this recommended
                  value for lsn -
                  buf_pool_get_oldest_modification()
                  is exceeded, we start an
                  asynchronous preflush of pool pages */
  lsn_t       max_modified_age_sync;
                  /*!< when this recommended
                  value for lsn -
                  buf_pool_get_oldest_modification()
                  is exceeded, we start a
                  synchronous preflush of pool pages */
  lsn_t       max_checkpoint_age_async;
                  /*!< when this checkpoint age
                  is exceeded we start an
                  asynchronous writing of a new
                  checkpoint */
  lsn_t       max_checkpoint_age;
                  /*!< this is the maximum allowed value
                  for lsn - last_checkpoint_lsn when a
                  new query step is started */
  ib_uint64_t next_checkpoint_no;
                  /*!< next checkpoint number */
  lsn_t       last_checkpoint_lsn;
                  /*!< latest checkpoint lsn */
  lsn_t       next_checkpoint_lsn;
                  /*!< next checkpoint lsn */

{% endhighlight %}


<!--
上述结构体中的很多值会在初始化的时候进行设置，调用流程如下。

So log_sys->buf_next_to_write is between 0 and log_sys->buf_free, log_sys->write_lsn is equal or less log_sys->lsn, log_sys->flushed_to_disk_lsn is less or equal to log_sys->write_lsn.


log_sys->buf_free
 
写入buffer的起始偏移量


log_sys->write_lsn
 当前正在执行的写操作使用的临界lsn值；
log_sys->current_flush_lsn
 当前正在执行的write + flush操作使用的临界lsn值，一般和log_sys->write_lsn相等；

log_sys->buf_next_to_write
 
buffer中还未写到log file的起始偏移量。下次执行write+flush操作时，将会从此偏移量开始
log_sys->max_buf_free
 
确定flush操作执行的时间点，当log_sys->buf_free比此值大时需要执行flush操作，具体看log_check_margins函数


lsn是联系dirty page，redo log record和redo log file的纽带。在每个redo log record被拷贝到内存的log buffer时会产生一个相关联的lsn，而每个页面修改时会产生一个log record，从而每个数据库的page也会有一个相关联的lsn，这个lsn记录在每个page的header字段中。为了保证WAL（Write-Ahead-Logging）要求的逻辑，dirty page要求其关联lsn的log record已经被写入log file才允许执行flush操作。
-->



### 日志生成

mtr (mini-transactions) 在代码中对应了 ```struct mtr_t``` 结构体，其内部有一个局部 buffer，会将一组 log record 集中起来，然后批量写入 log buffer；mtr_t 的结构体如下所示：

{% highlight text %}
struct mtr_t {
  struct Impl {
    mtr_buf_t  m_memo;  // 由此mtr涉及的操作所造成的脏页列表
    mtr_buf_t  m_log;   // mtr的局部缓存，记录log-records；
  };
};
{% endhighlight %}

其中 m_memo 对象会记录与本次事务相关的页以及锁信息，并在提交时 (复制到 log buffer) 之后将脏页添加到 flush_list 并释放所持有的锁，详细的内容可以参考 ```mtr_commit()->mtr_memo_pop_all()``` 函数调用。

简单来说，log record 的生成过程如下：

1. 创建一个 mtr_t 类型的对象；
2. 执行 ```mtr_start()``` 初始化 mtr_t 的字段，包括 local buffer；
3. 在对 BP 中的 Page 进行修改的同时，调用 ```mlog_write_ulint()```、```mlog_write_string()```、```mlog_write_null()``` 类似函数，生成 redo log record 并保存在 local buffer 中；
4. 执行 ```mtr_commit()``` 将 local buffer 中的日志拷贝到全局的 ```log_sys->buf+log_sys->buf_free```，同时将脏页添加到 flush list，供后续执行 flush 操作时使用。

对于这一过程的执行，可以将 ```page_cur_insert_rec_write_log()``` 函数作为参考。



{% highlight text %}
mtr_commit()                          提交mtr，对应了mtr_t::commit()@mtr0mtr.cc
 |-mtr_t::Command::execute()          执行，同样在mtr0mtr.cc文件中
   |-mtr_t::Command::prepare_write()  准备写入，会返回字节数
     |-fil_names_write_if_was_clean()
       |-fil_names_dirty_and_write()  如果在fil_names_clear()之后这是第一次写入
{% endhighlight %}





<!--
## 参考


Innodb redo log archiving
https://www.percona.com/blog/2014/03/28/innodb-redo-log-archiving/
-->



{% highlight text %}
{% endhighlight %}
