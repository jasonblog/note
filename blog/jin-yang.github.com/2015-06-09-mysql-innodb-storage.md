---
title: InnoDB 存儲空間
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,表空間,table space
description: InnoDB 表空間 (table space) 用來組織存儲保存的數據，本文中對錶空間管理進行分析。
---

InnoDB 表空間 (table space) 用來組織存儲保存的數據，本文中對錶空間管理進行分析。

<!-- more -->

## 簡介

InnoDB 實現的表空間是在文件系統之上又構建的一層邏輯存儲的空間管理，每個表空間在邏輯上劃分為瞭如下的幾層結構，依次包括 table space、segment inode、extent、page、record 。

![innodb files](/images/databases/mysql/innodb-tablespace.png){: .pull-center }

如圖所示，InnoDB 最小讀寫單位是 Page；為了高效管理，又將連續的 64 個頁稱為 "extent"，而且在一個表空間中，通常是以 "extent" 為單位進行資源分配。

除此之外，還在 extents 上層添加了一個 segment inode 管理，最多可以管理 256 個 extents。如果 page 採用默認的 16K，那麼對應的 extent 大小為 64*16K=1M，而 segment 對應 256M 。

### 參數設置

首先確認幾個與表空間管理相關的參數，也就是打開獨立表空間，且採用默認的頁大小為 16K 。

{% highlight text %}
mysql> SHOW VARIABLES LIKE 'innodb_file_per_table';
+-----------------------+-------+
| Variable_name         | Value |
+-----------------------+-------+
| innodb_file_per_table | ON    |
+-----------------------+-------+
1 row in set (0.01 sec)

mysql> SHOW VARIABLES LIKE 'innodb_page_size';
+------------------+-------+
| Variable_name    | Value |
+------------------+-------+
| innodb_page_size | 16384 |
+------------------+-------+
1 row in set (0.00 sec)
{% endhighlight %}

### 工具

在查看錶空間時可以使用幾個常用的工具，可以參考 [innodb_page_info](/reference/databases/mysql/innodb_page_info) 。

{% highlight text %}
----- 查看每個頁的詳細信息
$ innodb_page_info -v dept_emp.ibd

{% endhighlight %}


## 文件格式詳解

InnoDB 的存儲文件格式如上圖所示，我們直接從大到小介紹其結構，依次從 space file -> segment -> extent -> page -> row 逐一介紹。

### Space File 格式

Space File 主要包括了兩類：系統空間 (system space) 和 表空間 (Per-table space files)。一個表空間文件，實際就是一系列 Pages 的組合 (最大 2<sup>32</sup>)，為了方便管理，你可以理解為做了三級的劃分，分別是 segment、extent、page 。

其基本結構如下所示：

{% highlight text %}
0 KiB   +---------------------------------------------------+
        |   FSP_HDR: Filespace Header / Extent Descriptor   |
16 KiB  +---------------------------------------------------+
        |       IBUF_BITMAP: Insert Buffer Bookeeping       |
32 KiB  +---------------------------------------------------+
        |           INODE: Index Node Information           |
48 KiB  +---------------------------------------------------+
   |    |                                                   |
   |    ~                  More pages ...                   ~
   v    |                                                   |
256 MiB +---------------------------------------------------+
   |    |   XDES: Extent descriptor for next 16,384 pages   |
   |    +---------------------------------------------------+
   |    |  IBUF_BITMAP: IBUF Bitmap for next 16,384 pages   |
   |    +---------------------------------------------------+
   |    |                                                   |
   |    ~                  More pages ...                   ~
   v    |                                                   |
512 MiB +---------------------------------------------------+
   |    |   XDES: Extent descriptor for next 16,384 pages   |
   |    +---------------------------------------------------+
   |    |  IBUF_BITMAP: IBUF Bitmap for next 16,384 pages   |
   |    +---------------------------------------------------+
   |    |                                                   |
   |    ~                  More pages ...                   ~
   |    |                                                   |
   v    +---------------------------------------------------+
{% endhighlight %}

對於 Page0 肯定是 "File SPace HeaDeR"，其中維護了 extent 相關的信息，例如那些是空閒的、滿的、有碎片的 (fragmented)。在 FSP_HDR 頁中，只能維護 256 個 extent 相關的信息 (16,384 pages==256MiB)，這也就導致每隔 256M 都需要有一個 XDES 來管理接下來的 extent 元信息。

