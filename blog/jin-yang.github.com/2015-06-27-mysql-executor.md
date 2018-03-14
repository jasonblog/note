---
title: MySQL 執行簡介
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,執行
description: 最後是 MySQL 的執行。
---

最後是 MySQL 的執行。

<!-- more -->

## 簡介

MySQL 的 join 操作主要是採用 nest loop 算法，其它算法暫不討論，其中涉及的主要函數有如下 do_select()、sub_select()、evaluate_join_record()，其簡單的調用堆棧可以參考如下：

{% highlight text %}
$ pgrep mysqld                            ← 查看mysqld的進程號
32204

$ gdb
(gdb) attach 32204                        ← 鏈接到mysqld服務器
(gdb) b evaluate_join_record              ← 設置斷點，然後另一個終端鏈接mysql
(gdb) bt
#0  evaluate_join_record (...)     at sql/sql_executor.cc:1473
#1  sub_select (...)               at sql/sql_executor.cc:1291
#2  do_select (...)                at sql/sql_executor.cc:944
#3  JOIN::exec (...)               at sql/sql_executor.cc:199
#4  handle_query (...)             at sql/sql_select.cc:184
#5  execute_sqlcom_select (...)    at sql/sql_parse.cc:5143
#6  mysql_execute_command (...)    at sql/sql_parse.cc:2756
#7  mysql_parse (...)              at sql/sql_parse.cc:5559
#8  dispatch_command (...)         at sql/sql_parse.cc:1427
#9  do_command (...)               at sql/sql_parse.cc:995
#10 handle_connection (...)        at sql/conn_handler/connection_handler_per_thread.cc:300
#11 pfs_spawn_thread (...)         at storage/perfschema/pfs.cc:2188
#12 start_thread ()                at pthread_create.c
#13 clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S
{% endhighlight %}
<!--
#0  evaluate_join_record (join=0x7fc2e9e635a8, qep_tab=0x7fc2e9e64ea8) at sql/sql_executor.cc:1473
#1  0x00000000014c98bd in sub_select (join=0x7fc2e9e635a8, qep_tab=0x7fc2e9e64ea8, end_of_records=false) at sql/sql_executor.cc:1291
#2  0x00000000014c913e in do_select (join=0x7fc2e9e635a8) at sql/sql_executor.cc:944
#3  0x00000000014c70bf in JOIN::exec (this=0x7fc2e9e635a8) at sql/sql_executor.cc:199
#4  0x0000000001560bbe in handle_query (thd=0x7fc2e9cb9000, lex=0x7fc2e9cbb2f8, result=0x7fc2e9ccaf80, added_options=0, removed_options=0)
    at sql/sql_select.cc:184
#5  0x0000000001516bab in execute_sqlcom_select (thd=0x7fc2e9cb9000, all_tables=0x7fc2e9cca490) at sql/sql_parse.cc:5143
#6  0x00000000015103bc in mysql_execute_command (thd=0x7fc2e9cb9000, first_level=true) at /home/andy/Workspace/databases/mysql/mysql-5.7.16/sql/sql_parse.cc:2756
#7  0x0000000001517ab9 in mysql_parse (thd=0x7fc2e9cb9000, parser_state=0x7fc30c92c660) at /home/andy/Workspace/databases/mysql/mysql-5.7.16/sql/sql_parse.cc:5559
#8  0x000000000150d24d in dispatch_command (thd=0x7fc2e9cb9000, com_data=0x7fc30c92cdb0, command=COM_QUERY) at /home/andy/Workspace/databases/mysql/mysql-5.7.16/sql/sql_parse.cc:1427
#9  0x000000000150c117 in do_command (thd=0x7fc2e9cb9000) at /home/andy/Workspace/databases/mysql/mysql-5.7.16/sql/sql_parse.cc:995
#10 0x000000000163aecc in handle_connection (arg=0x7fc30a45c660) at /home/andy/Workspace/databases/mysql/mysql-5.7.16/sql/conn_handler/connection_handler_per_thread.cc:300
#11 0x000000000186328a in pfs_spawn_thread (arg=0x7fc30a458a20) at /home/andy/Workspace/databases/mysql/mysql-5.7.16/storage/perfschema/pfs.cc:2188
#12 0x00007fc30c370dc5 in start_thread () from /lib64/libpthread.so.0
#13 0x00007fc30b037ced in clone () from /lib64/libc.so.6
-->

