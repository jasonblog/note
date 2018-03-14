---
title: Hello World !!!
layout: post
comments: true
language: chinese
usemath: true
category: [misc]
keywords: hello world,示例,sample,markdown
description: 簡單記錄一下一些與 Markdown 相關的內容，包括了一些使用模版。
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

靜思

<div id="colorbox">
    <span style="width:125.2px;background-color: #81C2D6" alt="81C2D6">HEX<br>#81C2D6<br>RGB<br>129.194.214<br>CMYK<br>10.9.0.16</span>
    <span style="width:125.2px;background-color: #8192D6" alt="8192D6">HEX<br>#8192D6<br>RGB<br>129.146.214<br>CMYK<br>40.32.0.16</span>
    <span style="width:125.2px;background-color: #D9B3E6" alt="D9B3E6">HEX<br>#D9B3E6<br>RGB<br>217.179.230<br>CMYK<br>6.22.0.10</span>
    <span style="width:125.2px;background-color: #DCF7A1" alt="DCF7A1">HEX<br>#DCF7A1<br>RGB<br>220.247.161<br>CMYK<br>11.0.35.3</span>
    <span style="width:125.2px;background-color: #83FCD8" alt="83FCD8">HEX<br>#83FCD8<br>RGB<br>131.152.216<br>CMYK<br>48.0.14.1</span>
</div>
<div class="clearfix"></div>

## Heading 2，目錄 2

### Heading 3， 目錄 3

#### Heading 4，目錄 4

##### Heading 5，目錄 5

###### Heading 6， 目錄 6


## MISC

### Separator，分割線

下面的效果是相同的。

* * *

***

*****

- - -

---------------------------------------




### The Fonts， 字體設置

*This is emphasized 斜體*.   _This is also emphasized 還是斜體_.

**Strong font 粗體** __Also strong font 還是粗體__

Water is H<sub>2</sub>O. 4<sup>2</sup>=16. 上標、下標測試。

Code Use the `printf()` function，代碼模塊。

Code Use the <code>printf()</code> function，與上面功能一樣。

