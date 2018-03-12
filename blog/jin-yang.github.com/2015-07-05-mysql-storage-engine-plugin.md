---
title: MySQL 存储引擎
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,plugin,插件,存储引擎
description: 众所周知，在 MySQL 中很多功能是通过插件实现的，包括了其中的存储引擎。在此简单介绍一下与 MySQL 存储引擎相关的内容，包括了提供的接口，实现方法等。在 [MySQL 插件](/blog/mysql-plugin.html) 这篇文章中，已经讨论了与插件相关的内容，包括了编译、加载、使用方法等，同时也包括了存储引擎，详细使用方法可以参考这篇文章。在此，仅介绍下 MySQL 中，存储引擎的实现。
---

众所周知，在 MySQL 中很多功能是通过插件实现的，包括了其中的存储引擎。在此简单介绍一下与 MySQL 存储引擎相关的内容，包括了提供的接口，实现方法等。

在 [MySQL 插件](/blog/mysql-plugin.html) 这篇文章中，已经讨论了与插件相关的内容，包括了编译、加载、使用方法等，同时也包括了存储引擎，详细使用方法可以参考这篇文章。

在此，仅介绍下 MySQL 中，存储引擎的实现。

<!-- more -->

## 简介

实际上，所为的存储引擎，是按照按照 MySQL 的接口定义，提供所需接口的实现而已；如插入一条记录时将调用 write_row()，通过索引检索时将调用 index_read() 和 index_next() 等。

抽象后的接口极大地加快了在 MySQL 中加入其它存储引擎的步伐，该接口在 v3.22 升级到 v3.23 时引入，在快速集成 InnoDB 存储引擎时起了很大的帮助。

接下来看看 MySQL 的存储引擎是如何实现的。

### 数据结构

MySQL 与存储引擎之间的接口主要由 sql/handler.h 文件中的 class handler 和 struct handlerton。其中两者的主要区别是：struct handlerton 定义了事务操作接口；class handler 定义了表、索引及记录操作接口。

实际上，最初版本只存在 handler ，从 5.0 版本开始，为了避免在初始化/事务提交/保存事务点/回滚操作时需要操作 one-table 实例，才引入了 handlerton 。

也就是说，对不需要支持事务的存储引擎只需要创建一个 handler 的派生类，并重载该引擎需要支持的方法；而对于需要支持事务的存储引擎，还需要实现 handlerton 结构中相应的接口。

#### class handler@sql/handler.h

这个类定义了对表操作的常见接口，其只继承了 Sql_alloc 类，而 Sql_alloc 没有任何成员变量，纯粹重载了 new 和 delete 操作。

所以 handler 类分配内存是可以从连接相关的内存池来分配；而删除时不需要做任何事情，内存释放只会在 mysys/my_alloc.c 中的 free_root() 调用发生，无需显性去释放，在语句执行之后清理。

{% highlight cpp %}
class handler :public Sql_alloc
{
public:
    handlerton *ht;                  // 该handler的存储引擎
    uchar *ref;                      // 当前行的指针
    uint auto_inc_intervals_count;   // 自增值对应的内部变量
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

    // 创建表
    //   name: 创建表名；
    //   from: 表类型的结构，要创建表的定义，跟tablename.frm文件内容是匹配的
    //   info: 含客户端输入的CREATE TABLE语句的信息
    int ha_create(const char *name, TABLE *form, HA_CREATE_INFO *info);

    // 打开一个表
    int ha_open(TABLE *table, const char *name, int mode, uint test_if_locked);

    // 关闭表
    int ha_close(void);

    // 当客户端调用LOCK TABLE时
    virtual int external_lock(THD *thd __attribute__((unused)),
                              int lock_type __attribute__((unused)));

    // 初始化全表扫描，后者会带有统计值
    int rnd_init(bool scan) __attribute__ ((warn_unused_result));
    int ha_rnd_init(bool scan) __attribute__ ((warn_unused_result));
    // 从表中读取下一行
    virtual int rnd_next(uchar *buf)=0;
    virtual int ha_rnd_next(uchar *buf);

