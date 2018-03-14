---
title: MySQL 存儲引擎
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,plugin,插件,存儲引擎
description: 眾所周知，在 MySQL 中很多功能是通過插件實現的，包括了其中的存儲引擎。在此簡單介紹一下與 MySQL 存儲引擎相關的內容，包括了提供的接口，實現方法等。在 [MySQL 插件](/blog/mysql-plugin.html) 這篇文章中，已經討論了與插件相關的內容，包括了編譯、加載、使用方法等，同時也包括了存儲引擎，詳細使用方法可以參考這篇文章。在此，僅介紹下 MySQL 中，存儲引擎的實現。
---

眾所周知，在 MySQL 中很多功能是通過插件實現的，包括了其中的存儲引擎。在此簡單介紹一下與 MySQL 存儲引擎相關的內容，包括了提供的接口，實現方法等。

在 [MySQL 插件](/blog/mysql-plugin.html) 這篇文章中，已經討論了與插件相關的內容，包括了編譯、加載、使用方法等，同時也包括了存儲引擎，詳細使用方法可以參考這篇文章。

在此，僅介紹下 MySQL 中，存儲引擎的實現。

<!-- more -->

## 簡介

實際上，所為的存儲引擎，是按照按照 MySQL 的接口定義，提供所需接口的實現而已；如插入一條記錄時將調用 write_row()，通過索引檢索時將調用 index_read() 和 index_next() 等。

抽象後的接口極大地加快了在 MySQL 中加入其它存儲引擎的步伐，該接口在 v3.22 升級到 v3.23 時引入，在快速集成 InnoDB 存儲引擎時起了很大的幫助。

接下來看看 MySQL 的存儲引擎是如何實現的。

### 數據結構

MySQL 與存儲引擎之間的接口主要由 sql/handler.h 文件中的 class handler 和 struct handlerton。其中兩者的主要區別是：struct handlerton 定義了事務操作接口；class handler 定義了表、索引及記錄操作接口。

實際上，最初版本只存在 handler ，從 5.0 版本開始，為了避免在初始化/事務提交/保存事務點/回滾操作時需要操作 one-table 實例，才引入了 handlerton 。

也就是說，對不需要支持事務的存儲引擎只需要創建一個 handler 的派生類，並重載該引擎需要支持的方法；而對於需要支持事務的存儲引擎，還需要實現 handlerton 結構中相應的接口。

#### class handler@sql/handler.h

這個類定義了對錶操作的常見接口，其只繼承了 Sql_alloc 類，而 Sql_alloc 沒有任何成員變量，純粹重載了 new 和 delete 操作。

所以 handler 類分配內存是可以從連接相關的內存池來分配；而刪除時不需要做任何事情，內存釋放只會在 mysys/my_alloc.c 中的 free_root() 調用發生，無需顯性去釋放，在語句執行之後清理。

{% highlight cpp %}
class handler :public Sql_alloc
{
public:
    handlerton *ht;                  // 該handler的存儲引擎
    uchar *ref;                      // 當前行的指針
    uint auto_inc_intervals_count;   // 自增值對應的內部變量
public:
    int ha_rnd_init(bool scan) __attribute__ ((warn_unused_result));
    int ha_rnd_end();
    int ha_rnd_init_with_error(bool scan) __attribute__ ((warn_unused_result));
    int ha_reset();
    int ha_external_lock(THD *thd, int lock_type);
    int ha_write_row(uchar * buf);
    int ha_update_row(const uchar * old_data, uchar * new_data);
    int ha_delete_row(const uchar * buf);
    void ha_release_auto_increment();

    // 創建表
    //   name: 創建表名；
    //   from: 表類型的結構，要創建表的定義，跟tablename.frm文件內容是匹配的
    //   info: 含客戶端輸入的CREATE TABLE語句的信息
    int ha_create(const char *name, TABLE *form, HA_CREATE_INFO *info);

    // 打開一個表
    int ha_open(TABLE *table, const char *name, int mode, uint test_if_locked);

    // 關閉表
    int ha_close(void);

    // 當客戶端調用LOCK TABLE時
    virtual int external_lock(THD *thd __attribute__((unused)),
                              int lock_type __attribute__((unused)));

    // 初始化全表掃描，後者會帶有統計值
    int rnd_init(bool scan) __attribute__ ((warn_unused_result));
    int ha_rnd_init(bool scan) __attribute__ ((warn_unused_result));
    // 從表中讀取下一行
    virtual int rnd_next(uchar *buf)=0;
    virtual int ha_rnd_next(uchar *buf);

    // 使用索引前調用該方法
    virtual int index_init(uint idx, bool sorted);
    // 使用索引後調用該方法
    virtual int index_end();
    // 讀取索引第一條內容
    int ha_index_first(uchar * buf);
    // 讀取索引下一條內容
    int ha_index_next(uchar * buf);
    // 讀取索引前一條內容
    int ha_index_prev(uchar * buf);
    // 讀取索引最後一條內容
    int ha_index_last(uchar * buf);
    // 給定一個key基於索引讀取內容
    virtual int index_read(uchar * buf, const uchar * key, uint key_len,
                         enum ha_rkey_function find_flag);

