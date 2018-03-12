---
title: Hello World !!!
layout: post
comments: true
language: chinese
usemath: true
category: [misc]
keywords: hello world,示例,sample,markdown
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---

Hi, the world, I'm coooooooooming.

Oooops, just examples, ignore me, darling.

<!-- more -->

![hello world logo]({{ site.url }}/images/misc/hello-world-logo.jpg "hello world logo"){: .pull-center }

## 配色

<style type="text/css">
<!--
#colorbox span{display:block;float:left;height:195px;}
-->
</style>

静思

<div id="colorbox">
    <span style="width:125.2px;background-color: #81C2D6" alt="81C2D6">HEX<br>#81C2D6<br>RGB<br>129.194.214<br>CMYK<br>10.9.0.16</span>
    <span style="width:125.2px;background-color: #8192D6" alt="8192D6">HEX<br>#8192D6<br>RGB<br>129.146.214<br>CMYK<br>40.32.0.16</span>
    <span style="width:125.2px;background-color: #D9B3E6" alt="D9B3E6">HEX<br>#D9B3E6<br>RGB<br>217.179.230<br>CMYK<br>6.22.0.10</span>
    <span style="width:125.2px;background-color: #DCF7A1" alt="DCF7A1">HEX<br>#DCF7A1<br>RGB<br>220.247.161<br>CMYK<br>11.0.35.3</span>
    <span style="width:125.2px;background-color: #83FCD8" alt="83FCD8">HEX<br>#83FCD8<br>RGB<br>131.152.216<br>CMYK<br>48.0.14.1</span>
</div>
<div class="clearfix"></div>

## Heading 2，目录 2

### Heading 3， 目录 3

#### Heading 4，目录 4

##### Heading 5，目录 5

###### Heading 6， 目录 6


## MISC

### Separator，分割线

下面的效果是相同的。

* * *

***

*****

- - -

---------------------------------------




### The Fonts， 字体设置

*This is emphasized 斜体*.   _This is also emphasized 还是斜体_.

**Strong font 粗体** __Also strong font 还是粗体__

Water is H<sub>2</sub>O. 4<sup>2</sup>=16. 上标、下标测试。

Code Use the `printf()` function，代码模块。

Code Use the <code>printf()</code> function，与上面功能一样。

