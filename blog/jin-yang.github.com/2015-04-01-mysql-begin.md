---
title: MySQL 写在开头
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql
description: 保存一下经常使用的经典 MySQL 资源。
---

保存一下经常使用的经典 MySQL 资源。

<!-- more -->

## 常用网站

记录一些常用的网站。

### 官方网站

* [MySQL Documentation](http://dev.mysql.com/doc/)，MySQL 官方网站的参考文档，经常使用的包括了 [MySQL Reference Manual](http://dev.mysql.com/doc/refman/en/)、[MySQL Internals Manual](http://dev.mysql.com/doc/internals/en/)，还包括了一些 API 接口。

* [MySQL-InnoDB](https://blogs.oracle.com/mysqlinnodb/) InnoDB 的官方 blog ，很多不错的文章。

* [MySQL Server Blog](http://mysqlserverteam.com/)、[MySQL High Availability](http://mysqlhighavailability.com/) 。

* [Contributing Code to MySQL](https://community.oracle.com/docs/DOC-914911)，介绍如何向 MySQL 社区贡献代码。

* [Worklog Tasks](https://dev.mysql.com/worklog/)，也就是 MySQL 的任务列表，很多需求均可以在此查看。

### 数据库排名

关于当前数据库的排名比较有权威的是 [DB-Engines](http://db-engines.com/)，包括了关系型数据库、时序型数据库等等，其排名的数据计算是依据几个不同的因素，其中包括了：

* Google 以及 Bing 搜索引擎的关键字搜索数量；
* Google Trends 的搜索数量；
* Indeed 网站中的职位搜索量；
* LinkedIn 中提到关键字的个人资料数；
* Stackoverflow 上相关的问题和关注者数量。

其中各种类型数据库的综合排名可以参考 [DB-Engines Ranking](http://db-engines.com/en/ranking)，各个数据库的排名趋势则可以参考 [DB-Engines Ranking Trend](http://db-engines.com/en/ranking_trend) 。


## Blogs

一些比较常见、经典的 Blogs 。

### Jeremy Cole

目前在 Google 任职的大牛，分别在 MySQL AB、Yahoo、Twitter、Google 待过，看他的 blog 竟然还玩过 AVR 的 ^_^

可以参考其个人 blog：[Jeremy Cole](https://blog.jcole.us/) ，很大一部分是与 InnoDB 文件格式相关的内容，写的不错。

其维护了一个使用 Ruby 语言编写的 innodb_ruby 工具，用来查看 Innodb 的数据文件状态，当然只适用于实验测试环境。

innodb_ruby 工具的源码可以参考 [Github](https://github.com/jeremycole/innodb_ruby/)，其安装、使用方法可以参考 [Github WiKi](https://github.com/jeremycole/innodb_ruby/wiki) 。


## 经典文章

* [How does a relational database work](http://coding-geek.com/how-databases-work/)，一篇很好的文章，对关系型数据库的初步介绍。

* [An Outline for a Book on InnoDB](http://www.xaprb.com/blog/2015/08/08/innodb-book-outline/)，关于 InnoDB 的一些关键技术点。

* [Awesome MySQL](http://shlomi-noach.github.io/awesome-mysql/)，介绍很多与 MySQL 相关的软件、资源等。

## 经典课程

* [Stanford 数据库课程](http://web.stanford.edu/class/cs245/)，介绍了一些常用的数据库工具，如 [Innodb diagrams](https://github.com/jeremycole/innodb_diagrams)、[Innodb Ruby](https://github.com/jeremycole/innodb_ruby/) 。

<!--
<a href="http://www.innomysql.net/">Inside MySQL</a> 网易姜承尧的 blog 。<br><br>
<a href="https://github.com/percona/tokudb-engine">tokudb-engine github</a>，据说一个很牛掰的存储引擎，与 InnoDB 类似，percona 实现的。<br><br>
<a href="http://www.gpfeng.com/">Learn AND live</a> <br><br>
<a href="http://mysql.taobao.org/index.php?title=%E9%A6%96%E9%A1%B5">淘宝MySQL</a>，官方 blog 。<br><br>
http://cn.planet.mysql.com/
hotpu-meeting.b0.upaiyun.com/2014dtcc/post_pdf/hedengcheng.pdf




http://www.ywnds.com/?cat=31
http://mysqllover.com/?p=594
http://siddontang.com/?p=594
http://keithlan.github.io/      binlog
http://hatemysql.com/           binlog

备份
http://apprize.info/php/effective/index.html


http://www.cnblogs.com/cenliang/p/4908085.html MySQL高可用方案，包括相关资料


maatkit MySQL工具集

http://cybend.com/html/201602/a35.htm

https://www.percona.com/blog/2009/05/14/why-mysqls-binlog-do-db-option-is-dangerous/

http://keithlan.github.io/2015/11/02/mysql_replicate_rule/

如果有人问你数据库的原理，叫他看这篇文章
http://blog.jobbole.com/100349/



 
### In the future
http://www.cnblogs.com/wyeat/p/mysql_innodb_logs1.html
http://blog.itpub.net/22664653/viewspace-762668/
http://dba.stackexchange.com/questions/8011/any-better-way-out-of-mysql-innodb-log-in-the-future
https://www.percona.com/blog/2013/09/11/how-to-move-the-innodb-log-sequence-number-lsn-forward/
InnoDB log sequence number NNNNNN is in the future!
LSN比较重要的是崩溃恢复阶段以及BP的清理控制，正常来说LSN是一直递增的，

1. 逐渐增加innodb_force_recovery参数值，一般最好不要超过4，指导MySQL可以正常启动。
[mysqld]
innodb_force_recovery = 1
2. 将数据导出
mysqldump -u root -p --all-databases > all-databases.sql
3. 关闭服务器
4. 删除掉出错的数据文件
mv ib_logfile0 ib_logfile0.bak
mv ib_logfile1 ib_logfile1.bak
mv ibdata1 ibdata1.bak
5. 启动mysql，然后从备份文件恢复数据
sudo service mysql start
mysql -u root -p < all-databases.sql

### MySQL 密码
http://cenalulu.github.io/mysql/myall-about-mysql-password/
https://dev.mysql.com/doc/refman/5.7/en/start-slave.html


-->



{% highlight text %}
{% endhighlight %}
