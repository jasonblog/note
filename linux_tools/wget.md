# wget 文件下載
Linux系統中的wget是一個下載文件的工具，它用在命令行下。對於Linux用戶是必不可少的工具，我們經常要下載一些軟件或從遠程服務器恢復備份到本地服務器。wget支持HTTP，HTTPS和FTP協議，可以使用HTTP代理。

wget 可以跟蹤HTML頁面上的鏈接依次下載來創建遠程服務器的本地版本，完全重建原始站點的目錄結構。這又常被稱作”遞歸下載”。在遞歸下載的時候，wget 遵循Robot Exclusion標準(/robots.txt). wget可以在下載的同時，將鏈接轉換成指向本地文件，以方便離線瀏覽。

wget 非常穩定，它在帶寬很窄的情況下和不穩定網絡中有很強的適應性.如果是由於網絡的原因下載失敗，wget會不斷的嘗試，直到整個文件下載完畢。如果是服務器打斷下載過程，它會再次聯到服務器上從停止的地方繼續下載。這對從那些限定了鏈接時間的服務器上下載大文件非常有用。

### 17.1. 命令格式
wget [參數] [URL地址]

### 17.2. 命令參數：
啓動參數：
- -V, –version 顯示wget的版本後退出
- -h, –help 打印語法幫助
- -b, –background 啓動後轉入後臺執行
- -e, –execute=COMMAND 執行’.wgetrc’格式的命令，wgetrc格式參見/etc/wgetrc或~/.wgetrc

### 記錄和輸入文件參數
- -o, –output-file=FILE 把記錄寫到FILE文件中
- -a, –append-output=FILE 把記錄追加到FILE文件中
- -d, –debug 打印調試輸出
- -q, –quiet 安靜模式(沒有輸出)
- -v, –verbose 冗長模式(這是缺省設置)
- -nv, –non-verbose 關掉冗長模式，但不是安靜模式
- -i, –input-file=FILE 下載在FILE文件中出現的URLs
- -F, –force-html 把輸入文件當作HTML格式文件對待
- -B, –base=URL 將URL作爲在-F -i參數指定的文件中出現的相對鏈接的前綴

–sslcertfile=FILE 可選客戶端證書 –sslcertkey=KEYFILE 可選客戶端證書的KEYFILE –egd-file=FILE 指定EGD socket的文件名

### 下載參數
- -bind-address=ADDRESS 指定本地使用地址(主機名或IP，當本地有多個IP或名字時使用)
- -t, –tries=NUMBER 設定最大嘗試鏈接次數(0 表示無限制).
- -O –output-document=FILE 把文檔寫到FILE文件中
- -nc, –no-clobber 不要覆蓋存在的文件或使用.#前綴
- -c, –continue 接着下載沒下載完的文件
- -progress=TYPE 設定進程條標記
- -N, –timestamping 不要重新下載文件除非比本地文件新
- -S, –server-response 打印服務器的迴應
- -T, –timeout=SECONDS 設定響應超時的秒數
- -w, –wait=SECONDS 兩次嘗試之間間隔SECONDS秒
- -waitretry=SECONDS 在重新鏈接之間等待1…SECONDS秒
- -random-wait 在下載之間等待0…2*WAIT秒
- -Y, -proxy=on/off 打開或關閉代理
- -Q, -quota=NUMBER 設置下載的容量限制
- -limit-rate=RATE 限定下載輸率

### 目錄參數
- -nd –no-directories 不創建目錄
- -x, –force-directories 強制創建目錄
- -nH, –no-host-directories 不創建主機目錄
- -P, –directory-prefix=PREFIX 將文件保存到目錄 PREFIX/…
- -cut-dirs=NUMBER 忽略 NUMBER層遠程目錄
- HTTP 選項參數
- -http-user=USER 設定HTTP用戶名爲 USER.
- -http-passwd=PASS 設定http密碼爲 PASS
- -C, –cache=on/off 允許/不允許服務器端的數據緩存 (一般情況下允許)
- -E, –html-extension 將所有text/html文檔以.html擴展名保存
- -ignore-length 忽略 ‘Content-Length’頭域
- -header=STRING 在headers中插入字符串 STRING
- -proxy-user=USER 設定代理的用戶名爲 USER
- proxy-passwd=PASS 設定代理的密碼爲 PASS
- referer=URL 在HTTP請求中包含 ‘Referer: URL’頭
- -s, –save-headers 保存HTTP頭到文件
- -U, –user-agent=AGENT 設定代理的名稱爲 AGENT而不是 Wget/VERSION
- no-http-keep-alive 關閉 HTTP活動鏈接 (永遠鏈接)
- cookies=off 不使用 cookies
- load-cookies=FILE 在開始會話前從文件 FILE中加載cookie
- save-cookies=FILE 在會話結束後將 cookies保存到 FILE文件中

### FTP 選項參數
- -nr, –dont-remove-listing 不移走 ‘.listing’文件
- -g, –glob=on/off 打開或關閉文件名的 globbing機制
- passive-ftp 使用被動傳輸模式 (缺省值).
- active-ftp 使用主動傳輸模式
- retr-symlinks 在遞歸的時候，將鏈接指向文件(而不是目錄)

