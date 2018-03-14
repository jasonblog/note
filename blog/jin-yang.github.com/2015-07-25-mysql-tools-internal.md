---
title: MySQL 自帶工具
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,tools,工具
description: 簡單介紹下 MySQL 中自帶的工具集。
---

簡單介紹下 MySQL 中自帶的工具集。

<!-- more -->

## 簡介

MySQL 提供了許多命令行工具，這些工具可以用來管理 MySQL 服務器、進行數據庫備份和恢復、對數據庫進行訪問控制、對數據庫進行壓測等等。

首先，簡單介紹下各個命令行。

{% highlight text %}
----- MySQL安裝、初始化
mysql_install_db
    初始化 MySQL 數據目錄程序，主要是新建數據庫，初始化用戶；5.7 已使用 mysqld 替換；
mysql_secure_installation
    安裝完之後進行一些安全性配置，例如設置root用戶、刪除test數據庫等；
mysql_safe
    一個shell腳本程序，用來安全啟動mysqld程序；
mysql_tzinfo_to_sql
    將時區信息轉換為SQL語句，可以直接加載到MySQL的mysql.time_zone%文件中；
mysql_upgrade
    用於升級時檢查MySQL表；

----- MySQL客戶端程序
mysql
    MySQL命令行程序，用於鏈接數據庫；
mysqlslap
    壓測程序，可以用來簡單的對數據庫進行性能壓測；
mysqldump
    邏輯備份程序；
mysqlimport
    數據導入工具；

----- MySQL管理和實用程序
mysqlbinlog
    用於處理二進制日誌文件(binlog)，可以用於打印數據；
mysqldumpslow
    用於處理慢查詢日誌文件；
mysqladmin
    用於管理MySQL服務器；

----- 其它
my_print_defaults
    讀取配置文件中的配置選項；
{% endhighlight %}

<!--
mysqlcheck：一個表檢查維護工具
mysqlshow：顯示數據庫，表，列信息

mysqlhotcopy：數據備份工具
-->

<!--

下面先介紹一些現在需要用到的MySQL程序，更多的程序工具在後面哪裡用到就會講到。
連接MySQL

連接MySQL操作是一個連接進程和MySQL數據庫實例進行通信，從程序設計上來說，本質上是進程通信。如果對進程通信比較瞭解，可以知道常用的進程通信方式有管道、命令管道、TCP/IP套接字、UNIX域套接字。

TCP/IP

TCP/IP套接字方式是MySQL數據庫在任何平臺下都提供的連接方式，也是網絡中使用得最多的一種方式。這種方式在TCP/IP連接上建立一個基於網絡的連接請求，一般情況下客戶端在一臺服務器上，而MySQL實例在另一臺服務器上，這兩臺服務器通過一個TCP/IP網絡連接。例如在Windows下使用Sqlyog工具進行連接遠程MySQL實例就是通過TCP/IP連接。（需要輸入MySQL的IP地址及端口號，也稱為一個套接字。同時MySQL實例必須要允許遠程的IP地址和用戶名進程連接訪問，默認無法連接。）

共享內存

MySQL提供共享內存的連接方式看著是通過在配置文件中添加–shared-memory實現的，如果想使用共享內存（屬於進程間通信）的方式，在MySQL命令行客戶端連接時還必須使用–protocol=memory選項即可。

UNIX域套接字

在Linux和Unix環境下，還可以使用Unix域套接字。Unix域套接字不是一個網絡協議，所以只能在MySQL客戶端和數據庫實例在一臺服務器上的情況下使用。可以再配置文件中指定套接字文件的路徑，如–socket=/tmp/mysql.sock，一般在數據庫安裝時就指定好了路徑。當數據庫實例啟動後，用戶就可以通過Uuix域套接字進行連接。
Mysql：MySQL命令行程序

MySQL命令行是MySQL提供的一個命令行客戶端工具，MySQL客戶端工具並不具體指某個客戶端軟件，事實上MySQL客戶端是一種複合概念。包含不同程序語言編寫的前端應用程序，如Sqlyog是一個非常好用的圖形化界面客戶端連接工具，MySQL自帶的基於命令行的客戶端，phpmyadmin是一個web界面的客戶端，以及調用API接口等都稱為MySQL客戶端。

mysql命令行連接服務器
[root@localhost ~]# mysql [OPTIONS] [database]
[OPTIONS]
--help：顯示幫助信息
-uroot：指定登錄用戶
-predhat：指定登錄密碼（密碼與-p之間不能有空格，否則無法識別）
-h localhost：指定要登錄的主機（這裡登錄的主機名必須在Mysql庫中有授權才可以）
-e "select * from mysql.user;"：在Linux命令行中執行SQL語句
-s -e ""：可以去掉MySQL顯示邊框
-N -s -e ""：可以去掉MySQL顯示邊框和標題信息
--socke=/tmp/mysql.sock：指定套接字文件（不指定會去默認位置查找）
--port：指定端口
--protocol=memory：指定連接協議{tcp|socket|pipe|memory}
--html：以HTML格式顯示數據內容
--xml：以XML格式顯示數據內容
1
2
3
4
5
6
7
8
9
10
11
12
13
14

[root@localhost ~]# mysql [OPTIONS] [database]
[OPTIONS]
--help：顯示幫助信息
-uroot：指定登錄用戶
-predhat：指定登錄密碼（密碼與-p之間不能有空格，否則無法識別）
-h localhost：指定要登錄的主機（這裡登錄的主機名必須在Mysql庫中有授權才可以）
-e "select * from mysql.user;"：在Linux命令行中執行SQL語句
-s -e ""：可以去掉MySQL顯示邊框
-N -s -e ""：可以去掉MySQL顯示邊框和標題信息
--socke=/tmp/mysql.sock：指定套接字文件（不指定會去默認位置查找）
--port：指定端口
--protocol=memory：指定連接協議{tcp|socket|pipe|memory}
--html：以HTML格式顯示數據內容
--xml：以XML格式顯示數據內容

