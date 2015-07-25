# lsof 一切皆文件
3. lsof 一切皆文件
lsof（list open files）是一個查看當前系統文件的工具。在linux環境下，任何事物都以文件的形式存在，通過文件不僅僅可以訪問常規數據，還可以訪問網絡連接和硬件。如傳輸控制協議 (TCP) 和用戶數據報協議 (UDP) 套接字等，系統在後臺都為該應用程序分配了一個文件描述符，該文件描述符提供了大量關於這個應用程序本身的信息。

lsof打開的文件可以是：

普通文件
目錄
網絡文件系統的文件
字符或設備文件
(函數)共享庫
管道，命名管道
符號鏈接
網絡文件（例如：NFS file、網絡socket，unix域名socket）
還有其它類型的文件，等等
3.1. 命令參數
-a 列出打開文件存在的進程
-c<進程名> 列出指定進程所打開的文件
-g 列出GID號進程詳情
-d<文件號> 列出佔用該文件號的進程
+d<目錄> 列出目錄下被打開的文件
+D<目錄> 遞歸列出目錄下被打開的文件
-n<目錄> 列出使用NFS的文件
-i<條件> 列出符合條件的進程。（4、6、協議、:端口、 @ip ）
-p<進程號> 列出指定進程號所打開的文件
-u 列出UID號進程詳情
-h 顯示幫助信息
-v 顯示版本信息
3.2. 使用實例
### 實例1：無任何參數
```
$lsof| more
COMMAND     PID      USER   FD      TYPE             DEVICE SIZE/OFF       NODE NAME
init          1      root  cwd       DIR              253,0     4096          2 /
init          1      root  rtd       DIR              253,0     4096          2 /
init          1      root  txt       REG              253,0   150352    1310795 /sbin/init
init          1      root  mem       REG              253,0    65928    5505054 /lib64/libnss_files-2.12.so
init          1      root  mem       REG              253,0  1918016    5521405 /lib64/libc-2.12.so
init          1      root  mem       REG              253,0    93224    5521440 /lib64/libgcc_s-4.4.6-20120305.so.1
init          1      root  mem       REG              253,0    47064    5521407 /lib64/librt-2.12.so
init          1      root  mem       REG              253,0   145720    5521406 /lib64/libpthread-2.12.so
...
```
說明：

lsof輸出各列信息的意義如下：

COMMAND：進程的名稱
PID：進程標識符
PPID：父進程標識符（需要指定-R參數）
USER：進程所有者
PGID：進程所屬組
FD：文件描述符，應用程序通過文件描述符識別該文件。如cwd、txt等:
```
（1）cwd：表示current work dirctory，即：應用程序的當前工作目錄，這是該應用程序啟動的目錄，除非它本身對這個目錄進行更改
（2）txt ：該類型的文件是程序代碼，如應用程序二進制文件本身或共享庫，如上列表中顯示的 /sbin/init 程序
（3）lnn：library references (AIX);
（4）er：FD information error (see NAME column);
（5）jld：jail directory (FreeBSD);
（6）ltx：shared library text (code and data);
（7）mxx ：hex memory-mapped type number xx.
（8）m86：DOS Merge mapped file;
（9）mem：memory-mapped file;
（10）mmap：memory-mapped device;
（11）pd：parent directory;
（12）rtd：root directory;
（13）tr：kernel trace file (OpenBSD);
（14）v86  VP/ix mapped file;
（15）0：表示標準輸出
（16）1：表示標準輸入
（17）2：表示標準錯誤
一般在標準輸出、標準錯誤、標準輸入後還跟著文件狀態模式：r、w、u等
（1）u：表示該文件被打開並處於讀取/寫入模式
（2）r：表示該文件被打開並處於只讀模式
（3）w：表示該文件被打開並處於
（4）空格：表示該文件的狀態模式為unknow，且沒有鎖定
（5）-：表示該文件的狀態模式為unknow，且被鎖定
同時在文件狀態模式後面，還跟著相關的鎖
（1）N：for a Solaris NFS lock of unknown type;
（2）r：for read lock on part of the file;
（3）R：for a read lock on the entire file;
（4）w：for a write lock on part of the file;（文件的部分寫鎖）
（5）W：for a write lock on the entire file;（整個文件的寫鎖）
（6）u：for a read and write lock of any length;
（7）U：for a lock of unknown type;
（8）x：for an SCO OpenServer Xenix lock on part      of the file;
（9）X：for an SCO OpenServer Xenix lock on the      entire file;
（10）space：if there is no lock.
```

TYPE：文件類型，如DIR、REG等，常見的文件類型:
```
（1）DIR：表示目錄
（2）CHR：表示字符類型
（3）BLK：塊設備類型
（4）UNIX： UNIX 域套接字
（5）FIFO：先進先出 (FIFO) 隊列
（6）IPv4：網際協議 (IP) 套接字
```

DEVICE：指定磁盤的名稱
SIZE：文件的大小
NODE：索引節點（文件在磁盤上的標識）
NAME：打開文件的確切名稱
### 實例2：查找某個文件相關的進程
```
$lsof /bin/bash
COMMAND     PID USER  FD   TYPE DEVICE SIZE/OFF    NODE NAME
mysqld_sa  2169 root txt    REG  253,0   938736 4587562 /bin/bash
ksmtuned   2334 root txt    REG  253,0   938736 4587562 /bin/bash
bash      20121 root txt    REG  253,0   938736 4587562 /bin/bash
```

###實例3：列出某個用戶打開的文件信息
```
$lsof -u username
-u 選項，u是user的縮寫
```

### 實例4：列出某個程序進程所打開的文件信息
```
$lsof -c mysql
```

-c 選項將會列出所有以mysql這個進程開頭的程序的文件，其實你也可以寫成 lsof | grep mysql, 但是第一種方法明顯比第二種方法要少打幾個字符；

### 實例5：列出某個用戶以及某個進程所打開的文件信息
```
$lsof  -u test -c mysql
```
### 實例6：通過某個進程號顯示該進程打開的文件
```
$lsof -p 11968
```
### 實例7：列出所有的網絡連接
```
$lsof -i
```
### 實例8：列出所有tcp 網絡連接信息
```
$lsof -i tcp

$lsof -n -i tcp
COMMAND     PID  USER   FD   TYPE  DEVICE SIZE/OFF NODE NAME
svnserve  11552 weber    3u  IPv4 3799399      0t0  TCP *:svn (LISTEN)
redis-ser 25501 weber    4u  IPv4  113150      0t0  TCP 127.0.0.1:6379 (LISTEN)
```

### 實例9：列出誰在使用某個端口
```
$lsof -i :3306
```

### 實例10：列出某個用戶的所有活躍的網絡端口
```
$lsof -a -u test -i
```
### 實例11：根據文件描述列出對應的文件信息
```
$lsof -d description(like 2)
```
示例:
```
$lsof -d 3 | grep PARSER1
tail      6499 tde    3r   REG    253,3   4514722     417798 /opt/applog/open/log/HOSTPARSER1_ERROR_141217.log.001
```
說明： 0表示標準輸入，1表示標準輸出，2表示標準錯誤，從而可知：所以大多數應用程序所打開的文件的 FD 都是從 3 開始

### 實例12：列出被進程號為1234的進程所打開的所有IPV4 network files
```
$lsof -i 4 -a -p 1234
```

### 實例13：列出目前連接主機nf5260i5-td上端口為：20，21，80相關的所有文件信息，且每隔3秒重複執行
```
lsof -i @nf5260i5-td:20,21,80 -r 3
```

Next  Previous
