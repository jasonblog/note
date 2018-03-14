---
title: C 語言的奇技淫巧
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,c,cpp,c++,program
description: 整理下 C 語言中常用的技巧。變長數組。
---

整理下 C 語言中常用的技巧。

<!-- more -->

## 變長數組

實際編程中，經常會使用變長數組，但是 C 語言並不支持變長的數組，可以使用結構體實現。

類似如下的結構體，其中 value 成員變量不佔用內存空間，也可以使用 ```char value[]``` ，但是不要使用 ``` char *value```，該變量會佔用指針對應的空間。

常見的操作示例如下。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct foobar {
  int len;
  char value[0];
} foobar_t;

int main(int argc, char *argv[])
{
    // 兩者佔用的存儲空間相同，也就是value不佔用空間
    printf("%li %li\n", sizeof(int), sizeof(foobar_t));

    // 初始化
    int *values = (int *)malloc(10*sizeof(int)), i, j, *ptr;
    for (i = 0; i < 10; i++)
      values[i] = 10*i;
    for (i = 0; i < 10; i++)
      printf(" %i", values[i]);
    printf("\n");

    // 針對單個結構體的操作
    foobar_t *buff = (foobar_t *)malloc(sizeof(foobar_t) + 10*sizeof(int));
    buff->len = 10;
    memcpy(buff->value, values, 10*sizeof(int));
    ptr = (int *)buff->value;

    printf("length: %i, vlaues:", buff->len);
    for (i = 0; i < 10; i++)
      printf(" %i", ptr[i]);
    printf("\n");
    free(buff);

    // 針對數組的操作
    #define FOOBAR_T_SIZE(elements) (sizeof(foobar_t) + sizeof(int) * (elements))
    foobar_t **buf = (foobar_t **)malloc(6*FOOBAR_T_SIZE(10));
    foobar_t *ptr_buf;
    for (i = 0; i < 6; i++) {
      ptr_buf = (foobar_t*)((char *)buf + i*FOOBAR_T_SIZE(10));
      ptr_buf->len = i;
      memcpy(ptr_buf->value, values, 10*sizeof(int));

      ptr = (int *)ptr_buf->value;
      printf("length: %i, vlaues:", ptr_buf->len);
      for (j = 0; j < 10; j++)
        printf(" %i", ptr[j]);
      printf("\n");
    }
    free(values);
    free(buf);

    return 0;
}
{% endhighlight %}


## qsort

`qsort()` 會根據給出的比較函數進行快排，通過指針移動實現排序，時間複雜度為 `n*log(n)`，排序之後的結果仍然放在原數組中，不保證排序穩定性，如下是其聲明。

{% highlight text %}
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void qsort_r(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *arg);
    base: 數組起始地址；
    nmemb: 數組元素個數；
    size: 每個元素的大小；
    compar: 函數指針，指向定義的比較函數，當elem1>elem2返回正數，此時不交換。
{% endhighlight %}

通常可以對整數、字符串、結構體進行排序，如下是常用示例。

{% highlight text %}
----- 對int類型數組排序
int num[100];
int cmp(const void *a , const void *b)
{
	return *(int *)a - *(int *)b;
}
qsort(num, sizeof(num)/sizeof(num[0]), sizeof(num[0]), cmp);

----- 對結構體進行排序
struct foobar {
	int data;
	char string[10];
} s[100]
int cmp_int(const void *a, const void *b) /* 按照data遞增排序 */
{
	return (*(struct foobar *)a).data > (*(struct foobar *)b).data ? 1 : -1;
}
int cmp_string(const void *a, const void *b)
{
	return strcmp((*(struct foobar *)a).string, (*(struct foobar *)b).string);
}
qsort(num, sizeof(num)/sizeof(num[0]), sizeof(num[0]), cmp);
{% endhighlight %}

以及示例程序。

{% highlight c %}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct foobar {
	int data;
	char string[100];
} array[10];

int cmp_int(const void * a, const void * b)
{
	return (*(struct foobar *)a).data - (*(struct foobar *)b).data;
}

int cmp_string(const void * a, const void * b)
{
	return strcmp((*(struct foobar *)a).string, (*(struct foobar *)b).string);
}

int main (void)
{
	int i, j;
	int array_size = sizeof(array)/sizeof(array[0]);
	printf("Array size %d\n", array_size);

	srand((int)time(0));
	for (i = 0; i < array_size; i++) {
		int r = rand() % 100;
		array[i].data = r;
		for (j = 0; j < r; j++)
			array[i].string[j] = 'A' + rand() % 26;
		array[i].string[r] = 0;
	}

	printf("Before sorting the list is: \n");
	for (i = 0 ; i < array_size; i++ )
		printf("%d ", array[i].data);
	puts("");
	for (i = 0 ; i < array_size; i++ )
		printf("%s\n", array[i].string);

	printf("\nAfter sorting the list is: \n");
	qsort(array, array_size, sizeof(struct foobar), cmp_int);
	for (i = 0 ; i < array_size; i++ )
		printf("%d ", array[i].data);
	puts("");

	printf("\nAfter sorting the list is: \n");
	qsort(array, array_size, sizeof(struct foobar), cmp_string);
	for (i = 0 ; i < array_size; i++ )
		printf("%s\n", array[i].string);

	return 0;
}
{% endhighlight %}

<!--
https://www.felix021.com/blog/read.php?entryid=1951
-->

## 指針

指針或許是 C 語言中最複雜的東西了。

### 指針常量 VS. 常量指針

前面是一個修飾詞，後面的是中心詞。

#### 常量指針

**常量指針** 首先是一個指針，指向的是常量，即指向常量的指針；可以通過如下的方式定義：

{% highlight c %}
const int a = 7;
const int *p = &a;
{% endhighlight %}

對於常量，我們不能對其內容進行修改；指針的內容本身是一個地址，通過常量指針指向一個常量，為的就是防止我們寫程序過程中對指針誤操作出現了修改常量這樣的錯誤，如果我們修改常量指針的所指向的空間的時候，編譯系統就會提示我們出錯信息。

在 C 語言中，通常定義的字符串會返回一個常量指針，因此字符串不能賦值給字符數組，只能賦值到指針。

總結一下，<font color="red">常量指針就是指向常量的指針，指針所指向的地址的內容是不可修改的，指針本身的內容是可以修改的</font> 。


#### 指針常量

**指針常量**  首先是一個常量，再才是一個指針；可以通過如下的方式定義：

{% highlight c %}
int a = 7;
int * const p = &a; // OR int const *p = &a;
{% endhighlight %}

常量的性質是不能修改，指針的內容實際是一個地址，那麼指針常量就是內容不能修改的常量，即內容不能修改的指針，指針的內容是什麼呀？指針的內容是地址，所以，說到底，就是不能修改這個指針所指向的地址，一開始初始化，指向哪兒，它就只能指向哪兒了，不能指向其他的地方了，就像一個數組的數組名一樣，是一個固定的指針，不能對它移動操作。

