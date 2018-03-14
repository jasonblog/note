---
title: MySQL 用戶管理
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,database,數據庫,權限管理
description: MySQL 權限管理系統的主要功能是證實連接到一臺給定主機的用戶，並且賦予該用戶在數據庫上的相關權限，在認證的時候是通過用戶名+主機名定義。 此，簡單介紹下 MySQL 的權限管理，以及設置相應的用戶管理。
---

MySQL 權限管理系統的主要功能是證實連接到一臺給定主機的用戶，並且賦予該用戶在數據庫上的相關權限，在認證的時候是通過用戶名+主機名定義。

在此，簡單介紹下 MySQL 的權限管理，以及設置相應的用戶管理。

<!-- more -->

![mysql]({{ site.url }}/images/databases/mysql/account-logo.png "mysql"){: .pull-center width="75%" }

## 權限管理

簡單介紹一些用戶管理的操作。

### 常用操作

在設置權限時可以使用通配符，其中 % 表示可以匹配任意長度字符， _ 表示可以匹配單字符，常見的操作列舉如下：

{% highlight text %}
----- 查看幫助
mysql> help account management;
mysql> help create user;

----- 常用操作
mysql> SELECT current_user();                                        ← 查看當前用戶
mysql> SELECT user,host,password FROM mysql.user;                    ← 查看用戶信息
mysql> SHOW GRANTS;                                                  ← 當前用戶權限，會生成SQL語句
mysql> CREATE USER 'user'@'host' IDENTIFIED BY 'password';           ← 創建用戶
mysql> DROP USER 'user'@'host';                                      ← 刪除用戶
mysql> RENAME USER 'user'@'host' TO 'fool'@'host';                   ← 修改用戶名

----- 修改密碼
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new-password';   ← 修改密碼(recommand)
mysql> SET PASSWORD FOR 'root'@'localhost'=PASSWORD('new-password'); ← 修改密碼
mysql> UPDATE mysql.user SET password=PASSWORD('new-password')
       WHERE USER='root' AND Host='127.0.0.1';
mysql> UPDATE mysql.user SET password='' WHERE user='root';          ← 清除密碼
mysql> FLUSH PRIVILEGES;
$ mysqladmin -uROOT -pOLD_PASSWD password NEW_PASSWD                 ← 通過mysqladmin修改
$ mysqladmin -uROOT -p flush-privileges

----- 權限管理
mysql> GRANT ALL ON *.* TO 'user'@'%' [IDENTIFIED BY 'password'];
mysql> GRANT ALL PRIVILIGES ON [TABLE | DATABASE] student,course TO user1,user2;
mysql> GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, CREATE TEMPORARY, ALTER,
       DROP, REFERENCES, INDEX, CREATE VIEW, SHOW VIEW, CREATE ROUTINE,
       ALTER ROUTINE, EXECUTE
       ON db.tbl TO 'user'@'host' [IDENTIFIED BY 'password'];
mysql> GRANT ALL ON sampdb.* TO PUBLIC WITH GRANT OPTION;            ← 所有人，可以授權給其他人
mysql> GRANT UPDATE(col),SELECT ON TABLE tbl TO user;                ← 針對列賦值
mysql> SHOW GRANTS [FOR 'user'@'host'];                              ← 查看權限信息
mysql> REVOKE ALL ON *.* FROM 'user'@'host';                         ← 撤銷權限
mysql> REVOKE SELECT(user, host), UPDATE(host) ON db.tbl FROM 'user'@'%';

----- 查看用戶或者密碼為空的用戶
mysql> SELECT user, host, password FROM mysql.user
       WHERE (user IS NULL OR user='') OR (password IS NULL OR password='');

----- 重命名用戶
mysql> RENAME USER 'user1'@'%' TO 'user2'@'%';
{% endhighlight %}

WITH GRANT OPTION 表示可以將被授予的權力再次授權給其他人；實際上，在上述操作中，如下的步驟是相同的。

{% highlight text %}
mysql> CREATE USER 'user' IDENTIFIED BY 'password';
mysql> GRANT privileges TO 'user'@'host' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;

mysql> GRANT privileges  TO 'user'@'host' IDENTIFIED BY 'password' [WITH GRANT OPTION];
mysql> FLUSH PRIVILEGS;
{% endhighlight %}

也就是將創建用戶和授權是否分開處理。

#### 取消GRNAT