<!--
The third page in each space (page 2) will be an INODE page, which is used to store lists related to file segments (groupings of extents plus an array of singly-allocated “fragment” pages). Each INODE page can store 85 INODE entries, and each index requires two INODE entries. (A more detailed discussion of INODE entries and file segments is reserved for a future post.)

Alongside each FSP_HDR or XDES page will also be an IBUF_BITMAP page, which is used for bookkeeping information related to insert buffering, and is outside the scope of this post.
-->


#### 系統空間

在系統空間中的固定位置保存了與 InnoDB 相關的關鍵信息，其中的 FSP_HDR、IBUF_BITMAP、INODE 三個頁都是相同的，只是之後的頁會有區別。

{% highlight text %}
0 KiB    +---------------------------------------------------+
         |   FSP_HDR: Filespace Header / Extent Descriptor   |
16 KiB   +---------------------------------------------------+
         |       IBUF_BITMAP: Insert Buffer Bookeeping       |
32 KiB   +---------------------------------------------------+
         |           INODE: Index Node Information           |
48 KiB   +---------------------------------------------------+
         |             SYS: Insert Buffer Header             |
64 KiB   +---------------------------------------------------+
   |     |            INDEX: Insert Buffer Header            |
   |     +---------------------------------------------------+
   |     |        TRX_SYS: Transaction System Header         |
   |     +---------------------------------------------------+
   |     |           SYS: First Rollback Segment             |
   |     +---------------------------------------------------+
   |     |           SYS: Data Dictionary Header             |
   |     +---------------------------------------------------+
   |     |                                                   |
   |     ~                  More pages ...                   ~
   v     |                                                   |
Page 64  +---------------------------------------------------+
         |      Double Write Buffer Block 1 (64 pages)       |
Page 128 +---------------------------------------------------+
         |      Double Write Buffer Block 2 (64 pages)       |
Page 192 +---------------------------------------------------+
   |     |                                                   |
   |     ~                  More pages ...                   ~
   |     |                                                   |
   v     +---------------------------------------------------+
{% endhighlight %}
<!--
Page3 以下的頁定義為：

Page 3, type SYS: Headers and bookkeeping information related to insert buffering.
Page 4, type INDEX: The root page of the index structure used for insert buffering.
Page 5, type TRX_SYS: Information related to the operation of InnoDB’s transaction system, such as the latest transaction ID, MySQL binary log information, and the location of the double write buffer extents.
Page 6, type SYS: The first rollback segment page. Additional pages (or whole extents) are allocated as needed to store rollback segment data.
Page 7, type SYS: Headers related to the data dictionary, containing root page numbers for the indexes that make up the data dictionary. This information is required to be able to find any other indexes (tables), as their root page numbers are stored in the data dictionary itself.
Pages 64-127: The first block of 64 pages (an extent) in the double write buffer. The double write buffer is used as part of InnoDB’s recovery mechanism.
Pages 128-191: The second block of the double write buffer.

All other pages are allocated on an as-needed basis to indexes, rollback segments, undo logs, etc.
-->

#### 表空間 Per-table Space Files

這需要打開 innodb_file_per_table 變量之後才會生效。

{% highlight text %}
0 KiB    +---------------------------------------------------+
         |   FSP_HDR: Filespace Header / Extent Descriptor   |
16 KiB   +---------------------------------------------------+
         |       IBUF_BITMAP: Insert Buffer Bookeeping       |
32 KiB   +---------------------------------------------------+
         |           INODE: Index Node Information           |
48 KiB   +---------------------------------------------------+
         |         INDEX: Root page of first index           |
64 KiB   +---------------------------------------------------+
   |     |        INDEX: Root page of second index           |
   |     +---------------------------------------------------+
   |     |              INDEX: Node pages ...                |
   |     +---------------------------------------------------+
   |     |              INDEX: Leaf pages ...                |
   |     +---------------------------------------------------+
   |     |     ALLOCATED: Reserved bu unused pages ...       |
   |     +---------------------------------------------------+
   |     |                                                   |
   |     ~                  More pages ...                   ~
   |     |                                                   |
   v     +---------------------------------------------------+
{% endhighlight %}


