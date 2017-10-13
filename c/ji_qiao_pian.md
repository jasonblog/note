# 技巧篇


---
tags: C, CLANG, C LANGUAGE, recursion
---

# [你所不知道的C語言](http://hackfoldr.org/dykc/)：技巧篇
Copyright (**慣C**) 2017 [宅色夫](http://wiki.csie.ncku.edu.tw/User/jserv)
==[直播錄影](https://www.youtube.com/watch?v=Uzi_b-08uQE)==

![](images/sAjUxnk.png)
C-section 是 Cesarean section 的縮寫，意思是「剖腹產術」，在本次講座中，我們引申「拿刀侵入程式開發者，萃取出新的生命」。這裡也有 "C"！
[ [Source](https://www.pinterest.com/explore/c-section-recovery/) ]


## 從矩陣操作談起

* 2017 年春季「[作業系統設計與實作](http://wiki.csie.ncku.edu.tw/sysprog/schedule)」課程的作業 [B06: software-pipelining](https://hackmd.io/s/rks62p1sl) 的設計目標：
    * 重溫計算機結構並且透過實驗來驗證所學
    * 理解 prefetch 對 cache 的影響，從而設計實驗來釐清相關議題
* 額外要求：學習 [你所不知道的 C 語言：物件導向程式設計篇](https://hackmd.io/s/HJLyQaQMl) 提到的封裝技巧，以物件導向的方式封裝轉置矩陣的不同實作，得以透過一致的介面 (interface) 存取個別方法並且評估效能

- [ ] 最初的轉置矩陣實作: [impl.c](https://github.com/sysprog21/prefetcher/blob/master/impl.c)
```C
void naive_transpose(int *src, int *dst, int w, int h)
{
    for (int x = 0; x < w; x++)
        for (int y = 0; y < h; y++)
            *(dst + x * h + y) = *(src + y * w + x);
}
```

使用方式如下:
```C
    int *src  = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    int *out2 = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
    naive_transpose(src, out2, TEST_W, TEST_H);
```
這有什麼問題呢？
* 不同的轉置矩陣操作，例如 [naive](https://en.wiktionary.org/wiki/na%C3%AFve) (這詞彙源自法文，我們引申為天真無邪的版本，不是 "native"), SSE, AVX, AVX2，需要重複撰寫相當類似的程式碼，其實都可透過一致的介面來存取，這樣效能分析和正確性驗證的程式碼就能共用
* 矩陣的內部資料表達機制「一覽無遺」，違反封裝的原則，而且不同版本的矩陣運算往往伴隨著特製的資料欄位，但上述程式碼無法反映或區隔

- [ ] 需要更好的封裝，這樣才能夠處理不同的內部資料表示法 (data structure) 和演算法 (algorithms)，對外提供一致的介面: [matrix_oo](https://github.com/jserv/matrix_oo)

```c
typedef struct matrix_impl Matrix;
struct matrix_impl {
    float values[4][4];

    /* operations */
    bool (*equal)(const Matrix, const Matrix);
    Matrix (*mul)(const Matrix, const Matrix);
};

static Matrix mul(const Matrix a, const Matrix b)
{
    Matrix matrix = { .values = {
            { 0, 0, 0, 0, }, { 0, 0, 0, 0, },
            { 0, 0, 0, 0, }, { 0, 0, 0, 0, },
        },
    };
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                matrix.values[i][j] += a.values[i][k] * b.values[k][j];
    return matrix;
}

int main()
{
    Matrix m = {
        .equal = equal,
        .mul = mul,
        .values = { ... },
    };


    Matrix o = { .mul = mul, };
    o = o.mul(m, n);
```

這樣的好處是:
* 一旦建立矩陣操作的實例 (instance)，比方說這邊的 `Matrix m`，可以很容易指定特定的方法 (method)，藉由 [designated initializers](https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html)，在物件初始化的時候，就指定對應的實作，日後要變更 ([polymorphism](https://en.wikipedia.org/wiki/Polymorphism_(computer_science))) 也很方便
    * 延伸閱讀: [Why does C++11 not support designated initializer list as C99?](https://stackoverflow.com/questions/18731707/why-does-c11-not-support-designated-initializer-list-as-c99)
    * 再說一次，從 C99 (含) 以後，C 和 C++ 的發展就走向截然不同的路線。[Incompatibilities Between ISO C and ISO C++](http://david.tribble.com/text/cdiffs.htm)
* 省下指標操作，除了 function pointer 的使用外，在程式中沒有直接出現，對於數值表達較為友善
    * 善用 Compound Literals for Structure Assignment
* 克服命名空間 (namespace) 衝突的問題，注意上述 `equal` 和 `mul` 函式實作時都標註 `static`，所以只要特定實作存在獨立的 C 原始程式檔案中，就不會跟其他 compilation unit 定義的符號 (symbol) 有所影響，最終僅有 [API gateway](https://github.com/embedded2016/server-framework/blob/master/async.c#L258) 需要公開揭露

:::info
:girl: ==[Prefer to return a value rather than modifying pointers](https://github.com/mcinglis/c-style#prefer-to-return-a-value-rather-than-modifying-pointers)==

This encourages immutability, cultivates pure functions, and makes things simpler and easier to understand. It also improves safety by eliminating the possibility of a NULL argument.

:-1: unnecessary mutation (probably), and unsafe

```c
void drink_mix(Drink * const drink, Ingredient const ingr) {
    assert(drink);
    color_blend(&(drink->color), ingr.color);
    drink->alcohol += ingr.alcohol;
}
```

:+1: immutability rocks, pure and safe functions everywhere

```c
Drink drink_mix(Drink const drink, Ingredient const ingr) {
    return (Drink) {
        .color = color_blend(drink.color, ingr.color),
        .alcohol = drink.alcohol + ingr.alcohol
    };
}
```
:::

不過仍是不夠好，因為：
1. Matrix 物件的 `values` 欄位仍需要公開，我們就無法隱藏實作時期究竟用 `float`, `double`, 甚至是其他自訂的資料表達方式 (如 [Fixed-point arithmetic](https://en.wikipedia.org/wiki/Fixed-point_arithmetic))
2. 天底下的矩陣運算當然不是隻有 4x4，現有的程式碼缺乏彈性 (需要透過 malloc 來配置空間)
3. 如果物件配置的時候，沒有透過 [designated initializers](https://gcc.gnu.org/onlinedocs/gcc/Designated-Inits.html) 指定對應的方法，那麼後續執行 `m.mul()` 就註定會失敗
4. 如果 Matrix 物件本身已初始化，以乘法來說，我們期待`matrixA * matrixB`，對應程式碼為 `matO = matA.mul(matB)`，但在上述程式碼中，我們必須寫為 `Matrix o = m.mul(m, n)`，後者較不直覺
5. 延續 `2.`，如果初始化時配置記憶體，那要如何確保釋放物件時，相關的記憶體也會跟著釋放呢？若沒有充分處理，就會遇到 memory leaks
6. 初始化 Matrix 的各欄、各列的數值很不直覺，應該設計對應的巨集以化簡
7. 考慮到之後不同的矩陣運算可能會用 [plugin 的形式](http://eli.thegreenplace.net/2012/08/24/plugins-in-c) 載入到系統，現行封裝和 RTTI 不足
    * 要考慮的議題非常多，可見 [Beautiful Native Libraries](http://lucumr.pocoo.org/2013/8/18/beautiful-native-libraries/)

延伸閱讀:
* [Fun with C99 Syntax](http://www.dribin.org/dave/blog/archives/2010/05/15/c99_syntax/)
* [An object-oriented paradigm in the C programming language](http://www.cybletter.com/index.php?s=file_download&id=68)
* [以 C 語言實做 Javascript 的 prototype 特性](http://blog.linux.org.tw/~jserv/archives/002057.html)
* 更多矩陣運算: [matrix.h](https://github.com/openvenues/libpostal/blob/master/src/matrix.h), [matrix.c](https://github.com/openvenues/libpostal/blob/master/src/matrix.c)
* "Object-oriented design patterns in the kernel" 中文翻譯
    * [Part 1](https://paper.dropbox.com/doc/Object-oriented-design-patterns-in-the-kernel-part-1-kkc4L7OdvowY07GTfaOWZ#:a2=1441167125), [Part 2](https://hackmd.io/AwdgpgxgTAHAnAVgLQDYBGCRICwDMDMyauwAJkpiBMHMGjBBCEA=) 

:::success
作業要求:
1. 在 GitHub 上 fork [matrix_oo](https://github.com/jserv/matrix_oo)，嘗試改善上述提及的議題
2. 善用 `_Generic` 來提供矩陣 / 向量的乘法操作
:::


## 明確初始化特定結構的成員

* C99 給予我們頗多便利，比方說：
```c
const char *lookup[] = {
    [0] = "Zero",
    [1] = "One",
    [4] = "Four"
};
assert(!strcasecmp(lookup[0], "ZERO"));
```

也可變化如下:
```c
enum cities { Taipei, Tainan, Taichung, };
int zipcode[] = {
    [Taipei] = 100,
    [Tainan] = 700,
    [Taichung] = 400,                     
};
```


## 追蹤物件配置的記憶體

前述矩陣操作的程式，我們期望能導入下方這樣自動的處理方式:
```c
struct matrix { size_t rows, cols; int **data; };

struct matrix *matrix_new(size_t rows, size_t cols)
{
    struct matrix *m = ncalloc(sizeof(*m), NULL);
    m->rows = rows; m->cols = cols;
    m->data = ncalloc(rows * sizeof(*m->data), m);
    for (size_t i = 0; i < rows; i++)
        m->data[i] = nalloc(cols * sizeof(**m->data), m->data);
     return m;
 }
 
void matrix_delete(struct matrix *m) { nfree(m); }
```

其中 `nalloc` 和 `nfree` 是我們預期的自動管理機制，對應的實作可見 [nalloc](https://github.com/jserv/nalloc)

複製字串可用 strdup 函式：
```C
char * strdup(const char *s);
```
strdup 函式會呼叫 malloc 來配置足夠長度的記憶體，當然，你需要適時呼叫 free 以釋放資源。 [==heap==]

strdupa 函式透過 [alloca 函式](http://man7.org/linux/man-pages/man3/alloca.3.html)來配置記憶體，後者存在 [==stack==]，而非 heap，當函式返回時，整個 stack 空間就會自動釋放，不需要呼叫 free。
```C
char * strdupa(const char *s);
```
:::info
* `alloca` function is not in POSIX.1.
* alloca() function is machine- and compiler-dependent.  * For certain applications, its use can improve efficiency compared to the use of malloc(3) plus free(3).
* In certain cases, it can also simplify memory deallocation in applications that use longjmp(3) or siglongjmp(3).  Otherwise, its use is discouraged.
* strdupa() and strndupa() are GNU extensions.
:::

* `alloca()` 在不同軟硬體平臺的落差可能很大，在 Linux man-page 特別強調以下:
:::warning
RETURN VALUE
The alloca() function returns a pointer to the beginning of the allocated space. If the allocation causes stack overflow, program behaviour is ==undefined==.
:::

延伸閱讀:
* [停止使用 strncpy 函數！](http://blog.haipo.me/?p=1065)
* [Handling out-of-memory conditions in C](http://eli.thegreenplace.net/2009/10/30/handling-out-of-memory-conditions-in-c)


## Smart Pointer

* 在 C++11 的 STL，針對使用需求的不同，提供了三種不同的 Smart Pointer，分別是：
    * unique_ptr
      確保一份資源（被配置出來的記憶體空間）只會被一個 unique_ptr 物件管理的 smart pointer；當 unique_ptr 物件消失時，就會自動釋放資源。
    * shared_ptr
      可以有多個 shared_ptr 共用一份資源的 smart pointer，內部會記錄這份資源被使用的次數（reference counter），只要還有 shared_ptr 物件的存在、資源就不會釋放；只有當所有使用這份資源的 shared_ptr 物件都消失的時候，資源才會被自動釋放。
    * weak_ptr
    搭配 shared_ptr 使用的 smart pointer，和 shared_ptr 的不同點在於 weak_ptr 不會影響資源被使用的次數，也就是說的 weak_ptr 存在與否不代表資源會不會被釋放掉，

這些 smart pointer 都是 template class 的形式，所以適用範圍很廣泛；他們都是被定義在 `<memory>`標頭檔、在 std 這個 namespace 下。

* 延伸閱讀: [C++ 智慧型指標（Smart Pointer）：自動管理與回收記憶體](https://msdn.microsoft.com/zh-tw/library/hh279674.aspx)

* [Implementing smart pointers for C](https://snai.pe/c/c-smart-pointers/)
    * 原理：利用 GCC extension: [attribute cleanup](https://stackoverflow.com/questions/42025488/resource-acquisition-is-initialization-in-c-lang)
    ```C
    #define autofree \
        __attribute__((cleanup(free_stack)))
    
    __attribute__ ((always_inline))
    inline void free_stack(void *ptr) { free(*(void **) ptr); }
    ```
    * 接著就可以這樣用:
    ```C
    int main(void) {
        autofree int *i = malloc(sizeof (int));
        *i = 1;
        return *i;
    }
    ```
* [Smart pointers for the (GNU) C](https://github.com/Snaipe/libcsptr): Allocating a smart array and printing its contents before destruction:

```c
#include <stdio.h>
#include <csptr/smart_ptr.h>
#include <csptr/array.h>

// @param ptr points to the current element
void print_int(void *ptr, void *meta) { printf("%d\n", *(int *) ptr); }

int main(void)
{
    // Destructors for array types are run on every
    // element of the array before destruction.
    smart int *ints = unique_ptr(int[5],
                                 {5, 4, 3, 2, 1},
                                 print_int);

    /* Smart arrays are length-aware */
    for (size_t i = 0; i < array_length(ints); ++i)
        ints[i] = i + 1;

    return 0;
}
```
* GCC的C語言實作只能在variable attribute指定`__attribute__((cleanup))`。但是function回傳的unbound物件，以及function argument attribute皆無支援`__attribute__((cleanup))`。
* 沒有以上兩者，就無法做出「function的回傳物件無須特別處理，就會自動free」，以及「傳入fucntion的物件，不做特別處理（如move），就會自動free」
* C++的smart pointer實際上就是用物件的deallcator會在out-of-scope時會自動被呼叫的特性實作的。見[unique_ptr](https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.4/a01404.html)。若C有實作以上兩者功能，其實也可以在C當中做出完整的unique pointer。

## 不夠謹慎的 ARRAY_SIZE 巨集

* 考慮以下的使用案例:
```C
void foo(int (*a)[5])
{
    int nb = ARRAY_SIZE(*a);
}
```

* [Linux Kernel: ARRAY_SIZE()](https://frankchang0125.blogspot.tw/2012/10/linux-kernel-arraysize.html)
* [藝術與核心](http://blog.linux.org.tw/~jserv/archives/001888.html)
* 從 Linux 核心「提煉」出的 [array_size](http://ccodearchive.net/info/array_size.html)
* [_countof Macro](https://msdn.microsoft.com/en-us/library/ms175773.aspx)


## C99 Variable Length Arrays

* [Visual C++ 不目前支援可變長度陣列](https://msdn.microsoft.com/zh-tw/library/zb1574zs.aspx)
* 使用案例:
```C
void f(int m, int C[m][m])  
{  
    double v1[m];  
    ...  
    #pragma omp parallel firstprivate(C, v1)  
    ...  
}  
```
* Randy Meyers (chair of J11, the ANSI C committee) 的文章 [The New C:Why Variable Length Arrays?](http://www.drdobbs.com/the-new-cwhy-variable-length-arrays/184401444)，副標題是 "C meets Fortran, at long last."

* 一個特例是 [Arrays of Length Zero](https://gcc.gnu.org/onlinedocs/gcc/Zero-Length.html)，GNU C 支援，在 Linux 核心出現多次
    * C90 和 C99 語意不同
延伸閱讀: [Zero size arrays in C](https://news.ycombinator.com/item?id=11674374)

## `do { ... } while(0)` 巨集

* 避開 dangling else
* 延伸閱讀: [ multi-line macro: do/while(0) vs scope block](https://stackoverflow.com/questions/1067226/c-multi-line-macro-do-while0-vs-scope-block)


## GCC 支援 Plan 9 C Extension

* gcc 編譯選項 `-fplan9-extensions` 可支援 [Plan 9 C Compilers](https://9p.io/sys/doc/compiler.html) 特有功能
* 「繼承」比你想像中簡單
```C
typedef struct S {
    int i;
} S;

typedef struct T {
    S;           // <- "inheritance"
} T;

void bar(S *s) { }

void foo(T *t) {
    bar(t);   // <- call with implict conversion to "base class"
    bar(&t->S); // <- explicit access to "base class"
}
```

* 若要在寫出 gcc/clang 中都支援的版本，可考慮改用 `-fms-extensions` 編譯選項。見 [GCC Unnamed Fields](https://gcc.gnu.org/onlinedocs/gcc/Unnamed-Fields.html) 


## GCC transparent union

* C 語言實作繼承也可善用 [transparent union](https://gcc.gnu.org/onlinedocs/gcc/Common-Type-Attributes.html#Common-Type-Attributes)
* 以上的繼承範例，在呼叫 base class 時得用 `&t->S` 或 type cast `(S*)t`。但若用 transparent union，即可透過更漂亮的語法來實作：

```C
typedef union TPtr TPtr;

union TPtr {
    S *S;
    T *T;
} __attribute__((__transparent_union__));

void foo(TPtr t)
{
    t.S->s_element;
    t.T->t_element;
}

T* t;
foo(t); // T * can be passed in as TPtr without explicit casting
```

* 這個特性也可用來實作polymorphism
```C
typedef enum GenericType GenericType;
typedef struct A A;
typedef struct B B;

enum GenericType {
    TYPE_A = 0,
    TYPE_B,
};

struct A {
    GenericType type;
    ...
};

struct B {
    GenericType type;
    ...
};

union GenericPtr {
  GenericType *type;
  A *A;
  B *B;
} __attribute__((__transparent_union__));

void foo (GenericPtr ptr)
{
    switch (*ptr.type) {
    case TYPE_A:
	    ptr.A->a_elements;
	    break;
	case TYPE_B:
	    ptr.B->b_elements;
	    break;
	default:
	    assert(false);
    }
}

A *a;
B *b;
foo(a);
foo(b);
```


## 計算時間不只在意精準度，還要知道特性

* 參照 [時間處理與 time 函式使用](https://hackmd.io/s/HkiJpDPtx) 和 [計算機系統中的時間](http://blog.haipo.me/?p=906)
* [High Resolution Timers](http://elinux.org/High_Resolution_Timers)


## GOTO 沒想像中那麼可怕

* [有時不用 goto 會寫出更可怕的程式碼](http://eli.thegreenplace.net/2009/04/27/using-goto-for-error-handling-in-c)
* 參照 [Computed goto for efficient dispatch tables](http://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables)
    * Doing less per iteration
    * Branch prediction
* 臺大資工 Android Compiler and Virtual Machines ([2014](https://ntu-android-2014.hackpad.com/))


## 高階的 C 語言「開發框架」

* [Cello](http://libcello.org/) 在 C 語言的基礎上，提供以下進階特徵:
    * Generic Data Structures
    * Polymorphic Functions
    * Interfaces / Type Classes
    * Constructors / Destructors
    * Optional Garbage Collection
    * Exceptions
    * Reflection

可寫出以下風格的 C 程式:
```C
/* Stack objects are created using "$" */
var i0 = $(Int, 5);
var i1 = $(Int, 3);
var i2 = $(Int, 4);

/* Heap objects are created using "new" */
var items = new(Array, Int, i0, i1, i2);

/* Collections can be looped over */
foreach (item in items) {
    print("Object %$ is of type %$\n",
          item, type_of(item));
}
```

## Block

* 巨集限制很多，因為本質是「展開」，這會導致多次的 evalution
* 考慮以下程式碼:
```C
#define DOUBLE(a) ((a) + (a))
int foo()
{
    printf(__func__);
    return 3;
}

int main ()
{
    DOUBLE(foo()); /* 呼叫 2 次 foo() */
}
```

為此，我們可以使用區域變數，搭配 GNU extension `__typeof__`，改寫上述巨集:
```C
#define DOUBLE(a) ({ \
    __typeof__(a) _x_in_DOUBLE = (a); \
    _x_in_DOUBLE + _x_in_DOUBLE; \
})
```

為什麼有 `_x_in_DOUBLE` 這麼不直覺的命名呢？因為如果 `a` 的表示式中恰好存在與上述的區域變數同名的變數，那麼就會發生悲劇。

如果你的編譯器支援 [Block](https://en.wikipedia.org/wiki/Blocks_(C_language_extension))，比方說 clang，就可改寫為:
```C
#define DOUBLE(a) \
    (^(__typeof__(a) x){ return x + x; }(a))
```

:::info
* Block in C uses a lambda expression-like syntax to create closures.
* 在 clang 使用時，要加上 `-fblocks` 編譯選項
:::


延伸閱讀:
* [10 C99 tricks](https://blog.noctua-software.com/c-tricks.html)
* [C Tips and Tricks](https://www.desultoryquest.com/blog/c-tips-and-tricks-0/)
* [C 語言有什麼奇技淫巧？](https://www.zhihu.com/question/27417946)
* [C 語言的奇技淫巧](http://www.anwcl.com/wordpress/c%E8%AF%AD%E8%A8%80%E7%9A%84tricks%E4%B8%8E%E6%9C%AA%E5%AE%9A%E4%B9%89%E8%A1%8C%E4%B8%BA-undefined-behavior/)