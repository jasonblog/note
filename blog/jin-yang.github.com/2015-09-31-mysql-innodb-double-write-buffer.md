---
title: InnoDB Double Write Buffer
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,double write buffer
description: 從 Double Write Buffer 來看，貌似是內存中的一塊緩存區域，實際上，這裡的 buffer 並不只是一塊內存區域，而是還包括了存放在表空間中或是單獨指定的某個文件中的一個 buffer 。在此，介紹下為什麼會有 Double Write Buffer，以及其是如何實現的。
---

從 Double Write Buffer 來看，貌似是內存中的一塊緩存區域，實際上，這裡的 buffer 並不只是一塊內存區域，而是還包括了存放在表空間中或是單獨指定的某個文件中的一個 buffer 。

在此，介紹下為什麼會有 Double Write Buffer，以及其是如何實現的。

<!-- more -->

## 簡介

總體來說，Double Write Buffer 是 InnoDB 所使用的一種較為獨特的文件 Flush 實現技術，也就是犧牲了一點點寫性能，提高系統 Crash 或者斷電情況下數據的安全性，避免寫入的數據不完整。

在介紹 double write 的實現之前，有必要先了解一下 partial page write 問題。

<!--
一般來說，Innodb 在將數據同步到數據文件進行持久化之前，首先會將需要同步的內容寫入存在於表空間中的系統保留的存儲空間，也就是被我們稱之為 Double Write Buffer 的地方，然後再將數據進 行文件同步。所以實質上，Double Write Buffer 中就是存放了一份需要同步到文件中數據的一個備份， 以便在遇到系統 Crash 或者主機斷電的時候，能夠校驗最後一次文件同步是否準確的完成了，如果未完 成，則可以通過這個備份來繼續完成工作，保證數據的正確性。
-->

### 問題起因

InnoDB 中的默認頁大小是 16KB，通過 innodb_page_size 變量定義，很多的操作 (主要是對數據文件操作)，如數據校驗、寫入磁盤等，也是以頁為單位進行。

而計算機硬件和操作系統的原子操作通常小於該值，一般為 512 字節，也就意味著，在極端情況下（如宕機、斷電、OS Crash 等），往往並不能保證寫入頁的原子性。

{% highlight text %}
----- MySQL變量查看，數據寫入頁大小為16K
mysql> SHOW GLOBAL VARIABLES LIKE 'innodb_page_size';
+------------------+-------+
| Variable_name    | Value |
+------------------+-------+
| innodb_page_size | 16384 |
+------------------+-------+
1 row in set (0.06 sec)

----- 查看文件系統的塊大小，一般為4K
# getconf PAGESIZE
# blockdev --getbsz /dev/sda7
# dumpe2fs /dev/sda7 | grep "Block size"
dumpe2fs 1.42.9 (28-Dec-2013)
Block size:               4096

----- 查看sector的大小
# fdisk -l | grep Sector
Sector size (logical/physical): 512 bytes / 512 bytes
{% endhighlight %}

<!--從上面的結果可以看到DB page=4*OS page=16*IO page=32*sector size
磁盤IO除了IO block size，還有一個概念是扇區(IO sector)，扇區是磁盤物理操作的基本單位，而IO 塊是磁盤操作的邏輯單位，一個IO塊對應一個或多個扇區，扇區大小一般為512個字節。 -->

例如，16K 的數據，在寫入 4K 時機器宕機，此時只有一部分寫是成功的，這種情況下就是 partial page write 問題。

MySQL 在崩潰恢復階段，讀取數據頁時，需要檢查頁的 checksum，當發生 partial page write 時，頁已經損壞，就導致數據無法恢復。

為瞭解決上述問題，採用兩次寫，此時需要額外添加兩個部分，A) 內存中的兩次寫緩衝 (double write buffer)，大小為 2MB；B) 磁盤上共享表空間中連續的 128 頁，大小也為 2MB。

### 配置參數

在 InnoDB 中，可以通過如下方式查看 double write 的狀態。

{% highlight text %}
------ 查看是否啟用了double write，以及相關參數
mysql> SHOW VARIABLES LIKE 'innodb_doublewrite%';
+-------------------------------+-------+
| Variable_name                 | Value |
+-------------------------------+-------+
| innodb_doublewrite            | ON    |
| innodb_doublewrite_batch_size | 120   |
+-------------------------------+-------+
2 rows in set (0.02 sec)

