---
title: InnoDB 崩潰恢復
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,crash recovery,崩潰恢復
description: 如果 InnoDB 沒有正常關閉，會在服務器啟動的時候執行崩潰恢復 (Crash Recovery)，這一流程比較複雜，涉及到了 redo log、undo log 甚至包括了 binlog 。在此簡單介紹下 InnoDB 崩潰恢復的流程。
---

如果 InnoDB 沒有正常關閉，會在服務器啟動的時候執行崩潰恢復 (Crash Recovery)，這一流程比較複雜，涉及到了 redo log、undo log 甚至包括了 binlog 。

在此簡單介紹下 InnoDB 崩潰恢復的流程。

<!-- more -->

## 簡介

如果 InnoDB 意外宕機了，那麼會不會丟數據？

當然，這一問題比較複雜，根據不同的情況，可能會有數據丟失，不過至少有一點可以肯定，不會導致全部數據丟失。而這一過程，便涉及到了數據恢復。

### 初始化

在 MySQL 的主函數中，最終會通過 plugin_init() 對插件進行初始化，此時，會依次調用各個插件的初始化函數，同時也會調用 InnoDB 對應的初始化函數。

詳細的調用流程如下。

{% highlight text %}
mysqld_main()
 |-init_server_components()
   |-plugin_init()
     |-plugin_initialize()
       |-ha_initialize_handlerton()
         |-innobase_init()
           |-innobase_start_or_create_for_mysql()
{% endhighlight %}

InnoDB 崩潰恢復相關的入口是 ```innobase_start_or_create_for_mysql()``` 函數。首先，InnoDB 會檢查上次數據庫是否正常關閉，如果是則不需要恢復，否則就進入崩潰恢復的流程。

<!--
這裡把InnoDB的恢復過程主要劃分為兩個階段，第一階段主要依賴於redo log的恢復，而第二階段，恰恰需要binlog和undo log的共同參與，接下來，我們來具體瞭解下整個恢復的過程。
-->

### 系統檢查

數據庫啟動後，InnoDB 會通過 ```read_lsn_and_check_flags()``` 函數讀取系統表空間中 flushed_lsn ，這一個 LSN 只在系統表空間的第一個頁中存在，而且只有在正常關閉的時候寫入。

系統正常關閉時，會調用 ```logs_empty_and_mark_files_at_shutdown() -> fil_write_flushed_lsn()``` ，也就是在執行一次 sharp checkpoint 之後，將 LSN 寫入。

> flushed_lsn 只有在系統表空間的第一頁存在，偏移量為 FIL_PAGE_FILE_FLUSH_LSN(26)，也就是保證至少在此 LSN 之前的頁已經刷型到磁盤。

另外需要注意的是，寫 flushed_lsn 時會同時寫入到 Double Write Buffer，如果 flushed_lsn 對應的頁損壞，則可以從 dbwl 中進行恢復。

<br>

接下來，InnoDB 會通過 redo-log 日誌找到最近一次提交的 checkpoint，讀取該 checkpoint 對應的 LSN 。其中，checkpoint 信息會保存在 redo-log 的第一個文件中，在兩個固定偏移中輪流寫入；所以，需要同時讀取兩個，並比較獲取較大的一個值。

比較獲得的 flushed_lsn 以及 checkpoint_lsn ，如果兩者相同，則說明正常關閉；否則，就需要進行故障恢復。


## 重做日誌

簡單來說，如果需要執行崩潰恢復，會從上述讀取的 checkpoint 信息，直接找到 redo-log 文件中相應的偏移量，也就是從 checkpoint 指定的位置開始讀取日誌，並保存到一個哈希表中，最後通過遍歷哈希表中的 redo log 信息，讀取相關頁進行恢復。

### 日誌掃描

假設，從上述 checkpoint 定位到開始恢復的 redo log 位置是在 ib_logfile1 文件中的某個位置，那麼整個 redo log 掃描的過程可能是這樣的：

* 從 ib_logfile1 的指定位置開始讀取 redo log，每次讀取 RECV_SCAN_SIZE (4*page_size=64k) 大小，寫入時是以 block(512B) 為單位；

