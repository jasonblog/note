---
title: InnoDB Checkpoint
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,checkpoint
description: 如果 redo log 可以無限地增大，同時緩衝池也足夠大，是不是就意味著可以不將緩衝池中的髒頁刷新回磁盤上？宕機時，完全可以通過 redo log 來恢復整個數據庫系統中的數據。顯然，上述的前提條件是不滿足的，這也就引入了 checkpoint 技術。在這篇文章裡，就簡單介紹下 MySQL 中的實現。
---

如果 redo log 可以無限地增大，同時緩衝池也足夠大，是不是就意味著可以不將緩衝池中的髒頁刷新回磁盤上？宕機時，完全可以通過 redo log 來恢復整個數據庫系統中的數據。

顯然，上述的前提條件是不滿足的，這也就引入了 checkpoint 技術。

在這篇文章裡，就簡單介紹下 MySQL 中的實現。

<!-- more -->

## 簡介

Checkpoint (檢查點) 的目的是為瞭解決以下幾個問題：1、縮短數據庫的恢復時間；2、緩衝池不夠用時，將髒頁刷新到磁盤；3、重做日誌不可用時，刷新髒頁。

* 數據庫宕機時，不需要重做所有的日誌，因為 Checkpoint 之前的髒頁都已經刷新回磁盤，只需對 Checkpoint 後的 redo log 進行恢復即可，這樣就大大縮短了恢復的時間。

* 當緩衝池不夠用時，會根據 LRU 算法淘汰最近最少使用的頁，若此頁為髒頁，那麼需要強制執行 Checkpoint，將髒頁刷回磁盤。

* 當前數據庫對 redo log 的設計都是循環使用的，為了防止被覆蓋，必須強制 Checkpoint，將緩衝池中的頁至少刷新到當前 redo log 的位置。

InnoDB 通過 Log Sequence Number, LSN 來標記版本，這是 8 字節的數字，每個頁有 LSN，重做日誌中也有 LSN，Checkpoint 也有 LSN，這個是聯繫三者的關鍵變量。

LSN 當前狀態可以通過如下命令查看。

{% highlight text %}
mysql> SHOW ENGINE INNODB STATUS\G
---
LOG
---
Log sequence number 293590838           LSN1事務創建時一條日誌
Log flushed up to   293590838
Pages flushed up to 293590838
Last checkpoint at  293590829
0 pending log flushes, 0 pending chkp writes
1139 log i/o's done, 0.00 log i/o's/second
{% endhighlight %}

### 分類

通常有兩種 Checkpoint，分別為：Sharp Checkpoint、Fuzzy Checkpoint；前者在正常關閉數據庫時使用，會將所有髒頁刷回磁盤；後者，會在運行時使用，用於部分髒頁的刷新。

Checkpoint 所做的事情無外乎是將緩衝池中的髒頁刷回到磁盤，不同之處在於每次刷新多少頁到磁盤，每次從哪裡取髒頁，以及什麼時間觸發 Checkpoint。

#### Master Thread Checkpoint

InnoDB 的主線程以每秒或每十秒的速度從緩衝池的髒頁列表中刷新一定比例的頁回磁盤，這個過程是異步的，此時 InnoDB 可以進行其他的操作，用戶查詢線程不會阻塞。

#### FLUSH_LRU_LIST Checkpoint

InnoDB 要保證 BP 中有足夠空閒頁，在 1.1.x 之前，該操作發生在用戶查詢線程中，顯然這會阻塞用戶的查詢。如果沒有足夠空閒頁，需要將 LRU 列表尾端的頁移除，如果有髒頁，那麼就需要進行 Checkpoint，因為這些頁來自 LRU 列表，所以稱為 FLUSH_LRU_LIST Checkpoint 。

MySQL-5.6 (InnoDB-1.2.x) 版本開始，這個檢查被放在了一個單獨的 Page Cleaner 線程中進行，而且用戶可以通過參數 ```innodb_lru_scan_depth``` 控制 LRU 列表中可用頁的數量。

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

是指重做日誌文件不可用時，需要強制將髒頁列表中的一些頁刷新回磁盤，而此時髒頁是從髒頁列表中選取的，這可以保證重做日誌文件可循環使用。

