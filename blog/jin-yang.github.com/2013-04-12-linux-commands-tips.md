---
title: Linux 常用命令 -- 杂项
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,命令
description: 主要介绍下在 Linux 中，与文本处理相关的命令。
---

主要介绍下在 Linux 中，日常用到的一些常用指令，如 find 。

<!-- more -->

## find

对于 find 命令，即使系统中含有网络文件系统 (NFS)， find 命令在该文件系统中同样有效，只要具有相应的权限。

如下是 find 命令的格式。

{% highlight text %}
find [-H] [-L] [-P] [-D debugopts] [-Olevel] [path...] [expression]
{% endhighlight %}

路径默认为当前路径，默认 expression 为 `-print` ，参数通常以 `-`、`(`、`!` 开头。

find 命令会从左到右检测 expression 的条件，当知道结果时退出，如 and 为假时， or 为真时。如果在路径中使用通配符，则路径需要以 `./` 或 `/` 开头。

如下是常用选项：

{% highlight text %}
-P 默认选项。对于符号链接，检查时检测符号链接本身。
-L 对于符号链接，检查时检测符号链接指向的内容，如果指向文件不在或find无法找到，则使用链接符号。
-H 除非是处理命令行，否则只针对符号本身。

-print 将匹配的文件输出到标准输出，查找结果使用换行间隔。
-print0 查找结果使用NULL间隔。

-exec 对匹配的文件执行该参数所给出的 shell 命令，相应的形式为 'command' {  } \;，注意 {   } 和 \; 之间的空格。
-ok 和 -exec 相同，不过在执行每一个命令之前，都会给出提示，让用户来确定是否执行。

-name 按照指定文件名搜索。可以使用通配符，此时需要加引号。
-perm 指定文件的权限。
	mode 指定具体的权限，严格匹配，如 -perm g=w, 0020 。
	-mode 转换为二进制后，为 1 的必须都匹配。
	+/mode 转换为二进制后，为 1 的任何一位匹配即可，最好使用 / 。

-prune 不在当前指定的目录中查找，如果同时使用 -depth 选项，那么该选项被忽略。
-user username 指定用户名，也可以为 ID 。
-group groupname 指定组名，也可以为 ID 。
-amin +-n 按照文件访问时间来查找文件，n正好是n分钟，-n指n分钟以内，+n指n分钟以前。
-atime +-n 按照文件访问时间来查找文件，n正好是n天，-n指n天以内，+n指n天以前。
-cmin +-n 按照文件状态时间来查找文件，n正好是n分钟，-n指n分钟以内，+n指n分钟以前。
-ctime +-n 按照文件状态时间来查找文件，n正好是n天，-n指n天以内，+n指n天以前。
-mmin +-n 按照文件更改时间来查找文件，n正好是n分钟，-n指n分钟以内，+n指n分钟以前。
-mtime +-n 按照文件更改时间来查找文件，n正好是n天，-n指n天以内，+n指n天以前。
-nogroup 查无有效属组的文件，即文件的属组在/etc/groups中不存在。
-nouser 查无有效属主的文件，即文件的属主在/etc/passwd中不存在。
-group groupname 指定组名，也可以为ID。
-newer/cnewer/anewer file1 !file2 文件的修改时间/状态时间/访问时间，比file1早，比file2晚。
-newerXY reference XY可以是a/B/c/m/t，访问时间/创建时间（不支持）/状态时间/修改时间/指定时间。
-type b/d/c/p/l/f/s 块设备、目录、字符设备、管道、符号链接、普通文件、socket。
-size n[cwbkMG] 长度为n字节c/双字节w/块512字节b/千字节k/兆字节M/吉字节G的文件。
-fstype 查位于某一类型文件系统中的文件，这些文件系统类型通常可在 /etc/fstab 中找到。
-follow 如果遇到符号链接文件，就跟踪链接所指的文件。
-maxdepth levels 查找目录时的最大深度。
-path pattern 指定目录，可以使用通配符，与 Shell 相同。如果find .则path必须使用./doc，且不能以/结尾。
-depth 在查找文件时，首先查找当前目录中的文件，然后再在其子目录中查找。
{% endhighlight %}

### 示例

#### 查找文件

{% highlight text %}
find / -path "/etc*" -name "*.txt"
{% endhighlight %}

查找 `/etc` 目录下，且后缀是 `txt` 的文件，使用 `-iname` 忽略文件名的大小写。

#### 用户andy所拥有的文件

{% highlight text %}
find /etc -name "passwd*" -exec grep "andy" {} \;
find . -name "*.cpp" | xargs grep 'efg'
{% endhighlight %}

首先匹配所有文件名为 `"passwd*"` 的文件，如 `passwd`、`passwd.old`，然后执行 `grep` 命令看看在这些文件中是否存在一个 andy 用户。注意 `{` `}` 之间没有空格，相当于前面查找到的文件。

#### 忽略某个目录

