---
title: Bash 相關
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: bash,alias,PATH
description: 一些與 Bash 相關的內容，如命令執行順序、配置文件等。
---

一些與 Bash 相關的內容，如命令執行順序、配置文件、通配符、元字符及轉義字符等。

<!-- more -->

## 命令執行順序

Shell 一般會按照 `alias` `keyword` `function` `built-in` `$PATH` 的順序進行搜索。

在執行完命令之後會保存在一個 hash 表中，下次直接通過 hash 查找，如果將一個命令刪除可能會出現 `No such file or directory` 的錯誤；可以通過 `set -h` 啟用 hash 功能，`set +h` 禁用。

可通過 `hash` 查看當前列表，`hash -d command` 刪除該記錄，`hash -r` 刪除所有記錄，重新搜索。

通過 `hash` 命令查看時，其第一列為使用的次數，注意不會緩存 `function`、`built-in` 以及部分 `alias`；注意，不會緩存包含絕對路徑的 `alias` 命令。

> 可以通過 `alias cmd` 查看定義的 alias ，一般通過 `alias cmd='xxx'` 定義別名，`unalias cmd` 刪除別名。

常見操作可以參考如下：

{% highlight text %}
----- 可以查看該命令的所有類型
$ type -a cmd
----- 查看執行時的類型
$ type cmd

----- 查看所有的built-in命令
$ enable
----- 禁用built-in的cmd命令
$ enable -n cmd
----- 啟用built-in的命令
$ enable cmd
{% endhighlight %}

<!--
command如果有一個function為同名，則執行cmd實際執行的是。
<pre>
function gcc { echo “just a test for gcc”; }
gcc
command gcc
</pre>
-->

## 配置文件


Bash 的很多配置文件，要說清楚這些配置文件的區別，則首先要了解 login shell 和 no login shell 的區別。

* login shell 指的是完整的登錄流程，需要輸入用戶名和密碼，例如 `tty1~tty6` 等。
* no login shell，簡單來說是 XWindow 登陸後又啟動了多個終端，此時不需要輸入密碼。

Bash 配置文件包括全局配置文件和用戶相關的局部配置文件。


### 全局變量

全局配置文件 `/etc/profile` ，在任何用戶第一次登陸時 (或在切換用戶時使用 `-` 參數) 都會讀取該文件。

另外一個是 `/etc/bashrc` ，其中 Ubuntu 沒有這個文件，與之對應的是 `/etc/bash.bashrc` ，會在 bash 執行時會讀取此文件。

### 局部變量

在執行完全局變量後，執行用戶指定的配置文件，依次檢查 `~/.bash_profile` `~/.bash_login` `~/.profile` 最先找到的直接執行，然後退出。

其中相關的配置文件有如下：

#### ~/.bash_profile

Ubuntu 默認沒有此文件，可新建，只有 Bash 是以 login 形式執行時，才會讀取此文件，通常來說該配置文件還會配置成讀取 `~/.bashrc`。

#### ~/.bash_login

若 bash 是以 login 方式執行時，讀取 `~/.bash_profile` ，若它不存在，則讀取 `~/.bash_login`，若兩者都不存在，讀取 `~/.profile` 。

#### ~/.profile

除了上述的配置文件執行方式外，在圖形模式登錄時，此文件被讀取，即使存在 `~/.bash_profile` 和 `~/.bash_login`。

#### ~/.bashrc

當 Bash 是以 non-login 形式執行時，讀取此文件，若以 login 形式執行，則不會讀取此文件，但是通常在上述的三個文件中會調用該文件。

#### ~/.bash_logout

註銷時，且是 login 形式，此文件才會讀取。也就是說，在文本模式註銷時，此文件會被讀取，圖形模式註銷時，此文件不會被讀取。



### 讀取順序

login shell 和 non-login shell 讀取的配置文件數據並不一樣。

{% highlight text %}
----- login shell會讀配置文件，後面的三個讀取一個文件後就會退出
/etc/profile ~/.bash_profile或~/.bash_login或~/.profile

----- non-login shell會讀配置文件
~/.bashrc
{% endhighlight %}

