---
title: Collectd 源碼解析
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: collectd,monitor,linux
description: 介紹下 Collectd 中源碼的實現。
---

接下來介紹下 Collectd 中源碼的實現。

<!-- more -->

## 簡介

在編譯源碼前，需要先安裝部分依賴庫。

{% highlight text %}
# yum install libtool-ltdl-devel flex bison
{% endhighlight %}

<!--
libtool-ltdl-devel
ibtoolize: `COPYING.LIB' not found in `/usr/share/libtool/libltdl'
-->

接著介紹下源碼編譯時常用的命令。

{% highlight text %}
----- 如果修改了configure.ac
$ autoreconf -ivf

----- 修改Makefile.am
$ automake --add-missing --copy

----- 編譯測試##默認關閉所有插件，指定需要編譯的插件
$ mkdir build && cd build
$ ../configure --enable-debug --enable-all-plugins=no \
    --enable-logfile --enable-unixsock --enable-write-log \
    --enable-cpu --enable-load --enable-contextswitch \
    --enable-memory
$ make
$ make distclean

----- 單元測試##
$ make check

----- 壓測工具，會隨機生成host、plugin、values，儘量模擬正常的採集流量
$ collectd-tg

----- 源碼發佈打包
$ make distcheck
{% endhighlight %}

<!--
../configure --enable-debug --enable-all-plugins=no \
    --enable-logfile --enable-unixsock --enable-write-log \
    --enable-cpu --enable-load --enable-contextswitch \
    --enable-memory \
    --enable-java --with-java
-->

與測試相關的宏定義在 testing.h 文件中，執行 ```make check``` 需要定義 TESTS 變量，可以指定多個，如果返回非零則表示失敗，詳細可以查看 [Support for test suites](https://www.gnu.org/software/automake/manual/html_node/Tests.html) 。

對於 man 文檔，通過 man_MANS 變量定義，然後會根據後綴名自動安裝到相應的系統目錄。

<!--
* tries to do a VPATH build (see VPATH Builds), with the srcdir and all its content made read-only;
* runs the test suite (with make check) on this fresh build;
* installs the package in a temporary directory (with make install), and tries runs the test suite on the resulting installation (with make installcheck);
* checks that the package can be correctly uninstalled (by make uninstall) and cleaned (by make distclean);
* finally, makes another tarball to ensure the distribution is self-contained.
-->

打包時可以通過 EXTRA_DIST 變量指定比較特殊的一些文件。





## 常用概念

在介紹源碼的實現前，首先看下一些常見的概念。

插件採集的數據保存在 value_list_t 中，這個對應了 types.db 中的一行記錄，types.db 中一行可能會包含了多個採集結構，例如 load 指標，那麼與之對應 value_list_t 也可能會包含了多個採集值。

### 標示符 (Identifier)

value_list_t 中還包含了用於構建標示符 (Identifier) 的值，包括了: host、plugin、plugin instance (optional)、type、type instance (optional) 五部分，其中兩個是可選的，其含義如下：

* host 主機名稱，可以再插件中設置，否則直接獲取配置文件中的主機名；
* plugin 一般是插件名稱；
* plugin instance 可選，例如 CPU 在多核的場景；
* type 很多還是和插件相同，感覺有些衝突；
* type instance 可選，監控的不同類型，例如 CPU 的指標有 idle、usage 等。

其中每項命名的最大值為 64，建議不要使用 ```'/'``` 和 ```'-'```，格式化時通過 ```format_name()``` 實現，串行化的格式為：```host "/" plugin ["-" plugin instance] "/" type ["-" type instance]``` 。

如下，是幾個常見的示例：

{% highlight text %}
localhost/cpu-0/cpu-idle
localhost/memory/memory-used
wanda/disk-hdc1/disk_octets
leeloo/load/load
{% endhighlight %}

常見的在 global cache 中會使用，用於保存最近保存的一次數據，與 notification 相關的參數。

個人建議使用 Plugin + Plugin Instance + types.db 這種方式定義 Identifier 。


### 採集值類型

Collectd 中總共有四種類型，簡單介紹如下。

* GAUSE: 直接使用採樣值，通常為溫度、內存使用率等參數值；
* DERIVE: 計算採樣的速率，計算公式為 ```rate=(value_new-value_old)/(time_new-time_old)``` ，需要注意，如果值非遞增，那麼可能產生負值；
* COUNTER: 與 DERIVE 相同，只是當 ```value_new<value_old``` 時則認為是 "wrapped around" <!--，此時會按照 ```rate=(2**width-value_old+value_new)/(time_new-time_old)width=value_old< 2**32 ? 32 : 64```--> 。
* ABSOULUTE: 與 GAUSE 比較相似，只是該值是無符號整數，且會除以當前時間與上次採樣時間的差值；

需要注意的是，COUNTER 值採集的三種特殊情況：1) 計數器達到最大值，導致迴環；2) 監控服務維護(重啟或者手動計數器清零)；3) 監控 agent 重啟，導致上次計數丟失。這三種情況可能會導致採集指標異常，通常只有一個採集點，一般可以過濾或者忽略掉。

<!-- 詳見: https://collectd.org/wiki/index.php/Data_source -->

#### 源碼實現

相關的代碼內容如下。

{% highlight c %}
#define DS_TYPE_COUNTER 0
#define DS_TYPE_GAUGE 1
#define DS_TYPE_DERIVE 2
#define DS_TYPE_ABSOLUTE 3

typedef unsigned long long counter_t;
typedef double gauge_t;
typedef int64_t derive_t;
typedef uint64_t absolute_t;

union value_u {
  counter_t counter;
  gauge_t gauge;
  derive_t derive;
  absolute_t absolute;
};
typedef union value_u value_t;

