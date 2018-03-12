---
title: Nginx 入门
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,简介
description: Nginx (发音 "Engine X") 是一款轻量级且高性能的 Web 服务器、反向代理服务器，同时也是一个 IMAP POP3 SMTP 服务器，其代码完全通过 C 语言编写，支持多平台，并且在 BSD-like 协议下发行。是由俄罗斯的 Igor Sysoev 开发，其特点是占有内存少、并发能力强、丰富的功能集和示例配置文件。在此仅简单介绍其安装使用方法。
---

Nginx (发音 "Engine X") 是一款轻量级且高性能的 Web 服务器、反向代理服务器，同时也是一个 IMAP POP3 SMTP 服务器，完全通过 C 语言编写，支持多平台，并且在 BSD-like 协议下发行。

是由俄罗斯的 Igor Sysoev 开发，其特点是占有内存少、并发能力强、丰富的功能集和示例配置文件。在此仅简单介绍其安装使用方法。

<!-- more -->

![nginx logo](/images/linux/nginx-logo.png "nginx logo"){: .pull-center width='70%'}

## 简介

很多操作可以直接参考官方网站的文档 [www.nginx.org](http://nginx.org/en/docs/) ，在此仅列举其中比较常用的一些操作，使用的操作系统是 CentOS 。

{% highlight text %}
----- 在CentOS中直接通过yum安装
# yum install nginx --enablerepo=epel

----- 启动，然后可以通过127.1直接访问，也可以通过enable设置开机启动
# systemctl start nginx.service
{% endhighlight %}

仅介绍一下安装 Nginx 之后的默认配置选项：默认的服务器根目录是 ```/usr/share/nginx/html```，配置文件在 ```/etc/nginx``` 目录下，默认是 ```/etc/nginx/nginx.conf```，日志默认在 ```/var/log/nginx``` 目录下。

### 常用操作

如下是经常会使用的命令。

{% highlight text %}
----- 查看相关进程的资源
# ps axw -o pid,ppid,user,%cpu,vsz,wchan,command | egrep 'nginx:'

----- 其它的一些常见的命令
# kill -QUIT `cat /run/nginx.pid`        ← 从容停止
# kill -TERM|INT `cat /run/nginx.pid`    ← 快速停止
# kill -USR1 `cat /run/nginx.pid`        ← 重新打开一个日志文件
# kill -USR2 `cat /run/nginx.pid`        ← 升级可执行文件
# pkill -9 nginx                         ← 强制停止

# nginx -t -c /etc/nginx/nginx.conf      ← 检查配置文件的语法
# kill -HUP `cat /run/nginx.pid`         ← 重新加载配置文件
# nginx -s reload                        ← 同上
{% endhighlight %}

另外，为了防止由于安全配置导致报错，可通过 ```setenforce 0``` 关闭 SeLinux，之前被坑过 ^_^

现在可以直接通过浏览器打开 [http://localhost/](http://localhost/) 即可。当然，如果发现不符合期望的，可以直接查看默认的错误日志 ```/var/log/nginx/error.log``` 。


### 源码安装

如下是，如何通过源码安装、配置 nginx 。

#### 1. 安装头文件

安装时依赖 pcre、openssl、gzip 三个库，在 CentOS 中，可以通过如下命令查看以及安装。

{% highlight text %}
# rpm -qa | grep -E '(zlib|pcre|openssl)'
# yum install zlib zlib-devel pcre pcre-devel openssl openssl-devel
# yum install libxslt libxslt-devel
# yum install perl-ExtUtils-Embed
{% endhighlight %}

其中各个库的作用如下：

* pcre: 用来作地址重写的功能；
* zlib：用于传输数据打包，省流量，但消耗资源；
* openssl：提供ssl加密协议；
* xslt：用于过滤转换XML请求；
* gd: 用于JPEG/GIF/PNG图片的一个过滤器

#### 2. 添加用户

一般启动时会使用 nginx 用户，所以通过如下方式添加相应的用户、用户组。

{% highlight text %}
# groupadd nginx
# useradd -r -g nginx -s /sbin/nologin -M nginx
{% endhighlight %}

其中 -r 指定系统用户；-g 指定用户组；-s 指定 shell；-M 不创建 home 。

#### 3. 下载编译

直接从官网 [www.nginx.org](http://nginx.org/) 下载源码，然后通过如下方式编译、安装。

<!--
内容有些多，一眼看来难免头昏脑胀，但坚持看完，相信你一定会有所收获。
nginx参数：
--prefix= 指向安装目录
--sbin-path 指向（执行）程序文件（nginx）
--conf-path= 指向配置文件（nginx.conf）
--error-log-path= 指向错误日志目录
--pid-path= 指向pid文件（nginx.pid）
--lock-path= 指向lock文件（nginx.lock）（安装文件锁定，防止安装文件被别人利用，或自己误操作。）
--user= 指定程序运行时的非特权用户
--group= 指定程序运行时的非特权用户组
--builddir= 指向编译目录
--with-rtsig_module 启用rtsig模块支持（实时信号）
--with-select_module 启用select模块支持（一种轮询模式,不推荐在高载环境下使用）禁用：--without-select_module
--with-poll_module 启用poll模块支持（功能与select相同，与select特性相同，为一种轮询模式,不推荐在高载环境下使用）
--with-file-aio 启用file aio支持（一种APL文件传输格式）
--with-ipv6 启用ipv6支持
--with-http_ssl_module 启用ngx_http_ssl_module支持（使支持https请求，需已安装openssl）
--with-http_realip_module 启用ngx_http_realip_module支持（这个模块允许从请求标头更改客户端的IP地址值，默认为关）
--with-http_addition_module 启用ngx_http_addition_module支持（作为一个输出过滤器，支持不完全缓冲，分部分响应请求）
--with-http_geoip_module 启用ngx_http_geoip_module支持（该模块创建基于与MaxMind GeoIP二进制文件相配的客户端IP地址的ngx_http_geoip_module变量）
--with-http_sub_module 启用ngx_http_sub_module支持（允许用一些其他文本替换nginx响应中的一些文本）
--with-http_dav_module 启用ngx_http_dav_module支持（增加PUT,DELETE,MKCOL：创建集合,COPY和MOVE方法）默认情况下为关闭，需编译开启
--with-http_flv_module 启用ngx_http_flv_module支持（提供寻求内存使用基于时间的偏移量文件）
--with-http_gzip_static_module 启用ngx_http_gzip_static_module支持（在线实时压缩输出数据流）
--with-http_random_index_module 启用ngx_http_random_index_module支持（从目录中随机挑选一个目录索引）
--with-http_secure_link_module 启用ngx_http_secure_link_module支持（计算和检查要求所需的安全链接网址）
--with-http_degradation_module  启用ngx_http_degradation_module支持（允许在内存不足的情况下返回204或444码）
--with-http_stub_status_module 启用ngx_http_stub_status_module支持（获取nginx自上次启动以来的工作状态）
--without-http_charset_module 禁用ngx_http_charset_module支持（重新编码web页面，但只能是一个方向--服务器端到客户端，并且只有一个字节的编码可以被重新编码）
--without-http_gzip_module 禁用ngx_http_gzip_module支持（该模块同-with-http_gzip_static_module功能一样）
--without-http_ssi_module 禁用ngx_http_ssi_module支持（该模块提供了一个在输入端处理处理服务器包含文件（SSI）的过滤器，目前支持SSI命令的列表是不完整的）
--without-http_userid_module 禁用ngx_http_userid_module支持（该模块用来处理用来确定客户端后续请求的cookies）
--without-http_access_module 禁用ngx_http_access_module支持（该模块提供了一个简单的基于主机的访问控制。允许/拒绝基于ip地址）
--without-http_auth_basic_module禁用ngx_http_auth_basic_module（该模块是可以使用用户名和密码基于http基本认证方法来保护你的站点或其部分内容）
--without-http_autoindex_module 禁用disable ngx_http_autoindex_module支持（该模块用于自动生成目录列表，只在ngx_http_index_module模块未找到索引文件时发出请求。）
--without-http_geo_module 禁用ngx_http_geo_module支持（创建一些变量，其值依赖于客户端的IP地址）
--without-http_map_module 禁用ngx_http_map_module支持（使用任意的键/值对设置配置变量）
--without-http_split_clients_module 禁用ngx_http_split_clients_module支持（该模块用来基于某些条件划分用户。条件如：ip地址、报头、cookies等等）
--without-http_referer_module 禁用disable ngx_http_referer_module支持（该模块用来过滤请求，拒绝报头中Referer值不正确的请求）
--without-http_rewrite_module 禁用ngx_http_rewrite_module支持（该模块允许使用正则表达式改变URI，并且根据变量来转向以及选择配置。如果在server级别设置该选项，那么他们将在 location之前生效。如果在location还有更进一步的重写规则，location部分的规则依然会被执行。如果这个URI重写是因为location部分的规则造成的，那么 location部分会再次被执行作为新的URI。 这个循环会执行10次，然后Nginx会返回一个500错误。）
--without-http_proxy_module 禁用ngx_http_proxy_module支持（有关代理服务器）
--without-http_fastcgi_module 禁用ngx_http_fastcgi_module支持（该模块允许Nginx 与FastCGI 进程交互，并通过传递参数来控制FastCGI 进程工作。 ）FastCGI一个常驻型的公共网关接口。
--without-http_uwsgi_module 禁用ngx_http_uwsgi_module支持（该模块用来医用uwsgi协议，uWSGI服务器相关）
--without-http_scgi_module 禁用ngx_http_scgi_module支持（该模块用来启用SCGI协议支持，SCGI协议是CGI协议的替代。它是一种应用程序与HTTP服务接口标准。它有些像FastCGI但他的设计 更容易实现。）
--without-http_memcached_module 禁用ngx_http_memcached_module支持（该模块用来提供简单的缓存，以提高系统效率）
-without-http_limit_zone_module 禁用ngx_http_limit_zone_module支持（该模块可以针对条件，进行会话的并发连接数控制）
--without-http_limit_req_module 禁用ngx_http_limit_req_module支持（该模块允许你对于一个地址进行请求数量的限制用一个给定的session或一个特定的事件）
--without-http_empty_gif_module 禁用ngx_http_empty_gif_module支持（该模块在内存中常驻了一个1*1的透明GIF图像，可以被非常快速的调用）
--without-http_browser_module 禁用ngx_http_browser_module支持（该模块用来创建依赖于请求报头的值。如果浏览器为modern ，则$modern_browser等于modern_browser_value指令分配的值；如 果浏览器为old，则$ancient_browser等于 ancient_browser_value指令分配的值；如果浏览器为 MSIE中的任意版本，则 $msie等于1）
--without-http_upstream_ip_hash_module 禁用ngx_http_upstream_ip_hash_module支持（该模块用于简单的负载均衡）
--with-http_perl_module 启用ngx_http_perl_module支持（该模块使nginx可以直接使用perl或通过ssi调用perl）
--with-perl_modules_path= 设定perl模块路径
--with-perl= 设定perl库文件路径
--http-log-path= 设定access log路径
--http-client-body-temp-path= 设定http客户端请求临时文件路径
--http-proxy-temp-path= 设定http代理临时文件路径
--http-fastcgi-temp-path= 设定http fastcgi临时文件路径
--http-uwsgi-temp-path= 设定http uwsgi临时文件路径
--http-scgi-temp-path= 设定http scgi临时文件路径
-without-http 禁用http server功能
--without-http-cache 禁用http cache功能
--with-mail 启用POP3/IMAP4/SMTP代理模块支持
--with-mail_ssl_module 启用ngx_mail_ssl_module支持
--without-mail_pop3_module 禁用pop3协议（POP3即邮局协议的第3个版本,它是规定个人计算机如何连接到互联网上的邮件服务器进行收发邮件的协议。是因特网电子邮件的第一个离线协议标 准,POP3协议允许用户从服务器上把邮件存储到本地主机上,同时根据客户端的操作删除或保存在邮件服务器上的邮件。POP3协议是TCP/IP协议族中的一员，主要用于 支持使用客户端远程管理在服务器上的电子邮件）
--without-mail_imap_module 禁用imap协议（一种邮件获取协议。它的主要作用是邮件客户端可以通过这种协议从邮件服务器上获取邮件的信息，下载邮件等。IMAP协议运行在TCP/IP协议之上， 使用的端口是143。它与POP3协议的主要区别是用户可以不用把所有的邮件全部下载，可以通过客户端直接对服务器上的邮件进行操作。）
--without-mail_smtp_module 禁用smtp协议（SMTP即简单邮件传输协议,它是一组用于由源地址到目的地址传送邮件的规则，由它来控制信件的中转方式。SMTP协议属于TCP/IP协议族，它帮助每台计算机在发送或中转信件时找到下一个目的地。）
--with-google_perftools_module 启用ngx_google_perftools_module支持（调试用，剖析程序性能瓶颈）
--with-cpp_test_module 启用ngx_cpp_test_module支持
--add-module= 启用外部模块支持
--with-cc= 指向C编译器路径
--with-cpp= 指向C预处理路径
--with-cc-opt= 设置C编译器参数（PCRE库，需要指定–with-cc-opt=”-I /usr/local/include”，如果使用select()函数则需要同时增加文件描述符数量，可以通过–with-cc- opt=”-D FD_SETSIZE=2048”指定。）
--with-ld-opt= 设置连接文件参数。（PCRE库，需要指定–with-ld-opt=”-L /usr/local/lib”。）
--with-cpu-opt= 指定编译的CPU，可用的值为: pentium, pentiumpro, pentium3, pentium4, athlon, opteron, amd64, sparc32, sparc64, ppc64
--without-pcre 禁用pcre库
--with-pcre 启用pcre库
--with-pcre= 指向pcre库文件目录
--with-pcre-opt= 在编译时为pcre库设置附加参数
--with-md5= 指向md5库文件目录（消息摘要算法第五版，用以提供消息的完整性保护）
--with-md5-opt= 在编译时为md5库设置附加参数
--with-md5-asm 使用md5汇编源
--with-sha1= 指向sha1库目录（数字签名算法，主要用于数字签名）
--with-sha1-opt= 在编译时为sha1库设置附加参数
--with-sha1-asm 使用sha1汇编源
--with-zlib= 指向zlib库目录
--with-zlib-opt= 在编译时为zlib设置附加参数
--with-zlib-asm= 为指定的CPU使用zlib汇编源进行优化，CPU类型为pentium, pentiumpro
--with-libatomic 为原子内存的更新操作的实现提供一个架构
--with-libatomic= 指向libatomic_ops安装目录
--with-openssl= 指向openssl安装目录
--with-openssl-opt 在编译时为openssl设置附加参数
--with-debug 启用debug日志







make是用来编译的，它从Makefile中读取指令，然后编译。

make install是用来安装的，它也从Makefile中读取指令，安装到指定的位置。

configure命令是用来检测你的安装平台的目标特征的。它定义了系统的各个方面，包括nginx的被允许使用的连接处理的方法，比如它会检测你是不是有CC或GCC，并不是需要CC或GCC，它是个shell脚本，执行结束时，它会创建一个Makefile文件。nginx的configure命令支持以下参数：

--with-select_module --without-select_module 启用或禁用构建一个模块来允许服务器使用select()方法。该模块将自动建立，如果平台不支持的kqueue，epoll，rtsig或/dev/poll。
--with-poll_module --without-poll_module 启用或禁用构建一个模块来允许服务器使用poll()方法。该模块将自动建立，如果平台不支持的kqueue，epoll，rtsig或/dev/poll。
--without-http_gzip_module — 不编译压缩的HTTP服务器的响应模块。编译并运行此模块需要zlib库。
--without-http_rewrite_module 不编译重写模块。编译并运行此模块需要PCRE库支持。
--without-http_proxy_module — 不编译http_proxy模块。
--with-pcre=path — 设置PCRE库的源码路径。PCRE库的源码（版本4.4 - 8.30）需要从PCRE网站下载并解压。其余的工作是Nginx的./ configure和make来完成。正则表达式使用在location指令和 ngx_http_rewrite_module 模块中。
--with-pcre-jit —编译PCRE包含“just-in-time compilation”（1.1.12中， pcre_jit指令）。
--with-zlib=path —设置的zlib库的源码路径。要下载从 zlib（版本1.1.3 - 1.2.5）的并解压。其余的工作是Nginx的./ configure和make完成。ngx_http_gzip_module模块需要使用zlib 。
--with-cc-opt=parameters — 设置额外的参数将被添加到CFLAGS变量。例如,当你在FreeBSD上使用PCRE库时需要使用:--with-cc-opt="-I /usr/local/include。.如需要需要增加 select()支持的文件数量:--with-cc-opt="-D FD_SETSIZE=2048".
--with-ld-opt=parameters —设置附加的参数，将用于在链接期间。例如，当在FreeBSD下使用该系统的PCRE库,应指定:--with-ld-opt="-L /usr/local/lib".

典型实例(下面为了展示需要写在多行，执行时内容需要在同一行)

./configure
--sbin-path=/usr/local/nginx/nginx
--conf-path=/usr/local/nginx/nginx.conf
--pid-path=/usr/local/nginx/nginx.pid
--with-http_ssl_module
--with-pcre=../pcre-4.4
--with-zlib=../zlib-1.1.3

$ ./configure --prefix=/usr --sbin-path=/usr/sbin/nginx --conf-path=/etc/nginx/nginx.conf \
    --error-log-path=/var/log/nginx/error.log --pid-path=/var/run/nginx.pid \
    --user=nginx --group=nginx \
    --with-http_ssl_module \
    --with-http_flv_module \
    --with-http_gzip_static_module \
    --http-log-path=/var/log/nginx/access.log \
    --http-client-body-temp-path=/var/tmp/nginx/client \
    --http-proxy-temp-path=/var/tmp/nginx/proxy \
    --http-fastcgi-temp-path=/var/tmp/nginx/fcgi \
    --with-http_stub_status_module
-->

{% highlight text %}
$ ./configure      \
    --prefix=/usr/share/nginx \                   ← 安装目录，存放服务器文件
    --sbin-path=/usr/sbin/nginx \                 ← 可执行文件路径
    --conf-path=/etc/nginx/nginx.conf \           ← 默认配置文件
    --error-log-path=/var/log/nginx/error.log \   ← 报错文件，通过error_log修改
    --http-log-path=/var/log/nginx/access.log \   ← 日志文件，通过access_log修改
    --pid-path=/run/nginx.pid \                   ← 存储主进程号，通过pid修改
    --user=nginx \                                ← 进程工作用户，通过user指定
    --group=nginx \                               ← 用户组，通过user指定

    --lock-path=/run/lock/subsys/nginx  \
    --http-client-body-temp-path=/var/lib/nginx/tmp/client_body \
    --http-proxy-temp-path=/var/lib/nginx/tmp/proxy \
    --http-fastcgi-temp-path=/var/lib/nginx/tmp/fastcgi \
    --http-uwsgi-temp-path=/var/lib/nginx/tmp/uwsgi \
    --http-scgi-temp-path=/var/lib/nginx/tmp/scgi  \

    --with-file-aio  \                            ← 依赖libaio库
    --with-ipv6  \                                ← 支持IPV6
    --with-http_ssl_module  \                     ← 使用https协议模块
    --with-http_spdy_module  \                    ← Google的SPDY协议支持

    --with-pcre \                                 ← 支持地址重写
    --with-pcre-jit \
    --with-google_perftools_module \              ← 依赖Google Perf工具，没有研究过
    --with-debug \                                ← 添加调试日志

    --add-module=/path/to/echo-nginx-module       ← 编译添加三方模块

    --with-ld-opt='-Wl,-E'
{% endhighlight %}

<!--
    --with-http_realip_module \
    --with-http_addition_module \
    --with-http_xslt_module \                     # 过滤转换XML请求，需要libxslt
    --with-http_image_filter_module \             # 传输jpeg/gif/png图片的过滤器，依赖gd库
    --with-http_geoip_module  \                   # 依赖GeoIP，没有使用过
    --with-http_sub_module  \
    --with-http_dav_module  \
    --with-http_flv_module  \
    --with-http_mp4_module  \
    --with-http_gunzip_module  \
    --with-http_gzip_static_module \
    --with-http_random_index_module \
    --with-http_secure_link_module \
    --with-http_degradation_module \
    --with-http_stub_status_module \
    --with-http_perl_module \
    --with-mail \
    --with-mail_ssl_module \
    --with-pcre \
    --with-pcre-jit \
    --with-google_perftools_module \     # 依赖Google Perf工具，没有研究过
    --with-debug \

    --with-cc-opt='-O2 -g -pipe -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m64 -mtune=generic' \
-->

<!--
CentOS7 编译选项
nginx version: nginx/1.8.0
built by gcc 4.8.5 20150623 (Red Hat 4.8.5-4) (GCC)
built with OpenSSL 1.0.1e-fips 11 Feb 2013
TLS SNI support enabled
configure arguments: --prefix=/usr/share/nginx --sbin-path=/usr/sbin/nginx --conf-path=/etc/nginx/nginx.conf --error-log-path=/var/log/nginx/error.log --http-log-path=/var/log/nginx/access.log --pid-path=/run/nginx.pid --user=nginx --group=nginx --lock-path=/run/lock/subsys/nginx --http-client-body-temp-path=/var/lib/nginx/tmp/client_body --http-proxy-temp-path=/var/lib/nginx/tmp/proxy --http-fastcgi-temp-path=/var/lib/nginx/tmp/fastcgi --http-uwsgi-temp-path=/var/lib/nginx/tmp/uwsgi --http-scgi-temp-path=/var/lib/nginx/tmp/scgi --with-file-aio --with-ipv6 --with-http_ssl_module --with-http_spdy_module --with-pcre --with-pcre-jit --with-debug --add-module=/home/andy/Workspace/webserver/nginx/modules/echo-nginx-module-0.60 --with-cc-opt='-O2 -g -pipe -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -m64 -mtune=generic' --with-ld-opt=-Wl,-E
-->

nginx 没有通过 autotools 进行编译，而是通过自己的 shell 脚本实现，其中相应的脚本都在 auto 目录下，其中编译生成的文件都保存在 objs 目录下。

编译完成之后，可以通过如下命令安装即可。

{% highlight text %}
# make install
{% endhighlight %}


#### 4. 启动测试

源码编译完成之后，可以直接通过如下方式启动。

{% highlight text %}
# /usr/sbin/nginx
{% endhighlight %}


#### 5. systemd 支持

对于新一代的 init 支持，需要添加如下文件。

{% highlight text %}
$ cat /usr/lib/systemd/system/nginx.service
[Unit]
Description=The nginx HTTP and reverse proxy server
After=network.target remote-fs.target nss-lookup.target

[Service]
Type=forking
PIDFile=/run/nginx.pid
# Nginx will fail to start if /run/nginx.pid already exists but has the wrong
# SELinux context. This might happen when running `nginx -t` from the cmdline.
# https://bugzilla.redhat.com/show_bug.cgi?id=1268621
ExecStartPre=/usr/bin/rm -f /run/nginx.pid
ExecStartPre=/usr/sbin/nginx -t
ExecStart=/usr/sbin/nginx
ExecReload=/bin/kill -s HUP $MAINPID
KillSignal=SIGQUIT
TimeoutStopSec=5
KillMode=process
PrivateTmp=true

[Install]
WantedBy=multi-user.target
{% endhighlight %}

然后就可以通过如下方式进行控制了。

{% highlight text %}
$ systemctl [start|reload|restart|stop] nginx.service
$ journalctl -f -u nginx.service                            ← 查看日志
{% endhighlight %}



### 环境

配置完 Nginx 服务之后，实际上会在设置的 root 目录下添加符号 接，从而可以针对不同的运行环境，如 Bootstrap、echarts、php、DVWA，可以直接通过如下方式设置：

{% highlight text %}
basic:     只包括了基本的测试。
    ln -s basic html

bootstrap: 一个不错的前端框架。
    ln -s ../bootstrap/bootstrap-3.3.5-dist html

echarts:
    cd echarts-roots
    ln -s ../../echarts/echarts.min.js echarts.js
    cd ..
    ln -s echarts-root html

php-root:  PHP的安装环境
    ln -s php-root html

DVWA:      一个安全测试环境
    ln -s /home/andy/Workspace/security/dvwa/DVWA-1.9 html
{% endhighlight %}

## 变量

Nginx 配置文件的使用就像是一门微型的编程语言，既然如此，肯定不会少 “变量”，其支持内建变量，可以直接查看源码 ```http/ngx_http_variables.c``` 中 ```ngx_http_core_variables[]``` 的实现。

如下，使用了 ```ngx_rewrite``` 标准模块的 ```set``` 配置指令对变量 ```$foobar``` 进行了赋值操作，也即把字符串 hello world 赋给了它；如下需要依赖 echo 模块，详细安装方法参考下面的内容。

{% highlight text %}
location /hello {
    set $foobar "hello world";
    echo "foobar: $foobar";
}
{% endhighlight %}

然后使用类似如下的 curl 命令测试即可。

{% highlight text %}
$ curl 'http://localhost:80/hello'
{% endhighlight %}

### 内置变量

与 Apache 类似，在 core module 中支持一些内置的变量，常见的如下：

* $arg_PARAMETER<br>GET 请求的 PARAMETER 值。
* $args<br>请求行中的参数。
* $query_string<br>与 $args 相同。
* $binary_remote_addr<br>二进制码形式的客户端地址。
* $body_bytes_sent<br>发送页面的字节数。
* $content_length<br>请求头中的 Content-length 字段。
* $content_type<br>请求头中的 Content-Type 字段。
* $cookie_COOKIE<br> COOKIE 的值。
* $document_root<br>当前请求在 root 指令中指定的值。
* $document_uri<br>与 $uri 相同。
* $uri<br>请求中的当前URI，不含请求参数，参数位于$args，如 "/foo/bar.html" 。
* $request_uri<br>包含客户端请求参数的原始 URI，无法修改，可以查看 $uri 改写。
* $is_args<br>如果 $args 设置，值为 "?"，否则为 ""。
* $limit_rate<br>可以限制连接速率。
* $nginx_version<br>当前运行的 nginx 版本号。
* $remote_addr<br>客户端的 IP 地址。
* $remote_port<br>客户端的端口。
* $remote_user<br>已经经过 Auth Basic Module 验证的用户名。
* $request_filename<br>当前连接请求的文件路径，由 root 或 alias 指令与 URI 请求生成。
* $request_body<br>包含请求的主要信息，在 proxy_pass、fastcgi_pass 中比较有意义。
* $request_method<br>客户端请求的动作，通常为 GET 或 POST。
* $server_addr<br>服务器地址。
* $server_name<br>服务器名称。
* $server_port<br>请求到达服务器的端口号。
* $server_protocol<br>请求使用的协议，通常是 HTTP/1.0 或 HTTP/1.1。

## 配置文件

### server_name

根据 HTTP 请求的 header Host 选择 nginx 配置文件里符合条件的 server_name 的 server 配置，也就是说一个配置文件里可以配置多个不同域名的服务。

其匹配的顺序如下： 1) 完全匹配 (www.example.com)；2) 后缀匹配 (\*.example.com)；3) 前缀匹配 (www.example.\*)；4) 正则匹配。

