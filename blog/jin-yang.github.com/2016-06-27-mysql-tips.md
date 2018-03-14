---
title: MySQL 雜項
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,雜項
description: 簡單記錄下 MySQL 常見的一些操作。
---

簡單記錄下 MySQL 常見的一些操作。

<!-- more -->

## 初始化密碼

MySQL 5.7 在初始化時會生成默認的密碼，當然可以在初始化的時候使用 ```--initialize-insecure``` 參數不設置密碼，不過通過 mysql 客戶端登陸的時候，仍會提示輸入密碼。

對於自動化腳本，最好的方式是在啟動 MySQL 服務器時使用 ```--skip-grant-tables``` 參數，然後更新密碼。

{% highlight text %}
----- MySQL5.7.6及以上版本
mysql> UPDATE mysql.user SET authentication_string=PASSWORD('newpass') WHERE user='root';
----- MySQL 5.7.5及之前版本
mysql> UPDATE mysql.user SET password=PASSWORD('newpass') WHERE user='root';

----- 授權遠程訪問
mysql> GRANT ALL PRIVILEGES ON *.* TO 'root'@'%' IDENTIFIED BY 'newpass' WITH GRANT OPTION;
{% endhighlight %}

注意，如果使用了 ```--skip-grant-tables``` 參數，如果使用如下密碼更新時，將會報錯。

{% highlight text %}
mysql> ALTER USER 'root'@'localhost' IDENTIFIED BY 'new-password';
{% endhighlight %}

## pager

如果一次查看的數據集比較大，可以通過 ```pager``` 命令設置顯示。

{% highlight text %}
----- 使用less查看
mysql> pager less
PAGER set to 'less'

----- 如果查看InnoDB狀態，也可以使用如下方式
mysql> pager grep sequence
PAGER set to 'grep sequence'
mysql> SHOW ENGINE INNODB STATUS\G SELECT sleep(60); SHOW ENGINE INNODB STATUS\G
Log sequence number 84 3836410803
1 row in set (0.06 sec)

1 row in set (1 min 0.00 sec)

Log sequence number 84 3838334638
1 row in set (0.05 sec)
{% endhighlight %}





<!--
set global validate_password_policy=0; set global validate_password_mixed_case_count=2;

information_schema   tables   所有表的元數據信息 select table_name from information_schema.tables where table_schema='database-name'參考：http://dev.mysql.com/doc/refman/en/server-system-variables.html

* unknown variable 'default-character-set=utf8'<br>
  可以通過 mysql --help | grep my.cnf 查看配置文件的加載順序,用 character_set_server=utf8 來取代 default-character-set=utf8 能解決這個問題.

-->

{% highlight text %}
{% endhighlight %}