在 InnoDB 1.2.X 版本之前，Async Flush Checkpoint 會阻塞發現問題的用戶查詢線程，Sync Flush Checkpoint 會阻塞所有查詢線程；InnoDB 1.2.X 之後放到單獨的 Page Cleaner Thread。



<!--
若將已經寫入到重做日誌的 LSN 記為 redo_lsn，將已經刷新回磁盤最新頁的 LSN 記為 checkpoint_lsn，則可定義：

checkpoint_age = redo_lsn - checkpoint_lsn

再定義以下的變量：

async_water_mark = 75% * total_redo_log_file_size

sync_water_mark = 90% * total_redo_log_file_size

若每個重做日誌文件的大小為1GB，並且定義了兩個重做日誌文件，則重做日誌文件的總大小為2GB。那麼async_water_mark=1.5GB，sync_water_mark=1.8GB。則：

當checkpoint_age<async_water_mark時，不需要刷新任何髒頁到磁盤；

當async_water_mark<checkpoint_age<sync_water_mark時觸發Async Flush，從Flush列表中刷新足夠的髒頁回磁盤，使得刷新後滿足checkpoint_age<async_water_mark；

checkpoint_age>sync_water_mark這種情況一般很少發生，除非設置的重做日誌文件太小，並且在進行類似LOAD DATA的BULK INSERT操作。此時觸發Sync Flush操作，從Flush列表中刷新足夠的髒頁回磁盤，使得刷新後滿足checkpoint_age<async_water_mark。

可見，Async/Sync Flush Checkpoint是為了保證重做日誌的循環使用的可用性。在InnoDB 1.2.x版本之前，Async Flush Checkpoint會阻塞發現問題的用戶查詢線程，而Sync Flush Checkpoint會阻塞所有的用戶查詢線程，並且等待髒頁刷新完成。從InnoDB 1.2.x版本開始——也就是MySQL 5.6版本，這部分的刷新操作同樣放入到了單獨的Page Cleaner Thread中，故不會阻塞用戶查詢線程。

MySQL官方版本並不能查看刷新頁是從Flush列表中還是從LRU列表中進行Checkpoint的，也不知道因為重做日誌而產生的Async/Sync Flush的次數。但是InnoSQL版本提供了方法，可以通過命令SHOW ENGINE INNODB STATUS來觀察，如：

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

即髒頁數量太多時，InnoDB 會強制進行 Checkpoint 。

{% highlight text %}
mysql> SHOW GLOBAL VARIABLES LIKE 'innodb_max_dirty_pages_pct';
+----------------------------+-----------+
| Variable_name              | Value     |
+----------------------------+-----------+
| innodb_max_dirty_pages_pct | 75.000000 |
+----------------------------+-----------+
1 row in set (0.03 sec)
{% endhighlight %}

也即當緩衝池中髒頁的數量佔據 75% 時，強制進行 Checkpoint，刷新一部分的髒頁到磁盤，其目的還是為了保證緩衝池中有足夠可用的空閒頁。

### CheckPoint 機制

在 Innodb 每次都取最老的 modified page 對應的 LSN，並將此髒頁的 LSN 作為 Checkpoint 點記錄到日誌文件，意思就是 "此 LSN 之前對應的日誌和數據都已經刷新到磁盤" 。

當 MySQL 啟動做崩潰恢復時，會從 last checkpoint 對應的 LSN 開始掃描 redo log ，並將其應用到 buffer pool，直到 last checkpoint 對應的 LSN 等於 log flushed up to 對應的 LSN，則恢復完成。

如下是整個 redo log 的生命週期。

![innodb checkpoint lsn]({{ site.url }}/images/databases/mysql/checkpoint-lsn.png "innodb checkpoint lsn"){: .pull-center }

InnoDB 的一條事務日誌共經歷 4 個階段：

1. 創建階段 (log sequence number, LSN1)：事務創建一條日誌，當前系統 LSN 最大值，新的事務日誌 LSN 將在此基礎上生成，也就是 LSN1+新日誌的大小；

2. 日誌刷盤 (log flushed up to, LSN2)：當前已經寫入日誌文件做持久化的 LSN；

