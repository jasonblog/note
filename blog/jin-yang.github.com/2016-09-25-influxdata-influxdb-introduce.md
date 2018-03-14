---
title: InfluxDB
layout: post
comments: true
language: chinese
category: [linux,golang,database]
keywords: influxdb,telegraf,influxdata,時序數據庫
description: InfluxDB 是一個開源分佈式時序、事件和指標數據庫，使用 Go 語言編寫，無需外部依賴，其設計目標是實現分佈式和水平伸縮擴展。InfluxData 提供了 TICK 一套解決方案，不過使用比較多的是 InfluxDB，在此就介紹下 InfluxDB 以及 Telegraf 。
---

InfluxDB 是一個開源分佈式時序、事件和指標數據庫，使用 Go 語言編寫，無需外部依賴，其設計目標是實現分佈式和水平伸縮擴展。

InfluxData 提供了 TICK 一套解決方案，不過使用比較多的是 InfluxDB，在此就介紹下 InfluxDB 以及 Telegraf 。

<!-- more -->

![influxdb]({{ site.url }}/images/databases/influxdb/influxdb-golang.png "influxdb"){: .pull-center width="90%" }

## 簡介

在對時序數據進行存儲時，常見的有多種格式：A) 使用文件 (如: RRD, Whisper)；B) 使用 LSM 樹 (如: LevelDB, RocksDB, Cassandra)；C) 使用 B-Tree 排序和 k/v 存儲 (如: BoltDB, LMDB) 。

如下，就是 InfluxDATA 提供的一整套監控解決方案，包括了數據採集 (Telegraf)、數據存儲 (InfluxDB)、數據展示 (Chronograf) 以及報警 (Kapacitor) 。

![TICK]({{ site.url }}/images/databases/influxdb/tick-stack-grid.jpg "TICK"){: .pull-center width="60%" }

而實際上，使用比較多的是 InfluxDB ，這是一個時序數據庫，關於時序數據庫的排名，可以直接在 [db-engines.com](http://db-engines.com/en/ranking/time+series+dbms) 查看，InfluxDB 的活躍度基本排在第一。

除了上述 InfluxDATA 提供的解決方案之外，通常還可以使用如下的方案。

{% highlight text %}
採集數據 (collectd) ==>>==>> 存儲數據 (InfluxDB) ==>>==>> 顯示數據 (Grafana)
{% endhighlight %}

0.8.4 版本之前只能通過 [influxdb-collectd-proxy](https://github.com/hoonmin/influxdb-collectd-proxy) 採集 collectd 上報的數據，之後 InfluxDB 自動提供了接口；當然，默認是不開啟的，需要在配置文件中設置。

在此，還是重點介紹下 InfluxDB 。

#### 特性

簡單列舉下 InfluxDB 所支持的特性：

* 支持 Regular Timeseries 以及 Irregular Timeseries，前者是指時間間隔固定，後者指不固定，例如報警、入侵事件等；
* 可以將秒級監控在後臺轉換為分鐘級，減小存儲空間 (Continuous Queries)；
* 採用 Schemaless ，列存儲，壓縮率高，可以存儲任意數量的列；
* 提供 min, max, sum, count, mean 等聚合函數；
* 使用類 SQL 語句；
* Built-in Explorer 自帶管理工具，默認不打開，需要在配置文件中配置；
* Native HTTP API，採用內置 HTTP 服務 (Protobuf API 暫時不提供)。

<!--
Protocol Buffers 是一種輕便高效的結構化數據存儲格式，可以用於結構化數據串行化，很適合做數據存儲或RPC 數據交換格式。
influxdb/tsdb/internal/meta.proto
-->

### 常用概念

簡單列舉下常見的概念。

##### Database

數據庫，可以創建多個，不同數據庫中的數據文件存放在磁盤上的不同目錄。

##### Measurement

數據庫中的表，例如 memory 記錄了內存相關統計，其中包括了 used、cached、free 等。

##### Point

表裡的一行數據，由 A) 時間戳 (timestamp，每條記錄的時間，數據庫的主索引，會自動生成)；B) 數據 (field，記錄的 Key/Value 例如溫度、溼度)；C) 標籤 (tags，有索引如地區、海拔)組成。

##### Tag/tag key/tag value

有點不太好解釋，可以理解為標籤，或者二級索引，例如採集機器的 CPU 信息的時候，可以設置兩個 tag 分別是機器 IP 以及第幾個 CPU，在查詢的時候放在 where 條件中，從而不需要遍厲整個表，這是一個 ```map[stirng]string``` 結構。

