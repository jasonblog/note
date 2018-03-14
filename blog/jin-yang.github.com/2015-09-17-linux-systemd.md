---
title: systemd 使用簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: systemd,簡介,linux,init
description: 現在一般新發行的版本會採用新的 init 進程，也就是 systemd ，其中啟動過程可以通過 man bootup 查看。在此，簡單介紹一下 systemd 。
---

現在一般新發行的版本會採用新的 init 進程，也就是 systemd ，其中啟動過程可以通過 man bootup 查看。

在此，簡單介紹一下 systemd 。

<!-- more -->

## 簡介

Linux 內核通過執行 init 將 CPU 的控制權限，交給其它的任務，在 CentOS 中可以通過如下命令查看 init 來自於那個包。

{% highlight text %}
$ rpm -qif `which init`
{% endhighlight %}

init 進程經歷了，SysV、Upstart 以及 systemd，而 systemd 是最具爭議的一個項目，因為其不只替換了 init，而且還包括了一整套的系統任務。

作為最新的系統和服務管理器，其設計思路借鑑了 Mac 的啟動程序 Launchd，兼容 SysV 和 LSB 的啟動腳本。事實上其作用遠不僅是啟動系統，它還接管了系統服務的啟動、結束、狀態查詢和日誌歸檔等職責，並支持定時任務和通過特定事件，如插入特定 USB 設備和特定端口數據觸發的任務。

其有以下特性：支持並行化任務、同時採用 socket 和 D-BUS 總線式激活服務、按需啟動相應的守護進程、利用 Linux 的 cgroup 監控進程、支持快照和系統恢復、維護掛載點和自動掛載點，各服務間基於依賴關係進行精密控制。


<!--
Systemd同時也清晰地處理了系統關機過程。它在/usr/lib/systemd/目錄下有三個腳本，分別叫systemd-halt.service，systemd-poweroff.service，systemd-reboot.service。這幾個腳本會在用戶選擇關機，重啟或待機時執行。在接收到關機事件時，systemd首先卸載所有文件系統並停止所有內存交換設備，斷開存儲設備，之後停止所有剩下的進程。
-->

可以通過 pstree 查看啟動的進程樹，接下來查看一下 systemd 的特性。


## Systemd

Systemd 使用 ```target``` 來處理引導和服務管理過程，這些 systemd 裡的 ```target``` 文件被用於分組不同的引導單元以及啟動同步進程。

如果 A 要求 B 在 A 之前運行，則在 [Unit] 段中添加 Requires=B 和 After=B，如果依賴關係是可選的，可添加 Wants=B 和 After=B；注意 Wants= 和 Requires= 並不意味著 After=，即如果 After= 選項沒有設置，這兩個單元將被並行啟動。

### 執行順序

執行的第一個目標是 ```/etc/systemd/system/default.target```，對於桌面版本，通常會指向 ```/usr/lib/systemd/system/graphical.target```，該文件為文本，可以查看其實際會依次依賴於 ```multi-user.target => basic.target => sysinit.target``` 。

另外，```local-fs.target``` 單元並不會啟動用戶相關的服務，它只處理與文件系統相關的底層核心服務，會根據 ```/etc/fstab``` 和 ```/etc/inittab``` 來執行相關操作。

```sysinit.target``` 會啟動重要的系統服務例如系統掛載，內存交換空間和設備，內核補充選項等等。```basic.target``` 會啟動普通服務特別是圖形管理服務，它通過 ```/etc/systemd/system/basic.target.wants``` 目錄來決定哪些服務會被啟動。

在這個階段，會啟動 ```multi-user.target``` 而這個 target 將自己的子單元放在目錄 ```/etc/systemd/system/multi-user.target.wants``` 裡。這個 target 為多用戶支持設定系統環境，非 root 用戶會在這個階段的引導過程中啟用。防火牆相關的服務也會在這個階段啟動。

登陸是通過 ```systemd-logind.service``` 進行，可以通過 ```systemctl help systemd-logind.service``` 查看幫助，通常是針對 XWindow，而終端登陸則通過 ```/usr/lib/systemd/system/getty@.service``` 執行。

