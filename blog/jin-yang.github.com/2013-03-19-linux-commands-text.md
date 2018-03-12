---
title: Linux 常用命令 -- 文本处理
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,命令
description: 主要介绍下在 Linux 中，与文本处理相关的命令。
---

主要介绍下在 Linux 中，与文本处理相关的命令。

<!-- more -->

## diff & patch

该命令通常用于文本文件的区别查看。

{% highlight text %}
----- diff常用选项
    -r :  递归；
    -N :  确保能正确处理已创建或删除的文件；
    -u :  以统一格式创建，默认输出上下文前后 3 行，-u5 显示 5 行；
    -E :  忽略 tab 的改变；
    -b :  忽略 white space；
    -w :  忽略所有空白，当使用不同的对齐方式时非常方便，注意空白行仍然认为有差别；
    -B :  忽略空白行。

----- patch常用选项
    -R :  新旧版本互换；
    -E :  直接删除空文件。
{% endhighlight %}

假设文件 A 和文件 B ，其中 A 为原文件， B 为修改后的文件，经过 diff 之后生成了补丁文件 C ，那么着个过程相当于 A - B = C ，那么 patch 的过程就是 B + C = A 或 A - C = B。

也就是说，如果简单使用 diff A B &gt; C，那么只能用 patch A C，使用 patch B C 将出现错误，如果想恢复则应该使用 patch -RE A C。

#### 格式

其中简单示例如下：

{% highlight text %}
$ diff -uN A B
--- A   2013-02-17 11:20:08.926661164 +0800         # 原文件
+++ B   2013-02-17 11:20:38.666854034 +0800         # 修改后的文件
@@ -1 +1,2 @@                  # 原文件的第一行，以及改后的1~2行
 Hello World                   # 两者之差的内容即为修改的内容
 +foo bar                      # 其中减号('-')表示删除，加号('+')表示添加
{% endhighlight %}

#### 常用操作

{% highlight text %}
----- 单个文件
# diff -uN from-file to-file > to-file.patch     # 产生补丁
# patch -p0 < to-file.patch                      # 打补丁，针对目录下所有文件
# patch -p0 from-file to-file.patch              # 同上，但是指定文件
# patch -RE -p0 < to-file.patch                  # 恢复原文件

----- 多个文件
# diff -uNr from-docu to-docu > to-docu.patch
# patch -p1 < to-docu.patch
# patch -R -p1 < to-docu.patch
{% endhighlight %}

patch 使用时不用指定文件，在补丁文件中已经记载了原文件的路径和名称。

另外需要通过 -pn 来处理补丁中的路径问题，如 dir/dirA/A 、 dir/dirB/B ，在 dir 目录下执行 $ diff -rc dirA dirB > C，此时 C 中记录的路径为 dirA/A ；此时如果在 dirA/ 目录下，那么应该使用 patch -p1 ，即忽略 dirA/ 。

补丁失败的文件会以 .rej 结尾，下面命令可以找出所有 rej 文件， find . -name '*.rej' 。

{% highlight text %}
dir# diff -uNr from-dir to-dir > dir.patch
dir# patch -p0 < dir.patch                       # from-dir 将会被删除
dir/from-dir# patch -p1 < ../dir.patch           # 正确的处理方法
{% endhighlight %}



## cmp

二进制文件的比较。

cmp 命令会逐字节比较两个文件内容，如果两个文件内容完全，则 cmp 命令不显示任何内容。若两个文件内容有差异，会显示第一个不同之处的字节数和行数编号。

如果文件是 '-' 或没给出，则从标准输入读入内容。

{% highlight text %}
cmp [options] file1 file2
常用选项：
  -l, --verbose
    显示每一个不同点的字节号（10进制）和不同点的字节内容（8进制）；会显示所有不同字节。
  -b, --print-bytes
    以子符的形式显示不同的字节。
  -i NUM, --ignore-initial=NUM
    两个文件均越过开始的NUM个字节开始比较。
  -i NUM1:NUM2, --ignore-initial=NUM1:NUM2
    第一个文件越过开始的NUM1个字节，第二个文件越过开始的NUM2个字节，开始比较。
  -n NUM, --bytes=NUM
    设定比较的上限，最多比较 NUM 个字节。
{% endhighlight %}

文件相同只返回0；文件不同返回1；发生错误返回2。

{% highlight text %}
$ cmp file1 file2
file1 file2 differ: char 23, line 6
cmp: EOF on file1
{% endhighlight %}

第一行的结果表示 file1 与 file2 内容在第 6 行的第 23 个字符开始有差异。第二行的结果表示 file2 前半部分与 file1 相同，但在 file2 中还有其他数据。



<!--
awk -F : '$1=="root" {print $0}' /etc/passwd

/usr/sbin/useradd -M -N -g test -o -r -s /bin/false -c "Uagent Server" -u 66 test
/usr/sbin/groupadd -g 66 -o -r test
-->

## awk

{% highlight text %}
----- 通常使用的是单引号，如果是双引号，那么示例如下
$ ls -l $dir | awk "/$base\$/ { print \$1 \$3 \$4 }"
$ p=`ls -l $dir | awk "/$base\$/ { print \\\$1 \\\$3 \\\$4 }"`

----- 分割passwd文件
$ awk -F : '$1=="root" {print $0}' /etc/passwd
{% endhighlight %}

### 常用示例

#### 按列求和

可以使用 awk 命令计算文件中某一列的总和。

{% highlight text %}
----- 对第二列求和
$ awk 'BEGIN{sum=0}{sum+=$2}END{print sum}' data.txt

