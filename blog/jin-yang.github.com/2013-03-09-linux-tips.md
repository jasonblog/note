---
title: Linux 常用技巧
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,bash,技巧,經典
description: 簡單記錄下在 Linux 下常用的一些技巧，以方便查詢使用，例如 生成隨機字符串，特殊字符文件的處理， sudo 和 su 兩個命令的區別等等。
---

簡單記錄下在 Linux 下常用的一些技巧，以方便查詢使用，例如 生成隨機字符串，特殊字符文件的處理， sudo 和 su 兩個命令的區別等等。

<!-- more -->


## 隨機內容

在 Linux 中 ```/dev/urandom``` 可以用來產生真隨機的內容，然後直接讀取字符串的內容，從而生成隨機的字符串。

如下的命令中，C 表示生成的字符串的字符數；L 表示要生成多少行隨機字符。

{% highlight text %}
----- 生成全字符隨機的字串
$ cat /dev/urandom | strings -n C | head -n L

----- 生成數字加字母的隨機字串
$ cat /dev/urandom | sed 's/[^a-zA-Z0-9]//g' | strings -n C | head -n L

----- 也可以直接通過Base64轉換
$ head -c 32 /dev/random | base64
{% endhighlight %}

注意，如上的方式中會存在換行符，只有在出現長度是 ```C``` 的字符串時才打印，會導致打印速度很慢；所以，最好是通過如下方式生成，然後手動截取。

{% highlight text %}
$ cat /dev/urandom | sed 's/[^a-zA-Z0-9]//g' | strings | tr -d '\n\r'
{% endhighlight %}

如果要生成隨機整數，可以通過 shell 中的一個環境變量 ```RANDOM ```，它的範圍是 ```0~32767``` 。

{% highlight text %}
----- 生成 [0, 25] 的隨機數
$ echo $(($RANDOM%26))

----- 生成 [6, 100] 的隨機數
$ echo $(($RANDOM%95+6))
{% endhighlight %}

另外，```/dev/``` 目錄下存在兩個相關的隨機數發生器，也就是 ```random``` 和 ```urandom``` ，後者也就是 ```unblocked random``` ，可以通過如下方式測試其速度。

{% highlight text %}
$ dd if=/dev/random of=random.dat bs=512 count=5
$ dd if=/dev/urandom of=urandom.dat bs=512 count=5
{% endhighlight %}

相比來說，前者在內核中熵不足時會阻塞，而後者則不會，詳細可以查看 [Myths about /dev/urandom](https://www.2uo.de/myths-about-urandom/) 。

<!--
可以參考本地文檔Myths about /dev/urandom
/reference/linux/kernel/myths_about_dev_urandom.maff


Linux 中的隨機數可從 /dev/{random,urandom} 兩個特殊的設備文件中產生，會利用當前系統的熵池計算出一定數量的隨機比特，然後將這些比特作為字節流返回。熵池就是當前系統的環境噪音，可以通過很多參數來評估，如內存、文件的使用、不同類型的進程數量等等。

如果當前環境噪音變化的不是很劇烈或者當前環境噪音很小，比如剛開機的時候，而當前需要大量的隨機比特，這時產生的隨機數的隨機效果就不是很好了。

這也就是上述兩個文件的區別，前者不能產生新的隨機數時會阻塞，直到根據熵池產生新的隨機字節之後才返回；而後者不會阻塞，只是此時產生的可能是偽隨機數，對加密解密這樣的應用來說就不是一種很好的選擇。

所以使用 `/dev/random` 比使用 `/dev/urandom` 產生大量隨機數的速度要慢。

$ dd if=/dev/random of=random.dat bs=1024b count=1
0+1 records in
0+1 records out
128 bytes (128 B) copied, 0.000169 seconds, 757 kB/s

$ dd if=/dev/urandom of=random.dat bs=1024b count=1
1+0 records in
1+0 records out
524288 bytes (524 kB) copied, 0.091297 seconds, 5.7 MB/s
-->


## 特殊字符文件處理

在 Linux 中，文件名的長度最大可以達到 256 個字符，可以使用字符有：字母、數字、```'.'```(點)、```'_'```(下劃線)、```'-'```(連字符)、```' '```(空格)，其中開始字符不建議使用 ```'_'```、```'-'```、```' '``` 字符。```'/'```(反斜線) 用於標示目錄，不能用作文件或者文件夾名稱。

另外，在 shell 中，```'?'```(問號)、```'*'```(星號)、```'&'``` 字符有特殊含義，同樣不建議使用。

在 shell 中，將 ```--``` 之後的內容當作文件。

{% highlight text %}
$ cd .>-a                             ← 創建一個文件，或者 >-a
$ vi -- -a                            ← 編輯，或者 echo "">-a
$ rm -- -a                            ← 刪除，或者 rm ./-a
$ touch '><!*'                        ← 創建
$ touch '?* $&'                       ← 創建
{% endhighlight %}

對於這樣的文件，可以執行如下操作。

{% highlight text %}
----- 將非亂碼的文件移出到某個目錄下
$ find . -name "[a-z|A-Z]*" | xargs -I {} mv {} /somepath

----- 也可以通過inode刪除
$ ls -i
$ find -inum XXX | xargs -I {} rm {}
$ find -inum XXX -delete
{% endhighlight %}

如果文件的文件名含有終端無法正確顯示的字符，那麼可以通過 inode 來刪除，處理命令如下。

{% highlight text %}
----- 查看文件innode
# ls -li
total 0
358315 -rw-r--r-- 1 root root 0 Apr 6 23:13 ???}

