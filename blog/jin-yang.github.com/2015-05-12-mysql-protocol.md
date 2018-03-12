---
title: MySQL 通讯协议
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,通讯协议
description: Client/Server 通讯协议用于客户端链接、代理、主备复制等，支持 SSL、压缩，在链接阶段进行认证，在执行命令时可以支持 Prepared Statements 以及 Stored Procedures 。当打算编写数据库代理、数据库中间件、对 MySQL 数据包进行审核时，都需要了解底层的通信协议。在本文中，主要介绍 MySQL 通讯协议相关的内容。
---

Client/Server 通讯协议用于客户端链接、代理、主备复制等，支持 SSL、压缩，在链接阶段进行认证，在执行命令时可以支持 Prepared Statements 以及 Stored Procedures 。

当打算编写数据库代理、中间件、对 MySQL 数据包进行审核时，都需要了解底层的通信协议。在本文中，主要介绍 MySQL 通讯协议相关的内容。

<!-- more -->

## 简介

服务器启动后，会使用 TCP 监听一个本地端口，当客户端的连接请求到达时，就会执行三段握手以及 MySQL 的权限验证；验证成功后，客户端开始发送请求，服务器会以响应的报文格式返回数据；当客户端发送完成后，会发送一个特殊的报文，告知服务器已结束会话。

MySQL 定义了几种包类型，A) 客户端->服务器，登录时的 auth 包、执行 SQL 时的 CMD 包；B) 服务器->客户端，登录时的握手包、数据包、数据流结束包、成功包(OK Packet)、错误信息包。

协议定义了基本的数据类型，如 int、string 等；数据的传送格式等。

### 协议

MySQL 的客户端与服务器之间支持多种通讯方式，最广泛使用的是 TCP 通讯；另外，还支持命名管道和共享内存，而 TCP 就是最通用的一种方式，在此仅介绍 TCP 方式。

在 C/S 之间，实际采用的是一种类似半双工式的模式收发数据，即在一个 TCP 链路上，客户端发出请求数据后，只有在接收完所有的服务端响应数据以后才能发下一次请求，中间不能发其它数据，需要有很强的顺序性要求。

MySQL 客户端与服务器的交互主要分为两个阶段，分别为握手认证阶段和命令执行阶段，详细来说一次正常的过程如下：

{% highlight text %}
1. 三次握手建立 TCP 连接。

2. 建立 MySQL 连接，也就是认证阶段。
    服务端 -> 客户端：发送握手初始化包 (Handshake Initialization Packet)。
    客户端 -> 服务端：发送验证包 (Client Authentication Packet)。
    服务端 -> 客户端：认证结果消息。

3. 认证通过之后，客户端开始与服务端之间交互，也就是命令执行阶段。
    客户端 -> 服务端：发送命令包 (Command Packet)。
    服务端 -> 客户端：发送回应包 (OK Packet, or Error Packet, or Result Set Packet)。

4. 断开 MySQL 连接。
    客户端 -> 服务器：发送退出命令包。

5. 四次握手断开 TCP 连接。
{% endhighlight %}

从服务器发往客户端的数据包有四种：数据包、数据结束包、成功报告包以及错误消息包。Result Set Packet 首先发送包头+列包+EOF包+行包+EOF包。

<!--
TCP 连接时采用三次握手连接，主动断开时同样采用的是三次握手，而非四次握手；数据的发送过程总会有掺杂着一些 TCP 包，不知道做什么用的。
-->

### 报文格式

所有的包有统一的格式，并通过函数 my_net_write()@sql/net_serv.cc 写入 buffer 等待发送。

{% highlight text %}
+-------------------+--------------+---------------------------------------------------+
|      3 Bytes      |    1 Byte    |                   N Bytes                         |
+-------------------+--------------+---------------------------------------------------+
|<= length of msg =>|<= sequence =>|<==================== data =======================>|
|<============= header ===========>|<==================== body =======================>|
{% endhighlight %}

MySQL 报文格式如上，消息头包含了 A) 报文长度，标记当前请求的实际数据长度，以字节为单位；B) 序号，为了保证交互时报文的顺序，每次客户端发起请求时，序号值都会从 0 开始计算。

消息体用于存放报文的具体数据，长度由消息头中的长度值决定。

> 单个报文的最大长度为 (2^24-1)Bytes ，也即 (16M-1)Bytes，对于包长为 (2^24-1)Bytes 也会拆为两个包发送。这是因为最初没有考虑 16M 的限制，从而没有预留任何字段来标志这个包的数据不完整，所以只好把长度为 (2^24-1) 的包视做不完整的包，直到后面收到一个长度小于 (2^24-1) 的包，然后拼起来。
>
> 这也意味着最后一个包的长度有可能是 0。

### 基本类型

接下来介绍一下报文中的数据类型，也就是不同的数据类型在报文中的表现形式。

#### 整型值

MySQL 报文中整型值分别有 1、2、3、4、8 字节长度，使用小字节序传输。

#### 二进制数据

也就是 Length Coded Binary，其数据长度不固定，长度值由数据前的 1-9 个字节决定，其中长度值所占的字节数不定，字节数由第 1 个字节决定，如下：

{% highlight text %}
第一个字节值    后续字节数  长度值说明
    0-250            0   第一个字节值即为数据的真实长度
      251            0   空数据，数据的真实长度为零
      252            2   后续额外2个字节标识了数据的真实长度
      253            3   后续额外3个字节标识了数据的真实长度
      254            8   后续额外8个字节标识了数据的真实长度
{% endhighlight %}

#### 字符串

根据是否以 NULL 结尾，分为了有两种形式：

* 以 NULL 结尾，Null-Terminated String<br>字符串长度不固定，当遇到 'NULL'(0x00) 字符时结束。

* 长度编码，Length Coded String<br>字符串长度不固定，无 'NULL'(0x00) 结束符，编码方式与上面的二进制数据相同。

### 客户端请求报文

也就是从客户端发送到服务端的请求命令。

