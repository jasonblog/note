---
title: MySQL Sandbox
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,sandbox,测试
description: MySQL Sandbox 是个用于在本地快速部署 MySQL 服务的工具，包括了不同版本，不同模式的部署，例如单节点、主备部署、一主多备等等；如果配合 Gearman，甚至可以完成分布式的部署。该程序通常用于测试，是用 Perl 语言编写的，在此，仅简单介绍下如何使用。
---

MySQL Sandbox 是个用于在本地快速部署 MySQL 服务的工具，包括了不同版本，不同模式的部署，例如单节点、主备部署、一主多备等等；如果配合 [Gearman](http://gearman.org/)，甚至可以完成分布式的部署。

该程序通常用于测试，是用 Perl 语言编写的，在此，仅简单介绍下如何使用。

<!-- more -->

![mysql sandbox logo]({{ site.url }}/images/databases/mysql/sandbox-logo.png "mysql sandbox logo"){: .pull-center }

## 简介

MySQL-Sandbox 会在同一机器上部署不同的实例，实际上也就是安装时对应的 data-directory、Port、Unix Socket 不同，默认上述的三个参数与版本有关，以 MySQL-5.0.17 为例，对应如下。

{% highlight text %}
Data-Directory: $SANDBOX_HOME/msb_5.0.17/data
          Port: 5017
   Unix Socket: /tmp/mysql_5017.sock
{% endhighlight %}

另外，常见的命令简介如下。

{% highlight text %}
make_sandbox
  最简单创建sandbox，也就是一个实例；
low_level_make_sandbox
  创建单个sandbox，可以使用多个选项，不过不会直接使用；
make_replication_sandbox
  创建master-slave架构；
make_multiple_sandbox
  创建相同版本的sandbox；
make_multiple_custom_sandbox
  创建不同版本的sandbox；
make_sandbox_from_source
  从源码编译的build目录创建一个sandbox；
make_sandbox_from_installed
  从已安装的二进制文件创建一个sandbox；
sbtool
  sandbox管理工具。
{% endhighlight %}

具体用法参考源码目录下的 README.md 文档，非常详细，在此简单介绍如下。

### 安装

对于 Perl 程序可以通过 cpan 安装，当然需要 root 权限。

{% highlight text %}
# cpan
cpan> install MySQL::Sandbox
{% endhighlight %}

源码包可以直接从 [github](https://github.com/datacharmer/mysql-sandbox/releases) 上下载，然后通过如下方式安装。

注意，在如下执行 ```make test``` 时，会依赖 Perl 的测试框架，可通过 ```yum install perl-Test-Simple``` 命令进行安装。

{% highlight text %}
----- 解压、编译、安装
$ tar -xzf MySQL-Sandbox-x.x.x.tar.gz
$ cd MySQL-Sandbox-x.x.x
$ perl Makefile.PL PREFIX=/opt/mysql-sandbox
$ make
$ make test
$ make install

----- 设置$PERL5LIB以及$PATH变量
$ export PATH=$PATH:/opt/mysql-sandbox/bin
$ export PERL5LIB=/opt/mysql-sandbox/share/perl5
{% endhighlight %}

另外，建议使用普通用户进行测试，如果要使用 root 用户，则需要设置 ```export SANDBOX_AS_ROOT=1``` 环境变量。

## 安装方式

可直接下载二进制包，然后通过如下方式安装，如上所述，会安装到 ```$SANDBOX_HOME/msb_X_X_XX``` 目录下，如果未设置，默认 ```SANDBOX_HOME=$HOME/sandboxes``` 。

可以通过三种方式进行安装，简单介绍如下。

### 二进制包

可以直接从 [官网](https://dev.mysql.com/downloads/mysql/) 下载二进制包，例如 [mysql-5.6.35-linux-glibc2.5-x86_64.tar.gz](https://dev.mysql.com/downloads/mysql/5.6.html#downloads) ，包含了已经编译好的二进制包。

{% highlight text %}
----- 单个MySQL服务
$ make_sandbox /path/mysql-x.x.x.tar.gz

----- 主备方式
$ make_replication_sandbox /path/mysql-x.x.x.tar.gz

----- 主备循环
$ make_replication_sandbox --circular=5 /path/mysql-x.x.x.tar.gz

----- 多个MySQL服务
$ make_multiple_sandbox /path/mysql-x.x.x.tar.gz

----- 多个不同版本的MySQL服务
$ make_multiple_sandbox /path/mysql-x.x.x.tar.gz /path/mysql-x.x.x-other-tar.gz /path/another-tar.gz
{% endhighlight %}

默认会将上述二进制包解压到当前目录，并将目录重命名为版本号，例如 5.6.35 。

### 源码包

可以直接下载源码，然后编译，但是不想安装，可以通过如下命令搭建。

{% highlight text %}
$ make_sandbox_from_source {SOURCE_DIRECTORY} {sandbox_type} [options]
{% endhighlight %}

其中 ```sandbox_type``` 参数指定了类型，包括了 single、multiple、replication、circular ，示例如下。

{% highlight text %}
$ ./configure && make
$ make_sandbox_from_source $PWD single
{% endhighlight %}

也可以使用如下参数。

{% highlight text %}
$ make_sandbox_from_source $HOME/build/5.6.35 single --export_binaries --check_port 
$ make_sandbox_from_source $HOME/build/5.6.35 replication --how_many_slaves=5
{% endhighlight %}

### 已安装包

可将不同版本的 MySQL 放在 ```$SANDBOX_BINARY``` 环境变量指定目录下，然后使用更简单的方式。

{% highlight text %}
$ export SANDBOX_BINARY=$HOME/mybin
$ ls $SANDBOX_BINARY
5.6.35 5.7.17
$ make_sandbox 5.7.17
{% endhighlight %}

## 使用

在安装到 ```$SANDBOX_HOME``` 目录下之后，会生成一列的操作脚本。

### 服务器操作

单个实例对应命令如下：

{% highlight text %}
start
  启动服务，在数据目录下创建.pid文件，在/tmp目录下创建socket文件；如果.pid文件存在则立即退出。
stop
  停止服务，如果.pid文件不存在则立即退出。
clear
  通过stop停止服务，并删除test数据库中的文件。
send_kill
  发送SIGTERM信号，如果MySQL服务没有响应则发送SIGKILL信号。
use
  通过客户端链接到服务器，默认使用my_sandbox.cnf中的配置，用户名密码分别为msandbox/msandbox、root/msandbox 。
use "command"
  执行命令。
{% endhighlight %}

而对于多个实例，只需要在上述的操作中添加 ```_all``` 即可，例如 start_all stop_all clear_all use_all send_kill_all 。

在通过客户端链接到服务器时，对于一主多备可以使用 m s1 s2 s3 命令链接，而多个节点则可以使用 n1 n2 n3 命令。

### Sandbox Tools

提供了一个 sbtool 工具用于一些常见的操作。

{% highlight text %}
----- 将数据复制到不同环境下
$ make_sandbox 5.1.33
$ make_sandbox 5.1.34
$ sbtool -o copy \
    -s $HOME/sandboxes/msb_5_1_33 \
    -d $HOME/sandboxes/msb_5_1_34

----- 移动到其它目录下
$ make_sandbox 5.1.33
$ sbtool -o move \
    -s $HOME/sandboxes/msb_5_1_33 \
    -d /some/path/a_fancy_name

----- 新的测试脚本
$ cat << EOF >> check_single_server.sb
shell:
command  = make_sandbox 5.1.30 --no_confirm
expected = sandbox server started
msg = sandbox creation

sql:
path = $SANDBOX_HOME/msb_5_1_30
query = select version()
expected = 5.1.30
msg = checking version

shell:
command = $SANDBOX_HOME/msb_5_1_30/stop
expected = OK
msg = stopped
EOF
$ export TAP_MODE=1
$ perldoc Test::Harness::TAP
$ test_sandbox --tests=user \
    --user_test=t/check_single_server.sb

----- 也可以在一个测试中嵌套测试
$ prove -v t/06_test_user_defined.t

----- 创建复制树
$ make_multiple_sandbox --how_many_nodes=12 5.0.77
$ sbtool -o tree \
    --tree_dir=$HOME/sandbox/multi_msb_5_0_77 \
 --master_node=1 \
 --mid_nodes='2 3 4' \
 --leaf_nodes='5 6|7 8 9|10 11 12'
$ cd $HOME/sandbox/multi_msb_5_0_77
$ ./n1 -e "show slave hosts"
$ ./n2 -e "show slave hosts"

----- 检查端口
$ sbtool -o ports
$ sbtool -o ports --only_used

----- 检查range
$ sbtool -o range --min_range=5130

----- 保存禁止删除，如下删除时会报错
$ sbtool -o preserve \
    -s $HOME/sandboxes/msb_5_1_33/
$ $HOME/sandboxes/msb_5_1_33/clear

----- 允许删除
$ sbtool -o unpreserve \
    -s $HOME/sandboxes/msb_5_1_33/
$ $HOME/sandboxes/msb_5_1_33/clear

----- 删除所有，实际可以直接删除目录，需要使用绝对路径
$ sbtool -o delete \
    -s $HOME/sandboxes/msb_5_1_33/
{% endhighlight %}

### 其它

对于 mysqldump、mysqlbinlog、mysqladmin 可以通过如下方式启动，直接使用 my_sandbox.cnf 中的配置，不需要输入密码。

{% highlight text %}
./my sqlbinlog ./data/mysql-bin.000001 | less
./my sqldump dbname > dumpname
./my sqladmin extended-status
{% endhighlight %}

#### 常用示例

{% highlight text %}
----- 搭建不同版本MySQL服务
$ make_multiple_custom_sandbox 5.6.16 5.7.17

----- 也可以添加选项，可以通过--help查看可用参数
$ make_sandbox /path/mysql-x.x.x.tar.gz \
    --my_file={small|medium|large|huge}
$ make_sandbox /path/mysql-x.x.x.tar.gz \
    --interactive

----- 检查备库
./check_slaves

----- 所有节点执行命令
./use all "SELECT @@server_id"

----- 重启时指定参数
./restart --key_buffer=20M
{% endhighlight %}

<!--
--check_port

$SANDBOX_HOME
    指定测试数据目录所在位置。
-->



## 参考

详细可以参考官方网站 [mysqlsandbox.net](http://mysqlsandbox.net/index.html)，源码包可以直接从 [github](https://github.com/datacharmer/mysql-sandbox/releases) 上下载；另外，可以参考不错的 [PPT](/reference/databases/mysql/mysql_sandbox.pdf) 。

{% highlight text %}
{% endhighlight %}
