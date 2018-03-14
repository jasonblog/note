---
title: Linux 網絡設置
layout: post
comments: true
category: [linux, network]
language: chinese
keywords: linux,網絡設置
description: 主要記錄下在 Linux 中，一些常見的網絡配置，例如 IP 地址、路由、MAC 地址、主機名等設置方式。
---


<!-- more -->

![ftp logo]({{ site.url }}/images/network/ftp-logo.jpg "ftp logo"){: .pull-center width="70%" }

## 簡介

FTP 服務器登錄通常有三種不同的身份，分別是: 1) 實體賬號 real user；2) 訪客 guest；3) 匿名用戶 anonymous。

通常實體帳號擁有較多的權限，而匿名用戶通常只有上傳和下載的功能。

### 安裝使用

可以通過如下步驟安裝 FTP 服務器。

{% highlight text %}
----- 安裝服務器以及客戶端
# yum install vsftpd ftp

----- 啟動服務器
# systemctl start vsftpd

----- 使用客戶端
$ ftp user@hostname
ftp> open 127.1         # 打開鏈接，需要用戶登陸
ftp> pwd                # 查看當前目錄，一般默認是$HOME
ftp> ls                 # 查看目錄以及文件
ftp> cd /some/dir       # 切換目錄
ftp> lcd /some/dir      # 切換本地目錄
ftp> get /some/file     # 下載文件
ftp> mget *.txt         # 同時下載多個文件

ftp> put file           # 上傳文件
ftp> mput *.txt
{% endhighlight %}

也可以使用 `anonymous` 以及空密碼登陸。

<!--
ascii: 使用ascii類型傳輸方式。
bin: 使用二進制文件傳輸方式。
close: 中斷與遠程服務器的ftp會話(與open對應)。
delete remote-file: 刪除遠程主機文件。
mkdir: 創建目錄
mput local-files: 上傳多個文件
open host[port]: 建立指定ftp服務器連接，可指定連接端口。
put local-file[remote-file]: 上傳文件
$ macro-ame[args]： 執行宏定義macro-name。
account[password]： 提供登錄遠程系統成功後訪問系統資源所需的補充口令。
append local-file[remote-file]：將本地文件追加到遠程系統主機，若未指定遠程系統文件名，則使用本地文件名。
bell：每個命令執行完畢後計算機響鈴一次。
bye：退出ftp會話過程。
case：在使用mget時，將遠程主機文件名中的大寫轉為小寫字母。
cd remote-dir：進入遠程主機目錄。
cdup：進入遠程主機目錄的父目錄。
chmod mode file-name：將遠程主機文件file-name的存取方式設置為mode，如：chmod 777 a.out。