<!--
Ignoring “fast index creation” which adds indexes at runtime, after the requisite 3 initial pages, the next pages allocated in the space will be the root pages of each index in the table, in the order they were defined in the table creation. Page 3 will be the root of the clustered index, Page 4 will be the root of the first secondary key, etc.

Since most of InnoDB’s bookkeeping structures are stored in the system space, most pages allocated in a per-table space will be of type INDEX and store table data.
-->

### Segment

在 segment 中保存了與 extent 相關的元數據，實際上是在空間文件的固定位置保存了 extent 的元數據信息，其格式為 FSP_HDR/XDES，其結構也非常簡單，在 Page Body 中保存了 256 個 extent descriptors 。

{% highlight text %}
00000 +---------------------------------------------------+
      |                  FIL header (38)                  |
00038 +---------------------------------------------------+
      |   FSP Header (zero-filled for XDES pages) (112)   |
00150 +---------------------------------------------------+
      |     XDES Entry   0 (pages     0 ~    63) (40)     |
00190 +---------------------------------------------------+
      |     XDES Entry   1 (pages    64 ~   127) (40)     |
00230 +---------------------------------------------------+
      |     XDES Entry   2 (pages   128 ~   191) (40)     |
00270 +---------------------------------------------------+
      |     XDES Entry   3 (pages   192 ~   255) (40)     |
00310 +---------------------------------------------------+
      |                                                   |
      ~                    ... ...                        ~
      |                                                   |
10310 +---------------------------------------------------+
      |     XDES Entry 254 (pages 16256 ~ 16319) (40)     |
10350 +---------------------------------------------------+
      |     XDES Entry 255 (pages 16320 ~ 16383) (40)     |
10390 +---------------------------------------------------+
      |                                                   |
      ~            (Empty Space: 5,986 bytes)             ~
      |                                                   |
16376 +---------------------------------------------------+
      |                   FIL Trailer (8)                 |
16384 +---------------------------------------------------+
{% endhighlight %}

該頁的類型同樣在 fil0fil.h 中定義。

{% highlight c %}
#define FIL_PAGE_INODE      3   /*!< Index node */
{% endhighlight %}

該類型的頁定義為 inode，這與文件系統中的 inode 概念有點混淆，實際上與文件系統中的概念完全不同，在 InnoDB 中只是描述了與 extent 相關的信息。

### Extents and extent descriptors

extent 是為了更加方便的管理 page，也可以稱為簇或者區，對於 16KiB 大小的 page 一個 extent 管理 64 個，其定義在 fsp0types.h 文件中定義，不同大小的頁，對應的 extent 也有所不同。

{% highlight c %}
/** File space extent size in pages
page size | file space extent size
----------+-----------------------
   4 KiB  | 256 pages = 1 MiB
   8 KiB  | 128 pages = 1 MiB
  16 KiB  |  64 pages = 1 MiB
  32 KiB  |  64 pages = 2 MiB
  64 KiB  |  64 pages = 4 MiB
*/
/** File space extent size (one megabyte if default two or four if not) in pages */
#define FSP_EXTENT_SIZE     ((UNIV_PAGE_SIZE <= (16384) ?   \
                (1048576U / UNIV_PAGE_SIZE) :   \
                ((UNIV_PAGE_SIZE <= (32768)) ?  \
                (2097152U / UNIV_PAGE_SIZE) :   \
                (4194304U / UNIV_PAGE_SIZE))))
{% endhighlight %}

extent 是通過一個叫簇描述符 (extent descriptor) 來表示的，對應了上述的 XDES Entry，詳細格式如下：

{% highlight text %}
N    +------------------------------------+
     |        File Segment ID (8)         |
N+8  +------------------------------------+
     |    List node for XDES list (12)    |
N+20 +------------------------------------+
     |            State (4)               |
N+24 +------------------------------------+
     |      Page State Bitmap (16)        |
     |  2 bits per page, 1=free, 2=clean  |
N+40 +------------------------------------+
{% endhighlight %}

詳細的宏定義在 fsp0fsp.h 頭文件中。

