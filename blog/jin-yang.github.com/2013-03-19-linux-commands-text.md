---
title: Linux 常用命令 -- 文本處理
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,命令
description: 主要介紹下在 Linux 中，與文本處理相關的命令。
---

主要介紹下在 Linux 中，與文本處理相關的命令。

<!-- more -->

## diff & patch

該命令通常用於文本文件的區別查看。

{% highlight text %}
----- diff常用選項
    -r :  遞歸；
    -N :  確保能正確處理已創建或刪除的文件；
    -u :  以統一格式創建，默認輸出上下文前後 3 行，-u5 顯示 5 行；
    -E :  忽略 tab 的改變；
    -b :  忽略 white space；
    -w :  忽略所有空白，當使用不同的對齊方式時非常方便，注意空白行仍然認為有差別；
    -B :  忽略空白行。

----- patch常用選項
    -R :  新舊版本互換；
    -E :  直接刪除空文件。
{% endhighlight %}

假設文件 A 和文件 B ，其中 A 為原文件， B 為修改後的文件，經過 diff 之後生成了補丁文件 C ，那麼著個過程相當於 A - B = C ，那麼 patch 的過程就是 B + C = A 或 A - C = B。

也就是說，如果簡單使用 diff A B &gt; C，那麼只能用 patch A C，使用 patch B C 將出現錯誤，如果想恢復則應該使用 patch -RE A C。

#### 格式

其中簡單示例如下：

{% highlight text %}
$ diff -uN A B
--- A   2013-02-17 11:20:08.926661164 +0800         # 原文件
+++ B   2013-02-17 11:20:38.666854034 +0800         # 修改後的文件
@@ -1 +1,2 @@                  # 原文件的第一行，以及改後的1~2行
 Hello World                   # 兩者之差的內容即為修改的內容
 +foo bar                      # 其中減號('-')表示刪除，加號('+')表示添加
{% endhighlight %}

#### 常用操作

{% highlight text %}
----- 單個文件
# diff -uN from-file to-file > to-file.patch     # 產生補丁
# patch -p0 < to-file.patch                      # 打補丁，針對目錄下所有文件
# patch -p0 from-file to-file.patch              # 同上，但是指定文件
# patch -RE -p0 < to-file.patch                  # 恢復原文件

----- 多個文件
# diff -uNr from-docu to-docu > to-docu.patch
# patch -p1 < to-docu.patch
# patch -R -p1 < to-docu.patch
{% endhighlight %}

patch 使用時不用指定文件，在補丁文件中已經記載了原文件的路徑和名稱。

另外需要通過 -pn 來處理補丁中的路徑問題，如 dir/dirA/A 、 dir/dirB/B ，在 dir 目錄下執行 $ diff -rc dirA dirB > C，此時 C 中記錄的路徑為 dirA/A ；此時如果在 dirA/ 目錄下，那麼應該使用 patch -p1 ，即忽略 dirA/ 。

補丁失敗的文件會以 .rej 結尾，下面命令可以找出所有 rej 文件， find . -name '*.rej' 。

{% highlight text %}
dir# diff -uNr from-dir to-dir > dir.patch
dir# patch -p0 < dir.patch                       # from-dir 將會被刪除
dir/from-dir# patch -p1 < ../dir.patch           # 正確的處理方法
{% endhighlight %}



## cmp

二進制文件的比較。

cmp 命令會逐字節比較兩個文件內容，如果兩個文件內容完全，則 cmp 命令不顯示任何內容。若兩個文件內容有差異，會顯示第一個不同之處的字節數和行數編號。

如果文件是 '-' 或沒給出，則從標準輸入讀入內容。

{% highlight text %}
cmp [options] file1 file2
常用選項：
  -l, --verbose
    顯示每一個不同點的字節號（10進制）和不同點的字節內容（8進制）；會顯示所有不同字節。
  -b, --print-bytes
    以子符的形式顯示不同的字節。
  -i NUM, --ignore-initial=NUM
    兩個文件均越過開始的NUM個字節開始比較。
  -i NUM1:NUM2, --ignore-initial=NUM1:NUM2
    第一個文件越過開始的NUM1個字節，第二個文件越過開始的NUM2個字節，開始比較。
  -n NUM, --bytes=NUM
    設定比較的上限，最多比較 NUM 個字節。
{% endhighlight %}

文件相同只返回0；文件不同返回1；發生錯誤返回2。

{% highlight text %}
$ cmp file1 file2
file1 file2 differ: char 23, line 6
cmp: EOF on file1
{% endhighlight %}

第一行的結果表示 file1 與 file2 內容在第 6 行的第 23 個字符開始有差異。第二行的結果表示 file2 前半部分與 file1 相同，但在 file2 中還有其他數據。



<!--
awk -F : '$1=="root" {print $0}' /etc/passwd

/usr/sbin/useradd -M -N -g test -o -r -s /bin/false -c "Uagent Server" -u 66 test
/usr/sbin/groupadd -g 66 -o -r test
-->

## awk

{% highlight text %}
----- 通常使用的是單引號，如果是雙引號，那麼示例如下
$ ls -l $dir | awk "/$base\$/ { print \$1 \$3 \$4 }"
$ p=`ls -l $dir | awk "/$base\$/ { print \\\$1 \\\$3 \\\$4 }"`

----- 分割passwd文件
$ awk -F : '$1=="root" {print $0}' /etc/passwd
{% endhighlight %}

### 常用示例

#### 按列求和

可以使用 awk 命令計算文件中某一列的總和。