### 遞歸下載參數
- -r, –recursive 遞歸下載－－慎用!
- -l, –level=NUMBER 最大遞歸深度 (inf 或 0 代表無窮)
- -delete-after 在現在完畢後局部刪除文件
- -k, –convert-links 轉換非相對鏈接爲相對鏈接
- -K, –backup-converted 在轉換文件X之前，將之備份爲 X.orig
- -m, –mirror 等價於 -r -N -l inf -nr
- -p, –page-requisites 下載顯示HTML文件的所有圖片
- 遞歸下載中的包含和不包含(accept/reject)：
-
- -A, –accept=LIST 分號分隔的被接受擴展名的列表
- -R, –reject=LIST 分號分隔的不被接受的擴展名的列表
- -D, –domains=LIST 分號分隔的被接受域的列表
- -exclude-domains=LIST 分號分隔的不被接受的域的列表
- -follow-ftp 跟蹤HTML文檔中的FTP鏈接
- -follow-tags=LIST 分號分隔的被跟蹤的HTML標籤的列表
- -G, –ignore-tags=LIST 分號分隔的被忽略的HTML標籤的列表
- -H, –span-hosts 當遞歸時轉到外部主機
- -L, –relative 僅僅跟蹤相對鏈接
- -I, –include-directories=LIST 允許目錄的列表
- -X, –exclude-directories=LIST 不被包含目錄的列表
- -np, –no-parent 不要追溯到父目錄

wget -S –spider url 不下載只顯示過程

### 17.3. 使用實例
### 實例1：使用wget下載單個文件
```
$wget http://www.minjieren.com/wordpress-3.1-zh_CN.zip
```

說明：以上例子從網絡下載一個文件並保存在當前目錄，在下載的過程中會顯示進度條，包含（下載完成百分比，已經下載的字節，當前下載速度，剩餘下載時間）。

### 實例2：使用wget -O下載並以不同的文件名保存
```
$wget -O wordpress.zip http://www.minjieren.com/download.aspx?id=1080
```

wget默認會以最後一個符合”/”的後面的字符來命令，對於動態鏈接的下載通常文件名會不正確。

### 實例3：使用wget –limit -rate限速下載
```
$wget --limit-rate=300k http://www.minjieren.com/wordpress-3.1-zh_CN.zip
```

當你執行wget的時候，它默認會佔用全部可能的寬帶下載。但是當你準備下載一個大文件，而你還需要下載其它文件時就有必要限速了。

### 實例4：使用wget -c斷點續傳
```
$wget -c http://www.minjieren.com/wordpress-3.1-zh_CN.zip
```
使用wget -c重新啓動下載中斷的文件，對於我們下載大文件時突然由於網絡等原因中斷非常有幫助，我們可以繼續接着下載而不是重新下載一個文件。需要繼續中斷的下載時可以使用-c參數。

### 實例5：使用wget -b後臺下載
```
$wget -b http://www.minjieren.com/wordpress-3.1-zh_CN.zip
Continuing in background, pid 1840.
Output will be written to 'wget-log'.
```
對於下載非常大的文件的時候，我們可以使用參數-b進行後臺下載。

你可以使用以下命令來察看下載進度:
```
$tail -f wget-log
```
### 實例6：僞裝代理名稱下載
```
wget --user-agent="Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) AppleWebKit/534.16 (KHTML, like Gecko) Chrome/10.0.648.204 Safari/534.16" http://www.minjieren.com/wordpress-3.1-zh_CN.zip
```
有些網站能通過根據判斷代理名稱不是瀏覽器而拒絕你的下載請求。不過你可以通過–user-agent參數僞裝。

### 實例7：使用wget -i下載多個文件
首先，保存一份下載鏈接文件,接着使用這個文件和參數-i下載:
```
$cat > filelist.txt
url1
url2
url3
url4

$wget -i filelist.txt
```
### 實例8：使用wget –mirror鏡像網站
```
$wget --mirror -p --convert-links -P ./LOCAL URL
```

下載整個網站到本地
- -miror:開戶鏡像下載
- -p:下載所有爲了html頁面顯示正常的文件
- -convert-links:下載後，轉換成本地的鏈接
- -P ./LOCAL：保存所有文件和目錄到本地指定目錄
### 實例9: 使用wget -r -A下載指定格式文件
```
$wget -r -A.pdf url
```

可以在以下情況使用該功能：
- 下載一個網站的所有圖片
- 下載一個網站的所有視頻
- 下載一個網站的所有PDF文件

### 實例10：使用wget FTP下載
```
$wget ftp-url
$wget --ftp-user=USERNAME --ftp-password=PASSWORD url
```

可以使用wget來完成ftp鏈接的下載
- 使用wget匿名ftp下載：wget ftp-url
- 使用wget用戶名和密碼認證的ftp下載:wget –ftp-user=USERNAME –ftp-password=PASSWORD url

### 17.4. 編譯安裝
使用如下命令編譯安裝:
```
tar zxvf wget-1.9.1.tar.gz
cd wget-1.9.1
./configure
make
make install
```