{% highlight c %}
#define XDES_ID                            0
#define XDES_FLST_NODE                     8
#define XDES_STATE      (FLST_NODE_SIZE + 8)
#define XDES_BITMAP    (FLST_NODE_SIZE + 12)

#define XDES_SIZE (XDES_BITMAP + UT_BITS_IN_BYTES(FSP_EXTENT_SIZE * XDES_BITS_PER_PAGE))
{% endhighlight %}

每個 extent descriptor 會管理 64 個 pages，最後的 16Bytes 用來標識這 64 個 page 的狀態，如空閒、半空閒，主要目的是為了能夠快速從這 64 個 page 中找到空閒的 page。


<!--
XDES_ID 存放該extent所屬segment的id

XDES_FLST_NODE實際是一個文件鏈表節點，這樣可以把extent descriptor組織成一個鏈表，實際上我們在extent更上層管理extent時會把extent鏈在一起組成鏈表，那麼實際鏈表的節點就是extent descriptor的這個字段，比如某個segment的extent free list和表空間的extent frag list都是通過這個節點組織extent鏈表的，這個後續詳細說明

XDES_STATE標識該extent descriptor所代表的extent的所屬狀態，定義如下：

/* States of a descriptor */
#define    XDES_FREE     1    /* extent is in free list of space */
#define    XDES_FREE_FRAG     2    /* extent is in free fragment list of space */
#define    XDES_FULL_FRAG     3    /* extent is in full fragment list of space */
#define    XDES_FSEG     4    /* extent belongs to a segment */
-->


### Page

頁是 InnoDB 中的最小的邏輯存儲單位，很多基本操作都是以頁為單位進行，在 ```include/univ.i``` 文件中通過宏指定，默認大小是 16K。

{% highlight c %}
#define UNIV_PAGE_SIZE      ((ulint) srv_page_size)
{% endhighlight %}

而變量 ```srv_page_size``` 的大小，實際對應了 ```innodb_page_size``` 參數。與頁相關的宏定義基本都在 fil0fil.h 頭文件中，其中定義了多種頁類型，其中比較常見的舉例如下：

{% highlight c %}
#define FIL_PAGE_INDEX      17855    // 數據索引頁
#define FIL_PAGE_UNDO_LOG   2        // 事務回滾日誌頁
#define FIL_PAGE_INODE      3        // inode頁，用來管理segment
{% endhighlight %}

如上所述，默認每個頁的大小為 16K=16384Byte，每個頁通過一個 int32 類型的值標識其在表空間中頁的偏移量，也就是說默認的表空間最大是 2<sup>32</sup> x 16 KiB = 64 TiB 。

#### 頁格式

所有 page 的格式都是相同的，由 page_header、page_body、page_trailer 三部分組成。

{% highlight text %}
00000 +-------------------------------------------+
      |      FIL header (38) FIL_PAGE_DATA        |
00038 +-------------------------------------------+
      |    Body, other headers and page data,     |
      |        depending on page type.            |
      |                                           |
      |     Total usable space: 16,338 bytes      |
      |                                           |
16376 +-------------------------------------------+
      |    FIL Trailer (8) FIL_PAGE_DATA_END      |
16384 +-------------------------------------------+
{% endhighlight %}

如上，包括了 page 的頭信息，佔 38 個字節，像頁類型這樣的元數據信息會保存在該字段中；page trailer 也就是頁末尾的最後 8 個字節；剩餘中間的部分作為 page body，也即頁的實體信息，對這部分來說不同的頁類型包含了不同的內容。

而對應頁內各個 field 的大小，是在 fil0fil.h 頭文件中通過宏進行定義。需要注意的是，很大一部分宏，實際定義的是在頁內的偏移量，可以參考其註釋。

{% highlight c %}
#define FIL_PAGE_DATA       38  /*!< start of the data on the page */
#define FIL_PAGE_DATA_END   8   /*!< size of the page trailer */
{% endhighlight %}

頁中其它的偏移量同樣在 fil0fil.h 頭文件中定義。

#### Header+Tailer

其中，Header 和 Tailer 中保存元信息的詳細內容如下所示：

{% highlight text %}
00000 +-------------------------------------------+
      |                 Checksum (4)              |
00004 +-------------------------------------------+
      |          Offset (Page Number) (4)         |
