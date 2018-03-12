---
title: Bash 相关
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: bash,alias,PATH
description: 一些与 Bash 相关的内容，如命令执行顺序、配置文件等。
---

一些与 Bash 相关的内容，如命令执行顺序、配置文件、通配符、元字符及转义字符等。

<!-- more -->

## 命令执行顺序

Shell 一般会按照 `alias` `keyword` `function` `built-in` `$PATH` 的顺序进行搜索。

在执行完命令之后会保存在一个 hash 表中，下次直接通过 hash 查找，如果将一个命令删除可能会出现 `No such file or directory` 的错误；可以通过 `set -h` 启用 hash 功能，`set +h` 禁用。

可通过 `hash` 查看当前列表，`hash -d command` 删除该记录，`hash -r` 删除所有记录，重新搜索。

通过 `hash` 命令查看时，其第一列为使用的次数，注意不会缓存 `function`、`built-in` 以及部分 `alias`；注意，不会缓存包含绝对路径的 `alias` 命令。

> 可以通过 `alias cmd` 查看定义的 alias ，一般通过 `alias cmd='xxx'` 定义别名，`unalias cmd` 删除别名。

常见操作可以参考如下：

{% highlight text %}
----- 可以查看该命令的所有类型
$ type -a cmd
----- 查看执行时的类型
$ type cmd

----- 查看所有的built-in命令
$ enable
----- 禁用built-in的cmd命令
$ enable -n cmd
----- 启用built-in的命令
$ enable cmd
{% endhighlight %}

<!--
command如果有一个function为同名，则执行cmd实际执行的是。
<pre>
function gcc { echo “just a test for gcc”; }
gcc
command gcc
</pre>
-->

## 配置文件


Bash 的很多配置文件，要说清楚这些配置文件的区别，则首先要了解 login shell 和 no login shell 的区别。

* login shell 指的是完整的登录流程，需要输入用户名和密码，例如 `tty1~tty6` 等。
* no login shell，简单来说是 XWindow 登陆后又启动了多个终端，此时不需要输入密码。

Bash 配置文件包括全局配置文件和用户相关的局部配置文件。


### 全局变量

全局配置文件 `/etc/profile` ，在任何用户第一次登陆时 (或在切换用户时使用 `-` 参数) 都会读取该文件。

另外一个是 `/etc/bashrc` ，其中 Ubuntu 没有这个文件，与之对应的是 `/etc/bash.bashrc` ，会在 bash 执行时会读取此文件。

### 局部变量

在执行完全局变量后，执行用户指定的配置文件，依次检查 `~/.bash_profile` `~/.bash_login` `~/.profile` 最先找到的直接执行，然后退出。

其中相关的配置文件有如下：

#### ~/.bash_profile

Ubuntu 默认没有此文件，可新建，只有 Bash 是以 login 形式执行时，才会读取此文件，通常来说该配置文件还会配置成读取 `~/.bashrc`。

#### ~/.bash_login

若 bash 是以 login 方式执行时，读取 `~/.bash_profile` ，若它不存在，则读取 `~/.bash_login`，若两者都不存在，读取 `~/.profile` 。

#### ~/.profile

除了上述的配置文件执行方式外，在图形模式登录时，此文件被读取，即使存在 `~/.bash_profile` 和 `~/.bash_login`。

#### ~/.bashrc

当 Bash 是以 non-login 形式执行时，读取此文件，若以 login 形式执行，则不会读取此文件，但是通常在上述的三个文件中会调用该文件。

#### ~/.bash_logout

注销时，且是 login 形式，此文件才会读取。也就是说，在文本模式注销时，此文件会被读取，图形模式注销时，此文件不会被读取。



### 读取顺序

login shell 和 non-login shell 读取的配置文件数据并不一样。

{% highlight text %}
----- login shell会读配置文件，后面的三个读取一个文件后就会退出
/etc/profile ~/.bash_profile或~/.bash_login或~/.profile

----- non-login shell会读配置文件
~/.bashrc
{% endhighlight %}

