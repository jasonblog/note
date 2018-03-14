---
title: MySQL 插件詳解
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,plugin,插件
description: 在 MySQL 中，為了提高其靈活性，很多的功能都是通過插件來實現的，常見的比如 semi-sync、存儲引擎、登陸認證等等。因為 MySQL 是 C/C++ 實現的，對於插件來說實際為動態鏈接庫，保存在 plugin_dir 變量對應的目錄下。在此介紹一下 MySQL 的插件實現。
---

在 MySQL 中，為了提高其靈活性，很多的功能都是通過插件來實現的，常見的比如 semi-sync、存儲引擎、登陸認證等等。因為 MySQL 是 C/C++ 實現的，對於插件來說實際為動態鏈接庫，保存在 plugin_dir 變量對應的目錄下。

在此介紹一下 MySQL 的插件實現。

<!-- more -->

## 簡介

插件的安裝也比較簡單，可以在配置文件中通過 `plugin_load="XXX"` 設置，或者通過 MySQL 命令 `install plugin` 安裝，安裝之後會在 `mysql.plugin` 中插入一條記錄，在下次啟動時，將會自動加載。

另外，可以在啟動時通過 `--disable-plugin-plg-name` 或 `--plugin-plg-name=OFF` 禁止插件啟動，此時表中為 `DISABLED` 。如果不能加載插件，默認會繼續啟動，如果設置為沒有該插件則停止啟動可以使用，`--plugin-my-plugin=FORCE` 。

例如，可以通過如下方式加載插件。

{% highlight text %}
$ cat ~/.my.cnf
plugin_load="rpl_semi_sync_master=semisync_master.so"

mysql > SHOW VARIABLES LIKE 'plugin_dir';                                       ← 查看插件的目錄
mysql > INSTALL PLUGIN plg_name SONAME "plg_dynamic.so";                        ← 安裝插件
mysql > UNINSTALL PLUGIN plg_name;                                              ← 卸載插件
mysql > SHOW PLUGINS;                                                           ← 查看當前插件，同下
mysql > SELECT * FROM information_schema.plugins WHERE PLUGIN_NAME='plg-name';  ← 詳細信息
mysql > SELECT * FROM mysql.plugin;                                             ← 查看已經安裝的插件
{% endhighlight %}

在 `information_schema.plugins` 中可以看到，存在了三個版本信息，其中 `plugin_version` 作者可以隨意指定；`plugin_type_version` 和 `plugin_library_version` 決定了 API 的版本。

### 插件示例

插件的示例可以參考 `plugin/daemon_example` ，該插件會在 `show variable like 'datadir'` 目錄下創建一個 `mysql-heartbeat.log` 日誌文件，每隔 5s 打印一條日誌。

可以通過 `show plugins` 查看插件，也可以查看 `mysql.plugin` 表。

## 存儲引擎

MySQL 提供了訪問不同的存儲引擎數據表的虛擬層 API，"table handler" 指的是 storage engine 與 MySQL 優化器的接口，通過這些接口很多存儲引擎可以很快集成到 MySQL 中。

在 MySQL 中採用了插件式的存儲引擎，與插件類似，存儲引擎以 `.so` 文件的格式進行保存，均存放在 `plugin_dir` 變量指定大目錄下，可以通過如下的方式進行查看、安裝、卸載。

{% highlight text %}
mysql > SHOW VARIABLES LIKE 'have_dynamic_loading';     ← 查看是否支持動態加載
mysql > SHOW ENGINES;                                   ← 或者SHOW STORAGE ENGINES;
mysql > INSTALL PLUGIN Innodb SONAME "ha_innodb.so";
mysql > UNINSTALL PLUGIN Innodb;
{% endhighlight %}

注意安裝時指定的名稱需要與 `st_mysql_plugin.name` 相同，大小寫不敏感。

### TokuDB 使用

一個類似 InnoDB 的存儲引擎，對於 percona 發行版本本身已經帶了 TokuDB，可以通過如下方式查看。

{% highlight text %}
mysql> show variables like 'plugin_dir';                   ← 查看插件保存的路徑
$ ls {plugin_dir} | grep toku                              ← 查看是否存在該引擎

# yum instrall jemalloc                                    ← 安裝jemalloc庫，依賴epel
# LD_PRELOAD=/usr/lib64/libjemalloc.so.1 mysqld ...        ← 需要通過LD_PRELOAD指定庫的位置

# echo never > /sys/kernel/mm/transparent_hugepage/enabled ← 關閉hugepage，實際只需要關閉這一選項即可
# echo never > /sys/kernel/mm/transparent_hugepage/defrag

