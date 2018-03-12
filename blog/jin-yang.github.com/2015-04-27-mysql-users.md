---
title: MySQL 用户管理
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,database,数据库,权限管理
description: MySQL 权限管理系统的主要功能是证实连接到一台给定主机的用户，并且赋予该用户在数据库上的相关权限，在认证的时候是通过用户名+主机名定义。 此，简单介绍下 MySQL 的权限管理，以及设置相应的用户管理。
---

MySQL 权限管理系统的主要功能是证实连接到一台给定主机的用户，并且赋予该用户在数据库上的相关权限，在认证的时候是通过用户名+主机名定义。

在此，简单介绍下 MySQL 的权限管理，以及设置相应的用户管理。

<!-- more -->

![mysql]({{ site.url }}/images/databases/mysql/account-logo.png "mysql"){: .pull-center width="75%" }

## 权限管理

简单介绍一些用户管理的操作。

### 常用操作

在设置权限时可以使用通配符，其中 % 表示可以匹配任意长度字符， _ 表示可以匹配单字符，常见的操作列举如下：

{% highlight text %}
----- 查看帮助
mysql> help account management;
mysql> help create user;

----- 常用操作
mysql> SELECT current_user();                                        ← 查看当前用户
mysql> SELECT user,host,password FROM mysql.user;                    ← 查看用户信息
mysql> SHOW GRANTS;                                                  ← 当前用户权限，会生成SQL语句
mysql> CREATE USER 'user'@'host' IDENTIFIED BY 'password';           ← 创建用户
mysql> DROP USER 'user'@'host';                                      ← 删除用户
mysql> RENAME USER 'user'@'host' TO 'fool'@'host';                   ← 修改用户名

----- 修改密码
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new-password';   ← 修改密码(recommand)
mysql> SET PASSWORD FOR 'root'@'localhost'=PASSWORD('new-password'); ← 修改密码
mysql> UPDATE mysql.user SET password=PASSWORD('new-password')
       WHERE USER='root' AND Host='127.0.0.1';
mysql> UPDATE mysql.user SET password='' WHERE user='root';          ← 清除密码
mysql> FLUSH PRIVILEGES;
$ mysqladmin -uROOT -pOLD_PASSWD password NEW_PASSWD                 ← 通过mysqladmin修改
$ mysqladmin -uROOT -p flush-privileges

----- 权限管理
mysql> GRANT ALL ON *.* TO 'user'@'%' [IDENTIFIED BY 'password'];
mysql> GRANT ALL PRIVILIGES ON [TABLE | DATABASE] student,course TO user1,user2;
mysql> GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, CREATE TEMPORARY, ALTER,
       DROP, REFERENCES, INDEX, CREATE VIEW, SHOW VIEW, CREATE ROUTINE,
       ALTER ROUTINE, EXECUTE
       ON db.tbl TO 'user'@'host' [IDENTIFIED BY 'password'];
mysql> GRANT ALL ON sampdb.* TO PUBLIC WITH GRANT OPTION;            ← 所有人，可以授权给其他人
mysql> GRANT UPDATE(col),SELECT ON TABLE tbl TO user;                ← 针对列赋值
mysql> SHOW GRANTS [FOR 'user'@'host'];                              ← 查看权限信息
mysql> REVOKE ALL ON *.* FROM 'user'@'host';                         ← 撤销权限
mysql> REVOKE SELECT(user, host), UPDATE(host) ON db.tbl FROM 'user'@'%';

----- 查看用户或者密码为空的用户
mysql> SELECT user, host, password FROM mysql.user
       WHERE (user IS NULL OR user='') OR (password IS NULL OR password='');

----- 重命名用户
mysql> RENAME USER 'user1'@'%' TO 'user2'@'%';
{% endhighlight %}

WITH GRANT OPTION 表示可以将被授予的权力再次授权给其他人；实际上，在上述操作中，如下的步骤是相同的。

{% highlight text %}
mysql> CREATE USER 'user' IDENTIFIED BY 'password';
mysql> GRANT privileges TO 'user'@'host' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;

mysql> GRANT privileges  TO 'user'@'host' IDENTIFIED BY 'password' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;
{% endhighlight %}

也就是将创建用户和授权是否分开处理。

#### 取消GRNAT

默认 root 是有 GRANT 权限，为了安全考虑可以手动取消其权限。

{% highlight text %}
mysql> UPDATE mysql.user SET grant_priv='N' WHERE user='ROOT';
mysql> FLUSH PRIVILEGES;
{% endhighlight %}

注意，修改权限后需要重新登录生效！

### 用户设置

![mysql]({{ site.url }}/images/databases/mysql/account-logo2.png "mysql"){: .pull-right width="18%" }

