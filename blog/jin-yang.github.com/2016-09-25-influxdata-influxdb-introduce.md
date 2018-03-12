---
title: InfluxDB
layout: post
comments: true
language: chinese
category: [linux,golang,database]
keywords: influxdb,telegraf,influxdata,时序数据库
description: InfluxDB 是一个开源分布式时序、事件和指标数据库，使用 Go 语言编写，无需外部依赖，其设计目标是实现分布式和水平伸缩扩展。InfluxData 提供了 TICK 一套解决方案，不过使用比较多的是 InfluxDB，在此就介绍下 InfluxDB 以及 Telegraf 。
---

InfluxDB 是一个开源分布式时序、事件和指标数据库，使用 Go 语言编写，无需外部依赖，其设计目标是实现分布式和水平伸缩扩展。

InfluxData 提供了 TICK 一套解决方案，不过使用比较多的是 InfluxDB，在此就介绍下 InfluxDB 以及 Telegraf 。

<!-- more -->

![influxdb]({{ site.url }}/images/databases/influxdb/influxdb-golang.png "influxdb"){: .pull-center width="90%" }

## 简介

在对时序数据进行存储时，常见的有多种格式：A) 使用文件 (如: RRD, Whisper)；B) 使用 LSM 树 (如: LevelDB, RocksDB, Cassandra)；C) 使用 B-Tree 排序和 k/v 存储 (如: BoltDB, LMDB) 。

如下，就是 InfluxDATA 提供的一整套监控解决方案，包括了数据采集 (Telegraf)、数据存储 (InfluxDB)、数据展示 (Chronograf) 以及报警 (Kapacitor) 。

![TICK]({{ site.url }}/images/databases/influxdb/tick-stack-grid.jpg "TICK"){: .pull-center width="60%" }

而实际上，使用比较多的是 InfluxDB ，这是一个时序数据库，关于时序数据库的排名，可以直接在 [db-engines.com](http://db-engines.com/en/ranking/time+series+dbms) 查看，InfluxDB 的活跃度基本排在第一。

除了上述 InfluxDATA 提供的解决方案之外，通常还可以使用如下的方案。

{% highlight text %}
采集数据 (collectd) ==>>==>> 存储数据 (InfluxDB) ==>>==>> 显示数据 (Grafana)
{% endhighlight %}

0.8.4 版本之前只能通过 [influxdb-collectd-proxy](https://github.com/hoonmin/influxdb-collectd-proxy) 采集 collectd 上报的数据，之后 InfluxDB 自动提供了接口；当然，默认是不开启的，需要在配置文件中设置。

在此，还是重点介绍下 InfluxDB 。

#### 特性

简单列举下 InfluxDB 所支持的特性：

* 支持 Regular Timeseries 以及 Irregular Timeseries，前者是指时间间隔固定，后者指不固定，例如报警、入侵事件等；
* 可以将秒级监控在后台转换为分钟级，减小存储空间 (Continuous Queries)；
* 采用 Schemaless ，列存储，压缩率高，可以存储任意数量的列；
* 提供 min, max, sum, count, mean 等聚合函数；
* 使用类 SQL 语句；
* Built-in Explorer 自带管理工具，默认不打开，需要在配置文件中配置；
* Native HTTP API，采用内置 HTTP 服务 (Protobuf API 暂时不提供)。

<!--
Protocol Buffers 是一种轻便高效的结构化数据存储格式，可以用于结构化数据串行化，很适合做数据存储或RPC 数据交换格式。
influxdb/tsdb/internal/meta.proto
-->

### 常用概念

简单列举下常见的概念。

##### Database

数据库，可以创建多个，不同数据库中的数据文件存放在磁盘上的不同目录。

##### Measurement

数据库中的表，例如 memory 记录了内存相关统计，其中包括了 used、cached、free 等。

##### Point

表里的一行数据，由 A) 时间戳 (timestamp，每条记录的时间，数据库的主索引，会自动生成)；B) 数据 (field，记录的 Key/Value 例如温度、湿度)；C) 标签 (tags，有索引如地区、海拔)组成。