示例：
[root@localhost ~]# mysql -uroot -predhat -h localhost --port 3306 --socket=/tmp/mysql.sock -s -N -e "select @@VERSION"
5.5.28
1
2

[root@localhost ~]# mysql -uroot -predhat -h localhost --port 3306 --socket=/tmp/mysql.sock -s -N -e "select @@VERSION"
5.5.28

#這裡-p指定的密碼，必須要提前使用mysqladmin工具設定好，默認root是沒有密碼的。另外上面的很多選項都可以省略，因為MySQL啟動時會根據配置文件中設定的參數來自行指定，如執行ps命令會看到如下進程信息：
[root@localhost ~]# ps aux | grep mysqld
/bin/sh /usr/local/mysql/bin/mysqld_safe --datadir=/mydata --pid-file=/mydata/localhost.localdomain.pid
/usr/local/mysql/bin/mysqld --basedir=/usr/local/mysql --datadir=/mydata --plugin-dir=/usr/local/mysql/lib/plugin --user=mysql --log-error=/mydata/error.log --pid-file=/mydata/localhost.localdomain.pid --socket=/tmp/mysql.sock --port=3306
1
2
3

[root@localhost ~]# ps aux | grep mysqld
/bin/sh /usr/local/mysql/bin/mysqld_safe --datadir=/mydata --pid-file=/mydata/localhost.localdomain.pid
/usr/local/mysql/bin/mysqld --basedir=/usr/local/mysql --datadir=/mydata --plugin-dir=/usr/local/mysql/lib/plugin --user=mysql --log-error=/mydata/error.log --pid-file=/mydata/localhost.localdomain.pid --socket=/tmp/mysql.sock --port=3306

mysql命令

MySQL命令行客戶端命令執行有兩個模式：交互式模式和批處理模式

交互式模式

客戶端命令(不需要在服務器上執行而是由客戶端從服務端獲取信息)
mysql> help
  #查看命令幫助
mysql> charset utf8
  #改變默認字符集
mysql> status
  #獲取MYSQL的工作狀態，如當前默認數據，字符集等
mysql> use DATABASE_NAME
  #設置默認數據庫
mysql> delimiter \\
  #設置SQL語句結束符,存儲過程中常用（默認是;號）
mysql> quit
  #退出MySQL
mysql> system
  #在Mysql登錄提示符下操作Linux主機如"system ls /root"命令
mysql> prompt MY>
  #修改MySQL的登錄提示符為“my>”
mysql> select * from mysql.user
  -> \c
  #\c是結束命令，一般用於退出當忘記使用SQL結束符進入的模式
mysql> select * from mysql.user\G
  #\G做SQL語句的結束符並以縱向顯示輸出結果
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21

mysql> help
  #查看命令幫助
mysql> charset utf8
  #改變默認字符集
mysql> status
  #獲取MYSQL的工作狀態，如當前默認數據，字符集等
mysql> use DATABASE_NAME
  #設置默認數據庫
mysql> delimiter \\
  #設置SQL語句結束符,存儲過程中常用（默認是;號）
mysql> quit
  #退出MySQL
mysql> system
  #在Mysql登錄提示符下操作Linux主機如"system ls /root"命令
mysql> prompt MY>
  #修改MySQL的登錄提示符為“my>”
mysql> select * from mysql.user
  -> \c
  #\c是結束命令，一般用於退出當忘記使用SQL結束符進入的模式
mysql> select * from mysql.user\G
  #\G做SQL語句的結束符並以縱向顯示輸出結果

批處理模式
一般用於導入以.sql結尾的命令集，有以下三種方式：
1.[root@localhost ~]# mysql -u root -h localhost < /root/mysql.sql
2.mysql> source /root/mysql.sql
3.mysql> \. /root/mysql.sql

命令行編輯功能
mysql> ctrl+a：快速移動光標至行首
mysql> ctrl+e：快速移動光標至行尾
mysql> ctrl+w：刪除光標之前的單詞
mysql> ctrl+u：刪除行首至光標處的所有內容
mysql> ctrl+y：粘貼使用ctrl+w或ctrl+u刪除的內容
1
2
3
4
5
6
7
8
9
10
11

一般用於導入以.sql結尾的命令集，有以下三種方式：
1.[root@localhost ~]# mysql -u root -h localhost < /root/mysql.sql
2.mysql> source /root/mysql.sql
3.mysql> \. /root/mysql.sql

命令行編輯功能
mysql> ctrl+a：快速移動光標至行首
mysql> ctrl+e：快速移動光標至行尾
mysql> ctrl+w：刪除光標之前的單詞
mysql> ctrl+u：刪除行首至光標處的所有內容
mysql> ctrl+y：粘貼使用ctrl+w或ctrl+u刪除的內容

Mysqladmin：管理MySQL服務器客戶端工具

語法如下:
[root@localhost ~]# mysqladmin -u root -h localhost -p [Command] [arguments]
-u root：指定登錄用戶
-h localhost：指定要登錄的主機（本機可省略）
-p：指定密碼，密碼可以直接跟在-p後面

[Command]
passwd 'new password'  [-p 'old password']
  #設置密碼，其中“-p ’old password’”只有更換密碼時才需要執行指定舊密碼，第一次指定密碼時不需要
