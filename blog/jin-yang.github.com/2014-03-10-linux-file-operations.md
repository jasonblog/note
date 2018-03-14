---
title: Linux 文件操作
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,文件系統
description: Linux 的設計理念是：一切都是文件！在此簡單介紹下 Linux 中常見的文件操作。
---

Linux 的設計理念是：一切都是文件！在此簡單介紹下 Linux 中常見的文件操作。

<!-- more -->

## 文件

通過 ```ls -l test``` 命令可以查看文件 test 的屬性，通常可以得到如下的內容：

![Linux file properties]({{ site.url }}/images/linux/filesystem-file-property-1.jpg "Linux file properties"){: .pull-center}

第二欄表示的是有多少文件連接到 inode ，如果是一個文件，此時這一字段表示這個文件所具有的硬鏈接數，如果是一個目錄，則此字段表示該目錄所含子目錄的個數，包括 ```.``` 、```..``` 以及隱藏目錄。

每個文件都會將他的權限與屬性記錄到文件系統的 inode 中，不過，我們使用的目錄樹卻是使用文件名來記錄，因此每個檔名就會連結到一個 i-node 這個屬性記錄的，就是有多少不同的檔名連結到相同的一個 i-node 號碼去就是了。

### 文件類型

在 Linux 中，總共有四種文件類型，包括普通文件、目錄文件、連接文件和特殊文件，那麼可以通過 file 命令來查看。

* 普通文件(regular file)：包括純文本文件 (ASCII) ，二進制文件 (binary) ，資料格式文件 (data) ， data 主要是指以特定格式存儲的文件，如使用 Linux 登錄時會將記錄存放在 ```/var/log/wtmp``` 中，這就是一個 data file，可以通過 last 來查看，但是不能用 cat 。```ls -l``` 可以看到 ```-``` 。

* 目錄文件(directory)：包括文件名、子目錄名及其指針。```ls -l``` 可以看到 ```d``` 。

* 連接文件(link)：是指向同一索引節點的那些目錄條目，通常為符號鏈接。```ls -l``` 可以看到 ```l``` 。

* 設備相關文件(device)：這些文件通常放在 ```/dev``` 中，又可以分為塊設備文件 (如硬盤) 和字符文件 (如鍵盤、鼠標)。```ls -l``` 可以分別看到 ```b/c``` 。

* 網絡接口文件(sockets)：用於網絡上的數據傳輸，常在 ```/var/run``` 中。```ls -l``` 可以看到 ```s``` 。

* 管道文件(FIFO, pipe)：主要用於數據的讀寫，如命令管道。```ls -l``` 可以看到 ```p``` 。

### 文件權限

![Linux file properties]({{ site.url }}/images/linux/filesystem-file-property-2.png "Linux file properties"){: .pull-center}

其中第一位表示文件的類型；後九位中，每三位為一組，```r-read/4```、```w-write/2```、```x-excute/1```，分別表示 擁有者、同組用戶、其他人的權限。上圖表示該文件為目錄，擁有者有讀、寫、執行的權利，而用戶組其他成員和其他人沒有寫的權利。

<!--
* 當用戶對某個目錄只有讀權限時，那麼該用戶可以列出該目錄下的文件列表，即可以使用ls來列出目錄下的文件，但是不能進入該目錄，即不能cd目錄名來進入該目錄。同時也不能通過ls -l查看文件屬性，將顯示為?。</li><br><li>

    當用戶對某個目錄只有執行權限時，該用戶是可以進入該目錄的，即可以通過cd來進入該目錄；但該用戶不能列出這個目錄下的文件列表的，也即不能用ls命令列出該目錄下的信息。</li><br><li>

    當用戶具有寫權限時，用戶可以在當前目錄增加或者刪除文件，但需要幾個前提：1、需要有可執行權限2、要想刪除文件，那麼sticky bit位是沒有設置的。
-->


只有當文件所在上一級目錄有 w 的權限時，才可以新建或者刪除文件，對於已經存在的文件可以進行修改。

