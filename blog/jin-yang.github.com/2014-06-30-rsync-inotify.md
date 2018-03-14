---
title: Rsync & Inotify
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: rsync,inotify
description: rsync 是一個遠程數據同步工具，使用 "rsync算法" 來使本地和遠程兩個主機之間的文件達到同步，該算法只傳送兩個文件的不同部分；而 inotify 是一種強大的、細粒度的、異步的文件系統事件監控機制。這裡介紹下 rsync 和 inotify 的使用。
---

rsync 是一個遠程數據同步工具，使用 "rsync算法" 來使本地和遠程兩個主機之間的文件達到同步，該算法只傳送兩個文件的不同部分；而 inotify 是一種強大的、細粒度的、異步的文件系統事件監控機制。

這裡介紹下 rsync 和 inotify 的使用。

<!-- more -->

## RSYNC

也就是 Remote Synchronize，這是一個在 *nix 下的一個遠程數據同步工具，可以通過網絡快速同步多臺主機間的文件和目錄；而且並非每次都整份傳輸，而是隻傳輸兩個文件的不同部分，因此其傳輸速度相當快。

rsync 的傳輸效率要比 scp 高很多，其特點有：

* 可鏡像保存整個目錄樹和文件系統；
* 可做到保持原文件的權限、時間、軟硬連接等；

如下是常見的數據傳輸方式。

### 傳輸方式

這三種傳輸方式的主要差異是，是否有冒號 (:)，本地傳送不需要冒號，通過 ssh 或 rsh 時，需要一個冒號，通過 rsync 傳送需要兩個冒號。

#### 1. 複製本地文件

在本地直接運行，用法與 cp 幾乎一模一樣。

{% highlight text %}
$ rsync [OPTION]... SRC DEST

$ rsync -av /etc /tmp              ← 將/etc/的資料備份到/tmp/etc內
{% endhighlight %}

#### 2. 利用ssh

實際上就是使用遠程 shell 程序，一般為 rsh 或者 ssh 實現內容的遠程複製，通常路徑地址包含單個冒號 ":" 分隔符時啟動該模式。

{% highlight text %}
$ rsync [OPTION]... SRC [USER@]host:DEST

----- 將remote-server的/etc備份到本地主機/tmp內
$ rsync -ave ssh user@remote-server:/etc /tmp
{% endhighlight %}

#### 3. 使用rsync服務器

通過 rsync 提供的服務來傳送，此時 rsync 服務主機會啟動 873 端口，也就意味著必須要在服務端啟動 rsync 服務，可以查看 /etc/xinetd.d/rsync 服務。

此外，還需要 A) 編輯 /etc/rsyncd.conf 配置文件；B) 需要設置好客戶端密碼文件。

<!--
### 認證方式

實際上，對應上述的三種傳輸方式，也就對應了兩種認證方式，分別是 rsync-daemon 認證、ssh 認證。

A) rsync-daemon 需要服務端啟動rsync服務，默認監聽873端口，可以通過配置文件對服務進行配置。
B) ssh 類似於 scp 工具，可通過系統用戶進行認證，並不需要啟動rsync服務，只需要安裝即可。
-->

### 命令行

{% highlight text %}
$ rsync [OPTION]... SRC [USER@]HOST::DEST
$ rsync [OPTION]... rsync://[USER@]HOST[:PORT]/SRC [DEST]
$ rsync -av user@hostname::/dir/path /local/path
{% endhighlight %}

#### 參數列表

常用的參數列表如下。

{% highlight text %}
rsync [-avrlptgoD] [-e ssh] [user@host:/dir] [/local/path]

參數簡介：
  --compress/-z, --compress-level=level
    傳輸過程中是否進行數據壓縮，以及指定的壓縮級別；
  --archive/-a, --times, --perms, --owner, --group
    用於設置保留原來的更新時間戳、權限、所有者和組信息，注意，對於 -a 同時會設置 --recursive 和 --links，
    相當於 -rlptgoD；
  --recursive/-r VS. --dirs/-d
    前者遞歸複製所有目錄(含子目錄)；後者會跳過子目錄及其內容；
  --delete
    默認從源複製到目的端時，不刪除額外文件，通過該選項保證源和目標目錄的內容完全一致，如果源是空，那麼會
    導致目的目錄內容被刪除；
  --links/-l, --hard-links/-H, --copy-links/-L, --copy-unsafe-links, --safe-links
    分別為保持符號鏈接、保持硬鏈接、複製符號鏈接指向的內容而非符號鏈接本身、如果符號鏈接指向源目錄外則仍
    複製、指向源目錄外時為保證安全不復制；
  --verbose, --progress, --stats
    用於顯示正在執行的狀態，默認不打印任何信息；
  --rsh/-e
    默認使用的就是ssh，通過該參數強制使用，可以通過 --rsh "ssh -p 12345" 指定 ssh 的參數；
  --exclude, --include
    選擇要同步的文件，可以指定多次，例如 --exclude "*bak" --exclude "*~" ；
    示例：注意，指定時使用的是源地址的相對路徑。
    --exclude "checkout"    某個目錄
    --exclude "filename*"   某類文件
    --exclude-from=sync-exclude.list  通過文件指定要忽略的文件
  --update/-u
    增量傳輸，也就是會跳過所有存在於目標，且時間較新的文件。注意，只檢查文件名和最後修改時間，並不檢查文
    件大小，如果目的端有1M的A.txt文件，但是更新時間新於源端10M的A.txt那麼本地文件不會更新；
  --partial, -P
    斷點續傳，會保留沒有完全傳輸的文件，以加快隨後的再次傳輸，其中 -P 相當於 --partial --progress；注意，
    該參數與 -u 衝突，會導致傳了一半的文件會被 rsync 跳過，詳見上面的介紹；