00008 +-------------------------------------------+
      |          Previous Page Number (4)         |
00012 +-------------------------------------------+
      |            Next Page Number (4)           |
00016 +-------------------------------------------+
      |     LSN for last page modification (8)    |
00024 +-------------------------------------------+
      |               Page Type (2)               |
00026 +-------------------------------------------+
      |     Flush LSN OR version+checksum (8)     |
00034 +-------------------------------------------+
      |               Space ID (4)                |
00038 +-------------------------------------------+
      |                                           |
      ~                 ... ...                   ~
      |                                           |
16376 +-------------------------------------------+
      |          Old-style Checksum (4)           |
16380 +-------------------------------------------+
      |          Low 32 bit of LSN (4)            |
16384 +-------------------------------------------+
{% endhighlight %}

在 Page Header 中，各個字段的佔用空間以及大致的含義如下。

{% highlight text %}
FIL_PAGE_SPACE_OR_CHKSUM    # 4，對於大於4.0.14的版本，存儲的為checksum，否則為0
FIL_PAGE_OFFSET             # 4，頁號page no，一般是在表空間的物理偏移量
FIL_PAGE_PREV               # 4，前一頁的page no，B+tree的葉子節點是通過鏈表串起來，很多類型的頁不需要該值
FIL_PAGE_NEXT               # 4，後一頁的page no
FIL_PAGE_LSN                # 8，更改記錄時最大的redo log lsn，一般用在redo log恢復時使用
FIL_PAGE_TYPE               # 2，page的類型
FIL_PAGE_FILE_FLUSH_LSN     # 8，space文件最後被flush是的redo log lsn，只在第一頁中設置
FIL_PAGE_SPACE_ID           # 4，最後被歸檔的archive log file序號，同樣只在space的第一個頁中被設置
{% endhighlight %}

這裡的頁號 (page no) 實際上是相對於整個表空間的偏移量，表空間可以包含有多個文件，而這個頁號是連續的。在各個頁的頭部中，還保存了前後頁的 page no，從而形成了類似雙像鏈表的結構。

不同類型的頁對應 body 部分的數據結構是不同的，因此需要根據 header 中的頁類型來解析該頁的數據。另外，頁號會在頁初始化完成之後賦值，可以用來校驗通過偏移量讀取的頁是否正確。

對於 tailer 來說，高 4 位是用來存儲 FIL_PAGE_LSN 的部分信息，低 4 位用來表示 page 頁中數據老的 checksum 信息。

#### checksum