若前面四项都没匹配上，则根据以下顺序：1) listen 指令里配置了 default 或 default_server 的 server；2) 第一个匹配上 listen 的 server。



{% highlight text %}
server {
    listen 8000;
    server_name www;
    location / {
        echo "first";
    }
}

server {
    listen  8000;
    server_name www.example.com;
    location / {
        echo "second";
    }
}

server {
    listen 8000;
    server_name www.example.*;
    location / {
        echo "third";
    }
}

server {
    listen 8000;
    server_name ~\w+.com;
    location / {
        echo "forth";
    }
}

server {
    listen 8000;
    server_name ~.*example.com;
    location / {
        echo "fifth";
    }
}
{% endhighlight %}

对于如上的示例，可以通过如下命令进行测试，当然为了测试需要在 /etc/hosts 配置文件中添加如下内容。

{% highlight text %}
127.1 www www.example.com www.example.org www.foobar.com example.com example.org
{% endhighlight %}

注意 fifth 实际与 forth 冲突，不会出现 fifth 的。

{% highlight text %}
$ curl http://www:8000/                 ← 全匹配
first
$ curl http://www.example.com:8000/     ← 全匹配
second
$ curl http://www.example.org:8000/     ← 前缀匹配
third
$ curl http://example.com:8000/         ← 第一个正则匹配
forth
$ curl http://www.foobar.com:8000/      ← 同上
forth
$ curl http://example.org/              ← 无匹配，返回第一个符合的listen的
first
{% endhighlight %}