注意: 在指定複製源時，路徑是否有最後的 "/" 有不同的含義，例如：
    /data  ：表示將整個 /data 目錄複製到目標目錄含；
    /data/ ：表示將 /data/ 目錄中的所有內容複製到目標目錄。
{% endhighlight %}


## inotify

inotify 是基於 inode 級別的文件系統監控技術，是一種強大的、細粒度的、異步的機制。

<!--
sersync

不需要對被監視的目標打開文件描述符，而且如果被監視目標在可移動介質上，那麼在 umount 該介質上的文件系統後，被監視目標對應的 watch 將被自動刪除，並且會產生一個 umount 事件。
既可以監視文件，也可以監視目錄。
使用系統調用而非 SIGIO 來通知文件系統事件。
使用文件描述符作為接口，因而可以使用通常的文件 I/O 操作select 和 poll 來監視文件系統的變化。


IN_ACCESS : 即文件被訪問
IN_MODIFY : 文件被 write
IN_ATTRIB : 文件屬性被修改，如 chmod、chown、touch 等
IN_CLOSE_WRITE : 可寫文件被 close
IN_CLOSE_NOWRITE : 不可寫文件被 close
IN_OPEN : 文件被open
IN_MOVED_FROM : 文件被移走,如 mv
IN_MOVED_TO : 文件被移來，如 mv、cp
IN_CREATE : 創建新文件
IN_DELETE : 文件被刪除，如 rm
IN_DELETE_SELF : 自刪除，即一個可執行文件在執行時刪除自己
IN_MOVE_SELF : 自移動，即一個可執行文件在執行時移動自己
IN_UNMOUNT : 宿主文件系統被 umount
IN_CLOSE : 文件被關閉，等同於(IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)
IN_MOVE : 文件被移動，等同於(IN_MOVED_FROM | IN_MOVED_TO)


/proc/sys/fs/inotify/max_queued_events 默認值: 16384 該文件中的值為調用inotify_init時分配給inotify instance中可排隊的event的數目的最大值，超出這個值得事件被丟棄，但會觸發IN_Q_OVERFLOW事件

/proc/sys/fs/inotify/max_user_instances 默認值: 128 指定了每一個real user ID可創建的inotify instatnces的數量上限

/proc/sys/fs/inotify/max_user_watches 默認值: 8192 指定了每個inotify instance相關聯的watches的上限

注意: max_queued_events 是 Inotify 管理的隊列的最大長度，文件系統變化越頻繁，這個值就應該越大
如果你在日誌中看到Event Queue Overflow，說明max_queued_events太小需要調整參數後再次使用.
-->

