---
title: Linux 常用技巧
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,bash,技巧,经典
description: 简单记录下在 Linux 下常用的一些技巧，以方便查询使用，例如 生成随机字符串，特殊字符文件的处理， sudo 和 su 两个命令的区别等等。
---

简单记录下在 Linux 下常用的一些技巧，以方便查询使用，例如 生成随机字符串，特殊字符文件的处理， sudo 和 su 两个命令的区别等等。

<!-- more -->


## 随机内容

在 Linux 中 ```/dev/urandom``` 可以用来产生真随机的内容，然后直接读取字符串的内容，从而生成随机的字符串。

如下的命令中，C 表示生成的字符串的字符数；L 表示要生成多少行随机字符。

{% highlight text %}
----- 生成全字符随机的字串
$ cat /dev/urandom | strings -n C | head -n L

----- 生成数字加字母的随机字串
$ cat /dev/urandom | sed 's/[^a-zA-Z0-9]//g' | strings -n C | head -n L

----- 也可以直接通过Base64转换
$ head -c 32 /dev/random | base64
{% endhighlight %}

注意，如上的方式中会存在换行符，只有在出现长度是 ```C``` 的字符串时才打印，会导致打印速度很慢；所以，最好是通过如下方式生成，然后手动截取。

{% highlight text %}
$ cat /dev/urandom | sed 's/[^a-zA-Z0-9]//g' | strings | tr -d '\n\r'
{% endhighlight %}

如果要生成随机整数，可以通过 shell 中的一个环境变量 ```RANDOM ```，它的范围是 ```0~32767``` 。

{% highlight text %}
----- 生成 [0, 25] 的随机数
$ echo $(($RANDOM%26))

----- 生成 [6, 100] 的随机数
$ echo $(($RANDOM%95+6))
{% endhighlight %}

另外，```/dev/``` 目录下存在两个相关的随机数发生器，也就是 ```random``` 和 ```urandom``` ，后者也就是 ```unblocked random``` ，可以通过如下方式测试其速度。

{% highlight text %}
$ dd if=/dev/random of=random.dat bs=512 count=5
$ dd if=/dev/urandom of=urandom.dat bs=512 count=5
{% endhighlight %}

相比来说，前者在内核中熵不足时会阻塞，而后者则不会，详细可以查看 [Myths about /dev/urandom](https://www.2uo.de/myths-about-urandom/) 。

<!--
可以参考本地文档Myths about /dev/urandom
/reference/linux/kernel/myths_about_dev_urandom.maff


Linux 中的随机数可从 /dev/{random,urandom} 两个特殊的设备文件中产生，会利用当前系统的熵池计算出一定数量的随机比特，然后将这些比特作为字节流返回。熵池就是当前系统的环境噪音，可以通过很多参数来评估，如内存、文件的使用、不同类型的进程数量等等。

如果当前环境噪音变化的不是很剧烈或者当前环境噪音很小，比如刚开机的时候，而当前需要大量的随机比特，这时产生的随机数的随机效果就不是很好了。

这也就是上述两个文件的区别，前者不能产生新的随机数时会阻塞，直到根据熵池产生新的随机字节之后才返回；而后者不会阻塞，只是此时产生的可能是伪随机数，对加密解密这样的应用来说就不是一种很好的选择。

所以使用 `/dev/random` 比使用 `/dev/urandom` 产生大量随机数的速度要慢。

$ dd if=/dev/random of=random.dat bs=1024b count=1
0+1 records in
0+1 records out
128 bytes (128 B) copied, 0.000169 seconds, 757 kB/s

$ dd if=/dev/urandom of=random.dat bs=1024b count=1
1+0 records in
1+0 records out
524288 bytes (524 kB) copied, 0.091297 seconds, 5.7 MB/s
-->


## 特殊字符文件处理

在 Linux 中，文件名的长度最大可以达到 256 个字符，可以使用字符有：字母、数字、```'.'```(点)、```'_'```(下划线)、```'-'```(连字符)、```' '```(空格)，其中开始字符不建议使用 ```'_'```、```'-'```、```' '``` 字符。```'/'```(反斜线) 用于标示目录，不能用作文件或者文件夹名称。

另外，在 shell 中，```'?'```(问号)、```'*'```(星号)、```'&'``` 字符有特殊含义，同样不建议使用。

在 shell 中，将 ```--``` 之后的内容当作文件。

{% highlight text %}
$ cd .>-a                             ← 创建一个文件，或者 >-a
$ vi -- -a                            ← 编辑，或者 echo "">-a
$ rm -- -a                            ← 删除，或者 rm ./-a
$ touch '><!*'                        ← 创建
$ touch '?* $&'                       ← 创建
{% endhighlight %}

对于这样的文件，可以执行如下操作。

{% highlight text %}
----- 将非乱码的文件移出到某个目录下
$ find . -name "[a-z|A-Z]*" | xargs -I {} mv {} /somepath

----- 也可以通过inode删除
$ ls -i
$ find -inum XXX | xargs -I {} rm {}
$ find -inum XXX -delete
{% endhighlight %}

如果文件的文件名含有终端无法正确显示的字符，那么可以通过 inode 来删除，处理命令如下。

{% highlight text %}
----- 查看文件innode
# ls -li
total 0
358315 -rw-r--r-- 1 root root 0 Apr 6 23:13 ???}

