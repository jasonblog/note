---
title: SSH Simplify Your Life
layout: post
comments: true
category: [linux,misc]
language: chinese
keywords: ssh,sshd
description: 通常在一個 ssh 鏈接中，需要指定用戶名、主機名、主機端口號、配置參數等信息，如果每次登陸一臺主機都要輸入，那麼就會變的非常麻煩，而且一些 IP 地址通常又非常難記。為此，我們可以通過 OpenSSH 的客戶端配置文件簡化我們的登陸方式，下面簡單介紹下。
---

通常在一個 ssh 鏈接中，需要指定用戶名、主機名、主機端口號、配置參數等信息，如果每次登陸一臺主機都要輸入，那麼就會變的非常麻煩，而且一些 IP 地址通常又非常難記。

為此，我們可以通過 OpenSSH 的客戶端配置文件簡化我們的登陸方式，下面簡單介紹下。

<!-- more -->

## 常用操作

通過 SSH 可以在遠程主機上執行一些常用的操作，簡單列舉如下。

{% highlight text %}
$ ssh user@host 'mkdir -p .ssh && cat >> .ssh/authorized_keys' < ~/.ssh/id_rsa.pub
{% endhighlight %}

單引號中間的部分，表示在遠程主機上執行的操作，緊跟著的輸入重定向，表示數據通過 SSH 傳向遠程主機。這就是說，SSH 可以在用戶和遠程主機之間，建立命令和數據的傳輸通道，因此很多事情都可以通過 SSH 來完成。

下面看幾個常用的例子。

{% highlight text %}
----- 1. 將$HOME/src/下的所有文件，複製到遠程主機的$HOME/src/目錄
$ cd && tar czv src | ssh user@host 'tar xz'

----- 2. 將遠程主機$HOME/src/目錄下面的所有文件，複製到用戶的當前目錄
$ ssh user@host 'tar cz src' | tar xzv

----- 3. 查看遠程主機是否運行進程httpd
$ ssh user@host 'ps ax | grep [h]ttpd'
{% endhighlight %}

## 簡單配置

每次通過 ssh 登陸時，通常需要按照如下方式，指定用戶名、主機、端口號，如果擁有多個 ssh 賬號，那麼要記住每個 ssh 賬號的參數會非常麻煩。

{% highlight text %}
$ ssh foobar@192.168.9.123 -p 2222

常用參數:
   -p:  指定端口
   -i:  如果通過公私鑰認證，則通過該參數指定私鑰
   -v:  調試，用於查看認證過程
   -o:  指定參數，例如-o ServerAliveInterval=60
{% endhighlight %}

實際上，對於 OpenSSH 客戶端，可以通過配置文件簡化登陸。

OpenSSH 客戶端會使用 ```~/.ssh/config``` 用戶配置文件，也可使用全局配置 ```/etc/ssh/ssh_config```，如果沒有該文件則需要手動創建，該文件中的配置項通過 ```Key = Value``` 格式進行設置。

通過如下的方式指定別名，可以支持通配符 ```*%```，注意，配置項大小寫不敏感，值大小寫敏感。

{% highlight text %}
Host *                           # 對於所有主機通用配置項
    ServerAliveInterval 60       # 防止因為空閒鏈接斷開，每隔60秒發送一次請求，從而保持連接
    ServerAliveCountMax 3        # 發送請求後，如果服務端連續超過3次沒有響應，則自動斷開
    StrictHostKeyChecking no     # 默認首次添加到known_hosts時會有提示信息，配置為自動添加
    Protocol 2                   # 使用協議版本V2

Host mysql                       # 登陸時簡化MySQL主機的配置
    HostName 192.168.9.123       # 指定服務器的IP，如果與上述的Host參數相同，則可以忽略
    User foobar                  # 登陸用戶名
    Port 2222                    # 登陸使用端口號，默認22
    IdentityFile ~/.ssh/mysql    # 如果通過公私鑰認證方式，指定私鑰文件地址

Host dev backup www* mail
    HostName %h.example.com      # 利用上述的Host作為參數
{% endhighlight %}

例如，上述的 mysql 設置，可以直接通過 ssh mysql 登陸，而且 scp 等，同樣可以如此使用。

另外，對於上述的最後一個示例，需要簡單說明下，如果是通過 ssh dev 命令登陸，那麼實際登陸時對應的主機名為 dev.example.com，以此類推。

### 代理設置

為了安全設置，通常防火牆只允許 80/22 端口訪問，假設該服務器同時部署了 MySQL 服務，那麼我們就無法通過筆記本進行調試。當然可以通過 ssh 的本地代理實現，設置如下：

