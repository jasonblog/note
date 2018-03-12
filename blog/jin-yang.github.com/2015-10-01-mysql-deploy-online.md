---
title: MySQL 线上部署
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,线上部署
description: 主要介绍下 MySQL 如何在线上进行部署。
---

主要介绍下 MySQL 如何在线上进行部署。

<!-- more -->

![mysql deploy online logo]({{ site.url }}/images/databases/mysql/deploy-online-logo.png "mysql deploy online logo"){: .pull-center width="50%" }

可以通过如下步骤部署 MySQL 服务。

### 创建用户

首先创建 MySQL 用户和组。

{% highlight text %}
# groupadd -r mysql
# useradd -r -g mysql -M -s /sbin/nologin mysql
参数:
    -r 添加系统用户组、系统用户
    -g 指定用户需要添加到的group
    -M 不创建home目录
    -s 指定使用的shell
{% endhighlight %}

当出现权限问题时，可以通过如下方式进行调试。

{% highlight text %}
# usermod -s /bin/bash mysql
# su mysql -c 'echo "test" > /opt/data/test'
# su mysql -c 'cat /opt/data/test'
# usermod -s /sbin/nologin mysql
{% endhighlight %}

### 创建数据目录

创建数据目录，另外需要保证 mysql 用户对安装目录有读权限，否则加载插件时可能失败。

{% highlight text %}
# mkdir -pv /opt/data
# chown -R mysql:mysql /opt/data
# chown -R --verbose mysql:mysql /opt/mysql-5.7
{% endhighlight %}

### 数据库初始化操作

创建系统自带的数据库和表，如果后续误删除，可以直接再次执行如下命令。

{% highlight text %}
# /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/opt/data --user=mysql
{% endhighlight %}

### 添加配置文件

如下只是一个简单的配置，详细配置文件可以查看。

{% highlight text %}
# vi /etc/my.cnf
[client]
password       = your_password
port           = 3306
socket         = /tmp/mysql.sock

[mysqld]
user            = mysql
port            = 3306
socket          = /tmp/mysql.sock
basedir         = /opt/mysql-5.7
datadir         = /opt/data
pid-file        = /opt/data/mysqld.pid
ft_min_word_len = 4
max_allowed_packet = 32M
character-set-server = utf8
skip-name-resolve
{% endhighlight %}

### 启动脚本

注意，在编译后源码的 support-files 目录下，包括了一些常见的脚本。

如果使用的是 SysV 系统，可以拷贝服务启动脚本到 /etc/init.d/mysqld 目录下，不过现在一般采用 systemd 管理服务；当然，可以后直接通过该脚本启动服务。

{% highlight text %}
# cp support-files/mysql.server /etc/init.d/mysqld
# service mysqld start

# /opt/mysql-5.7/support-files/mysql.server start 
{% endhighlight %}

如果使用 systemd ，可以添加如下文件，如下是 MariaDB 在 CentOS 中的配置。

{% highlight text %}
$ cat /lib/systemd/system/mariadb.service
[Unit]
Description=MariaDB database server
After=syslog.target
After=network.target

[Service]
Type=simple
User=mysql
Group=mysql

ExecStartPre=/usr/libexec/mariadb-prepare-db-dir %n
# Note: we set --basedir to prevent probes that might trigger SELinux alarms,
# per bug #547485
ExecStart=/usr/bin/mysqld_safe --basedir=/usr
ExecStartPost=/usr/libexec/mariadb-wait-ready $MAINPID

# Give a reasonable amount of time for the server to start up/shut down
TimeoutSec=300

# Place temp files in a secure directory, not /tmp
PrivateTmp=true

[Install]
WantedBy=multi-user.target
{% endhighlight %}

### 安全加固

5.7 对系统安全进行了加固，例如：

* 在数据库初始化时，如果使用的是 initialize 参数初始化数据库，则会为 root 用户设置一个默认的密码，保存在 /root/.mysql_secret 文件中。

* 密码强度管理，该功能是通过 validate\_password 插件进行判断。

* 原 SET PASSWORD=PASSWORD('paasword'); 方式，修改为 SET password='password';，而且官方建议使用 alster 方式。

{% highlight text %}
----- 推荐密码修改方式
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new_password';

----- validate_password插件加载方式
$ cat /etc/my.cnf
[mysqld]
plugin-load=validate_password.so

mysql> INSTALL PLUGIN validate_password SONAME 'validate_password.so';
mysql> SHOW VARIABLES LIKE 'validate%';
{% endhighlight %}

另外，可以通过如下脚本加固，简单来说就是设置 root 密码，删除 test 数据库。

{% highlight text %}
# /opt/mysql-5.7/bin/mysql_secure_installation
{% endhighlight %}

### 防火墙设置

设置为允许 3306 端口可以远程访问，如下，将 192.168.1.0/24 网段的客户机设置为白名单。

{% highlight text %}
# vim /etc/sysconfig/iptables
-A INPUT -s 192.168.1.0/24 -p tcp -m tcp -m state --state NEW -dport 3306 -j ACCEPT
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
