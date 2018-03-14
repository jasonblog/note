---
title: MySQL CAPI 接口
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,c,api
description: 在編寫 MySQL 客戶端程序時，最常見的就是如何連接以及關閉 MySQL，這裡需要注意，如果使用不當將會造成內存洩漏。這裡，簡單介紹 MySQL 中如何通過 C 進行編程。
---

在編寫 MySQL 客戶端程序時，最常見的就是如何連接以及關閉 MySQL，這裡需要注意，如果使用不當將會造成內存洩漏。

這裡，簡單介紹 MySQL 中如何通過 C 進行編程。

<!-- more -->

## 簡介


單線程環境，一般使用 `-lmysqlclient` 鏈接 MySQL 客戶庫，其中 `mysql_init()` 會自動調用 `my_library_init()` 初始化 MySQL 庫，所以不需要，需要注意的是這兩個函數都是非線程安全的。

示例代碼如下。

{% highlight c %}
void wrap_mysql_connect(MYSQL *mysql_conn)
{
	my_init();
	if (NULL == mysql_init(mysql_conn)) {
		write_log("Colud not init mysql.");
		return;
	}

	if (!mysql_real_connect(mysql_conn, g_mysql->host, g_mysql->username,
			g_mysql->passwd, g_mysql->dbname, g_mysql->port, NULL, 0)) {
		write_log("Mysql connect error : .", mysql_error(mysql_conn));
	}
}

void wrap_mysql_close(MYSQL *mysql_conn)
{
	mysql_close(mysql_conn);
	mysql_conn = NULL;
}
{% endhighlight %}

多線程環境，一般調用 `-lmysqlclient_r` 安全類庫，需要在各個線程中調用 `mysql_library_init()` `mysql_library_end()` 來分配和釋放 MySQL 資源，或者增加線程鎖保護資源，否則會造成內存洩漏。

示例代碼如下。

{% highlight c %}
void wrap_mysql_connect(MYSQL *mysql_conn)
{
	my_init();
	if (NULL == mysql_init(mysql_conn)) {
		write_log("Colud not init mysql.");
		return;
	}

	if (mysql_library_init(0, NULL, NULL)) {
		write_log("Could not initialize mysql library.");
		return;
	}

	if (!mysql_real_connect(mysql_conn, g_mysql->host, g_mysql->username,
			g_mysql->passwd, g_mysql->dbname, g_mysql->port, NULL, 0)) {
		write_log("Mysql connect error : .", mysql_error(mysql_conn));
	}
}


void pa_mysql_close(MYSQL *mysql_conn)
{
	mysql_close(mysql_conn);
	mysql_conn = NULL;
	mysql_library_end();
}
{% endhighlight %}


在使用 MySQL 多線程時，需要注意如下的問題：

1. `mysql_library_init()` 和 `mysql_library_end()` 必須要在同一個線程中，否則會出現 `Error in my_thread_global_end(): 1 threads didn't exit` 的報錯。
2. 當通過 `mysql_ping()` 檢測到鏈接斷開後，直接調用 `mysql_real_connect()` 重新鏈接會導致本次鏈接的部分內存沒有釋放。
3. 如果設置成自動重新鏈接，那麼在其它線程通過 `mysql_ping()` 重新鏈接時，同樣會存在問題。


## 簡單示例

{% highlight c %}
#include <mysql.h>
#include <my_global.h>

int main(int argc, char **argv)
{
        printf("MySQL client version: %s\n", mysql_get_client_info());

        MYSQL *conn = mysql_init(NULL);
        if (conn == NULL) {
                printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
                exit(1);
        }

        /* arguments: host user password database port unix_socket client_flag */
        if (mysql_real_connect(conn, "localhost", "root", "Huawei@123", NULL, 0, NULL, 0) == NULL) {
                printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
                mysql_close(conn);
                exit(1);
        }

        if (mysql_ping(conn)) {
                printf("Error: lost connection\n");
                return 1;
        }

        if (mysql_query(conn, "DROP DATABASE IF EXISTS foobardb")) {
                printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
                mysql_close(conn);
                exit(1);
        }

        if (mysql_query(conn, "SHOW DATABASES")) {
                printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
                mysql_close(conn);
                exit(1);
        }

        MYSQL_RES *result = mysql_store_result(conn);
        int num_fields = mysql_num_fields(result), i;
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
                for(i = 0; i < num_fields; i++)
                        printf("%s ", row[i] ? row[i] : "NULL");
                printf("\n");
        }
        mysql_free_result(result);

        mysql_close(conn);

        return 0;
}
{% endhighlight %}