3. 數據刷盤 (oldest modified data log, LSN3)：當前最舊的髒頁數據對應的 LSN，寫 Checkpoint 的時候直接將此 LSN 寫入到日誌文件；

4. 寫CKP (last checkpoint at, LSN4)：當前已經寫入 Checkpoint 的 LSN，也就是上次的寫入；

對於系統來說，以上 4 個 LSN 是遞減的，即： LSN1>=LSN2>=LSN3>=LSN4 。如上所述，LSN 當前狀態可以通過如下命令查看。

{% highlight text %}
mysql> SHOW ENGINE INNODB STATUS\G
---
LOG
---
Log sequence number 293590838           LSN1事務創建時一條日誌
Log flushed up to   293590838
Pages flushed up to 293590838
Last checkpoint at  293590829
0 pending log flushes, 0 pending chkp writes
1139 log i/o's done, 0.00 log i/o's/second
{% endhighlight %}

如上的信息是在 log_print() 函數中打印。

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

### 日誌保護機制

InnoDB 中 LSN 是單調遞增的，而日誌文件大小卻是固定的，所以在寫入的時候通過取餘來計算偏移量，這樣存在兩個 LSN 寫入到同一位置的可能，如果日誌被覆蓋，而數據也沒有刷盤，一旦宕機，數據就丟失了。

為此，InnoDB 實現了一套日誌保護機制，詳細實現如下。

![checkpoint redo buffer protect]({{ site.url }}/images/databases/mysql/checkpoint-redo-buffer-protect.png "checkpoint redo buffer protect"){: .pull-center }

首先，明確下概念，上述的 buf 是指 redo log buffer，而 ckp 實際上與 buffer pool 相關，也就是髒頁的刷髒。上述直線表示 redo log 的空間，會乘 0.9 的安全係數。

* Ckp age (LSN1- LSN4) 還沒有做 Checkpoint 的日誌範圍，若超過日誌空間，說明被覆蓋的日誌可能還沒有刷到磁盤，而其 BP 中對應的數據 (髒頁) 肯定沒有刷到磁盤上；
* Buf age (LSN1- LSN3) 髒頁對應的日誌還沒有刷盤的範圍，若超過日誌空間，說明被覆蓋的日誌及其 BP 中對應數據肯定還沒有刷到磁盤；

<!--
* Buf async (日誌空間 * 7/8) 強制將 Buf age-Buf async 的髒頁刷盤，此時事務還可以繼續執行，所以為async，對事務的執行速度沒有直接影響（有間接影響，例如CPU和磁盤更忙了，事務的執行速度可能受到影響）
* Buf sync    日誌空間大小 * 15/16    強制將2*(Buf age-Buf async)的髒頁刷盤，此時事務停止執行，所以為sync，由於有大量的髒頁刷盤，因此阻塞的時間比Ckp sync要長。
* Ckp async   日誌空間大小 * 31/32    強制寫Checkpoint，此時事務還可以繼續執行，所以為async，對事務的執行速度沒有影響（間接影響也不大，因為寫Checkpoint的操作比較簡單）
* Ckp sync    日誌空間大小 * 64/64    強制寫Checkpoint，此時事務停止執行，所以為sync，但由於寫Checkpoint的操作比較簡單，即使阻塞，時間也很短
-->

當事務執行速度大於刷髒速度時，Ckp age和Buf age (innodb_flush_log_at_trx_commit!=1時) 都會逐步增長，當達到 async 點的時候，強制進行寫 redo-log 或者寫 Checkpoint，如果這樣做還是趕不上事務執行的速度，則為了避免數據丟失，到達 sync 點的時候，會阻塞其它所有的事務，專門進行 redo-log 刷盤或者寫 Checkpoint。

也就是說，只要事務執行速度大於髒頁刷盤速度，最終都會觸發日誌保護機制，進而將事務阻塞，導致 MySQL 操作掛起。

<!--
由於寫Checkpoint本身的操作相比寫髒頁要簡單，耗費時間也要少得多，且Ckp sync點在Buf sync點之後，因此絕大部分的阻塞都是阻塞在了Buf sync點，這也是當事務阻塞的時候，IO很高的原因，因為這個時候在不斷的刷髒頁數據到磁盤。例如如下截圖的日誌顯示了很多事務阻塞在了Buf sync點：
-->

