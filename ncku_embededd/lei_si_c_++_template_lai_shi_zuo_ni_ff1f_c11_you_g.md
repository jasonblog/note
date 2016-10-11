# 類似 C++ template 來實做呢？C11 有 _Generic 關鍵字


﻿# 2016q3 Homework2 (phonebook-concurrent)
contribute by <`c14006078`>

## refactor

看完兩個中文連結，就我理解上，所謂的 refactor的目的就是讓程式更易於維護、新增功能、容易找出bug出在那邊，將程式的分工並將功能最小化增加 reuse，而他是一個在寫程式時就該思考該改善的步驟，如和將程是的分工變得非常明確，不讓重複的code一直重寫。

《[Refactoring](http://jjhou.boolan.com/jjtbooks-refactoring.htm)》書中提到22個壞味道，包含：duplicated code、long method、large class、long parameter list、divergent change、shotgun surgery、feature envy、data clumps、primitive obsession、switch statements、parallel inheritance hierarchies、lazy class、speculative generality、temporary field、message chains、middle man、inappropriate intimacy、alternative classes with different interfaces、incomplete library class、data class、refused bequest、comments。


參考：[對於重構的兩則常見誤解](http://www.ithome.com.tw/node/79813)

## 檢討自己的缺失

1. 變數命名很爛

>> 解決方式：這個部份其實我參考了很多命名的文件，像是 Camel case 的方式(ex: myName)，或是匈牙利命名法，我最終選擇 Camel case(可是老師給我的 comment是 linux kernel 不使用這些...)，目前我已經有定立自己的 coding style。

2. 沒有註解，程是碼流程非常的混雜，讓人不清楚這一大串到底再幹嘛。
>> 解決方式：在每一個流程前給一個註解，應該說在寫 code 時就應該要建立這樣子的流程圖，而不是寫完之後再去加上。
3. 可以省略的變數有點多，有些根本是不必要的。
>> 解決方式：就以 append()這裡麵的很多變數可以不用的，像是 entryStart，搞不好連 nthread也可以省略，因為有 Macro 在。
4. main 裡面的 code太多了，會有 duplicated code, long method的問題。
>> 解決方式：說到 code reuse，其時我應該把大部分的 code 丟到phonebook opt裡，這樣會讓 main看起來簡單易懂，而且也能增進 reuse。
5. 在 make 時定義 THREAD NUM，這非常的麻煩...
>> 解決方式: [Get number of CPUs in Linux using C](http://stackoverflow.com/questions/4586405/get-number-of-cpus-in-linux-using-c) [name=jserv]
>> 水啦，這個好神啊[name=Yen-Kuan Wu]

## 回傳系統資訊 in C

`sysconf(3)` 主要回傳數字，透過不同的 flag 能得到不同的回傳值，算是挺全面的。

我用它來改我的 hardcode `THREAD_NUM`
`#define THREAD_NUM (int)(sysconf(_SC_NPROCESS_ONLN))`

我也有練習其他的部分，這邊就不列舉了。
https://github.com/c14006078/PracticeMakePerfect/commit/6e3ae2b25a75c6ad11d556838c4a75e7ab738b87

## phonebook物件化
說到 refactor的改善，我嘗試想把我的 code做物件化，我覺得他給我們最大的好處就是，讓別人一眼就看出來我們的 phonebook的 attribute 和 可以使用的 method，而不會是像 append這樣簡易的命名，讓我們不清楚他到底再 append哪些東西，而且要加額外的功能只要再多開一個 function pointer，大家一目瞭然phonebook 多了什麼功能。

* phonebook
```clike=
typedef struct _phonebook {	
    entry *pHead;
    entry *pLast;
	
    unsigned long long count;

    entry *(*append)(char *lastName, entry *pHead);
    entry *(*find)(char *lastName, entry *pHead);
} phonebook;
```
>> 請留意縮排風格，用 4 個空白而非 tab。我們期望寫作 `entry *pHead` 而非 `entry* pHead`，operator `*` 放的位置很重要 [name=jserv]
>> 好的，這我真的沒注意到，囧。[name=Yen-Kuan]

這個部份我會留在後面再改動，這其時會改到全部程式碼，因為我希望把 list都保護在phonebook裡。

## linkList reusable

想到 code reuse，我目前在做一份比較完整版的 linkList，考慮到了如果我們今天突然 linkList裡的變數不在是 int data的話，我要怎樣改動最少的 code呢？

code: [W2](https://github.com/c14006078/sysprog2016q3/tree/master/W2)
```clike=
typedef struct _context{
    int data;
} context;

typedef context *ctxPtr;
typedef struct _listNode {
    ctxPtr ctx;
    struct _listNode *next;
} listNode;
```

將 new 跟 init 分離。
```clike=
static ctxPtr initCtx()
{
    ctxPtr p = (ctxPtr) malloc(sizeof( context));///FIXME:Dup
    assert(p && "malloc error");

    return p;
}

static ctxPtr newCtx(int data)
{
    ctxPtr p = initCtx();
    p->data = data;

    return p;
}
```
這邊我又想到了，這個 malloc也是一直一直再寫，assert也要一直寫，所以我有沒有辦法把他包成一個 function呢？這的感覺就像是在寫 system 時要常常注意 open有沒有開成功，一堆的。

回到原點，我該怎樣讓一個 malloc 支援很多 type勒， template

## Template in C

這邊我想了一陣子，想到最初的版本就是 Macro，用他來替換字元是最好的方式
```clike=
#define malloc(type, ctxtype, num) \
    type malloc(sizeof(ctxtype) * num)
```

但後來覺得 Macro是直接替換，所以真的比較難 debug，所以我詢問老師後，老師建議我使用 C11 的 [_Generic](http://en.cppreference.com/w/c/language/generic)，大概用法如下，我也寫過一個範例如下，這感覺出乎意料的好用
```clike=
#define printf_dec_format(x) _Generic((x), \
    char: "%c", \
    signed char: "%hhd", \
    unsigned char: "%hhu", \
    signed short: "%hd", \
    unsigned short: "%hd", \
    signed int: "%d", \
    unsigned int: "%u", \
    unsigned long int: "%ld", \
    long long int: "%lld", \
    unsigned long long int: "%llu", \
    float: "%f", \
    double: "%lf", \
    long double: "%Lf", \
    char *: "%s", \
    void *: "%p")
```
```clike=
#define print(x)    printf( printf_dec_format(x), x)
#define printnl(x)  printf( printf_dec_format(x), x); printf("\n")

int main(int argc, char *argv)
{
    printnl('a');
    printnl((char) 'a');
    printnl(123);
    printnl(1.234);
}
```
>> 請修正 coding style! 不要亂加空白，「`(` 之後」和「`)` 之前」都不該有額外的空白。重申: 4 個 space，而非 tab，程式開發很在意紀律和規範。 [name=jserv]
>>慘了= =，我可以請問老師一下是直接敲4個space 還是說astyle 或是改 vimrc。[name=Yen-Kuan Wu]
>>直接在 .vimrc 改比較方便，把 tab 轉換成 spaces。
>>set expandtab " 空格取代 tab
>>set tabstop=4 " 4 個空格
>>set shiftwidth=4 " 針對縮排所需要的空格個數 [name=Charles Lee] 

## thread pool

參考 Mathias Brossard 的 [mbrossard/threadpool](https://github.com/c14006078/threadpool/tree/master/src) (BSD Licens)

他這份實作有完整程式碼，行數不是很多，但他開出來的 api跟 struct member都非常清楚，不像我的 `append_a`完全不知道是什麼。

這邊我看到他的 `thread` 用的 `struct` 很漂亮，也是我要改善再我實作裡面的。
```clike=
typedef struct {
    void (*function)(void *);
    void *argument;
} threadpool_task_t;
```

~~目前想要嘗試將其改寫成 ring buffer~~
>>他已經是...[name=Yen-Kuan Wu]

首先，我嘗試跟著 mbrossard的腳步重建一個 threadpool，先整理出我的初步構想。

* create 一個 threadpool後，裡面養著很多 thread並且使用 cond_wait的方式等待 signal
* 加入一個 task時就去 cond_signal，而且要 assign 給他目前的 function 和 argument
* 

## condition variable 用法及實例

我參考我 IPC的書籍，嘗試選寫一份 condition variable 的用法。

## 淺談 `do{ } while (0)`

常會看到這寫法，特別在 Linux 核心原始程式碼，在老師修改我的debug.h也出現，目前我看到的兩個用法是第一個

1. macro 時會有問題，如果你一次 define 一串複合的指令
2. 再來是可以使用 break，不之你有沒有什麼時候想要跳過某些 code，這是你就必須多設一個 flag，但如果用這個方式直接 break就可以跳過了

主要參考工程師的好朋友：http://stackoverflow.com/questions/257418/do-while-0-what-is-it-good-for

## mbrossard/threadpool：pull request 小插曲
[FIX: warning: ISO C90 forbids mixed declarations and code [-Wpedantic]](https://github.com/mbrossard/threadpool/pull/11)

再剛載下來時，compile時發現一些 warning，很像是 C90有特別規定 declaration有需要放在前面，我就順手改一下

但是 push 上去有個東西很神奇網站叫 [travis-ci](travis-ci.org)，他很像是個可以把你的 code 拿去 linux 和 mac 上，而且使用不同的 gcc 版本編譯，而我 pull 上去的版本有一沒過，`Mac gcc-5`，其實我有去看裡面的編譯過程

https://travis-ci.org/mbrossard/threadpool/jobs/164642802
```clike=
$ make
gcc-5 -D_REENTRANT -Wall -pedantic -Isrc   -c -o tests/thrdtest.o tests/thrdtest.c
make: gcc-5: No such file or directory
make: *** [tests/thrdtest.o] Error 1
```
看起來不太像我的問題，反而比較像是他們 gcc-5 沒裝成功的感覺。
>> 上游已在 Oct 4 修正了 [name=jserv]

看到老師的提點`git rebase -i`，我還在想說要怎麼樣讓他們重新測試哩。

## Git rebase -i

###### tags: `c14006078` `phonebook-concurrency` `sysprog21`