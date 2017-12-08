# 樹莓派autossh反向隧道


通常我們會將樹莓派放在家裡，做家庭影院之類的用途。而現在，各個運營商的寬帶都開始實施內網政策，也就是不會給民宅公網IP，所以像以前藉助花生殼軟件來實現DNS動態修改的方法是無法奏效了。

那麼該怎麼辦呢？

##ssh反向隧道

現在，我們想要在外部訪問家庭網絡中的樹莓派，可以使用ssh反向隧道技術，它的原理比較簡單：

樹莓派主動向某公網服務器建立ssh連接，並請求公網服務器開啟1個額外的SSH的服務端口，充當樹莓派的反向代理服務。樹莓派與公網服務器之間的TCP（SSH）連接是樹莓派主動發起的，而公網服務器與外部用戶之間的TCP（SSH）連接是外部用戶主動發起的，公網服務器在中間充當代理角色，轉發兩側的數據。

從更具體的角度來講，外部用戶到公網服務器之間可以建立多條TCP連接，而公網服務器到樹莓派則只有一條共享的反向TCP連接，這是整個技術實現角度的原理。

##配置autossh

樹莓派向公網服務器建立的ssh連接可能因為網絡問題斷開，所以一般我們不直接使用ssh命令而是使用一個監督程序叫做autossh，它負責拉起ssh命令，並且當ssh斷開後可以重新拉起ssh。

首先，因為autossh會幫我們建立到公網服務器的ssh連接，為了免去輸入密碼的問題，我們要讓公網服務器信任樹莓派。

為樹莓派生成ssh公鑰私鑰：


```sh
pi@raspberrypi:~ $ ssh-keygen -t rsa
```

然後通過ssh自帶的命令將樹莓派的私鑰拷貝到公網服務器，這樣公網服務器就完成了對樹莓派的信任配置：


```sh
pi@raspberrypi:~ $ ssh-copy-id -i ~/.ssh/id_rsa work@yuerblog.cc
```

上述命令將樹莓派pi用戶授信給公網服務器yuerblog.cc的work用戶，此後在pi用戶下ssh work@yuerblog.cc就不需要輸入密碼了，你可以自己驗證。

接下來，安裝autossh：


```sh
pi@raspberrypi:~ $ sudo apt-get install autossh
```

編寫啟停autossh的腳本：


```sh
pi@raspberrypi:~ $ vim /etc/init.d/autossh.sh
 
#Insert the following scripts
 
#!/bin/sh
### BEGIN INIT INFO
# Provides:          autossh
# Required-Start:    $local_fs $remote_fs $network $syslog
# Required-Stop:     $local_fs $remote_fs $network $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: starts the autossh
# Description:       starts the autossh
### END INIT INFO
 
case "$1" in
    start)
    echo "start autossh"
    killall -0 autossh
    if [ $? -ne 0 ];then
       sudo /usr/bin/autossh -M 888 -fN -o "PubkeyAuthentication=yes" -o "StrictHostKeyChecking=false" -o "PasswordAuthentication=no" -o "ServerAliveInterval 60" -o "ServerAliveCountMax 3" -R 2222:localhost:22 -i /home/pi/.ssh/id_rsa work@yuerblog.cc
    fi
    ;;
    stop)
    sudo killall autossh
    ;;
    restart)
    sudo killall autossh
    sudo /usr/bin/autossh -M 888 -fN -o "PubkeyAuthentication=yes" -o "StrictHostKeyChecking=false" -o "PasswordAuthentication=no" -o "ServerAliveInterval 60" -o "ServerAliveCountMax 3" -R 2222:localhost:22 -i /home/pi/.ssh/id_rsa work@yuerblog.cc
    ;;
    *)
    echo "Usage: $0 (start|stop|restart)"
    ;;
esac
exit 0
 
#Then save the file
```

這個腳本支持start/stop/restart三個命令，啟動autossh的命令參數作用如下：

- -o是指定ssh命令的參數，不需要做修改。
- -M是autossh啟動的ssh進程會監聽888這個端口，autossh進程會通過探測ssh進程的888端口來獲知ssh連接是否斷開。
- -f是指autossh後臺運行，不會阻塞shell繼續向下執行。
- -N是指建立的ssh連接只用於轉發數據，不解析命令。
- -R是指建立反向隧道，一般我們ssh某個服務器是正向隧道。
- 2222是公網服務器上的代理端口。
- localhost:22是指公網2222端口代理到的樹莓派端口。（當你連接公網服務器的2222端口後，數據通過樹莓派之間的反向ssh連接到達樹莓派，由樹莓派端的ssh進程代理髮起到localhost:22的2次連接，從而實現ssh訪問樹莓派的目的）
- -i指定的是之前授信給公網服務器的ssh私鑰
- 最後是公網服務器的地址，autossh會調用ssh建立到它的ssh反向隧道。

本來打算將上述腳本做到開啟自動啟動中，但是試驗發現開機沒有成功啟動autossh，索性把它寫到crontab裡算了：


```sh
* * * * * /bin/bash /etc/init.d/autossh.sh start
```

接下來，你可以觀察是否有autossh進程被拉起：


```sh
pi@raspberrypi:~ $ ps aux|grep autossh
root       692  0.0  0.2   1812  1276 ?        Ss   Sep13   0:00 /usr/lib/autossh/autossh -M 888 -N  -o PubkeyAuthentication=yes -o StrictHostKeyChecking=false -o PasswordAuthentication=no -o ServerAliveInterval 60 -o ServerAliveCountMax 3 -R 2222:localhost:22 -i /home/pi/.ssh/id_rsa work@yuerblog.cc
pi       11277  0.0  0.4   4744  2008 pts/0    S+   09:44   0:00 grep --color=auto autossh
```


##最後

現在，我登錄yuerblog.cc服務器，執行如下命令即可登錄到pi：

```sh
[work@vultr ~]$ ssh pi@localhost -p 2222
```