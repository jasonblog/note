---
title: Collectd 簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: collectd,monitor,linux
description: collectd 是一個後臺監控程序，用來採集其所運行系統上的系統信息，並提供各種存儲方式來存儲為不同值的格式，例如 RRD 文件形式、文本格式、MongoDB 等等。在此，簡單介紹下 collectd 。
---

collectd 是一個後臺監控程序，用來採集其所運行系統上的系統信息，並提供各種存儲方式來存儲為不同值的格式，例如 RRD 文件形式、文本格式、MongoDB 等等。

在此，簡單介紹下 collectd 。

<!-- more -->

![collectd logo]({{ site.url }}/images/linux/collectd_logo.png "collectd logo"){: .pull-center width="30%" }

## 簡介

Collectd 完全由 C 語言編寫，故性能很高，可移植性好，它允許運行在系統沒有腳本語言支持或者 cron daemon 的系統上，比如嵌入式系統；同時，它包含優化以及處理成百上千種數據集的新特性，目前包含了幾十種插件。

其中數據採集和採集數據的寫入都可以通過插件進行定義，目前支持的數據採集插件包括了幾十種，寫入包括了 Graphite、RRDtool、Riemann、MongoDB、HTTP 等。

![collectd architecture]({{ site.url }}/images/linux/collectd_architecture.png "collectd architecture"){: .pull-center width="70%" }

簡單來說，其特點如下：

1. C語言、插件、多線程(讀寫線程池)，支持 Python、Shell、Perl 等腳本語言；
2. 數據採集插件包括了 OS(CPU、Memory、Network、Disk等)、通用組件(Nginx、MySQL、PostgreSQL、Redis、HAProxy等)；
3. 寫入插件支持 RRDTool、Kafka、MongoDB、Redis、Riemann、Sensu、TSDB等；
4. 支持級聯方式，數據發送採用 UDP 報文；

如下是將採集的數據保存為 rrd 格式文件，並通過 rrdtool 工具繪製出來。

![collectd cpu rrd]({{ site.url }}/images/linux/collectd_cpu_example.png "collectd cpu rrd"){: .pull-center }

### 安裝

對於 CentOS 可以直接安裝 collectd 對應的 RPM 包，不同的插件可以安裝不同的包。

{% highlight text %}
----- 查看當前版本，以及所支持的插件
$ yum --enablerepo=epel list all | grep collectd
----- 安裝
# yum --enablerepo=epel install collectd
{% endhighlight %}

### 源碼編譯

編譯選項可以直接通過 ```./configure --help``` 命令查看，例如可以通過如下配置開啟調試選項，也就是 COLLECT_DEBUG 宏；對於插件會自動檢查是否存在頭文件，例如 rrdtool 需要 rrd.h 文件，也就是需要安裝 rrdtool-devel 包。

{% highlight text %}
$ ./configure --enable-debug --enable-all-plugins
{% endhighlight %}

注意，在配置完之後會打印配置項信息，包括了編譯、鏈接參數，以及支持哪些插件等，也可以查看 config.log 文件。

編譯完成之後，可以直接通過 ```make check``` 進行檢查；對於 RPM 包生成，在 contrib 中有相應的 spec 文件；對於幫助文檔可以查看 ```man -l src/collectd.1```；關於支持哪些組件，可以查看源碼中的 README 文件，例如測試時，可以直接開啟 write_log 寫入插件。

另外，編譯後的動態庫保存在 src/.libs 目錄下，如果不想安裝，可以直接調整配置文件即可。


### 常見操作

{% highlight text %}
----- 啟動服務
$ collectd -C collectd.conf
----- 檢查配置文件
$ collectd -C collectd.conf -t
----- 測試插件，會調用一次插件
$ collectd -C collectd.conf -T
----- 刷新，會生成單獨線程執行do_flush()操作
$ kill -SIGUSR1 `pidof collectd`
----- 關閉服務，也可以使用SIGTERM信號
$ kill -SIGINT `pidof collectd`
----- 採用多線程，可以通過該命令查看當前的線程數
$ ps -Lp `pidof collectd`
{% endhighlight %}

#### 狀態查看

