---
title: InnoDB Double Write Buffer
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,double write buffer
description: 从 Double Write Buffer 来看，貌似是内存中的一块缓存区域，实际上，这里的 buffer 并不只是一块内存区域，而是还包括了存放在表空间中或是单独指定的某个文件中的一个 buffer 。在此，介绍下为什么会有 Double Write Buffer，以及其是如何实现的。
---

从 Double Write Buffer 来看，貌似是内存中的一块缓存区域，实际上，这里的 buffer 并不只是一块内存区域，而是还包括了存放在表空间中或是单独指定的某个文件中的一个 buffer 。

在此，介绍下为什么会有 Double Write Buffer，以及其是如何实现的。

<!-- more -->

## 简介

总体来说，Double Write Buffer 是 InnoDB 所使用的一种较为独特的文件 Flush 实现技术，也就是牺牲了一点点写性能，提高系统 Crash 或者断电情况下数据的安全性，避免写入的数据不完整。

在介绍 double write 的实现之前，有必要先了解一下 partial page write 问题。

<!--
一般来说，Innodb 在将数据同步到数据文件进行持久化之前，首先会将需要同步的内容写入存在于表空间中的系统保留的存储空间，也就是被我们称之为 Double Write Buffer 的地方，然后再将数据进 行文件同步。所以实质上，Double Write Buffer 中就是存放了一份需要同步到文件中数据的一个备份， 以便在遇到系统 Crash 或者主机断电的时候，能够校验最后一次文件同步是否准确的完成了，如果未完 成，则可以通过这个备份来继续完成工作，保证数据的正确性。
-->

### 问题起因

InnoDB 中的默认页大小是 16KB，通过 innodb_page_size 变量定义，很多的操作 (主要是对数据文件操作)，如数据校验、写入磁盘等，也是以页为单位进行。

而计算机硬件和操作系统的原子操作通常小于该值，一般为 512 字节，也就意味着，在极端情况下（如宕机、断电、OS Crash 等），往往并不能保证写入页的原子性。

{% highlight text %}
----- MySQL变量查看，数据写入页大小为16K
mysql> SHOW GLOBAL VARIABLES LIKE 'innodb_page_size';
+------------------+-------+
| Variable_name    | Value |
+------------------+-------+
| innodb_page_size | 16384 |
+------------------+-------+
1 row in set (0.06 sec)

----- 查看文件系统的块大小，一般为4K
# getconf PAGESIZE
# blockdev --getbsz /dev/sda7
# dumpe2fs /dev/sda7 | grep "Block size"
dumpe2fs 1.42.9 (28-Dec-2013)
Block size:               4096

----- 查看sector的大小
# fdisk -l | grep Sector
Sector size (logical/physical): 512 bytes / 512 bytes
{% endhighlight %}

<!--从上面的结果可以看到DB page=4*OS page=16*IO page=32*sector size
磁盘IO除了IO block size，还有一个概念是扇区(IO sector)，扇区是磁盘物理操作的基本单位，而IO 块是磁盘操作的逻辑单位，一个IO块对应一个或多个扇区，扇区大小一般为512个字节。 -->

例如，16K 的数据，在写入 4K 时机器宕机，此时只有一部分写是成功的，这种情况下就是 partial page write 问题。

MySQL 在崩溃恢复阶段，读取数据页时，需要检查页的 checksum，当发生 partial page write 时，页已经损坏，就导致数据无法恢复。

为了解决上述问题，采用两次写，此时需要额外添加两个部分，A) 内存中的两次写缓冲 (double write buffer)，大小为 2MB；B) 磁盘上共享表空间中连续的 128 页，大小也为 2MB。

### 配置参数

在 InnoDB 中，可以通过如下方式查看 double write 的状态。

{% highlight text %}
------ 查看是否启用了double write，以及相关参数
mysql> SHOW VARIABLES LIKE 'innodb_doublewrite%';
+-------------------------------+-------+
| Variable_name                 | Value |
+-------------------------------+-------+
| innodb_doublewrite            | ON    |
| innodb_doublewrite_batch_size | 120   |
+-------------------------------+-------+
2 rows in set (0.02 sec)

----- 可以查询double write的使用情况
mysql> SHOW STATUS LIKE 'innodb_dblwr_%';
+----------------------------+-------+
| Variable_name              | Value |
+----------------------------+-------+
| Innodb_dblwr_pages_written | 14615 |   从BP写入到dblwr的page数
| Innodb_dblwr_writes        | 636   |   写文件的次数
+----------------------------+-------+
2 rows in set (0.02 sec)
{% endhighlight %}

