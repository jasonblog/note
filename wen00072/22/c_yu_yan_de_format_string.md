# C 語言的format string


format string也是看過[C Programming: A Modern Approach](http://knking.com/books/c2/index.html)才注意的。寫文的當下沒帶這本書，問男人好了。

```sh
$ man 3 printf
```

簡單來說，format string是由

* 普通文字，不包含單獨的`%`字元
* conversion specifications，就是你看到的`%s`之類的的東西
* 每個conversion specifier有對應的參數

整理conversion specifier如下

* 結構
  * 以`%`開頭
  * 以conversion specifier 結尾
  * 中間有些~~特異功能~~optional的描述如
    * [Flags](#fs_fl)
    * [最少欄位寬度規範](#fs_wd)
    * [精確度描述](#fs_prec)
    * [變數size輔助描述](#fs_sz)
  * `Conversion specifier`結尾，就是我們看過的`d`, `f` 等
    
示意如下

* `%`[<font color="red">Flag</font>][<font color="green">最少欄位寬度規範</font>][<font color="red">精確度描述</font>][<font color="green">變數size描述</font>]`Conversion specifier`


接下來依上面的部份說明如下

<a name="fs_fl"></a>
## Flags

* `#`
  * 自動對指定的數字進位數（八進位、十進位、十六進位等）加上合適的prefix如`0x`, `0`等。
* `0`
  * 填入`0`作為pad。pad是啥呢？中文意思是填充物，自己體會吧。
* `-`
  * 預設format string是靠右對齊，用了這個會變成靠左對齊。和樓上`0`一起會不會數字多好幾倍如`432`變`4320000`？不會，兩個同時出現`0`會被省略。
* `+`
  * 指定數字最前面要加正負號
* `' '`
  * 如果有設定數字要先顯示正負號，會在前面多加一個空白

以上是C 語言規範的，不同編譯器有加碼，懶得寫，自己問男人吧。


## 最少欄位寬度規範
一定要正整數，當印出的數字長度少於這邊指定的參數，會自動填空白或是0。印出超過這邊指定的寬度的話呢？超過就超過，不然要怎麼辦？

另外預設是往右邊對齊，想要往左對齊，請參考`-` flag。


## 精確度描述
以`.十進位數字表示`，不同的變數型態有不同的精確度定義。

舉例來說

* 整數，**最少**要出現的數字長度，少於這樣的數字，會直接在左邊或是基底符號（0, 0x)後面填零補完。
* 實數，也就是有小數點的，就是小數點後面**最多**可以出現的數字長度，超過了就截掉。


## 變數size輔助描述
一般來說，我們會知道`d`是整數，但是在C語言還是有`long int`，`long long int`這樣型態的整數，為了能夠更精確的顯示，format string提供了這樣的描述，讓你加在`Conversion specifier`，如


* `l`: long
* `h`: short

詳細列表和排列組合請問男人。

## 其他
flag除了C規格定的以外，不同廠商有加料，一樣去問男人，不過要用這個的話要考慮porting的問題。

除此之外，男人有列出所有的`Conversion specifier`，除了熟悉的`d`, `f`, `s`, `c`, `x`以外，我列幾個我感興趣的如下

* `a`, `A`: C99專用，實數的十六進位表示法
* `p`: void * 的位址


## 怪招
format string還有`*`和`%m$`這種鬼東西，目前搞不清楚為什麼要這樣幹。
不過幾然花了時間搞懂，就整理一下

* `*`指目前對應的參數的**下一個**參數
* `%m$*n$`把第n個的參數和第m個顯示**交換**

不知道是什麼鬼對不對？我也是，所以寫了程式測試一下

- test_format_string.c

```c 
#include <stdio.h>

int main(int argc, char **argv)
{
    int i = 2;
    int j = 3;
    int k = 4;
    
    printf("@@%*d\n", i, j);
    
    /* print third argument then first argument */
    printf("!!%3$*1$d, %1$*2$d\n", i, j, k);

    return 0;
}
```

看看輸出吧，一樣懶得寫Makefile

```sh
$ make test_format_string
cc     test_format_string.c   -o test_format_string

$ ./test_format_string 
@@ 3
!! 4,   2
```

## 結論
這篇文章文章我介紹了

* format string和他的語法簡單說明
* make 的implicit rule
* 取得Linux 下執行程式結束回傳值

## 參考資料
* `man 3 printf`
* [A printf format reference page (cheat sheet)](http://alvinalexander.com/programming/printf-format-cheat-sheet)
* [Printf Format Strings](http://www.cprogramming.com/tutorial/printf-format-strings.html)
