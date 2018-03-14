---
title: Linux 常用命令 -- 雜項
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,命令
description: 主要介紹下在 Linux 中，與文本處理相關的命令。
---

主要介紹下在 Linux 中，日常用到的一些常用指令，如 find 。

<!-- more -->

## find

對於 find 命令，即使系統中含有網絡文件系統 (NFS)， find 命令在該文件系統中同樣有效，只要具有相應的權限。

如下是 find 命令的格式。

{% highlight text %}
find [-H] [-L] [-P] [-D debugopts] [-Olevel] [path...] [expression]
{% endhighlight %}

路徑默認為當前路徑，默認 expression 為 `-print` ，參數通常以 `-`、`(`、`!` 開頭。

find 命令會從左到右檢測 expression 的條件，當知道結果時退出，如 and 為假時， or 為真時。如果在路徑中使用通配符，則路徑需要以 `./` 或 `/` 開頭。

如下是常用選項：

{% highlight text %}
-P 默認選項。對於符號鏈接，檢查時檢測符號鏈接本身。
-L 對於符號鏈接，檢查時檢測符號鏈接指向的內容，如果指向文件不在或find無法找到，則使用鏈接符號。
-H 除非是處理命令行，否則只針對符號本身。

-print 將匹配的文件輸出到標準輸出，查找結果使用換行間隔。
-print0 查找結果使用NULL間隔。

-exec 對匹配的文件執行該參數所給出的 shell 命令，相應的形式為 'command' {  } \;，注意 {   } 和 \; 之間的空格。
-ok 和 -exec 相同，不過在執行每一個命令之前，都會給出提示，讓用戶來確定是否執行。

-name 按照指定文件名搜索。可以使用通配符，此時需要加引號。
-perm 指定文件的權限。
	mode 指定具體的權限，嚴格匹配，如 -perm g=w, 0020 。
	-mode 轉換為二進制後，為 1 的必須都匹配。
	+/mode 轉換為二進制後，為 1 的任何一位匹配即可，最好使用 / 。

-prune 不在當前指定的目錄中查找，如果同時使用 -depth 選項，那麼該選項被忽略。
-user username 指定用戶名，也可以為 ID 。
-group groupname 指定組名，也可以為 ID 。
-amin +-n 按照文件訪問時間來查找文件，n正好是n分鐘，-n指n分鐘以內，+n指n分鐘以前。
-atime +-n 按照文件訪問時間來查找文件，n正好是n天，-n指n天以內，+n指n天以前。
-cmin +-n 按照文件狀態時間來查找文件，n正好是n分鐘，-n指n分鐘以內，+n指n分鐘以前。
-ctime +-n 按照文件狀態時間來查找文件，n正好是n天，-n指n天以內，+n指n天以前。
-mmin +-n 按照文件更改時間來查找文件，n正好是n分鐘，-n指n分鐘以內，+n指n分鐘以前。
-mtime +-n 按照文件更改時間來查找文件，n正好是n天，-n指n天以內，+n指n天以前。
-nogroup 查無有效屬組的文件，即文件的屬組在/etc/groups中不存在。
-nouser 查無有效屬主的文件，即文件的屬主在/etc/passwd中不存在。
-group groupname 指定組名，也可以為ID。
-newer/cnewer/anewer file1 !file2 文件的修改時間/狀態時間/訪問時間，比file1早，比file2晚。
-newerXY reference XY可以是a/B/c/m/t，訪問時間/創建時間（不支持）/狀態時間/修改時間/指定時間。
-type b/d/c/p/l/f/s 塊設備、目錄、字符設備、管道、符號鏈接、普通文件、socket。
-size n[cwbkMG] 長度為n字節c/雙字節w/塊512字節b/千字節k/兆字節M/吉字節G的文件。
-fstype 查位於某一類型文件系統中的文件，這些文件系統類型通常可在 /etc/fstab 中找到。
-follow 如果遇到符號鏈接文件，就跟蹤鏈接所指的文件。
-maxdepth levels 查找目錄時的最大深度。
-path pattern 指定目錄，可以使用通配符，與 Shell 相同。如果find .則path必須使用./doc，且不能以/結尾。
-depth 在查找文件時，首先查找當前目錄中的文件，然後再在其子目錄中查找。
{% endhighlight %}

### 示例

#### 查找文件

{% highlight text %}
find / -path "/etc*" -name "*.txt"
{% endhighlight %}

查找 `/etc` 目錄下，且後綴是 `txt` 的文件，使用 `-iname` 忽略文件名的大小寫。

#### 用戶andy所擁有的文件

{% highlight text %}
find /etc -name "passwd*" -exec grep "andy" {} \;
find . -name "*.cpp" | xargs grep 'efg'
{% endhighlight %}

首先匹配所有文件名為 `"passwd*"` 的文件，如 `passwd`、`passwd.old`，然後執行 `grep` 命令看看在這些文件中是否存在一個 andy 用戶。注意 `{` `}` 之間沒有空格，相當於前面查找到的文件。

#### 忽略某個目錄