Collectd 可以通過 unixsock 插件進行通訊、查看狀態等，也可以使用 collectdctl 命令；通過如下配置打開 unixsock 。

{% highlight text %}
LoadPlugin unixsock
<Plugin unixsock>
  SocketFile "/tmp/collectd.sock"
  SocketGroup "collectd"
  SocketPerms "0660"
  DeleteSocket true                  # 啟動時如果存在sock，是否嘗試刪除
</Plugin>
{% endhighlight %}

在開啟了 UnixSock 之後，可以通過如下命令與 Collectd 進行交互；對於 collectdctl 命令，可直接通過 ```man -l src/collectdctl.1``` 查看幫助。

{% highlight text %}
$ collectdctl -s /tmp/collectd.sock listval
$ echo "PUTNOTIF severity=okay time=$(date +%s) message=hello" | \
    socat - UNIX-CLIENT:/path/to/socket
{% endhighlight %}


### 日誌配置

collectd 支持多種日誌格式，包括了 syslog、logstash、本地日誌文件等；如果調試，也可以通過 write_log 插件將採集數據寫入到日誌文件中。

<!--
另外，可以使用 logrotate 管理日誌數據，配置如下。TODO:如何確認logrotate配置有效%%%%%無法自動重新打開,logrotate失效!!!