然後通過如下方式進行編譯。

{% highlight text %}
hello: main.c
	gcc -o hello main.c `mysql_config --cflags --libs`
{% endhighlight %}

## MySQL-Python

Python 中提供了統一的數據庫接口，也就是 [DB-API](https://www.python.org/dev/peps/pep-0249/)，在此之前不同類型的數據庫實現的接口各不相同，當更換不同數據庫時需要做大量的修改，而 DB-API 實際上提供了一個標準的數據庫訪問接口。

該規範定義了線程安全級別、連接接口、異常、遊標等等。

### MySQLdb 安裝

MySQLdb 是用於 Python 鏈接 MySQL 數據庫的模塊，實現了 Python 數據庫 API 規範 V2.0，基於 MySQL C API 建立。

{% highlight text %}
----- 可以通過PIP安裝
# apt-get install python-pip
# pip install --proxy=http://your-proxy-ip:port mysql-python

----- 或者使用easy_install方式安裝
# easy_install mysql-python

----- 也可以通過源碼安裝
# unzip MySQL-python-1.2.5.zip && cd
# python setup.py install
{% endhighlight %}

最簡單的還是通過類似 YUM APT-GET 的包管理工具安裝，否則會需要安裝 libmysqlclient-dev 之類的依賴庫，詳細內容可以參考 [PyPi MySQL-Python](https://pypi.python.org/pypi/MySQL-python) 以及 [MySQL Repositories](https://dev.mysql.com/downloads/repo/yum/) 。

MySQLdb 底層會調用 `_mysql` 模塊，這是一個 C 編寫的模塊，實際上就是對 MySQL-CAPI 的封裝，安裝後包保存在 `/usr/lib64/python2.6/site-packages/` 目錄下。

### 異常處理

MySQLdb 會有兩種異常，包括了 `MySQLdb.Warning`、`MySQLdb.Error`，不太清楚 Warning 是作什麼的，不是 SQL 返回的 Warning 。

如果在只捕獲 Warning 時，可能會發現程序並未捕獲警告，這是因為 Python 程序 try...except... 默認只能捕獲到錯誤異常，可以通過如下方式更改 Warning 的級別，讓 Python 程序認為這是一個錯誤，從而捕捉到它。

{% highlight text %}
from warnings import filterwarnings
filterwarnings('error', category = MySQLdb.Warning)

try：
    conn = MySQLdb.connect(... ...)
    ... ...
except MySQLdb.Warning, w:
    print w???
except MySQLdb.Error, e:
    print e[0], e[1]
{% endhighlight %}



## 參考

[MySQLdb User's Guide](http://mysql-python.sourceforge.net/MySQLdb.html) 。


<!--
http://www.runoob.com/python/python-mysql.html
http://www.crazyant.net/686.html


mysql_real_escape_string() 和 mysql_real_string() 兩者從入參實際上就可以做區分，前者需要鏈接 + 字符串，後者只需要字符串。前者會根據當前的符號集進行轉換，也就是可以正常的處理一些多字符的字符集，而後者可能會在一個字符中間插入轉義符。

不過除了採用 escaping ，建議使用參數化查詢。


MySQL timeout知多少
http://blog.csdn.net/sgbfblog/article/details/44262339



<br><br><br><h2>OB-error totally whack</h2><p>
MySQL 錯誤碼包括了服務器端和客戶端，而最大值在客戶端，錯誤的最大值為 CR_MAX_ERROR。在 _mysql 中，如果大於該值則直接返回 error totally whack 錯誤。<br><br>

而 OB 錯誤的最大值為 5999(my_config.h)，因此只需要將判斷修改為 5999 即可，此時錯誤類型為 OperationalError。可以通過如下的方式進行編譯。
<pre style="font-size:0.8em; face:arial;">
gcc _mysql.c -I/usr/include/python2.7 -I/usr/include/mysql -fPIC -shared -L/usr/bin -o _mysql.so
</pre>
然後將 _mysql.so 複製到 /usr/lib64/python2.6/site-packages/ 。
-->



{% highlight text %}
{% endhighlight %}