* 將從文件中讀取的日誌保存在 recv_sys->buf 中，然後進行校驗，並解析日誌，然後將結果保存在以 (space, page_no) 做 key 的 recv_sys->addr_hash 表中，這樣一個 key 就對應了一個數據頁的修改；

<!--
* 同時保存在恢復緩存中的redo信息會根據space，offset計算一個哈希值後保存到一個哈希表(recv_sys->addr_hash)中，相同的哈希值不同(space，offset)用鏈表存儲，相同的(space,offset)用列表保存，可能部分事務比較大，redo信息一個block不能保存，所以，每個body中可以用鏈表鏈接多body的值
-->

redo log 被保存到哈希表中之後，InnoDB 就可以開始進行數據恢復，只需要輪詢哈希表中的每個節點獲取 redo 信息，根據 (space, page_no) 讀取指定的數據頁，並進行日誌覆蓋。

### 優化

如上，在恢復時，需要獲取 space id 與 *.ibd 文件的對應關係，這就需要打開所有的 ibd 文件獲取，<!-- TODODO: 確認是否如此，是否有固定位置保存 sapce-id 與 ibd 文件的對應關係 -->如果文件有成百上千，甚至以萬計的時候，那麼這一操作將會非常耗時。

為此，5.7 在 redo log 中添加了兩個新的類型：```MLOG_FILE_NAME``` 記錄在 checkpoint 之後，所有被修改過的信息(space, filepath)；```MLOG_CHECKPOINT``` 用於標誌 ```MLOG_FILE_NAME``` 的結束。

<!--
上面兩種redo log類型的添加，完美解決了前面遺留的問題，redo log中保存了後續需要恢復的space和filepath對，所以，在恢復的時候，只需要從checkpoint的位置往後掃描到MLOG_CHECKPOINT的位置，這樣就能獲取到需要恢復的space和filepath，在恢復過程中，只需要打開這些ibd文件即可，當然由於space和filepath的對應關係通過redo存了下來，恢復的時候也不再依賴數據字典。
這裡需要強調的一點就是MLOG_CHECKPOINT在每個checkpoint點中最多隻存在一次，如果出現多次MLOG_CHECKPOINT類型的日誌，則說明redo已經損壞，InnoDB會報錯。最多存在一次，那麼會不會有不存在的情況？答案是肯定的，在每次checkpoint過後，如果沒有發生數據更新，那麼MLOG_CHECKPOINT就不會被記錄。所以只要簡單查找下redo log最新一個checkpoint後的MLOG_CHECKPOINT是否存在，就能判定上次MySQL是否正常關機。5.7版本的MySQL在InnoDB進行恢復的時候，也正是這樣做的，MySQL 5.7在進行恢復的時候，一般情況下需要進行最多3次的redo log掃描：

    第一次redo log的掃描，主要是查找MLOG_CHECKPOINT，不進行redo log的解析，如果沒有找到MLOG_CHECKPOINT，則說明InnoDB不需要進行recovery，後面的兩次掃描可以省略，如果找到了MLOG_CHECKPOINT，則獲取MLOG_FILE_NAME到指定列表，後續只需打開該鏈表中的表空間即可。
    第二次掃描是在第一次找到MLOG_CHECKPOINT基礎之上進行的，該次掃描會把redo log解析到哈希表中，如果掃描完整個文件，哈希表還沒有被填滿，則不需要第三次掃描，直接進行recovery就結束
    第三次掃描是在第二次基礎上進行的，第二次掃描把哈希表填滿後，還有redo log剩餘，則需要循環進行掃描，哈希表滿後立即進行recovery，直到所有的redo log被apply完為止。