----- 通过inode删除文件，如下两种方式相同
# find . -inum 358315 -delete
# rm -i `find . -maxdepth 1 -inum 358315 -print`
{% endhighlight %}


## sudo VS. su

在切换时实际上是两种策略：1) su 切换到相应的用户，所以需要切换用户的密码；2) sudo 不知道 root 密码的时候执行一些 root 的命令，需要在 suders 中配置+自己用户密码。

{% highlight text %}
$ sudo su root                        ← 需要用户的密码+sudoers配置
$ su root                             ← 需要root用户密码
{% endhighlight %}

注意，之所以使用 ```sudo su root``` 这种方式，可能是 btmp 等类似的文件，只有 root 可以写入，否则会报 Permission Denied，此时可以通过 strace 查看报错的文件。

## 文本替换

命令行批量替换多文件中的字符串，常用有三种方法：Mahuinan 法、Sumly 法和 30T 法。

{% highlight text %}
----- Mahuinan法，用sed命令批量替换多个文件中的字符串
$ sed -i "s/orig/sub/g" "`grep orig -rl directory`"
解读：
    sed -i 选项表示原地替换，若只显示结果则用-e替换；
    g 表示全局，否则只替换第一个匹配字符串；
    grep -r 表示对目录递归调用；-l(L小写)列出匹配的文件。

