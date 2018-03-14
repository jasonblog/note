---
title: SSH 簡介
layout: post
comments: true
category: [linux]
language: chinese
keywords: ssh,openssh
description: OpenSSH 是 SSH (Secure SHell) 協議的免費開源實現，SSH 協議族可以用來進行遠程控制，或在計算機之間傳送文件。傳統方式，如 telnet、rcp ftp、rlogin、rsh 都是極不安全的，並且會使用明文傳送密碼，OpenSSH 提供了服務端後臺程序和客戶端工具，用來加密遠程控件和文件傳輸過程中的數據，並由此來代替原來的類似服務。在此，簡單介紹下 OpenSSH 。
---

OpenSSH 是 SSH (Secure SHell) 協議的免費開源實現，SSH 協議族可以用來進行遠程控制，或在計算機之間傳送文件。

傳統方式，如 telnet、rcp ftp、rlogin、rsh 都是極不安全的，並且會使用明文傳送密碼，OpenSSH 提供了服務端後臺程序和客戶端工具，用來加密遠程控件和文件傳輸過程中的數據，並由此來代替原來的類似服務。

在此，簡單介紹下 OpenSSH 。

<!-- more -->

![OpenSSH Logo]({{ site.url }}/images/linux/ssh_logo.png "OpenSSH Logo"){: .pull-center}

## 簡介

SSH 採用的是公私鑰加密方式，利用公鑰進行加密，通過私鑰解密，公鑰任何人都可以看到，而私鑰是私有的。目前 ssh 有 version1 和 version2 兩個版本， version2 加入了聯機檢測功能，可以避免聯機過程中插入惡意攻擊代碼。

OpenSSH Server 在首次執行時會在 /etc/ssh/ 目錄下產生公鑰和私鑰，通常為 ssh_host_* 。如果想要產生新的密鑰可以刪除 ssh_host_* ，然後重啟服務即可。

客戶端會將已知服務器的公鑰保存在 ~/.ssh/known_hosts 中，當登入遠程服務器時，本地主機會主動的用接收到的服務器的 public key 去比對 ~/.ssh/known_hosts 有無相關的公鑰，若接收的公鑰尚未記錄，則詢問用戶是否記錄。若接收到的公鑰已有記錄，則比對記錄是否相同，若相同則繼續登入動作；若不相同，則出現警告信息, 且離開登入的動作。


![SSH Tunneling Sock]({{ site.url }}/images/linux/ssh_how_ssh_works.gif){: .pull-center}

服務器工作過程。

1. 服務器建立公鑰文件。<br>
每次啟動 sshd 服務時，服務器主動尋找 /etc/ssh/ssh_host* 文件，如果沒有則 sshd 會計算出這些文件，包括服務器自己的私鑰文件。

2. 客戶端主動連接請求。<br>
客戶端通過 ssh 等軟件來連接 SSH 服務器。

3. 服務器將公鑰傳遞給客戶端。<br>
接收到用戶端的請求後，服務器的第一步是取得公鑰文件傳送給客戶端，此時是明碼傳輸。

4. 客戶端記錄/比對服務器的公鑰，並隨機計算自己的公私鑰。<br>
本地主機主動用接收到的服務器的 public key 去比對 ~/.ssh/known_hosts 有無相關的公鑰。<br>
A) 若接收的公鑰尚未記錄，則詢問用戶是否記錄。若要記錄 (回答 yes) 則寫入 ~/.ssh/known_hosts 且繼續登入的後續工作；若不記錄 (回答 no) 則不寫入該檔案，並且離開登入工作。<br>
B) 若接收到的公鑰已有記錄，則比對記錄是否相同，若相同則繼續登入動作；若不相同，則出現警告信息, 且離開登入的動作。 這是客戶端的自我保護功能，避免你的服務器是被別人偽裝的。如果是在測試主機，那麼重新安裝時服務器的公鑰會經常改變，此時會無法正常登入。如果服務器的 Public key 改變，那麼，直接刪除 ~/.ssh/known_hosts 中對應的記錄即可。

5. 回傳用戶端的公鑰資料到服務器端。<br>
用戶將自己的公鑰傳送給服務器，此時服務器具有服務器的私鑰和用戶端的公鑰；而用戶端則有服務器的公鑰以及用戶端的自己的私鑰。

6. 開始雙向加/解密。<br>
服務器到用戶端：服務器在傳送資料時，使用用戶端的公鑰加密後傳送出去。用戶端接收後用自己的私鑰解密。<br>
用戶端到服務器：用戶端在傳送資料時，使用服務器的公鑰加密後傳送出去。服務器接收後用自己的私鑰解密。