create DATABASE_NAME
  #創建數據庫
drop DATABASE_NAME
  #刪除數據庫
processlist
  #顯示當前主機正在執行的任務列表
status
  #顯示Mysql的服務器狀態信息，如啟動時間
status --sleep 2 --count 2
  #2秒顯示一次一共顯示2此
extended-status
  #顯示MYSQL狀態變量及其值
variables
  #顯示MYSQL服務器變量及其值
shutdown
  #終止MySQL服務
flush-privileges
  #刷新授權表
flush-threads
  #清空線程緩存
flush-status
  #重置大多數的服務器狀態變量
flush-logs
  #刷新日誌
start-slave
stop-slave
  #啟動和關閉Slave
version
  #查看MYSQL的版本和狀態信息
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37

[root@localhost ~]# mysqladmin -u root -h localhost -p [Command] [arguments]
-u root：指定登錄用戶
-h localhost：指定要登錄的主機（本機可省略）
-p：指定密碼，密碼可以直接跟在-p後面

[Command]
passwd 'new password'  [-p 'old password']
  #設置密碼，其中“-p ’old password’”只有更換密碼時才需要執行指定舊密碼，第一次指定密碼時不需要
create DATABASE_NAME
  #創建數據庫
drop DATABASE_NAME
  #刪除數據庫
processlist
  #顯示當前主機正在執行的任務列表
status
  #顯示Mysql的服務器狀態信息，如啟動時間
status --sleep 2 --count 2
  #2秒顯示一次一共顯示2此
extended-status
  #顯示MYSQL狀態變量及其值
variables
  #顯示MYSQL服務器變量及其值
shutdown
  #終止MySQL服務
flush-privileges
  #刷新授權表
flush-threads
  #清空線程緩存
flush-status
  #重置大多數的服務器狀態變量
flush-logs
  #刷新日誌
start-slave
stop-slave
  #啟動和關閉Slave
version
  #查看MYSQL的版本和狀態信息

Mysqlshow：顯示數據庫，表，列信息

[root@localhost ~]# mysqlshow [options] [db_name [tbl_name [col_name]]]
如果沒有給出數據庫，顯示所有匹配的數據庫。
如果沒有給出表，顯示數據庫中所有匹配的表。
如果沒有給出列，顯示錶中所有匹配的列和列類型。
[options]
--help：詳細幫助信息
-uroot：指定用戶
-predhat：指定密碼

[root@localhost ~]# mysqlshow -uroot -predhat
  #查看所有數據庫
[root@localhost ~]# mysqlshow -uroot -predhat mysql
  #查看mysql庫的所有表
[root@localhost ~]# mysqlshow -uroot -predhat mysql user
  #查看mysql庫的user表所有記錄
[root@localhost ~]# mysqlshow -uroot -predhat mysql user show-table-type
  #查看錶的類型
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17

[root@localhost ~]# mysqlshow [options] [db_name [tbl_name [col_name]]]
如果沒有給出數據庫，顯示所有匹配的數據庫。
如果沒有給出表，顯示數據庫中所有匹配的表。
如果沒有給出列，顯示錶中所有匹配的列和列類型。
[options]
--help：詳細幫助信息
-uroot：指定用戶
-predhat：指定密碼

[root@localhost ~]# mysqlshow -uroot -predhat
  #查看所有數據庫
[root@localhost ~]# mysqlshow -uroot -predhat mysql
  #查看mysql庫的所有表
[root@localhost ~]# mysqlshow -uroot -predhat mysql user
  #查看mysql庫的user表所有記錄
[root@localhost ~]# mysqlshow -uroot -predhat mysql user show-table-type
  #查看錶的類型

Mysql_safe：這是一個腳本程序，用來安全啟動Mysqld服務

在上面我們查看mysql進程的時候發現，不光有mysqld還有一個使用bash執行的mysqld_safe程序。mysqld_safe是用來安全啟動mysqld程序的，一般直接運行mysqld程序來啟動MySQL服務的方法很少見，mysqld_safe增加了一些安全特性，例如當出現錯誤時重啟服務器並向錯誤日誌文件寫入運行時間信息。

在類UNIX系統中推薦使用mysqld_safe來啟動mysqld服務器。mysqld_safe從選項配置文件的[mysqld]、[server]和 [mysqld_safe]部分讀取所有選項。其實我們平時使用mysql.server腳本其實也是調用mysqld_safe腳本去啟動MySQL服務器的，所以如果你直接去啟動mysqld程序而不是用腳本的話一般都會有問題。

mysqld_safe通常做如下事情：

1.檢查系統和選項。

2.檢查MyISAM表。

3.保持MySQL服務器窗口。

4.啟動並監視mysqld，如果因錯誤終止則重啟。

5.將mysqld的錯誤消息發送到數據目錄中的host_name.err 文件。

6.將mysqld_safe的屏幕輸出發送到數據目錄中的host_name.safe文件。
-->


## mysql_tzinfo_to_sql

該命令用於加載時區表，首先看下 MySQL 中時區的設置。

{% highlight text %}
----- 查看MySQL當前時間以及當前時區
mysql> SELECT curtime();                      # 也可以使用now()
mysql> SHOW VARIABLES LIKE "%time_zone%";
+------------------+--------+
|   Variable_name  | Value  |
+------------------+--------+
| system_time_zone | CST    |                 # system使用CST時區
| time_zone        | SYSTEM |                 # 使用system時區，也就是上述的參數
+------------------+--------+
2 rows in set (0.00 sec)

