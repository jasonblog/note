---
title: Linux 常用網絡命令
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: linux,network,tcpdump,netcat
description: 簡單記錄一下 Linux 中的常用網絡命令，如 tcpdump、netcat 等。
---

簡單記錄一下 Linux 中的常用網絡命令，如 tcpdump、netcat 等。

<!-- more -->



## Wget

一個下載文件的工具，支持斷點下載、FTP 和 HTTP 下載、代理服務器，如下簡單介紹其使用方法。

##### 1. 下載單個文件

以下的例子是從網絡下載一個文件並保存在當前目錄

{% highlight text %}
$ wget http://foobar.example.com/your-file-name.tar.bz2
{% endhighlight %}

在下載的過程中會顯示進度條，包含下載完成百分比，已經下載的字節，當前下載速度，剩餘下載時間。

##### 2. 重命名保存文件

默認會以最後一個符合 `"/"` 的後面的字符來命名文件，對於動態鏈接的下載通常文件名會不正確，如下示例會下載一個文件並以名稱 `download.php?id=1` 保存。

{% highlight text %}
$ wget https://www.example.com/download?id=1
{% endhighlight %}

即使下載後的文件是 zip 格式，仍然以 `download.php?id=1` 命令，此時可以通過 `-O` 來指定一個下載文件名。

{% highlight text %}
$ wget -O example.zip https://www.example.com/download.php?id=1
{% endhighlight %}

##### 3. 限速下載

當執行 wget 時，默認會佔用全部可能的寬帶下載，但是當你準備下載一個大文件，而你還需要下載其它文件時就有必要限速了。

{% highlight text %}
$ wget --limit-rate=300k http://foobar.example.com/your-file-name.tar.bz2
{% endhighlight %}

##### 4. 斷點續傳

通過 `-c` 參數重新啟動下載中斷的文件。

{% highlight text %}
$ wget -c http://foobar.example.com/your-file-name.tar.bz2
{% endhighlight %}

對於下載大文件時突然由於網絡等原因中斷時非常有用。

##### 5.  後臺下載

下載非常大的文件時，可以使用參數 `-b` 進行後臺下載。

