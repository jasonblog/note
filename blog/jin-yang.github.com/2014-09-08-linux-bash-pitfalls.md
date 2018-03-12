---
title: Bash 安全编程
layout: post
comments: true
language: chinese
category: [linux, program]
keywords: bash,编程,安全
description:
---

简单介绍下 Linux 中 Bash 编程所需要注意的内容。

<!-- more -->

## 防止未初始化

对于如下场景，通常会导致预想不到的错误。

{% highlight bash %}
chroot=$may_not_exist
rm -rf $chroot/usr
{% endhighlight %}

那么上面的脚本可能会导致 `/usr` 目录被删除。

可以在脚本中使用 `set -u` 或者 `set -o nounset`，或者在命令行中使用 `bash -u your.sh` 。

## 执行错误退出

通常对于一些命令的返回值，可以通过如下的方式检查返回值，如果有异常则退出。

{% highlight bash %}
#----- 示例1
command
if [ "$?"-ne 0]; then echo "command failed"; exit 1; fi
#----- 示例2
command || { echo "command failed"; exit 1; }
#----- 示例3
if ! command; then echo "command failed"; exit 1; fi
{% endhighlight %}

可以通过 `set -e` 或者 `set -o errexit` 明确告知 Bash ，一但有任何一个语句返回非真的值则退出，避免错误滚雪球般的变成严重错误。

如果必须使用返回非 0 值的命令，或者对返回值不敏感，那么可以使用 `command || true`；如果有一段很长的代码，可以暂时关闭错误检查功能，当然需要谨慎使用。

{% highlight bash %}
set +e
# some commands
set -e
{% endhighlight %}

另外，需要注意管道的使用，Bash 默认返回管道中最后一个命令的值，例如 `false | true` 将会被认为命令执行成功，如果想让这样的命令被认为是执行失败，可以使用 `set -o pipefail` 命令进行设置。

## 设置陷阱

当程序异常退出时，通常需要处理一些处于中间状态的变量，例如锁文件、临时文件等等。

Bash 提供了一种方法，当收到一个 UNIX 信号时，可以运行一个命令或者一个函数；此时就需要使用 trap 命令。

{% highlight bash %}
trap command signal [signal ...]
{% endhighlight %}

所有的信号量可以通过 `kill -l` 查看，通常使用较多的是三个：INT、TERM 和 EXIT。

{% highlight text %}
INT   使用Ctrl-C终止脚本时被触发
TERM  使用kill杀死脚本进程时被触发
EXIT  伪信号，当脚本正常退出或者set -e后因为出错而退出时被触发
{% endhighlight %}

### 锁文件

当你使用锁文件时，可以这样写：

{% highlight bash %}
if [ ! -e $lockfile ]; then
    touch $lockfile
    critical-section
    rm $lockfile
else
    echo "critical-section is already running"
fi
{% endhighlight %}

如果 `critical-section` 在运行时被杀死了，那么锁文件仍然存在，但是在删除之前该脚本就无法运行了；此时就需要通过如下的方式进行设置。

{% highlight bash %}
if [ ! -e $lockfile ]; then
    trap " rm -f $lockfile; exit" INT TERM EXIT
    touch $lockfile
    critical-section
    rm $lockfile
    trap - INT TERM EXIT
else
    echo "critical-section is already running"
fi
{% endhighlight %}

这样，即使在 `critical-section` 运行时被杀死，锁文件会一同被删除。


<!--
### 竟态条件

另外，在上面锁文件示例中，会存在一个竟态条件，也就是在判断锁文件和创建锁文件之间。

其中一个可行的解决方法是使用IO重定向和bash的noclobber(wikipedia)模式，重定向到不存在的文件。我们可以这么做：
if ( set -o noclobber; echo "$$" > "$lockfile") 2> /dev/null;
then
trap 'rm -f "$lockfile"; exit $?' INT TERM EXIT
critical-section
rm -f "$lockfile"
trap - INT TERM EXIT
else
echo "Failed to acquire lockfile: $lockfile"
echo "held by $(cat $lockfile)"
fi
更复杂一点儿的问题是你要更新一大堆文件，当它们更新过程中出现问题时，你是否能让脚本挂得更加优雅一些。你想确认那些正确更新了，哪些根本没有变化。比如你需要一个添加用户的脚本。
add_to_passwd $user
cp -a /etc/skel /home/$user
chown $user /home/$user -R
当磁盘空间不足或者进程中途被杀死，这个脚本就会出现问题。在这种情况下，你也许希望用户账户不存在，而且他的文件也应该被删除。
rollback() {
del_from_passwd $user
if [ -e /home/$user ]; then
rm -rf /home/$user
fi
exit
}