----- 可以查詢double write的使用情況
mysql> SHOW STATUS LIKE 'innodb_dblwr_%';
+----------------------------+-------+
| Variable_name              | Value |
+----------------------------+-------+
| Innodb_dblwr_pages_written | 14615 |   從BP寫入到dblwr的page數
| Innodb_dblwr_writes        | 636   |   寫文件的次數
+----------------------------+-------+
2 rows in set (0.02 sec)
{% endhighlight %}

如上可以得到平均每次寫操作合併頁數為 ```Innodb_dblwr_pages_written/Innodb_dblwr_writes``` 。


### 工作過程

工作過程大致如下：

1. 當需要將緩衝池的髒頁刷新到 data file 時，並不直接寫到數據文件中，而是先拷貝至內存中的 double write buffer。
2. 接著從 double write buffer 分兩次寫入磁盤共享表空間中，每次寫入 1MB，並馬上調用 fsync 函數，同步到磁盤，避免緩衝帶來的問題。
3. 第 2 步完成後，再將兩次寫緩衝區寫入數據文件。

如下是執行示意圖。

![how innodb double write works]({{ site.url }}/images/databases/mysql/innodb-double-write-works.jpg "how innodb double write works"){: .pull-center }

在這個過程中，第二步的 double write 是順序寫，所以開銷並不大；而第三步，在將 double write buffer 寫入各表空間文件，是離散寫入；而 double write 實際引入的是第二步的開銷。

### 恢復過程

有 double write 後，恢復時就簡單多了，首先檢查數據頁，如果損壞，則嘗試從 double write 中恢復數據；然後，檢查 double writer 的數據的完整性，如果不完整直接丟棄，重新執行 redo log；如果 double write 的數據是完整的，用 double buffer 的數據更新該數據頁，跳過該 redo log。

![how innodb double write recovery]({{ site.url }}/images/databases/mysql/innodb-double-write-recovery.jpg "how innodb double write recovery"){: .pull-center }

有些時候，並不是所有的場景都需要使用 Double Write 這樣的機制來保證數據的安全準確性，比如當我們使用某些特別文件系統的時候，如在 Solaris 平臺上非常著名的 ZFS 文件系統，他就可以自己保證文件寫入的完整性。

再有就是從機，或者硬件也提供了類似的原子寫入功能，因此可以關閉 double write 功能。

也即將 innodb_doublewrite 變量設置為 OFF，此時的寫入過程大致如下。

![innodb double write closed]({{ site.url }}/images/databases/mysql/innodb-double-write-closed.png "innodb double write closed"){: .pull-center width="90%" }

## 源碼解析

如上所述，一個 dblwr 有 2MB 也就是 128 pages，其中默認有 120 pages 用於批量刷新，可以直接通過 ```innodb_doublewrite_batch_size``` 變量設置，其包括了 BUF_FLUSH_LRU、BUF_FLUSH_LIST，剩下的 8 個頁用於單個 page 的 flush。

{% highlight cpp %}
UNIV_INTERN buf_dblwr_t* buf_dblwr = NULL;          // 定義Double Write Buffer全局變量

struct buf_dblwr_t {
    ib_mutex_t  mutex;                 // 互斥量，用於保護first_free、write_buf
    ulint       block1;                // 第一個doubewrite塊(64 pages)的page no
    ulint       block2;                // 第二個doublewrite塊的page no
    ulint       first_free;            // 在write_buf中第一個空閒的位置，單位為UNIV_PAGE_SIZE
    ulint       b_reserved;            // 為batch flush預留的slot數
    os_event_t  b_event;               // 等待batch flush完成的事件
    ulint       s_reserved;            // 為單個page刷新預留的slot數
    os_event_t  s_event;               // 等待single flush完成的事件
    bool*       in_use;                // 標記一個slot是否被使用，只用於single page flush
    bool        batch_running;         // 當設置為TRUE時，表明有batch flush正在執行
    byte*       write_buf;             // dblwr在內存的緩存，以UNIV_PAGE_SIZE為單位對齊
    byte*       write_buf_unaligned;   // 未對齊的write_buf
    buf_page_t**    buf_block_arr;     // 存儲已經cache到write_buf的block的指針
};
{% endhighlight %}

對於 FLUSH 操作，有三種類型。