    // 開始一個事務
    virtual int start_stmt(THD *thd, thr_lock_type lock_type);
};
{% endhighlight %}

對錶接口的抽象類，提供了針對單個表的操作，如 open()、write_row() 等，大約 150 種方法。每一個 table 描述符對應一個 handler 實例，如果同一個 table 被打開多次，那麼這時候會出現多個 handler 實例。

#### struct handlerton@sql/handler.h

每個存儲引擎只有一個該結構，提供了會影響整個存儲引擎的接口，負責存儲引擎初始化，事務相關操作等，如 commit()、show_status() 等，大約有 30 多種。

{% highlight cpp %}
struct handlerton {
    int  (*close_connection)(handlerton *hton, THD *thd);
    void (*kill_query)(handlerton *hton, THD *thd, enum thd_kill_levels level);



    // 提交一個事務
    int (*commit)(handlerton *hton, THD *thd, bool all);
    // 回滾一個事務
    int  (*rollback)(handlerton *hton, THD *thd, bool all);
};
{% endhighlight %}


在插件初始化的時候，會對 handlerton 對象進行初始化操作，而該對象中又包含了對 handler 的創建入口。



### 添加三方存儲引擎

實際上，在 5.1 之後版本添加變得簡單多，可以根據 "blackhole" 存儲引擎的模式來改造；另外，還有 "example" 存儲引擎可以參考。

可以參考 [MySQL 插件詳解](/post/mysql-plugin.html) 中的介紹。

<!--
1、創建目錄 storage/xx-csv,實際文件ha_csv.h(cc)移至該目錄
2、Makefile.am 放入storage/xx-csv
3、configure.in 改寫MYSQL_STORAGE_ENGINE宏
4、autoconf & automake & ./configure --prefix=/usr --with-plugins=xx-csv & make
-->





<!--
## Step By Step

下面從頭開始創建一個存儲引擎。

<h3>只讀存儲引擎</h3><p><ol><li>
定義變量<br>
提供了兩個變量來控制存儲引擎的行為，變量通過MYSQL_SYSVAR_XXX宏來定義，定義的變量可以在啟動時通過--xxx設置。注意：在通過MYSQL_SYSVAR設置時，會自動添加存儲引擎的前綴。</li><br><li>

定義插件<br>
插件通過mysql_declare_plugin宏來定義，其中包括了一個初始化函數，主要用來設置handlerton結構體。在此，只設置handlerton.create函數變量，該函數用來創建繼承自class handler的類實例。</li><br><li>

定義class handler子類<br>
handler中定義了大量的接口函數，不過只有部分是純虛函數，也就是必須要實現的。
</li></ol>
表中通常會存在兩種鎖，一種是針對每個表的；另一種是針對handler實例的。通常來說handler實例會有多個，如self-join、多個連接同時讀取一個表，此時每個實例都需要有一個鎖，對此的實現是在類中添加一個變量。<br><br>

對於第一種，通常可以維護一個針對表明的hash表，並使用mutex對其進行保護，在此我們採用了其它的方法。因為在傳入表名的同時，會傳入一個TABLE對象，該對象是和handler一一對應的。同時TABLE對象維護了一個TABLE_SHARE對象，也是每個表都有一個TABLE_SHARE對象。如table是一個指向TABLE的指針，table->s就對應TABLE_SHARE，table->s->ha_data是一個void*指針，可以用於其它目的。<br><br>

如果沒有分區的話，我們可以直接返回table->s->ha_data即可。對於分區來說，所有的分區有同一個TABLE對象和TABLE_SHARE對象，但是分區從存儲引擎的角度來說是不同的，而且只能通過table-name區分，通常是表明+序號。為此，每個表都維護了一個struct STATIC_SHARE的單向鏈表。<br><br>


下面詳細介紹class handler定義的接口。<ul><li>
create()<br>
在存儲引擎中創建一個表。</li><br><li>

open()<br>
打開一個表。</li><br><li>


<pre style="font-size:0.8em; face:arial;">
mysql> install plugin STATIC_TEXT soname 'ha_text.so';
mysql> create table t1 (a int, b varchar(50)) engine=static_text;
mysql> select * from t1;
mysql> show variables like 'static%';
mysql> set global static_text_rows = 5;
mysql> set global static_text_text = 'This is a test!';
mysql> select * from t1;
mysql> select * from t1 where a > 3 order by a desc;
</pre>

<h3>功能詳解</h3><p>
接口通過抽象類 handler 來實現，handler類提供如打開/關閉table,掃表,查詢Key數據,寫記錄跟刪除記錄等基礎操作方法。每一個存儲引擎通過繼承handler類，實現以上提到的方法，在方法裡面實現對底層存儲引擎的讀寫接口的轉調。從5.0版本開始，為了避免在初始化/事務提交/保存事務點/回滾操作時需要操作one-table實例，引入了handlerton結構讓存儲引擎提供了發生上面提到操作時的鉤子操作。
MySQL提供了訪問不同的存儲引擎數據表的虛擬層API。過去,這些接口被稱之為"table handler".現在有了新的介紹——storage engine.目前我們所提到的storage engine,往往是與存儲/讀取數據的相關代碼，而table handler指的是storage engine與MySQL優化器的接口.


