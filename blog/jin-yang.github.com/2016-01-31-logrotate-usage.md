---
title: logrotate 使用方法
layout: post
comments: true
language: chinese
category: [linux, misc]
keywords: linux,log,日誌,滾動,logrotate,rotation
description: 通常我們會通過日誌記錄程序運行時各種信息，如有異常，則通過日誌分析用戶行為、記錄運行軌跡、查找程序問題。但是，磁盤空間有限，為了節省空間和方便整理，可以將日誌文件按時間或大小分成多份，刪除時間久遠的日誌文件。這就是通常說的日誌滾動 (log rotation)，在本文中，就簡單介紹下 Linux 中的 logrotate 程序。
---

通常我們會通過日誌記錄程序運行時各種信息，如有異常，則通過日誌分析用戶行為、記錄運行軌跡、查找程序問題。但是，磁盤空間有限，為了節省空間和方便整理，可以將日誌文件按時間或大小分成多份，刪除時間久遠的日誌文件。

這就是通常說的日誌滾動 (log rotation)，在本文中，就簡單介紹下 Linux 中的 logrotate 程序。

<!-- more -->

## 簡介

logrotate 是默認安裝的，一般會定時運行 logrotate，通常是每天一次；會通過 crontab 運行，也就是直接運行 /etc/cron.daily/logrotate 腳本。

其中，CentOS 系統中的腳本內容如下。

{% highlight bash %}
#!/bin/sh

/usr/sbin/logrotate -s /var/lib/logrotate/logrotate.status /etc/logrotate.conf
EXITVALUE=$?
if [ $EXITVALUE != 0 ]; then
    /usr/bin/logger -t logrotate "ALERT exited abnormally with [$EXITVALUE]"
fi
exit 0
{% endhighlight %}

具體執行時間可以查看 /etc/crontab 或者 /etc/anacrontab (CentOS)。

### 配置文件

默認使用 /etc/logrotate.conf 文件，作為全局配置，而不同應用的具體配置則在 /etc/logrotate.d 目錄下，通常以應用程序的名稱命名，例如 nginx、mysql、syslog、yum 等配置。

如下，是一個配置文件的具體內容。