* BUF_FLUSH_LRU<br>從 buffer pool 的 LRU 上掃描並刷新。
* BUF_FLUSH_LIST<br>從 buffer pool 的 FLUSH LIST 上掃描並刷新。
* BUF_FLUSH_SINGLE_PAGE<br>從 LRU 上只刷新一個 page，會通過 ```buf_dblwr_write_single_page()``` 來寫一個 page 。

前兩種屬於 BATCH FLUSH，最後一種屬於 SINGLE FLUSH，在 ```buf_flush_write_block_low()``` 函數中執行如下邏輯。

<!--
BUF_FLUSH_SINGLE_PAGE在幾種情況下使用到：
1.buf_flush_or_remove_page
2.buf_flush_single_page_from_LRU，這在FREE LIST不夠用時，IO-bound場景下，可能頻繁調用到（buf_LRU_get_free_block）
3.buf_flush_page_try
-->

{% highlight text %}
buf_flush_page()                      將可以刷新的頁寫入到磁盤中
 |-buf_flush_write_block_low()
   |-buf_dblwr_write_single_page()    刷新類型為BUF_FLUSH_SINGLE_PAGE時
   |                                  會寫入dblwr+sync，然後寫入datafile+sync
   |
   |-buf_dblwr_add_to_batch()         批量寫入，只要是非SINGLE都劃分到此類
   |
   |-fil_flush()                      如果是同步，則刷新並等待執行完成
   |-buf_page_io_complete()
{% endhighlight %}

在如上的配置中，只要不是 SINGLE 類型，都作為 BATCH 。

### single

也就是在 ```buf_dblwr_write_single_page()``` 函數中，用於將一個 page 加入到 double write buffer 中，並完成寫操作

