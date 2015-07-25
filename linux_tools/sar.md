# sar 找出系統瓶頸的利器
sar是System Activity Reporter（系統活動情況報告）的縮寫。sar工具將對系統當前的狀態進行取樣，然後通過計算數據和比例來表達系統的當前運行狀態。它的特點是可以連續對系統取樣，獲得大量的取樣數據；取樣數據和分析的結果都可以存入文件，所需的負載很小。sar是目前Linux上最為全面的系統性能分析工具之一，可以從14個大方面對系統的活動進行報告，包括文件的讀寫情況、系統調用的使用情況、串口、CPU效率、內存使用狀況、進程活動及IPC有關的活動等，使用也是較為複雜。

sar是查看操作系統報告指標的各種工具中，最為普遍和方便的；它有兩種用法；

追溯過去的統計數據（默認）
週期性的查看當前數據
### 12.1. 追溯過去的統計數據
默認情況下，sar從最近的0點0分開始顯示數據；如果想繼續查看一天前的報告；可以查看保存在/var/log/sysstat/下的sa日誌； 使用sar工具查看:
```
$sar -f /var/log/sysstat/sa28 \| head sar -r -f
/var/log/sysstat/sa28
```

### 12.2. 查看CPU使用率
sar -u : 默認情況下顯示的cpu使用率等信息就是sar -u；


可以看到這臺機器使用了虛擬化技術，有相應的時間消耗； 各列的指標分別是:

- %user 用戶模式下消耗的CPU時間的比例；
- %nice 通過nice改變了進程調度優先級的進程，在用戶模式下消耗的CPU時間的比例
- %system 系統模式下消耗的CPU時間的比例；
- %iowait CPU等待磁盤I/O導致空閒狀態消耗的時間比例；
- %steal 利用Xen等操作系統虛擬化技術，等待其它虛擬CPU計算佔用的時間比例；
- %idle CPU空閒時間比例；
### 12.3. 查看平均負載
sar -q: 查看平均負載

指定-q後，就能查看運行隊列中的進程數、系統上的進程大小、平均負載等；與其它命令相比，它能查看各項指標隨時間變化的情況；

- runq-sz：運行隊列的長度（等待運行的進程數）
- plist-sz：進程列表中進程（processes）和線程（threads）的數量
- ldavg-1：最後1分鐘的系統平均負載 ldavg-5：過去5分鐘的系統平均負載
- ldavg-15：過去15分鐘的系統平均負載


### 12.4. 查看內存使用狀況
sar -r： 指定-r之後，可查看物理內存使用狀況；
```
sar -r 1 3

Linux  shihyu-MS-7758  3.13.0-44-generic  #73-Ubuntu SMP Tue Dec 16 00:22:43 UTC 2014  x86_64  05/09/2015

01:21:10  memtot memfree buffers   cached  slabmem      swptot swpfree  _mem_
01:21:10  32125M  23697M    395M    2554M     265M        976M    976M
01:21:11  32125M  23697M    395M    2554M     265M        976M    976M
01:21:12  32125M  23697M    395M    2554M     265M        976M    976M
```

- kbmemfree：這個值和free命令中的free值基本一致,所以它不包括buffer和cache的空間.
- kbmemused：這個值和free命令中的used值基本一致,所以它包括buffer和cache的空間.
- %memused：物理內存使用率，這個值是kbmemused和內存總量(不包括swap)的一個百分比.
- kbbuffers和kbcached：這兩個值就是free命令中的buffer和cache.
- kbcommit：保證當前系統所需要的內存,即為了確保不溢出而需要的內存(RAM+swap).
- %commit：這個值是kbcommit與內存總量(包括swap)的一個百分比.

### 12.5. 查看頁面交換發生狀況
sar -W：查看頁面交換發生狀況
```
sar -W 1 3

Linux  shihyu-MS-7758  3.13.0-44-generic  #73-Ubuntu SMP Tue Dec 16 00:22:43 UTC 2014  x86_64  05/09/2015

01:20:17  in: dsc/s hder/s ader/s unkp/s ratim/s rfail/s ot: dsc/s nort/s  _ip_
01:20:17        0.0    0.0    0.0    0.0     0.0     0.0       0.0    0.0
01:20:18        0.0    0.0    0.0    0.0     0.0     0.0       0.0    0.0
01:20:19        0.0    0.0    0.0    0.0     0.0     0.0       0.0    0.0
```

頁面發生交換時，服務器的吞吐量會大幅下降；服務器狀況不良時，如果懷疑因為內存不足而導致了頁面交換的發生，可以使用這個命令來確認是否發生了大量的交換；

../_images/sar5.png
pswpin/s：每秒系統換入的交換頁面（swap page）數量
pswpout/s：每秒系統換出的交換頁面（swap page）數量
要判斷系統瓶頸問題，有時需幾個 sar 命令選項結合起來；

懷疑CPU存在瓶頸，可用 sar -u 和 sar -q 等來查看
懷疑內存存在瓶頸，可用sar -B、sar -r 和 sar -W 等來查看
懷疑I/O存在瓶頸，可用 sar -b、sar -u 和 sar -d 等來查看
### 12.6. 安裝
有的linux系統下，默認可能沒有安裝這個包，使用apt-get install sysstat 來安裝；
安裝完畢，將性能收集工具的開關打開： vi /etc/default/sysstat
設置 ENABLED=”true”
啟動這個工具來收集系統性能數據： /etc/init.d/sysstat start
### 12.7. sar參數說明
- -A 彙總所有的報告
- -a 報告文件讀寫使用情況
- -B 報告附加的緩存的使用情況
- -b 報告緩存的使用情況
- -c 報告系統調用的使用情況
- -d 報告磁盤的使用情況
- -g 報告串口的使用情況
- -h 報告關於buffer使用的統計數據
- -m 報告IPC消息隊列和信號量的使用情況
- -n 報告命名cache的使用情況
- -p 報告調頁活動的使用情況
- -q 報告運行隊列和交換隊列的平均長度
- -R 報告進程的活動情況
- -r 報告沒有使用的內存頁面和硬盤塊
- -u 報告CPU的利用率
- -v 報告進程、i節點、文件和鎖表狀態
- -w 報告系統交換活動狀況
- -y 報告TTY設備活動狀況

