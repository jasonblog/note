# ipcs 查詢進程間通信狀態
ipcs是Linux下顯示進程間通信設施狀態的工具。可以顯示消息隊列、共享內存和信號量的信息。對於程序員非常有用，普通的系統管理員一般用不到此指令。

### 7.1. IPC資源查詢
### 查看系統使用的IPC資源
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

分別查詢IPC資源:
```
$ipcs -m 查看系統使用的IPC共享內存資源
$ipcs -q 查看系統使用的IPC隊列資源
$ipcs -s 查看系統使用的IPC信號量資源
```

### 查看IPC資源被誰佔用
示例：有個IPCKEY(51036)，需要查詢其是否被佔用；

首先通過計算器將其轉為十六進制:
```
51036 -> c75c
```

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
### 7.2. 系統IPC參數查詢
```
ipcs -l

------ Shared Memory Limits --------
max number of segments = 4096
max seg size (kbytes) = 4194303
max total shared memory (kbytes) = 1073741824
min seg size (bytes) = 1

------ Semaphore Limits --------
max number of arrays = 128
max semaphores per array = 250
max semaphores system wide = 32000
max ops per semop call = 32
semaphore max value = 32767

------ Messages: Limits --------
max queues system wide = 2048
max size of message (bytes) = 524288
default max size of queue (bytes) = 5242880
```

以上輸出顯示，目前這個系統的允許的最大內存為1073741824kb；最大可使用128個信號量，每個消息的最大長度為524288bytes；

### 7.3. 修改IPC系統參數
以linux系統為例，在root用戶下修改/etc/sysctl.conf 文件，保存後使用sysctl -p生效:
```
$cat /etc/sysctl.conf
# 一個消息的最大長度
kernel.msgmax = 524288

# 一個消息隊列上的最大字節數
# 524288*10
kernel.msgmnb = 5242880

#最大消息隊列的個數
kernel.msgmni=2048

#一個共享內存區的最大字節數
kernel.shmmax = 17179869184

#系統範圍內最大共享內存標識數
kernel.shmmni=4096

#每個信號燈集的最大信號燈數 系統範圍內最大信號燈數 每個信號燈支持的最大操作數 系統範圍內最大信號燈集數
#此參數為系統默認，可以不用修改
#kernel.sem = <semmsl> <semmni>*<semmsl> <semopm> <semmni>
kernel.sem = 250 32000 32 128
```
### 顯示輸入不帶標誌的 ipcs：的輸出:
```
$ipcs
IPC status from /dev/mem as of Mon Aug 14 15:03:46 1989
T    ID         KEY        MODE       OWNER     GROUP
Message Queues:
q       0    0x00010381 -Rrw-rw-rw-   root      system
q   65537    0x00010307 -Rrw-rw-rw-   root      system
q   65538    0x00010311 -Rrw-rw-rw-   root      system
q   65539    0x0001032f -Rrw-rw-rw-   root      system
q   65540    0x0001031b -Rrw-rw-rw-   root      system
q   65541    0x00010339--rw-rw-rw-    root      system
q       6    0x0002fe03 -Rrw-rw-rw-   root      system
Shared Memory:
m   65537    0x00000000 DCrw-------   root      system
m  720898    0x00010300 -Crw-rw-rw-   root      system
m   65539    0x00000000 DCrw-------   root      system
Semaphores:
s  131072    0x4d02086a --ra-ra----   root      system
s   65537    0x00000000 --ra-------   root      system
s 1310722    0x000133d0 --ra-------   7003      30720
```
### 7.4. 清除IPC資源
使用ipcrm 命令來清除IPC資源：這個命令同時會將與ipc對象相關聯的數據也一起移除。當然，只有root用戶，或者ipc對象的創建者才有這項權利；

ipcrm用法:
```
ipcrm -M shmkey  移除用shmkey創建的共享內存段
ipcrm -m shmid    移除用shmid標識的共享內存段
ipcrm -Q msgkey  移除用msqkey創建的消息隊列
ipcrm -q msqid  移除用msqid標識的消息隊列
ipcrm -S semkey  移除用semkey創建的信號
ipcrm -s semid  移除用semid標識的信號
```
### 清除當前用戶創建的所有的IPC資源:
```
ipcs -q | awk '{ print "ipcrm -q "$2}' | sh > /dev/null 2>&1;
ipcs -m | awk '{ print "ipcrm -m "$2}' | sh > /dev/null 2>&1;
ipcs -s | awk '{ print "ipcrm -s "$2}' | sh > /dev/null 2>&1;
```
