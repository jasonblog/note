---
Date: October 19, 2013
title: MySQL 常用工具
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,monitor,监控,percona tools,sysbench,tcprstat,压测,响应时间,工具
description: 工欲善其事，必先利其器；在本文章中介绍了与 MySQL 相关的一些常见的工具，列入 percona 工具集，常见的压测工具 sysbench，服务器响应时间 tcprstat。包括了它们使用方法，详细的执行过程等。
---

在此介绍一些 MySQL 中常用的工具，包括了 percona-tools、sysbench 等工具。

接下来我们看看。

<!-- more -->

部分软件会依赖 automake autoconf libtool 等工具，需要在编译之前进行安装。

## percona-tools

![Percona Tools Logo]({{ site.url }}/images/databases/mysql/percona-tools-logo.png "Percona Tools Logo"){: .pull-right width="300px"}

该工具集基本上是 perl 脚本，可以直接从官方网站 [percona-toolkit](https://www.percona.com/downloads/percona-toolkit/) 上下载源码，或者 [github](https://github.com/percona/percona-toolkit/releases) ，文档也可以直接从上述的网站下载，只需要填写个表格即可。

为了方便下载，本地保存了一个版本 [percona-toolkit-2.2.19.tar.gz](/reference/mysql/percona-toolkit-2.2.19.tar.gz) 以及 [Percona-Toolkit-2.2.19.pdf](/reference/mysql/Percona-Toolkit-2.2.19.pdf) 。

该工具是 [Aspersa](http://aspersa.googlecode.com/svn/html/index.html) 和 Aspersa 两个工具的合并修改过来的，这是一套脚本工具，不过保存在 google code 中，所以需要翻墙；也可以查看一下 [本地版本](/reference/mysql/aspersa.tar.bz2) 以及 [本地保存文档](/reference/mysql/aspersa) 。

接下来我们看看如何使用这一工具集，不过在此介绍其中的一部分工具的使用，剩余的一部分工具将会在其它文章中继续介绍。

安装方式可以从源码包中的 INSTALL 文件中查看，或者简单执行如下命令。

{% highlight text %}
----- 安装依赖的perl三方包
# yum install perl-Digest-MD5

----- 安装percona-tools
# perl Makefile.PL
# make
# make test
# make install
{% endhighlight %}




## sysbench

<!--
一个不错的Sysbench示例
https://wiki.gentoo.org/wiki/Sysbench

ScaleArc: 使用 sysbench 对数据库进行压力测试
http://www.oschina.net/translate/scalearc-benchmarking-sysbench
-->

sysbench 用于基准测试，包括了：CPU 性能、磁盘 IO 性能、调度程序性能、内存分配及传输速度、POSIX 线程性能、数据库性能(OLTP基准测试，支持 MySQL、PostgreSQL、Oracle)等。

![Sysbench Logo]({{ site.url }}/images/databases/mysql/sysbench-logo.jpg "Sysbench Logo"){: .pull-center }

其命令格式如下：

{% highlight text %}
$ sysbench [common-options] --test=name [test-options] command
{% endhighlight %}

其中 \-\-test 指定测试工具，command 包括了 prepare、run、cleanup、help，prepare 用于准备数据，常用于 fileio、oltp 测试，其它用于运行、清理、查看帮助。

### 安装

最新的是在 github 上维护的 [akopytov/sysbench](https://github.com/akopytov/sysbench)，下载源码可以通过如下方式编译。

{% highlight text %}
$ ./autogen.sh && ./configure && make && cd sysbench
$ ./sysbench --help                                        // 查看帮助
$ ./sysbench --test=cpu help                               // 查看标准测试的帮助
{% endhighlight %}

在执行 ./configure 时，如果通过 mysql_config 无法找到库、头文件目录等，可以通过 \-\-with-mysql-includes=...、\-\-with-mysql-libs=... 进行配置。

如果想要支持 pgsql 和 Oracle，在配置时需要添加 \-\-with-pgsql 或者 \-\-with-oracle 选项，否则默认只支持 MySQL，相关的驱动源码保存在 sysbench/drivers 目录下。


### 基准测试

介绍一些常见的基准测试，关于 OLTP 单独再述。

#### CPU 性能测试

使用 64 位整数，测试计算质数直到某个最大值所需要的时间。

{% highlight text %}
$ ./sysbench --test=cpu help
$ ./sysbench --test=cpu --cpu-max-prime=2000 run
{% endhighlight %}


#### 线程基准测试

测试线程调度器的性能，对于高负载情况下测试线程调度器的行为非常有用。

{% highlight text %}
$ ./sysbench --test=threads help
$ ./sysbench --test=threads --num-threads=500 --thread-yields=100 --thread-locks=4 run
{% endhighlight %}

#### 内存基准测试

用于测试内存的连续读写性能。

{% highlight text %}
$ ./sysbench --test=memory help
$ ./sysbench --test=memory --memory-block-size=8k --memory-total-size=1G run
{% endhighlight %}


#### 互斥锁基准测试

互斥锁用于在多线程编程中，防止两个线程同时对同一公共资源，如全局变量，进行读写的机制来保证共享数据操作的完整性。测试互斥锁的性能，方式是模拟所有线程在同一时刻并发运行，并都短暂请求互斥锁。

{% highlight text %}
$ ./sysbench --test=mutex help
$ ./sysbench --test=mutex --num-threads=100 --mutex-num=1000 \
    --mutex-locks=100000 --mutex-loops=10000 run
{% endhighlight %}


#### 文件 IO 基准测试

测试包括了 prepare、run、cleanup，主要用于准备测试需要文件，运行测试，清理。注意，生成的数据文件至少要比内存大，否则操作系统可能会缓存大部分的数据，导致测试结果无法体现 IO 密集型的工作负载。


### OLTP

之前的版本 oltp 是内建的版本，现在采用的是 lua 脚本实现，保存在 tests/db 目录下，通过这种方式，使得需要定制个性化测试时不用再重新编译。

在此只针对 MySQL，进行测试；该驱动保存在源码的 drivers/mysql 目录下，关于驱动相关的参数，例如 mysql-host、mysql-socket 等，可以查看源码。如果不指定则默认使用 sbtest 数据库，此时需要手动创建该数据库。

编译完成之后，直接到源码的的 sysbench 目录下执行如下命令即可。

{% highlight text %}
$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=innodb \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 prepare

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=innodb \
    --num-threads=16 --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 run

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=innodb \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 cleanup
{% endhighlight %}

如上的命令，prepare 只需要执行一次，然后可以执行多次 run 即可。其它一些常见的参数可以参考：

{% highlight text %}
--num-threads=1         线程数
--report-interval=5     每隔5秒打印一次统计结果
{% endhighlight %}

对于 MyISAM 的测试可以直接将上述的存储引擎更换为 myisam 即可，命令详见如下。

{% highlight text %}
$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=myisam \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 prepare

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=myisam \
    --num-threads=16 --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 run

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=myisam \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 cleanup
{% endhighlight %}

### 源码相关

相关的驱动文件保存在 sysbench/drivers 目录下，测试相关模块保存在 sysbench/tests 目录下，下面看下与 DB 相关的脚本。

#### tests/db

首先看下 common.lua 这个文件，该文件用于读取参数、执行 prepare、执行 cleanup；其中后两者分别对应了相应的函数，prepare 会调用 create_insert 创建表并插入数据；而清除则是简单删除掉之前创建的表。

接下来看看 oltp.lua 文件，主要有两个函数：A) thread_init() 会调用 set_vars() 设置与 oltp 相关的参数，初始化每个线程相关的参数；B) event() 函数用于处理每次调用，包括了事务执行包含了那些语句，及其次数等。

其中一个比较简单的示例可以参考 oltp_simple.lua 文件。

## tcprstat

关于该工具详细可以参考 [通过 tcprstat 工具统计应答时间](/post/linux-tcprstat.html) 的内容，包括了详细代码的参考。

如文章中所示，如果服务器监听 127.1 会导致无法使用，可以通过如下方法解决。

{% highlight text %}
----- 1. 设置一个本地域的浮动IP地址
# ifconfig lo:1 127.168.1.1 netmask 255.0.0.0

----- 2. 配置文件中添加如下内容，然后重启服务器
[mysqld]
bind-address=127.168.1.1

----- 3. 通过如下方式启动tcprstat
$ tcprstat -l 127.1 -p 3306 -t 1 -n 0
{% endhighlight %}

按照如上方式，可以基本解决本地测试的 bug 。


## 其它

实时监控可以直接参考 [orzdba](http://code.taobao.org/p/orzdba/src/trunk/)，可直接下载安装使用；另外相关的还有 [Percona Monitoring Plugins](https://www.percona.com/downloads/percona-monitoring-plugins/LATEST/)，也就是 zabbix、nagios、cacti 插件。

[mycli](http://mycli.net/) 是一个使用 Python 编写的命令行终端，可以支持自动补全，以及高亮显示。

<!--
Galera replication for MySQL
http://www.gpfeng.com/?p=603
-->

{% highlight text %}
{% endhighlight %}