``There is a literal backtick (`) here.``，額，還是代碼模塊。

The New York Times <cite>(That’s a citation)</cite>. 引用測試，和斜體相似。

This is <u>Underline</u>. 下劃線。


### Code Snippets，代碼高亮顯示

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

下面時關於段引用的測試。

單段的引用。Just one paragraph.

> My conclusion after the trip was "well, now I know that there's at least one company in the world that can succeed with the waterfall model" and I decided to stop bashing the waterfall model as hard as I usually do. Now, however, with all the problems Toyota are having, I'm starting to reconsider.q q q q q q q q q q q q q q q<from>kkkkk</from>

多段的引用，one more paragraphs.


> My conclusion after the trip was "well, now I know that there's at least one company in the world that can succeed with the waterfall model" and I decided to stop bashing the waterfall model as hard as I usually do. Now, however, with all the problems Toyota are having, I'm starting to reconsider.
>
> My conclusion after the trip was "well, now I know that there's at least one company in the world that can succeed with the waterfall model" and I decided to stop bashing the waterfall model as hard as I usually do. Now, however, with all the problems Toyota are having, I'm starting to .q q q q q

單段，但較為複雜的引用。

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







### Unordered Lists，無序列表

如下是三種不同的表達方式。

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

如下的各個列表項中，各個項之間表示為段落，而之前的不是，也就是說添加了 ```<p></p>``` ，所以現在看起來各個段之間空隙有點大。

- Item one

- Item two

- Item three

### Ordered Lists，有序列表

有序表的表達方式，只與順序相關，而與列表前的數字無關。

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

### Lists Tips，列表補記

列表項目標記通常是放在最左邊，但是其實也可以縮進，最多 3 個空格，項目標記後面則一定要接著至少一個空格或製表符。

*   Lorem ipsum dolor sit amet, consectetuer adipiscing elit.
    Aliquam hendrerit mi posuere lectus. Vestibulum enim wisi,
    viverra nec, fringilla in, laoreet vitae, risus.
*   Donec sit amet nisl. Aliquam semper ipsum sit amet velit.
    Suspendisse id sem consectetuer libero luctus adipiscing.

如下顯示相同。

*   Lorem ipsum dolor sit amet, consectetuer adipiscing elit.
Aliquam hendrerit mi posuere lectus. Vestibulum enim wisi,
viverra nec, fringilla in, laoreet vitae, risus.
*   Donec sit amet nisl. Aliquam semper ipsum sit amet velit.
Suspendisse id sem consectetuer libero luctus adipiscing.

如下是在同一列表中，顯示兩個段落。

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

kramdown 默認支持表格，只需要設置好 ```table thead tbody th tr td``` 對應的屬性即可。

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

### MathJax, 數學表達式

如下是一個數學表達式。

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

段內插入LaTeX代碼是這樣的：$\exp(-\frac{x^2}{2})$，試試看看吧

### Pictures，圖片顯示

![If the picture doesnt exist]({{ site.url }}/images/linux-liberty.png "an example picture"){: .pull-left}

<div class="clearfix"></div>

![aaaaa]{: .pull-right width="20%"}

<div class="clearfix"></div>

[aaaaa]:    /images/linux-liberty.png    "MSN Search"


### Reference，引用

如下是一個簡單的鏈接 [an example](http://example.com/ "Title")，當然也可以使用網站的相對路徑 [About Me](/about/) 。

也可以將網站的引用與 URL 分別區分開，如下是其中的示例，而且不區分大小寫，[RTEMS] [1]、[Linux] [2]、[GUN][GUN]、[GUN][gun] 。

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


用pt-table-checksum校驗數據一致性
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

???支持基於所有數據庫、指定數據庫、指定表的複製。

默認使用基於GTID的異步複製，同時支持半同步複製 (semi-synchronous)，基於 NDB 的同步複製以及延遲複製 (Delayed Replication)




### NC
socat加強版
http://brieflyx.me/2015/linux-tools/socat-introduction/


### GCC
http://www.jianshu.com/p/dd425b9dc9db
http://filwmm1314.blog.163.com/blog/static/218259192012121225132/
http://www.cnblogs.com/respawn/archive/2012/07/09/2582078.html

### TSD

在多線程程序中，所有線程共享程序中的變量，如果每個線程需要保存獨自的數據，例如每個線程維護一個鏈表，但是通過相同的函數處理，這就是 Thread Specific Data 的作用。如下介紹 TSD 的使用方法：

1. 聲明一個 pthread_key_t 類型的全局變量；
2. 通過 pthread_key_create() 函數創建 TSD，實際就是分配一個實例，並將其賦值給 pthread_key_t 變量，所有的線程都可以通過該變量訪問，這就相當於提供了同名而不同值的全局變量；
3. 調用  pthread_setspcific()、pthread_getspecific() 存儲或者獲取各個線程特有的值；

TSD的實現詳見： https://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part2/

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);



### innodb_locks_unsafe_for_binlog 參數相關
另外設置 innodb_locks_unsafe_for_binlog=1 ,binlog也要設為row格式。
https://dev.mysql.com/doc/refman/5.7/en/innodb-parameters.html#sysvar_innodb_locks_unsafe_for_binlog
innodb默認使用了next-gap算法，這種算法結合了index-row鎖和gap鎖。正因為這樣的鎖算法，innodb在可重複讀這樣的默認隔離級別上，可以避免幻象的產生。innodb_locks_unsafe_for_binlog最主要的作用就是控制innodb是否對gap加鎖。
注意該參數如果是enable的，則是unsafe的，此時gap不會加鎖；反之，如果disable掉該參數，則gap會加鎖。當然對於一些和數據完整性相關的定義，如外鍵和唯一索引（含主鍵）需要對gap進行加鎖，那麼innodb_locks_unsafe_for_binlog的設置並不會影響gap是否加鎖。
在5.1.15的時候，innodb引入了一個概念叫做“semi-consistent”，這樣會在innodb_locks_unsafe_for_binlog的狀態為ennable時在一定程度上提高update併發性。
https://yq.aliyun.com/articles/62372



### FLUSH TABLES WITH READ LOCK
FLUSH TABLES WITH READ LOCK命令的作用是關閉所有打開的表，會將所有髒頁刷新到磁盤，同時對所有數據庫中的表都加一個全局讀鎖，直到顯示執行UNLOCK TABLES，才釋放持有的讀鎖。

該操作常用於獲取一致性數據備份。

http://aklaus.blog.51cto.com/9724632/1656991
http://blog.csdn.net/zbszhangbosen/article/details/7434181
http://www.cnblogs.com/cchust/p/4603599.html
http://www.cnblogs.com/sunss/archive/2012/02/02/2335960.html
http://www.cnblogs.com/zhenghongxin/p/5527527.html
http://myrock.github.io/2014/11/20/mysql-waiting-for-table-flush/
http://blog.csdn.net/arkblue/article/details/27376991
https://www.percona.com/blog/2012/03/23/how-flush-tables-with-read-lock-works-with-innodb-tables/
https://www.percona.com/blog/2010/04/24/how-fast-is-flush-tables-with-read-lock/

加鎖過程是什麼樣的，為什麼會阻塞現有的SELECT查詢？

http://www.penglixun.com/tech/database/the_process_of_mysqldump.html
http://www.cnblogs.com/digdeep/p/4947694.html
http://www.imysql.com/2008_10_24_deep_into_mysqldump_options
http://www.cnblogs.com/zhoujinyi/p/5789465.html
https://yq.aliyun.com/articles/59326


#### sql_slave_skip_counter=N
http://dinglin.iteye.com/blog/1236330
http://xstarcd.github.io/wiki/MySQL/MySQL_replicate_error.html

#### FOR UPDATE/
SELECT ... FOR UPDATE 用於對查詢的數據添加IX鎖(意向排它鎖)，此時，其它會話也就無法在這些記錄上添加任何的S鎖或X鎖，通常使用的場景是為了防止在低隔離級別下出現幻讀現象，用於保證 “本事務看到的是數據庫存儲的最新數據，並且看到的數據只能由本事務修改”。

InnoDB默認使用快照讀，使用 FOR UPDATE 之後不會阻塞其它會話的快照讀，當然會阻塞lock in share mode和for update這種顯示加鎖的查詢操作。

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


SELECT ... LOCK IN SHARE MODE對於查詢的數據添加IS(意向共享鎖)，此時，其它會話可以讀取這些記錄，也可以繼續添加IS鎖，但是無法修改這些記錄直到該事務執行完成。

通常用於兩張存在關聯關係表的寫操作，拿mysql官方文檔的例子來說，一個表是child表，一個是parent表，假設child表的某一列child_id映射到parent表的c_child_id列，那麼從業務角度講，此時我直接insert一條child_id=100記錄到child表是存在風險的，因為剛insert的時候可能在parent表裡刪除了這條c_child_id=100的記錄，那麼業務數據就存在不一致的風險。正確的方法是再插入時執行select * from parent where c_child_id=100 lock in share mode,鎖定了parent表的這條記錄，然後執行insert into child(child_id) values (100)就ok了。

http://blog.csdn.net/cug_jiang126com/article/details/50544728
http://chenzhou123520.iteye.com/blog/1860954
https://www.percona.com/blog/2006/08/06/select-lock-in-share-mode-and-for-update/


#### 複製過濾可能產生的異常

http://keithlan.github.io/2015/11/02/mysql_replicate_rule/

#### BLOG和TEXT區別
BLOB和TEXT分別用於保存二進制和非二進制字符串，保存數據可變，都可以分為四類：TINYBLOB、BLOB、MEDIUMBLOB 和 LONGBLOB ，TEXT與之類似，只是其容納長度有所不同；兩者區別如下：

* BLOB為二進制字符串，TEXT為非二進制字符串；
* BLOG列沒有字符集，並且排序和比較基於列值字節的數值；TEXT列有字符集，並且根據字符集的規則進行排序和比較；
* 兩種類型的DML不存在大小寫轉換，在非嚴格模式下超出最大長度時會截斷併產生告警；嚴格模式則會報錯；

兩者可以分別視為VARCHAR和VARBINARY，但是有如下區別：
* BLOB和TEXT列不能有默認值；

當保存或檢索BLOB和TEXT列的值時不刪除尾部空格。(這與VARBINARY和VARCHAR列相同）.

對於BLOB和TEXT列的索引，必須指定索引前綴的長度。對於CHAR和VARCHAR，前綴長度是可選的.

LONG和LONG VARCHAR對應MEDIUMTEXT數據類型。這是為了保證兼容性。如果TEXT列類型使用BINARY屬性，將為列分配列字符集的二元校對規則.

MySQL連接程序/ODBC將BLOB值定義為LONGVARBINARY，將MySQL TEXT值定義為LONGVARCHAR。由於BLOB和TEXT值可能會非常長，使用它們時可能遇到一些約束.

BLOB或TEXT對象的最大大小由其類型確定，但在客戶端和服務器之間實際可以傳遞的最大值由可用內存數量和通信緩存區大小確定。你可以通過更改max_allowed_packet變量的值更改消息緩存區的大小，但必須同時修改服務器和客戶端程序。例如，可以使用 MySQL和MySQLdump來更改客戶端的max_allowed_packet值.


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
循環複製
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

sql_cache意思是說，查詢的時候使用緩存。

sql_no_cache意思是查詢的時候不適用緩存。

sql_buffer_result意思是說，在查詢語句中，將查詢結果緩存到臨時表中。

這三者正好配套使用。sql_buffer_result將盡快釋放表鎖，這樣其他sql就能夠儘快執行。

使用 FLUSH QUERY CACHE 命令，你可以整理查詢緩存，以更好的利用它的內存。這個命令不會從緩存中移除任何查詢。FLUSH TABLES 會轉儲清除查詢緩存。
RESET QUERY CACHE 使命從查詢緩存中移除所有的查詢結果。

--------------------------------------------------

那麼MySQL到底是怎麼決定到底要不要把查詢結果放到查詢緩存中呢？

是根據query_cache_type這個變量來決定的。

這個變量有三個取值：0,1,2，分別代表了off、on、demand。

意思是說，如果是0，那麼query cache 是關閉的。如果是1，那麼查詢總是先到查詢緩存中查找，除非使用了sql_no_cache。如果是2，那麼，只有使用 sql_cache的查詢，才會去查詢緩存中查找。


###
ysql [(none)]>grant super on db1.* to 'dtstack'@'%';
ERROR 1221 (HY000): Incorrect usage of DB GRANT and GLOBAL PRIVILEGES        --因為super權限是管理級別的權限(super ,process,file)，不能夠指定某個數據庫on 後面必須跟*.*

正確用法：
mysql [(none)]>grant super on *.* to 'dtstack'@'%';
Query OK, 0 rows affected (0.01 sec)
釋：
1.super權限可以使用change master to 語句
2.kill ， mysqladmin kill  kill_threads
3.purge binary logs
    EG：
        wjj@(www.dtstack.com) [(none)]>show binary logs;
          mysql [(none)]>purge binary logs to 'mysql-bin.000005';  --解釋：刪除MySQLbinlog日誌，刪除mysql-bin.000005之前的，不包括他本身
          Query OK, 0 rows affected (0.03 sec
          mysql [(none)]>show binary logs;
       
4.可以設置全局參數模式set global xx
5.mysqladmin debug  啟動或禁用日誌記錄
6.有權限設置relad_only
7.連接數達到max_connections上限時無法創建連接時，擁有super權限的用戶可以創建一個連接

網絡設置，包括瞭如何設置服務端+客戶端、廣播、多播等。
https://collectd.org/wiki/index.php/Networking_introduction
寫入RRD文件，包括了Collectd對RRD的優化，以及其中一篇RRD優化介紹的文章
http://oss.oetiker.ch/rrdtool-trac/wiki/TuningRRD
https://collectd.org/wiki/index.php/Inside_the_RRDtool_plugin
驚群問題討論
http://www.voidcn.com/blog/liujiyong7/article/p-377809.html
linux時間相關結構體和函數整理
http://www.cnblogs.com/zhiranok/archive/2012/01/15/linux_c_time_struct.html
Heap數據結構(棧)
http://www.cnblogs.com/gaochundong/p/binary_heap.html
http://www.cnblogs.com/skywang12345/p/3610187.html


AVL數
https://courses.cs.washington.edu/courses/cse373/06sp/handouts/lecture12.pdf
https://www.cise.ufl.edu/~nemo/cop3530/AVL-Tree-Rotations.pdf
http://www.cnblogs.com/zhoujinyi/p/6497231.html

https://dev.mysql.com/doc/refman/5.7/en/backup-policy.html
https://dev.mysql.com/doc/refman/5.7/en/point-in-time-recovery.html

https://www.unixhot.com/page/ops




對於GROUP BY、ORDER BY、DISTINCT類似的SQL，可能會用到臨時表，通常在內存中使用MEMORY存儲引擎，或者磁盤上使用MyISAM存儲引擎。一般，當超過了tmp_table_size大小之後，會在磁盤上創建，另外，對於
https://www.percona.com/blog/2007/08/16/how-much-overhead-is-caused-by-on-disk-temporary-tables/
https://www.percona.com/blog/2007/08/18/how-fast-can-you-sort-data-with-mysql/





http://halobates.de/memorywaste.pdf
BuildBot


非常經典的《Linux平臺下的漏洞分析入門 》
https://github.com/1u4nx/Exploit-Exercises-Nebula
原文在這裡
https://www.mattandreko.com/

http://hustcat.github.io/iostats/
http://ykrocku.github.io/blog/2014/04/11/diskstats/
http://www.udpwork.com/item/12931.html

FIXME:
  linux-monitor-io.html
/proc/diskstats 中包括了主設備號、次設備號和設備名稱，剩餘的各個字段的含義簡單列舉如下，詳細可以查看內核文檔 [I/O statistics fields](https://www.kernel.org/doc/Documentation/iostats.txt) 。

可以通過 grep diskstats 找到對應內核源碼實現在 diskstats_show()@block/genhd.c 中。

獲取源碼 diskstats_show() + struct disk_stats 。

可以看到是通過 part_round_stats() 函數獲取每個磁盤的最新統計信息，通過 struct hd_struct 中的 struct disk_stats *dkstats 結構體保存，然後利用 part_stat_read() 函數統計各個 CPU 的值 (如果是多核)。


在 Bash 編程時，經常需要切換目錄，可以通過 pushd、popd、dirs 命令切換目錄。

pushd  切換到參數指定的目錄，並把原目錄和當前目錄壓入到一個虛擬的堆棧中，不加參數則在最近兩個目錄間切換；
popd   彈出堆棧中最近的目錄；
dirs   列出當前堆棧中保存的目錄列表；
  -v 在目錄前添加編號，每行顯示一個目錄；
  -c 清空棧；

切換目錄時，會將上次目錄保存在 $OLDPWD 變量中，與 "-" 相同，可以通過 cd - 切換回上次的目錄。



set -o history 開啟bash的歷史功能

判斷目錄是否存在，如果目錄名中有'-'則需要進行轉義。

dir="/tmp/0\-test"
if [ ! -d "${dir}" ]; then
  mkdir /myfolder
fi


c_avl_tree_t *c_avl_create(int (*compare)(const void *, const void *));
入參：
  比較函數，類似strcmp()；
實現：
  1. 保證 compare 有效，非 NULL；
  2. 申請內存，部分結構體初始化。
返回：
  成功返回結構體指針；參數異常或者沒有內存，返回 NULL；

int c_avl_insert(c_avl_tree_t *t, void *key, void *value);
返回：
  -1：內存不足；
  0： 節點寫入正常；
  1:  節點已經存在；

int c_avl_get(c_avl_tree_t *t, const void *key, void **value);
調用者保證 t 存在 (非NULL)。
返回：
  -1：對象不存在；
  0： 查找成功，對應的值保存在value中；

int c_avl_remove(c_avl_tree_t *t, const void *key, void **rkey, void **rvalue);
返回：
  -1：對象不存在；


_remove()
search()
rebalance()
verify_tree()


安全滲透所需的工具
https://wizardforcel.gitbooks.io/daxueba-kali-linux-tutorial/content/2.html






1/0 = &infin;
log (0) = -&infin;
sqrt (-1) = NaN

infinity (無窮)、NaN (非數值)

除了 infin 自身和 NaN 之外，infin (正無窮) 大於任何值；而 NaN 不等於任何值，包括其自身，而且與 <, >, <=, >= 使用時將會報錯。







如果有 group 採用相同的 group-id，也就是有 alias group ，那麼在刪除某個

groupdel: cannot remove the primary group of user 'monitor'

----- 將原用戶修改為其它分組
# usermod -g sys monitor
----- 刪除分組
# groupdel test
----- 修改回來
# usermod -g monitor monitor












http://fengyuzaitu.blog.51cto.com/5218690/1616268
http://www.runoob.com/python/os-statvfs.html
http://blog.csdn.net/papiping/article/details/6980573
http://blog.csdn.net/hepeng597/article/details/8925506






jinyang ALL=(root) NOPASSWD: ALL






shell版本號比較
http://blog.topspeedsnail.com/archives/3999
https://www.netfilter.org/documentation/HOWTO/NAT-HOWTO-6.html
man 3 yum.conf 確認下YUM配置文件中的變量信息
https://unix.stackexchange.com/questions/19701/yum-how-can-i-view-variables-like-releasever-basearch-yum0


FORTIFY.Key_Management--Hardcoded_Encryption_Key    strcasecmp("Interval", child->key)

int lt_dlinit (void);
  初始化，在使用前調用，可以多次調用，正常返回 0 ；
const char * lt_dlerror (void);
  返回最近一次可讀的錯誤原因，如果沒有錯誤返回 NULL；
void * lt_dlsym (lt_dlhandle handle, const char *name);
  返回指向 name 模塊的指針，如果沒有找到則返回 NULL 。
lt_dlhandle lt_dlopen (const char *filename);
  加載失敗返回 NULL，多次加載會返回相同的值；
int lt_dlclose (lt_dlhandle handle);
  模塊的應用次數減一，當減到 0 時會自動卸載；成功返回 0 。

https://github.com/carpedm20/awesome-hacking
http://jamyy.us.to/blog/2014/01/5800.html




Page Cache
https://www.thomas-krenn.com/en/wiki/Linux_Page_Cache_Basics

Page Cache, the Affair Between Memory and Files

http://duartes.org/gustavo/blog/post/page-cache-the-affair-between-memory-and-files/

https://www.quora.com/What-is-the-major-difference-between-the-buffer-cache-and-the-page-cache

從free到page cache
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
  從源碼生成可執行文件一般需要四個步驟，並且還會產生中間文件，該參數用於配置實用PIPE，一些平臺會失敗，不過 GNU 不受影響。
-fexceptions
  打開異常處理，該選項會生成必要的代碼來處理異常的拋出和捕獲，對於 C++ 等會觸發異常的語言來說，默認都會指定該選項。所生成的代碼不會造成性能損失，但會造成尺寸上的損失。因此，如果想要編譯不使用異常的 C++ 代碼，可能需要指定選項 -fno-exceptions 。
-Wall -Werror -O2 -g --param=ssp-buffer-size=4 -grecord-gcc-switches -specs=/usr/lib/rpm/redhat/redhat-hardened-cc1  -m64 -mtune=generic -DLT_LAZY_OR_NOW="RTLD_LAZY|RTLD_GLOBAL"




關於Linux內核很不錯的介紹
http://duartes.org/gustavo/blog/






編程時常有 Client 和 Server 需要各自得到對方 IP 和 Port 的需求，此時就可以通過 getsockname() 和 getpeername() 獲取。

python -c '
import sys
import socket
s=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("www.huawei.com", 80))
print s.getsockname()
s.close()'


Python判斷IP有效性
https://gist.github.com/youngsterxyf/5088954



安全滲透工具集
https://wizardforcel.gitbooks.io/daxueba-kali-linux-tutorial/content/2.html

hostname獲取方式，在啟動時通過 1) global_option_get() 配置文件獲取；2) gethostname()；3) getaddrinfo()。

#include <unistd.h>
int gethostname(char *name, size_t len);
  返回本地主機的標準主機名；正常返回 0 否則返回 -1，錯誤碼保存在 errno 中。

#include <netdb.h>
#include <sys/socket.h>
struct hostent *gethostbyname(const char *name);
  用域名或主機名獲取IP地址，注意只支持IPv4；正常返回一個 struct hostent 結構，否則返回 NULL。

#include<netdb.h>
int getaddrinfo(const char *hostname, const char *service, const struct addrinfo *hints, struct addrinfo **result);
  hostname: 一個主機名或者地址串，IPv4的點分十進制串或者IPv6的16進制串；
  service : 服務名可以是十進制的端口號，也可以是已定義的服務名稱，如ftp、http等；
  hints   : 可以為空，用於指定返回的類型信息，例如，服務支持 TCP/UDP 那麼，可以設置 ai_socktype 為 SOCK_DGRAM 只返回 UDP 信息；
  result  : 返回的結果。
  返回 0 成功。

struct addrinfo {
    int              ai_flags;
    int              ai_family;
    int              ai_socktype;
    int              ai_protocol;
    socklen_t        ai_addrlen;
    struct sockaddr *ai_addr;        // IP地址，需要通過inet_ntop()轉換為IP字符串
    char            *ai_canonname;   // 返回的主機名
    struct addrinfo *ai_next;
};
http://blog.csdn.net/a_ran/article/details/41871437


const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt);
  將類型為af的網絡地址結構src，轉換成主機序的字符串形式，存放在長度為cnt的字符串中。返回指向dst的一個指針。如果函數調用錯誤，返回值是NULL。



對於不信任的組件建議使用後者，因為 ldd 可能會加載後顯示依賴的庫，從而導致安全問題。

----- 查看依賴的庫
$ ldd /usr/bin/ssh
$ objdump -p /usr/bin/ssh | grep NEEDED

----- 運行程序加載的庫
# pldd $(pidof mysqld)


# pldd $(pidof uagent)


VIRT, Virtual Memory Size @
  該任務的總的虛擬內存，包括了 code、data、shared libraries、換出到磁盤的頁、已經映射但是沒有使用的頁。
USED, Memory in Use @
  包括了已使用的物理內存 RES ，以及換出到磁盤的內存 SWAP。
%MEM, Memory Usage(RES) @
  當前任務使用的內存與整個物理內存的佔比。
CODE, Code Size
  可執行代碼佔用的物理內存數，也被稱為 Text Resident Set, TRS。
DATA, Data+Stack Size
  除了代碼之外的物理內存佔用數，也就是 Data Resident Set, DRS 。
RES, Resident Memory Size @
  駐留在物理內存中的使用量。
SHR, Shared Memory Size @
  包括了共享內存以及共享庫的數據。

SWAP, Swapped Size
  換出到磁盤的內存。
nMaj, nMin, nDRT


RES = CODE + DATA???? DATA太大了，為什麼

====== ps
DRS, Data Resident Set <=> top(DATA) !!!
  除了代碼之外的物理內存佔用數。
RSS, Resident Set Size <=> top(RES)
  物理內存使用數。
TRS, Text Resident Set <=> top(CODE) !!!
  代碼在內存中的佔用數。
VSZ, Virtual Memory Size <=> top(VIRT) <=> pmap -d(mapped)
  虛擬內存的大小。

RES(top) 和 RSS(ps) 實際上讀取的是 /proc/$(pidof process)/stat 或者 /proc/$(pidof process)/status statm。
pmap -d $(pidof uagent)
pmap -x $(pidof uagent)
ps -o pid,pmem,drs,trs,rss,vsz Hp `pidof uagent`

另外，cgtop 中顯示的內存與什麼相關？？？？
ps(TRS) 和 top(CODE) 的值不相同。

http://blog.csdn.net/u011547375/article/details/9851455
https://stackoverflow.com/questions/7594548/res-code-data-in-the-output-information-of-the-top-command-why
https://static.googleusercontent.com/media/research.google.com/zh-CN//pubs/archive/36669.pdf
https://landley.net/kdocs/ols/2010/ols2010-pages-245-254.pdf
LDD 跟 PMAP 加載的庫不同？

awk 'BEGIN{sum=0};{if($3~/x/) {sum+=$2}};END{print sum}' /tmp/1

procs_refresh

Top用於查看Linux系統下進程信息，有時候需要選擇顯示那些列，以及按照某一列進行排序。查詢整理如下：


top 除了默認的列之外，可以選擇需要顯示的列，操作如下：

----- 選擇需要顯示的列
1) 按 f 鍵進入選擇界面；2) 方向鍵選擇需要的列；3) 通過空格選擇需要顯示的列。

列顯示位置調整：
執行top命令後，按 o 鍵，選擇要調整位置的列（如K:CUP Usageage），按動一下大寫K則顯示位置往上調整，按動一下小寫K則顯示位置往下調整。

列排序：
執行top命令後，按 shift + f（小寫），進入選擇排序列頁面，再按要排序的列的代表字母即可；

systemctl set-property --runtime uagent.service CPUQuota=5% MemoryLimit=30M

關於資源配置的選項可以通過 ```man 5 systemd.resource-control``` 方式查看，默認是沒有開啟審計的，所以通過 ```systemd-cgtop``` 沒有顯示具體的資源。

很多相關的內核文檔鏈接
https://www.freedesktop.org/software/systemd/man/systemd.resource-control.html

註冊信號處理函數

setsid()
pidfile_create()

https://www.ibm.com/support/knowledgecenter/zh/ssw_aix_61/com.ibm.aix.genprogc/ie_prog_4lex_yacc.htm

flex 通過 yylval 將數據傳遞給 yacc；如果在 yacc 中使用了 ```%union``` ，那麼各個條件的目的變量使用 yyval 。

https://www.howtoforge.com/storing-files-directories-in-memory-with-tmpfs



meminfo詳解
https://lwn.net/Articles/28345/

ps的SIZE以及RSS不含部分的內存統計，所以要比pmap -d統計的RSS小。
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
yyset_in() 設置入口


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

kmap 是用來建立映射的，映射後返回了被映射的高端內存在內核的線性地址
https://www.zhihu.com/question/30338816
http://blog.csdn.net/gatieme/article/details/52705142
http://www.cnblogs.com/zhiliao112/p/4251221.html
http://way4ever.com/?p=236
awk統計Linux最常用命令
http://www.ha97.com/3980.html
awk使用技巧
http://blog.csdn.net/ultrani/article/details/6750434
http://blog.csdn.net/u011204847/article/details/51205031 *****
http://ustb80.blog.51cto.com/6139482/1051310


關於smaps的詳細介紹
https://jameshunt.us/writings/smaps.html
$ cat /proc/self/smaps  相比maps顯示更詳細信息
$ cat /proc/self/maps
address                  perms   offset   dev   inode       pathname
7f571af7a000-7f571af7d000 ---p 00000000 00:00 0
7f571af7d000-7f571b080000 rw-p 00000000 00:00 0             [stack:4714]
7f571b0ac000-7f571b0ad000 r--p 00021000 08:01 1838227       /usr/lib/ld-2.21.so
7ffe49dbd000-7ffe49dbf000 r-xp 00000000 00:00 0             [vdso]

列說明:
    starting address - ending address
    permissions
        r : read
        w : write
        x : execute
        s : shared
        p : private (copy on write)
    offset   : 如果不是file，則為0；
    device   : 如果是file，則是file所在device的major and monior device number，否則為00:00；
    inode    : 如果是file，則是file的inode number，否則為0；
    pathname : 有幾種情況；
        file absolute path
        [stack]         the stack of the main process
        [stack:1001]    the stack of the thread with tid 1001
        [heap]
        [vdso] - virtual dynamic shared object, the kernel system call handler
        空白 -通常都是mmap創建的，用於其他一些用途的，比如共享內存


df -h
ls /dev/XXX -alh
echo $((0x0a))

print "Backed by file:\n";
print "  RO data                   r--  $mapped_rodata\n";
print "  Unreadable                ---  $mapped_unreadable\n"; 共享庫同時存在？？？？
print "  Unknown                        $mapped_unknown\n";
print "Anonymous:\n";
print "  Writable code (stack)     rwx  $writable_code\n";
print "  Data (malloc, mmap)       rw-  $data\n";
print "  RO data                   r--  $rodata\n";
print "  Unreadable                ---  $unreadable\n";
print "  Unknown                        $unbacked_unknown\n";

16進制求和，都是16進制
awk --non-decimal-data  '{sum=($1 + $2); printf("0x%x %s\n", sum,$3)}'
strtonum("0x" $1)
echo $(( 16#a36b ))
echo "obase=2;256"|bc ibase base


print "Backed by file:\n";
print "  Unreadable                ---  $mapped_unreadable\n"; 共享庫同時存在？？？？
print "  Unknown                        $mapped_unknown\n";
print "Anonymous:\n";
print "  Unreadable                ---  $unreadable\n";
print "  Unknown                        $unbacked_unknown\n";

代碼
r-x 代碼，包括程序(File)、共享庫(File)、vdso(2Pages)、vsyscall(1Page)
rwx 沒有，Backed by file: Write/Exec (jump tables); Anonymous: Writable code (stack)
r-- 程序中的只讀數據，如字符串，包括程序(File)、共享庫(File)
rw- 可讀寫變量，如全局變量；包括程序(File)、共享庫(File)、stack、heap、匿名映射

靜態數據、全局變量將保存在 ELF 的 .data 段中。
與smaps相關，以及一些實例
https://jameshunt.us/writings/smaps.html


各共享庫的代碼段，存放著二進制可執行的機器指令，是由kernel把該庫ELF文件的代碼段map到虛存空間；
各共享庫的數據段，存放著程序執行所需的全局變量，是由kernel把ELF文件的數據段map到虛存空間；

用戶代碼段，存放著二進制形式的可執行的機器指令，是由kernel把ELF文件的代碼段map到虛存空間；
用戶數據段之上是代碼段，存放著程序執行所需的全局變量，是由kernel把ELF文件的數據段map到虛存空間；

用戶數據段之下是堆(heap)，當且僅當malloc調用時存在，是由kernel把匿名內存map到虛存空間，堆則在程序中沒有調用malloc的情況下不存在；
用戶數據段之下是棧(stack)，作為進程的臨時數據區，是由kernel把匿名內存map到虛存空間，棧空間的增長方向是從高地址到低地址。

https://wiki.wxwidgets.org/Valgrind_Suppression_File_Howto


另外，可以通過 ldd 查看對應的映射地址，在實際映射到物理內存時，會添加隨機的變量，不過如上的各個共享庫的地址是相同的。

可以通過 echo $(( 0x00007f194de48000 - 0x00007f194dc2c000)) 計算差值。


maps 文件對應了內核中的 show_map()

show_map()
 |-show_map_vma()

address                  perms   offset   dev   inode       pathname

http://duartes.org/gustavo/blog/post/how-the-kernel-manages-your-memory/


主要是anon中的rw屬性導致
cat /proc/$(pidof uagent)/maps | grep stack | wc -l



Clean_pages 自從映射之後沒有被修改的頁；
Dirty_pages 反之；
RSS 包括了共享以及私有，Shared_Clean+Shared_Dirty、Private_Clean+Private_Dirty
PSS (Proportional set size) 包括了所有的私有頁 (Private Pages) 以及共享頁的平均值。例如，一個進程有100K的私有頁，與一個進程有500K的共享頁，與四個進程有500K的共享頁，那麼 PSS=100K+(500K/2)+(500K/5)=450K
USS (Unique set size) 私有頁的和。

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






可以通過mprotect設置內存的屬性
https://linux.die.net/man/2/mprotect
Memory protection keys
https://lwn.net/Articles/643797/
Memory Protection and ASLR on Linux
https://eklitzke.org/memory-protection-and-aslr

ES Collectd插件
https://www.elastic.co/guide/en/logstash/current/plugins-codecs-collectd.html




真隨機數等生成
http://www.cnblogs.com/bigship/archive/2010/04/04/1704228.html

在打印時，如果使用了 size_t 類型，那麼通過 ```%d``` 打印將會打印一個告警，可以通過如下方式修改，也就是添加 ```z``` 描述。

size_t x = ...;
ssize_t y = ...;
printf("%zu\n", x);  // prints as unsigned decimal
printf("%zx\n", x);  // prints as hex
printf("%zd\n", y);  // prints as signed decimal

/proc/iomem 保存物理地址的映射情況，每行代表一個資源 (地址範圍和資源名)，其中可用物理內存的資源名為 "System RAM" ，在內核中通過 insert_resource() 這個API註冊到 iomem_resource 這顆資源樹上。

例如，如下的內容：

01200000-0188b446 : Kernel code
0188b447-01bae6ff : Kernel data
01c33000-01dbbfff : Kernel bss

這些地址範圍都是基於物理地址的，在 ```setup_arch()@arch/x86/kernel/setup.c``` 中通過如下方式註冊。

max_pfn = e820_end_of_ram_pfn();
        code_resource.start = __pa_symbol(_text);
        code_resource.end = __pa_symbol(_etext)-1;
        insert_resource(&iomem_resource, &code_resource);

linux虛擬地址轉物理地址
http://luodw.cc/2016/02/17/address/
Linux內存管理
http://gityuan.com/2015/10/30/kernel-memory/
/proc/iomem和/proc/ioports
http://blog.csdn.net/ysbj123/article/details/51088644
port地址空間和memory地址空間是兩個分別編址的空間，都是從0地址開始
port地址也可以映射到memory空間中來，前提是硬件必須支持MMIO
iomem—I/O映射方式的I/O端口和內存映射方式的I/O端口
http://www.cnblogs.com/b2tang/archive/2009/07/07/1518175.html


協程
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

通過sed只顯示匹配行或者某些行
----- 顯示1,10行
$ sed -n '1,10p' filename
----- 顯示第10行
$ sed -n '10p' filename
----- 顯示某些匹配行
$ sed -n '/This/p' filename
sed -n '/\<collectd\>/p' *
sed -i 's/\<Collectd/Uagent/g' *




Proxy
簡單的實現，通常用於嵌入式系統
https://github.com/kklis/proxy
Golang實現的代理，包括了0複製技術等 
https://github.com/funny/proxy
dnscrypt-proxy DNS客戶端與服務端的加密傳輸，使用libevent庫
https://github.com/jedisct1/dnscrypt-proxy
ProxyChains Sock以及HTTP代理，通常用在類似TOR上面
https://github.com/haad/proxychains
https://github.com/rofl0r/proxychains-ng
基於libevent的簡單代理服務
https://github.com/wangchuan3533/proxy
支持多種協議的代理服務，包括FTP、DNS、TCP、UDP等等
https://github.com/z3APA3A/3proxy
一個加密的代理服務
https://github.com/proxytunnel/proxytunnel
Vanish緩存中使用的代理服務
https://github.com/varnish/hitch
360修改的MySQL代理服務
https://github.com/Qihoo360/Atlas
Twitter提供的Memchached代理服務
https://github.com/twitter/twemproxy

UDP可靠傳輸方案
http://blog.codingnow.com/2016/03/reliable_udp.html
http://blog.csdn.net/kennyrose/article/details/7557917
BitCoin中使用的可靠UDP傳輸方案
https://github.com/maidsafe-archive/MaidSafe-RUDP
UDP方案的優缺點
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

很多collectd插件的組合，很多不錯的監控指標梳理
https://github.com/signalfx/integrations
https://github.com/DataDog/the-monitor
https://www.librato.com/docs/kb/collect/integrations/haproxy/
https://www.datadoghq.com/blog/monitoring-haproxy-performance-metrics/
https://github.com/mleinart/collectd-haproxy/blob/master/haproxy.py
Request: (只對HTTP代理有效)
   request_rate(req_rate)      px->fe_req_per_sec           proxy_inc_fe_req_ctr()  請求速率
   req_rate_max 請求限制速率
   req_tot 目前為止總的請求數
Response: (只對HTTP代理有效)
  'hrsp_1xx': ('response_1xx', 'derive'),
  'hrsp_2xx': ('response_2xx', 'derive'),
  'hrsp_3xx': ('response_3xx', 'derive'),
  'hrsp_4xx': ('response_4xx', 'derive'),
  'hrsp_5xx': ('response_5xx', 'derive'),
  'hrsp_other': ('response_other', 'derive'),

>>>>>backend<<<<<
Time:
  qtime (v1.5+) 過去1024個請求在隊裡中的平均等待時間
  rtime (v1.5+) 過去1024個請求在隊裡中的平均響應時間









http://savannah.nongnu.org/projects/nss-mysql
https://github.com/NigelCunningham/pam-MySQL
http://lanxianting.blog.51cto.com/7394580/1767113
https://stackoverflow.com/questions/7271939/warning-ignoring-return-value-of-scanf-declared-with-attribute-warn-unused-r
https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
https://stackoverflow.com/questions/30813452/how-to-ignore-all-warnings-in-c
http://www.jianshu.com/p/7e84a33b46e9
https://github.com/flike/kingshard/blob/master/doc/KingDoc/kingshard_admin_api.md   微博
__attribute__((warn_unused_result))


協程
http://blog.163.com/shu_wang/blog/static/1721704112016114113331412
https://stackoverflow.com/questions/28977302/how-do-stackless-coroutines-differ-from-stackful-coroutines
http://www.infoq.com/cn/articles/CplusStyleCorourtine-At-Wechat
https://www.iamle.com/archives/1865.html
https://github.com/mcxiaoke/mqtt
http://www.infoq.com/cn/articles/fine-tuned-haproxy-to-achieve-concurrent-ssl-connections?utm_campaign=rightbar_v2&utm_source=infoq&utm_medium=articles_link&utm_content=link_text



JMX(Java Management Extensions) 是一個為應用程序植入管理功能的框架，是一套標準的代理和服務，實際上，用戶可以在任何 Java 應用程序中使用這些代理和服務實現管理。
http://blog.csdn.net/derekjiang/article/details/4532375
http://tomcat.apache.org/tomcat-6.0-doc/monitoring.html#Enabling_JMX_Remote
http://comeonbabye.iteye.com/blog/1463104
https://visualvm.github.io/
http://blog.csdn.net/kingzone_2008/article/details/50865350


Buddy 和  Slub 是 Linux 內核中的內存管理算法。Buddy 防止內存的 "外碎片"，即防止內存塊越分越小，而不能滿足大塊內存分配的需求。Slub 防止內存的 "內碎片"，即儘量按請求的大小分配內存塊，防止內存塊使用上的浪費。https://github.com/chenfangxin/buddy_slub

https://stackoverflow.com/questions/9873061/how-to-set-the-source-port-in-the-udp-socket-in-c
http://www.binarytides.com/programming-udp-sockets-c-linux/
https://www.cyberciti.biz/faq/linux-unix-open-ports/
https://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/udpclient.c
https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html
https://www.digitalocean.com/community/tutorials/how-to-use-netcat-to-establish-and-test-tcp-and-udp-connections-on-a-vps





http://my.fit.edu/~vkepuska/ece3551/ADI_Speedway_Golden/Blackfin%20Speedway%20Manuals/LwIP/socket-api/setsockopt_exp.html

socktop(systap使用)




Socket INTR的處理
http://blog.csdn.net/SUKHOI27SMK/article/details/43021081

http://www.tldp.org/HOWTO/html_single/C++-dlopen/

UDP-based Data Transfer Protocol
http://udt.sourceforge.net/
https://github.com/securesocketfunneling/udt
http://blog.leanote.com/post/caasi/Reliable-UDP-3

https://github.com/lsalzman/enet

https://askubuntu.com/questions/714503/regular-expressions-vs-filename-globbing


1.       #註釋
2.       變量：使用set命令顯式定義及賦值，在非if語句中，使用${}引用，if中直接使用變量名引用；後續的set命令會清理變量原來的值；
3.       command (args ...)  #命令不分大小寫，參數使用空格分隔，使用雙引號引起參數中空格
4.       set(var a;b;c) <=> set(var a b c)  #定義變量var並賦值為a;b;c這樣一個string list
5.       Add_executable(${var}) <=> Add_executable(a b c)   #變量使用${xxx}引用

----- 條件語句
if(var)   # 其中空 0 N No OFF FALSE 視為假，NOT 後為真
else()/elseif()
endif(var)

7.       循環語句

Set(VAR a b c)

Foreach(f ${VAR})       …Endforeach(f)

8.       循環語句

WHILE() … ENDWHILE()



INCLUDE_DIRECTORIES(include)  # 本地的include目錄設置
LINK_LIBRARIES('m')           # 添加庫依賴，等價於命令行中的-lm參數



GTest 實際上不建議直接使用二進制文件，而是建議從源碼開始編譯。https://github.com/google/googletest/blob/master/googletest/docs/FAQ.md
如果要使用二進制包，那麼可以使用如下方式進行配置。
find_package(PkgConfig)
pkg_check_modules(GTEST REQUIRED gtest>=1.7.0)
pkg_check_modules(GMOCK REQUIRED gmock>=1.7.0)

include_directories(
    ${GTEST_INCLUDE_DIRS}
    ${GMOCK_INCLUDE_DIRS}
)
http://www.yeolar.com/note/2014/12/16/cmake-how-to-find-libraries/
http://blog.csdn.net/netnote/article/details/4051620

find_package(Threads REQUIRED)   # 使用內置模塊查找thread庫支持


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(uagent)

ADD_SUBDIRECTORY(librudp )
INCLUDE_DIRECTORIES(include)

option(WITH_UNIT_TESTS "Compile with unit tests" OFF)



https://github.com/sohutv/cachecloud  Redis監控管理
https://github.com/apache/incubator-superset 牛掰的項目管理
https://github.com/huichen/wukong 悟空搜索引擎
https://github.com/sylnsfar/qrcode   動態二維碼生成
https://github.com/hellysmile/fake-useragent 偽裝瀏覽器身份
https://github.com/jwasham/coding-interview-university 谷歌面試題
https://github.com/Tencent/libco 騰訊協程庫
https://github.com/xtaci/kcptun 最快的UDP傳輸
https://github.com/reorx/httpstat 圖形顯示http處理耗時
https://github.com/ajermakovics/jvm-mon JVM監控
https://github.com/stampery/mongoaudit MongoDB審計
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


Chrome Vimium 快捷鍵可以直接通過 ? 查看。

git clone -b 0.10.0 https://github.com/pika/pika.git


日誌保存在 /var/log/rabbitmq 目錄下。

schema_integrity_check_failed
一般是由於 mnesia 數據庫的問題導致，簡單粗暴的方式是直接刪除。

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







首先通過 crontab(crontab.c) 完成任務的編輯，然後通過 poke_daemon() 通知 crond 程序，實際上就是通過 utime() 修改 SPOOL_DIR 目錄的訪問和修改時間。而在 crond(cron.c) 程序中，會通過 inotify 機制接收，然後進行更新。

http://blog.csdn.net/rain_qingtian/article/details/11008779

https://github.com/DaveGamble/cJSON

語法規則可以參考 [JSON: The Fat-Free Alternative to XML](yuhttp://www.json.org/fatfree.html) 。

parse_value()  正式的語法解析

https://github.com/staticlibs/ccronexpr

American Fuzzy Lop, AFL 是一種開源的模糊測試器，由谷歌的 Michal Zalewski 開發。可以在源碼編譯時添加，或者使用 QEMU 模式，也就是 QEMU-(User Mode) ，在執行時注入部分代碼進行測試。http://lcamtuf.coredump.cx/afl/
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


測試版本


core
gcov
CPP-Check
Flawfinder


靜態安全掃描 flawfinder、RATS、ITS4、VCG、CPPLint、SPlint

Python: Pychecker、Pylint、RATS


python -m SimpleHTTPServer

## flawfinder

一個 Python 寫的程序，用於掃描代碼，然後在規則庫 (c_ruleset) 中查找符合規則的場景。

源碼可以直接從 [www.dwheeler.com](https://www.dwheeler.com/flawfinder/) 上下載，安裝方式可以查看 README 文件，也就是如下命令。

$ tar xvzf FILENAME.tar.gz       # Uncompress distribution file
$ cd flawfinder-*                # cd into it.
# make prefix=/usr install       # Install in /usr

該工具只針對單個語句進行詞法分析，不檢查上下文，不分析數據類型和數據流；檢查運行時可能存在的問題，比如內存洩露；然後會根據規則庫給出代碼建議。這也就意味著會有部分的誤報，不過因為使用簡單，仍不失為一個不錯的靜態檢測工具。

檢查可以直接指定文件或者目錄，工具會自動查看所有的 C/C++ 文件，如果是 patch (diff -u、svn diff、git diff) 添加參數 --patch/-P 即可。嚴重等級從 0 到 5 依次增加，而且會標示出 [Common Weakness Enumeration, CWE](https://cwe.mitre.org/data/) 對應。

檢查時會讀取 ruleset 中的規則，然後如果匹配 (hit) 則將匹配數據保存到 hitlist 中，

### 常見操作

1. 重點檢查與外部不可信用戶的交互程序，先確保這部分程序無異常；
2. 如果已經審計的函數，可以通過 ```// Flawfinder: ignore``` 或者 ```/* Flawfinder: ignore */``` 減少異常輸出，為了兼容，也可以使用 ```ITS4``` 或者 ```RATS``` 替換 ```Flawfinder```；


--inputs/-I
  只檢查從外部用戶(不可信)獲取數據的函數；
--neverignore/-n
  默認可以通過上述的方式忽略標記的行，通過該參數用於強制檢測所有的代碼；
--savehitlist, --loadhitlist, --diffhitlist
  用於保存、加載、比較hitlist；
--minlevel=NUMBER
  指定最小的錯誤彙報級別；

--quiet/-Q
  默認會在檢測時打印檢查了哪些文件，通過該選項可以關閉，通常用於格式化輸出檢測；
--dataonly/-D
  不顯示header和footer，可以配合--quiet參數只顯示數據；
--singleline/-S
  檢測結果默認會多行顯示，該參數指定一行顯示；
--immediate/-i
  默認在全部文件檢測完之後，進行排序，然後顯示最終的結果，該參數可以在監測到異常後立即顯示；


----- 檢查所有的代碼，即使已經標記為ignore的代碼
$ flawfinder --neverignore src
----- 可以通過如下命令輸出，以供其它自動化工具使用
$ flawfinder -QD src
$ flawfinder -QDSC src
----- 檢查代碼只彙報CWE-120或者CWE-126
$ flawfinder --regex "CWE-120|CWE-126" src/


/* RATS: ignore */



uagent 調試，
export UAGENT_TRACE="yes"

flawfinder -Q --minlevel=5 src | less



int lcc_connect(const char *address, lcc_connection_t **ret_con);
功能：
  建立指向address的socket鏈接，通過ret_con返回鏈接信息；
入參:
  address socket地址，如/usr/var/run/uagent.sock、unix:/usr/var/run/uagent.sock；
  ret_con 返回的已經建立好的鏈接；
返回值：
  -1 入參異常，或者沒有內存；
  0  正常返回；

int lcc_disconnect(lcc_connection_t *c);
功能：
  關閉鏈接，釋放資源；

## coverage
http://blog.csdn.net/livelylittlefish/article/details/6448885
編譯鏈接時需要修改配置選項。

* 編譯的時候，增加 -fprofile-arcs -ftest-coverage 或者 –coverage；
* 鏈接的時候，增加 -fprofile-arcs 或者 –lgcov；
* 打開–g3 選項，去掉-O2以上級別的代碼優化選項，否則編譯器會對代碼做一些優化，例如行合併，從而影響行覆蓋率結果。

ifeq ($(coverage), yes)
CFLAGS   +=  -fprofile-arcs -ftest-coverage -g3
LDFLAGS  +=  -fprofile-arcs -ftest-coverage
endif

如下是測試步驟。
----- 1. 編譯源碼，此時每個文件都會生成一個*.gcno文件
$ make coverage=yes
----- 2. 運行，運行之後會生成 *.gcda 文件
$ ./helloworld
----- 3.1 可以通過如下命令生成單個文件的覆蓋率，生成的是文本文件*.gcov
$ gcov helloworld.c

除了使用 gcov 之外，還可以通過 lcov 查看覆蓋率，簡單說下 *.gcov 的文件格式。

    -:    2:#include <assert.h>   非有效行
    -:    3:#include <stdlib.h>
 ... ...
  148:   71:  if (n == NULL)      調用次數
#####:   72:    return (0);       未調用


簡單介紹下代碼覆蓋率的常見術語。


主要是基本塊（Basic Block），基本塊圖（Basic Block Graph），行覆蓋率（line coverage）, 分支覆蓋率（branch coverage）等。

##### 基本塊
這裡可以把基本塊看成一行整體的代碼，基本塊內的代碼是線性的，要不全部運行，要不都不運行，其詳細解釋如下：
A basic block is a sequence of instructions with only entry and only one exit. If any one of the instructions are executed, they will all be executed, and in sequence from first to last.





    基本塊圖（Basic Block Graph），基本塊的最後一條語句一般都要跳轉，否則後面一條語句也會被計算為基本塊的一部分。 如果跳轉語句是有條件的，就產生了一個分支(arc)，該基本塊就有兩個基本塊作為目的地。如果把每個基本塊當作一個節點，那麼一個函數中的所有基本塊就構成了一個有向圖，稱之為基本塊圖(Basic Block Graph)。且只要知道圖中部分BB或arc的執行次數就可以推算出所有的BB和所有的arc的執行次數；
    打樁，意思是在有效的基本塊之間增加計數器，計算該基本塊被運行的次數；打樁的位置都是在基本塊圖的有效邊上；

##### 行覆蓋率
就是源代碼有效行數與被執行的代碼行的比率；

##### 分支覆蓋率
有判定語句的地方都會出現 2 個分支，整個程序經過的分支與所有分支的比率是分支覆蓋率。注意，與條件覆蓋率(condition coverage)有細微差別，條件覆蓋率在判定語句的組合上有更細的劃分。

### gcc/g++ 編譯選項

如上所述，在編譯完成後會生成 *.gcno 文件，在運行正常結束後生成 *.gcda 數據文件，然後通過 gcov 工具查看結果。

--ftest-coverage
  讓編譯器生成與源代碼同名的*.gcno文件 (note file)，含有重建基本塊依賴圖和將源代碼關聯至基本塊的必要信息；
--fprofile-arcs
  讓編譯器靜態注入對每個源代碼行關聯的計數器進行操作的代碼，並在鏈接階段鏈入靜態庫libgcov.a，其中包含在程序正常結束時生成*.gcda文件的邏輯；

可以通過源碼解析來說明到底這 2 個選項做了什麼，命令如下：
g++ -c -o hello.s hello.c -g -Wall -S
g++ -c -o hello_c.s hello.c -g -Wall –coverage -S
vimdiff hello.s hello_c.s


1. 覆蓋率的結果只有被測試到的文件會被顯示，並非所有被編譯的代碼都被作為覆蓋率的分母

實際上，可以看到整個覆蓋率的產生的過程是4個步驟的流程，一般都通過外圍腳本，或者makefile/shell/python來把整個過程自動化。2個思路去解決這個問題，都是通過外圍的偽裝。第一個，就是修改lcov的 app.info ，中間文件，找到其他的文件與覆蓋率信息的地方，結合makefile，把所有被編譯過的源程序檢查是否存於 app.info 中，如果沒有，增加進去。第二個偽裝，是偽裝 *.gcda，沒有一些源碼覆蓋率信息的原因就是該文件沒有被調用到，沒有響應的gcda文件產生。


2. 後臺進程的覆蓋率數據收集；


其實上述覆蓋率信息的產生，不僅可以針對單元測試，對於功能測試同樣適用。但功能測試，一般linux下c/c++都是實現了某個Daemon進程，而覆蓋率產生的條件是程序需要正常退出，即用戶代碼調用 exit 正常結束時，gcov_exit 函數才得到調用，其繼續調用 __gcov_flush 函數輸出統計數據到 *.gcda 文件中。同樣2個思路可以解決這個問題，

第一，給被測程序增加一個 signal handler，攔截 SIGHUP、SIGINT、SIGQUIT、SIGTERM 等常見強制退出信號，並在 signal handler 中主動調用 exit 或 __gcov_flush 函數輸出統計結果。但這個需要修改被測程序。這個也是我們之前的通用做法。但參加過清無同學的一個講座後，發現了下面第二種更好的方法。

第二，借用動態庫預加載技術和 gcc 擴展的 constructor 屬性，我們可以將 signalhandler 和其註冊過程都封裝到一個獨立的動態庫中，並在預加載動態庫時實現信號攔截註冊。這樣，就可以簡單地通過如下命令行來實現異常退出時的統計結果輸出了。


### lcov

用於生成 html 格式的報告。

yum install --enablerepo=epel lcov perl-GD

----- 1. 生成*.info文件
$ lcov -d . -o 'hello_test.info' -t ‘Hello test’ -b . -c
參數解析：
   -d 指定目錄
----- 2. 生成html，-o指定輸出目錄，可以通過HTTP服務器查看了
$ genhtml -o result hello_test.info



## 靜態檢查

http://www.freebuf.com/sectool/119680.html

cppcheck、Splint(Secure Programming Lint)

### cppcheck

直接從 [github cppcheck](https://github.com/danmar/cppcheck) 下載，然後通過 make && make install 編譯安裝即可。

cppcheck -j 3 --force --enable=all src/*

--force
  如果#ifdef的宏定義過多，則cppcheck只檢查部分
-j
  檢查線程的個數，用於併發檢查；
--enable
  指定當前的檢查級別，可選的參數有all，style，information等；
--inconclusive
  默認只會打印一些確認的錯誤，通過該參數配置異常的都打印；



### Splint

http://www.cnblogs.com/bangerlee/archive/2011/09/07/2166593.html


## 圈複雜度 (Cyclomatic complexity)

OCLint(Mac) cppncss SourceMonitor(Windows)

常用概念介紹如下：

* Non Commenting Source Statements, NCSS 去除註釋的有效代碼行；
* Cyclomatic Complexity Number, CCN 圈複雜度。

同樣，一個函數的 CCN 意味著需要多少個測試案例來覆蓋其不同的路徑，當 CCN 發生很大波動或者 CCN 很高的代碼片段被變更時，意味改動引入缺陷風險高，一般最好小於 10 。



Findbugs (compiled code analysis)
PMD (static code analysis)

### SourceMonitor

http://www.campwoodsw.com/sourcemonitor.html

### cppncss

很簡單的計算圈複雜度的工具，java。


## 內存檢測

Valgrind


HAVE_LIBGEN_H 1
HAVE_FNMATCH_H 1

----- 當前目錄下生成buildbot_master目錄，以及配置文件master.cfg.sample
$ buildbot create-master buildbot_master


$ cd buildbot_master && mv master.cfg.sample master.cfg
$ buildbot checkconfig master.cfg
c['buildbotNetUsageData'] = None

----- 運行
$ buildbot start buildbot_master
# 查看日誌
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



onion_http_new() 會將onion_http_read_ready()賦值給read_ready

onion_http_read_ready()
 |-onion_request_process()


onion_url_new()
 |-onion_url_handler()

onion_listen_point_accept() 在listen端口上出現時調用

onion_listen_point_read_ready()








Micro Transport Protocol, μTP，因為 μ 輸入困難 μTP 通常被寫為 uTP 。這是一個由 BitTorrent 公司開發的協議，在 UDP 協議之上實現可靠傳輸與擁塞控制等特性，可以克服多數防火牆和 NAT 的阻礙，從而大大提高用戶的連接性以及下載速度。

關於該協議的詳細內容可以參考 [uTorrent Transport Protocol](http://www.bittorrent.org/beps/bep_0029.html) 中的內容。

max_window 最大窗口，在傳輸過程中 (in-flight) 可能的最大字節，也就是已經被髮送但是還沒有收到響應的報文；
cur_window 當前窗口，當前正在傳輸中的字節數。
wnd_size   窗口大小，也就是對端建議使用的窗口大小，同時會限制傳輸過程中的字節數。

只有當 (cur_window + packet_size) <= min(max_window, wnd_size) 時才可以發送數據，

A socket may only send a packet if cur_window + packet_size is less than or equal to min(max_window, wnd_size). The packet size may vary, see the packet sizes section.

上報協議格式內容
http://code.huawei.com/Monitor/CloudMonitor-Common/wikis/agent-protocol


cJSON_CreateObject()    創建新的對象，設置對應的type類型
 |-cJSON_New_Item()     新申請cJSON結構內存，並初始化為0
cJSON_CreateString()    和cJSON_CreateRaw()函數調用相同，只是設置的類型不同
 |-cJSON_New_Item()
 |-cJSON_strdup()       創建對象後會將字符串複製一份
cJSON_Print()
 |-print()
   |-print_value()


typedef struct cJSON {
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    struct cJSON *child;    對於Array類型，則會作為鏈表頭
    int type;               類型，包括了String、Number、Raw等
    char *valuestring;      如果是String或者Raw時使用
    int valueint;           這個已經取消，使用valuedouble替換，為了兼容未刪除
    double valuedouble;     如果是Number時使用

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} cJSON;

Invalid、True、False、NULL、Object、Array 通過 type 區分，

cJSON_Parse()
 |-cJSON_ParseWithOpts()
   |-cJSON_New_Item()
   |-parse_value()  根據不同的字段進行解析

cJSON_Duplicate()
cJSON_Minify()
???cJSON_Compare()

cJSON_Parse()
cJSON_Print()  按照可閱讀的格式打印，方便閱讀，一般用於交互
cJSON_PrintUnformatted() 最小化打印，一般用於格式發送
cJSON_PrintBuffered()
cJSON_PrintPreallocated()

string trim
https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way


scanf 中一種很少見但很有用的轉換字符 `[...]` 和 `[ ^...]` 。

#include <stdio.h>
int main()
{
    char strings[100];
    scanf("%[1234567890]", strings);
    printf("%s", strings);
    return 0;
}

運行，輸入 `1234werew` 後，結果是 `1234` ，也就是說，如果輸入的字符屬於方括號內字符串中某個字符，那麼就提取該字符；如果一經發現不屬於就結束提取。

這就是 ANSI C 增加的一種新特性，稱為掃描集 (scanset)，由一對方括號中的一串字符定義，左方括號前必須綴以百分號，通過 `^` 表示補集。

注意，其中必須至少包含一個字符，否則非法，如 `%[]` 和 `%[^]` 是非法的。

%[a-z]  讀取在 a-z 之間的字符串
    char s[]="hello, my friend";   // 注意: 逗號在不a-z之間
    sscanf(s, "%[a-z]", string);   // string=hello
%[^a-z] 讀取不在 a-z 之間的字符串，如果碰到a-z之間的字符則停止
    char s[]="HELLOkitty";
    sscanf( s, "%[^a-z]", string); // string=HELLO
%*[^=]  前面帶 * 號表示不保存變量，跳過符合條件的字符串。
    char s[]="notepad=1.0.0.1001" ;
    char szfilename [32] = "" ;
    int i = sscanf( s, "%*[^=]", szfilename ) ;
// szfilename=NULL,因為沒保存

int i = sscanf( s, "%*[^=]=%s", szfilename ) ;
// szfilename=1.0.0.1001


%40c 讀取40個字符


%[^=] 讀取字符串直到碰到’=’號，’^’後面可以帶更多字符,如：
              char s[]="notepad=1.0.0.1001" ;
              char szfilename [32] = "" ;
             int i = sscanf( s, "%[^=]", szfilename ) ;
           // szfilename=notepad
       如果參數格式是：%[^=:] ，那麼也可以從 notepad:1.0.0.1001讀取notepad
http://www.cnblogs.com/mafly/p/postman.html

http://www.quartz-scheduler.org/documentation/quartz-2.x/tutorials/crontrigger.html
http://www.blogjava.net/javagrass/archive/2011/07/12/354134.html
https://meekrosoft.wordpress.com/2009/11/09/unit-testing-c-code-with-the-googletest-framework/


https://en.wikipedia.org/wiki/Network_Time_Protocol

Linux 內核通過 adjtime() 或者 ntp_adjtime() 來進行時鐘的同步，ntptime
http://jfcarter.net/~jimc/documents/bugfix/12-ntp-wont-sync.html
http://libev.schmorp.de/bench.c
https://stackoverflow.com/questions/14621261/using-libev-with-multiple-threads
https://curl.haxx.se/libcurl/c/evhiperfifo.html
https://github.com/HardySimpson/zlog
http://kagachipg.blogspot.com/2013/10/multi-thread-in-libev.html














conn_config()
 |-port_collect_listening  如果配置了ListeningPorts變量則設置為1
 |-conn_get_port_entry()   對於LocalPort和RemotePort參數，如果存在則設置否則創建
 |-port_collect_total      如果配置了AllPortsSummary變量則設置為1
conn_read()
 |-
####

$ cat << EOF > request.txt
GET / HTTP/1.1
Host: 127.1
EOF
$ cat request.txt | openssl s_client -connect 127.1:443


printf 'GET / HTTP/1.1\r\nHost: github.com\r\n\r\n' | ncat --ssl github.com 443

----- 發送系統日誌內容
$ ncat -l -p 6500 | tee -a copy.out | tar -zx -C $(mktemp -d)
$ (tar -zc -C /var/log; tail -f /var/log/syslog) | ncat 127.1 6500

----- 使用SSL傳輸
$ ncat -l -p 6500 --ssl --ssl-cert /etc/ssl/host.crt \
    --ssl-key /etc/ssl/host.key > out.tgz
$ tar -zc ~ | ncat --ssl 127.1 6500

----- 使用UDP協議
$ ncat -l -p 6500 --udp > out.tgz
$ tar -zc ~ | ncat --udp machineb 6500

----- 使用SCTP
$ ncat --sctp -l -p 6500 > out.tgz
$ tar -zc ~ | ncat --sctp machineb 6500
給系統添加根證書
http://manuals.gfi.com/en/kerio/connect/content/server-configuration/ssl-certificates/adding-trusted-root-certificates-to-the-server-1605.html
https://segmentfault.com/a/1190000002569859
CentOS 會保存在 /etc/ssl/certs/ 目錄下，

--ssl                  Connect or listen with SSL
--ssl-cert             Specify SSL certificate file (PEM) for listening
--ssl-key              Specify SSL private key (PEM) for listening
--ssl-verify           Verify trust and domain name of certificates
--ssl-trustfile        PEM file containing trusted SSL certificates

http://blog.csdn.net/ljy1988123/article/details/51424162
http://blog.csdn.net/younger_china/article/details/72081779
http://blog.csdn.net/yusiguyuan/article/details/48265205

SSL Certificate File 文件中包含了一個 X.509 證書，實際上也就是加密用的公鑰，而 SSL Certificate Key File 文件中是公鑰對應的私鑰，在進行安全傳輸時就需要這對密鑰。有的程序是將兩者放在一起，如一些 Java 程序；有的則會分開存儲，如 Apache 。

一般在申請了證書之後，如通過 GoDaddy，會提供上述的兩個文件。

如果服務端只使用了上述的兩個文件，那麼實際上客戶端並不知道這個證書是誰頒發的；不過一般來說沒有太大問題，因為客戶端會保存很多的 CA 證書，包括中間證書以及根證書。如果要直接指定證書的依賴關係，可以通過 SSLCertificateChainFile 參數指定。

Nginx https配置
https://fatesinger.com/75967
https://imququ.com/post/my-nginx-conf-for-security.html


tail  -> coreutils
tailf -> util-linux

Linux Shell man 命令詳細介紹
http://blog.jobbole.com/93404/
http://www.lai18.com/content/1010397.html

網絡監控
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

C格式化檢查
sparse

indent                                \
 --ignore-profile                  \    不讀取indent的配置文件
 --k-and-r-style                   \    指定使用Kernighan&Ritchie的格式
 --indent-level8                   \    縮進多少字符，如果為tab的整數倍，用tab來縮進，否則用空格填充
 --tab-size8                       \    tab大小為8
 --swallow-optional-blank-lines    \    刪除多餘的空白行
 --line-length130                  \    設置每行的長度
 --no-space-after-casts            \    不要在cast後添加一個空格
 --space-special-semicolon         \    若for或while區段只有一行時，在分號前加上空格
 --else-endif-column1              \    將註釋置於else與elseif右側
    --use-tabs                        \    使用tab做縮進
 --blank-lines-after-procedures    \    函數結束後加空行
 --blank-lines-after-declarations  \    聲明結束後加空行
    load.c

find -type f -regextype posix-egrep -regex ".*(~|\.bak)$" -exec ls -alh {} \;

NAN 一種是 <math.h> 中提供的默認值，也可以自定義宏，如下

#define NAN          (0.0 / 0.0)
#define isnan(f)     ((f) != (f))
#define isfinite(f)  (((f) - (f)) == 0.0)
#define isinf(f)     (!isfinite(f) && !isnan(f))

http://zh.cppreference.com/w/c/numeric/math/fpclassify

其中使用 `isnan()` 時，`FLT_EVAL_METHOD` 將被忽略。

FIXME:
https://jin-yang.github.io/post/collectd-source-code.html
 |   | | | | |-FORMAT_VL()                    ← 實際上是調用format_name()將vl中的值生成標示符
             |-pthread_mutex_lock()
 |   | | | | |-c_avl_get()                    ← 利用上述標示符獲取cache_entry_t，在此會緩存最近的一次採集數據
             |-uc_insert() 如果不存在則插入，直接解鎖退出
    |-合法性檢查，上次的採集時間應該小於本次時間
    |-根據不同的類型進行檢查<<<DS-TYPE>>>，計算方法詳見如下
 |   | | | | |... ...                         ← 會根據不同的類型進行處理，例如DS_TYPE_GAUGE
 |   | | | | |-uc_check_range()               ← 檢查是否在指定的範圍內

values_raw   保存原始數據的值
values_gauge 會按照不同的類型進行計算，其中計算規則如下







git diff <local branch> <remote>/<remote branch>
----- 查看stage中的diff
git diff --cached/--staged
http://perthcharles.github.io/2015/08/25/clean-commit-log-before-push/
https://github.com/chenzhiwei/linux/tree/master/git
https://ddnode.com/2015/04/14/git-modify-remote-responsity-url.html

通過amend修改之後，需要使用--force強制推送該分支。
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
正則表達式
https://www.zhihu.com/question/27434493


以 `CHECK_SYMBOL_EXISTS()` 宏為例，對於 CentOS，在 `/usr/share/cmakeX/Modules` 中存在 `CheckSymbolExists.cmake` 模板，可以直接查看相關宏的定義；其它類似模板同樣可以進行相應的檢查。

ss -tan |awk 'NR>1{++S[$1]}END{for (a in S) print a,S[a]}' && ./tcpstatus

https://github.com/schweikert/fping
https://github.com/octo/liboping


https://www.typora.io/#windows
json-handle markdown edit chrome瀏覽器
http://blog.csdn.net/fandroid/article/details/45787423
進程通訊
http://blog.csdn.net/21aspnet/article/details/7479469

https://github.com/TeamStuQ/skill-map

## 網絡監控

Interface /proc/net/dev
TCPConns
PowerDNS
IPtables
netlink

命令
ethtool
   ethtool -i eth0
netstat

libev壓測
http://libev.schmorp.de/bench.c
spark經典
https://aiyanbo.gitbooks.io/spark-programming-guide-zh-cn/content/index.html
https://github.com/lw-lin/CoolplaySpark/blob/master/Spark%20Streaming%20%E6%BA%90%E7%A0%81%E8%A7%A3%E6%9E%90%E7%B3%BB%E5%88%97/readme.md
小型數據庫
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
NTPL生產者消費者模型
http://cis.poly.edu/cs3224a/Code/ProducerConsumerUsingPthreads.c

netlink編程，以及比較有意思的程序
http://edsionte.com/techblog/archives/4140
https://github.com/lebougui/netlink
https://github.com/eworm-de/netlink-notify
以及ss程序使用示例
http://www.binarytides.com/linux-ss-command/
https://www.cyberciti.biz/files/ss.html
https://serverfault.com/questions/510708/tail-inotify-cannot-be-used-reverting-to-polling-too-many-open-files
https://serverfault.com/questions/561107/how-to-find-out-the-reasons-why-the-network-interface-is-dropping-packets

多線程編程，其中有很多DESIGN_XXX.txt的文檔，甚至包括了Systemtap的使用，其底層用到的是系統提供的 futex_XXX() 調用。
https://github.com/lattera/glibc/tree/master/nptl
https://en.wikipedia.org/wiki/List_of_C%2B%2B_multi-threading_libraries

淺談C++ Multithreading Programming
http://dreamrunner.org/blog/2014/08/07/C-multithreading-programming/
Introduction to Parallel Computing
https://computing.llnl.gov/tutorials/parallel_comp/
解剖 Mutex
https://techsingular.org/2012/01/05/%E8%A7%A3%E5%89%96-mutex/
Pthreads並行編程之spin lock與mutex性能對比分析
http://www.parallellabs.com/2010/01/31/pthreads-programming-spin-lock-vs-mutex-performance-analysis/
Linux線程淺析
http://blog.csdn.net/qq_29924041/article/details/69213248

LinuxThreads VS. NPTL
https://www.ibm.com/developerworks/cn/linux/l-threading.html
http://pauillac.inria.fr/~xleroy/linuxthreads/

FUTEX簡析，也可以通過man 7 futex man 2 futex 查看
http://blog.sina.com.cn/s/blog_e59371cc0102v29b.html
futex and userspace thread syncronization (gnu/linux glibc/nptl) analysis
http://cottidianus.livejournal.com/325955.html
原始論文
https://www.kernel.org/doc/ols/2002/ols2002-pages-479-495.pdf
進程資源
http://liaoph.com/inux-process-management/

http://www.cnblogs.com/big-xuyue/p/4098578.html

BMON Ncurse編程
tcpconns
conn_read()
 |-conn_reset_port_entry()
 |-conn_read_netlink()
 | |-conn_handle_ports()
 |-conn_handle_line()
   |-conn_handle_ports()

ping
Host    會新建一個hostlist_t對象
SourceAddress   ping_source
Device          ping_device，要求OPING庫的版本大於1.3
TTL             ping_ttl，要求0<ttl<255
Interval        ping_interval，採集時間間隔，需要大於0
Size            ping_data，指定報文的大小
Timeout         ping_timeout，超時時間設置
MaxMissed       ping_max_missed

Timeout 不能超過 Interval ，否則會將 Timeout 調整為 0.9 * Interval 。
  
ping_init()
 |-start_thread()
   |-pthread_mutex_lock() 會做條件判斷，防止重複創建線程
   |-plugin_thread_create()  創建ping_thread()線程
會有一個線程ping_thread()一直採集數據，

./configure --prefix=/usr
  
https://cmake.org/Wiki/CMake_FAQ
cmake最終打印信息
https://stackoverflow.com/questions/25240105/how-to-print-messages-after-make-done-with-cmake





http://jialeicui.github.io/blog/file_atomic_operations.html

當多個進程通過 write/read 調用同時訪問一個文件時，無法保證操作的原子性，因為在兩個函數調用間，內核可能會將進程掛起執行另外的進程。

如果想要避免這種情況的話，則需要使用 pread/pwrite 函數。

ssize_t pread(int fd ，void *buffer ，size_t size，off_t offset);
  返回讀取到的字節數，offset是指的從文件開始位置起的offset個字節數開始讀。
  其定位和讀取操作為原子操作，而且讀取過後的文件偏移量不會發生改變。

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

LMDB 中所有的讀寫都是通過事務來執行，其事務具備以下特點：

支持事務嵌套(??)
讀寫事務可以併發執行，但寫寫事務需要被串行化
因此，在lmdb實現中，為了保證寫事務的串行化執行，事務執行之前首先要獲取全局的寫鎖。

底層讀寫的實現

lmdb 使用 mmap 訪問存儲，不管這個存儲是在內存上還是在持久存儲上，都是將需要訪問的文件只讀地裝載到宿主進程的地址空間，直接訪問相應的地址。

減少了硬盤、內核地址控件和用戶地址空間之間的拷貝，也簡化了平坦的“索引空間”上的實現，因為使用了read-only的mmap，規避了因為宿主程序錯誤將存儲結構寫壞的風險。IO的調度由操作系統的頁調度機制完成。

而寫操作，則是通過write系統調用進行的，這主要是為了利用操作系統的文件系統一致性，避免在被訪問的地址上進行同步。

https://hackage.haskell.org/package/lmdb
http://wiki.dreamrunner.org/public_html/C-C++/Library-Notes/LMDB.html
http://www.jianshu.com/p/yzFf8j
http://www.d-kai.me/lmdb%E8%B0%83%E7%A0%94/
https://github.com/exabytes18/mmap-benchmark
https://symas.com/understanding-lmdb-database-file-sizes-and-memory-utilization/
https://github.com/pmwkaa/sophia

http://www.zkt.name/skip-list/

FIXMAP 是什麼意思
MDB_FIXEDMAP

MDB_DUPFIXED
MDB_DUPSORT
內存減小時的優化場景
http://www.openldap.org/conf/odd-sandiego-2004/Jonghyuk.pdf
https://symas.com/performance-tradeoffs-in-lmdb/
http://gridmix.blog.51cto.com/4764051/1731411

## 事務

LMDB 每個事務都會分配唯一的事務編號 (txid)，而且會被持久化。

### 事務表

初始化時會創建一張讀事務表，該表記錄了當前所有的讀事務以及讀事務的執行者 (持有該事務的進程與線程 id )；讀事務表不僅會在內存中維護，同時會將該信息持久化到磁盤上，也就是位與數據庫文件相同目錄下的 lock.mdb 文件。

事務表的文件存儲格式如下圖所示：
http://www.d-kai.me/lmdb%E8%B0%83%E7%A0%94/

該事務表會在 LMDB 打開時初始化，也就是在 `mdb_env_setup_locks()` 函數中，其調用流程如下：

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

### 頁管理

這應該是 lmdb 中最核心的數據結構了，所有的數據和元數據都存儲在page內。

#### Meta Page

Meta Page 使用了起始的兩個 Page，在第一次創建的時候會初始化頁面，並將相同的內容寫入到 Page0 和 Page1 中，也就是說開始時兩個頁的內容一致；寫入是通過 `mdb_env_init_meta()` 函數完成。

因為存在兩個頁，每次寫入或者讀取時需要選一個頁，其計算規則很簡單：

meta = env->me_metas[txn->mt_txnid & 1];

也就是用本次操作的事務 id 取最低位，後面解釋這麼使用的原因。

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
 |   |-mdb_env_read_header()   嘗試從頭部讀取信息，如果是第一次創建，則會調用如下函數
 |   |-mdb_env_init_meta0()  第一次創建
 |   |-mdb_env_init_meta()   第一次創建時同樣需要初始化
 |   |-mdb_env_map()  調用mmap進行映射
 |-mdb_txn_begin()  開啟事務，允許嵌套
   |-mdb_txn_renew0()
 |-mdb_dbi_open()
 |-mdb_put()
   |-mdb_cursor_put() 最複雜的函數處理
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
  ps -Lf 60010 查看線程

CMAKE教程，很不錯
https://blog.gmem.cc/cmake-study-note


https://github.com/jamesroutley/write-a-hash-table

使用場景：
1. 不判斷內存是否申請失敗

cJSON *tag = cJSON_CreateObject() 不判斷是否為NULL
cJSON_AddStringToObject(tag, "key", "value"); 可能會導致內存洩露。場景tag=NULL時，會通過"value"創建一個對象，當tag為NULL時，則會直接退出，那麼通過 "value" 創建的對象將無法釋放。

cJSON_AddItemToArray(data, metric = cJSON_CreateObject());
cJSON_AddStringToObject(metric, "mi_n", m->string);

比如 Facebook 的 wdt (https://github.com/facebook/wdt)，Twitter 的 ( https://github.com/lg/murder )，百度的 Ginko 等等，還有包括亞馬遜 Apollo 裡面的文件分發系統，它們那個和我們的有點不太一樣，他們的是基於 S3 做的。
蜻蜓 - P2P文件分發
AOL - 集中配置管理

插件

PING_OPT_QOS:
  通過setsockopt()配置IP_TOS，實際上是設置IP頭的Type-of-Service字段，用於描述IP包的優先級和QoS選項，例如IPTOS_LOWDELAY、IPTOS_THROUGHPUT等。

會議內容：
1. 監控需求。通過常駐進程記錄上次歷史數據，用於計數類型指標統計，目前方案可能會丟失部分監控數據。該問題CloudAgent方的鄭力、王一力認可。
2. CloudAgent實現。目前針對日誌採集普羅米修斯已經開始開發常駐監控端口，細節尚未討論，該功能點基本功能滿足指標上報的需求。
3. 對接CloudAgent方案。具體細節需要接下來討論，包括對接方式、保活檢測、常駐進程的管理等等。
4. 監控Uagent需求。需要提供動態修改功能，目前來看開發量還比較大；監控插件部分需要做少量修改，之前實現的中間件如haproxy、nginx可以繼續使用。

參數修改
PING_OPT_TIMEOUT:
  設置超時時間。
PING_OPT_TTL:
  同樣是通過setsockopt()配置IP_TTL。
PING_OPT_SOURCE:
  需要通過getaddrinfo()獲取。

ENOTSUP

main()
 |-ping_construct()
 |-ping_setopt()
 |-ping_host_add()
 | |-ping_alloc()
 | |-getaddrinfo() reverse查找
 |-ping_send()
   |-ping_open_socket() 如果還沒有打開
   |-select() 執行異步接口
   |-ping_receive_one()
   |-ping_send_one()


https://stackoverflow.com/questions/16010622/reasoning-behind-c-sockets-sockaddr-and-sockaddr-storage?answertab=votes

FIXME:
獲取CPU核數
http://blog.csdn.net/turkeyzhou/article/details/5962041
浮點數比較
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
為什麼會使用http-parser
https://stackoverflow.com/questions/28891806/what-is-http-parser-where-it-is-used-what-does-it-do
http://mendsley.github.io/2012/12/19/tinyhttp.html
https://github.com/tinylcy/tinyhttpd




pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER;
http://www.cnblogs.com/renxinyuan/p/3875659.html
http://blog-kingshaohua.rhcloud.com/archives/54

設置線程名稱
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

如果沒有將線程設置為 detached ，而且沒有顯示的 pthread_exit()，那麼在通過 valgrind 進行測試時會發現出現了內存洩露。

在通過 pthread_key_create() 創建私有變量時，只有調用 pthread_exit() 後才會調用上述函數註冊的 destructor ；例如主進程實際上不會調用 destructors，此時可以通過 atexit() 註冊回調函數。



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

該錯誤信息是由於返回了 EADDRINUSE 錯誤碼，通常是由 TCP 套接字的 TIME_WAIT 狀態引起，該狀態在套接字關閉後會保留約 2~4 分鐘，只有在該狀態 TIME_WAIT 退出之後，套接字被刪除，該地址才能被重新綁定而不出問題。

在 C 中，可以通過如下方式設置端口允許重用。

int opt = 1;
setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

/linux-monitor-cpu.html
ps -ax -o uid,pid,ppid,tpgid,pgrp,session,lstart,cmd

Linux查看某個文件被誰佔用
當用戶卸載某個目錄的時候，因其他用戶在當前目錄或者當前目錄在運行一個程序，卸載時報busy的處理辦法：

1：fuser -av /mnt
查看佔用mnt的程序以及pid，根據pid去kill -9

2：fuser -km /mnt
查看佔用mnt的程序並自動kill
-m是指定被佔用的目錄，-k是kill

3：losf /mnt
查看正在使用的某個文件
4：遞歸查看某個目錄的文件信息
lsof +D /mnt/fat

5:列出某個用戶打開的文件信息

lsof -u student

6：列出某個程序打開的文件信息

lsof -c mysql
http://blog.csdn.net/kozazyh/article/details/5495532


System V IPC 分三類：消息隊列、信號量、共享內存區，都採用 `key_t` 作為其內部使用的標示，該類型在 `<sys/types.h>` 中定義，一般是 32 位整數。

通常可以使用 ftok() 函數，也就是 file to key，把一個已存在的路徑名和一個整數標識符轉換成一個 `key_t` 值，該函數聲明如下：

#include <sys/ipc.h>
key_t ftok (const char *pathname, int proj_id);

pathname 通常是跟本應用相關的路徑；proj_id 指的是本應用所用到的 IPC 的一個序列號，通常約定好，這樣可以獲取相同的 `key_t` 值。

注意，需要保證該路徑應用程序可以訪問，並且在運行期間不能刪除。

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

通過執行結果可看出，ftok獲取的鍵值是由ftok()函數的第二個參數的後8個bit，st_dev的後兩位，st_ino的後四位構成的

### semget

創建一個新的信號量或獲取一個已經存在的信號量的鍵值。

#include <sys/sem.h>
int semget(key_t key, int nsems, int semflg);

key: 為整型值，可以自己設定，有兩種場景
   1. IPC_PRIVATE 通常為 0，創建一個僅能被本進程給我的信號量。
   2. 非 0 的值，可以自己手動指定，或者通過 ftok() 函數獲取一個唯一的鍵值。
nsems: 初始化信號量的個數。
semflg: 信號量創建方式或權限，包括了 IPC_CREAT(不存在則創建，存在則獲取)；IPC_EXCL(不存在則建立，否則報錯)。

#include <stdio.h>
#include <sys/sem.h>

int main()
{
 int semid;
 semid = semget(666, 1, IPC_CREAT | 0666); // 創建了一個權限為666的信號量
 printf("semid=%d\n", semid);
 return 0;
}

可以用 ipcs –s 來查看是否創建成功。
用 ipcrm -s semid 號來刪除指定的信號量。



################################
# CMake
################################


針對特定對象，可以通過如下方式指定特定的編譯選項、頭文件路徑、宏定義。

target_compile_definitions(audio_decoder_unittests
 PRIVATE "AUDIO_DECODER_UNITTEST"
 PRIVATE "WEBRTC_CODEC_PCM16")
target_include_directories(audio_decoder_unittests
 PRIVATE "interface"
 PRIVATE "test"
 PRIVATE "../codecs/g711/include")
target_compile_options(RTPencode PRIVATE "/wd4267")


## 配置文件
CheckSymbolExists.cmake   宏定義檢查


################################
# Curl
################################

詳細可以查看 http://php.net/manual/zh/function.curl-setopt.php
https://moz.com/devblog/high-performance-libcurl-tips/

CURLOPT_NOSIGNAL

CURLOPT_WRITEFUNCTION 用於設置數據讀取之後的回調函數，通過該函數可以保存結果，其函數聲明如下。
    size_t function( char *ptr, size_t size, size_t nmemb, void *userdata);
CURLOPT_WRITEDATA 定義了上述函數聲明中userdata的值。

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
CURLOPT_PASSWORD 分別設置用戶名密碼，低版本可以通過CURLOPT_USERPWD選項設置，其值為"user:password" 。

CURLOPT_TIMEOUT_MS 設置超時時間。

CURLOPT_MAXREDIRS
CURLOPT_FOLLOWLOCATION 允許重定向，以及設置重定向的跳轉次數。


CURLOPT_SSL_VERIFYPEER 驗證證書，證書信息可以通過 CURLOPT_CAINFO 設置，或在 CURLOPT_CAPATH 中設置證書目錄。

CURLOPT_SSL_VERIFYHOST 設置為 1 是檢查服務器SSL證書中是否存在一個公用名(common name)。譯者注：公用名(Common Name)一般來講就是填寫你將要申請SSL證書的域名 (domain)或子域名(sub domain)。 設置成 2，會檢查公用名是否存在，並且是否與提供的主機名匹配。 0 為不檢查名稱。 在生產環境中，這個值應該是 2（默認值）。


./configure  \
    --disable-shared --enable-static                     不使用動態庫，而是靜態編譯
    --without-libidn2 --without-winidn                   忽略國際化庫
 --disable-ipv6 --disable-unix-sockets                關閉IPV6以及Unix Socket
 --without-ssl --without-gnutls --without-nss         關閉安全配置項1
 --without-libssh2 --disable-tls-srp --without-gssapi 關閉安全配置項2
    --without-zlib                                       不支持壓縮
 --disable-ares --disable-threaded-resolver      
 --without-librtmp  --disable-rtsp                    關閉不需要的協議1
 --disable-ldap --disable-ldaps                       關閉不需要的協議2
 --disable-dict --disable-file --disable-gopher
 --disable-ftp --disable-imap --disable-pop3
 --disable-smtp --disable-telnet --disable-tftp
 --disable-sspi                                       Windows選項
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
curl_easy_getinfo(session, CURLINFO_RESPONSE_CODE, &http_code);  /* 獲取返回碼 */



http://187.0.0.1:8080/status



使用 curl 測量 Web 站點的響應時間。

curl -o /dev/null -s -w '%{http_code}-%{time_namelookup}:%{time_connect}:%{time_appconnect}:%{time_pretransfer}:%{time_redirect}:%{time_starttransfer}:%{time_total}\n' 'http://187.0.0.1:8080/status'

time_namelookup     DNS解析時間，從請求開始到DNS解析完畢所用時間
time_connect     建立到服務器的 TCP 連接所用的時間
time_appconnect     連接建立完成時間，如SSL/SSH等建立連接或者完成三次握手時間
time_pretransfer    準備傳輸的時間，對於一些協議需要做一些初始化操作
time_redirect       重定向時間，包括到最後一次傳輸前的幾次重定向的DNS解析、連接、預傳輸、傳輸時間
time_starttransfer 傳輸時間，在發出請求之後，服務器返回數據的第一個字節所用的時間
time_total          完成請求所用的時間
speed_download      下載速度，單位是字節/秒
http_code           返回碼

注意，如果某一步失敗了，該步驟對應的值實際上顯示的是 0 ，此時需要通過總時間減去上一步的消耗時間。

上述的執行，是在執行 curl_easy_perform() 函數的時候開始的，在 docs/examples 目錄下，有很多的參考實例。


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

網卡緩存
https://zrj.me/archives/1102



http://www.cnblogs.com/bodhitree/p/6018369.html
sed高級用法
https://www.zhukun.net/archives/6975
http://gohom.win/2015/06/20/shell-symbol/
mysql core dump
http://xiezhenye.com/2015/05/%E8%8E%B7%E5%8F%96-mysql-%E5%B4%A9%E6%BA%83%E6%97%B6%E7%9A%84-core-file.html
文件句柄數
http://blog.sina.com.cn/s/blog_919f173b01014vol.html
http://www.opstool.com/article/166
rpm 升級到舊的版本
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


可以通過如下命令指定分支，提交後會修改原數據中的 `Subproject commit` 。
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



tar 打包可以通過 --exclude=dir 排除。
通過 `--transform` 參數可以根據 `sed` 語法進行一些轉換，例如增加前綴 `'s,^,prefix/,'` 或者 `s%^%prefix/%`。



支持數據類型float, int, str, text, log

支持函數：
    abschange 計算最新值和上次採集值相減的絕對值，對於字符串0/相同、1/不同，1->5=4, 3->1=2

https://www.zabbix.com/documentation/3.0/manual/appendix/triggers/functions


支持數據類型：
   Numeric (unsigned) - 64位無符號整數；
   Numeric (float) - 浮點數，可以存儲負值，範圍是 [-999999999999.9999, 999999999999.9999]，同時可以支持科學計算 1e+7、1e-4；
   Character - 短文本數據，最大255字節；
  
  
Log - 具有可選日誌相關屬性的長文本數據(timestamp, source, severity, logeventid)
Text - 長文本數據


Tim O’Reilly and Crew [5, p.726]
The load average tries to measure the number of active processes at any time. As a measure of CPU utilization, the load average is simplistic, poorly defined, but far from useless.

Adrian Cockcroft [6, p. 229]
The load average is the sum of the run queue length and the number of jobs currently running on the CPUs.


默認沒有移動平均值計算，只是針對單個值進行計算。

threshold_tree 仍然通過format_name格式化名稱。
 
目前分為了三種類型，分別為 Host > Plugin > Type ，需要按照層級進行排列，例如 Host 下面可以有 Plugin 和 Type 段；Plugin 下可以有 Type 但是不能有 Host 。

其它的配置項用於一些類似閾值的判斷等，只能在 Type 下面配置。

FailureMax Value
WarningMax Value
 設置報警的上限值，如果沒有配置則是正無窮。告警發送規則如下：
    A) (FailureMax, +infty) 發送 FAILURE 通知；
    B) (WarningMax, FailureMax] 發送 WARNING 通知；