为了最小化用户权限，通常可以设置四类：

* monitor 对内部表的只读权限，用于监控系统采集监控数据；
* readonly 业务表的只读权限，比如用于运营、开发排查问题等；
* admin 基本是最高权限，一些运维人员使用的帐号，例如建表、帐号管理等操作；
* foobar 业务帐号，也就是应用使用的帐号，业务业务表有增删改查的权限。

接下来，我们看看如何管理这些帐号。

#### monitor

创建完用户之后，默认是有 information_schema 库的读权限的。

{% highlight text %}
mysql> CREATE USER 'monitor'@'%' IDENTIFIED BY 'password';
mysql> SHOW GRANTS FOR 'monitor'@'%';
mysql> GRANT SELECT ON performance_schema.* TO 'monitor'@'%';
mysql> GRANT SELECT ON mysql.* TO 'monitor'@'%';

mysql> REVOKE SELECT ON mysql.* FROM 'monitor'@'%';
mysql> REVOKE SELECT ON performance_schema.* FROM 'monitor'@'%';
mysql> DROP USER 'monitor'@'%';
{% endhighlight %}

#### readonly

与 monitor 用户相同，不过是针对的业务数据库授权。

#### admin

只针对管理平面的机器设置权限，也就是指定 IP 地址。

{% highlight text %}
mysql> CREATE USER 'admin'@'IP' IDENTIFIED BY 'password';
mysql> GRANT ALL PRIVILEGES ON *.* TO 'admin'@'IP';

mysql> REVOKE ALL PRIVILEGES ON *.* FROM 'admin'@'IP';
mysql> DROP USER 'admin'@'IP';
{% endhighlight %}

#### root

保留只允许本地登陆的 root 用户，Always have a plan B 。

{% highlight text %}
mysql> GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost' WITH GRANT OPTION;
{% endhighlight %}

Plan C 是免密码登陆。

## 加固插件



### 密码复杂度检查

提供了一个插件，可以用于密码的加固，可以采用如下的其中一个方式加载。

1. 参数启动时指定 ```--plugin-load='validate_password.so'``` ；

2. 在配置文件 my.cnf 中的 mysqld 段添加 ```plugin-load=validate_password.so``` ，同时为了防止插件在 mysql 运行时的时候被卸载可以同时添加 ```validate-password=FORCE_PLUS_PERMANENT``` ；

3. 登陆 mysql，在命令行模式下直接安装/卸载插件插件 ```INSTALL PLUGIN validate_password SONAME 'validate_password.so';``` 以及 ```UNINSTALL PLUGIN validate_password;``` 。

注意：安装和卸载语句是不记录 binlog 的如果是复制环境，需要在主备库分别进行操作才可。

#### 使用简介

当修改 MySQL 的账号密码时，会检查当前的是密码策略如果不符合预定义的策略，不符合则返回 ```ER_NOT_VALID_PASSWORD``` 错误，相关的语句有 ```CREATE USER```、```GRANT```、```SET PASSWORD``` 。

可以通过 ```VALIDATE_PASSWORD_STRENGTH()``` 函数对密码强度进行检查，从 0 到 100 依次表示从弱到强；当然，需要提前加载插件，否则只会返回 0 。

{% highlight text %}
mysql> SELECT VALIDATE_PASSWORD_STRENGTH('123');
mysql> SELECT VALIDATE_PASSWORD_STRENGTH('123_abc');
mysql> SELECT VALIDATE_PASSWORD_STRENGTH('123_abc_ABC');

mysql> SHOW VARIABLES LIKE 'validate_password%';
+--------------------------------------+--------+
| Variable_name                        | Value  |
+--------------------------------------+--------+
| validate_password_dictionary_file    |        |  验证密码的目录路径
| validate_password_length             | 8      |  密码的最小长度
| validate_password_mixed_case_count   | 1      |  至少有一个大写和小写的字符
| validate_password_number_count       | 1      |  至少要有一个数字字符
| validate_password_policy             | MEDIUM |  密码安全策略LOW,MEDIUM,STRONG
| validate_password_special_char_count | 1      |  至少包含一个特殊字符
+--------------------------------------+--------+
6 rows in set (0.02 sec)
{% endhighlight %}

其中 LOW 表示只限制长度；MEDIUM 则为长度，字符，数字，大小写，特殊字符；STRONG 则在之前的基础上增加字典目录。


### 使用密码插件登陆

MySQL 提供了一个 ```mysql_config_editor``` 工具，用于加密用户密码，并提供了免密码登陆，适用于 ```mysql```、```mysqladmin```、```mysqldump``` 等。

该工具提供了 ```set```、```remove```、```print```、```reset```，如下简单介绍其使用方法：