它只是不能修改它指向的地方，但這個指向的地方里的內容是可以替換的，這和上面說的常量指針是完全不同的概念。

作一下總結，<font color="red">指針常量就是是指針的常量，它是不可改變地址的指針，但是可以對它所指向的內容進行修改</font> 。

源碼可以參考 [github const_pointer.c]({{ site.example_repository }}/c_cpp/c/const_pointer.c) 。

### 與一維數組

假設有如下數組，

{% highlight c %}
int Array[] = {1, 2, 3, 4};
int *ptr = Array;
{% endhighlight %}

其中 Array 為指針常量，而 ptr 為指針變量，且 ```ptr = &Array[0]```，那麼如下的操作相同 ```ptr[i] <=> *(ptr+i)``` 以及 ```Array[i] <=> *(Array + i)``` 。

如下，簡單介紹下常見操作。

#### *ptr++

由於 ```*``` 和 ```++``` 優先級相同，結合性為由右至左，即 ```*ptr++``` 等價於 ```*(ptr++)``` ，由於 ```++``` 為後繼加，所以當得到 ```*ptr``` 後再處理 ```++```；所以 ```*ptr++``` 等於 1，進行此項操作後 ```*ptr``` 等於 2。

執行的步驟為 1) ```++``` 操作符產生 ptr 的一份拷貝；2) ```++``` 操作符增加 ptr 的值；3) 在 ptr 上執行間接訪問操作。

#### ++*ptr

利用優先級和結合性可得，```++*ptr``` 等價於 ```++(*ptr)``` ，此時 ```Array[0]``` 為 2，返回 2 。

#### *ptr++

利用優先級和結合性可得，```*ptr++``` 等價於 ```*(ptr++)``` ，返回 1，ptr 值加 1 。

## 大小端

當數據類型大於一個字節時，其所佔用的字節在內存中的順序存在兩種模式：小端模式 (little endian) 和大端模式 (big endian)，其中 MSB(Most Significant Bit) 最高有效位，LSB(Least Significant Bit) 最低有效位.

{% highlight text %}
小端模式
MSB                             LSB
+-------------------------------+
|   1   |   2   |   3   |   4   | int 0x1234
+-------------------------------+
  0x03    0x02    0x01    0x00   Address

大端模式
MSB                             LSB
+-------------------------------+
|   1   |   2   |   3   |   4   | int 0x1234
+-------------------------------+
  0x00    0x01    0x02    0x03   Address
{% endhighlight %}

如下是一個測試程序。

{% highlight c %}
#include <stdio.h>

void main(void)
{
   int test = 0x41424344;
   char* pAddress = (char*)&test;

#ifdef DEBUG
   printf("int  Address:%x Value:%x\n", (unsigned int)&test, test);
   printf("\n------------------------------------\n");

   int j;
   for(j=0; j<=3; j++){
      printf("char Address:%x Value:%c\n", (unsigned int)pAddress, *pAddress);
      pAddress++;
   }
   printf("------------------------------------\n\n");
   pAddress = (char*)&test;
#endif
   if(*pAddress == 0x44)
      printf("Little-Endian\n");
   else if(*pAddress == 0x41)
      printf("Big-Endian\n");
   else
      printf("Something Error!\n");
}
{% endhighlight %}

如果採用大端模式，則在向某一個函數通過向下類型裝換來傳遞參數時可能會出錯。如一個變量為 ```int i=1;``` 經過函數 ```void foo(short *j);``` 的調用，即 ```foo((short*)&i);```，在 foo() 中將 i 修改為 3 則最後得到的 i 為 0x301 。

大端模式規定 MSB 在存儲時放在低地址，在傳輸時 MSB 放在流的開始；小段模式反之。

## 變參傳遞

頭文件 `stdarg.h` 中對相關的宏進行了定義，其基本內容如下所示：

{% highlight c %}
typedef char * va_list;

#define _INTSIZEOF(n)       ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_start(arg_ptr,v) (arg_ptr = (va_list)&v + _INTSIZEOF(v))
#define va_arg(arg_ptr,t)   (*(t *)((arg_ptr += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(arg_ptr)     (arg_ptr = (va_list)0)
{% endhighlight %}

<!--
_INTSIZEOF(n)：我們可以經過測試得到，如果在上面的測試程序中用一個double類型（長度為8byte）和一個 long double類型（長度為10byte）做可變參數。我們發現double類型佔了8byte,而long double佔了12byte，即都是4的整數倍。這裡是因為對齊，即對Intel80x86 機器來說就是要求每個變量的地址都是sizeof(int)的倍數。char類型的參數只佔了1byte，但是它後面的參數因為對齊的關係只能跳過3byte存儲，而那3byte也就浪費掉了。<br><br>

<font color=blue><strong>va_start(arg_ptr,v)</strong></font>：使參數列表指針arg_ptr指向函數參數列表中的第一個可選參數。v是位於第一個可選參數之前的固定參數，如果有一函數的聲明是void va_test(char a, char b, char c, …)，則它的固定參數依次是a,b,c，最後一個固定參數v為c，因此就是va_start(arg_ptr, c)。<br><br>

<font color=blue><strong>va_arg(arg_ptr, t)</strong></font>：返回參數列表中指針arg_ptr所指的參數，返回類型為t，並使指針arg_ptr指向參數列表中下一個參數。首先計算(arg_ptr += _INTSIZEOF(t))，此時arg_ptr將指向下一個參數；((arg_ptr += _INTSIZEOF(t)) - _INTSIZEOF(t))等價於arg_ptr，通過(t *)將arg_ptr轉換為t類型的指針，並通過*取其值。<br><br>

<font color=blue><strong>va_end(arg_ptr)</strong></font>：使ap不再指向堆棧,而是跟NULL一樣。<br><br>
-->

示例如下圖所示：

{% highlight c %}
#include <stdarg.h>
#include <stdio.h>

int max( int num,...)
{
    int m = -0x7FFFFFFF; /* 32系統中最小的整數 */
    int i = 0, t = 0;
    va_list ap;
    va_start( ap, num);
    for( i = 0; i < num; i++) {
         t = va_arg( ap, int);
         if( t > m)
            m = t;
    }
    va_end(ap);
    return m;
}

int main(int argc,char *argv[])
{
  int n, m;

  n = max( 5, 5, 6, 3, 8, 5);
  m = max( 7, 5, 1, 9, 8, 5, 7, 0);

  printf("%d\t%d\n",n,m);

  return 0;
}
{% endhighlight %}

其中函數傳參是通過棧傳遞，保存時從右至左依次入棧，以函數 `void func(int x, float y, char z)` 為例，調用該函數時 z、y、x 依次入棧，理論上來說，只要知道任意一個變量地址，以及所有變量的類型，那麼就可以通過指針移位獲取到所有的輸入變量。