FailureMin Value
WarningMin Value
 設置報警的下限值，如果沒有配置則是負無窮。告警發送規則如下：
    A) (-infty, FailureMin) 發送 FAILURE 通知；
    B) [FailureMin, WarningMin) 發送 WARNING 通知；

Persist true|false(default)
 多久發送一次報警，設置規則如下：
    true) 每次超過閾值之後都會發送一次報警通知；
    false) 只有在狀態發生轉換且前一次狀態是OKAY時才會發送一次通知。

PersistOK true|false(default)
 定義如何發送OKAY通知，設置規則如下：
    true) 每次在正常範圍內都會發送通知；
    false) 當本次狀態正常而且之前狀態不正常時才發送一次OK通知。

Hysteresis Value
 遲滯作用，用於處理在一個狀態內重複變換，在該閾值範圍內不會告警。

Hits Value
    告警條件必須連續滿足多少次之後才會發送告警。

Interesting true(default)|false
 發現數據未更新時是否發送告警，會根據插件的採集時間間隔以及 Timeout 參數判斷是否有事件發生。
    true) 發送FAILURE報警；
    false) 忽略該事件。

DataSource  <-> Types.db 中字段，例如midterm
Host <-> host
Plugin <-> plugin
Type <-> type
Instance <-> type_instance