inotify-tools 提供了一個命令行管理的命令，可以查看 [Github inotify-tools](https://github.com/rvoicilas/inotify-tools) ，也可以通過 [Github Wiki](https://github.com/rvoicilas/inotify-tools/wiki) 查看下載最新版本，此時會安裝 inotifywait 和 inotifywatch 兩個文件。

{% highlight text %}
$ tar -zxf inotify-tools-3.14.tar.gz && cd inotify-tools-3.14
$ ./configure --prefix=/usr && make && su -c 'make install'
# mv /lib/libinotifytools.* /lib64/
{% endhighlight %}


<!--
inotifywait可以在當事件發生一次時就退出，或者一直執行。
<ul><li>
    @&lt;file&gt;<br>
    當監控一個目錄時，可以設置不監控某個文件，可以使用該選項去除一些文件。</li><br><li>

    --fromfile<br>
    從文件讀取需要監視的文件或排除的文件，一個文件一行，排除的文件以@開頭。</li><br><li>

    -e , --event<br>
    指定監視的事件。</li><br><li>

    -m, --monitor<br>
    接收到一個事情而不退出，無限期地執行。默認的行為是接收到一個事情後立即退出。</li><br><li>

    -d, --daemon<br>
    跟--monitor一樣，除了是在後臺運行，需要指定--outfile把事情輸出到一個文件。也意味著使用了--syslog。</li><br><li>

    -o, --outfile<br>
    輸出事情到一個文件而不是標準輸出。</li><br><li>

    -s, --syslog<br>
    輸出錯誤信息到系統日誌。</li><br><li>

    -r, --recursive<br>
    監視一個目錄下的所有子目錄。</li><br><li>

    -q, --quiet<br>
    指定一次，不會輸出詳細信息，指定二次，除了致命錯誤，不會輸出任何信息。</li><br><li>

    --timefmt<br>
    指定時間格式，用於--format選項中的%T格式。</li><br><li>

    --format<br>
    指定輸出格式。 %w 表示發生事件的目錄 %f 表示發生事件的文件 %e 表示發生的事件 %Xe 事件以“X"分隔 %T 使用由--timefmt定義的時間格式</li><br><li>


--exclude
正則匹配需要排除的文件，大小寫敏感。
--excludei
正則匹配需要排除的文件，忽略大小寫。
-t , --timeout
設置超時時間，如果為0，則無限期地執行下去。
-c, --csv
輸出csv格式。

access  文件讀取
modify  文件更改。
attrib  文件屬性更改，如權限，時間戳等。
close_write     以可寫模式打開的文件被關閉，不代表此文件一定已經寫入數據。
close_nowrite   以只讀模式打開的文件被關閉。
close   文件被關閉，不管它是如何打開的。
open    文件打開。
moved_to    一個文件或目錄移動到監聽的目錄，即使是在同一目錄內移動，此事件也觸發。
moved_from  一個文件或目錄移出監聽的目錄，即使是在同一目錄內移動，此事件也觸發。
move    包括moved_to和 moved_from
move_self   文件或目錄被移除，之後不再監聽此文件或目錄。
create  文件或目錄創建
delete  文件或目錄刪除
delete_self     文件或目錄移除，之後不再監聽此文件或目錄
unmount     文件系統取消掛載，之後不再監聽此文件系統。

<ul><li>
    監控 java 目錄，通過 cat test/foo 測試。
    <pre>$ inotifywait test</pre></li><br><li>

    等待httpd相關的信息。
<pre>
#!/bin/sh
while inotifywait -e modify /var/log/messages; do
    if tail -n1 /var/log/messages | grep httpd; then
        kdialog --msgbox "Apache needs love!"
    fi
done
</pre></li><br><li>

    監控~/test，同時執行如下命 touch ~/test/badfile、touch ~/test/goodfile, rm ~/test/badfile 。
    <pre>$ inotifywait -m -r --format '%:e %f' ~/test</pre></li><br><li>


#!/bin/bash
src=/var/www
des=backup@192.168.1.200::web
/usr/local/bin/inotifywait -mrq --timefmt '%d/%m/%y %H:%M' --format '%T %w%f' \
-e modify,delete,create,attrib ${src} \
| while read x
    do
        /usr/bin/rsync -avz --delete --progress $src $des --password-file=/root/rsyncpass &&
        echo "$x was rsynced" &lt;&lt; /var/log/rsync.log
    done


註釋：
inotifywait
-m：保持監聽事件。
-r：遞歸查看目錄。
-q：打印出事件。
-e modify,delete,create,attrib：監聽寫入，刪除，創建，屬性改變事件。

rsync
-a：存檔模式，相當於使用-rlptgoD。
-v：詳細模式輸出。
-z：傳輸過程中壓縮文件。

為腳本加執行權限：

chmod +x /root/rsync.sh

在rc.local加入自啟動：



<pre>
#!/bin/bash
host=192.168.1.15
src=/tmp/
des=web
user=webuser
inotifywait -mrq --timefmt '%d/%m/%y %H:%M' --format '%T %w%f%e' -e modify,delete,create,attrib $src while read files
do
/usr/bin/rsync -vzrtopg --delete --progress --password-file=/usr/local/rsync/rsync.passwd $src $user@$host::$des
echo "${files} was rsynced" &gt;&gt;/tmp/rsync.log 2&gt;&amp;1
done</pre>

while inotifywait -qq -e create --exclude '\.(swp|txt|tmp)' /pis/src; do
    sleep 1
    gcc /pis/src/main.cpp -o /pis/bin/pistat
    chmod +x /pis/bin/*
done
</li></ul>
</p>


http://www.ttlsa.com/web/let-infotify-rsync-fast/

sersync

-->


















{% highlight text %}
{% endhighlight %}