~/.ssh/known_hosts 存在於客戶端，用於記錄已經通過驗證的服務器。在第一次登錄的時候，會提示是否將服務器添加到該文件中，以後再登錄時，會利用該文件檢測是否是之前的服務器，從而可以防止“中間人攻擊”。

~/.ssh/authorized_keys 是主機保存了用戶的公鑰，保存在登錄後的用戶主目錄下。正常情況下服務器不會主動保存客戶端的公鑰，需要用戶手動設置，通常用於“無密碼登錄”。

## 安裝使用

OpenSSH 不僅提供了一個 shell，而且還提供了一個較為安全的 ftp-server，也就是客戶端和服務器。

{% highlight text %}
# dpkg --list | grep ssh                                  # Ubuntu中查看/安裝OpenSSH Server
# apt-get install openssh-server

# yum install openssh-server openssh-clients openssh      # CentOS中安裝
{% endhighlight %}

Ubutnu 中安裝之後會包括 /etc/init.d/ssh 文件，在 Ubuntu 中可以通過如下方式啟動/停止/重啟 Openssh Server 。

{% highlight text %}
# /etc/init.d/ssh [start | stop | restart]
# service ssh [start | stop | restart]
{% endhighlight %}

通過如下命令可以查看服務是否已經啟動，其中 ssh 使用的是 22 號端口，使用 tcp 協議， ftp 同樣使用 22 號端口。

{% highlight text %}
# netstat -vatn | grep 22
{% endhighlight %}

在 CentOS7 中，可以通過如下命令啟動。

{% highlight text %}
# systemctl start sshd.service
{% endhighlight %}

## 客戶端使用

正常情況下在安裝完服務器後，可以通過如下命令進行簡單的登錄，如果遠程主機的用戶名和本地用戶名一致可以省略 user 。

{% highlight text %}
$ ssh user@host-name
{% endhighlight %}

關於 ssh 客戶端的詳細參數可以參考 man 命令，常用的參數有：

* -f :需要配合後面的 [command] , 不登入遠程主機直接發送一個指令過去而已。

* -o option:主要的參數項目有：<br>
    ConnectTimeout=秒數 : 聯機等待的秒數,減少等待的時間；<br>
    StrictHostKeyChecking=[yes|no|ask]:預設是 ask 若要讓 public key 主動加入 known_hosts，則可以設定為 no 即可；

* -p :如果你的 sshd 服務啟動在非正規的端口 (22),需使用此選項。

### 在服務端執行命令

登入對方主機執行過指令後立刻離開。
{% highlight text %}
$ ssh student@127.0.0.1 find / & > ~/find1.log
{% endhighlight %}
此時你會發現怎麼畫面卡住了，這是因為上頭的指令會造成，已經登入遠程主機，但是執行的指令尚未跑完，因此會在等待當中。

與上相似，但是讓對方主機自己跑該指令，立刻回到本地主機繼續工作。

{% highlight text %}
$ ssh -f student@127.0.0.1 find / &> ~/find1.log
{% endhighlight %}

此時你會立刻註銷 127.0.0.1 ，但 find 指令會自己在遠程服務器跑喔!


## 配置文件

默認包括瞭如下的配置文件：

* /etc/ssh/sshd_config ： OpenSSH 服務器配置文件。

* /etc/ssh/ssh_config ： OpenSSH 客戶端全局配置文件。

* ~/.ssh/authorized_keys ：已經授權的可以直接登陸的公鑰。

### 整體配置

{% highlight text %}
Protocol 2,1    # SSH協議版本，1可能會有中間人攻擊，建議只配置為2
{% endhighlight %}

### 安全配置



#### 登入設定部分

{% highlight text %}
PermitRootLogin yes                      # 是否允許root登入
StrictModes yes                          # 嚴格檢查目錄權限，權限設置出錯則不允許登陸
PubkeyAuthentication yes                 # 允許公鑰登陸
AuthorizedKeysFile .ssh/authorized_keys  # 公鑰保存文件
HostbasedAuthentication no               # 取消基於host的授權
PasswordAuthentication yes               # 使用密碼驗證
PermitEmptyPasswords no                  # 是否允許以空的密碼登入
AllowUsers list                          # 允許登陸的用戶，用空格分開，可以用*?通配符
DenyUsers foobar                         # 禁止某一個用戶登入
AllowGroups nosh                         # 允許某一個組登入
DenyGroups nosh                          # 禁止某一個組登入
{% endhighlight %}


#### 登入後設置