1. 根據value list中的參數獲取到具體的配置。
2.

Invert true|false
Percentage true|false

以 loadavg 為例，其實現在 fs/proc/loadavg.c 中。
calc_global_load()
監控指標，其中監控插件包括了

https://en.wikipedia.org/wiki/Moving_average
http://www.perfdynamics.com/CMG/CMGslides4up.pdf
https://zh.wikipedia.org/wiki/%E7%A7%BB%E5%8B%95%E5%B9%B3%E5%9D%87

移動平均 (Moving Average) 可以處理短期波動，反映長期趨勢或週期，從數學上看做是卷積。


## 簡單移動平均

Simple Moving Average, SMA 將變量的前 N 值做平均。

SMA = (V1 + V2 + ... + Vn) / n

當有新值之後，無需重複計算，只需要將最老的舊值刪除，然後加入新值。

SMAn = SMAn-1 - V1/n + V/n

這樣需要保存 N 個值。

## 加權移動平均

Weighted Moving Average, WMA 也就是在計算平均值時將部分數據乘以不同數值，

## 指數移動平均

Exponential Moving Average, EMA


----- 鎖定用戶該用戶不能再次登錄
ALTER USER username ACCOUNT LOCK;
----- 解鎖用戶
ALTER USER username ACCOUNT UNLOCK;

