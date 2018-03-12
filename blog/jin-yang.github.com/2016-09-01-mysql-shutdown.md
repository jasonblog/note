---
title: MySQL 关闭过程
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,shutdown,关闭
description: 简单分析下 mysqld 进程关闭的过程，并讨论如何安全地关闭 MySQL 实例。
---

简单分析下 mysqld 进程关闭的过程，并讨论如何安全地关闭 MySQL 实例。

<!-- more -->

## 简介

通常有几种方式关闭 MySQL 服务器，常见有如下：A) 执行 ```mysqladmin shutdown``` 命令；B) 向服务器发送 SIGTERM、SIGQUIT 信号。

### mysqladmin

首先，看下通过 ```mysqladmin shutdown``` 调用时的执行流程。

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

如上，当版本小于 5.7.9 时，实际上会调用 ```mysql_shutdown()``` 函数，而在 5.7.9 版本之后(包括该版本)，则会调用 ```mysql_query()``` 函数。

在此，主要看下后者的处理。

{% highlight text %}
dispatch_command()
 |                              ← COM_SHUTDOWN:
 |-shutdown()                   ← 调用相同文件下的函数，关闭mysqld服务
   |-check_global_access()
   |-my_ok()                    ← 返回客户端，COM_QUERY<=>my_ok()，COM_SHUTDOWN<=>my_eof()
   |-general_log_print()        ← 打印日志"Got shutdown command for"
   |-kill_mysql()
     |-pthread_kill()           ← 向signal_thread发送SIGTERM信号
{% endhighlight %}

也就是说，实际上还是通过用户线程向服务发送 SIGTERM 信号完成。 

### sigterm

如上，两种方式最终的实现都是通过发送 SIGTERM 信号完成的，那么我们接下来就看看 mysqld 服务是如何处理该信号量的。

#### 初始化

其中部分信号，如 SIGABRT、SIGSEGV 等会在 ```my_init_signals()``` 函数中进行初始化信号处理函数，而 SIGTERM、SIGQUIT、SIGHUP 则会在单独的线程中进行处理，对应 ```signal_hand()``` 函数。

{% highlight text %}
mysqld_main()
 |-my_init_signals()               ← 设置信号处理函数
 |-start_signal_handler()          ← 创建一个单独的信号处理线程
   |-mysql_thread_create()         ← 创建signal_hand()单独线程
{% endhighlight %}

#### 信号处理

接下来看看 ```signal_hand()``` 函数中的处理方式。

{% highlight text %}
signal_hand()
 |-abort_loop=true                 ← 设置abort_loop变量，很多循环会检查该变量
 |-pthread_kill()                  ← 先主线程发送SIGUSR1信号
 |-close_connections()             ← 关闭所有链接
{% endhighlight %}

实际上，在主线程中对 SIGUSR1 信号并没有进行处理，应该只是一个线程切换；接下来，还是看看主线程的处理：

{% highlight text %}
mysqld_main()
 |-my_thread_join()                ← 等待上述的signal_hand()处理线程
 |-clean_up()
   |-plugin_shutdown()
     |-plugin_deinitialize()
       |-ha_finalize_handlerton()
         |-hton->panic()           ← 调用插件的关闭函数
{% endhighlight %}

也就是会依次调用各个存储引擎的 ```panic()``` 函数，对应到 InnoDB 就是 ```innobase_end()``` 函数。

## InnoDB 关闭过程

在关闭 MySQL 时，可以通过 innodb_fast_shutdown 参数控制存储引擎 InnoDB 的行为，该参数可以取 0、1、2，各个值的含义分别如下：

0 关闭时 InnoDB 需要完成所有的 full purge 和 merge insert buffer 操作，这个过程会需要一定的时间，有时候可能会花上几个小时；通常在升级 InnoDB 时，需要设置为 0。

1 默认值，关闭 InooDB 时不完成 full purge 和 merge insert buffer，但是会将缓冲池中的脏页写到磁盘中。

2 意味着既不完成 full purge 和 merge insert buffer，同时也不将缓冲池中的脏页刷新到磁盘，但是会将 redo log 写入到磁盘；这种情况下，事务也不会丢失，但是下次启动时需要执行崩溃恢复。

接着看看 InnoDB 关闭时的详细处理过程。

### 源码解析

如下是代码的处理过程，其中 ```logs_empty_and_mark_files_at_shutdown()``` 函数是主要的关闭处理函数，在系统关闭时同时执行 sharp checkpoint 操作。

