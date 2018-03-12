---
title: InnoDB 崩溃恢复
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,crash recovery,崩溃恢复
description: 如果 InnoDB 没有正常关闭，会在服务器启动的时候执行崩溃恢复 (Crash Recovery)，这一流程比较复杂，涉及到了 redo log、undo log 甚至包括了 binlog 。在此简单介绍下 InnoDB 崩溃恢复的流程。
---

如果 InnoDB 没有正常关闭，会在服务器启动的时候执行崩溃恢复 (Crash Recovery)，这一流程比较复杂，涉及到了 redo log、undo log 甚至包括了 binlog 。

在此简单介绍下 InnoDB 崩溃恢复的流程。

<!-- more -->

## 简介

如果 InnoDB 意外宕机了，那么会不会丢数据？

当然，这一问题比较复杂，根据不同的情况，可能会有数据丢失，不过至少有一点可以肯定，不会导致全部数据丢失。而这一过程，便涉及到了数据恢复。

### 初始化

在 MySQL 的主函数中，最终会通过 plugin_init() 对插件进行初始化，此时，会依次调用各个插件的初始化函数，同时也会调用 InnoDB 对应的初始化函数。

详细的调用流程如下。

{% highlight text %}
mysqld_main()
 |-init_server_components()
   |-plugin_init()
     |-plugin_initialize()
       |-ha_initialize_handlerton()
         |-innobase_init()
           |-innobase_start_or_create_for_mysql()
{% endhighlight %}

InnoDB 崩溃恢复相关的入口是 ```innobase_start_or_create_for_mysql()``` 函数。首先，InnoDB 会检查上次数据库是否正常关闭，如果是则不需要恢复，否则就进入崩溃恢复的流程。

<!--
这里把InnoDB的恢复过程主要划分为两个阶段，第一阶段主要依赖于redo log的恢复，而第二阶段，恰恰需要binlog和undo log的共同参与，接下来，我们来具体了解下整个恢复的过程。
-->

### 系统检查

数据库启动后，InnoDB 会通过 ```read_lsn_and_check_flags()``` 函数读取系统表空间中 flushed_lsn ，这一个 LSN 只在系统表空间的第一个页中存在，而且只有在正常关闭的时候写入。

系统正常关闭时，会调用 ```logs_empty_and_mark_files_at_shutdown() -> fil_write_flushed_lsn()``` ，也就是在执行一次 sharp checkpoint 之后，将 LSN 写入。

> flushed_lsn 只有在系统表空间的第一页存在，偏移量为 FIL_PAGE_FILE_FLUSH_LSN(26)，也就是保证至少在此 LSN 之前的页已经刷型到磁盘。

另外需要注意的是，写 flushed_lsn 时会同时写入到 Double Write Buffer，如果 flushed_lsn 对应的页损坏，则可以从 dbwl 中进行恢复。

<br>

接下来，InnoDB 会通过 redo-log 日志找到最近一次提交的 checkpoint，读取该 checkpoint 对应的 LSN 。其中，checkpoint 信息会保存在 redo-log 的第一个文件中，在两个固定偏移中轮流写入；所以，需要同时读取两个，并比较获取较大的一个值。

比较获得的 flushed_lsn 以及 checkpoint_lsn ，如果两者相同，则说明正常关闭；否则，就需要进行故障恢复。


## 重做日志

简单来说，如果需要执行崩溃恢复，会从上述读取的 checkpoint 信息，直接找到 redo-log 文件中相应的偏移量，也就是从 checkpoint 指定的位置开始读取日志，并保存到一个哈希表中，最后通过遍历哈希表中的 redo log 信息，读取相关页进行恢复。

### 日志扫描

假设，从上述 checkpoint 定位到开始恢复的 redo log 位置是在 ib_logfile1 文件中的某个位置，那么整个 redo log 扫描的过程可能是这样的：

* 从 ib_logfile1 的指定位置开始读取 redo log，每次读取 RECV_SCAN_SIZE (4*page_size=64k) 大小，写入时是以 block(512B) 为单位；

* 将从文件中读取的日志保存在 recv_sys->buf 中，然后进行校验，并解析日志，然后将结果保存在以 (space, page_no) 做 key 的 recv_sys->addr_hash 表中，这样一个 key 就对应了一个数据页的修改；

<!--
* 同时保存在恢复缓存中的redo信息会根据space，offset计算一个哈希值后保存到一个哈希表(recv_sys->addr_hash)中，相同的哈希值不同(space，offset)用链表存储，相同的(space,offset)用列表保存，可能部分事务比较大，redo信息一个block不能保存，所以，每个body中可以用链表链接多body的值
-->