## 源碼解析

### 臨界範圍計算

如下是相關的變量以及臨界值的計算函數。

{% highlight text %}
----- 相關變量
innodb_log_buffer_size         = 16777216 = 16M
innodb_log_file_size           = 50331648 = 48M
innodb_log_files_in_group      = 2
innodb_flush_log_at_trx_commit = 1
innodb_thread_concurrency      = 0

----- 計算臨界函數調用棧
innobase_start_or_create_for_mysql()
 |-log_group_init()
   |-log_calc_max_ages()                計算臨界範圍
{% endhighlight %}

接下來看看臨界值是如何計算的。

{% highlight cpp %}
bool log_calc_max_ages(void)
{
    log_mutex_enter();
    group = UT_LIST_GET_FIRST(log_sys->log_groups);

    // 設置redo-log的最大磁盤空間，也就是64-bits正整數的最大值
    smallest_capacity = LSN_MAX;

    // 5.7實際只支持一個分組，獲取的是除了頭(LOG_FILE_HDR_SIZE)之外的總redo-log空間大小
    while (group) {
        if (log_group_get_capacity(group) < smallest_capacity) {
            smallest_capacity = log_group_get_capacity(group);
        }
        group = UT_LIST_GET_NEXT(log_groups, group);
    }

    // 實際真正可以使用的空間需要乘以一個安全係數0.9
    smallest_capacity = smallest_capacity - smallest_capacity / 10;

    // 為每個OS線程預留一部分存儲空間
    free = LOG_CHECKPOINT_FREE_PER_THREAD * (10 + srv_thread_concurrency)
        + LOG_CHECKPOINT_EXTRA_FREE;
    if (free >= smallest_capacity / 2) {  // 需要預留足夠的內存空間
        success = false;
        goto failure;
    } else {
        margin = smallest_capacity - free;
    }

    // 好吧，再預留一部分內存空間
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

關於邊界劃分可以簡單查看下圖。

![checkpoint max ages]({{ site.url }}/images/databases/mysql/innodb-checkpoint-max-ages.png "checkpoint max ages"){: .pull-center width="90%"}


### 邊界檢查

對於 ```max_modified_age_async``` 變量，也就是異步刷新，會在 page cleaner 線程中檢查<!--，在此暫時就不做過多介紹了，在後面會做詳細介紹 -->。

{% highlight text %}
#define PCT_IO(p) ((ulong) (srv_io_capacity * ((double) (p) / 100.0)))

buf_flush_page_cleaner_coordinator()            ← 該函數基本上每秒調用一次
 |-buf_flush_page_cleaner_coordinator()
   |-page_cleaner_flush_pages_recommendation()
     |-log_get_lsn()                            ← 獲取當前lsn，也就是log_sys->lsn
     |-af_get_pct_for_dirty()                   ← 是否需要刷新多個頁，返回IO-Capacity的百分比
     | |-buf_get_modified_ratio_pct()
     |   |-buf_get_total_list_len()
     |
     |-af_get_pct_for_lsn()                     ← 計算是否需要進行異步刷redo-log，返回IO-Capacity的百分比
     | |-log_get_max_modified_age_async()       ← 獲取max_modified_age_async
     |
     |-ut_max()                                 ← 獲取上述兩個返回值的最大值
{% endhighlight %}

<!--
至於異步刷髒，log_sys->max_modified_age_async被封裝在函數log_get_max_modified_age_async中， 被函數af_get_pct_for_lsn。顯而易見，異步刷髒是由page cleaner線程來完成的。

在函數af_get_pct_for_lsn中，根據當前的LSN，計算需要以IO capacity的百分之幾來刷髒

噹噹前lsn-buf_pool_get_oldest_modification()超過log_sys->max_modified_age_async時：

age = log_sys->lsn – buf_pool_get_oldest_modification()
lsn_age_factor = (age *100)/log_sys->max_modified_age_async )

返回值為：
pct =   [
                 (innodb_io_capacity_max/innodb_io_capacity)
                 *
                 (lsn_age_factor * sqrt((double)lsn_age_factor))
          ] /7.5


另外，如果參數innodb_adaptive_flushing設置為OFF，且沒有超過log_get_max_modified_age_async()的話，直接返回0