{% highlight text %}
$ ssh -f -N -L 9906:127.0.0.1:3306 foobar@database.example.com
{% endhighlight %}

如上，使得本地訪問 9906 端口時，會將請求轉發到 database.example.com 的 127.0.0.1:3306 ，也就是對應的 MySQL 服務器了。

當然，同樣可以通過如下方式簡化。

{% highlight text %}
Host tunnel
    HostName database.example.com
    IdentityFile ~/.ssh/foobar.example.key
    LocalForward 9906 127.0.0.1:3306
    User foobar
{% endhighlight %}

然後通過如下方式使用。

{% highlight text %}
$ ssh -f -N tunnel
{% endhighlight %}

## 共享連接

當登陸一臺服務器後，可以通過共享鏈接實現免登陸，也就是說第一次登陸之後，後面的會自動登陸。需要在添加配置文件 ```~/.ssh/config(600)``` 中添加如下內容。

{% highlight text %}
host *
    ControlMaster auto
    ControlPath /tmp/ssh_mux_%r@%h:%p      # 如果不使用該參數只能是最後一個免登
    ControlPersist 4h                      # 默認直接退出，該參數指定4小時後退出
{% endhighlight %}

此時會在 /tmp 目錄下創建 ssh_mux 的 sock 文件，它記錄了你目前登錄到的機器，這樣的話，你登錄同樣的機器就會重用同一個鏈接了。

這種方法也可以同時避免如下的錯誤 Shared connection to &lt;hostname&gt; closed.；這個主要是由於使用共享鏈接，當原鏈接關閉之後導致共享的鏈接同樣失敗，這樣共享的鏈接可以保持打開狀態，並且會複用。


## 公私鑰認證

如果需要通過自動化腳本定期進行備份，當輸入密碼並保證密碼的安全時，處理起來會比較麻煩。為此，可以通過公鑰登錄 (RSA/DSA認證)，省去輸入密碼的步驟。

所謂 "公鑰登錄" ，原理很簡單，就是用戶將自己的公鑰儲存在遠程主機上。登錄時，遠程主機會向用戶發送一段隨機字符串，用戶用自己的私鑰加密後，再發回來。遠程主機用事先儲存的公鑰進行解密，如果成功，就證明用戶是可信的，直接允許登錄 shell ，不再要求密碼。

只需要生成一個密鑰對，然後將公鑰傳送到服務器端，並保存在配置文件中的 AuthorizedKeysFile 指定的文件。

### 實踐

可以通過如下步驟執行。

#### 1. 服務端設置

在服務器端，首先確認 ```/etc/ssh/sshd_config``` 這個文件，檢查下面幾行是否有效。

{% highlight text %}
RSAAuthentication yes
PubkeyAuthentication yes
AuthorizedKeysFile .ssh/authorized_keys
{% endhighlight %}

#### 2. 生成公私鑰

直接通過 ssh-keygen 生成公私鑰密碼對即可。

{% highlight text %}
$ ssh-keygen [-t rsa/dsa]
$ ssh-keygen -t rsa -f ~/.ssh/id_rsa -N "" -C "comment"
參數：
    -t:   加密類型
    -f:   指定輸出文件
    -N:   通過該密碼加密私鑰，為空則會自動登陸
    -C:   註釋，通常為郵箱名稱，會保存在公鑰中
{% endhighlight %}

默認採用 rsa 加密，如上，如若想設置無密碼登錄，則直接回車即可。密鑰對默認保存在 .ssh/ 目錄下，包括了 id_rsa(私鑰) id_rsa.pub(公鑰)。

其它常用的加密方式還包括了 RSA, DSA, ECDSA, ED25519 。

#### 3. 將公鑰上傳到服務器

將公鑰上傳到服務器，此時會保存到需要免密碼登陸用戶 (在此為 foobar) 的 $HOME 目錄下。

{% highlight text %}
$ scp ~/.ssh/id_rsa.pub foobar@remote-server-ip:~
{% endhighlight %}

#### 4. 公鑰保存

配置文件中通過 AuthorizedKeysFile 指定公鑰保存的文件名，一般是 ```.ssh/authorized_keys``` 文件，然後直接添加到該文件中即可。

{% highlight text %}
$ mkdir ~/.ssh/ && chmod 700 ~/.ssh/
$ cat id_rsa.pub >> .ssh/authorized_keys
$ chmod 644 .ssh/authorized_keys
{% endhighlight %}

在此，需要明確如下文件的權限。

{% highlight text %}
$ chmod 700 ~/.ssh/
$ chmod 644 ~/.ssh/authorized_keys
$ chmod 600 ~/.ssh/id_rsa
{% endhighlight %}