trap rollback INT TERM EXIT
add_to_passwd $user

cp -a /etc/skel /home/$user
chown $user /home/$user -R
trap - INT TERM EXIT
在脚本最后需要使用trap关闭rollback调用，否则当脚本正常退出的时候rollback将会被调用，那么脚本等于什么都没做。
保持原子化

又是你需要一次更新目录中的一大堆文件，比如你需要将URL重写到另一个网站的域名。你也许会写：
for file in $(find /var/www -type f -name "*.html"); do
perl -pi -e 's/www.example.net/www.example.com/' $file
done
如果修改到一半是脚本出现问题，一部分使用www.example.com，而另一部分使用www.example.net。你可以使用备份和trap解决，但在升级过程中你的网站URL是不一致的。
解决方法是将这个改变做成一个原子操作。先对数据做一个副本，在副本中更新URL，再用副本替换掉现在工作的版本。你需要确认副本和工作版本目录在同一个磁盘分区上，这样你就可以利用Linux系统的优势，它移动目录仅仅是更新目录指向的inode节点。
cp -a /var/www /var/www-tmp
for file in $(find /var/www-tmp -type -f -name "*.html"); do
perl -pi -e 's/www.example.net/www.example.com/' $file
done
mv /var/www /var/www-old
mv /var/www-tmp /var/www
-->

## 其它

通常不同的命令在执行时，不同的参数可能会有不同的行为，例如 `mkdir -p` 可以防止父目录不存在时报错；`rm -f` 可以防止文件不存在时报错等等。

### 空格处理

一定要注意处理好文件中可能出现空格的场景，也就是要用引号包围变量，示例如下：

{% highlight bash %}
#----- 变量中有空格时会导致异常
if [ $filename = "foo" ];
#----- 正常应该使用如下的方式
if [ "$filename" = "foo" ];
{% endhighlight %}

另外，使用命令行参数时同样需要注意，例如 `$@` 变量，因为空格隔开的两个参数会被解释成两个独立的部分。

{% highlight bash %}
$ foo() { for i in $@; do echo $i; done }; foo bar "hello world"
bar
hello
world
$ foo() { for i in "$@"; do echo $i; done }; foo bar "hello world"
bar
hello world
{% endhighlight %}

还有，在同时使用 `find` 和 `xargs` 命令时，应该使用 `-print0` 来让字符分割文件名，而不是通过换行符分割。

{% highlight bash %}
$ touch "foo bar"
$ find | xargs ls
ls: ./foo: No such file or directory
ls: bar: No such file or directory
$ find -print0 | xargs -0 ls
./foo bar
{% endhighlight %}



<!--