{% highlight text %}
+-------------------+------------------------------------------------------------------+
|      1 Bytes      |                             N Bytes                              |
+-------------------+------------------------------------------------------------------+
|<==== command ====>|<============================ arguments =========================>|
{% endhighlight %}

客户端向服务端发送的请求，其中第一个字节用于标识当前请求消息的类型，这也就定义了请求的种类，其中包括了：切换数据库 COM_INIT_DB(0x02)、查询命令 COM_QUERY(0x03) 等。

命令的宏定义在 include/mysql_com.h 文件中，该命令会在 dispatch_command() 中根据不同的命令进入不同代码处理逻辑。

报文中的参数内容是用户在 MySQL 客户端输入的命令，不包括每行命令结尾的 ';' 分号，采用的是非 NULL 结尾的字符串表示方法。

例如：当在 MySQL 客户端中执行 use mysql; 命令时，发送的请求报文数据会是下面的样子：
{% highlight text %}
0x02 0x6d 0x79 0x73 0x71 0x6c
{% endhighlight %}

其中，0x02 为请求类型值 COM_INIT_DB，后面的 0x6d 0x79 0x73 0x71 0x6c 为 ASCII 字符 mysql 。

### 错误码

也就是当发生了错误之后，服务端发送给客户端的报文。

MySQL 的错误包含了三部分：A) MySQL 特定的错误码，数字类型，不通用；B) SQLSTATE，为 5 个字符的字符串，采用 ANSI SQL 和 ODBC 的标准；C) 错误信息。

对于错误报文的格式可以参照参考文件，其中第二字节表示由 MySQL 定义的错误编码，服务器状态实际是 ANSI SQL 对应的编码，两者并非一一对应。