下面主要講handler類和handlerton結構體，並提供一個實例，讀取逗號分隔符格式文件的簡單存儲引擎。

handler類
定義於sql/handler.h、sql/handler.cc
handler類從Sql_alloc繼承，Sql_alloc沒有任何成員變量，純粹重載了new和delete操作。所以handler類分配內存是可以從連接相關的內存池來分配。而delete操作時不做任何事情。內存的釋放只會在mysys/my_alloc.c的free_root()調用發生。無需顯性去釋放，在語句執行之後清理。

每一個table描述符對應一個handler的實例。注意針對同一個table可能會被打開多次的情況，這時候會出現多個handler實例。5.0版本引入index_merge方法後導致了一些有趣的方式，以前多個handler實例只會在table cache中拷貝描述符產生，引入Index_merge之後，handler實例可能會在優化階段被創建。

handler類成員變量
st_table*: 與handler實例相關的table描述符.
byte* ref: 保存當前記錄。存儲引擎類型不同，這個字段表示的意義不同。
     MyISAM: 在data-file裡面的offset InnoDB:primary key MEMORY:Point
ulonglong auto_increment_value;
ulong mean_rec_length; 記錄平均長度，SHOW TABLE STATUS
等
handler類成員函數(暫略)

handlerton
4.1之前的版本，從handler類繼承是唯一一個存儲引擎可以與核心結構交互的途徑。
當優化器需要針對存儲引擎做一些事情的時候，只可能調用當前table相關的handler實例的接口方法。但是隨著集成多種存儲引擎的發展，盡是靠handler方法來交互的形式是不太夠的，因此，handlerton概念誕生。

handlerton是一個幾乎全是回掉方法指針的C結構體。定義在sql/handler.h
回調函數在某一事件發生時針對具體的存儲引擎被調用(事務提交/保存事務點/連接關閉等)

每個鏈接都會有一個 handler 實例來處理 SQL 請求。
-->

## 源碼解析

在此簡單介紹 MySQL 中，存儲引擎的接口調用過程。

在此，以二級索引讀取為例，其入口函數實際為 ha_index_next()@sql/handler.cc，而最終是通過 MYSQL_TABLE_IO_WAIT() 宏調用，如未開啟 PSI 接口，則直接調用 index_next(buf) 函數。

{% highlight cpp %}
int handler::ha_index_next(uchar * buf)
{
  int result;
  DBUG_ENTER("handler::ha_index_next");
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  DBUG_ASSERT(inited == INDEX);
  DBUG_ASSERT(!pushed_idx_cond || buf == table->record[0]);

  // Set status for the need to update generated fields
  m_update_generated_read_fields= table->has_gcol();

  MYSQL_TABLE_IO_WAIT(PSI_TABLE_FETCH_ROW, active_index, result,
    { result= index_next(buf); })
  if (!result && m_update_generated_read_fields)
  {
    result= update_generated_read_fields(buf, table, active_index);
    m_update_generated_read_fields= false;
  }
  DBUG_RETURN(result);
}
{% endhighlight %}

對應不同的存儲引擎，實際是通過類似 int index_next(uchar * buf) 函數讀取的，如下是 handler 的定義，不同的存儲引擎會定義不同接口函數。

{% highlight cpp %}
class handler :public Sql_alloc
{
  /// @returns @see index_read_map().
  virtual int index_next(uchar * buf)
   { return  HA_ERR_WRONG_COMMAND; }
}
{% endhighlight %}

那麼，對於不同的接口，會有不同的調用，視情況而定。

## InnoDB

實際上，在 handler/ha_innodb.cc 文件中，除了定義 innodb 存儲引擎之外，還包括了一些其它插件，如 trx、locks 等，可以通過 SHOW PLUGINS 查看。如下是源碼中的部分內容：

{% highlight cpp %}
mysql_declare_plugin(innobase)
{
  MYSQL_STORAGE_ENGINE_PLUGIN,
  &innobase_storage_engine,
  innobase_hton_name,
  plugin_author,
  "Supports transactions, row-level locking, and foreign keys",
  PLUGIN_LICENSE_GPL,
  innobase_init, /* Plugin Init */
  NULL, /* Plugin Deinit */
  INNODB_VERSION_SHORT,
  innodb_status_variables_export,/* status variables             */
  innobase_system_variables, /* system variables */
  NULL, /* reserved */
  0,    /* flags */
},
i_s_innodb_trx,
i_s_innodb_locks,
i_s_innodb_lock_waits,
i_s_innodb_cmp,
i_s_innodb_cmp_reset,
... ...,
mysql_declare_plugin_end;
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