##### Fields

也就是實際記錄的數據值，是 ```map[string]interface{}``` 類型，類似於 C 語言中的 void * ，這裡的 ```interface{}``` 可以是 int、int32、int64、float32、float64，也就是真正需要顯示或者計算的值，例如 CPU 的 sys, user, iowait 等。

##### Retention Policy

存儲策略，InfluxDB 會自動清理數據，可以設置數據保留的時間，默認會創建存儲策略 autogen (保留時間為永久)，之後用戶可以自己設置，例如保留最近 30 天的數據。

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

### 安裝

實際上，編譯好的二進製程序，可以直接從官網 [www.influxdata.com/downloads/](https://www.influxdata.com/downloads/) 上下載，例如，在 CentOS 中，可以通過如下方式安裝，安裝後數據默認保存在 /var/lib/influxdb 目錄下。

{% highlight text %}
----- 下載二進制安裝包
$ wget https://dl.influxdata.com/influxdb/releases/influxdb-1.1.1.x86_64.rpm
----- 查看二進制包的內容
$ rpm -qpl influxdb-1.1.1.x86_64.rpm
----- 解壓二進制包
$ rpm2cpio influxdb-1.1.1.x86_64.rpm | cpio -div
----- 安裝二進制包
# rpm -ivh influxdb-1.1.1.x86_64.rpm
----- 啟動服務，可以查看/usr/lib/systemd/system/influxdb.service文件
# systemctl start influxdb
{% endhighlight %}

安裝完之後，可以看到如下的可執行文件。

* influxd，服務器，可以直接通過該命令啟動；
* influx，InfluxDB 命令行客戶端，可鏈接服務器，執行一些常見的命令；
* influx_inspect，用於查看磁盤 shards 上的詳細信息；
* influx_stress，壓力測試工具；
* influx_tsm，數據庫轉換工具，可以將數據庫從 b1 或 bz1 格式轉換為 tsm1 格式。


### 源碼編譯

當然，主要介紹下 influxdb 和 telegraf 的源碼編譯。

#### build.py

在源碼中，有一個編譯腳本 build.py，可以通過該腳本進行編譯、打包等操作。

實際上，在 build.py 腳本中，會通過 git 檢查相應的版本 (詳見 get_current_XXX 類函數)，所以需要保證這是一個 git 版本庫，也就是說，如果通過該工具編譯，需要通過 git clone 下載源碼。

另外，代碼庫的依賴是通過 Go Dependancy Manager, GDM 管理的，當然如果沒有安裝，則會自動安裝；然後下載依賴的代碼庫；最後，會直接調用 go install 安裝。

> TIP: 如果無法直接連接到網絡，可以將 get_current_XXX()、go_get()、local_changes() 返回正常的值即可，例如 get_current_commit() 返回 "foobar" ，go_get() 返回 true 等等；當然，此時，依賴的代碼庫需要手動下載 (執行go install時會顯示依賴包)。

編譯後的程序，會最終安裝到 $GOPATH/bin/ 目錄下；也可以打包成 rpm 包，不過沒仔細研究，後面再看看。


#### influxdb

對於 InfluxDB 可以通過如下方式安裝。

{% highlight text %}
----- 1. 下載相關的分支
$ git clone -b v1.1.1 https://github.com/influxdata/influxdb.git .
----- 2. 直接進行編譯，如果並非通過git下載源碼，需要如上修改build.py文件
$ ./build.py
----- 3. 直接複製到$GOPATH/bin目錄下即可
$ cp build/* $GOPATH/bin
{% endhighlight %}

<!--
或者如下的方式，不過安裝的時候有問題，暫不確認
go get github.com/influxdata/influxdb
cd $GOPATH/src/github.com/influxdata/
go get ./...
go install ./...
-->


#### telegraf

對於 telegraf 可以直接使用 make 安裝，對於其中一些依賴庫，如 golang.org/x/crypto ，可能會導致無法下載，可以直接從 github 上下載，然後刪除 Godeps 相關依賴。

另外，通過上述方法下載時，需要使用 git clone 下載，否則會由於缺少 .git 目錄導致報錯；這也意味著，如果可以確保相關的依賴都已經下載之後，直接將 build.py 腳本中的 go_get() 註釋掉即可，這樣就不會通過 gdm 下載依賴包了。

{% highlight text %}
$ go get github.com/influxdata/telegraf
$ cd $GOPATH/src/github.com/influxdata/telegraf
$ make
{% endhighlight %}

如官方所述，該工具是插件式的，這也就導致其在編譯時，會依賴很多的插件，如果只需要部分插件，可以在 plugins 目錄下，修改相應分類的 all/all.go 文件，將不需要的註釋掉即可。

其實，主要修改 ```plubins/{inputs,outputs}/all/all.go``` 即可。

<!--
telegraf開發：
通過源碼進行編譯，執行 script/build.py，該腳本依賴git+gdm，如果非聯網，則需要手動下載安裝插件，實際其執行命令如下。
GOOS=linux GOARCH=amd64 go build -o /home/jinyang/workspace/src/github.com/influxdata/telegraf/build/telegraf -ldflags="-X main.version=foobar_version -X main.branch=foobar_branch -X main.commit=foobar_commit" ./cmd/telegraf

生成配置文件：
     telegraf -sample-config -input-filter cpu:mem -output-filter influxdb > telegraf.conf
-->

#### tips

需要注意的是，通過 gdm 安裝依賴時，可能會由於牆 (你懂得) 部分包無法下載，但是現象是 hang 住 :-( 所以，可以從 github 上手動下載，一般都會有鏡像的。


###  配置文件

默認使用 ```/etc/influxdb/influxdb.conf``` ，默認會使用如下的端口：

* 8083: Web-Admin管理服務的端口，如果是本地可以直接通過 [http://localhost:8083](http://localhost:8083) 打開，不過默認沒有開啟，需要在配置文件中打開；
* 8086: HTTP-API的端口，用於接收請求和發送數據；
* 8088: 集群端口，用於 InfluxDB 集群通訊使用，不過 0.11.1 版本之後就不再提供集群化的解決方案了，只有企業版才提供該功能；


<!--
http://www.xusheng.org/blog/2016/08/12/influxdb-relay-performance-bottle-neck-analysing/
-->

簡單列舉下常見的配置。

{% highlight text %}
# 管理面需要打開如下配置
[admin]
enabled = true
bind-address = ":8083"

# 如果不指定，則會通過os.hostname()通過系統獲取，可能會報錯
hostname = '192.168.1.23'
# 默認每隔24小時會向usage.influxdata.com發送一些統計數據，可以關閉掉
reporting-disabled = true
{% endhighlight %}


### Python訪問

通過 Python 訪問 InfluxDB 需要安裝 [InfluxDB Python](https://pypi.python.org/pypi/influxdb/)，然後通過如下方式訪問即可。

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

在 InfluxDB 中，寫入數據採用行格式，可以粗略的將要存入的一條數據看作一個虛擬的 key 和其對應的 value (field value)，格式如下：

{% highlight text %}
<measurement>[,<tag-key>=<tag-value>...] <field-key>=<field-value>[,<field2-key>=<field2-value>...] [timestamp]

cpu,host=serverA,region=west value=0.64   服務器默認時間
temperature,zipcode=384250,province=zhejiang value=75,humidity=20 1434067467000000000
{% endhighlight %}
<!--
payment,device=mobile,product=Notepad,method=credit billed=33,licenses=3i 1434067467100293230
stock,symbol=AAPL bid=127.46,ask=127.48
-->

常見操作列舉如下，可以通過 \-precision 參數指定時間格式以及精度，例如 rfc3339 。

{% highlight text %}
----- 寫入數據
$ curl -i -XPOST 'http://localhost:8086/write?db=testDB'
    --data-binary 'weather,altitude=1000,area=北 temperature=11,humidity=-4'
$ curl -i -XPOST 'http://localhost:8086/write?db=testDB&precision=s'
    --data-binary 'weather,altitude=1000,area=北 temperature=11,humidity=-4'
influx> INSERT weather,altitude=1000,area=北 temperature=11,humidity=-4;

----- 查詢
$ curl -G 'http://localhost:8086/query?pretty=true' --data-urlencode "db=mydb" -precision 'rfc3339'
    --data-urlencode "q=SELECT * FROM weather ORDER BY time DESC LIMIT 3"
$ curl -G 'http://localhost:8086/query?u=readonly&p=password&pretty=true' --data-urlencode "db=mydb"
    --data-urlencode "q=SELECT * FROM weather ORDER BY time DESC LIMIT 3"

----- 設置過期策略
$ curl -G http://localhost:8086/query --data-urlencode
    "q=CREATE RETENTION POLICY bar ON foo DURATION 300d REPLICATION 3 DEFAULT"
{% endhighlight %}

另外，支持 JSON 格式寫入、GZIP 壓縮數據插入及查詢，詳細可以參考源碼中的 tests 目錄。

## 運維操作

基本的數據保存目錄如下。

{% highlight text %}
.
├── data
│   ├── _internal
│   │   └── monitor
│   │       ├── 16   Shard-ID
│   │       │   └── 000000004-000000003.tsm
│   │       └── 3
│   └── telegraf
│       └── autogen  存儲策略
│           ├── 12
│           │   └── 000000132-000000002.tsm
│           └── 7
│               └── 000000003-000000002.tsm
├── meta
│   └── meta.db    元數據
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

### 系統監控

可以查看 [官方文檔](https://docs.influxdata.com/influxdb/v1.1/troubleshooting/statistics/)，關於 GoLang 的內部運行狀態，可以參考 [https://golang.org/pkg/runtime](https://golang.org/pkg/runtime/) 。

{% highlight text %}
----- InfluxDB本身的監控統計數據，保存在內存中，重啟後會丟失
SHOW STATS FOR <'module'>

----- 包括編譯信息、主機名、系統配置、啟動時間、內存使用率、GoLang運行環境信息
SHOW DIAGNOSTICS FOR <'module'>

----- 內部監控進程保存的數據，包含了各種監控的歷史數據
_internal
{% endhighlight %}

### 元數據查看

僅簡單列舉一下常見的命令，詳細可以查看 [官方文檔](https://docs.influxdata.com/influxdb/v1.1/query_language/schema_exploration/) 。

{% highlight text %}
SHOW SERIES;
SHOW SERIES ON telegraf FROM mysql;

SHOW FIELD KEYS;
SHOW FIELD KEYS ON telegraf FROM cpu;

SHOW MEASUREMENTS
{% endhighlight %}

### 用戶權限管理

InfluxDB 的權限設置比較簡單，只有讀、寫、ALL 三種，詳細參考 [官方文檔](https://docs.influxdata.com/influxdb/v1.1/query_language/authentication_and_authorization/) 。默認不開啟用戶認證，需要修改配置文件：

{% highlight text %}
[http]
auth-enabled = true
{% endhighlight %}

常見命令如下：

{% highlight text %}
----- 授權
GRANT [READ,WRITE,ALL] ON <database_name> TO <username>
GRANT ALL PRIVILEGES TO "username"

----- 撤銷權限
REVOKE [READ,WRITE,ALL] ON <database_name> FROM <username>
REVOKE ALL PRIVILEGES FROM "username"

----- 查看權限
SHOW GRANTS FOR <user_name>

----- 顯示用戶
SHOW USERS

----- 創建用戶
CREATE USER "readonly" WITH PASSWORD 'password'

----- 創建管理員權限的用戶
CREATE USER "readonly" WITH PASSWORD 'password' WITH ALL PRIVILEGES

----- 刪除用戶
DROP USER "readonly"

----- 修改密碼
SET PASSWORD FOR <username> = '<password>'
{% endhighlight %}

### 備份恢復

只支持全量備份，不支持增量，包括了元數據以及增量數據的備份，可以參考 [官方文檔](https://docs.influxdata.com/influxdb/v1.1/administration/backup_and_restore/) 。

{% highlight text %}
----- 元數據備份
$ influxd backup <path-to-backup>

----- 數據備份
$ influxd backup -database <mydatabase> <path-to-backup>
$ influxd backup -database telegraf -retention autogen -since 2016-02-01T00:00:00Z /tmp/backup
$ influxd backup -database mydatabase -host 10.0.0.1:8088 /tmp/remote-backup

----- 恢復
$ influxd restore -metadir /var/lib/influxdb/meta /tmp/backup
$ influxd restore -database telegraf -datadir /var/lib/influxdb/data /tmp/backup
{% endhighlight %}

### 數據保存策略

也就是 Retention Policies，可以設置保存的時間，例如保存 30 天。

{% highlight text %}
----- 查詢
SHOW RETENTION POLICIES ON "database_name";

----- 新建
CREATE RETENTION POLICY "rp_name" ON "db_name" DURATION 30d REPLICATION 1 DEFAULT;

----- 修改
ALTER RETENTION POLICY "rp_name" ON db_name DURATION 3w DEFAULT;

----- 刪除
DROP RETENTION POLICY "rp_name" ON "db_name";
{% endhighlight %}

### 連續查詢

也就是 Continuous Queries，當數據超過保存策略裡指定的時間之後，就會被刪除；可以通過連續查詢把原來的秒級數據，保存為分鐘級或者小時級的數據，從而減小數據的佔用空間。

{% highlight text %}
----- 查看
SHOW CONTINUOUS QUERIES;

----- 創建
CREATE CONTINUOUS QUERY cq-name ON db-name BEGIN
    SELECT mean(tbl-name) INTO newtbl-name FROM tbl-name GROUP BY time(30m) END;

----- 刪除
DROP CONTINUOUS QUERY <cq-name> ON <db-name>;
{% endhighlight %}

### 其它

1\. 獲取最近更新數據，並轉換為當前時間

{% highlight text %}
select threads_running from mysql order by time desc limit 1;
date -d @`echo 1483441750000000000 | awk '{print substr($0,1,10)}'` +"%Y-%m-%d %H:%M:%S"
{% endhighlight %}

2\. 檢查系統是否存活

{% highlight text %}
$ curl -sl -I localhost:8086/ping
{% endhighlight %}

3\. 常用操作

{% highlight text %}
----- 簡單查詢
SELECT * FROM weather ORDER BY time DESC LIMIT 3;

----- 指定時間範圍，時間格式也可以為'2017-01-03 00:00:00'
SELECT usage_idle FROM cpu WHERE time >= '2017-01-03T12:40:38.708Z' AND time <= '2017-01-03T12:40:50.708Z';

----- 最近40min內的數據
SELECT * FROM mysql WHERE time >= now() - 40m;

----- 最近5分鐘的秒級差值
SELECT derivative("queries", 1s) AS "queries" from "mysql" where time > now() - 5m;

----- 最近5min的秒級寫入
$ influx -database '_internal' -precision 'rfc3339'
      -execute 'select derivative(pointReq, 1s) from "write" where time > now() - 5m'

----- 也可以通過日誌查看
$ grep 'POST' /var/log/influxdb/influxd.log | awk '{ print $10 }' | sort | uniq -c
$ journalctl -u influxdb.service | awk '/POST/ { print $10 }' | sort | uniq -c
{% endhighlight %}

<!--
存儲引擎 (Storage Engine)： https://docs.influxdata.com/influxdb/v1.1/concepts/storage_engine/

進程運行時會啟動多個進程，可以通過 pstree -p PID 查看，其中包括了一個 HTTPD 服務進程，也就對應了 services/httpd 目錄下的代碼。

以查詢請求為例，實際的會路由到 handler.go 中指定的處理函數，也就是 serveQuery() 函數，而該函數中的處理邏輯基本相同：

A) 解析SQL；B) 執行；C) 返回結果。
-->

## 存儲引擎






Bolt 是一個 Go 語言編寫的嵌入式 KV 數據庫，提供了一個簡單可靠的方式做數據持久化，按照作者在 [Github BoltDB](https://github.com/boltdb/bolt) 中的介紹：

{% highlight text %}
Bolt is a pure Go key/value store inspired by Howard Chu's LMDB project. The goal
of the project is to provide a simple, fast, and reliable database for projects
that don't require a full database server such as Postgres or MySQL.
{% endhighlight %}

與 LevelDB 有所區別，BoltDB 支持完全可序列化的 ACID 事務；而且，提供穩定的 API 接口，而非類似 SQLite 的 SQL 接口；從而也就意味著它更加方便地整合到其它系統。


<!--
BoltDB將數據保存在一個單獨的內存映射的文件裡。它沒有wal、線程壓縮和垃圾回收；它僅僅安全地處理一個文件。
LevelDB和BoltDB的不同

LevelDB是Google開發的，也是一個k/v的存儲數據庫，和BoltDB比起起來有很大的不同。對於使用者而言，最大的不同就是LevelDB沒有事務。在其內部，也有很多的不同：LevelDB實現了一個日誌結構化的merge tree。它將有序的key/value存儲在不同文件的之中，並通過“層級”把它們分開，並且週期性地將小的文件merge為更大的文件。這讓其在隨機寫的時候會很快，但是讀的時候卻很慢。這也讓LevelDB的性能不可預知：但數據量很小的時候，它可能性能很好，但是當隨著數據量的增加，性能只會越來越糟糕。而且做merge的線程也會在服務器上出現問題。LevelDB是C++寫的，但是也有些Go的實現方式，如syndtr/goleveldb、leveldb-go。

BoltDB使用一個單獨的內存映射的文件，實現一個寫入時拷貝的B+樹，這能讓讀取更快。而且，BoltDB的載入時間很快，特別是在從crash恢復的時候，因為它不需要去通過讀log（其實它壓根也沒有）去找到上次成功的事務，它僅僅從兩個B+樹的根節點讀取ID。
-->

### 安裝、使用

在設置好 golang 的環境變量之後，可以很簡單的通過 go get 獲取源碼並安裝到 $GOPATH/bin 目錄下，其命令如下：

{% highlight text %}
$ go get github.com/boltdb/bolt/...
{% endhighlight %}

<!--

使用Bolt
打開數據庫並初始化事務

Bolt將所有數據都存儲在一個文件中，這讓它很容易使用和部署，用戶肯定很高興地發現他們根本不需要去配置數據庫或是要DBA去維護它，我們需要對這文件所做的就是打開他們 ，如果你想要打開的文件不存在就會新建。

在這個例子中我們blog.db這個數據庫在當前文件夾：

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

在你打開之後，你有兩種處理它的方式：讀-寫和只讀操作，讀-寫方式開始於db.Update方法：

err := db.Update(func(tx *bolt.Tx) error {
    // ...read or write...
    return nil
})

可以看到，你傳入了db.update函數一個參數，在函數內部你可以get/set數據和處理error。如果返回為nil，事務就會從數據庫得到一個commit，但是如果你返回一個實際的錯誤，則會做回滾，你在函數中做的任何事情都不會commit到磁盤上。這很方便和自然，因為你不需要人為地去關心事務的回滾，只需要返回一個錯誤，其他的由Bolt去幫你完成。

只讀事務在db.View函數之中：

err := db.View(func(tx *bolt.Tx) error {
    // ...
    return nil
})

在函數中你可以讀取，但是不能做修改。
存儲數據

Bolt是一個k/v的存儲並提供了一個映射表，這意味著你可以通過name拿到值，就像Go原生的map，但是另外因為key是有序的，所以你可以通過key來遍歷。

這裡還有另外一層：k-v存儲在bucket中，你可以將bucket當做一個key的集合或者是數據庫中的表。（順便提一句，buckets中可以包含其他的buckets，這將會相當有用）

你可以通過下面打方法update數據庫;

db.Update(func(tx *bolt.Tx) error {
    b, err := tx.CreateBucketIfNotExists([]byte("posts"))
    if err != nil {
        return err
    }
    return b.Put([]byte("2015-01-01"), []byte("My New Year post"))
})

我們新建了一個名為“posts”的bucket，然後將key為“2014-01-01”的vaue置為“My New Year Post”。注意bucket的名字、key和value都是bytes的slices。
讀取數據

在你存儲了一些值到數據庫之後，你可以這樣讀取他們：

db.View(func(tx *bolt.Tx) error {
    b := tx.Bucket([]byte("posts"))
    v := b.Get([]byte("2015-01-01"))
    fmt.Printf("%sn", v)
    return nil
})

你可以在Bolt的Readme裡讀到更多處理事務和遍歷key的例子。
Go中的簡單持久化

Bolt存儲bytes，但是如果我們想存儲一些結構體呢？這很容易通過Go的標準庫實現，我們可以存儲Json或是Gob編碼後的結構化數據。或者，可以不限你自己使用標準庫，你也可以使用Protocal Buffer或是其他的序列化方法。

例如，如果你想要存儲一個博客的描述：

type Post struct {
    Created time.Time
    Title   string
    Content string
}

我們可以先編碼，例如使用Json，然後將編碼後的bytes存儲到BoltDB中：

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

當讀取的時候，只需要將bytes unmarshal為結構體。
使用Bolt的命令行

BoltDB提供了一個名叫bolt的命令行工具，你可以列出buckets和keys、檢索values、一致性檢驗。

用法可以使用--help查看。

例如，檢查blog.db數據庫的一致性，核對每個頁面: bolt check blog.db
總結

Bolt真是一個簡單易用的數據庫，在Go的生態系統裡將會有光明的未來。現在已經成熟併成功使用在一些項目之上，並且在大的讀寫中性能很好。比如現在這個不讓我們省心的時間序列數據庫Influxdb。
-->



















<!--

1. 交互
  監控頁面提供分組機制，用戶可以按照業務類型分組，也可以按照業務類型+DB類型分組。
  按照集群分組顯示，不同的數據庫類型會自動選擇一個默認的主機圖表顯示。
  提供三個入口：A)數據庫管理頁面，選擇相應集群或者機器的監控；B) 監控頁面中的搜索功能；C) 監控大盤中的部分，也可以提供搜索。
  Reference 14款基於JQuery的實時搜索插件： http://www.iteye.com/news/25269   

2. API接口定義
  請求：
  {
       'host': '192.145.48.68',                                 # 主機IP
       'series': [
            {
               'name'       : 'cpu',                            # 對應InfluxDB的measurement
               'columns'    : ['usage_idle', 'usage_iowait'],   # 查詢的fields，或者列(為了對顯示友好可以通過js添加映射)
               'start_time' : '2017-01-04T07:30:23.283Z',       # 開始時間ISO(UTC)
               'end_time'   : '2017-01-04T07:30:23.283Z'        # 可選，如果為空則獲取開始時間後的所有數據
            },
            {
               'name'       : 'memeory'
            },
            ... ...
       ]
   }
   響應(需要保證values遞增，多個查詢返回多個series)：
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
   異常：
   {
        'error': 'error message'
   }
注意：查詢數據的範圍(start_time,end_time]為了防止不同指標的採集數據不同，從而將範圍放到了measurements請求中。


Bugfix:
1. 修改時間採集間隔之後，會導致顯示問題。
   原因：目前是獲取一個點之後，直接shift移除一個，從而導致如果前後時間差不同，如 10s=>1s 會導致顯示出現問題；建議按照時間區間處理。




influx_inspect 用於查看磁盤shards上的詳細信息；將shard的信息轉換為line protocol模式，然後重新寫入到數據庫中。

usage: influx_inspect [[command] [arguments]]

The commands are:
    dumptsm              dumps low-level details about tsm1 files.
    export               exports raw data from a shard to line protocol
    help                 display this help message
    report               displays a shard level report

默認查看當前目錄下的 tsm 文件，例如 influx_inspect report -detailed 。

刪除表: 可能會導致寫入失敗 https://blog.xiagaogao.com/20160117/influxdb%20%E4%B8%AD%E7%94%A8%E6%AD%A3%E7%A1%AE%E7%9A%84%E5%A7%BF%E5%8A%BF%E5%88%A0%E9%99%A4measurement.html
    DROP SERIES FROM 'measurement_name'
    DROP MEASUREMENT <measurement_name>
-->

## 參考

* 關於時序數據庫的排名，可以參考 [db-engines.com](http://db-engines.com/en/ranking/time+series+dbms)。
* 官方網站 [www.influxdata.com](https://www.influxdata.com/)，包括了相關的文檔。
* 源碼可以直接從 Github 下載，[Github InfluxDB](https://github.com/influxdata/influxdb)、[Github Telegraf](https://github.com/influxdata/telegraf) 。

<!--
[Github Grafana](https://github.com/grafana/grafana)、[grafana.org](http://grafana.org/)

     http://blog.fatedier.com/2016/08/05/detailed-in-influxdb-tsm-storage-engine-one/
InfluxDB Docs:
     https://docs.influxdata.com/influxdb/v1.1/introduction/
InfluxDB(InfluxDB詳解之TSM存儲引擎解析):
     http://blog.fatedier.com/2016/08/05/detailed-in-influxdb-tsm-storage-engine-one/
Time Structured Merge Tree:
     https://www.influxdata.com/new-storage-engine-time-structured-merge-tree/
Gorilla: A Fast, Scalable, InMemory Time Series Database
     http://www.vldb.org/pvldb/vol8/p1816-teller.pdf
InfluxDB Storage Engine:
     https://docs.influxdata.com/influxdb/v1.1/concepts/storage_engine/
InfluxDB Glossary of Terms: 各種名詞解釋
     https://docs.influxdata.com/influxdb/v1.1/concepts/glossary/
MySQLMTOP(國內的MySQL監控):
     http://www.lepus.cc/page/opensource
該網站有很多不錯的文章，可供參考
     http://www.opscoder.info/boltdb_intro.html

ClusterControl(Percona集群監控)
-->

{% highlight text %}
{% endhighlight %}
