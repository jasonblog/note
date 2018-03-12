---
title: MySQL 简介
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,introduce,简介
description: MySQL 是一款最流行的开源关系型数据库，最初由瑞典的 MySQL AB 公司开发，目前已被 Oracle 收购，现在比较流行的分支包括了 MariaDB 和 Percona。现在来看，MySQL 被收购之后，实际上添加了很多不错的特性，解掉了很多之前未解的问题，而现在 MariaDB 离主干的特性越来越远；而 Percona 据说为了保证质量是不接受社区提供的代码的，不过其提供的工具却是极好的^_^ 在此，简单介绍，二进制安装、源码编译安装、单实例以及多实例的启动、常见的调试方法。
---

MySQL 是一款最流行的开源关系型数据库，最初由瑞典的 MySQL AB 公司开发，目前已被 Oracle 收购，现在比较流行的分支包括了 MariaDB 和 Percona。

现在来看，MySQL 被收购之后，实际上添加了很多不错的特性，解掉了很多之前未解的问题，而现在 MariaDB 离主干的特性越来越远；而 Percona 据说为了保证质量是不接受社区提供的代码的，不过其提供的工具却是极好的^_^

在此，简单介绍，二进制安装、源码编译安装、单实例以及多实例的启动、常见的调试方法。

<!-- more -->

![mysql intro logo]({{ site.url }}/images/databases/mysql/mysql-intro-logo.jpg "mysql intro logo"){: .pull-center width="400px"}

## 架构简介

MySQL 作为一个大型的网络程序、数据管理系统，架构非常复杂，不过 MySQL 基本采用的是插件模式，下图大致画出了其逻辑架构。

![mysql architecture]({{ site.url }}/images/databases/mysql/mysql-architecture.png "mysql architecture"){: .pull-center }

简单介绍一下如上的结构内容，先大致有个了解，详细的内容后面详细介绍。

#### Connectors

实际上就是在 TCP 之上定义的 MySQL 应用层协议，比较常用的是调用 Native C API、JDBC、PHP 等各语言 MySQL Connector 。

#### Connection Management

MySQL 为每个连接绑定一个线程，这个连接上的所有操作都在这个线程中执行。为了避免频繁创建和销毁线程带来开销，通常会缓存线程或者使用线程池，从而避免频繁的创建和销毁线程。

客户端与 MySQL 成功连接后，在正式使用 MySQL 的功能之前，还需要进行认证，默认的认证方式是基于用户名、主机名、密码；如果用了 SSL 或者 TLS 的方式进行连接，还会进行证书认证。

#### SQL Interface

MySQL 支持 DML (数据操作语言)、DDL (数据定义语言)、存储过程、视图、触发器、自定义函数等多种 SQL 语言接口。

#### Parser

MySQL 会按照 SQL 语法对客户端发送的请求进行解析，并为其创建语法树，并根据数据字典丰富查询语法树，会验证该客户端是否具有执行该查询的权限。创建好语法树后，MySQL 还会对 SQL 查询进行语法上的优化，进行查询重写。

#### Optimizer

语法解析和查询重写之后，MySQL 会根据语法树和数据的统计信息对 SQL 进行优化，包括决定表的读取顺序、选择合适的索引等，最终生成 SQL 的具体执行步骤。这些具体的执行步骤里真正的数据操作都是通过预先定义好的存储引擎 API 来进行的，与具体的存储引擎实现无关。

#### Caches & Buffers

MySQL 内部维持着一些 Cache 和 Buffer，比如 Query Cache 用来缓存一条 Select 语句的执行结果，如果能够在其中找到对应的查询结果，那么就不必再进行查询解析、优化和执行的整个过程了。

#### Pluggable Storage Engine

存储引擎的具体实现，这些存储引擎都实现了 MySQL 定义好的存储引擎 API 的部分或者全部。MySQL 可以动态安装或移除存储引擎，可以有多种存储引擎同时存在，可以为每个 Table 设置不同的存储引擎。

存储引擎负责在文件系统之上，管理表的数据、索引的实际内容，同时也会管理运行时的 Cache、Buffer、事务、Log 等数据和功能。



## 安装

对于 Ubuntu、CentOS 这样的发行版本可以直接通过命令行安装，从 CentOS 7 之后，实际安装的是 MariaDB ，也可以通过 Oracle 提供的官方 YUM repository ，然后安装 MySQL Community Server，下面以 CentOS 为例。