----- 修改MySQL時區為北京時間，也即東8區；並立即生效
mysql> SET [GLOBAL | SESSION] time_zone='+8:00';
mysql> FLUSH PRIVILEGES;

----- 也可以直接修改配置文件，需要重啟服務器
$ cat /etc/my.cnf
[mysqld]
default-time_zone = '+8:00'
{% endhighlight %}

Linux 系統中，與時區相關的內容保存在 /usr/share/zoneinfo 目錄下；MySQL 中與時區相關的信息保存在 mysql.time_zone% 表中，可以通過該命令將時區信息導入到 MySQL 中。

{% highlight text %}
----- 將全部時區信息，或者指定時區信息轉換為SQL，並導入到MySQL中
$ mysql_tzinfo_to_sql /usr/share/zoneinfo | mysql -u root mysql
$ mysql_tzinfo_to_sql tz_file tz_name | mysql -u root mysql

----- 如果有閏秒，則同時設置
$ mysql_tzinfo_to_sql --leap tz_file | mysql -u root mysql
{% endhighlight %}


## mysqlslap

這是一個壓測工具，源碼在 client/mysqlslap.c，測試的步驟為：A) 創建數據庫+表；B) 插入測試數據；C) 執行壓測；D) 刪除創建的數據庫。

常見參數為：

{% highlight text %}
--delimiter
    分隔符，用於分割命令行或文件指定的SQL；
--auto-generate-sql, -a
    自動生成測試表和數據，並自動生成 SQL 腳本來測試併發壓力；
--create-schema (mysqlslap)
    測試 schema 名稱，也就是 database；
--only-print
    只打印測試語句而不實際執行；
--concurrency=N, -c N
    控制併發，用於模擬多少個客戶端同時執行select，可以指定多個數值，用於不同迭代時的併發；
--iterations=N, -i N
    執行的迭代次數，代表要在不同併發環境下，各自運行測試多少次；
--number-of-queries=N
    總的測試查詢次數，其值 = 併發客戶數 x 每次查詢數；
--debug-info, -T
    最後執行結果打印內存和 CPU 的相關信息；
--auto-generate-sql-load-type=type
    測試語句的類型，代表要測試的環境是讀操作還是寫操作還是兩者混合的，取值包括：read，key，write，update和mixed(默認)；
--auto-generate-sql-add-auto-increment
    代表對生成的表自動添加auto_increment列，從5.1.18版本開始支持；
--number-char-cols=N, -x N
    自動生成的測試表中包含多少個字符類型的列，默認1；
--number-int-cols=N, -y N
    自動生成的測試表中包含多少個數字類型的列，默認1；
--query=name,-q
    使用自定義腳本執行測試，例如可以調用自定義的一個存儲過程或者sql語句來執行測試；
--commint=N
    多少條DML後提交一次；
--compress, -C
    如果服務器和客戶端支持都壓縮，則壓縮信息傳遞；
--engine=engine_name, -e engine_name
    代表要測試的引擎，可以有多個，用分隔符隔開。例如：--engines=myisam,innodb；
--detach=N
    執行N條語句後斷開重連。
{% endhighlight %}

如下是常見的測試用例。

{% highlight text %}
----- 單線程測試，打印執行的SQL，查看具體做了什麼
$ mysqlslap --auto-generate-sql --only-print -uroot -pYourPassword

----- 設置併發以及循環次數，也就是說在併發為10,20,50時，均執行4次，返回10,20,50時的統計結果
$ mysqlslap --auto-generate-sql --concurrency=10,20,50 --iterations=4 --number-of-queries 1000 \
    -uroot -pYourPassword

----- 測試不同類型存儲引擎的性能指標
$ mysqlslap --auto-generate-sql --concurrency=10,20,50 --iterations=4 --number-of-queries 1000 \
    --engine=myisam,innodb -uroot -pYourPassword
{% endhighlight %}

## mysqldump

這是一個邏輯備份工具，其處理流程基本如下。

{% highlight text %}
01 Connect     root@localhost on
02 Query       /*!40100 SET @@SQL_MODE='' */
03 Init DB     foobar
04 Query       SHOW TABLES LIKE 'foobar'
05 Query       LOCK TABLES `foobar` READ /*!32311 LOCAL */
06 Query       SET OPTION SQL_QUOTE_SHOW_CREATE=1
07 Query       SHOW CREATE TABLE `foobar`
08 Query       SHOW FIELDS FROM `foobar`
09 Query       SHOW TABLE STATUS LIKE 'foobar'
11 Query       SELECT * FROM `foobar`
12 Query       UNLOCK TABLES
13 Quit
{% endhighlight %}

接下來，主要查看一些常見的參數。

{% highlight text %}
-q
    導出數據時加了一個SQL_NO_CACHE來確保不會讀取緩存裡的數據，第11行修改如下；
    Query       SELECT /*!40001 SQL_NO_CACHE */ * FROM `foobar`

--lock-tables
    跟上面類似，不過多加了一個READ LOCAL LOCK，該鎖不會阻止讀，也不會阻止新的數據插入；

--lock-all-tables
    備份前會發起一個全局的讀鎖，會阻止對所有表的寫入，以此確保數據的一致性，備份完成後會話斷開，會自動解鎖；
    會在開頭增加如下命令；
    Query       FLUSH TABLES
    Query       FLUSH TABLES WITH READ LOCK

--master-data
    和--lock-all-tables參數相同，同時多了個SHOW MASTER STATUS命令；
    Query       FLUSH TABLES
    Query       FLUSH TABLES WITH READ LOCK
    Query       SHOW MASTER STATUS

