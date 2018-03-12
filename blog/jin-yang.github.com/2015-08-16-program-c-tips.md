---
title: C 语言的奇技淫巧
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,c,cpp,c++,program
description: 整理下 C 语言中常用的技巧。变长数组。
---

整理下 C 语言中常用的技巧。

<!-- more -->

## 变长数组

实际编程中，经常会使用变长数组，但是 C 语言并不支持变长的数组，可以使用结构体实现。

类似如下的结构体，其中 value 成员变量不占用内存空间，也可以使用 ```char value[]``` ，但是不要使用 ``` char *value```，该变量会占用指针对应的空间。

常见的操作示例如下。

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
    // 两者占用的存储空间相同，也就是value不占用空间
    printf("%li %li\n", sizeof(int), sizeof(foobar_t));

    // 初始化
    int *values = (int *)malloc(10*sizeof(int)), i, j, *ptr;
    for (i = 0; i < 10; i++)
      values[i] = 10*i;
    for (i = 0; i < 10; i++)
      printf(" %i", values[i]);
    printf("\n");

    // 针对单个结构体的操作
    foobar_t *buff = (foobar_t *)malloc(sizeof(foobar_t) + 10*sizeof(int));
    buff->len = 10;
    memcpy(buff->value, values, 10*sizeof(int));
    ptr = (int *)buff->value;

    printf("length: %i, vlaues:", buff->len);
    for (i = 0; i < 10; i++)
      printf(" %i", ptr[i]);
    printf("\n");
    free(buff);

    // 针对数组的操作
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

`qsort()` 会根据给出的比较函数进行快排，通过指针移动实现排序，时间复杂度为 `n*log(n)`，排序之后的结果仍然放在原数组中，不保证排序稳定性，如下是其声明。

{% highlight text %}
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void qsort_r(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *, void *), void *arg);
    base: 数组起始地址；
    nmemb: 数组元素个数；
    size: 每个元素的大小；
    compar: 函数指针，指向定义的比较函数，当elem1>elem2返回正数，此时不交换。
{% endhighlight %}

通常可以对整数、字符串、结构体进行排序，如下是常用示例。

{% highlight text %}
----- 对int类型数组排序
int num[100];
int cmp(const void *a , const void *b)
{
	return *(int *)a - *(int *)b;
}
qsort(num, sizeof(num)/sizeof(num[0]), sizeof(num[0]), cmp);

----- 对结构体进行排序
struct foobar {
	int data;
	char string[10];
} s[100]
int cmp_int(const void *a, const void *b) /* 按照data递增排序 */
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

## 指针

指针或许是 C 语言中最复杂的东西了。

### 指针常量 VS. 常量指针

前面是一个修饰词，后面的是中心词。

#### 常量指针

**常量指针** 首先是一个指针，指向的是常量，即指向常量的指针；可以通过如下的方式定义：

{% highlight c %}
const int a = 7;
const int *p = &a;
{% endhighlight %}

对于常量，我们不能对其内容进行修改；指针的内容本身是一个地址，通过常量指针指向一个常量，为的就是防止我们写程序过程中对指针误操作出现了修改常量这样的错误，如果我们修改常量指针的所指向的空间的时候，编译系统就会提示我们出错信息。

在 C 语言中，通常定义的字符串会返回一个常量指针，因此字符串不能赋值给字符数组，只能赋值到指针。

总结一下，<font color="red">常量指针就是指向常量的指针，指针所指向的地址的内容是不可修改的，指针本身的内容是可以修改的</font> 。


#### 指针常量

**指针常量**  首先是一个常量，再才是一个指针；可以通过如下的方式定义：

{% highlight c %}
int a = 7;
int * const p = &a; // OR int const *p = &a;
{% endhighlight %}

常量的性质是不能修改，指针的内容实际是一个地址，那么指针常量就是内容不能修改的常量，即内容不能修改的指针，指针的内容是什么呀？指针的内容是地址，所以，说到底，就是不能修改这个指针所指向的地址，一开始初始化，指向哪儿，它就只能指向哪儿了，不能指向其他的地方了，就像一个数组的数组名一样，是一个固定的指针，不能对它移动操作。

它只是不能修改它指向的地方，但这个指向的地方里的内容是可以替换的，这和上面说的常量指针是完全不同的概念。

作一下总结，<font color="red">指针常量就是是指针的常量，它是不可改变地址的指针，但是可以对它所指向的内容进行修改</font> 。

源码可以参考 [github const_pointer.c]({{ site.example_repository }}/c_cpp/c/const_pointer.c) 。

### 与一维数组

假设有如下数组，

{% highlight c %}
int Array[] = {1, 2, 3, 4};
int *ptr = Array;
{% endhighlight %}

其中 Array 为指针常量，而 ptr 为指针变量，且 ```ptr = &Array[0]```，那么如下的操作相同 ```ptr[i] <=> *(ptr+i)``` 以及 ```Array[i] <=> *(Array + i)``` 。

如下，简单介绍下常见操作。

#### *ptr++

由于 ```*``` 和 ```++``` 优先级相同，结合性为由右至左，即 ```*ptr++``` 等价于 ```*(ptr++)``` ，由于 ```++``` 为后继加，所以当得到 ```*ptr``` 后再处理 ```++```；所以 ```*ptr++``` 等于 1，进行此项操作后 ```*ptr``` 等于 2。

