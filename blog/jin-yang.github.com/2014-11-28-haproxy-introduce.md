---
title: HAProxy 简介
layout: post
comments: true
language: chinese
category: [linux,network,misc]
keywords: haproxy
description: HAProxy 是一个免费的负载均衡软件，可以运行于大部分主流的 Linux 操作系统上，提供了 L4 和 L7 两种负载均衡能力，可媲美商用负载均衡器的性能和稳定性，使其不仅仅是免费负载均衡软件的首选，更几乎成为了唯一选择。这里简单介绍下 HAProxy 的使用方式。
---

HAProxy 是一个免费的负载均衡软件，可以运行于大部分主流的 Linux 操作系统上，提供了 L4 和 L7 两种负载均衡能力，可媲美商用负载均衡器的性能和稳定性，使其不仅仅是免费负载均衡软件的首选，更几乎成为了唯一选择。

这里简单介绍下 HAProxy 的使用方式。

<!-- more -->

## 简介

相比 Nginx 来说，HAProxy 支持自定义 URL 健康监测；会话保持除了可以使用 `IP_HASH` 外，还可以使用 `URL_HASH` 算法；支持多种负载均衡等等，如下是 HAProxy 提供的主要功能：

* 负载均衡。提供 L4 和 L7 两种模式，支持 `RoundRobin`、`Static RoundRobin`、`LeastConnection`、`Source IP Hash`、`URI Hash`、`URL_PARAM Hash`、`HTTP_HEADER Hash` 等丰富的负载均衡算法；
* 健康检查。支持 `TCP`、`HTTP`、`SSL`、`MySQL`、`Redis` 等多种健康检查模式；
* 会话保持。对于未实现会话共享的应用集群，可通过 `Insert Cookie`、`Rewrite Cookie`、`Prefix Cookie` 以及上述的多种 `Hash` 方式实现会话保持；
* SSL 支持。可以解析 `HTTPS` 协议，并能够将请求解密为 `HTTP` 后向后端传输；
* HTTP。可以对请求进行重写与重定向，支持多种 `HTTP` 模式；
* 监控与统计。提供了基于 Web 的统计页面，展现健康状态和流量数据。

在配置文件中可以通过 `balance` 参数指定算法，如下仅介绍一些常用的算法：

{% highlight text %}
roundrobin:
  轮询，适用于短链接，会根据权重比选择服务器，支持4096个后端服务器；注意，这里的权重信息可以动态修改。
static-rr:
  与上类似，只是这里的服务器权重不能动态修改，而且对于服务器的数量没有限制，消耗CPU相对也较少。
leastconn:
  选择链接数最少的服务器建立链接，通常在使用长连接时，例如MySQL、LDAP等，对于短链接HTTP不建议使用；
  这里的服务器权重可以动态调整。
first:
  忽略权重信息，选择第一个可用服务器，需要设置服务器的最大连接数，适用于长连接；常用于动态扩容的场景，
  一般有工具用于动态监测，提供动态伸缩服务。
source
  源地址算法，通过客户端的原IP地址进行hash，只要没有服务器宕机，那么一个客户端的请求都会由同一台服务器处理。
uri
  算法，需要使用HTTP服务，通过 HTTP URI 地址来选择对应服务器。
hdr
  通过 HTTP eder 内容来选择对应服务器。
{% endhighlight %}