mysql> INSTALL PLUGIN tokudb SONAME 'ha_tokudb.so';        ← 安裝TokuDB
mysql> SHOW ENGINES;                                       ← 查看所有的引擎
mysql> SHOW VARIABLES LIKE 'tokudb%';                      ← 查看TokuDB相關配置

mysql> UNINSTALL PLUGIN  tokudb;                           ← 卸載之
{% endhighlight %}

使用 TokuDB 引擎時，需要修改內核配置，禁用 transparent_hugepage，否則可能會導致 TokuDB 內存洩露。另外，需要注意的是，在第一次插入之後再啟動時會自動加載。

如果是通過 mysqld_safe 啟動，需要在配置文件中添加如下的選項。

{% highlight text %}
[mysqld_safe]
malloc_lib = /usr/lib64/libjemalloc.so
{% endhighlight %}

另外，可以參考 TokuDB 的官方文檔，[Percona TokuDB - Documentation](https://www.percona.com/doc/percona-tokudb/index.html) 。

### 存儲引擎示例

在源碼的 `storage/example` 目錄下有一個簡單的存儲引擎示例，基本就是什麼也不做，只是告知你應該如何添加一個存儲引擎。

可以在編譯時添加 `-DWITH_EXAMPLE_STORAGE_ENGINE=ON` 選項，或者在該目錄下執行 `make` 命令，此時會生成一個 `ha_example.so` 文件。然後可以通過如下的方式添加該引擎：

{% highlight text %}
mysql > INSTALL PLUGIN example SONAME 'ha_example.so';
mysql > CREATE TABLE test (i INT) ENGINE = EXAMPLE;     // 測試
Query OK, 0 rows affected (0.02 sec)
mysql > INSERT INTO test VALUES(1),(2),(3);
Query OK, 3 rows affected (0.00 sec)
Records: 3  Duplicates: 0  Warnings: 0
mysql > SELECT * FROM test;
Empty set (0.00 sec)
mysql > UNINSTALL PLUGIN example;
{% endhighlight %}

也可以參考一個示例 [skeleton-mysql-engine](http://bazaar.launchpad.net/~statik/+junk/skeleton-mysql-engine/files) ，不過有點老了。

### DIY 創建簡單存儲引擎

在此我們簡單創建一個示例存儲引擎，實際上類似於上述的 example 存儲引擎。

創建一個 foobar 存儲引擎，可以直接從簡單的示例複製新目錄即可，此處假設從 blackhole 複製到新的文件夾，該存儲引擎可以創建 insert 之後不能 select 查到。

blackhole 存儲引擎，實際不會存儲任何數據，如果開啟了 binlog 則會記錄 binlog 。

{% highlight text %}
mysql> CREATE TABLE test(i INT, c CHAR(10)) ENGINE = BLACKHOLE;
mysql> INSERT INTO test VALUES(1,'record one'),(2,'record two');
mysql> SELECT * FROM test;
{% endhighlight %}

下面以 blackhole 作為示例創建我們的 foobar 。

{% highlight text %}
$ cd storage/ && cp blackhole foobar -rf && cd foobar
$ sed -e s/\<BLACKHOLE\>/FOOBAR/g -e s/\<blackhole\>/foobar/g ha_blackhole.h > ha_foobar.h
$ sed -e s/\<BLACKHOLE\>/FOOBAR/g -e s/\<blackhole\>/foobar/g ha_blackhole.cc > ha_foobar.cc
$ cat CMakeLists.txt
SET(FOOBAR_SOURCES  ha_foobar.cc ha_foobar.h)
MYSQL_ADD_PLUGIN(foobar ${FOOBAR_SOURCES} STORAGE_ENGINE)
$ cd ../../build && make                                 ← 第一次創建時
$ cd ../../build/storage/foobar && make                  ← 重新修改之後，只需要在該目錄make即可
# make install                                           ← 安裝需要root權限
{% endhighlight %}

到此為止，可以像使用 blackhole 引擎一樣使用 foobar 。



## 實現

在此介紹一下 MySQL 中，插件的源碼實現機制。

### 插件定義

這裡也就是一些插件的通用實現，plugin API 的實現在 sql/sql_plugin.cc 這個文件中，載入 plugin 使用 dl_open 動態加載共享庫的方法打開 so 文件，獲得需要執行的加載函數和卸載函數的指針。

插件類型及相關宏定義在 include/sql/plugin.h，包括了些常見的類型，例如：

{% highlight c %}
#define MYSQL_UDF_PLUGIN                 0  /* User-defined function        */
#define MYSQL_STORAGE_ENGINE_PLUGIN      1  /* Storage Engine               */
#define MYSQL_FTPARSER_PLUGIN            2  /* Full-text parser plugin      */
#define MYSQL_DAEMON_PLUGIN              3  /* The daemon/raw plugin type */
#define MYSQL_INFORMATION_SCHEMA_PLUGIN  4  /* The I_S plugin type */
#define MYSQL_AUDIT_PLUGIN               5  /* The Audit plugin type        */
#define MYSQL_REPLICATION_PLUGIN         6  /* The replication plugin type */
#define MYSQL_AUTHENTICATION_PLUGIN      7  /* The authentication plugin type */
#define MYSQL_MAX_PLUGIN_TYPE_NUM        8  /* The number of plugin types   */
{% endhighlight %}

如果自己要實現一種新的插件，需要在這裡添加一種新的插件類型。

使用預編譯宏和函數指針來實現插件，通過 mysql_declare_plugin, mysql_declare_plugin_end 兩個宏來定義一個插件，其中包含了一些常見的變量，而實際最終定義的是一個 struct st_mysql_plugin 結構體。

其中動態加載和靜態加載會對應不同的宏定義，下面以動態加載舉例。

{% highlight c %}
#define mysql_declare_plugin(NAME) \
__MYSQL_DECLARE_PLUGIN(NAME, \
                 builtin_ ## NAME ## _plugin_interface_version, \
                 builtin_ ## NAME ## _sizeof_struct_st_plugin, \
                 builtin_ ## NAME ## _plugin)

#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYSQL_PLUGIN_EXPORT int _mysql_plugin_interface_version_;                     \
int _mysql_plugin_interface_version_= MYSQL_PLUGIN_INTERFACE_VERSION;         \
MYSQL_PLUGIN_EXPORT int _mysql_sizeof_struct_st_plugin_;                      \
int _mysql_sizeof_struct_st_plugin_= sizeof(struct st_mysql_plugin);          \
MYSQL_PLUGIN_EXPORT struct st_mysql_plugin _mysql_plugin_declarations_[];     \
struct st_mysql_plugin _mysql_plugin_declarations_[]= {

#define mysql_declare_plugin_end ,{0,0,0,0,0,0,0,0,0,0,0,0,0}}

struct st_mysql_plugin
{
    int type;                    // 插件類型，這裡填MYSQL_DAEMON_PLUGIN 即可
    void *info;                  // 插件類型描述符，對於daemon類來說沒用，指向一個常量即可
    const char *name;            // 插件名，這個就是前面的install命令裡的plugin_name
    const char *author;          // 插件作者，長度可以任意，但是mysql只使用前64個字符
    const char *descr;           // 插件描述，可以任意長
    int license;                 // PLUGIN_LICENSE_GPL
    int (*init)(void);           // install或啟動時調用的函數，0-success，1-failure
    int (*deinit)(void);         // uninstall或關閉時調用的函數
    unsigned int version;        // 插件版本，隨意
    struct st_mysql_show_var     // 指向的show_var，可為NULL
    struct st_mysql_sys_var      // 指向的sys_var，可為NULL
    void * __reserved1;          // 保留字段
};
{% endhighlight %}

假設定義簡單的插件 foobar ，以及其擴展後的結構，實際上定義了三個變量，分別為： \_mysql_plugin_interface_version_, \_mysql_sizeof_struct_st_plugin_, \_mysql_plugin_declarations_[]。

{% highlight c %}
mysql_declare_plugin(foobar)
{ ... }
mysql_declare_plugin_end;

extern "C" int _mysql_plugin_interface_version_;
int _mysql_plugin_interface_version_= MYSQL_PLUGIN_INTERFACE_VERSION;
extern "C" int _mysql_sizeof_struct_st_plugin_;
int _mysql_sizeof_struct_st_plugin_= sizeof(struct st_mysql_plugin);
extern "C" struct st_mysql_plugin _mysql_plugin_declarations_[];
struct st_mysql_plugin _mysql_plugin_declarations_[]= {
{ ... },{0,0,0,0,0,0,0,0,0,0,0,0,0}};
{% endhighlight %}

上述展開的是動態加載，如果是靜態加載則每個變量會帶上插件的名稱，個人感覺動態加載不需要用名稱去區分。

### 加載過程

在 MySQL 的主函數中，會通過 plugin_init()@sql/sql_plugin.cc 對插件進行初始化，該函數是所有靜態鏈接的初始化入口，動態加載應該是在 install 時加載。

{% highlight text %}
mysqld_main()
 |-init_server_components()
   |-plugin_init()
     |-plugin_initialize()
       |-ha_initialize_handlerton()
         |-innobase_init()
{% endhighlight %}

插件的初始化過程為。

{% highlight c %}
int plugin_init(int *argc, char **argv, int flags) {
    ... ...
    init_alloc_root(...);                // 初始化內存分配pool
    my_hash_init(...);                   // hash結構初始化
    my_init_dynamic_array(...);          // 初始化運行時plugin數組，plugin_dl_array用來保存動態加載plugin，
                                         // plugin_array保存靜態鏈接plugin。而且最多各自能有16個plugin。

   // 初始化靜態鏈接plugin
   for (builtins= mysqld_builtins; *builtins; builtins++) {
      // 每一個plugin還可以有多個子plugin，參見見面的plugin聲明
      for (plugin= *builtins; plugin->info; plugin++) {
         register_builtin(plugin, &tmp, &plugin_ptr); // 將plugin放到plugin_array和plugin_hash中
         // 這個時候只初始化csv或者myisam plugin。
         // 初始化plugin，調用初始化函數，將插件狀態變量加入到狀態變量列表中等操作
         plugin_initialize(plugin_ptr);
      }
   }

   // 根據用戶選項初始化動態加載plugin
   if (!(flags & PLUGIN_INIT_SKIP_DYNAMIC_LOADING)) {
      if (opt_plugin_load)
         // 根據配置加載制定的plugin，包括找到dll、加載、尋找符號並設置plugin結構
         plugin_load_list(&tmp_root, argc, argv, opt_plugin_load);
      if (!(flags & PLUGIN_INIT_SKIP_PLUGIN_TABLE))
         // 加載系統plugin table中的plugin
         plugin_load(&tmp_root, argc, argv);
   }

   // 初始化剩下的plugin。
   for (i= 0; i < plugin_array.elements; i++) {
      plugin_ptr= *dynamic_element(&plugin_array, i, struct st_plugin_int **);
      if (plugin_ptr->state == PLUGIN_IS_UNINITIALIZED) {
         if (plugin_initialize(plugin_ptr)) {
            plugin_ptr->state= PLUGIN_IS_DYING;
            *(reap++)= plugin_ptr;
         }
      }
   }
   ...
}
{% endhighlight %}

這個函數執行結束以後，在 plugin_array、plugin_dl_array、plugin_hash 中保存了當前加載了的所有的 plugin，到此插件的初始化結束。

接下來，我們到 plugin_initialize() 函數中查看相應的內容，也就是插件初始化的執行流程。

#### plugin_initialize()

在該函數中，會調用每個 plugin 自己的 init() 函數，而且對於各種不同類型的 plugin 其初始化函數的參數也不一樣，這是通過一個全局的 plugin_type_initialize() 間接層來實現的。

該數組對每種類型定義了一個函數，比如對於 storage plugin 對應的是 ha_initialize_handlerton()，對於 information scheme 對應的是 initialize_schema_table()，然後在這些函數中再調用 plugin 的初始化函數。

實際上，對於其它類型的 plugin，如果沒有定義這個中間層初始化函數，那麼就會直接調用了插件的初始化函數。

{% highlight c %}
static int plugin_initialize(MEM_ROOT *tmp_root, struct st_plugin_int *plugin,
                             int *argc, char **argv, bool options_only)
{
  int ret= 1;
  DBUG_ENTER("plugin_initialize");

  mysql_mutex_assert_owner(&LOCK_plugin);
  uint state= plugin->state;
  DBUG_ASSERT(state == PLUGIN_IS_UNINITIALIZED);

  mysql_mutex_unlock(&LOCK_plugin);

  mysql_rwlock_wrlock(&LOCK_system_variables_hash);
  if (test_plugin_options(tmp_root, plugin, argc, argv))
    state= PLUGIN_IS_DISABLED;
  mysql_rwlock_unlock(&LOCK_system_variables_hash);

  if (options_only || state == PLUGIN_IS_DISABLED) {
    ret= 0;
    state= PLUGIN_IS_DISABLED;
    goto err;
  }

  if (plugin_type_initialize[plugin->plugin->type]) {
    if ((*plugin_type_initialize[plugin->plugin->type])(plugin))
    {
      sql_print_error("Plugin '%s' registration as a %s failed.",
                      plugin->name.str, plugin_type_names[plugin->plugin->type].str);
      goto err;
    }
  } else if (plugin->plugin->init) {
    if (plugin->plugin->init(plugin))
    {
      sql_print_error("Plugin '%s' init function returned error.",
                      plugin->name.str);
      goto err;
    }
  }
  state= PLUGIN_IS_READY; // plugin->init() succeeded

  if (plugin->plugin->status_vars)
  {
    /*
      historical ndb behavior caused MySQL plugins to specify
      status var names in full, with the plugin name prefix.
      this was never fixed in MySQL.
      MariaDB fixes that but supports MySQL style too.
    */
    SHOW_VAR *show_vars= plugin->plugin->status_vars;
    SHOW_VAR tmp_array[2]= {
      {plugin->plugin->name, (char*)plugin->plugin->status_vars, SHOW_ARRAY},
      {0, 0, SHOW_UNDEF}
    };
    if (strncasecmp(show_vars->name, plugin->name.str, plugin->name.length))
      show_vars= tmp_array;

    if (add_status_vars(show_vars))
      goto err;
  }

  ret= 0;

err:
  if (ret)
    mysql_del_sys_var_chain(plugin->system_vars);

  mysql_mutex_lock(&LOCK_plugin);
  plugin->state= state;

  DBUG_RETURN(ret);
}
{% endhighlight %}

代碼的處理邏輯可以很容易從上述的代碼中查看。另外，需要注意的是關於強制初始化的插件。

實際上也就是在初始化時有一個 mysql_mandatory_plugins[]，該數組實際在 cmake/plugin.cmake 或者 sql/sql_builtin.cc.in 中定義，其中必須包含的插件通常有但不僅限於 binlog、mysql_password。



<!--
每一個插件都需要實現一個 init 函數，該函數實在插件安裝的時候調用，該函數的功能是：新建一個functions_t的對象，並將對象中的func1和func2分別指向插件中的功能函數。類似下面的過程：

plugin1_init(void *p)
{
    p = malloc(sizeof(functions_t));
    p->func1= function1;
    p->func2 = function2;
}
插件安裝完成之後MySQL上層運行FUNC1和FUNC2時就可以調用到插件中的功能函數function1和function2了。

插件的安裝過程中調用的函數是plugin_init，該函數在sql_plugin中實現，這個函數最終會調用插件模塊內實現的初始化函數。

每一個插件模塊中都要實現一個通用的插件接口，以半同步插件的master為例，該接口的格式如下所示：

mysql_declare_plugin(semi_sync_master)
{
  MYSQL_REPLICATION_PLUGIN,
  &semi_sync_master_plugin,
  "rpl_semi_sync_master",
  "He Zhenxing",
  "Semi-synchronous replication master",
  PLUGIN_LICENSE_GPL,
  semi_sync_master_plugin_init, /* Plugin Init */
  semi_sync_master_plugin_deinit, /* Plugin Deinit */
  0x0100 /* 1.0 */,
  semi_sync_master_status_vars, /* status variables */
  semi_sync_master_system_vars, /* system variables */
  NULL,                         /* config options */
  0,                            /* flags */
}
mysql_declare_plugin_end;

其中，semi_sync_master_plugin_init是該插件的init函數，在插件安裝的時候調用。相反semi_sync_master_plugin_deinit在插件卸載的時候調用，可以實現資源的釋放等操作。semi_sync_master_status_vars和semi_sync_master_system_vars中分別定義了插件中所使用的狀態和變量，他們都有固定的格式。
-->






## 參考

首先是官方文檔，可以參考 [MySQL Internals Manual - Writing a Custom Storage Engine](https://dev.mysql.com/doc/internals/en/custom-engine.html)，關於存儲引擎還可以參考 [MySQL中文全文索引插件 mysqlcft 1.0.0](http://zyan.cc/post/356/)，也可以參考 [本地文檔](/reference/mysql/mysqlcft.mht) 。

有本很經典的書，比較詳細介紹了一些 MySQL 插件的實現方法，可以查看 [MySQl 5.1 Plugin Development](http://mofedogroup.synthasite.com/resources/MySQL%205.1%20Plugin%20Development.pdf) 或者 [本地文檔](/reference/mysql/MySQL 5.1 Plugin Development(Sergei & Andrew).pdf) 。

在如上文章中，介紹了一個 blackhole 引擎，關於其應用可以參考 [MySQL replication using blackhole engine](http://jroller.com/dschneller/entry/mysql_replication_using_blackhole_engine)，一個關於 blackhole 的主備複製應用，或者參考 [本地文檔](/reference/mysql/mysql_replication_using_blackhole_engine.mht)。

{% highlight text %}
{% endhighlight %}