{% highlight text %}
ClientAliveInterval 300        # 若300秒內沒有收到客戶端消息，則發送心跳報文
ClientAliveCountMax 0          # 客戶端斷開連接之前，能發送保活信息的條數，此時踢出用戶
{% endhighlight %}

#### 安全設置

{% highlight text %}
IgnoreRhosts yes              # 取消使用~/.ssh/.rhosts來做為認證
{% endhighlight %}


#### 其它

{% highlight text %}
UseDNS yes                    # 關閉DNS驗證，詳見後面的介紹
{% endhighlight %}




<!--

You need to firewall ssh port # 22 by updating iptables or pf firewall configurations. Usually, OpenSSH server must only accept connections from your LAN or other remote WAN sites only.
Netfilter (Iptables) Configuration

Update /etc/sysconfig/iptables (Redhat and friends specific file) to accept connection only from 192.168.1.0/24 and 202.54.1.5/29, enter:

-A RH-Firewall-1-INPUT -s 192.168.1.0/24 -m state --state NEW -p tcp --dport 22 -j ACCEPT
-A RH-Firewall-1-INPUT -s 202.54.1.5/29 -m state --state NEW -p tcp --dport 22 -j ACCEPT

If you’ve dual stacked sshd with IPv6, edit /etc/sysconfig/ip6tables (Redhat and friends specific file), enter:

 -A RH-Firewall-1-INPUT -s ipv6network::/ipv6mask -m tcp -p tcp --dport 22 -j ACCEPT

 Replace ipv6network::/ipv6mask with actual IPv6 ranges.
-->





## 雜項

#### 客戶端心跳報文

{% highlight text %}
SecureCRT:
   Properties->Terminal->Anti-dile
   設置每隔80秒發送一個字符串或是NO-OP協議包
Putty:
   Properties->Connection->Seconds between keepalives(0 to turn off)
   默認為0，改為80
Xshell:
   File->Properties->Connection->Keep Alive
   設置為80s
Linux:
   在/etc/ssh/ssh_config添加ServerAliveInterval 80
{% endhighlight %}

#### 關閉DNS檢查

OpenSSH 服務器有一個 DNS 查找選項 UseDNS 默認是打開的。

當客戶端試圖登錄 OpenSSH 服務器時，服務器端先根據客戶端的 IP 地址進行 DNS PTR 反向查詢，查詢出客戶端的 host name，然後根據查詢出的客戶端 host name 進行 DNS 正向 A 記錄查詢，驗證與其原始 IP 地址是否一致，這是防止客戶端欺騙的一種手段，但一般 IP 是動態的，不會有 PTR 記錄的，打開這個選項不過是在白白浪費時間而已。

#### 目錄權限設置