如果在配置文件里再加入一个配置。

{% highlight text %}
server {
    listen 8000 default;
    server_name _;

    location / {
        echo "sixth";
    }
}
{% endhighlight %}

则访问 http://example.org/ 返回 sixth 。

### accept_mutex

简单来说，这个参数主要用来处理惊群问题，可以通过如下方式配置。

{% highlight text %}
events { 
	accept_mutex off; 
} 
{% endhighlight %}

当新连接到达时，如果激活了 `accept_mutex`，那么多个 worker 将以串行方式来处理，只有一个会被唤醒，其它的 worker 继续保持休眠状态；如果没有激活 `accept_mutex`，那么所有的 worker 都被唤醒，不过只有一个 worker 能获取新连接，其它的 worker 会重新进入休眠状态。

不过不像 Apache 会启动成百上千的进程，如果发生惊群问题的话，影响相对较大；但是对 Nginx 而言，一般来说，`worker_processes` 会设置成 CPU 个数，所以最多也就几十个，即便发生惊群问题的话，影响相对也较小。

另外，高版本的 Linux 中，accept 不存在惊群问题，但是 epoll_wait 等操作还有。

如果关闭了它，可能会引起一定程度的惊群问题，表现为上下文切换增多 (`sar -w`) 或者负载上升，但是如果网站访问量比较大，为了系统的吞吐量，建议关闭。