依賴關係可以查看 [System bootup process](https://www.freedesktop.org/software/systemd/man/bootup.html) 。


### 啟動優化

sysvinit 只能一次一個串行地啟動進程，而 Systemd 則並行地啟動系統服務進程，並且最初僅啟動確實被依賴的那些服務，極大地減少了系統引導的時間。

任何啟動項，只要是在系統啟動時有被執行到，不論啟動成功還是失敗，systemd 都能夠記錄下他們的狀態，可以通過 systemctl 查看當前的服務。

詳細信息可以通過 ```systemctl status systemd-logind.service``` 查看；啟動的結構樹可以通過 ```systemd-cgls``` 查看。

{% highlight text %}
# systemd-analyze                         ← 查看系統引導用時
# systemd-analyze time                    ← 同上
# systemd-analyze blame                   ← 查看初始化任務所消耗的時間
# systemd-analyze plot > systemd.svg      ← 將啟動過程輸出為svg圖
# systemd-cgtop                           ← 查看資源的消耗狀態
{% endhighlight %}

### 常用 systemctl 命令

通過 systemctl 命令可以管理整個系統。

{% highlight text %}
----- 查看、設置、取消開機啟動
# systemctl is-enabled nginx
# systemctl enable nginx
# systemctl disable nginx

----- 啟動、停止、kill、重啟、查看服務狀態
# systemctl start nginx
# systemctl stop nginx
# systemctl kill nginx
# systemctl restart nginx
$ systemctl status nginx

----- 修改service之後重新加載
# systemctl daemon-reload

----- 殺死一個服務的所有進程，傳遞信號到指定服務的所有進程
# systemctl kill crond.service
----- 指定信號類型，如下兩者相同，所有fork的進程都會被殺死
# systemctl kill -s SIGKILL crond.service
# systemctl kill -s 9 crond.service
----- 發送指定信號到服務的主進程
# systemctl kill -s HUP --kill-who=main crond.service

----- 其它操作
# systemctl                               ← 列出正在運行的單元
# systemctl list-units                    ← 同上
# systemctl list-units --type service     ← 同上，只是以service為單位
# systemctl --failed                      ← 查看失敗的任務
# systemctl list-unit-files               ← 所有已經安裝的任務
# systemctl list-dependencies nginx       ← 查看特定服務的依賴關係
{% endhighlight %}

所有可用的單元文件存放在 ```/usr/lib/systemd/system``` 和 ```/etc/systemd/system``` 目錄中，一個單元配置文件可以為，系統服務(.service) 、掛載點(.mount)、sockets(.sockets)、系統設備、交換分區/文件、啟動目標(target)、文件系統路徑、由 systemd 管理的計時器，詳見 ```man 5 systemd.unit``` 。

通過enable 設置為開機啟動時，相當於在 ```/etc/systemd/system/default.target``` 符號鏈接指向的目標對應目錄下添加指向 nginx 的符號鏈接。

### 設置啟動級別

在 sysVinit 的 runlevels 大多是以數字分級的，常用的命令如下。

{% highlight text %}
# systemctl isolate graphical.target       // 改變當前目標
# systemctl list-units --type=target       // 列出當前目標
# systemctl get-default                    // 查看默認目標
# systemctl set-default graphical.target   // 改變默認目標
{% endhighlight %}

### 日誌管理

journald 是 systemd 獨有的日誌系統，替換了 sysVinit 中的 syslog 守護進程，通過命令 journalctl 讀取日誌。

Systemd 統一管理所有 Unit 的啟動日誌，好處是，只需要 journalctl 一個命令，就可以查看所有日誌 (內核日誌和應用日誌) 的內容；配置文件為 /etc/systemd/journald.conf 。

{% highlight text %}
----- 查看某個Unit的日誌
# journalctl -u nginx.service
# journalctl -u nginx.service --since today

----- 實時滾動顯示某個Unit的最新日誌
# journalctl -u nginx.service -f

# 合併顯示多個 Unit 的日誌
$ journalctl -u nginx.service -u php-fpm.service --since today
{% endhighlight %}

更多內容可以查看 [How To Use Journalctl to View and Manipulate Systemd Logs](https://www.digitalocean.com/community/tutorials/how-to-use-journalctl-to-view-and-manipulate-systemd-logs) 。

<!--
{% highlight text %}
# journalctl                               // 查看日誌
# journalctl -b                            // 啟動日誌
# journalctl -f                            // 實時顯示系統日誌
# journalctl /usr/sbin/dnsmasq             // 查看特定任務的日誌
{% endhighlight %}
-->

### 電源管理

systemctl 命令也可以用來關機、重啟、掛起、休眠。

{% highlight text %}
# systemctl poweroff
# systemctl reboot
# systemctl suspend
# systemctl hibernate
{% endhighlight %}

### 時區設置

systemd 提供了一個 timedatectl 命令行，可用於配置時區信息。

{% highlight text %}
----- 查看當前所支持的時區信息
$ timedatectl list-timezones
----- 選擇上述中的時區，然後設置
# timedatectl set-timezone zone
----- 查看當前時區設置的狀態
# timedatectl status
{% endhighlight %}

## 管理目標

服務 systemctl 腳本存放在 ```/usr/lib/systemd/``` 目錄下，有系統 (system) 和用戶 (user) 之分，前者開機後無需登錄即可運行，後者則需要在用戶登錄後才能運行程序。

常見的服務如 nginx 等存放在 ```/usr/lib/systemd/system``` 目錄下；下面以 nginx 為例，編寫腳本時可以直接參考 nginx 的編寫方法。

{% highlight text %}
[Unit]
Description=The nginx HTTP and reverse proxy server
After=network.target remote-fs.target nss-lookup.target

[Service]
Type=forking
PIDFile=/run/nginx.pid
# Nginx will fail to start if /run/nginx.pid already exists but has the wrong
# SELinux context. This might happen when running `nginx -t` from the cmdline.
# https://bugzilla.redhat.com/show_bug.cgi?id=1268621
ExecStartPre=/usr/bin/rm -f /run/nginx.pid
ExecStartPre=/usr/sbin/nginx -t
ExecStart=/usr/sbin/nginx
ExecReload=/bin/kill -s HUP $MAINPID
KillMode=process
KillSignal=SIGQUIT
TimeoutStopSec=5
PrivateTmp=true

[Install]
WantedBy=multi-user.target
{% endhighlight %}

一個服務以 .service 結尾，一般會分為 3 部分：```[Unit]```、```[Service]``` 和 ```[Install]``` 。

服務腳本按照上面編寫完成後，以 754 的權限保存在 ```/usr/lib/systemd/system``` 目錄下，這時就可以利用 ```systemctl``` 進行配置了。

### Unit

設置描述、幫助文檔、啟動順序以及服務的啟動依賴條件等，如下是 SSHD 服務的。

{% highlight text %}
Description=OpenSSH server daemon
Documentation=man:sshd(8) man:sshd_config(5)
After=network.target sshd-keygen.service
Wants=sshd-keygen.service
{% endhighlight %}

```After``` 和 ```Before``` 分別設置在哪些服務之後或者之前啟動，用於配置各個服務的啟動順序，注意，這裡是啟動順序而非依賴關係。

通過 ```Wants``` 和 ```Requires``` 設置弱依賴和強依賴關係，前者表示依賴的服務啟停不會影響當前服務，或者表示如果依賴退出，那麼該服務同時退出。

### Service

用於配置如何啟動服務；注意，命令應該使用絕對路徑。

<!--
#  forking：ExecStart字段將以fork()方式啟動，此時父進程將會退出，子進程將成為主進程
#  oneshot：類似於simple，但只執行一次，Systemd 會等它執行完，才啟動其他服務
#  dbus：類似於simple，但會等待 D-Bus 信號後啟動
#  idle：類似於simple，但是要等到其他任務都執行完，才會啟動該服務。一種使用場合是為讓該服務的輸出，不與其他服務的輸出相混合
-->

{% highlight bash %}
User=nginx
Group=nginx
PIDFile=/run/nginx.pid

Type=forking                               # 定義啟動類型
#  simple : 默認值，通過ExecStart字段啟動進程
#  notify : 類似於simple，服務啟動結束後會發出通知信號，然後Systemd再啟動其他服務

EnvironmentFile=-/etc/sysconfig/nginx      # 依賴環境，可以指定多個
EnvironmentFile=-/etc/default/nginx

ExecStartPre=/usr/bin/rm -f /run/nginx.pid # 啟動服務之前執行的命令
ExecStartPre=/usr/sbin/nginx -t
ExecStart=/usr/sbin/nginx                  # 啟動時，多個會被最後一個覆蓋
ExecStartPost=                             # 啟動服務之後執行的命令
ExecReload=/bin/kill -s HUP $MAINPID       # 重啟服務時執行的命令
ExecStop=                                  # 停止服務時執行的命令
ExecStopPost=                              # 停止服務之後執行的命令
TimeoutStopSec=5                           # 設置停止超時時間

KillMode=process                           # 重啟行為配置，詳見如下介紹
#  control-group : 默認值，當前控制組裡面的所有子進程，都會被殺掉
#  process       : 只殺主進程，信號可以通過如下方式定義
#  mixed         : 主進程將收到SIGTERM信號，子進程收到SIGKILL信號
#  none          : 沒有進程會被殺掉，只是執行服務的stop命令
KillSignal=SIGQUIT

Restart=on-failure                         # 意外失敗後重啟方式，正常停止不重啟
#  no          : 默認值，退出後不重啟
#  on-success  : 只有正常退出時(退出狀態碼為0)，才會重啟
#  on-failure  : 非正常退出時 (退出狀態碼非0)，包括被信號終止和超時，才會重啟
#  on-abnormal : 只有被信號終止和超時，才會重啟
#  on-abort    : 只有在收到沒有捕捉到的信號終止時，才會重啟
#  on-watchdog : 超時退出，才會重啟
#  always      : 不管是什麼退出原因，總是重啟
RestartSec=10                              # 重啟服務之前，需要等待的秒數，默認100ms

PrivateTmp=True                            # 給服務分配獨立的臨時空間
{% endhighlight %}

在所有的服務配置之前，都可以加上一個連詞號 (```-```)，表示 "抑制錯誤"，也即即使發生錯誤也不影響其他命令的執行。

### Install

在通過 enable 設置為開機啟動時，添加到那個 target 裡面，也即定義如何安裝這個配置文件，即怎樣做到開機啟動。

### 資源隔離

相關內容可以查看 [容器之 CGroup](/post/linux-container-cgroup-introduce.html) 。


## 其它

systemd 帶來了一整套與操作系統交互的新途徑，如可以用 hostnamectl 獲得機器的 hostname 和其它有用的獨特信息。

{% highlight text %}
# hostnamectl                              // 查看機器信息
{% endhighlight %}

除了 restart 命令，也可以使用 try-start 選項，它只會在服務已經在運行中的時候重啟服務。


<!--

在sysvinit的時代，如果需要結束一個服務及其啟動的所有進程，可能會遇到一些糟糕的進程無法正確結束掉，即便是我們使用kill，killall等命令來結束它們，到了systemd的時代一切都變得不一樣，systemd號稱是第一個能正確的終結一項服務的程序，下面來看看具體如何操作的：
# systemctl kill crond.service
或者指定一個信號發送出去
# systemctl kill -s SIGKILL crond.service
例如可以像這樣執行一次reload操作
# systemctl kill -s HUP --kill-who=main crond.service

http://www.csdn.net/article/2015-01-08/2823477/1<br><br>
http://www.linuxidc.com/Linux/2014-11/110023.htm<br><br>
http://www.linuxidc.com/Linux/2014-12/110777.htm<br><br>
http://www.ithov.com/linux/136324.shtml  比較詳細介紹了原理

首先，使用systemctl start [服務名（也是文件名）]可測試服務是否可以成功運行，如果不能運行則可以使用systemctl status [服務名（也是文件名）]查看錯誤信息和其他服務信息，然後根據報錯進行修改，直到可以start，如果不放心還可以測試restart和stop命令。

接著，只要使用systemctl enable xxxxx就可以將所編寫的服務添加至開機啟動即可。

這樣看來，雖然systemctl比較陌生，但是其實比init.d那種方式簡單不少，而且使用簡單，systemctl能簡化的操作還有很多，現在也有不少的資料，看來RHEL/CentOS比其他的Linux發行版還是比較先進的，此次更新也終於捨棄了Linux 2.6內核，無論是速度還是穩定性都提升不少。

如果使用 systemd 日誌可以直接打印到 STDOUT ，然後由 systemd 統一管理。


bcmp ：比較兩個內存中的內容
bcopy : 複製內存中的內容
bzero ： 將一個內存內容全清零
ffs : 在一個整數中查找第一個值為真的位
index : 查找字符串中第一個出現的指定字符
memccpy ：複製內存中的內容
memchr ：在一塊內存指定範圍內查找一個指定字符
memcmp ：比較內存中存放的內容
memcpy ： 複製一塊內存內容到另一塊
memfrob ： 對某個內存區重新編碼
memmove ： 複製內存內容
memset ：將某值填入到一個內存區域
rindex ：查找字符串中最後一個出現的指定字符
* strcat ：將一個字符串連接到另一個字符串的尾部
* strncat ：將一個字符串的前n個字符連接到另一個字符串的尾部
strchr ；查找字符串中指定字符
strcmp ：比較兩個字符串
strcoll ：根據當前環境信息來比較字符串
strcpy ：複製一個字符串的內容到另一個字符串中
strdup ：複製字符串的內容
strfry ：隨機重組一個字符串
strlen ： 返回字符串的長度
* strncasecmp ：忽略大小寫比較兩個字符串
* strcasecmp ：忽略大小寫比較字符串
strncmp ：比較字符串前n個字符
strncpy ： 複製字符串的前n個字符
strpbrk ：查找字符串中第一個出現的指定字符
strrchr ：查找字符串中最後一個出現的指定字符
strspn ：計算字符串中由指定字符集字符組成的子字符串的長度
strcspn ：計算字符串中由非指定字符集字符組成的子字符串的長度

https://github.com/jnavila/memtester


    # 查看所有日誌（默認情況下 ，只保存本次啟動的日誌）
    $ sudo journalctl

    # 查看內核日誌（不顯示應用日誌）
    $ sudo journalctl -k

    # 查看系統本次啟動的日誌
    $ sudo journalctl -b
    $ sudo journalctl -b -0

    # 查看上一次啟動的日誌（需更改設置）
    $ sudo journalctl -b -1

    # 查看指定時間的日誌
    $ sudo journalctl --since="2012-10-30 18:17:16"
    $ sudo journalctl --since "20 min ago"
    $ sudo journalctl --since yesterday
    $ sudo journalctl --since "2015-01-10" --until "2015-01-11 03:00"
    $ sudo journalctl --since 09:00 --until "1 hour ago"

    # 顯示尾部的最新10行日誌
    $ sudo journalctl -n

    # 顯示尾部指定行數的日誌
    $ sudo journalctl -n 20

    # 實時滾動顯示最新日誌
    $ sudo journalctl -f

    # 查看指定服務的日誌
    $ sudo journalctl /usr/lib/systemd/systemd

    # 查看指定進程的日誌
    $ sudo journalctl _PID=1

    # 查看某個路徑的腳本的日誌
    $ sudo journalctl /usr/bin/bash

    # 查看指定用戶的日誌
    $ sudo journalctl _UID=33 --since today



    # 查看指定優先級（及其以上級別）的日誌，共有8級
    # 0: emerg
    # 1: alert
    # 2: crit
    # 3: err
    # 4: warning
    # 5: notice
    # 6: info
    # 7: debug
    $ sudo journalctl -p err -b

    # 日誌默認分頁輸出，--no-pager 改為正常的標準輸出
    $ sudo journalctl --no-pager

    # 以 JSON 格式（單行）輸出
    $ sudo journalctl -b -u nginx.service -o json

    # 以 JSON 格式（多行）輸出，可讀性更好
    $ sudo journalctl -b -u nginx.serviceqq
     -o json-pretty

    # 顯示日誌佔據的硬盤空間
    $ sudo journalctl --disk-usage

    # 指定日誌文件佔據的最大空間
    $ sudo journalctl --vacuum-size=1G

    # 指定日誌文件保存多久
    $ sudo journalctl --vacuum-time=1years
-->


### 自動登陸

在此查看下如何自動登陸，首先創建一個新的類似於 ```getty@.service``` 的服務。

{% highlight text %}
# cp /lib/systemd/system/getty@.service /etc/systemd/system/autologin@.service
# ln -s /etc/systemd/system/autologin@.service /etc/systemd/system/getty.target.wants/getty@tty8.service
# vi /etc/systemd/system/getty.target.wants/getty@tty8.service
...
ExecStart=-/sbin/mingetty --autologin USERNAME %I
Restart=no
...
Alias=getty.target.wants/getty@tty8.service
{% endhighlight %}

最後重新加載守護進程，運行服務：

{% highlight text %}
# systemctl daemon-reload
# systemctl start getty@tty8.service
{% endhighlight %}

需要注意的是，如果你退出了 tty8 的會話，你需要等到下次重新啟動才能使用，除非你給 Restart 的值是 ```'always'```，這樣可以使用 systemctl 手動開啟，不過，出於安全考慮，強烈建議不要那麼做。

### 對比

![SystemD VS. SysVinit]({{ site.url }}/images/linux/systemd-sysvinit.jpg "SystemD VS. SysVinit"){: .pull-center width="90%"}

### 雜項

如果重啟過於頻繁會報 "uagent.service start request repeated too quickly, refusing to start."

## 參考

[systemd System and Service Manager](http://www.freedesktop.org/wiki/Software/systemd/)，system daemon 官方網站。



<!--
http://www.ruanyifeng.com/blog/2016/03/systemd-tutorial-commands.html
-->


{% highlight text %}
{% endhighlight %}