{% highlight text %}
innobase_end()
 |-srv_fast_shutdown                           ← 根据参数innodb_fast_shutdown判断是否快速关闭
 |-hash_table_free()                           ← 释放innodb表占用的内存
 |-innobase_shutdown_for_mysql()
 | |-fts_optimize_shutdown()
 | |-dict_stats_shutdown()
 | |-logs_empty_and_mark_files_at_shutdown()   ← 1.  将buffer pool落盘，并将LSN写入表空间，主要函数，后面均为资源清理
 | | |-ib::info()                              ← 1.0 打印Starting shutdown日志 <<<<<<
 | | |-srt_shutdown_state                            设置变量，进入SRV_SHUTDOWN_CLEANUP状态
 | | |
 | | |-srv_any_background_threads_are_active() ← 1.1 等待后台线程关闭，没1min打印一次等待线程信息
 | | |
 | | |-trx_sys_any_active_transactions()       ← 1.2 如果事务已经在prepare阶段了，则等待其处理完成
 | | |
 | | |-srv_get_active_thread_type()            ← 1.3 等待worker、master、purge线程进入suspend状态
 | | |
 | | |-srt_shutdown_state                            设置变量，进入SRV_SHUTDOWN_FLUSH_PHASE状态
 | | |-buf_page_cleaner_is_active              ← 1.4 正常此时只剩下了page_cleaner线程刷Buffer Pool了，等待其完成
 | | |-buf_pool_check_no_pending_io()                检查IO操作是否都已经完成
 | | |
 | | |                                         ← 1.5 如果srv_fast_shutdown变量值为2
 | | |-log_buffer_flush_to_disk()                    将redo-log刷新到磁盘
 | | |-srv_any_background_threads_are_active()
 | | |-fil_close_all_files()                         关闭文件，然后返回
 | | |
 | | |-log_make_checkpoint_at()                ← 1.6 将最近LSN做checkpoint
 | | |-fil_flush_file_spaces()                       将表空间文件和日志文件刷新到磁盘
 | | |-srt_shutdown_state                            设置变量，进入SRV_SHUTDOWN_LAST_PHASE状态
 | | |-srv_get_active_thread_type()                  再次确认所有服务已经关闭
 | | |
 | | |-fil_write_flushed_lsn()                 ← 1.7 将LSN写入到系统表空间的第一页中
 | | |-fil_close_all_files()                         关闭所有文件
 | |
 | |-srv_conc_get_active_threads()             ← 正常应该无活跃的线程，有则打印到日志
 | |
 | |                                           ← 2. 接下来是一些资源的清理
 | |-srv_shutdown_all_bg_threads()                  关闭InnoDB创建的后台线程
 | |-fclose()                                       关闭InnoDB打开的文件
 | |-dict_stats_thread_deinit()
 | |                                                释放mutexes，释放内存
 | |-os_thread_free()                               释放线程相关的资源
 | |-sync_check_close()                             释放同步相关资源
 |
 |-innobase_space_shutdown()
{% endhighlight %}

实际上，Buffer Pool 中脏页刷到磁盘的操作是最耗时的，脏页越多需要 flush 的块也就越多，从而导致关闭时间变长；可以通过下面的命令来观察 Dirty Page 的数量：

{% highlight text %}
$ mysqladmin -uroot ext -i 1 | grep "Innodb_buffer_pool_pages_dirty"
{% endhighlight %}

<!--
2. 参数innodb_max_dirty_pages_pct

Buffer_Pool中Dirty_Page所占的数量，直接影响InnoDB的关闭时间。参数innodb_max_dirty_pages_pct可以直接控制了Dirty_Page在Buffer_Pool中所占的比率，而且幸运的是innodb_max_dirty_pages_pct是可以动态改变的。

所以，在关闭InnoDB之前先将innodb_max_dirty_pages_pct调小，强制数据块Flush一段时间，则能够大大缩短MySQL关闭的时间。
set global innodb_max_dirty_pages_pct=0;

一般执行了上面的命令之后，Dirty_Page的Flush仍需要一段时间，所以要稍等一会儿，再关闭MySQL才有效果。
3. 关闭前做些什么

有时候，就算你改变innodb_max_dirty_pages_pct=0，仍然不能保证InnoDB快速关闭。还有一些注意事项。

设置数据库为只读：如果数据库一直是活跃的，有连接在向里面写数据，那么Dirty Page则还可能不断的产生。

如果是备库，在innodb_max_dirty_pages_pct设置成0的同时，最好先stop slave：这个很关键，而且对关闭时间影响也会很大。第一，主动stop slave后，MySQL在关闭时，需要停止的线程其实是更少了的。第二，如果slave的SQL线程还在执行的话，Buffer Pool则还在活动，Dirty Page也可能还会不断的增多。

一般，如果注意到了上面三点：
set global innodb_max_dirty_pages_pct=0
set global read_only=1
stop slave

关闭数据库，就会很快了。如果你把上面三步做完，然后观察Dirty Page的数量，当数量很少时，再执行命令关库，这样总能保证以较快的速度完成关库命令。
4. 一个注意事项

这里需要注意的是，你不需等到Dirty Page的数量到零，才开始关闭MySQL。因为有时候，即使已经没有活动的会话时，InnoDB的Insert Buffer的合并仍然会产生一些Dirty Page，所以这时你可能会发现，等了很久很久很久Dirty Page的数量仍然大于零。

其实这时，你已经可以快速的关闭数据库了。我怎么判断这种情况呢？这时我们可以通过InnoDB的LSN来判断，下面是SHOW InnoDB Status里面获取的信息：

Log sequence number 814 3121743145
Log flushed up to   814 3121092043
Last checkpoint at  814 2826361389

这里看到，当前的LSN是814 3121743145，最后一个检查点在814 2826361389，也就是说两者相差了3121743145-2826361389=295381756，那么意味着InnoDB还有很多Dirty Page需要Flush。

下面是另一个库的LSN信息：

Log sequence number 0 1519256161
Log flushed up to   0 1519256161
Last checkpoint at  0 1519256161

可以看到，这里的Dirty Page都已经Flush了，那么关闭InnoDB也就很快了。
-->




### FAQs

1\. 如何查看 InnoDB 在等待那些线程？

InnoDB 在关闭时，会每隔 100ms 检查一次后台线程是否已经全部退出，具体检查那些线程可以查看 ```srv_any_background_threads_are_active()``` 函数；等待超过了 1min ，则会打印日志 "Waiting for" 。

2\. 为什么要等待出于 prepare 阶段的事务？

如果事务处于 prepare 阶段，说明已经在 InnoDB 执行了 commit 操作，也就是在存储引擎中认为已经提交，只需要服务端提交即可，显然，我们不希望丢失这部分数据。

实际上，接下来的操作不会再与客户端进行交互，都是服务端的执行流程了，正常来说执行的时间会非常短。

## 参考

[Reference Manual - The Server Shutdown Process](https://dev.mysql.com/doc/refman/en/server-shutdown.html)

<!--

-->


{% highlight text %}
{% endhighlight %}