如上可以得到平均每次写操作合并页数为 ```Innodb_dblwr_pages_written/Innodb_dblwr_writes``` 。


### 工作过程

工作过程大致如下：

1. 当需要将缓冲池的脏页刷新到 data file 时，并不直接写到数据文件中，而是先拷贝至内存中的 double write buffer。
2. 接着从 double write buffer 分两次写入磁盘共享表空间中，每次写入 1MB，并马上调用 fsync 函数，同步到磁盘，避免缓冲带来的问题。
3. 第 2 步完成后，再将两次写缓冲区写入数据文件。

如下是执行示意图。

![how innodb double write works]({{ site.url }}/images/databases/mysql/innodb-double-write-works.jpg "how innodb double write works"){: .pull-center }

在这个过程中，第二步的 double write 是顺序写，所以开销并不大；而第三步，在将 double write buffer 写入各表空间文件，是离散写入；而 double write 实际引入的是第二步的开销。

### 恢复过程

有 double write 后，恢复时就简单多了，首先检查数据页，如果损坏，则尝试从 double write 中恢复数据；然后，检查 double writer 的数据的完整性，如果不完整直接丢弃，重新执行 redo log；如果 double write 的数据是完整的，用 double buffer 的数据更新该数据页，跳过该 redo log。

![how innodb double write recovery]({{ site.url }}/images/databases/mysql/innodb-double-write-recovery.jpg "how innodb double write recovery"){: .pull-center }

有些时候，并不是所有的场景都需要使用 Double Write 这样的机制来保证数据的安全准确性，比如当我们使用某些特别文件系统的时候，如在 Solaris 平台上非常著名的 ZFS 文件系统，他就可以自己保证文件写入的完整性。

再有就是从机，或者硬件也提供了类似的原子写入功能，因此可以关闭 double write 功能。

也即将 innodb_doublewrite 变量设置为 OFF，此时的写入过程大致如下。

![innodb double write closed]({{ site.url }}/images/databases/mysql/innodb-double-write-closed.png "innodb double write closed"){: .pull-center width="90%" }

## 源码解析

如上所述，一个 dblwr 有 2MB 也就是 128 pages，其中默认有 120 pages 用于批量刷新，可以直接通过 ```innodb_doublewrite_batch_size``` 变量设置，其包括了 BUF_FLUSH_LRU、BUF_FLUSH_LIST，剩下的 8 个页用于单个 page 的 flush。

{% highlight cpp %}
UNIV_INTERN buf_dblwr_t* buf_dblwr = NULL;          // 定义Double Write Buffer全局变量

struct buf_dblwr_t {
    ib_mutex_t  mutex;                 // 互斥量，用于保护first_free、write_buf
    ulint       block1;                // 第一个doubewrite块(64 pages)的page no
    ulint       block2;                // 第二个doublewrite块的page no
    ulint       first_free;            // 在write_buf中第一个空闲的位置，单位为UNIV_PAGE_SIZE
    ulint       b_reserved;            // 为batch flush预留的slot数
    os_event_t  b_event;               // 等待batch flush完成的事件
    ulint       s_reserved;            // 为单个page刷新预留的slot数
    os_event_t  s_event;               // 等待single flush完成的事件
    bool*       in_use;                // 标记一个slot是否被使用，只用于single page flush
    bool        batch_running;         // 当设置为TRUE时，表明有batch flush正在执行
    byte*       write_buf;             // dblwr在内存的缓存，以UNIV_PAGE_SIZE为单位对齐
    byte*       write_buf_unaligned;   // 未对齐的write_buf
    buf_page_t**    buf_block_arr;     // 存储已经cache到write_buf的block的指针
};
{% endhighlight %}

对于 FLUSH 操作，有三种类型。

* BUF_FLUSH_LRU<br>从 buffer pool 的 LRU 上扫描并刷新。
* BUF_FLUSH_LIST<br>从 buffer pool 的 FLUSH LIST 上扫描并刷新。
* BUF_FLUSH_SINGLE_PAGE<br>从 LRU 上只刷新一个 page，会通过 ```buf_dblwr_write_single_page()``` 来写一个 page 。

前两种属于 BATCH FLUSH，最后一种属于 SINGLE FLUSH，在 ```buf_flush_write_block_low()``` 函数中执行如下逻辑。