下面就逐个分析一下这篇文章中提到的错误。不是完全的翻译，有些没用的话就略过了， 有些地方则加了些注释。

    for i in `ls *.mp3`

    常见的错误写法：

     for i in `ls *.mp3`; do     # Wrong!

    为什么错误呢？因为for…in语句是按照空白来分词的，包含空格的文件名会被拆成多个词。 如遇到 01 - Don’t Eat the Yellow Snow.mp3 时，i的值会依次取 01，-，Don’t，等等。

    用双引号也不行，它会将ls *.mp3的全部结果当成一个词来处理。

     for i in "`ls *.mp3`"; do   # Wrong!

    正确的写法是

     for i in *.mp3; do

    cp $file $target

    这句话基本上正确，但同样有空格分词的问题。所以应当用双引号：

     cp "$file" "$target"

    但是如果凑巧文件名以 - 开头，这个文件名会被 cp 当作命令行选项来处理，依旧很头疼。可以试试下面这个。

     cp -- "$file" "$target"

    运气差点的再碰上一个不支持 – 选项的系统，那只能用下面的方法了：使每个变量都以目录开头。

     for i in ./*.mp3; do
       cp "$i" /target
       ...

    [ $foo = "bar" ]

    当$foo为空时，上面的命令就变成了

     [ = "bar" ]

    类似地，当$foo包含空格时：

     [ multiple words here = "bar" ]

    两者都会出错。所以应当用双引号将变量括起来：

     [ "$foo" = bar ]      # 几乎完美了。

    但是！当$foo以 - 开头时依然会有问题。 在较新的bash中你可以用下面的方法来代替，[[ 关键字能正确处理空白、空格、带横线等问题。

     [[ $foo = bar ]]      # 正确

    旧版本bash中可以用这个技巧（虽然不好理解）：

     [ x"$foo" = xbar ]    # 正确

    或者干脆把变量放在右边，因为 [ 命令的等号右边即使是空白或是横线开头，依然能正常工作。 （Java编程风格中也有类似的做法，虽然目的不一样。）

     [ bar = "$foo" ]      # 正确

    cd `dirname "$f"`

    同样也存在空格问题。那么加上引号吧。

     cd "`dirname "$f"`"

    问题来了，是不是写错了？由于双引号的嵌套，你会认为`dirname 是第一个字符串，`是第二个字符串。 错了，那是C语言。在bash中，命令替换（反引号``中的内容）里面的双引号会被正确地匹配到一起， 不用特意去转义。

    $()语法也相同，如下面的写法是正确的。

     cd "$(dirname "$f")"

    [ "$foo" = bar && "$bar" = foo ]

    [ 中不能使用 && 符号！因为 [ 的实质是 test 命令，&& 会把这一行分成两个命令的。应该用以下的写法。

     [ bar = "$foo" -a foo = "$bar" ]       # Right!
     [ bar = "$foo" ] && [ foo = "$bar" ]   # Also right!
     [[ $foo = bar && $bar = foo ]]         # Also right!

    [ $foo > 7 ]

    很可惜 [[ 只适用于字符串，不能做数字比较。数字比较应当这样写：

     (( $foo > 7 ))

    或者用经典的写法：

     [ $foo -gt 7 ]

    但上述使用 -gt 的写法有个问题，那就是当 $foo 不是数字时就会出错。你必须做好类型检验。

    这样写也行。

     [[ $foo -gt 7 ]]

    grep foo bar | while read line; do ((count++)); done

    这行代码数出bar文件中包含foo的行数，虽然很麻烦（等同于grep -c foo bar或者 grep foo bar | wc -l）。 乍一看没有问题，但执行之后count变量却没有值。因为管道中的每个命令都放到一个新的子shell中执行， 所以子shell中定义的count变量无法传递出来。

    if [grep foo myfile]

    初学者常犯的错误，就是将 if 语句后面的 [ 当作if语法的一部分。实际上它是一个命令，相当于 test 命令， 而不是 if 语法。这一点C程序员特别应当注意。

    if 会将 if 到 then 之间的所有命令的返回值当作判断条件。因此上面的语句应当写成

     if grep foo myfile > /dev/null; then

    if [bar="$foo"]

    同样，[ 是个命令，不是 if 语句的一部分，所以要注意空格。

     if [ bar = "$foo" ]

    if [ [ a = b ] && [ c = d ] ]

    同样的问题，[ 不是 if 语句的一部分，当然也不是改变逻辑判断的括号。它是一个命令。可能C程序员比较容易犯这个错误？

    if [ a = b ] && [ c = d ]        # 正确

    cat file | sed s/foo/bar/ > file

    你不能在同一条管道操作中同时读写一个文件。根据管道的实现方式，file要么被截断成0字节，要么会无限增长直到填满整个硬盘。 如果想改变原文件的内容，只能先将输出写到临时文件中再用mv命令。

    sed 's/foo/bar/g' file > tmpfile && mv tmpfile file

    echo $foo

    这句话还有什么错误码？一般来说是正确的，但下面的例子就有问题了。

    MSG="Please enter a file name of the form *.zip"
    echo $MSG         # 错误！

    如果恰巧当前目录下有zip文件，就会显示成

    Please enter a file name of the form freenfss.zip lw35nfss.zip

    所以即使是echo也别忘记给变量加引号。

    $foo=bar

    变量赋值时无需加 $ 符号——这不是Perl或PHP。

    foo = bar

    变量赋值时等号两侧不能加空格——这不是C语言。

    echo <<EOF

    here document是个好东西，它可以输出成段的文字而不用加引号也不用考虑换行符的处理问题。 不过here document输出时应当使用cat而不是echo。

    # This is wrong:
    echo <<EOF
    Hello world
    EOF

    # This is right:
    cat <<EOF
    Hello world
    EOF

    su -c 'some command'

    原文的意思是，这条基本上正确，但使用者的目的是要将 -c ‘some command’ 传给shell。 而恰好 su 有个 -c 参数，所以su 只会将 ‘some command’ 传给shell。所以应该这么写：

    su root -c 'some command'

    但是在我的平台上，man su 的结果中关于 -c 的解释为

    -c, --commmand=COMMAND
                pass a single COMMAND to the shell with -c

    也就是说，-c ‘some command’ 同样会将 -c ‘some command’ 这样一个字符串传递给shell， 和这条就不符合了。不管怎样，先将这一条写在这里吧。

    cd /foo; bar

    cd有可能会出错，出错后 bar 命令就会在你预想不到的目录里执行了。所以一定要记得判断cd的返回值。

    cd /foo && bar

    如果你要根据cd的返回值执行多条命令，可以用      。

    cd /foo || exit 1;
    bar
    baz

    关于目录的一点题外话，假设你要在shell程序中频繁变换工作目录，如下面的代码：

    find ... -type d | while read subdir; do
      cd "$subdir" && whatever && ... && cd -
    done

    不如这样写：

    find ... -type d | while read subdir; do
      (cd "$subdir" && whatever && ...)
    done

    括号会强制启动一个子shell，这样在这个子shell中改变工作目录不会影响父shell（执行这个脚本的shell）， 就可以省掉cd - 的麻烦。

    你也可以灵活运用 pushd、popd、dirs 等命令来控制工作目录。

    [ bar == "$foo" ]

    [ 命令中不能用 ==，应当写成

    [ bar = "$foo" ] && echo yes
    [[ bar == $foo ]] && echo yes

    for i in {1..10}; do ./something &; done

    & 后面不应该再放 ; ，因为 & 已经起到了语句分隔符的作用，无需再用;。

    for i in {1..10}; do ./something & done

    cmd1 && cmd2 || cmd3

    有人喜欢用这种格式来代替 if…then…else 结构，但其实并不完全一样。如果cmd2返回一个非真值，那么cmd3则会被执行。 所以还是老老实实地用 if cmd1; then cmd2; else cmd3 为好。

    UTF-8的BOM(Byte-Order Marks)问题

    UTF-8编码可以在文件开头用几个字节来表示编码的字节顺序，这几个字节称为BOM。但Unix格式的UTF-8编码不需要BOM。 多余的BOM会影响shell解析，特别是开头的 #!/bin/sh 之类的指令将会无法识别。

    MS-DOS格式的换行符(CRLF)也存在同样的问题。如果你将shell程序保存成DOS格式，脚本就无法执行了。

    $ ./dos
    -bash: ./dos: /bin/sh^M: bad interpreter: No such file or directory

    echo "Hello World!"

    交互执行这条命令会产生以下的错误：

    -bash: !": event not found

    因为 !” 会被当作命令行历史替换的符号来处理。不过在shell脚本中没有这样的问题。

    不幸的是，你无法使用转义符来转义!：

    $ echo "hi\!"
    hi\!

    解决方案之一，使用单引号，即

    $ echo 'Hello, world!'

    如果你必须使用双引号，可以试试通过 set +H 来取消命令行历史替换。

    set +H
    echo "Hello, world!"

    for arg in $*

    $*表示所有命令行参数，所以你可能想这样写来逐个处理参数，但参数中包含空格时就会失败。如：

    #!/bin/bash
    # Incorrect version
    for x in $*; do
      echo "parameter: '$x'"
    done

    $ ./myscript 'arg 1' arg2 arg3
    parameter: 'arg'
    parameter: '1'
    parameter: 'arg2'
    parameter: 'arg3'

    正确的方法是使用 $@。

    #!/bin/bash
    # Correct version
    for x in "$@"; do
      echo "parameter: '$x'"
    done

    $ ./myscript 'arg 1' arg2 arg3
    parameter: 'arg 1'
    parameter: 'arg2'
    parameter: 'arg3'

    在 bash 的手册中对 $* 和 $@ 的说明如下：

    *    Expands to the positional parameters, starting from one.
         When the expansion occurs within double quotes, it
         expands to a single word with the value of each parameter
         separated by the first character of the IFS special variable.
         That is, "$*" is equivalent to "$1c$2c...",
    @    Expands to the positional parameters, starting from one.
         When the expansion occurs within double quotes, each
         parameter expands to a separate word.  That  is,  "$@"
         is equivalent to "$1" "$2" ...

    可见，不加引号时 $* 和 $@ 是相同的，但$* 会被扩展成一个字符串，而 $@ 会 被扩展成每一个参数。

    function foo()

    在bash中没有问题，但其他shell中有可能出错。不要把 function 和括号一起使用。 最为保险的做法是使用括号，即

    foo() {
      ...
    }




/reference/linux/BashPitfalls.mhtml
-->


## 参考

这是参考 [Bash Pitfalls](http://bash.cumulonim.biz/BashPitfalls.html) 的一篇文章，虽说是参考，大部分都是翻译，建议初学者好好看看这篇文章，避免一些常见的错误。


<!--
Bash编程易犯的错误
http://blog.jobbole.com/46191/
Bash 老司机也可能忽视的 10 大编程细节
https://www.leiphone.com/news/201703/i49ztcRDDymM7Id5.html
初识Bash编程
http://www.jianshu.com/p/d590aa13b124

ShellCheck
-->

{% highlight text %}
{% endhighlight %}