--single-transaction
    InnoDB表在備份時，通常會啟用參數來保證備份的一致性，其工作原理是設定本次會話的隔離級別為REPEATABLE READ，
    以確保本次會話(dump)時，不會看到其他會話已經提交了的數據；同樣開始增加如下命令：
    Query       SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ
    Query       BEGIN
    Query       UNLOCK TABLES
{% endhighlight %}



<!--
 6. --single-transaction and --master-data
 本例中，由於增加了選項 --master-data，因此還需要提交一個快速的全局讀鎖。在這裡，可以看到和上面的不同之處在於少了發起 BEGIN 來顯式聲明事務的開始。這裡採用 START TRANSACTION WITH CONSISTENT SNAPSHOT 來代替 BEGIN 的做法的緣故不是太瞭解，可以看看源代碼來分析下。

 081022 17:27:07       2 Connect     root@localhost on
 2 Query       /*!40100 SET @@SQL_MODE='' */
 2 Query       FLUSH TABLES
 2 Query       FLUSH TABLES WITH READ LOCK
 2 Query       SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ
 2 Query       START TRANSACTION WITH CONSISTENT SNAPSHOT
 2 Query       SHOW MASTER STATUS
 2 Query       UNLOCK TABLES
 2 Init DB     foobar
 2 Query       SHOW TABLES LIKE 'foobar'
 2 Query       SET OPTION SQL_QUOTE_SHOW_CREATE=1
 2 Query       show create table `foobar`
 2 Query       show fields from `foobar`
 2 Query       show table status like 'foobar'
 2 Query       SELECT /*!40001 SQL_NO_CACHE */ * FROM `foobar`
 2 Quit

 關於隔離級別可以看手冊 13.2.10.3. InnoDB and TRANSACTION ISOLATION LEVEL，或者本站之前的文章：[InnoDB系列] - 實例解析Innodb的隔離級別以及鎖模式。

 關於 START TRANSACTION WITH CONSISTENT SNAPSHOT 的說明可以看下手冊描述：

 The WITH CONSISTENT SNAPSHOT clause starts a consistent read for storage engines that are capable of it. This applies only to InnoDB. The effect is the same as issuing a START TRANSACTION followed by a SELECT from any InnoDB  table. See Section 13.2.10.4, “Consistent Non-Locking Read”. The WITH CONSISTENT SNAPSHOT clause does not change the current transaction isolation level, so it provides a consistent snapshot only if the current isolation level is one that allows consistent read (REPEATABLE READ or SERIALIZABLE).

 12.4.1. START TRANSACTION, COMMIT, and ROLLBACK Syntax

mysqldump -u root -p --all-databases –flush-privileges --single-transaction --flush-logs --triggers --routines --events –hex-blob > /path/to/backupdir/full_backup-$TIMESTAMP.sql
-->











## mysqldumpslow

該命令用於處理慢查詢日誌，這是一個 Perl 腳本程序。首先，需要設置好配置文件。

{% highlight text %}
$ cat /etc/my.cnf
[mysqld]
long_query_time=2                    # 設置慢查詢的超時時間，單位為秒
log-slow-queries=slow-query.log
{% endhighlight %}

另外，可以通過 log-queries-not-using-indexes 參數設置沒有使用索引的 SQL 。

可以使用該命令查看慢查詢日誌，常用的命令可以通過如下方式查看。

{% highlight text %}
常用參數：
   -h    : 查看幫助信息；
   -t NUM: 顯示頭NUM條記錄；
   -s ARG: 排序參數，有如下的選項，其中前面添加a表示倒序，如ac、at、al、ar；
        c: 記錄的次數；
        t: 查詢時的時間戳；
        l: 查詢使用的時間；
        r: 返回記錄數目；
   -g REG: 根據正則表達式進行匹配，大小寫不敏感；

實例：
----- 返回訪問次數最多的20個SQL語句
$ mysqldumpslow -s c -t 20 slow-query.log
----- 按照時間返回前10條裡面含有左連接的SQL語句
$ mysqldumpslow -s t -t 10 -g "left join" slow-query.log
{% endhighlight %}

通過這個工具可以查詢出來那些 SQL 語句是性能的瓶頸，如下是上述命令的輸出：

{% highlight text %}
Count: 2  Time=2.31s (5s)  Lock=0.00s (0s)  Rows=1000.0 (2000), root[root]@[localhost]
    SELECT * FROM sms_send WHERE service_id<=N GROUP BY content LIMIT N, N;

輸出解析：
    出現次數(Count) 2；
    最大耗時時間(Time) 2.31s；
    累計總耗費時間(Time) 5s；
    等待鎖的時間(Lock) 0.00s；
    等待鎖的總耗時(Lock) 0s；
    發送給客戶端的行總數(Rows) 1000；
    掃描的行總數(Rows) 2000；
    用戶以及SQL語句本身，其中的數字會被替換為N。
{% endhighlight %}


## mysqlcheck

數據庫經常可能遇到錯誤，譬如數據寫入磁盤時發生錯誤、索引沒有同步更新、數據庫宕機等；從而可能會導致數據庫異常。

mysqlcheck 的功能類似 myisamchk，但其工作不同，前者需要在服務器運行的時候執行，而後者需要停服務。

實際上，mysqlcheck 只是提供了一種方便的使用 SQL 語句的方式，會根據不同類型拼接 SQL 語句，真正調用的還是 ```CHECK TABLE```、```REPAIR TABLE```、```ANALYZE TABLE``` 和 ```OPTIMIZE TABLE``` 命令。

可以通過 3 種方式來調用 mysqlcheck 。

{% highlight text %}
----- 檢查表
$ mysqlcheck [options] db_name [tables]

