# C99的inline Function


[inline function](http://en.wikipedia.org/wiki/Inline_function)是一個keyword，提醒compiler可以將function本體直接填入呼叫該function的位置。從[這邊](http://en.wikipedia.org/wiki/Inline_expansion)可以看到，優點為

* 減少呼叫function的overhead，如參數傳遞、回傳值處理等。
* 切換到function會jump，也就是會有branch的行為。因為CPU的pipeline和cache的特性，會影響效能。
* 經由代換程式碼，也許可以增加最佳化的可能性。

廢話少說，先來一段程式碼。
測試環境

* Ubuntu 12.04.4
* GCC 4.6.3

- inline_test.c

```c 
#include <stdio.h>

inline void hello()
{
    printf("Hello World\n");
}

int main(void)
{
    hello();

    return 0;
}
```

可以正常編譯

```sh
$ cc -g -Wall -Werror -c -o inline_test.o inline_test.c
$ cc -g -Wall -Werror -o inline_test inline_test.o
```

有趣的是，換成C99就會編譯錯誤，檢查symbol的確不存在。
```sh
cc -g -Wall -Werror -std=c99   -c -o inline_test.o inline_test.c
cc -g -Wall -Werror -std=c99 -o inline_test inline_test.o
inline_test.o: In function `main':
inline_test.c:10: undefined reference to `hello'
collect2: ld returned 1 exit status
make: *** [inline_test] Error 1

$ nm inline_test.o 
                 U hello
0000000000000000 T main
```

從[這邊](http://stackoverflow.com/questions/216510/extern-inline/216546#21654)可以看到C99的`inline`定義是和GNU C的`extern inline`相反。所以最簡單的懶人法就是在C99裏面加上`extern`收工。

- inline_test_c99.c

```c 
#include <stdio.h>

extern inline void hello()
{
    printf("Hello World\n");
}

int main(void)
{
    hello();

    return 0;
}
```

---
## 加碼測試
不過對於組裝工而言，為何要在`inline`前面加`extern`或是`static`實在有趣，所以多測了幾下。沒興趣的就直接看[結論](#concl)吧

## GNU C測試 
如果我們把程式碼改成
- test_inline.c

```c 
#include <stdio.h>

extern inline void hello()
{
    int i = 100;
    printf("Hello World: %d\n", i);
}

inline void hello()
{
    printf("Hello World 2\n");
}

int main(void)
{
    hello();

    return 0;
}
```
在GNU C下編譯執行會印`Hello World 2`，使用`objdump -S`反組譯可以看到

* main 呼叫`4004f4`位址
* `4004f4`真正的程式碼是印出`Hello World2`，也就是說`extern inline void hello()`裏面的程式碼是寫心酸的。
* 看起來這種情況GCC沒有把inline function展開。

另外一點有趣的是C99下面把`extern inline`和`inline`對調會編譯失敗。

```sh
00000000004004f4 <hello>:
    int i = 100;
    printf("Hello World: %d\n", i);
}

inline void hello()
{
  4004f4:	55                   	push   %rbp
  4004f5:	48 89 e5             	mov    %rsp,%rbp
    printf("Hello World 2\n");
  4004f8:	bf 0c 06 40 00       	mov    $0x40060c,%edi
  4004fd:	e8 ee fe ff ff       	callq  4003f0 <puts@plt>
}
  400502:	5d                   	pop    %rbp
  400503:	c3                   	retq   

0000000000400504 <main>:

int main(void)
{
  400504:	55                   	push   %rbp
  400505:	48 89 e5             	mov    %rsp,%rbp
    hello();
  400508:	b8 00 00 00 00       	mov    $0x0,%eax
  40050d:	e8 e2 ff ff ff       	callq  4004f4 <hello>

    return 0;
  400512:	b8 00 00 00 00       	mov    $0x0,%eax
}
```

## 結論

* GNU C和C99 inline的`extern`定義相反。
* C99下面只有`inline`只是一個宣告，不會產生symbol。要使用`extern`編譯器才會產生symbol。猜測可能單純`inline`是在header file宣告用，而`extern inline`則是在source code實作時使用。

## 參考資料

* [Inline function](http://en.wikipedia.org/wiki/Inline_function)
* [Inline expansion](http://en.wikipedia.org/wiki/Inline_expansion)
* [extern inline](http://stackoverflow.com/questions/216510/extern-inline/216546#216546)
* [Extern Vs Static Inline](http://elinux.org/Extern_Vs_Static_Inline)
* [Is 「inline」 without 「static」 or 「extern」 ever useful in C99?](http://stackoverflow.com/questions/6312597/is-inline-without-static-or-extern-ever-useful-in-c99)