``There is a literal backtick (`) here.``，额，还是代码模块。

The New York Times <cite>(That’s a citation)</cite>. 引用测试，和斜体相似。

This is <u>Underline</u>. 下划线。


### Code Snippets，代码高亮显示

Syntax highlighting via Pygments. css java sh c gas asm cpp c++

{% highlight css linenos %}
#container {
  float: left;
  margin: 0 -240px 0 0;
  width: 100%;
}
#container {
  float: left;
  margin: 0 -240px 0 0;
  width: 100%;
}
{% endhighlight %}


{% highlight c %}
int main ()
{
   return 0;
}
{% endhighlight %}

Non Pygments code example

    <div id="awesome">
        <p>This is great isn't it?</p>
    </div>

















### Block Quote， 段引用

下面时关于段引用的测试。

单段的引用。Just one paragraph.

> My conclusion after the trip was "well, now I know that there's at least one company in the world that can succeed with the waterfall model" and I decided to stop bashing the waterfall model as hard as I usually do. Now, however, with all the problems Toyota are having, I'm starting to reconsider.q q q q q q q q q q q q q q q<from>kkkkk</from>

多段的引用，one more paragraphs.


> My conclusion after the trip was "well, now I know that there's at least one company in the world that can succeed with the waterfall model" and I decided to stop bashing the waterfall model as hard as I usually do. Now, however, with all the problems Toyota are having, I'm starting to reconsider.
>
> My conclusion after the trip was "well, now I know that there's at least one company in the world that can succeed with the waterfall model" and I decided to stop bashing the waterfall model as hard as I usually do. Now, however, with all the problems Toyota are having, I'm starting to .q q q q q

单段，但较为复杂的引用。

> My conclusion after the trip was "well,
> now I know that there's at least one company in the world
> that can succeed with the waterfall model" and I decided to
> stop bashing the waterfall model as hard as I usually do.
> Now, however, with all the problems Toyota are having, I'm starting to reconsider.

嵌套引用。

> My conclusion after the trip was "well,
> now I know that there's at least one company in the world
> > that can succeed with the waterfall model" and I decided to
> stop bashing the waterfall model as hard as I usually do.
> Now, how ever, with all the problems Toyota are having, I'm starting to re consider.







### Unordered Lists，无序列表

如下是三种不同的表达方式。

#### Unordered Lists 1

* Item one
* Item two
* Item three

#### Unordered Lists 2

+ Item one
+ Item two
+ Item three

#### Unordered Lists 3

- Item one
- Item two
- Item three

#### 其它

如下的各个列表项中，各个项之间表示为段落，而之前的不是，也就是说添加了 ```<p></p>``` ，所以现在看起来各个段之间空隙有点大。

- Item one

- Item two

- Item three

### Ordered Lists，有序列表

有序表的表达方式，只与顺序相关，而与列表前的数字无关。

#### Ordered Lists 1

1. Item one
   1. sub item one
   2. sub item two
   3. sub item three
2. Item two

#### Ordered Lists 2

1. Item one
1. Item two
1. Item three

#### Ordered Lists 3

3. Item one
9. Item two
5. Item three

### Lists Tips，列表补记

列表项目标记通常是放在最左边，但是其实也可以缩进，最多 3 个空格，项目标记后面则一定要接着至少一个空格或制表符。

*   Lorem ipsum dolor sit amet, consectetuer adipiscing elit.
    Aliquam hendrerit mi posuere lectus. Vestibulum enim wisi,
    viverra nec, fringilla in, laoreet vitae, risus.
*   Donec sit amet nisl. Aliquam semper ipsum sit amet velit.
    Suspendisse id sem consectetuer libero luctus adipiscing.

如下显示相同。

*   Lorem ipsum dolor sit amet, consectetuer adipiscing elit.
Aliquam hendrerit mi posuere lectus. Vestibulum enim wisi,
viverra nec, fringilla in, laoreet vitae, risus.
*   Donec sit amet nisl. Aliquam semper ipsum sit amet velit.
Suspendisse id sem consectetuer libero luctus adipiscing.

如下是在同一列表中，显示两个段落。

1.  This is a list item with two paragraphs. Lorem ipsum dolor
    sit amet, consectetuer adipiscing elit. Aliquam hendrerit
    mi posuere lectus.

    Vestibulum enim wisi, viverra nec, fringilla in, laoreet
    vitae, risus. Donec sit amet nisl. Aliquam semper ipsum
    sit amet velit.

2.  Suspendisse id sem consectetuer libero luctus adipiscing.

    This is the second paragraph in the list item. You're
only required to indent the first line. Lorem ipsum dolor
sit amet, consectetuer adipiscing elit.

### Tables, 表格

kramdown 默认支持表格，只需要设置好 ```table thead tbody th tr td``` 对应的属性即可。

|head1 head1 head1|head2 head2 head2|head3 head3 head3|head4 head4 head4|
|---|:---|:---:|---:|
|row1text1|row1text3|row1text3|row1text4|
|row2text1|row2text3|row2text3|row2text4|

<!--
dog | bird | cat
----|------|----
foo | foo  | foo
bar | bar  | bar
baz | baz  | baz
-->

### MathJax, 数学表达式

如下是一个数学表达式。

$$
\begin{align*}
  & \phi(x,y) = \phi \left(\sum_{i=1}^n x_ie_i, \sum_{j=1}^n y_je_j \right)
  = \sum_{i=1}^n \sum_{j=1}^n x_i y_j \phi(e_i, e_j) = \\
  & (x_1, \ldots, x_n) \left( \begin{array}{ccc}
      \phi(e_1, e_1) & \cdots & \phi(e_1, e_n) \\
      \vdots & \ddots & \vdots \\
      \phi(e_n, e_1) & \cdots & \phi(e_n, e_n)
    \end{array} \right)
  \left( \begin{array}{c}
      y_1 \\
      \vdots \\
      y_n
    \end{array} \right)
\end{align*}
$$

段内插入LaTeX代码是这样的：$\exp(-\frac{x^2}{2})$，试试看看吧

### Pictures，图片显示

![If the picture doesnt exist]({{ site.url }}/images/linux-liberty.png "an example picture"){: .pull-left}

<div class="clearfix"></div>

![aaaaa]{: .pull-right width="20%"}

<div class="clearfix"></div>

[aaaaa]:    /images/linux-liberty.png    "MSN Search"


### Reference，引用

如下是一个简单的链接 [an example](http://example.com/ "Title")，当然也可以使用网站的相对路径 [About Me](/about/) 。

也可以将网站的引用与 URL 分别区分开，如下是其中的示例，而且不区分大小写，[RTEMS] [1]、[Linux] [2]、[GUN][GUN]、[GUN][gun] 。

<!-- the following can occur on anywhere -->
[1]: http://www.rtems.com                              "A Real Time kernel"
[2]: http://www.Linux.com                              'Linux'
[Gun]: http://www.gun.com                              (GUN)


<!--

<figure class="highlight"><pre><code class="language-css" data-lang="css"><table style="border-spacing: 0"><tbody><tr><td class="gutter gl" style="text-align: right"><pre class="lineno">1
2
3
4
5
6
7
8
9
10</pre></td><td class="code"><pre><span class="nf">#container</span> <span class="p">{</span>
  <span class="nl">float</span><span class="p">:</span> <span class="nb">left</span><span class="p">;</span>
  <span class="nl">margin</span><span class="p">:</span> <span class="m">0</span> <span class="m">-240px</span> <span class="m">0</span> <span class="m">0</span><span class="p">;</span>
  <span class="nl">width</span><span class="p">:</span> <span class="m">100%</span><span class="p">;</span>
<span class="p">}</span>
<span class="nf">#container</span> <span class="p">{</span>
  <span class="nl">float</span><span class="p">:</span> <span class="nb">left</span><span class="p">;</span>
  <span class="nl">margin</span><span class="p">:</span> <span class="m">0</span> <span class="m">-240px</span> <span class="m">0</span> <span class="m">0</span><span class="p">;</span>
  <span class="nl">width</span><span class="p">:</span> <span class="m">100%</span><span class="p">;</span>
<span class="p">}</span><span class="w">
</span></pre></td></tr></tbody></table></code></pre></figure>


①
②
③
④
⑤
⑥
⑦
⑧
⑨
⑩
⑪
⑫
⑬
⑭
⑮
⑯
⑰
⑱
⑲
⑳

❶
❷
❸
❹
❺
❻
❼
❽
❾
❿
⓿
➊
➋
➌
➍
➎
➏
➐
➑
➒
➓


用pt-table-checksum校验数据一致性
http://www.nettedfish.com/blog/2013/06/04/check-replication-consistency-by-pt-table-checksum/

Choosing MySQL High Availability Solutions

Database Daily Ops Series: GTID Replication
https://www.percona.com/blog/2016/11/10/database-daily-ops-series-gtid-replication/

http://hatemysql.com/?p=526
http://mysqllover.com/?p=594
http://in355hz.iteye.com/blog/1770401

binlog文件格式
http://www.jianshu.com/p/c16686b35807
http://slideplayer.com/slide/6146721/
http://miguelangelnieto.net/index.php/how-to-createrestore-a-slave-using-gtid-replication-in-mysql-5-6/

???支持基于所有数据库、指定数据库、指定表的复制。

默认使用基于GTID的异步复制，同时支持半同步复制 (semi-synchronous)，基于 NDB 的同步复制以及延迟复制 (Delayed Replication)




### NC
socat加强版
http://brieflyx.me/2015/linux-tools/socat-introduction/


### GCC
http://www.jianshu.com/p/dd425b9dc9db
http://filwmm1314.blog.163.com/blog/static/218259192012121225132/
http://www.cnblogs.com/respawn/archive/2012/07/09/2582078.html

### TSD

在多线程程序中，所有线程共享程序中的变量，如果每个线程需要保存独自的数据，例如每个线程维护一个链表，但是通过相同的函数处理，这就是 Thread Specific Data 的作用。如下介绍 TSD 的使用方法：

1. 声明一个 pthread_key_t 类型的全局变量；
2. 通过 pthread_key_create() 函数创建 TSD，实际就是分配一个实例，并将其赋值给 pthread_key_t 变量，所有的线程都可以通过该变量访问，这就相当于提供了同名而不同值的全局变量；
3. 调用  pthread_setspcific()、pthread_getspecific() 存储或者获取各个线程特有的值；

TSD的实现详见： https://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part2/

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);



### innodb_locks_unsafe_for_binlog 参数相关
另外设置 innodb_locks_unsafe_for_binlog=1 ,binlog也要设为row格式。
https://dev.mysql.com/doc/refman/5.7/en/innodb-parameters.html#sysvar_innodb_locks_unsafe_for_binlog
innodb默认使用了next-gap算法，这种算法结合了index-row锁和gap锁。正因为这样的锁算法，innodb在可重复读这样的默认隔离级别上，可以避免幻象的产生。innodb_locks_unsafe_for_binlog最主要的作用就是控制innodb是否对gap加锁。
注意该参数如果是enable的，则是unsafe的，此时gap不会加锁；反之，如果disable掉该参数，则gap会加锁。当然对于一些和数据完整性相关的定义，如外键和唯一索引（含主键）需要对gap进行加锁，那么innodb_locks_unsafe_for_binlog的设置并不会影响gap是否加锁。
在5.1.15的时候，innodb引入了一个概念叫做“semi-consistent”，这样会在innodb_locks_unsafe_for_binlog的状态为ennable时在一定程度上提高update并发性。
https://yq.aliyun.com/articles/62372



### FLUSH TABLES WITH READ LOCK
FLUSH TABLES WITH READ LOCK命令的作用是关闭所有打开的表，会将所有脏页刷新到磁盘，同时对所有数据库中的表都加一个全局读锁，直到显示执行UNLOCK TABLES，才释放持有的读锁。

该操作常用于获取一致性数据备份。

http://aklaus.blog.51cto.com/9724632/1656991
http://blog.csdn.net/zbszhangbosen/article/details/7434181
http://www.cnblogs.com/cchust/p/4603599.html
http://www.cnblogs.com/sunss/archive/2012/02/02/2335960.html
http://www.cnblogs.com/zhenghongxin/p/5527527.html
http://myrock.github.io/2014/11/20/mysql-waiting-for-table-flush/
http://blog.csdn.net/arkblue/article/details/27376991
https://www.percona.com/blog/2012/03/23/how-flush-tables-with-read-lock-works-with-innodb-tables/
https://www.percona.com/blog/2010/04/24/how-fast-is-flush-tables-with-read-lock/

加锁过程是什么样的，为什么会阻塞现有的SELECT查询？

http://www.penglixun.com/tech/database/the_process_of_mysqldump.html
http://www.cnblogs.com/digdeep/p/4947694.html
http://www.imysql.com/2008_10_24_deep_into_mysqldump_options
http://www.cnblogs.com/zhoujinyi/p/5789465.html
https://yq.aliyun.com/articles/59326


#### sql_slave_skip_counter=N
http://dinglin.iteye.com/blog/1236330
http://xstarcd.github.io/wiki/MySQL/MySQL_replicate_error.html

#### FOR UPDATE/
SELECT ... FOR UPDATE 用于对查询的数据添加IX锁(意向排它锁)，此时，其它会话也就无法在这些记录上添加任何的S锁或X锁，通常使用的场景是为了防止在低隔离级别下出现幻读现象，用于保证 “本事务看到的是数据库存储的最新数据，并且看到的数据只能由本事务修改”。

InnoDB默认使用快照读，使用 FOR UPDATE 之后不会阻塞其它会话的快照读，当然会阻塞lock in share mode和for update这种显示加锁的查询操作。

CREATE TABLE `foobar` (
  `id` int(11) NOT NULL,
  `col` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
INSERT INTO foobar VALUES(1, 11),(2, 22);
set session transaction isolation level REPEATABLE READ;
set autocommit=0;
select * from foobar where col = 11; **FOR UPDATE**

set session transaction isolation level REPEATABLE READ;
set autocommit=0;
select * from foobar where col = 11;
update foobar set col = 33 where col=11;

select * from foobar where col = 11;
update foobar set col = col*2 where col=11;


SELECT ... LOCK IN SHARE MODE对于查询的数据添加IS(意向共享锁)，此时，其它会话可以读取这些记录，也可以继续添加IS锁，但是无法修改这些记录直到该事务执行完成。

通常用于两张存在关联关系表的写操作，拿mysql官方文档的例子来说，一个表是child表，一个是parent表，假设child表的某一列child_id映射到parent表的c_child_id列，那么从业务角度讲，此时我直接insert一条child_id=100记录到child表是存在风险的，因为刚insert的时候可能在parent表里删除了这条c_child_id=100的记录，那么业务数据就存在不一致的风险。正确的方法是再插入时执行select * from parent where c_child_id=100 lock in share mode,锁定了parent表的这条记录，然后执行insert into child(child_id) values (100)就ok了。

http://blog.csdn.net/cug_jiang126com/article/details/50544728
http://chenzhou123520.iteye.com/blog/1860954
https://www.percona.com/blog/2006/08/06/select-lock-in-share-mode-and-for-update/


#### 复制过滤可能产生的异常

http://keithlan.github.io/2015/11/02/mysql_replicate_rule/

#### BLOG和TEXT区别
BLOB和TEXT分别用于保存二进制和非二进制字符串，保存数据可变，都可以分为四类：TINYBLOB、BLOB、MEDIUMBLOB 和 LONGBLOB ，TEXT与之类似，只是其容纳长度有所不同；两者区别如下：

* BLOB为二进制字符串，TEXT为非二进制字符串；
* BLOG列没有字符集，并且排序和比较基于列值字节的数值；TEXT列有字符集，并且根据字符集的规则进行排序和比较；
* 两种类型的DML不存在大小写转换，在非严格模式下超出最大长度时会截断并产生告警；严格模式则会报错；

两者可以分别视为VARCHAR和VARBINARY，但是有如下区别：
* BLOB和TEXT列不能有默认值；

当保存或检索BLOB和TEXT列的值时不删除尾部空格。(这与VARBINARY和VARCHAR列相同）.

对于BLOB和TEXT列的索引，必须指定索引前缀的长度。对于CHAR和VARCHAR，前缀长度是可选的.

LONG和LONG VARCHAR对应MEDIUMTEXT数据类型。这是为了保证兼容性。如果TEXT列类型使用BINARY属性，将为列分配列字符集的二元校对规则.

MySQL连接程序/ODBC将BLOB值定义为LONGVARBINARY，将MySQL TEXT值定义为LONGVARCHAR。由于BLOB和TEXT值可能会非常长，使用它们时可能遇到一些约束.

BLOB或TEXT对象的最大大小由其类型确定，但在客户端和服务器之间实际可以传递的最大值由可用内存数量和通信缓存区大小确定。你可以通过更改max_allowed_packet变量的值更改消息缓存区的大小，但必须同时修改服务器和客户端程序。例如，可以使用 MySQL和MySQLdump来更改客户端的max_allowed_packet值.


create table foobar (
 
) engine=innodb;

mysql>CREATE TABLE foobar (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  book CHAR(10) DEFAULT NULL,
  msg BLOB,
  ) ENGINE=InnoDB;
http://blog.sina.com.cn/s/blog_4f925fc30102edg8.html
http://imysql.com/2014/09/28/mysql-optimization-case-blob-stored-in-innodb-optimization.shtml
http://www.qttc.net/201207121.html
http://53873039oycg.iteye.com/blog/2002448



#### 高可用
循环复制
http://xiezhenye.com/2015/08/%E8%AE%B0%E4%B8%80%E6%AC%A1-mysql-%E5%BE%AA%E7%8E%AF%E5%A4%8D%E5%88%B6.html
http://www.cnblogs.com/cchust/p/5450510.html
http://imysql.com/2015/09/14/solutions-of-mysql-ha.shtml
http://blog.csdn.net/yangbutao/article/details/8807813
http://www.jianshu.com/p/cc6746ac4fc2
http://raffaelexr.blog.51cto.com/8555551/1747784
http://wenson.iteye.com/blog/2263956

#### VIP
http://www.cnblogs.com/pangguoping/p/5721517.html


##### MySQL Cache

sql_cache意思是说，查询的时候使用缓存。

sql_no_cache意思是查询的时候不适用缓存。

sql_buffer_result意思是说，在查询语句中，将查询结果缓存到临时表中。

这三者正好配套使用。sql_buffer_result将尽快释放表锁，这样其他sql就能够尽快执行。

使用 FLUSH QUERY CACHE 命令，你可以整理查询缓存，以更好的利用它的内存。这个命令不会从缓存中移除任何查询。FLUSH TABLES 会转储清除查询缓存。
RESET QUERY CACHE 使命从查询缓存中移除所有的查询结果。

--------------------------------------------------

那么MySQL到底是怎么决定到底要不要把查询结果放到查询缓存中呢？

是根据query_cache_type这个变量来决定的。

这个变量有三个取值：0,1,2，分别代表了off、on、demand。

意思是说，如果是0，那么query cache 是关闭的。如果是1，那么查询总是先到查询缓存中查找，除非使用了sql_no_cache。如果是2，那么，只有使用 sql_cache的查询，才会去查询缓存中查找。


###
ysql [(none)]>grant super on db1.* to 'dtstack'@'%';
ERROR 1221 (HY000): Incorrect usage of DB GRANT and GLOBAL PRIVILEGES        --因为super权限是管理级别的权限(super ,process,file)，不能够指定某个数据库on 后面必须跟*.*

正确用法：
mysql [(none)]>grant super on *.* to 'dtstack'@'%';
Query OK, 0 rows affected (0.01 sec)
释：
1.super权限可以使用change master to 语句
2.kill ， mysqladmin kill  kill_threads
3.purge binary logs
    EG：
        wjj@(www.dtstack.com) [(none)]>show binary logs;
          mysql [(none)]>purge binary logs to 'mysql-bin.000005';  --解释：删除MySQLbinlog日志，删除mysql-bin.000005之前的，不包括他本身
          Query OK, 0 rows affected (0.03 sec
          mysql [(none)]>show binary logs;
       
4.可以设置全局参数模式set global xx
5.mysqladmin debug  启动或禁用日志记录
6.有权限设置relad_only
7.连接数达到max_connections上限时无法创建连接时，拥有super权限的用户可以创建一个连接

网络设置，包括了如何设置服务端+客户端、广播、多播等。
https://collectd.org/wiki/index.php/Networking_introduction
写入RRD文件，包括了Collectd对RRD的优化，以及其中一篇RRD优化介绍的文章
http://oss.oetiker.ch/rrdtool-trac/wiki/TuningRRD
https://collectd.org/wiki/index.php/Inside_the_RRDtool_plugin
惊群问题讨论
http://www.voidcn.com/blog/liujiyong7/article/p-377809.html
linux时间相关结构体和函数整理
http://www.cnblogs.com/zhiranok/archive/2012/01/15/linux_c_time_struct.html
Heap数据结构(栈)
http://www.cnblogs.com/gaochundong/p/binary_heap.html
http://www.cnblogs.com/skywang12345/p/3610187.html


AVL数
https://courses.cs.washington.edu/courses/cse373/06sp/handouts/lecture12.pdf
https://www.cise.ufl.edu/~nemo/cop3530/AVL-Tree-Rotations.pdf
http://www.cnblogs.com/zhoujinyi/p/6497231.html

https://dev.mysql.com/doc/refman/5.7/en/backup-policy.html
https://dev.mysql.com/doc/refman/5.7/en/point-in-time-recovery.html

https://www.unixhot.com/page/ops




对于GROUP BY、ORDER BY、DISTINCT类似的SQL，可能会用到临时表，通常在内存中使用MEMORY存储引擎，或者磁盘上使用MyISAM存储引擎。一般，当超过了tmp_table_size大小之后，会在磁盘上创建，另外，对于
https://www.percona.com/blog/2007/08/16/how-much-overhead-is-caused-by-on-disk-temporary-tables/
https://www.percona.com/blog/2007/08/18/how-fast-can-you-sort-data-with-mysql/





http://halobates.de/memorywaste.pdf
BuildBot


非常经典的《Linux平台下的漏洞分析入门 》
https://github.com/1u4nx/Exploit-Exercises-Nebula
原文在这里
https://www.mattandreko.com/

http://hustcat.github.io/iostats/
http://ykrocku.github.io/blog/2014/04/11/diskstats/
http://www.udpwork.com/item/12931.html

FIXME:
  linux-monitor-io.html
/proc/diskstats 中包括了主设备号、次设备号和设备名称，剩余的各个字段的含义简单列举如下，详细可以查看内核文档 [I/O statistics fields](https://www.kernel.org/doc/Documentation/iostats.txt) 。

可以通过 grep diskstats 找到对应内核源码实现在 diskstats_show()@block/genhd.c 中。

获取源码 diskstats_show() + struct disk_stats 。

可以看到是通过 part_round_stats() 函数获取每个磁盘的最新统计信息，通过 struct hd_struct 中的 struct disk_stats *dkstats 结构体保存，然后利用 part_stat_read() 函数统计各个 CPU 的值 (如果是多核)。


在 Bash 编程时，经常需要切换目录，可以通过 pushd、popd、dirs 命令切换目录。

pushd  切换到参数指定的目录，并把原目录和当前目录压入到一个虚拟的堆栈中，不加参数则在最近两个目录间切换；
popd   弹出堆栈中最近的目录；
dirs   列出当前堆栈中保存的目录列表；
  -v 在目录前添加编号，每行显示一个目录；
  -c 清空栈；

切换目录时，会将上次目录保存在 $OLDPWD 变量中，与 "-" 相同，可以通过 cd - 切换回上次的目录。



set -o history 开启bash的历史功能

判断目录是否存在，如果目录名中有'-'则需要进行转义。

dir="/tmp/0\-test"
if [ ! -d "${dir}" ]; then
  mkdir /myfolder
fi


c_avl_tree_t *c_avl_create(int (*compare)(const void *, const void *));
入参：
  比较函数，类似strcmp()；
实现：
  1. 保证 compare 有效，非 NULL；
  2. 申请内存，部分结构体初始化。
返回：
  成功返回结构体指针；参数异常或者没有内存，返回 NULL；

int c_avl_insert(c_avl_tree_t *t, void *key, void *value);
返回：
  -1：内存不足；
  0： 节点写入正常；
  1:  节点已经存在；

int c_avl_get(c_avl_tree_t *t, const void *key, void **value);
调用者保证 t 存在 (非NULL)。
返回：
  -1：对象不存在；
  0： 查找成功，对应的值保存在value中；

int c_avl_remove(c_avl_tree_t *t, const void *key, void **rkey, void **rvalue);
返回：
  -1：对象不存在；


_remove()
search()
rebalance()
verify_tree()


安全渗透所需的工具
https://wizardforcel.gitbooks.io/daxueba-kali-linux-tutorial/content/2.html






1/0 = &infin;
log (0) = -&infin;
sqrt (-1) = NaN

infinity (无穷)、NaN (非数值)

除了 infin 自身和 NaN 之外，infin (正无穷) 大于任何值；而 NaN 不等于任何值，包括其自身，而且与 <, >, <=, >= 使用时将会报错。







如果有 group 采用相同的 group-id，也就是有 alias group ，那么在删除某个

groupdel: cannot remove the primary group of user 'monitor'

----- 将原用户修改为其它分组
# usermod -g sys monitor
----- 删除分组
# groupdel test
----- 修改回来
# usermod -g monitor monitor












http://fengyuzaitu.blog.51cto.com/5218690/1616268
http://www.runoob.com/python/os-statvfs.html
http://blog.csdn.net/papiping/article/details/6980573
http://blog.csdn.net/hepeng597/article/details/8925506






jinyang ALL=(root) NOPASSWD: ALL






shell版本号比较
http://blog.topspeedsnail.com/archives/3999
https://www.netfilter.org/documentation/HOWTO/NAT-HOWTO-6.html
man 3 yum.conf 确认下YUM配置文件中的变量信息
https://unix.stackexchange.com/questions/19701/yum-how-can-i-view-variables-like-releasever-basearch-yum0


FORTIFY.Key_Management--Hardcoded_Encryption_Key    strcasecmp("Interval", child->key)

int lt_dlinit (void);
  初始化，在使用前调用，可以多次调用，正常返回 0 ；
const char * lt_dlerror (void);
  返回最近一次可读的错误原因，如果没有错误返回 NULL；
void * lt_dlsym (lt_dlhandle handle, const char *name);
  返回指向 name 模块的指针，如果没有找到则返回 NULL 。
lt_dlhandle lt_dlopen (const char *filename);
  加载失败返回 NULL，多次加载会返回相同的值；
int lt_dlclose (lt_dlhandle handle);
  模块的应用次数减一，当减到 0 时会自动卸载；成功返回 0 。

https://github.com/carpedm20/awesome-hacking
http://jamyy.us.to/blog/2014/01/5800.html




Page Cache
https://www.thomas-krenn.com/en/wiki/Linux_Page_Cache_Basics

Page Cache, the Affair Between Memory and Files

http://duartes.org/gustavo/blog/post/page-cache-the-affair-between-memory-and-files/

https://www.quora.com/What-is-the-major-difference-between-the-buffer-cache-and-the-page-cache

从free到page cache
http://www.cnblogs.com/hustcat/archive/2011/10/27/2226995.html



getaddrinfo()
http://www.cnblogs.com/cxz2009/archive/2010/11/19/1881693.html


#define unlikely(x) __builtin_expect((x),0)
http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html


libcurl使用
http://www.cnblogs.com/moodlxs/archive/2012/10/15/2724318.html




https://lwn.net/Articles/584225/
https://en.wikipedia.org/wiki/Stack_buffer_overflow#Stack_canaries
https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
https://outflux.net/blog/archives/2014/01/27/fstack-protector-strong/
-pipe
  从源码生成可执行文件一般需要四个步骤，并且还会产生中间文件，该参数用于配置实用PIPE，一些平台会失败，不过 GNU 不受影响。
-fexceptions
  打开异常处理，该选项会生成必要的代码来处理异常的抛出和捕获，对于 C++ 等会触发异常的语言来说，默认都会指定该选项。所生成的代码不会造成性能损失，但会造成尺寸上的损失。因此，如果想要编译不使用异常的 C++ 代码，可能需要指定选项 -fno-exceptions 。
-Wall -Werror -O2 -g --param=ssp-buffer-size=4 -grecord-gcc-switches -specs=/usr/lib/rpm/redhat/redhat-hardened-cc1  -m64 -mtune=generic -DLT_LAZY_OR_NOW="RTLD_LAZY|RTLD_GLOBAL"




关于Linux内核很不错的介绍
http://duartes.org/gustavo/blog/






编程时常有 Client 和 Server 需要各自得到对方 IP 和 Port 的需求，此时就可以通过 getsockname() 和 getpeername() 获取。

python -c '
import sys
import socket
s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("www.huawei.com", 80))
print s.getsockname()
s.close()'


Python判断IP有效性
https://gist.github.com/youngsterxyf/5088954



安全渗透工具集
https://wizardforcel.gitbooks.io/daxueba-kali-linux-tutorial/content/2.html

hostname获取方式，在启动时通过 1) global_option_get() 配置文件获取；2) gethostname()；3) getaddrinfo()。

#include <unistd.h>
int gethostname(char *name, size_t len);
  返回本地主机的标准主机名；正常返回 0 否则返回 -1，错误码保存在 errno 中。

#include <netdb.h>
#include <sys/socket.h>
struct hostent *gethostbyname(const char *name);
  用域名或主机名获取IP地址，注意只支持IPv4；正常返回一个 struct hostent 结构，否则返回 NULL。

#include<netdb.h>
int getaddrinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result);
  hostname: 一个主机名或者地址串，IPv4的点分十进制串或者IPv6的16进制串；
  service : 服务名可以是十进制的端口号，也可以是已定义的服务名称，如ftp、http等；
  hints   : 可以为空，用于指定返回的类型信息，例如，服务支持 TCP/UDP 那么，可以设置 ai_socktype 为 SOCK_DGRAM 只返回 UDP 信息；
  result  : 返回的结果。
  返回 0 成功。

struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;        // IP地址，需要通过inet_ntop()转换为IP字符串
    char            *ai_canonname;   // 返回的主机名
    struct addrinfo *ai_next;
};
http://blog.csdn.net/a_ran/article/details/41871437


const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
  将类型为af的网络地址结构src，转换成主机序的字符串形式，存放在长度为cnt的字符串中。返回指向dst的一个指针。如果函数调用错误，返回值是NULL。



对于不信任的组件建议使用后者，因为 ldd 可能会加载后显示依赖的库，从而导致安全问题。

----- 查看依赖的库
$ ldd /usr/bin/ssh
$ objdump -p /usr/bin/ssh | grep NEEDED

----- 运行程序加载的库
# pldd $(pidof mysqld)


# pldd $(pidof uagent)


VIRT, Virtual Memory Size @
  该任务的总的虚拟内存，包括了 code、data、shared libraries、换出到磁盘的页、已经映射但是没有使用的页。
USED, Memory in Use @
  包括了已使用的物理内存 RES ，以及换出到磁盘的内存 SWAP。
%MEM, Memory Usage(RES) @
  当前任务使用的内存与整个物理内存的占比。
CODE, Code Size
  可执行代码占用的物理内存数，也被称为 Text Resident Set, TRS。
DATA, Data+Stack Size
  除了代码之外的物理内存占用数，也就是 Data Resident Set, DRS 。
RES, Resident Memory Size @
  驻留在物理内存中的使用量。
SHR, Shared Memory Size @
  包括了共享内存以及共享库的数据。

SWAP, Swapped Size
  换出到磁盘的内存。
nMaj, nMin, nDRT


RES = CODE + DATA???? DATA太大了，为什么

====== ps
DRS, Data Resident Set <=> top(DATA) !!!
  除了代码之外的物理内存占用数。
RSS, Resident Set Size <=> top(RES)
  物理内存使用数。
TRS, Text Resident Set <=> top(CODE) !!!
  代码在内存中的占用数。
VSZ, Virtual Memory Size <=> top(VIRT) <=> pmap -d(mapped)
  虚拟内存的大小。

RES(top) 和 RSS(ps) 实际上读取的是 /proc/$(pidof process)/stat 或者 /proc/$(pidof process)/status statm。
pmap -d $(pidof uagent)
pmap -x $(pidof uagent)
ps -o pid,pmem,drs,trs,rss,vsz Hp `pidof uagent`

另外，cgtop 中显示的内存与什么相关？？？？
ps(TRS) 和 top(CODE) 的值不相同。

http://blog.csdn.net/u011547375/article/details/9851455
https://stackoverflow.com/questions/7594548/res-code-data-in-the-output-information-of-the-top-command-why
https://static.googleusercontent.com/media/research.google.com/zh-CN//pubs/archive/36669.pdf
https://landley.net/kdocs/ols/2010/ols2010-pages-245-254.pdf
LDD 跟 PMAP 加载的库不同？

awk 'BEGIN{sum=0};{if($3~/x/) {sum+=$2}};END{print sum}' /tmp/1

procs_refresh

Top用于查看Linux系统下进程信息，有时候需要选择显示那些列，以及按照某一列进行排序。查询整理如下：


top 除了默认的列之外，可以选择需要显示的列，操作如下：

----- 选择需要显示的列
1) 按 f 键进入选择界面；2) 方向键选择需要的列；3) 通过空格选择需要显示的列。

列显示位置调整：
执行top命令后，按 o 键，选择要调整位置的列（如K:CUP Usageage），按动一下大写K则显示位置往上调整，按动一下小写K则显示位置往下调整。

列排序：
执行top命令后，按 shift + f（小写），进入选择排序列页面，再按要排序的列的代表字母即可；

systemctl set-property --runtime uagent.service CPUQuota=5% MemoryLimit=30M

关于资源配置的选项可以通过 ```man 5 systemd.resource-control``` 方式查看，默认是没有开启审计的，所以通过 ```systemd-cgtop``` 没有显示具体的资源。

很多相关的内核文档链接
https://www.freedesktop.org/software/systemd/man/systemd.resource-control.html

注册信号处理函数

setsid()
pidfile_create()

https://www.ibm.com/support/knowledgecenter/zh/ssw_aix_61/com.ibm.aix.genprogc/ie_prog_4lex_yacc.htm

flex 通过 yylval 将数据传递给 yacc；如果在 yacc 中使用了 ```%union``` ，那么各个条件的目的变量使用 yyval 。

https://www.howtoforge.com/storing-files-directories-in-memory-with-tmpfs



meminfo详解
https://lwn.net/Articles/28345/

ps的SIZE以及RSS不含部分的内存统计，所以要比pmap -d统计的RSS小。
The SIZE and RSS fields don't count some parts of a process including the page tables, kernel stack, struct thread_info
https://techtalk.intersec.com/2013/07/memory-part-2-understanding-process-memory/
http://tldp.org/LDP/tlk/mm/memory.html
http://tldp.org/LDP/khg/HyperNews/get/memory/linuxmm.html
https://lwn.net/Articles/230975/
https://gist.github.com/CMCDragonkai/10ab53654b2aa6ce55c11cfc5b2432a4
https://yq.aliyun.com/ziliao/75375
http://elinux.org/Runtime_Memory_Measurement
https://access.redhat.com/security/vulnerabilities/stackguard
http://events.linuxfoundation.org/sites/events/files/slides/elc_2016_mem_0.pdf
http://blog.csdn.net/lijzheng/article/details/23618365
https://yq.aliyun.com/articles/54405
https://stackoverflow.com/questions/31328349/stack-memory-management-in-linux
/post/mysql-parser.html
yyset_in() 设置入口


pmap -d $(pidof uagent)
pmap -x $(pidof uagent)

top -Hp $(pidof uagent)
ps -o pid,pmem,drs,trs,rss,vsz Hp `pidof uagent`
/proc/$(pidof uagent)/stat
/proc/$(pidof uagent)/status
/proc/$(pidof uagent)/maps
VmRSS、VmSize

$ ps aux|grep /usr/bin/X|grep -v grep | awk '{print $2}'   # 得出X server 的 pid   ...
1076
$ cat /proc/1076/stat | awk '{print $23 / 1024}'
139012
$ cat /proc/1076/status | grep -i vmsize
VmSize:      106516 kB
VmSize = memory + memory-mapped hardware (e.g. video card memory).

kmap 是用来建立映射的，映射后返回了被映射的高端内存在内核的线性地址
https://www.zhihu.com/question/30338816
http://blog.csdn.net/gatieme/article/details/52705142
http://www.cnblogs.com/zhiliao112/p/4251221.html
http://way4ever.com/?p=236
awk统计Linux最常用命令
http://www.ha97.com/3980.html
awk使用技巧
http://blog.csdn.net/ultrani/article/details/6750434
http://blog.csdn.net/u011204847/article/details/51205031 *****
http://ustb80.blog.51cto.com/6139482/1051310


关于smaps的详细介绍
https://jameshunt.us/writings/smaps.html
$ cat /proc/self/smaps  相比maps显示更详细信息
$ cat /proc/self/maps
address                  perms   offset   dev   inode       pathname
7f571af7a000-7f571af7d000 ---p 00000000 00:00 0
7f571af7d000-7f571b080000 rw-p 00000000 00:00 0             [stack:4714]
7f571b0ac000-7f571b0ad000 r--p 00021000 08:01 1838227       /usr/lib/ld-2.21.so
7ffe49dbd000-7ffe49dbf000 r-xp 00000000 00:00 0             [vdso]

列说明:
    starting address - ending address
    permissions
        r : read
        w : write
        x : execute
        s : shared
        p : private (copy on write)
    offset   : 如果不是file，则为0；
    device   : 如果是file，则是file所在device的major and monior device number，否则为00:00；
    inode    : 如果是file，则是file的inode number，否则为0；
    pathname : 有几种情况；
        file absolute path
        [stack]         the stack of the main process
        [stack:1001]    the stack of the thread with tid 1001
        [heap]
        [vdso] - virtual dynamic shared object, the kernel system call handler
        空白 -通常都是mmap创建的，用于其他一些用途的，比如共享内存


df -h
ls /dev/XXX -alh
echo $((0x0a))

print "Backed by file:\n";
print "  RO data                   r--  $mapped_rodata\n";
print "  Unreadable                ---  $mapped_unreadable\n"; 共享库同时存在？？？？
print "  Unknown                        $mapped_unknown\n";
print "Anonymous:\n";
print "  Writable code (stack)     rwx  $writable_code\n";
print "  Data (malloc, mmap)       rw-  $data\n";
print "  RO data                   r--  $rodata\n";
print "  Unreadable                ---  $unreadable\n";
print "  Unknown                        $unbacked_unknown\n";

16进制求和，都是16进制
awk --non-decimal-data  '{sum=($1 + $2); printf("0x%x %s\n", sum,$3)}'
strtonum("0x" $1)
echo $(( 16#a36b ))
echo "obase=2;256"|bc ibase base


print "Backed by file:\n";
print "  Unreadable                ---  $mapped_unreadable\n"; 共享库同时存在？？？？
print "  Unknown                        $mapped_unknown\n";
print "Anonymous:\n";
print "  Unreadable                ---  $unreadable\n";
print "  Unknown                        $unbacked_unknown\n";

代码
r-x 代码，包括程序(File)、共享库(File)、vdso(2Pages)、vsyscall(1Page)
rwx 没有，Backed by file: Write/Exec (jump tables); Anonymous: Writable code (stack)
r-- 程序中的只读数据，如字符串，包括程序(File)、共享库(File)
rw- 可读写变量，如全局变量；包括程序(File)、共享库(File)、stack、heap、匿名映射

静态数据、全局变量将保存在 ELF 的 .data 段中。
与smaps相关，以及一些实例
https://jameshunt.us/writings/smaps.html


各共享库的代码段，存放着二进制可执行的机器指令，是由kernel把该库ELF文件的代码段map到虚存空间；
各共享库的数据段，存放着程序执行所需的全局变量，是由kernel把ELF文件的数据段map到虚存空间；

用户代码段，存放着二进制形式的可执行的机器指令，是由kernel把ELF文件的代码段map到虚存空间；
用户数据段之上是代码段，存放着程序执行所需的全局变量，是由kernel把ELF文件的数据段map到虚存空间；

用户数据段之下是堆(heap)，当且仅当malloc调用时存在，是由kernel把匿名内存map到虚存空间，堆则在程序中没有调用malloc的情况下不存在；
用户数据段之下是栈(stack)，作为进程的临时数据区，是由kernel把匿名内存map到虚存空间，栈空间的增长方向是从高地址到低地址。

https://wiki.wxwidgets.org/Valgrind_Suppression_File_Howto


另外，可以通过 ldd 查看对应的映射地址，在实际映射到物理内存时，会添加随机的变量，不过如上的各个共享库的地址是相同的。

可以通过 echo $(( 0x00007f194de48000 - 0x00007f194dc2c000)) 计算差值。


maps 文件对应了内核中的 show_map()

show_map()
 |-show_map_vma()

address                  perms   offset   dev   inode       pathname

http://duartes.org/gustavo/blog/post/how-the-kernel-manages-your-memory/


主要是anon中的rw属性导致
cat /proc/$(pidof uagent)/maps | grep stack | wc -l



Clean_pages 自从映射之后没有被修改的页；
Dirty_pages 反之；
RSS 包括了共享以及私有，Shared_Clean+Shared_Dirty、Private_Clean+Private_Dirty
PSS (Proportional set size) 包括了所有的私有页 (Private Pages) 以及共享页的平均值。例如，一个进程有100K的私有页，与一个进程有500K的共享页，与四个进程有500K的共享页，那么 PSS=100K+(500K/2)+(500K/5)=450K
USS (Unique set size) 私有页的和。

awk -f test.awk /proc/$(pidof uagent)/maps
#! /bin/awk -f
BEGIN {
    mapped_executable    = 0
    mapped_wrexec        = 0
    mapped_rodata        = 0
    mapped_rwdata        = 0
    mapped_unreadable    = 0
    mapped_unknown       = 0
    writable_code        = 0
    data                 = 0
    rodata               = 0
    unreadable           = 0
    vdso                 = 0
    unbacked_unknown     = 0
}

{
    split($1, addr, "-")
    pages = (strtonum("0x" addr[2]) - strtonum("0x" addr[1]))/4096
    if ( $4 == "00:00") {
        if      ( $2 ~ /rwx/ ) {     writable_code += pages }
        else if ( $2 ~ /rw-/ ) {              data += pages }
        else if ( $2 ~ /r-x/ ) {              vdso += pages }
        else if ( $2 ~ /r--/ ) {            rodata += pages }
        else if ( $2 ~ /---/ ) {        unreadable += pages }
        else                   {  unbacked_unknown += pages }
    } else {
        if      ( $2 ~ /rwx/ ) {     mapped_wrexec += pages }
        else if ( $2 ~ /rw-/ ) {     mapped_rwdata += pages }
        else if ( $2 ~ /r-x/ ) { mapped_executable += pages }
        else if ( $2 ~ /r--/ ) {     mapped_rodata += pages }
        else if ( $2 ~ /---/ ) { mapped_unreadable += pages }
        else                   {    mapped_unknown += pages }
    }
}
END {
    printf ("Backed by file:\n")
    printf ("  Write/Exec (jump tables)  rwx  %d\n", mapped_wrexec)
    printf ("  Data                      rw-  %d\n", mapped_rwdata)
    printf ("  Executable                r-x  %d\n", mapped_executable)
    printf ("  RO data                   r--  %d\n", mapped_rodata)
    printf ("  Unreadable                ---  %d\n", mapped_unreadable)
    printf ("  Unknown                        %d\n", mapped_unknown)
    printf ("Anonymous:\n")
    printf ("  Writable code (stack)     rwx  %d\n", writable_code)
    printf ("  Data (malloc, mmap)       rw-  %d\n", data)
    printf ("  vdso, vsyscall            r-x  %d\n", vdso)
    printf ("  RO data                   r--  %d\n", rodata)
    printf ("  Unreadable                ---  %d\n", unreadable)
    printf ("  Unknown                        %d\n", unbacked_unknown)
}

pmap -x $(pidof uagent) > /tmp/1
awk -f test.awk /tmp/1
#! /bin/awk -f
BEGIN {
    lib_dirty_rx         = 0
    lib_dirty_rw         = 0
    lib_dirty_r          = 0
    lib_dirty_unknown    = 0
    lib_rss_rx         = 0
    lib_rss_rw         = 0
    lib_rss_r          = 0
    lib_rss_unknown    = 0

    uagent_dirty_rx      = 0
    uagent_dirty_rw      = 0
    uagent_dirty_r       = 0
    uagent_dirty_unknown = 0
    uagent_rss_rx      = 0
    uagent_rss_rw      = 0
    uagent_rss_r       = 0
    uagent_rss_unknown = 0

    anon_dirty_rw        = 0
    anon_dirty_rx        = 0
    anon_dirty_unknown   = 0
    anon_dirty_r         = 0
    anon_rss_rw        = 0
    anon_rss_rx        = 0
    anon_rss_unknown   = 0
    anon_rss_r         = 0

    count          = 0
    actual          = 0
}

$NF ~ /(^lib|^ld)/ {
    if      ( $5 ~ /r-x/ ) {
        lib_rss_rx += $3
        lib_dirty_rx += $4
    } else if ( $5 ~ /rw-/ ) {
        lib_rss_rw += $3
        lib_dirty_rw += $4
    } else if ( $5 ~ /r--/ ) {
        lib_rss_r += $3
        lib_dirty_r += $4
    } else {
        lib_rss_unknown += $3
        lib_dirty_unknown += $4
    }
    count += $3
}
$NF ~ /([a-zA-Z]+\.so$|^uagent\>)/ {
    if      ( $5 ~ /r-x/ ) {
        uagent_rss_rx += $3
        uagent_dirty_rx += $4
    } else if ( $5 ~ /rw-/ ) {
        uagent_rss_rw += $3
        uagent_dirty_rw += $4
    } else if ( $5 ~ /r--/ ) {
        uagent_rss_r += $3
        uagent_dirty_r += $4
    } else {
        uagent_rss_unknown += $3
        uagent_dirty_unknown += $4
    }
    count += $3
}
$NF ~ /^]$/ {
    if      ( $5 ~ /r-x/ ) {
        anon_rss_rx += $3
        anon_dirty_rx += $4
    } else if ( $5 ~ /rw-/ ) {
        anon_rss_rw += $3
        anon_dirty_rw += $4
    } else if ( $5 ~ /r--/ ) {
        anon_rss_r += $3
        anon_dirty_r += $4
    } else {
        anon_rss_unknown += $3
        anon_dirty_unknown += $4
    }
    count += $3
}
$1 ~ /^total\>/ {
    actual = $4
}
END {
    printf ("Libraries info:\n")
    printf (" Perm        RSS   Dirty\n")
    printf ("  r-x        %5d     %5d\n", lib_rss_rx, lib_dirty_rx)
    printf ("  rw-        %5d     %5d\n", lib_rss_rw, lib_dirty_rw)
    printf ("  r--        %5d     %5d\n", lib_rss_r,  lib_dirty_r)
    printf ("  Unknown    %5d     %5d\n", lib_rss_unknown, lib_dirty_unknown)

    printf ("Uagent info:\n")
    printf (" Perm        RSS   Dirty\n")
    printf ("  r-x        %5d     %5d\n", uagent_rss_rx,      uagent_dirty_rx)
    printf ("  rw-        %5d     %5d\n", uagent_rss_rw,      uagent_dirty_rw)
    printf ("  r--        %5d     %5d\n", uagent_rss_r,       uagent_dirty_r)
    printf ("  Unknown    %5d     %5d\n", uagent_rss_unknown, uagent_dirty_unknown)

    printf ("Anon info:\n")
    printf (" Perm        RSS   Dirty\n")
    printf ("  r-x        %5d     %5d\n", anon_rss_rx,      anon_dirty_rx)
    printf ("  rw-        %5d     %5d\n", anon_rss_rw,      anon_dirty_rw)
    printf ("  r--        %5d     %5d\n", anon_rss_r,       anon_dirty_r)
    printf ("  Unknown    %5d     %5d\n", anon_rss_unknown, anon_dirty_unknown)

    printf ("\nCount: %d  Actual: %d\n", count, actual)
}






可以通过mprotect设置内存的属性
https://linux.die.net/man/2/mprotect
Memory protection keys
https://lwn.net/Articles/643797/
Memory Protection and ASLR on Linux
https://eklitzke.org/memory-protection-and-aslr

ES Collectd插件
https://www.elastic.co/guide/en/logstash/current/plugins-codecs-collectd.html




真随机数等生成
http://www.cnblogs.com/bigship/archive/2010/04/04/1704228.html

在打印时，如果使用了 size_t 类型，那么通过 ```%d``` 打印将会打印一个告警，可以通过如下方式修改，也就是添加 ```z``` 描述。

size_t x = ...;
ssize_t y = ...;
printf("%zu\n", x);  // prints as unsigned decimal
printf("%zx\n", x);  // prints as hex
printf("%zd\n", y);  // prints as signed decimal

/proc/iomem 保存物理地址的映射情况，每行代表一个资源 (地址范围和资源名)，其中可用物理内存的资源名为 "System RAM" ，在内核中通过 insert_resource() 这个API注册到 iomem_resource 这颗资源树上。

例如，如下的内容：

01200000-0188b446 : Kernel code
0188b447-01bae6ff : Kernel data
01c33000-01dbbfff : Kernel bss

这些地址范围都是基于物理地址的，在 ```setup_arch()@arch/x86/kernel/setup.c``` 中通过如下方式注册。

max_pfn = e820_end_of_ram_pfn();
        code_resource.start = __pa_symbol(_text);
        code_resource.end = __pa_symbol(_etext)-1;
        insert_resource(&iomem_resource, &code_resource);

linux虚拟地址转物理地址
http://luodw.cc/2016/02/17/address/
Linux内存管理
http://gityuan.com/2015/10/30/kernel-memory/
/proc/iomem和/proc/ioports
http://blog.csdn.net/ysbj123/article/details/51088644
port地址空间和memory地址空间是两个分别编址的空间，都是从0地址开始
port地址也可以映射到memory空间中来，前提是硬件必须支持MMIO
iomem—I/O映射方式的I/O端口和内存映射方式的I/O端口
http://www.cnblogs.com/b2tang/archive/2009/07/07/1518175.html


协程
https://github.com/Tencent/libco


#if FOO < BAR
#error "This section will only work on UNIX systems"
#endif
http://hbprotoss.github.io/posts/cyu-yan-hong-de-te-shu-yong-fa-he-ji-ge-keng.html
https://linuxtoy.org/archives/pass.html
https://en.m.wikipedia.org/wiki/Padding_(cryptography)





 sed -e 's/collectd/\1/' *
sed只取匹配部分
http://mosquito.blog.51cto.com/2973374/1072249
http://blog.sina.com.cn/s/blog_470ab86f010115kv.html

通过sed只显示匹配行或者某些行
----- 显示1,10行
$ sed -n '1,10p' filename
----- 显示第10行
$ sed -n '10p' filename
----- 显示某些匹配行
$ sed -n '/This/p' filename
sed -n '/\<collectd\>/p' *
sed -i 's/\<Collectd/Uagent/g' *




Proxy
简单的实现，通常用于嵌入式系统
https://github.com/kklis/proxy
Golang实现的代理，包括了0复制技术等 
https://github.com/funny/proxy
dnscrypt-proxy DNS客户端与服务端的加密传输，使用libevent库
https://github.com/jedisct1/dnscrypt-proxy
ProxyChains Sock以及HTTP代理，通常用在类似TOR上面
https://github.com/haad/proxychains
https://github.com/rofl0r/proxychains-ng
基于libevent的简单代理服务
https://github.com/wangchuan3533/proxy
支持多种协议的代理服务，包括FTP、DNS、TCP、UDP等等
https://github.com/z3APA3A/3proxy
一个加密的代理服务
https://github.com/proxytunnel/proxytunnel
Vanish缓存中使用的代理服务
https://github.com/varnish/hitch
360修改的MySQL代理服务
https://github.com/Qihoo360/Atlas
Twitter提供的Memchached代理服务
https://github.com/twitter/twemproxy

UDP可靠传输方案
http://blog.codingnow.com/2016/03/reliable_udp.html
http://blog.csdn.net/kennyrose/article/details/7557917
BitCoin中使用的可靠UDP传输方案
https://github.com/maidsafe-archive/MaidSafe-RUDP
UDP方案的优缺点
https://blog.wilddog.com/?p=668




export http_proxy="http://<username>:<password>@proxy.foobar.com:8080"
export https_proxy="http://<username>:<password>@proxy.foobar.com:8080"
export ftp_proxy="http://<username>:<password>@proxy.foobar.com:8080"
export no_proxy="xxxx,xxxx"


https://github.com/wglass/collectd-haproxy  Python
https://github.com/Fotolia/collectd-mod-haproxy  C
https://github.com/funzoneq/collectd-haproxy-nbproc
https://github.com/signalfx/collectd-haproxy  ***
https://github.com/mleinart/collectd-haproxy  *

很多collectd插件的组合，很多不错的监控指标梳理
https://github.com/signalfx/integrations
https://github.com/DataDog/the-monitor
https://www.librato.com/docs/kb/collect/integrations/haproxy/
https://www.datadoghq.com/blog/monitoring-haproxy-performance-metrics/
https://github.com/mleinart/collectd-haproxy/blob/master/haproxy.py
Request: (只对HTTP代理有效)
   request_rate(req_rate)      px->fe_req_per_sec           proxy_inc_fe_req_ctr()  请求速率
   req_rate_max 请求限制速率
   req_tot 目前为止总的请求数
Response: (只对HTTP代理有效)
  'hrsp_1xx': ('response_1xx', 'derive'),
  'hrsp_2xx': ('response_2xx', 'derive'),
  'hrsp_3xx': ('response_3xx', 'derive'),
  'hrsp_4xx': ('response_4xx', 'derive'),
  'hrsp_5xx': ('response_5xx', 'derive'),
  'hrsp_other': ('response_other', 'derive'),

>>>>>backend<<<<<
Time:
  qtime (v1.5+) 过去1024个请求在队里中的平均等待时间
  rtime (v1.5+) 过去1024个请求在队里中的平均响应时间









http://savannah.nongnu.org/projects/nss-mysql
https://github.com/NigelCunningham/pam-MySQL
http://lanxianting.blog.51cto.com/7394580/1767113
https://stackoverflow.com/questions/7271939/warning-ignoring-return-value-of-scanf-declared-with-attribute-warn-unused-r
https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
https://stackoverflow.com/questions/30813452/how-to-ignore-all-warnings-in-c
http://www.jianshu.com/p/7e84a33b46e9
https://github.com/flike/kingshard/blob/master/doc/KingDoc/kingshard_admin_api.md   微博
__attribute__((warn_unused_result))


协程
http://blog.163.com/shu_wang/blog/static/1721704112016114113331412
https://stackoverflow.com/questions/28977302/how-do-stackless-coroutines-differ-from-stackful-coroutines
http://www.infoq.com/cn/articles/CplusStyleCorourtine-At-Wechat
https://www.iamle.com/archives/1865.html
https://github.com/mcxiaoke/mqtt
http://www.infoq.com/cn/articles/fine-tuned-haproxy-to-achieve-concurrent-ssl-connections?utm_campaign=rightbar_v2&utm_source=infoq&utm_medium=articles_link&utm_content=link_text



JMX(Java Management Extensions) 是一个为应用程序植入管理功能的框架，是一套标准的代理和服务，实际上，用户可以在任何 Java 应用程序中使用这些代理和服务实现管理。
http://blog.csdn.net/derekjiang/article/details/4532375
http://tomcat.apache.org/tomcat-6.0-doc/monitoring.html#Enabling_JMX_Remote
http://comeonbabye.iteye.com/blog/1463104
https://visualvm.github.io/
http://blog.csdn.net/kingzone_2008/article/details/50865350


Buddy 和  Slub 是 Linux 内核中的内存管理算法。Buddy 防止内存的 "外碎片"，即防止内存块越分越小，而不能满足大块内存分配的需求。Slub 防止内存的 "内碎片"，即尽量按请求的大小分配内存块，防止内存块使用上的浪费。https://github.com/chenfangxin/buddy_slub

https://stackoverflow.com/questions/9873061/how-to-set-the-source-port-in-the-udp-socket-in-c
http://www.binarytides.com/programming-udp-sockets-c-linux/
https://www.cyberciti.biz/faq/linux-unix-open-ports/
https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/udpclient.c
https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html
https://www.digitalocean.com/community/tutorials/how-to-use-netcat-to-establish-and-test-tcp-and-udp-connections-on-a-vps





http://my.fit.edu/~vkepuska/ece3551/ADI_Speedway_Golden/Blackfin%20Speedway%20Manuals/LwIP/socket-api/setsockopt_exp.html

socktop(systap使用)




Socket INTR的处理
http://blog.csdn.net/SUKHOI27SMK/article/details/43021081

http://www.tldp.org/HOWTO/html_single/C++-dlopen/

UDP-based Data Transfer Protocol
http://udt.sourceforge.net/
https://github.com/securesocketfunneling/udt
http://blog.leanote.com/post/caasi/Reliable-UDP-3

https://github.com/lsalzman/enet

https://askubuntu.com/questions/714503/regular-expressions-vs-filename-globbing


1.       #注释
2.       变量：使用set命令显式定义及赋值，在非if语句中，使用${}引用，if中直接使用变量名引用；后续的set命令会清理变量原来的值；
3.       command (args ...)  #命令不分大小写，参数使用空格分隔，使用双引号引起参数中空格
4.       set(var a;b;c) <=> set(var a b c)  #定义变量var并赋值为a;b;c这样一个string list
5.       Add_executable(${var}) <=> Add_executable(a b c)   #变量使用${xxx}引用

----- 条件语句
if(var)   # 其中空 0 N No OFF FALSE 视为假，NOT 后为真
else()/elseif()
endif(var)

7.       循环语句

Set(VAR a b c)

Foreach(f ${VAR})       …Endforeach(f)

8.       循环语句

WHILE() … ENDWHILE()



INCLUDE_DIRECTORIES(include)  # 本地的include目录设置
LINK_LIBRARIES('m')           # 添加库依赖，等价于命令行中的-lm参数



GTest 实际上不建议直接使用二进制文件，而是建议从源码开始编译。https://github.com/google/googletest/blob/master/googletest/docs/FAQ.md
如果要使用二进制包，那么可以使用如下方式进行配置。
find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest>=1.7.0)
pkg_check_modules(GMOCK REQUIRED gmock>=1.7.0)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
)
http://www.yeolar.com/note/2014/12/16/cmake-how-to-find-libraries/
http://blog.csdn.net/netnote/article/details/4051620

find_package(Threads REQUIRED)   # 使用内置模块查找thread库支持


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(uagent)

ADD_SUBDIRECTORY(librudp )
INCLUDE_DIRECTORIES(include)

option(WITH_UNIT_TESTS "Compile with unit tests" OFF)



https://github.com/sohutv/cachecloud  Redis监控管理
https://github.com/apache/incubator-superset 牛掰的项目管理
https://github.com/huichen/wukong 悟空搜索引擎
https://github.com/sylnsfar/qrcode   动态二维码生成
https://github.com/hellysmile/fake-useragent 伪装浏览器身份
https://github.com/jwasham/coding-interview-university 谷歌面试题
https://github.com/Tencent/libco 腾讯协程库
https://github.com/xtaci/kcptun 最快的UDP传输
https://github.com/reorx/httpstat 图形显示http处理耗时
https://github.com/ajermakovics/jvm-mon JVM监控
https://github.com/stampery/mongoaudit MongoDB审计
https://github.com/alexazhou/VeryNginx
https://github.com/helloqingfeng/Awsome-Front-End-learning-resource
https://github.com/shimohq/chinese-programmer-wrong-pronunciation
https://github.com/egonelbre/gophers
https://github.com/dbcli/mycli
https://github.com/nextcloud/server
https://github.com/SpaceVim/SpaceVim
https://github.com/nlohmann/json
https://github.com/alisaifee/flask-limiter
https://github.com/nicolargo/glances
https://github.com/nonstriater/Learn-Algorithms
https://github.com/ZuzooVn/machine-learning-for-software-engineers
https://github.com/jumpserver/jumpserver
https://github.com/FredWe/How-To-Ask-Questions-The-Smart-Way/blob/master/README-zh_CN.md
https://github.com/drduh/macOS-Security-and-Privacy-Guide
https://github.com/chrislgarry/Apollo-11
https://github.com/taizilongxu/interview_python
https://github.com/FallibleInc/security-guide-for-developers
https://github.com/SamyPesse/How-to-Make-a-Computer-Operating-System
https://github.com/yiminghe/learning-react


FIXME:
  /post/python-modules.html
  Python 包管理
  http://www.jianshu.com/p/9acc85d0ff16
  http://xiaorui.cc/2014/09/20/%E4%BD%BF%E7%94%A8hashring%E5%AE%9E%E7%8E%B0python%E4%B8%8B%E7%9A%84%E4%B8%80%E8%87%B4%E6%80%A7hash/
360
https://github.com/flike/kingshard
https://github.com/Qihoo360/Atlas
https://tech.meituan.com/dbproxy-pr.html
http://www.cnblogs.com/wunaozai/p/3955156.html
https://tech.meituan.com/
HMAC
http://www.oschina.net/question/12_2828
https://github.com/awslabs/s2n
https://btrfs.wiki.kernel.org/index.php/Main_Page

http://blog.csdn.net/factor2000/article/details/3929816
http://www.tldp.org/HOWTO/html_single/C++-dlopen/


Chrome Vimium 快捷键可以直接通过 ? 查看。

git clone -b 0.10.0 https://github.com/pika/pika.git


日志保存在 /var/log/rabbitmq 目录下。

schema_integrity_check_failed
一般是由于 mnesia 数据库的问题导致，简单粗暴的方式是直接删除。

systemd
rabbitmq-server.service: Got notification message from PID 10513



MTU
Maximum Transmission Unit
 ifconfig eth0 mtu number
/etc/sysconfig/network-scripts/ifcfg-eth0
MTU=1000
IPV6_MTU=1000
http://www.361way.com/linux-mtu-jumbo-frames/4055.html
http://www.microhowto.info/howto/change_the_mtu_of_a_network_interface.html
http://www.cnblogs.com/liu-yao/p/5678161.html
http://blog.csdn.net/anzhsoft/article/details/19563091







首先通过 crontab(crontab.c) 完成任务的编辑，然后通过 poke_daemon() 通知 crond 程序，实际上就是通过 utime() 修改 SPOOL_DIR 目录的访问和修改时间。而在 crond(cron.c) 程序中，会通过 inotify 机制接收，然后进行更新。

http://blog.csdn.net/rain_qingtian/article/details/11008779

https://github.com/DaveGamble/cJSON

语法规则可以参考 [JSON: The Fat-Free Alternative to XML](yuhttp://www.json.org/fatfree.html) 。

parse_value()  正式的语法解析

https://github.com/staticlibs/ccronexpr

American Fuzzy Lop, AFL 是一种开源的模糊测试器，由谷歌的 Michal Zalewski 开发。可以在源码编译时添加，或者使用 QEMU 模式，也就是 QEMU-(User Mode) ，在执行时注入部分代码进行测试。http://lcamtuf.coredump.cx/afl/
https://github.com/google/syzkaller
https://github.com/xxg1413/fuzzer/tree/master/iFuzz
https://stfpeak.github.io/2017/06/12/AFL-Cautions/
http://bobao.360.cn/news/detail/3354.html
http://www.jianshu.com/p/015c471f5a9d
http://ele7enxxh.com/Use-AFL-For-Stagefright-Fuzzing-On-Linux.html
http://www.freebuf.com/articles/system/133210.html
http://www.hackdig.com/07/hack-24522.htm
http://aes.jypc.org/?p=38207
https://fuzzing-project.org/tutorial3.html
afl-fuzz -i afl_in -o afl_out ./binutils/readelf -a @@
afl-gcc afl-clang-fast

http://blog.codingnow.com/2017/07/float_inconsistence.html#more


:set nopaste
:UltiSnipsEdit
:verbose imap <tab>
http://www.cnblogs.com/acbingo/p/4757275.html
http://blog.guorongfei.com/2016/12/03/vim-ultisnipt-google-c-cpp-header-gurad/
http://vimzijun.net/2016/10/30/ultisnip/
http://www.linuxidc.com/Linux/2016-11/137665.htm
https://segmentfault.com/q/1010000000610373

http://liulixiaoyao.blog.51cto.com/1361095/814329













https://casatwy.com/pthreadde-ge-chong-tong-bu-ji-zhi.html
http://blog.csdn.net/willib/article/details/32942189

https://github.com/beego/beedoc/blob/master/zh-CN/module/grace.md
https://www.nginx.com/resources/wiki/start/topics/tutorials/commandline/
http://blog.csdn.net/brainkick/article/details/7192144
http://shzhangji.com/cnblogs/2012/12/23/nginx-live-upgrade/

http://www.freebuf.com/sectool/119680.html
http://tonybai.com/2011/04/21/apply-style-check-to-c-code/
https://github.com/dspinellis/cqmetrics

VGC、RATS、Source Insight


测试版本


core
gcov
CPP-Check
Flawfinder


静态安全扫描 flawfinder、RATS、ITS4、VCG、CPPLint、SPlint

Python: Pychecker、Pylint、RATS


python -m SimpleHTTPServer

## flawfinder

一个 Python 写的程序，用于扫描代码，然后在规则库 (c_ruleset) 中查找符合规则的场景。

源码可以直接从 [www.dwheeler.com](https://www.dwheeler.com/flawfinder/) 上下载，安装方式可以查看 README 文件，也就是如下命令。

$ tar xvzf FILENAME.tar.gz       # Uncompress distribution file
$ cd flawfinder-*                # cd into it.
# make prefix=/usr install       # Install in /usr

该工具只针对单个语句进行词法分析，不检查上下文，不分析数据类型和数据流；检查运行时可能存在的问题，比如内存泄露；然后会根据规则库给出代码建议。这也就意味着会有部分的误报，不过因为使用简单，仍不失为一个不错的静态检测工具。

检查可以直接指定文件或者目录，工具会自动查看所有的 C/C++ 文件，如果是 patch (diff -u、svn diff、git diff) 添加参数 --patch/-P 即可。严重等级从 0 到 5 依次增加，而且会标示出 [Common Weakness Enumeration, CWE](https://cwe.mitre.org/data/) 对应。

检查时会读取 ruleset 中的规则，然后如果匹配 (hit) 则将匹配数据保存到 hitlist 中，

### 常见操作

1. 重点检查与外部不可信用户的交互程序，先确保这部分程序无异常；
2. 如果已经审计的函数，可以通过 ```// Flawfinder: ignore``` 或者 ```/* Flawfinder: ignore */``` 减少异常输出，为了兼容，也可以使用 ```ITS4``` 或者 ```RATS``` 替换 ```Flawfinder```；


--inputs/-I
  只检查从外部用户(不可信)获取数据的函数；
--neverignore/-n
  默认可以通过上述的方式忽略标记的行，通过该参数用于强制检测所有的代码；
--savehitlist, --loadhitlist, --diffhitlist
  用于保存、加载、比较hitlist；
--minlevel=NUMBER
  指定最小的错误汇报级别；

--quiet/-Q
  默认会在检测时打印检查了哪些文件，通过该选项可以关闭，通常用于格式化输出检测；
--dataonly/-D
  不显示header和footer，可以配合--quiet参数只显示数据；
--singleline/-S
  检测结果默认会多行显示，该参数指定一行显示；
--immediate/-i
  默认在全部文件检测完之后，进行排序，然后显示最终的结果，该参数可以在监测到异常后立即显示；


----- 检查所有的代码，即使已经标记为ignore的代码
$ flawfinder --neverignore src
----- 可以通过如下命令输出，以供其它自动化工具使用
$ flawfinder -QD src
$ flawfinder -QDSC src
----- 检查代码只汇报CWE-120或者CWE-126
$ flawfinder --regex "CWE-120|CWE-126" src/


/* RATS: ignore */



uagent 调试，
export UAGENT_TRACE="yes"

flawfinder -Q --minlevel=5 src | less



int lcc_connect(const char *address, lcc_connection_t **ret_con);
功能：
  建立指向address的socket链接，通过ret_con返回链接信息；
入参:
  address socket地址，如/usr/var/run/uagent.sock、unix:/usr/var/run/uagent.sock；
  ret_con 返回的已经建立好的链接；
返回值：
  -1 入参异常，或者没有内存；
  0  正常返回；

int lcc_disconnect(lcc_connection_t *c);
功能：
  关闭链接，释放资源；

## coverage
http://blog.csdn.net/livelylittlefish/article/details/6448885
编译链接时需要修改配置选项。

* 编译的时候，增加 -fprofile-arcs -ftest-coverage 或者 –coverage；
* 链接的时候，增加 -fprofile-arcs 或者 –lgcov；
* 打开–g3 选项，去掉-O2以上级别的代码优化选项，否则编译器会对代码做一些优化，例如行合并，从而影响行覆盖率结果。

ifeq ($(coverage), yes)
CFLAGS   +=  -fprofile-arcs -ftest-coverage -g3
LDFLAGS  +=  -fprofile-arcs -ftest-coverage
endif

如下是测试步骤。
----- 1. 编译源码，此时每个文件都会生成一个*.gcno文件
$ make coverage=yes
----- 2. 运行，运行之后会生成 *.gcda 文件
$ ./helloworld
----- 3.1 可以通过如下命令生成单个文件的覆盖率，生成的是文本文件*.gcov
$ gcov helloworld.c

除了使用 gcov 之外，还可以通过 lcov 查看覆盖率，简单说下 *.gcov 的文件格式。

    -:    2:#include <assert.h>   非有效行
    -:    3:#include <stdlib.h>
 ... ...
  148:   71:  if (n == NULL)      调用次数
#####:   72:    return (0);       未调用


简单介绍下代码覆盖率的常见术语。


主要是基本块（Basic Block），基本块图（Basic Block Graph），行覆盖率（line coverage）, 分支覆盖率（branch coverage）等。

##### 基本块
这里可以把基本块看成一行整体的代码，基本块内的代码是线性的，要不全部运行，要不都不运行，其详细解释如下：
A basic block is a sequence of instructions with only entry and only one exit. If any one of the instructions are executed, they will all be executed, and in sequence from first to last.





    基本块图（Basic Block Graph），基本块的最后一条语句一般都要跳转，否则后面一条语句也会被计算为基本块的一部分。 如果跳转语句是有条件的，就产生了一个分支(arc)，该基本块就有两个基本块作为目的地。如果把每个基本块当作一个节点，那么一个函数中的所有基本块就构成了一个有向图，称之为基本块图(Basic Block Graph)。且只要知道图中部分BB或arc的执行次数就可以推算出所有的BB和所有的arc的执行次数；
    打桩，意思是在有效的基本块之间增加计数器，计算该基本块被运行的次数；打桩的位置都是在基本块图的有效边上；

##### 行覆盖率
就是源代码有效行数与被执行的代码行的比率；

##### 分支覆盖率
有判定语句的地方都会出现 2 个分支，整个程序经过的分支与所有分支的比率是分支覆盖率。注意，与条件覆盖率(condition coverage)有细微差别，条件覆盖率在判定语句的组合上有更细的划分。

### gcc/g++ 编译选项

如上所述，在编译完成后会生成 *.gcno 文件，在运行正常结束后生成 *.gcda 数据文件，然后通过 gcov 工具查看结果。

--ftest-coverage
  让编译器生成与源代码同名的*.gcno文件 (note file)，含有重建基本块依赖图和将源代码关联至基本块的必要信息；
--fprofile-arcs
  让编译器静态注入对每个源代码行关联的计数器进行操作的代码，并在链接阶段链入静态库libgcov.a，其中包含在程序正常结束时生成*.gcda文件的逻辑；

可以通过源码解析来说明到底这 2 个选项做了什么，命令如下：
g++ -c -o hello.s hello.c -g -Wall -S
g++ -c -o hello_c.s hello.c -g -Wall –coverage -S
vimdiff hello.s hello_c.s


1. 覆盖率的结果只有被测试到的文件会被显示，并非所有被编译的代码都被作为覆盖率的分母

实际上，可以看到整个覆盖率的产生的过程是4个步骤的流程，一般都通过外围脚本，或者makefile/shell/python来把整个过程自动化。2个思路去解决这个问题，都是通过外围的伪装。第一个，就是修改lcov的 app.info ，中间文件，找到其他的文件与覆盖率信息的地方，结合makefile，把所有被编译过的源程序检查是否存于 app.info 中，如果没有，增加进去。第二个伪装，是伪装 *.gcda，没有一些源码覆盖率信息的原因就是该文件没有被调用到，没有响应的gcda文件产生。


2. 后台进程的覆盖率数据收集；


其实上述覆盖率信息的产生，不仅可以针对单元测试，对于功能测试同样适用。但功能测试，一般linux下c/c++都是实现了某个Daemon进程，而覆盖率产生的条件是程序需要正常退出，即用户代码调用 exit 正常结束时，gcov_exit 函数才得到调用，其继续调用 __gcov_flush 函数输出统计数据到 *.gcda 文件中。同样2个思路可以解决这个问题，

第一，给被测程序增加一个 signal handler，拦截 SIGHUP、SIGINT、SIGQUIT、SIGTERM 等常见强制退出信号，并在 signal handler 中主动调用 exit 或 __gcov_flush 函数输出统计结果。但这个需要修改被测程序。这个也是我们之前的通用做法。但参加过清无同学的一个讲座后，发现了下面第二种更好的方法。

第二，借用动态库预加载技术和 gcc 扩展的 constructor 属性，我们可以将 signalhandler 和其注册过程都封装到一个独立的动态库中，并在预加载动态库时实现信号拦截注册。这样，就可以简单地通过如下命令行来实现异常退出时的统计结果输出了。


### lcov

用于生成 html 格式的报告。

yum install --enablerepo=epel lcov perl-GD

----- 1. 生成*.info文件
$ lcov -d . -o 'hello_test.info' -t ‘Hello test’ -b . -c
参数解析：
   -d 指定目录
----- 2. 生成html，-o指定输出目录，可以通过HTTP服务器查看了
$ genhtml -o result hello_test.info



## 静态检查

http://www.freebuf.com/sectool/119680.html

cppcheck、Splint(Secure Programming Lint)

### cppcheck

直接从 [github cppcheck](https://github.com/danmar/cppcheck) 下载，然后通过 make && make install 编译安装即可。

cppcheck -j 3 --force --enable=all src/*

--force
  如果#ifdef的宏定义过多，则cppcheck只检查部分
-j
  检查线程的个数，用于并发检查；
--enable
  指定当前的检查级别，可选的参数有all，style，information等；
--inconclusive
  默认只会打印一些确认的错误，通过该参数配置异常的都打印；



### Splint

http://www.cnblogs.com/bangerlee/archive/2011/09/07/2166593.html


## 圈复杂度 (Cyclomatic complexity)

OCLint(Mac) cppncss SourceMonitor(Windows)

常用概念介绍如下：

* Non Commenting Source Statements, NCSS 去除注释的有效代码行；
* Cyclomatic Complexity Number, CCN 圈复杂度。

同样，一个函数的 CCN 意味着需要多少个测试案例来覆盖其不同的路径，当 CCN 发生很大波动或者 CCN 很高的代码片段被变更时，意味改动引入缺陷风险高，一般最好小于 10 。



Findbugs (compiled code analysis)
PMD (static code analysis)

### SourceMonitor

http://www.campwoodsw.com/sourcemonitor.html

### cppncss

很简单的计算圈复杂度的工具，java。


## 内存检测

Valgrind


HAVE_LIBGEN_H 1
HAVE_FNMATCH_H 1

----- 当前目录下生成buildbot_master目录，以及配置文件master.cfg.sample
$ buildbot create-master buildbot_master


$ cd buildbot_master && mv master.cfg.sample master.cfg
$ buildbot checkconfig master.cfg
c['buildbotNetUsageData'] = None

----- 运行
$ buildbot start buildbot_master
# 查看日志
tail -f master/twistd.log




https://github.com/nodejs/http-parser
https://github.com/shellphish/how2heap
https://github.com/DhavalKapil/heap-exploitation

https://github.com/maidsafe-archive/MaidSafe-RUDP/wiki
RTP  https://tools.ietf.org/html/rfc3550
UDT http://udt.sourceforge.net/
https://github.com/dorkbox/UDT

https://github.com/greensky00/avltree
http://blog.csdn.net/q376420785/article/details/8286292
http://xstarcd.github.io/wiki/shell/UDP_Hole_Punching.html



http://www.wowotech.net/timer_subsystem/tick-device-layer.html
https://www.w3.org/CGI/
https://github.com/HardySimpson/zlog











main()
 |-handle_read()
   |-httpd_start_request()
     |-really_start_request()
       |-cgi()
         |-cgi_child()
           |-make_envp()
             |-build_env()



onion_http_new() 会将onion_http_read_ready()赋值给read_ready

onion_http_read_ready()
 |-onion_request_process()


onion_url_new()
 |-onion_url_handler()

onion_listen_point_accept() 在listen端口上出现时调用

onion_listen_point_read_ready()








Micro Transport Protocol, μTP，因为 μ 输入困难 μTP 通常被写为 uTP 。这是一个由 BitTorrent 公司开发的协议，在 UDP 协议之上实现可靠传输与拥塞控制等特性，可以克服多数防火墙和 NAT 的阻碍，从而大大提高用户的连接性以及下载速度。

关于该协议的详细内容可以参考 [uTorrent Transport Protocol](http://www.bittorrent.org/beps/bep_0029.html) 中的内容。

max_window 最大窗口，在传输过程中 (in-flight) 可能的最大字节，也就是已经被发送但是还没有收到响应的报文；
cur_window 当前窗口，当前正在传输中的字节数。
wnd_size   窗口大小，也就是对端建议使用的窗口大小，同时会限制传输过程中的字节数。

只有当 (cur_window + packet_size) <= min(max_window, wnd_size) 时才可以发送数据，

A socket may only send a packet if cur_window + packet_size is less than or equal to min(max_window, wnd_size). The packet size may vary, see the packet sizes section.

上报协议格式内容
http://code.huawei.com/Monitor/CloudMonitor-Common/wikis/agent-protocol


cJSON_CreateObject()    创建新的对象，设置对应的type类型
 |-cJSON_New_Item()     新申请cJSON结构内存，并初始化为0
cJSON_CreateString()    和cJSON_CreateRaw()函数调用相同，只是设置的类型不同
 |-cJSON_New_Item()
 |-cJSON_strdup()       创建对象后会将字符串复制一份
cJSON_Print()
 |-print()
   |-print_value()


typedef struct cJSON {
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    struct cJSON *child;    对于Array类型，则会作为链表头
    int type;               类型，包括了String、Number、Raw等
    char *valuestring;      如果是String或者Raw时使用
    int valueint;           这个已经取消，使用valuedouble替换，为了兼容未删除
    double valuedouble;     如果是Number时使用

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} cJSON;

Invalid、True、False、NULL、Object、Array 通过 type 区分，

cJSON_Parse()
 |-cJSON_ParseWithOpts()
   |-cJSON_New_Item()
   |-parse_value()  根据不同的字段进行解析

cJSON_Duplicate()
cJSON_Minify()
???cJSON_Compare()

cJSON_Parse()
cJSON_Print()  按照可阅读的格式打印，方便阅读，一般用于交互
cJSON_PrintUnformatted() 最小化打印，一般用于格式发送
cJSON_PrintBuffered()
cJSON_PrintPreallocated()

string trim
https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way


scanf 中一种很少见但很有用的转换字符 `[...]` 和 `[ ^...]` 。

#include <stdio.h>
int main()
{
    char strings[100];
    scanf("%[1234567890]", strings);
    printf("%s", strings);
    return 0;
}

运行，输入 `1234werew` 后，结果是 `1234` ，也就是说，如果输入的字符属于方括号内字符串中某个字符，那么就提取该字符；如果一经发现不属于就结束提取。

这就是 ANSI C 增加的一种新特性，称为扫描集 (scanset)，由一对方括号中的一串字符定义，左方括号前必须缀以百分号，通过 `^` 表示补集。

注意，其中必须至少包含一个字符，否则非法，如 `%[]` 和 `%[^]` 是非法的。

%[a-z]  读取在 a-z 之间的字符串
    char s[]="hello, my friend";   // 注意: 逗号在不a-z之间
    sscanf(s, "%[a-z]", string);   // string=hello
%[^a-z] 读取不在 a-z 之间的字符串，如果碰到a-z之间的字符则停止
    char s[]="HELLOkitty";
    sscanf( s, "%[^a-z]", string); // string=HELLO
%*[^=]  前面带 * 号表示不保存变量，跳过符合条件的字符串。
    char s[]="notepad=1.0.0.1001" ;
    char szfilename [32] = "" ;
    int i = sscanf( s, "%*[^=]", szfilename ) ;
// szfilename=NULL,因为没保存

int i = sscanf( s, "%*[^=]=%s", szfilename ) ;
// szfilename=1.0.0.1001


%40c 读取40个字符


%[^=] 读取字符串直到碰到’=’号，’^’后面可以带更多字符,如：
              char s[]="notepad=1.0.0.1001" ;
              char szfilename [32] = "" ;
             int i = sscanf( s, "%[^=]", szfilename ) ;
           // szfilename=notepad
       如果参数格式是：%[^=:] ，那么也可以从 notepad:1.0.0.1001读取notepad
http://www.cnblogs.com/mafly/p/postman.html

http://www.quartz-scheduler.org/documentation/quartz-2.x/tutorials/crontrigger.html
http://www.blogjava.net/javagrass/archive/2011/07/12/354134.html
https://meekrosoft.wordpress.com/2009/11/09/unit-testing-c-code-with-the-googletest-framework/


https://en.wikipedia.org/wiki/Network_Time_Protocol

Linux 内核通过 adjtime() 或者 ntp_adjtime() 来进行时钟的同步，ntptime
http://jfcarter.net/~jimc/documents/bugfix/12-ntp-wont-sync.html
http://libev.schmorp.de/bench.c
https://stackoverflow.com/questions/14621261/using-libev-with-multiple-threads
https://curl.haxx.se/libcurl/c/evhiperfifo.html
https://github.com/HardySimpson/zlog
http://kagachipg.blogspot.com/2013/10/multi-thread-in-libev.html














conn_config()
 |-port_collect_listening  如果配置了ListeningPorts变量则设置为1
 |-conn_get_port_entry()   对于LocalPort和RemotePort参数，如果存在则设置否则创建
 |-port_collect_total      如果配置了AllPortsSummary变量则设置为1
conn_read()
 |-
####

$ cat << EOF > request.txt
GET / HTTP/1.1
Host: 127.1
EOF
$ cat request.txt | openssl s_client -connect 127.1:443


printf 'GET / HTTP/1.1\r\nHost: github.com\r\n\r\n' | ncat --ssl github.com 443

----- 发送系统日志内容
$ ncat -l -p 6500 | tee -a copy.out | tar -zx -C $(mktemp -d)
$ (tar -zc -C /var/log; tail -f /var/log/syslog) | ncat 127.1 6500

----- 使用SSL传输
$ ncat -l -p 6500 --ssl --ssl-cert /etc/ssl/host.crt \
    --ssl-key /etc/ssl/host.key > out.tgz
$ tar -zc ~ | ncat --ssl 127.1 6500

----- 使用UDP协议
$ ncat -l -p 6500 --udp > out.tgz
$ tar -zc ~ | ncat --udp machineb 6500

----- 使用SCTP
$ ncat --sctp -l -p 6500 > out.tgz
$ tar -zc ~ | ncat --sctp machineb 6500
给系统添加根证书
http://manuals.gfi.com/en/kerio/connect/content/server-configuration/ssl-certificates/adding-trusted-root-certificates-to-the-server-1605.html
https://segmentfault.com/a/1190000002569859
CentOS 会保存在 /etc/ssl/certs/ 目录下，

--ssl                  Connect or listen with SSL
--ssl-cert             Specify SSL certificate file (PEM) for listening
--ssl-key              Specify SSL private key (PEM) for listening
--ssl-verify           Verify trust and domain name of certificates
--ssl-trustfile        PEM file containing trusted SSL certificates

http://blog.csdn.net/ljy1988123/article/details/51424162
http://blog.csdn.net/younger_china/article/details/72081779
http://blog.csdn.net/yusiguyuan/article/details/48265205

SSL Certificate File 文件中包含了一个 X.509 证书，实际上也就是加密用的公钥，而 SSL Certificate Key File 文件中是公钥对应的私钥，在进行安全传输时就需要这对密钥。有的程序是将两者放在一起，如一些 Java 程序；有的则会分开存储，如 Apache 。

一般在申请了证书之后，如通过 GoDaddy，会提供上述的两个文件。

如果服务端只使用了上述的两个文件，那么实际上客户端并不知道这个证书是谁颁发的；不过一般来说没有太大问题，因为客户端会保存很多的 CA 证书，包括中间证书以及根证书。如果要直接指定证书的依赖关系，可以通过 SSLCertificateChainFile 参数指定。

Nginx https配置
https://fatesinger.com/75967
https://imququ.com/post/my-nginx-conf-for-security.html


tail  -> coreutils
tailf -> util-linux

Linux Shell man 命令详细介绍
http://blog.jobbole.com/93404/
http://www.lai18.com/content/1010397.html

网络监控
https://stackoverflow.com/questions/614795/simulate-delayed-and-dropped-packets-on-linux

The f_frsize value is the actual minimum allocation unit of the
filesystem, while the f_bsize is the block size that would lead to
most efficient use of the disk with io calls.  All of the block counts
are in terms of f_frsize, since it is the actual allocation unit size.
 The BSD manpages are a bit more informative on this function than the
POSIX ones.

https://blog.blahgeek.com/glibc-and-symbol-versioning/
http://www.runoob.com/linux/linux-comm-indent.html
http://riemann.io/quickstart.html
http://blog.csdn.net/c80486/article/details/45066439
http://www.hzrcj.org.cn/personnel/pd01/findda_qc01
https://github.com/mkirchner/tcping/blob/master/tcping.c

C格式化检查
sparse

indent                                \
 --ignore-profile                  \    不读取indent的配置文件
 --k-and-r-style                   \    指定使用Kernighan&Ritchie的格式
 --indent-level8                   \    缩进多少字符，如果为tab的整数倍，用tab来缩进，否则用空格填充
 --tab-size8                       \    tab大小为8
 --swallow-optional-blank-lines    \    删除多余的空白行
 --line-length130                  \    设置每行的长度
 --no-space-after-casts            \    不要在cast后添加一个空格
 --space-special-semicolon         \    若for或while区段只有一行时，在分号前加上空格
 --else-endif-column1              \    将注释置于else与elseif右侧
    --use-tabs                        \    使用tab做缩进
 --blank-lines-after-procedures    \    函数结束后加空行
 --blank-lines-after-declarations  \    声明结束后加空行
    load.c

find -type f -regextype posix-egrep -regex ".*(~|\.bak)$" -exec ls -alh {} \;

NAN 一种是 <math.h> 中提供的默认值，也可以自定义宏，如下

#define NAN          (0.0 / 0.0)
#define isnan(f)     ((f) != (f))
#define isfinite(f)  (((f) - (f)) == 0.0)
#define isinf(f)     (!isfinite(f) && !isnan(f))

http://zh.cppreference.com/w/c/numeric/math/fpclassify

其中使用 `isnan()` 时，`FLT_EVAL_METHOD` 将被忽略。

FIXME:
https://jin-yang.github.io/post/collectd-source-code.html
 |   | | | | |-FORMAT_VL()                    ← 实际上是调用format_name()将vl中的值生成标示符
             |-pthread_mutex_lock()
 |   | | | | |-c_avl_get()                    ← 利用上述标示符获取cache_entry_t，在此会缓存最近的一次采集数据
             |-uc_insert() 如果不存在则插入，直接解锁退出
    |-合法性检查，上次的采集时间应该小于本次时间
    |-根据不同的类型进行检查<<<DS-TYPE>>>，计算方法详见如下
 |   | | | | |... ...                         ← 会根据不同的类型进行处理，例如DS_TYPE_GAUGE
 |   | | | | |-uc_check_range()               ← 检查是否在指定的范围内

values_raw   保存原始数据的值
values_gauge 会按照不同的类型进行计算，其中计算规则如下







git diff <local branch> <remote>/<remote branch>
----- 查看stage中的diff
git diff --cached/--staged
http://perthcharles.github.io/2015/08/25/clean-commit-log-before-push/
https://github.com/chenzhiwei/linux/tree/master/git
https://ddnode.com/2015/04/14/git-modify-remote-responsity-url.html

通过amend修改之后，需要使用--force强制推送该分支。
git push --force origin feature/ping:feature/ping

etcd
https://tonydeng.github.io/2015/10/19/etcd-application-scenarios/
https://tonydeng.github.io/2015/11/24/etcd-the-first-using/
http://cizixs.com/2016/08/02/intro-to-etcd
http://debugo.com/using-etcd/
curl
http://blog.likewise.org/2011/08/brushing-up-on-curl/
http://www.ruanyifeng.com/blog/2011/09/curl.html
http://www.cnblogs.com/gbyukg/p/3326825.html
https://stackoverflow.com/questions/27368952/linux-best-way-in-two-way-ipc-in-c
http://www.cnblogs.com/zhang-shijie/p/5439210.html
正则表达式
https://www.zhihu.com/question/27434493


以 `CHECK_SYMBOL_EXISTS()` 宏为例，对于 CentOS，在 `/usr/share/cmakeX/Modules` 中存在 `CheckSymbolExists.cmake` 模板，可以直接查看相关宏的定义；其它类似模板同样可以进行相应的检查。

ss -tan |awk 'NR>1{++S[$1]}END{for (a in S) print a,S[a]}' && ./tcpstatus

https://github.com/schweikert/fping
https://github.com/octo/liboping


https://www.typora.io/#windows
json-handle markdown edit chrome浏览器
http://blog.csdn.net/fandroid/article/details/45787423
进程通讯
http://blog.csdn.net/21aspnet/article/details/7479469

https://github.com/TeamStuQ/skill-map

## 网络监控

Interface /proc/net/dev
TCPConns
PowerDNS
IPtables
netlink

命令
ethtool
   ethtool -i eth0
netstat

libev压测
http://libev.schmorp.de/bench.c
spark经典
https://aiyanbo.gitbooks.io/spark-programming-guide-zh-cn/content/index.html
https://github.com/lw-lin/CoolplaySpark/blob/master/Spark%20Streaming%20%E6%BA%90%E7%A0%81%E8%A7%A3%E6%9E%90%E7%B3%BB%E5%88%97/readme.md
小型数据库
lmdb
cdb  https://github.com/rmind/libcdb
Kyoto Cabinet   http://fallabs.com/kyotocabinet/
https://github.com/symisc/unqlite
https://github.com/numetriclabz/awesome-db
https://github.com/gstrauss/mcdb
https://github.com/tklauser/inotail/blob/master/inotail.c
https://github.com/adamierymenko/kissdb



Futex
http://www.cnblogs.com/bbqzsl/p/6814031.html
http://www.jianshu.com/p/570a61f08e27
http://blog.csdn.net/Javadino/article/details/2891385
http://blog-kingshaohua.rhcloud.com/archives/84
http://blog.csdn.net/michael_r_chang/article/details/30717763
http://www.anscen.cn/article1.html
http://kouucocu.lofter.com/post/1cdb8c4b_50f62fe
http://blog.sina.com.cn/s/blog_e59371cc0102v29b.html
https://bg2bkk.github.io/post/futex%E5%92%8Clinux%E7%9A%84%E7%BA%BF%E7%A8%8B%E5%90%8C%E6%AD%A5%E6%9C%BA%E5%88%B6/
http://kexianda.info/2017/08/17/%E5%B9%B6%E5%8F%91%E7%B3%BB%E5%88%97-5-%E4%BB%8EAQS%E5%88%B0futex%E4%B8%89-glibc-NPTL-%E7%9A%84mutex-cond%E5%AE%9E%E7%8E%B0/

https://github.com/Hack-with-Github/Awesome-Hacking
NTPL生产者消费者模型
http://cis.poly.edu/cs3224a/Code/ProducerConsumerUsingPthreads.c

netlink编程，以及比较有意思的程序
http://edsionte.com/techblog/archives/4140
https://github.com/lebougui/netlink
https://github.com/eworm-de/netlink-notify
以及ss程序使用示例
http://www.binarytides.com/linux-ss-command/
https://www.cyberciti.biz/files/ss.html
https://serverfault.com/questions/510708/tail-inotify-cannot-be-used-reverting-to-polling-too-many-open-files
https://serverfault.com/questions/561107/how-to-find-out-the-reasons-why-the-network-interface-is-dropping-packets

多线程编程，其中有很多DESIGN_XXX.txt的文档，甚至包括了Systemtap的使用，其底层用到的是系统提供的 futex_XXX() 调用。
https://github.com/lattera/glibc/tree/master/nptl
https://en.wikipedia.org/wiki/List_of_C%2B%2B_multi-threading_libraries

浅谈C++ Multithreading Programming
http://dreamrunner.org/blog/2014/08/07/C-multithreading-programming/
Introduction to Parallel Computing
https://computing.llnl.gov/tutorials/parallel_comp/
解剖 Mutex
https://techsingular.org/2012/01/05/%E8%A7%A3%E5%89%96-mutex/
Pthreads并行编程之spin lock与mutex性能对比分析
http://www.parallellabs.com/2010/01/31/pthreads-programming-spin-lock-vs-mutex-performance-analysis/
Linux线程浅析
http://blog.csdn.net/qq_29924041/article/details/69213248

LinuxThreads VS. NPTL
https://www.ibm.com/developerworks/cn/linux/l-threading.html
http://pauillac.inria.fr/~xleroy/linuxthreads/

FUTEX简析，也可以通过man 7 futex man 2 futex 查看
http://blog.sina.com.cn/s/blog_e59371cc0102v29b.html
futex and userspace thread syncronization (gnu/linux glibc/nptl) analysis
http://cottidianus.livejournal.com/325955.html
原始论文
https://www.kernel.org/doc/ols/2002/ols2002-pages-479-495.pdf
进程资源
http://liaoph.com/inux-process-management/

http://www.cnblogs.com/big-xuyue/p/4098578.html

BMON Ncurse编程
tcpconns
conn_read()
 |-conn_reset_port_entry()
 |-conn_read_netlink()
 | |-conn_handle_ports()
 |-conn_handle_line()
   |-conn_handle_ports()

ping
Host    会新建一个hostlist_t对象
SourceAddress   ping_source
Device          ping_device，要求OPING库的版本大于1.3
TTL             ping_ttl，要求0<ttl<255
Interval        ping_interval，采集时间间隔，需要大于0
Size            ping_data，指定报文的大小
Timeout         ping_timeout，超时时间设置
MaxMissed       ping_max_missed

Timeout 不能超过 Interval ，否则会将 Timeout 调整为 0.9 * Interval 。
  
ping_init()
 |-start_thread()
   |-pthread_mutex_lock() 会做条件判断，防止重复创建线程
   |-plugin_thread_create()  创建ping_thread()线程
会有一个线程ping_thread()一直采集数据，

./configure --prefix=/usr
  
https://cmake.org/Wiki/CMake_FAQ
cmake最终打印信息
https://stackoverflow.com/questions/25240105/how-to-print-messages-after-make-done-with-cmake





http://jialeicui.github.io/blog/file_atomic_operations.html

当多个进程通过 write/read 调用同时访问一个文件时，无法保证操作的原子性，因为在两个函数调用间，内核可能会将进程挂起执行另外的进程。

如果想要避免这种情况的话，则需要使用 pread/pwrite 函数。

ssize_t pread(int fd ，void *buffer ，size_t size，off_t offset);
  返回读取到的字节数，offset是指的从文件开始位置起的offset个字节数开始读。
  其定位和读取操作为原子操作，而且读取过后的文件偏移量不会发生改变。

https://github.com/Garik-/ev_httpclient
https://github.com/zhaojh329/evmongoose
https://github.com/dexgeh/webserver-libev-httpparser
https://github.com/bachan/ugh
https://github.com/novator24/libeva
https://github.com/titilambert/packaging-efl
https://github.com/hyperblast/libevhtp
https://github.com/tailhook/libwebsite
https://github.com/cinsk/evhttpserv
https://github.com/erikarn/libevhtp-http
http://beej.us/guide/bgnet/output/html/multipage/recvman.html

LMDB 中所有的读写都是通过事务来执行，其事务具备以下特点：

支持事务嵌套(??)
读写事务可以并发执行，但写写事务需要被串行化
因此，在lmdb实现中，为了保证写事务的串行化执行，事务执行之前首先要获取全局的写锁。

底层读写的实现

lmdb 使用 mmap 访问存储，不管这个存储是在内存上还是在持久存储上，都是将需要访问的文件只读地装载到宿主进程的地址空间，直接访问相应的地址。

减少了硬盘、内核地址控件和用户地址空间之间的拷贝，也简化了平坦的“索引空间”上的实现，因为使用了read-only的mmap，规避了因为宿主程序错误将存储结构写坏的风险。IO的调度由操作系统的页调度机制完成。

而写操作，则是通过write系统调用进行的，这主要是为了利用操作系统的文件系统一致性，避免在被访问的地址上进行同步。

https://hackage.haskell.org/package/lmdb
http://wiki.dreamrunner.org/public_html/C-C++/Library-Notes/LMDB.html
http://www.jianshu.com/p/yzFf8j
http://www.d-kai.me/lmdb%E8%B0%83%E7%A0%94/
https://github.com/exabytes18/mmap-benchmark
https://symas.com/understanding-lmdb-database-file-sizes-and-memory-utilization/
https://github.com/pmwkaa/sophia

http://www.zkt.name/skip-list/

FIXMAP 是什么意思
MDB_FIXEDMAP

MDB_DUPFIXED
MDB_DUPSORT
内存减小时的优化场景
http://www.openldap.org/conf/odd-sandiego-2004/Jonghyuk.pdf
https://symas.com/performance-tradeoffs-in-lmdb/
http://gridmix.blog.51cto.com/4764051/1731411

## 事务

LMDB 每个事务都会分配唯一的事务编号 (txid)，而且会被持久化。

### 事务表

初始化时会创建一张读事务表，该表记录了当前所有的读事务以及读事务的执行者 (持有该事务的进程与线程 id )；读事务表不仅会在内存中维护，同时会将该信息持久化到磁盘上，也就是位与数据库文件相同目录下的 lock.mdb 文件。

事务表的文件存储格式如下图所示：
http://www.d-kai.me/lmdb%E8%B0%83%E7%A0%94/

该事务表会在 LMDB 打开时初始化，也就是在 `mdb_env_setup_locks()` 函数中，其调用流程如下：

mdb_env_open(MDB_env *env, const char *path, unsigned int flags, mdb_mode_t mode) {
    ......
    /* For RDONLY, get lockfile after we know datafile exists */
    if (!(flags & (MDB_RDONLY|MDB_NOLOCK))) {
        rc = mdb_env_setup_locks(env, lpath, mode, &excl);
        if (rc)
             goto leave;
    }
    ......
}

### 页管理

这应该是 lmdb 中最核心的数据结构了，所有的数据和元数据都存储在page内。

#### Meta Page

Meta Page 使用了起始的两个 Page，在第一次创建的时候会初始化页面，并将相同的内容写入到 Page0 和 Page1 中，也就是说开始时两个页的内容一致；写入是通过 `mdb_env_init_meta()` 函数完成。

因为存在两个页，每次写入或者读取时需要选一个页，其计算规则很简单：

meta = env->me_metas[txn->mt_txnid & 1];

也就是用本次操作的事务 id 取最低位，后面解释这么使用的原因。

main()
 |-mdb_env_create()
 |-mdb_env_set_maxreaders()
 |-mdb_env_set_mapsize()
 |-mdb_env_open()
 | |-mdb_fname_init()
 | |-pthread_mutex_init()
 | |-mdb_fopen()
 | |-mdb_env_setup_locks()
 | | |-mdb_fopen()
 | | |-mdb_env_excl_lock()
 | |-mdb_env_open2()
 |   |-mdb_env_read_header()   尝试从头部读取信息，如果是第一次创建，则会调用如下函数
 |   |-mdb_env_init_meta0()  第一次创建
 |   |-mdb_env_init_meta()   第一次创建时同样需要初始化
 |   |-mdb_env_map()  调用mmap进行映射
 |-mdb_txn_begin()  开启事务，允许嵌套
   |-mdb_txn_renew0()
 |-mdb_dbi_open()
 |-mdb_put()
   |-mdb_cursor_put() 最复杂的函数处理
 |-mdb_txn_commit()
 |-mdb_env_stat()
mdb_get

#ifndef LOG_ERR
/* NOTE: please keep consistent with <syslog.h> */
#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* (used) critical conditions, print stack message and quit, nomemory */
#define LOG_ERR         3       /* (used) error conditions */
#define LOG_WARNING     4       /* (used) warning conditions */
#define LOG_NOTICE      5       /* normal but significant condition */
#define LOG_INFO        6       /* (used) informational */
#define LOG_DEBUG       7       /* (used) debug-level messages */
#define LOG_DEBUG0      8       /* (used) print more debug-level messages */
#else
/* Others check the <syslog.h> file. */
#define LOG_DEBUG0      8       /* (used) print more debug-level messages */
#endif
int main(int argc, char *argv[])
{
        char time_str[16]; /* 08-28 12:07:11 */
        time_t tt;
        struct tm local_time;
        time(&tt);
        localtime_r(&tt, &local_time);
        strftime(time_str, sizeof(time_str), "%m-%d %T", &local_time);
        puts(time_str);
        static char level_info[] = {' ', ' ', 'C', 'E', 'W', ' ', 'I', 'D', '0'};
        return 0;
}

FIXME:
/post/linux-monitor-cpu.html
  ps -Lf 60010 查看线程

CMAKE教程，很不错
https://blog.gmem.cc/cmake-study-note


https://github.com/jamesroutley/write-a-hash-table

使用场景：
1. 不判断内存是否申请失败

cJSON *tag = cJSON_CreateObject() 不判断是否为NULL
cJSON_AddStringToObject(tag, "key", "value"); 可能会导致内存泄露。场景tag=NULL时，会通过"value"创建一个对象，当tag为NULL时，则会直接退出，那么通过 "value" 创建的对象将无法释放。

cJSON_AddItemToArray(data, metric = cJSON_CreateObject());
cJSON_AddStringToObject(metric, "mi_n", m->string);

比如 Facebook 的 wdt (https://github.com/facebook/wdt)，Twitter 的 ( https://github.com/lg/murder )，百度的 Ginko 等等，还有包括亚马逊 Apollo 里面的文件分发系统，它们那个和我们的有点不太一样，他们的是基于 S3 做的。
蜻蜓 - P2P文件分发
AOL - 集中配置管理

插件

PING_OPT_QOS:
  通过setsockopt()配置IP_TOS，实际上是设置IP头的Type-of-Service字段，用于描述IP包的优先级和QoS选项，例如IPTOS_LOWDELAY、IPTOS_THROUGHPUT等。

会议内容：
1. 监控需求。通过常驻进程记录上次历史数据，用于计数类型指标统计，目前方案可能会丢失部分监控数据。该问题CloudAgent方的郑力、王一力认可。
2. CloudAgent实现。目前针对日志采集普罗米修斯已经开始开发常驻监控端口，细节尚未讨论，该功能点基本功能满足指标上报的需求。
3. 对接CloudAgent方案。具体细节需要接下来讨论，包括对接方式、保活检测、常驻进程的管理等等。
4. 监控Uagent需求。需要提供动态修改功能，目前来看开发量还比较大；监控插件部分需要做少量修改，之前实现的中间件如haproxy、nginx可以继续使用。

参数修改
PING_OPT_TIMEOUT:
  设置超时时间。
PING_OPT_TTL:
  同样是通过setsockopt()配置IP_TTL。
PING_OPT_SOURCE:
  需要通过getaddrinfo()获取。

ENOTSUP

main()
 |-ping_construct()
 |-ping_setopt()
 |-ping_host_add()
 | |-ping_alloc()
 | |-getaddrinfo() reverse查找
 |-ping_send()
   |-ping_open_socket() 如果还没有打开
   |-select() 执行异步接口
   |-ping_receive_one()
   |-ping_send_one()


https://stackoverflow.com/questions/16010622/reasoning-behind-c-sockets-sockaddr-and-sockaddr-storage?answertab=votes

FIXME:
获取CPU核数
http://blog.csdn.net/turkeyzhou/article/details/5962041
浮点数比较
http://www.cnblogs.com/youxin/p/3306136.html
MemAvailable
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=34e431b0ae398fc54ea69ff85ec700722c9da773

https://stackoverflow.com/questions/1631450/c-regular-expression-howto

Share your terminal over the web
https://github.com/tsl0922/ttyd
http-parser
https://github.com/nodejs/http-parser
picohttpparser
https://github.com/h2o/picohttpparser
为什么会使用http-parser
https://stackoverflow.com/questions/28891806/what-is-http-parser-where-it-is-used-what-does-it-do
http://mendsley.github.io/2012/12/19/tinyhttp.html
https://github.com/tinylcy/tinyhttpd




pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
http://www.cnblogs.com/renxinyuan/p/3875659.html
http://blog-kingshaohua.rhcloud.com/archives/54

设置线程名称
http://blog.csdn.net/jasonchen_gbd/article/details/51308638
https://gxnotes.com/article/78417.html
http://www.cprogramming.com/debugging/segfaults.html
http://cering.github.io/2015/11/10/%E8%BD%AC-%E8%AF%A6%E8%A7%A3coredump/
https://www.ibm.com/developerworks/cn/linux/l-cn-deadlock/
http://blog.jobbole.com/106738/
http://blog.csdn.net/gebushuaidanhenhuai/article/details/73799824
http://www.cnblogs.com/yuuyuu/p/5103744.html
http://blog.csdn.net/sunshixingh/article/details/50988109
https://michaelyou.github.io/2015/03/07/epoll%E7%9A%84%E4%BA%8B%E4%BB%B6%E8%A7%A6%E5%8F%91%E6%96%B9%E5%BC%8F/
http://kimi.it/515.html
https://jeff-linux.gitbooks.io/muduo-/chapter2.html
https://www.zhihu.com/question/20502870
http://www.firefoxbug.com/index.php/archives/1942/
http://cr.yp.to/daemontools.html
http://www.voidcn.com/article/p-vfwivasm-ru.html

如果没有将线程设置为 detached ，而且没有显示的 pthread_exit()，那么在通过 valgrind 进行测试时会发现出现了内存泄露。

在通过 pthread_key_create() 创建私有变量时，只有调用 pthread_exit() 后才会调用上述函数注册的 destructor ；例如主进程实际上不会调用 destructors，此时可以通过 atexit() 注册回调函数。



################################
# Core Dump
################################
http://happyseeker.github.io/kernel/2016/03/04/core-dump-mechanism.html
http://blog.csdn.net/work_msh/article/details/8470277


←





<!--
O_NONBLOCK VS. O_NDELAY
http://blog.csdn.net/ww2000e/article/details/4497349

SO_ACCEPTFILTER
http://blog.csdn.net/hbhhww/article/details/8237309
-->



Address already in use

该错误信息是由于返回了 EADDRINUSE 错误码，通常是由 TCP 套接字的 TIME_WAIT 状态引起，该状态在套接字关闭后会保留约 2~4 分钟，只有在该状态 TIME_WAIT 退出之后，套接字被删除，该地址才能被重新绑定而不出问题。

在 C 中，可以通过如下方式设置端口允许重用。

int opt = 1;
setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

/linux-monitor-cpu.html
ps -ax -o uid,pid,ppid,tpgid,pgrp,session,lstart,cmd

Linux查看某个文件被谁占用
当用户卸载某个目录的时候，因其他用户在当前目录或者当前目录在运行一个程序，卸载时报busy的处理办法：

1：fuser -av /mnt
查看占用mnt的程序以及pid，根据pid去kill -9

2：fuser -km /mnt
查看占用mnt的程序并自动kill
-m是指定被占用的目录，-k是kill

3：losf /mnt
查看正在使用的某个文件
4：递归查看某个目录的文件信息
lsof +D /mnt/fat

5:列出某个用户打开的文件信息

lsof -u student

6：列出某个程序打开的文件信息

lsof -c mysql
http://blog.csdn.net/kozazyh/article/details/5495532


System V IPC 分三类：消息队列、信号量、共享内存区，都采用 `key_t` 作为其内部使用的标示，该类型在 `<sys/types.h>` 中定义，一般是 32 位整数。

通常可以使用 ftok() 函数，也就是 file to key，把一个已存在的路径名和一个整数标识符转换成一个 `key_t` 值，该函数声明如下：

#include <sys/ipc.h>
key_t ftok (const char *pathname, int proj_id);

pathname 通常是跟本应用相关的路径；proj_id 指的是本应用所用到的 IPC 的一个序列号，通常约定好，这样可以获取相同的 `key_t` 值。

注意，需要保证该路径应用程序可以访问，并且在运行期间不能删除。

#include <stdio.h>        
#include <stdlib.h>   
#include <sys/stat.h> 
 
int main() 
{ 
        char filename[50]; 
        struct stat buf; 
        int ret; 
        strcpy(filename, "/home/satellite/" ); 
        ret = stat( filename, &buf ); 
        if(ret) { 
                printf( "stat error\n" ); 
                return -1; 
        } 
 
        printf( "the file info: ftok( filename, 0x27 ) = %x, st_ino = %x, st_dev= %x\n", ftok( filename, 0x27 ), buf.st_ino, buf.st_dev );

        return 0; 
}

通过执行结果可看出，ftok获取的键值是由ftok()函数的第二个参数的后8个bit，st_dev的后两位，st_ino的后四位构成的

### semget

创建一个新的信号量或获取一个已经存在的信号量的键值。

#include <sys/sem.h>
int semget(key_t key, int nsems, int semflg);

key: 为整型值，可以自己设定，有两种场景
   1. IPC_PRIVATE 通常为 0，创建一个仅能被本进程给我的信号量。
   2. 非 0 的值，可以自己手动指定，或者通过 ftok() 函数获取一个唯一的键值。
nsems: 初始化信号量的个数。
semflg: 信号量创建方式或权限，包括了 IPC_CREAT(不存在则创建，存在则获取)；IPC_EXCL(不存在则建立，否则报错)。

#include <stdio.h>
#include <sys/sem.h>

int main()
{
 int semid;
 semid = semget(666, 1, IPC_CREAT | 0666); // 创建了一个权限为666的信号量
 printf("semid=%d\n", semid);
 return 0;
}

可以用 ipcs –s 来查看是否创建成功。
用 ipcrm -s semid 号来删除指定的信号量。



################################
# CMake
################################


针对特定对象，可以通过如下方式指定特定的编译选项、头文件路径、宏定义。

target_compile_definitions(audio_decoder_unittests
 PRIVATE "AUDIO_DECODER_UNITTEST"
 PRIVATE "WEBRTC_CODEC_PCM16")
target_include_directories(audio_decoder_unittests
 PRIVATE "interface"
 PRIVATE "test"
 PRIVATE "../codecs/g711/include")
target_compile_options(RTPencode PRIVATE "/wd4267")


## 配置文件
CheckSymbolExists.cmake   宏定义检查


################################
# Curl
################################

详细可以查看 http://php.net/manual/zh/function.curl-setopt.php
https://moz.com/devblog/high-performance-libcurl-tips/

CURLOPT_NOSIGNAL

CURLOPT_WRITEFUNCTION 用于设置数据读取之后的回调函数，通过该函数可以保存结果，其函数声明如下。
    size_t function( char *ptr, size_t size, size_t nmemb, void *userdata);
CURLOPT_WRITEDATA 定义了上述函数声明中userdata的值。

#include <stdio.h>
#include <curl/curl.h>

size_t save_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int main(void)
{
    CURL *curl;
    CURLcode res;
    FILE *fp;

    fp = fopen("index.html", "wb");

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "www.baidu.com");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, save_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    return 0;
}

CURLOPT_USERNAME
CURLOPT_PASSWORD 分别设置用户名密码，低版本可以通过CURLOPT_USERPWD选项设置，其值为"user:password" 。

CURLOPT_TIMEOUT_MS 设置超时时间。

CURLOPT_MAXREDIRS
CURLOPT_FOLLOWLOCATION 允许重定向，以及设置重定向的跳转次数。


CURLOPT_SSL_VERIFYPEER 验证证书，证书信息可以通过 CURLOPT_CAINFO 设置，或在 CURLOPT_CAPATH 中设置证书目录。

CURLOPT_SSL_VERIFYHOST 设置为 1 是检查服务器SSL证书中是否存在一个公用名(common name)。译者注：公用名(Common Name)一般来讲就是填写你将要申请SSL证书的域名 (domain)或子域名(sub domain)。 设置成 2，会检查公用名是否存在，并且是否与提供的主机名匹配。 0 为不检查名称。 在生产环境中，这个值应该是 2（默认值）。


./configure  \
    --disable-shared --enable-static                     不使用动态库，而是静态编译
    --without-libidn2 --without-winidn                   忽略国际化库
 --disable-ipv6 --disable-unix-sockets                关闭IPV6以及Unix Socket
 --without-ssl --without-gnutls --without-nss         关闭安全配置项1
 --without-libssh2 --disable-tls-srp --without-gssapi 关闭安全配置项2
    --without-zlib                                       不支持压缩
 --disable-ares --disable-threaded-resolver      
 --without-librtmp  --disable-rtsp                    关闭不需要的协议1
 --disable-ldap --disable-ldaps                       关闭不需要的协议2
 --disable-dict --disable-file --disable-gopher
 --disable-ftp --disable-imap --disable-pop3
 --disable-smtp --disable-telnet --disable-tftp
 --disable-sspi                                       Windows选项
 --without-libpsl --without-libmetalink
    --with-nghttp2
 
resolver:         ${curl_res_msg}
Built-in manual:  ${curl_manual_msg}
--libcurl option: ${curl_libcurl_msg}
Verbose errors:   ${curl_verbose_msg}
ca cert bundle:   ${ca}${ca_warning}
ca cert path:     ${capath}${capath_warning}
ca fallback:      ${with_ca_fallback}
HTTP2 support:    ${curl_h2_msg}                                                                               
Protocols:        ${SUPPORT_PROTOCOLS}


curl_code = curl_easy_perform (session);
long http_code = 0;
curl_easy_getinfo(session, CURLINFO_RESPONSE_CODE, &http_code);  /* 获取返回码 */



http://187.0.0.1:8080/status



使用 curl 测量 Web 站点的响应时间。

curl -o /dev/null -s -w '%{http_code}-%{time_namelookup}:%{time_connect}:%{time_appconnect}:%{time_pretransfer}:%{time_redirect}:%{time_starttransfer}:%{time_total}\n' 'http://187.0.0.1:8080/status'

time_namelookup     DNS解析时间，从请求开始到DNS解析完毕所用时间
time_connect     建立到服务器的 TCP 连接所用的时间
time_appconnect     连接建立完成时间，如SSL/SSH等建立连接或者完成三次握手时间
time_pretransfer    准备传输的时间，对于一些协议需要做一些初始化操作
time_redirect       重定向时间，包括到最后一次传输前的几次重定向的DNS解析、连接、预传输、传输时间
time_starttransfer 传输时间，在发出请求之后，服务器返回数据的第一个字节所用的时间
time_total          完成请求所用的时间
speed_download      下载速度，单位是字节/秒
http_code           返回码

注意，如果某一步失败了，该步骤对应的值实际上显示的是 0 ，此时需要通过总时间减去上一步的消耗时间。

上述的执行，是在执行 curl_easy_perform() 函数的时候开始的，在 docs/examples 目录下，有很多的参考实例。


const char *optstr;
char *endptr = NULL;
double v;
long value;

/* NOTE: if optstr = NULL, strtol() will raise 'Segmentation fault' */
optstr = "  not a number"; /* errno=0, optstr == endptr */
errno = 0;
value = strtol(optstr, &endptr, /* base = */ 0);
assert(value == 0);
assert(errno == 0);
assert(optstr == endptr);
printf("errno=%d, optstr=%p, endptr=%p, endchar='%c'/0x%02x, value=%ld\n", errno, optstr, endptr, *endptr, *endptr, value);

optstr = "  12part number";
errno = 0;
value = strtol(optstr, &endptr, /* base = */ 0);
printf("errno=%d, optstr=%p, endptr=%p, endchar='%c'/0x%02x, value=%ld\n", errno, optstr, endptr, *endptr, *endptr, value);

optstr = "  12";
errno = 0;
value = strtol(optstr, &endptr, /* base = */ 0);
printf("errno=%d, optstr=%p, endptr=%p, endchar='%c'/0x%02x, value=%ld\n", errno, optstr, endptr, *endptr, *endptr, value);


memory-barriers
https://www.kernel.org/doc/Documentation/memory-barriers.txt
http://ifeve.com/linux-memory-barriers/
https://dirtysalt.github.io/memory-barrier.html
http://preshing.com/20120625/memory-ordering-at-compile-time/
http://events.linuxfoundation.org/sites/events/files/slides/dbueso-elc2016-membarriers-final.pdf
http://larmbr.com/2014/02/14/the-memory-barriers-in-linux-kernel(1)/
https://www.zhihu.com/question/47990356
http://www.wowotech.net/kernel_synchronization/Why-Memory-Barriers.html
http://blog.csdn.net/qb_2008/article/details/6840570

网卡缓存
https://zrj.me/archives/1102



http://www.cnblogs.com/bodhitree/p/6018369.html
sed高级用法
https://www.zhukun.net/archives/6975
http://gohom.win/2015/06/20/shell-symbol/
mysql core dump
http://xiezhenye.com/2015/05/%E8%8E%B7%E5%8F%96-mysql-%E5%B4%A9%E6%BA%83%E6%97%B6%E7%9A%84-core-file.html
文件句柄数
http://blog.sina.com.cn/s/blog_919f173b01014vol.html
http://www.opstool.com/article/166
rpm 升级到旧的版本
http://ftp.rpm.org/max-rpm/s1-rpm-upgrade-nearly-identical.html#S2-RPM-UPGRADE-OLDPACKAGE-OPTION
https://stackoverflow.com/questions/2452226/master-branch-and-origin-master-have-diverged-how-to-undiverge-branches
C hash算法
http://troydhanson.github.io/uthash/index.html

https://blog.zengrong.net/post/1746.html
https://stackoverflow.com/questions/9537392/git-fetch-remote-branch

http://www.cnblogs.com/yuuyuu/p/5103744.html
https://codeascraft.com/2011/02/15/measure-anything-measure-everything/


http://wkevin.github.io/2014/05/05/git-submodule/
http://xstarcd.github.io/wiki/sysadmin/ntpd.html
ps -ax -o lstart,cmd


可以通过如下命令指定分支，提交后会修改原数据中的 `Subproject commit` 。
cd submodule_directory
git checkout v1.0
cd ..
git add submodule_directory
git commit -m "moved submodule to v1.0"
git push

http://docs.python-requests.org/zh_CN/latest/user/quickstart.html
https://liam0205.me/2016/02/27/The-requests-library-in-Python/

















https://github.com/MarkDickinson/scheduler
https://github.com/Meituan-Dianping/DBProxy
https://github.com/greensky00/avltree
http://www.freebuf.com/sectool/151426.html
http://www.freebuf.com/sectool/150367.html



tar 打包可以通过 --exclude=dir 排除。
通过 `--transform` 参数可以根据 `sed` 语法进行一些转换，例如增加前缀 `'s,^,prefix/,'` 或者 `s%^%prefix/%`。



支持数据类型float, int, str, text, log

支持函数：
    abschange 计算最新值和上次采集值相减的绝对值，对于字符串0/相同、1/不同，1->5=4, 3->1=2

https://www.zabbix.com/documentation/3.0/manual/appendix/triggers/functions


支持数据类型：
   Numeric (unsigned) - 64位无符号整数；
   Numeric (float) - 浮点数，可以存储负值，范围是 [-999999999999.9999, 999999999999.9999]，同时可以支持科学计算 1e+7、1e-4；
   Character - 短文本数据，最大255字节；
  
  
Log - 具有可选日志相关属性的长文本数据(timestamp, source, severity, logeventid)
Text - 长文本数据


Tim O’Reilly and Crew [5, p.726]
The load average tries to measure the number of active processes at any time. As a measure of CPU utilization, the load average is simplistic, poorly defined, but far from useless.

Adrian Cockcroft [6, p. 229]
The load average is the sum of the run queue length and the number of jobs currently running on the CPUs.


默认没有移动平均值计算，只是针对单个值进行计算。

threshold_tree 仍然通过format_name格式化名称。
 
目前分为了三种类型，分别为 Host > Plugin > Type ，需要按照层级进行排列，例如 Host 下面可以有 Plugin 和 Type 段；Plugin 下可以有 Type 但是不能有 Host 。

其它的配置项用于一些类似阈值的判断等，只能在 Type 下面配置。

FailureMax Value
WarningMax Value
 设置报警的上限值，如果没有配置则是正无穷。告警发送规则如下：
    A) (FailureMax, +infty) 发送 FAILURE 通知；
    B) (WarningMax, FailureMax] 发送 WARNING 通知；