----- 檢查多個數據庫
$ mysqlcheck [options] ---database DB1 [DB2 DB3...]

----- 檢查所有的數據庫
$ mysqlcheck [options] --all--database


選項：
  --database，-B
    指定數據庫名，可以為多個；
  --all--database，-A
    檢查所有數據庫；

{% endhighlight %}

<!--
·         --analyze，-a
分析表。

·         --all-in-1，-1
不是為每個表發出一個語句，而是為命名數據庫中待處理的所有表的每個數據庫執行一個語句。

·         --auto-repair
如果某個被檢查的表破壞了，自動修復它。檢查完所有表後自動進行所有需要的修復。

·         --character-sets-dir=path
字符集的安裝目錄。參見5.10.1節，“數據和排序用字符集”。

·         --check，-c
檢查表的錯誤。

·         --check-only-changed，-C
只檢查上次檢查以來已經更改的或沒有正確關閉的表。

·         --compress
壓縮在客戶端和服務器之間發送的所有信息（如果二者均支持壓縮）。

·         ---debug[=debug_options]，-# [debug_options]

寫調試日誌。debug_options字符串通常為'd:t:o,file_name'。

·         --default-character-set=charset

使用charsetas默認字符集。參見5.10.1節，“數據和排序用字符集”。

·         --extended，-e

如果你正使用該選項來檢查表，可以確保它們100%地一致，但需要很長的時間。

如果你正使用該選項來修復表，則運行擴展修復，不但執行的時間很長，而且還會產生大量的垃圾行!

·         --fast，-F

只檢查沒有正確關閉的表。

·         --force，-f

即使出現SQL錯誤也繼續。

·         --host=host_name，-h host_name

連接給定主機上的MySQL服務器。

·         --medium-check，-m

執行比--extended操作更快的檢查。只能發現99.99%的錯誤，在大多數情況下這已經足夠了。

·         --optimize，-o

優化表。

·         --password[=password]，-p[password]

當連接服務器時使用的密碼。如果使用短選項形式(-p)，選項和 密碼之間不能有空格。如果在命令行中--password或-p選項後面沒有 密碼值，則提示輸入一個密碼。

·         --port=port_num，-P port_num

用於連接的TCP/IP端口號。

·         --protocol={TCP | SOCKET | PIPE | MEMORY}

使用的連接協議。

·         --quick，-q

如果你正使用該選項在檢查表，它防止掃描行以檢查錯誤鏈接的檢查。這是最快的檢查方法。

如果你正使用該選項在修復表，它嘗試只修復索引樹。這是最快的修復方法。

·         --repair，-r

執行可以修復大部分問題的修復，只是唯一值不唯一時不能修復。

·         --silent，-s

沉默模式。只打印錯誤消息。

·         --socket=path，-S path

用於連接的套接字文件。

·         --tables

覆蓋---database或-B選項。選項後面的所有參量被視為表名。

·         --user=user_name，-u user_name

當連接服務器時使用的MySQL用戶名。

·         --verbose，-v

冗長模式。打印關於各階段程序操作的信息。

·         --version，-V

顯示版本信息並退出。





同其它客戶端比較，mysqlcheck有一個特殊特性。重新命名二進制可以更改檢查表的默認行為(--check)。如果你想要一個工具默認可以修復表的工具，只需要將mysqlcheck重新複製為mysqlrepair，或者使用一個符號鏈接mysqlrepair鏈接mysqlcheck。如果調用mysqlrepair，可按照命令修復表。

下面的名可用來更改mysqlcheck的默認行為：
mysqlrepair
默認選項為--repair

mysqlanalyze
默認選項為--analyze

mysqloptimize
默認選項為--optimize

-->

源碼在 client/check 目錄下，處理過程簡單介紹如下。


{% highlight text %}
main()
 |-get_options()                    ← 加載配置文件默認配置
 | |-load_defaults()                ← 通過load_default_groups指定配置文件加載的groups
 |
 |-mysql_check()
   |-disable_binlog()               ← 根據參數設置SET SQL_LOG_BIN=0
   |-process_all_databases()        ← 處理所有數據庫
   | |-process_one_db()
   |   |-process_all_tables_in_db()
   |     |-process_selected_tables()
   |
   |-process_selected_tables()
   | |-handle_request_for_tables()  ← 真正的拼接命令處
   |
   |-process_databases()
{% endhighlight %}

其中 handle_request_for_tables() 函數的處理流程如下。

{% highlight cpp %}
static int handle_request_for_tables(string tables)
{
  string operation, options;

  switch (what_to_do) {
  case DO_CHECK:
    operation = "CHECK";
    if (opt_quick)              options+= " QUICK";
    if (opt_fast)               options+= " FAST";
    if (opt_medium_check)       options+= " MEDIUM"; /* Default */
    if (opt_extended)           options+= " EXTENDED";
    if (opt_check_only_changed) options+= " CHANGED";
    if (opt_upgrade)            options+= " FOR UPGRADE";
    break;
  case DO_REPAIR:
    operation= (opt_write_binlog) ? "REPAIR" : "REPAIR NO_WRITE_TO_BINLOG";
    if (opt_quick)              options+= " QUICK";
    if (opt_extended)           options+= " EXTENDED";
    if (opt_frm)                options+= " USE_FRM";
    break;
  case DO_ANALYZE:
    operation= (opt_write_binlog) ? "ANALYZE" : "ANALYZE NO_WRITE_TO_BINLOG";
    break;
  case DO_OPTIMIZE:
    operation= (opt_write_binlog) ? "OPTIMIZE" : "OPTIMIZE NO_WRITE_TO_BINLOG";
    break;
  case DO_UPGRADE:
    return fix_table_storage_name(tables);
  }

  string query= operation + " TABLE " + tables + " " + options;

  if (mysql_real_query(sock, query.c_str(), (ulong)query.length()))
  {
    DBError(sock,
      "when executing '" + operation + " TABLE ... " + options + "'");
    return 1;
  }
  print_result();
  return 0;
}
{% endhighlight %}