默認 root 是有 GRANT 權限，為了安全考慮可以手動取消其權限。

{% highlight text %}
mysql> UPDATE mysql.user SET grant_priv='N' WHERE user='ROOT';
mysql> FLUSH PRIVILEGES;
{% endhighlight %}

注意，修改權限後需要重新登錄生效！

### 用戶設置

![mysql]({{ site.url }}/images/databases/mysql/account-logo2.png "mysql"){: .pull-right width="18%" }

為了最小化用戶權限，通常可以設置四類：

* monitor 對內部表的只讀權限，用於監控系統採集監控數據；
* readonly 業務表的只讀權限，比如用於運營、開發排查問題等；
* admin 基本是最高權限，一些運維人員使用的帳號，例如建表、帳號管理等操作；
* foobar 業務帳號，也就是應用使用的帳號，業務業務表有增刪改查的權限。

接下來，我們看看如何管理這些帳號。

#### monitor

創建完用戶之後，默認是有 information_schema 庫的讀權限的。

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

與 monitor 用戶相同，不過是針對的業務數據庫授權。

#### admin

只針對管理平面的機器設置權限，也就是指定 IP 地址。

{% highlight text %}
mysql> CREATE USER 'admin'@'IP' IDENTIFIED BY 'password';
mysql> GRANT ALL PRIVILEGES ON *.* TO 'admin'@'IP';

mysql> REVOKE ALL PRIVILEGES ON *.* FROM 'admin'@'IP';
mysql> DROP USER 'admin'@'IP';
{% endhighlight %}

#### root

保留只允許本地登陸的 root 用戶，Always have a plan B 。

{% highlight text %}
mysql> GRANT ALL PRIVILEGES ON *.* TO 'root'@'localhost' WITH GRANT OPTION;
{% endhighlight %}

Plan C 是免密碼登陸。

## 加固插件



### 密碼複雜度檢查

提供了一個插件，可以用於密碼的加固，可以採用如下的其中一個方式加載。

1. 參數啟動時指定 ```--plugin-load='validate_password.so'``` ；

2. 在配置文件 my.cnf 中的 mysqld 段添加 ```plugin-load=validate_password.so``` ，同時為了防止插件在 mysql 運行時的時候被卸載可以同時添加 ```validate-password=FORCE_PLUS_PERMANENT``` ；

3. 登陸 mysql，在命令行模式下直接安裝/卸載插件插件 ```INSTALL PLUGIN validate_password SONAME 'validate_password.so';``` 以及 ```UNINSTALL PLUGIN validate_password;``` 。

注意：安裝和卸載語句是不記錄 binlog 的如果是複製環境，需要在主備庫分別進行操作才可。

#### 使用簡介

當修改 MySQL 的賬號密碼時，會檢查當前的是密碼策略如果不符合預定義的策略，不符合則返回 ```ER_NOT_VALID_PASSWORD``` 錯誤，相關的語句有 ```CREATE USER```、```GRANT```、```SET PASSWORD``` 。

可以通過 ```VALIDATE_PASSWORD_STRENGTH()``` 函數對密碼強度進行檢查，從 0 到 100 依次表示從弱到強；當然，需要提前加載插件，否則只會返回 0 。

{% highlight text %}
mysql> SELECT VALIDATE_PASSWORD_STRENGTH('123');
mysql> SELECT VALIDATE_PASSWORD_STRENGTH('123_abc');
mysql> SELECT VALIDATE_PASSWORD_STRENGTH('123_abc_ABC');

mysql> SHOW VARIABLES LIKE 'validate_password%';
+--------------------------------------+--------+
| Variable_name                        | Value  |
+--------------------------------------+--------+
| validate_password_dictionary_file    |        |  驗證密碼的目錄路徑
| validate_password_length             | 8      |  密碼的最小長度
| validate_password_mixed_case_count   | 1      |  至少有一個大寫和小寫的字符
| validate_password_number_count       | 1      |  至少要有一個數字字符
| validate_password_policy             | MEDIUM |  密碼安全策略LOW,MEDIUM,STRONG
| validate_password_special_char_count | 1      |  至少包含一個特殊字符
+--------------------------------------+--------+
6 rows in set (0.02 sec)
{% endhighlight %}

其中 LOW 表示只限制長度；MEDIUM 則為長度，字符，數字，大小寫，特殊字符；STRONG 則在之前的基礎上增加字典目錄。


