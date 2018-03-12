---
title: Nginx 源码解析
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,源码解析
description: Nginx 是业界之名的高性能服务器，采用模块化实现，这里简单看下其实现方式。
---

Nginx 是业界之名的高性能服务器，采用模块化实现，这里简单看下其实现方式。

<!-- more -->

## 模块

Nginx 的基础架构是模块化的，从 [官方文档](http://nginx.org/en/docs/) 中可以发现，很大一部分是关于模块的介绍，可执行文件对应的模块可以通过 ```nginx -V``` 查看，也就是编译选项。

nginx 模块要负责三种角色：1) handler，接收请求并产生输出；2) filters，处理 hander 产生的输出；3) load-balancer，负载均衡，选择一个后端 server 发送请求。

在执行 ```configure``` 命令时，会根据配置选项动态生成 ```objs/ngx_modules.c``` 文件，有效的模块会包含在 ```ngx_modules[]``` 中。模块包括了核心模块和可选模块，前者是必需的，不会出现在源码编译的配置项中，如 ```ngx_core_module``` 模块；而后者是可以在编译的时候进行配置的，只有添加了编译选项之后，才会出现在 ```ngx_modules[]``` 中。

### 相关结构体

其中与模块相关的数据结构包括。

1. ngx_module_t<br>代表模块本身，由 nginx 统一定义的结构，其指针通过编译的配置项被放入 ```ngx_modules[]``` 中。

2. ngx_(module name)_conf_t<br>用来表示模块的配置内容，由各个模块进行定义，其中部分成员可以通过配置文件进行配置。

3. ngx_(module name)_module_t<br>各个模块的通用回调函数，由各个模块定义，根据各个模块的特性在不同阶段调用相关函数。

4. ngx_command_t<br>对应配置文件中的指令，由 nginx 统一定义。

<!--
所有的模块通过 struct ngx_module_t 进行定义，除去保留字段之外，主要字段含义如下。

struct ngx_module_s {
    ngx_uint_t       ctx_index;                           // 在ngx_modules数组中，该模块在相同类型的模块中的次序
    ngx_uint_t       index;                               // 模块在ngx_modules[]中的顺序，在初始化时设置
    ngx_uint_t       version;
    void            *ctx;                                 // 与模块相关的上下文
    ngx_command_t   *commands;                            // 一组配置文件中的指令
    ngx_uint_t       type;                                // 模块类型有CORF、CONF、EVNT、HTTP、MAIL五种
    ngx_int_t      (*init_master)(ngx_log_t *log);
    ngx_int_t      (*init_module)(ngx_cycle_t *cycle);
    ngx_int_t      (*init_process)(ngx_cycle_t *cycle);
    ngx_int_t      (*init_thread)(ngx_cycle_t *cycle);
    void           (*exit_thread)(ngx_cycle_t *cycle);
    void           (*exit_process)(ngx_cycle_t *cycle);
    void           (*exit_master)(ngx_cycle_t *cycle);
};


struct ngx_command_s {
    ngx_str_t             name;    // 指令名，与配置文件中一致
    ngx_uint_t            type;    // 指令类型，以及参数的个数

    /*
     * 回调函数，在解析配置文件时，遇到这个指令时调用。
     * cf: 包括配置参数信息cf->args（ngx_array_t类型），以及指令对应的模块上下文cf->ctx
     *      在解析不同模块的指令时，这个上下文信息不同。比如在解析core module时，cf->ctx
     *      是ngx_cycle->conf_ctx也就是所有core module的配置结构数组，而在解析http module
     *      时cf->ctx是ngx_http_conf_ctx_t类型的，其中包含所有http module的main、srv、loc
     *      的配置结构数组。
     * cmd: 指令对应的ngx_command_t结构。
     * conf：指令对应的模块的配置信息。
     */
    char               *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

    /**
     * 对http module有效，http module的配置结构信息（main、srv、loc）都存放在ngx_http_conf_ctx_t
     * 中对应的数组，conf属性指示这个指令的配置结构是main、srv还是loc。
     */
    ngx_uint_t            conf;
    ngx_uint_t            offset;   // 指令对应属性在模块配置结构中的偏移量。
    void                 *post;     // 一般是函数指针，在set回调函数中调用
};

-->

nginx 的模块分为 4 个大类型的模块，包括事件模块、HTTP 模块、邮件代理相关的 mail 模块。

### 新建模块

开发一个 HTTP 模块的步骤如下，当打开 ```http://localhost/hello_world``` 时，显示 ```hello world, foobar!!!``` 。

#### 1. 修改配置文件

默认是 ```/etc/nginx/nginx.conf```，在 ```location / { ... }``` 中添加如下内容。

{% highlight text %}
    ... ...
    location /hello_world {
        hello_world;
    }
    ... ...
{% endhighlight %}

#### 2. 添加编译

编写 ```auto/modules``` 文件，为了让 nginx 在 ```configure``` 过程能找到编写的模块。

{% highlight text %}
$ cat auto/modules
... ...
if [ $HTTP_ACCESS = YES ]; then
    HTTP_MODULES="$HTTP_MODULES $HTTP_ACCESS_MODULE"
    HTTP_SRCS="$HTTP_SRCS $HTTP_ACCESS_SRCS"
fi
# 上面是原有的, 这里才是加上的

HTTP_MODULES="$HTTP_MODULES ngx_http_test_module"
HTTP_SRCS="$HTTP_SRCS src/http/modules/ngx_http_test_module.c"
... ...
{% endhighlight %}

#### 3. 编写源码

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

然后可以直接通过 ```./configure && make``` 编译。