执行的步骤为 1) ```++``` 操作符产生 ptr 的一份拷贝；2) ```++``` 操作符增加 ptr 的值；3) 在 ptr 上执行间接访问操作。

#### ++*ptr

利用优先级和结合性可得，```++*ptr``` 等价于 ```++(*ptr)``` ，此时 ```Array[0]``` 为 2，返回 2 。

#### *ptr++

利用优先级和结合性可得，```*ptr++``` 等价于 ```*(ptr++)``` ，返回 1，ptr 值加 1 。

## 大小端

当数据类型大于一个字节时，其所占用的字节在内存中的顺序存在两种模式：小端模式 (little endian) 和大端模式 (big endian)，其中 MSB(Most Significant Bit) 最高有效位，LSB(Least Significant Bit) 最低有效位.

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

如下是一个测试程序。

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

如果采用大端模式，则在向某一个函数通过向下类型装换来传递参数时可能会出错。如一个变量为 ```int i=1;``` 经过函数 ```void foo(short *j);``` 的调用，即 ```foo((short*)&i);```，在 foo() 中将 i 修改为 3 则最后得到的 i 为 0x301 。

大端模式规定 MSB 在存储时放在低地址，在传输时 MSB 放在流的开始；小段模式反之。

## 变参传递

头文件 `stdarg.h` 中对相关的宏进行了定义，其基本内容如下所示：

{% highlight c %}
typedef char * va_list;

#define _INTSIZEOF(n)       ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#define va_start(arg_ptr,v) (arg_ptr = (va_list)&v + _INTSIZEOF(v))
#define va_arg(arg_ptr,t)   (*(t *)((arg_ptr += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(arg_ptr)     (arg_ptr = (va_list)0)
{% endhighlight %}

<!--
_INTSIZEOF(n)：我们可以经过测试得到，如果在上面的测试程序中用一个double类型（长度为8byte）和一个 long double类型（长度为10byte）做可变参数。我们发现double类型占了8byte,而long double占了12byte，即都是4的整数倍。这里是因为对齐，即对Intel80x86 机器来说就是要求每个变量的地址都是sizeof(int)的倍数。char类型的参数只占了1byte，但是它后面的参数因为对齐的关系只能跳过3byte存储，而那3byte也就浪费掉了。<br><br>

<font color=blue><strong>va_start(arg_ptr,v)</strong></font>：使参数列表指针arg_ptr指向函数参数列表中的第一个可选参数。v是位于第一个可选参数之前的固定参数，如果有一函数的声明是void va_test(char a, char b, char c, …)，则它的固定参数依次是a,b,c，最后一个固定参数v为c，因此就是va_start(arg_ptr, c)。<br><br>

<font color=blue><strong>va_arg(arg_ptr, t)</strong></font>：返回参数列表中指针arg_ptr所指的参数，返回类型为t，并使指针arg_ptr指向参数列表中下一个参数。首先计算(arg_ptr += _INTSIZEOF(t))，此时arg_ptr将指向下一个参数；((arg_ptr += _INTSIZEOF(t)) - _INTSIZEOF(t))等价于arg_ptr，通过(t *)将arg_ptr转换为t类型的指针，并通过*取其值。<br><br>

<font color=blue><strong>va_end(arg_ptr)</strong></font>：使ap不再指向堆栈,而是跟NULL一样。<br><br>
-->

示例如下图所示：

{% highlight c %}
#include <stdarg.h>
#include <stdio.h>

int max( int num,...)
{
    int m = -0x7FFFFFFF; /* 32系统中最小的整数 */
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

其中函数传参是通过栈传递，保存时从右至左依次入栈，以函数 `void func(int x, float y, char z)` 为例，调用该函数时 z、y、x 依次入栈，理论上来说，只要知道任意一个变量地址，以及所有变量的类型，那么就可以通过指针移位获取到所有的输入变量。

`va_list` 是一个字符指针，可以理解为指向当前参数的一个指针，取参必须通过这个指针进行。

在使用时，其步骤如下：
1. 调用之前定义一个 va_list 类型的变量，一般变量名为 ap 。
 2. 通过 va_start(ap, first) 初始化 ap ，指向可变参数列表中的第一个参数，其中 first 就是 ... 之前的那个参数。
 3. 接着调用 va_arg(ap, type) 依次获取参数，其中第二个参数为获取参数的类型，该宏会返回指定类型的值，并指向下一个变量。
 4. 最后关闭定义的变量，实际上就是将指针赋值为 NULL 。

其中的使用关键是如何获取变量的类型，通常有两种方法：A) 提前约定好，如上面的示例；B) 通过入参判断，如 printf() 。
 
另外，常见的用法还有获取省略号指定的参数，例如：

{% highlight c %}
void foobar(char *str, size_t size, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_vsnprintf(str, size, fmt, ap);
	va_end(ap);
}
{% endhighlight %}

假设，在调用上述的函数时，如果在 `_vsnprintf()` 中会再调用类似的函数，那么可以通过 `va_list args; va_copy(args, ap);` 复制一份。

{% highlight text %}
va_list args;
va_copy(args, ap);
some_other_foobar(str, size, fmt, args);
{% endhighlight %}