<!--
下面是在本機的幾個例子：<ol><li>
	圖形模式登錄時，順序讀取：/etc/profile和~/.profile。</li><li>
	圖形模式登錄後，打開終端時，順序讀取：/etc/bash.bashrc和~/.bashrc。</li><li>
	文本模式登錄時，順序讀取：/etc/profile，/etc/bash.bashrc和~/.bash_profile ~/.bash_login ~/.profile之一。</li><li>從其它用戶su到該用戶，則分兩種情況：<ul><li>
		如果帶-l參數（或-參數，--login參數），如：su -l username，則bash是login的，它將順序讀取以下配置文件：/etc/profile，/etc/bash.bashrc和~/.bash_profile ~/.bash_login ~/.profile之一。</li><li>
		如果沒有帶-l參數，則bash是non-login的，它將順序讀取：/etc/bash.bashrc和~/.bashrc。</li></ul></li><li>
	註銷時，或退出su登錄的用戶，如果是longin方式，那麼bash會讀取：~/.bash_logout。</li><li>
	執行自定義的shell文件時，若使用“bash -l a.sh”的方式，則bash會讀取行：/etc/profile和~/.bash_profile ~/.bash_login ~/.profile之一，若使用其它方式，如：bash a.sh， ./a.sh，sh a.sh（這個不屬於bash shell），則不會讀取上面的任何文件。。</li></ol>
-->

## 通配符、元字符及轉義字符

介紹一些與之相關的概念。

### 通配符

通配符是由 shell 處理的，在遇到了通配符時，shell 會將其當作路徑或文件名去在磁盤上搜尋可能的匹配：若符合要求的匹配存在，則進行代換 (路徑擴展)；否則就將該通配符作為一個普通字符傳遞給 "命令"，然後再由命令進行處理。

總之，通配符實際上就是一種 shell 實現的路徑擴展功能，在通配符被處理後，shell 會先完成該命令的重組，然後再繼續處理重組後的命令，直至執行該命令。

常見示例如下：

{% highlight text %}
*               匹配0或多個字符，a*b: acb, a123db
?               匹配任意單一字符，a?b: acb, a1b
[list]          匹配list中的任意單一字符，a[xyz]b: axb, ayb, azb
[!list]         匹配除list中的任意單一字符，a[!0-9]b: aab, acb, a-b
[c1-c2]         匹配c1到c2中的任意單一字符，a[0-9]b: a0b, a1b
{str1, str2}    匹配str1或str2中的任意字符串，a{ab, cd}b: aabb, acdb
{% endhighlight %}

### 元字符

Shell 除了有通配符之外，由 shell 負責預先先解析後，將處理結果傳給命令行之外，shell 還有一系列自己的其他特殊字符。

{% highlight text %}
IFS      由 space 或 tab 或 enter 三者之一組成(我們常用 space )。
CR       由 enter 產生。
=        設定變量。
$        作變量或運算替換(請不要與 shell prompt 搞混了)。
>        重導向 stdout。
<        重導向 stdin。
|        命令管線。
&        重導向 file descriptor ，或將命令置於背境執行。
( )      將其內的命令置於 nested subshell 執行，或用於運算或命令替換。
{ }      將其內的命令置於 non-named function 中執行，或用在變量替換的界定範圍。
;        在前一個命令結束時，而忽略其返回值，繼續執行下一個命令。
&&       在前一個命令結束時，若返回值為 true，繼續執行下一個命令。
||       在前一個命令結束時，若返回值為 false，繼續執行下一個命令。
!        執行 history 列表中的命令。
{% endhighlight %}

### 轉義字符

有時候，我們想讓通配符，或者元字符變成普通字符，不需要使用它。那麼這裡我們就需要用到轉義符了。shell提供轉義符有三種。


