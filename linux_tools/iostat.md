# iostat 監視I/O子系統
iostat是I/O statistics（輸入/輸出統計）的縮寫，用來動態監視系統的磁盤操作活動。

### 11.1. 命令格式
iostat[參數][時間][次數]

### 11.2. 命令功能
通過iostat方便查看CPU、網卡、tty設備、磁盤、CD-ROM 等等設備的活動情況, 負載信息。

### 11.3. 命令參數
- -C 顯示CPU使用情況
- -d 顯示磁盤使用情況
- -k 以 KB 爲單位顯示
- -m 以 M 爲單位顯示
- -N 顯示磁盤陣列(LVM) 信息
- -n 顯示NFS 使用情況
- -p[磁盤] 顯示磁盤和分區的情況
- -t 顯示終端和CPU的信息
- -x 顯示詳細信息
- -V 顯示版本信息

### 11.4. 工具實例
### 實例1：顯示所有設備負載情況
```
/root$iostat
Linux 2.6.32-279.el6.x86_64 (colin)   07/16/2014      _x86_64_        (4 CPU)

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
10.81    0.00   14.11    0.18    0.00   74.90

Device:            tps   Blk_read/s   Blk_wrtn/s   Blk_read   Blk_wrtn
sda               1.95         1.48        70.88    9145160  437100644
dm-0              3.08         0.55        24.34    3392770  150087080
dm-1              5.83         0.93        46.49    5714522  286724168
dm-2              0.01         0.00         0.05      23930     289288
```

### cpu屬性值說明：
- %user：CPU處在用戶模式下的時間百分比。
- %nice：CPU處在帶NICE值的用戶模式下的時間百分比。
- %system：CPU處在系統模式下的時間百分比。
- %iowait：CPU等待輸入輸出完成時間的百分比。
- %steal：管理程序維護另一個虛擬處理器時，虛擬CPU的無意識等待時間百分比。
- %idle：CPU空閒時間百分比。

注：如果%iowait的值過高，表示硬盤存在I/O瓶頸，%idle值高，表示CPU較空閒，如果%idle值高但系統響應慢時，有可能是CPU等待分配內存，此時應加大內存容量。%idle值如果持續低於10，那麼系統的CPU處理能力相對較低，表明系統中最需要解決的資源是CPU。

### disk屬性值說明：
- rrqm/s: 每秒進行 merge 的讀操作數目。即 rmerge/s
- wrqm/s: 每秒進行 merge 的寫操作數目。即 wmerge/s
- r/s: 每秒完成的讀 I/O 設備次數。即 rio/s
- w/s: 每秒完成的寫 I/O 設備次數。即 wio/s
- rsec/s: 每秒讀扇區數。即 rsect/s
- wsec/s: 每秒寫扇區數。即 wsect/s
- r kB/s: 每秒讀K字節數。是 rsect/s 的一半，因爲每扇區大小爲512字節。
- wkB/s: 每秒寫K字節數。是 wsect/s 的一半。
- avgrq-sz: 平均每次設備I/O操作的數據大小 (扇區)。
- avgqu-sz: 平均I/O隊列長度。
- wait: 平均每次設備I/O操作的等待時間 (毫秒)。
- svctm: 平均每次設備I/O操作的服務時間 (毫秒)。
- %util: 一秒中有百分之多少的時間用於 I/O 操作，即被io消耗的cpu百分比

備註：如果 %util 接近 100%，說明產生的I/O請求太多，I/O系統已經滿負荷，該磁盤可能存在瓶頸。如果 svctm 比較接近 await，說明 I/O 幾乎沒有等待時間；如果 await 遠大於 svctm，說明I/O 隊列太長，io響應太慢，則需要進行必要優化。如果avgqu-sz比較大，也表示有當量io在等待。

### 實例2：定時顯示所有信息
```
/root$iostat 2 3
Linux 2.6.32-279.el6.x86_64 (colin)   07/16/2014      _x86_64_        (4 CPU)

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
10.81    0.00   14.11    0.18    0.00   74.90

Device:            tps   Blk_read/s   Blk_wrtn/s   Blk_read   Blk_wrtn
sda               1.95         1.48        70.88    9145160  437106156
dm-0              3.08         0.55        24.34    3392770  150088376
dm-1              5.83         0.93        46.49    5714522  286728384
dm-2              0.01         0.00         0.05      23930     289288

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
22.62    0.00   19.67    0.26    0.00   57.46

Device:            tps   Blk_read/s   Blk_wrtn/s   Blk_read   Blk_wrtn
sda               2.50         0.00        28.00          0         56
dm-0              0.00         0.00         0.00          0          0
dm-1              3.50         0.00        28.00          0         56
dm-2              0.00         0.00         0.00          0          0

avg-cpu:  %user   %nice %system %iowait  %steal   %idle
22.69    0.00   19.62    0.00    0.00   57.69

Device:            tps   Blk_read/s   Blk_wrtn/s   Blk_read   Blk_wrtn
sda               0.00         0.00         0.00          0          0
dm-0              0.00         0.00         0.00          0          0
dm-1              0.00         0.00         0.00          0          0
dm-2              0.00         0.00         0.00          0          0
```
說明：每隔 2秒刷新顯示，且顯示3次

### 實例3：查看TPS和吞吐量
```
/root$iostat -d -k 1 1
Linux 2.6.32-279.el6.x86_64 (colin)   07/16/2014      _x86_64_        (4 CPU)

Device:            tps    kB_read/s    kB_wrtn/s    kB_read    kB_wrtn
sda               1.95         0.74        35.44    4572712  218559410
dm-0              3.08         0.28        12.17    1696513   75045968
dm-1              5.83         0.46        23.25    2857265  143368744
dm-2              0.01         0.00         0.02      11965     144644
```

