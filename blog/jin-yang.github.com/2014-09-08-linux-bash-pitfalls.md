---
title: Bash 安全編程
layout: post
comments: true
language: chinese
category: [linux, program]
keywords: bash,編程,安全
description:
---

簡單介紹下 Linux 中 Bash 編程所需要注意的內容。

<!-- more -->

## 防止未初始化

對於如下場景，通常會導致預想不到的錯誤。

{% highlight bash %}
chroot=$may_not_exist
rm -rf $chroot/usr
{% endhighlight %}

那麼上面的腳本可能會導致 `/usr` 目錄被刪除。

可以在腳本中使用 `set -u` 或者 `set -o nounset`，或者在命令行中使用 `bash -u your.sh` 。

## 執行錯誤退出

通常對於一些命令的返回值，可以通過如下的方式檢查返回值，如果有異常則退出。

{% highlight bash %}
#----- 示例1
command
if [ "$?"-ne 0]; then echo "command failed"; exit 1; fi
#----- 示例2
command || { echo "command failed"; exit 1; }
#----- 示例3
if ! command; then echo "command failed"; exit 1; fi
{% endhighlight %}

可以通過 `set -e` 或者 `set -o errexit` 明確告知 Bash ，一但有任何一個語句返回非真的值則退出，避免錯誤滾雪球般的變成嚴重錯誤。

如果必須使用返回非 0 值的命令，或者對返回值不敏感，那麼可以使用 `command || true`；如果有一段很長的代碼，可以暫時關閉錯誤檢查功能，當然需要謹慎使用。

{% highlight bash %}
set +e
# some commands
set -e
{% endhighlight %}

另外，需要注意管道的使用，Bash 默認返回管道中最後一個命令的值，例如 `false | true` 將會被認為命令執行成功，如果想讓這樣的命令被認為是執行失敗，可以使用 `set -o pipefail` 命令進行設置。

## 設置陷阱

當程序異常退出時，通常需要處理一些處於中間狀態的變量，例如鎖文件、臨時文件等等。

Bash 提供了一種方法，當收到一個 UNIX 信號時，可以運行一個命令或者一個函數；此時就需要使用 trap 命令。

{% highlight bash %}
trap command signal [signal ...]
{% endhighlight %}

所有的信號量可以通過 `kill -l` 查看，通常使用較多的是三個：INT、TERM 和 EXIT。

{% highlight text %}
INT   使用Ctrl-C終止腳本時被觸發
TERM  使用kill殺死腳本進程時被觸發
EXIT  偽信號，當腳本正常退出或者set -e後因為出錯而退出時被觸發
{% endhighlight %}

### 鎖文件

當你使用鎖文件時，可以這樣寫：

{% highlight bash %}
if [ ! -e $lockfile ]; then
    touch $lockfile
    critical-section
    rm $lockfile
else
    echo "critical-section is already running"
fi
{% endhighlight %}

如果 `critical-section` 在運行時被殺死了，那麼鎖文件仍然存在，但是在刪除之前該腳本就無法運行了；此時就需要通過如下的方式進行設置。

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

這樣，即使在 `critical-section` 運行時被殺死，鎖文件會一同被刪除。


<!--
### 竟態條件

另外，在上面鎖文件示例中，會存在一個竟態條件，也就是在判斷鎖文件和創建鎖文件之間。

其中一個可行的解決方法是使用IO重定向和bash的noclobber(wikipedia)模式，重定向到不存在的文件。我們可以這麼做：
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
更復雜一點兒的問題是你要更新一大堆文件，當它們更新過程中出現問題時，你是否能讓腳本掛得更加優雅一些。你想確認那些正確更新了，哪些根本沒有變化。比如你需要一個添加用戶的腳本。
add_to_passwd $user
cp -a /etc/skel /home/$user
chown $user /home/$user -R
當磁盤空間不足或者進程中途被殺死，這個腳本就會出現問題。在這種情況下，你也許希望用戶賬戶不存在，而且他的文件也應該被刪除。
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
在腳本最後需要使用trap關閉rollback調用，否則當腳本正常退出的時候rollback將會被調用，那麼腳本等於什麼都沒做。
保持原子化