{% highlight text %}
find /etc -path "/etc/fonts" -a -prune -o -print
{% endhighlight %}

可以使用 `-prune` 指定需要忽略的目錄，`-a` 可以去掉。使用該選項需要注意的是，如果同時使用了 `-depth` 選項，那麼 `-prune` 選項就會被 find 命令忽略。

`-a` `-o` 均支持短路操作，因此實際上述等於如下的偽代碼：`if -path "/etc/fonts": -prune else: -print` 。

#### 忽略多個目錄

{% highlight text %}
find /usr \( -path /usr/dir -o -path /usr/file \) -a -prune -o -print
{% endhighlight %}

`-a` 選項可以去掉，表示 `and` ，`-o` 表示 `or` 或者是 `-not`，`-prune` 始終會返回 `1` 。

#### 查找某一特定文件

{% highlight text %}
find /usr \( -path /usr/dir -o -path /usr/file \) -a -prune -o -name "test" -print
{% endhighlight %}

注意，`-name` 等，應該放置到最後一個 `-o` 選項後面。


<!--
	<pre>find /etc/rc.d -name '*crond' -type f -perm 755 -size 10M -exec file '{}' \;</pre>
    -exec 可以使用 -ok 代替，此時會詢問是否執行命令。注意： exec 和 file 間是一個空格， file 和 {} 間是一個空格， {} 和 \; 之間是一個空格， \; 是一個整體， \ 表示轉義。 {} 表示文件名，也就是 find 前面處理過程中過濾出來的文件，用於 command 命令進行處理。<pre>find / -size +50M -size -100M -exec ls -lh {} \; 2>&1 | grep -v Permission</pre>查找大於 50MB 小於 100MB 的文件。</li><br><li>


    <pre>find /tmp -name core -type f -print[-print0] | xargs [-0] /bin/rm -f<br>find . -name '*wine*' -exec rm -f { } \;</pre>
	其中 xargs 將輸入分隔成不同的小塊，並執行。 exec 將所有匹配的參數傳遞給命令，此時可能會由於命令太長而出錯。如果文件或是目錄名含有空格，則可能會有些問題，這是因為 xargs 默認會按照空白字符來劃分輸入，通過 -0 選項即可。</li><br><li>

	<pre>find . -type f \( -name 'core' -o[-or/-a/-and/!/-not] -name '*.o' \)</pre>
	其中 () 用來表示強制的優先級。</li><br><li>



	<pre>find . ! -type d -print</pre>
	在當前目錄下查找除目錄以外的所有類型的文件。</li><br><li>


	<pre>find / -name "CON.FILE" -depth -print</pre>
	先匹配所有的文件，再在子目錄中查找。如，使用 find 命令備份文件系統時，希望首先備份所有的文件，其次再備份子目錄中的文件。</li><br><li>


	<pre>ssh 192.168.30.137 "ps -ef | grep httpd | awk '{print $2}' | xargskill -9"</pre>
	在殺死遠程主機的進程的時候，顯示的信息會在本地顯示。</li><br><li>


	<pre>find . -name "jdk*" -type f -print | xargs file</pre>
	查看所有以 jdk 開頭文件的文件類型。</li><br><li>


	<pre>find / -name "core" -print | xargs echo "" > /tmp/core.log</pre>
	在整個系統中查找內存信息轉儲文件 (core dump) ，然後把結果保存到 /tmp/core.log 文件中。</li><br><li>


	<pre>find . -perm 777 -print | xargs chmod o-w<br>find / -type f -perm 777 -print -exec chmod 644 {} \;</pre>
	在當前目錄下查找所有用戶具有讀、寫和執行權限的文件，並收回相應的寫權限。
	<pre>
find / -type f ! -perm 777    // 權限非 777 的文件
find / -perm 2644             // SGID 位已設置，且權限為 644
find / -perm 1551             // Sticky 位已設置，且權限為 551
find / -perm /u=s             // 已設置 SUID 的文件
find / -perm /g+s             // 已設置 SGID 的文件
find / -perm /u=r             // 只讀文件
find / -perm /a=x             // 可執行文件
</pre></li><br><li>


	<pre>find /tmp -type f -empty</pre>
	查找空文件，或者空目錄 -type d 。</li><br><li>

	<pre>find / -mtime +50 –mtime -100<br>find / -mmin -60</pre>
	查找最近 50-100 天內修改的文件；最近一個小時修改的文件。</li><br><li>
	-->

#### 無主文件

{% highlight text %}
find /home -nouser -print
{% endhighlight %}

查找屬主帳戶已經被刪除的文件，這樣就能夠找到那些屬主在 `/etc/passwd` 文件中沒有有效帳戶的文件。

#### 查看某個時間點文件

{% highlight text %}
find ./ -name "jdk*" | xargs ls -l --full-time 2>/dev/null | grep "2011-11-11 03:41:54"
{% endhighlight %}

查找 `2011-11-11 03:41:54` 時更改過的 jdk 文件。

{% highlight text %}
{% endhighlight %}



{% highlight text %}
{% endhighlight %}