----- 通過inode刪除文件，如下兩種方式相同
# find . -inum 358315 -delete
# rm -i `find . -maxdepth 1 -inum 358315 -print`
{% endhighlight %}


## sudo VS. su

在切換時實際上是兩種策略：1) su 切換到相應的用戶，所以需要切換用戶的密碼；2) sudo 不知道 root 密碼的時候執行一些 root 的命令，需要在 suders 中配置+自己用戶密碼。

{% highlight text %}
$ sudo su root                        ← 需要用戶的密碼+sudoers配置
$ su root                             ← 需要root用戶密碼
{% endhighlight %}

注意，之所以使用 ```sudo su root``` 這種方式，可能是 btmp 等類似的文件，只有 root 可以寫入，否則會報 Permission Denied，此時可以通過 strace 查看報錯的文件。

## 文本替換

命令行批量替換多文件中的字符串，常用有三種方法：Mahuinan 法、Sumly 法和 30T 法。

{% highlight text %}
----- Mahuinan法，用sed命令批量替換多個文件中的字符串
$ sed -i "s/orig/sub/g" "`grep orig -rl directory`"
解讀：
    sed -i 選項表示原地替換，若只顯示結果則用-e替換；
    g 表示全局，否則只替換第一個匹配字符串；
    grep -r 表示對目錄遞歸調用；-l(L小寫)列出匹配的文件。

