# 進程管理工具
目錄

進程管理工具
查詢進程
終止進程
進程監控
分析線程棧
綜合運用
總結
這一節我們介紹進程管理工具；

使用進程管理工具，我們可以查詢程序當前的運行狀態，或終止一個進程；

任何進程都與文件關聯；我們會用到lsof工具（list opened files），作用是列舉系統中已經被打開的文件。在linux環境中，任何事物都是文件，設備是文件，目錄是文件，甚至sockets也是文件。用好lsof命令，對日常的linux管理非常有幫助。

5.1. 查詢進程
查詢正在運行的進程信息
```
$ps -ef
```

eg:查詢歸屬於用戶colin115的進程

```
$ps -ef | grep colin115
$ps -lu colin115
```

查詢進程ID（適合只記得部分進程字段）

```
$pgrep 查找進程

eg:查詢進程名中含有re的進程
[/home/weber#]pgrep -l re
2 kthreadd
28 ecryptfs-kthrea
29515 redis-server
```

以完整的格式顯示所有的進程

```
$ps -ajx
```

顯示進程信息，並實時更新

```
$top
```

查看端口佔用的進程狀態：

```
lsof -i:3306
```

查看用戶username的進程所打開的文件

```
$lsof -u username
```

查詢init進程當前打開的文件

```
$lsof -c init
```

查詢指定的進程ID(23295)打開的文件：

```
$lsof -p 23295
```

查詢指定目錄下被進程開啓的文件（使用+D 遞歸目錄）：
```
$lsof +d mydir1/
```

5.2. 終止進程
殺死指定PID的進程 (PID爲Process ID)

```
$kill PID
```

殺死相關進程

```
kill -9 3434
```

殺死job工作 (job爲job number)

```
$kill %job
```

5.3. 進程監控
查看系統中使用CPU、使用內存最多的進程；

```
$top
(->)P
```

輸入top命令後，進入到交互界面；接着輸入字符命令後顯示相應的進程狀態：

對於進程，平時我們最常想知道的就是哪些進程佔用CPU最多，佔用內存最多。以下兩個命令就可以滿足要求:

```
P：根據CPU使用百分比大小進行排序。
M：根據駐留內存大小進行排序。
i：使top不顯示任何閒置或者僵死進程。
```

這裏介紹最使用的幾個選項,對於更詳細的使用，詳見 top linux下的任務管理器 ;

5.4. 分析線程棧
使用命令pmap，來輸出進程內存的狀況，可以用來分析線程堆棧；

```
$pmap PID

eg:
[/home/weber#]ps -fe| grep redis
weber    13508 13070  0 08:14 pts/0    00:00:00 grep --color=auto redis
weber    29515     1  0  2013 ?        02:55:59 ./redis-server redis.conf
[/home/weber#]pmap 29515
29515:   ./redis-server redis.conf
08048000    768K r-x--  /home/weber/soft/redis-2.6.16/src/redis-server
08108000      4K r----  /home/weber/soft/redis-2.6.16/src/redis-server
08109000     12K rw---  /home/weber/soft/redis-2.6.16/src/redis-server
```

5.5. 綜合運用
將用戶colin115下的所有進程名以av_開頭的進程終止:

```
ps -u colin115 |  awk '/av_/ {print "kill -9 " $1}' | sh
```

將用戶colin115下所有進程名中包含HOST的進程終止:

```
ps -fe| grep colin115|grep HOST |awk '{print $2}' | xargs kill -9;
```

5.6. 總結
ps top lsof kill pmap
