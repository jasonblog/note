# C 語言的逗號


因為只能和 C語言裝熟，只好看書看能不能裝更熟點。感謝成大同學推荐的書本：[C Programming: A Modern Approach](http://knking.com/books/c2/index.html)。今天看到逗號的用法，手癢來用一下。

簡單來說，`,`就是把[expression](http://en.wikipedia.org/wiki/Expression_(computer_science\))串在一起，然後回傳最後一個expression的值。注意的是expression是compiler專有名詞，自己看連結，不解釋。

知道這個特性，當然來個小實驗。照慣例還是要講一下測試環境。

```sh
$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 14.04.2 LTS
Release:	14.04
Codename:	trusty
```

接下來寫個小程式：

- comma.c

```c 
#include <stdio.h>
int main(int argc, char **argv)
{
    int i = 0;
    int j = 10;

    return -1, i + j;
}
```

程式太簡單，懶得寫Makefile，直接make：

```sh
$ make comma
cc     comma.c   -o comma
```

接下來就是驗收

```sh
$ ./comma
$ echo $?
10
```

接下來要問問題時間

* 為啥要這樣幹？
* 這個要用在這什麼地方？

先回答第一個問題，就是要把一堆expression擠到被認為是一個expression。有了這樣的想法後，很多語法會只有一個express，如`if`, `for`, `while`，這時候想要在裏面多做事想得不得了的時候就可以用`,`，不過和條件有關的情況下，記得注意僅回傳最後一個expression的特色。

題外話，這個`,`目前我印象中除了變數宣告和`for`以外，只看過[一次](https://github.com/embedded2013/rtenv/blob/master/kernel.c#L365)，那次看到讓我有點反應不過來，感覺單純只是想省掉`{}`而已。