cat <<- EOF > /etc/logrotate.d/collectd.conf
var/log/nginx/*.log /var/log/tomcat/*log {   # 可以指定多個路徑
    daily                      # 每天切換一次日誌
    rotate 15                  # 默認保存15天數據，超過的則刪除
    compress                   # 切割後壓縮
    delaycompress              # 切割時對上次的日誌文件進行壓縮
    dateext                    # 日誌文件切割時添加日期後綴
    missingok                  # 如果沒有日誌文件也不報錯
    notifempty                 # 日誌為空時不進行切換，默認為ifempty
    create 644 monitor monitor # 使用該模式創建日誌文件
    postrotate # TODO
        if [ -f /var/run/nginx.pid ]; then
            kill -USR1 `cat /var/run/nginx.pid`
        fi
    endscript
}

FIXME: 如果啟用logfile，則嘗試打開默認文件，沒有權限則報錯。用於打印 [2017-04-27 13:57:32] plugin_load: plugin "logfile" successfully loaded. 信息。
-->

## 推薦配置

源碼中有一個 src/collectd.conf 參考配置文件，如下是一些常見的配置項。

{% highlight text %}
# 指定上報的主機名，可以為IP或者按照規則定義主機名稱(service_name.componet_name)
Hostname    "foobar"
# 是否允許主機名查找，如果DNS配置可能有誤，建議不要開啟
FQDNLookup   false

# 各種文件、目錄的設置
BaseDir     "/var/lib/collectd"
PIDFile     "/var/run/collectd.pid"
PluginDir   "/home/andy/Workspace/packages/collectd/collectd-5.7.1/src/.libs"
TypesDB     "/home/andy/Workspace/packages/collectd/collectd-5.7.1/src/types.db"

# 設置為true時，對於<Plugin XXX>可以自動加載插件，無需指定LoadPlugin，詳見dispatch_block_plugin()
AutoLoadPlugin false

# 是否同時上報collectd自身的狀態，會通過plugin_register_read()註冊讀取函數，也即會通過
# 執行plugin_update_internal_statistics()函數上報當前collectd的狀態
CollectInternalStats false

# 設置全局的數據採集時間間隔，單位是秒，可以在插件中進行覆蓋；關於最大采集時間間隔，詳見後面的介紹
Interval                 1
MaxReadInterval        180

# 注意這裡單位不是秒，實際超時時間是timeout*interval
Timeout                  2

# 用於配置讀寫線程數
WriteThreads             2
ReadThreads              5

# 配置緩存的上下限
WriteQueueLimitLow    8000
WriteQueueLimitHigh  12000

# 創建一個Unix Socket用於一些命令發送，狀態查看等
LoadPlugin unixsock
<Plugin unixsock>
  SocketFile "/tmp/collectd.sock"
  SocketGroup "collectd"
  SocketPerms "0660"
  DeleteSocket true                  # 啟動時如果存在sock，是否嘗試刪除
</Plugin>

# 設置日誌文件，保存到本地文件中，可以通過logrotate管理
LoadPlugin logfile
<Plugin logfile>
    LogLevel info
    File "/var/log/collectd/collectd.log"  # 也可以配置為STDOUT，作為標準輸出
    Timestamp true
    PrintSeverity true
</Plugin>

# 通過write_log插件將採集數據保存到日誌中，默認採用的是Graphite格式
LoadPlugin write_log
<Plugin write_log>
  Format JSON
</Plugin>
{% endhighlight %}

如下簡單列舉一些常用的配置。

1\. 數據緩存上下限設置。

可以通過 ```WriteQueueLimitHigh```、```WriteQueueLimitLow``` 參數可以配置緩存隊列的大小，在源碼中對應了 ```write_limit_high```、```write_limit_low``` 變量。

當小於 ```WriteQueueLimitLow``` 時不會丟棄所採集的數據，當大於上述值而小於 ```WriteQueueLimitHigh``` 時會概率性丟棄，而大於 High 則直接丟棄。

2\. 插件採集時間間隔。

如果調用插件讀取數據時失敗，則會將下次採集時間 double，但是最大值為配置文件中的設置值；當一次讀取成功時，則會直接恢復到正常的採集時間間隔。

3\. 關於Timeout參數

主線程會檢查是否有數據採集超時，注意這裡真正的超時時間是 timeout*interval ，如果超時則會調用註冊的 missing 回調函數，而且會在 global cache 中刪除這個對象。

4\. types.db 的使用

type.db 每行由兩個字段組成，由空格或者 tab 分隔，分別表示：A) 數據集名稱；B) 數據源說明的列表，對列表單元都以逗號分隔。

每個數據源通過冒號分為 4 部分：數據源名、類型、最小值和最大值 ```ds-name:ds-type:min:max```；其中 ds-type 包含 4 種類型 (```ABSOULUTE```, ```COUNTER```, ```DERIVE```, ```GAUSE```)，其中，mix 和 max 定義了固定值範圍，U 表示範圍未知或者不限定。

每行數據對應了一個 ```value_list_t```，可以通過 ```format_values()``` 函數進行格式化。

5\. 如何查看 collectd 的狀態？

自身狀態查看，可以在配置文件中添加 ```CollectInternalStats true``` 選項，或者開啟 unixsocket，然後通過 collectdctl 命令進行查看。

顯然，一個是可以遠程採集數據的，一個是隻能本地訪問的。

## Filter-Chain

從 4.6 版本開始支持 filter-chain 功能，可用於將某個值發送給特定的輸出，其工作模式類似於 ip_tables，包括了 matches 和 targets，前者用於匹配一個規則，後者用於執行某個操作。

如下是處理流程圖。

{% highlight text %}
 +---------+
 ! Chain   !
 +---------+
      !
      V
 +---------+  +---------+  +---------+  +---------+
 ! Rule    !->! Match   !->! Match   !->! Target  !
 +---------+  +---------+  +---------+  +---------+
      !
      V
 +---------+  +---------+  +---------+
 ! Rule    !->! Target  !->! Target  !
 +---------+  +---------+  +---------+
      !
      V
      :
      :
      !
      V
 +---------+  +---------+  +---------+
 ! Rule    !->! Match   !->! Target  !
 +---------+  +---------+  +---------+
      !
      V
 +---------+
 ! Default !
 ! Target  !
 +---------+
{% endhighlight %}

簡單介紹下如上的常見概念：

* Match，用於匹配規則，例如採集指標名稱以及當前採集的值，通常規則使用 match_ 插件，不過在使用前需要提前加載。
* Target，就是將要執行的一些操作，插件以 target_ 開頭。
* Rule，零到多個 match 規則以及至少一個 target 組成 rule ，如果沒有規則對所有的值匹配。
* Chain，包括了一系列的 rule 以及可能的默認目標，會按照規則進行匹配，如果滿足則指定 target 執行。

Chain 規則，目前只支持 Pre-Chain 和 Post-Chain 兩種，在配置文件中可通過 PreCacheChain、PostCacheChain 用於配置 filter-chain 功能。


<!--
There are four ways to control which way a value takes through the filter mechanism:

jump
    The built-in jump target can be used to ``call'' another chain, i. e. process the value with another chain. When the called chain finishes, usually the next target or rule after the jump is executed.
stop
    The stop condition, signaled for example by the built-in target stop, causes all processing of the value to be stopped immediately.
return
    Causes processing in the current chain to be aborted, but processing of the value generally will continue. This means that if the chain was called via Jump, the next target or rule after the jump will be executed. If the chain was not called by another chain, control will be returned to the daemon and it may pass the value to another chain.
continue
    Most targets will signal the continue condition, meaning that processing should continue normally. There is no special built-in target for this condition.
-->

### 示例

一個簡單的示例如下。

{% highlight text %}
PostCacheChain "PostCache"
<Chain "PostCache">
  <Rule "ignore_mysql_show">
    <Match "regex">
      Plugin "^mysql$"
      Type "^mysql_command$"
      TypeInstance "^show_"
    </Match>
    <Target "stop">
    </Target>
  </Rule>
  <Target "write">
    Plugin "rrdtool"
  </Target>
</Chain>
{% endhighlight %}

上述的配置中，會忽略 ```plugin=mysql + type=mysql_command + type_instance=show_``` 採集的值，不過需要注意的是，這個操作是在 ```PostCache``` 鏈表中，所以仍然可以通過 unixsock 插件查看採集值。




<!--
## 優化建議




通過cdtime()函數獲取當前時間，實際上調用clock_gettime()系統函數，統計從1970.1.1開始時間；然後通過nanosleep()休眠。

struct timespec {
    time_t tv_sec; /* seconds */
    long tv_nsec;  /* nanoseconds */
};
typedef uint64_t cdtime_t;
#define NS_TO_CDTIME_T(ns)                                                     \
  (cdtime_t) {                                                                 \
    ((((cdtime_t)(ns)) / 1000000000) << 30) |                                  \
        ((((((cdtime_t)(ns)) % 1000000000) << 30) + 500000000) / 1000000000)   \
  }