Page cleaner線程會同時根據LSN 和髒頁比例來獲取pct，並取其中的最大值。
-->


除了 ```max_modified_age_async``` 變量之外，其它相關的變量都會在 ```log_checkpoint_margin()``` 函數中進行比較，詳細內容可以直接查看如下函數。

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

    // 判斷是否需要執行flush或者checkpoint，不需要則直接返回
    if (!log->check_flush_or_checkpoint) {
        log_mutex_exit();
        return;
    }

    // 找出當前所有buffer pool實例中最老的LSN，實際上直接讀取每個flush_list的尾部即可
    oldest_lsn = log_buf_pool_get_oldest_modification();

    // 如果計算的age大於max_modified_age_sync，則需要做一次同步刷新
    age = log->lsn - oldest_lsn;
    if (age > log->max_modified_age_sync) {
        /* A flush is urgent: we have to do a synchronous preflush */
        advance = age - log->max_modified_age_sync;
    }

    // 計算checkpoint_age，並判斷是否需要做checkpoint以及是否需要同步
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
        // 需要同步刷新，則將LSN推進到新的LSN位置
        success = log_preflush_pool_modified_pages(new_oldest);

        // 如果失敗說明有其它的線程在處理
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
另外，這幾個變量在函數log_close中會被用到，它會去做一件重要的事情：設置log_sys->check_flush_or_checkpoint。
-->

在用戶線程中，會調用 ```log_free_check()``` 函數檢查是否需要將日誌刷新到磁盤。

{% highlight text %}
log_free_check()
 |-log_check_margins()
   |-log_write_up_to()
{% endhighlight %}

在 ```log_check_margins()``` 函數中，會檢查 ```log_sys->buf_free > log->max_buf_free```，如果成立則會執行日誌刷盤操作。


### 檢查點寫入

一般會通過調用 ```log_checkpoint()``` 函數完成 checkpoint 的寫入，需要注意的是，該函數中只會完成 checkpoint 的寫入，並不會刷髒頁。

當然也可以調用 ```log_make_checkpoint_at()``` 完成刷髒以及 checkpoint 的寫入。

{% highlight text %}
log_make_checkpoint_at()
 |-log_preflush_pool_modified_pages()
 |-log_checkpoint()                         ← 並不從BP中刷髒頁，只檢查BP中的最大LSN，然後刷新到磁盤
   |-log_mutex_enter()                      ← 持有log_sys->mutex鎖
   |-log_buf_pool_get_oldest_modification()
   | |-buf_pool_get_oldest_modification()   ← 遍厲所有BP實例，獲取最大lsn，之前都已經寫入磁盤
   |
   |-fil_names_clear()
   | |-mtr_t::commit_checkpoint()
   |
   |-log_write_up_to()
   |
   |-log_write_checkpoint_info()
     |-log_group_checkpoint()               ← 將checkpoint信息寫入redolog頭部，兩個寫入點輪流寫入
{% endhighlight %}

checkpoint 信息分別保存在 ib_logfile0 的 512 字節和 1536(3*512) 字節處，每個 checkpoint 默認大小為 512 字節，當然，其中很大一部分是空白，詳細可以參考如下函數。

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

InnoDB 的 checkpoint 主要有 3 部分信息組成：

* checkpoint no<br>每次寫入都會遞增，用於輪流寫入 redo log 的頭部的兩部分，可以通過該值判斷那個比較新；
* checkpoint lsn<br>記錄了產生該 checkpoint 時 log_sys->next_checkpoint_lsn 是 flush 的 LSN，確保在該 LSN 前面的數據頁都已經落盤，不再需要通過 redo log 進行恢復；
* checkpoint offset<br>記錄了該 checkpoint 產生時，redo log 在 ib_logfile 中的偏移量，通過該值就可以找到需要恢復的 redo log 開始位置。

每次在啟動時，都會嘗試讀取兩個值，並比較兩者，獲取較大的值。

<!--
## 參考

http://tech.uc.cn/?p=716

Checkpoint原理
http://hedengcheng.com/?p=88

關於checkpoint機制
http://www.cnblogs.com/chenpingzhao/p/5107480.html

-->


{% highlight text %}
{% endhighlight %}
