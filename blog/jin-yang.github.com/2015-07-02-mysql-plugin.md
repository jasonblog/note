---
title: MySQL 插件详解
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,plugin,插件
description: 在 MySQL 中，为了提高其灵活性，很多的功能都是通过插件来实现的，常见的比如 semi-sync、存储引擎、登陆认证等等。因为 MySQL 是 C/C++ 实现的，对于插件来说实际为动态链接库，保存在 plugin_dir 变量对应的目录下。在此介绍一下 MySQL 的插件实现。
---

在 MySQL 中，为了提高其灵活性，很多的功能都是通过插件来实现的，常见的比如 semi-sync、存储引擎、登陆认证等等。因为 MySQL 是 C/C++ 实现的，对于插件来说实际为动态链接库，保存在 plugin_dir 变量对应的目录下。

在此介绍一下 MySQL 的插件实现。

<!-- more -->

## 简介

插件的安装也比较简单，可以在配置文件中通过 `plugin_load="XXX"` 设置，或者通过 MySQL 命令 `install plugin` 安装，安装之后会在 `mysql.plugin` 中插入一条记录，在下次启动时，将会自动加载。

另外，可以在启动时通过 `--disable-plugin-plg-name` 或 `--plugin-plg-name=OFF` 禁止插件启动，此时表中为 `DISABLED` 。如果不能加载插件，默认会继续启动，如果设置为没有该插件则停止启动可以使用，`--plugin-my-plugin=FORCE` 。

例如，可以通过如下方式加载插件。

{% highlight text %}
$ cat ~/.my.cnf
plugin_load="rpl_semi_sync_master=semisync_master.so"

mysql > SHOW VARIABLES LIKE 'plugin_dir';                                       ← 查看插件的目录
mysql > INSTALL PLUGIN plg_name SONAME "plg_dynamic.so";                        ← 安装插件
mysql > UNINSTALL PLUGIN plg_name;                                              ← 卸载插件
mysql > SHOW PLUGINS;                                                           ← 查看当前插件，同下
mysql > SELECT * FROM information_schema.plugins WHERE PLUGIN_NAME='plg-name';  ← 详细信息
mysql > SELECT * FROM mysql.plugin;                                             ← 查看已经安装的插件
{% endhighlight %}

在 `information_schema.plugins` 中可以看到，存在了三个版本信息，其中 `plugin_version` 作者可以随意指定；`plugin_type_version` 和 `plugin_library_version` 决定了 API 的版本。

### 插件示例

插件的示例可以参考 `plugin/daemon_example` ，该插件会在 `show variable like 'datadir'` 目录下创建一个 `mysql-heartbeat.log` 日志文件，每隔 5s 打印一条日志。

可以通过 `show plugins` 查看插件，也可以查看 `mysql.plugin` 表。

## 存储引擎

MySQL 提供了访问不同的存储引擎数据表的虚拟层 API，"table handler" 指的是 storage engine 与 MySQL 优化器的接口，通过这些接口很多存储引擎可以很快集成到 MySQL 中。

在 MySQL 中采用了插件式的存储引擎，与插件类似，存储引擎以 `.so` 文件的格式进行保存，均存放在 `plugin_dir` 变量指定大目录下，可以通过如下的方式进行查看、安装、卸载。

{% highlight text %}
mysql > SHOW VARIABLES LIKE 'have_dynamic_loading';     ← 查看是否支持动态加载
mysql > SHOW ENGINES;                                   ← 或者SHOW STORAGE ENGINES;
mysql > INSTALL PLUGIN Innodb SONAME "ha_innodb.so";
mysql > UNINSTALL PLUGIN Innodb;
{% endhighlight %}

注意安装时指定的名称需要与 `st_mysql_plugin.name` 相同，大小写不敏感。

### TokuDB 使用

一个类似 InnoDB 的存储引擎，对于 percona 发行版本本身已经带了 TokuDB，可以通过如下方式查看。

{% highlight text %}
mysql> show variables like 'plugin_dir';                   ← 查看插件保存的路径
$ ls {plugin_dir} | grep toku                              ← 查看是否存在该引擎