----- Sumly法
$ perl -pi -e "s/China/Sumly/g" /www/*.htm /www/*.txt
解读：
    将www文件夹下所有的htm和txt文件中的"China"都替换为"Sumly"

----- 30T法
$ perl -pi -e 's/baidu/30T/g' `find /www -type f`
解读：
　　将www文件夹下所有文件，不分扩展名，所有的"baidu"都替换为"30T"
{% endhighlight %}

注意，在 Mahuinan 中，为了防止文件名中存在空格，所以使用引号包裹。

<!--
splint替换，首先通过 $ sed -n "/\/\*@[a-z]\{4,20\}@\*\//p" null1.c进行测试，然后对grep进行测试
$ grep "\/\*@[a-z]\{1,\}@\*\/" -lr .  为防止文件名之间由空格，将其由双引号包裹。 最后的实际命令为：
$ sed -i "s/\/\*@[a-z]\{1,\}@\*\/ //g"  "`grep "\/\*@[a-z]\{1,\}@\*\/" -lr .` "
-->


## install 和 cp 命令

两者都可以将文件/目录拷贝到指定的目录，不过 install 允许你控制目标文件的属性，常用于程序的 Makefile 文件。

{% highlight text %}
install [OPTION]... SOURCE DIRECTORY
常用参数：
    -m, --mode=0700
        设定权限；
    -v, --verbose
        打印处理的每个文件/目录名称；
    -d, --directory
        所有参数都作为目录处理，而且会创建指定目录；
    -g, --group=mysql
        设定所属组，而不是进程目前的所属组；
    -o, --owner=mysql
        设定所有者，只适用于超级用户；
    -s, --strip
        使用strip命令删除文件中的符号表 (symbol table)；
    -S, --suffix=exe
        指定安装文件的后缀；
    -p, --preserve-timestamps
        以源文件的访问/修改时间作为相应的目的地文件的时间属性；
    -t, --target-directory
        如果最后一个文件是一个目录并且没有使用-T,--no-target-directory选项，
        则将每个源文件拷贝到指定的目录，源和目标文件名相同；
{% endhighlight %}

<!--
--backup[=CONTROL]：为每个已存在的目的地文件进行备份。 -b：类似 --backup，但不接受任何参数。
-->

使用 -d 选项时，如果指定安装位置为 /usr/local/foo/bar，一般 /usr/loacal 已存在，install 会直接创建 foo/bar 目录，并把程序安装到指定位置。

如果指定了两个文件名，则将第一个文件拷贝到第二个,

{% highlight text %}
----- 安装目录，等价于mkdir /tmp/bin
$ install --verbose -d -m 0755 /tmp/bin

----- 安装文件，等价于cp a/e c
$ install -v -m 0755 a/e c

----- 安装文件，等价于mkdir -p a/b && cp x a/b/c
$ install -v -m 0755 -D x a/b/c
{% endhighlight %}

## Here Document

可以使用 echo 添加到文件，不过这样会比较麻烦，可以使用如下方式；不过 ```$``` 需要做转义，或者使用 ```"EOF"``` 也可以。

{% highlight text %}
$ cat << EOF >> /tmp/foobar.conf
net.core.rmem_default = 262144
net.core.rmem_max = 262144
net.core.wmem_default = 262144
net.core.wmem_max = 262144
export PATH=\$PATH:\$HOME/bin
EOF
{% endhighlight %}

在此使用的就是 Here Document，这是一种在 Linux Shell 中的一种特殊的重定向方式，它的基本的形式如下，通常 delimiter 使用 EOF ：

{% highlight text %}
cmd << delimiter
  Here Document Content
delimiter
{% endhighlight %}

也可以在终端中输入 ```cat << EOF``` ，然后输入多行信息，最终以 EOF 结束，其中间输入的信息将会回显在屏幕上。

另外，可以通过 ```<<-``` 删除 Here Document 的内容部分每行前面的 tab (制表符) ， 这种用法是为了编写 Here Document 的时候可以将内容部分进行缩进，方便阅读代码。


## 避免误删目录


今天就来聊聊 linux 下一个常见的问题：如何避免误删目录。下文会详细的讲述不同的场景下误删目录，以及相应的解决方案。

{% highlight bash %}
#--- 1. 变量为空导致误删文件，如果file为空或命令返回空
rm -rf /usr/sbin/$file
#--- 使用变量扩展功能，变量为空使用默认值或抛出异常退出
rm -rf /usr/sbin/${file:?var is empty}
#--- 人肉判断变量是否为空
[[ ${file} == "" ]] && echo 1
[[ -z ${file} ]] && echo 1

#--- 2. 路径含有空格导致误删文件
path="/usr/local /sbin"
rm -rf $path
#--- 变量加引号防止扩展
rm -rf "$path"

#--- 3. 目录或文件含有特殊字符导致误删文件，例如 "~"
#--- 变量加引号防止扩展
rm -rf "~"

#--- 4. cd切换目录失败，导致文件被误删
#--- 使用逻辑短路操作
cd path && rm -rf *.exe
#--- 检测path是否存在
[[ -d path ]] && echo 1
{% endhighlight %}


## man

Linux 上的 manpage 是用 groff 语法编写的，实际上可以通过如下的命令查看：

{% highlight text %}
zcat man.1.gz | groff -man -Tascii | more
{% endhighlight %}

其查找路径可以通过 ```man -w``` 命令查看，或者查看配置文件 /etc/man.config；很多帮助文档保存在 /usr/share/man/ 目录下，又按照不同类型保存不同的子目录下，例如 /usr/share/man/man1/mysqlshow.1.gz 。

如下是常见的查看命令：

{% highlight text %}
----- 查看搜索路径
$ man -w
----- 显示man命令搜索到的第一个文件路径
$ man -w passwd
----- 显示所有匹配的man文档
$ man -aw passwd

----- 指定领域限制
$ man 5 passwd
$ man -S 1:2 passwd

----- 同命令whatis ，将在whatis数据库查找以关键字开同的帮助索引信息
$ man -f httpd
----- 同命令apropos 将搜索whatis数据库，模糊查找关键字
$ man -k httpd
{% endhighlight %}




## 备份脚本

如下是一个备份用的脚本，不过 email 没有调试使用过，暂时记录下。

* `~/.backuprc` 配置文件，列举出那些文件需要备份，使用 `#` 做注释；
* 使用 `~/tmp` 作为临时目录；

另外，在使用 tar 备份打包+解压时，默认为相对路径，为了使用绝对路径可以在压缩+解压时都使用 ```-P``` 参数，这样直接解压即可覆盖原有文件。

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

如下，是一个配置文件。

{% highlight text %}
/home/foobar/.vimrc
/home/foobar/.tmux
/home/foobar/.tmux.conf
{% endhighlight %}


## 日志清理脚本

在 Linux 中可以通过 logrotate 对日志进行归档，如下是一个日志清理的脚本。

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

## 换行处理

*nix 系统里，每行结尾只有 ```"<换行>"```，即 ```"\n"``` ；Windows 系统里面，每行结尾是 ```"<换行><回车>"``` ，即 ```"\n\r"``` 。

如果不进行转换，那么 *nix 系统下的文件在 Windows 里打开所有文字会变成一行；而 Windows 里的文件在 *nix 下打开的话，在每行的结尾可能会多出一个 ```^M``` 符号。

要把文件转换一下，有两种方法：

1. 命令 ```dos2unix test.file```；
2. 去掉 ```"\r"``` ，用命令 ```sed -i 's/\r//' test.file``` 。


## 字符集设置

程序运行时需要使用一套语言环境，包括了：字符集 (数据) 和字体 (显示)，在 Linux 中通过 locale 来设置程序运行的不同语言环境，locale 由 ANSI C 提供支持，可以根据不同的国家地区设置不同的语言环境。

locale 的命名规则为 ```<语言>_<地区>.<字符集编码>``` ，例如 ```zh_CN.UTF-8``` 中 ```zh``` 代表中文，```CN``` 代表大陆地区，```UTF-8``` 表示字符集。另外，在 locale 会通过一组环境变量，针对不同场景配置。

{% highlight text %}
LC_COLLATE
  定义该环境的排序和比较规则
LC_CTYPE
  用于字符分类和字符串处理，控制所有字符的处理方式，包括字符编码，字符是单字节还是
  多字节，如何打印等，是最重要的一个环境变量。
LC_MONETARY
  货币格式。
LC_NUMERIC
  非货币的数字显示格式。
LC_TIME
  时间和日期格式。
LC_MESSAGES
  提示信息的语言，与之详细的还有LANGUAGE参数，当LANGUAGE设置后LC_MESSAGES将会失效，
  而且可同时设置多种语言信息，如LANGUANE="zh_CN.GB18030:zh_CN.GB2312:zh_CN"。
LANG
  LC_*的默认值，是最低级别的设置，如果LC_*没有设置，则使用该值。
LC_ALL
  一个宏，如果该值设置了，则该值会覆盖所有LC_*的设置值。注意，LANG的值不受该宏影响。
{% endhighlight %}

简单介绍下常见的操作。

{% highlight text %}
----- 设置成中文环境
export LANG="zh_CN.UTF-8"
export LANGUAGE="zh_CN:zh:en_US:en"

----- 设置成英文环境
export LANG="en_US.UTF-8"
export LANGUAGE="en_US:en"

----- 查看现有语言环境
$ locale

----- 所有可用语言环境
$ locale -a
{% endhighlight %}

注意，图形界面可能需要更多的设置，暂时先不讨论了。


## 历史命令


{% highlight text %}
----- 去除重复命令，包括不连续的命令，通过ignoredups去除连续重复的命令
export HISTCONTROL=erasedups
{% endhighlight %}

<!--
https://linuxtoy.org/archives/history-command-usage-examples.html
-->


## 杂项

### 目录合并

可以将两个目录通过 `cp -r dir1/* dir2/* merged/` 进行合并，由于是复制，对于较大的文件会导致速度较慢。如果通过 `mv` 则会报 `Directory not empty` 的错误。

可以通过如下命令采用硬链接的方式进行复制，通过 `tree --inodes` 命令查看文件的 inode 号。

{% highlight text %}
$ cp -r --link dir1/* dir2/* merged/
{% endhighlight %}

### shell 操作

{% highlight text %}
----- 查看当前所有shell
$ chsh -l
$ cat /etc/shells
{% endhighlight %}

查看当前的 Shell ，可以查看 `SHELL` 变量，不过如果是通过 bash dash sh 等直接运行的话，那么 SHELL 不变，可以通过如下方式查看。

{% highlight text %}
$ ps | grep $$ | awk '{print $4}'
$ echo $0
$ tom                                # 输入没有的命令
{% endhighlight %}

`chsh -s /bin/dash` 实际修改的是 `/etc/passwd` 文件里和你的用户名相对应的那一行，重启 Shell 后即可。

### 其它

{% highlight text %}
----- 生成随机文件
$ head -c 10M < /dev/random > /tmp/foobar.txt

----- 日期转换
$ date +%s -d "04/24/2014 15:30:00"         // 将日期转换成时间戳
$ date -d @1398324600                       // 将时间戳转换成日期
$ date +%s                                  // 将当前日期转换成时间戳
$ date -d "1970-01-01 UTC `echo "$(date +%s)-$(cat /proc/uptime|cut -f 1 -d' ')+12288812.926194"|bc ` seconds"
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