关闭之后一般各个工作进程的负载会更均衡，可以通过 [github ngx-req-distr](https://github.com/openresty/openresty-systemtap-toolkit#ngx-req-distr) 测试。

## 常用模块

安装完 Nginx 之后，可以通过 -V 参数，查看编译时使用的参数；很多三方的模块可以参考 [NGINX 3<sup>rd</sup> Party Modules](https://www.nginx.com/resources/wiki/modules/) ，保存了一个比较详细的列表。

对于三方模块，可以通过如下方法添加到二进制文件中。

{% highlight text %}
$ ./configure \
   --add-module=/path/to/echo-nginx-module
{% endhighlight %}

### echo

用来显示一些常见的变量，不过这个模块不包含在 Nginx 源码中，可以通过如下方法进行安装；首先从 [github openresty/echo-nginx-module](https://github.com/openresty/echo-nginx-module) 。

可以下载之后通过如下方式编译。

{% highlight text %}
$ ./configure --add-module=/path/to/echo-nginx-module
{% endhighlight %}

简单的可以通过如下方式使用，也可以从官网查看详细的使用方法。

{% highlight text %}
location /hello {
  echo "hello, world!";
}
{% endhighlight %}

然后执行如下命令测试即可。

{% highlight text %}
$ curl 'http://localhost:80/hello'
{% endhighlight %}


### ngx_http_map_module

该模块可以用来创建变量，将这些变量的值与其它变量相关联，允许分类或者同时映射多个值到多个不同值并储存到一个变量中，而且仅在变量被接受的时候执行视图映射操作，对于处理没有引用变量的请求时，这个模块并没有性能上的缺失。

详细可以参考 Nginx 官方文档 [Module ngx_http_map_module](http://nginx.org/en/docs/http/ngx_http_map_module.html) 。

### auth_basic_module

关于该模块的内容详细可以查看 [Nginx 基本认证模块](http://nginx.org/en/docs/http/ngx_http_auth_basic_module.html) ，使用简单介绍如下。

需要保证已经安装了 ```ngx_http_auth_basic_module``` 模块，同样可以通过 ```nginx -V``` 查看，如果不需要可以使用 ```--without-http_auth_basic_module``` 。

{% highlight text %}
location / {
    auth_basic           "closed site";
    auth_basic_user_file conf/htpasswd;
}
{% endhighlight %}

其中 ```auth_basic``` 的字符串内容可以任意，例如 ```Restricted```、```NginxStatus``` 等；另外，需要注意的是 ```auth_basic_user_file``` 的路径，否则会出现 403 错误。

其中密码文件的格式内容如下，其中密码通过 ```crypt()``` 函数加密，以及 Apache 基于 MD5 的变种 Hash 算法 (apr1) 生成，可以通过 Apache 中的 ```httpd-tools``` 中的 ```htpasswd``` 加密，或者使用 ```openssl passwd``` 获取。

{% highlight text %}
# comment
name1:password1
name2:password2:comment
name3:password3
{% endhighlight %}

可以通过如下命令生成密码。

{% highlight text %}
$ sh -c "openssl passwd -apr1 >> /etc/nginx/.htpasswd"
$ printf "ttlsa:$(openssl passwd -crypt 123456)\n" >>conf/htpasswd
$ htpasswd -c conf/htpasswd username
{% endhighlight %}

另外，可以查看 Python 版本 [github htpasswd.py]( {{ site.example_repository }}/python/misc/htpasswd.py )，使用方法如下。

{% highlight text %}
$ htpasswd.py -b -c pass.txt admin 123456
{% endhighlight %}

<!--
http://coderschool.cn/2207.html
http://trac.edgewall.org/export/10890/trunk/contrib/htpasswd.py
-->




## 其它

记录一些常见的问题，以及解决方法。

### 管理脚本

一个 Bash 管理脚本。

{% highlight bash %}
#!/bin/bash
# chkconfig: - 30 21
# description: http service.
# Source Function Library
. /etc/init.d/functions
# Nginx Settings
#the shell is used as a tool that start, stop, restart the servie nginx

NGINX_SBIN="/usr/local/nginx/sbin/nginx"
NGINX_CONF="/usr/local/nginx/conf/nginx.conf"
NGINX_PID="/usr/local/nginx/logs/nginx.pid"
RETVAL=0
prog="Nginx"

start() {
        echo -n $"Starting $prog: "
        mkdir -p /dev/shm/nginx_temp
        daemon $NGINX_SBIN -c $NGINX_CONF
        RETVAL=$?
        echo
        return $RETVAL
}

stop() {
        echo -n $"Stopping $prog: "
        killproc -p $NGINX_PID $NGINX_SBIN -TERM
        rm -rf /dev/shm/nginx_temp
        RETVAL=$?
        echo
        return $RETVAL
}

reload(){
        echo -n $"Reloading $prog: "
        killproc -p $NGINX_PID $NGINX_SBIN -HUP
        RETVAL=$?
        echo
        return $RETVAL
}

restart(){
        stop
        start
}

configtest(){
    $NGINX_SBIN -c $NGINX_CONF -t
    return 0
}

case "$1" in
  start)
        start
        ;;
  stop)
        stop
        ;;
  reload)
        reload
        ;;
  restart)
        restart
        ;;
  configtest)
        configtest
        ;;
  *)
        echo $"Usage: $0 {start|stop|reload|restart|configtest}"
        RETVAL=1