FailureMin Value
WarningMin Value
 设置报警的下限值，如果没有配置则是负无穷。告警发送规则如下：
    A) (-infty, FailureMin) 发送 FAILURE 通知；
    B) [FailureMin, WarningMin) 发送 WARNING 通知；

Persist true|false(default)
 多久发送一次报警，设置规则如下：
    true) 每次超过阈值之后都会发送一次报警通知；
    false) 只有在状态发生转换且前一次状态是OKAY时才会发送一次通知。

PersistOK true|false(default)
 定义如何发送OKAY通知，设置规则如下：
    true) 每次在正常范围内都会发送通知；
    false) 当本次状态正常而且之前状态不正常时才发送一次OK通知。

Hysteresis Value
 迟滞作用，用于处理在一个状态内重复变换，在该阈值范围内不会告警。

Hits Value
    告警条件必须连续满足多少次之后才会发送告警。

Interesting true(default)|false
 发现数据未更新时是否发送告警，会根据插件的采集时间间隔以及 Timeout 参数判断是否有事件发生。
    true) 发送FAILURE报警；
    false) 忽略该事件。

DataSource  <-> Types.db 中字段，例如midterm
Host <-> host
Plugin <-> plugin
Type <-> type
Instance <-> type_instance

1. 根据value list中的参数获取到具体的配置。
2.