{% highlight text %}
$ chown -R user:group file/dir             # 修改用戶和用戶組，-R 表示遞歸
$ chmod -R 740 file/dir                    # 修改文件或目錄的權限，-R 表示遞歸
$ chmod u/g/o/a +/-/= r/w/x file/dir       # 分別表示user/group/others/all
{% endhighlight %}

文件默認屬性可通過 ```umask``` 設置，```umask``` 就是指 "Linux文件的默認屬性需要減掉的權限"。

對於 Linux 來說，普通文件的最大默認屬性是 666，目錄的最大屬性是 777；如果不想要用戶在新建立文件時使用默認的屬性，那麼就要設置 umask 值。

在 CentOS 中，系統默認的 umask 值是 0002，那麼用戶在新建立普通文件時，普通文件的屬性就是 666-0002=664，新建目錄時，目錄的屬性就是 777-0002=755。

可以通過如下的命令進行查看設置。

{% highlight text %}
$ umask                  # 查看
$ umaks 0002             # 設置
{% endhighlight %}

### 文件的特殊權限

除了上述的讀寫執行權限之外，Linux 還定義了其他的權限。

#### SUID

當一個設置了 SUID 位的可執行文件被執行時，無論那個用戶來執行該文件，該文件將以所有者的身份運行，都有文件所有者的特權。如果所有者是 root 的話，那麼執行人就有超級用戶的特權了。

例如 ```/etc/passwd``` 的權限為 ```-rw-r--r-- root root``` ，也就是說只有 root 才能修改，但是我們可以通過 ```/usr/bin/passwd``` 來修改密碼。通過 ```ls -l /usr/bin/passwd``` 可以看到其屬性為 ```-rwsr-xr-x root root``` 因此在執行時獲得了 root 的權限。

**NOTE**: 該屬性僅可用在可執行文件，不用於腳本文件和目錄(可以進行設置，但是應該無效)。因為腳本時一系列命令的集合體，不同的命令可能會對應於不同的屬性。

#### SGID

當一個設置了 SGID 位的可執行文件運行時，該文件將具有所屬組的特權， 任意存取整個組所能使用的系統資源。

若目錄設置了 SGID ，則所有被複制到這個目錄下的文件，其屬組都會被重設為和這個目錄一樣，除非在複製文件時加上 ```-p``` (preserve，保留文件屬性) 參數，才能保留原來所屬的群組設置。

**NOTE**: 一般來說， SGID 多用在特定的多人團隊的項目開發上，在系統中用得較少。

#### 粘著位(Sticky Bit)

只針對目錄有效，對文件沒有效果。如果用戶在該目錄下面具有 w 和 x 權限，當用戶在該目錄下建立文件或者目錄時，只有文件擁有者和 root 用戶才有權力刪除(注意目錄的所有者也可以刪除文件的)。

#### 設置

可以通過如下的命令進行設置。

{% highlight text %}
$ chmod u+s(SUID) g+s(SGID) o+t(STICKY)
$ chmod 4755
{% endhighlight %}

一個文件的屬性通常有 4 個八進製表示，ls 只顯示了 3 個，而對於 ```xxx-SUID,SGID,STICKY(4,2,1)```，與 ```rwx``` 相似，也採用一個數字進行表示。

設置後，可以用 ```ls -l``` 來查看，如果本來在該位上有 x (均顯示在x位上)，則這些特殊標誌顯示為小寫字母 ```(s, s, t)```；否則，顯示為大寫字母 ```(S, S, T)```。

對於目錄來說如果通過 lsattr 無法查看屬性，可以通過 ```ls | lsattr``` 查看；或者直接執行 lsattr。

### 文件特殊屬性

文件系統中除了上述的權限配置之外，還可以通過 chattr 進行設置，使用 chattr 必須為 root 用戶才可以。

{% highlight text %}
chattr [-R] [-+=] [AacDdijsSu] 文件名
{% endhighlight %}

* A(atime)：如果設置了A屬性，則這個文件的最後訪問時間atime不能被修改。

