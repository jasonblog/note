---
title: SELinux 簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,selinux
description: SELinux 給 Linux 帶來的最重要價值是：提供了一個靈活的，可配置的 MAC 機制。包括了內核中的模塊，以及用戶態的工具，對於用戶來說是透明的，只有同時滿足了 "標準 Linux 訪問控制" 和 "SELinux 訪問控制" 時，主體才能訪問客體。
---

SELinux 給 Linux 帶來的最重要價值是：提供了一個靈活的，可配置的 MAC 機制。包括了內核中的模塊，以及用戶態的工具，對於用戶來說是透明的，只有同時滿足了 "標準 Linux 訪問控制" 和 "SELinux 訪問控制" 時，主體才能訪問客體。

<!-- more -->

## SELinux (Security-Enhanced Linux)

SELinux 是一種強制存取控制 (Mandatory Access Control, MAC) 的實現，它的作法是以最小權限原則 (Principle of least privilege) 為基礎。最初由美國國家安全局 (Natinal Security Agency) 設計，可以直接參考 [www.nsa.gov](https://www.nsa.gov/research/selinux/) 。

最初的 Linux 採用的是自主式存取控制 (Discretionary Access Control, DAC)，基本上就是根據程序的擁有者與文件資源的 rwx 權限來判斷存取的能力。這種策略存在著一些問題，如 root 可以讀取所有的文檔；如果將權限誤設置為 777，那麼所有的用戶都可以讀取該文件了。

而 MAC 可以依據條件決定是否有存取權限，可以規範個別細緻的項目進行存取控制，提供完整的徹底化規範限制。可以對文件，目錄，網絡，套接字等進行規範，所有動作必須先得到 DAC 授權，然後得到 MAC 授權才可以存取。

### 運行機制

在 SELinux 中，訪問控制屬性叫做安全上下文。所有客體（文件、進程間通訊通道、套接字、網絡主機等）和主體（進程）都有與其關聯的安全上下文，一個安全上下文由三部分組成：用戶、角色和類型標識符。

![selinux decision process]({{ site.url }}/images/linux/selinux-decision-process.png){: .pull-center}

當一個 subject (如一個應用) 試圖訪問一個 object (如一個文件) 時，在 Kernel 中的策略執行服務器將檢查 AVC (Access Vector Cache)。如果基於 AVC 中的數據不能做出決定，則請求安全服務器，根據查詢結果允許或拒絕訪問。

Selinux總共分為三個等級，

* Enforcing<br>默認模式會在系統上啟用並實施 SELinux 的安全性政策，包括拒絕訪問及記錄行動。

* Permissive<br>啟用但不會實施安全性政策，而只會發出警告及記錄行動，在排除錯誤時比較有用。

* Disabled<br>已被停用。

SElinux 主要是用來管理程序，因此其主體 (Subject) 等價於程序；目標 (Object) 主要是指文件系統；政策 (Policy) 設置的基本的安全存儲策略。

### 配置文件

SELinux 配置文件以及策略文件都位於 ```/etc/selinux``` 目錄下，相關的策略可以通過如下方式進行設置。

{% highlight text %}
----- 列出SELinux的所有布爾值
# getsebool -a
----- 設置SELinux布爾值，-P表示持久化，即使reboot之後，仍然有效
# setsebool -P dhcpd_disable_trans=0
{% endhighlight %}

### 查看

通過 ```-Z``` 選項可以查看策略，如 ```ls -Z```、```ps -Z```、```id -Z``` 等，除了常規的參數之外還會有 Security Context ，由 (identify:role:type) 三部分組成；安全性本文 (Security Context) 保存在 inode 中。

![selinux context]({{ site.url }}/images/linux/selinux-context.png){: .pull-center}

字段的含義如下：

* user<br>用戶區域，指的是 selinux 環境下的用戶，與登陸用戶的影射關係可通過 ```semanage login -l``` 查看，常有 user_u(登陸用戶)；system_u(Linux啟動過程中默認值)；root(也就是root用戶)。

* role<br>通常由於安全類型的分組，常見的有object_r(通常為文件，實際只是一個佔位符)。

* type<br>與role類型實際決定了那些角色可以執行那些類型，這兩種類型實際決定了那些。

* level<br>控制級別，Multi-Category Security(MCS)。

### 常見操作

SELinux 缺省會通過 Linux 審計系統 auditd 將日誌寫在 ```/var/log/audit/audit.log``` 內。

{% highlight text %}
----- 查看SELinux狀態
# sestatus -v                 # 如果SELinux status參數為enabled即為開啟狀態
# getenforce                  # 也可以用這個命令檢查

----- 關閉SELinux
# setenforce 0                # 設置SELinux 成為permissive(0)或者enforcing(1)模式

$ cat /etc/selinux/config     # 可以直接設置配置文件
SELINUX=enforcing/disabled
{% endhighlight %}

當由 Diabled 切換至 Permissive 或 Enforcing 模式時，最好重啟，重新標籤文件系統。SELinux 包含了很多策略軟件包 (policy package)，後綴名為 .pp，一般保存在 ```/etc/selinux``` 目錄下，可以通過 find 命令查找。

出現權限問題後可以通過 setroubleshoot 工具包。

{% highlight text %}
$ echo > /var/log/audit/audit.log                        # 清空，防止有過多的信息
$ sealert -a /var/log/audit/audit.log > /tmp/result.txt  # 輸出信息
{% endhighlight %}

其中的結果包括了建議執行的命令。


## 配置實例

### Apache 服務標準配置修改

主要查看下如何通過 SELinux 修改 Apache 的權限配置。

#### 1. 讓 Apache 可以訪問非默認目錄

首先，獲取默認 ```/var/www``` 目錄的 SELinux 上下文。

{% highlight text %}
# semanage fcontext -l | grep '/var/www'
/var/www(/.*)?          all     files   system_u:object_r:httpd_sys_content_t:s0
{% endhighlight %}

也就是 Apache 只能訪問包含 ```httpd_sys_content_t``` 標籤的文件，假設希望其使用 ```/opt/www``` 作為網站文件目錄，就需要給這個目錄下的文件增加 ```httpd_sys_content_t``` 標籤，分兩步實現。

{% highlight text %}
----- 1. 為/opt/www目錄下的文件添加默認標籤類型
# semanage fcontext -a -t httpd_sys_content_t '/srv/www(/.*)?'

----- 2. 用新的標籤類型標註已有文件
# restorecon -Rv /srv/www
{% endhighlight %}

其中 restorecon 命令用於恢復文件默認標籤，比較常用，比如從用戶主目錄下將某個文件複製到 Apache 網站目錄下，Apache 默認是無法訪問，因為用戶主目錄的下的文件標籤是 ```user_home_t```，此時就需要 restorecon 將其恢復為可被 Apache 訪問的 ```httpd_sys_content_t``` 類型。

#### 2. 讓 Apache 偵聽非標準端口

默認情況下 Apache 只偵聽 80 和 443 兩個端口，若是直接指定其偵聽 888 端口的話，會在通過 systemclt 啟動或者重起時報 Permission denied 錯誤。

這個時候，若是在桌面環境下 SELinux 故障排除工具應該已經彈出來報錯了。若是在終端下，可以通過查看 ```/var/log/{messages, audit/audit.log}``` 日誌，用 ```sealert -l``` 加編號的方式查看，或者直接使用 ```sealert -b``` 瀏覽。

<!--
可以看出 SELinux 根據三種不同情況分別給出了對應的解決方法。在這裡，第一種情況是我們想要的，於是按照其建議輸入：

semanage port -a -t http_port_t -p tcp 888

之後再次啟動 Apache 服務就不會有問題了。

這裡又可以見到 semanage 這個 SELinux 管理配置工具。它第一個選項代表要更改的類型，然後緊跟所要進行操作。詳細內容參考 Man 手冊
-->

#### 3. 允許 Apache 訪問創建私人網站

若是希望用戶可以通過在 ```~/public_html/``` 放置文件的方式創建自己的個人網站的話，那麼需要在 Apache 策略中允許該操作執行。使用：

{% highlight text %}
# setsebool httpd_enable_homedirs 1
{% endhighlight %}

setsebool 用來切換由布爾值控制的 SELinux 策略的，當前布爾值策略的狀態可以通過 getsebool 來獲知。

<!--
默認情況下 setsebool 的設置只保留到下一次重啟之前，若是想永久生效的話，需要添加 -P 參數，比如：
setsebool -P httpd_enable_homedirs 1
-->

## capabilities

通常 capabilities 是和 SELinux 配合使用的，以往，如果要運行一個需要 root 權限的程序，那麼需要保證有運行權限，且是 root 用戶；通過 capabilities 就可以只賦予程序所需要的權限。

以 ping 命令為例，因為需要發送 raw 格式數據，部分發行版本使用了 ```setuid + root```，實際上只需要賦予 ```CAP_NET_RAW``` 權限，然後去除 setuid 即可。

{% highlight text %}
----- 直接複製一個ping命令，然後進行測試
# cp ping anotherping
# chcon -t ping_exec_t anotherping
$ ping -c 1 127.0.0.1
PING 127.0.0.1 (127.0.0.1) 56(84) bytes of data.
64 bytes from 127.0.0.1: icmp_seq=1 ttl=64 time=0.057 ms
$ anotherping -c 1 127.0.0.1
ping: icmp open socket: Operation not permitted

----- 新增CAP_NET_RAW權限，然後用非root用戶重新測試
# setcap cap_net_raw+ep anotherping
$ anotherping -c 1 127.0.0.1
PING 127.0.0.1 (127.0.0.1) 56(84) bytes of data.
64 bytes from 127.0.0.1: icmp_seq=1 ttl=64 time=0.054 ms
{% endhighlight %}

## 參考


<!--
chcon 用於修改文件的 SELinux 安全上下文，或者說是修改安全策略。

進程所屬的上下文類型稱為域，而文件所屬的上下文類型稱為類型，在 SELinux 開啟後，進程只能操作域類型與上下文類型一樣的文件。

在 CentOS 中，semanage 命令默認沒有安裝，可以通過 ```yum install policycoreutils-python``` 安裝。

----- 查看哪些服務受SELinux管理，也就是布爾值；包括瞭如何設置，-P永久生效
# getsebool -a
# setsebool samba_enable_home_dirs=1
# setsebool samba_enable_home_dirs=on
----- 恢復默認上下文
# restorecon /var/www/html
----- 默認上下文類型？保存在那個文件中
# semanage fcontext -l
----- 用戶映射關係
# semanage user -l
----- 查看受SELinux控制的端口，以及將指定端口添加到規則中
# semanage port -l
# semanage port -a -t http_port_t -p tcp 8060

chcon [選項]... 環境 文件...
chcon [選項]... [-u 用戶] [-r 角色] [-l 範圍] [-t 類型] 文件...
chcon [選項]... --reference=參考文件 文件...

二、chcon命令參數：
參數名 描述
-u 用戶
--user=用戶 設置指定用戶的目標安全環境；
 -r 角色
--role=角色 設置指定角色的目標安全環境；
-t 類型
--type=類型 設置指定類型的目標安全環境；
-l 範圍
--range=範圍 設置指定範圍的目標安全環境；
-v
--verbose 處理的所有文件時顯示診斷信息；

-R/--recursive
  遞歸處理目錄、文件；


-h
--no-dereference 影響符號連接而非引用的文件；
--reference=file 使用指定參考文件file的安全環境，而非指定值；
-H  如果一個命令行參數是一個目錄的符號鏈接，則遍歷它；
-L 遍歷每一個符號連接指向的目錄；
-P 不遍歷任何符號鏈接（默認）；
--help 顯示此幫助信息並退出；
--version 顯示版本信息並退出；

三、chcon用法演示：
1、mysql：讓selinux

允許mysqld做為數據目錄訪問“/storage/mysql”：
-----
# chcon -R -t mysqld_db_t /storage/mysql
2、ftp：將共享目錄開放給匿名用戶：
[root@aiezu.com ~]# chcon -R -t public_content_t /storage/ftp/
#允許匿名用戶上傳文件：
[root@aiezu.com ~]# chcon -R -t public_content_rw_t /storage/ftp
[root@aiezu.com ~]# setsebool -P allow_ftpd_anon_write=1

3、為網站目錄開放httpd訪問權限：
chcon -R -t httpd_sys_content_t /storage/web

https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/6/html/Security-Enhanced_Linux/chap-Security-Enhanced_Linux-SELinux_Contexts.html
https://debian-handbook.info/browse/zh-CN/stable/sect.selinux.html
https://wiki.centos.org/zh/HowTos/SELinux
http://blog.siphos.be/2013/05/overview-of-linux-capabilities-part-1/
http://www.cis.syr.edu/~wedu/seed/Labs/Documentation/Linux/How_Linux_Capability_Works.pdf
-->


{% highlight text %}
{% endhighlight %}
