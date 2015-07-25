# 學會使用命令幫助


1.1. 概述
在linux終端，面對命令不知道怎麼用，或不記得命令的拼寫及參數時，我們需要求助於系統的幫助文檔； linux系統內置的幫助文檔很詳細，通常能解決我們的問題，我們需要掌握如何正確的去使用它們；

在只記得部分命令關鍵字的場合，我們可通過man -k來搜索；
需要知道某個命令的簡要說明，可以使用whatis；而更詳細的介紹，則可用info命令；
查看命令在哪個位置，我們需要使用which；
而對於命令的具體參數及使用方法，我們需要用到強大的man；
下面介紹這些命令；

1.2. 命令使用
查看命令的簡要說明
簡要說明命令的作用（顯示命令所處的man分類頁面）:
```
$whatis command
```
正則匹配:
```
$whatis -w "loca*"
```
更加詳細的說明文檔:
```
$info command
```
使用man
查詢命令command的說明文檔:
```
$man command
eg：man date
```

使用page up和page down來上下翻頁

在man的幫助手冊中，將幫助文檔分為了9個類別，對於有的關鍵字可能存在多個類別中， 我們就需要指定特定的類別來查看；（一般我們查詢bash命令，歸類在1類中）；

man頁面所屬的分類標識(常用的是分類1和分類3)
```
(1)、用戶可以操作的命令或者是可執行文件
(2)、系統核心可調用的函數與工具等
(3)、一些常用的函數與數據庫
(4)、設備文件的說明
(5)、設置文件或者某些文件的格式
(6)、遊戲
(7)、慣例與協議等。例如Linux標準文件系統、網絡協議、ASCⅡ，碼等說明內容
(8)、系統管理員可用的管理條令
(9)、與內核有關的文件
```
前面說到使用whatis會顯示命令所在的具體的文檔類別，我們學習如何使用它
```
eg:
$whatis printf
printf               (1)  - format and print data
printf               (1p)  - write formatted output
printf               (3)  - formatted output conversion
printf               (3p)  - print formatted output
printf [builtins]    (1)  - bash built-in commands, see bash(1)
```
我們看到printf在分類1和分類3中都有；分類1中的頁面是命令操作及可執行文件的幫助；而3是常用函數庫說明；如果我們想看的是C語言中printf的用法，可以指定查看分類3的幫助：
```
$man 3 printf

$man -k keyword
```
查詢關鍵字 根據命令中部分關鍵字來查詢命令，適用於只記住部分命令的場合；

eg：查找GNOME的config配置工具命令:
```
$man -k GNOME config| grep 1
```
對於某個單詞搜索，可直接使用/word來使用: /-a; 多關注下SEE ALSO 可看到更多精彩內容

查看路徑
查看程序的binary文件所在路徑:
```
$which command
```
eg:查找make程序安裝路徑:
```
$which make
/opt/app/openav/soft/bin/make install
```
查看程序的搜索路徑:
```
$whereis command
```
當系統中安裝了同一軟件的多個版本時，不確定使用的是哪個版本時，這個命令就能派上用場；

總結
whatis info man which whereis