redo log全部被解析並且apply完成，整個InnoDB recovery的第一階段也就結束了，在該階段中，所有已經被記錄到redo log但是沒有完成數據刷盤的記錄都被重新落盤。然而，InnoDB單靠redo log的恢復是不夠的，這樣還是有可能會丟失數據(或者說造成主從數據不一致)，因為在事務提交過程中，寫binlog和寫redo log提交是兩個過程，寫binlog在前而redo提交在後，如果MySQL寫完binlog後，在redo提交之前發生了宕機，這樣就會出現問題：binlog中已經包含了該條記錄，而redo沒有持久化。binlog已經落盤就意味著slave上可以apply該條數據，redo沒有持久化則代表了master上該條數據並沒有落盤，也不能通過redo進行恢復。這樣就造成了主從數據的不一致，換句話說主上丟失了部分數據，那麼MySQL又是如何保證在這樣的情況下，數據還是一致的？這就需要進行第二階段恢復。
-->



## 源碼分析

InnoDB 的數據恢復是一個很複雜的過程，在其恢復過程中，需要 redolog、binlog、undolog 等參與，接下來從源碼角度具體瞭解下整個恢復的過程。

{% highlight text %}
innobase_init()
 |-innobase_start_or_create_for_mysql()
   |
   |-recv_sys_create()   創建崩潰恢復所需要的內存對象
   |-recv_sys_init()
   | |-hash_create()
   |
   |-srv_sys_space.check_file_spce()                檢查系統表空間是否正常
   |-srv_sys_space.open_or_create()              1. 打開系統表空間，並獲取flushed_lsn
   | |-read_lsn_and_check_flags()
   |   |-open_or_create()
   |   |-read_first_page()
   |   |-buf_dblwr_init_or_load_pages()             將雙寫緩存加載到內存中，如果ibdata日誌損壞，則通過dblwr恢復
   |   |-validate_first_page()                      校驗第一個頁是否正常，並讀取flushed_lsn
   |   | |-mach_read_from_8()                       讀取LSN，偏移為FIL_PAGE_FILE_FLUSH_LSN
   |   |-restore_from_doublewrite()                 如果有異常，則從dblwr恢復
   |
   |-log_group_init()                               redo log的結構初始化
   |-srv_undo_tablespaces_init()                    對於undo log表空間恢復結構初始化
   |
   |-recv_recovery_from_checkpoint_start()       2. 從redo-log的checkpoint開始恢復；注意，正常啟動也會調用
   | |-buf_flush_init_flush_rbt()                   創建一個紅黑樹，用於加速插入flush list
   | |                                              通過force_recovery判斷是否大於SRV_FORCE_NO_LOG_REDO
   | |-recv_find_max_checkpoint()                   查找最新的checkpoint點，在此會校驗redo log的頭部信息
   | | |-log_group_header_read()                    讀取512字節的頭部信息
   | | |-mach_read_from_4()                         讀取redo log的版本號LOG_HEADER_FORMAT
   | | |-recv_check_log_header_checksum()           版本1則校驗頁的完整性
   | | | |-log_block_get_checksum()                 獲取頁中的checksum，也就是頁中的最後四個字節
   | | | |-log_block_calc_checksum_crc32()          並與計算後的checksum比較
   | | |-recv_find_max_checkpoint_0()
   | |   |-log_group_header_read()
   | |
   | |-recv_group_scan_log_recs()                3.1 從checkpoint-lsn處開始查找MLOG_CHECKPOINT
   | | |-log_group_read_log_seg()                   從文件中讀取64K日誌，並未校驗
   | | |-recv_scan_log_recs()
   | |   |-log_block_get_hdr_no()
   | |   |-log_block_convert_lsn_to_no()
   | |   |-log_block_checksum_is_ok()               校驗頁是否正常
   | |   |-recv_parse_log_recs()                    解析redo-log，並添加到hash表中
   | |     |-recv_add_to_hash_table()
   | |       |-recv_hash()
   | |
   | |-recv_group_scan_log_recs()
   | |                                              ##如果flushed_lsn和checkponit lsn不同則恢復
   | |-recv_init_crash_recovery()
   | |-recv_init_crash_recovery_spaces()
   | |
   | |-recv_group_scan_log_recs()
   |
   |-trx_sys_init_at_db_start()
   |
   |-recv_apply_hashed_log_recs()                    當頁LSN小於log-record中的LSN時，應用redo日誌
   | |-recv_recover_page()                           實際調用recv_recover_page_func()
   |   |-recv_parse_or_apply_log_rec_body()
   |
   |-recv_recovery_from_checkpoint_finish()          完成崩潰恢復
{% endhighlight %}