----- 对满足开始为/foobar/的行求和
$ awk 'BEGIN{sum=0};/foobar/{sum+=$2};END{print sum}' data.txt

----- 另外比较完整的一个例子
$ awk -F ',' 'BEGIN{sum=0;count=0}{if ($(NF-11)==2 && $NF==0 && $3=="11" && $6~/TIME|ESTABLISHED/) \
     {sum +=$5; count++;} } END {print "sum="sum" count="count " avg="sum/count}'
{% endhighlight %}

```$N``` 表示第 N 列，从 0 开始计算；```$0``` 表示所有列；```NF``` 表示 Number of Fields，也就是字段数， ```$NF``` 表示最后一个字段，```$(NF-1)``` 表示倒数第二个字段。



## sed

常用操作如下。

{% highlight text %}
----- 文件头部添加一行，命令1表示第一行，i表示插入，之后是内容
sed -i "1ifoobar" filename
  i 行前插入 insert
  a 行后插入 append
  c 行替换   change
----- 规则匹配行前插入
sed -i "/foobar/ifoo" filename

----- 删除指定行，$表示最后一行
$ sed -i '1d' filename
$ sed -i '$d' filename
$ sed -i '/foobar/d' filename
{% endhighlight %}

### 多行替换

sed 经常用来替换文件的内容，通常是处理单行的，但通过它的一些内建功能，也能实现多行替换。假设有如下的文本：

{% highlight text %}
  hello <<<comment part 1
  comment part 2>>>
  foobar
{% endhighlight %}

现在需要把 ```<<< ... >>>``` 这一段替换为 "COMMENT"，那么 sed 语法应当是：

{% highlight text %}
:begin
/<<</,/>>>/ {
    />>>/! {
        $! {
            N;
             b begin
        }
    }
    s/<<<.*>>>/COMMENT/;
}
{% endhighlight %}

上述语句存储在 test.sed 中，那么执行的方式和结果就是：

{% highlight text %}
$ sed -f foobar.sed foobar
  hello COMMENT
  foobar
{% endhighlight %}

把正则直接写到命令里面也可以，用 ```";"``` 来分隔命令即可，注意右花括号之后也要加上分号 ```";"```，如果再加上 -i 参数就可以直接把改动写到原文件中去了：

{% highlight text %}
$ sed -e ":begin; /<<</,/>>>/ { />>>/! { $! { N; b begin }; }; s/<<<.*>>>/COMMENT/; };" test
  hello COMMENT
  foobar
{% endhighlight %}

各个步骤介绍如下：

1. 花括号 ```{}``` 代表命令块的开始，类似 C 语法；
1. ```:begin``` 是一个标号 (label)，用于跳转，供 b、t、T 等命令使用，这里使用了 b 命令；
1. ```/<<</,/>>>/``` 通过逗号分隔 (开始+结束位置) 用于标示地址范围，后面 {} 中的命令只对地址范围之间的内容使用。
1. ```/>>>/!``` ```$!``` 其中叹号表示取反，而 $ 在 sed 中表示为最后一行，也就意味着 "如果在本行没有发现结束标记，而且本行不是文件的最后一行" 那么执行下面的操作；
1. ```N;``` 用于把下一行的内容追加到缓冲区 (pattern)，也就是相当于合并为一行；
1. ```b begin``` 由于仍然没有找到结束标记跳回到 begin，重新执行追加命令；
1. ```s/<<<.*>>>/COMMENT/;``` 匹配完成之后，可以通过该命令替换。

<!--
http://man.linuxde.net/sed

Update @ 2007-12-14

在和bxy讨论的过程中，又发现sed的另外一种用途，从html或xml中按照tag对应关系，筛选打印出指定的tag内容，使用了正则中的p命令，好像默认就没有“不能处理多行内容”以及“贪婪性”的问题，很好用，很强大：

    $ sed -n -e '/<title>/p' -e '/<text /,/<\/text>/p' from.xml

注意//不在同一行的时候才好用，不然会匹配到下一个实例出现的位置作为结束边界。

sed -e ":begin; /\/\*/,/>>>/ { />>>/! { $! { N; b begin }; }; s/<<<.*>>>/COMMENT/; };"

----- 文件头以/** **/标示，可以通过如下方式打印或者删除
sed -n -e '/\/\*\*/,/^ \*\*\//p' plugin.c
sed -i -e ':begin; /\/\*\*/,/^ \*\*\//d' plugin.c

----- 只显示匹配行
sed -n '/This/p' plugin.c


## 多行合并

将某个目录下的文件合并成一行，中间用冒号 ```:``` 作为分隔符：

ls /tmp | paste --serial --delimiters=":"
ls /tmp | tr "\n" ":"

http://www.361way.com/awk-sed-convert-oneline/5127.html

另外，在 sed 中包含了 ```@``` 符号，类似于 ```/``` 用于分割正则表达式，尤其是在正则表达式中有 ```/``` 时比较实用。

默认只替换第一个就结束，通过 ```g``` 表示会检查所有的行。

----- 行首/行尾添加字符串，
sed 's/^/HEAD&/g' test.file
sed 's/$/&TAIL/g' test.file
sed '/./{s/^/HEAD&/;s/$/&TAIL/}' test.file
-->


## 常用技巧

如果其中的部分参数需要动态获取，而 ```''``` 则会原样输出字符内容，那么可以通过类似如下的方式使用。

{% highlight text %}
$ echo "'$(hostname)'" | xargs sed filename -e
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