{% highlight text %}
# yum install -y mariadb-server mariadb                                      ← 安装MariaDB
# systemctl start mariadb                                                    ← 启动server
# systemctl enable mysqld                                                    ← 设置开机启动

# rpm -Uvh http://dev.mysql.com/get/mysql-community-release-el7-5.noarch.rpm ← 安装仓库
# yum install mysql-community-server                                         ← 安裝社区版
# cat << EOF > /etc/my.cnf                                                   ← 设置配置文件
[client]
password        = new_password
port            = 5506
socket          = /tmp/mysql-5506.sock
[mysqld]
user            = mysql
port            = 5506
socket          = /tmp/mysql-5506.sock
basedir         = /usr
datadir         = /home/foobar/databases/data-5506
pid-file        = /home/foobar/databases/data-5506/mysqld.pid
EOF
# /usr/sbin/mysqld --initialize-insecure --basedir=/usr --user=mysql \       ← 初始化数据，可通过空白密码登陆
    --datadir=/home/foobar/databases/data-5506
# systemctl start mysqld                                                     ← 启动server

$ mysql -h 127.1                                                             ← 直接登陆
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new_password';           ← 修改密码
mysql> FLUSH PRIVILEGES;
{% endhighlight %}

通过 RPM 包安装时，默认是保存到 `/var/lib/mysql` 目录下。另外，需要注意，需要保证上级目录权限为 755，否则可能会报 Permission denied 错误。

当然也可以从 [MySQL 官网](http://dev.mysql.com/downloads/)、[MariaDB 官网](https://downloads.mariadb.org/) 或者 [Percona 官网](https://www.percona.com/) 下载相应的安装包，或者源码。对于 CentOS 7 来说，可以从 [MySQL Community Downloads](https://dev.mysql.com/downloads/) 下载 server、client、common、libs 对应的 RPM 包，也可从 [sohu 镜像](http://mirrors.sohu.com/mysql/) 下载，然后安装即可。

#### 密码配置

如果没有手动初始化数据直接启动，那么会将 root 密码打印到日志中，然后可以直接登陆；也可以修改配置文件跳过认证，也就是添加 ```skip-grant-tables``` 选项。

{% highlight text %}
----- 不建议使用如下方式修改密码
mysql> SET PASSWORD=PASSWORD('Root1234@');
mysql> FLUSH PRIVILEGES;

----- 可以使用如下方式修改，且不需要FLUSH，建议时候后者
mysql> SET password='Root1234@';
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new_password';
{% endhighlight %}

### 简单配置

MySQL 的配置文件保存在 /etc/my.cnf，包括了 datadir、pid-file 等的设置。另外，安装完之后 root 的密码为空，然后可以通过 mysql_secure_installation 来进行一些安全的配置，尤其是线上服务器，包括删除 test 数据库和默认创建的匿名用户。

为了方便登陆，可以设置 ~/.my.cnf，仅为了本地方便登陆，线上生产环境一定不要配置。

{% highlight text %}
[client]
user=root
password=YOUR_PASSWORD
socket=/tmp/mysql.sock
host=127.1
{% endhighlight %}

可以通过 mysqld \-\-verbose \-\-help \| head -15 查看配置文件的默认加载顺序，当然，也可以直接通过 mysql \-\-help \| grep my.cnf 过滤进行查看；配置文件中通过 # 表示注释。

安装完成之后，可以通过如下命令查看编译时使用的配置项。

{% highlight text %}
$ mysqld --verbose --help
{% endhighlight %}

### 编译安装

还是直接从官网下载源码即可，假设直接解压到 /home/foobar/Workspace/mysql 目录下，并将安装目录设置为 /opt/mysql-5.7 。

下面介绍如何通过源码进行安装，最简单的是通过 BUILD/compile-ARCH 脚本进行编译，当然，可以选择使用 autotools 或者 CMake 指定相应的参数进行安装，在此通过 CMake 安装。

通过 CMake 编译时，有两种编译方式：A) 内部编译，即在源码内部编译，此时会有一些编译产生的临时文件，影响代码结构；B) 外部编译，不会影响原代码的结构，在此采用外部编译。

所谓的外部编译，就是在一个新的目录下进行编译，此时编译的中间文件不会影响到源码的目录结构。源码可以直接从官网下载，三个发行版本的源码编译方式都类似，在此以 MySQL 为例。