`va_list` 是一個字符指針，可以理解為指向當前參數的一個指針，取參必須通過這個指針進行。

在使用時，其步驟如下：
1. 調用之前定義一個 va_list 類型的變量，一般變量名為 ap 。
 2. 通過 va_start(ap, first) 初始化 ap ，指向可變參數列表中的第一個參數，其中 first 就是 ... 之前的那個參數。
 3. 接著調用 va_arg(ap, type) 依次獲取參數，其中第二個參數為獲取參數的類型，該宏會返回指定類型的值，並指向下一個變量。
 4. 最後關閉定義的變量，實際上就是將指針賦值為 NULL 。

其中的使用關鍵是如何獲取變量的類型，通常有兩種方法：A) 提前約定好，如上面的示例；B) 通過入參判斷，如 printf() 。
 
另外，常見的用法還有獲取省略號指定的參數，例如：

{% highlight c %}
void foobar(char *str, size_t size, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(str, size, fmt, ap);
	va_end(ap);
}
{% endhighlight %}

假設，在調用上述的函數時，如果在 `_vsnprintf()` 中會再調用類似的函數，那麼可以通過 `va_list args; va_copy(args, ap);` 複製一份。

{% highlight text %}
va_list args;
va_copy(args, ap);
some_other_foobar(str, size, fmt, args);
{% endhighlight %}

## 調試

當調試時定義 DEBUG 輸出信息，通常有如下的幾種方式。

{% highlight c %}
// 常用格式
#ifdef DEBUG
	#define debug(fmt, args...) printf("debug: " fmt "\n", ##args) // OR
	#define debug(fmt, ...) printf("debug: " fmt "\n", ## __VA_ARGS__);
#else
	#define debug(fmt,args...)
#endif

// 輸出文件名、函數名、行數
#ifdef DEBUG
	#define debug(fmt, args...) printf("%s, %s, %d: " fmt , __FILE__, __FUNCTION__, __LINE__, ##args)
#else
	#define debug(fmt, args...)
#endif