    // 使用索引前调用该方法
    virtual int index_init(uint idx, bool sorted);
    // 使用索引后调用该方法
    virtual int index_end();
    // 读取索引第一条内容
    int ha_index_first(uchar * buf);
    // 读取索引下一条内容
    int ha_index_next(uchar * buf);
    // 读取索引前一条内容
    int ha_index_prev(uchar * buf);
    // 读取索引最后一条内容
    int ha_index_last(uchar * buf);
    // 给定一个key基于索引读取内容
    virtual int index_read(uchar * buf, const uchar * key, uint key_len,
                         enum ha_rkey_function find_flag);

    // 开始一个事务
    virtual int start_stmt(THD *thd, thr_lock_type lock_type);
};
{% endhighlight %}

对表接口的抽象类，提供了针对单个表的操作，如 open()、write_row() 等，大约 150 种方法。每一个 table 描述符对应一个 handler 实例，如果同一个 table 被打开多次，那么这时候会出现多个 handler 实例。

#### struct handlerton@sql/handler.h

每个存储引擎只有一个该结构，提供了会影响整个存储引擎的接口，负责存储引擎初始化，事务相关操作等，如 commit()、show_status() 等，大约有 30 多种。

{% highlight cpp %}
struct handlerton {
    int  (*close_connection)(handlerton *hton, THD *thd);
    void (*kill_query)(handlerton *hton, THD *thd, enum thd_kill_levels level);



    // 提交一个事务
    int (*commit)(handlerton *hton, THD *thd, bool all);
    // 回滚一个事务
    int  (*rollback)(handlerton *hton, THD *thd, bool all);
};
{% endhighlight %}


在插件初始化的时候，会对 handlerton 对象进行初始化操作，而该对象中又包含了对 handler 的创建入口。



### 添加三方存储引擎

实际上，在 5.1 之后版本添加变得简单多，可以根据 "blackhole" 存储引擎的模式来改造；另外，还有 "example" 存储引擎可以参考。

可以参考 [MySQL 插件详解](/post/mysql-plugin.html) 中的介绍。

<!--
1、创建目录 storage/xx-csv,实际文件ha_csv.h(cc)移至该目录
2、Makefile.am 放入storage/xx-csv
3、configure.in 改写MYSQL_STORAGE_ENGINE宏
4、autoconf & automake & ./configure --prefix=/usr --with-plugins=xx-csv & make
-->





<!--
## Step By Step

下面从头开始创建一个存储引擎。

<h3>只读存储引擎</h3><p><ol><li>
定义变量<br>
提供了两个变量来控制存储引擎的行为，变量通过MYSQL_SYSVAR_XXX宏来定义，定义的变量可以在启动时通过--xxx设置。注意：在通过MYSQL_SYSVAR设置时，会自动添加存储引擎的前缀。</li><br><li>

定义插件<br>
插件通过mysql_declare_plugin宏来定义，其中包括了一个初始化函数，主要用来设置handlerton结构体。在此，只设置handlerton.create函数变量，该函数用来创建继承自class handler的类实例。</li><br><li>

定义class handler子类<br>
handler中定义了大量的接口函数，不过只有部分是纯虚函数，也就是必须要实现的。
</li></ol>
表中通常会存在两种锁，一种是针对每个表的；另一种是针对handler实例的。通常来说handler实例会有多个，如self-join、多个连接同时读取一个表，此时每个实例都需要有一个锁，对此的实现是在类中添加一个变量。<br><br>

对于第一种，通常可以维护一个针对表明的hash表，并使用mutex对其进行保护，在此我们采用了其它的方法。因为在传入表名的同时，会传入一个TABLE对象，该对象是和handler一一对应的。同时TABLE对象维护了一个TABLE_SHARE对象，也是每个表都有一个TABLE_SHARE对象。如table是一个指向TABLE的指针，table->s就对应TABLE_SHARE，table->s->ha_data是一个void*指针，可以用于其它目的。<br><br>

如果没有分区的话，我们可以直接返回table->s->ha_data即可。对于分区来说，所有的分区有同一个TABLE对象和TABLE_SHARE对象，但是分区从存储引擎的角度来说是不同的，而且只能通过table-name区分，通常是表明+序号。为此，每个表都维护了一个struct STATIC_SHARE的单向链表。<br><br>