- tps：該設備每秒的傳輸次數（Indicate the number of transfers per second that were issued to the device.）。“一次傳輸”意思是“一次I/O請求”。多個邏輯請求可能會被合併爲“一次I/O請求”。“一次傳輸”請求的大小是未知的。
- kB_read/s：每秒從設備（drive expressed）讀取的數據量；
- kB_wrtn/s：每秒向設備（drive expressed）寫入的數據量；
- kB_read：讀取的總數據量；kB_wrtn：寫入的總數量數據量；
這些單位都爲Kilobytes。

上面的例子中，我們可以看到磁盤sda以及它的各個分區的統計數據，當時統計的磁盤總TPS是1.95，下面是各個分區的TPS。（因爲是瞬間值，所以總TPS並不嚴格等於各個分區TPS的總和）

### 實例4：查看設備使用率（%util）和響應時間（await）
```
/root$iostat -d -x -k 1 1
Linux 2.6.32-279.el6.x86_64 (colin)   07/16/2014      _x86_64_        (4 CPU)

Device:         rrqm/s   wrqm/s     r/s     w/s    rkB/s    wkB/s avgrq-sz avgqu-sz   await  svctm  %util
sda               0.02     7.25    0.04    1.90     0.74    35.47    37.15     0.04   19.13   5.58   1.09
dm-0              0.00     0.00    0.04    3.05     0.28    12.18     8.07     0.65  209.01   1.11   0.34
dm-1              0.00     0.00    0.02    5.82     0.46    23.26     8.13     0.43   74.33   1.30   0.76
dm-2              0.00     0.00    0.00    0.01     0.00     0.02     8.00     0.00    5.41   3.28   0.00
```

- rrqm/s： 每秒進行 merge 的讀操作數目.即 delta(rmerge)/s
- wrqm/s： 每秒進行 merge 的寫操作數目.即 delta(wmerge)/s
- r/s： 每秒完成的讀 I/O 設備次數.即 delta(rio)/s
- w/s： 每秒完成的寫 I/O 設備次數.即 delta(wio)/s
- rsec/s： 每秒讀扇區數.即 delta(rsect)/s
- sec/s： 每秒寫扇區數.即 delta(wsect)/s
- rkB/s： 每秒讀K字節數.是 rsect/s 的一半,因爲每扇區大小爲512字節.(需要計算)
- wkB/s： 每秒寫K字節數.是 wsect/s 的一半.(需要計算)
- avgrq-sz：平均每次設備I/O操作的數據大小 (扇區).delta(rsect+wsect)/delta(rio+wio)
- avgqu-sz：平均I/O隊列長度.即 delta(aveq)/s/1000 (因爲aveq的單位爲毫秒).
- await： 平均每次設備I/O操作的等待時間 (毫秒).即 delta(ruse+wuse)/delta(rio+wio)
- svctm： 平均每次設備I/O操作的服務時間 (毫秒).即 delta(use)/delta(rio+wio)
- %util： 一秒中有百分之多少的時間用於 I/O 操作,或者說一秒中有多少時間 I/O 隊列是非空的，即 delta(use)/s/1000 (因爲use的單位爲毫秒)


如果 %util 接近 100%，說明產生的I/O請求太多，I/O系統已經滿負荷，該磁盤可能存在瓶頸。 idle小於70% IO壓力就較大了，一般讀取速度有較多的wait。 同時可以結合vmstat 查看查看b參數(等待資源的進程數)和wa參數(IO等待所佔用的CPU時間的百分比，高過30%時IO壓力高)。

另外 await 的參數也要多和 svctm 來參考。差的過高就一定有 IO 的問題。

avgqu-sz 也是個做 IO 調優時需要注意的地方，這個就是直接每次操作的數據的大小，如果次數多，但數據拿的小的話，其實 IO 也會很小。如果數據拿的大，才IO 的數據會高。也可以通過 avgqu-sz × ( r/s or w/s ) = rsec/s or wsec/s。也就是講，讀定速度是這個來決定的。

svctm 一般要小於 await (因爲同時等待的請求的等待時間被重複計算了)，svctm 的大小一般和磁盤性能有關，CPU/內存的負荷也會對其有影響，請求過多也會間接導致 svctm 的增加。await 的大小一般取決於服務時間(svctm) 以及 I/O 隊列的長度和 I/O 請求的發出模式。如果 svctm 比較接近 await，說明 I/O 幾乎沒有等待時間；如果 await 遠大於 svctm，說明 I/O 隊列太長，應用得到的響應時間變慢，如果響應時間超過了用戶可以容許的範圍，這時可以考慮更換更快的磁盤，調整內核 elevator 算法，優化應用，或者升級 CPU。

隊列長度(avgqu-sz)也可作爲衡量系統 I/O 負荷的指標，但由於 avgqu-sz 是按照單位時間的平均值，所以不能反映瞬間的 I/O 洪水。

### 形象的比喻：
- r/s+w/s 類似於交款人的總數
- 平均隊列長度(avgqu-sz)類似於單位時間裏平均排隊人的個數
- 平均服務時間(svctm)類似於收銀員的收款速度
- 平均等待時間(await)類似於平均每人的等待時間
- 平均I/O數據(avgrq-sz)類似於平均每人所買的東西多少
- I/O 操作率 (%util)類似於收款臺前有人排隊的時間比例

設備IO操作:總IO(io)/s = r/s(讀) +w/s(寫)

平均等待時間=單個I/O服務器時間*(1+2+...+請求總數-1)/請求總數

每秒發出的I/0請求很多,但是平均隊列就4,表示這些請求比較均勻,大部分處理還是比較及時。
