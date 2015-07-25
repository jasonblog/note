# strace 跟蹤進程中的系統調用
strace常用來跟蹤進程執行時的系統調用和所接收的信號。 在Linux世界，進程不能直接訪問硬件設備，當進程需要訪問硬件設備(比如讀取磁盤文件，接收網絡數據等等)時，必須由用戶態模式切換至內核態模式，通過系統調用訪問硬件設備。strace可以跟蹤到一個進程產生的系統調用,包括參數，返回值，執行消耗的時間。

6.1. 輸出參數含義
每一行都是一條系統調用，等號左邊是系統調用的函數名及其參數，右邊是該調用的返回值。 strace 顯示這些調用的參數並返回符號形式的值。strace 從內核接收信息，而且不需要以任何特殊的方式來構建內核。

```
$strace cat /dev/null
execve("/bin/cat", ["cat", "/dev/null"], [/* 22 vars */]) = 0
brk(0)                                  = 0xab1000
access("/etc/ld.so.nohwcap", F_OK)      = -1 ENOENT (No such file or directory)
mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7f29379a7000
access("/etc/ld.so.preload", R_OK)      = -1 ENOENT (No such file or directory)
...
```

6.2. 參數
```
-c 統計每一系統調用的所執行的時間,次數和出錯的次數等.
-d 輸出strace關於標準錯誤的調試信息.
-f 跟蹤由fork調用所產生的子進程.
-ff 如果提供-o filename,則所有進程的跟蹤結果輸出到相應的filename.pid中,pid是各進程的進程號.
-F 嘗試跟蹤vfork調用.在-f時,vfork不被跟蹤.
-h 輸出簡要的幫助信息.
-i 輸出系統調用的入口指針.
-q 禁止輸出關於脫離的消息.
-r 打印出相對時間關於,,每一個系統調用.
-t 在輸出中的每一行前加上時間信息.
-tt 在輸出中的每一行前加上時間信息,微秒級.
-ttt 微秒級輸出,以秒了表示時間.
-T 顯示每一調用所耗的時間.
-v 輸出所有的系統調用.一些調用關於環境變量,狀態,輸入輸出等調用由於使用頻繁,默認不輸出.
-V 輸出strace的版本信息.
-x 以十六進制形式輸出非標準字符串
-xx 所有字符串以十六進制形式輸出.
-a column
設置返回值的輸出位置.默認 為40.
-e expr
指定一個表達式,用來控制如何跟蹤.格式如下:
[qualifier=][!]value1[,value2]...
qualifier只能是 trace,abbrev,verbose,raw,signal,read,write其中之一.value是用來限定的符號或數字.默認的 qualifier是 trace.感歎號是否定符號.例如:
-eopen等價於 -e trace=open,表示只跟蹤open調用.而-etrace!=open表示跟蹤除了open以外的其他調用.有兩個特殊的符號 all 和 none.
注意有些shell使用!來執行歷史記錄裡的命令,所以要使用\\.
-e trace=set
只跟蹤指定的系統 調用.例如:-e trace=open,close,rean,write表示只跟蹤這四個系統調用.默認的為set=all.
-e trace=file
只跟蹤有關文件操作的系統調用.
-e trace=process
只跟蹤有關進程控制的系統調用.
-e trace=network
跟蹤與網絡有關的所有系統調用.
-e strace=signal
跟蹤所有與系統信號有關的 系統調用
-e trace=ipc
跟蹤所有與進程通訊有關的系統調用
-e abbrev=set
設定 strace輸出的系統調用的結果集.-v 等與 abbrev=none.默認為abbrev=all.
-e raw=set
將指 定的系統調用的參數以十六進制顯示.
-e signal=set
指定跟蹤的系統信號.默認為all.如 signal=!SIGIO(或者signal=!io),表示不跟蹤SIGIO信號.
-e read=set
輸出從指定文件中讀出 的數據.例如:
-e read=3,5
-e write=set
輸出寫入到指定文件中的數據.
-o filename
將strace的輸出寫入文件filename
-p pid
跟蹤指定的進程pid.
-s strsize
指定輸出的字符串的最大長度.默認為32.文件名一直全部輸出.
-u username
以username 的UID和GID執行被跟蹤的命令
```

6.3. 命令實例
跟蹤可執行程序
```
strace -f -F -o ~/straceout.txt myserver
```

-f -F選項告訴strace同時跟蹤fork和vfork出來的進程，-o選項把所有strace輸出寫到~/straceout.txt裡 面，myserver是要啟動和調試的程序。

跟蹤服務程序
```
strace -o output.txt -T -tt -e trace=all -p 28979
```

跟蹤28979進程的所有系統調用（-e trace=all），並統計系統調用的花費時間，以及開始時間（並以可視化的時分秒格式顯示），最後將記錄結果存在output.txt文件裡面。
