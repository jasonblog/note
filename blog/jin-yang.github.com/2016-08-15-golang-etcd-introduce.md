---
title: ETCD 簡介
layout: post
comments: true
language: chinese
category: [program,golang,linux]
keywords: golang,go,etcd
description: Etcd 是一個分佈式可靠的鍵值存儲系統，提供了與 ZooKeeper 相似的功能，通過 GoLang 開發而非 Java ，採用 RAFT 算法而非 PAXOS 算法。相比來所，etcd 的安裝使用更加簡單有效。
---

Etcd 是一個分佈式可靠的鍵值存儲系統，提供了與 ZooKeeper 相似的功能，通過 GoLang 開發而非 Java ，採用 RAFT 算法而非 PAXOS 算法。

相比來所，etcd 的安裝使用更加簡單有效。

<!-- more -->

## 簡介

A distributed, reliable key-value store for the most critical data of a distributed system.

嚴格來說，ETCD 主要用於保存一些元數據信息，一般小於 1GB 對大於 1GB 的可以使用新型的分佈式數據庫，例如 TiDB 等，通常適用於 CP 場景。

## 安裝

可以直接從 [github release](https://github.com/coreos/etcd/releases) 下載非源碼包，也就是已經編譯好的二進制包，一般包括了 etcd + etcdctl 。

#### 單機單進程測試

啟動單進程服務，並進行測試。

{% highlight text %}
----- 啟動單個本地進程，會監聽127.1:2379端口
$ ./etcd

----- 使用API v3版本，並測試添加獲取參數
$ export ETCDCTL_API=3
$ ./etcdctl put foo bar
OK
$ ./etcdctl get foo
foo
bar

$ ./etcdctl --write-out=table --endpoints=localhost:2379 member list

----- 只打印值信息，不打印key
$ ./etcdctl get foo --print-value-only
bar
----- 打印十六進制格式
$ ./etcdctl get foo --hex
\x66\x6f\x6f
\x62\x61\x72
----- 指定範圍為foo~foo3
$ ./etcdctl get foo foo3
foo
foo1
foo2
foo3
----- 指定前綴，且只顯示前兩個
$ ./etcdctl get --prefix --limit=2 foo
foo
foo1
{% endhighlight %}



<!--
--name
  集群中節點名，可區分且不重複即可；
--listen-peer-urls
監聽的用於節點之間通信的url，可監聽多個，集群內部將通過這些url進行數據交互(如選舉，數據同步等)
--initial-advertise-peer-urls
建議用於節點之間通信的url，節點間將以該值進行通信。
--listen-client-urls
監聽的用於客戶端通信的url,同樣可以監聽多個。
--advertise-client-urls
建議使用的客戶端通信url,該值用於etcd代理或etcd成員與etcd節點通信。
--initial-cluster-token etcd-cluster-1
節點的token值，設置該值後集群將生成唯一id,併為每個節點也生成唯一id,當使用相同配置文件再啟動一個集群時，只要該token值不一樣，etcd集群就不會相互影響。
--initial-cluster
也就是集群中所有的initial-advertise-peer-urls 的合集
--initial-cluster-state new
新建集群的標誌

./etcd --name infra0 --initial-advertise-peer-urls http://10.0.1.111:2380 \
  --listen-peer-urls http://10.0.1.111:2380 \
  --listen-client-urls http://10.0.1.111:2379,http://127.0.0.1:2379 \
  --advertise-client-urls http://10.0.1.111:2379 \
  --initial-cluster-token etcd-cluster-1 \
  --initial-cluster infra0=http://10.0.1.111:2380,infra1=http://10.0.1.109:2380,infra2=http://10.0.1.110:2380 \
  --initial-cluster-state new

./etcd --name infra1 --initial-advertise-peer-urls http://10.0.1.109:2380 \
  --listen-peer-urls http://10.0.1.109:2380 \
  --listen-client-urls http://10.0.1.109:2379,http://127.0.0.1:2379 \
  --advertise-client-urls http://10.0.1.109:2379 \
  --initial-cluster-token etcd-cluster-1 \
  --initial-cluster infra0=http://10.0.1.111:2380,infra1=http://10.0.1.109:2380,infra2=http://10.0.1.110:2380 \
  --initial-cluster-state new

./etcd --name infra2 --initial-advertise-peer-urls http://10.0.1.110:2380 \
  --listen-peer-urls http://10.0.1.110:2380 \
  --listen-client-urls http://10.0.1.110:2379,http://127.0.0.1:2379 \
  --advertise-client-urls http://10.0.1.110:2379 \
  --initial-cluster-token etcd-cluster-1 \
  --initial-cluster infra0=http://10.0.1.111:2380,infra1=http://10.0.1.109:2380,infra2=http://10.0.1.110:2380 \
  --initial-cluster-state new

按如上配置分別啟動集群，啟動集群后，將會進入集群選舉狀態，若出現大量超時，則需要檢查主機的防火牆是否關閉，或主機之間是否能通過2380端口通信，集群建立後通過以下命令檢查集群狀態。

數據持久化基於多版本併發控制，


性能壓測的核心指標包括了：延遲(Latency)、吞吐量(Throughput)；延遲表示完成一次請求的處理時間；吞吐量表示某個時間段內完成的請求數。

一般來說，當吞吐量增加增加時，對應的延遲也會同樣增加；低負載時 1ms 以內可以完成一次請求，高負載時可以完成 3W/s 的請求。對於 ETCD 來說，會涉及到一致性以及持久化的問題，所以其性能與網絡帶寬、磁盤IO的性能緊密相關。

其底層的存儲基於 BoltDB，一個基於 GoLang 的 KV 數據庫，支持 ACID 特性，與 lmdb 相似。

https://coreos.com/etcd/docs/latest/op-guide/performance.html
-->


<!--
configuration management, service discovery, and coordinating distributed work. Many organizations use etcd to implement production systems such as container schedulers, service discovery services, and distributed data storage. Common distributed patterns using etcd include leader election, distributed locks, and monitoring machine liveness.
-->


## API

實際上 API 基本上決定了 etcd 提供了哪些服務，通過 HTTP API 對外提供服務，這種接口更方便各種語言對接，命令行可以使用 httpie 或者 curl 調用。

數據按照樹形的結構組織，類似於 Linux 的文件系統，也有目錄和文件的區別，不過一般被稱為 nodes，其中數據相關的 endpoint 都是以 `/v2/keys` 開頭 (v2 表示當前 API 的版本)，比如 `/v2/keys/names/cizixs` 。

要創建一個值，只要使用 PUT 方法在對應的 url endpoint 設置就行。如果對應的 key 已經存在， PUT 也會對 key 進行更新。

### CURD

{% highlight text %}
----- 不存在則創建，否則修改，當超過TTL後，會自動刪除
http PUT http://127.0.0.1:2379/v2/keys/message value=="hello, etcd" ttl==5
http GET http://127.0.0.1:2379/v2/keys/message
http DELETE http://127.0.0.1:2379/v2/keys/message
{% endhighlight %}

在創建 key 的時候，如果它所在路徑的目錄不存在，會自動被創建，所以在多數情況下我們不需要關心目錄的創建，如果要單獨創建一個目錄可以指定參數 `dir=true`。

{% highlight text %}
http PUT http://127.0.0.1:2379/v2/keys/anotherdir dir==true
{% endhighlight %}

注意，ECTD 提供了類似 Linux 中 `.` 開頭的隱藏機制，以 `_` 開頭的節點也是默認隱藏的，不會在列出目錄的時候顯示，只有知道隱藏節點的完整路徑，才能夠訪問它的信息。


### 監聽機制

通過監聽機制，可以在某個 key 發生變化時，通知對應的客戶端，主要用於服務發現，集群中有信息更新時可以被及時發現並作出相應處理。

{% highlight text %}
http http://127.0.0.1:2379/v2/keys/foo wait==true
{% endhighlight %}

使用 `recursive=true` 參數，可以用來監聽某個目錄。

### 比較更新

在分佈式環境中，需要解決多個客戶端的競爭問題，通過 etcd 提供的原子操作 CompareAndSwap (CAS)，可以很容易實現分佈式鎖。簡單來說，這個命令只有在客戶端提供的條件成立的情況下才會更新對應的值。

{% highlight text %}
http PUT http://127.0.0.1:2379/v2/keys/foo prevValue==bar value==changed
{% endhighlight %}

只有當之前的值為 bar 時，才會將其更新成 changed 。

### 比較刪除

同樣是原子操作，只有在客戶端提供的條件成立的情況下，才會執行刪除操作；支持 prevValue 和 prevIndex 兩種條件檢查，沒有 prevExist，因為刪除不存在的值本身就會報錯。

{% highlight text %}
http DELETE http://127.0.0.1:2379/v2/keys/foo prevValue==bar
{% endhighlight %}

### 監控集群

Etcd 還保存了集群的數據信息，包括節點之間的網絡信息，操作的統計信息。

<!--
/v2/stats/leader  會返回集群中 leader 的信息，以及 followers 的基本信息
/v2/stats/self 會返回當前節點的信息
/v2/state/store：會返回各種命令的統計信息
-->

### 成員管理

在 `/v2/members` 下保存著集群中各個成員的信息。

<!--
/version   獲取版本服務器以及集群的版本號
-->


## 常見操作


### etcdctl

這個實際上是封裝了 HTTP 請求的一個客戶端，用於更方便的與服務端進行交互。

{% highlight text %}
----- 設置一個key的值
$ etcdctl set /message "hello, etcd"
hello, etcd

----- 獲取key的值
$ etcdctl get /message
hello, etcd

----- 獲取key值的同時，顯示更詳細的元數據信息
$ etcdctl -o extended get /message
Key: /message
Created-Index: 1073
Modified-Index: 1073
TTL: 0
Index: 1073

hello, etcd

----- 如果獲取的key不存在，則會直接報錯
$ etcdctl get /notexist
Error:  100: Key not found (/notexist) [1048]

----- 設置key的ttl，過期後會被自動刪除
$ etcdctl set /tempkey "gone with wind" --ttl 5
gone with wind

----- 如果key的值是"hello, etcd"，就把它替換為"goodbye, etcd"
$ etcdctl set --swap-with-value "hello, world" /message "goodbye, etcd"
Error:  101: Compare failed ([hello, world != hello, etcd]) [1050]
$ etcdctl set --swap-with-value "hello, etcd" /message "goodbye, etcd"
goodbye, etcd

----- 僅當key不存在時創建
$ etcdctl mk /foo bar
bar
$ etcdctl mk /foo bar
Error:  105: Key already exists (/foo) [1052]

----- 自動創建排序的key
$ etcdctl mk --in-order /queue job1
job1
$ etcdctl mk --in-order /queue job2
job2
$ etcdctl ls --sort /queue
/queue/00000000000000001053
/queue/00000000000000001054

----- 更新key的值或者ttl，只有當key已經存在的時候才會生效，否則報錯
$ etcdctl update /message "I'am changed"
I'am changed
$ etcdctl get /message
I'am changed
$ etcdctl update /notexist "I'am changed"
Error:  100: Key not found (/notexist) [1055]
$ etcdctl update --ttl 3 /message "I'am changed"
I'am changed
$ etcdctl get /message
Error:  100: Key not found (/message) [1057]

----- 刪除某個key
$ etcdctl mk /foo bar
bar
$ etcdctl rm /foo
PrevNode.Value: bar
$ etcdctl get /foo
Error:  100: Key not found (/foo) [1062]

----- 只有當key的值匹配的時候，才進行刪除
$ etcdctl mk /foo bar
bar
$ etcdctl rm --with-value wrong /foo
Error:  101: Compare failed ([wrong != bar]) [1063]
$ etcdctl rm --with-value bar /foo

----- 創建一個目錄
$ etcdctl mkdir /dir

----- 刪除空目錄
$ etcdctl mkdir /dir/subdir/
$ etcdctl rmdir /dir/subdir/

----- 刪除非空目錄
$ etcdctl rmdir /dir
Error:  108: Directory not empty (/dir) [1071]
$ etcdctl rm --recursive /dir

----- 列出目錄的內容
$ etcdctl ls /
/queue
/anotherdir
/message

----- 遞歸列出目錄的內容
$ etcdctl ls --recursive /
/anotherdir
/message
/queue
/queue/00000000000000001053
/queue/00000000000000001054

----- 監聽某個key，當key改變的時候會打印出變化
$ etcdctl watch /message
changed

----- 監聽某個目錄，當目錄中任何node改變的時候，都會打印出來
$ etcdctl watch --recursive /
[set] /message
changed

----- 一直監聽，除非CTRL + C導致退出監聽
$ etcdctl watch --forever /message
new value
chaned again
Wola

----- 監聽目錄，並在發生變化的時候執行一個命令
$ etcdctl exec-watch --recursive / -- sh -c "echo change detected."
change detected.
change detected.
{% endhighlight %}

**注意** 默認只保存了 1000 個歷史事件，所以不適合有大量更新操作的場景，這樣會導致數據的丟失，其使用的典型應用場景是配置管理和服務發現，這些場景都是讀多寫少的。

## 參考


<!--
https://tonydeng.github.io/2015/10/19/etcd-application-scenarios/
http://jolestar.com/etcd-architecture/
http://www.infoq.com/cn/articles/etcd-interpretation-application-scenario-implement-principle

場景一：服務發現（Service Discovery）
場景二：消息發佈與訂閱
場景三：負載均衡
場景四：分佈式通知與協調
場景五：分佈式鎖、分佈式隊列
場景六：集群監控與Leader競選
-->

{% highlight text %}
{% endhighlight %}
