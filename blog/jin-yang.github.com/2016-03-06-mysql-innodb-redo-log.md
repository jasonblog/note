---
title: InnoDB Redo Log
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,redo log,重做日誌
description:
---

當事務需要修改某條記錄時，會先記錄到 redo log，在此介紹下其實現。

<!-- more -->

## 簡介

InnoDB 有 Buffer Pool 也就是數據庫的頁面緩存，對數據頁的任何修改都會先在 BP 上修改，然後這樣的頁面將被標記為 dirty 並被放到專門的 flush list 上，後續將由 master thread 或專門的刷髒線程階段性的將這些頁面寫入磁盤。

這樣帶來的好處是避免每次寫操作都會導致大量的隨機 IO，階段性的刷髒可以將多次對頁面的修改合併成一次 IO 操作，同時異步寫入也降低了訪問的時延。

然而，如果在髒頁未刷入磁盤時，服務器或者進程非正常關閉，將會導致這些修改操作丟失，如果寫入操作正在進行，甚至會由於損壞數據文件導致數據庫不可用。

為了避免上述問題，InnoDB 會將所有對頁面的修改操作寫入一個特定的文件，也就是 redolog，並在數據庫啟動時從此文件進行恢復操作。這樣推遲了 BP 頁面的刷髒，提升了數據庫的吞吐，有效的降低了訪問時延；帶來的問題是額外的寫 redo log 操作的開銷 (順序 IO 速度較快)，以及數據庫啟動時恢復操作所需的時間。

接下來將結合 MySQL 代碼看下 Log 文件的結構、生成過程以及數據庫啟動時的恢復流程。

### LSN

LSN(Log Sequence Number，日誌序列號，ib_uint64_t) 保存在 log_sys.lsn 中，在 log_init() 中初始化，初始值為 LOG_START_LSN(8192) 。改值實際上對應日誌文件的偏移量，新的 LSN＝舊的LSN + 寫入的日誌大小，在調用日誌寫入函數時，LSN 就一直隨著寫入的日誌長度增加。

{% highlight text %}
#define OS_FILE_LOG_BLOCK_SIZE      512
#define LOG_START_LSN       ((lsn_t) (16 * OS_FILE_LOG_BLOCK_SIZE))
{% endhighlight %}

日誌通過 log_write_low()@log/log0log.c 函數寫入。

{% highlight cpp %}
void log_write_low(byte* str, ulint str_len)
{
    log_t* log = log_sys;
part_loop:
    ... ...                                        // 計算寫入日誌的長度
    ut_memcpy(log->buf + log->buf_free, str, len); // 將日誌內容拷貝到log buffer
    ... ...
    log->lsn += len;
}
{% endhighlight %}

如上所述，LSN 是不會減小的，它是日誌位置的唯一標記，在重做日誌寫入、checkpoint 構建和 PAGE 頭裡面都有 LSN。

例如當前重做日誌的 LSN=2048，這時候調用 log_write_low() 寫入一個長度為 700 的日誌，2048 剛好是 4 個 block 長度，那麼需要存儲 700 長度的日誌，需要兩個 block(單個block只能存496個字節)，那麼很容易得出新的 LSN 為。

{% highlight text %}
LSN=2048+700+2*LOG_BLOCK_HDR_SIZE(12)+LOG_BLOCK_TRL_SIZE(4)=2776
{% endhighlight %}

<!--
lsn是聯繫dirty page、redo log record和redo log file的紐帶，每個redo log record被拷貝到內存的log buffer時會產生一個相關聯的lsn，而每個頁面修改時會產生一個log record，從而每個數據庫的page也會有一個相關聯的lsn，這個lsn記錄在每個page的header字段中。為了保證WAL（Write-Ahead-Logging）要求的邏輯，dirty page要求其關聯lsn的log record已經被寫入log file才允許執行flush操作。<br><br>