Invert true|false
Percentage true|false

以 loadavg 为例，其实现在 fs/proc/loadavg.c 中。
calc_global_load()
监控指标，其中监控插件包括了

https://en.wikipedia.org/wiki/Moving_average
http://www.perfdynamics.com/CMG/CMGslides4up.pdf
https://zh.wikipedia.org/wiki/%E7%A7%BB%E5%8B%95%E5%B9%B3%E5%9D%87

移动平均 (Moving Average) 可以处理短期波动，反映长期趋势或周期，从数学上看做是卷积。


## 简单移动平均

Simple Moving Average, SMA 将变量的前 N 值做平均。

SMA = (V1 + V2 + ... + Vn) / n

当有新值之后，无需重复计算，只需要将最老的旧值删除，然后加入新值。

SMAn = SMAn-1 - V1/n + V/n

这样需要保存 N 个值。

## 加权移动平均

Weighted Moving Average, WMA 也就是在计算平均值时将部分数据乘以不同数值，

## 指数移动平均

Exponential Moving Average, EMA


----- 锁定用户该用户不能再次登录
ALTER USER username ACCOUNT LOCK;
----- 解锁用户
ALTER USER username ACCOUNT UNLOCK;

aussdb plugin: Connect to database failed: FATAL:  The account has been locked.
FATAL:  The account has been locked.