#define TIMESPEC_TO_CDTIME_T(ts)                                               \
  NS_TO_CDTIME_T(1000000000ULL * (ts)->tv_sec + (ts)->tv_nsec)

在內部維護了list以及heap兩個結構，分別用於保存當前的插件，以及下個週期需要調用那個插件。


LoadPlugin rrdtool
對於rrdtool插件，會在DataDir目錄下創建 {hostname}/{measurement} 目錄保存採集數據。
-->

## 通訊協議

Collectd 提供了兩種協議 [Plain text protocol](https://collectd.org/wiki/index.php/Plain_text_protocol) 以及 [Binary protocol](https://collectd.org/wiki/index.php/Binary_protocol) 。

### Plain text protocol

這一協議目前可以在使用 Exec 執行腳本時使用，或者通過 UnixSock 查看當前 Collectd 服務的狀態時使用。注意：一行的請求需要小於 1024 字節，否則報 "Parsing options failed" 錯誤。

#### LISTVAL

語法是 ```LISTVAL``` ，用於查看當前監控項，會打印出上次採集的時間點。

{% highlight text %}
$ collectdctl -s /tmp/collectd.sock listval
$ echo "LISTVAL" | nc -U /tmp/collectd.sock
$ echo "LISTVAL" | socat - UNIX-CLIENT:/tmp/collectd.sock
{% endhighlight %}

#### GETVAL

語法是 ```GETVAL Identifier``` ，獲取某個指標的採集值，指標通過上述的標示符 (Identifier) 定義。

{% highlight text %}
$ echo 'GETVAL "localhost/memory/memory-buffered"' | nc -U /tmp/collectd.sock
{% endhighlight %}

#### PUTVAL

語法是 ```PUTVAL Identifier [OptionList] Valuelist``` ，用於將某個監控項提交給 Collectd 服務，會自動轉給寫插件。

* OptionList: 選項列表，每項通過 KV 表示，目前只支持 interval=seconds 這一模式。
* Valuelist: 通過冒號分割的時間戳以及值，支持的數據類型有 uint(COUNTER+ABSOLUTE), int(DERIVE), double(GAUGE)；對於 GAUGE 可以使用 "U" 標示未定義值，但是不能對 COUNTER 使用 "U"；時間戳採用 UNIX epoch，也可以使用 "N" 表示使用當前時間。

{% highlight text %}
$ echo 'PUTVAL "localhost/load/load" interval=10 N:13:456:5000' | nc -U /tmp/collectd.sock
{% endhighlight %}

<!--
##### PUTNOTIF

echo "PUTNOTIF severity=okay time=$(date +%s) message=hello" | \
  socat - UNIX-CLIENT:/path/to/socket
-->

## Thresholds

4.3.0 版本之後，開始支持監控，實際上也就是說不僅支持數據的採集上報，而且會對採集數據進行判斷，如果定義的事件發生則會上報。

這一操作主要是通過 threshold 插件完成，然後發送 notification 消息；而其它的插件則可以註冊接收該消息並作進一步的處理。

注意，這裡沒有對採集的數據進行處理，也就是沒有做類似移動平均線 (Moving Average) 之類的處理，也就意味著比較敏感，可能會有誤報。

如下是一個簡單的配置文件。

{% highlight text %}
LoadPlugin "threshold"
<Plugin "threshold">
  <Type "foo">
    WarningMin    0.00
    WarningMax 1000.00
    FailureMin    0.00
    FailureMax 1200.00
    Invert false
    Instance "bar"
  </Type>

  <Plugin "interface">
    Instance "eth0"
    <Type "if_octets">
      FailureMax 10000000
      DataSource "rx"
    </Type>
  </Plugin>

  <Host "hostname">
    <Type "cpu">
      Instance "idle"
      FailureMin 10
    </Type>

    <Plugin "memory">
      <Type "memory">
        Instance "cached"
        WarningMin 100000000
      </Type>
    </Plugin>

    <Type "load">
       DataSource "midterm"
       FailureMax 4
       Hits 3
       Hysteresis 3
    </Type>
  </Host>
</Plugin>
{% endhighlight %}

在插件中可以通過 Host Plugin Type 配置塊決定需要監控的指標 (注意需要按順序嵌套)，而且 Plugin Type 還可以通過 Instance 決定哪個示例。

<!--
(WarningMax, FailureMax] + [FailureMin, WarningMin) Warning信息
(FailureMax, ) + ( , FailureMin) Failure信息

DataSource DSName
  某些監控項包括了多個數據源，例如 if_octets 包括了 rx 和 tx、load 包括了 shortterm、midterm、longterm 等等；默認會檢查所有的採集項，或者通過該參數指定需要檢查的採集項。
Invert true|false(default)
  監控時間策略默認如上所示，當然可以通過該參數取反，也就是在 FailureMin~FailureMax 範圍內是異常的。
Persist true|false(default)
  設置Notify通知的頻率，true 每個採集點都會發送一個通知；false 越過邊界時發送通知，也就是上次正常，本次異常。
PersistOK true|false

    Sets how OKAY notifications act. If set to true one notification will be generated for each value that is in the acceptable range. If set to false (the default) then a notification is only generated if a value is in range but the previous value was not.
Percentage true|false

    If set to true, the minimum and maximum values given are interpreted as percentage value, relative to the other data sources. This is helpful for example for the "df" type, where you may want to issue a warning when less than 5 % of the total space is available. Defaults to false.
Hits Value

    Sets the number of occurrences which the threshold must be raised before to dispatch any notification or, in other words, the number of Intervals that the threshold must be match before dispatch any notification.
Hysteresis Value

    Sets the hysteresis value for threshold. The hysteresis is a method to prevent flapping between states, until a new received value for a previously matched threshold down below the threshold condition (WarningMax, FailureMin or everything else) minus the hysteresis value, the failure (respectively warning) state will be keep.
Interesting true|false

    If set to true (the default), a notification with severity FAILURE will be created when a matching value list is no longer updated and purged from the internal cache. When this happens depends on the interval of the value list and the global Timeout setting. See the Interval and Timeout settings in collectd.conf(5) for details. If set to false, this event will be ignored.

https://collectd.org/wiki/index.php/Notifications_and_thresholds
-->



## RRDTool

Round Robin Database, RRD 適用於存儲和時間序列相關的數據，是一種循環使用存儲空間的數據庫，也就是創建之後其大小已經固定，不再需要維護。

可以通過 rrdtool 保存數據，此時磁盤 IO 可能會成為瓶頸，為此可以使用 RRDCacheD 後臺進程。


### 簡介

首先介紹下常見概念。

Primary Data Point, PDP：正常情況下每個週期都會收到一個採集值，然後根據不同的類型計算出一個值，這個計算出的值就是 PDP ；注意，除非是 GAUGE，否則改值不是收到的值。

Consolidation Data Point, CPD：通過定義的 CF 函數，使用多個 PDP 計算出一個 CDP 值並保存在 RRA 中，繪圖時就使用這些數據。

### 創建文件

其中 filename 、DS 部分和 RRA 部分是必須的，其它兩個參數可選。

{% highlight text %}
rrdtool create filename [--start|-b start time] [--step|-s step]
    [DS:variable_name:DST:heartbeat:min:max]
    [RRA:CF:xff:step:rows]

參數解析：
  --step NUM
    預計多久收到一個值，默認是5分鐘，與MRTG中的interval相同；
  --start TIMESTAMP
    給出第一個記錄的起始時間，小於等於該時間的數據不保存，例如指定為一天前--start $(date -d '1 days ago' +%s)；
  DS:variable_name:DST:heartbeat:min:max
    用於定義 Data Soure Name，常見有eth0_in, eth0_out，一般為1-19個字符，必須是0-9,a-z,A-Z；
    以及數據類型(Data Source Type, DST)，包括了COUNTER、GUAGE、DERIVE、ABSOLUTE、COMPUTE 5 種：
      COUNTER
        必須是遞增的，除非是計數器溢出 (overflows) 此時會自動修改收到的值；這也是最常見的類型，例如網絡接口流量。
      DERIVE
        和 COUNTER 類似，但可以是遞增，也可以遞減，例如數據庫的鏈接數。
      ABSOLUTE
        會計算斜率，每次都會假設其前一個值是 0，類似於每次讀取監控數據後統計值自動清零。
      GAUGE
        沒有上述的平均值概念，收到後原樣保存。
      COMPUTE
        這一類型比較特殊，它並不接受輸入，而是定義一個表達式，通過引用其它 DS 並自動計算出值。
  RRA:CF:xff:step:rows
    RRA定義了數據如何存放，可以把一個 RRA 看成一個表，各保存不同 interval 的統計結果。
    Consolidation Function, CF 也就是指定的合併功能函數，有 AVERAGE、MAX、MIN、LAST 四種，分別表
    示對多個PDP取平均、最大值、最小值、當前值四種類型。
    xfile factor 取值範圍是 0~1 ，表示一個範圍內採樣值為 UNKNOWN 值比率超過多少就會將聚合值設置為UNKNOWN；
    例如 RRA:AVERAGE:0.5:24:600 ，當 PDP 中有超過 12 值為 UNKNOWN 時該聚合的值就是 UNKNOWN 。

----- 創建文件
$ rrdtool create eth0.rrd \
    --start $(date –d '1 days ago' +%s) \    # 保存一天的數據
    --step 300 \                             # 採集時間間隔為5min
    DS:eth0_in:COUNTER:600:0:12500000 \      # 600 是 heartbeat；0 是最小值；12500000 表示最大值；
    DS:eth0_out:COUNER:600:0:12500000 \      # 如果沒有最小值/最大值，可以用 U 代替，例如 U:U
    RRA:AVERAGE:0.5:1:600 \                  # 1表示對1個PDP取平均，實際上就等於PDP的值
    RRA:AVERAGE:0.5:4:600 \                  # 4表示每4個PDP合成為一個CDP，也就是20分鐘，方法是對4個PDP取平均
    RRA:AVERAGE:0.5:24:600 \                 # 同上，也就是24*5=120分鐘=2小時
    RRA:AVERAGE:0.5:288:730                  # 同上，也就是 288*5=1440分鐘=1天

----- 查看文件信息，主要包括了創建時的信息，例如DS、RRA等
$ rrdtool info eth0.rrd

----- 查看最近一次，第一次更新時間
$ rrdtool last eth0.rrd |xargs -i date -d '1970-01-01 {} sec utc'
$ rrdtool first eth0.rrd |xargs -i date -d '1970-01-01 {} sec utc'

{% endhighlight %}

關於 heartbeat ，如上，如果在 300 秒內沒有收到數據，那麼就再等待 ```heartbeat-step=300``` 秒，如果還沒有收到則設置為 UNKNOWN 。

### 更新數據

{% highlight text %}
$ rrdtool {update | updatev} filename
    [--template|-t ds-name[:ds-name]...]
    N|timestamp:value[:value...]
    at-timestamp@value[:value...]
    [timestamp:value[:value...] ...]

----- 更新一個值，需要注意時間戳要大於最近更新的時間，DS數目要保持一致
$ rrdtool update eth0.rrd 1479477014:1:15

----- 兩個命令類似，不過會回顯寫入的結果，類似upate verbose
$ rrdtool updatev eth0.rrd 1479477014:1:15
{% endhighlight %}

時間定義比較靈活，可以是 ```N(date +%s)``` ```AT(man 1 at)``` 兩種格式，可以通過 ```date -d '2016-11-18 21:50:14' +%s``` 獲取時間戳。

### 獲取數據

{% highlight text %}
rrdtool fetch filename CF [--resolution|-r resolution] [--start|-s start] [--end|-e end]

----- 最簡單方式
$ rrdtool fetch eth0.rrd AVERAGE
----- 指定範圍，可以用AT方式
$ rrdtool fetch eth0.rrd AVERAGE --start end-1day --end 1164553800
----- 指定resolution，如果不存在則使用>=該值的最小值，不過需要保證改值為resolution的整數倍
$ rrdtool fetch eth0.rrd AVERAGE -r 1200 --end $((($(date +%s)/1200)*1200))
{% endhighlight %}

默認 ```--end``` 是 now ，```--start``` 是 end-1day ，也就是 1 天前；指定的 CF 類型需要保證建庫時有該 CF 類型的 RRA 才可以。通常一個文件中包含了多個 RRA ，那麼其選擇條件如下：

1. 該 RRA 必須要滿足所請求時間範圍，例如，查看兩天的數據，如果一個 RRA 保存了一天，那麼肯定不會選擇該 RRA 。
2. 如果多個 RRA 滿足，除非指定 resolution ，否者會選擇 resolution 最小的一個。

<!--
http://bbs.chinaunix.net/thread-864861-1-1.html

http://www.yunweipai.com/archives/4220.html
-->

### RRDTool 插件

在 RRDTool 插件中，比較核心的配置分為了兩部分：A) 如何創建文件；B) 如何寫文件。