從上面的結構定義可以看出有很多LSN相關的定義，那麼這些LSN直接的關係是怎麼樣的呢？理解這些LSN之間的關係對理解整個重做日誌系統的運作機理會有極大的信心。以下各種LSN的解釋：
<ul><li>
lsn<br>
當前log系統最後寫入日誌時的LSN。</li><br><li>

flush_lsn<br>
redo-log buffer最後一次數據刷盤數據末尾的LSN，作為下次刷盤的起始LSN。</li><br><li>

written_to_some_lsn<br>
單個日誌組最後一次日誌刷盤時的起始LSN。</li><br><li>

written_to_all_lsn<br>
所有日誌組最後一次日誌刷盤是的起始LSN。

 last_checkpoint_lsn        最後一次建立checkpoint日誌數據起始的LSN

 next_checkpoint_lsn        下一次建立checkpoint的日誌    數據起始的LSN,用log_buf_pool_get_oldest_modification獲得的

 archived_lsn               最後一次歸檔日誌數據起始的LSN
 next_archived_lsn          下一次歸檔日誌數據的其實LSN
</li></ul>



b)        文件為順序寫入，當達到最後一個文件末尾時，會從第一個文件開始順序複用。

c)        lsn: Log Sequence Number，是一個遞增的整數。 Ib_logfile中的每次寫入操作都包含至少1個log，每個log都帶有一個lsn。在內存page修復過程中，只有大於page_lsn的log才會被使用。

d)        lsn的保存在全局變量log_sys中。遞增數值等於每個log的實際內容長度。即如果新增的一個log長度是len，則log_sys->lsn += len.

e)        ib_logfile每次寫入以512（OS_FILE_LOG_BLOCK_SIZE）字節為單位。實際寫入函數 log_group_write_buf (log/log0log.c)

f)         每次寫盤後是否flush，由參數innodb_flush_log_at_trx_commit控制。

在 innodb 中，維護了一個全局變量 struct log_struct log_sys ，
written_to_all_lsn
n表示實際已經寫盤的lsn。需要這個值是因為並非每次生成log後就寫盤。
flushed_to_disk_lsn
表示刷到磁盤的lsn。需要這個值是因為並非每次寫盤後就flush。
buf
待寫入的內容保存在buf中
buf_size
buf的大小。由配置中innodb_log_buffer_size決定，實際大小為innodb_log_buffer_size /16k * 16k。
buf_next_to_write
buf中下一個要寫入磁盤的位置
buf_free
buf中實際內容的最後位置。當buf_free> buf_next_to_write時，說明內存中還有數據未寫盤。



log 採用循環寫法，
有三種情況――overflow， checkpoint和commit――可以導致寫盤操作
如果data buffer滿了，InnoDB將最近使用的buffer寫入到數據庫中，但是不可能足夠的快。這種情況下，頁頭的LSN就起作用了。第一，InnoDB檢查它的LSN是否比log文件中最近的log記錄的LSN大，只有當log趕上了data的時候，才會將數據寫到磁盤。換句話說，數據頁不會寫盤，直到相應的log記錄需要寫盤的時候。這就是先寫日誌策略。
-->

### 變量設置

簡單來說 InnoDB 通過兩個核心參數 ```innodb_buffer_pool_size```、```innodb_log_file_size```，分別定義了數據緩存和 redolog 的大小，而後者的大小也決定了 BP 中可以有多少髒頁。當然，也不能因此就增大 redolog 文件的大小，這樣，可能會導致系統啟動時 Crash Recovery 時間增大。

redolog 保存在 ```innodb_log_group_home_dir``` 參數指定的目錄下，文件名為 ib_logfile\*；undolog 保存在共享表空間 ibdata\* 文件中。

redolog 由一組固定大小的文件組成，順序寫入，而且文件循環使用，文件名為 ib_logfileN (其中N為從0開始的數字)，可以通過 ```innodb_log_file_size``` 和 ```innodb_log_files_in_group``` 參數控制文件的大小和數目，日誌總大小為兩者之積。

#### innodb_log_file_size

簡單來說，該變量設置時至少要保證 redo log 在峰值的時候可以容納 1 小時的日誌，當前的寫入值可以通過如下方式查看。

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