在源码中，其实现在 `lb_XXX.c` 文件中，关于 HAProxy 的健康检查可以参考 [Health checking](https://www.haproxy.com/doc/aloha/7.0/haproxy/healthchecks.html) 。

## 配置文件

详细可以查看官方的文档 [doc/configuration.txt](http://www.haproxy.org/download/1.7/doc/configuration.txt) ，这里简单介绍常见的概念。

如下是常见的运维操作。

{% highlight text %}
----- 测试配置文件是否有语法错误
# haproxy -c -f /etc/haproxy/haproxy.cfg
----- 没有问题则尝试启动
# haproxy -f /etc/haproxy/haproxy.cfg
----- 重新加载
# haproxy -f /etc/haproxy/haproxy.cfg -sf `cat /var/run/haproxy.pid`
{% endhighlight %}

在通过 `systemctl status haproxy` 启动时，也就是使用 systemd 时，实际会有一个 wrap 程序启动，所以会看到有多个进程启动。

HAProxy 配置文件由全局配置+代理配置两部分组成，又分为五段：global、defaults、frontend、backend、listen。

{% highlight text %}
global:
  全局配置内容，用于定义全局参数，属于进程级的配置，通常和操作系统配置有关。
default:
  作为frontend、backend、listen的默认配置参数。
frontend:
  接收请求的前端虚拟节点，在1.3版本引入，用于简化haproxy配置文件复杂度，可以通过ACL规则指定要使用的后端backend。
backend:
  后端服务器配置。
listen:
  frontend+backend结合体，1.3版本之前使用，主要为了保持兼容性。
{% endhighlight %}

### 日志配置

在 `global` 中设置日志的级别，然后通过 syslog 进行保存。

{% highlight text %}
$ cat /etc/haproxy/haproxy.conf
global
  log 127.0.0.1 local2 info
{% endhighlight %}

使用 `rsyslog` 保存。

{% highlight text %}
----- 确认添加了include子目录，因为如下的HAProxy配置会放置到子目录下。
cat /etc/rsyslog.conf | grep "IncludeConfig"
----- 修改rsyslog的主配置文件，开启远程日志，查看是否开启UDP端口
cat /etc/sysconfig/rsyslog | grep "SYSLOGD_OPTIONS"
SYSLOGD_OPTIONS="-c 2 -r -m 0"
#-c 2 使用兼容模式，默认是 -c 5
#-r 开启远程日志
#-m 0 标记时间戳。单位是分钟，为0时，表示禁用该功能

----- 添加HAProxy配置文件，需要注意最后一行，否则会同时写入haproxy.log和message文件
cat /etc/rsyslog.d/haproxy.conf
$ModLoad imudp
$UDPServerRun 514
local2.*     /var/log/haproxy.log
&~
{% endhighlight %}

### 配置文件


{% highlight text %}
#---------------------------------------------------------------------
# Global settings
#---------------------------------------------------------------------
global
    # to have these messages end up in /var/log/haproxy.log you will
    # need to:
    # 1) configure syslog to accept network log events.  This is done
    #    by adding the '-r' option to the SYSLOGD_OPTIONS in
    #    /etc/sysconfig/[r]syslog
    # 2) configure local2 events to go to the /var/log/haproxy.log
    #   file. A line like the following can be added to
    #   /etc/sysconfig/syslog
    #    local2.*                       /var/log/haproxy.log
    log         127.0.0.1 local2 debug


    chroot      /var/lib/haproxy
    pidfile     /var/run/haproxy.pid
    maxconn     4000
    user        haproxy
    group       haproxy
    daemon

    # turn on stats unix socket
    stats socket /var/run/haproxy.sock level admin process 1
    stats timeout 5m                   # 设置等待输入超时时间为5min

#---------------------------------------------------------------------
# common defaults that all the 'listen' and 'backend' sections will
# use if not designated in their block
#---------------------------------------------------------------------
defaults
    mode                    http
    log                     global
    option                  tcplog                 # 默认只打印很少的信息，指定打印详细信息
    option                  httplog
    option                  dontlognull            # 默认会将保活等信息记录到日志，可通过该选项关闭
    option http-server-close
    option                  redispatch             # 使用cookies后会发送到指定服务器，如果服务器不可用则重新调度
    retries                 3                      # 3次连接失败则认为服务不可用
    timeout http-request    10s                    # 默认http请求超时时间
    timeout queue           1m                     # 默认队列超时时间
    timeout connect         10s                    # 默认连接超时时间
    timeout client          1m                     # 默认客户端超时时间
    timeout server          1m                     # 默认服务器超时时间
    timeout http-keep-alive 10s                    # 默认持久连接超时时间
    timeout check           10s                    # 默认检查时间间隔
    maxconn                 3000                   # 最大连接数

#---------------------------------------------------------------------
# main frontend which proxys to the backends
#---------------------------------------------------------------------
frontend  main *:5000
    # 新建acl策略path_beg以/static /images等开头的访问路径，-i忽略大小写 
    acl url_static       path_beg       -i /static /images /javascript /stylesheets
    # 新建acl策略path_end以.jpg .gif等结尾的访问路径，-i忽略大小写 
    acl url_static       path_end       -i .jpg .gif .png .css .js

    # 如果匹配url_static这个acl策略，则使用static这个后端 
    use_backend static          if url_static
    # 没有任何配置的情况下，使用默认的后端app 
    default_backend             app

#---------------------------------------------------------------------
# static backend for serving up images, stylesheets and such
#---------------------------------------------------------------------
backend static
    balance     roundrobin
    server      static 127.0.0.1:4331 check

#---------------------------------------------------------------------
# round robin balancing between the various backends
#---------------------------------------------------------------------
backend app
    balance     roundrobin
    server  app1 127.0.0.1:5001 check
    server  app2 127.0.0.1:5002 check
    server  app3 127.0.0.1:5003 check
    server  app4 127.0.0.1:5004 check

#---------------------------------------------------------------------
# 1.3之前使用，为了保持向后兼容，建议使用frontend+backend代替
# 关于server的详细配置选项可以参考Server and default-server options
#  * maxconn NUM 每个后端服务器的最大链接数，超过该值则保存在队列中
#  * maxqueue NUM 超过该队列后会重新选择后端服务器
#---------------------------------------------------------------------
listen http-proxy 192.168.1.201:80
    mode http
    option httpchk GET /test
    balance roundrobin
    timeout server 30s
    timeout client 30s
    server server-01 192.168.1.101:80 check inter 2s
    server server-02 192.168.1.102:80 check inter 2s
    server server-03 192.168.1.103:80 check inter 2s
{% endhighlight %}


<!--
### HTTPS 配置 (SSL Termination With HAProxy)

对 http://www.foobar.com 域名的访问会自动跳转为 https://www.foobar.com ，而对 http://haproxy.foobar.com 访问走 http 协议。

http://virtuallyhyper.com/2013/05/configure-haproxy-to-load-balance-sites-with-ssl/
http://www.ilanni.com/?p=10641
http://www.oschina.net/translate/getting-the-most-of-haproxy?print

另外，源码目录下有很多不错的文档可以参考，例如架构(architecture.txt)、配置文件(configuration.txt)、管理(management.txt)等，还有很多内部设计文档。

/etc/haproxy
/etc/haproxy/haproxy.cfg #配置文件
/etc/logrotate.d/haproxy #日志轮转
/etc/rc.d/init.d/haproxy #运行脚本
/usr/bin/halog           #日志分析工具
/usr/sbin/haproxy

可以通过haproxy -vv查看详细的编译参数。


## 监控信息

关于监控项的详细介绍可以查看源码 doc/management.txt 文件中 Statistics and monitoring 的内容，目前主要分为了三部分：1) Frontend；2) Backend(Servers)；3) HAProxy Health Check 。

**注意** 按照 HAProxy 的模型来说，是单进程的，如果为了优化多核场景采用多进程，那么需要为每个进程配置一个访问地址。

### 监控页面

可以通过如下配置打开一个 HTTP 监控页面，然后可以直接通过浏览器访问，如果要查看 csv 格式，可以通过 ```curl --silent "http://192.144.35.35:85/status;csv"``` 命令查看。