{% highlight text %}
/var/log/nginx/*.log /var/log/tomcat/*log {   # 可以指定多個路徑
    daily                      # 日誌輪詢週期，weekly,monthly,yearly
    rotate 30                  # 保存30天數據，超過的則刪除
    size +100M                 # 超過100M時分割，單位K,M,G，優先級高於daily
    compress                   # 切割後壓縮，也可以為nocompress
    delaycompress              # 切割時對上次的日誌文件進行壓縮
    dateext                    # 日誌文件切割時添加日期後綴
    missingok                  # 如果沒有日誌文件也不報錯
    notifempty                 # 日誌為空時不進行切換，默認為ifempty
    create 640 nginx nginx     # 使用該模式創建日誌文件
    sharedscripts              # 所有的文件切割之後只執行一次下面腳本
    postrotate
        if [ -f /var/run/nginx.pid ]; then
            kill -USR1 `cat /var/run/nginx.pid`
        fi
    endscript
}
{% endhighlight %}

當配置完成後，可以通過如下方式進行測試。

{% highlight text %}
----- 可直接手動執行
$ logrotate --force /etc/logrotate.d/nginx

----- 顯示詳細的信息；而且--debug/-d實際上不會操作具體文件(Dry Run)
$ logrotate --debug --verbose --force /etc/logrotate.d/nginx
{% endhighlight %}

在 CentOS 中，默認會將日誌保存 /var/lib/logrotate.status 文件中，如果需要指定到其它文件，可以通過 ```-s/--state``` 參數指定。

## 日誌處理

首先，介紹下在 Linux 系統中是如何處理文件的。

進程每打開一個文件，系統會分配一個新的文件描述符給這個文件；這個文件描述符中保存了文件的狀態信息、當前文件位置以及文件的 inode 信息。對文件的操作是通過文件描述符完成，所以文件路徑發生改變不會影響文件操作。

### 測試

可以通過 python 腳本進行簡單測試，示例如下：

{% highlight python %}
#!/bin/python
import time
with open("/tmp/foobar.txt", "w") as file:
    while 1:
        file.write("test\n")
        file.flush()
        time.sleep(1)
{% endhighlight %}

當通過 ```mv /tmp/{foobar.txt,test.txt}``` 移動文件之後，可以看到 test.txt 文件仍然在寫入。原因如上所述，Linux 在執行 mv 操作時，文件描述符是不變的，只修改了文件名。

其中 logrotate 提供了兩種方式，分別為 create 和 copytruncate 。

### create

這也就是默認的方案，可以通過 create 命令配置文件的權限和屬組設置；這個方案的思路是重命名原日誌文件，創建新的日誌文件。詳細步驟如下：

1. 重命名正在輸出日誌文件，因為重命名只修改目錄以及文件的名稱，而進程操作文件使用的是 inode，所以並不影響原程序繼續輸出日誌。

2. 創建新的日誌文件，文件名和原日誌文件一樣，注意，此時只是文件名稱一樣，而 inode 編號不同，原程序輸出的日誌還是往原日誌文件輸出。

3. 最後通過某些方式通知程序，重新打開日誌文件；由於重新打開日誌文件會用到文件路徑而非 inode 編號，所以打開的是新的日誌文件。

如上也就是 logrotate 的默認操作方式，也就是 mv+create 執行完之後，通知應用重新在新文件寫入即可。mv+create 成本都比較低，幾乎是原子操作，如果應用支持重新打開日誌文件，如 syslog, nginx, mysql 等，那麼這是最好的方式。

不過，有些程序並不支持這種方式，壓根沒有提供重新打開日誌的接口；而如果重啟應用程序，必然會降低可用性，為此引入瞭如下方式。

### copytruncate

該方案是把正在輸出的日誌拷 (copy) 一份出來，再清空 (trucate) 原來的日誌；詳細步驟如下：

1. 將當前正在輸出的日誌文件複製為目標文件，此時程序仍然將日誌輸出到原來文件中，此時，原文件名也沒有變。

2. 清空日誌文件，原程序仍然還是輸出到預案日誌文件中，因為清空文件只把文件的內容刪除了，而 inode 並沒改變，後續日誌的輸出仍然寫入該文件中。

<!--
文件清空並不影響到輸出日誌的程序的文件表裡的文件位置信息，因為各進程的文件表是獨立的。那麼文件清空後，程序輸出的日誌應該接著之前日誌的偏移位置輸出，這個位置之前會被\0填充才對。但實際上logroate清空日誌文件後，程序輸出的日誌都是從文件開始處開始寫的。這是怎麼做到的？這個問題讓我糾結了很久，直到某天靈光一閃，這不是logrotate做的，而是成熟的寫日誌的方式，都是用O_APPEND的方式寫的。如果程序沒有用O_APPEND方式打開日誌文件，變會出現copytruncate後日誌文件前面會被一堆\0填充的情況。

日誌在拷貝完到清空文件這段時間內，程序輸出的日誌沒有備份就清空了，這些日誌不是丟了嗎？是的，copytruncate有丟失部分日誌內容的風險。所以能用create的方案就別用copytruncate。所以很多程序提供了通知我更新打開日誌文件的功能來支持create方案，或者自己做了日誌滾動，不依賴logrotate。
-->

如上所述，對於 copytruncate 也就是先複製一份文件，然後清空原有文件。

通常來說，清空操作比較快，但是如果日誌文件太大，那麼複製就會比較耗時，從而可能導致部分日誌丟失。不過這種方式不需要應用程序的支持即可。


## FAQ

簡單列舉下與 logrotate 相關的常見問題。

#### 問題：sharedscripts的作用是什麼？

如上配置中，通過通配符 ```'*'``` 指定了多個日誌文件，例如 access.log、error.log ，這時會在所有的日誌文件都執行完成之後，再統一執行一次腳本；如果沒有這條命令，則每個日誌文件執行完成後都會執行一次腳本。

#### 問題：rotate和maxage的區別是什麼？

都是用來控制保存多少日誌文件的，區別在於 rotate 是以個數為單位的，而 maxage 是以天數為單位的。如果是以天來輪轉日誌，那麼二者的差別就不大。


<!--
問題：如何告訴應用程序重新打開日誌文件？
Nginx通過postrotate指令發送USR1信號來通知Nginx重新打開日誌文件的；MySQL通過flush-logs來重新打開日誌文件的。更有甚者，有些應用程序就壓根沒有提供類似的方法，如果要重新打開日誌文件，就必須重啟服務。

。還好Logrotate提供了一個名為copytruncate的指令，此方法採用的是先拷貝再清空的方式，整個過程中日誌文件的操作句柄沒有發生改變，所以不需要通知應用程序重新打開日誌文件，但是需要注意的是，在拷貝和清空之間有一個時間差，所以可能會丟失部分日誌數據。

BTW：MySQL本身在support-files目錄已經包含了一個名為mysql-log-rotate的腳本，不過它比較簡單，更詳細的日誌輪轉詳見「Rotating MySQL Slow Logs Safely」。

-->

## 參考

關於 MySQL 如何安全地處理日誌文件，可以查看 [Rotating MySQL Slow Logs Safely](https://engineering.groupon.com/2013/mysql/rotating-mysql-slow-logs-safely/) 中的相關介紹，否則可能會導致異常。



<!--
logrotate的copytruncate參數導致打開的文件產生空洞

日誌文件保存時候使用> 將其保存，對文件進行清除和日誌切割(logrotate)時，容易出現文件空洞。

需要使用“>>” 可以避免該問題。修改後問題確實解決了。

在此Mark一下。

同理，在用fopen 打開或創建日誌文件時，打開方式使用“w”類似於>，使用"a"類似於>>。如果需要對日誌文件進行logrotate備份，最好使用"a"方式創建或打開。（經過測試）

以上情況發生的條件是，日誌文件一直處於打開狀態，進程沒有被重啟，如果進程在備份完成之後重新啟動或運行，則不會導致文件空洞，因為文件的offset指示器已經到了文件頭。




logrotate機制和原理
http://www.lightxue.com/how-logrotate-works
-->


{% highlight text %}
{% endhighlight %}
