---
title: MySQL 線上部署
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,線上部署
description: 主要介紹下 MySQL 如何在線上進行部署。
---

主要介紹下 MySQL 如何在線上進行部署。

<!-- more -->

![mysql deploy online logo]({{ site.url }}/images/databases/mysql/deploy-online-logo.png "mysql deploy online logo"){: .pull-center width="50%" }

可以通過如下步驟部署 MySQL 服務。

### 創建用戶

首先創建 MySQL 用戶和組。

{% highlight text %}
# groupadd -r mysql
# useradd -r -g mysql -M -s /sbin/nologin mysql
參數:
    -r 添加系統用戶組、系統用戶
    -g 指定用戶需要添加到的group
    -M 不創建home目錄
    -s 指定使用的shell
{% endhighlight %}

當出現權限問題時，可以通過如下方式進行調試。

{% highlight text %}
# usermod -s /bin/bash mysql
# su mysql -c 'echo "test" > /opt/data/test'
# su mysql -c 'cat /opt/data/test'
# usermod -s /sbin/nologin mysql
{% endhighlight %}

### 創建數據目錄

創建數據目錄，另外需要保證 mysql 用戶對安裝目錄有讀權限，否則加載插件時可能失敗。

{% highlight text %}
# mkdir -pv /opt/data
# chown -R mysql:mysql /opt/data
# chown -R --verbose mysql:mysql /opt/mysql-5.7
{% endhighlight %}

### 數據庫初始化操作

創建系統自帶的數據庫和表，如果後續誤刪除，可以直接再次執行如下命令。

{% highlight text %}
# /opt/mysql-5.7/bin/mysqld --initialize-insecure --basedir=/opt/mysql-5.7 \
    --datadir=/opt/data --user=mysql
{% endhighlight %}

### 添加配置文件

如下只是一個簡單的配置，詳細配置文件可以查看。

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

### 啟動腳本

注意，在編譯後源碼的 support-files 目錄下，包括了一些常見的腳本。

如果使用的是 SysV 系統，可以拷貝服務啟動腳本到 /etc/init.d/mysqld 目錄下，不過現在一般採用 systemd 管理服務；當然，可以後直接通過該腳本啟動服務。

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

5.7 對系統安全進行了加固，例如：

* 在數據庫初始化時，如果使用的是 initialize 參數初始化數據庫，則會為 root 用戶設置一個默認的密碼，保存在 /root/.mysql_secret 文件中。

* 密碼強度管理，該功能是通過 validate\_password 插件進行判斷。

* 原 SET PASSWORD=PASSWORD('paasword'); 方式，修改為 SET password='password';，而且官方建議使用 alster 方式。

{% highlight text %}
----- 推薦密碼修改方式
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new_password';

----- validate_password插件加載方式
$ cat /etc/my.cnf
[mysqld]
plugin-load=validate_password.so

mysql> INSTALL PLUGIN validate_password SONAME 'validate_password.so';
mysql> SHOW VARIABLES LIKE 'validate%';
{% endhighlight %}

另外，可以通過如下腳本加固，簡單來說就是設置 root 密碼，刪除 test 數據庫。

{% highlight text %}
# /opt/mysql-5.7/bin/mysql_secure_installation
{% endhighlight %}

### 防火牆設置

設置為允許 3306 端口可以遠程訪問，如下，將 192.168.1.0/24 網段的客戶機設置為白名單。

{% highlight text %}
# vim /etc/sysconfig/iptables
-A INPUT -s 192.168.1.0/24 -p tcp -m tcp -m state --state NEW -dport 3306 -j ACCEPT
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