Zabbix上报数据格式
http://www.ttlsa.com/zabbix/zabbix-active-and-passive-checks/
Open-falcon 上报数据
http://blog.niean.name/2015/08/06/falcon-intro
main_timer_loop() 周期计算定义的触发值，如果有事件发生，那么就直接写入到数据库中。


timer_thread()     main_timer_loop时间相关的处理
 |-process_time_functions()
 | |-DCconfig_get_time_based_triggers() 从缓存中获取trigger表达式
 | |-evaluate_expressions()       触发器表达式的主要处理函数，同时会产生事件
 | | |-substitute_simple_macros() 宏分为两类，分别是{...} {$...}
 | | |-substitute_functions()
 | | | |-zbx_evaluate_item_functions()
 | | |   |-evaluate_function()
 | | |-evaluate()
 | |
 | |-DBbegin()
 | |-process_triggers() T:triggers
 | | |-process_trigger()
 | |   |-add_event()              会保存到内存的events数组中
 | |-process_events()             处理事件，主要是将新事件插入数据库
 | | |-save_events() T:events
 | | |-process_actions() T:actions
 | | |-clean_events()
 | |-DBcommit()
 |
 |-process_maintenance()


源码解析
https://jackywu.github.io/articles/zabbix_server%E6%BA%90%E7%A0%81%E5%88%86%E6%9E%90/