redo log 被保存到哈希表中之后，InnoDB 就可以开始进行数据恢复，只需要轮询哈希表中的每个节点获取 redo 信息，根据 (space, page_no) 读取指定的数据页，并进行日志覆盖。

### 优化

如上，在恢复时，需要获取 space id 与 *.ibd 文件的对应关系，这就需要打开所有的 ibd 文件获取，<!-- TODODO: 确认是否如此，是否有固定位置保存 sapce-id 与 ibd 文件的对应关系 -->如果文件有成百上千，甚至以万计的时候，那么这一操作将会非常耗时。

为此，5.7 在 redo log 中添加了两个新的类型：```MLOG_FILE_NAME``` 记录在 checkpoint 之后，所有被修改过的信息(space, filepath)；```MLOG_CHECKPOINT``` 用于标志 ```MLOG_FILE_NAME``` 的结束。

<!--
上面两种redo log类型的添加，完美解决了前面遗留的问题，redo log中保存了后续需要恢复的space和filepath对，所以，在恢复的时候，只需要从checkpoint的位置往后扫描到MLOG_CHECKPOINT的位置，这样就能获取到需要恢复的space和filepath，在恢复过程中，只需要打开这些ibd文件即可，当然由于space和filepath的对应关系通过redo存了下来，恢复的时候也不再依赖数据字典。
这里需要强调的一点就是MLOG_CHECKPOINT在每个checkpoint点中最多只存在一次，如果出现多次MLOG_CHECKPOINT类型的日志，则说明redo已经损坏，InnoDB会报错。最多存在一次，那么会不会有不存在的情况？答案是肯定的，在每次checkpoint过后，如果没有发生数据更新，那么MLOG_CHECKPOINT就不会被记录。所以只要简单查找下redo log最新一个checkpoint后的MLOG_CHECKPOINT是否存在，就能判定上次MySQL是否正常关机。5.7版本的MySQL在InnoDB进行恢复的时候，也正是这样做的，MySQL 5.7在进行恢复的时候，一般情况下需要进行最多3次的redo log扫描：

    第一次redo log的扫描，主要是查找MLOG_CHECKPOINT，不进行redo log的解析，如果没有找到MLOG_CHECKPOINT，则说明InnoDB不需要进行recovery，后面的两次扫描可以省略，如果找到了MLOG_CHECKPOINT，则获取MLOG_FILE_NAME到指定列表，后续只需打开该链表中的表空间即可。
    第二次扫描是在第一次找到MLOG_CHECKPOINT基础之上进行的，该次扫描会把redo log解析到哈希表中，如果扫描完整个文件，哈希表还没有被填满，则不需要第三次扫描，直接进行recovery就结束
    第三次扫描是在第二次基础上进行的，第二次扫描把哈希表填满后，还有redo log剩余，则需要循环进行扫描，哈希表满后立即进行recovery，直到所有的redo log被apply完为止。

redo log全部被解析并且apply完成，整个InnoDB recovery的第一阶段也就结束了，在该阶段中，所有已经被记录到redo log但是没有完成数据刷盘的记录都被重新落盘。然而，InnoDB单靠redo log的恢复是不够的，这样还是有可能会丢失数据(或者说造成主从数据不一致)，因为在事务提交过程中，写binlog和写redo log提交是两个过程，写binlog在前而redo提交在后，如果MySQL写完binlog后，在redo提交之前发生了宕机，这样就会出现问题：binlog中已经包含了该条记录，而redo没有持久化。binlog已经落盘就意味着slave上可以apply该条数据，redo没有持久化则代表了master上该条数据并没有落盘，也不能通过redo进行恢复。这样就造成了主从数据的不一致，换句话说主上丢失了部分数据，那么MySQL又是如何保证在这样的情况下，数据还是一致的？这就需要进行第二阶段恢复。
-->



## 源码分析

InnoDB 的数据恢复是一个很复杂的过程，在其恢复过程中，需要 redolog、binlog、undolog 等参与，接下来从源码角度具体了解下整个恢复的过程。