又是你需要一次更新目錄中的一大堆文件，比如你需要將URL重寫到另一個網站的域名。你也許會寫：
for file in $(find /var/www -type f -name "*.html"); do
perl -pi -e 's/www.example.net/www.example.com/' $file
done
如果修改到一半是腳本出現問題，一部分使用www.example.com，而另一部分使用www.example.net。你可以使用備份和trap解決，但在升級過程中你的網站URL是不一致的。
解決方法是將這個改變做成一個原子操作。先對數據做一個副本，在副本中更新URL，再用副本替換掉現在工作的版本。你需要確認副本和工作版本目錄在同一個磁盤分區上，這樣你就可以利用Linux系統的優勢，它移動目錄僅僅是更新目錄指向的inode節點。
cp -a /var/www /var/www-tmp
for file in $(find /var/www-tmp -type -f -name "*.html"); do
perl -pi -e 's/www.example.net/www.example.com/' $file
done
mv /var/www /var/www-old
mv /var/www-tmp /var/www
-->

## 其它

通常不同的命令在執行時，不同的參數可能會有不同的行為，例如 `mkdir -p` 可以防止父目錄不存在時報錯；`rm -f` 可以防止文件不存在時報錯等等。

### 空格處理

一定要注意處理好文件中可能出現空格的場景，也就是要用引號包圍變量，示例如下：

{% highlight bash %}
#----- 變量中有空格時會導致異常
if [ $filename = "foo" ];
#----- 正常應該使用如下的方式
if [ "$filename" = "foo" ];
{% endhighlight %}

另外，使用命令行參數時同樣需要注意，例如 `$@` 變量，因為空格隔開的兩個參數會被解釋成兩個獨立的部分。

{% highlight bash %}
$ foo() { for i in $@; do echo $i; done }; foo bar "hello world"
bar
hello
world
$ foo() { for i in "$@"; do echo $i; done }; foo bar "hello world"
bar
hello world
{% endhighlight %}

還有，在同時使用 `find` 和 `xargs` 命令時，應該使用 `-print0` 來讓字符分割文件名，而不是通過換行符分割。

{% highlight bash %}
$ touch "foo bar"
$ find | xargs ls
ls: ./foo: No such file or directory
ls: bar: No such file or directory
$ find -print0 | xargs -0 ls
./foo bar
{% endhighlight %}



<!--