<!--
BUF_FLUSH_SINGLE_PAGE在几种情况下使用到：
1.buf_flush_or_remove_page
2.buf_flush_single_page_from_LRU，这在FREE LIST不够用时，IO-bound场景下，可能频繁调用到（buf_LRU_get_free_block）
3.buf_flush_page_try
-->

{% highlight text %}
buf_flush_page()                      将可以刷新的页写入到磁盘中
 |-buf_flush_write_block_low()
   |-buf_dblwr_write_single_page()    刷新类型为BUF_FLUSH_SINGLE_PAGE时
   |                                  会写入dblwr+sync，然后写入datafile+sync
   |
   |-buf_dblwr_add_to_batch()         批量写入，只要是非SINGLE都划分到此类
   |
   |-fil_flush()                      如果是同步，则刷新并等待执行完成
   |-buf_page_io_complete()
{% endhighlight %}

在如上的配置中，只要不是 SINGLE 类型，都作为 BATCH 。

### single

也就是在 ```buf_dblwr_write_single_page()``` 函数中，用于将一个 page 加入到 double write buffer 中，并完成写操作

{% highlight cpp %}
void buf_dblwr_write_single_page(buf_page_t* bpage, bool sync)
{
    // 计算为single刷新预留的dblwr page，其中size为dblwr总的页数，一般为128 pages
    size = 2 * TRX_SYS_DOUBLEWRITE_BLOCK_SIZE;
    n_slots = size - srv_doublewrite_batch_size;  // 默认为8 pages

    // 接下来需要检查下数据的有效性
    if (buf_page_get_state(bpage) == BUF_BLOCK_FILE_PAGE) {

        /* Check that the actual page in the buffer pool is
        not corrupt and the LSN values are sane. */
        buf_dblwr_check_block((buf_block_t*) bpage);

        /* Check that the page as written to the doublewrite
        buffer has sane LSN values. */
        if (!bpage->zip.data) {
            buf_dblwr_check_page_lsn(
                ((buf_block_t*) bpage)->frame);
        }
    }

retry:
    // 当s_reserved值等于最大single page数量时，线程会等待有空闲slot
    mutex_enter(&buf_dblwr->mutex);
    if (buf_dblwr->s_reserved == n_slots) {

        /* All slots are reserved. */
        int64_t sig_count = os_event_reset(buf_dblwr->s_event);
        mutex_exit(&buf_dblwr->mutex);
        os_event_wait_low(buf_dblwr->s_event, sig_count);

        goto retry;
    }

    // 找到一个没有在使用的slot，分配给当前page，将in_use设为TRUE，并递增s_reserved
    for (i = srv_doublewrite_batch_size; i < size; ++i) {
        if (!buf_dblwr->in_use[i]) {
            break;
        }
    }
    buf_dblwr->in_use[i] = true;
    buf_dblwr->s_reserved++;
    buf_dblwr->buf_block_arr[i] = bpage;
    mutex_exit(&buf_dblwr->mutex);        // 同时释放buf_dblwr->mutex

    // 将单个page写入到double write buffer中
    if (i < TRX_SYS_DOUBLEWRITE_BLOCK_SIZE) {
        offset = buf_dblwr->block1 + i;
    } else {
        offset = buf_dblwr->block2 + i
             - TRX_SYS_DOUBLEWRITE_BLOCK_SIZE;
    }
    if (bpage->size.is_compressed()) {
        memcpy(buf_dblwr->write_buf + univ_page_size.physical() * i,
               bpage->zip.data, bpage->size.physical());
        // 对于压缩页，会补0
        memset(buf_dblwr->write_buf + univ_page_size.physical() * i
               + bpage->size.physical(), 0x0,
               univ_page_size.physical() - bpage->size.physical());

        fil_io(IORequestWrite, true,
               page_id_t(TRX_SYS_SPACE, offset), univ_page_size, 0,
               univ_page_size.physical(),
               (void*) (buf_dblwr->write_buf
                + univ_page_size.physical() * i),
               NULL);
    } else {
        /* It is a regular page. Write it directly to the
        doublewrite buffer */
        fil_io(IORequestWrite, true,
               page_id_t(TRX_SYS_SPACE, offset), univ_page_size, 0,
               univ_page_size.physical(),
               (void*) ((buf_block_t*) bpage)->frame,
               NULL);
    }

    // 将doublewrite buffer中的数据，也就是系统表，写入到磁盘
    fil_flush(TRX_SYS_SPACE);

    // 将数据写入到数据表中，此时可能是同步操作
    buf_dblwr_write_block_to_datafile(bpage, sync);
}
{% endhighlight %}