## 表结构

{<server>:<key>.<function>(<parameter>)}<operator><constant>

hosts  包含了主机以及模板信息。
    select hostid, host from hosts where status = 0;
    hostid  主机ID
 host    主机、模板名
 status  0->主机 3->模板
goups   主机的逻辑分组
hosts_grous 分组和主机之间的关联关系
items 监控项，保存了每个主机的监控项，该监控项来自的模板ID
    select itemid, name, key_, templateid, delay, status, units from items where hostid = 10107;
triggers 触发器，其中表达式中使用的是function id
 select triggerid, expression, status, value, description from triggers;
 select * from functions where functionid = 13302;
functions

Paxos协议解析
https://zhuanlan.zhihu.com/p/21438357?refer=lynncui

https://github.com/hanc00l/wooyun_public
https://github.com/niezhiyang/open_source_team
http://www.jianshu.com/p/43c604177c08
http://kenwheeler.github.io/slick/

http://lovestblog.cn/blog/2016/07/20/jstat/
http://blog.phpdr.net/java-visualvm%E8%AE%BE%E7%BD%AEjstat%E5%92%8Cjmx.html


http://metrics20.org/spec/
Stack Overflow 的架构
https://zhuanlan.zhihu.com/p/22353191
GCC部分文件取消告警
http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
http://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
http://gcc.gnu.org/onlinedocs/gcc-4.0.4/gcc/Warning-Options.html
-->

{% highlight text %}
{% endhighlight %}