下面就逐個分析一下這篇文章中提到的錯誤。不是完全的翻譯，有些沒用的話就略過了， 有些地方則加了些註釋。

    for i in `ls *.mp3`

    常見的錯誤寫法：

     for i in `ls *.mp3`; do     # Wrong!

    為什麼錯誤呢？因為for…in語句是按照空白來分詞的，包含空格的文件名會被拆成多個詞。 如遇到 01 - Don’t Eat the Yellow Snow.mp3 時，i的值會依次取 01，-，Don’t，等等。

    用雙引號也不行，它會將ls *.mp3的全部結果當成一個詞來處理。

     for i in "`ls *.mp3`"; do   # Wrong!

    正確的寫法是

     for i in *.mp3; do

    cp $file $target

    這句話基本上正確，但同樣有空格分詞的問題。所以應當用雙引號：

     cp "$file" "$target"

    但是如果湊巧文件名以 - 開頭，這個文件名會被 cp 當作命令行選項來處理，依舊很頭疼。可以試試下面這個。

     cp -- "$file" "$target"

    運氣差點的再碰上一個不支持 – 選項的系統，那隻能用下面的方法了：使每個變量都以目錄開頭。

     for i in ./*.mp3; do
       cp "$i" /target
       ...

    [ $foo = "bar" ]

    當$foo為空時，上面的命令就變成了

     [ = "bar" ]

    類似地，當$foo包含空格時：

     [ multiple words here = "bar" ]

    兩者都會出錯。所以應當用雙引號將變量括起來：

     [ "$foo" = bar ]      # 幾乎完美了。

    但是！當$foo以 - 開頭時依然會有問題。 在較新的bash中你可以用下面的方法來代替，[[ 關鍵字能正確處理空白、空格、帶橫線等問題。

     [[ $foo = bar ]]      # 正確

    舊版本bash中可以用這個技巧（雖然不好理解）：

     [ x"$foo" = xbar ]    # 正確

    或者乾脆把變量放在右邊，因為 [ 命令的等號右邊即使是空白或是橫線開頭，依然能正常工作。 （Java編程風格中也有類似的做法，雖然目的不一樣。）

     [ bar = "$foo" ]      # 正確

    cd `dirname "$f"`

    同樣也存在空格問題。那麼加上引號吧。

     cd "`dirname "$f"`"

    問題來了，是不是寫錯了？由於雙引號的嵌套，你會認為`dirname 是第一個字符串，`是第二個字符串。 錯了，那是C語言。在bash中，命令替換（反引號``中的內容）裡面的雙引號會被正確地匹配到一起， 不用特意去轉義。

    $()語法也相同，如下面的寫法是正確的。

     cd "$(dirname "$f")"

    [ "$foo" = bar && "$bar" = foo ]

    [ 中不能使用 && 符號！因為 [ 的實質是 test 命令，&& 會把這一行分成兩個命令的。應該用以下的寫法。

     [ bar = "$foo" -a foo = "$bar" ]       # Right!
     [ bar = "$foo" ] && [ foo = "$bar" ]   # Also right!
     [[ $foo = bar && $bar = foo ]]         # Also right!

    [ $foo > 7 ]

    很可惜 [[ 只適用於字符串，不能做數字比較。數字比較應當這樣寫：

     (( $foo > 7 ))

    或者用經典的寫法：

     [ $foo -gt 7 ]

    但上述使用 -gt 的寫法有個問題，那就是當 $foo 不是數字時就會出錯。你必須做好類型檢驗。

    這樣寫也行。

     [[ $foo -gt 7 ]]

    grep foo bar | while read line; do ((count++)); done

    這行代碼數出bar文件中包含foo的行數，雖然很麻煩（等同於grep -c foo bar或者 grep foo bar | wc -l）。 乍一看沒有問題，但執行之後count變量卻沒有值。因為管道中的每個命令都放到一個新的子shell中執行， 所以子shell中定義的count變量無法傳遞出來。

    if [grep foo myfile]

    初學者常犯的錯誤，就是將 if 語句後面的 [ 當作if語法的一部分。實際上它是一個命令，相當於 test 命令， 而不是 if 語法。這一點C程序員特別應當注意。

    if 會將 if 到 then 之間的所有命令的返回值當作判斷條件。因此上面的語句應當寫成

     if grep foo myfile > /dev/null; then

    if [bar="$foo"]

    同樣，[ 是個命令，不是 if 語句的一部分，所以要注意空格。

     if [ bar = "$foo" ]

    if [ [ a = b ] && [ c = d ] ]

    同樣的問題，[ 不是 if 語句的一部分，當然也不是改變邏輯判斷的括號。它是一個命令。可能C程序員比較容易犯這個錯誤？

    if [ a = b ] && [ c = d ]        # 正確

    cat file | sed s/foo/bar/ > file

    你不能在同一條管道操作中同時讀寫一個文件。根據管道的實現方式，file要麼被截斷成0字節，要麼會無限增長直到填滿整個硬盤。 如果想改變原文件的內容，只能先將輸出寫到臨時文件中再用mv命令。

    sed 's/foo/bar/g' file > tmpfile && mv tmpfile file

    echo $foo

    這句話還有什麼錯誤碼？一般來說是正確的，但下面的例子就有問題了。

    MSG="Please enter a file name of the form *.zip"
    echo $MSG         # 錯誤！

    如果恰巧當前目錄下有zip文件，就會顯示成

    Please enter a file name of the form freenfss.zip lw35nfss.zip

    所以即使是echo也別忘記給變量加引號。

    $foo=bar

    變量賦值時無需加 $ 符號——這不是Perl或PHP。

    foo = bar

    變量賦值時等號兩側不能加空格——這不是C語言。

    echo <<EOF

    here document是個好東西，它可以輸出成段的文字而不用加引號也不用考慮換行符的處理問題。 不過here document輸出時應當使用cat而不是echo。

    # This is wrong:
    echo <<EOF
    Hello world
    EOF

    # This is right:
    cat <<EOF
    Hello world
    EOF

    su -c 'some command'

    原文的意思是，這條基本上正確，但使用者的目的是要將 -c ‘some command’ 傳給shell。 而恰好 su 有個 -c 參數，所以su 只會將 ‘some command’ 傳給shell。所以應該這麼寫：

    su root -c 'some command'

    但是在我的平臺上，man su 的結果中關於 -c 的解釋為

    -c, --commmand=COMMAND
                pass a single COMMAND to the shell with -c

    也就是說，-c ‘some command’ 同樣會將 -c ‘some command’ 這樣一個字符串傳遞給shell， 和這條就不符合了。不管怎樣，先將這一條寫在這裡吧。

    cd /foo; bar

    cd有可能會出錯，出錯後 bar 命令就會在你預想不到的目錄裡執行了。所以一定要記得判斷cd的返回值。

    cd /foo && bar

    如果你要根據cd的返回值執行多條命令，可以用      。

    cd /foo || exit 1;
    bar
    baz

    關於目錄的一點題外話，假設你要在shell程序中頻繁變換工作目錄，如下面的代碼：

    find ... -type d | while read subdir; do
      cd "$subdir" && whatever && ... && cd -
    done

    不如這樣寫：

    find ... -type d | while read subdir; do
      (cd "$subdir" && whatever && ...)
    done

    括號會強制啟動一個子shell，這樣在這個子shell中改變工作目錄不會影響父shell（執行這個腳本的shell）， 就可以省掉cd - 的麻煩。

    你也可以靈活運用 pushd、popd、dirs 等命令來控制工作目錄。

    [ bar == "$foo" ]

    [ 命令中不能用 ==，應當寫成

    [ bar = "$foo" ] && echo yes
    [[ bar == $foo ]] && echo yes

    for i in {1..10}; do ./something &; done

    & 後面不應該再放 ; ，因為 & 已經起到了語句分隔符的作用，無需再用;。

    for i in {1..10}; do ./something & done

    cmd1 && cmd2 || cmd3

    有人喜歡用這種格式來代替 if…then…else 結構，但其實並不完全一樣。如果cmd2返回一個非真值，那麼cmd3則會被執行。 所以還是老老實實地用 if cmd1; then cmd2; else cmd3 為好。

    UTF-8的BOM(Byte-Order Marks)問題

    UTF-8編碼可以在文件開頭用幾個字節來表示編碼的字節順序，這幾個字節稱為BOM。但Unix格式的UTF-8編碼不需要BOM。 多餘的BOM會影響shell解析，特別是開頭的 #!/bin/sh 之類的指令將會無法識別。

    MS-DOS格式的換行符(CRLF)也存在同樣的問題。如果你將shell程序保存成DOS格式，腳本就無法執行了。

    $ ./dos
    -bash: ./dos: /bin/sh^M: bad interpreter: No such file or directory

    echo "Hello World!"

    交互執行這條命令會產生以下的錯誤：

    -bash: !": event not found

    因為 !” 會被當作命令行歷史替換的符號來處理。不過在shell腳本中沒有這樣的問題。

    不幸的是，你無法使用轉義符來轉義!：

    $ echo "hi\!"
    hi\!

    解決方案之一，使用單引號，即

    $ echo 'Hello, world!'

    如果你必須使用雙引號，可以試試通過 set +H 來取消命令行歷史替換。

    set +H
    echo "Hello, world!"

    for arg in $*

    $*表示所有命令行參數，所以你可能想這樣寫來逐個處理參數，但參數中包含空格時就會失敗。如：

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

    正確的方法是使用 $@。

    #!/bin/bash
    # Correct version
    for x in "$@"; do
      echo "parameter: '$x'"
    done

    $ ./myscript 'arg 1' arg2 arg3
    parameter: 'arg 1'
    parameter: 'arg2'
    parameter: 'arg3'

    在 bash 的手冊中對 $* 和 $@ 的說明如下：

    *    Expands to the positional parameters, starting from one.
         When the expansion occurs within double quotes, it
         expands to a single word with the value of each parameter
         separated by the first character of the IFS special variable.
         That is, "$*" is equivalent to "$1c$2c...",
    @    Expands to the positional parameters, starting from one.
         When the expansion occurs within double quotes, each
         parameter expands to a separate word.  That  is,  "$@"
         is equivalent to "$1" "$2" ...

    可見，不加引號時 $* 和 $@ 是相同的，但$* 會被擴展成一個字符串，而 $@ 會 被擴展成每一個參數。

    function foo()

    在bash中沒有問題，但其他shell中有可能出錯。不要把 function 和括號一起使用。 最為保險的做法是使用括號，即

    foo() {
      ...
    }




/reference/linux/BashPitfalls.mhtml
-->


## 參考

這是參考 [Bash Pitfalls](http://bash.cumulonim.biz/BashPitfalls.html) 的一篇文章，雖說是參考，大部分都是翻譯，建議初學者好好看看這篇文章，避免一些常見的錯誤。


<!--
Bash編程易犯的錯誤
http://blog.jobbole.com/46191/
Bash 老司機也可能忽視的 10 大編程細節
https://www.leiphone.com/news/201703/i49ztcRDDymM7Id5.html
初識Bash編程
http://www.jianshu.com/p/d590aa13b124

ShellCheck
-->

{% highlight text %}
{% endhighlight %}