{% highlight text %}
find /etc -path "/etc/fonts" -a -prune -o -print
{% endhighlight %}

可以使用 `-prune` 指定需要忽略的目录，`-a` 可以去掉。使用该选项需要注意的是，如果同时使用了 `-depth` 选项，那么 `-prune` 选项就会被 find 命令忽略。

`-a` `-o` 均支持短路操作，因此实际上述等于如下的伪代码：`if -path "/etc/fonts": -prune else: -print` 。

#### 忽略多个目录

{% highlight text %}
find /usr \( -path /usr/dir -o -path /usr/file \) -a -prune -o -print
{% endhighlight %}

`-a` 选项可以去掉，表示 `and` ，`-o` 表示 `or` 或者是 `-not`，`-prune` 始终会返回 `1` 。

#### 查找某一特定文件

{% highlight text %}
find /usr \( -path /usr/dir -o -path /usr/file \) -a -prune -o -name "test" -print
{% endhighlight %}

注意，`-name` 等，应该放置到最后一个 `-o` 选项后面。


<!--
	<pre>find /etc/rc.d -name '*crond' -type f -perm 755 -size 10M -exec file '{}' \;</pre>
    -exec 可以使用 -ok 代替，此时会询问是否执行命令。注意： exec 和 file 间是一个空格， file 和 {} 间是一个空格， {} 和 \; 之间是一个空格， \; 是一个整体， \ 表示转义。 {} 表示文件名，也就是 find 前面处理过程中过滤出来的文件，用于 command 命令进行处理。<pre>find / -size +50M -size -100M -exec ls -lh {} \; 2>&1 | grep -v Permission</pre>查找大于 50MB 小于 100MB 的文件。</li><br><li>


    <pre>find /tmp -name core -type f -print[-print0] | xargs [-0] /bin/rm -f<br>find . -name '*wine*' -exec rm -f { } \;</pre>
	其中 xargs 将输入分隔成不同的小块，并执行。 exec 将所有匹配的参数传递给命令，此时可能会由于命令太长而出错。如果文件或是目录名含有空格，则可能会有些问题，这是因为 xargs 默认会按照空白字符来划分输入，通过 -0 选项即可。</li><br><li>

	<pre>find . -type f \( -name 'core' -o[-or/-a/-and/!/-not] -name '*.o' \)</pre>
	其中 () 用来表示强制的优先级。</li><br><li>



	<pre>find . ! -type d -print</pre>
	在当前目录下查找除目录以外的所有类型的文件。</li><br><li>


	<pre>find / -name "CON.FILE" -depth -print</pre>
	先匹配所有的文件，再在子目录中查找。如，使用 find 命令备份文件系统时，希望首先备份所有的文件，其次再备份子目录中的文件。</li><br><li>


	<pre>ssh 192.168.30.137 "ps -ef | grep httpd | awk '{print $2}' | xargskill -9"</pre>
	在杀死远程主机的进程的时候，显示的信息会在本地显示。</li><br><li>


	<pre>find . -name "jdk*" -type f -print | xargs file</pre>
	查看所有以 jdk 开头文件的文件类型。</li><br><li>


	<pre>find / -name "core" -print | xargs echo "" > /tmp/core.log</pre>
	在整个系统中查找内存信息转储文件 (core dump) ，然后把结果保存到 /tmp/core.log 文件中。</li><br><li>


	<pre>find . -perm 777 -print | xargs chmod o-w<br>find / -type f -perm 777 -print -exec chmod 644 {} \;</pre>
	在当前目录下查找所有用户具有读、写和执行权限的文件，并收回相应的写权限。
	<pre>
find / -type f ! -perm 777    // 权限非 777 的文件
find / -perm 2644             // SGID 位已设置，且权限为 644
find / -perm 1551             // Sticky 位已设置，且权限为 551
find / -perm /u=s             // 已设置 SUID 的文件
find / -perm /g+s             // 已设置 SGID 的文件
find / -perm /u=r             // 只读文件
find / -perm /a=x             // 可执行文件
</pre></li><br><li>


	<pre>find /tmp -type f -empty</pre>
	查找空文件，或者空目录 -type d 。</li><br><li>

	<pre>find / -mtime +50 –mtime -100<br>find / -mmin -60</pre>
	查找最近 50-100 天内修改的文件；最近一个小时修改的文件。</li><br><li>
	-->

#### 无主文件

{% highlight text %}
find /home -nouser -print
{% endhighlight %}

查找属主帐户已经被删除的文件，这样就能够找到那些属主在 `/etc/passwd` 文件中没有有效帐户的文件。

#### 查看某个时间点文件

{% highlight text %}
find ./ -name "jdk*" | xargs ls -l --full-time 2>/dev/null | grep "2011-11-11 03:41:54"
{% endhighlight %}

查找 `2011-11-11 03:41:54` 时更改过的 jdk 文件。

{% highlight text %}
{% endhighlight %}



{% highlight text %}
{% endhighlight %}