```
listen stats_auth 192.144.35.35:85
    mode http
    stats enable
    bind-process 1                   # 监控的HAProxy进程
    stats uri /status                # 监控URI地址
    stats admin if LOCALHOST         # 默认监控页面是只读，可以通过该选项开启启停服务等操作
    stats auth monitor:foobar        # 指定登陆用户密码，可以配置多个
    stats realm "Haproxy Statistics" # 如果没有登陆，浏览器的用户名密码弹出窗口
    stats refresh 5s                 # 自动刷新，通常用于一直打开的浏览器页面
    stats hide-version               # 隐藏版本信息
    stats scope .                    # 指定监控显示的范围
    stats show-desc Node for Europe  # 后续的都是描述信息
    stats show-legends
    stats show-node
```

Starting frontend GLOBAL: cannot bind UNIX socket [/var/run/haproxy1.sock]

### Unix Socket

相比来说这是更好的方式，方便自动化、可以在线修改配置、安全，可以通过如下配置进行修改。

```
global # Make sure you add it to the global section
    stats socket /var/run/haproxy.sock mode 600 level admin process 1  # uid 0 gid 0
    stats timeout 2m # Wait up to 2 minutes for input
```

timeout 选项通常是用在交互场景下；而 mode 600 level admin 分别表示 socket 的权限，以及是否开启管理模式，默认是只读的；然后就可以通过 netcat 或者 socat 进行访问，当然官方推荐使用后者，而实际上前者更加通用些；Socket 方式提供了非交互式以及交互式两种。

非交互方式比较适合脚本采集监控数据，可以一次发送多个命令，如下分别返回 HAProxy 的进程信息，以及监控数据(页面 URI 返回的数据)。

echo "show info;show stat" | nc -U /var/run/haproxy.sock

交互模式实际上也就是单行，比较适合登陆查看，如下通过 prompt 命令，表示进入交互模式；更多命令可以查看源码 doc/management.txt 文件中 Unix Socket commands 的内容。

$ nc -U /var/run/haproxy.sock
$ prompt
> show info

cannot bind socket
cat /etc/sysctl.conf
net.ipv4.ip_nonlocal_bind=1
sysctl -p


### 监控指标

列举常用的监控指标，以及源码中对应的函数。

#### show info

对应源码中的 stats_fill_info() 函数，主要包括了当前 HAProxy 的配置信息，例如资源限制、资源使用率等信息。

Name: HAProxy
Version: 1.5.18
Release_date: 2016/05/10
Nbproc: 4                  配置的进程数，在配置文件中通过nbproc配置
Process_num: 1             当前的进程号，编号从1开始
Pid: 96699                 当前的进程ID
Uptime: 1d 20h10m06s       服务启动时间，格式化后
Uptime_sec: 159006         服务启动时间，按秒计算
Memmax_MB: 0               当前进程可以使用的最大内存，对应了global.rlimit_memmax变量，会在启动时通过global.rlimit_memmax_all计算
Ulimit-n: 8036             单个进程最大的描述符数global.rlimit_nofile，可以在配置文件中通过ulimit-n设置，否则与maxsock相同
Maxsock: 8036              最大sock数目，在程序中单独计算，与maxconn、pipe数量相关，详见global.maxsock变量
Maxconn: 4000              可以在配置文件中通过maxconn设置
Hard_maxconn: 4000
CurrConns: 0
CumConns: 3
CumReq: 3
Maxpipes: 0
PipesUsed: 0
PipesFree: 0
ConnRate: 0
ConnRateLimit: 0
MaxConnRate: 0
SessRate: 0
SessRateLimit: 0
MaxSessRate: 0
ZlibMemUsage: 0
MaxZlibMemUsage: 0
Tasks: 11                  内存中保存的任务数，通过task_new()函数分配，注意可能部分任务只是缓存，并没有使用
Run_queue: 1               当前运行队列的大小，可以参考run_queue_cur变量，包含了当前查询的链接，详见process_runnable_tasks()函数
Idle_pct: 100              空闲比率
node: hamonitor1           节点信息

si_attach_conn()
 |-conn_attach()


cfg_register_section() 通过该函数将配置文件解析函数添加到sections中，会调用section_parser()函数解析。

整个HAProxy配置文件解析完成后，listener相关也已初始化完毕，如下简单梳理一下几个accept方法的设计逻辑：
* listener_accept(): 对应不同协议的accept()方法，IPv4对应该方法，负责接收并新建TCP连接；该函数中通过accept()系统调用建立链接后，接着会触发listener自己的accept方法也就是session_accept_fd()；
* session_accept_fd(): 负责创建+初始化session，并在stream_new()函数中调用frontend的accept方法frontend_accept()；
* frontend_accept(): 该函数主要负责session前端的TCP连接的初始化，包括socket设置，log设置，以及session部分成员的初始化。
接下来 TCP 新建连接处理过程，基本上都是与这三个函数相关。


建链的处理都是通过_do_poll()处理，只是不同的多路复用方式实现不同，而入口的函数名相同，位于不同的文件中。

session_accept()


tcp_bind_listener()

stats_parse_global()
 |-str2listener() 监听地址解析

上述客户端的处理在cli.c文件中，
cli_io_handler()
cli_parse_request()

uxst_bind_listener() 报错


### tips

#### 多路复用
http://www.cnblogs.com/Anker/p/3263780.html
HAProxy支持多种多路复用机制，包括了select、poll、epoll、kqueue等，简单介绍下ev_epoll.c中实现的epoll机制。


int fd_nbupdt = 0;             // number of updates in the list
unsigned int *fd_updt = NULL;  // FD updates list
这两个全局变量用来记录状态需要更新的 fd 的数量及具体的 fd ，在 _do_poll() 函数中会根据这些信息修改对应 fd 的 epoll 设置。

/* info about one given fd */
struct fdtab {
    int (*iocb)(int fd);                 /* I/O handler, returns FD_WAIT_* */
    void *owner;                         /* the connection or listener associated with this fd, NULL if closed */
    unsigned int  cache;                 /* position+1 in the FD cache. 0=not in cache. */
    unsigned char state;                 /* FD state for read and write directions (2*3 bits) */
    unsigned char ev;                    /* event seen in return of poll() : FD_POLL_* */
    unsigned char new:1;                 /* 1 if this fd has just been created */
    unsigned char updated:1;             /* 1 if this fd is already in the update list */
    unsigned char linger_risk:1;         /* 1 if we must kill lingering before closing */
    unsigned char cloned:1;              /* 1 if a cloned socket, requires EPOLL_CTL_DEL on close */
};
struct fdtab *fdtab = NULL;     /* array of all the file descriptors */