## 调试

当调试时定义 DEBUG 输出信息，通常有如下的几种方式。

{% highlight c %}
// 常用格式
#ifdef DEBUG
	#define debug(fmt, args...) printf("debug: " fmt "\n", ##args) // OR
	#define debug(fmt, ...) printf("debug: " fmt "\n", ## __VA_ARGS__);
#else
	#define debug(fmt,args...)
#endif

// 输出文件名、函数名、行数
#ifdef DEBUG
	#define debug(fmt, args...) printf("%s, %s, %d: " fmt , __FILE__, __FUNCTION__, __LINE__, ##args)
#else
	#define debug(fmt, args...)
#endif

// 输出信息含有彩色
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

另外，也可以通过如下方式判断支持可变参数的格式。

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


## 对齐操作

为了性能上的考虑，很多的平台都会从某一个特定的地址开始读取数据，比如偶地址。

数据结构中的数据变量都是按照定义的顺序来定义，第一个变量的地址等同于数据结构的地址，结构体中的成员也要对齐，最后结构体也同样需要对齐。对齐是指 **起始地址对齐**，其中对齐规则如下:

1. 数据成员对齐规则<br>结构体(struct)或联合(union)的数据成员，第一个数据成员放在offset为0的地方，以后每个数据成员的对齐按照`#pragma pack`指定的数值n和这个数据成员自身长度中，比较小的那个进行。

2. 结构体(或联合)的整体对齐规则<br>在数据成员完成各自对齐之后，结构体(或联合)本身也要进行对齐，对齐将按照`#pragma pack`指定的数值n和结构体(或联合)最大数据成员长度中，比较小的那个进行。

3. 当`#pragma pack`的n值等于或超过所有数据成员长度的时候，这个n值的大小将不生任何效果。

现举例如下：

#### 1字节对齐

{% highlight c %}
#pragma pack(1)
struct test_t {
    int    a;   // 长度4 > 1 按1对齐；起始offset=0 0%1=0；存放位置区间[0,3]
    char   b;   // 长度1 = 1 按1对齐；起始offset=4 4%1=0；存放位置区间[4]
    short  c;   // 长度2 > 1 按1对齐；起始offset=5 5%1=0；存放位置区间[5,6]
    char   d;   // 长度1 = 1 按1对齐；起始offset=7 7%1=0；存放位置区间[7]
};
#pragma pack()  // 取消对齐
{% endhighlight %}