其實，通過上述的兩個變量查看的值會有些區別，一般後者的計算值會偏大，嚴格來說：LSN 是在寫入 log buffer 時遞增；而 ```Innodb_os_log_written``` 則是在寫入文件時遞增的。

而將 redo log buffer 中的內容寫入到文件的時候是以 512 字節為單位來寫的，而且會覆蓋寫。

簡單舉例來說：

1. 事務 A 寫了 100B 日誌，此時 LSN 的值增加 100，而刷到磁盤時，會從某個偏移開始寫入 512B，也就是說 innodb_os_log_written 增加 512 ，只是這 512B 本次有效的只有 100B 。

2. B 寫了 200B 日誌，此時 LSN 的值增加 200，刷磁盤時 innodb_os_log_written 又增加 512 。

也就是說最好參考 ```SHOW ENGINE INNODB STATUS``` 變量中的 ```Log sequence number``` 計算值。

<!-- https://www.percona.com/blog/2012/10/08/measuring-the-amount-of-writes-in-innodb-redo-logs/ -->

#### innodb_log_buffer_size

該參數就是用來設置 InnoDB 的 Log Buffer 大小，系統默認值為 16MB，主要作用就是緩衝 redo log 數據，增加緩存可以使大事務在提交前不用寫入磁盤，從而提高寫 IO 性能。

可以通過系統狀態參數，查看性能統計數據來分析 Log 的使用情況：

{% highlight text %}
mysql> SHOW STATUS LIKE 'innodb_log%';
+---------------------------+-------+
| Variable_name             | Value |
+---------------------------+-------+
| Innodb_log_waits          | 0     |  由於緩存過小，導致事務必須等待的次數
| Innodb_log_write_requests | 30    |  日誌寫請求數
| Innodb_log_writes         | 21    |  向日志文件的物理寫次數
+---------------------------+-------+
3 rows in set (0.03 sec)
{% endhighlight %}

<!--
通過這三個狀態參數我們可以很清楚的看到 Log Buffer 的等待次數等性能狀態。

當然，如果完全從 Log Buffer 本身來說，自然是大一些會減少更多的磁盤 IO。但是由於 Log 本身是 為了保護數據安全而產生的，而 Log 從 Buffer 到磁盤的刷新頻率和控制數據安全一致的事務直接相關， 並且也有相關參數來控制（innodb_flush_log_at_trx_commit），所以關於 Log 相關的更詳細的實現機 制和優化在後面的“事務優化”中再做更詳細的分析，這裡就不展開了。

innodb_additional_mem_pool_size 參數理解

innodb_additional_mem_pool_size 所設置的是用於存放 Innodb 的字典信息和其他一些內部結構所 需要的內存空間。所以我們的 Innodb 表越多，所需要的空間自然也就越大，系統默認值僅有 1MB。當 然，如果 Innodb 實際運行過程中出現了實際需要的內存比設置值更大的時候，Innodb 也會繼續通過 OS 來申請內存空間，並且會在 MySQL 的錯誤日誌中記錄一條相應的警告信息讓我們知曉。

從我個人的經驗來看，一個常規的幾百個 Innodb 表的 MySQL，如果不是每個表都是上百個字段的 話，20MB 內存已經足夠了。當然，如果你有足夠多的內存，完全可以繼續增大這個值的設置。實際上， innodb_additional_mem_pool_size 參數對系統整體性能並無太大的影響，所以只要能存放需要的數據即 可，設置超過實際所需的內存並沒有太大意義，只是浪費內存而已。
-->


## 文件結構

先明確下常用概念，每個日誌組都會在第一個文件中有頭部信息，通過 LOG_FILE_HDR_SIZE 宏定義，也就是 4*OS_FILE_LOG_BLOCK_SIZE(512) 。

redolog 寫入通常是以 ```OS_FILE_LOG_BLOCK_SIZE``` (512字節，編譯時設置) 為單位順序寫入文件，每個 LogBlock (512B) 包含了一個 header 段、一個 tailer 段、一組 LogRecords (多條記錄組成)；每條記錄都有自己的 LSN，表示從日誌記錄創建開始到特定的日誌記錄已經寫入的字節數。