// 輸出信息含有彩色
#ifdef DEBUG
   #define debug(fmt,args...)    \
      do{                        \
         printf("\033[32;40m");  \
         printf(fmt, ##args);    \
         printf("\033[0m");      \
      } while(0);
#else
   #define debug(fmt,args...)
#endif
{% endhighlight %}

另外，也可以通過如下方式判斷支持可變參數的格式。

{% highlight c %}
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
	#define _debug(...) do { printf("debug: " __VA_ARGS__); putchar('\n'); } while(0);
	#define _warn(...)  do { printf("warn : " __VA_ARGS__); putchar('\n'); } while(0);
	#define _error(...) do { printf("error: " __VA_ARGS__); putchar('\n'); } while(0);
#elif defined __GNUC__
	#define _debug(fmt, args...)  do { printf("debug: " fmt, ## args); putchar('\n'); } while(0);
	#define _warn(fmt, args...)   do { printf("warn: "  fmt, ## args); putchar('\n'); } while(0);
	#define _error(fmt, args...)  do { printf("error: " fmt, ## args); putchar('\n'); } while(0);
#endif
{% endhighlight %}


## 對齊操作

為了性能上的考慮，很多的平臺都會從某一個特定的地址開始讀取數據，比如偶地址。

數據結構中的數據變量都是按照定義的順序來定義，第一個變量的地址等同於數據結構的地址，結構體中的成員也要對齊，最後結構體也同樣需要對齊。對齊是指 **起始地址對齊**，其中對齊規則如下:

1. 數據成員對齊規則<br>結構體(struct)或聯合(union)的數據成員，第一個數據成員放在offset為0的地方，以後每個數據成員的對齊按照`#pragma pack`指定的數值n和這個數據成員自身長度中，比較小的那個進行。

2. 結構體(或聯合)的整體對齊規則<br>在數據成員完成各自對齊之後，結構體(或聯合)本身也要進行對齊，對齊將按照`#pragma pack`指定的數值n和結構體(或聯合)最大數據成員長度中，比較小的那個進行。

3. 當`#pragma pack`的n值等於或超過所有數據成員長度的時候，這個n值的大小將不生任何效果。

現舉例如下：

#### 1字節對齊

{% highlight c %}
#pragma pack(1)
struct test_t {
    int    a;   // 長度4 > 1 按1對齊；起始offset=0 0%1=0；存放位置區間[0,3]
    char   b;   // 長度1 = 1 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
    short  c;   // 長度2 > 1 按1對齊；起始offset=5 5%1=0；存放位置區間[5,6]
    char   d;   // 長度1 = 1 按1對齊；起始offset=7 7%1=0；存放位置區間[7]
};
#pragma pack()  // 取消對齊
{% endhighlight %}

{% highlight text %}
輸出結果 sizeof(struct test_t) = 8
整體對齊係數 min((max(int,short,char), 1) = 1
整體大小(size)=$(成員總大小8) 按 $(整體對齊係數) 圓整 = 8
{% endhighlight %}

#### 2字節對齊

{% highlight c %}
#pragma pack(2)
struct test_t {
    int    a;   // 長度4 > 2 按2對齊；起始offset=0 0%2=0；存放位置區間[0,3]
    char   b;   // 長度1 < 2 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
    short  c;   // 長度2 = 2 按2對齊；起始offset=6 6%2=0；存放位置區間[6,7]
    char   d;   // 長度1 < 2 按1對齊；起始offset=8 8%1=0；存放位置區間[8]
};
#pragma pack()  // 取消對齊
{% endhighlight %}

{% highlight text %}
輸出結果 sizeof(struct test_t) = 10
整體對齊係數 = min((max(int,short,char), 2) = 2
整體大小(size)=$(成員總大小9) 按 $(整體對齊係數) 圓整 = 10
{% endhighlight %}

#### 4字節對齊

{% highlight c %}
#pragma pack(4)
struct test_t {
    int    a;   // 長度4 = 4 按4對齊；起始offset=0 0%4=0；存放位置區間[0,3]
    char   b;   // 長度1 < 4 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
    short  c;   // 長度2 < 4 按2對齊；起始offset=6 6%2=0；存放位置區間[6,7]
    char   d;   // 長度1 < 4 按1對齊；起始offset=8 8%1=0；存放位置區間[8]
};
#pragma pack() // 取消對齊
{% endhighlight %}

{% highlight text %}
輸出結果 sizeof(struct test_t) = 12
整體對齊係數 = min((max(int,short,char), 4) = 4
整體大小(size)=$(成員總大小9) 按 $(整體對齊係數) 圓整 = 12
{% endhighlight %}

#### 8字節對齊

{% highlight c %}
#pragma pack(8)
struct test_t {
    int    a;   // 長度4 < 8 按4對齊；起始offset=0 0%4=0；存放位置區間[0,3]
    char   b;   // 長度1 < 8 按1對齊；起始offset=4 4%1=0；存放位置區間[4]
    short  c;   // 長度2 < 8 按2對齊；起始offset=6 6%2=0；存放位置區間[6,7]
    char   d;   // 長度1 < 8 按1對齊；起始offset=8 8%1=0；存放位置區間[8]
};
#pragma pack()  // 取消對齊
{% endhighlight %}

{% highlight text %}
輸出結果 sizeof(struct test_t) = 12
整體對齊係數 = min((max(int,short,char), 8) = 4
整體大小(size)=$(成員總大小9) 按 $(整體對齊係數) 圓整 = 12
{% endhighlight %}

### \_\_attribute((aligned(n)))

另一種方式是 `__attribute((aligned(n)))` 讓所作用的結構成員對齊在 `n` 字節自然邊界上，如果結構中有成員長度大於 `n `，則按照最大的成員的長度對齊。

示例如下：

{% highlight text %}
struct test_t {
    int    a;
    char   b;
    short  c;
    char   d;
} __attribute((aligned(n)));
{% endhighlight %}

`__attribute__((packed))` 取消編譯過程中的優化對齊，按照實際佔用字節數進行對齊。


<!--
採用宏進行對齊操作，計算a以size為倍數的上下界數，
#define alignment_down(a, size)  (a & (~(size - 1)))
#define alignment_up(a, size)    ((a + size - 1) & (~(size - 1)))
-->

詳見參考程序 [github align.c]({{ site.example_repository }}/c_cpp/c/align.c) 。

## 參數解析

`getopt()` 是採用緩衝機制，因此對於多線程編程是 **不安全** 的。

{% highlight text %}
#include <unistd.h>
// 選項的參數指針
extern char *optarg;

// 初值為1, 下次調用時，從optind位置開始檢測，通過agrv[optind]可以得到下一個參數，從而可以自行檢測
extern int optind;

// 對於不能識別的參數將輸出錯誤信息，可以將opterr設置為0，從而阻止向stderr輸出錯誤信息
extern int opterr;

// 如果選項字符不再optstring中則返回':'或'?'，並將字符保存在optopt中
extern int optopt;

int getopt(int argc, char * const argv[],const char *optstring);
描述：
  該函數處理"-"起始的參數，有些平臺可能支持"--"
參數：
  argc、argv分別為main()傳入的參數；其中optstring可以有如下的選項:
    單個字符，表示選項。
    單個字符後接一個冒號，表示該選項後必須跟一個參數，參數緊跟在選項後或者以空格隔開，該參數的指針賦給optarg。
    單個字符後跟兩個冒號，表示該選項後必須跟一個參數，參數必須緊跟在選項後不能以空格隔開，否則optarg指向為NULL，
        該參數的指針賦給optarg，這個特性是GNU的擴展。
{% endhighlight %}

如 `optstring="ab:c::d::"` ，命令行為 `getopt.exe -a -b host -ckeke -d haha`，在這個命令行參數中，`-a` `-b` 和 `-c` 是選項元素，去掉 `'-'`，a b c 就是選項。

host 是 b 的參數，keke 是 c 的參數，但 haha 並不是 d 的參數，因為它們中間有空格隔開。

**注意**：如果 optstring 中的字符串以 `'+'` 加號開頭或者環境變量 `POSIXLY_CORRE` 被設置，那麼一遇到不包含選項的命令行參數，getopt 就會停止，返回 -1；命令參數中的 `"--"` 用來強制終止掃描。

默認情況下 getopt 會重新排列命令行參數的順序，所以到最後所有不包含選項的命令行參數都排到最後，如 `getopt -a ima -b host -ckeke -d haha`，都最後命令行參數的順序是 `-a -b host -ckeke -d ima haha` 。

如果檢測到設置的參數項，則返回參數項；如果檢測完成則返回 -1；如果有不能識別的參數則將該參數保存在 optopt 中，輸出錯誤信息到 stderr，如果 optstring 以 `':'` 開頭則返回 `':'` 否則返回 `'?'`。

源碼可以參考 [github getopt.c]({{ site.example_repository }}/c_cpp/c/getopt.c) 。

### 長選項

{% highlight text %}
#include <getopt.h>
int getopt_long(int argc, char * const argv[],
    const char *optstring, const struct option *longopts, int *longindex);
int getopt_long_only(int argc, char * const argv[],
    const char *optstring, const struct option *longopts, int *longindex);

描述：
  該函數與getopt函數類似，不過可以接收長選項(通常以"--"開頭)，如果只接收長選項則optstring應該設置為""，而非NULL。
{% endhighlight %}

<!--
<font color=blue size=3><strong>參數：</strong></font><br>
    前3個參數與getopt類似，struct option定義在&lt;getopt.h&gt;中，<br>
<pre>
struct option {
    const char *name;   //name表示的是長參數名
    int has_arg; // 有3個值，
                 // no_argument(或者是0)，表示該參數後面不跟參數值
                 // required_argument(或者是1),表示該參數後面一定要跟個參數值
                 // optional_argument(或者是2),表示該參數後面可以跟，也可以不跟參數值
    int *flag; // 用來決定，getopt_long()的返回值到底是什麼。如果flag是null，
               // 則函數會返回與該項option匹配的val值；否則如果找到參數則返回0，
               // flag指向該option的val，如果沒有找到則保持不變。
    int val; // 和flag聯合決定返回值
}</pre>
    如果longindex沒有設置為空則，longindex指向longopts<br><br>
-->

源碼可以參考 [github getopt_long.c]({{ site.example_repository }}/c_cpp/c/getopt_long.c) 。

{% highlight text %}
$ getopt-long a.out -a -b -c -x foo --add --back  --check --extra=foo
{% endhighlight %}

`getopt_long_only()` 和 `getopt_long()` 類似，但是 `'-'` 和 `'--'` 均被認為是長選項，只有當 `'-'` 沒有對應的選項時才會與相應的短選項匹配。







## 整型溢出

以 8-bits 的數據為例，unsigned 取值範圍為 0~255，signed 的取值範圍為 -128~127。在計算機中數據以補碼（正數原碼與補碼相同，原碼=除符號位的補碼求反+1）的形式存在，且規定 0x80 為-128 。

### 無符號整數

對於無符號整數，當超過 255 後將會溢出，常見的是 Linux 內核中的 jiffies 變量，jiffies 以及相關的宏保存在 linux/jiffies.h 中，如果 a 發生在 b 之後則返回真，即 a>b 返回真，無論是否有溢出。

{% highlight c %}
#define time_after(a,b)     \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((long)((b) - (a)) < 0))
{% endhighlight %}

<!--
    下面以8-bit數據進行講解，在 0x00~0x7F 範圍內，表示 0~127；在 0x80~0xFF 範圍內表示 -128~-1，對於可能出現的數據有四種情況：<ol><li>
        都位於0x00~0x7F<br>
        如果a發生在b之後，則a > b。(char)b - (char)a < 0，100 - 125 = -25 < 0。</li><br><li>

        都位於0x80~0xFF<br>
        如果a發生在b之後，則a > b。(char)b - (char)a < 0，150 - 180 = -106 - (-76) = -30 < 0。</li><br><li>

        b位於0x00~0x7F，a位於0x80~0xFF<br>
        如果a發生在b之後，則a > b。(char)b - (char)a < 0，100 - 150 = 100 - (-106) = 206 = -50 < 0。<br><strong>注意，此時在a-b<=128時有效。</strong></li><br><li>

        a位於0x00~0x7F，b位於0x80~0xFF<br>
    如果a發生在b之後，此時有溢出。(char)b - (char)a < 0，150 - 10 = -106 - 10 = -116 < 0。<br><strong>注意，此時在a-b<=128時有效。</strong></li></ol>

    <div style="padding: 10pt 0pt 10pt 0pt ;" align="right">
    <table frame="void" width="90%">
        <tbody><tr><td><b>Tips:</b><br><span style="color : #009000"><font size="-1">typecheck位於相同文件夾下的typecheck.h文件中，當兩個參數不是同一個類型是將會產生警告，提醒用戶注意，只是提醒。</font></span></td>
    <td><img src="src/info.png" width="80" heigh="80"></td></tr></tbody></table></div>
    </p>
-->



<!--
<br id="Error_handling"><br><br>
<h1>錯誤處理_OK</h1>
<p>
    需要包含的頭文件及函數原型，<pre>
#include &lt;stdio.h&gt;
void perror(const char *s);

#include &lt;errno.h&gt;
const char *sys_errlist[];
int sys_nerr;  // 前兩個變量是參考了BSD，glibc中保存在&lt;stdio.h&gt;
int errno;</pre>

    如果s不為NULL且*s != '\0'則輸出s並加上": "+錯誤信息+換行，否則只輸出錯誤信息+換行。通常s應該為出錯的函數名稱，此函數需要調用errno。如果函數調用錯誤後沒有直接調用<tt>perror()</tt>，則為防止其他錯誤將其覆蓋，需要保存errno。<br><br>


sys_errlist保存了所有的錯誤信息，errno(注意出現錯誤時進行了設置，但是正確調用時可能沒有清除)為索引，最大的索引為<tt>(sys_nerr - 1)</tt>。當直接調用sys_errlist時可能錯誤信息還沒有添加。
</p>

## errno

http://www.bo56.com/linux%E4%B8%ADc%E8%AF%AD%E8%A8%80errno%E7%9A%84%E4%BD%BF%E7%94%A8/
https://www.ibm.com/developerworks/cn/aix/library/au-errnovariable/

-->



## Clang

![clang logo]({{ site.url }}/images/programs/clang_logo.png "clang logo"){: .pull-center }

Clang 是一個 C++ 編寫，基於 LLVM 的 C/C++、Objective-C 語言的輕量級編譯器，在 2013.04 開始，已經全面支持 C++11 標準。

### pragma

```#pragma``` 宏定義在本質上是聲明，常用的功能就是註釋，尤其是給 Code 分段註釋；另外，還支持處理編譯器警告。

{% highlight c %}
#pragma clang diagnostic push

//----- 方法棄用告警
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
//----- 不兼容指針類型
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
//----- 未使用變量
#pragma clang diagnostic ignored "-Wunused-variable"
//----- 無返回值
#pragma clang diagnostic ignored "-Wmissing-noreturn"
//... ...

#pragma clang diagnostic pop
{% endhighlight %}

## \_\_attribute\_\_

### \_\_attribute\_\_((format))

該屬性用於自實現的字符串格式化參數添加類似 printf() 的格式化參數的校驗，判斷需要格式化的參數與入參是否相同。

{% highlight text %}
format (archetype, string-index, first-to-check)

__attribute__((format(printf,m,n)))
__attribute__((format(scanf,m,n)))
  m : 第m個參數為格式化字符串(從1開始)；
  n : 變長參數(也即"...")的第一個參數排在總參數的第幾個；
{% endhighlight %}

如下是使用示例。

{% highlight text %}
void myprint(const char *format,...) __attribute__((format(printf,1,2)));
void myprint(int l，const char *format,...) __attribute__((format(printf,2,3)));
{% endhighlight %}

如下是一個簡單的使用示例。

{% highlight c %}
#include <stdio.h>

extern void myprint(const char *format,...) __attribute__((format(printf,1,2)));

int myprint(char *fmt, ...)
{
    int result;
    va_list args;
    va_start(args, fmt);
    fputs("foobar: ", stderr);
    result = vfprintf(stderr, fmt, args);
    va_end(args);
    return result;
}
int main(int argc, char **argv)
{
    myprint("i=%d\n",6);
    myprint("i=%s\n",6);
    myprint("i=%s\n","abc");
    myprint("%s,%d,%d\n",1,2);
 return 0;
}
{% endhighlight %}

編譯時添加 `-Wall` 就會打印 Warning 信息，如果去除，實際上不會顯示任何信息，通常可以提前發現常見的問題。

### \_\_attribute\_\_((constructor))

這是 GCC 的擴展機制，通過上述的屬性，可以使程序在開始執行或停止時調用指定的函數。

```__attribute__((constructor))``` 在 main() 之前執行，```__attribute__((destructor))``` 在 main() 執行結束之後執行。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>

static  __attribute__((constructor)) void before()
{
    printf("Hello World\n");
}

static  __attribute__((destructor)) void after()
{
    printf("Bye World!\n");
}

int main(int args,char ** argv)
{
    printf("Live...\n");
    return EXIT_SUCCESS;
}
{% endhighlight %}

如果有多個函數，可以指定優先級，其中 0~100 (含100)系統保留。在 main 之前順序為有小到大，退出時順序為由大到小。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>

static  __attribute__((constructor(102))) void before102()
{
    printf("Hello World 102\n");
}

static  __attribute__((destructor(102))) void after102()
{
    printf("Bye World! 102\n");
}

static  __attribute__((constructor(101))) void before101()
{
    printf("Hello World 101\n");
}

static  __attribute__((destructor(101))) void after101()
{
    printf("Bye World! 101\n");
}

int main(int args,char ** argv)
{
    printf("Live...\n");
    return EXIT_SUCCESS;
}
{% endhighlight %}

在使用時也可以先聲明然再定義


{% highlight c %}
#include <stdio.h>
#include <stdlib.h>

void before() __attribute__((constructor));
void after() __attribute__((destructor));

void before()
{
    printf("Hello World\n");
}

void after()
{
    printf("Bye World!\n");
}

int main(int args,char ** argv)
{
    printf("Live...\n");
    return EXIT_SUCCESS;
}
{% endhighlight %}

### \_\_attribute\_\_((visibility))

程序調用某個函數 A，而 A 函數存在於兩個動態鏈接庫 liba.so 和 libb.so 中，並且程序執行需要鏈接這兩個庫，此時程序調用的 A 函數到底是來自於 a 還是 b 呢？

這取決於鏈接時的順序，首先鏈接的庫會更新符號表，比如先鏈接 liba.so，這時候通過 liba.so 的導出符號表就可以找到函數 A 的定義，並加入到符號表中，而不會再查找 libb.so 。

也就是說，這裡的調用嚴重的依賴於鏈接庫加載的順序，可能會導致混亂。

gcc 的擴展中有如下屬性 `__attribute__ ((visibility("hidden")))` 可以用於抑制將一個函數的名稱被導出，對連接該庫的程序文件來說，該函數是不可見的，使用的方法如下：

<!--
-fvisibility=default|internal|hidden|protected
gcc的visibility是說，如果編譯的時候用了這個屬性，那麼動態庫的符號都是hidden的，除非強制聲明。
-->

#### 1. 創建一個c源文件

{% highlight c %}
#include<stdio.h>
#include<stdlib.h>

__attribute ((visibility("default"))) void not_hidden()
{
    printf("exported symbol\n");
}

void is_hidden()
{
    printf("hidden one\n");
}
{% endhighlight %}

想要做的是，第一個函數符號可以被導出，第二個被隱藏。

#### 2. 生成動態庫

先編譯成一個動態庫，使用到屬性 `-fvisibility` 。

{% highlight text %}
----- 編譯
$ gcc -shared -o libvis.so -fvisibility=hidden foobar.c

----- 查看符號鏈接
# readelf -s libvis.so |grep hidden
 7: 0000040c 20 FUNC GLOBAL DEFAULT 11 not_hidden
48: 00000420 20 FUNC LOCAL  HIDDEN  11 is_hidden
51: 0000040c 20 FUNC GLOBAL DEFAULT 11 not_hidden
{% endhighlight %}

可以看到，屬性確實有作用了。

#### 3. 編譯鏈接

現在試圖鏈接程序。

{% highlight c %}
int main()
{
    not_hidden();
    is_hidden();
    return 0;
}
{% endhighlight %}

試圖編譯成一個可執行文件，鏈接到剛才生成的動態庫。

{% highlight text %}
$ gcc -o exe main.c -L ./ -lvis
/tmp/cckYTHcl.o: In function `main':
main.c:(.text+0x17): undefined reference to `is_hidden'
{% endhighlight %}

說明瞭 hidden 確實起到作用了。

### \_\_attribute\_\_((sentinel))

該屬性表示，此可變參數函數需要一個 `NULL` 作為最後一個參數，這個 `NULL` 參數一般被叫做 "哨兵參數"。例如，有如下程序：

{% highlight c %}
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>

void foo(char *first, ...)
{
    char *p = (char *)malloc(100), *q = first;

    va_list args;
    va_start(args, first);
    while (q) {
        strcat(p, q);
        q = va_arg(args, char *);
    }
    va_end(args);

    printf("%s\n", p);

    free(p);
}

int main(void)
{
    foo("Hello", "World");

    return 0;
}
{% endhighlight %}

當通過 `gcc main.c -Wall` 進行編譯時，會發現沒有任何警告，不過很顯然，調用 `foo()` 時最後一個參數應該是個 `NULL` 以表明 "可變參數就這麼多"。

編譯完成後，如果嘗試運行則會打印一些亂碼，顯然是有問題的。

正常來說，應該通過如下方式調用 `foo("Hello", "World", NULL);`，為此，就需要用到了上述的屬性，用於表示最後一個參數需要為 `NULL` 。

{% highlight c %}
void foo(char *first, ...) __attribute__((sentinel));
{% endhighlight %}

這樣再不寫哨兵參數，在編譯時編譯器就會發出警告了。


但是，對於同樣使用可變參數的 `printf()` 來說，為什麼就不需要哨兵屬性，實際上，通過第一個參數就可以確定需要多少個參數，如下。

{% highlight c %}
/*
 * 第一個參數中規定了有兩個待打印項，所以打印時會取 "string" 和 1，多寫的 "another_string" 會被忽略。
 * printf()在被調用時明確知道此次調用需要多少個參數，所以也就無需哨兵參數的幫忙。
 */
printf("%s %d\n", "string", 1, "another_string");
{% endhighlight %}

## atexit()

很多時候我們需要在程序退出的時候做一些諸如釋放資源的操作，但程序退出的方式有很多種，比如 main() 函數運行結束、在程序的某個地方用 exit() 結束程序、用戶通過 Ctrl+C 或 Ctrl+break 操作來終止程序等等，因此需要有一種與程序退出方式無關的方法來進行程序退出時的必要處理。

方法就是用 atexit() 函數來註冊程序正常終止時要被調用的函數。

{% highlight c %}
#include <stdlib.h>
int atexit(void(*func)(void));
{% endhighlight %}

成功時返回零，失敗時返回非零。

在一個程序中至少可以用 atexit() 註冊 32 個處理函數，依賴於編譯器。這些處理函數的調用順序與其註冊的順序相反，也即最先註冊的最後調用，最後註冊的最先調用。

{% highlight c %}
void fnExit1 (void) { puts ("Exit function 1."); }
void fnExit2 (void) { puts ("Exit function 2."); }

int main ()
{
    atexit (fnExit1);
    atexit (fnExit2);
    puts ("Main function.");
    return 0;
}
{% endhighlight %}

## do-while

如果通過 ```define``` 定義一個含有多個語句的宏，通常我們使用 ```do{...} while(0);``` 進行定義，具體原因，如下詳細介紹。

如果想在宏中包含多個語句，可能會如下這樣寫。

{% highlight c %}
#define do_something() \
   do_a(); \
   do_b();
{% endhighlight %}

通常，這樣就可以用 ```do_somethin()``` 來執行一系列操作，但這樣會有個問題：如果通過如下的方式用這個宏，將會出錯。

{% highlight c %}
if (...)
   do_something();

// 宏被展開後
if (...)
   do_a();
   do_b();
{% endhighlight %}

原代碼的目的是想在 if 為真的時候執行 ```do_a()``` 和 ```do_b()```, 但現在，實際上只有 ```do_a()``` 在條件語句中，而 ```do_b()``` 任何時候都會執行。

當然這時可以通過如下的方式將那個宏改進一下。

{% highlight c %}
#define do_something() { \
   do_a(); \
   do_b(); \
}
{% endhighlight %}

然而，即使是這樣，仍然有錯。假設有一個宏是這個樣子的，

{% highlight c %}
#define do_something() { \
   if (a)     \
      do_a(); \
   else       \
      do_b(); \
{% endhighlight %}

在使用如下情況時，仍會出錯。

{% highlight c %}
if (...)
   do_something();
else {
   ...
}

// 宏展開後
if (...)
{
   if (a)
      do_a();
   else
      do_b();
}; else {
   ...
}
{% endhighlight %}

此時第二個 else 前邊會有一個分號，那麼編譯時就會出錯。

因此對於含有多條語句的宏我們使用 ```do{...} while(0);``` ，do-while 語句是需要分號來結束的，另外，現代編譯器的優化模塊能夠足夠聰明地注意到這個循環只會執行一次而將其優化掉。

綜上所述，```do{...} while(0);``` 這個技術就是為了類似的宏可以在任何時候使用。


## assert()

其作用是如果它的條件返回錯誤，則輸出錯誤信息 (包括文件名，函數名等信息)，並終止程序執行，原型定義：

{% highlight c %}
#include <assert.h>
void assert(int expression);
{% endhighlight %}

如下是一個簡單的示例。

{% highlight c %}
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE *fp;

   fp = fopen( "test.txt", "w" ); // 不存在就創建一個同名文件
   assert( fp );                  // 所以這裡不會出錯
   fclose( fp );

    fp = fopen( "noexitfile.txt", "r" );  // 不存在就打開文件失敗
    assert( fp );                         // 這裡出錯
    fclose( fp );                         // 程序不會執行到此處

    return 0;
}
{% endhighlight %}

當在 ```<assert.h>``` 之前定義 NDEBUG 時，assert 不會產生任何代碼，否則會顯示錯誤。

### 判斷程序是否有 assert

在 glibc 中，會定義如下的內容：

{% highlight c %}
#define assert(e) ((e \
    ? ((void)0) \
    :__assert_fail(#e,__FILE__,__LINE__))
{% endhighlight %}

可以通過 nm 查看程序，判斷是否存在 ```__assert_fail@@GLIBC_2.2.5``` ，如果存在該函數則說明未關閉 ```assert()``` 。

對於 autotool 可以通過如下的一種方式關閉：

1. 在 ```configure.ac``` 文件中添加 ```AC_HEADER_ASSERT``` ，然後如果關閉是添加 ```--disable-assert``` 參數，注意，一定要保證源碼包含了 ```config.h``` 頭文件；
2. 執行 configure 命令前設置環境變量，如 ```CPPFLAGS="CPPFLAGS=-DNDEBUG" ./configure```；
3. 也可以在 ```Makefile.am``` 中設置 ```AM_CPPFLAGS += -DNDEBUG``` 參數。

## backtrace

一般可以通過 gdb 的 bt 命令查看函數運行時堆棧，但是，有時為了分析程序的 BUG，可以在程序出錯時打印出函數的調用堆棧。

在 glibc 頭文件 `execinfo.h` 中聲明瞭三個函數用於獲取當前線程的函數調用堆棧。

{% highlight text %}
int backtrace(void **buffer,int size);
    用於獲取當前線程的調用堆棧，獲取的信息將會被存放在buffer中，它是一個指針列表。參數size用來指
    定buffer中可以保存多少個void*元素，該函數返回值是實際獲取的指針個數，最大不超過size大小；

char **backtrace_symbols(void *const *buffer, int size);
    將從上述函數獲取的信息轉化為字符串數組，參數buffer應該是從backtrace()獲取的指針數組，size是該
    數組中的元素個數，也就是backtrace()的返回值。
    函數返回值是一個指向字符串數組的指針，它的大小同buffer相同，每個字符串包含了一個相對於buffer中
    對應元素的可打印信息，包括函數名、函數的偏移地址和實際的返回地址。

void backtrace_symbols_fd(void *const *buffer, int size, int fd);
    與上述函數相同，只是將結果寫入文件描述符為fd的文件中，每個函數對應一行。
{% endhighlight %}

注意，需要傳遞相應的符號給鏈接器以能支持函數名功能，比如，在使用 GNU ld 鏈接器的時需要傳遞 `-rdynamic` 參數，該參數用來通知鏈接器將所有符號添加到動態符號表中。

下面是 glibc 中的實例。

{% highlight c %}
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 100

void myfunc3(void)
{
    int j, nptrs;
    void *buffer[100];
    char **strings;

    nptrs = backtrace(buffer, SIZE);
    printf("backtrace() returned %d addresses\n", nptrs);

    /*
     * The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
     * would produce similar output to the following:
     */
    strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    for (j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

    free(strings);
}

static void myfunc2(void) /* "static" means don't export the symbol... */
{
    myfunc3();
}

void myfunc(int ncalls)
{
    if (ncalls > 1)
        myfunc(ncalls - 1);
    else
        myfunc2();
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "%s num-calls\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    myfunc(atoi(argv[1]));
    exit(EXIT_SUCCESS);
}
{% endhighlight %}

然後通過如下方式編譯，執行。

{% highlight text %}
$ cc -rdynamic prog.c -o prog
$ ./prog 2
backtrace() returned 7 addresses
./prog(myfunc3+0x1f) [0x400a7c]
./prog() [0x400b11]
./prog(myfunc+0x25) [0x400b38]
./prog(myfunc+0x1e) [0x400b31]
./prog(main+0x59) [0x400b93]
/lib64/libc.so.6(__libc_start_main+0xf5) [0x7f727d449b35]
./prog() [0x400999]
{% endhighlight %}

還可以利用 backtrace 來定位段錯誤位置。

<!--
通常情況系，程序發生段錯誤時系統會發送SIGSEGV信號給程序，缺省處理是退出函數。我們可以使用 signal(SIGSEGV, &your_function);函數來接管SIGSEGV信號的處理，程序在發生段錯誤後，自動調用我們準備好的函數，從而在那個函數裡來獲取當前函數調用棧。

舉例如下：

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <execinfo.h>
#include <signal.h>

void dump(int signo)
{
 void *buffer[30] = {0};
 size_t size;
 char **strings = NULL;
 size_t i = 0;

 size = backtrace(buffer, 30);
 fprintf(stdout, "Obtained %zd stack frames.nm\n", size);
 strings = backtrace_symbols(buffer, size);
 if (strings == NULL)
 {
  perror("backtrace_symbols.");
  exit(EXIT_FAILURE);
 }

 for (i = 0; i < size; i++)
 {
  fprintf(stdout, "%s\n", strings[i]);
 }
 free(strings);
 strings = NULL;
 exit(0);
}

void func_c()
{
 *((volatile char *)0x0) = 0x9999;
}

void func_b()
{
 func_c();
}

void func_a()
{
 func_b();
}

int main(int argc, const char *argv[])
{
 if (signal(SIGSEGV, dump) == SIG_ERR)
  perror("can't catch SIGSEGV");
 func_a();
 return 0;
}

編譯程序：
gcc -g -rdynamic test.c -o test; ./test
輸出如下：

Obtained6stackframes.nm
./backstrace_debug(dump+0x45)[0x80487c9]
[0x468400]
./backstrace_debug(func_b+0x8)[0x804888c]
./backstrace_debug(func_a+0x8)[0x8048896]
./backstrace_debug(main+0x33)[0x80488cb]
/lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x129113]

接著：
objdump -d test > test.s
在test.s中搜索804888c如下：

8048884 <func_b>:
8048884: 55          push %ebp
8048885: 89 e5      mov %esp, %ebp
8048887: e8 eb ff ff ff      call 8048877 <func_c>
804888c: 5d            pop %ebp
804888d: c3            ret

其中80488c時調用（call 8048877）C函數後的地址，雖然並沒有直接定位到C函數，通過彙編代碼， 基本可以推出是C函數出問題了（pop指令不會導致段錯誤的）。
我們也可以通過addr2line來查看

addr2line 0x804888c -e backstrace_debug -f

輸出：

func_b
/home/astrol/c/backstrace_debug.c:57
-->





<!--
## 宏定義
http://hbprotoss.github.io/posts/cyu-yan-hong-de-te-shu-yong-fa-he-ji-ge-keng.html
http://gcc.gnu.org/onlinedocs/cpp/Macros.html
-->

## 其它

### is not a symbolic link

正常情況下，類似庫 ```libxerces-c-3.0.so``` 應該是個符號鏈接，而不是實體文件，對於這種情況只需要修改其為符號鏈接即可。

{% highlight text %}
# mv libxerces-c-3.0.so libxerces-c.so.3.0
# ln -s libxerces-c.so.3.0 libxerces-c-3.0.so
{% endhighlight %}

### 結構體初始化

對於 C 中結構體初始化可以通過如下設置。

{% highlight c %}
#include <stdio.h>

struct foobar {
        int foo;
        struct a {
                int type;
                int value;
        } *array;
        int length;
};

int main(int argc, char **argv)
{
        int i = 0;

        struct foobar f = {
                .foo = 1,
                .length = 3,
                .array = (struct a[]){
                        {.type = 1, .value = 2},
                        {.type = 1, .value = 3},
                        {.type = 1, .value = 3}
                }
        };

        for (i = 0; i < f.length; i++)
                printf(">>>> %d %d\n", i, f.array[i].type);

        return 0;
}
{% endhighlight %}

### 結構體地址定位

通過結構體可以將多種不同類型的對象聚合到一個對象中，編譯器會按照成員列表順序分配內存，不過由於內存對齊機制不同，導致不同架構有所區別，所以各個成員之間可能會有間隙，所以不能簡單的通過成員類型所佔的字長來推斷其它成員或結構體對象的地址。

假設有如下的一個鏈表。

{% highlight text %}
typedef struct list_node {
	int ivar;
	char cvar;
	double dvar;
	struct list_node *next;
} list_node;
{% endhighlight %}

當已知一個變量的地址時，如何獲取到某個成員的偏移量，Linux 內核中的實現如下。

{% highlight c %}
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
{% endhighlight %}

當知道了成員偏移量，那麼就可以通過結構體成員的地址，反向求結構體的地址，如下。

{% highlight c %}
#define container_of(ptr, type, member) ({
	const typeof(((type *)0)->member ) *__mptr = (ptr);
	(type *)((char *)__mptr - offsetof(type,member));
	})
{% endhighlight %}

現在很多的動態語言是可以支持動態獲取變量類型的，其中 GCC 提供了 `typeof` 關鍵字，所不同的是這個只在預編譯時，最後實際轉化為數據類型被編譯器處理。基本用法是這樣的：

{% highlight c %}
int a;
typeof(a)  b; // int b;
typeof(&a) c; // int* c;
{% endhighlight %}

如上的宏定義中， ptr 代表已知成員的地址，type 代表結構體的類型，member 代表已知的成員。

### 指針參數修改

一個比較容易犯錯的地方，願意是在 `foobar()` 函數內修改 `main()` 中的 v 指向的變量，其中後者實際上是修改的本地棧中保存的臨時版本。

{% highlight c %}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct value_s {
  union {
    char *string;
  } value;
} value_t;

#if 1
void foobar ( value_t *t )
{
  char **v = &t->value.string;
  printf("foobar %p %s\n", *v, *v);
  *v = "yang";
  printf("foobar %p %s\n", *v, *v);
}
#else
void foobar ( value_t *t )
{
  value_t v = *t;
  printf("foobar %p %s\n", v.value.string, v.value.string);
  v.value.string = "yang";
  printf("foobar %p %s\n", v.value.string, v.value.string);
}
#endif

int main()
{
  value_t v;
  v.value.string = "jin";
  printf("       %p %s\n", v.value.string, v.value.string);
  foobar( &v );
  printf("       %p %s\n", v.value.string, v.value.string);

  return(0);
}
{% endhighlight %}

### 取固定大小的變量

Linux 每個數據類型的大小可以在 `sys/types.h` 中查看

{% highlight c %}
#include <sys/types.h>
typedef    int8_t            S8;
typedef    int16_t           S16;
typedef    int32_t           S32;
typedef    int64_t           S64;

typedef    u_int8_t          U8;
typedef    u_int16_t         U16;
typedef    u_int32_t         U32;
typedef    u_int64_t         U64;
{% endhighlight %}

### 環境變量

簡單介紹下 C 中，如何獲取以及設置環境變量。

其中設置環境變量方法包括了 `putenv()` 以及 `setenv()` 兩種，前者必須是 `Key=Value` 這種格式，後者則以參數形式傳遞。

對於 `putenv()` 如果環境變量已經存在則替換，而 `setenv()` 則可以設置是否覆蓋 。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char *p;

	if((p = getenv("USER")))
		printf("USER = %s\n", p);

	setenv("USER", "test", 1);
	printf("USER = %s\n", getenv("USER"));

	putenv("NAME=foobar");
	printf("NAME = %s\n", getenv("NAME"));

	unsetenv("USER");
	printf("USER = %s\n", getenv("USER"));
}
{% endhighlight %}


### 其它

#### FLT_RADIX

C 語言標準庫 `float.h` 中的 `FLT_RADIX` 常數用於定義指數的基數，也就是以這個數為底的多少次方。

{% highlight text %}
FLT_RADIX 10     10 的多少次方
FLT_RADIX 2       2 的多少次方
{% endhighlight %}

例如：

{% highlight text %}
#define FLT_MAX_EXP 128
#define FLT_RADIX   2
{% endhighlight %}

意思是 float 型，最大指數是 128，它的底是 2，也就說最大指數是 2 的 128 方。

<!--
FLT 是 float 的 縮寫。( DBL 是 double 的 縮寫。)
-->

#### implicit declaration

按常規來講，出現 `implicit declaration of function 'xxxx'` 是因為頭文件未包含導致的！

這裡是由於 `nanosleep()` 函數的報錯，而實際上 `time.h` 頭文件已經包含了，後來才發現原來是在 `Makefile` 中添加了 `-std=c99` 導致，可以通過 `-std=gnu99` 替換即可。

另外，不能定義 `-D_POSIX_SOURCE` 宏。



## 參考

[Schemaless Benchmarking Suite](https://github.com/ludocode/schemaless-benchmarks)



{% highlight text %}
{% endhighlight %}