cr：使用asscii方式傳輸文件時，將回車換行轉換為回行。
debug[debug-value]：設置調試方式， 顯示發送至遠程主機的每條命令，如：deb up 3，若設為0，表示取消debug。
dir[remote-dir][local-file]：顯示遠程主機目錄，並將結果存入本地文件
disconnection：同close。
form format：將文件傳輸方式設置為format，缺省為file方式。
get remote-file[local-file]： 將遠程主機的文件remote-file傳至本地硬盤的local-file。
glob：設置mdelete，mget，mput的文件名擴展，缺省時不擴展文件名，同命令行的-g參數。
hash：每傳輸1024字節，顯示一個hash符號(#)。
help[cmd]：顯示ftp內部命令cmd的幫助信息，如：help get。
idle[seconds]：將遠程服務器的休眠計時器設為[seconds]秒。
image：設置二進制傳輸方式(同binary)。
lcd[dir]：將本地工作目錄切換至dir。
ls[remote-dir][local-file]：顯示遠程目錄remote-dir， 並存入本地文件local-file。
macdef macro-name：定義一個宏，遇到macdef下的空行時，宏定義結束。
mdelete[remote-file]：刪除遠程主機文件。
mdir remote-files local-file：與dir類似，但可指定多個遠程文件，如 ：mdir *.o.*.zipoutfile 。
mget remote-files：傳輸多個遠程文件。
mkdir dir-name：在遠程主機中建一目錄。
mls remote-file local-file：同nlist，但可指定多個文件名。
mode[modename]：將文件傳輸方式設置為modename， 缺省為stream方式。
modtime file-name：顯示遠程主機文件的最後修改時間。
mput local-file：將多個文件傳輸至遠程主機。
newer file-name： 如果遠程機中file-name的修改時間比本地硬盤同名文件的時間更近，則重傳該文件。
nlist[remote-dir][local-file]：顯示遠程主機目錄的文件清單，並存入本地硬盤的local-file。
nmap[inpattern outpattern]：設置文件名映射機制， 使得文件傳輸時，文件中的某些字符相互轉換， 如：nmap $1.$2.$3[$1，$2].[$2，$3]，則傳輸文件a1.a2.a3時，文件名變為a1，a2。 該命令特別適用於遠程主機為非UNIX機的情況。
ntrans[inchars[outchars]]：設置文件名字符的翻譯機制，如ntrans1R，則文件名LLL將變為RRR。
passive：進入被動傳輸方式。
prompt：設置多個文件傳輸時的交互提示。
proxy ftp-cmd：在次要控制連接中，執行一條ftp命令， 該命令允許連接兩個ftp服務器，以在兩個服務器間傳輸文件。第一條ftp命令必須為open，以首先建立兩個服務器間的連接。
put local-file[remote-file]：將本地文件local-file傳送至遠程主機。
pwd：顯示遠程主機的當前工作目錄。
quit：同bye，退出ftp會話。
quote arg1，arg2...：將參數逐字發至遠程ftp服務器，如：quote syst.
recv remote-file[local-file]：同get。
reget remote-file[local-file]：類似於get， 但若local-file存在，則從上次傳輸中斷處續傳。
rhelp[cmd-name]：請求獲得遠程主機的幫助。
rstatus[file-name]：若未指定文件名，則顯示遠程主機的狀態， 否則顯示文件狀態。
rename[from][to]：更改遠程主機文件名。
reset：清除回答隊列。
restart marker：從指定的標誌marker處，重新開始get或put，如：restart 130。
rmdir dir-name：刪除遠程主機目錄。
runique：設置文件名只一性存儲，若文件存在，則在原文件後加後綴.1， .2等。
send local-file[remote-file]：同put。
sendport：設置PORT命令的使用。
site arg1，arg2...：將參數作為SITE命令逐字發送至遠程ftp主機。
size file-name：顯示遠程主機文件大小，如：site idle 7200。
status：顯示當前ftp狀態。
struct[struct-name]：將文件傳輸結構設置為struct-name， 缺省時使用stream結構。
sunique：將遠程主機文件名存儲設置為只一(與runique對應)。
system：顯示遠程主機的操作系統類型。
tenex：將文件傳輸類型設置為TENEX機的所需的類型。
tick：設置傳輸時的字節計數器。
trace：設置包跟蹤。
type[type-name]：設置文件傳輸類型為type-name，缺省為ascii，如:type binary，設置二進制傳輸方式。
umask[newmask]：將遠程服務器的缺省umask設置為newmask，如：umask 3
user user-name[password][account]：向遠程主機表明自己的身份，需要口令時，必須輸入口令，如：user anonymous my@email。
verbose：同命令行的-v參數，即設置詳盡報告方式，ftp 服務器的所有響 應都將顯示給用戶，缺省為on.
?[cmd]：同help.
下載cmd  中登入後，用get 文件名即可下載，下載文件放在才C:\Documents and Settings\Administrator
上傳put e:\linghongli.txt即可上傳了
-->

## 鏈接模式

FTP 在建立鏈接時需要兩個通道，分別為命令通道和數據通道，命令通道通常為 21 號端口，數據通道通常為 22 號端口。FTP 在建立鏈接時分為主動和被動鏈接，注意所謂主動被動是在建立數據通道時對於服務器而言。

### 主動模式

![ftp connect active]({{ site.url }}/images/network/ftp-connect-active.gif  "ftp connect active"){: .pull-center }

FTP 默認是主動鏈接，鏈接過程如下。

1. 建立命令通道，客戶端與 FTP 服務器端的 port 21 通過三次握手鍊接，建立鏈接後客戶端便可以通過這個通道來對 FTP 服務器下達指令，例如包括查詢文件名、下載、上傳等。

<!--
2. 客戶端發出Active鏈接請求且告知端口號
	通知 FTP 服務器端使用 active 且告知連接的端口， FTP 服務器的 21 埠號主要用在命令的下達，當牽涉到數據流時，就不是使用這個通道了。客戶端在需要數據的情況下，會告知服務器端要用什麼方式來聯機，如果是主動式 (active) 聯機時，客戶端會先隨機啟用一個端口 (port BB) ，且透過命令通道告知 FTP 服務器這兩個信息，並等待 FTP 服務器的聯機。</li><li>

<font color="blue">FTP 服務器『主動』向客戶端聯機</font><br />
FTP 服務器由命令通道瞭解客戶端的需求後，會主動的由 20 這個端口向客戶端的 port BB 聯機。此時 FTP 的客戶端與服務器端共會建立兩條聯機，分別用在命令的下達與數據的傳遞。</li></ol>
-->





{% highlight text %}
{% endhighlight %}