也即是實際上會生成如下的命令。

{% highlight text %}
   CHECK TABLE table_name {QUICK|FAST|MEDIUM|EXTENDED|CHANGED|FOR UPGRADE}
  REPAIR NO_WRITE_TO_BINLOG TABLE table_name {QUICK|EXTENDED|USE_FRM}
 ANALYZE NO_WRITE_TO_BINLOG TABLE table_name
OPTIMIZE NO_WRITE_TO_BINLOG TABLE table_name
{% endhighlight %}

每個表會記錄最近的一次檢查時間，可以通過如下命令查看。

{% highlight text %}
mysql> SELECT table_name, check_time FROM information_schema.tables
          WHERE table_name = 'tbl-name' AND table_schema = 'db-name';
{% endhighlight %}

<!--
create databasename             創建一個新數據庫
drop databasename               刪除一個數據庫及其所有表
flush-hosts                     洗掉所有緩存的主機
flush-logs                      洗掉所有日誌
flush-tables                    洗掉所有表
flush-privileges                再次裝載授權表(同reload)
kill id,id,...                  殺死mysql線程
reload                          重載授權表
refresh                         洗掉所有表並關閉和打開日誌文件
-->

## mysqladmin

該工具最常見的是用來關閉數據庫，還可以查看 MySQL 運行狀態、進程信息、關閉進程等，如下是常用的子命令；所有命令可以通過 ```--help``` 查看幫助文檔。

{% highlight text %}
mysqladmin [option] command [command-option] command ......
參數如下：
  extended-status
    可獲得所有MySQL性能指標，即SHOW GLOBAL STATUS的輸出
  status
    獲取當前MySQL的幾個基本的狀態值，包括線程數、查詢量、慢查詢等
  variables
    打印出可用變量
  ping
    查看服務器是否存活
  shutdown
    關掉服務器
  processlist
    顯示服務其中活躍線程列表
  version
    得到服務器的版本信息
  password 'new-password'
    新口令，將老口令改為新口令
{% endhighlight %}


### extended-status

默認輸出的都是累計值，可以通過 ```-r/--relative``` 查看各個指標的差值；然後再配合 ```-i/--sleep``` 指定刷新的頻率。

{% highlight text %}
$ mysqladmin -uroot -pnew-password -h127.1 -P3307 -r -i 1 extended-status |\
   grep "Questions\|Queries\|Innodb_rows\|Com_select \|Com_insert \|Com_update \|Com_delete"

$ mysqladmin -uroot -pnew-password -h127.1 -P3307 -r -i 1 extended-status |\
   awk -F"|" '{\
     if($2 ~ /Variable_name/){\
       print " <-------------    "  strftime("%H:%M:%S") "    ------------->";\
     }\
     if($2 ~ /Questions|Queries|Innodb_rows|Com_select |Com_insert |Com_update |Com_delete/)\
       print $2 $3;\
   }'

$ mysqladmin -uroot -pnew-password -h127.1 -P3307 -r -i 1 extended-status |\
   awk 'BEGIN{ FS="|"; count=0; } { \
   if($2 ~ /Variable_name/ && ((++count)%20 == 1)){ \
       print "----------|---------|--- MySQL Command Status --|----- Innodb row operation ----";\
       print "---Time---|---QPS---|select insert update delete|  read inserted updated deleted";\
   } \
   else if ($2 ~ /Queries/){queries=$3;}\
   else if ($2 ~ /Com_select /){com_select=$3;}\
   else if ($2 ~ /Com_insert /){com_insert=$3;}\
   else if ($2 ~ /Com_update /){com_update=$3;}\
   else if ($2 ~ /Com_delete /){com_delete=$3;}\
   else if ($2 ~ /Innodb_rows_read/){innodb_rows_read=$3;}\
   else if ($2 ~ /Innodb_rows_deleted/){innodb_rows_deleted=$3;}\
   else if ($2 ~ /Innodb_rows_inserted/){innodb_rows_inserted=$3;}\
   else if ($2 ~ /Innodb_rows_updated/){innodb_rows_updated=$3;}\
   else if ($2 ~ /Uptime / && count >= 2){\
     printf(" %s |%9d",strftime("%H:%M:%S"),queries);\
     printf("|%6d %6d %6d %6d",com_select,com_insert,com_update,com_delete);\
     printf("|%6d %8d %7d %7d\n",innodb_rows_read,innodb_rows_inserted,innodb_rows_updated,innodb_rows_deleted);\
   }}'
{% endhighlight %}

## mysqlbinlog

二進制日誌 (Binary Log) 是由多個事件 (events) 組成，用於描述對於數據庫的修改內容，MySQL 服務器以二進制的形式寫入，可以通過該工具顯示文件中具體事件的內容。

{% highlight text %}
----- 備份時指定多個binlog文件
$ mysqlbinlog --stop-date="2015-04-20 9:59:59" mysql-bin.[0-9]* | \
    mysql -u root -pyour-password

----- 只恢復單個庫example
$ mysqlbinlog --stop-date="2015-04-20 9:59:59" mysql-bin.000001 | \
    mysql -u root -pyour-password --one-database example

