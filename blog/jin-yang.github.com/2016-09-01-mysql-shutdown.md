---
title: MySQL 關閉過程
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,shutdown,關閉
description: 簡單分析下 mysqld 進程關閉的過程，並討論如何安全地關閉 MySQL 實例。
---

簡單分析下 mysqld 進程關閉的過程，並討論如何安全地關閉 MySQL 實例。

<!-- more -->

## 簡介

通常有幾種方式關閉 MySQL 服務器，常見有如下：A) 執行 ```mysqladmin shutdown``` 命令；B) 向服務器發送 SIGTERM、SIGQUIT 信號。

### mysqladmin

首先，看下通過 ```mysqladmin shutdown``` 調用時的執行流程。

{% highlight cpp %}
static int execute_commands(MYSQL *mysql,int argc, char **argv)
{
  ... ...
  for (; argc > 0 ; argv++,argc--) {
    int option;
    bool log_warnings= true;
    switch (option= find_type(argv[0], &command_typelib, FIND_TYPE_BASIC)) {
    case ADMIN_SHUTDOWN:
      ... ...
      if(mysql_get_server_version(mysql) < 50709)
        resShutdown= mysql_shutdown(mysql, SHUTDOWN_DEFAULT);
      else
        resShutdown= mysql_query(mysql, "shutdown");
      if(resShutdown) {
        my_printf_error(0, "shutdown failed; error: '%s'", error_flags,
        mysql_error(mysql));
        return -1;
      }
    }
  }
}
{% endhighlight %}

如上，當版本小於 5.7.9 時，實際上會調用 ```mysql_shutdown()``` 函數，而在 5.7.9 版本之後(包括該版本)，則會調用 ```mysql_query()``` 函數。

在此，主要看下後者的處理。

{% highlight text %}
dispatch_command()
 |                              ← COM_SHUTDOWN:
 |-shutdown()                   ← 調用相同文件下的函數，關閉mysqld服務
   |-check_global_access()
   |-my_ok()                    ← 返回客戶端，COM_QUERY<=>my_ok()，COM_SHUTDOWN<=>my_eof()
   |-general_log_print()        ← 打印日誌"Got shutdown command for"
   |-kill_mysql()
     |-pthread_kill()           ← 向signal_thread發送SIGTERM信號
{% endhighlight %}

也就是說，實際上還是通過用戶線程向服務發送 SIGTERM 信號完成。 

### sigterm

如上，兩種方式最終的實現都是通過發送 SIGTERM 信號完成的，那麼我們接下來就看看 mysqld 服務是如何處理該信號量的。

#### 初始化

其中部分信號，如 SIGABRT、SIGSEGV 等會在 ```my_init_signals()``` 函數中進行初始化信號處理函數，而 SIGTERM、SIGQUIT、SIGHUP 則會在單獨的線程中進行處理，對應 ```signal_hand()``` 函數。

{% highlight text %}
mysqld_main()
 |-my_init_signals()               ← 設置信號處理函數
 |-start_signal_handler()          ← 創建一個單獨的信號處理線程
   |-mysql_thread_create()         ← 創建signal_hand()單獨線程
{% endhighlight %}

#### 信號處理

接下來看看 ```signal_hand()``` 函數中的處理方式。

{% highlight text %}
signal_hand()
 |-abort_loop=true                 ← 設置abort_loop變量，很多循環會檢查該變量
 |-pthread_kill()                  ← 先主線程發送SIGUSR1信號
 |-close_connections()             ← 關閉所有鏈接
{% endhighlight %}

實際上，在主線程中對 SIGUSR1 信號並沒有進行處理，應該只是一個線程切換；接下來，還是看看主線程的處理：

{% highlight text %}
mysqld_main()
 |-my_thread_join()                ← 等待上述的signal_hand()處理線程
 |-clean_up()
   |-plugin_shutdown()
     |-plugin_deinitialize()
       |-ha_finalize_handlerton()
         |-hton->panic()           ← 調用插件的關閉函數
{% endhighlight %}

也就是會依次調用各個存儲引擎的 ```panic()``` 函數，對應到 InnoDB 就是 ```innobase_end()``` 函數。

## InnoDB 關閉過程