### batch

入口函数为 ```buf_dblwr_add_to_batch()```，也就是将一个 page 加入到 double write buffer 中，如果 batch 满了，则刷 double write buffer 到磁盘。

{% highlight cpp %}
void buf_dblwr_add_to_batch(buf_page_t* bpage)
{
try_again:
    // 获取buf_dblwr->mutex锁
    mutex_enter(&buf_dblwr->mutex);

    // 当batch_running为TRUE，表示已有线程开始做batch flush来刷dblwr，释放互斥锁，重新等待
    if (buf_dblwr->batch_running) {
        /* 正常来说，只有后台线程才会做batche flush操作，正常不会有竞争；
           唯一的例外是当达到sync checkpoint时，用户线程强制做batch flush操作。*/
        int64_t sig_count = os_event_reset(buf_dblwr->b_event);
        mutex_exit(&buf_dblwr->mutex);
        os_event_wait_low(buf_dblwr->b_event, sig_count);
        goto try_again;
    }

    // 如果batch满了，则释放mutex，主动把dblwr的写到磁盘
    if (buf_dblwr->first_free == srv_doublewrite_batch_size) {
        mutex_exit(&(buf_dblwr->mutex));
        buf_dblwr_flush_buffered_writes();
        goto try_again;
    }

    // 将page拷贝到第buf_dblwr->first_free个槽位，并设置相应的变量
    byte*   p = buf_dblwr->write_buf
        + univ_page_size.physical() * buf_dblwr->first_free;
    if (bpage->size.is_compressed()) {
        UNIV_MEM_ASSERT_RW(bpage->zip.data, bpage->size.physical());
        /* Copy the compressed page and clear the rest. */
        memcpy(p, bpage->zip.data, bpage->size.physical());
        memset(p + bpage->size.physical(), 0x0,
               univ_page_size.physical() - bpage->size.physical());
    } else {
        ut_a(buf_page_get_state(bpage) == BUF_BLOCK_FILE_PAGE);
        UNIV_MEM_ASSERT_RW(((buf_block_t*) bpage)->frame,
                   bpage->size.logical());
        memcpy(p, ((buf_block_t*) bpage)->frame, bpage->size.logical());
    }

    buf_dblwr->buf_block_arr[buf_dblwr->first_free] = bpage;
    buf_dblwr->first_free++;
    buf_dblwr->b_reserved++;

    // 再次判断batch是否满了，是则释放mutex，主动把dblwr的写到磁盘
    if (buf_dblwr->first_free == srv_doublewrite_batch_size) {
        mutex_exit(&(buf_dblwr->mutex));
        buf_dblwr_flush_buffered_writes();
        return;
    }

    mutex_exit(&(buf_dblwr->mutex));
}

{% endhighlight %}

接下来，再看看上述函数中调用的 ```buf_dblwr_flush_buffered_writes()```，该函数会对 batch flush 操作批量刷 double write buffer 函数。

