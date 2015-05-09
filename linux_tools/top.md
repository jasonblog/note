# top linux下的任務管理器
top命令是Linux下常用的性能分析工具，能夠實時顯示系統中各個進程的資源佔用狀況，類似於Windows的任務管理器。top是一個動態顯示過程,即可以通過用戶按鍵來不斷刷新當前狀態.如果在前臺執行該命令,它將獨佔前臺,直到用戶終止該程序爲止.比較準確的說,top命令提供了實時的對系統處理器的狀態監視.它將顯示系統中CPU最“敏感”的任務列表.該命令可以按CPU使用.內存使用和執行時間對任務進行排序；而且該命令的很多特性都可以通過交互式命令或者在個人定製文件中進行設定。
```
$top
    top - 09:14:56 up 264 days, 20:56,  1 user,  load average: 0.02, 0.04, 0.00
    Tasks:  87 total,   1 running,  86 sleeping,   0 stopped,   0 zombie
    Cpu(s):  0.0%us,  0.2%sy,  0.0%ni, 99.7%id,  0.0%wa,  0.0%hi,  0.0%si,  0.2%st
    Mem:    377672k total,   322332k used,    55340k free,    32592k buffers
    Swap:   397308k total,    67192k used,   330116k free,    71900k cached
    PID USER      PR  NI  VIRT  RES  SHR S %CPU %MEM    TIME+  COMMAND
    1 root      20   0  2856  656  388 S  0.0  0.2   0:49.40 init
    2 root      20   0     0    0    0 S  0.0  0.0   0:00.00 kthreadd
    3 root      20   0     0    0    0 S  0.0  0.0   7:15.20 ksoftirqd/0
    4 root      RT   0     0    0    0 S  0.0  0.0   0:00.00 migration/0
```

- 第一行
09:14:56 ： 系統當前時間
264 days, 20:56 ： 系統開機到現在經過了多少時間
1 users ： 當前2用戶在線
load average: 0.02, 0.04, 0.00： 系統1分鐘、5分鐘、15分鐘的CPU負載信息
- 第二行
Tasks：任務;
87 total：很好理解，就是當前有87個任務，也就是87個進程。
1 running：1個進程正在運行
86 sleeping：86個進程睡眠
0 stopped：停止的進程數
0 zombie：僵死的進程數
- 第三行
Cpu(s)：表示這一行顯示CPU總體信息
0.0%us：用戶態進程佔用CPU時間百分比，不包含renice值爲負的任務佔用的CPU的時間。
0.7%sy：內核佔用CPU時間百分比
0.0%ni：改變過優先級的進程佔用CPU的百分比
99.3%id：空閒CPU時間百分比
0.0%wa：等待I/O的CPU時間百分比
0.0%hi：CPU硬中斷時間百分比
0.0%si：CPU軟中斷時間百分比
注：這裏顯示數據是所有cpu的平均值，如果想看每一個cpu的處理情況，按1即可；摺疊，再次按1；
- 第四行
Men：內存的意思
8175320kk total：物理內存總量
8058868k used：使用的物理內存量
116452k free：空閒的物理內存量
283084k buffers：用作內核緩存的物理內存量
- 第五行
Swap：交換空間
6881272k total：交換區總量
4010444k used：使用的交換區量
2870828k free：空閒的交換區量
4336992k cached：緩衝交換區總量
- 進程信息
再下面就是進程信息：
PID：進程的ID
USER：進程所有者
PR：進程的優先級別，越小越優先被執行
NInice：值
VIRT：進程佔用的虛擬內存
RES：進程佔用的物理內存
SHR：進程使用的共享內存
S：進程的狀態。S表示休眠，R表示正在運行，Z表示僵死狀態，N表示該進程優先值爲負數
%CPU：進程佔用CPU的使用率
%MEM：進程使用的物理內存和總內存的百分比
TIME+：該進程啓動後佔用的總的CPU時間，即佔用CPU使用時間的累加值。
COMMAND：進程啓動命令名稱
### 8.1. top命令交互操作指令
下面列出一些常用的 top命令操作指令

q：退出top命令
<Space>：立即刷新
s：設置刷新時間間隔
c：顯示命令完全模式
t:：顯示或隱藏進程和CPU狀態信息
m：顯示或隱藏內存狀態信息
l：顯示或隱藏uptime信息
f：增加或減少進程顯示標誌
S：累計模式，會把已完成或退出的子進程佔用的CPU時間累計到父進程的MITE+
P：按%CPU使用率排行
T：按MITE+排行
M：按%MEM排行
u：指定顯示用戶進程
r：修改進程renice值
kkill：進程
i：只顯示正在運行的進程
W：保存對top的設置到文件^/.toprc，下次啓動將自動調用toprc文件的設置。
h：幫助命令。
q：退出
注：強調一下，使用頻率最高的是P、T、M，因爲通常使用top，我們就想看看是哪些進程最耗cpu資源、佔用的內存最多； 注：通過”shift + >”或”shift + <”可以向右或左改變排序列 如果只需要查看內存：可用free命令。只查看uptime信息（第一行），可用uptime命令；