int value_to_rate(gauge_t *ret_rate,
                  value_t value, int ds_type, cdtime_t t,
                  value_to_rate_state_t *state) {
  gauge_t interval;

  /* Another invalid state: The time is not increasing. */
  if (t <= state->last_time) {
    memset(state, 0, sizeof(*state));
    return (EINVAL);
  }

  interval = CDTIME_T_TO_DOUBLE(t - state->last_time);

  /* Previous value is invalid. */
  if (state->last_time == 0) {
    state->last_value = value;
    state->last_time = t;
    return (EAGAIN);
  }

  switch (ds_type) {
  case DS_TYPE_DERIVE: {
    derive_t diff = value.derive - state->last_value.derive;
    *ret_rate = ((gauge_t)diff) / ((gauge_t)interval);
    break;
  }
  case DS_TYPE_GAUGE: {
    *ret_rate = value.gauge;
    break;
  }
  case DS_TYPE_COUNTER: {
    counter_t diff = counter_diff(state->last_value.counter, value.counter);
    *ret_rate = ((gauge_t)diff) / ((gauge_t)interval);
    break;
  }
  case DS_TYPE_ABSOLUTE: {
    absolute_t diff = value.absolute;
    *ret_rate = ((gauge_t)diff) / ((gauge_t)interval);
    break;
  }
  default:
    return EINVAL;
  }

  state->last_value = value;
  state->last_time = t;
  return (0);
}
{% endhighlight %}





## 插件實現

首先，簡單說下插件是如何加載的。

有個配置項 ```AutoLoadPlugin false``` 用於控制是否會自動加載插件，默認需要先通過 LoadPlugin 指令加載插件，然後再在 ```<Plugin XXX>``` 中進行配置，否則會報錯；當然也可以將該參數修改為 true ，然後在遇到 ```<Plugin XXX>``` 時自動加載。