{% highlight text %}
----- 安装依赖包，以及相关的开发库
# yum install gcc-c++ g++ ncurses-devel readline-devel zlib-devel

----- 源码安装
$ tar -xf mysql-boost-x.x.xx.tar.bz2
$ cd mysql-boost-x.x.xx && mkdir build && cd build       ← 新建编译的目录
$ cmake ..                                     \
    -DCMAKE_INSTALL_PREFIX=/opt/mysql-5.7      \         ← 安装根目录
    -DMYSQL_DATADIR=/opt/data                  \         ← 数据文件的默认路径
    -DSYSCONFDIR=/etc                          \         ← 配置文件目录，默认/etc/my.cnf
                                               \
    -DWITH_INNOBASE_STORAGE_ENGINE=ON          \         ← 使用InnoDB存储引擎
    -DWITH_BLACKHOLE_STORAGE_ENGINE=ON         \         ← 黑洞存储引擎
    -DWITH_EXAMPLE_STORAGE_ENGINE=ON           \         ← 使用存储示例
                                               \
    -DWITHOUT_FEDERATED_STORAGE_ENGINE=ON      \         ← 取消一些不常用的存储引擎
    -DWITHOUT_PARTITION_STORAGE_ENGINE=ON      \
    -DWITHOUT_ARCHIVE_STORAGE_ENGINE=ON        \         ← 日志记录和聚合分析，不支持索引
                                               \
    -DMYSQL_TCP_PORT=3306                      \         ← 默认使用的端口号
    -DMYSQL_UNIX_ADDR=/tmp/mysql.sock          \         ← 默认套接字文件路径
    -DMYSQL_USER=mysql                         \         ← 默认使用的用户
                                               \
    -DEXTRA_CHARSETS=all                       \         ← 是否支持额外的字符集
    -DDEFAULT_CHARSET=utf8                     \         ← 默认编码机制
    -DDEFAULT_COLLATION=utf8_general_ci        \         ← 设定默认语言的排序规则
                                               \
    -DWITH_READLINE=ON                         \         ← 使用readline库，支持类shell的快捷键以及历史命令
    -DENABLED_LOCAL_INFILE=ON                  \         ← 支持批量导入
    -DWITH_LIBWRAP=OFF                         \         ← 是否可以基于WRAP实现访问控制
    -DWITH_SSL=system                          \         ← 支持ssl会话，实现基于ssl的数据复制
    -DWITH_ZLIB=system                         \         ← 压缩库
    -DWITH_EMBEDDED_SERVER=OFF                 \         ← 取消embedded模式
    -DENABLE_PROFILING=ON                      \         ← 启用性能分析功能
    -DCMAKE_BUILD_TYPE=Debug                   \         ← 打开调试选项
    -DWITH_SAFEMALLOC=OFF                      \         ← 关闭safemalloc，经常会报错误，5.7不需要
    -DWITH_BOOST=../boost                                ← 5.7需要该boost库，直接下载含该库的包

----- 编译源码并安装
$ make -j`cat /proc/cpuinfo | grep 'processor' | wc -l`  ← 可以简单使用make并行编译，指定并发
# make install                                           ← 安装

----- 设置环境
$ export PATH=$PATH:/opt/mysql-5.7/bin                   ← 设置环境变量
$ vi /etc/ld.so.conf && sudo ldconfig                    ← 设置动态库目录
$ mysql_config                                           ← 可用于查看环境配置，很多MySQL软件依赖该命令

----- 源码安装时最好将build目录中的如下文件保存下
# cat install_manifest.txt | xargs rm                    ← 直接手动删除
{% endhighlight %}

<!--
#!/bin/bash
cmake ..                                       \
    -DCMAKE_INSTALL_PREFIX=/opt/mysql-5.7      \
    -DCMAKE_BUILD_TYPE=Debug                   \
    -DWITH_EMBEDDED_SERVER=OFF                 \
    -DWITH_EXAMPLE_STORAGE_ENGINE=ON           \
    -DWITH_BOOST=../boost

TODODO: UNIV_LRU_DEBUG 该编辑项如何打开
-->

注意，除了上述列举的选项外，可通过 ```cmake .. -LH``` 查看当前版本所支持的选项，如要重新编译可以直接运行 ```make clean; rm CMakeCache.txt; cmake ..``` 即可。