### 8.2. 實例
### 實例1：多核CPU監控
在top基本視圖中，按鍵盤數字“1”，可監控每個邏輯CPU的狀況；
```
[rdtfr@bl685cb4-t ^]$ top
top - 09:10:44 up 20 days, 16:51,  4 users,  load average: 3.82, 4.40, 4.40
Tasks: 1201 total,  10 running, 1189 sleeping,   0 stopped,   2 zombie
Cpu0  :  1.3%us,  2.3%sy,  0.0%ni, 96.4%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st
Cpu1  :  1.3%us,  2.6%sy,  0.0%ni, 96.1%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st
Cpu2  :  1.0%us,  2.0%sy,  0.0%ni, 92.5%id,  0.0%wa,  0.0%hi,  4.6%si,  0.0%st
Cpu3  :  3.9%us,  7.8%sy,  0.0%ni, 83.2%id,  0.0%wa,  0.0%hi,  5.2%si,  0.0%st
Cpu4  :  4.2%us, 10.4%sy,  0.0%ni, 63.8%id,  0.0%wa,  0.0%hi, 21.5%si,  0.0%st
Cpu5  :  6.8%us, 12.7%sy,  0.0%ni, 80.5%id,  0.0%wa,  0.0%hi,  0.0%si,  0.0%st
Cpu6  :  2.9%us,  7.2%sy,  0.0%ni, 85.3%id,  0.0%wa,  0.0%hi,  4.6%si,  0.0%st
Cpu7  :  6.2%us, 13.0%sy,  0.0%ni, 75.3%id,  0.0%wa,  0.0%hi,  5.5%si,  0.0%st
Mem:  32943888k total, 32834216k used,   109672k free,   642704k buffers
Swap: 35651576k total,  5761928k used, 29889648k free, 16611500k cached
```

### 實例2：高亮顯示當前運行進程
在top基本視圖中,按鍵盤“b”（打開/關閉加亮效果）；
實例3：顯示完整的程序命令
```
命令：top -c
```
```
[rdtfr@bl685cb4-t ^]$ top -c
top - 09:14:35 up 20 days, 16:55,  4 users,  load average: 5.77, 5.01, 4.64
Tasks: 1200 total,   5 running, 1192 sleeping,   0 stopped,   3 zombie
Cpu(s):  4.4%us,  6.0%sy,  0.0%ni, 83.8%id,  0.2%wa,  0.0%hi,  5.5%si,  0.0%st
Mem:  32943888k total, 32842896k used,   100992k free,   591484k buffers
Swap: 35651576k total,  5761808k used, 29889768k free, 16918824k cached
PID USER      PR  NI  VIRT  RES  SHR S %CPU %MEM    TIME+  COMMAND
2013 apache    18   0  403m  88m 5304 S 25.0  0.3   6:37.44 /usr/sbin/httpd
18335 pubtest   22   0 65576  996  728 R  7.8  0.0   0:00.24 netstat -naltp
16499 rdtfare   15   0 13672 2080  824 R  2.6  0.0   0:00.38 top -c
29684 rdtfare   15   0 1164m 837m  14m S  2.3  2.6 148:47.54 ./autodata data1.txt
12976 pubtest   18   0  238m 9000 1932 S  1.6  0.0 439:28.44 tscagent -s TOEV_P
```
### 實例4：顯示指定的進程信息
命令：top -p pidid
```
/opt/app/tdv1/config#top -p 17265
top - 09:17:34 up 455 days, 17:55,  2 users,  load average: 3.76, 4.56, 4.46
Tasks:   1 total,   0 running,   1 sleeping,   0 stopped,   0 zombie
Cpu(s):  7.8%us,  1.9%sy,  0.0%ni, 89.2%id,  0.0%wa,  0.1%hi,  1.0%si,  0.0%st
Mem:   8175452k total,  8103988k used,    71464k free,   268716k buffers
Swap:  6881272k total,  4275424k used,  2605848k free,  6338184k cached
PID USER      PR  NI  VIRT  RES  SHR S %CPU %MEM    TIME+  COMMAND
17265 tdv1      15   0 56504  828  632 S  0.0  0.0 195:53.25 redis-server
```

### 8.3. 更強大的工具
### htop
htop 是一個 Linux 下的交互式的進程瀏覽器，可以用來替換Linux下的top命令。

與Linux傳統的top相比，htop更加人性化。它可讓用戶交互式操作，支持顏色主題，可橫向或縱向滾動瀏覽進程列表，並支持鼠標操作。

與top相比，htop有以下優點：

可以橫向或縱向滾動瀏覽進程列表，以便看到所有的進程和完整的命令行。
在啓動上，比top 更快。
殺進程時不需要輸入進程號。
htop 支持鼠標操作。
