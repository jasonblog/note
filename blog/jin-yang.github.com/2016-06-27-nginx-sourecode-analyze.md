---
title: Nginx 源碼解析
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,源碼解析
description: Nginx 是業界之名的高性能服務器，採用模塊化實現，這裡簡單看下其實現方式。
---

Nginx 是業界之名的高性能服務器，採用模塊化實現，這裡簡單看下其實現方式。

<!-- more -->

## 模塊

Nginx 的基礎架構是模塊化的，從 [官方文檔](http://nginx.org/en/docs/) 中可以發現，很大一部分是關於模塊的介紹，可執行文件對應的模塊可以通過 ```nginx -V``` 查看，也就是編譯選項。

nginx 模塊要負責三種角色：1) handler，接收請求併產生輸出；2) filters，處理 hander 產生的輸出；3) load-balancer，負載均衡，選擇一個後端 server 發送請求。

在執行 ```configure``` 命令時，會根據配置選項動態生成 ```objs/ngx_modules.c``` 文件，有效的模塊會包含在 ```ngx_modules[]``` 中。模塊包括了核心模塊和可選模塊，前者是必需的，不會出現在源碼編譯的配置項中，如 ```ngx_core_module``` 模塊；而後者是可以在編譯的時候進行配置的，只有添加了編譯選項之後，才會出現在 ```ngx_modules[]``` 中。

### 相關結構體

其中與模塊相關的數據結構包括。

1. ngx_module_t<br>代表模塊本身，由 nginx 統一定義的結構，其指針通過編譯的配置項被放入 ```ngx_modules[]``` 中。

2. ngx_(module name)_conf_t<br>用來表示模塊的配置內容，由各個模塊進行定義，其中部分成員可以通過配置文件進行配置。

3. ngx_(module name)_module_t<br>各個模塊的通用回調函數，由各個模塊定義，根據各個模塊的特性在不同階段調用相關函數。

4. ngx_command_t<br>對應配置文件中的指令，由 nginx 統一定義。

<!--
所有的模塊通過 struct ngx_module_t 進行定義，除去保留字段之外，主要字段含義如下。

struct ngx_module_s {
    ngx_uint_t       ctx_index;                           // 在ngx_modules數組中，該模塊在相同類型的模塊中的次序
    ngx_uint_t       index;                               // 模塊在ngx_modules[]中的順序，在初始化時設置
    ngx_uint_t       version;
    void            *ctx;                                 // 與模塊相關的上下文
    ngx_command_t   *commands;                            // 一組配置文件中的指令
    ngx_uint_t       type;                                // 模塊類型有CORF、CONF、EVNT、HTTP、MAIL五種
    ngx_int_t      (*init_master)(ngx_log_t *log);
    ngx_int_t      (*init_module)(ngx_cycle_t *cycle);
    ngx_int_t      (*init_process)(ngx_cycle_t *cycle);
    ngx_int_t      (*init_thread)(ngx_cycle_t *cycle);
    void           (*exit_thread)(ngx_cycle_t *cycle);
    void           (*exit_process)(ngx_cycle_t *cycle);
    void           (*exit_master)(ngx_cycle_t *cycle);
};


struct ngx_command_s {
    ngx_str_t             name;    // 指令名，與配置文件中一致
    ngx_uint_t            type;    // 指令類型，以及參數的個數

    /*
     * 回調函數，在解析配置文件時，遇到這個指令時調用。
     * cf: 包括配置參數信息cf->args（ngx_array_t類型），以及指令對應的模塊上下文cf->ctx
     *      在解析不同模塊的指令時，這個上下文信息不同。比如在解析core module時，cf->ctx
     *      是ngx_cycle->conf_ctx也就是所有core module的配置結構數組，而在解析http module
     *      時cf->ctx是ngx_http_conf_ctx_t類型的，其中包含所有http module的main、srv、loc
     *      的配置結構數組。
     * cmd: 指令對應的ngx_command_t結構。
     * conf：指令對應的模塊的配置信息。
     */
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

    /**
     * 對http module有效，http module的配置結構信息（main、srv、loc）都存放在ngx_http_conf_ctx_t
     * 中對應的數組，conf屬性指示這個指令的配置結構是main、srv還是loc。
     */
    ngx_uint_t            conf;
    ngx_uint_t            offset;   // 指令對應屬性在模塊配置結構中的偏移量。
    void                 *post;     // 一般是函數指針，在set回調函數中調用
};