MySQL 的執行器與優化器類似，同樣共享 JOIN 中的內容，也就是一個查詢的上下文信息；而真正執行的函數為 JOIN::exec()@sql/sql_executor.cc。

在此主要查看下 exec 的執行過程，實際的執行函數在 do_select() 中，接下來簡單分析下源碼的執行過程。




## 源碼解析




MySQL 執行器的源碼主要在 sql_executor.cc 中，如下簡單說明下函數的執行過程，只是包含了主要的路徑。

{% highlight c %}
static int do_select(JOIN *join,List<Item> *fields,TABLE *table,Procedure *procedure)
{
    ... ...
    Next_select_func end_select= setup_end_select_func(join);
    if (join->table_count) {
       join->join_tab[join->top_join_tab_count - 1].next_select= end_select;
       join_tab=join->join_tab+join->const_tables;        // 對const表的優化
    }
    ... ...
    if (join->table_count == join->const_tables) {
        ... ...
    } else {
      if (join->outer_ref_cond && !join->outer_ref_cond->val_int())
        error= NESTED_LOOP_NO_MORE_ROWS;
      else
        error= sub_select(join,join_tab,0);   // 會調用sub_select，且不是最後一條
      if ((error == NESTED_LOOP_OK || error == NESTED_LOOP_NO_MORE_ROWS)
          && join->thd->killed != ABORT_QUERY)
        error= sub_select(join,join_tab,1);   // 最後的調用函數
      ... ...
    }
}

enum_nested_loop_state sub_select(JOIN *join,JOIN_TAB *join_tab,bool end_of_records)
{
  ... ...
  if (end_of_records)     // 如果已經到了記錄的結束位置，則直接調用qep_tab->next_select()
  {
    enum_nested_loop_state nls=
    (*qep_tab->next_select)(join,qep_tab+1,end_of_records); // 詳見###1
    DBUG_RETURN(nls);
  }
  READ_RECORD *info= &qep_tab->read_record;
  ... ...

  while (rc == NESTED_LOOP_OK && join->return_tab >= qep_tab_idx)
  {
    int error;
    if (in_first_read)                        // 首先讀取第一條記錄
    {                                         // 詳見###2，會在後面判斷是否滿足條件
      in_first_read= false;
      error= (*qep_tab->read_first_record)(qep_tab);
    }
    else
      error= info->read_record(info);         // 循環讀取記錄直到結束位置，詳見###3

    if (error > 0 || (join->thd->is_error())) // Fatal error
      rc= NESTED_LOOP_ERROR;
    else if (error < 0)
      break;
    else if (join->thd->killed)               // Aborted by user
    {
      join->thd->send_kill_message();
      rc= NESTED_LOOP_KILLED;
    }
    else
    {                                         // 如果沒有異常，則對記錄判斷是否滿足條件
      if (qep_tab->keep_current_rowid)
        qep_tab->table()->file->position(qep_tab->table()->record[0]);
      rc= evaluate_join_record(join, qep_tab);
    }
  }
  ... ...
}