另外，MariaDB 不支持使用 ```-DWITH_DEBUG=1```，应该使用 ```-DCMAKE_BUILD_TYPE=Debug```；更多 MySQL 编译选项可以参考 [Source Configration Options](http://dev.mysql.com/doc/mysql-sourcebuild-excerpt/en/source-configuration-options.html) 。

在使用 make 进行编译时，可以通过 -j 选项指定使用多个 Jobs 进行编译，也就是通过多个 CPU 进行编译，这样可以加快编译速度。

源码编译完成后，默认会将程序保存在 sql/mysqld 目录下，然后即可以通过 make install 将其安装在 /opt/mysql-5.7 目录下，为了方便运行，可以将 /opt/mysql-5.7/bin 添加到环境变量 PATH 中。

### 客户端

在此仅简单记录下 mysql 命令行链接数据库时的常用命令，其源码保存在 client/mysql.cc，当尝试调试客户端时可以使用如下所述的 debug 模式排查问题，可以通过如下命令启动。

{% highlight text %}
$ mysql --debug='d:t:i:o,/tmp/mysql.trace'
{% endhighlight %}

如上所述，在本机测试时，为了方便登陆可以配置 ~/.my.cnf 文件。

{% highlight text %}
$ cat ~/.my.cnf
[client]
user=root
password=YOUR_PASSWD
socket=/tmp/mysql.sock
default-character-set=utf8
{% endhighlight %}

常见的参数选项：

{% highlight text %}
--prompt
    可通过该参数设置 mysql 提示符中显示当前用户、数据库、时间等信息；例如，可以通过如下方式启动：
    $ mysql -u root -p --prompt="\\u@\\h:\\d >"

--column-type-info
    显示元数据信息。

-A, --no-auto-rehash
    不预读数据库信息（默认操作），切换数据库时不会预读，因此切换更快。可以使用反之为--auto-rehash，此时可以补全表明/库名。

-D, --database
    直接指定数据库。

-B, --batch
    不会输出间隔。

反引号
    通过反引号包含的字符串表示非关键字。

(NUM)
    int() 表示的是显示的长度，与保存字节数无关；char() 指定字符串长度，长度固定，varchar 表示可变字符串。
{% endhighlight %}

当数据较多无法一屏显示时，可以通过 pager more 设置，此时会通过 more 命令进行分页。

## 运行实例

如下，简单以源码安装时的路径为例，如果是通过类似 YUM 安装的，则可以直接替换可执行文件的路径即可，不再赘述。

### 数据初始化

第一次运行时需要做些初始化操作，主要是需要创建一些所必须的系统数据库以及相应的表，例如 mysql、preformance_schema、information_schema 等。

{% highlight text %}
----- 5.6以及之前的版本
$ /opt/mysql-5.6/script/mysql_install_db --no-defaults --basedir=/opt/mysql-5.6 \
    --datadir=/home/foobar/Workspace/mysql/data --user=mysql

----- 5.7以及之后的版本，使用--initialize时密码保存在~/.mysql_secret
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/home/foobar/Workspace/mysql/data --user=mysql
{% endhighlight %}

<!--
/opt/mysql-5.7/bin/mysql_install_db --no-defaults --basedir=/opt/mysql-5.7 \
    --datadir=/home/andy/Workspace/databases/mysql/data-mysql
-->

如果不通过 \-\-user 参数指定用户名，则使用默认的当前用户，此时不需要设置 datadir 目录的权限，否则需要设置相应权限。

basedir 指定 MySQL 的安装目录；datadir 指定数据的存放位置；可以使用 \-\-defaults-file=my.cnf 指定默认的配置文件，可以使用相对路径，默认相对于 datadir。在 share 子目录下，包含了一些配置文件，以及初次安装运行需要的初始数据库文件比如 mysql_system_tables.sql。

注意，如果要使用参数指定默认配置文件，则 \-\-defaults-file 选项应该放置在第一个参数，而且 datadir、basedir 必须使用绝对路径。

### 单实例运行

新建配置文件 data/my.cnf (可以为空)，并通过如下命令启动/停止/连接服务器。

{% highlight text %}
----- 1. 切换到工作目录
$ cd /home/foobar/Workspace/mysql

----- 2. 启动单个MySQL实例，前台运行，如果要后台运行直接添加&即可
$ /opt/mysql-5.7/bin/mysqld --defaults-file=data/my.cnf  \
    --datadir=/home/foobar/Workspace/mysql/data          \
    --basedir=/opt/mysql-5.7 --skip-networking --debug
$ /opt/mysql-5.7/bin/mysqld_safe --defaults-file=data/my.cnf  \
    --datadir=/home/foobar/Workspace/mysql/data          \
    --basedir=/opt/mysql-5.7 --skip-networking --debug &

----- 3. 如下命令连接到数据库，5.6之前为密码为空，5.7密码保存在~/.mysql_secret
$ /opt/mysql-5.7/bin/mysql -uroot -p'init-password' -S /tmp/mysql.sock

----- 4. 可以通过如下命令关闭、查看状态
$ /opt/mysql-5.7/bin/mysqladmin -uroot -p'init-password' shutdown
$ /opt/mysql-5.7/bin/mysqladmin -uroot -p'init-password' status
{% endhighlight %}

<!--
/opt/mysql-5.7/bin/mysqld --defaults-file=data-mysql/my.cnf   \
    --datadir=/home/andy/Workspace/databases/mysql/data-mysql \
    --basedir=/opt/mysql-5.7 --skip-networking --debug
/opt/mysql-5.7/bin/mysqld_safe --defaults-file=data-mysql/my.cnf   \
    --datadir=/home/andy/Workspace/databases/mysql/data-mysql \
    --basedir=/opt/mysql-5.7 --skip-networking --debug
-->

在通过上述命令启动服务器时，添加了 skip-networking 参数，也就是不启动 TCP 网络，因此只能通过 sock 连接。

需要注意的是，如果不使用 \-\-defaults-file 选项，mysqld 会自动读取 /etc/my.cnf 配置文件，如果默认值与期望值不符，从而可能会导致部分错误，当然可以删除该配置文件，不过不建议。

**推荐** 可以将对应的启动、关闭命令设置为 alias ，这样可以简化启动方式，同时仍然可以动态添加参数，如直接再在命令的末尾添加 \-\-debug 。

### 多实例运行

多实例主要是端口以及保存的数据目录不同，通过简单设置这些参数，可以在同一台 server 上启动多个实例，下面启动一个端口号为 3307 的实例，其中配置文件 mysql3307.cnf 的内容如下。

{% highlight text %}
$ cat mysql3307.cnf
[mysqld]
port = 3307
server-id = 3307
basedir = /opt/mysql-5.7
socket = /tmp/mysql3307.sock
pid-file = /home/foobar/Workspace/mysql/data3307/mysql3307.pid
datadir = /home/foobar/Workspace/mysql/data3307
{% endhighlight %}

然后，可以通过如下命令设置并启动一个新的 MySQL 实例。

{% highlight text %}
----- 1. 创建目录
$ cd /home/foobar/Workspace/mysql && mkdir data3307

----- 2. 如上，初始化实例
$ /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/home/foobar/Workspace/mysql/data3307 --user=mysql

----- 3. 启动实例
$ /opt/mariadb/bin/mysqld --defaults-file=data3307/mysql3307.cnf

----- 4. 连接数据库
$ mysql -uroot -h127.1 -P3307

----- 5. 关闭数据库
$ /opt/mariadb/bin/mysqladmin -uroot -S /tmp/mysql3307.sock shutdown
{% endhighlight %}

对于多实例，可以通过上述的方法创建实例，不过管理操作还是略微有点复杂，实际上可以直接通过 mysqld_multi 命令管理。

另外，可以通过 mysqld_multi \-\-example 查看示例配置文件，或者使用 multi.cnf 。

{% highlight text %}
$ cat multi.cnf
[mysqld_multi]
mysqld = /opt/mysql-5.7/bin/mysqld
mysqladmin = /opt/mysql-5.7/bin/mysqladmin
user = root

[mysqld3307]
socket = /tmp/mysql3307.sock
port = 3307
pid-file = /home/foobar/Workspace/mysql/data3307/mysql3307.pid
datadir = /home/foobar/Workspace/mysql/data3307
basedir = /opt/mysql-5.7
server-id=3307

[mysqld3308]
socket = /tmp/mysql3308.sock
port = 3308
pid-file = /home/foobar/Workspace/mysql/data3308/mysql3308.pid
datadir = /home/foobar/Workspace/mysql/data3308
basedir = /opt/mysql-5.7
server-id=3308
{% endhighlight %}

如下是一些常见的操作。

{% highlight text %}
$ cd /home/foobar/Workspace/mysql
$ /opt/mysql-5.7/bin/mysqld_multi --defaults-file=multi.cnf report     ← 查看多个实例状态
$ /opt/mysql-5.7/bin/mysqld_multi --defaults-file=multi.cnf start
$ /opt/mysql-5.7/bin/mysqld_multi --defaults-file=multi.cnf stop
{% endhighlight %}


## 调试程序

如果要调试需要在编译时添加 -DCMAKE_BUILD_TYPE=Debug 选项，可以通过 gdb 或者使用 dbug 打印调试信息。

### 使用 GDB

可以通过如下的方式加载程序，然后运行。

{% highlight text %}
$ gdb -args /opt/mariadb/bin/mysqld --defaults-file=......
(gdb) run
{% endhighlight %}

如果 MySQL 已经启动，此时可以通过 gdb 的 attach PID 命令来调试，一般会阻塞在系统调用 poll() 处；调试结束通过 detach 命令退出。注意，要先 **设置断点** 。

如果通过 CMake 进行编译，往往会采用绝对路径，可以通过 readelf -p .debug_str 查看，此时设置/查看断点时应该使用绝对路径。可以修改/添加如下文件。

{% highlight text %}
$ cat CMakeLists.txt                         # 添加如下内容
... ...
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/gcc_debug_fix.sh.in"
    "${PROJECT_BINARY_DIR}/gcc_debug_fix.sh"
    @ONLY)