在關閉 MySQL 時，可以通過 innodb_fast_shutdown 參數控制存儲引擎 InnoDB 的行為，該參數可以取 0、1、2，各個值的含義分別如下：

0 關閉時 InnoDB 需要完成所有的 full purge 和 merge insert buffer 操作，這個過程會需要一定的時間，有時候可能會花上幾個小時；通常在升級 InnoDB 時，需要設置為 0。

1 默認值，關閉 InooDB 時不完成 full purge 和 merge insert buffer，但是會將緩衝池中的髒頁寫到磁盤中。

2 意味著既不完成 full purge 和 merge insert buffer，同時也不將緩衝池中的髒頁刷新到磁盤，但是會將 redo log 寫入到磁盤；這種情況下，事務也不會丟失，但是下次啟動時需要執行崩潰恢復。

接著看看 InnoDB 關閉時的詳細處理過程。

### 源碼解析

如下是代碼的處理過程，其中 ```logs_empty_and_mark_files_at_shutdown()``` 函數是主要的關閉處理函數，在系統關閉時同時執行 sharp checkpoint 操作。

{% highlight text %}
innobase_end()
 |-srv_fast_shutdown                           ← 根據參數innodb_fast_shutdown判斷是否快速關閉
 |-hash_table_free()                           ← 釋放innodb表佔用的內存
 |-innobase_shutdown_for_mysql()
 | |-fts_optimize_shutdown()
 | |-dict_stats_shutdown()
 | |-logs_empty_and_mark_files_at_shutdown()   ← 1.  將buffer pool落盤，並將LSN寫入表空間，主要函數，後面均為資源清理
 | | |-ib::info()                              ← 1.0 打印Starting shutdown日誌 <<<<<<
 | | |-srt_shutdown_state                            設置變量，進入SRV_SHUTDOWN_CLEANUP狀態
 | | |
 | | |-srv_any_background_threads_are_active() ← 1.1 等待後臺線程關閉，沒1min打印一次等待線程信息
 | | |
 | | |-trx_sys_any_active_transactions()       ← 1.2 如果事務已經在prepare階段了，則等待其處理完成
 | | |
 | | |-srv_get_active_thread_type()            ← 1.3 等待worker、master、purge線程進入suspend狀態
 | | |
 | | |-srt_shutdown_state                            設置變量，進入SRV_SHUTDOWN_FLUSH_PHASE狀態
 | | |-buf_page_cleaner_is_active              ← 1.4 正常此時只剩下了page_cleaner線程刷Buffer Pool了，等待其完成
 | | |-buf_pool_check_no_pending_io()                檢查IO操作是否都已經完成
 | | |
 | | |                                         ← 1.5 如果srv_fast_shutdown變量值為2
 | | |-log_buffer_flush_to_disk()                    將redo-log刷新到磁盤
 | | |-srv_any_background_threads_are_active()
 | | |-fil_close_all_files()                         關閉文件，然後返回
 | | |
 | | |-log_make_checkpoint_at()                ← 1.6 將最近LSN做checkpoint
 | | |-fil_flush_file_spaces()                       將表空間文件和日誌文件刷新到磁盤
 | | |-srt_shutdown_state                            設置變量，進入SRV_SHUTDOWN_LAST_PHASE狀態
 | | |-srv_get_active_thread_type()                  再次確認所有服務已經關閉
 | | |
 | | |-fil_write_flushed_lsn()                 ← 1.7 將LSN寫入到系統表空間的第一頁中
 | | |-fil_close_all_files()                         關閉所有文件
 | |
 | |-srv_conc_get_active_threads()             ← 正常應該無活躍的線程，有則打印到日誌
 | |
 | |                                           ← 2. 接下來是一些資源的清理
 | |-srv_shutdown_all_bg_threads()                  關閉InnoDB創建的後臺線程
 | |-fclose()                                       關閉InnoDB打開的文件
 | |-dict_stats_thread_deinit()
 | |                                                釋放mutexes，釋放內存
 | |-os_thread_free()                               釋放線程相關的資源
 | |-sync_check_close()                             釋放同步相關資源
 |
 |-innobase_space_shutdown()
{% endhighlight %}