下面详细介绍class handler定义的接口。<ul><li>
create()<br>
在存储引擎中创建一个表。</li><br><li>

open()<br>
打开一个表。</li><br><li>


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

<h3>功能详解</h3><p>
接口通过抽象类 handler 来实现，handler类提供如打开/关闭table,扫表,查询Key数据,写记录跟删除记录等基础操作方法。每一个存储引擎通过继承handler类，实现以上提到的方法，在方法里面实现对底层存储引擎的读写接口的转调。从5.0版本开始，为了避免在初始化/事务提交/保存事务点/回滚操作时需要操作one-table实例，引入了handlerton结构让存储引擎提供了发生上面提到操作时的钩子操作。
MySQL提供了访问不同的存储引擎数据表的虚拟层API。过去,这些接口被称之为"table handler".现在有了新的介绍——storage engine.目前我们所提到的storage engine,往往是与存储/读取数据的相关代码，而table handler指的是storage engine与MySQL优化器的接口.


下面主要讲handler类和handlerton结构体，并提供一个实例，读取逗号分隔符格式文件的简单存储引擎。

handler类
定义于sql/handler.h、sql/handler.cc
handler类从Sql_alloc继承，Sql_alloc没有任何成员变量，纯粹重载了new和delete操作。所以handler类分配内存是可以从连接相关的内存池来分配。而delete操作时不做任何事情。内存的释放只会在mysys/my_alloc.c的free_root()调用发生。无需显性去释放，在语句执行之后清理。

每一个table描述符对应一个handler的实例。注意针对同一个table可能会被打开多次的情况，这时候会出现多个handler实例。5.0版本引入index_merge方法后导致了一些有趣的方式，以前多个handler实例只会在table cache中拷贝描述符产生，引入Index_merge之后，handler实例可能会在优化阶段被创建。

handler类成员变量
st_table*: 与handler实例相关的table描述符.
byte* ref: 保存当前记录。存储引擎类型不同，这个字段表示的意义不同。
     MyISAM: 在data-file里面的offset InnoDB:primary key MEMORY:Point
ulonglong auto_increment_value;
ulong mean_rec_length; 记录平均长度，SHOW TABLE STATUS
等
handler类成员函数(暂略)

handlerton
4.1之前的版本，从handler类继承是唯一一个存储引擎可以与核心结构交互的途径。
当优化器需要针对存储引擎做一些事情的时候，只可能调用当前table相关的handler实例的接口方法。但是随着集成多种存储引擎的发展，尽是靠handler方法来交互的形式是不太够的，因此，handlerton概念诞生。

handlerton是一个几乎全是回掉方法指针的C结构体。定义在sql/handler.h
回调函数在某一事件发生时针对具体的存储引擎被调用(事务提交/保存事务点/连接关闭等)

每个链接都会有一个 handler 实例来处理 SQL 请求。
-->

## 源码解析

在此简单介绍 MySQL 中，存储引擎的接口调用过程。

在此，以二级索引读取为例，其入口函数实际为 ha_index_next()@sql/handler.cc，而最终是通过 MYSQL_TABLE_IO_WAIT() 宏调用，如未开启 PSI 接口，则直接调用 index_next(buf) 函数。

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

对应不同的存储引擎，实际是通过类似 int index_next(uchar * buf) 函数读取的，如下是 handler 的定义，不同的存储引擎会定义不同接口函数。

{% highlight cpp %}
class handler :public Sql_alloc
{
  /// @returns @see index_read_map().
  virtual int index_next(uchar * buf)
   { return  HA_ERR_WRONG_COMMAND; }
}
{% endhighlight %}

那么，对于不同的接口，会有不同的调用，视情况而定。

## InnoDB

实际上，在 handler/ha_innodb.cc 文件中，除了定义 innodb 存储引擎之外，还包括了一些其它插件，如 trx、locks 等，可以通过 SHOW PLUGINS 查看。如下是源码中的部分内容：

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
