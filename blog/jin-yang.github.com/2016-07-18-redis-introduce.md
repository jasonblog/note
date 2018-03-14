---
title: Redis 簡介
layout: post
comments: true
language: chinese
category: [misc]
keywords: hello world,示例,sample,markdown
description: 簡單記錄一下一些與 Markdown 相關的內容，包括了一些使用模版。
---

REmote DIctionary Server (Redis) 是一個由 Salvatore Sanfilippo 寫的 KV 存儲系統，採用 C 語言實現，同時提供了多種語言的 API 。

Redis 是完全開源免費的高性能 KV 數據庫，相比其它的 KV 數據庫來說，Redis 還支持持久化，可以將內存中的數據保存到磁盤中，重啟時再次加載；除了 KV 接口，還支持 list、set、hash 等多種數據庫結構；同時支持備份以及集群模式。


<!-- more -->

## 簡介

Redis 是一個高性能的內存非關係型數據庫，如下是一些特性：

* 提供多種持久化方式，可以將內存中的數據保存在磁盤中，並在重啟時進行數據恢復。
* 提供了 5 種數據結構類型分別為 STRING、LIST、SET、HASH 和 ZSET (有序集合)，同時提供了原子性操作。
* 支持主備模式的數據備份。
* 支持集群模式。

常用於緩存、隊列等場景。

## 常用命令

可以使用命令行直接獲取。

{% highlight text %}
----- 通過YUM直接安裝
# yum --enablerepo=epel install redis

# redis-cli shutdown

# redis-cli
127.0.0.1:6379> set foo bar
OK
127.0.0.1:6379> get foo
"bar"

----- 查看服務器信息
# redis-cli info

------ 通過管道批量執行
# cat cmds.txt | redis-cli > cmds_out.txt
{% endhighlight %}


### Hash 類型操作

{% highlight text %}
----- 設置單個或者多個值
hset key field value
hmset key field value [field value ...]

----- 獲取值
hget key field
hmget key field [key field ...]
hgetall key                             # 獲取字段的名稱及其值
hvals key                               # 只獲取字段對應的值

----- 刪除字段，會返回刪除字段的個數
hdel key field [field ...]

----- 判斷是否存在
hexists key field

----- 當字段不存在時賦值
hsetnx key field value

----- 增加int類型的字段值，每次增加NUM
hincrby key field NUM

----- 其它
hkeys key               # 獲取KEY字段名
hlen key                # 獲取KEY字段數量
{% endhighlight %}

### List 類型操作

{% highlight text %}
lpush key value [value ...]
lpushx key value                # 插入到頭部
lrange key start stop
lpop key
llen key                        # 查看鏈表中的數量
lrem key count value
lset key index value
lindex key index

ltrim key start stop
{% endhighlight %}

### Set 類型操作

Set 是 string 類型對象的無序集合，對對象操作的時間複雜度是 `O(1)`，最多可以包含 `2^32 - 1` 個對象，使用 SET 操作時，原有的值會被覆蓋，即使是不同的類型。

{% highlight text %}
----- 添加成員，返回增加元素的個數，對相同的add，只保存最早的一份
sadd key member [member ...]

----- 獲取key的所有成員
smembers key

----- 計算多個set的並集
sunion key [key ...]

----- 計算多個set的交集
sinter key [key ...]
{% endhighlight %}

<!--
sismemeber key member            # 判斷是否是set的成員
scard key                        # 返回成員個數
srandmember key                  # 隨機返回成員
spop key                         # 移出並返回隨機的成員
srem key member [member ...]
smove src dst member             # 將成員從src移動到dst
sunionstore dest key [key ...]   多個set的並集，並保存到dest
-->

另外一種類型就是有序的 SET，也就是 zset，在 set 的基礎上增加了順序，形成一個有序的集合。

## 持久化

Redis 可以將數據同步保存到磁盤做數據持久化，用於重啟後的數據恢復，目前主要由兩種方式：A) Snapshot(RDB) 默認方式；B) Append Only File(AOF) 方式。

### RDB

默認持久化方式，將當前數據的快照保存成一個數據文件，可以按照一定的策略週期性的將數據保存到磁盤，其對應產生的數據文件為 dump.rdb ，通過配置文件中的 save 參數來定義快照的週期，也可以通過客戶端 save/bgsave 手動做快照持久化。

實際上 Redis 藉助了 fork 命令的 Copy On Write 機制，在生成快照時，將當前進程 fork 出一個子進程，然後在子進程中循環所有的數據，將數據寫成為 RDB 文件。

另外需要注意，save 操作是在主線程中保存快照，而且 Redis 是通過主線程來處理所有客戶端請求，因此這種方式會阻塞所有客戶端的請求，因此不推薦使用。而且，每次快照持久化都是將內存數據完整寫入到磁盤一次，而非增量同步，當數據量大時，會引起大量的磁盤 IO ，從而嚴重影響性能。