<!--
下面是在本机的几个例子：<ol><li>
	图形模式登录时，顺序读取：/etc/profile和~/.profile。</li><li>
	图形模式登录后，打开终端时，顺序读取：/etc/bash.bashrc和~/.bashrc。</li><li>
	文本模式登录时，顺序读取：/etc/profile，/etc/bash.bashrc和~/.bash_profile ~/.bash_login ~/.profile之一。</li><li>从其它用户su到该用户，则分两种情况：<ul><li>
		如果带-l参数（或-参数，--login参数），如：su -l username，则bash是login的，它将顺序读取以下配置文件：/etc/profile，/etc/bash.bashrc和~/.bash_profile ~/.bash_login ~/.profile之一。</li><li>
		如果没有带-l参数，则bash是non-login的，它将顺序读取：/etc/bash.bashrc和~/.bashrc。</li></ul></li><li>
	注销时，或退出su登录的用户，如果是longin方式，那么bash会读取：~/.bash_logout。</li><li>
	执行自定义的shell文件时，若使用“bash -l a.sh”的方式，则bash会读取行：/etc/profile和~/.bash_profile ~/.bash_login ~/.profile之一，若使用其它方式，如：bash a.sh， ./a.sh，sh a.sh（这个不属于bash shell），则不会读取上面的任何文件。。</li></ol>
-->

## 通配符、元字符及转义字符

介绍一些与之相关的概念。

### 通配符

通配符是由 shell 处理的，在遇到了通配符时，shell 会将其当作路径或文件名去在磁盘上搜寻可能的匹配：若符合要求的匹配存在，则进行代换 (路径扩展)；否则就将该通配符作为一个普通字符传递给 "命令"，然后再由命令进行处理。

总之，通配符实际上就是一种 shell 实现的路径扩展功能，在通配符被处理后，shell 会先完成该命令的重组，然后再继续处理重组后的命令，直至执行该命令。

常见示例如下：

{% highlight text %}
*               匹配0或多个字符，a*b: acb, a123db
?               匹配任意单一字符，a?b: acb, a1b
[list]          匹配list中的任意单一字符，a[xyz]b: axb, ayb, azb
[!list]         匹配除list中的任意单一字符，a[!0-9]b: aab, acb, a-b
[c1-c2]         匹配c1到c2中的任意单一字符，a[0-9]b: a0b, a1b
{str1, str2}    匹配str1或str2中的任意字符串，a{ab, cd}b: aabb, acdb
{% endhighlight %}

### 元字符

Shell 除了有通配符之外，由 shell 负责预先先解析后，将处理结果传给命令行之外，shell 还有一系列自己的其他特殊字符。

{% highlight text %}
IFS      由 space 或 tab 或 enter 三者之一组成(我们常用 space )。
CR       由 enter 产生。
=        设定变量。
$        作变量或运算替换(请不要与 shell prompt 搞混了)。
>        重导向 stdout。
<        重导向 stdin。
|        命令管线。
&        重导向 file descriptor ，或将命令置于背境执行。
( )      将其内的命令置于 nested subshell 执行，或用于运算或命令替换。
{ }      将其内的命令置于 non-named function 中执行，或用在变量替换的界定范围。
;        在前一个命令结束时，而忽略其返回值，继续执行下一个命令。
&&       在前一个命令结束时，若返回值为 true，继续执行下一个命令。
||       在前一个命令结束时，若返回值为 false，继续执行下一个命令。
!        执行 history 列表中的命令。
{% endhighlight %}

### 转义字符

有时候，我们想让通配符，或者元字符变成普通字符，不需要使用它。那么这里我们就需要用到转义符了。shell提供转义符有三种。