在 5.6.3 中，引入了 [innodb_checksum_algorithm](http://dev.mysql.com/doc/refman/en/innodb-parameters.html#sysvar_innodb_checksum_algorithm) 這一變量，用於設置 checksum 的計算方式，默認採用的是 INNODB 。其中 checksum 分別在 header 和 tailer 保存，在 tailer 中保存的是老的方式，後面將會被取消掉。



## Index

在 InnoDB 中，所有的都是通過 Index 表示，包括主表和索引表，該類型的頁同樣在 fil0fil.h 中定義。

{% highlight c %}
#define FIL_PAGE_INDEX      17855   /*!< B-tree node */
{% endhighlight %}

該頁的結構如下。

{% highlight text %}
00000 +-------------------------------+
      |       FIL header (38)         |
00038 +-------------------------------+
      |      INDEX Header (36)        |
00074 +-------------------------------+
      |       FSEG Header (20)        |
00094 +-------------------------------+
      |     System Records (26)       |
00120 +-------------------------------+
      |                               |
      ~        User Records           ~
      |                               |
      +-------------------------------+
 Top  |         Free Space            |
      +-------------------------------+
      |                               |
      ~       Page Directory          ~
      |                               |
16376 +-------------------------------+
      |       FIL Trailer (8)         |
16384 +-------------------------------+
{% endhighlight %}





#### Index Header



{% highlight text %}
38 +-------------------------------+
   |  Number of Directory Slots (2)
40 +-------------------------------+
   | Heap Top Positon (2)          |
42 +-------------------------------+
   | Number of Heap Records / Format Flag (2)
44 +-------------------------------+
{% endhighlight %}


#### System Records

每個 INDEX 頁都包含兩個被稱為 infimum 和 supremum 的系統記錄，而且其偏移量是固定的，分別為 offset 99 以及 offset 112 。

{% highlight text %}
094 +------------------------------------+
    |       Info Flags (4 bits)          |
    +------------------------------------+
    |  Number of Records Owned (4 bits)  |
095 +------------------------------------+
    |          Order (13 bits)           |
    +------------------------------------+
    |        Record Type (3 bits)        |
097 +------------------------------------+
    |       Next Record Offset (2)       |
099 +------------------------------------+
    |           "infimum\0" (8)          |
107 +------------------------------------+
    |       Info Flags (4 bits)          |
    +------------------------------------+
    |  Number of Records Owned (4 bits)  |
108 +------------------------------------+
    |          Order (13 bits)           |
    +------------------------------------+
    |        Record Type (3 bits)        |
110 +------------------------------------+
    |       Next Record Offset (2)       |
112 +------------------------------------+
    |           "supremum\0" (8)         |
120 +------------------------------------+
{% endhighlight %}



rem0rec.cc

## Record

記錄頭信息如下。

{% highlight text %}
094 +------------------------------------+
    |       Info Flags (4 bits)          |
    +------------------------------------+
    |  Number of Records Owned (4 bits)  |
095 +------------------------------------+
    |          Order (13 bits)           |
    +------------------------------------+
    |        Record Type (3 bits)        |
097 +------------------------------------+
    |       Next Record Offset (2)       |
099 +------------------------------------+
    |           "infimum\0" (8)          |
107 +------------------------------------+
    |       Info Flags (4 bits)          |
    +------------------------------------+
    |  Number of Records Owned (4 bits)  |
108 +------------------------------------+
    |          Order (13 bits)           |
    +------------------------------------+
    |        Record Type (3 bits)        |
110 +------------------------------------+
    |       Next Record Offset (2)       |
112 +------------------------------------+
    |   Next Record Offset (2)           |
N   +------------------------------------+
{% endhighlight %}



其中 index page body 由 5 部分組成：body header、recorders、free recorders、free heap 和 page directory，其中 body header 的結構定義如下：




## 實踐

如上，介紹了 InnoDB 存儲的數據格式，接下來實際查看下；本文中在測試的時候，使用的工具可以參考 [innodb_ruby](https://github.com/jeremycole/innodb_ruby) 。

{% highlight text %}
----- 新建表
mysql> create table test.foobar (id int);
Query OK, 0 rows affected (0.05 sec)

----- 新建之後，直接查看狀態
$ innodb_space -f test/foobar.ibd space-page-type-regions
start       end         count       type
0           0           1           FSP_HDR
1           1           1           IBUF_BITMAP
2           2           1           INODE
3           3           1           INDEX
4           5           2           FREE (ALLOCATED)
{% endhighlight %}

此時，只分配了標準的空間頁，總共 6 Pages，包括了 FSP_HDR、IBUF_BITMAP、INODE、ROOT INDEX page，而且還有兩個已經分配的未使用的頁。







## 參考

關於表空間的介紹可以參考 PPT [InnoDB Internals: InnoDB File Formats and Source Code Structure](/reference/mysql/InnoDBFileFormat.pdf) 的介紹，上面的很多圖片是直接從該 PPT 複製下來的。

關於 InnoDB 的文件格式，詳細內容可以參考 Jeremy Cole 的一系列文章 [InnoDB internals, structures, and behavior](https://blog.jcole.us/innodb/) ，對文件存儲格式講解的十分詳細。





<!--
http://www.itpub.net/thread-1813772-1-1.html
http://blog.csdn.net/yuanrxdu/article/details/41925279
http://blog.csdn.net/yuanrxdu/article/details/4221598
http://www.linuxidc.com/Linux/2016-03/128827.htm
http://www.cnblogs.com/zhoujinyi/archive/2012/10/17/2726462.html

innodb文件類型格式整理，故障恢復
https://github.com/chhabhaiya/undrop-for-innodb
http://www.askmaclean.com/archives/mysql-recover-innodb.html


Chapter 22 InnoDB Storage Engine
https://dev.mysql.com/doc/internals/en/innodb.html

MySQL · 引擎特性 · InnoDB文件系統管理
https://yq.aliyun.com/articles/5586
-->