{% highlight text %}
----- 1. 创建MySQL测试账号密码
mysql> CREATE USER 'mysqldba'@'localhost' IDENTIFIED BY 'NEW-123_password';
mysql> DROP USER 'mysqldba'@'localhost';

----- 2. 创建/删除/查看加密文件，此时会生成一个~/.mylogin.cnf加密二进制文件
$ mysql_config_editor set --login-path=mysqldba --host=192.30.16.136 \
  --user=mysqldba --password='NEW-123_password'
$ mysql_config_editor print --all
$ mysql_config_editor print --login-path=mysqldba
$ mysql_config_editor remove --login-path=mysqldba

$ mysql --login-path=mysqldba
{% endhighlight %}

<!--
可以参考如下内容，解密
http://www.kikikoo.com/uid-20708886-id-5599401.html
-->


## 其它

在此，主要列举一些常见的用户相关操作。

### 重置 root 用户密码

如果忘记了 root 用户，可以通过如下方法进行更改。

{% highlight text %}
----- 1. 停止mysql服务器
# systemctl stop mysqld
# /opt/mysql-5.7/bin/mysqladmin -uroot -p'init-password' shutdown
Shutting down MySQL..     done

----- 2. 获取跳过认证的启动参数
# mysqld --help --verbose | grep 'skip-grant-tables' -A1
    --skip-grant-tables Start without grant tables. This gives all users FULL
                          ACCESS to all tables.

----- 3. 启动服务器，跳过认证
# mysqld --skip-grant-tables --user=mysql &
[1] 10209

----- 4. 取消密码
mysql> UPDATE mysql.user SET password='' WHERE user='root';
Query OK, 2 rows affected (0.00 sec)
Rows matched: 2  Changed: 2  Warnings: 0
{% endhighlight %}

然后正常重启即可，空密码登陆，再设置好密码。

### 密码存储

MySQL 把所有用户的用户名和密码的密文存放在 mysql.user 表中，而 5.7 将原有的 password 字段替换为了 authentication_string 字段，如果使用 mysql_native_password 插件，则结果相同。

{% highlight text %}
mysql> SELECT user,plugin,authentication_string FROM mysql.user;
+-----------+-----------------------+-------------------------------------------+
| user      | plugin                | authentication_string                     |
+-----------+-----------------------+-------------------------------------------+
| root      | mysql_native_password | *4A82FDF1D80BA7470BA2E17FEEFD5A53D5D3B762 |
| mysql.sys | mysql_native_password | *THISISNOTAVALIDPASSWORDTHATCANBEUSEDHERE |
| mysync    | mysql_native_password | *09A25204677367FC91363E3AE2F5BEFE4602EA70 |
+-----------+-----------------------+-------------------------------------------+
3 rows in set (0.00 sec)
{% endhighlight %}

一般来说密文是通过不可逆加密算法得到的，这样即使敏感信息泄漏，除了暴力破解是无法快速从密文直接得到明文的。

MySQL 使用了两次 SHA1，以及夹杂一次 unhex 的方式对用户密码进行了加密；算法的具体公式可以通过如下方式表示：```password_str = concat('*', sha1(unhex(sha1(password))))``` 。

{% highlight text %}
mysql> SELECT PASSWORD('password'),UPPER(CONCAT('*',SHA1(UNHEX(SHA1('password')))));
+-------------------------------------------+--------------------------------------------------+
| PASSWORD('password')                      | UPPER(CONCAT('*',SHA1(UNHEX(SHA1('password'))))) |
+-------------------------------------------+--------------------------------------------------+
| *2470C0C06DEE42FD1618BB99005ADCA2EC9D1E19 | *2470C0C06DEE42FD1618BB99005ADCA2EC9D1E19        |
+-------------------------------------------+--------------------------------------------------+
1 row in set, 1 warning (0.00 sec)
{% endhighlight %}

其实 MySQL 在 5.6 以前，对安全性的重视度非常低，甚至可以从 binlog 中直接通过 mysqlbinlog 将密码解密出来；5.7 之后在 binlog 中对密码进行了加密处理。

不过 ```CHANGE MASTER TO``` 命令仍会将密码明文保存在 master.info 文件中。

### Tips

1. 创建 MySQL 账号密码时，如果在配置文件中使用了 bind-address 选项，而且有多个 IP 地址，那么需要注意绑定的 IP 地址，否则会报 ```ERROR 2003 (HY000): Can't connect to MySQL server on 'IP' (111)``` 。


## 参考

官方文档 [MySQL Reference Manual - Security](http://dev.mysql.com/doc/refman/en/security.html)，主要是与安全相关的内容，包括了用户管理。

{% highlight text %}
{% endhighlight %}