aussdb plugin: Connect to database failed: FATAL:  The account has been locked.
FATAL:  The account has been locked.

Zabbix上報數據格式
http://www.ttlsa.com/zabbix/zabbix-active-and-passive-checks/
Open-falcon 上報數據
http://blog.niean.name/2015/08/06/falcon-intro
main_timer_loop() 週期計算定義的觸發值，如果有事件發生，那麼就直接寫入到數據庫中。


timer_thread()     main_timer_loop時間相關的處理
 |-process_time_functions()
 | |-DCconfig_get_time_based_triggers() 從緩存中獲取trigger表達式
 | |-evaluate_expressions()       觸發器表達式的主要處理函數，同時會產生事件
 | | |-substitute_simple_macros() 宏分為兩類，分別是{...} {$...}
 | | |-substitute_functions()
 | | | |-zbx_evaluate_item_functions()
 | | |   |-evaluate_function()
 | | |-evaluate()
 | |
 | |-DBbegin()
 | |-process_triggers() T:triggers
 | | |-process_trigger()
 | |   |-add_event()              會保存到內存的events數組中
 | |-process_events()             處理事件，主要是將新事件插入數據庫
 | | |-save_events() T:events
 | | |-process_actions() T:actions
 | | |-clean_events()
 | |-DBcommit()
 |
 |-process_maintenance()