{% highlight cpp %}
void buf_dblwr_write_single_page(buf_page_t* bpage, bool sync)
{
    // 計算為single刷新預留的dblwr page，其中size為dblwr總的頁數，一般為128 pages
    size = 2 * TRX_SYS_DOUBLEWRITE_BLOCK_SIZE;
    n_slots = size - srv_doublewrite_batch_size;  // 默認為8 pages

    // 接下來需要檢查下數據的有效性
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
    // 當s_reserved值等於最大single page數量時，線程會等待有空閒slot
    mutex_enter(&buf_dblwr->mutex);
    if (buf_dblwr->s_reserved == n_slots) {

        /* All slots are reserved. */
        int64_t sig_count = os_event_reset(buf_dblwr->s_event);
        mutex_exit(&buf_dblwr->mutex);
        os_event_wait_low(buf_dblwr->s_event, sig_count);

        goto retry;
    }

    // 找到一個沒有在使用的slot，分配給當前page，將in_use設為TRUE，並遞增s_reserved
    for (i = srv_doublewrite_batch_size; i < size; ++i) {
        if (!buf_dblwr->in_use[i]) {
            break;
        }
    }
    buf_dblwr->in_use[i] = true;
    buf_dblwr->s_reserved++;
    buf_dblwr->buf_block_arr[i] = bpage;
    mutex_exit(&buf_dblwr->mutex);        // 同時釋放buf_dblwr->mutex

    // 將單個page寫入到double write buffer中
    if (i < TRX_SYS_DOUBLEWRITE_BLOCK_SIZE) {
        offset = buf_dblwr->block1 + i;
    } else {
        offset = buf_dblwr->block2 + i
             - TRX_SYS_DOUBLEWRITE_BLOCK_SIZE;
    }
    if (bpage->size.is_compressed()) {
        memcpy(buf_dblwr->write_buf + univ_page_size.physical() * i,
               bpage->zip.data, bpage->size.physical());
        // 對於壓縮頁，會補0
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

    // 將doublewrite buffer中的數據，也就是系統表，寫入到磁盤
    fil_flush(TRX_SYS_SPACE);

    // 將數據寫入到數據表中，此時可能是同步操作
    buf_dblwr_write_block_to_datafile(bpage, sync);
}
{% endhighlight %}


### batch

入口函數為 ```buf_dblwr_add_to_batch()```，也就是將一個 page 加入到 double write buffer 中，如果 batch 滿了，則刷 double write buffer 到磁盤。

{% highlight cpp %}
void buf_dblwr_add_to_batch(buf_page_t* bpage)
{
try_again:
    // 獲取buf_dblwr->mutex鎖
    mutex_enter(&buf_dblwr->mutex);

    // 當batch_running為TRUE，表示已有線程開始做batch flush來刷dblwr，釋放互斥鎖，重新等待
    if (buf_dblwr->batch_running) {
        /* 正常來說，只有後臺線程才會做batche flush操作，正常不會有競爭；
           唯一的例外是當達到sync checkpoint時，用戶線程強製做batch flush操作。*/
        int64_t sig_count = os_event_reset(buf_dblwr->b_event);
        mutex_exit(&buf_dblwr->mutex);
        os_event_wait_low(buf_dblwr->b_event, sig_count);
        goto try_again;
    }

    // 如果batch滿了，則釋放mutex，主動把dblwr的寫到磁盤
    if (buf_dblwr->first_free == srv_doublewrite_batch_size) {
        mutex_exit(&(buf_dblwr->mutex));
        buf_dblwr_flush_buffered_writes();
        goto try_again;
    }

    // 將page拷貝到第buf_dblwr->first_free個槽位，並設置相應的變量
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

    // 再次判斷batch是否滿了，是則釋放mutex，主動把dblwr的寫到磁盤
    if (buf_dblwr->first_free == srv_doublewrite_batch_size) {
        mutex_exit(&(buf_dblwr->mutex));
        buf_dblwr_flush_buffered_writes();
        return;
    }

    mutex_exit(&(buf_dblwr->mutex));
}

{% endhighlight %}

接下來，再看看上述函數中調用的 ```buf_dblwr_flush_buffered_writes()```，該函數會對 batch flush 操作批量刷 double write buffer 函數。

{% highlight cpp %}
void buf_dblwr_flush_buffered_writes(void)
{
    // 如果沒有開啟doublewrite buffer則直接調用同步寫入函數
    if (!srv_use_doublewrite_buf || buf_dblwr == NULL) {
        /* Sync the writes to the disk. */
        buf_dblwr_sync_datafiles();
        return;
    }

try_again:
    mutex_enter(&buf_dblwr->mutex);  // 獲取鎖

    // 第一次寫入時，直接調用同步寫入
    if (buf_dblwr->first_free == 0) {
        mutex_exit(&buf_dblwr->mutex);
        os_aio_simulated_wake_handler_threads();
        return;
    }

    // 如果batch_running為TRUE，表示正有線程在做batch flush，則等待一段時間重試
    if (buf_dblwr->batch_running) {
        int64_t sig_count = os_event_reset(buf_dblwr->b_event);
        mutex_exit(&buf_dblwr->mutex);
        os_event_wait_low(buf_dblwr->b_event, sig_count);
        goto try_again;
    }

    // 設置buf_dblwr->batch_running為true，防止併發寫入
    // 正常來說批量寫入只有後臺函數以及checkpoint sync的用戶線程，但是單頁也有可能
    buf_dblwr->batch_running = true;
    first_free = buf_dblwr->first_free;

    mutex_exit(&buf_dblwr->mutex); // 釋放鎖
    write_buf = buf_dblwr->write_buf;

    // 檢查每一個將要寫dblwr的block以及write_buf中的page是否被損壞或者LSN值是否正確
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

    // 將write_buf中的page寫入到文件中，先寫第一個block，再寫第二個block
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
    8.將double write buffer刷到磁盤後（fil_flush(TRX_SYS_SPACE);），逐個開始寫數據文件(OS_AIO_SIMULATED_WAKE_LATER)

     866                 buf_dblwr_write_block_to_datafile(
     867                         buf_dblwr->buf_block_arr[i]);
    9.喚醒IO線程（os_aio_simulated_wake_handler_threads）

注意這裡，在函數結束時並沒有將batch_running設置為FALSE，因為這裡對數據文件做的是異步寫，設置標記位的工作留給了IO線程來完成
io_handler_thread-> fil_aio_wait-> buf_page_io_complete->buf_flush_write_complete->buf_dblwr_update()：
看起來在double write buffer中，BATCH FLUSH 和SINGER PAGE FLUSH對應的slot非常獨立，那麼我們是否可以把這個mutex拆成兩個呢？

簡單的測試了一把，結果是。。。。。差別不大
我把自己的想法提到buglist上了，不知道官方的怎麼看。。。
-->











{% highlight text %}
{% endhighlight %}
