---
Date: October 19, 2013
title: MySQL 常用工具
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,monitor,監控,percona tools,sysbench,tcprstat,壓測,響應時間,工具
description: 工欲善其事，必先利其器；在本文章中介紹了與 MySQL 相關的一些常見的工具，列入 percona 工具集，常見的壓測工具 sysbench，服務器響應時間 tcprstat。包括了它們使用方法，詳細的執行過程等。
---

在此介紹一些 MySQL 中常用的工具，包括了 percona-tools、sysbench 等工具。

接下來我們看看。

<!-- more -->

部分軟件會依賴 automake autoconf libtool 等工具，需要在編譯之前進行安裝。

## percona-tools

![Percona Tools Logo]({{ site.url }}/images/databases/mysql/percona-tools-logo.png "Percona Tools Logo"){: .pull-right width="300px"}

該工具集基本上是 perl 腳本，可以直接從官方網站 [percona-toolkit](https://www.percona.com/downloads/percona-toolkit/) 上下載源碼，或者 [github](https://github.com/percona/percona-toolkit/releases) ，文檔也可以直接從上述的網站下載，只需要填寫個表格即可。

為了方便下載，本地保存了一個版本 [percona-toolkit-2.2.19.tar.gz](/reference/mysql/percona-toolkit-2.2.19.tar.gz) 以及 [Percona-Toolkit-2.2.19.pdf](/reference/mysql/Percona-Toolkit-2.2.19.pdf) 。

該工具是 [Aspersa](http://aspersa.googlecode.com/svn/html/index.html) 和 Aspersa 兩個工具的合併修改過來的，這是一套腳本工具，不過保存在 google code 中，所以需要翻牆；也可以查看一下 [本地版本](/reference/mysql/aspersa.tar.bz2) 以及 [本地保存文檔](/reference/mysql/aspersa) 。

接下來我們看看如何使用這一工具集，不過在此介紹其中的一部分工具的使用，剩餘的一部分工具將會在其它文章中繼續介紹。

安裝方式可以從源碼包中的 INSTALL 文件中查看，或者簡單執行如下命令。

{% highlight text %}
----- 安裝依賴的perl三方包
# yum install perl-Digest-MD5

----- 安裝percona-tools
# perl Makefile.PL
# make
# make test
# make install
{% endhighlight %}




## sysbench

<!--
一個不錯的Sysbench示例
https://wiki.gentoo.org/wiki/Sysbench

ScaleArc: 使用 sysbench 對數據庫進行壓力測試
http://www.oschina.net/translate/scalearc-benchmarking-sysbench
-->

sysbench 用於基準測試，包括了：CPU 性能、磁盤 IO 性能、調度程序性能、內存分配及傳輸速度、POSIX 線程性能、數據庫性能(OLTP基準測試，支持 MySQL、PostgreSQL、Oracle)等。

![Sysbench Logo]({{ site.url }}/images/databases/mysql/sysbench-logo.jpg "Sysbench Logo"){: .pull-center }

其命令格式如下：

{% highlight text %}
$ sysbench [common-options] --test=name [test-options] command
{% endhighlight %}

其中 \-\-test 指定測試工具，command 包括了 prepare、run、cleanup、help，prepare 用於準備數據，常用於 fileio、oltp 測試，其它用於運行、清理、查看幫助。

### 安裝

最新的是在 github 上維護的 [akopytov/sysbench](https://github.com/akopytov/sysbench)，下載源碼可以通過如下方式編譯。

{% highlight text %}
$ ./autogen.sh && ./configure && make && cd sysbench
$ ./sysbench --help                                        // 查看幫助
$ ./sysbench --test=cpu help                               // 查看標準測試的幫助
{% endhighlight %}

在執行 ./configure 時，如果通過 mysql_config 無法找到庫、頭文件目錄等，可以通過 \-\-with-mysql-includes=...、\-\-with-mysql-libs=... 進行配置。

如果想要支持 pgsql 和 Oracle，在配置時需要添加 \-\-with-pgsql 或者 \-\-with-oracle 選項，否則默認只支持 MySQL，相關的驅動源碼保存在 sysbench/drivers 目錄下。


### 基準測試

介紹一些常見的基準測試，關於 OLTP 單獨再述。

#### CPU 性能測試

使用 64 位整數，測試計算質數直到某個最大值所需要的時間。

{% highlight text %}
$ ./sysbench --test=cpu help
$ ./sysbench --test=cpu --cpu-max-prime=2000 run
{% endhighlight %}


#### 線程基準測試

測試線程調度器的性能，對於高負載情況下測試線程調度器的行為非常有用。

{% highlight text %}
$ ./sysbench --test=threads help
$ ./sysbench --test=threads --num-threads=500 --thread-yields=100 --thread-locks=4 run
{% endhighlight %}

#### 內存基準測試

用於測試內存的連續讀寫性能。

{% highlight text %}
$ ./sysbench --test=memory help
$ ./sysbench --test=memory --memory-block-size=8k --memory-total-size=1G run
{% endhighlight %}


#### 互斥鎖基準測試

互斥鎖用於在多線程編程中，防止兩個線程同時對同一公共資源，如全局變量，進行讀寫的機制來保證共享數據操作的完整性。測試互斥鎖的性能，方式是模擬所有線程在同一時刻併發運行，並都短暫請求互斥鎖。

{% highlight text %}
$ ./sysbench --test=mutex help
$ ./sysbench --test=mutex --num-threads=100 --mutex-num=1000 \
    --mutex-locks=100000 --mutex-loops=10000 run
{% endhighlight %}


#### 文件 IO 基準測試

測試包括了 prepare、run、cleanup，主要用於準備測試需要文件，運行測試，清理。注意，生成的數據文件至少要比內存大，否則操作系統可能會緩存大部分的數據，導致測試結果無法體現 IO 密集型的工作負載。


### OLTP

之前的版本 oltp 是內建的版本，現在採用的是 lua 腳本實現，保存在 tests/db 目錄下，通過這種方式，使得需要定製個性化測試時不用再重新編譯。

在此只針對 MySQL，進行測試；該驅動保存在源碼的 drivers/mysql 目錄下，關於驅動相關的參數，例如 mysql-host、mysql-socket 等，可以查看源碼。如果不指定則默認使用 sbtest 數據庫，此時需要手動創建該數據庫。

編譯完成之後，直接到源碼的的 sysbench 目錄下執行如下命令即可。

{% highlight text %}
$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=innodb \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 prepare

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=innodb \
    --num-threads=16 --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 run

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=innodb \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 cleanup
{% endhighlight %}

如上的命令，prepare 只需要執行一次，然後可以執行多次 run 即可。其它一些常見的參數可以參考：

{% highlight text %}
--num-threads=1         線程數
--report-interval=5     每隔5秒打印一次統計結果
{% endhighlight %}

對於 MyISAM 的測試可以直接將上述的存儲引擎更換為 myisam 即可，命令詳見如下。

{% highlight text %}
$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=myisam \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 prepare

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=myisam \
    --num-threads=16 --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 run

$ ./sysbench --mysql-host=127.1 --mysql-port=3306 --mysql-user=root \
    --mysql-password=new-password --mysql-table-engine=myisam \
    --test=tests/db/oltp.lua --oltp_tables_count=1 --oltp-table-size=1000 cleanup
{% endhighlight %}

### 源碼相關

相關的驅動文件保存在 sysbench/drivers 目錄下，測試相關模塊保存在 sysbench/tests 目錄下，下面看下與 DB 相關的腳本。

#### tests/db

首先看下 common.lua 這個文件，該文件用於讀取參數、執行 prepare、執行 cleanup；其中後兩者分別對應了相應的函數，prepare 會調用 create_insert 創建表並插入數據；而清除則是簡單刪除掉之前創建的表。

接下來看看 oltp.lua 文件，主要有兩個函數：A) thread_init() 會調用 set_vars() 設置與 oltp 相關的參數，初始化每個線程相關的參數；B) event() 函數用於處理每次調用，包括了事務執行包含了那些語句，及其次數等。

其中一個比較簡單的示例可以參考 oltp_simple.lua 文件。

## tcprstat

關於該工具詳細可以參考 [通過 tcprstat 工具統計應答時間](/post/linux-tcprstat.html) 的內容，包括了詳細代碼的參考。

如文章中所示，如果服務器監聽 127.1 會導致無法使用，可以通過如下方法解決。

{% highlight text %}
----- 1. 設置一個本地域的浮動IP地址
# ifconfig lo:1 127.168.1.1 netmask 255.0.0.0

----- 2. 配置文件中添加如下內容，然後重啟服務器
[mysqld]
bind-address=127.168.1.1

----- 3. 通過如下方式啟動tcprstat
$ tcprstat -l 127.1 -p 3306 -t 1 -n 0
{% endhighlight %}

按照如上方式，可以基本解決本地測試的 bug 。


## 其它

實時監控可以直接參考 [orzdba](http://code.taobao.org/p/orzdba/src/trunk/)，可直接下載安裝使用；另外相關的還有 [Percona Monitoring Plugins](https://www.percona.com/downloads/percona-monitoring-plugins/LATEST/)，也就是 zabbix、nagios、cacti 插件。

[mycli](http://mycli.net/) 是一個使用 Python 編寫的命令行終端，可以支持自動補全，以及高亮顯示。

<!--
Galera replication for MySQL
http://www.gpfeng.com/?p=603
-->

{% highlight text %}
{% endhighlight %}