{% highlight text %}
innobase_init()
 |-innobase_start_or_create_for_mysql()
   |
   |-recv_sys_create()   创建崩溃恢复所需要的内存对象
   |-recv_sys_init()
   | |-hash_create()
   |
   |-srv_sys_space.check_file_spce()                检查系统表空间是否正常
   |-srv_sys_space.open_or_create()              1. 打开系统表空间，并获取flushed_lsn
   | |-read_lsn_and_check_flags()
   |   |-open_or_create()
   |   |-read_first_page()
   |   |-buf_dblwr_init_or_load_pages()             将双写缓存加载到内存中，如果ibdata日志损坏，则通过dblwr恢复
   |   |-validate_first_page()                      校验第一个页是否正常，并读取flushed_lsn
   |   | |-mach_read_from_8()                       读取LSN，偏移为FIL_PAGE_FILE_FLUSH_LSN
   |   |-restore_from_doublewrite()                 如果有异常，则从dblwr恢复
   |
   |-log_group_init()                               redo log的结构初始化
   |-srv_undo_tablespaces_init()                    对于undo log表空间恢复结构初始化
   |
   |-recv_recovery_from_checkpoint_start()       2. 从redo-log的checkpoint开始恢复；注意，正常启动也会调用
   | |-buf_flush_init_flush_rbt()                   创建一个红黑树，用于加速插入flush list
   | |                                              通过force_recovery判断是否大于SRV_FORCE_NO_LOG_REDO
   | |-recv_find_max_checkpoint()                   查找最新的checkpoint点，在此会校验redo log的头部信息
   | | |-log_group_header_read()                    读取512字节的头部信息
   | | |-mach_read_from_4()                         读取redo log的版本号LOG_HEADER_FORMAT
   | | |-recv_check_log_header_checksum()           版本1则校验页的完整性
   | | | |-log_block_get_checksum()                 获取页中的checksum，也就是页中的最后四个字节
   | | | |-log_block_calc_checksum_crc32()          并与计算后的checksum比较
   | | |-recv_find_max_checkpoint_0()
   | |   |-log_group_header_read()
   | |
   | |-recv_group_scan_log_recs()                3.1 从checkpoint-lsn处开始查找MLOG_CHECKPOINT
   | | |-log_group_read_log_seg()                   从文件中读取64K日志，并未校验
   | | |-recv_scan_log_recs()
   | |   |-log_block_get_hdr_no()
   | |   |-log_block_convert_lsn_to_no()
   | |   |-log_block_checksum_is_ok()               校验页是否正常
   | |   |-recv_parse_log_recs()                    解析redo-log，并添加到hash表中
   | |     |-recv_add_to_hash_table()
   | |       |-recv_hash()
   | |
   | |-recv_group_scan_log_recs()
   | |                                              ##如果flushed_lsn和checkponit lsn不同则恢复
   | |-recv_init_crash_recovery()
   | |-recv_init_crash_recovery_spaces()
   | |
   | |-recv_group_scan_log_recs()
   |
   |-trx_sys_init_at_db_start()
   |
   |-recv_apply_hashed_log_recs()                    当页LSN小于log-record中的LSN时，应用redo日志
   | |-recv_recover_page()                           实际调用recv_recover_page_func()
   |   |-recv_parse_or_apply_log_rec_body()
   |
   |-recv_recovery_from_checkpoint_finish()          完成崩溃恢复
{% endhighlight %}

<!--
fil_op_write_log() 些日志

fil_names_write()  写入MLOG_FILE_NAME
fil_name_write()
fil_op_write_log()
-->

接下来，首先重点看下 redo-log 的扫描函数。

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

    // 在此会根据三个不同的阶段调用不同的变量
    // 1. 如果还没有扫描到MLOG_CHECKPOINT，则为STORE_NO
    // 2. 第二次扫描则为STORE_YES
    // 3. 第三次扫描则为STORE_IF_EXISTS
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
        end_lsn += RECV_SCAN_SIZE; // 每次读取的大小

        // 从磁盘中读取数据
        log_group_read_log_seg(
            log_sys->buf, group, start_lsn, end_lsn);

    // 从缓存中读取日志，并解析，当hash表满时则直接执行
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
## 参考

InnoDB recovery详细流程，通过图画出恢复的流程
http://www.sysdb.cn/index.php/2016/01/14/innodb-recovery/

MySQL · 引擎特性 · MySQL5.7 崩溃恢复优化
https://yq.aliyun.com/articles/64677

MySQL redo log及recover过程浅析
http://www.cnblogs.com/liuhao/p/3714012.html

MySQL系列：innodb源码分析之redolog恢复
http://www.2cto.com/database/201501/369079.html

[MySQL学习] Innodb崩溃恢复流程
http://mysqllover.com/?p=376
-->


{% highlight text %}
{% endhighlight %}