-->

nginx 的模塊分為 4 個大類型的模塊，包括事件模塊、HTTP 模塊、郵件代理相關的 mail 模塊。

### 新建模塊

開發一個 HTTP 模塊的步驟如下，當打開 ```http://localhost/hello_world``` 時，顯示 ```hello world, foobar!!!``` 。

#### 1. 修改配置文件

默認是 ```/etc/nginx/nginx.conf```，在 ```location / { ... }``` 中添加如下內容。

{% highlight text %}
    ... ...
    location /hello_world {
        hello_world;
    }
    ... ...
{% endhighlight %}

#### 2. 添加編譯

編寫 ```auto/modules``` 文件，為了讓 nginx 在 ```configure``` 過程能找到編寫的模塊。

{% highlight text %}
$ cat auto/modules
... ...
if [ $HTTP_ACCESS = YES ]; then
    HTTP_MODULES="$HTTP_MODULES $HTTP_ACCESS_MODULE"
    HTTP_SRCS="$HTTP_SRCS $HTTP_ACCESS_SRCS"
fi
# 上面是原有的, 這裡才是加上的

HTTP_MODULES="$HTTP_MODULES ngx_http_test_module"
HTTP_SRCS="$HTTP_SRCS src/http/modules/ngx_http_test_module.c"
... ...
{% endhighlight %}

#### 3. 編寫源碼

添加如下 ```src/http/module/ngx_http_test_module.c``` 文件。

{% highlight c %}
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_config.h>

static char *set(ngx_conf_t *, ngx_command_t *, void *);
static ngx_int_t handler(ngx_http_request_t *);