#### 5. 登陸

如果使用私鑰登錄多臺主機，可以通過 -i 選項指定私鑰文件。

{% highlight text %}
$ ssh -i id_rsa user-name@remote-server-ip
{% endhighlight %}

#### 6. 添加到known_hosts

如果是初次登陸時，會提示是否將對應的主機添加到 known_hosts 文件中，可以通過如下命令自動添加，而不需要交互。

{% highlight text %}
$ ssh -T -o StrictHostKeyChecking=no 192.144.51.85 'date'
{% endhighlight %}


### 簡化配置

也可以通過如下命令簡單地進行設置。

{% highlight text %}
$ ssh-keygen -t dsa -f ~/.ssh/id_dsa -C "foobar@kidding.com" -N ""
$ ssh-copy-id -i /home/foobar/.ssh/id_dsa.pub foobar@remote-host-ip
$ ssh remote-host-ip
{% endhighlight %}

可以通過如下方式在本地進行簡單的測試。

{% highlight text %}
$ ssh-keygen -t ed25519
$ cat .ssh/id_rsa.pub >> .ssh/authorized_keys
$ ssh 127.1
{% endhighlight %}

ssh-copy-id 命令會自動將公鑰添加到 ```~/.ssh/authorized_keys``` 文件中。

### 安全相關

在生成密鑰對時，如果密碼沒有輸入，也就是為空，那麼可以自動登陸，不過同樣會導致當有人獲取了該私鑰後，可以登陸你配置的所有服務器。

為此，為了保證私鑰的安全，生成密鑰對時，最好輸入密碼，這樣即使有人獲取了你的私鑰文件，他仍然無法登陸。

不過這也導致了每次登陸時需要輸入 **私鑰保護密碼**，會產生與之前相同的問題。

## ssh-agent

為了解決上述的問題，可以使用 ssh-agent，這是一個守護進程，設計它的唯一目的就是對解密用的專用密鑰進行高速緩存。

ssh 支持與 ssh-agent 通信，允許 ssh 建立新連接時自動獲取解密的專用密鑰；你所需要做的就是，使用 ssh-add 命令把密鑰添加到 ssh-agent 的高速緩存中即可。

還沒有仔細研究過。

<!--
### 工作原理

在 CentOS 中，會默認啟動 ssh-agent 服務，當然，也可以手動啟動；此是會輸出一些信息，包括兩個重要的環境變量 SSH_AUTH_SOCK(ssh/scp用來連接的套接字) 和 SSH_AGENT_PID 。

可以通過 ssh-add id_rsa 將私鑰添加到 agent 中；另外，如果出現了錯誤 Agent admitted failure to sign using the key ，則同樣需要執行上述命令。

### 常用命令

可以通過 ssh-add 管理私鑰，常用命令參數如下：

* -L：列出所緩存的公鑰，與服務器端 authorized_keys 文件中保存的值相同；
* -D：清空緩存中的密碼，此時如果登陸則需要使用之前設置的私鑰保護密碼；
* -X/x：對agent加鎖，不知道具體的作用，仍然可以免密碼登陸。

{% highlight text %}
$ eval $(ssh-keygen)         # 啟動並設置環境變量
$ eval $(ssh-keygen -k)      # 退出
{% endhighlight %}

## 缺點