{% highlight cpp %}
void buf_dblwr_flush_buffered_writes(void)
{
    // 如果没有开启doublewrite buffer则直接调用同步写入函数
    if (!srv_use_doublewrite_buf || buf_dblwr == NULL) {
        /* Sync the writes to the disk. */
        buf_dblwr_sync_datafiles();
        return;
    }

try_again:
    mutex_enter(&buf_dblwr->mutex);  // 获取锁

    // 第一次写入时，直接调用同步写入
    if (buf_dblwr->first_free == 0) {
        mutex_exit(&buf_dblwr->mutex);
        os_aio_simulated_wake_handler_threads();
        return;
    }

    // 如果batch_running为TRUE，表示正有线程在做batch flush，则等待一段时间重试
    if (buf_dblwr->batch_running) {
        int64_t sig_count = os_event_reset(buf_dblwr->b_event);
        mutex_exit(&buf_dblwr->mutex);
        os_event_wait_low(buf_dblwr->b_event, sig_count);
        goto try_again;
    }

    // 设置buf_dblwr->batch_running为true，防止并发写入
    // 正常来说批量写入只有后台函数以及checkpoint sync的用户线程，但是单页也有可能
    buf_dblwr->batch_running = true;
    first_free = buf_dblwr->first_free;

    mutex_exit(&buf_dblwr->mutex); // 释放锁
    write_buf = buf_dblwr->write_buf;

    // 检查每一个将要写dblwr的block以及write_buf中的page是否被损坏或者LSN值是否正确
    for (ulint len2 = 0, i = 0;
         i < buf_dblwr->first_free;
         len2 += UNIV_PAGE_SIZE, i++) {

        const buf_block_t*  block;
        block = (buf_block_t*) buf_dblwr->buf_block_arr[i];
        if (buf_block_get_state(block) != BUF_BLOCK_FILE_PAGE
            || block->page.zip.data) {
            /* No simple validate for compressed
            pages exists. */
            continue;
        }
        buf_dblwr_check_block(block);
        buf_dblwr_check_page_lsn(write_buf + len2);
    }

    // 将write_buf中的page写入到文件中，先写第一个block，再写第二个block
    /* Write out the first block of the doublewrite buffer */
    len = ut_min(TRX_SYS_DOUBLEWRITE_BLOCK_SIZE,
             buf_dblwr->first_free) * UNIV_PAGE_SIZE;
    fil_io(IORequestWrite, true,
           page_id_t(TRX_SYS_SPACE, buf_dblwr->block1), univ_page_size,
           0, len, (void*) write_buf, NULL);
    if (buf_dblwr->first_free <= TRX_SYS_DOUBLEWRITE_BLOCK_SIZE) {
        /* No unwritten pages in the second block. */
        goto flush;
    /* Write out the second block of the doublewrite buffer. */
    len = (buf_dblwr->first_free - TRX_SYS_DOUBLEWRITE_BLOCK_SIZE)
           * UNIV_PAGE_SIZE;
    write_buf = buf_dblwr->write_buf
            + TRX_SYS_DOUBLEWRITE_BLOCK_SIZE * UNIV_PAGE_SIZE;
    fil_io(IORequestWrite, true,
           page_id_t(TRX_SYS_SPACE, buf_dblwr->block2), univ_page_size,
           0, len, (void*) write_buf, NULL);

flush:
    /* increment the doublewrite flushed pages counter */
    srv_stats.dblwr_pages_written.add(buf_dblwr->first_free);
    srv_stats.dblwr_writes.inc();

    /* Now flush the doublewrite buffer data to disk */
    fil_flush(TRX_SYS_SPACE);

    /* We know that the writes have been flushed to disk now
    and in recovery we will find them in the doublewrite buffer
    blocks. Next do the writes to the intended positions. */

    /* Up to this point first_free and buf_dblwr->first_free are
    same because we have set the buf_dblwr->batch_running flag
    disallowing any other thread to post any request but we
    can't safely access buf_dblwr->first_free in the loop below.
    This is so because it is possible that after we are done with
    the last iteration and before we terminate the loop, the batch
    gets finished in the IO helper thread and another thread posts
    a new batch setting buf_dblwr->first_free to a higher value.
    If this happens and we are using buf_dblwr->first_free in the
    loop termination condition then we'll end up dispatching
    the same block twice from two different threads. */
    ut_ad(first_free == buf_dblwr->first_free);
    for (ulint i = 0; i < first_free; i++) {
        buf_dblwr_write_block_to_datafile(
            buf_dblwr->buf_block_arr[i], false);
    }

    /* Wake possible simulated aio thread to actually post the
    writes to the operating system. We don't flush the files
    at this point. We leave it to the IO helper thread to flush
    datafiles when the whole batch has been processed. */
    os_aio_simulated_wake_handler_threads();
}
{% endhighlight %}

<!--
    8.将double write buffer刷到磁盘后（fil_flush(TRX_SYS_SPACE);），逐个开始写数据文件(OS_AIO_SIMULATED_WAKE_LATER)

     866                 buf_dblwr_write_block_to_datafile(
     867                         buf_dblwr->buf_block_arr[i]);
    9.唤醒IO线程（os_aio_simulated_wake_handler_threads）

注意这里，在函数结束时并没有将batch_running设置为FALSE，因为这里对数据文件做的是异步写，设置标记位的工作留给了IO线程来完成
io_handler_thread-> fil_aio_wait-> buf_page_io_complete->buf_flush_write_complete->buf_dblwr_update()：
看起来在double write buffer中，BATCH FLUSH 和SINGER PAGE FLUSH对应的slot非常独立，那么我们是否可以把这个mutex拆成两个呢？

简单的测试了一把，结果是。。。。。差别不大
我把自己的想法提到buglist上了，不知道官方的怎么看。。。
-->











{% highlight text %}
{% endhighlight %}
