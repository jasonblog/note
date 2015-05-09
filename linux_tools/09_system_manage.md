# 系統管理及IPC資源管理
目錄

系統管理及IPC資源管理
系統管理
查詢系統版本
查詢硬件信息
設置系統時間
IPC資源管理
IPC資源查詢
檢測和設置系統資源限制
總結
9.1. 系統管理
查詢系統版本
查看Linux系統版本:

```
$uname -a
$lsb_release -a
```

查看Unix系統版本：操作系統版本:

```
$more /etc/release
```

查詢硬件信息
查看CPU使用情況:

```
$sar -u 5 10
```

查詢CPU信息:

```
$cat /proc/cpuinfo
```

查看CPU的核的個數:

```
$cat /proc/cpuinfo | grep processor | wc -l
```

查看內存信息:

```
$cat /proc/meminfo
```

顯示內存page大小（以KByte爲單位）:

```
$pagesize
```

顯示架構:

```
$arch
```

設置系統時間
顯示當前系統時間:

```
$date
```

設置系統日期和時間(格式爲2014-09-15 17:05:00):

```
$date -s 2014-09-15 17:05:00
$data -s 2014-09-15
$date -s 17:05:00
```

設置時區:

```
選擇時區信息。命令爲：tzselect
根據系統提示，選擇相應的時區信息。
```

強制把系統時間寫入CMOS（這樣，重啓後時間也正確了）:

```
$clock -w
```

警告

設置系統時間需要root用戶權限.
9.2. IPC資源管理
IPC資源查詢
查看系統使用的IPC資源:

```
$ipcs

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status

------ Semaphore Arrays --------
key        semid      owner      perms      nsems
0x00000000 229376     weber      600        1

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
```

查看系統使用的IPC共享內存資源:

```
$ipcs -m
```

查看系統使用的IPC隊列資源:

```
$ipcs -q
```

查看系統使用的IPC信號量資源:

```
$ipcs -s
```

應用示例：查看IPC資源被誰佔用

有個IPCKEY：51036 ，需要查詢其是否被佔用；

首先通過計算器將其轉爲十六進制:
51036 -> c75c

如果知道是被共享內存佔用:

```
$ipcs -m | grep c75c
0x0000c75c 40403197   tdea3    666        536870912  2
```

如果不確定，則直接查找:

```
$ipcs | grep c75c
0x0000c75c 40403197   tdea3    666        536870912  2
0x0000c75c 5079070    tdea3    666        4
```

檢測和設置系統資源限制
顯示當前所有的系統資源limit 信息:

```
ulimit – a
```

對生成的 core 文件的大小不進行限制:

```
ulimit – c unlimited
```

9.3. 總結

uname sar arch date ipcs ulimit