### 使用密碼插件登陸

MySQL 提供了一個 ```mysql_config_editor``` 工具，用於加密用戶密碼，並提供了免密碼登陸，適用於 ```mysql```、```mysqladmin```、```mysqldump``` 等。

該工具提供了 ```set```、```remove```、```print```、```reset```，如下簡單介紹其使用方法：

{% highlight text %}
----- 1. 創建MySQL測試賬號密碼
mysql> CREATE USER 'mysqldba'@'localhost' IDENTIFIED BY 'NEW-123_password';
mysql> DROP USER 'mysqldba'@'localhost';

----- 2. 創建/刪除/查看加密文件，此時會生成一個~/.mylogin.cnf加密二進制文件
$ mysql_config_editor set --login-path=mysqldba --host=192.30.16.136 \
  --user=mysqldba --password='NEW-123_password'
$ mysql_config_editor print --all
$ mysql_config_editor print --login-path=mysqldba
$ mysql_config_editor remove --login-path=mysqldba

$ mysql --login-path=mysqldba
{% endhighlight %}

<!--
可以參考如下內容，解密
http://www.kikikoo.com/uid-20708886-id-5599401.html
-->


## 其它

在此，主要列舉一些常見的用戶相關操作。

### 重置 root 用戶密碼

如果忘記了 root 用戶，可以通過如下方法進行更改。

{% highlight text %}
----- 1. 停止mysql服務器
# systemctl stop mysqld
# /opt/mysql-5.7/bin/mysqladmin -uroot -p'init-password' shutdown
Shutting down MySQL..     done

----- 2. 獲取跳過認證的啟動參數
# mysqld --help --verbose | grep 'skip-grant-tables' -A1
    --skip-grant-tables Start without grant tables. This gives all users FULL
                          ACCESS to all tables.

----- 3. 啟動服務器，跳過認證
# mysqld --skip-grant-tables --user=mysql &
[1] 10209

----- 4. 取消密碼
mysql> UPDATE mysql.user SET password='' WHERE user='root';
Query OK, 2 rows affected (0.00 sec)
Rows matched: 2  Changed: 2  Warnings: 0
{% endhighlight %}

然後正常重啟即可，空密碼登陸，再設置好密碼。

### 密碼存儲

MySQL 把所有用戶的用戶名和密碼的密文存放在 mysql.user 表中，而 5.7 將原有的 password 字段替換為了 authentication_string 字段，如果使用 mysql_native_password 插件，則結果相同。

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

一般來說密文是通過不可逆加密算法得到的，這樣即使敏感信息洩漏，除了暴力破解是無法快速從密文直接得到明文的。

MySQL 使用了兩次 SHA1，以及夾雜一次 unhex 的方式對用戶密碼進行了加密；算法的具體公式可以通過如下方式表示：```password_str = concat('*', sha1(unhex(sha1(password))))``` 。

{% highlight text %}
mysql> SELECT PASSWORD('password'),UPPER(CONCAT('*',SHA1(UNHEX(SHA1('password')))));
+-------------------------------------------+--------------------------------------------------+
| PASSWORD('password')                      | UPPER(CONCAT('*',SHA1(UNHEX(SHA1('password'))))) |
+-------------------------------------------+--------------------------------------------------+
| *2470C0C06DEE42FD1618BB99005ADCA2EC9D1E19 | *2470C0C06DEE42FD1618BB99005ADCA2EC9D1E19        |
+-------------------------------------------+--------------------------------------------------+
1 row in set, 1 warning (0.00 sec)
{% endhighlight %}

其實 MySQL 在 5.6 以前，對安全性的重視度非常低，甚至可以從 binlog 中直接通過 mysqlbinlog 將密碼解密出來；5.7 之後在 binlog 中對密碼進行了加密處理。

不過 ```CHANGE MASTER TO``` 命令仍會將密碼明文保存在 master.info 文件中。

### Tips

1. 創建 MySQL 賬號密碼時，如果在配置文件中使用了 bind-address 選項，而且有多個 IP 地址，那麼需要注意綁定的 IP 地址，否則會報 ```ERROR 2003 (HY000): Can't connect to MySQL server on 'IP' (111)``` 。


## 參考

官方文檔 [MySQL Reference Manual - Security](http://dev.mysql.com/doc/refman/en/security.html)，主要是與安全相關的內容，包括了用戶管理。

{% highlight text %}
{% endhighlight %}