在 MySQL 中可以通过 perror ERROR 查看；详细的文档，可以参考官方文档 [Appendix B Errors, Error Codes, and Common Problems](http://dev.mysql.com/doc/refman/en/error-handling.html) 。

### 抓包分析

可以通过 tcpdump 捕获包并保存在文件中，然后通过 Wireshark 打开文件，查看网络包的内容，相对来说比较方便。可以通过 tcpdump -D 查看支持的网卡接口，通过 -i 指定接口，在此使用 lo。

注意，tcpdump 不能捕获 unix socket，链接时不能使用 -S /tmp/mysql.sock 或者 -h localhost 参数，应当使用 -h 127.1 。

可以将 tcpdump 的包输出到 stdout 或者通过 -w 保存到文件，然后用 Wireshark 分析。

{% highlight text %}
----- 将抓包的数据保存到文件
# tcpdump -i lo port 3306 -w filename

----- 当然，也可以打印到终端，然后处理数据
# tcpdump -i lo port 3306 -nn -X -q
# tcpdump -i any -s 0 -l -w - dst port 3306 | strings | grep -iE 'select|update'
{% endhighlight %}

<!--tcpflow -c -p -i eth0 dst port 3306 | grep -i -E "select|insert|update|delete|replace" | sed 's%\(.*\)\([.]\{4\}\)\(.*\)%\3%'-->

## 认证协议

认证稍微有点复杂，单独拉出来。

MySQL 的用户管理模块信息存储在系统表 mysql.user 中，其中包括了授权用户的基本信息以及一些权限信息。在登陆时，只会用到 host、user、passwd 三个字段，也就是说登陆认证需要 host+user 关联，当然可以使用 * 通配符。

服务器在收到新的连接请求时，会调用 login_connection() 作身份验证，先根据 IP 做 ACL 检查，然后才进入用户名密码验证阶段。

其中报文的格式如下。

![auth protocol]({{ site.url }}/images/databases/mysql/auth_protocol_handshake.png "auth protocol"){: .pull-center }

MySQL 认证采用经典的 CHAP 协议，即挑战握手认证协议，在 native_password_authenticate() 函数的注释中简单介绍了该协议的执行过程：

{% highlight text %}
1. the server sends the random scramble to the client.
2. client sends the encrypted password back to the server.
3. the server checks the password.
{% endhighlight %}

random scramble 在 4.1 之前的版本中是 8 字节整数，在 4.1 以及后续版本是 20 字节整数，该值是通过 create_random_string() 函数生成。

根据版本不同，分为了两类。

<!--
The Server Initialization Packet and the Client Authentication Packet both have an 8-byte field, scramble_buff. The value in this field is used for password authentication.

Relevant MySQL Source Code: libmysql/password.c, see also comments at start of file. It works thus:


[edit] 4.0 and before
    The server sends a random string to the client, in scramble_buff.
    The client encrypts the scramble_buff value using the hash of a password that the user has entered. This happens in sql/password.c:scramble() function.
    The client sends the encrypted scramble_buff value to the server.
    The server encrypts the original random string using a value in the mysql database, mysql.user.Password.
    The server compares its encrypted random string to what the client sent in scramble_buff.
    If they are the same, the password is okay.

In this protocol, snooping on the wire doesn't reveal the password. But note the problem - if the client doesn't know the password, but knows a hash of it (as stored in mysql.user.Password) it can connect to the server. In other words, the hash of a password is the real password; if one can get the value of mysql.user.Password - he can connect to the server.

[edit] 4.1 and later
Remember that mysql.user.Password stores SHA1(SHA1(password))
    The server sends a random string (scramble) to the client
    the client calculates:
        stage1_hash = SHA1(password), using the password that the user has entered.
        token = SHA1(scramble + SHA1(stage1_hash)) XOR stage1_hash
    the client sends the token to the server
    the server calculates
        stage1_hash' = token XOR SHA1(scramble + mysql.user.Password)
    the server compares SHA1(stage1_hash') and mysql.user.Password
    If they are the same, the password is okay.

(Note SHA1(A+B) is the SHA1 of the concatenation of A with B.)

This protocol fixes the flaw of the old one, neither snooping on the wire nor mysql.user.Password are sufficient for a successful connection. But when one has both mysql.user.Password and the intercepted data on the wire, he has enough information to connect. `
-->

### 4.0版本之前

基本流程如下：

1. 服务器发送随机字符串 (scramble) 给客户端。可以参考 create_random_string() 的生成方法。

2. 客户端把用户明文密码加密一下，然后再将其与服务器发送的随机字符串加密一下，然后变成了新的 scramble_buff 发送给服务端。可以参考 scramble() 函数的实现。

3. 服务端将 mysql.user.password 中的值加上原始随机字符串进行加密，如果加密后的值和客户端发送过来的内容一样，则验证成功。

需要注意的是：真正意义上的密码是明文密码的加密 hash 值; 如果有人知道了这个用户的 password 哈希值，而不用知道原始明文密码，实际上他就能直接登录服务器。

### 4.1 以后版本

数据库中保存的密码是用 SHA1(SHA1(password)) 加密的，其流程为：

1. 服务器发送随机字符串 (scramble) 给客户端。

2. 客户端作如下计算，然后客户端将 token 发送给服务端。

    stage1_hash = SHA1(明文密码)

    token = SHA1(scramble + SHA1(stage1_hash)) XOR stage1_hash

3. 服务端作如下计算，比对 SHA1(stage1_hash) 和 mysql.user.password 是否相同。

    stage1_hash = token XOR SHA1(scramble + mysql.user.password)

这里实际用到了异或的自反性： A XOR B XOR B = A ，对于给定的数 A，用同样的运算因子 B 作两次异或运算后仍得到 A 本身。对于当前情况的话，实际的计算过程如下。

{% highlight text %}
token = SHA1(scramble + SHA1(SHA1(password))) XOR SHA1(password)         // 客户端返回的值
      = PASSWORD XOR SHA1(password)

stage1_hash = token XOR SHA1(scramble + mysql.user.password) = token XOR PASSWORD
            = [PASSWORD XOR SHA1(password)] XOR PASSWORD
            = SHA1(password)
{% endhighlight %}

因此，校验时，只需要 SHA1(stage1_hash) 与 mysql.user.password 比较一下即可。

这次没上一个版本的缺陷了. 有了 mysql.user.password 和 scramble 也不能获得 token，因为没法获得 stage1_hash。

但是如果用户的 mysql.user.password 泄露，并且可以在网络上截取的一次完整验证数据，从而可以反解出 stage1_hash 的值。而该值是不变的，因此下次连接获取了新的 scramble 后，自己加密一下 token 仍然可以链接到服务器。


## 源码分析

接下来分别介绍客户端、服务端的程序。

### 客户端

对于 mysql 客户端，源码保存在 client/mysql.cc 文件中，下面是 main() 函数的主要执行流程。

{% highlight text %}
main()
 |-sql_connect()
 | |-sql_real_connect()
 |   |-mysql_init()                             # 调用MySQL初始化
 |   |-mysql_options()                          # 设置链接选项
 |   |-mysql_real_connect()                     # sql-common/client.c
 |     |-connect_sync_or_async()                # 通过该函数尝试链接
 |     | |-my_connect()                         # 实际通过该函数建立链接
 |     |-cli_safe_read()                        # 等待第一个handshake包
 |     |-run_plugin_auth()                      # 通过插件实现认证
 |
 |-put_info()                                   # 打印客户端的欢迎信息
 |-read_and_execute()                           # 开始等待输入、执行SQL
{% endhighlight %}

客户端最终会调用 mysql_real_connect()，实际调用的是 cli_mysql_real_connect()，通过该函数建立链接，其中认证方式可以通过 run_plugin_auth() 时用插件实现。

然后，会输出一系列的欢迎信息，并通过 read_and_execute() 执行 SQL 命令。

在 MySQL 客户端执行时，并非所有的命令都是需要发送到服务端的，其中有一个数组定义了常见的命令。

{% highlight c %}
static COMMANDS commands[] = {
  { "?",      '?', com_help,   1, "Synonym for `help'." },
  { "clear",  'c', com_clear,  0, "Clear the current input statement."},
  ... ...
};
{% endhighlight %}

每次读取一行都会通过 find_command() 函数进行检测，如果满足对应的命令，且对应的函数变量非空，则直接执行，如 clear，此时不需要输入分号即可；如果没有找到，则必须要等待输入分号。

{% highlight c %}
int read_and_execute(bool interactive)
{
    while (!aborted) {
        if (!interactive) {                               // 是否为交互模式
            ... ...   // 非交互模式，直接执行
        } else {                                          // 交互模式
            char *prompt = ...;                           // 首先会设置提示符
            line = readline(prompt);                      // 从命令行读取
            if ( ... && (com= find_command(line))) {      // 从commands[]中查找
                (*com->func)(&glob_buffer,line);          // 如果是help、edit等指令，则直接执行
            }
            add_line(...);                                // 常见的SQL，最终在此执行
        }
    }
}

int com_go(String *buffer,char *line)
{
    timer=start_timer();                                                // 设置时间
    error= mysql_real_query_for_lazy(buffer->ptr(),buffer->length());   // 执行查询SQL
    do {
        // 获取结果
    } while(!(err= mysql_next_result(&mysql)));
}
{% endhighlight %}

在 add_line() 函数中，最终会调用 com_go() 函数，该函数是执行的主要函数，会最终调用 MySQL API 执行相应的 SQL、返回结果、输出时间等统计信息。

### 服务端

服务端通过 network_init() 执行一系列初始化之后，会阻塞在 handle_connections_sockets() 函数的 select()/poll() 函数处。

对于 one_thread_per_connection 这种方式，会新建一个线程执行 handle_one_connection() 。

{% highlight text %}
handle_one_connection()
 |-thd_prepare_connection()
   |-login_connection()
     |-check_connection()
       |-acl_authenticate()
{% endhighlight %}

源码内容如下。

<!-- send_server_handshake_packet(...);  // sql/sql_acl.cc -->

{% highlight c %}
/* sql/sql_connect.cc */
int check_connection(THD *thd)
{
    if (!thd->main_security_ctx.host) {  // 通过TCP/IP连接，或者本地用-h 127.1
         if (acl_check_host(...))        // 检查hostname
    } else {                             // 使用unix sock连接，不会进行检测
        ... ...
    }
    return acl_authenticate(thd, connect_errors, 0)
}

/* sql/sql_acl.cc */
bool acl_authenticate(THD *thd, uint connect_errors, uint com_change_user_pkt_len)
{
    if (command == COM_CHANGE_USER) {

    } else {
        do_auth_once()                      // 执行认证模式

    }
}
{% endhighlight %}

在 acl_check_host() 会检查两个对象，一个是 hash 表 acl_check_hosts；另一个是动态数组 acl_wild_hosts 。这2个对象是在启动的时候，通过 init_check_host() 从 mysq.users 表里读出并加载的，其中 acl_wild_hosts 用于存放有统配符的主机，acl_check_hosts 存放具体的主机。

最终会调用 acl_authenticate() 这是主要的认证函数。

### 插件实现

MySQL 的认证授权采用插件实现。

默认采用 mysql_native_password 插件，也就是使用 native_password_auth_client() 作用户端的认证，实际有效的函数是 scramble()。

上述的函数通过用户输入的 password、服务器返回的 scramble 生成 reply，返回给服务端；可以通过 ```password('string')``` 查看加密后的密文。

<!--MySQL 对于登陆认证采用插件方式，默认会采用 mysql_native_password 插件，也即新的协议，-->

以 plugin/auth/ 目录下的插件为例，在启动服务器时，可添加 \-\-plugin-load=auth_test_plugin.so 参数自动加载相应的授权插件。

{% highlight text %}
----- 获得foobar的加密格式
mysql> select password('foobar');
----- 旧的加密格式
mysql> select old_password('foobar');
----- 默认方式
mysql> create user 'foobar2'@'localhost' identified via mysql_native_password using 'xxx';

----- 也可以动态加载
mysql> install plugin test_plugin_server soname 'auth_test_plugin.so';
----- 查看当前支持的插件
mysql> select * from information_schema.plugins where plugin_type='authentication';

mysql> create user 'foobar'@'localhost' identified with test_plugin_server;
mysql> SET PASSWORD FOR 'foobar'@'localhost'=PASSWORD('new_password');
mysql> DROP USER 'foobar'@'localhost';
mysql> FLUSH PRIVILEGES;
mysql> SELECT host, user, password, plugin FROM mysql.user;
{% endhighlight %}

在 plugin 目录下有很多 auth 插件可供参考，详细可参考官网 [Writing Authentication Plugins](http://dev.mysql.com/doc/refman/en/writing-authentication-plugins.html) 。

## 总结

在如下列举客户端与服务端的详细交互过程，其中客户端代码在 client 目录下。

{% highlight text %}
### Client(mysql)  ###                       ### Server(mysqld) ###
----------------------------------------     --------------------------------------------------
main()                                       mysqld_main()
 |-sql_connect()                              |-init_ssl()
 | |-sql_real_connect() {for(;;)}             |-network_init()
 |   |-mysql_init()                           |-handle_connections_sockets()
 |   |-init_connection_options()                |-select()/poll()
 |   |-mysql_real_connect()                     |
 |     |-cli_mysql_real_connect()               |
 |       |-socket()                             |
 |       |-vio_new()                            |
 |       |-vio_socket_connect()                 |
 |       | |-mysql_socket_connect()             |
 |       |   |-connect()                        |
 |       |   |                                  |
 |       |   |        [Socket Connect]          |
 |       |   |>>==========>>==========>>======>>|
 |       |                                      |-accept()
 |       |-vio_keepalive()                      |-vio_new()
 |       |-my_net_set_read_timeout()            |-my_net_init()
 |       |-my_net_set_write_timeout()           |-create_new_thread()
 |       |-vio_io_wait()                          |-handle_one_connection()    {新线程}
 |       |                                          |-thd_prepare_connection() {for(;;)}
 |       |                                          | |-login_connection()
 |       |                                          |   |-check_connection()
 |       |                                          |     |-acl_check_host()
 |       |                                          |     |-vio_keepalive()
 |       |                                          |     |-acl_authenticate()
 |       |                                          |       |-do_auth_once()
 |       |                                          |       | |-native_password_authenticate()  {插件实现}
 |       |                                          |       |   |-create_random_string()
 |       |                                          |       |   |-send_server_handshake_packet()
 |       |                                          |       |   |
 |       |              [Handshake Initialization]  |       |   |
 |       |<<==<<==========<<==========<<==========<<==========<<|
 |       |-cli_safe_read()                          |       |   |-my_net_read()
 |       |-run_plugin_auth()                        |       |   |
 |       | |-native_password_auth_client()          |       |   |
 |       |   |-scramble()                           |       |   |
 |       |     |-my_net_write()                     |       |   |
 |       |     |                                    |       |   |
 |       |     |            [Client Authentication] |       |   |
 |       |     |>>==========>>==========>>==========>>========>>|
 |       |                                          |       |   |-check_scramble()
 |       |                                          |       |-mysql_change_db()
 |       |                                          |       |-my_ok()
 |       |                      [OK]                |       |
 |       |<<==========<<==========<<==========<<==========<<|
 |       |-cli_safe_read()                          |
 |                                                  |
 |                                                  |
 |                                                  |
 |                                                  |
 |-put_info() {Welcome Info}                        |
 |-read_and_execute() [for(;;)]                     |
                                                    |-thd_is_connection_alive()  [while()]
                                                    |-do_command()
{% endhighlight %}




<!--

最终会调用 native_password_authenticate() 函数实现认证，<br><br>

mysql -h a.b.c.d -u root -pxxxx

最终都会调用到mysql_real_connect(sql/client.c的1856行的宏CLI_MYSQL_REAL_CONNECT)，我们从这个函数出发。

<ol><li>
客户端发起 socket 连接，等待三次握手的通过。</li><br><li>

三次握手通过之后，客户端进入 client_safe_read() 阻塞，同时服务端从 handle_connection_sockets() 函数的 select()/poll() 处返回，获知有新的连接。</li><br><li>

服务端创建线程，然后开始对这个连接进行校验，其中 check_connection() 为的主校验函数，会检测该用户是否可以链接，然后产生一个长度为 20 的 scramble 发送给客户端。</li><br><li>






密码验证的plugin初始化比较隐晦，只能估摸着摸索一下：
前面提到了那个重要的函数native_password_authenticate， 这个是密码验证插件的定义函数。也是密码验证算法入口。mysql 通过以下方式将其注册成plugin.
1. 初始化一个插件结构，该结构定义了一个插件所对应的handler，对应的安全认证函数handler为native_password_handler或者old_password_handler
mysql_declare_plugin(mysql_password)
{
  MYSQL_AUTHENTICATION_PLUGIN,
  &native_password_handler,
  native_password_plugin_name.str,
  "R.J.Silk, Sergei Golubchik",
  "Native MySQL authentication",
  PLUGIN_LICENSE_GPL,
  NULL,
  NULL,
  0x0100,
  NULL,
  NULL,
  NULL
},
{
  MYSQL_AUTHENTICATION_PLUGIN,
  &old_password_handler,
  old_password_plugin_name.str,
  "R.J.Silk, Sergei Golubchik",
  "Old MySQL-4.0 authentication",
  PLUGIN_LICENSE_GPL,
  NULL,
  NULL,
  0x0100,
  NULL,
  NULL,
  NULL
}
mysql_declare_plugin_end;
native_password_handler被定义为：
static struct st_mysql_auth native_password_handler=
{
  MYSQL_AUTHENTICATION_INTERFACE_VERSION,
  native_password_plugin_name.str,
  native_password_authenticate //那个重要的函数
};
2. 下面就要看mysql_declare_plugin是怎么定义的。它是一个很曲折的宏定义：
#ifndef MYSQL_DYNAMIC_PLUGIN
#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYSQL_PLUGIN_EXPORT int VERSION= MYSQL_PLUGIN_INTERFACE_VERSION;                                  \
MYSQL_PLUGIN_EXPORT int PSIZE= sizeof(struct st_mysql_plugin);                                    \
MYSQL_PLUGIN_EXPORT struct st_mysql_plugin DECLS[]= {
#else
#define __MYSQL_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYSQL_PLUGIN_EXPORT int _mysql_plugin_interface_version_= MYSQL_PLUGIN_INTERFACE_VERSION;         \
MYSQL_PLUGIN_EXPORT int _mysql_sizeof_struct_st_plugin_= sizeof(struct st_mysql_plugin);          \
MYSQL_PLUGIN_EXPORT struct st_mysql_plugin _mysql_plugin_declarations_[]= {
#endif

#define mysql_declare_plugin(NAME) \
__MYSQL_DECLARE_PLUGIN(NAME, \
                 builtin_ ## NAME ## _plugin_interface_version, \
                 builtin_ ## NAME ## _sizeof_struct_st_plugin, \
                 builtin_ ## NAME ## _plugin)
该定义在c++中做了一个c的export声明，声明了名为 builtin_## NAME ## _plugin， 类型为st_mysql_plugin的插件数组。在前面第一步中，使用了 mysql_password作为NAME, 因此，这个插件数组名字为builtin_mysql_password_plugin,里面放了两个安全验证插件，一个为native_password_handler 另一个为old_password_handler。
3. 初始化插件
在plugin.cc的plugin_init方法中，有一段初始化插件的for循环，循环中的mysql_mandatory_plugins是如何定义的呢？

  for (builtins= mysql_mandatory_plugins; *builtins || mandatory; builtins++)
  {
    if (!*builtins)
    {
      builtins= mysql_optional_plugins;
      mandatory= false;
      if (!*builtins)
        break;
    }
mysql_mandatory_plugins定义在了sql_builtin.cc.in
struct st_mysql_plugin *mysql_mandatory_plugins[]=
{
  builtin_binlog_plugin, builtin_mysql_password_plugin, @mysql_mandatory_plugins@ 0
};
在cmakefile中，
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/sql/sql_builtin.cc.in ${CMAKE_BINARY_DIR}/sql/sql_builtin.cc)将builtin.cc.in 转为源文件进行编译。于是安全验证插件被包含在了mysql_mandatory_plugins中，并在上面介绍的方法中得到了初始化。



下面以 plugin/auth_examples 目录下的插件为例，在启动 server 时，添加 --plugin-load=auth_test_plugin.so 参数自动加载。现在还不太清楚？？？？？？？？
<pre style="font-size:0.8em; face:arial;">
mysql> select password('foobar');                                                           // 获得foobar的加密格式
mysql> select old_password('foobar');                                                       // 旧的加密格式
mysql> create user 'foobar2'@'localhost' identified via mysql_native_password using 'xxx';  // 默认方式

mysql> install plugin test_plugin_server soname 'auth_test_plugin.so';                      // 也可以动态加载
mysql> select * from information_schema.plugins where plugin_type='authentication';         // 查看当前支持的插件



mysql> create user 'foobar'@'localhost' identified with test_plugin_server;
mysql> SET PASSWORD FOR 'foobar'@'localhost'=PASSWORD('new_password');
mysql> DROP USER 'foobar'@'localhost';
mysql> FLUSH PRIVILEGES;
select host, user, password, plugin from mysql.user;
</pre>




<pre style="font-size:0.8em; face:arial;">
typedef struct st_net {
    unsigned int pkt_nr;                // 发送包的顺序，SVR+CLI都从0开始
    my_bool compress;                    // 是否压缩
} NET;

vio_read();                 // 最终读取的函数，会调用read()



struct st_vio {   /* include/violite.h */

};
typedef struct st_vio Vio;


typedef struct st_mysql {
    NET net;               // 网络参数

} MYSQL;
</pre>
在 Vio 中，包含了大量的接口函数，用于屏蔽底层的不同协议。

<br><br>
客户端和服务端的 NET.pkt_nr 都会从 0 开始，在接受包时比较 packet number 和 NET.pkt_nr 是否相等，否则报 packet number 乱序，连接报错，也就是说必须要保证数据的顺序性。<br><br>


<br><br><h1>网络实现</h1><p>
MySQL 中通讯相关的代码保存在 vio 目录下，主要完成对底层通讯协议以及跨平台的封装，现在支持的协议包括 TCP/IP、共享内存、SSL、命名管道等。这样只需要关心相关的算法、处理逻辑即可。<br><br>

与网络协议相关的主要有两个数据结构：struct st_net@include/mysql_com.h(NET)、struct st_vio@include/violite.h(Vio)。其中 NET 是 MySQL 网络通信数据结构，Vio 是对网络通信底层的进行封装结构。通过 Vio 屏蔽了跨平台的差异性、统一了不同读写策略的接口等。<br><br>

常见的接口有。
<ul><li>
net_write_command()@sql/net_serv.cc<br>
所有的 SQL 最终调用这个命令发送出去。</li><br><li>

my_net_write()@sql/net_serv.cc<br>
连接阶段的 socket write 操作调用这个函数。</li><br><li>

my_net_read()@sql/net_serv.ccc<br>
读取包,会判断包大小,是否是分包
<!--
网络协议关键函数
my_real_read解析MySQL packet,第一次读取4字节,根据packet length再读取余下来的长度
cli_safe_read客户端解包函数,包含了my_net_read
</li></ul>

<ul><li>
net_send_ok()@src/protocol.cc<br>
发送 OK 包，通常为命令、insert、update、delete 的返回结果
● 包体首字节为0.
● insert_id, affect_rows也是一并发过来.
</li></ul>

<ul><li>
NET 缓冲<br>
每次 socket 操作都会先将数据读写到 NET.buf 缓冲区中，从而减少系统调用调用的次数。当写入的数据 + buff 内的数据超过 buff 大小才会发出一次 write() 操作，写入不会导致 buff 区域扩展，可以参考 net_write_buff@sql/net_serv.cc。<br><br>
net->buff大小初始net->max_packet, 读取会导致会导致 buff的realloc最大net->max_packet_size
注意，一次 SQL 命令结束时，会调用 net_flush() 把 buff 里的数据都写到 socket 里。</li><br><li>


VIO 缓冲<br>
● 从my_read_read可以看出每次packet读取都是按需读取， 为了减少系统调用,vio层面加了一个read_buffer.
● 每次读取前先判断vio->read_buffer所需数据的长度是 否足够.如果存在则直接copy. 如果不够,则触发一次 socket read 读取2048个字(vio/viosocket.c: vio_read_buff)



ok packet
● ok包,命令和insert,update,delete的返回结果
● 包体首字节为0.
● insert_id, affect_rows也是一并发过来.
● src/protocol.cc:net_send_ok

error packet

● 错误的命令,非法的sql的返回包
● 包体首字节为255.
● error code就是CR_***,include/errmsg.h
● sqlstate marker是#
● sqlstate是错误状态,include/sql_state.h
● message是错误的信息
● sql/protocol.cc:net_send_error_packet

resultset packet

● 结果集的数据包,由多个packet组合而成
● 例如查询一个结构集,顺序如下:
    ○ header
    ○ field1....fieldN
    ○ eof
    ○ row1...rowN
    ○ eof
● sql/client.c:cli_read_query_result
● 下面是一个sql "select * from d"查询结果集的例子,结果 集是6行，3个字段
    ○ 公式：假设结果集有N行, M个字段.则包的个数为，header(1) + field (M) + eof(1) + row(N) + eof(1)
    ○ 所以这个例子的MySQL packet的个数是12个

resultset packet - header

● field packet number决定了接下来的field packet的个数.
● 一个返回6行记录,3个字段的查询语句

resultset packet - field

● 结果集中一个字段一个field packet.
● tables_alias是sql语句里表的别名,org_table才是表的真 实名字.
● sql/protocol.cc:Protocol::send_fields
● sql/client.c:cli_read_query_result

resultset packet - eof

● eof包是用于分割field packet和row packet.
● 包体首字节为254
● sql/protocol.cc:net_send_eof

resultset packet - row

● row packet里才是真正的数据包.一行数据一个packet.
● row里的每个字段都是length coded binary
● 字段的个数在header packet里
● sql/client.c:cli_read_rows

command packet

● 命令包,包括我们的sql语句还有一些常见的命令.
● 包体首字母表示命令的类型(include/mysql_com.h),大 部分命令都是COM_QUERY.

网络协议关键函数

● net_write_command(sql/net_serv.cc)所有的sql最终调用这个命令发送出去.
● my_net_write(sql/net_serv.cc)连接阶段的socket write操作调用这个函数.
● my_net_read读取包,会判断包大小,是否是分包
● my_real_read解析MySQL packet,第一次读取4字节,根据packet length再读取余下来的长度
● cli_safe_read客户端解包函数,包含了my_net_read

NET缓冲

● 每次socket操作都会先把数据写,读到net->buff,这是一 个缓冲区, 减少系统调用调用的次数.
● 当写入的数据和buff内的数据超过buff大小才会发出一次 write操作,然后再把要写入的buff里插入数, 写入不会 导致buff区区域扩展.(sql/net_serv.cc: net_write_buff).
● net->buff大小初始net->max_packet, 读取会导致会导致 buff的realloc最大net->max_packet_size
● 一次sql命令的结束都会调用net_flush,把buff里的数据 都写到socket里.

VIO缓冲

● 从my_read_read可以看出每次packet读取都是按需读取， 为了减少系统调用,vio层面加了一个read_buffer.
● 每次读取前先判断vio->read_buffer所需数据的长度是 否足够.如果存在则直接copy. 如果不够,则触发一次 socket read 读取2048个字(vio/viosocket.c: vio_read_buff)

MySQL API

● 数据从mysql_send_query处发送给服务端,实际调用的是 net_write_command.
● cli_read_query_result解析header packet, field packet,获 得field_count的个数
● mysql_store_result解析了row packet,并存储在result- >data里
● myql_fetch_row其实遍历result->data

________________________________

PACKET NUMBER

在做proxy的时候在这里迷糊过,翻了几遍代码才搞明白，细节如下： 客户端服务端的net->pkt_nr都从0开始.接受包时比较packet number 和net->pkt_nr是否相等,否则报packet number乱序,连接报错;相等则pkt_nr自增.发送包时把net->pkt_nr作为packet number发送,然后对net->pkt_nr进行自增保持和对端的同步.

接收包

sql/net_serv.c:my_real_read

898     if (net->buff[net->where_b + 3] != (uchar) net->pkt_nr)

发送包

sql/net_serv.c:my_net_write

392   int3store(buff,len);
393   buff[3]= (uchar) net->pkt_nr++;

我们来几个具体场景的packet number, net->pkt_nr的变化

连接

0 c ———–> s 0  connect

0 c <—-0——s 1  handshake

2 c —–1—–>s 1  auth

2 c <—–2——s 0  ok

开始两方都为0,服务端发送handshake packet(pkt=0)之后自增为1,然后等待对端发送过来pkt=1的包

查询

每次查询,服务客户端都会对net->pkt_nr进行清零

include/mysql_com.h
388 #define net_new_transaction(net) ((net)->pkt_nr=0)
sql/sql_parse.cc:do_command
805   net_new_transaction(net);

sql/client.c:cli_advanced_command

800   net_clear(&mysql->net, (command != COM_QUIT));

开始两方net->pkt_nr皆为0, 命令发送后客户端端为1,服务端开始发送分包,分包的pkt_nr的依次递增,客户端的net->pkt_nr也随之增加.

1 c ——0—–> s 0  query

1 c <—-1——s 2  resultset

2 c <—-2——s 3  resultset

解包的细节

my_net_read负责解包，首先读取4个字节，判断packet number是否等于net->pkt_nr然后再次读取packet_number长度的包体。

伪代码如下：

remain=4
for(i = 0; i < 2; i++) {
    //数据是否读完
    while (remain>0)  {
        length = read(fd, net->buff, remain)
        remain = remain - length
    }
    //第一次
    if (i=0) {
        remain = uint3korr(net->buff+net->where_b);
    }
}

网络层优化

从ppt里可以看到,一个resultset packet由多个包组成,如果每次读写包都导致系统调用那肯定是不合理,常规优化方法:写大包加预读

NET->BUFF

每个包发送到网络或者从网络读包都会先把数据包保存在net->buff里,待到net->buff满了或者一次命令结束才会通过socket发出给对端.net->buff有个初始大小(net->max_packet),会随读取数据的增多而扩展.

VIO->READ_BUFFER

每次从网络读包,并不是按包的大小读取,而是会尽量读取2048个字节,这样一个resultset包的读取不会再引起多次的系统调用了.header packet读取完毕后, 接下来的field,eof, row apcket读取仅仅需要从vio-read_buffer拷贝指定字节的数据即可.

MYSQL API说明

api和MySQL客户端都会使用sql/client.c这个文件,解包的过程都是使用sql/client.c:cli_read_query_result.

mysql_store_result来解析row packet,并把数据存储到res->data里,此时所有数据都存内存里了.

mysql_fetch_row仅仅是使用内部的游标,遍历result->data里的数据

3052     if (!res->data_cursor)
3053     {
3054       DBUG_PRINT("info",("end of data"));
3055       DBUG_RETURN(res->current_row=(MYSQL_ROW) NULL);
3056     }
3057     tmp = res->data_cursor->data;
3058     res->data_cursor = res->data_cursor->next;
3059     DBUG_RETURN(res->current_row=tmp);

mysql_free_result是把result->data指定的行数据释放掉.


● handshake
● auth
● ok|error
● resultset
    ○ header
    ○ field
    ○ eof
    ○ row
● command packet



● 数据从mysql_send_query处发送给服务端,实际调用的是 net_write_command.
● cli_read_query_result解析header packet, field packet,获 得field_count的个数
● mysql_store_result解析了row packet,并存储在result- >data里
● myql_fetch_row其实遍历result->data


net->buff

每个包发送到网络或者从网络读包都会先把数据包保存在net->buff里,待到net->buff满了或者一次命令结束才会通过socket发出给对端.net->buff有个初始大小(net->max_packet),会随读取数据的增多而扩展.
vio->read_buffer

每次从网络读包,并不是按包的大小读取,而是会尽量读取2048个字节,这样一个resultset包的读取不会再引起多次的系统调用了.header packet读取完毕后, 接下来的field,eof, row apcket读取仅仅需要从vio-read_buffer拷贝指定字节的数据即可.
MySQL api说明

api和MySQL客户端都会使用sql/client.c这个文件,解包的过程都是使用sql/client.c:cli_read_query_result.

mysql_store_result来解析row packet,并把数据存储到res->data里,此时所有数据都存内存里了.

mysql_fetch_row仅仅是使用内部的游标,遍历result->data里的数据

3052     if (!res->data_cursor)
3053     {
    3054       DBUG_PRINT("info",("end of data"));
    3055       DBUG_RETURN(res->current_row=(MYSQL_ROW) NULL);
    3056     }
    3057     tmp = res->data_cursor->data;
    3058     res->data_cursor = res->data_cursor->next;
    3059     DBUG_RETURN(res->current_row=tmp);

    mysql_free_result是把result->data指定的行数据释放掉.





 枚举类型enum_vio_type定义了Vio的几种不同的网络连接类型，根据不同的类型，在网络通信时有相应不同的处理逻辑。

       Vio数据结构的定义分为几个部分：成员变量部分、接口部分、SSL部分、共享内存部分和windows特有成员变量。其中成员变量包括：socket描述符sd；管道描述符hPipe；localhost表示是否为本机；fcntl_mode表示socket文件描述符的模式，通过fcntl()函数（在windows下是fcntlsocket()函数）设置sd描述符的一些特性；local和remote分别表示本地和远端的网络地址，其中sockaddr_storage是通用网络地址数据结构；addrLen远端的网络地址长度；type表示网络连接类型；数组desc是Vio的描述信息；read_buffer、read_pos、read_end分别表示读buffer缓冲的指针地址、读取的当前位置以及结束地址，socket数据读取采用缓冲机制，提高读的性能。接口部分是socket的基本操作的函数指针，根据不同的平台和不同的读写策略，分别指向不同的处理函数。SSL部分是指当定义了HAVE_OPENSSL宏时，定义SSL相关的成员变量ssl_arg。共享内存部分是指当定义了HAVE_SMEM宏时，定义共享内存相关的成员变量。如果定义了_WIN32宏时，那么需要定义windows操作的成员变量。SSL部分、共享内存部分和windows特有成员变量都是根据编译时定义的宏及平台决定的，是不同读写策略时，使用的成员变量。

源码实现

       MySQL数据结构Vio对网络通信的封转，在源码的/vio/vio.c、/vio/viosocket.c、/vio/viossl.c、/vio/viosslfactories.c中实现。由于大多数的实现是根据不同的连接类型，对底层函数的调用，基本不涉及复杂的算法和处理逻辑。因此，以下内容中，仅对几个比较核心的处理过程进行简要的分析。

vio_init函数

       vio_init()函数是Vio的内部初始化函数，对外接口vio_new*()初始化函数，都调用vio_init()函数进行初始化。该函数根据不同的连接类型，初始化相应类型的处理函数指针。参考源码/vio/vio.c。

vio_read_buffer函数

       vio_read_buffer()函数是网络缓冲写方法，该方法同IO_CACHE的io读有相似之处。读取策略分为三种：如果read_buffer中有数据，直接从read_buffer中读取数据；如果读取的数据长度小于16K，那么调用vio_read()函数读取16K数据到read_buffer中，然后再从read_buffer中读取数据；如果数据长度大于16K，直接通过vio_read()读取制定长度的数据，不需要写入缓冲read_buffer。这种读写策略，可以有效的提高小数据量读取的性能。例如readline这种方式，需要通过逐个字符判断是否为换行符，如果直接调用vio_read()进行读取，会每次产生一次寻址、读取等物理操作。而采用缓冲读策略，所有的操作从read_buffer中获取数据，避免频繁的物理操作。

vio_close函数

       vio_close()函数是关闭网络连接的操作，在该过程中，有一个问题需要特别说明。在调用close()（windows下使用closesocket()函数）进行关闭socket连接之前，调用shutdown()函数将读写关闭。这是因为，在多个进程共享一个socket套接字，调用close()只是引用数减1，其他进程仍然可以通信，直到计数为0，才将socket套接字释放。而调用shutdown(2)则使得其他进程也无法进行通信。具体close()[1]和shutdown()[2]的区别，可以参考Linux manual中相应的解释。

socket_poll_read函数

       socket_poll_read()函数是实现IO复用的封装函数，从该封装函数中可以看出，MySQL在IO复用中，如果操作系统是windows，那么采用select()函数（在linux下select()的最大文件描述数目为1024，windows下无限制），在Linux系统中，使用poll()函数。select()和poll()类似，性能比select()略高。然而，这两种方式都存在一个问题，因为他们都需要遍历所有的文件描述符，当监听描述符个数增加时，监听效率降低，并且select和poll每次都要在用户态和内核态拷贝监听的描述符参数。更为高效的解决方案是epoll()方法，可以解决select()和poll()存在的不足。具体详细的分析和差异，参考相应的文档。

       SSL相关的处理函数在源码的/vio/viossl.c和/vio/viosslfactories.c，具体的实现主要是对SSL相关函数的封装，不再赘述。

结论

       通过以上分析可知，MySQL数据结构Vio主要封装了不同连接方式的网络通信接口，从而使得网络通信可以忽略平台差异和连接方式的不同，并且可以支持共享内存、SSL安全连接方式等通信方式。

       然而在IO复用方面，MySQL数据结构Vio存在一定的不足，使用了poll()方式。对于数据库这种高并发系统来说，会随着连接数的增加，使得poll()的系统调用次数严重下降，处理能力也随之降低。因此，对MySQL来说，一般的优化方案是限制连接数，而在应用层使用连接池的策略，来解决这个问题。然而当多个不同系统同时使用一个数据库实例时，仍然会导致连接数增加，如果该值设置较小的话，会导致连接失败。最佳的方式是使用epoll()方式代替poll()方式，从本质上提高处理能力。





用户通过 mysql 客户端登录时，实际最终调用的是 mysql_real_connect() 函数。<br><br>


MySQL 的用户管理模块信息存储在系统表 mysql.user 中，包括了授权用户的基本信息以及一些权限信息。其中登陆时，只会用到 host、user、passwd，也就是说登陆认证需要 host+user 关联，当然可以使用 * 通配符。<br><br>


信息系统中，访问控制分为自主访问控制(DAC)和强制访问控制(MAC)。具体到DBMS，自主访问控制就是我们所熟悉的GRANT，REVOKE，大多数数据库都支持自助的访问控制。强制访问控制就是ORACLE中的LABEL，只有很少的一些系统支持MAC。
严格来说，登录并不属于访问控制机制，而应该属于用户身份识别和认证。在Mysql中，将登录和DAC的相关接口都实现在了
sql_acl.cc中(其实说登录是用户拥有的一种权限也未尝不可，正如ORACLE中的CREATE SESSION，不过登录并不仅仅是一种权
限，还包含很多其他的属性)，从文件名大家可以看出来，ACL即ACCESS CONTROL LIST，访问控制列表，这是实现访问控制的
基本方法。下图是Mysql的整个访问控制的流程。

-->

## 参考

关于 MySQL的认证流程，包括客户端和服务器端，可以参考本地 [MySQL认证协议](/reference/databases/mysql/MySQL_Auth_Procotol.mht)；详细的协议介绍可以参考 [MySQL Client/Server Protocol](http://dev.mysql.com/doc/internals/en/client-server-protocol.html)，或者 [中文资料](http://blog.geekcome.com/archives/270)，或者保存的本地资料 [MySQL服务器和客户端通信协议分析](/reference/databases/mysql/MySQL_Client_Server_Protocol.mht) 。

MySQL 的认证授权可以采用插件，在 plugin 目录下有很多 auth 插件可供参考，具体可以参考官网的 [MySQL Reference - Writing Authentication Plugins](http://dev.mysql.com/doc/refman/en/writing-authentication-plugins.html) 。


{% highlight text %}
{% endhighlight %}