{% highlight text %}
''      硬转义，其内部所有的shell 元字符、通配符都会被关掉。注意，硬转义中不允许出现'(单引号)。
""      软转义，其内部只允许出现特定的shell 元字符：$用于参数代换 `用于命令代替 \用于转义。
\       转义，去除其后紧跟的元字符或通配符的特殊意义。
{% endhighlight %}

如下，是 bash 脚本的执行过程。

![hello world logo]({{ site.url }}/images/linux/bash-wildcard.png "hello world logo"){: .pull-center }

<!--
针对文件操作
–b 	当file存在并且是块文件时返回真
-c 	当file存在并且是字符文件时返回真
-d 	当pathname存在并且是一个目录时返回真
-e 	当pathname指定的文件或目录存在时返回真
-f 	当file存在并且是常规文件时返回真
-g 	当由pathname指定的文件或目录存在并且设置了SGID位时返回为真
-h 	当file存在并且是符号链接文件时返回真，该选项在一些老系统上无效
-k 	当由pathname指定的文件或目录存在并且设置了“粘滞”位时返回真
-L 	如果 filename 为符号链接，则为真
-p 	当file存在并且是命令管道时返回为真
-r 	当由pathname指定的文件或目录存在并且可读时返回为真
-s 	当file存在文件大小大于0时返回真
-u 	当由pathname指定的文件或目录存在并且设置了SUID位时返回真
-w 	当由pathname指定的文件或目录存在并且可执行时返回真。一个目录为了它的内容被访问必然是可执行的
-x 	如果 filename 可执行，则为真
-nt 	如果 前者 比 后者 新，则为真
-ot 	如果 前者 比 后者 旧，则为真
字符串操作
-z 	如果 string 长度为零，则为真
-n 	如果 string 长度非零，则为真
= 	两字符串相等，则为真
!= 	两字符串不想等，则为真
数字比较操作
-eq 	等于
-ne 	不等于
-gt 	大于
-lt 	小于
-le 	小于等于
-ge 	大于等于
-->


## Bash 历史

输入命令时 Bash 将其保存在内存中，其大小由 `HISTSIZE` 决定，在 Bash 退出时将 `HISTFILESIZE` 的最近的命令保存在 `HISTFILE` 中，在 `~/.bash_profile` 中可以修改设置。

在启动 Bash 时会从 `HISTFILE` 读入 `HISTSIZE` 大小的命令行，通常在写入历史文件时使用的是覆盖方式，如果需要以追加方式添加可以 `shopt -s listappend`。


<!--
bash每执行完一条命令，都要显示一个新的提示符，而在显示提示符的同时，会执行保存在环境变量PROMPT_COMMAND里面的命令（通常被用来为终端设置标题），利用这一机制，将如下代码放入.bashrc文件里即可在每次输入命令时即将命令写入文件。PROMPT_COMMAND="history -a; $PROMPT_COMMAND"。
-->


{% highlight text %}
----- 显示命令执行的时间戳，之前的命令不会
$ export HISTTIMEFORMAT='%F %T '
----- 剔除连续重复的条目，erasedups剔除整个历史中重复的条目，ignorespace在不想被记住的命令前加上空格
$ export HISTCONTROL ignoredups
----- 忽略特定的命令
$ export HISTIGNORE="pwd:ls:ls -ltr:"
{% endhighlight %}


可以通过 `Ctrl-R` 和 `Ctrl-S` 来对历史进行搜索。

{% highlight text %}
history            显示所有的历史命令，包括序号。
history N          显示最近的N条历史命令。
history -c         清楚所有的历史命令。
history -d N       清除第N条历史命令。
!string            用来查找最近的以string开头历史命令。
!?string           用来查找最近含有string的命令，不是严格以string开头历史命令。
^string1^string2^  将上一条命令中的string1替换为string2。
!number            执行历史中的第number条命令。
!!                 执行上一条命令。
!-number           执行最近的第N条命令。
!$ <<==>> !!:$     获得上一条命令的最后一个参数。
!^ <<==>> !!:^     获得上一条命令的第一个参数。
!cp:2              查找最近以cp开头的命令，并取得第二个参数。

示例：
$ echo a b c d e
a b c d e
$ echo !!:2
b
{% endhighlight %}

<!-- :2对应第二个参数，$对应最后一个参数，^对应第一个参数，*对应所有参数，3-4第3个到第4个参数。-->

## 快捷键

{% highlight text %}
移动光标
    Ctrl-A               跳转到行首。
    Ctrl-E               跳转到行尾。
    Ctrl-B               左移一个字符。
    Ctrl-F               右移一个字符。
    ESC/Alt-B            左移一个单词(在 XWindow 中冲突)。
    ESC/Alt-F            右移一个单词(在 XWindow 中冲突)。
    Ctrl-x-x             光标在当前和行首之间切换。

删除、复制
    Ctrl-D               删除光标所在字符，无字符时退出。
    Ctrl-H               与Backspace相同。
    Ctrl-K               清除光标之后，含光标。
    Ctrl-U Alt-BackSpace 清除光标以前内容，当前光标不清除（密码输入错误）。
    Esc-Backspace/Ctrl-W 清除光标之前的一个单词。
    Alt-D                删除光标所在位置后的单词。
    Esc D                删除当前以及之后到空格为止的单词。
    Ctrl-Y               粘贴刚才Ctrl-W或Ctrl-K的内容。

{% endhighlight %}


<!--
<li>历史<ul>
	<li>Ctrl-P 当前行上一个命令，set -o mode (vi/emacs)。</li>
	<li>Ctrl-N 当前行下一个命令。</li>
	<li>Ctrl-R 在历史中查找，输入几个字符后，Ctrl-Shift-r 向后找。</li>
	<li>Ctrl-J 结束搜索，并将搜索内容输入到 stdin ，相当于 Enter 。</li>
  <li>Ctrl-I 同 Tab</li>
	<li>Ctrl-G 退出搜索，并到其实位置。</li>
	<li>Alt-Ctrl-Y 输入上一条命令的第一个参数。</li>
	<li>Alt-./_ 输入上一条命令的最后一个参数。</li>
</ul></li>

<li>撤销<ul>
	<li>Ctrl-_ 撤销上一次的编辑。</li>
	<li>Alt-R 撤销所有的编辑。</li>
</ul></li>

<li>补全<ul>
	<li>Alt-? 列出所有可能的命令。</li>
	<li>Alt-* 插入所有可能的命令。</li>
</ul></li>

<li>Ubuntu<ul>
	<li>Ctrl-- 减小字体。</li>
	<li>Ctrl-+ 加大字体。</li>
</ul></li>

<li>其他<ul>
	<li>Ctrl-Z 挂起当前进程，也即切换为后台睡眠，bg 后台运行，jobs 查看，fg 恢复。</li>
	<li>Ctrl-L 清屏，类似与clear。</li>
	<li>Ctrl-S 将屏幕设置为假死状态，实际仍可输入。</li>
	<li>Ctrl-Q 从假死状态退出。</li>
	<li>Ctrl-T 切换光标之前的两个字符。</li>
	<li>Esc/Alt-T 切换光标之前的两个单词。</li>
	<li>Ctrl-V 加上要输入的特殊字元， ex: Ctrl-V Ctrl-C 会出现 ^C 而不是中断。</li>
	<li>Ctrl-V TAB 输入 TAB，而不是命令列扩展。</li>
	<li>Alt-C 用于将当前光标处的字符变成大写，同时本光标所在单词的后续字符都变成小写。</li>
	<li>Alt-L 用于将光标所在单词及所在单词的后续字符都变成小写。</li>
	<li>Alt-U 用于将光标所在单词的光标所在处及之后的所有字符变成大写。</li>
  <li>C-x C-e 可以使用 $EDITER 写入 script 并让 Shell 一次执行。</li>
</ul></li></ol>

Bash Shell 可以設多種 Mode, 可參考如下:

    set -o # 可以列出所有 Mode
    set -o vi # 可以用 vi 操作法來操作 Bash Shell(用此 Mode 上述的快速鍵即無作用)
    set -o emacs # Bash 預設就是此 Mode.
-->


{% highlight text %}
{% endhighlight %}