##### Tag/tag key/tag value

有点不太好解释，可以理解为标签，或者二级索引，例如采集机器的 CPU 信息的时候，可以设置两个 tag 分别是机器 IP 以及第几个 CPU，在查询的时候放在 where 条件中，从而不需要遍厉整个表，这是一个 ```map[stirng]string``` 结构。

##### Fields

也就是实际记录的数据值，是 ```map[string]interface{}``` 类型，类似于 C 语言中的 void * ，这里的 ```interface{}``` 可以是 int、int32、int64、float32、float64，也就是真正需要显示或者计算的值，例如 CPU 的 sys, user, iowait 等。

##### Retention Policy

存储策略，InfluxDB 会自动清理数据，可以设置数据保留的时间，默认会创建存储策略 autogen (保留时间为永久)，之后用户可以自己设置，例如保留最近 30 天的数据。

<!-- Series<br>measurement, retention policy, tag set 的集合。 -->

{% highlight text %}
                       +----------+
                   +-->|  series  |
                   |   +----------+
+-------------+    |   +----------+
| measurement | ---+-->|  series  |
+-------------+    |   +----------+
                   |   +----------+
                   +-->|  series  |
                       +----------+
{% endhighlight %}

### 安装

实际上，编译好的二进制程序，可以直接从官网 [www.influxdata.com/downloads/](https://www.influxdata.com/downloads/) 上下载，例如，在 CentOS 中，可以通过如下方式安装，安装后数据默认保存在 /var/lib/influxdb 目录下。

{% highlight text %}
----- 下载二进制安装包
$ wget https://dl.influxdata.com/influxdb/releases/influxdb-1.1.1.x86_64.rpm
----- 查看二进制包的内容
$ rpm -qpl influxdb-1.1.1.x86_64.rpm
----- 解压二进制包
$ rpm2cpio influxdb-1.1.1.x86_64.rpm | cpio -div
----- 安装二进制包
# rpm -ivh influxdb-1.1.1.x86_64.rpm
----- 启动服务，可以查看/usr/lib/systemd/system/influxdb.service文件
# systemctl start influxdb
{% endhighlight %}

安装完之后，可以看到如下的可执行文件。

* influxd，服务器，可以直接通过该命令启动；
* influx，InfluxDB 命令行客户端，可链接服务器，执行一些常见的命令；
* influx_inspect，用于查看磁盘 shards 上的详细信息；
* influx_stress，压力测试工具；
* influx_tsm，数据库转换工具，可以将数据库从 b1 或 bz1 格式转换为 tsm1 格式。


### 源码编译

当然，主要介绍下 influxdb 和 telegraf 的源码编译。

#### build.py

在源码中，有一个编译脚本 build.py，可以通过该脚本进行编译、打包等操作。

实际上，在 build.py 脚本中，会通过 git 检查相应的版本 (详见 get_current_XXX 类函数)，所以需要保证这是一个 git 版本库，也就是说，如果通过该工具编译，需要通过 git clone 下载源码。

另外，代码库的依赖是通过 Go Dependancy Manager, GDM 管理的，当然如果没有安装，则会自动安装；然后下载依赖的代码库；最后，会直接调用 go install 安装。

> TIP: 如果无法直接连接到网络，可以将 get_current_XXX()、go_get()、local_changes() 返回正常的值即可，例如 get_current_commit() 返回 "foobar" ，go_get() 返回 true 等等；当然，此时，依赖的代码库需要手动下载 (执行go install时会显示依赖包)。

编译后的程序，会最终安装到 $GOPATH/bin/ 目录下；也可以打包成 rpm 包，不过没仔细研究，后面再看看。


#### influxdb

对于 InfluxDB 可以通过如下方式安装。

{% highlight text %}
----- 1. 下载相关的分支
$ git clone -b v1.1.1 https://github.com/influxdata/influxdb.git .
----- 2. 直接进行编译，如果并非通过git下载源码，需要如上修改build.py文件
$ ./build.py
----- 3. 直接复制到$GOPATH/bin目录下即可
$ cp build/* $GOPATH/bin
{% endhighlight %}

<!--
或者如下的方式，不过安装的时候有问题，暂不确认
go get github.com/influxdata/influxdb
cd $GOPATH/src/github.com/influxdata/
go get ./...
go install ./...
-->


#### telegraf

对于 telegraf 可以直接使用 make 安装，对于其中一些依赖库，如 golang.org/x/crypto ，可能会导致无法下载，可以直接从 github 上下载，然后删除 Godeps 相关依赖。

另外，通过上述方法下载时，需要使用 git clone 下载，否则会由于缺少 .git 目录导致报错；这也意味着，如果可以确保相关的依赖都已经下载之后，直接将 build.py 脚本中的 go_get() 注释掉即可，这样就不会通过 gdm 下载依赖包了。

{% highlight text %}
$ go get github.com/influxdata/telegraf
$ cd $GOPATH/src/github.com/influxdata/telegraf
$ make
{% endhighlight %}

如官方所述，该工具是插件式的，这也就导致其在编译时，会依赖很多的插件，如果只需要部分插件，可以在 plugins 目录下，修改相应分类的 all/all.go 文件，将不需要的注释掉即可。

其实，主要修改 ```plubins/{inputs,outputs}/all/all.go``` 即可。

<!--
telegraf开发：
通过源码进行编译，执行 script/build.py，该脚本依赖git+gdm，如果非联网，则需要手动下载安装插件，实际其执行命令如下。
GOOS=linux GOARCH=amd64 go build -o /home/jinyang/workspace/src/github.com/influxdata/telegraf/build/telegraf -ldflags="-X main.version=foobar_version -X main.branch=foobar_branch -X main.commit=foobar_commit" ./cmd/telegraf

生成配置文件：
     telegraf -sample-config -input-filter cpu:mem -output-filter influxdb > telegraf.conf
-->

#### tips

需要注意的是，通过 gdm 安装依赖时，可能会由于墙 (你懂得) 部分包无法下载，但是现象是 hang 住 :-( 所以，可以从 github 上手动下载，一般都会有镜像的。


###  配置文件

默认使用 ```/etc/influxdb/influxdb.conf``` ，默认会使用如下的端口：

* 8083: Web-Admin管理服务的端口，如果是本地可以直接通过 [http://localhost:8083](http://localhost:8083) 打开，不过默认没有开启，需要在配置文件中打开；
* 8086: HTTP-API的端口，用于接收请求和发送数据；
* 8088: 集群端口，用于 InfluxDB 集群通讯使用，不过 0.11.1 版本之后就不再提供集群化的解决方案了，只有企业版才提供该功能；


<!--
http://www.xusheng.org/blog/2016/08/12/influxdb-relay-performance-bottle-neck-analysing/
-->

简单列举下常见的配置。

{% highlight text %}
# 管理面需要打开如下配置
[admin]
enabled = true
bind-address = ":8083"

# 如果不指定，则会通过os.hostname()通过系统获取，可能会报错
hostname = '192.168.1.23'
# 默认每隔24小时会向usage.influxdata.com发送一些统计数据，可以关闭掉
reporting-disabled = true
{% endhighlight %}


### Python访问

通过 Python 访问 InfluxDB 需要安装 [InfluxDB Python](https://pypi.python.org/pypi/influxdb/)，然后通过如下方式访问即可。

{% highlight text %}
$ python
>>> from influxdb import InfluxDBClient
>>> json_body = [
    {
        "measurement": "cpu_load_short",
        "tags": {
            "host": "server01",
            "region": "us-west"
        },
        "time": "2009-11-10T23:00:00Z",
        "fields": {
            "value": 0.64
        }
    }
]
>>> client = InfluxDBClient('localhost', 8086, 'root', 'root', 'example')
>>> client.create_database('example')
>>> client.write_points(json_body)
>>> result = client.query('select value from cpu_load_short;')
>>> print("Result: {0}".format(result))
{% endhighlight %}

### 常用操作

在 InfluxDB 中，写入数据采用行格式，可以粗略的将要存入的一条数据看作一个虚拟的 key 和其对应的 value (field value)，格式如下：

{% highlight text %}
<measurement>[,<tag-key>=<tag-value>...] <field-key>=<field-value>[,<field2-key>=<field2-value>...] [timestamp]

cpu,host=serverA,region=west value=0.64   服务器默认时间
temperature,zipcode=384250,province=zhejiang value=75,humidity=20 1434067467000000000
{% endhighlight %}
<!--
payment,device=mobile,product=Notepad,method=credit billed=33,licenses=3i 1434067467100293230
stock,symbol=AAPL bid=127.46,ask=127.48
-->

常见操作列举如下，可以通过 \-precision 参数指定时间格式以及精度，例如 rfc3339 。

{% highlight text %}
----- 写入数据
$ curl -i -XPOST 'http://localhost:8086/write?db=testDB'
    --data-binary 'weather,altitude=1000,area=北 temperature=11,humidity=-4'
$ curl -i -XPOST 'http://localhost:8086/write?db=testDB&precision=s'
    --data-binary 'weather,altitude=1000,area=北 temperature=11,humidity=-4'
influx> INSERT weather,altitude=1000,area=北 temperature=11,humidity=-4;

----- 查询
$ curl -G 'http://localhost:8086/query?pretty=true' --data-urlencode "db=mydb" -precision 'rfc3339'
    --data-urlencode "q=SELECT * FROM weather ORDER BY time DESC LIMIT 3"
$ curl -G 'http://localhost:8086/query?u=readonly&p=password&pretty=true' --data-urlencode "db=mydb"
    --data-urlencode "q=SELECT * FROM weather ORDER BY time DESC LIMIT 3"

----- 设置过期策略
$ curl -G http://localhost:8086/query --data-urlencode
    "q=CREATE RETENTION POLICY bar ON foo DURATION 300d REPLICATION 3 DEFAULT"
{% endhighlight %}

另外，支持 JSON 格式写入、GZIP 压缩数据插入及查询，详细可以参考源码中的 tests 目录。

## 运维操作

基本的数据保存目录如下。

{% highlight text %}
.
├── data
│   ├── _internal
│   │   └── monitor
│   │       ├── 16   Shard-ID
│   │       │   └── 000000004-000000003.tsm
│   │       └── 3
│   └── telegraf
│       └── autogen  存储策略
│           ├── 12
│           │   └── 000000132-000000002.tsm
│           └── 7
│               └── 000000003-000000002.tsm
├── meta
│   └── meta.db    元数据
└── wal
    ├── _internal
    │   └── monitor
    │       ├── 16
    │       │   └── _00015.wal
    │       └── 3
    │           ├── _00001.wal
    │           ├── _00002.wal
    │           ├── _00003.wal
    │           └── _00006.wal
    └── telegraf
        └── autogen
            ├── 12
            │   └── _00395.wal
            └── 7
                └── _00010.wal
{% endhighlight %}

### 系统监控

可以查看 [官方文档](https://docs.influxdata.com/influxdb/v1.1/troubleshooting/statistics/)，关于 GoLang 的内部运行状态，可以参考 [https://golang.org/pkg/runtime](https://golang.org/pkg/runtime/) 。

{% highlight text %}
----- InfluxDB本身的监控统计数据，保存在内存中，重启后会丢失
SHOW STATS FOR <'module'>

----- 包括编译信息、主机名、系统配置、启动时间、内存使用率、GoLang运行环境信息
SHOW DIAGNOSTICS FOR <'module'>

----- 内部监控进程保存的数据，包含了各种监控的历史数据
_internal
{% endhighlight %}

### 元数据查看

仅简单列举一下常见的命令，详细可以查看 [官方文档](https://docs.influxdata.com/influxdb/v1.1/query_language/schema_exploration/) 。

{% highlight text %}
SHOW SERIES;
SHOW SERIES ON telegraf FROM mysql;

SHOW FIELD KEYS;
SHOW FIELD KEYS ON telegraf FROM cpu;

SHOW MEASUREMENTS
{% endhighlight %}

### 用户权限管理

InfluxDB 的权限设置比较简单，只有读、写、ALL 三种，详细参考 [官方文档](https://docs.influxdata.com/influxdb/v1.1/query_language/authentication_and_authorization/) 。默认不开启用户认证，需要修改配置文件：

{% highlight text %}
[http]
auth-enabled = true
{% endhighlight %}

常见命令如下：

{% highlight text %}
----- 授权
GRANT [READ,WRITE,ALL] ON <database_name> TO <username>
GRANT ALL PRIVILEGES TO "username"

----- 撤销权限
REVOKE [READ,WRITE,ALL] ON <database_name> FROM <username>
REVOKE ALL PRIVILEGES FROM "username"

----- 查看权限
SHOW GRANTS FOR <user_name>

----- 显示用户
SHOW USERS

----- 创建用户
CREATE USER "readonly" WITH PASSWORD 'password'

----- 创建管理员权限的用户
CREATE USER "readonly" WITH PASSWORD 'password' WITH ALL PRIVILEGES

----- 删除用户
DROP USER "readonly"

----- 修改密码
SET PASSWORD FOR <username> = '<password>'
{% endhighlight %}

### 备份恢复

只支持全量备份，不支持增量，包括了元数据以及增量数据的备份，可以参考 [官方文档](https://docs.influxdata.com/influxdb/v1.1/administration/backup_and_restore/) 。

{% highlight text %}
----- 元数据备份
$ influxd backup <path-to-backup>

----- 数据备份
$ influxd backup -database <mydatabase> <path-to-backup>
$ influxd backup -database telegraf -retention autogen -since 2016-02-01T00:00:00Z /tmp/backup
$ influxd backup -database mydatabase -host 10.0.0.1:8088 /tmp/remote-backup

----- 恢复
$ influxd restore -metadir /var/lib/influxdb/meta /tmp/backup
$ influxd restore -database telegraf -datadir /var/lib/influxdb/data /tmp/backup
{% endhighlight %}

### 数据保存策略

也就是 Retention Policies，可以设置保存的时间，例如保存 30 天。

{% highlight text %}
----- 查询
SHOW RETENTION POLICIES ON "database_name";

----- 新建
CREATE RETENTION POLICY "rp_name" ON "db_name" DURATION 30d REPLICATION 1 DEFAULT;

----- 修改
ALTER RETENTION POLICY "rp_name" ON db_name DURATION 3w DEFAULT;

----- 删除
DROP RETENTION POLICY "rp_name" ON "db_name";
{% endhighlight %}

### 连续查询

也就是 Continuous Queries，当数据超过保存策略里指定的时间之后，就会被删除；可以通过连续查询把原来的秒级数据，保存为分钟级或者小时级的数据，从而减小数据的占用空间。

{% highlight text %}
----- 查看
SHOW CONTINUOUS QUERIES;

----- 创建
CREATE CONTINUOUS QUERY cq-name ON db-name BEGIN
    SELECT mean(tbl-name) INTO newtbl-name FROM tbl-name GROUP BY time(30m) END;

----- 删除
DROP CONTINUOUS QUERY <cq-name> ON <db-name>;
{% endhighlight %}

### 其它

1\. 获取最近更新数据，并转换为当前时间

{% highlight text %}
select threads_running from mysql order by time desc limit 1;
date -d @`echo 1483441750000000000 | awk '{print substr($0,1,10)}'` +"%Y-%m-%d %H:%M:%S"
{% endhighlight %}

2\. 检查系统是否存活

{% highlight text %}
$ curl -sl -I localhost:8086/ping
{% endhighlight %}

3\. 常用操作

{% highlight text %}
----- 简单查询
SELECT * FROM weather ORDER BY time DESC LIMIT 3;

----- 指定时间范围，时间格式也可以为'2017-01-03 00:00:00'
SELECT usage_idle FROM cpu WHERE time >= '2017-01-03T12:40:38.708Z' AND time <= '2017-01-03T12:40:50.708Z';

----- 最近40min内的数据
SELECT * FROM mysql WHERE time >= now() - 40m;

----- 最近5分钟的秒级差值
SELECT derivative("queries", 1s) AS "queries" from "mysql" where time > now() - 5m;

----- 最近5min的秒级写入
$ influx -database '_internal' -precision 'rfc3339'
      -execute 'select derivative(pointReq, 1s) from "write" where time > now() - 5m'

----- 也可以通过日志查看
$ grep 'POST' /var/log/influxdb/influxd.log | awk '{ print $10 }' | sort | uniq -c
$ journalctl -u influxdb.service | awk '/POST/ { print $10 }' | sort | uniq -c
{% endhighlight %}

<!--
存储引擎 (Storage Engine)： https://docs.influxdata.com/influxdb/v1.1/concepts/storage_engine/

进程运行时会启动多个进程，可以通过 pstree -p PID 查看，其中包括了一个 HTTPD 服务进程，也就对应了 services/httpd 目录下的代码。

以查询请求为例，实际的会路由到 handler.go 中指定的处理函数，也就是 serveQuery() 函数，而该函数中的处理逻辑基本相同：

A) 解析SQL；B) 执行；C) 返回结果。
-->

## 存储引擎






Bolt 是一个 Go 语言编写的嵌入式 KV 数据库，提供了一个简单可靠的方式做数据持久化，按照作者在 [Github BoltDB](https://github.com/boltdb/bolt) 中的介绍：

{% highlight text %}
Bolt is a pure Go key/value store inspired by Howard Chu's LMDB project. The goal
of the project is to provide a simple, fast, and reliable database for projects
that don't require a full database server such as Postgres or MySQL.
{% endhighlight %}

与 LevelDB 有所区别，BoltDB 支持完全可序列化的 ACID 事务；而且，提供稳定的 API 接口，而非类似 SQLite 的 SQL 接口；从而也就意味着它更加方便地整合到其它系统。


<!--
BoltDB将数据保存在一个单独的内存映射的文件里。它没有wal、线程压缩和垃圾回收；它仅仅安全地处理一个文件。
LevelDB和BoltDB的不同

LevelDB是Google开发的，也是一个k/v的存储数据库，和BoltDB比起起来有很大的不同。对于使用者而言，最大的不同就是LevelDB没有事务。在其内部，也有很多的不同：LevelDB实现了一个日志结构化的merge tree。它将有序的key/value存储在不同文件的之中，并通过“层级”把它们分开，并且周期性地将小的文件merge为更大的文件。这让其在随机写的时候会很快，但是读的时候却很慢。这也让LevelDB的性能不可预知：但数据量很小的时候，它可能性能很好，但是当随着数据量的增加，性能只会越来越糟糕。而且做merge的线程也会在服务器上出现问题。LevelDB是C++写的，但是也有些Go的实现方式，如syndtr/goleveldb、leveldb-go。

BoltDB使用一个单独的内存映射的文件，实现一个写入时拷贝的B+树，这能让读取更快。而且，BoltDB的载入时间很快，特别是在从crash恢复的时候，因为它不需要去通过读log（其实它压根也没有）去找到上次成功的事务，它仅仅从两个B+树的根节点读取ID。
-->

### 安装、使用

在设置好 golang 的环境变量之后，可以很简单的通过 go get 获取源码并安装到 $GOPATH/bin 目录下，其命令如下：

{% highlight text %}
$ go get github.com/boltdb/bolt/...
{% endhighlight %}

<!--

使用Bolt
打开数据库并初始化事务

Bolt将所有数据都存储在一个文件中，这让它很容易使用和部署，用户肯定很高兴地发现他们根本不需要去配置数据库或是要DBA去维护它，我们需要对这文件所做的就是打开他们 ，如果你想要打开的文件不存在就会新建。

在这个例子中我们blog.db这个数据库在当前文件夹：

package main

import (
    "log"

    "github.com/boltdb/bolt"
)

func main() {
    db, err := bolt.Open("blog.db", 0600, nil)
    if err != nil {
        log.Fatal(err)
    }
    defer db.Close()

    // ...
}

在你打开之后，你有两种处理它的方式：读-写和只读操作，读-写方式开始于db.Update方法：

err := db.Update(func(tx *bolt.Tx) error {
    // ...read or write...
    return nil
})

可以看到，你传入了db.update函数一个参数，在函数内部你可以get/set数据和处理error。如果返回为nil，事务就会从数据库得到一个commit，但是如果你返回一个实际的错误，则会做回滚，你在函数中做的任何事情都不会commit到磁盘上。这很方便和自然，因为你不需要人为地去关心事务的回滚，只需要返回一个错误，其他的由Bolt去帮你完成。

只读事务在db.View函数之中：

err := db.View(func(tx *bolt.Tx) error {
    // ...
    return nil
})

在函数中你可以读取，但是不能做修改。
存储数据

Bolt是一个k/v的存储并提供了一个映射表，这意味着你可以通过name拿到值，就像Go原生的map，但是另外因为key是有序的，所以你可以通过key来遍历。

这里还有另外一层：k-v存储在bucket中，你可以将bucket当做一个key的集合或者是数据库中的表。（顺便提一句，buckets中可以包含其他的buckets，这将会相当有用）

你可以通过下面打方法update数据库;

db.Update(func(tx *bolt.Tx) error {
    b, err := tx.CreateBucketIfNotExists([]byte("posts"))
    if err != nil {
        return err
    }
    return b.Put([]byte("2015-01-01"), []byte("My New Year post"))
})

我们新建了一个名为“posts”的bucket，然后将key为“2014-01-01”的vaue置为“My New Year Post”。注意bucket的名字、key和value都是bytes的slices。
读取数据

在你存储了一些值到数据库之后，你可以这样读取他们：

db.View(func(tx *bolt.Tx) error {
    b := tx.Bucket([]byte("posts"))
    v := b.Get([]byte("2015-01-01"))
    fmt.Printf("%sn", v)
    return nil
})

你可以在Bolt的Readme里读到更多处理事务和遍历key的例子。
Go中的简单持久化

Bolt存储bytes，但是如果我们想存储一些结构体呢？这很容易通过Go的标准库实现，我们可以存储Json或是Gob编码后的结构化数据。或者，可以不限你自己使用标准库，你也可以使用Protocal Buffer或是其他的序列化方法。

例如，如果你想要存储一个博客的描述：

type Post struct {
    Created time.Time
    Title   string
    Content string
}

我们可以先编码，例如使用Json，然后将编码后的bytes存储到BoltDB中：

post := &Post{
   Created: time.Now(),
   Title:   "My first post",
   Content: "Hello, this is my first post.",
}

db.Update(func(tx *bolt.Tx) error {
    b, err := tx.CreateBucketIfNotExists([]byte("posts"))
    if err != nil {
        return err
    }
    encoded, err := json.Marshal(post)
    if err != nil {
        return err
    }
    return b.Put([]byte(post.Created.Format(time.RFC3339)), encoded)
})

当读取的时候，只需要将bytes unmarshal为结构体。
使用Bolt的命令行

BoltDB提供了一个名叫bolt的命令行工具，你可以列出buckets和keys、检索values、一致性检验。

用法可以使用--help查看。

例如，检查blog.db数据库的一致性，核对每个页面: bolt check blog.db
总结

Bolt真是一个简单易用的数据库，在Go的生态系统里将会有光明的未来。现在已经成熟并成功使用在一些项目之上，并且在大的读写中性能很好。比如现在这个不让我们省心的时间序列数据库Influxdb。
-->



















<!--

1. 交互
  监控页面提供分组机制，用户可以按照业务类型分组，也可以按照业务类型+DB类型分组。
  按照集群分组显示，不同的数据库类型会自动选择一个默认的主机图表显示。
  提供三个入口：A)数据库管理页面，选择相应集群或者机器的监控；B) 监控页面中的搜索功能；C) 监控大盘中的部分，也可以提供搜索。
  Reference 14款基于JQuery的实时搜索插件： http://www.iteye.com/news/25269   

2. API接口定义
  请求：
  {
       'host': '192.145.48.68',                                 # 主机IP
       'series': [
            {
               'name'       : 'cpu',                            # 对应InfluxDB的measurement
               'columns'    : ['usage_idle', 'usage_iowait'],   # 查询的fields，或者列(为了对显示友好可以通过js添加映射)
               'start_time' : '2017-01-04T07:30:23.283Z',       # 开始时间ISO(UTC)
               'end_time'   : '2017-01-04T07:30:23.283Z'        # 可选，如果为空则获取开始时间后的所有数据
            },
            {
               'name'       : 'memeory'
            },
            ... ...
       ]
   }
   响应(需要保证values递增，多个查询返回多个series)：
   {
        'results': [ {
            'series': [
                {
                    'name'   : 'cpu',
                    'columns': [
                         'time',
                    ],
                    'values': [
                        [
                             '2017-01-04T07:30:23.28Z',
                             80.899123,
                             8.834136
                        ],
                        [
                             '2017-01-04T07:30:24.28Z',
                             80.899123,
                             8.834136
                        ]
                    ]
                }
            ] }, {
           'series': [
               ... ...
           ] }
        ]
    }
   异常：
   {
        'error': 'error message'
   }
注意：查询数据的范围(start_time,end_time]为了防止不同指标的采集数据不同，从而将范围放到了measurements请求中。


Bugfix:
1. 修改时间采集间隔之后，会导致显示问题。
   原因：目前是获取一个点之后，直接shift移除一个，从而导致如果前后时间差不同，如 10s=>1s 会导致显示出现问题；建议按照时间区间处理。




influx_inspect 用于查看磁盘shards上的详细信息；将shard的信息转换为line protocol模式，然后重新写入到数据库中。

usage: influx_inspect [[command] [arguments]]

The commands are:
    dumptsm              dumps low-level details about tsm1 files.
    export               exports raw data from a shard to line protocol
    help                 display this help message
    report               displays a shard level report

默认查看当前目录下的 tsm 文件，例如 influx_inspect report -detailed 。

删除表: 可能会导致写入失败 https://blog.xiagaogao.com/20160117/influxdb%20%E4%B8%AD%E7%94%A8%E6%AD%A3%E7%A1%AE%E7%9A%84%E5%A7%BF%E5%8A%BF%E5%88%A0%E9%99%A4measurement.html
    DROP SERIES FROM 'measurement_name'
    DROP MEASUREMENT <measurement_name>
-->

## 参考

* 关于时序数据库的排名，可以参考 [db-engines.com](http://db-engines.com/en/ranking/time+series+dbms)。
* 官方网站 [www.influxdata.com](https://www.influxdata.com/)，包括了相关的文档。
* 源码可以直接从 Github 下载，[Github InfluxDB](https://github.com/influxdata/influxdb)、[Github Telegraf](https://github.com/influxdata/telegraf) 。

<!--
[Github Grafana](https://github.com/grafana/grafana)、[grafana.org](http://grafana.org/)

     http://blog.fatedier.com/2016/08/05/detailed-in-influxdb-tsm-storage-engine-one/
InfluxDB Docs:
     https://docs.influxdata.com/influxdb/v1.1/introduction/
InfluxDB(InfluxDB详解之TSM存储引擎解析):
     http://blog.fatedier.com/2016/08/05/detailed-in-influxdb-tsm-storage-engine-one/
Time Structured Merge Tree:
     https://www.influxdata.com/new-storage-engine-time-structured-merge-tree/
Gorilla: A Fast, Scalable, InMemory Time Series Database
     http://www.vldb.org/pvldb/vol8/p1816-teller.pdf
InfluxDB Storage Engine:
     https://docs.influxdata.com/influxdb/v1.1/concepts/storage_engine/
InfluxDB Glossary of Terms: 各种名词解释
     https://docs.influxdata.com/influxdb/v1.1/concepts/glossary/
MySQLMTOP(国内的MySQL监控):
     http://www.lepus.cc/page/opensource
该网站有很多不错的文章，可供参考
     http://www.opscoder.info/boltdb_intro.html

ClusterControl(Percona集群监控)
-->

{% highlight text %}
{% endhighlight %}
