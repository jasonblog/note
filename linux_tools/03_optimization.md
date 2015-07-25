# 性能優化
目錄

性能優化
分析系統瓶頸
分析內存瓶頸
分析IO瓶頸
分析進程調用
優化程序代碼
gprof使用步驟
其它工具
性能優化的核心是找出系統的瓶頸點，問題找到了，優化的工作也就完成了大半； 這裡介紹的性能優化主要從兩個層面來介紹：系統層面和程序層面；

3.1. 分析系統瓶頸
系統響應變慢，首先得定位大致的問題出在哪裡，是IO瓶頸、CPU瓶頸、內存瓶頸還是程序導致的系統問題；

使用top工具能夠比較全面的查看我們關注的點:
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
    4 root      RT   0     0    0    0 S  0.0  0.0   0:00.00 migration/
```

進入交互模式後:
輸入M，進程列表按內存使用大小降序排序，便於我們觀察最大內存使用者使用有問題（檢測內存洩漏問題）;
輸入P，進程列表按CPU使用大小降序排序，便於我們觀察最耗CPU資源的使用者是否有問題；
top第三行顯示當前系統的，其中有兩個值很關鍵:
%id：空閒CPU時間百分比，如果這個值過低，表明系統CPU存在瓶頸；
%wa：等待I/O的CPU時間百分比，如果這個值過高，表明IO存在瓶頸；
3.2. 分析內存瓶頸
查看內存是否存在瓶頸，使用top指令看比較麻煩，而free命令更為直觀:

```
[/home/weber#]free
             total       used       free     shared    buffers     cached
Mem:        501820     452028      49792      37064       5056     136732
-/+ buffers/cache:     310240     191580
Swap:            0          0          0
[/home/weber#]top
top - 17:52:17 up 42 days,  7:10,  1 user,  load average: 0.02, 0.02, 0.05
Tasks:  80 total,   1 running,  79 sleeping,   0 stopped,   0 zombie
%Cpu(s):  0.0 us,  0.0 sy,  0.0 ni,100.0 id,  0.0 wa,  0.0 hi,  0.0 si,  0.0 st
KiB Mem:    501820 total,   452548 used,    49272 free,     5144 buffers
KiB Swap:        0 total,        0 used,        0 free.   136988 cached Mem
```

top工具顯示了free工具的第一行所有信息，但真實可用的內存，還需要自己計算才知道; 系統實際可用的內存為free工具輸出第二行的free+buffer+cached；也就是第三行的free值191580；關於free命令各個值的詳情解讀，請參考這篇文章 free 查詢可用內存 ;

如果是因為缺少內存，系統響應變慢很明顯，因為這使得系統不停的做換入換出的工作;

進一步的監視內存使用情況，可使用vmstat工具，實時動態監視操作系統的內存和虛擬內存的動態變化。 參考： vmstat 監視內存使用情況 ;

3.3. 分析IO瓶頸
如果IO存在性能瓶頸，top工具中的%wa會偏高；

進一步分析使用iostat工具:
```
/root$iostat -d -x -k 1 1
Linux 2.6.32-279.el6.x86_64 (colin)   07/16/2014      _x86_64_        (4 CPU)

Device:         rrqm/s   wrqm/s     r/s     w/s    rkB/s    wkB/s avgrq-sz avgqu-sz   await  svctm  %util
sda               0.02     7.25    0.04    1.90     0.74    35.47    37.15     0.04   19.13   5.58   1.09
dm-0              0.00     0.00    0.04    3.05     0.28    12.18     8.07     0.65  209.01   1.11   0.34
dm-1              0.00     0.00    0.02    5.82     0.46    23.26     8.13     0.43   74.33   1.30   0.76
dm-2              0.00     0.00    0.00    0.01     0.00     0.02     8.00     0.00    5.41   3.28   0.00
```

如果%iowait的值過高，表示硬盤存在I/O瓶頸。
如果 %util 接近 100%，說明產生的I/O請求太多，I/O系統已經滿負荷，該磁盤可能存在瓶頸。
如果 svctm 比較接近 await，說明 I/O 幾乎沒有等待時間；
如果 await 遠大於 svctm，說明I/O 隊列太長，io響應太慢，則需要進行必要優化。
如果avgqu-sz比較大，也表示有大量io在等待。
更多參數說明請參考 iostat 監視I/O子系統 ;

3.4. 分析進程調用
通過top等工具發現系統性能問題是由某個進程導致的之後，接下來我們就需要分析這個進程；繼續 查詢問題在哪；

這裡我們有兩個好用的工具： pstack和pstrace

pstack用來跟蹤進程棧，這個命令在排查進程問題時非常有用，比如我們發現一個服務一直處於work狀態（如假死狀態，好似死循環），使用這個命令就能輕鬆定位問題所在；可以在一段時間內，多執行幾次pstack，若發現代碼棧總是停在同一個位置，那個位置就需要重點關注，很可能就是出問題的地方；

示例：查看bash程序進程棧:
```
/opt/app/tdev1$ps -fe| grep bash
tdev1   7013  7012  0 19:42 pts/1    00:00:00 -bash
tdev1  11402 11401  0 20:31 pts/2    00:00:00 -bash
tdev1  11474 11402  0 20:32 pts/2    00:00:00 grep bash
/opt/app/tdev1$pstack 7013
#0  0x00000039958c5620 in __read_nocancel () from /lib64/libc.so.6
#1  0x000000000047dafe in rl_getc ()
#2  0x000000000047def6 in rl_read_key ()
#3  0x000000000046d0f5 in readline_internal_char ()
#4  0x000000000046d4e5 in readline ()
#5  0x00000000004213cf in ?? ()
#6  0x000000000041d685 in ?? ()
#7  0x000000000041e89e in ?? ()
#8  0x00000000004218dc in yyparse ()
#9  0x000000000041b507 in parse_command ()
#10 0x000000000041b5c6 in read_command ()
#11 0x000000000041b74e in reader_loop ()
#12 0x000000000041b2aa in main ()
```

而strace用來跟蹤進程中的系統調用；這個工具能夠動態的跟蹤進程執行時的系統調用和所接收的信號。是一個非常有效的檢測、指導和調試工具。系統管理員可以通過該命令容易地解決程序問題。

參考： strace 跟蹤進程中的系統調用 ;

3.5. 優化程序代碼
優化自己開發的程序，建議採用以下準則:

二八法則：在任何一組東西中，最重要的只佔其中一小部分，約20%，其餘80%的儘管是多數，卻是次要的；在優化實踐中，我們將精力集中在優化那20%最耗時的代碼上，整體性能將有顯著的提升；這個很好理解。函數A雖然代碼量大，但在一次正常執行流程中，只調用了一次。而另一個函數B代碼量比A小很多，但被調用了1000次。顯然，我們更應關注B的優化。
編完代碼，再優化；編碼的時候總是考慮最佳性能未必總是好的；在強調最佳性能的編碼方式的同時，可能就損失了代碼的可讀性和開發效率；
gprof使用步驟
用gcc、g++、xlC編譯程序時，使用-pg參數，如：g++ -pg -o test.exe test.cpp編譯器會自動在目標代碼中插入用於性能測試的代碼片斷，這些代碼在程序運行時採集並記錄函數的調用關係和調用次數，並記錄函數自身執行時間和被調用函數的執行時間。
執行編譯後的可執行程序，如：./test.exe。該步驟運行程序的時間會稍慢於正常編譯的可執行程序的運行時間。程序運行結束後，會在程序所在路徑下生成一個缺省文件名為gmon.out的文件，這個文件就是記錄程序運行的性能、調用關係、調用次數等信息的數據文件。
使用gprof命令來分析記錄程序運行信息的gmon.out文件，如：gprof test.exe gmon.out則可以在顯示器上看到函數調用相關的統計、分析信息。上述信息也可以採用gprof test.exe gmon.out> gprofresult.txt重定向到文本文件以便於後續分析。
關於gprof的使用案例，請參考 [f1] ;

3.6. 其它工具
調試內存洩漏的工具valgrind，感興趣的朋友可以google瞭解；

OProfile: Linux 平臺上的一個功能強大的性能分析工具,使用參考 [f2] ;

除了上面介紹的工具，還有一些比較全面的性能分析工具，比如sar（Linux系統上默認不安裝，需要手動安裝下）； 將sar的常駐監控工具打開後，能夠收集比較全面的性能分析數據；

關於sar的使用，參考 sar 找出系統瓶頸的利器 ;

[f1]	C++的性能優化實踐 http://www.cnblogs.com/me115/archive/2013/06/05/3117967.html
[f2]	用 OProfile 徹底瞭解性能 http://www.ibm.com/developerworks/cn/linux/l-oprof/
[f3]	Linux上的free命令詳解 http://www.cnblogs.com/coldplayerest/archive/2010/02/20/1669949.html
Next  Previous