關於 RDB 文件一致性的問題，實際上寫操作是在一個新進程中完成的，會先將數據寫入到一個臨時文件中，然後利用文件系統的原子 rename 操作將文件重命名為 RDB 文件，從而可以保證故障是 RDB 文件的完整性。

#### 配置項

{% highlight text %}
save 60 10000                    # 設置備份頻率，也就是多少秒內至少要多個key被修改
stop-writes-on-bgsave-error yes  # 當備份出現錯誤時停止備份
rdbcompression yes               # 在進行鏡像備份時是否壓縮，yes壓縮，也就是時間換空間
rdbchecksum yes                  # 會在文件末尾添加CRC64校驗，當保存或者加載時會有部分的性能損耗
dbfilename dump.rdb              # 快照的文件名
dir /var/lib/redis               # 以及存放快照的目錄
{% endhighlight %}

### AOF

Append Only File 持久化會將每個收到的寫命令都通過 Write 函數追加到文件最後，類似於 MySQL 的 binlog ，當重啟時會重新執行文件中保存的寫命令來在內存中重建整個數據庫的內容。

這種方式會導致 AOF 文件越來越大，為此提供了 bgrewriteaof 命令可以通過類似 RDB 的方式進行數據保存。

**注意** 如果同時配置了 RBD 和 AOF ，那麼啟動時只加載 AOF 文件恢復數據。

#### 配置項

{% highlight text %}
appendonly yes                   # 是否開啟AOF，默認關閉
appendfilename appendonly.aof    # 指定 AOF 文件名
appendfsync everysec             # 刷新模式總共三種，always每條命令都會刷新，everysec每秒鐘刷新一次，no依賴OS刷新機制
no-appendfsync-on-rewrite yes    # 日誌重寫時不做fsync操作，暫時緩存在內存中
auto-aof-rewrite-percentage 100  # 當前AOF文件是上次重寫AOF二倍時，自動啟動新的日誌重寫
auto-aof-rewrite-min-size 64mb   # 啟動新日誌重寫的最小值，避免頻繁重寫
{% endhighlight %}

### 比較

從上面處理邏輯可以看出，兩種方式都是順序 IO 操作，性能都很高；在恢復時一般也是順序加載，同樣只有很少的隨機讀。

通常，如果想要提供很高的數據保障性，可以同時使用兩種持久化方式；如果可以接受丟失幾分鐘數據，那麼可以僅使用 RDB ，因為 RDB 可以經常做數據的完整快照，並提供更快的重啟，所以最好還是使用 RDB 。

RDB 的啟動時間更短原因主要有兩個：A) 文件中每條數據只有一條記錄，而 AOF 對於同一條數據可能需要操作多次；B) RDB 中的數據存儲格式跟 Redis 內存編碼格式相同，無需編碼轉換。

注意，在執行備份時，如果會有頻繁的更新，可能會導致內存 double 。例如，fork 出來進行 dump 操作的子進程會佔用與父進程一樣的內存，如果在 dump 期間，內存被修改執行了 Copy-On-Write 操作，最大就可能會佔用 12G 內存空間，所以在使用時一定要做好容量規劃。

目前，一般的設計思路是利用 Replication 機制來彌補 Snapshot、AOF 在性能上的不足，也就是 Master 不做備份保證性能，在 Slave 則可以同時開啟 Snapshot 和 AOF 來進行持久化，保證數據的安全性。

















































## 常用概念

### 數據庫

Redis 可以劃分邏輯數據庫，類似於命名空間，不同的數據庫中命名不會衝突，只是不能對其進行命名，只能是通過一個整數索引進行標識(從0開始)，默認連接到 0 ，可以通過 SELECT 選擇不同的數據庫。

可以在配置文件中，通過 `databases 64` 配置項限制支持的最大數量，默認是 16 個。

在持久化時，會將所有的數據庫保存在相同的文件中，但是部分命令可以針對單獨的數據庫，例如 FLUSHDB、SWAPDB、RANDOMKEY ，而 FLUSHALL 會清除所有 DB 。

實際使用時，建議不要將多個應用通過 DB 隔離。另外需要注意，在集群模式中，只有 0 這個數據庫。

## 客戶端

### Python

Redis-py 是 Python 中的客戶端，可以通過模塊與 Redis 服務器進行交互。

{% highlight text %}
----- 可以通過PIP安裝
# apt-get install python-pip
# pip install --proxy=http://your-proxy-ip:port redis

----- 或者使用easy_install方式安裝
# easy_install redis

----- 也可以通過源碼安裝
# tar -xf redis-2.10.6.tar.gz && cd redis-2.10.6
# python setup.py install
{% endhighlight %}