{% highlight text %}
$ wget -c http://foobar.example.com/your-file-name.tar.bz2
Continuing in background, pid 9817.
Output will be written to `wget-log'.
{% endhighlight %}

可以使用 `tail -f wget-log` 命令查看進度。

##### 6. 偽裝代理下載

有些網站能根據代理是不是瀏覽器而拒絕下載，此時可以通過 `--user-agent` 參數偽裝代理。

{% highlight text %}
$ wget --user-agent="Mozilla/5.0 (Windows NT 6.1; en-US)" http://foobar.example.com/file.tar.bz2
{% endhighlight %}

##### 7. 測試下載鏈接

通常在 A) 定時下載之前進行檢查；B) 間隔檢測網站是否可用；C) 檢查網站頁面的死鏈接，可以通過 `--spider` 檢查鏈接是否可用。

如果下載鏈接正常，將會顯示：

{% highlight text %}
$ wget --spider URL
Spider mode enabled. Check if remote file exists.
HTTP request sent, awaiting response... 200 OK
Length: unspecified [text/html]
Remote file exists and could contain further links,
but recursion is disabled -- not retrieving.
{% endhighlight %}

而對於錯誤鏈接，將會顯示如下錯誤：

{% highlight text %}
$ wget --spider URL
Spider mode enabled. Check if remote file exists.
HTTP request sent, awaiting response... 404 Not Found
Remote file does not exist -- broken link!!!
{% endhighlight %}

##### 8. 增加重試次數

如果網絡有問題或下載一個大文件也有可能失敗，默認重試 20 次連接下載文件，如果需要，可以使用 `--tries` 增加重試次數。

{% highlight text %}
$ wget --tries=40 URL
{% endhighlight %}

##### 9. 下載多個文件

通過 `-i` 參數指定下載文件，下載多個文件。

{% highlight text %}
$ cat filelist.txt
url1
url2
url3

$ wget -i filelist.txt
{% endhighlight %}

##### 10. 鏡像網站

使用 `--mirror` 下載整個網站，下面的例子是下載整個網站到本地。

{% highlight text %}
$ wget --mirror -p --convert-links -P ./LOCAL URL
--miror  鏡像下載
-p       下載所有為了html頁面顯示正常的文件
--convert-links  下載後，轉換成本地的鏈接
-P ./LOCAL       保存所有文件和目錄到本地指定目錄
{% endhighlight %}

##### 11. 過濾指定格式

你想下載一個網站，但你不希望下載圖片，你可以使用以下命令。

{% highlight text %}
$ wget --reject=gif url
{% endhighlight %}


<!--
12、使用wget -o把下載信息存入日誌文件

你不希望下載信息直接顯示在終端而是在一個日誌文件，可以使用以下命令：

    wget -o download.log URL

13、使用wget -Q限制總下載文件大小

當你想要下載的文件超過5M而退出下載，你可以使用以下命令:

    wget -Q5m -i filelist.txt

注意：這個參數對單個文件下載不起作用，只能遞歸下載時才有效。
14、使用wget -r -A下載指定格式文件

可以在以下情況使用該功能

    下載一個網站的所有圖片
    下載一個網站的所有視頻
    下載一個網站的所有PDF文件

    wget -r -A.pdf url

15、使用wget FTP下載

你可以使用wget來完成ftp鏈接的下載。
使用wget匿名ftp下載

    wget ftp-url

使用wget用戶名和密碼認證的ftp下載

    wget --ftp-user=USERNAME --ftp-password=PASSWORD url
-->



















<!--
## Wget

	需要下載某個目錄下面的所有文件。命令如下

<pre>wget -c -r -np -k -L -p www.xxx.org/pub/path/index.html</pre>

在下載時。有用到外部域名的圖片或連接。如果需要同時下載就要用-H參數。

wget -np -nH -r --span-hosts www.xxx.org/pub/path/

-c 斷點續傳
-r 遞歸下載，下載指定網頁某一目錄下（包括子目錄）的所有文件
-nd 遞歸下載時不創建一層一層的目錄，把所有的文件下載到當前目錄
-np 遞歸下載時不搜索上層目錄，如wget -c -r www.xxx.org/pub/path/沒有加參數-np，就會同時下載path的上一級目錄pub下的其它文件
-k 將絕對鏈接轉為相對鏈接，下載整個站點後脫機瀏覽網頁，最好加上這個參數
-L 遞歸時不進入其它主機，如wget -c -r www.xxx.org/
如果網站內有一個這樣的鏈接：
www.yyy.org，不加參數-L，就會像大火燒山一樣，會遞歸下載www.yyy.org網站
-p 下載網頁所需的所有文件，如圖片等
-A 指定要下載的文件樣式列表，多個樣式用逗號分隔
-i 後面跟一個文件，文件內指明要下載的URL


















還有其他的用法，我從網上搜索的，也一併寫上來，方便以後自己使用。

wget的常見用法

wget的使用格式
Usage: wget [OPTION]… [URL]…

* 用wget做站點鏡像:
wget -r -p -np -k http://dsec.pku.edu.cn/~usr_name/
# 或者
wget -m http://www.tldp.org/LDP/abs/html/

* 在不穩定的網絡上下載一個部分下載的文件，以及在空閒時段下載
wget -t 0 -w 31 -c http://dsec.pku.edu.cn/BBC.avi -o down.log &
# 或者從filelist讀入要下載的文件列表
wget -t 0 -w 31 -c -B ftp://dsec.pku.edu.cn/linuxsoft -i filelist.txt -o
down.log &

上面的代碼還可以用來在網絡比較空閒的時段進行下載。我的用法是:在mozilla中將不方便當時下載的URL鏈接拷貝到內存中然後粘貼到文件filelist.txt中，在晚上要出去系統前執行上面代碼的第二條。

* 使用代理下載
wget -Y on -p -k https://sourceforge.net/projects/wvware/

代理可以在環境變量或wgetrc文件中設定

# 在環境變量中設定代理
export PROXY=http://211.90.168.94:8080/
# 在~/.wgetrc中設定代理
http_proxy = http://proxy.yoyodyne.com:18023/
ftp_proxy = http://proxy.yoyodyne.com:18023/

wget各種選項分類列表

* 啟動

-V, –version 顯示wget的版本後退出
-h, –help 打印語法幫助
-e, –execute=COMMAND
執行`.wgetrc'格式的命令，wgetrc格式參見/etc/wgetrc或~/.wgetrc

* 記錄和輸入文件

-o, –output-file=FILE 把記錄寫到FILE文件中
-a, –append-output=FILE 把記錄追加到FILE文件中
-d, –debug 打印調試輸出
-q, –quiet 安靜模式(沒有輸出)
-v, –verbose 冗長模式(這是缺省設置)
-nv, –non-verbose 關掉冗長模式，但不是安靜模式
-F, –force-html 把輸入文件當作HTML格式文件對待
-B, –base=URL 將URL作為在-F -i參數指定的文件中出現的相對鏈接的前綴
–sslcertfile=FILE 可選客戶端證書
–sslcertkey=KEYFILE 可選客戶端證書的KEYFILE
–egd-file=FILE 指定EGD socket的文件名

* 下載

–bind-address=ADDRESS
指定本地使用地址(主機名或IP，當本地有多個IP或名字時使用)
-t, –tries=NUMBER 設定最大嘗試鏈接次數(0 表示無限制).
-O –output-document=FILE 把文檔寫到FILE文件中
-nc, –no-clobber 不要覆蓋存在的文件或使用.#前綴
–progress=TYPE 設定進程條標記
-S, –server-response 打印服務器的迴應
–spider 不下載任何東西
-T, –timeout=SECONDS 設定響應超時的秒數
-w, –wait=SECONDS 兩次嘗試之間間隔SECONDS秒
–waitretry=SECONDS 在重新鏈接之間等待1…SECONDS秒
–random-wait 在下載之間等待0…2*WAIT秒
-Y, –proxy=on/off 打開或關閉代理
-Q, –quota=NUMBER 設置下載的容量限制


