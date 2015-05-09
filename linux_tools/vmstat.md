# vmstat 監視內存使用情況
vmstat是Virtual Meomory Statistics（虛擬內存統計）的縮寫，可實時動態監視操作系統的虛擬內存、進程、CPU活動。

### 10.1. vmstat的語法
　　vmstat [-V] [-n] [delay [count]]

- -V表示打印出版本信息；
- -n表示在週期性循環輸出時，輸出的頭部信息僅顯示一次；
- delay是兩次輸出之間的延遲時間；
- count是指按照這個時間間隔統計的次數。

```
/root$vmstat 5 5
procs -----------memory---------- ---swap-- -----io---- --system-- -----cpu-----
r  b   swpd   free   buff  cache   si   so    bi    bo   in   cs us sy id wa st
6  0      0 27900472 204216 28188356    0    0     0     9    1    2 11 14 75  0  0
9  0      0 27900380 204228 28188360    0    0     0    13 33312 126221 22 20 58  0  0
2  0      0 27900340 204240 28188364    0    0     0    10 32755 125566 22 20 58  0  0
```

### 10.2. 字段說明
### Procs（進程）:
- r: 運行隊列中進程數量
- b: 等待IO的進程數量

### Memory（內存）:
- swpd: 使用虛擬內存大小
- free: 可用內存大小
- buff: 用作緩衝的內存大小
- cache: 用作緩存的內存大小

### Swap:
- si: 每秒從交換區寫到內存的大小
- so: 每秒寫入交換區的內存大小

###IO：（現在的Linux版本塊的大小爲1024bytes）
- bi: 每秒讀取的塊數
- bo: 每秒寫入的塊數

###system：
- in: 每秒中斷數，包括時鐘中斷
- cs: 每秒上下文切換數

###CPU（以百分比表示）
- us: 用戶進程執行時間(user time)
- sy: 系統進程執行時間(system time)
- id: 空閒時間(包括IO等待時間)
- wa: 等待IO時間