static enum_nested_loop_state evaluate_join_record(JOIN *join, JOIN_TAB *join_tab, int error)
{
    ... ...
    COND* select_cond= join_tab->select_cond; // 取出相應的條件，其初始化在優化階段裡調用make_join_select()->
                             // add_not_null_conds() -> make_cond_for_table() 完成相應join_table的select_cond初始化
    ... ...
    if (select_cond) {       // 如果有查詢條件
        select_cond_result= MY_TEST(select_cond->val_int()); // 完成從引擎獲得的數據與query中該table的cond比較的過程
                             // 其最終調用的是Item_cmpfunc裡的接口，如對於簡單的數字等值比較使用Item_func_eq::val_int()->
                             // intArg_comparator::compare_int_signed()
    ... ...
    if (found) {
        rc= (*join_tab->next_select)(join,join_tab+1, 0);
    } else {
        join_tab->read_record.unlock_row(join_tab);   // 調用rr_unlock_row最終調用引擎的unlock_row對行進行解鎖
    }
    ... ...
}
{% endhighlight %}


對於如上的函數調用，其主要的處理過程在 sub_query() 函數中，暫時小結一下上面出現的三個比較重要的函數指針：

* join_tab->read_first_record<br>讀取第一條記錄使用的方法。

* info->read_record<br>讀取非第一條記錄使用的方法，對於多條記錄會循環調用該函數，該方法是根據 optimize 選擇的 join type 來指定的。

* join_tab->next_select<br>在 sub_select() 中的最後一條記錄調用，也就是連接下一個 table 的方法，這裡除了最後一個表使用 end_select 之外，其它的都使用 sub_select。

上述的三個指針基本組成了 nest loop 的主要過程。MySQL 會循環調用 read_record() 函數讀取當前表的記錄，如果有滿足條件的記錄則會調用 next_select 指針指定的函數。

接下來，分別講解上述源碼中的標記部分。

#### next_select

這也就是源碼中 ```###1``` 的標記；在創建 JOIN 實例時，也就是一個查詢的上下文信息時，默認該變量的值是 sub_select() 。

<!--
另外，在優化器中，會對該變量進行賦值。

其中 next_select 是在 optimize 的 make_join_readinfo() 裡初始化，會把所有的 join 表的 next_select 指針初始化為 sub_selec()，也就是說這裡如果還有 join_tab 需要 join 的話，再次進入 sub_select，這就是 nest_loop 的思想所在。

對於最後一個表的 next_select 則會在 do_select() 裡調用 setup_end_select() 來選擇對應的 end_select 函數。當執行到 end_select 時，這個就說明 join 操作完成，並把數據返回到客戶端。
-->



#### read_first_record

對應源碼中的 ```###2```，該函數主要用於讀取第一條記錄。

<!--
* 在 exec() -> exec_inner() -> make_simple_join() 中，將上述的值賦值為 join_init_read_record()，也就是默認值，對於沒有被優化的 join 則會使用該方法。
-->

* 在 optimize() -> pick_table_access_method() 根據 join type 的不同類型選擇相應的方法。<font color='blue'>注意：這裡的 join type 就是 explain 中的 type 字段對應值</font> 。

#### read_record

也就是源碼中的 ```###3```，用於讀取數據，該方法主要在兩個地方做賦值：

* 在讀取第一條記錄的入口函數中，會調用 <font color="blue">init_read_record() 函數初始化 info->read_record 變量</font>，如全表掃描調用 rr_sequential() 函數，對於 ref join 則調用 join_read_next_same() 等。

* 同樣在 pick_table_access_method() 函數中，設置 read_first_record 的同時設置了該變量。



## 總結

實際上的執行流程如下，在此以簡單的查詢為例。

{% highlight text %}
JOIN::exec()                              ← 在優化完成之後，根據執行計劃進行相應的查詢操作
  |-THD_STAGE_INFO()                      ← 設置線程的狀態為executing，在sql/mysqld.cc文件中定義
  |-set_executed()                        ← 設置為執行狀態，JOIN::executed=true
  |-prepare_result()
  |-send_result_set_metadata()            ← 先將元數據發送給客戶端
  |
  |-do_select()                           ←### 查詢的實際入口函數，做JOIN操作，會返回給客戶端或寫入表
    |                                     ←### <1>
    |
    |-join->first_select(join,qep_tab,0)  ← 1. 執行nest loop操作，默認會調用sub_select()函數，
    | |                                   ←    也即循環調用rnd_next()+evaluate_join_record()
    | |
    | |###while循環讀取數據###
    | |                                   ← 2. 調用存儲引擎接口讀取數據
    | |-qep_tab->read_first_record()      ← 2.1. 首次調用，實際為join_init_read_record()
    | | |-tab->quick()->reset()           ← 對於quick調用QUICK_RANGE_SELECT::reset()函數
    | | | |-file->ha_index_init()         ← 會調用存儲引擎接口
    | | | | |-index_init()
    | | | |   |-change_active_index()
    | | | |     |-innobase_get_index()
    | | | |-file->multi_range_read_init()
    | | |-init_read_record()              ← 設置read_record指針，在此為rr_quick
    | |
    | |-info->read_record()               ← 2.2 再次調用，如上，該函數在init_read_record()中初始化
    | | |-info->quick->get_next()         ← 實際調用QUICK_RANGE_SELECT::get_next()
    | |   |-file->multi_range_read_next() ← 調用handler.cc文件中函數
    | |     |-read_range_first()          ← 對於第一次調用
    | |     | |-ha_index_read_map()       ← 存儲引擎調用
    | |     |   |-index_read()
    | |     |     |-row_search_mvcc()
    | |     |
    | |     |-read_range_next()           ← 對於非第一次調用
    | |       |-ha_index_next()
    | |         |-general_fetch()
    | |           |-row_search_mvcc()
    | |
    | |-evaluate_join_record()            ← 2.3 處理讀取的記錄，判斷是否滿足條件，包括了第一條記錄
    |   |-qep_tab->next_select()          ← 對於查詢，實際會調用end_send()
    |     |-Query_result_send::send_data()
    |
    |-join->first_select(join,qep_tab,1)  ← 3. 一個table已經讀取數據結束，同樣默認調用sub_select()
    | |-join_tab->next_select()           ← 調用該函數處理下個表或者結束處理
    |
    |-join->select_lex->query_result()->send_eof()
{% endhighlight %}

在 ```<1>``` 中，對於簡單的查詢 (const且不需要臨時文件)，實際上只需要執行 end_select() 即可，上述介紹的是常用的查詢方式。

### sub_select()

這是 MySQL 的 JOIN 實現比較重要的函數，很多比較有用的函數信息，可以查看註釋。

{% highlight text %}
enum_nested_loop_state sub_select(JOIN *join, QEP_TAB *const qep_tab,bool end_of_records);

參數:
  join          : 本次查詢的上下文信息。
  join_tab      :
  end_of_records: 是否執行獲取記錄的最後一步。
{% endhighlight %}






## 示例

接下來通過幾個示例查看 nest loop 的執行過程。

### 全表掃描

如下是單表的全表掃描。

{% highlight text %}
mysql> explain select emp_no from salaries where salary = 90930;
+------+-------------+----------+------+---------------+------+---------+------+---------+-------------+
| id   | select_type | table    | type | possible_keys | key  | key_len | ref  | rows    | Extra       |
+------+-------------+----------+------+---------------+------+---------+------+---------+-------------+
|    1 | SIMPLE      | salaries | ALL  | NULL          | NULL | NULL    | NULL | 2838426 | Using where |
+------+-------------+----------+------+---------------+------+---------+------+---------+-------------+
1 row in set (0.00 sec)
{% endhighlight %}

這裡只有一個表，所以 join_tab 只有一個；而且是全表掃描，所以 read_first_record、read_record 兩個指針都被指定為 rr_sequential() 函數。

又因為是 select 語句，會直接將數據返回給客戶端，所以其中的 next_select 就是 end_send() 函數；如果是 insert ... select ... 語句的話，那麼會在 setup_end_select_func() 中設置為 end_write() 完成數據的寫入。

其執行流程基本如下。

![sequential](/images/databases/mysql/innodb-rr-sequential-1.png){: .pull-center}

通過 gdb 查看的結果如下：

{% highlight text %}
(gdb) set print pretty on
(gdb) b sub_select
Breakpoint 1 at 0x7fe9b7453ac8: file sql/sql_select.cc, line 18197.

----- 查看錶的數量，此時join_tab只有一個
(gdb) p join->table_count
$1 = 1

----- const表的數量，為0
(gdb) p join->const_tables
$2 = 0

(gdb) p join_tab->next_select
$3 = (Next_select_func) 0x7fe9b745653f <end_send(JOIN*, JOIN_TAB*, bool)>
(gdb) p join_tab->read_first_record
$4 = (READ_RECORD::Setup_func) 0x7fe9b7455d02 <join_init_read_record(st_join_table*)>
(gdb) p join_tab->read_record->read_record
$5 = (READ_RECORD::Read_func) 0x7fe9b776837b <rr_sequential(READ_RECORD*)>
{% endhighlight %}

實際的處理流程很簡單。

### 兩表簡單JOIN

如下的兩個表的 JOIN 操作，其中 e.gender 和 s.salary 都不是索引。

{% highlight text %}
mysql> explain select * from salaries s join employees e on (s.emp_no=e.emp_no) where
    -> e.gender='F' and s.salary=90930;
+------+-------------+-------+------+---------------+---------+---------+----------+--------+-------------+
| id   | select_type | table | type | possible_keys | key     | key_len | ref      | rows   | Extra       |
+------+-------------+-------+------+---------------+---------+---------+----------+--------+-------------+
| 1    | SIMPLE      | e     | ALL  | PRIMARY       | NULL    | NULL    | NULL     | 299469 | Using where |
| 1    | SIMPLE      | s     | ref  | PRIMARY       | PRIMARY | 4       | e.emp_no | 4      | Using where |
+------+-------------+-------+------+---------------+---------+---------+----------+--------+-------------+
2 rows in set (0.00 sec)
{% endhighlight %}

如上的 explain 結果可以基本確定 MySQL 的執行過程。

大概的執行過程是：e 表進入 sub_select() 執行，通過 rr_sequential() 函數獲取該表的每條記錄，然後通過 evaluate_join_record() 判斷該記錄是否滿足 e.gender='F' [using where]，如果沒有滿足則接著取下一條，滿足的話，則把它的 e.emp_no 傳遞給 s 表。

接下來 s 執行 sub_select，其 type 是 ref，就是通過索引來獲得記錄而非全表掃描，也即拿 e.emp_no 的值來檢索 s 的 PRIMARY KEY 來獲得記錄；最後再通過 s 的 evaluate_join_record() 判斷是否滿足 salary=90930 這個條件，如果滿足是直接發送給客戶端，否則獲得記錄進行判斷。

![sequential](/images/databases/mysql/innodb-rr-sequential-2.png){: .pull-center}

接下來仍然通過 gdb 查看上面兩個表的三個函數指針來驗證上面的過程：

{% highlight text %}
----- 查看錶名的順序，如下的兩種方式相同
(gdb) p join->join_tab[0]->table->alias->Ptr
(gdb) p join->table[0]->alias->Ptr
$1 = 0x7fe9870171c8 "e"

----- 查看錶以及const表的數量
(gdb) p join->table_count
$2 = 2
(gdb) p join->const_tables
$3 = 0

(gdb) p join_tab[0]->next_select
$4 = (Next_select_func) 0x7fe9b7453aac <sub_select(JOIN*, st_join_table*, bool)>
(gdb) p join_tab[1]->next_select
$5 = (Next_select_func) 0x7fe9b745653f <end_send(JOIN*, JOIN_TAB*, bool)>

(gdb) p join_tab[0]->read_first_record
$6 = (READ_RECORD::Setup_func) 0x7fe9b7455d02 <join_init_read_record(st_join_table*)>
(gdb) p join_tab[1]->read_first_record
$7 = (READ_RECORD::Setup_func) 0x7fe9b7455615 <join_read_always_key(JOIN_TAB*)>

(gdb) p join_tab[0]->read_record->read_record
$8 = (READ_RECORD::Read_func) 0x7fe9b776837b <rr_sequential(READ_RECORD*)>
(gdb) p join_tab[1]->read_record->read_record
$9 = (READ_RECORD::Read_func) 0x7fe9b7455985 <join_read_next_same(READ_RECORD*)>
{% endhighlight %}

處理上述的處理過程是兩個表的 JOIN 處理。



### 兩表 Const JOIN

與上面 SQL 不同的是，emp_no 採用的是主健，執行計劃如下：

{% highlight text %}
mysql> explain select * from salaries s join employees e on (s.emp_no=e.emp_no) where
    -> e.emp_no=62476 and s.salary=90930;
+------+-------------+-------+-------+---------------+---------+---------+-------+------+-------------+
| id   | select_type | table | type  | possible_keys | key     | key_len | ref   | rows | Extra       |
+------+-------------+-------+-------+---------------+---------+---------+-------+------+-------------+
| 1    | SIMPLE      | e     | const | PRIMARY       | PRIMARY | 4       | const | 1    |             |
| 1    | SIMPLE      | s     | ref   | PRIMARY       | PRIMARY | 4       | const | 15   | Using where |
+------+-------------+-------+-------+---------------+---------+---------+-------+------+-------------+
2 rows in set (0.00 sec)
{% endhighlight %}

正常來說，執行過程仍然如上所示，其中驅動表是 e，但是確實是這樣的嗎？實際上，在 sub_select() 設置斷點後，實際第一次的入參時 s 表，而非我們開始認為的 e 表。

{% highlight text %}
----- 查看錶名的順序，如下的兩種方式相同
(gdb) p join->join_tab[0]->table->alias->Ptr
$1 = 0x7fe9870171c8 "e"

----- 查看錶以及const表的數量
(gdb) p join->table_count
$2 = 2
(gdb) p join->const_tables
$3 = 1

----- 查看第一次進入sub_select()時的表名稱
(gdb) p join_tab->table->alias->Ptr
$4 = 0x7fe987017248 "s"

(gdb) p join_tab[0]->next_select
$4 = (Next_select_func) 0x7fe9b7453aac <sub_select(JOIN*, st_join_table*, bool)>
(gdb) p join_tab[1]->next_select
$5 = (Next_select_func) 0x7fe9b745653f <end_send(JOIN*, JOIN_TAB*, bool)>

(gdb) p join_tab[0]->read_first_record
$6 = (READ_RECORD::Setup_func) 0x7fe9b7455d02 <join_init_read_record(st_join_table*)>
(gdb) p join_tab[1]->read_first_record
$7 = (READ_RECORD::Setup_func) 0x7fe9b7455615 <join_read_always_key(JOIN_TAB*)>

(gdb) p join_tab[0]->read_record->read_record
$8 = (READ_RECORD::Read_func) 0x7fe9b776837b <rr_sequential(READ_RECORD*)>
(gdb) p join_tab[1]->read_record->read_record
$9 = (READ_RECORD::Read_func) 0x7fe9b7455985 <join_read_next_same(READ_RECORD*)>
{% endhighlight %}

實際上對於 const 類型，我們可以直接獲取到對應的查詢條件，此時只需要對 s 表進行過濾即可。

<!--
在代碼中，選擇第一個 table 時使用的時
--->

如果直接查看 salaries 表中 emp_no 是 62476 的記錄，會發現總共有 15 條記錄；那麼在 gdb 調試時，如果在 evaluate_join_record() 中設置斷點之後，總共執行了 16 次。除了比較 15 次記錄之外，在最後一條記錄，同樣會調用該函數。

這一部分在優化器 make_join_statistics()，詳細內容後面再介紹。

<!--
其實有這個現象的根本原因是do_select調用sub_select時指定的join_tab：
即這個join_tab是由const_tables指定的。而這個值則是在optimize的make_join_statistics根據優化情況進行賦值的。這個優化主要是指對const join可以直接獲得它的記錄，而不必通過sub_select去獲得。
這裡我們簡單說明一下make_join_statistics的過程：

http://blog.csdn.net/wudongxu/article/details/6683846
-->


{% highlight text %}
{% endhighlight %}