實際上，Buffer Pool 中髒頁刷到磁盤的操作是最耗時的，髒頁越多需要 flush 的塊也就越多，從而導致關閉時間變長；可以通過下面的命令來觀察 Dirty Page 的數量：

{% highlight text %}
$ mysqladmin -uroot ext -i 1 | grep "Innodb_buffer_pool_pages_dirty"
{% endhighlight %}

<!--
2. 參數innodb_max_dirty_pages_pct

Buffer_Pool中Dirty_Page所佔的數量，直接影響InnoDB的關閉時間。參數innodb_max_dirty_pages_pct可以直接控制了Dirty_Page在Buffer_Pool中所佔的比率，而且幸運的是innodb_max_dirty_pages_pct是可以動態改變的。

所以，在關閉InnoDB之前先將innodb_max_dirty_pages_pct調小，強制數據塊Flush一段時間，則能夠大大縮短MySQL關閉的時間。
set global innodb_max_dirty_pages_pct=0;

一般執行了上面的命令之後，Dirty_Page的Flush仍需要一段時間，所以要稍等一會兒，再關閉MySQL才有效果。
3. 關閉前做些什麼

有時候，就算你改變innodb_max_dirty_pages_pct=0，仍然不能保證InnoDB快速關閉。還有一些注意事項。

設置數據庫為只讀：如果數據庫一直是活躍的，有連接在向裡面寫數據，那麼Dirty Page則還可能不斷的產生。

如果是備庫，在innodb_max_dirty_pages_pct設置成0的同時，最好先stop slave：這個很關鍵，而且對關閉時間影響也會很大。第一，主動stop slave後，MySQL在關閉時，需要停止的線程其實是更少了的。第二，如果slave的SQL線程還在執行的話，Buffer Pool則還在活動，Dirty Page也可能還會不斷的增多。

一般，如果注意到了上面三點：
set global innodb_max_dirty_pages_pct=0
set global read_only=1
stop slave

關閉數據庫，就會很快了。如果你把上面三步做完，然後觀察Dirty Page的數量，當數量很少時，再執行命令關庫，這樣總能保證以較快的速度完成關庫命令。
4. 一個注意事項

這裡需要注意的是，你不需等到Dirty Page的數量到零，才開始關閉MySQL。因為有時候，即使已經沒有活動的會話時，InnoDB的Insert Buffer的合併仍然會產生一些Dirty Page，所以這時你可能會發現，等了很久很久很久Dirty Page的數量仍然大於零。

其實這時，你已經可以快速的關閉數據庫了。我怎麼判斷這種情況呢？這時我們可以通過InnoDB的LSN來判斷，下面是SHOW InnoDB Status裡面獲取的信息：

Log sequence number 814 3121743145
Log flushed up to   814 3121092043
Last checkpoint at  814 2826361389

這裡看到，當前的LSN是814 3121743145，最後一個檢查點在814 2826361389，也就是說兩者相差了3121743145-2826361389=295381756，那麼意味著InnoDB還有很多Dirty Page需要Flush。

下面是另一個庫的LSN信息：

Log sequence number 0 1519256161
Log flushed up to   0 1519256161
Last checkpoint at  0 1519256161

可以看到，這裡的Dirty Page都已經Flush了，那麼關閉InnoDB也就很快了。
-->




### FAQs

1\. 如何查看 InnoDB 在等待那些線程？

InnoDB 在關閉時，會每隔 100ms 檢查一次後臺線程是否已經全部退出，具體檢查那些線程可以查看 ```srv_any_background_threads_are_active()``` 函數；等待超過了 1min ，則會打印日誌 "Waiting for" 。

2\. 為什麼要等待出於 prepare 階段的事務？

如果事務處於 prepare 階段，說明已經在 InnoDB 執行了 commit 操作，也就是在存儲引擎中認為已經提交，只需要服務端提交即可，顯然，我們不希望丟失這部分數據。

實際上，接下來的操作不會再與客戶端進行交互，都是服務端的執行流程了，正常來說執行的時間會非常短。

## 參考

[Reference Manual - The Server Shutdown Process](https://dev.mysql.com/doc/refman/en/server-shutdown.html)

<!--

-->


{% highlight text %}
{% endhighlight %}