{% highlight text %}
输出结果 sizeof(struct test_t) = 8
整体对齐系数 min((max(int,short,char), 1) = 1
整体大小(size)=$(成员总大小8) 按 $(整体对齐系数) 圆整 = 8
{% endhighlight %}

#### 2字节对齐

{% highlight c %}
#pragma pack(2)
struct test_t {
    int    a;   // 长度4 > 2 按2对齐；起始offset=0 0%2=0；存放位置区间[0,3]
    char   b;   // 长度1 < 2 按1对齐；起始offset=4 4%1=0；存放位置区间[4]
    short  c;   // 长度2 = 2 按2对齐；起始offset=6 6%2=0；存放位置区间[6,7]
    char   d;   // 长度1 < 2 按1对齐；起始offset=8 8%1=0；存放位置区间[8]
};
#pragma pack()  // 取消对齐
{% endhighlight %}

{% highlight text %}
输出结果 sizeof(struct test_t) = 10
整体对齐系数 = min((max(int,short,char), 2) = 2
整体大小(size)=$(成员总大小9) 按 $(整体对齐系数) 圆整 = 10
{% endhighlight %}

#### 4字节对齐

{% highlight c %}
#pragma pack(4)
struct test_t {
    int    a;   // 长度4 = 4 按4对齐；起始offset=0 0%4=0；存放位置区间[0,3]
    char   b;   // 长度1 < 4 按1对齐；起始offset=4 4%1=0；存放位置区间[4]
    short  c;   // 长度2 < 4 按2对齐；起始offset=6 6%2=0；存放位置区间[6,7]
    char   d;   // 长度1 < 4 按1对齐；起始offset=8 8%1=0；存放位置区间[8]
};
#pragma pack() // 取消对齐
{% endhighlight %}

{% highlight text %}
输出结果 sizeof(struct test_t) = 12
整体对齐系数 = min((max(int,short,char), 4) = 4
整体大小(size)=$(成员总大小9) 按 $(整体对齐系数) 圆整 = 12
{% endhighlight %}

#### 8字节对齐

{% highlight c %}
#pragma pack(8)
struct test_t {
    int    a;   // 长度4 < 8 按4对齐；起始offset=0 0%4=0；存放位置区间[0,3]
    char   b;   // 长度1 < 8 按1对齐；起始offset=4 4%1=0；存放位置区间[4]
    short  c;   // 长度2 < 8 按2对齐；起始offset=6 6%2=0；存放位置区间[6,7]
    char   d;   // 长度1 < 8 按1对齐；起始offset=8 8%1=0；存放位置区间[8]
};
#pragma pack()  // 取消对齐
{% endhighlight %}

{% highlight text %}
输出结果 sizeof(struct test_t) = 12
整体对齐系数 = min((max(int,short,char), 8) = 4
整体大小(size)=$(成员总大小9) 按 $(整体对齐系数) 圆整 = 12
{% endhighlight %}

### \_\_attribute((aligned(n)))

另一种方式是 `__attribute((aligned(n)))` 让所作用的结构成员对齐在 `n` 字节自然边界上，如果结构中有成员长度大于 `n `，则按照最大的成员的长度对齐。

示例如下：

{% highlight text %}
struct test_t {
    int    a;
    char   b;
    short  c;
    char   d;
} __attribute((aligned(n)));
{% endhighlight %}

`__attribute__((packed))` 取消编译过程中的优化对齐，按照实际占用字节数进行对齐。


<!--
采用宏进行对齐操作，计算a以size为倍数的上下界数，
#define alignment_down(a, size)  (a & (~(size - 1)))
#define alignment_up(a, size)    ((a + size - 1) & (~(size - 1)))
-->

详见参考程序 [github align.c]({{ site.example_repository }}/c_cpp/c/align.c) 。

## 参数解析

`getopt()` 是采用缓冲机制，因此对于多线程编程是 **不安全** 的。

{% highlight text %}
#include <unistd.h>
// 选项的参数指针
extern char *optarg;

// 初值为1, 下次调用时，从optind位置开始检测，通过agrv[optind]可以得到下一个参数，从而可以自行检测
extern int optind;

// 对于不能识别的参数将输出错误信息，可以将opterr设置为0，从而阻止向stderr输出错误信息
extern int opterr;

// 如果选项字符不再optstring中则返回':'或'?'，并将字符保存在optopt中
extern int optopt;

int getopt(int argc, char * const argv[],const char *optstring);
描述：
  该函数处理"-"起始的参数，有些平台可能支持"--"
参数：
  argc、argv分别为main()传入的参数；其中optstring可以有如下的选项:
    单个字符，表示选项。
    单个字符后接一个冒号，表示该选项后必须跟一个参数，参数紧跟在选项后或者以空格隔开，该参数的指针赋给optarg。
    单个字符后跟两个冒号，表示该选项后必须跟一个参数，参数必须紧跟在选项后不能以空格隔开，否则optarg指向为NULL，
        该参数的指针赋给optarg，这个特性是GNU的扩展。
{% endhighlight %}

如 `optstring="ab:c::d::"` ，命令行为 `getopt.exe -a -b host -ckeke -d haha`，在这个命令行参数中，`-a` `-b` 和 `-c` 是选项元素，去掉 `'-'`，a b c 就是选项。

host 是 b 的参数，keke 是 c 的参数，但 haha 并不是 d 的参数，因为它们中间有空格隔开。

**注意**：如果 optstring 中的字符串以 `'+'` 加号开头或者环境变量 `POSIXLY_CORRE` 被设置，那么一遇到不包含选项的命令行参数，getopt 就会停止，返回 -1；命令参数中的 `"--"` 用来强制终止扫描。

默认情况下 getopt 会重新排列命令行参数的顺序，所以到最后所有不包含选项的命令行参数都排到最后，如 `getopt -a ima -b host -ckeke -d haha`，都最后命令行参数的顺序是 `-a -b host -ckeke -d ima haha` 。

如果检测到设置的参数项，则返回参数项；如果检测完成则返回 -1；如果有不能识别的参数则将该参数保存在 optopt 中，输出错误信息到 stderr，如果 optstring 以 `':'` 开头则返回 `':'` 否则返回 `'?'`。

源码可以参考 [github getopt.c]({{ site.example_repository }}/c_cpp/c/getopt.c) 。

### 长选项

{% highlight text %}
#include <getopt.h>
int getopt_long(int argc, char * const argv[],
    const char *optstring, const struct option *longopts, int *longindex);
int getopt_long_only(int argc, char * const argv[],
    const char *optstring, const struct option *longopts, int *longindex);

描述：
  该函数与getopt函数类似，不过可以接收长选项(通常以"--"开头)，如果只接收长选项则optstring应该设置为""，而非NULL。
{% endhighlight %}

<!--
<font color=blue size=3><strong>参数：</strong></font><br>
    前3个参数与getopt类似，struct option定义在&lt;getopt.h&gt;中，<br>
<pre>
struct option {
    const char *name;   //name表示的是长参数名
    int has_arg; // 有3个值，
                 // no_argument(或者是0)，表示该参数后面不跟参数值
                 // required_argument(或者是1),表示该参数后面一定要跟个参数值
                 // optional_argument(或者是2),表示该参数后面可以跟，也可以不跟参数值
    int *flag; // 用来决定，getopt_long()的返回值到底是什么。如果flag是null，
               // 则函数会返回与该项option匹配的val值；否则如果找到参数则返回0，
               // flag指向该option的val，如果没有找到则保持不变。
    int val; // 和flag联合决定返回值
}</pre>
    如果longindex没有设置为空则，longindex指向longopts<br><br>
-->

源码可以参考 [github getopt_long.c]({{ site.example_repository }}/c_cpp/c/getopt_long.c) 。

{% highlight text %}
$ getopt-long a.out -a -b -c -x foo --add --back  --check --extra=foo
{% endhighlight %}

`getopt_long_only()` 和 `getopt_long()` 类似，但是 `'-'` 和 `'--'` 均被认为是长选项，只有当 `'-'` 没有对应的选项时才会与相应的短选项匹配。







## 整型溢出

以 8-bits 的数据为例，unsigned 取值范围为 0~255，signed 的取值范围为 -128~127。在计算机中数据以补码（正数原码与补码相同，原码=除符号位的补码求反+1）的形式存在，且规定 0x80 为-128 。

### 无符号整数

对于无符号整数，当超过 255 后将会溢出，常见的是 Linux 内核中的 jiffies 变量，jiffies 以及相关的宏保存在 linux/jiffies.h 中，如果 a 发生在 b 之后则返回真，即 a>b 返回真，无论是否有溢出。

{% highlight c %}
#define time_after(a,b)     \
    (typecheck(unsigned long, a) && \
     typecheck(unsigned long, b) && \
     ((long)((b) - (a)) < 0))
{% endhighlight %}

<!--
    下面以8-bit数据进行讲解，在 0x00~0x7F 范围内，表示 0~127；在 0x80~0xFF 范围内表示 -128~-1，对于可能出现的数据有四种情况：<ol><li>
        都位于0x00~0x7F<br>
        如果a发生在b之后，则a > b。(char)b - (char)a < 0，100 - 125 = -25 < 0。</li><br><li>

        都位于0x80~0xFF<br>
        如果a发生在b之后，则a > b。(char)b - (char)a < 0，150 - 180 = -106 - (-76) = -30 < 0。</li><br><li>

        b位于0x00~0x7F，a位于0x80~0xFF<br>
        如果a发生在b之后，则a > b。(char)b - (char)a < 0，100 - 150 = 100 - (-106) = 206 = -50 < 0。<br><strong>注意，此时在a-b<=128时有效。</strong></li><br><li>

        a位于0x00~0x7F，b位于0x80~0xFF<br>
    如果a发生在b之后，此时有溢出。(char)b - (char)a < 0，150 - 10 = -106 - 10 = -116 < 0。<br><strong>注意，此时在a-b<=128时有效。</strong></li></ol>

    <div style="padding: 10pt 0pt 10pt 0pt ;" align="right">
    <table frame="void" width="90%">
        <tbody><tr><td><b>Tips:</b><br><span style="color : #009000"><font size="-1">typecheck位于相同文件夹下的typecheck.h文件中，当两个参数不是同一个类型是将会产生警告，提醒用户注意，只是提醒。</font></span></td>
    <td><img src="src/info.png" width="80" heigh="80"></td></tr></tbody></table></div>
    </p>
-->



<!--
<br id="Error_handling"><br><br>
<h1>错误处理_OK</h1>
<p>
    需要包含的头文件及函数原型，<pre>
#include &lt;stdio.h&gt;
void perror(const char *s);

#include &lt;errno.h&gt;
const char *sys_errlist[];
int sys_nerr;  // 前两个变量是参考了BSD，glibc中保存在&lt;stdio.h&gt;
int errno;</pre>

    如果s不为NULL且*s != '\0'则输出s并加上": "+错误信息+换行，否则只输出错误信息+换行。通常s应该为出错的函数名称，此函数需要调用errno。如果函数调用错误后没有直接调用<tt>perror()</tt>，则为防止其他错误将其覆盖，需要保存errno。<br><br>


sys_errlist保存了所有的错误信息，errno(注意出现错误时进行了设置，但是正确调用时可能没有清除)为索引，最大的索引为<tt>(sys_nerr - 1)</tt>。当直接调用sys_errlist时可能错误信息还没有添加。
</p>

## errno

http://www.bo56.com/linux%E4%B8%ADc%E8%AF%AD%E8%A8%80errno%E7%9A%84%E4%BD%BF%E7%94%A8/
https://www.ibm.com/developerworks/cn/aix/library/au-errnovariable/

-->



## Clang

![clang logo]({{ site.url }}/images/programs/clang_logo.png "clang logo"){: .pull-center }

Clang 是一个 C++ 编写，基于 LLVM 的 C/C++、Objective-C 语言的轻量级编译器，在 2013.04 开始，已经全面支持 C++11 标准。

### pragma

```#pragma``` 宏定义在本质上是声明，常用的功能就是注释，尤其是给 Code 分段注释；另外，还支持处理编译器警告。

{% highlight c %}
#pragma clang diagnostic push

//----- 方法弃用告警
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
//----- 不兼容指针类型
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
//----- 未使用变量
#pragma clang diagnostic ignored "-Wunused-variable"
//----- 无返回值
#pragma clang diagnostic ignored "-Wmissing-noreturn"
//... ...

#pragma clang diagnostic pop
{% endhighlight %}

## \_\_attribute\_\_

### \_\_attribute\_\_((format))

该属性用于自实现的字符串格式化参数添加类似 printf() 的格式化参数的校验，判断需要格式化的参数与入参是否相同。

{% highlight text %}
format (archetype, string-index, first-to-check)

__attribute__((format(printf,m,n)))
__attribute__((format(scanf,m,n)))
  m : 第m个参数为格式化字符串(从1开始)；
  n : 变长参数(也即"...")的第一个参数排在总参数的第几个；
{% endhighlight %}

如下是使用示例。

{% highlight text %}
void myprint(const char *format,...) __attribute__((format(printf,1,2)));
void myprint(int l，const char *format,...) __attribute__((format(printf,2,3)));
{% endhighlight %}

如下是一个简单的使用示例。

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

编译时添加 `-Wall` 就会打印 Warning 信息，如果去除，实际上不会显示任何信息，通常可以提前发现常见的问题。

### \_\_attribute\_\_((constructor))

这是 GCC 的扩展机制，通过上述的属性，可以使程序在开始执行或停止时调用指定的函数。

```__attribute__((constructor))``` 在 main() 之前执行，```__attribute__((destructor))``` 在 main() 执行结束之后执行。

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

如果有多个函数，可以指定优先级，其中 0~100 (含100)系统保留。在 main 之前顺序为有小到大，退出时顺序为由大到小。

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

在使用时也可以先声明然再定义


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

程序调用某个函数 A，而 A 函数存在于两个动态链接库 liba.so 和 libb.so 中，并且程序执行需要链接这两个库，此时程序调用的 A 函数到底是来自于 a 还是 b 呢？

这取决于链接时的顺序，首先链接的库会更新符号表，比如先链接 liba.so，这时候通过 liba.so 的导出符号表就可以找到函数 A 的定义，并加入到符号表中，而不会再查找 libb.so 。

也就是说，这里的调用严重的依赖于链接库加载的顺序，可能会导致混乱。

gcc 的扩展中有如下属性 `__attribute__ ((visibility("hidden")))` 可以用于抑制将一个函数的名称被导出，对连接该库的程序文件来说，该函数是不可见的，使用的方法如下：

<!--
-fvisibility=default|internal|hidden|protected
gcc的visibility是说，如果编译的时候用了这个属性，那么动态库的符号都是hidden的，除非强制声明。
-->

#### 1. 创建一个c源文件

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

想要做的是，第一个函数符号可以被导出，第二个被隐藏。

#### 2. 生成动态库

先编译成一个动态库，使用到属性 `-fvisibility` 。

{% highlight text %}
----- 编译
$ gcc -shared -o libvis.so -fvisibility=hidden foobar.c

----- 查看符号链接
# readelf -s libvis.so |grep hidden
 7: 0000040c 20 FUNC GLOBAL DEFAULT 11 not_hidden
48: 00000420 20 FUNC LOCAL  HIDDEN  11 is_hidden
51: 0000040c 20 FUNC GLOBAL DEFAULT 11 not_hidden
{% endhighlight %}

可以看到，属性确实有作用了。

#### 3. 编译链接

现在试图链接程序。

{% highlight c %}
int main()
{
    not_hidden();
    is_hidden();
    return 0;
}
{% endhighlight %}

试图编译成一个可执行文件，链接到刚才生成的动态库。

{% highlight text %}
$ gcc -o exe main.c -L ./ -lvis
/tmp/cckYTHcl.o: In function `main':
main.c:(.text+0x17): undefined reference to `is_hidden'
{% endhighlight %}

说明了 hidden 确实起到作用了。

### \_\_attribute\_\_((sentinel))

该属性表示，此可变参数函数需要一个 `NULL` 作为最后一个参数，这个 `NULL` 参数一般被叫做 "哨兵参数"。例如，有如下程序：

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

当通过 `gcc main.c -Wall` 进行编译时，会发现没有任何警告，不过很显然，调用 `foo()` 时最后一个参数应该是个 `NULL` 以表明 "可变参数就这么多"。

编译完成后，如果尝试运行则会打印一些乱码，显然是有问题的。

正常来说，应该通过如下方式调用 `foo("Hello", "World", NULL);`，为此，就需要用到了上述的属性，用于表示最后一个参数需要为 `NULL` 。

{% highlight c %}
void foo(char *first, ...) __attribute__((sentinel));
{% endhighlight %}

这样再不写哨兵参数，在编译时编译器就会发出警告了。


但是，对于同样使用可变参数的 `printf()` 来说，为什么就不需要哨兵属性，实际上，通过第一个参数就可以确定需要多少个参数，如下。

{% highlight c %}
/*
 * 第一个参数中规定了有两个待打印项，所以打印时会取 "string" 和 1，多写的 "another_string" 会被忽略。
 * printf()在被调用时明确知道此次调用需要多少个参数，所以也就无需哨兵参数的帮忙。
 */
printf("%s %d\n", "string", 1, "another_string");
{% endhighlight %}

## atexit()

很多时候我们需要在程序退出的时候做一些诸如释放资源的操作，但程序退出的方式有很多种，比如 main() 函数运行结束、在程序的某个地方用 exit() 结束程序、用户通过 Ctrl+C 或 Ctrl+break 操作来终止程序等等，因此需要有一种与程序退出方式无关的方法来进行程序退出时的必要处理。

方法就是用 atexit() 函数来注册程序正常终止时要被调用的函数。

{% highlight c %}
#include <stdlib.h>
int atexit(void(*func)(void));
{% endhighlight %}

成功时返回零，失败时返回非零。

在一个程序中至少可以用 atexit() 注册 32 个处理函数，依赖于编译器。这些处理函数的调用顺序与其注册的顺序相反，也即最先注册的最后调用，最后注册的最先调用。

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

如果通过 ```define``` 定义一个含有多个语句的宏，通常我们使用 ```do{...} while(0);``` 进行定义，具体原因，如下详细介绍。

如果想在宏中包含多个语句，可能会如下这样写。

{% highlight c %}
#define do_something() \
   do_a(); \
   do_b();
{% endhighlight %}

通常，这样就可以用 ```do_somethin()``` 来执行一系列操作，但这样会有个问题：如果通过如下的方式用这个宏，将会出错。

{% highlight c %}
if (...)
   do_something();

// 宏被展开后
if (...)
   do_a();
   do_b();
{% endhighlight %}

原代码的目的是想在 if 为真的时候执行 ```do_a()``` 和 ```do_b()```, 但现在，实际上只有 ```do_a()``` 在条件语句中，而 ```do_b()``` 任何时候都会执行。

当然这时可以通过如下的方式将那个宏改进一下。

{% highlight c %}
#define do_something() { \
   do_a(); \
   do_b(); \
}
{% endhighlight %}

然而，即使是这样，仍然有错。假设有一个宏是这个样子的，

{% highlight c %}
#define do_something() { \
   if (a)     \
      do_a(); \
   else       \
      do_b(); \
{% endhighlight %}

在使用如下情况时，仍会出错。

{% highlight c %}
if (...)
   do_something();
else {
   ...
}

// 宏展开后
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

此时第二个 else 前边会有一个分号，那么编译时就会出错。

因此对于含有多条语句的宏我们使用 ```do{...} while(0);``` ，do-while 语句是需要分号来结束的，另外，现代编译器的优化模块能够足够聪明地注意到这个循环只会执行一次而将其优化掉。

综上所述，```do{...} while(0);``` 这个技术就是为了类似的宏可以在任何时候使用。


## assert()

其作用是如果它的条件返回错误，则输出错误信息 (包括文件名，函数名等信息)，并终止程序执行，原型定义：

{% highlight c %}
#include <assert.h>
void assert(int expression);
{% endhighlight %}

如下是一个简单的示例。

{% highlight c %}
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE *fp;

   fp = fopen( "test.txt", "w" ); // 不存在就创建一个同名文件
   assert( fp );                  // 所以这里不会出错
   fclose( fp );

    fp = fopen( "noexitfile.txt", "r" );  // 不存在就打开文件失败
    assert( fp );                         // 这里出错
    fclose( fp );                         // 程序不会执行到此处

    return 0;
}
{% endhighlight %}

当在 ```<assert.h>``` 之前定义 NDEBUG 时，assert 不会产生任何代码，否则会显示错误。

### 判断程序是否有 assert

在 glibc 中，会定义如下的内容：

{% highlight c %}
#define assert(e) ((e \
    ? ((void)0) \
    :__assert_fail(#e,__FILE__,__LINE__))
{% endhighlight %}

可以通过 nm 查看程序，判断是否存在 ```__assert_fail@@GLIBC_2.2.5``` ，如果存在该函数则说明未关闭 ```assert()``` 。

对于 autotool 可以通过如下的一种方式关闭：

1. 在 ```configure.ac``` 文件中添加 ```AC_HEADER_ASSERT``` ，然后如果关闭是添加 ```--disable-assert``` 参数，注意，一定要保证源码包含了 ```config.h``` 头文件；
2. 执行 configure 命令前设置环境变量，如 ```CPPFLAGS="CPPFLAGS=-DNDEBUG" ./configure```；
3. 也可以在 ```Makefile.am``` 中设置 ```AM_CPPFLAGS += -DNDEBUG``` 参数。

## backtrace

一般可以通过 gdb 的 bt 命令查看函数运行时堆栈，但是，有时为了分析程序的 BUG，可以在程序出错时打印出函数的调用堆栈。

在 glibc 头文件 `execinfo.h` 中声明了三个函数用于获取当前线程的函数调用堆栈。

{% highlight text %}
int backtrace(void **buffer,int size);
    用于获取当前线程的调用堆栈，获取的信息将会被存放在buffer中，它是一个指针列表。参数size用来指
    定buffer中可以保存多少个void*元素，该函数返回值是实际获取的指针个数，最大不超过size大小；

char **backtrace_symbols(void *const *buffer, int size);
    将从上述函数获取的信息转化为字符串数组，参数buffer应该是从backtrace()获取的指针数组，size是该
    数组中的元素个数，也就是backtrace()的返回值。
    函数返回值是一个指向字符串数组的指针，它的大小同buffer相同，每个字符串包含了一个相对于buffer中
    对应元素的可打印信息，包括函数名、函数的偏移地址和实际的返回地址。

void backtrace_symbols_fd(void *const *buffer, int size, int fd);
    与上述函数相同，只是将结果写入文件描述符为fd的文件中，每个函数对应一行。
{% endhighlight %}

注意，需要传递相应的符号给链接器以能支持函数名功能，比如，在使用 GNU ld 链接器的时需要传递 `-rdynamic` 参数，该参数用来通知链接器将所有符号添加到动态符号表中。

下面是 glibc 中的实例。

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

然后通过如下方式编译，执行。

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

还可以利用 backtrace 来定位段错误位置。

<!--
通常情况系，程序发生段错误时系统会发送SIGSEGV信号给程序，缺省处理是退出函数。我们可以使用 signal(SIGSEGV, &your_function);函数来接管SIGSEGV信号的处理，程序在发生段错误后，自动调用我们准备好的函数，从而在那个函数里来获取当前函数调用栈。

举例如下：

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

编译程序：
gcc -g -rdynamic test.c -o test; ./test
输出如下：

Obtained6stackframes.nm
./backstrace_debug(dump+0x45)[0x80487c9]
[0x468400]
./backstrace_debug(func_b+0x8)[0x804888c]
./backstrace_debug(func_a+0x8)[0x8048896]
./backstrace_debug(main+0x33)[0x80488cb]
/lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf3)[0x129113]

接着：
objdump -d test > test.s
在test.s中搜索804888c如下：

8048884 <func_b>:
8048884: 55          push %ebp
8048885: 89 e5      mov %esp, %ebp
8048887: e8 eb ff ff ff      call 8048877 <func_c>
804888c: 5d            pop %ebp
804888d: c3            ret

其中80488c时调用（call 8048877）C函数后的地址，虽然并没有直接定位到C函数，通过汇编代码， 基本可以推出是C函数出问题了（pop指令不会导致段错误的）。
我们也可以通过addr2line来查看

addr2line 0x804888c -e backstrace_debug -f

输出：

func_b
/home/astrol/c/backstrace_debug.c:57
-->





<!--
## 宏定义
http://hbprotoss.github.io/posts/cyu-yan-hong-de-te-shu-yong-fa-he-ji-ge-keng.html
http://gcc.gnu.org/onlinedocs/cpp/Macros.html
-->

## 其它

### is not a symbolic link

正常情况下，类似库 ```libxerces-c-3.0.so``` 应该是个符号链接，而不是实体文件，对于这种情况只需要修改其为符号链接即可。

{% highlight text %}
# mv libxerces-c-3.0.so libxerces-c.so.3.0
# ln -s libxerces-c.so.3.0 libxerces-c-3.0.so
{% endhighlight %}

### 结构体初始化

对于 C 中结构体初始化可以通过如下设置。

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

### 结构体地址定位

通过结构体可以将多种不同类型的对象聚合到一个对象中，编译器会按照成员列表顺序分配内存，不过由于内存对齐机制不同，导致不同架构有所区别，所以各个成员之间可能会有间隙，所以不能简单的通过成员类型所占的字长来推断其它成员或结构体对象的地址。

假设有如下的一个链表。

{% highlight text %}
typedef struct list_node {
	int ivar;
	char cvar;
	double dvar;
	struct list_node *next;
} list_node;
{% endhighlight %}

当已知一个变量的地址时，如何获取到某个成员的偏移量，Linux 内核中的实现如下。

{% highlight c %}
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
{% endhighlight %}

当知道了成员偏移量，那么就可以通过结构体成员的地址，反向求结构体的地址，如下。

{% highlight c %}
#define container_of(ptr, type, member) ({
	const typeof(((type *)0)->member ) *__mptr = (ptr);
	(type *)((char *)__mptr - offsetof(type,member));
	})
{% endhighlight %}