{% highlight text %}
LoadPlugin rrdtool
<Plugin rrdtool>
  DataDir Directory       # 指定數據保存目錄，默認會保存在BaseDir目錄下
  CreateFilesAsync false  # 如果需要創建幾百個文件可以設置為異步，此時在文件創建完成前的數據將會丟棄，
                          #     如果同步則會阻塞
  StepSize Seconds        # 設置RRD文件的step，默認會從上報的數據中選擇step，建議不要設置
  HeartBeat Seconds       # 一般設置為step的兩倍，同樣不建議設置
  RRARows NumRows         # 默認是1200，一般保存15個RRA，是MIN, AVERAGE, MAX與hour, day, week, month,
                          #     year的組合，計算方式為PDPs=timespan/(stepsize*rrarows)
  RRATimespan Seconds     # 手動設置範圍，可以配置多次，單位是秒；如果沒有配置默認是(3600, 86400,
                          #     604800, 2678400, 31622400)
  XFF Factor              # 設置XFiles Factor參數，默認是0.1，範圍是[0.0-1.0)

  CacheFlush Seconds      # 當設置了緩存時，每次接收到新值時，如果超過了這裡設置的時間，就會檢查所有
                          #     的採集值；適用於部分指標由於異常一直沒有更新，導致始終保存在緩存中，
                          #     因為消耗資源比較大，建議設置為較大的值，如900、7200秒。
  CacheTimeout Seconds    # 大於0時會將值緩存到內存中，如果緩存的時間差超過改值，則放入到update queue隊列中
  WritesPerSecond Updates # 如果採集的指標比較多，可能會一直在刷新，那麼如果此時有 IO 操作，那麼可能會
                          #     導致阻塞。為此，可以通過該參數限制每秒更新的次數，從而不會造成過大的磁
                          #     盤壓力，建議設置為 25~80；此時也可以根據 RRD 文件數，大致評估一次刷新鎖
                          #     消耗的時間。
  RandomTimeout Seconds   # 選擇隨機的時間，其範圍是 CacheTimeout-RandomTimeout ~ CacheTimeout+RandomTimeout ，
                          #     用於防止由於同一間隔導致峰值。