----- 指定起始時間以及庫
$ mysqlbinlog --start-datetime='2015-08-05 00:00:00' --stop-datetime='2015-08-05 10:00:00' \
    --database=db_name mysql-bin.000001

----- 也可以指定binlog的position位置
$ mysqlbinlog --start-postion=107 --stop-position=1000 --database=db_name mysql-bin.000001

----- 從遠程服務器讀取
$ mysqlbinlog -u username -p password -h127.1 -P3306 --read-from-remote-server \
    --start-datetime='2015-08-05 00:00:00' --stop-datetime='2015-08-05 10:00:00' mysql-bin.000001
{% endhighlight %}

### ROW格式解析

首先準備下數據。

{% highlight sql %}
CREATE DATABASE test;
USE test;
CREATE TABLE foobar (
    id INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name CHAR(20) NOT NULL,
    sex ENUM('F','M') NOT NULL DEFAULT 'M',
    address VARCHAR(30) NOT NULL
) Engine=InnoDB;
INSERT INTO foobar(name,sex,address) VALUES('Barton','M','Washington'),('Borg','M','New Mexico'),
    ('Steven','M','Colorado');
UPDATE foobar SET address='Texas';
{% endhighlight %}

可以直接通過 mysqlbinlog 解析。

{% highlight text %}
----- 解析ROW格式binlog文件
$ mysqlbinlog --no-defaults -v -v --base64-output=DECODE-ROWS mysql-bin.000003
... ...
### INSERT INTO `test`.`foobar`
### SET
###   @1=1 /* INT meta=0 nullable=0 is_null=0 */
###   @2='Barton' /* STRING(60) meta=65084 nullable=0 is_null=0 */
###   @3=2 /* ENUM(1 byte) meta=63233 nullable=0 is_null=0 */
###   @4='Washington' /* VARSTRING(90) meta=90 nullable=0 is_null=0 */
... ...
### UPDATE `test`.`foobar`
### WHERE
###   @1=3 /* INT meta=0 nullable=0 is_null=0 */
###   @2='Steven' /* STRING(60) meta=65084 nullable=0 is_null=0 */
###   @3=2 /* ENUM(1 byte) meta=63233 nullable=0 is_null=0 */
###   @4='Colorado' /* VARSTRING(90) meta=90 nullable=0 is_null=0 */
### SET
###   @1=3 /* INT meta=0 nullable=0 is_null=0 */
###   @2='Steven' /* STRING(60) meta=65084 nullable=0 is_null=0 */
###   @3=2 /* ENUM(1 byte) meta=63233 nullable=0 is_null=0 */
###   @4='Texas' /* VARSTRING(90) meta=90 nullable=0 is_null=0 */
{% endhighlight %}

@1、@2、@3、@4 分別代表了第 1~4 列。

<!--
MySQL 誤操作後數據恢復（update,delete忘加where條件）
http://www.cnblogs.com/gomysql/p/3582058.html
-->

## innochecksum

使用該工具時必須停止 MySQL 服務器，否則會報 "Unable to lock file" 錯誤，如果是在線的最好使用 ```CHECK TABLE``` 命令。

簡單來說，該工具會讀取 InnoDB 的表空間，計算每個頁的 Checksum 值，然後與頁中的值比較，如果不同則會報錯。

{% highlight text %}
innochecksum --verbose=FALSE --log=/tmp/innocheck.log
常用參數：
  --help/info
    查看幫助信息；
  --verbose
    打印詳細信息，可以通過--verbose=FALSE關閉；
  --count/-c
    只打印頁的數量信息；
  --start-page=NUM/-s NUM; --end-page=NUM/-e NUM; --page=NUM/-p NUM
    指定開始、結束頁，或者只查看指定的頁；
  --strict-check/-C
    指定checksum算法，通常有innodb、crc32、none，默認是從三者中選擇，否則強制指定；
  --page-type-summary/-S
    打印文件中頁的統計信息，包括了總頁類型以及數量；
  --page-type-dump=file-name/-D file-name
    打印各個頁的詳細信息，將其輸出到一個文件中；

常用示例：
  ----- 檢查系統表空間，也可以使用table-name.ibd，默認出錯時才會輸出異常
  innodbchecksum ibdata1
  ----- 保存文件中各個頁的信息，並在最後打印統計信息
  innodbchecksum -S -D /tmp/page.info schema/*.ibd
{% endhighlight %}


詳細使用文檔可以參考 [innochecksum](https://dev.mysql.com/doc/refman/en/innochecksum.html) 。

<!--
innochecksum /tmp/data/*/*.ibd
-->

## my_print_defaults

會按照順序讀取配置文件，並提取相應屬組的配置項，可以指定多個屬組。

{% highlight text %}
----- 使用示例
$ my_print_defaults mysqlcheck client
--user=myusername
--password=secret
--host=localhost

常見參數如下：
  --config-file=file_name, --defaults-file=file_name, -c file_name
    只讀取如上選項指定的配置文件。
  --defaults-extra-file=file_name, --extra-file=file_name, -e file_name
    讀取全局配置項且在讀取用戶配置前的配置文件。
{% endhighlight %}

如果不添加任何參數，可以看到配置文件默認的加載順序。

{% highlight text %}
Default options are read from the following files in the given order:
/etc/mysql/my.cnf /etc/my.cnf ~/.my.cnf
{% endhighlight %}

## 參考

關於 MySQL 自帶的程序，可以直接參考官方網站 [Reference Manual - MySQL Programs](https://dev.mysql.com/doc/refman/en/programs.html) 。

{% highlight text %}
{% endhighlight %}