ssh 對目錄的權限有要求，代碼中要設置下新生成的 config 文件權限才行。~ 目錄權限是750，~/.ssh 的是700， ~/.ssh/* 的是600，~/.ssh/config 是700


#### 減少延遲

如果每次連接服務器都意味著你需要等待幾十秒而無所事事，那麼你或許應該試試在你的 SSH 配置中加入下面這條，這一配置可以在客戶端或者服務端配置，或者在 ssh 登錄時加上 "-o GSSAPIAuthentication=no" 。

{% highlight text %}
GSSAPIAuthentication no
{% endhighlight %}

GSSAPI 是 Generic Security Services Application Program Interface (通用安全服務應用程序接口) 是 ITEF 的一個標準，允許不同的安全算法使用一個標準化的 API 來為網絡產品提供加密認證。

OpenSSH 使用這個 API 並且底層的 kerberos 5 協議代碼提供除了 ssh_keys 的另一種 ssh 認證方式。



<!--
### 加速連接

　　如果你確保你和某個服務器之間的連接是安全的（比如通過公司內網連接），那麼你就可以通過選擇arcfourencryption算法來讓數據傳輸更快一些：

Host dev
Ciphers arcfour

　　注意這個加速是以犧牲數據的加密性為代價的，所以如果你連接的是位於網上的服務器，千萬不要打開這個選項，並且確保你是通過VPN建立的連接。
-->



<!--
tar -cvf - /bin | ssh id@hostname "tar -xvf -"
從服務器複製到其它地方


當主機的私鑰修改後，可以通過 ssh-keyscan 進行掃描，列出相應主機對應的公鑰信息，其中參數中的 host 信息可以查看 known_hosts 中每行的第一列。

f=$(mktemp) && ssh-keyscan korell > $f && ssh-keygen -l -f $f && rm $f

ssh-agent 會通過一個後臺程序，將私鑰信息保存在內存中，需要時發送給相應的程序。

當 ssh-agent 啟動之後，會自動更新 SSH_AUTH_SOCK 和 SSH_AGENT_PID 環境變量，用來供其它程序使用

介紹最佳實踐方式，包括了一個漏洞的鏈接
https://blog.0xbadc0de.be/archives/300



<h2>中間人攻擊</h2>
<p>
	在進行登錄時，存在風險：如果有人截獲了登錄請求，然後冒充遠程主機，將偽造的公鑰發給用戶，那麼用戶很難辨別真偽。因為不像 https 協議， SSH 協議的公鑰是沒有證書中心 （CA） 公證的，也就是說，都是自己簽發的。<br><br>

	如果攻擊者在用戶與遠程主機之間（如公共的 wifi 區域），用偽造的公鑰，獲取用戶的登錄密碼。再用這個密碼登錄遠程主機，那麼 SSH 的安全機制就蕩然無存了。這種風險就是著名的 "中間人攻擊" （Man-in-the-middle attack）。<br><br>

	為了解決這一問題，如果你是第一次登錄對方主機，系統會出現類似下面的提示：
<pre>
$ ssh user@host
The authenticity of host 'host (12.18.429.21)' can't be established.
RSA key fingerprint is 98:2e:d7:e0:de:9f:ac:67:28:c2:42:2d:37:16:58:4d.
Are you sure you want to continue connecting (yes/no)?</pre>
	所謂 "公鑰指紋" ，是指公鑰長度較長（這裡採用 RSA 算法，長達 1024 位），很難比對，所以對其進行 MD5 計算，將它變成一個 128 位的指紋。上例中是 98:2e:d7:e0:de:9f:ac:67:28:c2:42:2d:37:16:58:4d ，再進行比較，就容易多了。<br><br>


	通常我們可以通過如下的方法來獲取公鑰對指紋。<ol><li>
		最簡單的方法是遠程主機在自己的網站上貼出公鑰指紋，以便用戶自行核對。</li><br><li>

		可以直接通過網絡從遠程服務器上獲取公共密鑰，然後通過這個文件生成指紋。
		<pre>$ ssh-keyscan -p 22 -t rsa,dsa REMOTE_HOST > /tmp/ssh_host_rsa_dsa_key.pub<br>$ ssh-keygen -l -f /tmp/ssh_host_rsa_dsa_key.pub</pre></li><br><li>

		想辦法通過其他方式登錄到真機上，如通過管理控制檯，然後通過如下命令直接生成指紋。
		<pre>$ ssh-keygen -lf /etc/ssh/ssh_host_rsa_key</pre>
	</li></ol>
如果指紋不匹配，那麼你應該先做一個針對ARP請求的網絡掃描，看看有哪個IP地址迴應了ARP請求。在ping的時候掃描一下看看有沒有ARP請求。如果有兩個主機，那麼它們會為一個ARP條目而“互掐”起來，你應該能看到兩個迴應。

一旦你知道了這個神祕主機的以太網地址，就可以通過路由（或交換機）接口上跟蹤到ARP流量的去處。

	假定經過風險衡量以後，用戶決定接受這個遠程主機的公鑰，然後，會要求輸入密碼，如果密碼正確，就可以登錄了。<br><br>

	當遠程主機的公鑰被接受以後，它就會被保存在文件 ~/.ssh/known_hosts 之中。下次再連接這臺主機，系統就會認出它的公鑰已經保存在本地了，從而跳過警告部分，直接提示輸入密碼。<br><br>

	如果再次登錄時，發現 SSH 密鑰改變了。這種情況可能是由於中間人攻擊導致，但更多的情況下，是因為主機被重建，生成了新的 SSH 密鑰，所以在重建服務器的時候要養成保存恢復 SSH 密鑰習慣。<br><br>

	其它內容可以參考碩士論文，《SSH 協議的中間人攻擊研究》。
	</p>
-->



## 參考

可以參考 [SSH The Secure Shell The Definitive Guide](/reference/linux/SSH_The_Secure_Shell_The_Definitive_Guide.pdf) 文檔。

github 上收集的與 SSH 相關的經典內容 [ssh awesome](https://github.com/moul/awesome-ssh) 。


<!--
https://github.com/arthepsy/ssh-audit
https://nvd.nist.gov/
https://zzz.buzz/zh/2016/04/18/hardening-sshd/
https://networkjutsu.com/ssh-brute-force-attack/
封IP可以通過 denyhosts、iptables、Fail2Ban
-->

{% highlight text %}
{% endhighlight %}