#### 4. 三方模块

也可以通过如下方式添加。

{% highlight text %}
$ cat /path/to/module1/source/config
ngx_addon_name=ngx_http_test_module
HTTP_MODULES="$HTTP_MODULES ngx_http_test_module"
NGX_ADDON_SRCS="$NGX_ADDON_SRCS $ngx_addon_dir/ngx_http_test_module.c"
CORE_LIBS="$CORE_LIBS -lfoo"

$ cat /path/to/module1/source/ngx_http_test_module.c
... ...

$ ./configure --add-module=/path/to/module1/source     # 编译
$ make
{% endhighlight %}

## 源码解析

{% highlight text %}
main()   @ src/core/nginx.c
 |-ngx_debug_init()
 |-ngx_strerror_init()                         // 初始化错误提示列表，以errno为下标，元素就是对应的错误提示信息
 |-ngx_get_options()                           // 解析命令参数
 |-ngx_time_init()                             // 初始化并更新时间，如全局变量ngx_cached_time
 |-ngx_regex_init()                            // 正则表达式的初始化
 |-ngx_log_init()                              // 初始化日志
 |-ngx_create_pool()                           // 创建以1024大小的cycle内存块
 |-ngx_save_argv()                             // 保存命令行参数，方便后面进行热代码替换
 |-ngx_process_options()                       // 处理配置文件相关信息，并初始化cycle
 |-ngx_os_init()                               // 处理操作系统相关初始化
 |-ngx_crc32_table_init()                      // 初始化循环冗余检验表
 |-ngx_add_inherited_sockets()                 // 通过环境变量NGINX完成socket的继承
 |-... ...                                     // +++包括了对ngx_modules[]的index初始化
 |-ngx_init_cycle()                            // 主要的初始化过程
 | |-gethostname()                             // 获取设置hostname
 | |-... ...                                   // 调用core-modules的create_conf()函数，并准备解析配置文件的数据结构
 | |-ngx_conf_param()                          // -g参数的解析
 | |-ngx_conf_parse()                          // 解析配置文件
 | | |-ngx_conf_handler()
 | |-... ...                                   // 调用core-modules的init_conf()函数
 | |-ngx_create_pidfile()                      // 创建PID文件
 | |-... ...                                   // 创建文件路径、打开文件描述符以及创建共享内存
 | |-ngx_open_listening_sockets()              // 打开所有的监听socket
 | | |-ngx_socket()                            // 创建socket套接字
 | | |-setsockopt()                            // 设置可重用
 | | |-ngx_nonblocking()                       // 设置为非阻塞
 | | |-bind()                                  // 绑定要监听的socket地址
 | | |-listen()                                // 监听socket
 | |-... ...                                   // 调用所有模块的init_module()函数
 |-ngx_init_signals()                          // 设置信号处理函数
 |
 |-ngx_single_process_cycle()                  // 如果是单进程
 |-ngx_master_process_cycle()                  // 以master方式运行多进程，主要解析该流程
   |-ngx_start_worker_processes()
   | |-ngx_spawn_process()                     // 创建子进程ngx_worker_process_cycle()
   |   |
   |   |-ngx_worker_process_cycle()            // worker的主要处理流程，子进程处理流程
   |     |-ngx_worker_process_init()           // 初始化
   |     | |-... ...                           // 设置执行环境、优先级等参数
   |     | |-ngx_modules[i]->init_process()    // 调用所有模块的初始化回调函数
   |     | |-... ...                           // 关闭不使用的socket
   |     |-ngx_setproctitle()                  // 设置进程名称worker process，然后接下来是死循环
   |     |
   |     |-... ...                             // 通过ngx_exiting判断，如果进程退出，关闭所有连接
   |     |-ngx_process_events_and_timers()     // 处理事件和计时，处理惊群现象
   |     | |-... ...                           // 在此会通过ngx_use_accept_mutex防止惊群
   |     | |-ngx_process_events()              // 实际调用ngx_event_actions.process_events()
   |     | |-ngx_event_process_posted()
   |     |-... ...                             // 处理终止、退出、重新打开命令
   |
   |-ngx_start_cache_manager_processes()
{% endhighlight %}

### 事件机制

在配置文件中可以通过如下方式选择不同的事件机制。

{% highlight text %}
events {
    use epoll;  # kqueue, rtsig, epoll, select, poll
}
{% endhighlight %}

在 Linux 中默认会采用 epoll 实现 Nginx 的事件驱动，在 `src/event/modules/ngx_epoll_module.c` 中实现；其中 `ngx_epoll_commands[]` 表明该模块对那些配置项感兴趣。

初始化通过 `ngx_epoll_init()` 实现。

{% highlight text %}
ngx_epoll_init()                     // +++初始化
  |-ngx_event_get_conf()             // 获取解析所得的配置项的值
  |-epoll_create()                   // 创建epoll对象，多数Linux中的参数无效
  |-ngx_epoll_aio_init()             // 异步IO
  |-ngx_alloc()                      // 创建event_list数组，用于进行epoll_wait调用时传递内核对象
{% endhighlight %}


## 参考

一篇介绍 Nginx 模块的文章，非常不过的入门文章，可以参考 [Emiller's Guide To Nginx Module Development](http://www.evanmiller.org/nginx-modules-guide.html)，也可以查看 [本地文档](/reference/linux/Guides_to_Nginx_Module_Development.maff) 。

[github nginx-systemtap-toolkit](https://github.com/openresty/nginx-systemtap-toolkit) 通过 systemtap 监控 nginx 的一系列工具。

{% highlight text %}
{% endhighlight %}
