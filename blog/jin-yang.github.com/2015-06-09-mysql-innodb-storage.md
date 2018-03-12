---
title: InnoDB 存储空间
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,innodb,表空间,table space
description: InnoDB 表空间 (table space) 用来组织存储保存的数据，本文中对表空间管理进行分析。
---

InnoDB 表空间 (table space) 用来组织存储保存的数据，本文中对表空间管理进行分析。

<!-- more -->

## 简介

InnoDB 实现的表空间是在文件系统之上又构建的一层逻辑存储的空间管理，每个表空间在逻辑上划分为了如下的几层结构，依次包括 table space、segment inode、extent、page、record 。

![innodb files](/images/databases/mysql/innodb-tablespace.png){: .pull-center }

如图所示，InnoDB 最小读写单位是 Page；为了高效管理，又将连续的 64 个页称为 "extent"，而且在一个表空间中，通常是以 "extent" 为单位进行资源分配。

除此之外，还在 extents 上层添加了一个 segment inode 管理，最多可以管理 256 个 extents。如果 page 采用默认的 16K，那么对应的 extent 大小为 64*16K=1M，而 segment 对应 256M 。

### 参数设置

首先确认几个与表空间管理相关的参数，也就是打开独立表空间，且采用默认的页大小为 16K 。

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

在查看表空间时可以使用几个常用的工具，可以参考 [innodb_page_info](/reference/databases/mysql/innodb_page_info) 。

{% highlight text %}
----- 查看每个页的详细信息
$ innodb_page_info -v dept_emp.ibd

{% endhighlight %}


## 文件格式详解

InnoDB 的存储文件格式如上图所示，我们直接从大到小介绍其结构，依次从 space file -> segment -> extent -> page -> row 逐一介绍。

### Space File 格式

Space File 主要包括了两类：系统空间 (system space) 和 表空间 (Per-table space files)。一个表空间文件，实际就是一系列 Pages 的组合 (最大 2<sup>32</sup>)，为了方便管理，你可以理解为做了三级的划分，分别是 segment、extent、page 。

其基本结构如下所示：

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

对于 Page0 肯定是 "File SPace HeaDeR"，其中维护了 extent 相关的信息，例如那些是空闲的、满的、有碎片的 (fragmented)。在 FSP_HDR 页中，只能维护 256 个 extent 相关的信息 (16,384 pages==256MiB)，这也就导致每隔 256M 都需要有一个 XDES 来管理接下来的 extent 元信息。

<!--
The third page in each space (page 2) will be an INODE page, which is used to store lists related to file segments (groupings of extents plus an array of singly-allocated “fragment” pages). Each INODE page can store 85 INODE entries, and each index requires two INODE entries. (A more detailed discussion of INODE entries and file segments is reserved for a future post.)

Alongside each FSP_HDR or XDES page will also be an IBUF_BITMAP page, which is used for bookkeeping information related to insert buffering, and is outside the scope of this post.
-->


#### 系统空间

在系统空间中的固定位置保存了与 InnoDB 相关的关键信息，其中的 FSP_HDR、IBUF_BITMAP、INODE 三个页都是相同的，只是之后的页会有区别。

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
Page3 以下的页定义为：

Page 3, type SYS: Headers and bookkeeping information related to insert buffering.
Page 4, type INDEX: The root page of the index structure used for insert buffering.
Page 5, type TRX_SYS: Information related to the operation of InnoDB’s transaction system, such as the latest transaction ID, MySQL binary log information, and the location of the double write buffer extents.
Page 6, type SYS: The first rollback segment page. Additional pages (or whole extents) are allocated as needed to store rollback segment data.
Page 7, type SYS: Headers related to the data dictionary, containing root page numbers for the indexes that make up the data dictionary. This information is required to be able to find any other indexes (tables), as their root page numbers are stored in the data dictionary itself.
Pages 64-127: The first block of 64 pages (an extent) in the double write buffer. The double write buffer is used as part of InnoDB’s recovery mechanism.
Pages 128-191: The second block of the double write buffer.

All other pages are allocated on an as-needed basis to indexes, rollback segments, undo logs, etc.
-->

#### 表空间 Per-table Space Files

这需要打开 innodb_file_per_table 变量之后才会生效。

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

在 segment 中保存了与 extent 相关的元数据，实际上是在空间文件的固定位置保存了 extent 的元数据信息，其格式为 FSP_HDR/XDES，其结构也非常简单，在 Page Body 中保存了 256 个 extent descriptors 。

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

该页的类型同样在 fil0fil.h 中定义。