首先，如果在 bash_profile 中使用 eval \`ssh-agent\`，那麼每次登錄會話都會啟動一個新的 ssh-agent 副本；這不僅僅是有一丁點兒浪費，而且還意味著您得使用 ssh-add 向每個新的 ssh-agent 副本添加專用密鑰。

如果您只想打開系統上的一個終端或控制檯，這沒什麼大不了的，但是我們中大多數人打開相當多的終端，每次新打開控制檯都需要鍵入密碼短語。

從技術角度講，既然一個 ssh-agent 進程的確應當足夠了，要是我們還需這樣做，這毫無道理，為此可以使用 keychain ，在 bash_profile 文件裡追加 eval \`keychain --eval id_dsa\` 即可實現到其他機器的免登。





## Login to internal lan server at 192.168.0.251 via our public uk office ssh based gateway using ##
## $ ssh uk.gw.lan ##
Host uk.gw.lan uk.lan
     HostName 192.168.0.251
     User nixcraft
     ProxyCommand  ssh nixcraft@gateway.uk.cyberciti.biz nc %h %p 2> /dev/null


    ProxyCommand : Specifies the command to use to connect to the server. The command string extends to the end of the line, and is executed with the user's shell. In the command string, any occurrence of %h will be substituted by the host name to connect, %p by the port, and %r by the remote user name. The command can be basically anything, and should read from its standard input and write to its standard output. This directive is useful in conjunction with nc(1) and its proxy support. For example, the following directive would connect via an HTTP proxy at 192.1.0.253:
    ProxyCommand /usr/bin/nc -X connect -x 192.1.0.253:3128 %h %p
    LocalForward : Specifies that a TCP port on the local machine be forwarded over the secure channel to the specified host and port from the remote machine. The first argument must be [bind_address:]port and the second argument must be host:hostport.
    Protocol : Specifies the protocol versions ssh(1) should support in order of preference. The possible values are 1 and 2.
    ServerAliveInterval : Sets a timeout interval in seconds after which if no data has been received from the server, ssh(1) will send a message through the encrypted channel to request a response from the server. See blogpost "Open SSH Server connection drops out after few or N minutes of inactivity" for more information.
    ServerAliveCountMax : Sets the number of server alive messages which may be sent without ssh(1) receiving any messages back from the server. If this threshold is reached while server alive messages are being sent, ssh will disconnect from the server, terminating the session.

通過本地端口訪問服務器<br>
有時可能有些服務，比如數據庫或是Web服務器，它們運行在遠程服務器上，但是如果有用方式可以直接從本地程序連接它們，那會非常有用，要做到這一點，你需要用到端口轉發(port forwarding)，舉個例子，如果你的服務器運行Postgres（並且只允許本地訪問），那麼你就可以在你的SSH配置中加入：
<pre>
Host db
    LocalForward 5433 localhost:5432
</pre>
現在當你連接你的SSH服務器時，它會在你本地電腦打開一個5433端口（我隨便挑的），並將所有發送到這個端口的數據轉發到服務器的5432端口（Postgres的默認端口），然後，只要你和服務器建立了連接，你就可以通過5433端口來訪問服務器的Postgres了。<br>
$ ssh db<br>
現在打開另外一個窗口，你就可以通過下面這條命令在本地連接你的Postgres數據庫了：<br>
$ psql -h localhost -p 5443 orders</li><br><li>

# 登陸跳板機的方法

現在假設有 ABC 三臺機器，A 是本地機器，B 是跳板服務器（123.456.789.110），C 是工作服務器（192.168.1.111），其中 A 可以訪問 B，但不能直接訪問 C，B 可以直接訪問 C。


### 通過 ssh 端口轉發命令

ssh -CfNg -L 8833:192.168.1.111:22 userc@192.168.1.111<br>

句話的意思是當我訪問B機器8833端口時，其實是在訪問C的22端口。以後就可以直接在A機器命令行輸入。<br>
ssh -p 8833 userc@123.456.789.110<br>
就可以登錄C機器了，這裡要注意的是此處用戶名是B登錄C的用戶名，而ip地址是B的ip地址，要和普通的ssh登錄做區分。</li><br><li>

通過proxycommand+netcat<br>
這種方法不需要對B和C進行任何操作，只需要對A機器上的ssh_config文件做些修改就可以了。我們首先修改A機器上的ssh_config文件，添加這麼幾行。
<pre>
Host Cserver
    HostName 192.168.1.111
    Port 22 【C機器的ssh端口】
    ProxyCommand ssh -p 9022【B機器的ssh端口】 userb@123.456.789.110 nc %h %p
</pre>
然後在A機器的命令行輸入<br>
ssh userc@Cserver</li><br><li>

無腦笨方法<br>
我們知道ssh是可以直接執行遠程命令的，於是，在A機器命令行輸入：<br>
ssh -t -p 9022【B機器ssh端口】 userb@123.456.789.110 "ssh userc@192.168.1.111"<br>
 當然這還是要輸入兩次ssh命令，沒有任何技術含量。無腦懶人可以把這個在.bashrc裡面alias一下，記住要source哦。





1. 在服務器間跳轉

有些時候，你可能沒法直接連接到某臺服務器，而需要使用一臺中間服務器進行中轉，這個過程也可以自動化。首先確保你已經為服務器配置了公鑰訪問，並開啟了agent forwarding，現在你就可以通過2條命令來連接目標服務器，不會有任何提示輸入：

$ ssh gateway

gateway $ ssh db

然後在你的本地SSH配置中，添加下面這條配置：

Host db

HostName db.internal.example.com

ProxyCommand ssh gateway netcat -q 600 %h %p

現在你就可以通過一條命令來直接連接目標服務器了:

$ ssh db

這裡你可能會需要等待長一點的時間，因為SSH需要進行兩次認證，，注意netcat也有可能被寫成nc或者ncat或者前面還需要加上g，你需要檢查你的中間服務器來確定實際的參數。
 -->






{% highlight text %}
{% endhighlight %}