接著看看各個部分的定義。

### 頭部信息

頭部信息，在 log0log.h 文件中定義，簡單看下常用的保存字段。

{% highlight text %}
LOG_HEADER_FORMAT
  之前為LOG_GROUP_ID，用於標示redolog的分組ID，但是現在只支持一個分組；
LOG_HEADER_START_LSN
  日誌文件記錄的初始LSN，佔用8字節，注意其偏移量與老版本有所區別；
LOG_HEADER_CREATOR
  備份程序會將填寫備份程序和創建時間，通常是使用xtrabackup、mysqlbackup時會填充；
LOG_CHECKPOINT_1/2
  在頭文件中定義的checkpoint位置，注意只有日誌組了的第一文件會有該記錄，每次checkpoint都會更新這兩個值；
{% endhighlight %}

塊的宏偏移量同樣在 log0log.h 文件中定義，其中頭部包括了 LOG_BLOCK_HDR_SIZE(12) 個字節，詳細如下：

{% highlight text %}
LOG_BLOCK_HDR_NO
  四字節標示這是第幾個block塊，該值是通過LSN計算得來，詳見log_block_convert_lsn_to_no()函數；
LOG_BLOCK_HDR_DATA_LEN
  兩字節表示該block中已經有多少字節被使用；
LOG_BLOCK_FIRST_REC_GROUP
  兩個字節表示該block中作為一個新的MTR開始log
LOG_BLOCK_CHECKPOINT_NO
  四個字節表示該block的checkpoint，也就是log_sys->next_checkpoint_no；
LOG_BLOCK_CHECKSUM
  四個字節記錄了該block的校驗值，通過innodb_checksum_algorithm變量指定算法；
{% endhighlight %}


<!--
innodb_fast_shutdown

innodb_force_recovery
innodb_force_recovery_crash

????LSN對應了日誌文件的偏移量，為了減小故障恢復時間，引入了Checkpoint機制，

InnoDB在啟動時會自動檢測InnoDB數據和事務日誌是否一致，是否需要執行相應的操作？？？保證數據一致性；當然，故障恢復時間與事務日誌的大小相關。
-->

## 源碼解析

在此涉及到兩塊內存緩衝，包括了 mtr_t 、log_sys 等內部結構，接下來一一介紹。

### 數據結構

首先看下 InnoDB 在內存中保存的一個全局變量，也就是 ```log_t* log_sys=NULL;``` 定義，其維護了一塊稱為 log buffer 的全局內存區域，也就是 ```log_sys->buff```，同時維護有若干 lsn 值等信息表示 logging 進行的狀態；會在 ```log_init()``` 中對所有的內部區域進行分配並對各個變量進行初始化。

變量 log_sys 是 InnoDB 日誌系統的中樞及核心對象，控制著日誌的拷貝、寫入、checkpoint 等核心功能，是連接 InnoDB 日誌文件及 log buffer 的樞紐。

接下來，簡單介紹下 log_t 中比較重要的字段值：

{% highlight cpp %}
struct log_t{
  char        pad1[CACHE_LINE_SIZE];
  lsn_t       lsn;                    // 接下來將要生成的log record使用此lsn的值；
  ulint       buf_size;               // buf大小，通過innodb-log-buffer-size變量指定
  ulint       buf_free;               // 下條log record寫入的位置
  ulint       max_buf_free;           // 當buf_free超過該值後，需要進行刷新，可以查看log_free_check()函數
  byte*       buf;                    // 全局的log buffer
  ulint       buf_next_to_write;      // 下條記錄寫入到磁盤的偏移量
  lsn_t       write_lsn;              // 已經寫入的LSN
  lsn_t       flushed_to_disk_lsn;    // 已經刷新到磁盤的LSN值，小於該值的日誌已經被安全地記錄到了磁盤上