esac

exit $RETVAL
{% endhighlight %}

### 监控 nginx 的工作状态

监控需要在编译时添加 ```--with-http_stub_status_module``` 选项，默认不会包含在内；然后在配置文件中添加如下内容。

{% highlight text %}
location /status {
    stub_status on;
    access_log off;
    allow 127.0.0.1;
    deny all;
}
{% endhighlight %}

修改完之后重新加载，然后可以通过 http://localhost/status 查看，此时会显示如下信息。

{% highlight text %}
Active connections: 557             # 对后端发起的活动连接数
server accepts handled requests     # 总共处理的链接数；成功握手次数；总共处理的请求
35 35 65
# 读取到客户端Header信息数；返回给客户端的Header信息数；等待下次请求的驻留连接
Reading: 3 Writing: 16 Waiting: 8
{% endhighlight %}

注意，对于最后一行的 Waiting，当开启 keep-alive 的情况下，这个值等于 active - (reading + writing)，意思就是说已经处理完正在等候下一次请求指令的驻留连接。

### 启动报错

记录下遇到的一些常见问题。

#### Can't load XXX

例如报错为 ```Can't load '/usr/local/lib64/perl5/auto/nginx/nginx.so'```，该报错是在使用 perl 模块时可能引入的报错，也就是编译时使用 ```--with-http_perl_module``` 参数。

