# free 查詢可用內存
free工具用來查看系統可用內存:
```
/opt/app/tdev1$free
             total       used       free     shared    buffers     cached
Mem:       8175320    6159248    2016072          0     310208    5243680
-/+ buffers/cache:     605360    7569960
Swap:      6881272      16196    6865076
```

解釋一下Linux上free命令的輸出。

下面是free的運行結果，一共有4行。為了方便說明，我加上了列號。這樣可以把free的輸出看成一個二維數組FO(Free Output)。例如:

```
FO[2][1] = 24677460
FO[3][2] = 10321516

                   1          2          3          4          5          6
1              total       used       free     shared    buffers     cached
2 Mem:      24677460   23276064    1401396          0     870540   12084008
3 -/+ buffers/cache:   10321516   14355944
4 Swap:     25151484     224188   24927296
```

free的輸出一共有四行，第四行為交換區的信息，分別是交換的總量（total），使用量（used）和有多少空閒的交換區（free），這個比較清楚，不說太多。

free輸出地第二行和第三行是比較讓人迷惑的。這兩行都是說明內存使用情況的。第一列是總量（total），第二列是使用量（used），第三列是可用量（free）。

　　第一行的輸出時從操作系統（OS）來看的。也就是說，從OS的角度來看，計算機上一共有:

24677460KB（缺省時free的單位為KB）物理內存，即FO[2][1]； 在這些物理內存中有23276064KB（即FO[2][2]）被使用了； 還用1401396KB（即FO[2][3]）是可用的；

這裡得到第一個等式：

FO[2][1] = FO[2][2] + FO[2][3]

FO[2][4]表示被幾個進程共享的內存的，現在已經deprecated，其值總是0（當然在一些系統上也可能不是0，主要取決於free命令是怎麼實現的）。

FO[2][5]表示被OS buffer住的內存。FO[2][6]表示被OS cache的內存。在有些時候buffer和cache這兩個詞經常混用。不過在一些比較低層的軟件裡是要區分這兩個詞的，看老外的洋文:
```
A buffer is something that has yet to be "written" to disk.
A cache is something that has been "read" from the disk and stored for later use.
```

也就是說buffer是用於存放要輸出到disk（塊設備）的數據的，而cache是存放從disk上讀出的數據。這二者是為了提高IO性能的，並由OS管理。

Linux和其他成熟的操作系統（例如windows），為了提高IO read的性能，總是要多cache一些數據，這也就是為什麼FO[2][6]（cached memory）比較大，而FO[2][3]比較小的原因。我們可以做一個簡單的測試:

釋放掉被系統cache佔用的數據:
```
echo 3>/proc/sys/vm/drop_caches
```

讀一個大文件，並記錄時間；
關閉該文件；
重讀這個大文件，並記錄時間；
第二次讀應該比第一次快很多。原來我做過一個BerkeleyDB的讀操作，大概要讀5G的文件，幾千萬條記錄。在我的環境上，第二次讀比第一次大概可以快9倍左右。

free輸出的第二行是從一個應用程序的角度看系統內存的使用情況。

- 對於FO[3][2]，即-buffers/cache，表示一個應用程序認為系統被用掉多少內存；
- 於FO[3][3]，即+buffers/cache，表示一個應用程序認為系統還有多少內存；
因為被系統cache和buffer佔用的內存可以被快速回收，所以通常FO[3][3]比FO[2][3]會大很多。

這裡還用兩個等式:
```
FO[3][2] = FO[2][2] - FO[2][5] - FO[2][6]
FO[3][3] = FO[2][3] + FO[2][5] + FO[2][6]
```

這二者都不難理解。

free命令由procps.*.rpm提供（在Redhat系列的OS上）。free命令的所有輸出值都是從/proc/meminfo中讀出的。

在系統上可能有meminfo(2)這個函數，它就是為瞭解析/proc/meminfo的。procps這個包自己實現了meminfo()這個函數。可以下載一個procps的tar包看看具體實現，現在最新版式3.2.8。

文章出處:

http://www.cnblogs.com/coldplayerest/archive/2010/02/20/1669949.html