# yum instrall jemalloc                                    ← 安装jemalloc库，依赖epel
# LD_PRELOAD=/usr/lib64/libjemalloc.so.1 mysqld ...        ← 需要通过LD_PRELOAD指定库的位置

# echo never > /sys/kernel/mm/transparent_hugepage/enabled ← 关闭hugepage，实际只需要关闭这一选项即可
# echo never > /sys/kernel/mm/transparent_hugepage/defrag

mysql> INSTALL PLUGIN tokudb SONAME 'ha_tokudb.so';        ← 安装TokuDB
mysql> SHOW ENGINES;                                       ← 查看所有的引擎
mysql> SHOW VARIABLES LIKE 'tokudb%';                      ← 查看TokuDB相关配置

mysql> UNINSTALL PLUGIN  tokudb;                           ← 卸载之
{% endhighlight %}

使用 TokuDB 引擎时，需要修改内核配置，禁用 transparent_hugepage，否则可能会导致 TokuDB 内存泄露。另外，需要注意的是，在第一次插入之后再启动时会自动加载。

如果是通过 mysqld_safe 启动，需要在配置文件中添加如下的选项。

{% highlight text %}
[mysqld_safe]
malloc_lib = /usr/lib64/libjemalloc.so
{% endhighlight %}

另外，可以参考 TokuDB 的官方文档，[Percona TokuDB - Documentation](https://www.percona.com/doc/percona-tokudb/index.html) 。

### 存储引擎示例

在源码的 `storage/example` 目录下有一个简单的存储引擎示例，基本就是什么也不做，只是告知你应该如何添加一个存储引擎。

可以在编译时添加 `-DWITH_EXAMPLE_STORAGE_ENGINE=ON` 选项，或者在该目录下执行 `make` 命令，此时会生成一个 `ha_example.so` 文件。然后可以通过如下的方式添加该引擎：

{% highlight text %}
mysql > INSTALL PLUGIN example SONAME 'ha_example.so';
mysql > CREATE TABLE test (i INT) ENGINE = EXAMPLE;     // 测试
Query OK, 0 rows affected (0.02 sec)
mysql > INSERT INTO test VALUES(1),(2),(3);
Query OK, 3 rows affected (0.00 sec)
Records: 3  Duplicates: 0  Warnings: 0
mysql > SELECT * FROM test;
Empty set (0.00 sec)
mysql > UNINSTALL PLUGIN example;
{% endhighlight %}

也可以参考一个示例 [skeleton-mysql-engine](http://bazaar.launchpad.net/~statik/+junk/skeleton-mysql-engine/files) ，不过有点老了。

### DIY 创建简单存储引擎

在此我们简单创建一个示例存储引擎，实际上类似于上述的 example 存储引擎。

创建一个 foobar 存储引擎，可以直接从简单的示例复制新目录即可，此处假设从 blackhole 复制到新的文件夹，该存储引擎可以创建 insert 之后不能 select 查到。

blackhole 存储引擎，实际不会存储任何数据，如果开启了 binlog 则会记录 binlog 。

{% highlight text %}
mysql> CREATE TABLE test(i INT, c CHAR(10)) ENGINE = BLACKHOLE;
mysql> INSERT INTO test VALUES(1,'record one'),(2,'record two');
mysql> SELECT * FROM test;
{% endhighlight %}

下面以 blackhole 作为示例创建我们的 foobar 。

{% highlight text %}
$ cd storage/ && cp blackhole foobar -rf && cd foobar
$ sed -e s/\<BLACKHOLE\>/FOOBAR/g -e s/\<blackhole\>/foobar/g ha_blackhole.h > ha_foobar.h
$ sed -e s/\<BLACKHOLE\>/FOOBAR/g -e s/\<blackhole\>/foobar/g ha_blackhole.cc > ha_foobar.cc
$ cat CMakeLists.txt
SET(FOOBAR_SOURCES  ha_foobar.cc ha_foobar.h)
MYSQL_ADD_PLUGIN(foobar ${FOOBAR_SOURCES} STORAGE_ENGINE)
$ cd ../../build && make                                 ← 第一次创建时
$ cd ../../build/storage/foobar && make                  ← 重新修改之后，只需要在该目录make即可
# make install                                           ← 安装需要root权限
{% endhighlight %}

到此为止，可以像使用 blackhole 引擎一样使用 foobar 。



## 实现

在此介绍一下 MySQL 中，插件的源码实现机制。

### 插件定义

这里也就是一些插件的通用实现，plugin API 的实现在 sql/sql_plugin.cc 这个文件中，载入 plugin 使用 dl_open 动态加载共享库的方法打开 so 文件，获得需要执行的加载函数和卸载函数的指针。

插件类型及相关宏定义在 include/sql/plugin.h，包括了些常见的类型，例如：

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

如果自己要实现一种新的插件，需要在这里添加一种新的插件类型。

使用预编译宏和函数指针来实现插件，通过 mysql_declare_plugin, mysql_declare_plugin_end 两个宏来定义一个插件，其中包含了一些常见的变量，而实际最终定义的是一个 struct st_mysql_plugin 结构体。

其中动态加载和静态加载会对应不同的宏定义，下面以动态加载举例。

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
    int type;                    // 插件类型，这里填MYSQL_DAEMON_PLUGIN 即可
    void *info;                  // 插件类型描述符，对于daemon类来说没用，指向一个常量即可
    const char *name;            // 插件名，这个就是前面的install命令里的plugin_name
    const char *author;          // 插件作者，长度可以任意，但是mysql只使用前64个字符
    const char *descr;           // 插件描述，可以任意长
    int license;                 // PLUGIN_LICENSE_GPL
    int (*init)(void);           // install或启动时调用的函数，0-success，1-failure
    int (*deinit)(void);         // uninstall或关闭时调用的函数
    unsigned int version;        // 插件版本，随意
    struct st_mysql_show_var     // 指向的show_var，可为NULL
    struct st_mysql_sys_var      // 指向的sys_var，可为NULL
    void * __reserved1;          // 保留字段
};
{% endhighlight %}

假设定义简单的插件 foobar ，以及其扩展后的结构，实际上定义了三个变量，分别为： \_mysql_plugin_interface_version_, \_mysql_sizeof_struct_st_plugin_, \_mysql_plugin_declarations_[]。

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

上述展开的是动态加载，如果是静态加载则每个变量会带上插件的名称，个人感觉动态加载不需要用名称去区分。

### 加载过程

在 MySQL 的主函数中，会通过 plugin_init()@sql/sql_plugin.cc 对插件进行初始化，该函数是所有静态链接的初始化入口，动态加载应该是在 install 时加载。

{% highlight text %}
mysqld_main()
 |-init_server_components()
   |-plugin_init()
     |-plugin_initialize()
       |-ha_initialize_handlerton()
         |-innobase_init()
{% endhighlight %}

插件的初始化过程为。

{% highlight c %}
int plugin_init(int *argc, char **argv, int flags) {
    ... ...
    init_alloc_root(...);                // 初始化内存分配pool
    my_hash_init(...);                   // hash结构初始化
    my_init_dynamic_array(...);          // 初始化运行时plugin数组，plugin_dl_array用来保存动态加载plugin，
                                         // plugin_array保存静态链接plugin。而且最多各自能有16个plugin。

   // 初始化静态链接plugin
   for (builtins= mysqld_builtins; *builtins; builtins++) {
      // 每一个plugin还可以有多个子plugin，参见见面的plugin声明
      for (plugin= *builtins; plugin->info; plugin++) {
         register_builtin(plugin, &tmp, &plugin_ptr); // 将plugin放到plugin_array和plugin_hash中
         // 这个时候只初始化csv或者myisam plugin。
         // 初始化plugin，调用初始化函数，将插件状态变量加入到状态变量列表中等操作
         plugin_initialize(plugin_ptr);
      }
   }

   // 根据用户选项初始化动态加载plugin
   if (!(flags & PLUGIN_INIT_SKIP_DYNAMIC_LOADING)) {
      if (opt_plugin_load)
         // 根据配置加载制定的plugin，包括找到dll、加载、寻找符号并设置plugin结构
         plugin_load_list(&tmp_root, argc, argv, opt_plugin_load);
      if (!(flags & PLUGIN_INIT_SKIP_PLUGIN_TABLE))
         // 加载系统plugin table中的plugin
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

这个函数执行结束以后，在 plugin_array、plugin_dl_array、plugin_hash 中保存了当前加载了的所有的 plugin，到此插件的初始化结束。

接下来，我们到 plugin_initialize() 函数中查看相应的内容，也就是插件初始化的执行流程。

#### plugin_initialize()

在该函数中，会调用每个 plugin 自己的 init() 函数，而且对于各种不同类型的 plugin 其初始化函数的参数也不一样，这是通过一个全局的 plugin_type_initialize() 间接层来实现的。

该数组对每种类型定义了一个函数，比如对于 storage plugin 对应的是 ha_initialize_handlerton()，对于 information scheme 对应的是 initialize_schema_table()，然后在这些函数中再调用 plugin 的初始化函数。

实际上，对于其它类型的 plugin，如果没有定义这个中间层初始化函数，那么就会直接调用了插件的初始化函数。

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

代码的处理逻辑可以很容易从上述的代码中查看。另外，需要注意的是关于强制初始化的插件。

实际上也就是在初始化时有一个 mysql_mandatory_plugins[]，该数组实际在 cmake/plugin.cmake 或者 sql/sql_builtin.cc.in 中定义，其中必须包含的插件通常有但不仅限于 binlog、mysql_password。



<!--
每一个插件都需要实现一个 init 函数，该函数实在插件安装的时候调用，该函数的功能是：新建一个functions_t的对象，并将对象中的func1和func2分别指向插件中的功能函数。类似下面的过程：

plugin1_init(void *p)
{
    p = malloc(sizeof(functions_t));
    p->func1= function1;
    p->func2 = function2;
}
插件安装完成之后MySQL上层运行FUNC1和FUNC2时就可以调用到插件中的功能函数function1和function2了。

插件的安装过程中调用的函数是plugin_init，该函数在sql_plugin中实现，这个函数最终会调用插件模块内实现的初始化函数。

每一个插件模块中都要实现一个通用的插件接口，以半同步插件的master为例，该接口的格式如下所示：

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

其中，semi_sync_master_plugin_init是该插件的init函数，在插件安装的时候调用。相反semi_sync_master_plugin_deinit在插件卸载的时候调用，可以实现资源的释放等操作。semi_sync_master_status_vars和semi_sync_master_system_vars中分别定义了插件中所使用的状态和变量，他们都有固定的格式。
-->






## 参考

首先是官方文档，可以参考 [MySQL Internals Manual - Writing a Custom Storage Engine](https://dev.mysql.com/doc/internals/en/custom-engine.html)，关于存储引擎还可以参考 [MySQL中文全文索引插件 mysqlcft 1.0.0](http://zyan.cc/post/356/)，也可以参考 [本地文档](/reference/mysql/mysqlcft.mht) 。

有本很经典的书，比较详细介绍了一些 MySQL 插件的实现方法，可以查看 [MySQl 5.1 Plugin Development](http://mofedogroup.synthasite.com/resources/MySQL%205.1%20Plugin%20Development.pdf) 或者 [本地文档](/reference/mysql/MySQL 5.1 Plugin Development(Sergei & Andrew).pdf) 。

在如上文章中，介绍了一个 blackhole 引擎，关于其应用可以参考 [MySQL replication using blackhole engine](http://jroller.com/dschneller/entry/mysql_replication_using_blackhole_engine)，一个关于 blackhole 的主备复制应用，或者参考 [本地文档](/reference/mysql/mysql_replication_using_blackhole_engine.mht)。

{% highlight text %}
{% endhighlight %}