现在很多的动态语言是可以支持动态获取变量类型的，其中 GCC 提供了 `typeof` 关键字，所不同的是这个只在预编译时，最后实际转化为数据类型被编译器处理。基本用法是这样的：

{% highlight c %}
int a;
typeof(a)  b; // int b;
typeof(&a) c; // int* c;
{% endhighlight %}

如上的宏定义中， ptr 代表已知成员的地址，type 代表结构体的类型，member 代表已知的成员。

### 指针参数修改

一个比较容易犯错的地方，愿意是在 `foobar()` 函数内修改 `main()` 中的 v 指向的变量，其中后者实际上是修改的本地栈中保存的临时版本。

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

### 取固定大小的变量

Linux 每个数据类型的大小可以在 `sys/types.h` 中查看

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

### 环境变量

简单介绍下 C 中，如何获取以及设置环境变量。

其中设置环境变量方法包括了 `putenv()` 以及 `setenv()` 两种，前者必须是 `Key=Value` 这种格式，后者则以参数形式传递。

对于 `putenv()` 如果环境变量已经存在则替换，而 `setenv()` 则可以设置是否覆盖 。

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

C 语言标准库 `float.h` 中的 `FLT_RADIX` 常数用于定义指数的基数，也就是以这个数为底的多少次方。

{% highlight text %}
FLT_RADIX 10     10 的多少次方
FLT_RADIX 2       2 的多少次方
{% endhighlight %}

例如：

{% highlight text %}
#define FLT_MAX_EXP 128
#define FLT_RADIX   2
{% endhighlight %}

意思是 float 型，最大指数是 128，它的底是 2，也就说最大指数是 2 的 128 方。

<!--
FLT 是 float 的 缩写。( DBL 是 double 的 缩写。)
-->

#### implicit declaration

按常规来讲，出现 `implicit declaration of function 'xxxx'` 是因为头文件未包含导致的！

这里是由于 `nanosleep()` 函数的报错，而实际上 `time.h` 头文件已经包含了，后来才发现原来是在 `Makefile` 中添加了 `-std=c99` 导致，可以通过 `-std=gnu99` 替换即可。

另外，不能定义 `-D_POSIX_SOURCE` 宏。



## 参考

[Schemaless Benchmarking Suite](https://github.com/ludocode/schemaless-benchmarks)



{% highlight text %}
{% endhighlight %}