{% highlight c %}
#define FIL_PAGE_INODE      3   /*!< Index node */
{% endhighlight %}

该类型的页定义为 inode，这与文件系统中的 inode 概念有点混淆，实际上与文件系统中的概念完全不同，在 InnoDB 中只是描述了与 extent 相关的信息。

### Extents and extent descriptors

extent 是为了更加方便的管理 page，也可以称为簇或者区，对于 16KiB 大小的 page 一个 extent 管理 64 个，其定义在 fsp0types.h 文件中定义，不同大小的页，对应的 extent 也有所不同。

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

extent 是通过一个叫簇描述符 (extent descriptor) 来表示的，对应了上述的 XDES Entry，详细格式如下：

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

详细的宏定义在 fsp0fsp.h 头文件中。

{% highlight c %}
#define XDES_ID                            0
#define XDES_FLST_NODE                     8
#define XDES_STATE      (FLST_NODE_SIZE + 8)
#define XDES_BITMAP    (FLST_NODE_SIZE + 12)

#define XDES_SIZE (XDES_BITMAP + UT_BITS_IN_BYTES(FSP_EXTENT_SIZE * XDES_BITS_PER_PAGE))
{% endhighlight %}

每个 extent descriptor 会管理 64 个 pages，最后的 16Bytes 用来标识这 64 个 page 的状态，如空闲、半空闲，主要目的是为了能够快速从这 64 个 page 中找到空闲的 page。


<!--
XDES_ID 存放该extent所属segment的id

XDES_FLST_NODE实际是一个文件链表节点，这样可以把extent descriptor组织成一个链表，实际上我们在extent更上层管理extent时会把extent链在一起组成链表，那么实际链表的节点就是extent descriptor的这个字段，比如某个segment的extent free list和表空间的extent frag list都是通过这个节点组织extent链表的，这个后续详细说明

XDES_STATE标识该extent descriptor所代表的extent的所属状态，定义如下：

/* States of a descriptor */
#define    XDES_FREE     1    /* extent is in free list of space */
#define    XDES_FREE_FRAG     2    /* extent is in free fragment list of space */
#define    XDES_FULL_FRAG     3    /* extent is in full fragment list of space */
#define    XDES_FSEG     4    /* extent belongs to a segment */
-->


### Page

页是 InnoDB 中的最小的逻辑存储单位，很多基本操作都是以页为单位进行，在 ```include/univ.i``` 文件中通过宏指定，默认大小是 16K。

{% highlight c %}
#define UNIV_PAGE_SIZE      ((ulint) srv_page_size)
{% endhighlight %}

而变量 ```srv_page_size``` 的大小，实际对应了 ```innodb_page_size``` 参数。与页相关的宏定义基本都在 fil0fil.h 头文件中，其中定义了多种页类型，其中比较常见的举例如下：

{% highlight c %}
#define FIL_PAGE_INDEX      17855    // 数据索引页
#define FIL_PAGE_UNDO_LOG   2        // 事务回滚日志页
#define FIL_PAGE_INODE      3        // inode页，用来管理segment
{% endhighlight %}

如上所述，默认每个页的大小为 16K=16384Byte，每个页通过一个 int32 类型的值标识其在表空间中页的偏移量，也就是说默认的表空间最大是 2<sup>32</sup> x 16 KiB = 64 TiB 。

#### 页格式

所有 page 的格式都是相同的，由 page_header、page_body、page_trailer 三部分组成。

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

如上，包括了 page 的头信息，占 38 个字节，像页类型这样的元数据信息会保存在该字段中；page trailer 也就是页末尾的最后 8 个字节；剩余中间的部分作为 page body，也即页的实体信息，对这部分来说不同的页类型包含了不同的内容。

而对应页内各个 field 的大小，是在 fil0fil.h 头文件中通过宏进行定义。需要注意的是，很大一部分宏，实际定义的是在页内的偏移量，可以参考其注释。

{% highlight c %}
#define FIL_PAGE_DATA       38  /*!< start of the data on the page */
#define FIL_PAGE_DATA_END   8   /*!< size of the page trailer */
{% endhighlight %}

页中其它的偏移量同样在 fil0fil.h 头文件中定义。

#### Header+Tailer

其中，Header 和 Tailer 中保存元信息的详细内容如下所示：

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

在 Page Header 中，各个字段的占用空间以及大致的含义如下。

