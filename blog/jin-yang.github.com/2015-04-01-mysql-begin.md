---
title: MySQL 寫在開頭
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql
description: 保存一下經常使用的經典 MySQL 資源。
---

保存一下經常使用的經典 MySQL 資源。

<!-- more -->

## 常用網站

記錄一些常用的網站。

### 官方網站

* [MySQL Documentation](http://dev.mysql.com/doc/)，MySQL 官方網站的參考文檔，經常使用的包括了 [MySQL Reference Manual](http://dev.mysql.com/doc/refman/en/)、[MySQL Internals Manual](http://dev.mysql.com/doc/internals/en/)，還包括了一些 API 接口。

* [MySQL-InnoDB](https://blogs.oracle.com/mysqlinnodb/) InnoDB 的官方 blog ，很多不錯的文章。

* [MySQL Server Blog](http://mysqlserverteam.com/)、[MySQL High Availability](http://mysqlhighavailability.com/) 。

* [Contributing Code to MySQL](https://community.oracle.com/docs/DOC-914911)，介紹如何向 MySQL 社區貢獻代碼。

* [Worklog Tasks](https://dev.mysql.com/worklog/)，也就是 MySQL 的任務列表，很多需求均可以在此查看。

### 數據庫排名

關於當前數據庫的排名比較有權威的是 [DB-Engines](http://db-engines.com/)，包括了關係型數據庫、時序型數據庫等等，其排名的數據計算是依據幾個不同的因素，其中包括了：

* Google 以及 Bing 搜索引擎的關鍵字搜索數量；
* Google Trends 的搜索數量；
* Indeed 網站中的職位搜索量；
* LinkedIn 中提到關鍵字的個人資料數；
* Stackoverflow 上相關的問題和關注者數量。

其中各種類型數據庫的綜合排名可以參考 [DB-Engines Ranking](http://db-engines.com/en/ranking)，各個數據庫的排名趨勢則可以參考 [DB-Engines Ranking Trend](http://db-engines.com/en/ranking_trend) 。


## Blogs

一些比較常見、經典的 Blogs 。

### Jeremy Cole

目前在 Google 任職的大牛，分別在 MySQL AB、Yahoo、Twitter、Google 待過，看他的 blog 竟然還玩過 AVR 的 ^_^

可以參考其個人 blog：[Jeremy Cole](https://blog.jcole.us/) ，很大一部分是與 InnoDB 文件格式相關的內容，寫的不錯。

其維護了一個使用 Ruby 語言編寫的 innodb_ruby 工具，用來查看 Innodb 的數據文件狀態，當然只適用於實驗測試環境。

innodb_ruby 工具的源碼可以參考 [Github](https://github.com/jeremycole/innodb_ruby/)，其安裝、使用方法可以參考 [Github WiKi](https://github.com/jeremycole/innodb_ruby/wiki) 。


## 經典文章

* [How does a relational database work](http://coding-geek.com/how-databases-work/)，一篇很好的文章，對關係型數據庫的初步介紹。

* [An Outline for a Book on InnoDB](http://www.xaprb.com/blog/2015/08/08/innodb-book-outline/)，關於 InnoDB 的一些關鍵技術點。

* [Awesome MySQL](http://shlomi-noach.github.io/awesome-mysql/)，介紹很多與 MySQL 相關的軟件、資源等。

## 經典課程

* [Stanford 數據庫課程](http://web.stanford.edu/class/cs245/)，介紹了一些常用的數據庫工具，如 [Innodb diagrams](https://github.com/jeremycole/innodb_diagrams)、[Innodb Ruby](https://github.com/jeremycole/innodb_ruby/) 。

<!--
<a href="http://www.innomysql.net/">Inside MySQL</a> 網易姜承堯的 blog 。<br><br>
<a href="https://github.com/percona/tokudb-engine">tokudb-engine github</a>，據說一個很牛掰的存儲引擎，與 InnoDB 類似，percona 實現的。<br><br>
<a href="http://www.gpfeng.com/">Learn AND live</a> <br><br>
<a href="http://mysql.taobao.org/index.php?title=%E9%A6%96%E9%A1%B5">淘寶MySQL</a>，官方 blog 。<br><br>
http://cn.planet.mysql.com/
hotpu-meeting.b0.upaiyun.com/2014dtcc/post_pdf/hedengcheng.pdf




http://www.ywnds.com/?cat=31
http://mysqllover.com/?p=594
http://siddontang.com/?p=594
http://keithlan.github.io/      binlog
http://hatemysql.com/           binlog

備份
http://apprize.info/php/effective/index.html


http://www.cnblogs.com/cenliang/p/4908085.html MySQL高可用方案，包括相關資料


maatkit MySQL工具集

http://cybend.com/html/201602/a35.htm

https://www.percona.com/blog/2009/05/14/why-mysqls-binlog-do-db-option-is-dangerous/

http://keithlan.github.io/2015/11/02/mysql_replicate_rule/

如果有人問你數據庫的原理，叫他看這篇文章
http://blog.jobbole.com/100349/



 
### In the future
http://www.cnblogs.com/wyeat/p/mysql_innodb_logs1.html
http://blog.itpub.net/22664653/viewspace-762668/
http://dba.stackexchange.com/questions/8011/any-better-way-out-of-mysql-innodb-log-in-the-future
https://www.percona.com/blog/2013/09/11/how-to-move-the-innodb-log-sequence-number-lsn-forward/
InnoDB log sequence number NNNNNN is in the future!
LSN比較重要的是崩潰恢復階段以及BP的清理控制，正常來說LSN是一直遞增的，

1. 逐漸增加innodb_force_recovery參數值，一般最好不要超過4，指導MySQL可以正常啟動。
[mysqld]
innodb_force_recovery = 1
2. 將數據導出
mysqldump -u root -p --all-databases > all-databases.sql
3. 關閉服務器
4. 刪除掉出錯的數據文件
mv ib_logfile0 ib_logfile0.bak
mv ib_logfile1 ib_logfile1.bak
mv ibdata1 ibdata1.bak
5. 啟動mysql，然後從備份文件恢復數據
sudo service mysql start
mysql -u root -p < all-databases.sql

### MySQL 密碼
http://cenalulu.github.io/mysql/myall-about-mysql-password/
https://dev.mysql.com/doc/refman/5.7/en/start-slave.html


-->



{% highlight text %}
{% endhighlight %}