{% highlight text %}
''      硬轉義，其內部所有的shell 元字符、通配符都會被關掉。注意，硬轉義中不允許出現'(單引號)。
""      軟轉義，其內部只允許出現特定的shell 元字符：$用於參數代換 `用於命令代替 \用於轉義。
\       轉義，去除其後緊跟的元字符或通配符的特殊意義。
{% endhighlight %}

如下，是 bash 腳本的執行過程。

![hello world logo]({{ site.url }}/images/linux/bash-wildcard.png "hello world logo"){: .pull-center }

<!--
針對文件操作
–b 	當file存在並且是塊文件時返回真
-c 	當file存在並且是字符文件時返回真
-d 	當pathname存在並且是一個目錄時返回真
-e 	當pathname指定的文件或目錄存在時返回真
-f 	當file存在並且是常規文件時返回真
-g 	當由pathname指定的文件或目錄存在並且設置了SGID位時返回為真
-h 	當file存在並且是符號鏈接文件時返回真，該選項在一些老系統上無效
-k 	當由pathname指定的文件或目錄存在並且設置了“粘滯”位時返回真
-L 	如果 filename 為符號鏈接，則為真
-p 	當file存在並且是命令管道時返回為真
-r 	當由pathname指定的文件或目錄存在並且可讀時返回為真
-s 	當file存在文件大小大於0時返回真
-u 	當由pathname指定的文件或目錄存在並且設置了SUID位時返回真
-w 	當由pathname指定的文件或目錄存在並且可執行時返回真。一個目錄為了它的內容被訪問必然是可執行的
-x 	如果 filename 可執行，則為真
-nt 	如果 前者 比 後者 新，則為真
-ot 	如果 前者 比 後者 舊，則為真
字符串操作
-z 	如果 string 長度為零，則為真
-n 	如果 string 長度非零，則為真
= 	兩字符串相等，則為真
!= 	兩字符串不想等，則為真
數字比較操作
-eq 	等於
-ne 	不等於
-gt 	大於
-lt 	小於
-le 	小於等於
-ge 	大於等於
-->


## Bash 歷史

輸入命令時 Bash 將其保存在內存中，其大小由 `HISTSIZE` 決定，在 Bash 退出時將 `HISTFILESIZE` 的最近的命令保存在 `HISTFILE` 中，在 `~/.bash_profile` 中可以修改設置。

在啟動 Bash 時會從 `HISTFILE` 讀入 `HISTSIZE` 大小的命令行，通常在寫入歷史文件時使用的是覆蓋方式，如果需要以追加方式添加可以 `shopt -s listappend`。


<!--
bash每執行完一條命令，都要顯示一個新的提示符，而在顯示提示符的同時，會執行保存在環境變量PROMPT_COMMAND裡面的命令（通常被用來為終端設置標題），利用這一機制，將如下代碼放入.bashrc文件裡即可在每次輸入命令時即將命令寫入文件。PROMPT_COMMAND="history -a; $PROMPT_COMMAND"。
-->


{% highlight text %}
----- 顯示命令執行的時間戳，之前的命令不會
$ export HISTTIMEFORMAT='%F %T '
----- 剔除連續重複的條目，erasedups剔除整個歷史中重複的條目，ignorespace在不想被記住的命令前加上空格
$ export HISTCONTROL ignoredups
----- 忽略特定的命令
$ export HISTIGNORE="pwd:ls:ls -ltr:"
{% endhighlight %}


可以通過 `Ctrl-R` 和 `Ctrl-S` 來對歷史進行搜索。

{% highlight text %}
history            顯示所有的歷史命令，包括序號。
history N          顯示最近的N條歷史命令。
history -c         清楚所有的歷史命令。
history -d N       清除第N條歷史命令。
!string            用來查找最近的以string開頭歷史命令。
!?string           用來查找最近含有string的命令，不是嚴格以string開頭歷史命令。
^string1^string2^  將上一條命令中的string1替換為string2。
!number            執行歷史中的第number條命令。
!!                 執行上一條命令。
!-number           執行最近的第N條命令。
!$ <<==>> !!:$     獲得上一條命令的最後一個參數。
!^ <<==>> !!:^     獲得上一條命令的第一個參數。
!cp:2              查找最近以cp開頭的命令，並取得第二個參數。

示例：
$ echo a b c d e
a b c d e
$ echo !!:2
b
{% endhighlight %}

<!-- :2對應第二個參數，$對應最後一個參數，^對應第一個參數，*對應所有參數，3-4第3個到第4個參數。-->

## 快捷鍵

{% highlight text %}
移動光標
    Ctrl-A               跳轉到行首。
    Ctrl-E               跳轉到行尾。
    Ctrl-B               左移一個字符。
    Ctrl-F               右移一個字符。
    ESC/Alt-B            左移一個單詞(在 XWindow 中衝突)。
    ESC/Alt-F            右移一個單詞(在 XWindow 中衝突)。
    Ctrl-x-x             光標在當前和行首之間切換。

刪除、複製
    Ctrl-D               刪除光標所在字符，無字符時退出。
    Ctrl-H               與Backspace相同。
    Ctrl-K               清除光標之後，含光標。
    Ctrl-U Alt-BackSpace 清除光標以前內容，當前光標不清除（密碼輸入錯誤）。
    Esc-Backspace/Ctrl-W 清除光標之前的一個單詞。
    Alt-D                刪除光標所在位置後的單詞。
    Esc D                刪除當前以及之後到空格為止的單詞。
    Ctrl-Y               粘貼剛才Ctrl-W或Ctrl-K的內容。

{% endhighlight %}


<!--
<li>歷史<ul>
	<li>Ctrl-P 當前行上一個命令，set -o mode (vi/emacs)。</li>
	<li>Ctrl-N 當前行下一個命令。</li>
	<li>Ctrl-R 在歷史中查找，輸入幾個字符後，Ctrl-Shift-r 向後找。</li>
	<li>Ctrl-J 結束搜索，並將搜索內容輸入到 stdin ，相當於 Enter 。</li>
  <li>Ctrl-I 同 Tab</li>
	<li>Ctrl-G 退出搜索，併到其實位置。</li>
	<li>Alt-Ctrl-Y 輸入上一條命令的第一個參數。</li>
	<li>Alt-./_ 輸入上一條命令的最後一個參數。</li>
</ul></li>

<li>撤銷<ul>
	<li>Ctrl-_ 撤銷上一次的編輯。</li>
	<li>Alt-R 撤銷所有的編輯。</li>
</ul></li>

<li>補全<ul>
	<li>Alt-? 列出所有可能的命令。</li>
	<li>Alt-* 插入所有可能的命令。</li>
</ul></li>

<li>Ubuntu<ul>
	<li>Ctrl-- 減小字體。</li>
	<li>Ctrl-+ 加大字體。</li>
</ul></li>

<li>其他<ul>
	<li>Ctrl-Z 掛起當前進程，也即切換為後臺睡眠，bg 後臺運行，jobs 查看，fg 恢復。</li>
	<li>Ctrl-L 清屏，類似與clear。</li>
	<li>Ctrl-S 將屏幕設置為假死狀態，實際仍可輸入。</li>
	<li>Ctrl-Q 從假死狀態退出。</li>
	<li>Ctrl-T 切換光標之前的兩個字符。</li>
	<li>Esc/Alt-T 切換光標之前的兩個單詞。</li>
	<li>Ctrl-V 加上要輸入的特殊字元， ex: Ctrl-V Ctrl-C 會出現 ^C 而不是中斷。</li>
	<li>Ctrl-V TAB 輸入 TAB，而不是命令列擴展。</li>
	<li>Alt-C 用於將當前光標處的字符變成大寫，同時本光標所在單詞的後續字符都變成小寫。</li>
	<li>Alt-L 用於將光標所在單詞及所在單詞的後續字符都變成小寫。</li>
	<li>Alt-U 用於將光標所在單詞的光標所在處及之後的所有字符變成大寫。</li>
  <li>C-x C-e 可以使用 $EDITER 寫入 script 並讓 Shell 一次執行。</li>
</ul></li></ol>

Bash Shell 可以設多種 Mode, 可參考如下:

    set -o # 可以列出所有 Mode
    set -o vi # 可以用 vi 操作法來操作 Bash Shell(用此 Mode 上述的快速鍵即無作用)
    set -o emacs # Bash 預設就是此 Mode.
-->


{% highlight text %}
{% endhighlight %}