* HTTP 選項

–http-user=USER 設定HTTP用戶名為 USER.
–http-passwd=PASS 設定http密碼為 PASS.
-C, –cache=on/off 允許/不允許服務器端的數據緩存 (一般情況下允許).
-E, –html-extension 將所有text/html文檔以.html擴展名保存
–ignore-length 忽略 `Content-Length'頭域
–header=STRING 在headers中插入字符串 STRING
–proxy-user=USER 設定代理的用戶名為 USER
–proxy-passwd=PASS 設定代理的密碼為 PASS
–referer=URL 在HTTP請求中包含 `Referer: URL'頭
-s, –save-headers 保存HTTP頭到文件
-U, –user-agent=AGENT 設定代理的名稱為 AGENT而不是 Wget/VERSION.
–no-http-keep-alive 關閉 HTTP活動鏈接 (永遠鏈接).
–cookies=off 不使用 cookies.
–load-cookies=FILE 在開始會話前從文件 FILE中加載cookie
–save-cookies=FILE 在會話結束後將 cookies保存到 FILE文件中

* FTP 選項

-nr, –dont-remove-listing 不移走 `.listing'文件
-g, –glob=on/off 打開或關閉文件名的 globbing機制
–passive-ftp 使用被動傳輸模式 (缺省值).
–active-ftp 使用主動傳輸模式
–retr-symlinks 在遞歸的時候，將鏈接指向文件(而不是目錄)

* 遞歸下載

-l, –level=NUMBER 最大遞歸深度 (inf 或 0 代表無窮).
–delete-after 在現在完畢後局部刪除文件
-K, –backup-converted 在轉換文件X之前，將之備份為 X.orig

* 遞歸下載中的包含和不包含(accept/reject)

-D, –domains=LIST 分號分隔的被接受域的列表
–exclude-domains=LIST 分號分隔的不被接受的域的列表
–follow-ftp 跟蹤HTML文檔中的FTP鏈接
–follow-tags=LIST 分號分隔的被跟蹤的HTML標籤的列表
-G, –ignore-tags=LIST 分號分隔的被忽略的HTML標籤的列表
-H, –span-hosts 當遞歸時轉到外部主機
-I, –include-directories=LIST 允許目錄的列表
-X, –exclude-directories=LIST 不被包含目錄的列表
-np, –no-parent 不要追溯到父目錄















<br><br><br><br><br><br><br><br>
<ul><li>
* 啟動<ul><li>
-b, –-background 啟動後轉入後臺執行。
</li></ul></li><br><li>


* 記錄和輸入文件<ul><li>
-i, --input-file=FILE 下載在 FILE 文件中出現的 URLs 。</li></ul></li><br><li>

* 目錄<ul><li>
-nd, --no-directories 遞歸下載時不創建一層一層的目錄，把所有的文件下載到當前目錄</li><li>
-x, --force-directories 強制創建目錄</li><li>
-nH, --no-host-directories 不創建主機目錄</li><li>
-P, --directory-prefix=PREFIX 將文件保存到目錄 PREFIX/…</li><li>
–cut-dirs=NUMBER 忽略 NUMBER層遠程目錄</li></ul></li><br><li>

* 下載<ul><li>
-c, --continue 接著下載沒下載完的文件。</li><li>
-N, –-timestamping 不要重新下載文件除非比本地文件新。</li><li>
-–limit-rate=RATE 限定下載輸率。
</li></ul></li><br><li>



* 遞歸下載<ul><li>
-r, –-recursive 遞歸下載，下載指定網頁某一目錄下（包括子目錄）的所有文件</li><li>
-k, –-convert-links 將絕對鏈接轉為相對鏈接，下載整個站點後脫機瀏覽網頁，最好加上這個參數</li><li>
-p, -–page-requisites 下載網頁所需的所有文件，如圖片等</li><li>
-m, –mirror 等價於 -r -N -l inf -nr.

</li></ul></li><br><li>

* 遞歸下載中的包含和不包含(accept/reject)<ul><li>
-np, –-no-parent 遞歸下載時不搜索上層目錄。如 wget -r www.xxx.org/pub/path/ 沒有加參數 -np ，就會同時下載 path 的上一級目錄 pub 下的其它文件。</li><li>
-L, –-relative 僅僅跟蹤相對鏈接，遞歸時不進入其它主機。</li><li>
-A, –-accept=LIST 分號分隔的被接受擴展名的列表，多個樣式用逗號分隔</li><li>
-R, –-reject=LIST 分號分隔的不被接受的擴展名的列表</li></ul>
</li></ul>

<h2>舉例</h2><p><ul><li>
下載某個目錄下面的所有文件，可以下載 Html 的文檔資料。
<pre>$ wget -c -r -np -k -L -p www.xxx.org/pub/path/</pre></li><br><li>

使用 -O 選項設置下載後的文件名。
<pre>wget -O wget.zip http://ftp.gnu.org/gnu/wget/wget-1.5.3.tar.gz</pre></li><br><li>

後臺下載，並將 log 保存在 /wget/log.txt 。
<pre> wget -b /wget/log.txt http://ftp.gnu.org/gnu/wget/wget-1.5.3.tar.gz</pre></li><br><li>

指定下載時的用戶名和密碼。
<pre># wget --http-user=narad --http-password=password http://ftp.gnu.org/gnu/wget/wget-1.5.3.tar.gz
# wget --ftp-user=narad --ftp-password=password ftp://ftp.gnu.org/gnu/wget/wget-1.5.3.tar.gz</pre>
-->

## tcpdump

tcpdump 會根據表達式 (expression) 來決定是否過濾報文，如果滿足條件則會捕獲報文，如果沒有給出任何條件則會將所有的報文捕獲；tcpdump 命令參數如下：

{% highlight text %}
tcpdump [ -AbdDefhHIJKlLnNOpqRStuUvxX ] [ -B buffer_size ] [ -c count ]
        [ -C file_size ] [ -G rotate_seconds ] [ -F file ]
        [ -i interface ] [ -j tstamp_type ] [ -m module ] [ -M secret ]
        [ -r file ] [ -s snaplen ] [ -T type ] [ -w file ]
        [ -W filecount ]
        [ -E spi@ipaddr algo:secret,...  ]
        [ -y datalinktype ] [ -z postrotate-command ] [ -Z user ]
        [ expression ]

參數詳見：
  -n           : 不轉換地址；-nn 不轉換地址和端口號。
  -i interface : 指明要監聽的介面，如eth0、lo等，any表示所有的，可以通過tcpdump -D查看對應的編號。
  -X           : 顯示ASCII和十六進制。
  -XX          : 同-X，但同時顯示包的頭部信息。
  -S           : 使用絕對地址而非相對地址顯示序列號。
  -s NUM       : 默認96字節的數據，0(或1514)表示捕獲所有數據。
  -v -vv -vvv  : 顯示詳細信息，對應不同等級。
  -c           : 獲取一定數量的包後停止。
  -e           : 同時獲取ethernet header。
  -q           : 減少協議信息的顯示。
  -E           : 通過提供的密碼解密IPSEC包。
  -w/r         : 寫入或者讀取文件。

在表達式 (expression) 中通常有如下幾種關鍵字:
  類型
    host: 指明是一臺主機，如host 192.168.9.100。
    net: 捕獲與網絡地址的通訊包，如net 192.168.0.0/16。
    port: 指明是端口號，如port 21；也可以指定範圍 portrange 21-23。

  傳輸方向
    src: 源地址，如src 192.168.9.100。
    dst: 目的地址，如dst 192.168.9.100。
    dst and src: 是目的地址且是源地址。
    dst or src(default): 是目的地址或是源地址。
    src|dst port: 指定目的或源端口號。

  協議
    fddi: 在FDDI(分佈式光纖數據接口網絡)上的特定網絡協議，是ether的別名。
    ip arp rarp tcp udp icmp: 指定協議的類型。

  過濾包的大小
    less/greater 32 接受小於或大於32bytes的包，或者< >=等，其它運算符也可以。

  其它
    gateway broadcast less greater
    邏輯運算: 非'not'/'!'，與'and'/'&&'，或'or'/'||'，通過邏輯可以將上述的條件連接起來。
{% endhighlight %}

另外常見的示例舉例如下：

<ul><li>
指定主機，可以設定源和目的地，可以為主機名或者 IP 。<br>

{% highlight text %}
tcpdump host 192.168.1.0                                      # 所有進出的數據包
tcpdump host 192.168.1.0 and \(192.168.1.2 or 192.168.1.3 \)  # 截獲0和2或3之間的通訊
tcpdump ip host foo and ! bar                                 # 截獲foo和非bar之間的通訊，!可換為not
tcpdump src host hostname                                     # 主機發送的數據
tcpdump dst host hostname                                     # 發送到主機的數據
{% endhighlight %} </li><li>

指定端口，協議，協議可以為tcp/udp<br>

{% highlight text %}
tcpdump tcp port 23 host 192.168.1.0
{% endhighlight %} </li><li>

截獲ftp發送的信息，獲取用戶名和密碼<br>

{% highlight text %}
tcpdump -i lo -nn -X port 21
{% endhighlight %} </li><li>

從192.168網絡法往10或172.16網絡的數據包<br>

{% highlight text %}
tcpdump -nvX src net 192.168.0.0/16 and dst net 10.0.0.0/8 or 172.16.0.0/16
{% endhighlight %} </li><li>

從172.16網絡發往192.168.0.2的非ICMP包<br>

{% highlight text %}
tcpdump -nvvXSs 1514 dst 192.168.0.2 and src net 172.16.0.0/16 and not icmp
{% endhighlight %}
</li></ul>

對於複雜的條件可以使用 ```()```，終端中需要```\( \)```轉義，或者用 ```''``` 包裹，還可指定標誌如 ACK 。

### 高級選項

常見的，只想監控 TCP 的三次握手建立鏈接以及四次握手斷開連接，也就是說只需要捕獲 TCP 控制包，如 SYN、ACK 或 FIN 標記相關的包。

基於 libpcap 的 tcpdump 支持標準的包過濾規則，如基於包頭的過濾，也就是基於源目的 IP 地址、端口和協議類型，而且也支持更多通用分組表達式。包中的任意字節範圍都可以使用關係或二進制操作符進行檢查；對於字節範圍表達，可以使用以下格式：

{% highlight text %}
proto [ expr : size ]
參數詳解：
  proto: 常見的協議之一，如 ip、arp、tcp、udp、icmp、ipv6
  expr : 與指定的協議頭開頭相關的字節偏移量，如直接偏移量(tcpflags)、取值常量(tcp-syn、tcp-ack、tcp-fin)
  size : 可選，從字節偏移量開始檢查的字節數量
{% endhighlight %}

常見的場景：

{% highlight text %}
# tcpdump -i eth0 "tcp[tcpflags] & (tcp-syn) != 0"           // 只捕獲TCP SYNC包
# tcpdump -i eth0 "tcp[13] & (2) != 0"                       // 同上

# tcpdump -i eth0 "tcp[tcpflags] & (tcp-ack) != 0"           // 只捕獲TCP ACK包
# tcpdump -i eth0 "tcp[tcpflags] & (tcp-fin) != 0"           // 只捕獲TCP FIN包
# tcpdump -i eth0 "tcp[tcpflags] & (tcp-syn|tcp-ack) != 0"   // 只捕獲TCP SYN或ACK包
{% endhighlight %}

可見 tcpflags==13，其它的還有 SYNCHRONIZE(2)、ACKNOWLEDGE(16)、URGENT(32)、PUSH(8)、RESET(4)、FINISH(1)，如果是捕獲 SYNC+ACK(18) 。

<!--
Traffic with the ‘Evil Bit’ Set
# tcpdump ‘ip[6] & 128 != 0‘
通過如下方式方便記憶。
Unskilled Attackers Pester Real Security Folks
Unskilled = URG
Attackers = ACK
Pester = PSH
Real = RST
Security = SYN
Folks = FIN
-->


## Netcat

Netcat 用於調試、檢查網絡的工具包，可用於創建 TCP/IP 連接，最大的用途就是用來處理 TCP UDP 套接字。在網絡安全領域被稱作 "TCP IP 的瑞士軍刀" (Swiss-army knife for TCP/IP)。

在 CentOS 中，可以通過如下方式安裝。

{% highlight text %}
----- CentOS 6版本安裝
# yum install nc

----- CentOS 7版本安裝
# yum install nmap-ncat
{% endhighlight %}

常用參數簡單列舉如下。

{% highlight text %}
常用參數：
  -k      保持打開，默認只監聽一個客戶端，如果某一端退出則退出
  -l      監聽模式，用於接收鏈接請求，默認客戶端退出後服務同時退出
  -n      直接使用IP地址，不做DNS解析
  -v      用於顯示更詳細的信息，可以使用多個
  -u      UDP模式
  -w secs 連接超時
  -t      使用TELNET協議
  -e file 鏈接後執行的命令
{% endhighlight %}


### 常見示例

在此簡單列舉下常見的示例。

##### 運行於服務器模式，偵聽指定端口

默認使用 ipv4(-4) ，可以顯示使用 ipv6(-6)；默認當客戶端退出後，服務端同時也會退出，可以通過 -k 防止服務端退出；通過 -u 指定是 UDP 連接。

{% highlight text %}
$ nc -l 2389                                     // 創建服務端，監聽2389端口
$ nc 127.1 2389                                  // 使用客戶端模式來連接到2389端口
{% endhighlight %}

##### 端口掃描

{% highlight text %}
----- 掃描10.1.1.180主機上的80或者1~14000號TCP端口
$ nc -v -z -w2 10.1.1.180 80
(UNKNOWN) [10.1.1.180] 80 (www) open
$ nc -v -z -w2 10.1.1.180 1-14000

----- 掃描10.1.1.180主機上的1~14000號UDP端口
$ nc -u -v -z -w2 10.1.1.180 1-14000
{% endhighlight %}

##### REMOTE主機綁定SHELL

{% highlight text %}
----- 綁定到5354端口
$ nc -l -p 5354 -t -e c:\winnt\system32\cmd.exe
$ nc -l -p 5354 -t -e /bin/bash

----- 鏈接到服務器，可以執行Bash命令
$ nc 10.1.1.180 5354
{% endhighlight %}

##### 傳輸文件/文件夾

將客戶端的 testfile 傳輸到服務器端的 test，可以是文本文件，也可以是二進制文件。對於目錄需要通過 tar 打包，然後再發送。

{% highlight text %}
$ nc -l 2389 > test                              // 服務器端將文件重定向到test
$ cat testfile | nc 127.1 2389                   // 客戶端傳輸文件，或者如下
$ nc 127.1 2389 &lt; testfile

$ tar -cvf -- DIR-NAME | nc -l 1567              // 打包目錄
$ nc -n 172.1 1567 | tar -xvf -
$ tar -cvf -- DIR-NAME | bzip2 -z | nc -l 1567   // 同時進行壓縮，減小帶寬的使用
$ nc -n 172.1 1567 | bzip2 -d |tar -xvf -
{% endhighlight %}

##### 加密傳輸

{% highlight text %}
$ nc 127.1 1567 | mcrypt --flush --bare -F -q -d -m ecb > file.txt   // 在此使用mcrypt工具加密數據
$ mcrypt --flush --bare -F -q -m ecb < file.txt | nc -l 1567
{% endhighlight %}

以上兩個命令會提示需要密碼，確保兩端使用相同的密碼，也可以使用其它任意加密工具。

##### 視頻流

雖然不是生成流視頻的最好方法，但如果服務器上沒有特定的工具，使用 netcat 。

{% highlight text %}
$ cat video.avi | nc -l 1567
$ nc 172.31.100.7 1567 | mplayer -vo x11 -cache 3000 -
{% endhighlight %}

##### 克隆一個設備

如果已經安裝配置一臺 Linux 機器並且需要重複同樣的操作對其他的機器，而你不想在重複配置一遍。

{% highlight text %}
$ dd if=/dev/sda | nc -l 1567             // 假設系統在磁盤/dev/sda上
$ nc -n 172.1 1567 | dd of=/dev/sda       // 複製到其它機器
{% endhighlight %}

如果已經做過分區並且只需要克隆 root 分區，可以根據系統 root 分區的位置，更改 sda 為 sda1，sda2 等等。

##### 超時控制
多數情況我們不希望連接一直保持，那麼我們可以使用 -w 參數來指定連接的空閒超時時間，該參數緊接一個數值，代表秒數，如果連接超過指定時間則連接會被終止。

{% highlight text %}
$ nc -l 2389
$ nc -w 10 localhost 2389                // 該連接將在 10 秒後中斷。
{% endhighlight %}

注意: 不要在服務器端同時使用 -w 和 -l 參數，因為 -w 參數將在服務器端無效果。


## curl

curl 用於向服務器傳輸數據，它支持 http、https、ftp、ftps、scp、sftp、tftp、telnet 等協議。

### 常見示例


{% highlight text %}
----- 查看源碼，保存到某個文件；默認輸出到終端
$ curl -o filename www.sina.com

----- 自動調轉到新地址，會自動跳轉為www.sina.com.cn
$ curl -L www.sina.com

----- 顯示頭信息以及網頁代碼，如果只查看頭則可以使用-I參數
$ curl -i www.sina.com

----- 顯示通信過程，通過-v可顯示http通信的整個過程，包括端口連接和http request頭信息
$ curl -v www.sina.com                             # 顯示交互信息
$ curl --trace output.txt www.sina.com             # 顯示更加信息的信息
$ curl --trace-ascii output.txt www.sina.com

----- 發送GET表單
$ curl www.example.com/form.cgi?data=xxx

----- 發送POST表單信息，需要把數據和網址分開，也就是--data參數，通過第一個參數對錶單編碼
$ curl --data-urlencode --data "data=April 1" www.example.com/form.cgi
{% endhighlight %}


<!--
六、文件上傳

假定文件上傳的表單是下面這樣：
<form method="POST" enctype='multipart/form-data' action="upload.cgi">
　　<input type=file name=upload>
　　<input type=submit name=press value="OK">
</form>

你可以用curl這樣上傳文件：
1

curl --form upload=@localfilename --form press=OK <a class="CODE" name="xxx"www.example.com</pre" href="http://www.jobbole.com/entry.php/1370"></a>

七、Referer字段

有時你需要在http request頭信息中，提供一個referer字段，表示你是從哪裡跳轉過來的。
1

curl --referer http://www.example.com http://www.example.com

八、User Agent字段

這個字段是用來表示客戶端的設備信息。服務器有時會根據這個字段，針對不同設備，返回不同格式的網頁，比如手機版和桌面版。

iPhone4的User Agent是
1

Mozilla/5.0 (iPhone; U; CPU iPhone OS 4_0 like Mac OS X; en-us)  AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8A293  Safari/6531.22.7

curl可以這樣模擬：
1

curl --user-agent "[User Agent]" [URL]

九、cookie

使用–cookie參數，可以讓curl發送cookie。
1

curl --cookie "name=xxx"www.example.com

至於具體的cookie的值，可以從http response頭信息的Set-Cookie字段中得到。

十、增加頭信息

有時需要在http request之中，自行增加一個頭信息。–header參數就可以起到這個作用。
1

curl --header "xxx: xxxxxx" http://example.com

十一、HTTP認證

有些網域需要HTTP認證，這時curl需要用到–user參數。
1

curl --user name:password example.com


如何使用curl

curl並不是雙擊即可運行，你需要在命令提示符下使用它
如何進入命令提示符
點擊“開始”——“運行”——輸入CMD

或，WIN+R，輸入CMD

下載我下載版本的CURL後，解壓後文件夾放在如：D:\curl的文件夾裡（curl路徑）

命令提示符窗口中輸入"d:"回車，然後輸入“cd curl”即可跳轉到curl文件夾，

至此可以輸入curl命令了(根據你的curl類推)或你也可以將curl加入到系統環境變量如何將curl所在文件夾加入系統變量
右鍵單擊“我的電腦”——“屬性”——“高級”——“環境變量”——

“系統變量”——“Path”——“編輯”——加入“;D:\curl”(注意分號，

D:\curl換成你的curl路徑)——“確定”加入到系統環境變量後可在命令提示符內直接運行如

“curl -O http://curl.haxx.se/download/curl-7.19.5-win32-ssl-sspi.zip”

這樣的命令，無需進入curl所在文件夾curl命令用法

一個不錯的Curl教程
1)
二話不說，先從這裡開始吧！
curl http://www.yahoo.com

回車之後，www.yahoo.com 的html就稀里嘩啦地顯示在屏幕上了~~~~~

2)
嗯，要想把讀過來頁面存下來，是不是要這樣呢？
curl http://www.yahoo.com > page.html

當然可以，但不用這麼麻煩的！
用curl的內置option就好，存下http的結果，用這個option: -o
curl -o page.html http://www.yahoo.com

這樣，你就可以看到屏幕上出現一個下載頁面進度指示。等進展到100%，自然就OK咯

3)
什麼什麼？！訪問不到？肯定是你的proxy沒有設定了。
使用curl的時候，用這個option可以指定http訪問所使用的proxy服務器及其端口： -x
curl -x 123.45.67.89:1080 -o page.html http://www.yahoo.com


4)
訪問有些網站的時候比較討厭，他使用cookie來記錄session信息。
像IE/NN這樣的瀏覽器，當然可以輕易處理cookie信息，但我們的curl呢？.....
我們來學習這個option: -D <-- 這個是把http的response裡面的cookie信息存到一個特別的文件中去
curl -x 123.45.67.89:1080 -o page.html -D cookie0001.txt http://www.yahoo.com

這樣，當頁面被存到page.html的同時，cookie信息也被存到了cookie0001.txt裡面了


5）
那麼，下一次訪問的時候，如何繼續使用上次留下的cookie信息呢？要知道，很多網站都是靠監視你的cookie信息，
來判斷你是不是不按規矩訪問他們的網站的。
這次我們使用這個option來把上次的cookie信息追加到http request裡面去： -b
curl -x 123.45.67.89:1080 -o page1.html -D cookie0002.txt -b cookie0001.txt http://www.yahoo.com

這樣，我們就可以幾乎模擬所有的IE操作，去訪問網頁了！


6）
稍微等等~~~~~我好像忘記什麼了~~~~~
對了！是瀏覽器信息~~~~

有些討厭的網站總要我們使用某些特定的瀏覽器去訪問他們，有時候更過分的是，還要使用某些特定的版本~~~~
NND，哪裡有時間為了它去找這些怪異的瀏覽器呢！？

好在curl給我們提供了一個有用的option，可以讓我們隨意指定自己這次訪問所宣稱的自己的瀏覽器信息： -A
curl -A "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)" -x 123.45.67.89:1080 -o page.html -D cookie0001.txt http://www.yahoo.com

這樣，服務器端接到訪問的要求，會認為你是一個運行在Windows 2000上的IE6.0，嘿嘿嘿，其實也許你用的是蘋果機呢！

而"Mozilla/4.73 [en] (X11; U; Linux 2.2; 15 i686"則可以告訴對方你是一臺PC上跑著的Linux，用的是Netscape 4.73，呵呵呵


7）
另外一個服務器端常用的限制方法，就是檢查http訪問的referer。比如你先訪問首頁，再訪問裡面所指定的下載頁，這第二次訪問的 referer地址就是第一次訪問成功後的頁面地址。這樣，服務器端只要發現對下載頁面某次訪問的referer地址不是首頁的地址，就可以斷定那是個盜連了~~~~~

討厭討厭~~~我就是要盜連~~~~~！！
幸好curl給我們提供了設定referer的option： -e
curl -A "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)" -x 123.45.67.89:1080 -e "mail.yahoo.com" -o page.html -D cookie0001.txt http://www.yahoo.com

這樣，就可以騙對方的服務器，你是從mail.yahoo.com點擊某個鏈接過來的了，呵呵呵


8）
寫著寫著發現漏掉什麼重要的東西了！----- 利用curl 下載文件

剛才講過了，下載頁面到一個文件裡，可以使用 -o ，下載文件也是一樣。
比如， curl -o 1.jpg http://cgi2.tky.3web.ne.jp/~zzh/screen1.JPG
這裡教大家一個新的option： -O
大寫的O，這麼用： curl -O http://cgi2.tky.3web.ne.jp/~zzh/screen1.JPG
這樣，就可以按照服務器上的文件名，自動存在本地了！

再來一個更好用的。
如果screen1.JPG以外還有screen2.JPG、screen3.JPG、....、screen10.JPG需要下載，難不成還要讓我們寫一個script來完成這些操作？
不幹！
在curl裡面，這麼寫就可以了：
curl -O http://cgi2.tky.3web.ne.jp/~zzh/screen[1-10].JPG

呵呵呵，厲害吧？！~~~

9）
再來，我們繼續講解下載！
curl -O http://cgi2.tky.3web.ne.jp/~/[001-201].JPG

這樣產生的下載，就是
~zzh/001.JPG
~zzh/002.JPG
...
~zzh/201.JPG
~nick/001.JPG
~nick/002.JPG
...
~nick/201.JPG

夠方便的了吧？哈哈哈

咦？高興得太早了。
由於zzh/nick下的文件名都是001，002...，201，下載下來的文件重名，後面的把前面的文件都給覆蓋掉了~~~

沒關係，我們還有更狠的！
curl -o #2_#1.jpg http://cgi2.tky.3web.ne.jp/~/[001-201].JPG

--這是.....自定義文件名的下載？
--對頭，呵呵！

#1是變量，指的是這部分，第一次取值zzh，第二次取值nick
#2代表的變量，則是第二段可變部分---[001-201]，取值從001逐一加到201
這樣，自定義出來下載下來的文件名，就變成了這樣：
原來： ~zzh/001.JPG --- 下載後： 001-zzh.JPG
原來： ~nick/001.JPG --- 下載後： 001-nick.JPG

這樣一來，就不怕文件重名啦，呵呵

9）
繼續講下載
我們平時在windows平臺上，flashget這樣的工具可以幫我們分塊並行下載，還可以斷線續傳。
curl在這些方面也不輸給誰，嘿嘿

比如我們下載screen1.JPG中，突然掉線了，我們就可以這樣開始續傳
curl -c -O http://cgi2.tky.3wb.ne.jp/~zzh/screen1.JPG

當然，你不要拿個flashget下載了一半的文件來糊弄我~~~~別的下載軟件的半截文件可不一定能用哦~~~

分塊下載，我們使用這個option就可以了： -r
舉例說明
比如我們有一個http://cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3 要下載（趙老師的電話朗誦 :D ）
我們就可以用這樣的命令：
curl -r 0-10240 -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3 &\
curl -r 10241-20480 -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3 &\
curl -r 20481-40960 -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3 &\
curl -r 40961- -o "zhao.part1" http:/cgi2.tky.3web.ne.jp/~zzh/zhao1.mp3

這樣就可以分塊下載啦。
不過你需要自己把這些破碎的文件合併起來
如果你用UNIX或蘋果，用 cat zhao.part* > zhao.mp3就可以
如果用的是Windows，用copy /b 來解決吧，呵呵

上面講的都是http協議的下載，其實ftp也一樣可以用。
用法嘛，
curl -u name:passwd ftp://ip:port/path/file
或者大家熟悉的
curl ftp://name:passwd@ip:port/path/file



10)
說完了下載，接下來自然該講上傳咯
上傳的option是 -T

比如我們向ftp傳一個文件： curl -T localfile -u name:passwd ftp://upload_site:port/path/

當然，向http服務器上傳文件也可以
比如 curl -T localfile http://cgi2.tky.3web.ne.jp/~zzh/abc.cgi
注意，這時候，使用的協議是HTTP的PUT method

剛才說到PUT，嘿嘿，自然讓老服想起來了其他幾種methos還沒講呢！
GET和POST都不能忘哦。

http提交一個表單，比較常用的是POST模式和GET模式

GET模式什麼option都不用，只需要把變量寫在url裡面就可以了
比如：
curl http://www.yahoo.com/login.cgi?user=nickwolfe&password=12345

而POST模式的option則是 -d

比如，curl -d "user=nickwolfe&password=12345" http://www.yahoo.com/login.cgi
就相當於向這個站點發出一次登陸申請~~~~~

到底該用GET模式還是POST模式，要看對面服務器的程序設定。

一點需要注意的是，POST模式下的文件上的文件上傳，比如
<form method="POST" enctype="multipar/form-data" action="http://cgi2.tky.3web.ne.jp/~zzh/up_file.cgi">
<input type=file name=upload>
<input type=submit name=nick value="go">
</form>
這樣一個HTTP表單，我們要用curl進行模擬，就該是這樣的語法：
curl -F upload=@localfile -F nick=go http://cgi2.tky.3web.ne.jp/~zzh/up_file.cgi

羅羅嗦嗦講了這麼多，其實curl還有很多很多技巧和用法
比如 https的時候使用本地證書，就可以這樣
curl -E localcert.pem https://remote_server

再比如，你還可以用curl通過dict協議去查字典~~~~~
curl dict://dict.org/d:computer
-->








{% highlight text %}
{% endhighlight %}