static ngx_command_t test_commands[] = {
    {
        ngx_string("hello_world"),
        NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        set,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t test_ctx = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

ngx_module_t ngx_http_test_module = {
    NGX_MODULE_V1,
    &test_ctx,
    test_commands,
    NGX_HTTP_MODULE,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NGX_MODULE_V1_PADDING
};

static char *set(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_core_loc_conf_t *corecf;
    corecf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    corecf->handler = handler;
    return NGX_CONF_OK;
};

static ngx_int_t handler(ngx_http_request_t *req) {
    u_char html[1024] = "&lt;h1&gt;hello world, foorbar!!!&lt;/h1&gt;";
    req->headers_out.status = 200;
    int len = sizeof(html) - 1;
    req->headers_out.content_length_n = len;
    ngx_str_set(&req->headers_out.content_type, "text/html");
    ngx_http_send_header(req);

    ngx_buf_t *b;
    b = ngx_pcalloc(req->pool, sizeof(ngx_buf_t));
    ngx_chain_t out;
    out.buf = b;
    out.next = NULL;
    b->pos = html;
    b->last = html + len;
    b->memory = 1;
    b->last_buf = 1;

    return ngx_http_output_filter(req, &out);
}
{% endhighlight %}

然後可以直接通過 ```./configure && make``` 編譯。

#### 4. 三方模塊

也可以通過如下方式添加。

{% highlight text %}
$ cat /path/to/module1/source/config
ngx_addon_name=ngx_http_test_module
HTTP_MODULES="$HTTP_MODULES ngx_http_test_module"
NGX_ADDON_SRCS="$NGX_ADDON_SRCS $ngx_addon_dir/ngx_http_test_module.c"
CORE_LIBS="$CORE_LIBS -lfoo"

$ cat /path/to/module1/source/ngx_http_test_module.c
... ...

$ ./configure --add-module=/path/to/module1/source     # 編譯
$ make
{% endhighlight %}

## 源碼解析

{% highlight text %}
main()   @ src/core/nginx.c
 |-ngx_debug_init()
 |-ngx_strerror_init()                         // 初始化錯誤提示列表，以errno為下標，元素就是對應的錯誤提示信息
 |-ngx_get_options()                           // 解析命令參數
 |-ngx_time_init()                             // 初始化並更新時間，如全局變量ngx_cached_time
 |-ngx_regex_init()                            // 正則表達式的初始化
 |-ngx_log_init()                              // 初始化日誌
 |-ngx_create_pool()                           // 創建以1024大小的cycle內存塊
 |-ngx_save_argv()                             // 保存命令行參數，方便後面進行熱代碼替換
 |-ngx_process_options()                       // 處理配置文件相關信息，並初始化cycle
 |-ngx_os_init()                               // 處理操作系統相關初始化
 |-ngx_crc32_table_init()                      // 初始化循環冗餘檢驗表
 |-ngx_add_inherited_sockets()                 // 通過環境變量NGINX完成socket的繼承
 |-... ...                                     // +++包括了對ngx_modules[]的index初始化
 |-ngx_init_cycle()                            // 主要的初始化過程
 | |-gethostname()                             // 獲取設置hostname
 | |-... ...                                   // 調用core-modules的create_conf()函數，並準備解析配置文件的數據結構
 | |-ngx_conf_param()                          // -g參數的解析
 | |-ngx_conf_parse()                          // 解析配置文件
 | | |-ngx_conf_handler()
 | |-... ...                                   // 調用core-modules的init_conf()函數
 | |-ngx_create_pidfile()                      // 創建PID文件
 | |-... ...                                   // 創建文件路徑、打開文件描述符以及創建共享內存
 | |-ngx_open_listening_sockets()              // 打開所有的監聽socket
 | | |-ngx_socket()                            // 創建socket套接字
 | | |-setsockopt()                            // 設置可重用
 | | |-ngx_nonblocking()                       // 設置為非阻塞
 | | |-bind()                                  // 綁定要監聽的socket地址
 | | |-listen()                                // 監聽socket
 | |-... ...                                   // 調用所有模塊的init_module()函數
 |-ngx_init_signals()                          // 設置信號處理函數
 |
 |-ngx_single_process_cycle()                  // 如果是單進程
 |-ngx_master_process_cycle()                  // 以master方式運行多進程，主要解析該流程
   |-ngx_start_worker_processes()
   | |-ngx_spawn_process()                     // 創建子進程ngx_worker_process_cycle()
   |   |
   |   |-ngx_worker_process_cycle()            // worker的主要處理流程，子進程處理流程
   |     |-ngx_worker_process_init()           // 初始化
   |     | |-... ...                           // 設置執行環境、優先級等參數
   |     | |-ngx_modules[i]->init_process()    // 調用所有模塊的初始化回調函數
   |     | |-... ...                           // 關閉不使用的socket
   |     |-ngx_setproctitle()                  // 設置進程名稱worker process，然後接下來是死循環
   |     |
   |     |-... ...                             // 通過ngx_exiting判斷，如果進程退出，關閉所有連接
   |     |-ngx_process_events_and_timers()     // 處理事件和計時，處理驚群現象
   |     | |-... ...                           // 在此會通過ngx_use_accept_mutex防止驚群
   |     | |-ngx_process_events()              // 實際調用ngx_event_actions.process_events()
   |     | |-ngx_event_process_posted()
   |     |-... ...                             // 處理終止、退出、重新打開命令
   |
   |-ngx_start_cache_manager_processes()
{% endhighlight %}

### 事件機制

在配置文件中可以通過如下方式選擇不同的事件機制。

{% highlight text %}
events {
    use epoll;  # kqueue, rtsig, epoll, select, poll
}
{% endhighlight %}

在 Linux 中默認會採用 epoll 實現 Nginx 的事件驅動，在 `src/event/modules/ngx_epoll_module.c` 中實現；其中 `ngx_epoll_commands[]` 表明該模塊對那些配置項感興趣。

初始化通過 `ngx_epoll_init()` 實現。

{% highlight text %}
ngx_epoll_init()                     // +++初始化
  |-ngx_event_get_conf()             // 獲取解析所得的配置項的值
  |-epoll_create()                   // 創建epoll對象，多數Linux中的參數無效
  |-ngx_epoll_aio_init()             // 異步IO
  |-ngx_alloc()                      // 創建event_list數組，用於進行epoll_wait調用時傳遞內核對象
{% endhighlight %}


## 參考

一篇介紹 Nginx 模塊的文章，非常不過的入門文章，可以參考 [Emiller's Guide To Nginx Module Development](http://www.evanmiller.org/nginx-modules-guide.html)，也可以查看 [本地文檔](/reference/linux/Guides_to_Nginx_Module_Development.maff) 。

[github nginx-systemtap-toolkit](https://github.com/openresty/nginx-systemtap-toolkit) 通過 systemtap 監控 nginx 的一系列工具。

{% highlight text %}
{% endhighlight %}
