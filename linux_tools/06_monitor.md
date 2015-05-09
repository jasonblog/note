# 性能監控
目錄

性能監控
監控CPU
查詢內存
查詢頁面交換
查詢硬盤使用
綜合應用
總結
附錄
在使用操作系統的過程中，我們經常需要查看當前的性能如何，需要瞭解CPU、內存和硬盤的使用情況； 本節介紹的這幾個工具能滿足日常工作要求；

6.1. 監控CPU
查看CPU使用率

```
$sar -u

eg:
$sar -u 1 2
[/home/weber#]sar -u 1 2
Linux 2.6.35-22-generic-pae (MyVPS)     06/28/2014      _i686_  (1 CPU)

09:03:59 AM     CPU     %user     %nice   %system   %iowait    %steal     %idle
09:04:00 AM     all      0.00      0.00      0.50      0.00      0.00     99.50
09:04:01 AM     all      0.00      0.00      0.00      0.00      0.00    100.00
```

後面的兩個參數表示監控的頻率，比如例子中的1和2，表示每秒採樣一次，總共採樣2次；

查看CPU平均負載

```
$sar -q 1 2
```

sar指定-q後，就能查看運行隊列中的進程數、系統上的進程大小、平均負載等；

6.2. 查詢內存
查看內存使用狀況 sar指定-r之後，可查看內存使用狀況;

```
$sar -r 1 2
09:08:48 AM kbmemfree kbmemused  %memused kbbuffers  kbcached  kbcommit   %commit  kbactive   kbinact
09:08:49 AM     17888    359784     95.26     37796     73272    507004     65.42    137400    150764
09:08:50 AM     17888    359784     95.26     37796     73272    507004     65.42    137400    150764
Average:        17888    359784     95.26     37796     73272    507004     65.42    137400    150764
```

查看內存使用量

```
$free -m
```

6.3. 查詢頁面交換
查看頁面交換發生狀況 頁面發生交換時，服務器的吞吐量會大幅下降；服務器狀況不良時，如果懷疑因爲內存不足而導致了頁面交換的發生，可以使用sar -W這個命令來確認是否發生了大量的交換；

```
$sar -W 1 3
```

6.4. 查詢硬盤使用
查看磁盤空間利用情況

```
$df -h
```

查詢當前目錄下空間使用情況

```
du -sh  -h是人性化顯示 s是遞歸整個目錄的大小
```

查看該目錄下所有文件夾的排序後的大小

```
for i in `ls`; do du -sh $i; done | sort
或者
du -sh `ls`
```

6.5. 綜合應用
當系統中sar不可用時，可以使用以下工具替代：linux下有 vmstat、Unix系統有prstat

eg： 查看cpu、內存、使用情況： vmstat n m （n 爲監控頻率、m爲監控次數）

```
[/home/weber#]vmstat 1 3
procs -----------memory---------- ---swap-- -----io---- -system-- ----cpu----
r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa
0  0  86560  42300   9752  63556    0    1     1     1    0    0  0  0 99  0
1  0  86560  39936   9764  63544    0    0     0    52   66   95  5  0 95  0
0  0  86560  42168   9772  63556    0    0     0    20  127  231 13  2 84  0
```

使用watch 工具監控變化 當需要持續的監控應用的某個數據變化時，watch工具能滿足要求； 執行watch命令後，會進入到一個界面，輸出當前被監控的數據，一旦數據變化，便會高亮顯示變化情況；

eg：操作redis時，監控內存變化：

```
$watch -d -n 1 './redis-cli info | grep memory'
(以下爲watch工具中的界面內容，一旦內存變化，即實時高亮顯示變化）
Every 1.0s: ./redis-cli info | grep memory                                                                  Mon Apr 28 16:10:36 2014

used_memory:45157376
used_memory_human:43.07M
used_memory_rss:47628288
used_memory_peak:49686080
used_memory_peak_human:47.38M
```

6.6. 總結
top / sar / free / watch

6.7. 附錄
關於sar的使用詳解參考：sar 找出系統瓶頸的利器
