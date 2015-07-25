# 網絡工具
目錄

網絡工具
查詢網絡服務和端口
網絡路由
鏡像下載
ftp sftp ssh
網絡複製
總結
7.1. 查詢網絡服務和端口
netstat 命令用於顯示各種網絡相關信息，如網絡連接，路由表，接口狀態 (Interface Statistics)，masquerade 連接，多播成員 (Multicast Memberships) 等等。

列出所有端口 (包括監聽和未監聽的):
```
netstat -a
```

列出所有 tcp 端口:

```
netstat -at
```

列出所有有監聽的服務狀態:

```
netstat -l
```

使用netstat工具查詢端口:

```
$netstat -antp | grep 6379
tcp        0      0 127.0.0.1:6379          0.0.0.0:*               LISTEN      25501/redis-server

$ps 25501
  PID TTY      STAT   TIME COMMAND
25501 ?        Ssl   28:21 ./redis-server ./redis.conf
```

lsof（list open files）是一個列出當前系統打開文件的工具。在linux環境下，任何事物都以文件的形式存在，通過文件不僅僅可以訪問常規數據，還可以訪問網絡連接和硬件。所以如傳輸控制協議 (TCP) 和用戶數據報協議 (UDP) 套接字等； 在查詢網絡端口時，經常會用到這個工具。

查詢7902端口現在運行什麼程序:

```
#分為兩步
#第一步，查詢使用該端口的進程的PID；
    $lsof -i:7902
    COMMAND   PID   USER   FD   TYPE    DEVICE SIZE NODE NAME
    WSL     30294 tuapp    4u  IPv4 447684086       TCP 10.6.50.37:tnos-dp (LISTEN)

#查到30294
#使用ps工具查詢進程詳情：
$ps -fe | grep 30294
tdev5  30294 26160  0 Sep10 ?        01:10:50 tdesl -k 43476
root     22781 22698  0 00:54 pts/20   00:00:00 grep 11554
```

註解

以上介紹lsof關於網絡方面的應用，這個工具非常強大，需要好好掌握，詳見 lsof 一切皆文件 ;
7.2. 網絡路由
查看路由狀態:

```
$route -n
```

發送ping包到地址IP:

```
$ping IP
```

探測前往地址IP的路由路徑:

```
$traceroute IP
```

DNS查詢，尋找域名domain對應的IP:

```
$host domain
```

反向DNS查詢:

```
$host IP
```

7.3. 鏡像下載
直接下載文件或者網頁:

```
wget url
```

常用選項:

–limit-rate :下載限速
-o：指定日誌文件；輸出都寫入日誌；
-c：斷點續傳
7.4. ftp sftp ssh
SSH登陸:

```
$ssh ID@host
```

ssh登陸遠程服務器host，ID為用戶名。

ftp/sftp文件傳輸:

```
$sftp ID@host
```

登陸服務器host，ID為用戶名。sftp登陸後，可以使用下面的命令進一步操作：

get filename # 下載文件
put filename # 上傳文件
ls # 列出host上當前路徑的所有文件
cd # 在host上更改當前路徑
lls # 列出本地主機上當前路徑的所有文件
lcd # 在本地主機更改當前路徑
7.5. 網絡複製
將本地localpath指向的文件上傳到遠程主機的path路徑:

```
$scp localpath ID@host:path
```

以ssh協議，遍歷下載path路徑下的整個文件系統，到本地的localpath:
```

$scp -r ID@site:path localpath
```

7.6. 總結
netstat lsof route ping host wget sftp scp