全局变量 *fdtab 记录了 HAProxy 中所有与 fd 相关的信息，数组的 index 是 fd 对应的值，用于快速定位到某个 fd 的信息。

在 fd.h 头文件中定义了一些操作 fd event 的函数，主要是fd_(want|stop)_(recv|send)类似的函数，用于设置 fd 启动、停止接收以及发送；简单来说就是修改fdtabl[fd]中的状态值，如果有更新则添加到fd_updt[]数组中。

接下来，看下_do_poll()函数中的主要处理过程，大致可以分为三部分：
1. 从fd_updt[]获取更新的fd列表，判断各个fd event的变化情况，并通过epoll_ctl()进行设置；
2. 计算epoll_wait需要等待的最大超时时间，并调用epoll_wait()获取当前活动的fd；
3. 逐一处理所有有 IO 事件的 fd。

        fdtab[fd].updated = 0; // 只有被修改过(更新、新建)且有owner才会被使用
        eo = fdtab[fd].state;  // 关于状态的概念、转换等信息可以查看src/fd.c文件中的注释
        en = fd_compute_new_polled_status(eo);  // 状态转换可以查看fd.c中的注释
        if ((eo ^ en) & FD_EV_POLLED_RW) { // 1. 按位异或，如果有变更则通过epoll_ctl()修改

#### 位或 VS. 逻辑或
在HAProxy中有一个函数用来判断buffer是否为空的代码：
static inline int buffer_not_empty(const struct buffer *buf)
{
 return buf->i | buf->o;
}
其中 buf->i 和 buf->o 都是 unsigned int 类型，这两个成员任何一个非零就代表着 buffer 不空，正常来说应该采用 logical-OR 但是这里采用的却是 bitwise-OR ，其原因是两者的功能相同，而前者生成的机器代码更少效率更高。
static inline int buffer_not_empty(const struct buffer *buf)
{
 return buf->i || buf->o;
}

#### 实现文件复制

最简单的方式就是申请一份内存buffer，通过系统调用 read() 读取源文件的一段数据到 buffer，然后将此 buffer 再通过 write() 系统调用写到目标文件，示例代码如下：

char buf[max_read];
off_t size = stat_buf.st_size, off_in=0;
while ( off_in < size ) {
 int len = size - off_in > max_read ? max_read : size - off_in;
 len = read(f_in, buf, len);
 off_in += len;
 write(f_out, buf, len);
}

还有一种方式是通过 mmap() 系统调用实现，示例代码如下：

size_t filesize = stat_buf.st_size;
source = mmap(0, filesize, PROT_READ, MAP_SHARED, f_in, 0);
target = mmap(0, filesize, PROT_WRITE, MAP_SHARED, f_out, 0);
memcpy(target, source, filesize);

因为 mmap() 不需要内核态和用户态的内存拷贝，从而效率大大提高。另外一种是通过 splice() 系统调用实现，这是 Linux 2.6.17 新加入的系统调用，用于在两个文件间移动数据，而无需内核态和用户态的内存拷贝，但需要借助管道 (pipe) 实现。

大概原理就是通过pipe buffer实现一组内核内存页（pages of kernel memory）的引用计数指针（reference-counted pointers），数据拷贝过程中并不真正拷贝数据，而是创建一个新的指向内存页的指针。也就是说拷贝过程实质是指针的拷贝。示例代码如下：

int pipefd[2];
pipe( pipefd );
int max_read = 4096;
off_t size = stat_buf.st_size;
while ( off_in < size ) {
 int len = size - off_in > max_read ? max_read : size - off_in;
 len = splice(f_in, &off_in, pipefd[1], NULL, len, SPLICE_F_MORE |SPLICE_F_MOVE);
 splice(pipefd[0], NULL, f_out, &off_out, len, SPLICE_F_MORE |SPLICE_F_MOVE);
}

使用splice一定要注意，因为其借助管道实现，而管道有众所周知的空间限制问题，超过了限制就会hang住，所以每次写入管道的数据量好严格控制，保守的建议值是一个内存页大小，即4k。另外，off_in和off_out传递的是指针，其值splice会做一定变动，使用时应注意。
http://abcdxyzk.github.io/blog/2015/05/07/kernel-mm-splice/
https://www.ibm.com/developerworks/cn/linux/l-cn-zerocopy2/

http://blog.chinaunix.net/uid/10167808/list/1.html?cid=178438
http://tech.uc.cn/?p=1523
http://blog.csdn.net/dylan_csdn/article/details/51261421
https://www.linux.com/blog/how-analyze-haproxy-logs
http://fangpeishi.com/haproxy_best_practice_notes.html
http://abcdxyzk.github.io/blog/2015/07/29/tools-haproxy_src1/
http://tech.uc.cn/?tag=haproxy
http://tech.uc.cn/?p=1738#more-1738
http://www.ttlsa.com/linux/haproxy-study-tutorial/

https://github.com/1u4nx/Exploit-Exercises-Nebula

minconn, maxconn and fullconn参数的对比
http://www.serverphorums.com/read.php?10,292171
运维0宕机时间，使用TC+IPtables技术
https://engineeringblog.yelp.com/2015/04/true-zero-downtime-haproxy-reloads.html
性能调优
https://medium.freecodecamp.com/how-we-fine-tuned-haproxy-to-achieve-2-000-000-concurrent-ssl-connections-d017e61a4d27
常用Python工具
https://github.com/feurix/hatop/blob/master/bin/hatop
https://github.com/unixsurfer/haproxyadmin
https://github.com/breakwang/zabbix-external-script-haproxy_monitor/blob/master/haproxy_monitor.py
HAProxy源码介绍
http://blog.chinaunix.net/uid/10167808/list/1.html?cid=178438
用HAProxy来检测MySQL复制的延迟的教程
http://www.jb51.net/article/64615.htm
官方blog
https://blog.haproxy.com/
haproxy的stick-table实际应用探索
http://blog.sina.com.cn/s/blog_704836f40102w243.html
HAProxy的独门武器：ebtree
http://mp.weixin.qq.com/s?__biz=MzA5NzA0ODA1Nw==&mid=2650513447&idx=1&sn=a44562e281beef32e6e2f59d2d887b9c&scene=5&srcid=05258uXjxyX8evtbriDv0wYn#rd

#### show pools

对应源码中 dump_pools_to_trash() 函数，用于显示当前内存池的使用情况。

```
Dumping pools usage. Use SIGQUIT to flush them.
  - Pool pipe (32 bytes) : 5 allocated (160 bytes), 5 used, 3 users [SHARED]
  - Pool capture (64 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool channel (80 bytes) : 2 allocated (160 bytes), 2 used, 1 users [SHARED]
  - Pool task (112 bytes) : 11 allocated (1232 bytes), 11 used, 1 users [SHARED]
  - Pool uniqueid (128 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool connection (320 bytes) : 2 allocated (640 bytes), 2 used, 1 users [SHARED]
  - Pool hdr_idx (416 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool session (864 bytes) : 1 allocated (864 bytes), 1 used, 1 users [SHARED]
  - Pool requri (1024 bytes) : 0 allocated (0 bytes), 0 used, 1 users [SHARED]
  - Pool buffer (16416 bytes) : 2 allocated (32832 bytes), 2 used, 1 users [SHARED]
Total: 10 pools, 35888 bytes allocated, 35888 used.
```

#### show stat

对应到源码中的 cli_io_handler_dump_stat() 函数，这里实际会打印一个 proxy 对应的信息，frontend 和 backend 略有区别，而 backend 和 server 基本相同。

```
>>>>>frontend<<<<<
Session:
   current(scur)               px->feconn                   session_accept_fd()     前端建立的会话数，可以用于评估当前资源使用量，是否达到了maxconn；如果迅速上涨，说明backend异常或者DDoS攻击。
   max(smax)                   px->fe_counters.conn_max     session_accept_fd()     计数清零以来，所建立连接的最大值。
   limit(slim)                 px->maxconn                                          在配置文件中设置的最大值
   total(stot)                 px->fe_counters.cum_sess     proxy_inc_fe_sess_ctr() 已处理的请求数，一直递增，在session_count_new()函数中调用
Session Rate:                 
   Current Connection Rate     px->fe_conn_per_sec          proxy_inc_fe_conn_ctr()
   Current Session Rate(rate)  px->fe_sess_per_sec          proxy_inc_fe_sess_ctr()
   Max connection rate         px->fe_counters.cps_max      proxy_inc_fe_conn_ctr()
   Max session rate            px->fe_counters.sps_max      proxy_inc_fe_sess_ctr()
   (rate_lim)                  px->fe_sps_lim
   (rate_max)                  px->fe_counters.sps_max
Bytes:                        
   In(in)                      px->fe_counters.bytes_in
   Out(out)                    px->fe_counters.bytes_out
   Out(bypassed)               px->fe_counters.comp_byp
Denied:                       
   Req(dreq)                   px->fe_counters.denied_req  tcp_exec_action_silent_drop() 处于安全考虑拒绝的请求数，一般是根据规则丢弃
   Resp(dresp)                 px->fe_counters.denied_resp 同上，拒绝的响应数
Errors:                       
   Req(ereq)                   px->fe_counters.failed_req  在HTTP模式中会有失败
>>>>>backend<<<<<
Queue:
   current(qcur)               px->nbpend                   pendconn_add()          默认会直接添加到各个后端Server服务器的队列中，如果已满则添加到be队列中。
   max(qmax)                   px->be_counters.nbpend_max   pendconn_add()          计数清零以来，所建立会话的最大值。
Session:
   current(scur)               px->beconn                   stream_set_backend()    默认会直接添加到各个后端Server服务器的队列中，如果已满则添加到be队列中。
   max(smax)                   px->be_counters.nbpend_max   pendconn_add()          计数清零以来，所建立会话的最大值。
   limit(slim)                 px->fullconn                                         在配置文件中设置，可以通过fullconn设置，否则计算所有proxy的累加值
   total(stot)                 px->be_counters.cum_conn     proxy_inc_be_ctr()      已处理的请求数，一直递增
Session Rate: xxx                
   Current Connection Rate     px->fe_conn_per_sec          proxy_inc_fe_conn_ctr()
   Current Session Rate(rate)  px->fe_sess_per_sec          proxy_inc_fe_sess_ctr()
   Max connection rate         px->fe_counters.cps_max      proxy_inc_fe_conn_ctr()
   Max session rate            px->fe_counters.sps_max      proxy_inc_fe_sess_ctr()
   (rate_lim)                  px->fe_sps_lim
   (rate_max)                  px->fe_counters.sps_max
Bytes:                        
   In(in)                      px->be_counters.bytes_in
   Out(out)                    px->be_counters.bytes_out
Denied:                       
   Req(dreq)                   px->be_counters.denied_req  处于安全考虑拒绝的请求数，一般是根据规则丢弃
   Resp(dresp)                 px->be_counters.denied_resp 同上，拒绝的响应数
Errors:                       
   Conn(econ)                  px->be_counters.failed_conns                         场景的是队列满、没有服务器等，导致请求发送失败
   Resq(eresp)                 px->be_counters.failed_resp                          只在HTTP模式中存在
Warnings:
   Retr(wretr)                 px->be_counters.retries                              链接后端失败的次数，可以尝试多次
   Redis(wredis)               px->be_counters.redispatches                         尝试多次后仍然失败，则发送给其他服务器，注意需要配置允许redispatch选项
Server:
   Act(act)                    px->srv_act                                          当前有多少服务是正常的
   Bck(bck)                    px->srv_bck                                          backup服务器可用数
```

#### 其它

除此之外，还可以通过 ```show errors```、```show sess [id]```、```show table [id]```、```show acl [id]```、```show map [id]``` 命令查看状态。

Connection和Session的区别

在建立连接之后，需要一些初始化操作，只有正常完成这些操作之后，才会作为会话。

tcp_connect_server()
 |-conn_ctrl_init()

listener_accept()
 |-accept() 调用系统API建立连接

session_accept_fd
 |-proxy_inc_fe_conn_ctr() 新增connect统计信息
 |-session_count_new() 新增session统计信息
 | |-proxy_inc_fe_sess_ctr() 统计信息更新

HAProxy当前的任务信息保存在static struct eb_root rqueue指定的队列中，这是一个ebtree结构体，每个任务通过struct task结构体表示。

很多的状态信息处理在src/stats.c文件中，例如listener的监控数据处理在stats_fill_li_stats()函数中；而一些宏则通过enum stat_field定义，例如ST_F_PXNAME(pxname)值。



### 配置文件解析
cfgparse_init() 会在main函数启动前添加默认解析配置项

  clear counters : clear max statistics counters (add 'all' for all counters)
  clear table    : remove an entry from a table
  help           : this message
  prompt         : toggle interactive mode with prompt
  quit           : disconnect
 
  show info      : report information about the running process
  show pools     : report information about the memory pools usage
  show stat      : report counters for each proxy and server
  show errors    : report last request and response errors for each proxy
  show sess [id] : report the list of current sessions or dump this session
  show table [id]: report table usage stats or dump this table's contents
  get weight     : report a server's current weight
  set weight     : change a server's weight
  set server     : change a server's state or weight
  set table [id] : update or create a table entry's data
  set timeout    : change a timeout setting
  set maxconn    : change a maxconn setting
  set rate-limit : change a rate limiting value
  disable        : put a server or frontend in maintenance mode
  enable         : re-enable a server or frontend which is in maintenance mode
  shutdown       : kill a session or a frontend (eg:to release listening ports)
  show acl [id]  : report available acls or dump an acl's contents
  get acl        : reports the patterns matching a sample for an ACL
  add acl        : add acl entry
  del acl        : delete acl entry
  clear acl <id> : clear the content of this acl
  show map [id]  : report available maps or dump a map's contents
  get map        : reports the keys and values matching a sample for a map
  set map        : modify map entry
  add map        : add map entry
  del map        : delete map entry
  clear map <id> : clear the content of this map
  set ssl <stmt> : set statement for ssl

TCP(Layer 4)和HTTP(Layer 7)负载均衡，反向代理。HAProxy单点，可以使用Pacemaker或者Keepalived提供高可用

ACLs
  Extract some information, make decision
  Block request, select backend, rewrite headers, etc.
TCP mode (Layer 4)
  Basic TCP services, SSL passthrough
  Some ACLs available
HTTP mode (Layer 7)
  HTTP header inspection ACLs
  Persistence with cookie insertion
Stick Tables
  Record information in table, eg. source address
  Check table for new connections, select backend

后台服务器只能看到HAProxy，HTTP可以通过添加X-Forwarded-For头部信息。

curl --silent "http://192.144.35.35:85/status;csv"

info_field_names[] <==> show info
stat_field_names[] <==> show stat

http_stats_io_handler() 内置HTTP服务器处理
 |-stats_dump_stat_to_buffer()     // same as above, but used for CSV or HTML
   |-stats_dump_csv_header()      // emits the CSV headers (same as above)
   |-stats_dump_html_head()       // emits the HTML headers
   |-stats_dump_html_info()       // emits the equivalent of "show info" at the top
   |-stats_dump_proxy_to_buffer() // same as above, valid for CSV and HTML
   | |-stats_dump_html_px_hdr()
   | |-stats_dump_fe_stats() frontend statistics
   | | |-stats_fill_fe_stats()
   | |-stats_dump_li_stats() listener statistics
   | | |-stats_fill_li_stats() 将数据保存在静态变量stats中
   | |-stats_dump_sv_stats() server statistics
   | | |-stats_fill_sv_stats()
   | |-stats_dump_be_stats() backend statistics
   | |-stats_dump_html_px_end()
   |-stats_dump_html_end()       // emits HTML trailer

 * cli_io_handler()
 *     -> stats_dump_sess_to_buffer()     // "show sess"
 *     -> stats_dump_errors_to_buffer()   // "show errors"
 *     -> stats_dump_info_to_buffer()     // "show info"
 *     -> stats_dump_backend_to_buffer()  // "show backend"
 *     -> stats_dump_servers_state_to_buffer() // "show servers state [<backend name>]"
 *     -> stats_dump_stat_to_buffer()     // "show stat"
 *        -> stats_dump_resolvers_to_buffer() // "show stat resolver <id>"
 *        -> stats_dump_csv_header()
 *        -> stats_dump_proxy_to_buffer()
 *           -> stats_dump_fe_stats()
 *           -> stats_dump_li_stats()
 *           -> stats_dump_sv_stats()
 *           -> stats_dump_be_stats()
 *
 * http_stats_io_handler()
 *     -> stats_dump_stat_to_buffer()     // same as above, but used for CSV or HTML
 *        -> stats_dump_csv_header()      // emits the CSV headers (same as above)
 *        -> stats_dump_html_head()       // emits the HTML headers
 *        -> stats_dump_html_info()       // emits the equivalent of "show info" at the top
 *        -> stats_dump_proxy_to_buffer() // same as above, valid for CSV and HTML
 *           -> stats_dump_html_px_hdr()
 *           -> stats_dump_fe_stats()
 *           -> stats_dump_li_stats()
 *           -> stats_dump_sv_stats()
 *           -> stats_dump_be_stats()
 *           -> stats_dump_html_px_end()
 *        -> stats_dump_html_end()       // emits HTML trailer

$ nc -U /var/run/haproxy.sock
$ prompt
> show info
cli_io_handler() Unix Socket的命令处理函数接口，这里的处理通过状态机完成
 |-bo_getline() 读取命令行
 |-cli_parse_request() 调用定义的回调函数解析命令，并复制给appctx变量
 | |-cli_find_kw() 查找定义的结构体，主要是各种回调函数
 |-appctx->io_handler() 调用各个插件定义的处理函数

struct cli_kw_list {
  struct list list;
  struct cli_kw kw[VAR_ARRAY];
};


show info; 结构体定义<->enum info_field@stats.h  回调函数<->cli_io_handler_dump_info()@stat.c
cli_io_handler_dump_info()
 |-stats_dump_info_to_buffer()
   |-stats_fill_info() 填充数值，各个HAProxy监控信息与内部变量的对应关系

show pools; 通过pools
cli_io_handler_dump_pools()
 |-dump_pools_to_trash()

echo "show stat" | nc -U /var/run/haproxy.sock
cli_io_handler_dump_stat()
 |-stats_dump_stat_to_buffer() HTML和CLI都会调用该函数
   |-stats_dump_csv_header() 打印CSV头信息
   |-stats_dump_proxy_to_buffer() 会循环调用打印proxy信息，使用csv格式
     |-stats_dump_fe_stats() frontend statistics
     | |-stats_fill_fe_stats()
     |-stats_dump_li_stats() listener statistics
     | |-stats_fill_li_stats() 将数据保存在静态变量stats中
     |-stats_dump_sv_stats() server statistics
     | |-stats_fill_sv_stats()
     |-stats_dump_be_stats() backend statistics
  | |-stats_fill_be_stats()
     |-stats_dump_html_px_end()
  
echo "show sess" | nc -U /var/run/haproxy.sock
cli_io_handler_dump_sess()

echo "show table" | nc -U /var/run/haproxy.sock
cli_io_handler_table()

echo "show acl" | nc -U /var/run/haproxy.sock
cli_parse_show_map()

echo "show map" | nc -U /var/run/haproxy.sock
cli_parse_show_map()

stick table
一般来说可以通过cookies和hash来映射front+back end，所以就有了stick table。

客户端支持的命令通过struct cli_kw_list结构体定义，例如：show pools(memory.c)、show stats(stats.c)等，该结构体中包括了解析函数、处理函数的定义；而且利用GCC的__attribute__((constructor))特性，在执行main()函数之前调用cli_register_kw()函数执行，而该函数实际就是将其添加到cli_keywords.list中。

/usr/bin/python /usr/local/bin/denyhosts.py --daemon --config=/usr/share/denyhosts/denyhosts.cfg

 
### 服务器状态检测
用于检查后台的服务器是否正常，可以根据不同服务类型进行检测。
check: httpchk、smtpchk、mysql-check、pgsql-check、ssl-hello-chk
  默认认为服务器是正常的，通过该参数表示需要进行检测确认服务器状态；
rise:
  多少次检测正常后认为该服务器是正常的；
fall:
  多少次检测失败后认为该服务器异常；
inter:
  设置检测的时间间隔，另外还可以定义fastinter、downinter参数，根据不同的状态设置检测时间间隔；



timeout check 通过min("timeout connect", "inter")作为与后台服务器建立连接的超时时间；该参数对应了建立链接后读取数据的超时时间；如果该参数没有设置，那么就直接使用inter作为(connect+read)的超时时间；一般来说检查的时间要小于正常业务处理时间，所以该值要小于timeout server指定的超时时间。

timeout client                            X          X         X         -
timeout client-fin                        X          X         X         -
timeout connect                           X          -         X         X
timeout http-keep-alive                   X          X         X         X
timeout http-request                      X          X         X         X
timeout queue                             X          -         X         X
timeout server                            X          -         X         X
timeout server-fin                        X          -         X         X
timeout tarpit                            X          X         X         X
timeout tunnel                            X          -         X         X

haproxy 负责处理请求的核心数据结构是 struct session，本文不对该数据结构进行分析。

从业务的处理的角度，简单介绍一下对 session 的理解：

    haproxy 每接收到 client 的一个连接，便会创建一个 session 结构，该结构一直伴随着连接的处理，直至连接被关闭，session 才会被释放
    haproxy 其他的数据结构，大多会通过引用的方式和 session 进行关联
    一个业务 session 上会存在两个 TCP 连接，一个是 client 到 haproxy，一个是 haproxy 到后端 server。

此外，一个 session，通常还要对应一个 task，haproxy 最终用来做调度的是通过 task。
struct proxy {
  struct lbprm lbprm;  // 负载均衡参数
};
-->

## 源码解析

`HAProxy is a single-threaded, event-driven, non-blocking daemon.`

启动非常简单，只需要一个可执行文件+配置文件即可，启动之后基本处理如下的三个操作：

{% highlight text %}
1. 处理链接；
   1.1 从frontend指定的监听端口创建链接；
   1.2 应用frontend指定的规则，包括了阻塞、修改头部信息、统计等；
   1.3 将链接传递给backend指定的服务器；
   1.4 应用backend指定的规则；
   1.5 根据策略决定将链接发送给那个服务器；
   1.6 对响应数据应用backend指定的规则；
   1.7 对响应数据应用frontend指定的规则；
   1.8 发送日志；
   1.9 如果是HTTP那么会等待新请求，否则就关闭链接；
2. 周期性检查服务器的状态；
3. 与其它HAProxy节点交换数据；
{% endhighlight %}

然后，直接看下源码解析过程。

{% highlight text %}
main()
 |-init()                                  ← 所有的初始化操作，包括各个模块初始化、命令行解析等
 | |-cfgfiles_expand_directories()         ← 处理配置文件，参数解析会将配置文件保存在cfg_cfgfiles
 | |-init_default_instance()               ← 初始化默认配置
 | |-readcfgfile()                         ← 读取配置文件，并调用sections->section_parser()对应的函数
 | | |-cfg_parse_listen()                  ← 对于frontend、backend、listen段的参数解析验证
 | |   |-str2listener()
 | |     |-l->frontend=curproxy
 | |     |-tcpv4_add_listener()            ← 添加到proto_tcpv4对象中的链表，真正监听会在proto_XXX.c文件中
 | |       |-listener->proto=&proto_tcpv4  ← 会设置该变量，后续的接收链接也就对应了accept变量
 | |-check_config_validity()               ← 配置文件格式校验
 | | |-listener->frontend=curproxy         ← 在上面解析，实际上curporxy->accept=frontend_accept
 | | |-listener->accept=session_accept_fd
 | | |-listener->handler=process_stream
 | | |-根据不同的后端服务器选择算法选择
 | |
 | | <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<解析完配置文件>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 | |
 | |-start_checks()                        ← 执行后端检查任务
 | | |-start_check_task()
 | |   |-process_chk()
 | |     |-process_chk_conn()
 | |       |-connect_conn_chk()
 | |         |-event_srv_chk_r()           ← 健康检查，在check_conn_cb变量中定义，通过conn_attach()绑定
 | |           |-tcpcheck_main()           ← 如果是TCP检查PR_O2_TCPCHK_CHK
 | |
 | |-init_pollers()                        ← 选择多路复用方法，也就是设置cur_poller
 |   |-calloc()                            ← 分配资源fd_cache+fd_updt，其大小是global.maxsock
 |   |-bp->init()                          ← 调用各个可用poll方法的初始化函数，选择第一个可用方法
 |   |-memcpy()                            ← 复制到cur_poller全局变量中
 |
 |-start_proxies()                         ← 开始启动，调用各协议bind接口，对TCPv4就是tcp_bind_listener()
 |-protocol_bind_all()
 |-protocol_enable_all()                   ← 启动各个协议，例如ipv4/ipv6/unix等
 |
 | <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<主循环处理流程>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 |
 |-run_poll_loop()
 | |-tv_update_date()
 | | ###WHILE###BEGIN
 | |-process_runnable_tasks()              ← 调用可运行任务
 | | |-process_stream()                    ← 一般是调用该函数，也可执行struct task.process中自定义函数
 | |   |
 | |   | <<PHASE:解析请求>>                ← 会根据定义的各种规则选择后端
 | |   |-process_switching_rules()         ← 除了默认使用的后端服务器之外，会根据规则再次选择<<RULES>>
 | |   | |-stream_set_backend()            ← 选择该后端，<<STAT:be->beconn>>
 | |   |   |-proxy_inc_be_ctr()            ← 后端统计值的更新
 | |   |-http_process_tarpit()
 | |   |
 | |   | <<PHASE:解析响应>>
 | |   |-process_store_rules()             ← 处理规则
 | |   |
 | |   | <<PAHSE:正式处理请求>>
 | |   |-sess_prepare_conn_req()           ← 选择后端的服务器
 | |     |-srv_redispatch_connect()
 | |       |-assign_server_and_queue()     ← 选择后端的服务器，并添加到队列中
 | |         |-assign_server()             ← 根据负载均衡配置选择后端服务器
 | |         | |-get_server_sh()
 | |         |   |-chash_get_server_hash() ← 使用一致性hash算法
 | |         |-sess_change_server()
 | |         |-pendconn_add()              ← 如果后端服务器已满，则添加到proxy队列中<<STAT:px->nbpend++>>
 | |
 | |-signal_process_queue()                ← 信号队列，如果捕获了信号则处理
 | |-wake_expired_tasks()                  ← 超时任务
 | |-cur_poller.poll()                     ← 不同平台的多路复用技术
 | | |-_do_poll()                          ← 以ev_epoll.c中的epoll为例
 | |   |-epoll_wait()
 | |   |-fd_may_recv()
 | |   | |-fd_update_cache()               ← 在处理函数中只添加到cache中，真正的处理过程在后面
 | |   |-fd_may_send()
 | |     |-fd_update_cache()
 | |
 | |-fd_process_cached_events()            ← 真正处理epoll()中触发的事件
 | | |-fdtab[fd].iocb(fd)                  ← 调用注册的回调函数，一般是conn_fd_handler()
 | |   |-conn->data->recv(conn)            ← 实际调用si_conn_recv_cb()函数，也就是负责接收的函数
 | |   | |-conn->xprt->rcv_pipe()          ← 如果不启用SSL则调用raw_sock_to_pipe()，否则调用下面的buff函数
 | |   | |-conn->xprt->rcv_buf()           ← 不启用SSL则调用raw_sock_to_buf()，否则调用ssl_sock_to_buf()
 | |   |-conn->data->recv(conn)            ← 实际调用si_conn_send_cb()函数，也就是负责发送的函数
 | |
 | |-applet_run_active()
 | | ###WHILE###END
 |-deinit() 清理操作
{% endhighlight %}

## 参考

官方网站为 [www.haproxy.org](http://www.haproxy.org/)，源码可以直接从 [https://github.com/haproxy/haproxy](https://github.com/haproxy/haproxy) 下载，很多帮助文档可以直接查看源码中的 doc 目录。

关于帮助文档也可以查看 [haproxy-dconv](http://cbonte.github.io/haproxy-dconv/)，另外有个监控工具 [HATop](http://feurix.org/projects/hatop/) 。

[MySQL Load Balancing with HAProxy - Tutorial](http://severalnines.com/tutorials/mysql-load-balancing-haproxy-tutorial) 同时包含了一个不错的终端录制工具 [asciinema.org](https://asciinema.org/) 。


<!--
http://jaminzhang.github.io/lb/HAProxy-Get-Started/
-->


{% highlight text %}
{% endhighlight %}