{% highlight text %}
# /usr/sbin/nginx
Can't load '/usr/local/lib64/perl5/auto/nginx/nginx.so' for module nginx:
    /usr/local/lib64/perl5/auto/nginx/nginx.so: undefined symbol:
    ngx_http_perl_handle_request at /usr/share/perl5/XSLoader.pm line 68.
at /usr/local/lib64/perl5/nginx.pm line 56.
Compilation failed in require.
BEGIN failed–compilation aborted.
nginx: [alert] perl_parse() failed: 255
{% endhighlight %}

此时，需要在编译的时候添加一个连接选项。

{% highlight text %}
$ ./configure --with-http_perl_module --with-ld-opt="-Wl,-E"
{% endhighlight %}

然后，重新安装即可。


### 403 Forbidden

可以通过查看 error 日志，通常是由于权限不足导致，最简单的方式可以尝试在配置文件中设置 user root;，也就是 nginx 的子进程采用 root 用户。

当然，通常也可以使用 ```chown nginx:nginx -R /var/www``` 解决。

<!--
如果 root 在用户目录下，可以通过如下命令允许 nginx 访问用户文件，否则会报 403 Forbidden 错误。

{% highlight text %}
# setsebool -P httpd_read_user_content 1
# setsebool -P httpd_enable_homedirs 1
{% endhighlight %}
-->



<!--
## SSL 支持

### SPDY

普通的 https 慢而且又吃资源，Google 发明的 SPDY 协议。

{% highlight text %}
listen [::]:443 ssl;
listen 443 ssl;

listen [::]:443 ssl spdy;
listen 443 ssl spdy;
{% endhighlight %}
-->



## 参考

可以参考官方文档 [nginx.org docs](http://nginx.org/en/docs/) 。

<!--
NGINX引入线程池 性能提升9倍
http://www.infoq.com/cn/articles/thread-pools-boost-performance-9x
-->




{% highlight text %}
{% endhighlight %}