----- Sumly法
$ perl -pi -e "s/China/Sumly/g" /www/*.htm /www/*.txt
解讀：
    將www文件夾下所有的htm和txt文件中的"China"都替換為"Sumly"

----- 30T法
$ perl -pi -e 's/baidu/30T/g' `find /www -type f`
解讀：
　　將www文件夾下所有文件，不分擴展名，所有的"baidu"都替換為"30T"
{% endhighlight %}

注意，在 Mahuinan 中，為了防止文件名中存在空格，所以使用引號包裹。

<!--
splint替換，首先通過 $ sed -n "/\/\*@[a-z]\{4,20\}@\*\//p" null1.c進行測試，然後對grep進行測試
$ grep "\/\*@[a-z]\{1,\}@\*\/" -lr .  為防止文件名之間由空格，將其由雙引號包裹。 最後的實際命令為：
$ sed -i "s/\/\*@[a-z]\{1,\}@\*\/ //g"  "`grep "\/\*@[a-z]\{1,\}@\*\/" -lr .` "
-->


## install 和 cp 命令

兩者都可以將文件/目錄拷貝到指定的目錄，不過 install 允許你控制目標文件的屬性，常用於程序的 Makefile 文件。

{% highlight text %}
install [OPTION]... SOURCE DIRECTORY
常用參數：
    -m, --mode=0700
        設定權限；
    -v, --verbose
        打印處理的每個文件/目錄名稱；
    -d, --directory
        所有參數都作為目錄處理，而且會創建指定目錄；
    -g, --group=mysql
        設定所屬組，而不是進程目前的所屬組；
    -o, --owner=mysql
        設定所有者，只適用於超級用戶；
    -s, --strip
        使用strip命令刪除文件中的符號表 (symbol table)；
    -S, --suffix=exe
        指定安裝文件的後綴；
    -p, --preserve-timestamps
        以源文件的訪問/修改時間作為相應的目的地文件的時間屬性；
    -t, --target-directory
        如果最後一個文件是一個目錄並且沒有使用-T,--no-target-directory選項，
        則將每個源文件拷貝到指定的目錄，源和目標文件名相同；
{% endhighlight %}

<!--
--backup[=CONTROL]：為每個已存在的目的地文件進行備份。 -b：類似 --backup，但不接受任何參數。
-->

使用 -d 選項時，如果指定安裝位置為 /usr/local/foo/bar，一般 /usr/loacal 已存在，install 會直接創建 foo/bar 目錄，並把程序安裝到指定位置。

如果指定了兩個文件名，則將第一個文件拷貝到第二個,

{% highlight text %}
----- 安裝目錄，等價於mkdir /tmp/bin
$ install --verbose -d -m 0755 /tmp/bin

----- 安裝文件，等價於cp a/e c
$ install -v -m 0755 a/e c

----- 安裝文件，等價於mkdir -p a/b && cp x a/b/c
$ install -v -m 0755 -D x a/b/c
{% endhighlight %}

## Here Document

可以使用 echo 添加到文件，不過這樣會比較麻煩，可以使用如下方式；不過 ```$``` 需要做轉義，或者使用 ```"EOF"``` 也可以。

{% highlight text %}
$ cat << EOF >> /tmp/foobar.conf
net.core.rmem_default = 262144
net.core.rmem_max = 262144
net.core.wmem_default = 262144
net.core.wmem_max = 262144
export PATH=\$PATH:\$HOME/bin
EOF
{% endhighlight %}

在此使用的就是 Here Document，這是一種在 Linux Shell 中的一種特殊的重定向方式，它的基本的形式如下，通常 delimiter 使用 EOF ：

{% highlight text %}
cmd << delimiter
  Here Document Content
delimiter
{% endhighlight %}

也可以在終端中輸入 ```cat << EOF``` ，然後輸入多行信息，最終以 EOF 結束，其中間輸入的信息將會回顯在屏幕上。

另外，可以通過 ```<<-``` 刪除 Here Document 的內容部分每行前面的 tab (製表符) ， 這種用法是為了編寫 Here Document 的時候可以將內容部分進行縮進，方便閱讀代碼。


## 避免誤刪目錄


今天就來聊聊 linux 下一個常見的問題：如何避免誤刪目錄。下文會詳細的講述不同的場景下誤刪目錄，以及相應的解決方案。

{% highlight bash %}
#--- 1. 變量為空導致誤刪文件，如果file為空或命令返回空
rm -rf /usr/sbin/$file
#--- 使用變量擴展功能，變量為空使用默認值或拋出異常退出
rm -rf /usr/sbin/${file:?var is empty}
#--- 人肉判斷變量是否為空
[[ ${file} == "" ]] && echo 1
[[ -z ${file} ]] && echo 1

#--- 2. 路徑含有空格導致誤刪文件
path="/usr/local /sbin"
rm -rf $path
#--- 變量加引號防止擴展
rm -rf "$path"

#--- 3. 目錄或文件含有特殊字符導致誤刪文件，例如 "~"
#--- 變量加引號防止擴展
rm -rf "~"

#--- 4. cd切換目錄失敗，導致文件被誤刪
#--- 使用邏輯短路操作
cd path && rm -rf *.exe
#--- 檢測path是否存在
[[ -d path ]] && echo 1
{% endhighlight %}


## man

Linux 上的 manpage 是用 groff 語法編寫的，實際上可以通過如下的命令查看：

{% highlight text %}
zcat man.1.gz | groff -man -Tascii | more
{% endhighlight %}

其查找路徑可以通過 ```man -w``` 命令查看，或者查看配置文件 /etc/man.config；很多幫助文檔保存在 /usr/share/man/ 目錄下，又按照不同類型保存不同的子目錄下，例如 /usr/share/man/man1/mysqlshow.1.gz 。

如下是常見的查看命令：

{% highlight text %}
----- 查看搜索路徑
$ man -w
----- 顯示man命令搜索到的第一個文件路徑
$ man -w passwd
----- 顯示所有匹配的man文檔
$ man -aw passwd

----- 指定領域限制
$ man 5 passwd
$ man -S 1:2 passwd

----- 同命令whatis ，將在whatis數據庫查找以關鍵字開同的幫助索引信息
$ man -f httpd
----- 同命令apropos 將搜索whatis數據庫，模糊查找關鍵字
$ man -k httpd
{% endhighlight %}




## 備份腳本

如下是一個備份用的腳本，不過 email 沒有調試使用過，暫時記錄下。

* `~/.backuprc` 配置文件，列舉出那些文件需要備份，使用 `#` 做註釋；
* 使用 `~/tmp` 作為臨時目錄；

另外，在使用 tar 備份打包+解壓時，默認為相對路徑，為了使用絕對路徑可以在壓縮+解壓時都使用 ```-P``` 參數，這樣直接解壓即可覆蓋原有文件。

{% highlight bash %}
#!/bin/bash
# mybackup - Backup selected files & directories and email them as .tar.gz file to
# your email account.
# List of BACKUP files/dirs stored in file ~/.mybackup

FILE=~/.backuprc
NOW=`date +"%d-%m-%Y"`
OUT="`echo $USER.$HOSTNAME`.$NOW.tar.gz"
TAR=/usr/bin/tar

## mail setup
#MTO="nixbackup@somedom.com"
#MSUB="Backup (`echo $USER @ $HOSTNAME`) as on `date`"
#MES=~/tmp/mybackup.txt
#MATT=~/tmp/$OUT

# make sure we put backup in our own tmp and not in /tmp
[ ! -d ~/tmp ] && mkdir ~/tmp || :
if [ -f $FILE ]; then
    IN="`cat $FILE | grep -E -v "^#"`"
else
    echo "File $FILE does not exists"
    exit 3
fi

if [ "$IN" == "" ]; then
    echo "$FILE is empty, please add list of files/directories to backup"
    exit 2
fi

$TAR -zcPf ~/tmp/$OUT $IN >/dev/null

## create message for mail
#echo "Backup successfully done. Please see attached file." > $MES
#echo "" >> $MES
#echo "Backup file: $OUT" >> $MES
#echo "" >> $MES
#
## bug fix, we can't send email with attachment if mutt is not installed
#which mutt > /dev/null
#if [ $? -eq 0 ]; then
#    # now mail backup file with this attachment
#    mutt -s "$MSUB" -a "$MATT" $MTO < $MES
#else
#    echo "Command mutt not found, cannot send an email with attachment"
#fi
#
## clean up
#/bin/rm -f $MATT
#/bin/rm -f $MES
{% endhighlight %}

如下，是一個配置文件。

{% highlight text %}
/home/foobar/.vimrc
/home/foobar/.tmux
/home/foobar/.tmux.conf
{% endhighlight %}


## 日誌清理腳本

在 Linux 中可以通過 logrotate 對日誌進行歸檔，如下是一個日誌清理的腳本。

{% highlight bash %}
#!/bin/sh
# log cleaner.

# location of logs lies
LOGPATH=${1:-"/var/log/appname/"}

# days to expire, logs older than ${EXPIRE} days will be removed
EXPIRE=${2:-10}
TMPFILE="/tmp/old_log_files"

echo "log=$LOGPATH, expire=${EXPIRE}"
find ${LOGPATH} -regextype posix-basic -regex "${LOGPATH}[a-z]\+.log.[0-9]\+" -a -mtime "+${EXPIRE}" > ${TMPFILE}
if [ $? -ne 0 ];then
  echo "find older log files failed"
  exit 1
fi

for f in `cat ${TMPFILE}`
do
  /usr/sbin/lsof|grep -q $f
  if [ $? -eq 0 ];then
    echo "$f is still open"
  else
    echo "deleteing file:$f"
    rm -f $f
  fi
done
{% endhighlight %}

## 換行處理

*nix 系統裡，每行結尾只有 ```"<換行>"```，即 ```"\n"``` ；Windows 系統裡面，每行結尾是 ```"<換行><回車>"``` ，即 ```"\n\r"``` 。

如果不進行轉換，那麼 *nix 系統下的文件在 Windows 裡打開所有文字會變成一行；而 Windows 裡的文件在 *nix 下打開的話，在每行的結尾可能會多出一個 ```^M``` 符號。

要把文件轉換一下，有兩種方法：

1. 命令 ```dos2unix test.file```；
2. 去掉 ```"\r"``` ，用命令 ```sed -i 's/\r//' test.file``` 。


## 字符集設置

程序運行時需要使用一套語言環境，包括了：字符集 (數據) 和字體 (顯示)，在 Linux 中通過 locale 來設置程序運行的不同語言環境，locale 由 ANSI C 提供支持，可以根據不同的國家地區設置不同的語言環境。

locale 的命名規則為 ```<語言>_<地區>.<字符集編碼>``` ，例如 ```zh_CN.UTF-8``` 中 ```zh``` 代表中文，```CN``` 代表大陸地區，```UTF-8``` 表示字符集。另外，在 locale 會通過一組環境變量，針對不同場景配置。

{% highlight text %}
LC_COLLATE
  定義該環境的排序和比較規則
LC_CTYPE
  用於字符分類和字符串處理，控制所有字符的處理方式，包括字符編碼，字符是單字節還是
  多字節，如何打印等，是最重要的一個環境變量。
LC_MONETARY
  貨幣格式。
LC_NUMERIC
  非貨幣的數字顯示格式。
LC_TIME
  時間和日期格式。
LC_MESSAGES
  提示信息的語言，與之詳細的還有LANGUAGE參數，當LANGUAGE設置後LC_MESSAGES將會失效，
  而且可同時設置多種語言信息，如LANGUANE="zh_CN.GB18030:zh_CN.GB2312:zh_CN"。
LANG
  LC_*的默認值，是最低級別的設置，如果LC_*沒有設置，則使用該值。
LC_ALL
  一個宏，如果該值設置了，則該值會覆蓋所有LC_*的設置值。注意，LANG的值不受該宏影響。
{% endhighlight %}

簡單介紹下常見的操作。

{% highlight text %}
----- 設置成中文環境
export LANG="zh_CN.UTF-8"
export LANGUAGE="zh_CN:zh:en_US:en"

----- 設置成英文環境
export LANG="en_US.UTF-8"
export LANGUAGE="en_US:en"

----- 查看現有語言環境
$ locale

----- 所有可用語言環境
$ locale -a
{% endhighlight %}

注意，圖形界面可能需要更多的設置，暫時先不討論了。


## 歷史命令


{% highlight text %}
----- 去除重複命令，包括不連續的命令，通過ignoredups去除連續重複的命令
export HISTCONTROL=erasedups
{% endhighlight %}

<!--
https://linuxtoy.org/archives/history-command-usage-examples.html
-->


## 雜項

### 目錄合併

可以將兩個目錄通過 `cp -r dir1/* dir2/* merged/` 進行合併，由於是複製，對於較大的文件會導致速度較慢。如果通過 `mv` 則會報 `Directory not empty` 的錯誤。

可以通過如下命令採用硬鏈接的方式進行復制，通過 `tree --inodes` 命令查看文件的 inode 號。

{% highlight text %}
$ cp -r --link dir1/* dir2/* merged/
{% endhighlight %}

### shell 操作

{% highlight text %}
----- 查看當前所有shell
$ chsh -l
$ cat /etc/shells
{% endhighlight %}

查看當前的 Shell ，可以查看 `SHELL` 變量，不過如果是通過 bash dash sh 等直接運行的話，那麼 SHELL 不變，可以通過如下方式查看。

{% highlight text %}
$ ps | grep $$ | awk '{print $4}'
$ echo $0
$ tom                                # 輸入沒有的命令
{% endhighlight %}

`chsh -s /bin/dash` 實際修改的是 `/etc/passwd` 文件裡和你的用戶名相對應的那一行，重啟 Shell 後即可。

### 其它

{% highlight text %}
----- 生成隨機文件
$ head -c 10M < /dev/random > /tmp/foobar.txt

----- 日期轉換
$ date +%s -d "04/24/2014 15:30:00"         // 將日期轉換成時間戳
$ date -d @1398324600                       // 將時間戳轉換成日期
$ date +%s                                  // 將當前日期轉換成時間戳
$ date -d "1970-01-01 UTC `echo "$(date +%s)-$(cat /proc/uptime|cut -f 1 -d' ')+12288812.926194"|bc ` seconds"
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