{% highlight text %}
----- 對第二列求和
$ awk 'BEGIN{sum=0}{sum+=$2}END{print sum}' data.txt

----- 對滿足開始為/foobar/的行求和
$ awk 'BEGIN{sum=0};/foobar/{sum+=$2};END{print sum}' data.txt

----- 另外比較完整的一個例子
$ awk -F ',' 'BEGIN{sum=0;count=0}{if ($(NF-11)==2 && $NF==0 && $3=="11" && $6~/TIME|ESTABLISHED/) \
     {sum +=$5; count++;} } END {print "sum="sum" count="count " avg="sum/count}'
{% endhighlight %}

```$N``` 表示第 N 列，從 0 開始計算；```$0``` 表示所有列；```NF``` 表示 Number of Fields，也就是字段數， ```$NF``` 表示最後一個字段，```$(NF-1)``` 表示倒數第二個字段。



## sed

常用操作如下。

{% highlight text %}
----- 文件頭部添加一行，命令1表示第一行，i表示插入，之後是內容
sed -i "1ifoobar" filename
  i 行前插入 insert
  a 行後插入 append
  c 行替換   change
----- 規則匹配行前插入
sed -i "/foobar/ifoo" filename

----- 刪除指定行，$表示最後一行
$ sed -i '1d' filename
$ sed -i '$d' filename
$ sed -i '/foobar/d' filename
{% endhighlight %}

### 多行替換

sed 經常用來替換文件的內容，通常是處理單行的，但通過它的一些內建功能，也能實現多行替換。假設有如下的文本：

{% highlight text %}
  hello <<<comment part 1
  comment part 2>>>
  foobar
{% endhighlight %}

現在需要把 ```<<< ... >>>``` 這一段替換為 "COMMENT"，那麼 sed 語法應當是：

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

上述語句存儲在 test.sed 中，那麼執行的方式和結果就是：

{% highlight text %}
$ sed -f foobar.sed foobar
  hello COMMENT
  foobar
{% endhighlight %}

把正則直接寫到命令裡面也可以，用 ```";"``` 來分隔命令即可，注意右花括號之後也要加上分號 ```";"```，如果再加上 -i 參數就可以直接把改動寫到原文件中去了：

{% highlight text %}
$ sed -e ":begin; /<<</,/>>>/ { />>>/! { $! { N; b begin }; }; s/<<<.*>>>/COMMENT/; };" test
  hello COMMENT
  foobar
{% endhighlight %}

各個步驟介紹如下：

1. 花括號 ```{}``` 代表命令塊的開始，類似 C 語法；
1. ```:begin``` 是一個標號 (label)，用於跳轉，供 b、t、T 等命令使用，這裡使用了 b 命令；
1. ```/<<</,/>>>/``` 通過逗號分隔 (開始+結束位置) 用於標示地址範圍，後面 {} 中的命令只對地址範圍之間的內容使用。
1. ```/>>>/!``` ```$!``` 其中歎號表示取反，而 $ 在 sed 中表示為最後一行，也就意味著 "如果在本行沒有發現結束標記，而且本行不是文件的最後一行" 那麼執行下面的操作；
1. ```N;``` 用於把下一行的內容追加到緩衝區 (pattern)，也就是相當於合併為一行；
1. ```b begin``` 由於仍然沒有找到結束標記跳回到 begin，重新執行追加命令；
1. ```s/<<<.*>>>/COMMENT/;``` 匹配完成之後，可以通過該命令替換。

<!--
http://man.linuxde.net/sed

Update @ 2007-12-14

在和bxy討論的過程中，又發現sed的另外一種用途，從html或xml中按照tag對應關係，篩選打印出指定的tag內容，使用了正則中的p命令，好像默認就沒有“不能處理多行內容”以及“貪婪性”的問題，很好用，很強大：

    $ sed -n -e '/<title>/p' -e '/<text /,/<\/text>/p' from.xml

注意//不在同一行的時候才好用，不然會匹配到下一個實例出現的位置作為結束邊界。

sed -e ":begin; /\/\*/,/>>>/ { />>>/! { $! { N; b begin }; }; s/<<<.*>>>/COMMENT/; };"

----- 文件頭以/** **/標示，可以通過如下方式打印或者刪除
sed -n -e '/\/\*\*/,/^ \*\*\//p' plugin.c
sed -i -e ':begin; /\/\*\*/,/^ \*\*\//d' plugin.c

----- 只顯示匹配行
sed -n '/This/p' plugin.c


## 多行合併

將某個目錄下的文件合併成一行，中間用冒號 ```:``` 作為分隔符：

ls /tmp | paste --serial --delimiters=":"
ls /tmp | tr "\n" ":"

http://www.361way.com/awk-sed-convert-oneline/5127.html

另外，在 sed 中包含了 ```@``` 符號，類似於 ```/``` 用於分割正則表達式，尤其是在正則表達式中有 ```/``` 時比較實用。

默認只替換第一個就結束，通過 ```g``` 表示會檢查所有的行。

----- 行首/行尾添加字符串，
sed 's/^/HEAD&/g' test.file
sed 's/$/&TAIL/g' test.file
sed '/./{s/^/HEAD&/;s/$/&TAIL/}' test.file
-->


## 常用技巧

如果其中的部分參數需要動態獲取，而 ```''``` 則會原樣輸出字符內容，那麼可以通過類似如下的方式使用。

{% highlight text %}
$ echo "'$(hostname)'" | xargs sed filename -e
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
