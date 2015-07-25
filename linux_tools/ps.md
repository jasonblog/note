# ps 進程查看器
Linux中的ps命令是Process Status的縮寫。ps命令用來列出系統中當前運行的那些進程。ps命令列出的是當前那些進程的快照，就是執行ps命令的那個時刻的那些進程，如果想要動態的顯示進程信息，就可以使用top命令。

要對進程進行監測和控制，首先必須要瞭解當前進程的情況，也就是需要查看當前進程，而 ps 命令就是最基本同時也是非常強大的進程查看命令。使用該命令可以確定有哪些進程正在運行和運行的狀態、進程是否結束、進程有沒有僵死、哪些進程佔用了過多的資源等等。總之大部分信息都是可以通過執行該命令得到的。

ps 為我們提供了進程的一次性的查看，它所提供的查看結果並不動態連續的；如果想對進程時間監控，應該用 top linux下的任務管理器 工具。

注：kill 命令用於殺死進程。

linux上進程有5種狀態:

運行(正在運行或在運行隊列中等待)
中斷(休眠中, 受阻, 在等待某個條件的形成或接受到信號)
不可中斷(收到信號不喚醒和不可運行, 進程必須等待直到有中斷發生)
僵死(進程已終止, 但進程描述符存在, 直到父進程調用wait4()系統調用後釋放)
停止(進程收到SIGSTOP, SIGSTP, SIGTIN, SIGTOU信號後停止運行運行)
ps工具標識進程的5種狀態碼:

D 不可中斷 uninterruptible sleep (usually IO)
R 運行 runnable (on run queue)
S 中斷 sleeping
T 停止 traced or stopped
Z 僵死 a defunct (”zombie”) process
4.1. 命令參數
a 顯示所有進程
-a 顯示同一終端下的所有程序
-A 顯示所有進程
c 顯示進程的真實名稱
-N 反向選擇
-e 等於“-A”
e 顯示環境變量
f 顯示程序間的關係
-H 顯示樹狀結構
r 顯示當前終端的進程
T 顯示當前終端的所有程序
u 指定用戶的所有進程
-au 顯示較詳細的資訊
-aux 顯示所有包含其他使用者的行程
-C<命令> 列出指定命令的狀況
–lines<行數> 每頁顯示的行數
–width<字符數> 每頁顯示的字符數
–help 顯示幫助信息
–version 顯示版本顯示
4.2. 輸出列的含義
F 代表這個程序的旗標 (flag)， 4 代表使用者為 super user
S 代表這個程序的狀態 (STAT)，關於各 STAT 的意義將在內文介紹
UID 程序被該 UID 所擁有
PID 進程的ID
PPID 則是其上級父程序的ID
C CPU 使用的資源百分比
PRI 這個是 Priority (優先執行序) 的縮寫，詳細後面介紹
NI 這個是 Nice 值，在下一小節我們會持續介紹
ADDR 這個是 kernel function，指出該程序在內存的那個部分。如果是個 running的程序，一般就是 “-“
SZ 使用掉的內存大小
WCHAN 目前這個程序是否正在運作當中，若為 - 表示正在運作
TTY 登入者的終端機位置
TIME 使用掉的 CPU 時間。
CMD 所下達的指令為何
4.3. 使用實例
### 實例1：顯示所有進程信息
```
[root@localhost test6]# ps -A
PID TTY          TIME CMD
1 ?        00:00:00 init
2 ?        00:00:01 migration/0
3 ?        00:00:00 ksoftirqd/0
4 ?        00:00:01 migration/1
5 ?        00:00:00 ksoftirqd/1
6 ?        00:29:57 events/0
7 ?        00:00:00 events/1
8 ?        00:00:00 khelper
49 ?        00:00:00 kthread
54 ?        00:00:00 kblockd/0
55 ?        00:00:00 kblockd/1
56 ?        00:00:00 kacpid
217 ?        00:00:00 cqueue/0
……省略部分結果
```

### 實例2：顯示指定用戶信息
```
[root@localhost test6]# ps -u root
PID TTY          TIME CMD
1 ?        00:00:00 init
2 ?        00:00:01 migration/0
3 ?        00:00:00 ksoftirqd/0
4 ?        00:00:01 migration/1
5 ?        00:00:00 ksoftirqd/1
6 ?        00:29:57 events/0
7 ?        00:00:00 events/1
8 ?        00:00:00 khelper
49 ?        00:00:00 kthread
54 ?        00:00:00 kblockd/0
55 ?        00:00:00 kblockd/1
56 ?        00:00:00 kacpid
……省略部分結果
```

### 實例3：顯示所有進程信息，連同命令行

```
[root@localhost test6]# ps -ef
UID        PID  PPID  C STIME TTY          TIME CMD
root         1     0  0 Nov02 ?        00:00:00 init [3]
root         2     1  0 Nov02 ?        00:00:01 [migration/0]
root         3     1  0 Nov02 ?        00:00:00 [ksoftirqd/0]
root         4     1  0 Nov02 ?        00:00:01 [migration/1]
root         5     1  0 Nov02 ?        00:00:00 [ksoftirqd/1]
root         6     1  0 Nov02 ?        00:29:57 [events/0]
root         7     1  0 Nov02 ?        00:00:00 [events/1]
root         8     1  0 Nov02 ?        00:00:00 [khelper]
root        49     1  0 Nov02 ?        00:00:00 [kthread]
root        54    49  0 Nov02 ?        00:00:00 [kblockd/0]
root        55    49  0 Nov02 ?        00:00:00 [kblockd/1]
root        56    49  0 Nov02 ?        00:00:00 [kacpid]
```

### 實例4： ps 與grep 組合使用，查找特定進程
```
[root@localhost test6]# ps -ef|grep ssh
root      2720     1  0 Nov02 ?        00:00:00 /usr/sbin/sshd
root     17394  2720  0 14:58 ?        00:00:00 sshd: root@pts/0
root     17465 17398  0 15:57 pts/0    00:00:00 grep ssh
```

### 實例5：將與這次登入的 PID 與相關信息列示出來
```
[root@localhost test6]# ps -l
F S   UID   PID  PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
4 S     0 17398 17394  0  75   0 - 16543 wait   pts/0    00:00:00 bash
4 R     0 17469 17398  0  77   0 - 15877 -      pts/0    00:00:00 ps
```

###實例6：列出目前所有的正在內存中的程序
```
[root@localhost test6]# ps aux
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
root         1  0.0  0.0  10368   676 ?        Ss   Nov02   0:00 init [3]
root         2  0.0  0.0      0     0 ?        S<   Nov02   0:01 [migration/0]
root         3  0.0  0.0      0     0 ?        SN   Nov02   0:00 [ksoftirqd/0]
root         4  0.0  0.0      0     0 ?        S<   Nov02   0:01 [migration/1]
root         5  0.0  0.0      0     0 ?        SN   Nov02   0:00 [ksoftirqd/1]
root         6  0.0  0.0      0     0 ?        S<   Nov02  29:57 [events/0]
root         7  0.0  0.0      0     0 ?        S<   Nov02   0:00 [events/1]
root         8  0.0  0.0      0     0 ?        S<   Nov02   0:00 [khelper]
root        49  0.0  0.0      0     0 ?        S<   Nov02   0:00 [kthread]
root        54  0.0  0.0      0     0 ?        S<   Nov02   0:00 [kblockd/0]
root        55  0.0  0.0      0     0 ?        S<   Nov02   0:00 [kblockd/1]
root        56  0.0  0.0      0     0 ?        S<   Nov02   0:00 [kacpid]
```
Next  Previous