  UT_LIST_BASE_NODE_T(log_group_t)    // 日誌組，當前版本僅支持一組日誌
          log_groups;                 // 包含了當前日誌組的文件個數、每個文件的大小、space id等信息

  lsn_t       log_group_capacity;     // 當前日誌文件的總容量在log_calc_max_ages()中計算，
                                      // (redo-log文件大小-頭部)*0.9，其中乘0.9是一個安全係數

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
上述結構體中的很多值會在初始化的時候進行設置，調用流程如下。

So log_sys->buf_next_to_write is between 0 and log_sys->buf_free, log_sys->write_lsn is equal or less log_sys->lsn, log_sys->flushed_to_disk_lsn is less or equal to log_sys->write_lsn.


log_sys->buf_free
 
寫入buffer的起始偏移量


log_sys->write_lsn
 當前正在執行的寫操作使用的臨界lsn值；
log_sys->current_flush_lsn
 當前正在執行的write + flush操作使用的臨界lsn值，一般和log_sys->write_lsn相等；

log_sys->buf_next_to_write
 
buffer中還未寫到log file的起始偏移量。下次執行write+flush操作時，將會從此偏移量開始
log_sys->max_buf_free
 
確定flush操作執行的時間點，當log_sys->buf_free比此值大時需要執行flush操作，具體看log_check_margins函數


lsn是聯繫dirty page，redo log record和redo log file的紐帶。在每個redo log record被拷貝到內存的log buffer時會產生一個相關聯的lsn，而每個頁面修改時會產生一個log record，從而每個數據庫的page也會有一個相關聯的lsn，這個lsn記錄在每個page的header字段中。為了保證WAL（Write-Ahead-Logging）要求的邏輯，dirty page要求其關聯lsn的log record已經被寫入log file才允許執行flush操作。
-->



### 日誌生成

mtr (mini-transactions) 在代碼中對應了 ```struct mtr_t``` 結構體，其內部有一個局部 buffer，會將一組 log record 集中起來，然後批量寫入 log buffer；mtr_t 的結構體如下所示：

{% highlight text %}
struct mtr_t {
  struct Impl {
    mtr_buf_t  m_memo;  // 由此mtr涉及的操作所造成的髒頁列表
    mtr_buf_t  m_log;   // mtr的局部緩存，記錄log-records；
  };
};
{% endhighlight %}

其中 m_memo 對象會記錄與本次事務相關的頁以及鎖信息，並在提交時 (複製到 log buffer) 之後將髒頁添加到 flush_list 並釋放所持有的鎖，詳細的內容可以參考 ```mtr_commit()->mtr_memo_pop_all()``` 函數調用。

簡單來說，log record 的生成過程如下：

1. 創建一個 mtr_t 類型的對象；
2. 執行 ```mtr_start()``` 初始化 mtr_t 的字段，包括 local buffer；
3. 在對 BP 中的 Page 進行修改的同時，調用 ```mlog_write_ulint()```、```mlog_write_string()```、```mlog_write_null()``` 類似函數，生成 redo log record 並保存在 local buffer 中；
4. 執行 ```mtr_commit()``` 將 local buffer 中的日誌拷貝到全局的 ```log_sys->buf+log_sys->buf_free```，同時將髒頁添加到 flush list，供後續執行 flush 操作時使用。

對於這一過程的執行，可以將 ```page_cur_insert_rec_write_log()``` 函數作為參考。



{% highlight text %}
mtr_commit()                          提交mtr，對應了mtr_t::commit()@mtr0mtr.cc
 |-mtr_t::Command::execute()          執行，同樣在mtr0mtr.cc文件中
   |-mtr_t::Command::prepare_write()  準備寫入，會返回字節數
     |-fil_names_write_if_was_clean()
       |-fil_names_dirty_and_write()  如果在fil_names_clear()之後這是第一次寫入
{% endhighlight %}





<!--
## 參考


Innodb redo log archiving
https://www.percona.com/blog/2014/03/28/innodb-redo-log-archiving/
-->



{% highlight text %}
{% endhighlight %}