* a(append only)：如果設置了a屬性，則這個文件只能增加數據，不允許任何進程覆蓋或截斷這個文件。如果某個目錄具有這個屬性，那麼只能在這個目錄下建立和修改文件，而不能刪除任何文件。注意，由於vi會創建一些swp文件，因此此時會出錯，使用nano。

* d(No dump)：在進行文件系統備份時，dump程序將忽略這個文件。

* i(immutable)：如果設置了i屬性，則不能對這個文件做任何修改，包括刪除、修改內容等，如果要刪除必須去掉該屬性。如果某個目錄具有這個屬性，那麼只能修改該目錄下的文件，而不能建立和刪除文件。這個是針對所有用戶，而修改目錄的w權限對root無效。

<!--
    c(compressed): 系統以透明的方式壓縮這個文件。從這個文件讀取時，返回的是解壓之後的數據；而向這個文件中寫入數據時，數據首先被壓縮之後才寫入磁盤。</li><br><li>

    s(secure deletion)：如果設置了s屬性，則這個文件將從硬盤空間中完全刪除，使用0填充所在區域。</li><br><li>

    u(undeletable)：與s完全相反。如果設置了u屬性，則這個文件雖然被刪除了，但是還在硬盤空間中存在，還可以用來還原恢復。

    （D）synchronous directory updates；（j）data journalling；（S）synchronous updates；（T）and top of directory hierarchy；（t）no tail-merging。-->


### 文件時間屬性

在 Linux 中，沒有文件創建時間的概念，只有文件的訪問時間、修改時間、狀態改變時間，也就是說不能知道文件的創建時間。

但如果文件創建後就沒有修改過，修改時間 = 創建時間；如果文件創建後，狀態就沒有改變過，那麼狀態改變時間 = 創建時間；如果文件創建後，沒有被讀取過，那麼訪問時間 = 創建時間，這個基本不太可能。

* modification time(mtime，修改時間)：cp，這個時間指的是文件內容修改的時間，而不是文件屬性的修改，當數據內容修改時，這個時間就會改變，用命令 ls -l 默認顯示的就是這個時間。

* access time(atime，訪問時間)：cp, cat, more，當讀取文件內容時，就會更改這個時間，例如使用 cat more vi 等命令，那麼該文件的 atime 就會改變，ls stat 不會改變訪問時間。注意：可能由於 Linux 緩存機制，第一次讀取文件後，下次可能不會訪問該文件，因此atime不會改變。

* change time(ctime，屬性或位置修改時間)：mv, cp, chmod, chown，當一個文件的狀態改變時，這個時間就會改變，例如更改了文件的權限與屬性等，它就會改變。

## 其它

### 常用命令

{% highlight text %}
----- 查看文件的類型
$ file /tmp/test

----- 使用stat顯示文件詳細信息
$ stat file                                    # 顯示文件的信息
$ stat -t file                                 # 與上相同，不過在一行顯示，方便腳本處理
$ stat -f file                                 # 顯示文件系統的信息

----- 顯示時間信息
$ ls -l --full-time file                       # 修改時間的完整格式，包含時區
$ ls -l --time=atime/ctime --full-time file    # 顯示atime/ctime
{% endhighlight %}

<!--
touch [-actmd] file 用來修改atime和mtime，實際同時也會修改ctime<ul><li>
    -a:如果沒有指定時間，則修改atime/ctime；否則atime設置未指定時間，ctime為當今時間。</li><li>
    -m:如果沒有指定時間，則修改mtime/ctime；否則mtime設置未指定時間，ctime為當今時間。</li><li>
    -c, --no-create: 不創建任何文件，但同時會修改atime/mtime/ctime</li><li>
    -d:後面可以接日期，也可以使用--date="日期或時間"</li><li>
    -t:後面可以接時間，格式為：[YYMMDDhhmm]</li></ul>
    舉例：結果atime和mtime將會改變而ctime不會改變:<br>

touch -d "May 25 3:51 pm" file<br>
touch -d "2 days ago" file<br>
touch -c -m -t 201101110000 file
-->

## 參考

{% highlight text %}
{% endhighlight %}