詳細內容可以參考 https://pypi.python.org/pypi/redis 以及 https://github.com/andymccurdy/redis-py 。

#### 連接池

Redis-py 使用連接池維護與 Redis 服務器的連接，默認每次創建都會返回一個連接池實例，也可以在創建時指定一個現有的連接池。

{% highlight python %}
pool = redis.ConnectionPool(host='localhost', port=6379, db=0)
r = redis.Redis(connection_pool=pool)
{% endhighlight %}

Redis 連接實例是線程安全的，可以直接將連接作為全局變量使用，沒有實現 SELECT 接口，如果需要鏈接到另一個 Redis 實例時，就需要重新創建 Redis 連接實例來獲取一個新的連接。

Python 鏈接庫的實現接口與 Redis 提供的 API 略有區別，如上訴的 SELECT 接口。

**注意** 對於鏈接池，如果使用了 SELECT 命令，那麼在返還連接的時候該鏈接除非顯示重新調用 SELECT 指令，否則會導致不一致問題。

#### 基本腳本

{% highlight python %}
import redis
r = redis.Redis(host='localhost',port=6379,db=0)
r.set('foobar', 'Hello World!')
r.get('foobar')
r['foobar']
r.keys()
r.dbsize()         # 當前數據庫包含多少條數據
r.delete('guo')
r.save()           # 將內存中的數據寫回磁盤，會阻塞
r.flushdb()        # 清空當前數據庫中的所有數據
{% endhighlight %}

#### 批量執行

通過管道可以將多個命令通過單個請求發送到服務器，可以有效減少服務器和客戶端之間反覆的 TCP 數據交互，提高執行批量執行命令。

{% highlight python %}
p = r.pipeline()
p.set('hello','redis')
p.sadd('faz','baz')
p.incr('num')
p.execute()
{% endhighlight %}

甚至也可以將多個命令合併到一起。

{% highlight python %}
p.set('hello','redis').sadd('faz','baz').incr('num').execute()
{% endhighlight %}

默認的情況下，管道里執行的命令會保證原子性，可以在初始化的時候進行設置，也就是通過 `pipe = r.pipeline(transaction=False)` 禁用這一特性。


## 監控

在 Redis 中可以通過 `CONFIG SET` 去修改配置，不過這個只會修改內存，不會持久化到配置文件，為了防止出現不一致，需要通過定期巡檢檢查是否一致。

Redis 提供了很多命令用戶查看當前的執行狀態，例如當前客戶端信息可以通過 `CLIENT LIST` 命令查看，集群信息可以通過 `CLUSTER INFO` 命令查看等等。

其中使用最多的是通過 INFO 查看系統整體的狀態信息，在 Python 中可以通過如下方式採集內存相關狀態信息。


<!--
{% highlight text %}
----- 查看KEY時可以使用*,?,[]通配符
keys *         # 查看當前數據庫所有的key
type key       # 查看KEY對應的數據類型
monitor        # 打印所有發送到服務器的命令

exists foo   查看是否存在
del foo      刪除對象
move foo 1   移動到ID為1數據庫
select 1     打開ID為1的數據庫
rename foo hello  重命名

----- 超時時間設置相關
expire key seconds      # 設置將該健值的超時時間，單位為秒
expireat key timestamp  # 將該健值超時時間設置為具體的時間點
ttl foo                 # 查看剩餘時間
persist foo             # 取消超時時間

randomkey       隨機返回任意健值
flushdb         清空所有數據
{% endhighlight %}

需要注意的是，當使用 KEYS 命令查看時，Redis 都會遍厲數據庫中的所有 KEY，也就意味著該操作會非常耗時，如果有類似的需求的時候，最後將這些值存放到 set 類型變量中。
-->

## 參考

中文社區 [www.redis.cn](http://www.redis.cn/)，[C 客戶端hiredis](https://github.com/redis/hiredis)，[Redis.io 命令參考](https://redis.io/commands) 。

[twemproxy](https://github.com/twitter/twemproxy) 一個 Redis 和 Memcached 的代理，由 twitter 開發並開源。

<!--
Redis Cookbook

http://debugo.com/python-redis/

http://redisdoc.com/server/info.html
Redis運維集合
http://www.redisfans.com/?p=55
http://www.ywnds.com/?cat=63
Redis PipeLine 性能優化
https://redis.io/topics/pipelining
CAP 理論
http://blog.csdn.net/dc_726/article/details/42784237
通過集群，Redis 完成了從 CP 到 AP 的轉換，詳細介紹可以參考，在使用集群之前，一般採用Sentinel方案。
https://redis.io/topics/cluster-tutorial
Redis Cluster設計總覽
http://blog.csdn.net/dc_726/article/details/48552531
-->

{% highlight text %}
{% endhighlight %}
