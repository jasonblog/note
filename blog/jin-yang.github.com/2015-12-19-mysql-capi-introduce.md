---
title: MySQL CAPI 接口
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,c,api
description: 在编写 MySQL 客户端程序时，最常见的就是如何连接以及关闭 MySQL，这里需要注意，如果使用不当将会造成内存泄漏。这里，简单介绍 MySQL 中如何通过 C 进行编程。
---

在编写 MySQL 客户端程序时，最常见的就是如何连接以及关闭 MySQL，这里需要注意，如果使用不当将会造成内存泄漏。

这里，简单介绍 MySQL 中如何通过 C 进行编程。

<!-- more -->

## 简介


单线程环境，一般使用 `-lmysqlclient` 链接 MySQL 客户库，其中 `mysql_init()` 会自动调用 `my_library_init()` 初始化 MySQL 库，所以不需要，需要注意的是这两个函数都是非线程安全的。

示例代码如下。

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

多线程环境，一般调用 `-lmysqlclient_r` 安全类库，需要在各个线程中调用 `mysql_library_init()` `mysql_library_end()` 来分配和释放 MySQL 资源，或者增加线程锁保护资源，否则会造成内存泄漏。

示例代码如下。

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


在使用 MySQL 多线程时，需要注意如下的问题：

1. `mysql_library_init()` 和 `mysql_library_end()` 必须要在同一个线程中，否则会出现 `Error in my_thread_global_end(): 1 threads didn't exit` 的报错。
2. 当通过 `mysql_ping()` 检测到链接断开后，直接调用 `mysql_real_connect()` 重新链接会导致本次链接的部分内存没有释放。
3. 如果设置成自动重新链接，那么在其它线程通过 `mysql_ping()` 重新链接时，同样会存在问题。


## 简单示例

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

然后通过如下方式进行编译。

{% highlight text %}
hello: main.c
	gcc -o hello main.c `mysql_config --cflags --libs`
{% endhighlight %}

## MySQL-Python

Python 中提供了统一的数据库接口，也就是 [DB-API](https://www.python.org/dev/peps/pep-0249/)，在此之前不同类型的数据库实现的接口各不相同，当更换不同数据库时需要做大量的修改，而 DB-API 实际上提供了一个标准的数据库访问接口。

该规范定义了线程安全级别、连接接口、异常、游标等等。

### MySQLdb 安装

MySQLdb 是用于 Python 链接 MySQL 数据库的模块，实现了 Python 数据库 API 规范 V2.0，基于 MySQL C API 建立。

{% highlight text %}
----- 可以通过PIP安装
# apt-get install python-pip
# pip install --proxy=http://your-proxy-ip:port mysql-python

----- 或者使用easy_install方式安装
# easy_install mysql-python

----- 也可以通过源码安装
# unzip MySQL-python-1.2.5.zip && cd
# python setup.py install
{% endhighlight %}

最简单的还是通过类似 YUM APT-GET 的包管理工具安装，否则会需要安装 libmysqlclient-dev 之类的依赖库，详细内容可以参考 [PyPi MySQL-Python](https://pypi.python.org/pypi/MySQL-python) 以及 [MySQL Repositories](https://dev.mysql.com/downloads/repo/yum/) 。

MySQLdb 底层会调用 `_mysql` 模块，这是一个 C 编写的模块，实际上就是对 MySQL-CAPI 的封装，安装后包保存在 `/usr/lib64/python2.6/site-packages/` 目录下。

### 异常处理

MySQLdb 会有两种异常，包括了 `MySQLdb.Warning`、`MySQLdb.Error`，不太清楚 Warning 是作什么的，不是 SQL 返回的 Warning 。

如果在只捕获 Warning 时，可能会发现程序并未捕获警告，这是因为 Python 程序 try...except... 默认只能捕获到错误异常，可以通过如下方式更改 Warning 的级别，让 Python 程序认为这是一个错误，从而捕捉到它。

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



## 参考

[MySQLdb User's Guide](http://mysql-python.sourceforge.net/MySQLdb.html) 。


<!--
http://www.runoob.com/python/python-mysql.html
http://www.crazyant.net/686.html


mysql_real_escape_string() 和 mysql_real_string() 两者从入参实际上就可以做区分，前者需要链接 + 字符串，后者只需要字符串。前者会根据当前的符号集进行转换，也就是可以正常的处理一些多字符的字符集，而后者可能会在一个字符中间插入转义符。

不过除了采用 escaping ，建议使用参数化查询。


MySQL timeout知多少
http://blog.csdn.net/sgbfblog/article/details/44262339



<br><br><br><h2>OB-error totally whack</h2><p>
MySQL 错误码包括了服务器端和客户端，而最大值在客户端，错误的最大值为 CR_MAX_ERROR。在 _mysql 中，如果大于该值则直接返回 error totally whack 错误。<br><br>

而 OB 错误的最大值为 5999(my_config.h)，因此只需要将判断修改为 5999 即可，此时错误类型为 OperationalError。可以通过如下的方式进行编译。
<pre style="font-size:0.8em; face:arial;">
gcc _mysql.c -I/usr/include/python2.7 -I/usr/include/mysql -fPIC -shared -L/usr/bin -o _mysql.so
</pre>
然后将 _mysql.so 复制到 /usr/lib64/python2.6/site-packages/ 。
-->



{% highlight text %}
{% endhighlight %}