... ...

$ cat cmake/mysql_add_executable.cmake       # 添加如下内容
ADD_EXECUTABLE(... ...)
SET_TARGET_PROPERTIES(${target} PROPERTIES
    RULE_LAUNCH_COMPILE "${PROJECT_BINARY_DIR}/gcc_debug_fix.sh")
... ...

$ cat gcc_debug_fix.sh.in                    # 添加如下文件，需要设置可执行
#!/bin/sh
PROJECT_BINARY_DIR="@PROJECT_BINARY_DIR@"
PROJECT_SOURCE_DIR="@PROJECT_SOURCE_DIR@"

# shell script invoked with the following arguments
# $(CXX) $(CXX_DEFINES) $(CXX_FLAGS) -o OBJECT_FILE -c SOURCE_FILE

# extract parameters
SOURCE_FILE="${@: -1:1}"
OBJECT_FILE="${@: -3:1}"
COMPILER_AND_FLAGS=${@:1:$#-4}

# make source file path relative to project source dir
SOURCE_FILE_RELATIVE="${SOURCE_FILE:${#PROJECT_SOURCE_DIR} + 1}"

# make object file path absolute
OBJECT_FILE_ABSOLUTE="$PROJECT_BINARY_DIR/$OBJECT_FILE"

cd "$PROJECT_SOURCE_DIR"

# invoke compiler
exec $COMPILER_AND_FLAGS -c "${SOURCE_FILE_RELATIVE}" -o "${OBJECT_FILE_ABSOLUTE}"
{% endhighlight %}

编译方式仍如上所示，相关内容可以参考 [Make gcc put relative filenames in debug information](http://www.stackoverflow.com/questions/9607155/make-gcc-put-relative-filenames-in-debug-information) 。

### 调试技巧

为了比较好的查看变量内容，可以设置如下。

{% highlight text %}
(gdb) set print vtbl on         ← 用比较规整的格式来显示虚函数表
(gdb) ptype obj/class/struct    ← 查看obj/class/struct的成员，但是会把基类指针指向的派生类识别为基类
(gdb) set print object on       ← 查看派生对象的真实类名
(gdb) set print pretty on       ← 以树形打印对象的成员，可以清晰展示继承关系
{% endhighlight %}

链接到 mysqld 后，会阻塞在 poll()，然后设置调试入口，在此简单列举一下常见的调试入口点。

* handle_connections_sockets() @ mysqld.cc<br>
    等待接收处理新的链接。
* do_handle_one_connection() @ sql_connect.cc<br>
    接下来的处理过程为处理链接，注意该处理函数与链接的处理方式有关。
* do_command() @ sql_parse.cc; dispatch_command() @ sql_parse.cc<br>
    接着是处理接收到的 SQL，其中有一个变量 THD thd @ sql_class.h 会经常使用，该结构体中包含了当前线程的相关信息。

如下是简单的在 dispatch_command() 断点处打印的信息，用 @ 简单标示断点信息。

{% highlight text %}
# @ dispatch_command()
(gdb) p thd.net                 ← 查看网络的信息
(gdb) p packet                  ← 请求的命令
{% endhighlight %}

### 使用 DBUG

对于 MySQL 需要在编译时添加调试选项 -DCMAKE_BUILD_TYPE=Debug ，然后在启动时使用 \-\-debug 参数，默认会启动所有的调试选项，调试的信息会保存在 /tmp/mysqld.trace 文件中。

可以在启动时使用 \-\-debug[=debug_options] 添加调试选项，默认值是 d:t:i:o,/tmp/mysqld.trace 。选项的格式为 [field1]:[field2]... 每个 field 之间用冒号分割，各个 field 的内容为 flag[,modifier,modifier...] 。

常用的标记有：

{% highlight text %}
d
    允许对当前状态从DBUG_<N>宏输出。可能跟着一列关键词，这些关键词仅对那些带有关键词的DBUG宏选择输出。
    一个空的关键词列意味着对所有宏输出。
t
    允许函数调用/退出跟踪行。可能跟着一个给出最大跟踪级别的数字列（只含一个修改量），超过这个数字，
    调试中或跟踪中的宏不能产生任何输出。 默认为一个编译时间选项。
i
    对调试或跟踪输出的每一行用PID或线程ID识别进程。
o
    重定向调试器输出流到指定文件，默认输出是stderr 文件。
{% endhighlight %}

注意：调试输出选项是在 mysqld_main() 中的 handle_options() 返回后开始输出的，之前应该是还没有初始化，所以 ......

另外，在运行的时候也可以通过，如下的方式修改，日志的保存位置可以查看该变量。

{% highlight text %}
mysql> SHOW VARIABLES LIKE 'debug';
mysql> SET GLOBAL debug = 'd:t:i:o,/tmp/mysqld.trace';
mysql> SET SESSION debug = 'd:t:i:o,/tmp/mysqld.trace';
{% endhighlight %}

其中，DBUG_RETURN() 后面的数字对应的是行号。

其它与 dbug 相关的内容可以参考 [dbug 源码](http://sourceforge.net/projects/dbug/)，其中有一个 example.c 文件，可以将其复制到 src 目录下，最简单的方式是直接通过 gcc example.c dbug.c 编译，编译后运行将直接输出到终端上；当要取消调试选项时只需要添加宏 #define DBUG_OFF。

注意：MySQL 的 dbug 库是经过修改的，会有稍微的差别。

## 打印执行的SQL

通常在测试某些功能时，希望可以直接输出所执行的 SQL，通常可以使用如下的两种方式。

### general log

可以通过 general_log 打开日志，记录所有的执行日志，当然只用于测试，生产环境不要打开。

{% highlight text %}
mysql> show global variables like 'general%';
mysql> set global general_log=1;                  # 打开查询日志
{% endhighlight %}

默认日志保存在 $datadir/$general_log_file 文件中。

另外，slow query log 用于记录执行时间超过 long_query_time 的 SQL 。

### tcpdump

可以通过 tcpdump 截取 MySQL 报文，假设使用的时 lo 接口；需要注意的是：使用本地连接可能会直接用 Linux sock 链接，通过客户端创建连接时需要指定 -h 127.1。

{% highlight text %}
# tcpdump -i lo -s 0 -l -w - dst port 3306 | strings

其中的选项为：
-s 0     将截取报文的长度设置为默认值 65536 bytes
-i lo    指定监听的网络接口
-l       输出变为缓冲行形式
-w       直接将包写入文件中，并不分析和打印出来，在此输出到标准输入

# tcpdump -i lo -s 0 -l -w - dst port 3306 | strings | perl -e '
while(<>) { chomp; next if /^[^ ]+[ ]*$/;
  if(/^(SELECT|UPDATE|DELETE|INSERT|SET|COMMIT|ROLLBACK|CREATE|DROP|ALTER)/i) {
    if (defined $q) { print "$q\n"; }
    $q=$_;
  } else {
    $_ =~ s/^[ \t]+//; $q.=" $_";
  }
}'
{% endhighlight %}

## 参考

其中调试时的相对路径设置可以参考 [stackflow](http://stackoverflow.com/questions/9607155/make-gcc-put-relative-filenames-in-debug-information) 或者 [本地文档](/reference/databases/mysql/make gcc put relative filenames in debug information.mht) 。


{% highlight text %}
{% endhighlight %}