源碼中有一個示例插件，可查看 ```contrib/examples``` 目錄下的文件；當然，也可以參考 [官方文檔](https://collectd.org/wiki/index.php/Plugin_architecture) 。

插件的實現實際上主要是通過將回調函數註冊到鏈表上。

{% highlight text %}
int plugin_register_init()<<<1>>>
註冊初始化函數，正式創建讀寫線程之前執行。
使用鏈表: list_init

int plugin_register_config()<<<2>>>
簡單的配置處理，只含有配置項以及回調函數。
使用鏈表: first_callback

int plugin_register_complex_config()<<<2>>>
通過一個註冊的回調函數進行配置解析，在鏈表末尾寫入；該函數會對配置項進行檢測，包括了一些複雜的邏輯處理。
使用鏈表: complex_callback_head

int plugin_register_flush()<<<3>>>
有部分插件會緩存數據，例如network、rrdtool等，通過函數註冊到鏈表，在執行 flush 命令時會調用鏈表上的各個函數。
使用鏈表: list_flush，通過plugin_flush()函數調用

int plugin_register_read()<<<3>>>
int plugin_register_complex_read()
兩個函數的區別在於回調函數中是否需要傳入參數，詳見plugin_read_thread()中的調用。
使用鏈表：read_list

int plugin_register_write()<<<3>>>
寫入插件的回調函數註冊。
使用鏈表：list_write

int plugin_register_missing()<<<3>>>
會在每次的時間間隔檢查是否有數據丟失，當發現有數據丟失時會調用註冊的函數。
使用鏈表：list_missing

int plugin_register_shutdown()<<<4>>>
通過plugin_shutdown_all()函數調用。
使用鏈表：list_shutdown
{% endhighlight %}

<!--
int plugin_register_data_set()
int plugin_register_log()
int plugin_register_notification()
https://collectd.org/wiki/index.php/Plugin_architecture
-->

### 新增插件

首先，新增 autoconf 的配置。

需要通過自定義的宏 ```AC_PLUGIN()``` 添加插件編譯，其中第一個參數為 ```--enable-XXX``` 使用，第二個表示是否需要編譯 (例如是否依賴的庫存在，如果出了C99/POSIX不依賴其它則設置為 "yes" 即可)，第三個參數為註釋，用於 ```./configure --help``` 。

如下是簡單的示例。

{% highlight text %}
AC_PLUGIN([name],                $with_dependency,          [description])
AC_PLUGIN([xencpu],              [$plugin_xencpu],          [Xen Host CPU usage])
AC_PLUGIN([xmms],                [$with_libxmms],           [XMMS statistics])
AC_PLUGIN([zookeeper],           [yes],                     [Zookeeper statistics])
{% endhighlight %}

添加上述的宏之後，會增加如下功能：
 * 使用 configure 命令配置時，可以通過 ```--enable-XXX``` ```--disable-XXX``` 或者 ```--enable-XXX=force``` 參數配置是否需要編譯
 * 在 Makefile.am 中可以通過 ```if BUILD_PLUGIN_XXX``` 判斷是否需要編譯生成動態庫；
 * 最終生成的配置頭文件 src/config.h 中含有 ```HAVE_PLUGIN_XXX``` 定義宏。

接著增加 automake 配置。

在如上的 ```AC_PLUGIN()``` 宏中，會通過 ```AM_CONDITIONAL()``` 定義一個 ```BUILD_PLUGIN_XXX```，不同的目錄下都有相應的 Makefile.am 文件；例如，要增加一個插件，可以使用類似如下配置。

{% highlight text %}
if BUILD_PLUGIN_FOOBAR
    pkglib_LTLIBRARIES += foobar.la
    foobar_la_SOURCES = foobar.c
    foobar_la_LDFLAGS = -module -avoid-version
    collectd_LDADD += "-dlopen" foobar.la
    collectd_DEPENDENCIES += foobar.la
endif
{% endhighlight %}

然後就可以通過 autoreconf + automake 重新生成。


## 源碼詳解

介紹下調用流程。

### 內存管理

這裡以 load 插件為例。

1. 通過 ```plugin_register_read()``` 函數註冊到鏈表中，調用回調函數採集指標；
2. 回調函數，load 通過 ```VALUE_LIST_INIT``` 宏初始化，也就是初始化一個本地的臨時變量，然後通過 ```plugin_dispatch_values()``` 函數發送給 collectd 核心；
3. 在 ```plugin_write_enqueue()``` 函數中主要的內存分配函數，包括了 ```malloc(sizeof(write_queue_t)) + malloc(sizeof(value_list_t)) + calloc(len,sizeof(values)) + calloc(1,sizeof(meta_data_t))```，如果中間有內存分配失敗則釋放；
4. 而寫線程插件會阻塞在 ```plugin_write_dequeue()``` 中，該函數會釋放 ```sfree(write_queue_t)``` ，然後返回 ```value_list_t``` 指針；
5. 線程的主循環 ```plugin_write_thread()``` 函數中，在執行完分發後會調用 ```plugin_value_list_free()``` 釋放之前申請的主要內存。

calloc() 會將申請的緩存初始化為 0 ，而 malloc() 不會初始化內存。

### 調用流程

首先看下整個軟件調用流程。

{% highlight text %}
main()
 | <<<<<<<<<========= 讀取配置文件
 |-plugin_init_ctx()                            <ctx>
 |-cf_read()                                  ← 讀取配置文件
 | |-cf_read_generic()                        ← 判斷文件是否存在
 | | |-wordexp()                              ← 文件擴展，確保文件存在
 | | |-stat()                                 ← 判斷是文件夾還是文件
 | | |-cf_read_file()                         ← 讀取配置文件內容
 | | | |-oconfig_parse_file()                 ← 解析配置項
 | | | | |-fopen()
 | | | | |-oconfig_parse_fh()
 | | | | | |-yyset_in()                       ← 入參是FILE*類型
 | | | | | |-yyparse()                        ← 使用LEX+YACC進行詞法+語法解析
 | | | | | |-yyset_in()                       ← 處理完成，修改為NULL值
 | | | | |-fclose()
 | | | |-cf_include_all()                     ← 在解析完上述配置文件後查看是否有Include選項
 | | |-cf_read_dir()                          ← 如果是目錄，會遞歸調用cf_read_generic()
 | | |-cf_ci_append_children()                ← 添加到root中
 | |
 | |-dispatch_value()                         ← 沒有子配置項，一般為全局配置
 | | |-dispatch_global_option()               ← 全局配置，也就是cf_global_options變量中的定義
 | | | |-global_option_set()                  ← 設置全局變量
 | | |-dispatch_value_typesdb()               ← 對於cf_value_map中的定義，例如Typesdb，解析types.db
 | |   |-read_types_list()                    ← 讀取一個配置文件，可以通過TypesDB指定多個配置文件
 | |     |-fopen()
 | |     |-parse_file()
 | |     | |-fgets()
 | |     | |-parse_line()
 | |     |   |-plugin_register_data_set()     ← 註冊data_set_t類型，通過avl保存在data_sets全局變量中
 | |     |-fclose()
 | |
 | |-dispatch_block()                         ← 有子配置項時，也就是配置塊
 | | |-dispatch_loadplugin()                  ← LoadPlugin，會調用plugin_load()
 | | | |-plugin_set_ctx()                    <ctx>獲取插件上下文
 | | | |-plugin_load()
 | | |   |-strcasecmp()                       ← 對於python、perl插件，需要做部分初始化操作
 | | |   |-plugin_load_file()
 | | |     |-lt_dlsym()                       ← 調用各個插件的module_register()函數
 | | |       |-plugin_register_complex_read() ← 會生成read_func_t對象
 | | |         |-plugin_insert_read()         ← 寫入到list以及heap
 | | | |-plugin_set_ctx()                    獲取插件上下文
 | | |
 | | |-dispatch_block_plugin()                ← Plugin，會調用plugin_load()
 | | | |-plugin_load()                        ← 需要配置AutoLoadPlugin(true)參數
 | | | | ### 調用complex_callback_head鏈表
 | | | |-dispatch_value_plugin()
 | | |   |-cf_dispatch()
 | | |     |-cf_search()                      ← 查找first_callback鏈表中的回調函數
 | | |
 | | |-fc_configure()                         ← Chain，模塊
 | |   |-fc_init_once()
 | |   |-fc_config_add_chain()
 | |-oconfig_free()                           ← 釋放配置項
 | | |-oconfig_free_all()                     ← 釋放配置，會遞歸調用
 | |-read_types_list()                        ← 如果配置文件中沒有指定types.db，則加載默認
 |
 | <<<<<<<<<========= 系統初始化
 |-change_basedir()                           ← 如果設置了basedir，則嘗試切換
 | |-chdir()                                  ← 切換到指定目錄下
 |-init_global_variables()                    ← 初始化全局變量，如interval_g、timeout_g、hostname
 | |-cf_get_default_interval()                ← 設置全局的時間間隔
 | |-init_hostname()                          ← 獲取主機名
 | | |-global_option_get()                    ← 嘗試從配置文件中通過Hostname選項獲取主機名
 | | |-gethostname()                          ← 沒有配置則嘗試通過該系統調用獲取
 | | |-global_option_get()                    ← 通過FQDNLookup確認是否需要查找主機名
 | | |-getaddrinfo()                          ← 如果上述配置為true
 | |-return()                                 ← 如果使用了-t參數
 | |-fork()                                   ← daemonize
 |-sigaction()                                ← 註冊信號處理函數，包括了SIGINT、SIGTERM、SIGUSR1
 |
 | <<<<<<<<<========= 創建工作線程
 |-do_init()
 | |-plugin_init_all()                        ← 會設置緩存隊列的大小
 |   |-uc_init()                              ← 初始化全局變量cache_tree(平衡二叉樹)
 |   | ###BEGIN讀取所需要的配置參數
 |   |-plugin_register_read()                 ← 如果已經配置CollectInternalStats，則註冊一個讀取插件
 |   |-fc_chain_get_by_name()                 ← 讀取PreCacheChain、PostCacheChain配置參數
 |   |
 |   |-cf_callback()                          ← <<<1>>>調用list_init鏈表中回調函數，檢測插件是否正常
 |   |
 |   |-start_write_threads()                  ← 開啟寫線程
 |   | |-plugin_write_thread()                ← 啟動寫線程，真正的線程執行函數
 |   | | | ###BEGIN###while循環調用，判斷write_loop是否可用
 |   | | |-plugin_write_dequeue()             ← 從write_queue鏈表的首部獲取一個對象，隊列遞減
 |   | | | |-pthread_cond_wait()              ← 等待write_cond條件變量，使用write_lock鎖
 |   | | |-plugin_dispatch_values_internal()  ← 入參是value_list_t類型
 |   | | | |-c_avl_get()                      ← 從data_sets中獲取對應的數據類型，也就是types.db中
 |   | | | |-escape_slashes()                 ← 處理host、plugin、type中可能的轉移字符
 |   | | | |-fc_process_chain()               ← 如果pre_cache_chain不為空，則調用該函數
 |   | | | |-uc_update()                      ← 更新緩存cache_tree，入參為data_set_t和value_list_t
 |   | | | | |-FORMAT_VL()                    ← 實際上是調用format_name()將vl中的值生成標示符
 |   | | | | |-c_avl_get()                    ← 利用上述標示符獲取cache_entry_t，在此會緩存最近的一次採集數據
 |   | | | | |... ...                         ← 會根據不同的類型進行處理，例如DS_TYPE_GAUGE
 |   | | | | |-uc_check_range()               ← 檢查是否在指定的範圍內
 |   | | | |-fc_process_chain()  ##OR1##      ← 如果有post_cache_chain則調用
 |   | | | |-fc_default_action() ##OR1##      ← 調用寫入插件寫入
 |   | | |   |-fc_bit_write_invoke()
 |   | | |     |-plugin_write()               ← 通過list_write鏈表調用各個組件寫入
 |   | | |       |                            ← 當所有插件都寫入失敗時返回-1，否則返回0
 |   | | |       |-cf_callback()
 |   | | | ###END###while
 |   | | |-plugin_value_list_free()           ← TODODO:是否使用緩存池
 |   | |-set_thread_name()                    ← 設置線程名稱writerN
 |   |
 |   |-start_read_threads()                   ← 創建多個讀線程，需要保證read_heap!=NULL
 |     |-plugin_read_thread()                 ← 線程實際入口，在一個死循環中執行，詳細見後面的解析
 |     | | ###BEGIN###while循環調用，判斷read_loop是否可用
 |     | |-pthread_cond_wait()                ← 等待read_cond條件變量，使用read_lock鎖
 |     | |-c_heap_get_root()
 |     | | ###END###
 |     |-set_thread_name()                    ← 設置線程名稱writerN
 |
 |-test_readall()                             ← 如果使用參數-T，則調用插件讀取一行
 | |-plugin_read_all_once()                   ← 調用各個插件的讀取函數，測試是否有誤
 |   |-c_heap_get_root()
 |   |-rf_callback()                          ← 調用各個插件的讀取函數
 |
 |-do_loop()                                  ← 在此包括了時間間隔
 | |-cf_get_default_interval()                ← 獲取默認的時間間隔
 | |-cdtime()
 | | ###BEGIN###while循環調用，判斷loop是否可用
 | |-plugin_read_all()
 |   |-uc_check_timeout()                     ← 主線程檢查是否有插件超時
 |     |-plugin_dispatch_missing()            ← 如果有超時，則調用註冊的回調函數
 |
 |-do_shutdown()                              ← 通過INT、TERM信號用於關閉
   |-plugin_shutdown_all()                    ← 調用鏈表上回調函數
     |-destroy_all_callbacks()

----- 註冊簡單讀取插件
plugin_register_read()                        ← 新建read_func_t對象
 |-plugin_insert_read()
   |-llist_create()                           ← 如果第一次調用，則生成read_list
   |-c_heap_create()                          ← 如果第一次調用，則生成read_heap
   |-llist_search()                           ← 是否重複註冊，直接返回
   |-c_heap_insert()                          ← 寫入read_heap，通過rf_next_read排序，也即下次要讀取時間

----- 各個插件在採集完數據之後用於保存數據
plugin_dispatch_values()
 |-check_drop_value()                         ← 檢查是否會丟棄數據
 | |-get_drop_probability()                   ← 用於計算是否丟棄
 |-plugin_write_enqueue()                     ← 直接寫入到write_queue鏈表的末尾
   |-plugin_value_list_clone()                ← 複製一份，會自動填充主機名、採集時間、採樣頻率
   | |-malloc()
   | |-meta_data_clone()
   |-plugin_get_ctx()                        保存讀插件的上下文信息
   |-pthread_mutex_lock()                    對write_lock加鎖
   |-pthread_cond_signal()                   向write_cond發送信號
   |-pthread_cond_signal()                    ← 添加到write_queue鏈表中，併發送write_cond

----- 註冊簡單寫回調函數，保存到list_write鏈表中
plugin_register_write()
 |-create_register_callback()
   |-calloc()                                 ← 分配插件需要空間
   |-register_callback()                      ← 添加到list_write鏈表中

----- 註冊到鏈表中，部分需要插件需要執行刷新，如rrdtool、network會註冊該函數
plugin_register_flush()
 |-create_register_callback()                 ← 添加到list_flush鏈表中
 |-plugin_register_complex_read()             ← 如果刷新時間間隔不為0，則調用該插件註冊
                                              ← 回調函數plugin_flush_timeout_callback()
{% endhighlight %}

<!--
cache_tree中的值什麼時候開始刷新？保存了什麼樣的數據？
線程切換時上下文的調用？
-->

### 讀線程

在通過 ```plugin_register_read()``` 函數進行註冊時，會生成 read_list 和 read_heap 兩個全局變量，其中 read_list 只是用來維護註冊的讀插件，實際用途不大；而 read_heap 類似於堆排序，用於獲取最近需要進行讀取的插件，也就是說，下個時間點調用那個函數，通過 heap 進行排序。

如果調用插件讀取失敗，則會採用 double 的退避措施，也就是將下次採集時間乘以 2，直到 max_read_interval 值；讀取成功時則會直接恢復原有的採集時間。

{% highlight c %}
static void *plugin_read_thread(void __attribute__((unused)) * args) {
  while (read_loop != 0) {
    // 從read_heap中讀取排序在前的對象，一般只是由於多線程之間競爭導致阻塞
    // 不過，還有可能會存在線程數大於插件數的情況，此時就會進入條件等待
    pthread_mutex_lock(&read_lock);
    rf = c_heap_get_root(read_heap);
    if (rf == NULL) {
      pthread_cond_wait(&read_cond, &read_lock);
      pthread_mutex_unlock(&read_lock);
      continue;
    }
    pthread_mutex_unlock(&read_lock);

    // 也就是監控的時間間隔，正常會在插件加載的時候已經配置
    // 默認是採用全局的配置，不過也可以在插件配置中設置自己的參數
    if (rf->rf_interval == 0) {
      rf->rf_interval = plugin_get_interval();
      rf->rf_effective_interval = rf->rf_interval;
      rf->rf_next_read = cdtime();
    }

    // 正常來說我們可以在此執行nsleep()做等待，但是在關閉讀線程時，仍使用read_cond
    pthread_mutex_lock(&read_lock);
    // 對於Linux來說，不會有驚群現象，但是對於NetBSD而且CPU>1時會存在問題
    rc = 0;
    while ((read_loop != 0) && (cdtime() < rf->rf_next_read) && rc == 0) {
      rc = pthread_cond_timedwait(&read_cond, &read_lock,
                                  &CDTIME_T_TO_TIMESPEC(rf->rf_next_read));
    }
    rf_type = rf->rf_type;          // 需要持有read_lock讀取
    pthread_mutex_unlock(&read_lock);

    // 如上所述，此時可能是需要退出的，在此檢查下
    if (read_loop == 0) {
      c_heap_insert(read_heap, rf); // 為了可以正常free，需要將rf重新插入到read_heap中
      break;
    }

    // 在plugin_unregister_read()函數中，已經將該插件刪除，此時只需要刪除即可
    if (rf_type == RF_REMOVE) {
      DEBUG("plugin_read_thread: Destroying the `%s' callback.", rf->rf_name);
      sfree(rf->rf_name);
      destroy_callback((callback_func_t *)rf);
      rf = NULL;
      continue;
    }

    // OK，開始正式調用插件讀取數據了
    DEBUG("plugin_read_thread: Handling `%s'.", rf->rf_name);
    start = cdtime();
    old_ctx = plugin_set_ctx(rf->rf_ctx);
    if (rf_type == RF_SIMPLE) {
      int (*callback)(void);
      callback = rf->rf_callback;
      status = (*callback)();
    } else {
      plugin_read_cb callback;
      assert(rf_type == RF_COMPLEX);
      callback = rf->rf_callback;
      status = (*callback)(&rf->rf_udata);
    }
    plugin_set_ctx(old_ctx);


    // 如果失敗則會將下次採集的時間間隔double，當然是有上限的；成功時則會恢復原有的採集時間間隔
    if (status != 0) {
      rf->rf_effective_interval *= 2;
      if (rf->rf_effective_interval > max_read_interval)
        rf->rf_effective_interval = max_read_interval;
    } else {
      rf->rf_effective_interval = rf->rf_interval;
    }
    now = cdtime();
    elapsed = (now - start);  // 計算本次花費的時間
    if (elapsed > rf->rf_effective_interval)
      WARNING(... ...);

    // 計算下次需要調用插件的時間
    rf->rf_next_read += rf->rf_effective_interval;

    // 如果採集時間超過了時間間隔，則立即重新採集
    if (rf->rf_next_read < now) {
      rf->rf_next_read = now;
    }

    // 重新寫入到read_heap中，到此插件調用結束
    c_heap_insert(read_heap, rf);
  } /* while (read_loop) */

  pthread_exit(NULL);
  return ((void *)0);
} /* void *plugin_read_thread */
{% endhighlight %}

從上述的函數調用可知，collectd 框架不會負責數據採集寫入，需要由各個插件負責。

### 插件採集數據

實際上，在 contrib/examples 目錄下有個插件的示例程序；在此選一個比較簡單的插件 load，一般最終會通過 ```plugin_dispatch_values()``` 函數提交。該函數主要是將數據添加到 write_queue_head 列表中，併發送 write_cond。

<!--
### 線程上下文

TODO:保存的什麼信息？？？？

plugin_init_ctx()
 |-pthread_key_create()
 |-plugin_ctx_key_initialized=1 標識已經完成初始化

struct plugin_ctx_s {
  cdtime_t interval;
  cdtime_t flush_interval;
  cdtime_t flush_timeout;
};
typedef struct plugin_ctx_s plugin_ctx_t;

struct write_queue_s {
  value_list_t *vl;      採集的指標，對應types.db中的一行
  plugin_ctx_t ctx;      線程上下文，幹嘛用的？？？
  write_queue_t *next;
};
typedef struct write_queue_s write_queue_t;

value_list_t


寫入插件的回調函數接口：
typedef int (*plugin_write_cb)(const data_set_t *, const value_list_t *, user_data_t *);


typedef unsigned long long counter_t;
typedef double gauge_t;
typedef int64_t derive_t;
typedef uint64_t absolute_t;

union value_u {
  counter_t counter;
  gauge_t gauge;
  derive_t derive;
  absolute_t absolute;
};
typedef union value_u value_t;

struct value_list_s {
  value_t *values; 採集指標，該值類型與數量與types.db中的定義相同，插件填充
  size_t values_len; 採集指標數，同樣與types.db中的值相同，插件填充
  cdtime_t time; 可以在plugin_value_list_clone()中自動填充
  cdtime_t interval; 同上
  char host[DATA_MAX_NAME_LEN]; 同上
  char plugin[DATA_MAX_NAME_LEN]; 從哪個插件獲取，需要插件填充
  char plugin_instance[DATA_MAX_NAME_LEN];  同上，可選
  char type[DATA_MAX_NAME_LEN]; 根據該參數從data_sets中查找，需要插件填充
  char type_instance[DATA_MAX_NAME_LEN]; 同上，可選
  meta_data_t *meta; 元數據，用於插件特定目的使用，目前多數插件沒有使用
};
typedef struct value_list_s value_list_t;




在 Collectd 中，其時間通過 cdtime_t 結構體表示，採用 64-bits 無符號整型，所有相關的處理都通過對應的函數處理，例如 讀取時間 ```cf_util_get_cdtime()```、獲取當前時間 ```cdtime()```、時間轉換為浮點 ```CDTIME_T_TO_DOUBLE()``` 等等。

關於時間相關的其它輔助函數，可以查看 utils_time.c 文件中的內容。


target_notification.c


plugin_register_notification() 插件註冊信息
 |-create_register_callback() 註冊到list_notification鏈表中

plugin_dispatch_notification() 發送notify信息，很多插件都可以通過該函數發送消息


The Notification target can be used to create a notification with arbitrary text.
Synopsis

<Target "notification">
  Message "Oops, the %{type_instance} temperature is currently %{ds:value}!"
  Severity "WARNING"
</Target>



Placeholders

The following placeholders will be replaces by their respective values in the Message option:

    %{host}
    %{plugin}
    %{plugin_instance}
    %{type}
    %{type_instance}
    %{ds:name}
    Will be replaced by the current rate of of the data source "name". Because counter values need to be transformed to rates, using this target in the PreCache chain may yield unexpected results.









http://en.proft.me/2014/05/6/monitoring-cpu-ram-network-collectd/


https://collectd.org/wiki/index.php/Chains

https://mailman.verplant.org/pipermail/collectd/2017-April/thread.html


collectd 數據類型詳解

http://blog.kankanan.com/article/collectd-6570636e7c7b578b8be689e3.html

https://collectd.org/wiki/index.php/Build_system

https://collectd.org/wiki/index.php/High_resolution_time_format

https://collectd.org/wiki/index.php/Release_process



plugin_dispatch_multivalue() memory


complex 主要是會在檢查配置文件讀取配置項時，判斷是否需要註冊各種回調函數。


## threshold 實現


plugin_register_complex_config()
 |-cf_register_complex() 會添加到complex_callback_head鏈表的尾部

在配置初始化函數 ut_config() 函數中，會註冊兩個主要的回調函數 ut_missing() 以及 ut_check_threshold() 兩個函數；其插件規則保存在threshold_tree，一個自平衡二叉樹(AVL)結構體。



ut_check_threshold()
 |-threshold_search()
 |-uc_get_rate()
 |-ut_check_one_threshold()
 |-ut_report_state()



thresholds解析


## 插件實現

SRC-TODO:
  1. plugin_read_thread()添加線程ID，方便調試。
  2. 從一個固定整點時間點開始，而非從啟動時間開始計時。


https://collectd.org/wiki/index.php/Plugin_architecture
InnoDB: Error: page 570 log sequence number 7289495
InnoDB: is in the future! Current system log sequence number 5574939.
InnoDB: Your database may be corrupt or you may have copied the InnoDB
InnoDB: tablespace but not the InnoDB log files. See
InnoDB: http://dev.mysql.com/doc/refman/5.6/en/forcing-innodb-recovery.html
InnoDB: for more information.

日誌顯示是數據文件的 LSN 比 Redo Log 的 LSN 要大，當系統嘗試使用 Redo Log 去修復數據頁面的時候，發現 Redo Log LSN 比數據頁面還小，所以導致錯誤。數據頁的 LSN 在一般情況下，都是小於 Redo Log 的，因為在事物提交時先順序寫入 Redo Log ，然後後臺線程將髒頁刷新到數據文件中，所以，數據頁的 LSN 正常情況下永遠會比 Redo Log 的 LSN 小。

可能場景：
1. 如果有批量更新未提交，然後 kill 進程之後執行回滾操作，但是未等回滾執行完畢就 kill -9 mysql 導致回滾崩潰。

buf_page_io_complete() BP的異步讀寫完成
 |-buf_page_is_corrupted() 直接報錯函數
   |-memcmp() 非壓縮頁，則檢查頁頭部與尾部的LSN是否相同
   |-log_peek_lsn() 獲取當前的log_sys->lsn，TODO如何獲取？？？
   | <<<IMPORTANT>>> 如果當前LSN小於頁面保存的FIL_PAGE_LSN那麼則報錯"in the future"
   |-buf_page_is_checksum_valid_crc32() 根據不同的頁校驗算法進行校驗

頁是否損壞判斷：
1. 非壓縮頁校驗 FIL_PAGE_LSN 和 FIL_PAGE_END_LSN_OLD_CHKSUM 所在的低 4 Bytes 是否相同，LSN是8Bytes為什麼只校驗低4Bytes？？？
2. 如果是

InnoDB 恢復流程
https://forums.cpanel.net/threads/innodb-corruption-repair-guide.418722/
https://boknowsit.wordpress.com/2012/12/22/mysql-log-is-in-the-future/


FIXME:
mysql-innodb-crash-recovery.html
recv_recovery_from_checkpoint_start() 函數末尾設置recv_lsn_checks_on為TRUE
 |-recv_find_max_checkpoint() 從redolog中...
 |-log_group_read_checkpoint_info()
collectd.html

增加 History 配置項，用於保存歷史數據。


typedef unsigned long long counter_t;
typedef double gauge_t;
typedef int64_t derive_t;
typedef uint64_t absolute_t;

global cache

如果沒有則通過 uc_insert() 寫入，



Programs must be written for people to read, and only incidentally for machines to execute.

I'm not that familiar with automake, but I am pretty sure there is some way to solve this dilemma. Can anybody here give me a hint


?int uc_get_history(const data_set_t *ds, const value_list_t *vl, gauge_t *ret_history, size_t num_steps, size_t num_ds);

ds+vl 為調用 write 回調函數時的入參，



什麼情況下會阻塞？？

fc_register_target()  添加到target_list_head鏈表的末尾

fc_configure()
 |-fc_init_once() 初始化內置target，包括了jump、stop、return、write
 |-fc_config_add_chain() 如果使用Chain參數
   |-fc_config_add_target()

fc_process_chain() 流程的處理

https://collectd.org/wiki/index.php/Target:Notification

fc_register_match() 添加到match_list_head鏈表的末尾
-->


## 插件實現

### exec

這是一個通用的插件，不過每次執行時都需要 fork 一個進程，如果需要多次採集那麼其性能會變的很差，所以對於一些特定的插件，如 Python 建議不要使用該插件。

如下是一個 exec 插件的配置示例。

{% highlight text %}
Loadplugin exec
<Plugin exec>
  Exec "user:group" "program"
  Exec "some-user" "/path/to/another/binary" "arg0" "arg1"
  NotificationExec "user" "/path/to/handle_notification"
</Plugin>
{% endhighlight %}

以 bash 插件為例，直接通過 [Plain text protocol](https://collectd.org/wiki/index.php/Plain_text_protocol) 方式向 Collectd 發送數據。

{% highlight bash %}
#!/bin/bash

HOSTNAME="${COLLECTD_HOSTNAME:-`hostname -f`}"
INTERVAL="${COLLECTD_INTERVAL:-10}"
PORT=6379

while sleep "$INTERVAL"; do
    info=$(echo info|nc -w 1 127.0.0.1 $PORT)
    connected_clients=$(echo "$info"|awk -F : '$1 == "connected_clients" {print $2}')
    connected_slaves=$(echo "$info"|awk -F : '$1 == "connected_slaves" {print $2}')
    uptime=$(echo "$info"|awk -F : '$1 == "uptime_in_seconds" {print $2}')
    used_memory=$(echo "$info"|awk -F ":" '$1 == "used_memory" {print $2}'|sed -e 's/\r//')
    changes_since_last_save=$(echo "$info"|awk -F : '$1 == "changes_since_last_save" {print $2}')
    total_commands_processed=$(echo "$info"|awk -F : '$1 == "total_commands_processed" {print $2}')
    keys=$(echo "$info"|egrep -e "^db0"|sed -e 's/^.\+:keys=//'|sed -e 's/,.\+//')

    echo "PUTVAL $HOSTNAME/redis-$PORT/memcached_connections-clients interval=$INTERVAL N:$connected_clients"
    echo "PUTVAL $HOSTNAME/redis-$PORT/memcached_connections-slaves interval=$INTERVAL N:$connected_slaves"
    echo "PUTVAL $HOSTNAME/redis-$PORT/uptime interval=$INTERVAL N:$uptime"
    echo "PUTVAL $HOSTNAME/redis-$PORT/df-memory interval=$INTERVAL N:$used_memory:U"
    echo "PUTVAL $HOSTNAME/redis-$PORT/files-unsaved_changes interval=$INTERVAL N:$changes_since_last_save"
    echo "PUTVAL $HOSTNAME/redis-$PORT/memcached_command-total interval=$INTERVAL N:$total_commands_processed"
    echo "PUTVAL $HOSTNAME/redis-$PORT/memcached_items-db0 interval=$INTERVAL N:$keys"
done
{% endhighlight %}

在配置文件中添加如下內容即可。

{% highlight text %}
<Plugin exec>
    Exec nobody "/etc/collectd/redis.sh"
</Plugin>
{% endhighlight %}

<!-- https://collectd.org/wiki/index.php/Plugin:Exec -->

### python

Python 中有很多不錯的庫，例如可以通過 [pypi/collectd](https://pypi.python.org/pypi/collectd) 庫，可以向 collectd 的服務端發送數據。而 collectd 的 Python 插件實現，實際上是在插件中以 C 語言的形式實現了一個 collectd 插件。

#### 示例

Collectd 的插件回調函數同樣可以在 Python 中調用，也即 ```config```、```init```、```read```、```write```、```log```、```flush```、```shutdown``` 等接口，當然需要在其前添加 ```register_*``` 。

{% highlight python %}
import collectd

PATH = '/proc/uptime'

def config_func(config):
    path_set = False
    for node in config.children:
        key = node.key.lower()
        val = node.values[0]

        if key == 'path':
            global PATH
            PATH = val
            path_set = True
        else:
            collectd.info('cpu_temp plugin: Unknown config key "%s"' % key)

    if path_set:
        collectd.info('cpu_temp plugin: Using overridden path %s' % PATH)
    else:
        collectd.info('cpu_temp plugin: Using default path %s' % PATH)

def read_func():
    # Read raw value
    with open(PATH, 'rb') as f:
        temp = f.read().strip()

    # Convert to degrees celsius
    deg = float(int(temp)) / 1000

    # Dispatch value to collectd
    val = collectd.Values(type='temperature')
    val.plugin = 'cpu_temp'
    val.dispatch(values=[deg])

collectd.register_config(config_func)
collectd.register_read(read_func)
{% endhighlight %}

然後在配置文件中添加如下內容即可。

{% highlight text %}
LoadPlugin python
<Plugin python>
    ModulePath "/opt/collectd_plugins"
    Import "cpu_temp"
    <Module cpu_temp>
        Path "/sys/class/thermal/thermal_zone0/temp"
    </Module>
</Plugin>
{% endhighlight %}

### java

與 Python 相似，同樣是內嵌了 JVM ，並將 API 暴露給 JAVA 程序，這樣就不需要每次重新調用生成新的進程以及啟動 JVM 。

在 CentOS 中，編譯前需要安裝開發包，如 ```java-1.8.0-openjdk-devel```，在通過 ```configure``` 命令進行配置時需要添加 ```--enable-java --with-java=$JAVA_HOME``` 參數；除了上述兩個參數外，還可以通過命令行指定 JAVA 相關的參數，示例如下：

{% highlight text %}
$ ./configure --with-java=$JAVA_HOME JAVA_CFLAGS="-I$JAVA_HOME/include -I$JAVA_HOME/include/linux" \
    JAVA_CPPFLAGS="-I$JAVA_HOME/include -I$JAVA_HOME/include/linux"                                \
    JAVA_LDFLAGS="-I$JAVA_HOME/include -I$JAVA_HOME/include/linux"                                 \
    JAVA_LIBS="-I$JAVA_HOME/include" JAR="/path/to/jar" JAVAC="/path/to/javac"                     \
    --enable-java=force
{% endhighlight %}

編譯完成後，會生成 ```bindings/java/.libs/{collectd-api.jar,generic-jmx.jar}``` 兩個 jar 包；當通過 RPM 包安裝時，默認會安裝到 `/usr/share/collectd/java/` 目錄下。

如下是一個 java 配置內容。

{% highlight text %}
<Plugin "java">
  JVMArg "-verbose:jni"
  JVMArg "-Djava.class.path=/lib/collectd/bindings/java"

  LoadPlugin "org.collectd.java.Foobar"
  <Plugin "org.collectd.java.Foobar">
    # To be parsed by the plugin
  </Plugin>
</Plugin>
{% endhighlight %}

以及採集程序示例。

{% highlight java %}
import org.collectd.api.Collectd;
import org.collectd.api.ValueList;
import org.collectd.api.CollectdReadInterface;

public class Foobar implements CollectdReadInterface
{
  public Foobar ()
  {
    Collectd.registerRead ("Foobar", this);
  }

  public int read ()
  {
    ValueList vl;

    /* Do something... */

    Collectd.dispatchValues (vl);
  }
}
{% endhighlight %}

<!--
https://github.com/auxesis/collectd-opentsdb
https://github.com/BlackBeltTechnology/collectd
https://github.com/allixender/collectd-mqtt
https://github.com/Gnome-OPW/collectd
https://github.com/auxesis/collectd-opentsdb
http://www.programcreek.com/java-api-examples/index.php?api=org.collectd.api.ValueList

內核加密算法
http://bbs.chinaunix.net/thread-1984676-1-1.html
http://blog.chinaunix.net/uid-26126915-id-3687668.html
http://bbs.chinaunix.net/thread-3627341-1-1.html
-->


<!--
### perl

### lua
-->




## 雜項

### libtoolize

如果報 ```libtoolize: 'COPYING.LIB' not found in '/usr/share/libtool/libltdl'``` 錯誤。

依賴 ```libtool-ltdl-devel``` 庫，一個 GUN 提供的庫，類似於 POSIX 的 ```dlopen()``` ，不過據說更簡單且強大；詳細可以參考官方文檔 [Using libltdl](https://www.gnu.org/software/libtool/manual/html_node/Using-libltdl.html) 。

另外，需要注意的是，該庫並不能保證線程安全。

## BUG-FIX

### 日誌輸出

如果將日誌輸出到文件時，可能會報如下的錯誤。

{% highlight text %}
collectd -C collectd.conf -t
logfile plugin: fopen (/opt/collectd/var/log/collectd.log) failed: No such file or directory
{% endhighlight %}

修改 logfile.c 中的默認文件，設置為標準錯誤輸出 ```STDERR``` 。

{% highlight c %}
//#define DEFAULT_LOGFILE LOCALSTATEDIR "/log/collectd.log"
#define DEFAULT_LOGFILE "stderr"
{% endhighlight %}

## FAQ

1\. meta data 的作用是？

meta data (meta_data_t) 用於將一些數據附加到已經存在的結構體上，如 values_list_t，對應的是 KV 結構，其中一個使用場景是 network 插件，用於標示從哪個插件採集來的，防止出現循環，也用於該插件的 Forward 選項。



## 參考

一些與 collectd 相關的工具，可以參考 [Related sites](http://collectd.org/related.shtml) 。






<!--

### 配置文件解析
http://www.cppblog.com/woaidongmao/archive/2008/11/23/67637.html
https://www2.cs.arizona.edu/~debray/Teaching/CSc453/DOCS/lex%20tutorial.ppt
http://web.eecs.utk.edu/~bvz/teaching/cs461Sp11/notes/flex/

通過 flex+bison 解析，源碼保存在 src/liboconfig 目錄下；正常源碼編譯時需要通過 flex+bison 生成源碼文件，這裡實際在發佈前已經轉換，所以在編譯時就免去了這一步驟。
-->



{% highlight text %}
{% endhighlight %}