</Plugin>
{% endhighlight %}

關於 RRDTool 插件，可以參考 [Inside the RRDtool plugin](https://collectd.org/wiki/index.php/Inside_the_RRDtool_plugin) 。

簡言之，如果有很多個監控指標，那麼直接更新到磁盤時，就會導致大量的小文件+隨機寫，將嚴重影響到 HDD 甚至 SSD 的寫入性能。

![collectd rrdtool plugin]({{ site.url }}/images/linux/collectd_rrdtool_plugin.png "collectd rrdtool plugin"){: .pull-center width="70%" }

更新一個值時，一般為 8 Bytes，為了更新該值需要讀取 512B(HDD)、16KB(SSD) 字節，在內存中修改對應的值，然後再寫入內存，顯然這將嚴重影響到磁盤 IO 性能，為此需要將多個指標合併寫入磁盤。

通過一個平衡二叉樹，每個節點是採集值或者 RRD 文件，每次收到一個採集值後會與最老的值比較時間戳，如果超過了設置的超時時間，那麼該節點就被放 update queue 中。如果一個新寫入的值已經在 update queue 隊列中了，那麼新值會直接寫入到 update queue 中相應的節點。


### collectd-web

直接從 [github collectd-web](https://github.com/httpdss/collectd-web) 上下載即可，在配置文件 ```/etc/collectd/collection.conf``` 中指定 ```datadir``` 目錄，也就是 rrd 保存的目錄；例如：

{% highlight text %}
datadir: "/etc/collectd/collectd-web/"
{% endhighlight %}

然後，通過如下方式運行即可。

{% highlight text %}
./runserver.py 0.0.0.0 8989
{% endhighlight %}

#### No DS

對於 ```No DS called 'contextswitches' in``` 之類的報錯，其原因是在 ```collection.modified.cgi``` 文件中的 ```load_graph_definitions()``` 寫死；解決方法是先通過 ```rrdtool info filename``` 查看是否存在對應的 DS name 是否存在，一般類似：

{% highlight text %}
ds[value].index = 0
ds[value].type = "DERIVE"
ds[value].minimal_heartbeat = 2
ds[value].min = 0.0000000000e+00
ds[value].max = NaN
ds[value].last_ds = "6950152000"
ds[value].value = 0.0000000000e+00
ds[value].unknown_sec = 0
{% endhighlight %}

其中的 value 就是所謂的 DS name 。


## 常用插件

### tail

主要用於日誌文件。


## 參考

關於告警可以參考 [Collectd Thresholds and alerting](https://www.netways.de/fileadmin/images/Events_Trainings/Events/OSMC/2015/Slides_2015/collectd_Thresholds_Plugin_and_Icinga_-_Florian_Forster.pdf) 。

<!--
http://www.linuxhowtos.org/manpages/5/collectd.conf.htm
https://collectd.org/wiki/index.php/Chains





配置文件
1. 配置項的值有三種類型：字符串、浮點型、布爾值
{% highlight text %}
A) 字符串，必須通過雙引號包裹
Hostname "localhost"                # localhost
Hostname "local'host"               # local'host
Hostname "local\"host"              # local"host
Hostname "local\"host \             # local"host locahost
         localhost"
{% endhighlight %}



插件通過 LoadPlugin 語句加載，有兩種方式：
LoadPlugin syslog        # 加載和配置分開，可以配置多次，例如Python腳本
<Plugin syslog>
    config
</Plugin>
<Plugin syslog>
    config
</Plugin>

<LoadPlugin syslog>        # 加載的同時進行配置
    config
</LoadPlugin>
-->






{% highlight text %}
{% endhighlight %}