{% highlight text %}
FIL_PAGE_SPACE_OR_CHKSUM    # 4，对于大于4.0.14的版本，存储的为checksum，否则为0
FIL_PAGE_OFFSET             # 4，页号page no，一般是在表空间的物理偏移量
FIL_PAGE_PREV               # 4，前一页的page no，B+tree的叶子节点是通过链表串起来，很多类型的页不需要该值
FIL_PAGE_NEXT               # 4，后一页的page no
FIL_PAGE_LSN                # 8，更改记录时最大的redo log lsn，一般用在redo log恢复时使用
FIL_PAGE_TYPE               # 2，page的类型
FIL_PAGE_FILE_FLUSH_LSN     # 8，space文件最后被flush是的redo log lsn，只在第一页中设置
FIL_PAGE_SPACE_ID           # 4，最后被归档的archive log file序号，同样只在space的第一个页中被设置
{% endhighlight %}

这里的页号 (page no) 实际上是相对于整个表空间的偏移量，表空间可以包含有多个文件，而这个页号是连续的。在各个页的头部中，还保存了前后页的 page no，从而形成了类似双像链表的结构。

不同类型的页对应 body 部分的数据结构是不同的，因此需要根据 header 中的页类型来解析该页的数据。另外，页号会在页初始化完成之后赋值，可以用来校验通过偏移量读取的页是否正确。

对于 tailer 来说，高 4 位是用来存储 FIL_PAGE_LSN 的部分信息，低 4 位用来表示 page 页中数据老的 checksum 信息。

#### checksum

在 5.6.3 中，引入了 [innodb_checksum_algorithm](http://dev.mysql.com/doc/refman/en/innodb-parameters.html#sysvar_innodb_checksum_algorithm) 这一变量，用于设置 checksum 的计算方式，默认采用的是 INNODB 。其中 checksum 分别在 header 和 tailer 保存，在 tailer 中保存的是老的方式，后面将会被取消掉。



## Index

在 InnoDB 中，所有的都是通过 Index 表示，包括主表和索引表，该类型的页同样在 fil0fil.h 中定义。

{% highlight c %}
#define FIL_PAGE_INDEX      17855   /*!< B-tree node */
{% endhighlight %}

该页的结构如下。

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

每个 INDEX 页都包含两个被称为 infimum 和 supremum 的系统记录，而且其偏移量是固定的，分别为 offset 99 以及 offset 112 。

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

记录头信息如下。

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



其中 index page body 由 5 部分组成：body header、recorders、free recorders、free heap 和 page directory，其中 body header 的结构定义如下：




## 实践

如上，介绍了 InnoDB 存储的数据格式，接下来实际查看下；本文中在测试的时候，使用的工具可以参考 [innodb_ruby](https://github.com/jeremycole/innodb_ruby) 。

{% highlight text %}
----- 新建表
mysql> create table test.foobar (id int);
Query OK, 0 rows affected (0.05 sec)

----- 新建之后，直接查看状态
$ innodb_space -f test/foobar.ibd space-page-type-regions
start       end         count       type
0           0           1           FSP_HDR
1           1           1           IBUF_BITMAP
2           2           1           INODE
3           3           1           INDEX
4           5           2           FREE (ALLOCATED)
{% endhighlight %}

此时，只分配了标准的空间页，总共 6 Pages，包括了 FSP_HDR、IBUF_BITMAP、INODE、ROOT INDEX page，而且还有两个已经分配的未使用的页。







## 参考

关于表空间的介绍可以参考 PPT [InnoDB Internals: InnoDB File Formats and Source Code Structure](/reference/mysql/InnoDBFileFormat.pdf) 的介绍，上面的很多图片是直接从该 PPT 复制下来的。

关于 InnoDB 的文件格式，详细内容可以参考 Jeremy Cole 的一系列文章 [InnoDB internals, structures, and behavior](https://blog.jcole.us/innodb/) ，对文件存储格式讲解的十分详细。





<!--
http://www.itpub.net/thread-1813772-1-1.html
http://blog.csdn.net/yuanrxdu/article/details/41925279
http://blog.csdn.net/yuanrxdu/article/details/4221598
http://www.linuxidc.com/Linux/2016-03/128827.htm
http://www.cnblogs.com/zhoujinyi/archive/2012/10/17/2726462.html

innodb文件类型格式整理，故障恢复
https://github.com/chhabhaiya/undrop-for-innodb
http://www.askmaclean.com/archives/mysql-recover-innodb.html


Chapter 22 InnoDB Storage Engine
https://dev.mysql.com/doc/internals/en/innodb.html

MySQL · 引擎特性 · InnoDB文件系统管理
https://yq.aliyun.com/articles/5586
-->