源碼解析
https://jackywu.github.io/articles/zabbix_server%E6%BA%90%E7%A0%81%E5%88%86%E6%9E%90/

## 表結構

{<server>:<key>.<function>(<parameter>)}<operator><constant>

hosts  包含了主機以及模板信息。
    select hostid, host from hosts where status = 0;
    hostid  主機ID
 host    主機、模板名
 status  0->主機 3->模板
goups   主機的邏輯分組
hosts_grous 分組和主機之間的關聯關係
items 監控項，保存了每個主機的監控項，該監控項來自的模板ID
    select itemid, name, key_, templateid, delay, status, units from items where hostid = 10107;
triggers 觸發器，其中表達式中使用的是function id
 select triggerid, expression, status, value, description from triggers;
 select * from functions where functionid = 13302;
functions

Paxos協議解析
https://zhuanlan.zhihu.com/p/21438357?refer=lynncui

https://github.com/hanc00l/wooyun_public
https://github.com/niezhiyang/open_source_team
http://www.jianshu.com/p/43c604177c08
http://kenwheeler.github.io/slick/

http://lovestblog.cn/blog/2016/07/20/jstat/
http://blog.phpdr.net/java-visualvm%E8%AE%BE%E7%BD%AEjstat%E5%92%8Cjmx.html


http://metrics20.org/spec/
Stack Overflow 的架構
https://zhuanlan.zhihu.com/p/22353191
GCC部分文件取消告警
http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
http://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
http://gcc.gnu.org/onlinedocs/gcc-4.0.4/gcc/Warning-Options.html
-->

{% highlight text %}
{% endhighlight %}