<!--
fil_op_write_log() 些日誌

fil_names_write()  寫入MLOG_FILE_NAME
fil_name_write()
fil_op_write_log()
-->

接下來，首先重點看下 redo-log 的掃描函數。

{% highlight cpp %}
static bool recv_group_scan_log_recs(
    log_group_t*    group,
    lsn_t*      contiguous_lsn,
    bool        last_phase)
{
    mutex_enter(&recv_sys->mutex);
    recv_sys->len = 0;
    recv_sys->recovered_offset = 0;
    recv_sys->n_addrs = 0;
    recv_sys_empty_hash();
    srv_start_lsn = *contiguous_lsn;
    recv_sys->parse_start_lsn = *contiguous_lsn;
    recv_sys->scanned_lsn = *contiguous_lsn;
    recv_sys->recovered_lsn = *contiguous_lsn;
    recv_sys->scanned_checkpoint_no = 0;
    recv_previous_parsed_rec_type = MLOG_SINGLE_REC_FLAG;
    recv_previous_parsed_rec_offset = 0;
    recv_previous_parsed_rec_is_multi = 0;
    ut_ad(recv_max_page_lsn == 0);
    ut_ad(last_phase || !recv_writer_thread_active);
    mutex_exit(&recv_sys->mutex);

    lsn_t   checkpoint_lsn  = *contiguous_lsn;
    lsn_t   start_lsn;
    lsn_t   end_lsn;

    // 在此會根據三個不同的階段調用不同的變量
    // 1. 如果還沒有掃描到MLOG_CHECKPOINT，則為STORE_NO
    // 2. 第二次掃描則為STORE_YES
    // 3. 第三次掃描則為STORE_IF_EXISTS
    store_t store_to_hash   = recv_sys->mlog_checkpoint_lsn == 0
        ? STORE_NO : (last_phase ? STORE_IF_EXISTS : STORE_YES);

    ulint   available_mem   = UNIV_PAGE_SIZE
        * (buf_pool_get_n_pages()
           - (recv_n_pool_free_frames * srv_buf_pool_instances));

    end_lsn = *contiguous_lsn = ut_uint64_align_down(
        *contiguous_lsn, OS_FILE_LOG_BLOCK_SIZE);

    do {
        if (last_phase && store_to_hash == STORE_NO) {
            store_to_hash = STORE_IF_EXISTS;
            /* We must not allow change buffer
            merge here, because it would generate
            redo log records before we have
            finished the redo log scan. */
            recv_apply_hashed_log_recs(FALSE);
        }

        start_lsn = end_lsn;
        end_lsn += RECV_SCAN_SIZE; // 每次讀取的大小

        // 從磁盤中讀取數據
        log_group_read_log_seg(
            log_sys->buf, group, start_lsn, end_lsn);

    // 從緩存中讀取日誌，並解析，當hash表滿時則直接執行
    } while (!recv_scan_log_recs(
             available_mem, &store_to_hash, log_sys->buf,
             RECV_SCAN_SIZE,
             checkpoint_lsn,
             start_lsn, contiguous_lsn, &group->scanned_lsn));

    if (recv_sys->found_corrupt_log || recv_sys->found_corrupt_fs) {
        DBUG_RETURN(false);
    }

    DBUG_RETURN(store_to_hash == STORE_NO);
}
{% endhighlight %}




<!--
## 參考

InnoDB recovery詳細流程，通過圖畫出恢復的流程
http://www.sysdb.cn/index.php/2016/01/14/innodb-recovery/

MySQL · 引擎特性 · MySQL5.7 崩潰恢復優化
https://yq.aliyun.com/articles/64677

MySQL redo log及recover過程淺析
http://www.cnblogs.com/liuhao/p/3714012.html

MySQL系列：innodb源碼分析之redolog恢復
http://www.2cto.com/database/201501/369079.html

[MySQL學習] Innodb崩潰恢復流程
http://mysqllover.com/?p=376
-->


{% highlight text %}
{% endhighlight %}
