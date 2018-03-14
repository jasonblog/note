---
title: C 語言的字符串
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,c,cpp,c++,program,string
description: 簡單介紹下 C 語言中與字符串操作相關的函數。
---

簡單介紹下 C 語言中與字符串操作相關的函數。

<!-- more -->

## 簡介

C 語言中通過雙引號表示字符串，與字符串操作相關的函數可以通過 ```man 3 string``` 查看幫助文檔。

{% highlight c %}
#include <stdio.h>
int main (int argc, char **argv)
{
  char *ptr = "Hello world";
  puts(ptr);
  return 0;
}
{% endhighlight %}

可以理解為，雙引號做了 3 件事：

1. 在只讀常量區申請空間存放了字符串；
2. 在字符串尾添加了'\0'；
3. 返回字符串開始地址。

注意，可以把字符串賦值給字符指針 ```char *ptr```，而不能把字符串賦值給一個字符數組。

{% highlight c %}
char a[10] = "hello"; // 支持初始化方式
a = "hello";          // 此時錯誤
{% endhighlight %}

```"hello"``` 返回的是一個地址，而 a 雖然也有地址，但是這與指針是不一樣的，指針的值是地址，而數組的值雖然也是地址，但是卻是一個常量，所以不能給常量賦值；此時需要通過 ```strncpy()``` 複製。

另外，數組會分配存儲空間，而 ```char *``` 實際上只是一個指針，當嘗試複製到只讀數據區時，那麼此時就會導致 ```"Segmentation fault"``` 。

{% highlight c %}
#include <stdio.h>
#include <string.h>
int main (int argc, char **argv)
{
// 如下代碼會出現"Segmentation fault"
//  char *ptr = "Hello world";
//  memcpy(ptr, "Hi foobar", strlen(ptr));

  char ptr[] = "Hello world";
  memcpy(ptr,  "Hey foobar!!!!!!!", sizeof(ptr));
  ptr[sizeof(ptr)-1] = '\0';
  puts (ptr);
}
{% endhighlight %}

### 檢查宏

對於字符串操作，在 gcc 中，可以通過 ```#define _FORTIFY_SOURCE 2``` 宏定義，對一些常見內存以及字符串處理函數 (memcpy, strcpy, stpcpy, gets 等) 的安全檢查，詳細可以查看 ```man 7 feature_test_macros``` 。

下面以字符串拼接函數為例。

{% highlight c %}
#inclue <string.h>
char * strncat(char *dest, const char *src, size_t n);
{% endhighlight %}

將 src 中開始的 n 個字符複製到 dest 的結尾，也就是將 dest 字符串最後的 ```'\0'``` 覆蓋掉，字符追加完成後，再追加 ```'\0'```；所以需要保證 dest 最小為 ```strlen(dest)+n+1``` 。

<!--
BAD: strncat(buffer,charptr,sizeof(buffer))
GOOD: strncat(buffer,charptr,sizeof(buffer)-strlen(buffer)-1)
-->

{% highlight c %}
#include <stdio.h>
#include <string.h>

int main(){
  char site[100] = "http://www.douniwai.com";
  char uri[300] = "/c/strncatiii";
  strncat(site, uri, 1000);  // BAD, 1000遠遠超過path的長度，那麼可能會導致segment fault
  strncat(site, uri, sizeof(site)-strlen(site)-1); // GOOD
  puts(site);
  return  0;
}
{% endhighlight %}

可以通過 ```gcc -Wall -D_FORTIFY_SOURCE=2 -O2 main.c``` 命令編譯測試。

















## 內存申請

{% highlight c %}
#include <stdlib.h>
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
{% endhighlight %}

malloc() 申請 size 字節大小大內存，內存空間未初始化，如果 size 是 0 可能會返回 NULL 或者一個後續可以 free 的指針；分配內存失敗則返回 NULL；calloc() 為申請數組空間，會自動初始化為 0 。

realloc() 的功能比較複雜，可以實現內存分配、釋放，會將 ptr 所指向的內存塊的大小修改為 size，並將新的內存指針返回，注意，不會保證返回的指針不變，會保證內存地址連續。注意，使用 malloc() 替換 realloc() 。

* 如果 ptr 和 size 均不為 0 時，會調整內存大小，返回新的開始指針，並保證源 size 內容不變；
* 如果 ptr 為 NULL 而 size 不為 0 ，那麼等同於 malloc(size)；
* 如果 size = 0，那麼相當於調用 free(ptr)。

一般來說，一個函數只提供一個功能，如上的函數卻賦予了好幾個功能，這並不是良好的函數設計，估計也是為了兼容性，才容忍這個函數一直在 C 庫中。

另外，根據 C 的標準 [ISO-IEC 9899 7.20.3.2/2](http://www.open-std.org/JTC1/SC22/wg14/www/docs/n1124.pdf)：

{% highlight text %}
void free(void *ptr);
  If ptr is a null pointer, no action occurs.
{% endhighlight %}

所以，在執行 free() 時，不需要檢查是否為 NULL，因此可以實現一個 sfree() 宏。

{% highlight c %}
#define sfree(m) do { \
    free(m);          \
    m = NULL;         \
} while(0);
{% endhighlight %}


### 注意事項

realloc()有可能操作失敗，返回NULL，所以不要把它的返回值直接賦值給原來的指針變量，以免原值丟失：

{% highlight c %}
// 錯誤處理，返回失敗時，原指針丟失，導致內存洩露
void *ptr = realloc(ptr, new_size);
if (!ptr) {
}
// 正確使用方法
void *new_ptr = realloc(ptr, new_size);
if (!new_ptr) {
}
ptr = new_ptr
{% endhighlight %}

一般 malloc(0) 和 calloc(0) 是合法的語句，會返還一個合法的指針，且該指針可以通過 free() 去釋放；而對於 realloc() 則有不同的行為，如果為 0 ，那麼最後再次 free() 時，就會導致重複釋放。

### 示例

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  int *p = NULL;
  p = (int *) malloc(sizeof(int));
  if (NULL == p) {
    printf("Can't get memory!\n");
    exit(EXIT_FAILURE);
  }
  memset(p, 0, sizeof(int));
  *p = 2;
  printf("%d\n", *p);
  free(p);

  #define SIZE 5
  int i = 0;
  p = (int *) calloc(SIZE, sizeof(int));
  if (NULL == p) {
    printf("Can't get memory!\n");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < SIZE; i++) {
    p[i] = i;
  }
  for (i = 0; i < SIZE; i++) {
    printf("p[%d]=%d\n", i, p[i]);
  }
  return 0;
}
{% endhighlight %}




## 字符串格式化

{% highlight c %}
#include <stdio.h>
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
{% endhighlight %}

除了上述一類的格式化函數之外，還有一類是上述函數添加了 v 開頭的函數，如 ```vprintf()```、```vfprintf()``` 等；區別是後或者可以用戶自定義打印函數，示例如下：

{% highlight c %}
int error(char *fmt, ...)
{
    int result;
    va_list args;
    va_start(args, fmt);
    fputs("Error: ", stderr);
    result = vfprintf(stderr, fmt, args);
    va_end(args);
    return result;
}
{% endhighlight %}

其中 ```printf()``` 和 ```fprintf()``` 輸出到緩衝區，所以不需要考慮溢出，對於 ```sprintf()``` 和 ```snprintf()``` 建議使用後者；後者最多從源串中拷貝 n-1 個字符到目標串中，然後再在字符串後面添加 ```'\0'``` 。

若成功則返回存入數組的字符數(不含```'\0'```)；如果空間不足，則返回 "如果有足夠空間存儲，所應能輸出的字符數(不含'\0')"，所以可以通過判斷返回值是否大於 n 決定是否有溢出；若編碼出錯則返回負值。

如下是一個簡單的示例。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv)
{
  char buffer[255];
  int offset = 0, i;

  for ( i = 0; i < 10; i++) {
    offset += snprintf(buffer + offset, 255 - offset, ":%x\0", i);
  }
  printf ("buffer: %s\n", buffer);
  return 0;
}
{% endhighlight %}



### 示例

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>

int main()
{
  char str[10];
  int ret;

  // 超過10個字符，返回應該使用的strlen()，也就是12
  ret = snprintf(str, sizeof(str), "Hey %s !", "foobar");
  if (ret < 0) {
    perror("snprintf");
    exit(EXIT_FAILURE);
  } else if (ret >= sizeof(str)) {
    fprintf(stderr, "no enough buffer\n");
    printf("%s, ret %d\n", str, ret);
  } else {
    printf("%s, ret %d\n", str, ret);
  }

  return 0;
}
{% endhighlight %}


## 字符串整形轉換

將字符串轉換為長整型數，會掃描參數 nptr 字符串，跳過前面的空白字符 (如空格、tab等)，直到遇上數字或正負符號才開始做轉換，然後再遇到非數字或字符串結束符(```'\0'```)結束轉換，並將結果返回。

{% highlight text %}
#include <stdlib.h>
long int strtol (const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);

參數：
  str    : 要轉換的字符串；
  endstr : 第一個不能轉換的字符的指針，非NULL傳回不符合條件而終止的字符串指針；
  base   : 字符串str所採用的進制，範圍從2~36，也可以為0(默認為10進制；遇到0x/0X前置採用16進制；遇到0前置採用8進制) 。
返回值：
  轉換後的長整型數；如果不能轉換或者nptr為空字符串，返回 0(0L)；
  如果轉換得到的值超出long int所能表示的範圍，函數將返回LONG_MAX或LONG_MIN (在limits.h中定義)，並將errno設置為ERANGE。
{% endhighlight %}

ANSI C 規範定義了 ```stof()```、```atoi()```、```atol()```、```strtod()```、```strtol()```、```strtoul()``` 共 6 個可以將字符串轉換為數字的函數；在 C99/C++11 規範中又新增了 5 個函數，分別是 ```atoll()```、```strtof()```、```strtold()```、```strtoll()```、```strtoull()```。

```atoi()``` 和 ```strtol()``` 的區別在於，前者無法判斷轉換是否成功，後者可以通過返回指針以及 errno 判斷是否轉換成功。

<!-- 如何通過宏定義判斷是否支持後面的函數。-->

### 示例

{% highlight c %}
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* strtol */
#include <errno.h>      /* errno */
#include <limits.h>     /* LONG_MAX, LONG_MIN */

int main (int argc, char **argv)
{
  char *numbers = "2001 60c0c0 0351 -1101110100110100100000 0x6fffff";
  char *endptr;
  long int li1, li2, li3, li4, li5;
  errno = 0;
  li1 = strtol (numbers, &endptr, 10);
  if ((errno == ERANGE && (li1 == LONG_MAX || li1 == LONG_MIN))
                || (errno != 0 && li1 == 0)) {
      perror("strtol");
      exit(EXIT_FAILURE);
  }
  if (endptr == numbers) { // || *pEend != '\0'
    fprintf(stderr, "No digits were found\n");
    exit(EXIT_FAILURE);
  }
  if (*endptr != '\0')
    printf("Further characters after number: %s\n", endptr);
  li2 = strtol (endptr, &endptr, 16);
  li3 = strtol (endptr, &endptr, 8);
  li4 = strtol (endptr, &endptr, 2);
  li5 = strtol (endptr, NULL, 0);
  printf ("The decimal equivalents are: %ld, 0x%lX, 0%lo, %ld and 0x%lX.\n", li1, li2, li3, li4, li5);

  return 0;
}
{% endhighlight %}

另外是浮點數的轉換。

{% highlight text %}
double strtod (const char* str, char** endptr);
{% endhighlight %}

會自動掃描參數 `str` 字符串，並跳過空白字符 (通過 isspace 函數檢測)，直到遇上數字或正負符號才開始做轉換，當出現非數字時停止轉換，其中 `endptr` 返回第一個不能轉換的字符指針。

當 `endptr` 為 `NULL` 或者 等於 `str` 時表示轉換失敗。

## 字符串查找

簡單介紹下常見的字符串操作函數。

### strtok()

可以參考如下的示例，在 ```strtok()``` 代碼中，會保存上次查詢的狀態，就導致不可以重入以及線程不安全。

{% highlight c %}
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
   char str[1024]="Fred male 25,John male 62,Anna female 16";
   char *token, *subtoken;

   for(token = strtok(str, ","); token != NULL; token = strtok(NULL, ",")) {
      printf( " %s\n", token );
      for(subtoken = strtok(token, " "); subtoken != NULL; subtoken = strtok(NULL, " ")) {
          printf( "  -> %s\n", subtoken );
      }
   }

   return 0;
}
{% endhighlight %}

如上的代碼中，實際只提取了第一個人的信息，而非我們所預想的會提取所有代碼。

原因是，在第一次外循環中，```strtok()``` 將 ```Fred male 25,``` 後的逗號，改為了 ```'\0'```，這時 ```strtok()``` 內部的指針指向的是逗號的後一個字符 ```'J'```，經過第一次的內循環，分別提取出了 ```"Fred"```、 ```"male"```、 ```"25"```，而且在提取完 ```"25"``` 之後，函數內部的指針被修改指向了 ```"25"``` 後面的 ```'\0'``` 。

內循環結束後開始第二次的外循環，而此時 ```strtok()``` 函數內部的指針已經指向了 ```NULL``` ，所以，當執行第二次的循環時會直接退出而不再執行。

#### strtok_r()

```strtok_r()``` 是 Linux 平臺下 ```strtok()``` 的線程安全版，```saveptr``` 參數是一個指向 ```char *``` 的指針變量，用來在 ```strtok_r()``` 內部保存切分時的上下文，以應對連續調用分解相同源字符串。

第一次調用 ```strtok_r()``` 時，```str``` 參數必須指向待提取的字符串，```saveptr``` 參數的值可以忽略；後續調用時，```str``` 賦值為 ```NULL```，```saveptr``` 為上次調用後返回的值，不要修改。

關於示例程序可以查看 [man 3 strtok_r](https://linux.die.net/man/3/strtok_r) ，如下是一個 GUN C 的實現程序。

{% highlight c %}
/* Parse S into tokens separated by characters in DELIM.
   If S is NULL, the saved pointer in SAVE_PTR is used as
   the next starting point.  For example:
        char s[] = "-abc-=-def";
        char *sp;
        x = strtok_r(s, "-", &sp);      // x = "abc", sp = "=-def"
        x = strtok_r(NULL, "-=", &sp);  // x = "def", sp = NULL
        x = strtok_r(NULL, "=", &sp);   // x = NULL
                // s = "abc\0-def\0"
*/
char *strtok_r(char *s, const char *delim, char **save_ptr) {
    char *token;

    if (s == NULL) s = *save_ptr;

    /* Scan leading delimiters.  */
    s += strspn(s, delim);
    if (*s == '\0')
        return NULL;

    /* Find the end of the token.  */
    token = s;
    s = strpbrk(token, delim);
    if (s == NULL)
        /* This token finishes the string.  */
        *save_ptr = strchr(token, '\0');
    else {
        /* Terminate the token and make *SAVE_PTR point past it.  */
        *s = '\0';
        *save_ptr = s + 1;
    }

    return token;
}
{% endhighlight %}


### strchr()

該函數是字符查找，找到則返回第一個字符的地址，否則返回 NULL 。如下是一個簡單的程序，用來分割一個字符串，類似與 ```/etc/shadow``` 中保存的密碼。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv)
{
    char *encrypted = strdup("$1$test$justfortest");
    char *id, *salt, *passwd;

    id = strchr(encrypted, '$');
    if (id == NULL)
        exit(EXIT_FAILURE);
    *id = 0;
    id++;

    salt = strchr(id, '$');
    if (salt == NULL)
        exit(EXIT_FAILURE);
    *salt = 0;
    salt++;

    passwd = strchr(salt, '$');
    if (passwd == NULL)
        exit(EXIT_FAILURE);
    *passwd = 0;
    passwd++;

    printf("id: %s salt: %s password: %s\n", id, salt, passwd);
    free(encrypted);

    return 0;
}
{% endhighlight %}

### 其它


#### 字符串查找

{% highlight c %}
#include <string.h>
char *strstr(const char *haystack, const char *needle);
{% endhighlight %}

也就是從 `haystack` (乾草堆) 中找到 `needle` (針) 第一次出現的位置，不包括 `needle` 中的串結束符；如找不到，返回空指針。


## 字符串複製

{% highlight c %}
#include <string.h>
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
{% endhighlight %}

前者會將 src 複製到 dest ，包括 ```'\0'``` 字符，需要保證 dest 有足夠的空間，否則可能會發生溢出。而後者最多複製 n 個字符串到 dest ，注意，如果這 n 個字符中沒有 ```'\0'``` 那麼需要手動添加。

返回字符串 dest 。

### strdup()

該函數會通過 ```malloc()``` 申請內存，然後將字符串複製過去，所以需要調用者手動釋放內存。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
  char *src ="This is the jibo";
  char *dest;
  dest = strdup(src);
  printf("the dest %s\n",dest);
  free(dest);

  return 0;
}
{% endhighlight %}

如果不通過 ```free()``` 釋放內存，那麼會導致內存洩露，可以通過 valgrind 命令檢查，如下：

{% highlight text %}
$ valgrind --tool=memcheck --leak-check=yes ./test
{% endhighlight %}

### memcpy VS. memmove

{% highlight c %}
#include <stdio.h>
#include <string.h>
int main ()
{
  char str[] = "memmove can be very useful......";
  memmove (str+20, str+15, 11);
  puts (str);
  return 0;
}
{% endhighlight %}

注意，即使替換為 ```memcpy()``` 上述的代碼仍有可能是正確的，這主要是由於編譯器的優化所致，詳細可以反彙編查看。

{% highlight c %}
#include <string.h>
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
{% endhighlight %}

兩者都是將 N 個字節的源內存地址的內容拷貝到目標內存地址中，需要保證 dest 有足夠的空間；區別是後者可以有效處理重疊區域<!--，首先會將 src 的內容複製到臨時緩存區，然後再複製到 dest 中-->，其處理措施如下：

* 當源內存的首地址等於目標內存的首地址時，不進行任何拷貝；
* 當源內存的首地址大於目標內存的首地址時，實行正向拷貝；
* 當源內存的首地址小於目標內存的首地址時，實行反向拷貝；

更詳細的場景可以分為如下：

{% highlight text %}
內存低端 <-----s-----> <-----d-----> 內存高端 start at end of s
內存低端 <-----s--<==>--d----->      內存高端 start at end of s
內存低端 <-----sd----->              內存高端 do nothing
內存低端 <-----d--<==>--s----->      內存高端 start at beginning of s
內存低端 <-----d-----> <-----s-----> 內存高端 start at beginning of s
{% endhighlight %}

如下是兩者的實現代碼：

{% highlight c %}
void* memcpy(void* dest, const void* src, size_t n)
{
  char*        d = (char*) dest;
  const char*  s = (const char*) src;
  while(n--)
    *d++ = *s++;
  return dest;
}

void* memmove(void* dest, const void* src, size_t n)
{
  char*        d = (char*) dest;
  const char*  s = (const char*) src;

  if (s>d) {        // start at beginning of s
    while (n--)
      *d++ = *s++;
  } else if (s<d) { // start at end of s
    d = d+n-1;
    s = s+n-1;
    while (n--)
      *d-- = *s--;
  }
  return dest;
}
{% endhighlight %}

與之相關的還有 ```memccpy()```、```mempcpy()``` 以及 ```bcopy()``` 函數：

* ```memccpy()``` 將 src 指向內存的前 n 個字節複製到 dest ，期間在遇到某個特定值 (int c) 就立即停止複製。
* ```mempcpy()``` 類似於 ```memcpy()``` ，不同的是該函數返回複製到最後地址的下一個地址，而非返回 dest，通常用於流式的複製。

## 通配符使用

當 shell 在參數中遇到了通配符時，會嘗試將其當作路徑或文件名去在磁盤上搜尋可能的匹配：若符合要求的匹配存在，則進行代換(路徑擴展)；否則就將該通配符作為一個普通字符傳遞給命令，然後再由命令進行處理。

也就是說，通配符是由 shell 處理的，在傳遞給可執行程序之前已經進行了處理，實際上就是一種 shell 實現的路徑擴展功能。

{% highlight text %}
*                       匹配0或多個字符             a*b  ab a012b aabcd
?                       匹配任意一個字符            a?b  abb acb a0b
[list]                  匹配list中的任意單一字符    a[xyz]b  axb ayb azb
[!list]                 匹配除list中的任意單一字符  a[!0-9]b  axb abb a-b
[c1-c2]                 匹配c1到c2中的任意單一字符  a[0-9]b   a0b a9b
{string1,string2,...}   匹配sring1或string2(或更多)其一字符串
{% endhighlight %}

另外，需要注意 shell 通配符和正則表達式之間的區別。

### fnmatch()

就是判斷字符串是不是符合 pattern 所指的結構，這裡的 pattern 是 `shell wildcard pattern`，其中部分匹配行為可以通過 flags 配置，詳見 `man 3 fnmatch` 。

<!-- int fnmatch(const char *pattern, const char *string, int flags); -->

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <sys/types.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    int ret;
    char *pattern = "*.c";
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir("/tmp")) == NULL) {
        perror("opendir()");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir)) != NULL) { // 逐個獲取文件夾中文件
        ret = fnmatch(pattern, entry->d_name, FNM_PATHNAME|FNM_PERIOD);
        if (ret == 0) {         //符合pattern的結構
            printf("%s\n", entry->d_name);
        } else if (ret == FNM_NOMATCH){
            continue ;
        } else {
            printf("error file=%s\n", entry->d_name);
        }
    }
    closedir(dir);
    return 0;
}
{% endhighlight %}

### wordexp()

按照 `Shell-Style Word Expansion` 擴展將輸入字符串拆分，返回的格式為 `wordexp_t` 變量，其中包括了三個變量，兩個比較重要的是：A) `we_wordc` 成員數；B) `we_wodv` 數組。

注意，在解析時會動態分配內存，所以在執行完 `wordexp()` 後，需要執行 `wordfree()`；另外，如果遇到內存不足會返回 `WRDE_NOSPACE` 錯誤，此時可能已經分配了部分地址，所以仍需要執行 `wordfree()` 。

1) 按照空格解析；2) 正則表達式；3) 環境變量。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>

int main(int argc, char **argv)
{
    int i, ret;
    wordexp_t p;

    ret = wordexp("foo bar $SHELL *[0-9].c *.c", &p, 0);
    if (ret == WRDE_NOSPACE) {
        wordfree(&p);
        exit(EXIT_FAILURE);
    } else if (ret != 0) {
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < p.we_wordc; i++)
        printf("%s\n", p.we_wordv[i]);
    wordfree(&p);
    exit(EXIT_SUCCESS);
}
{% endhighlight %}

<!-- http://www.gnu.org/software/libc/manual/html_node/Word-Expansion.html -->

### qsort()

只用於數組的排序，對於鏈表等無效。

{% highlight text %}
void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*));
    base  : 數組的基地址
    nitems: 數組包含的元素；
    size  : 每個元素的大小；
    compar: 比較函數；
{% endhighlight %}

示例程序如下。

{% highlight c %}
#include <stdio.h>
#include <stdlib.h>

int cmpfunc (const void *a, const void *b)
{
   return ( *(int*)a - *(int*)b );
}

int main()
{
   int n;
   int values[] = { 88, 56, 100, 2, 25 };

   printf("Before sorting the list is: \n");
   for( n = 0 ; n < 5; n++ ) {
      printf("%d ", values[n]);
   }
   putchar('\n');
   qsort(values, 5, sizeof(int), cmpfunc);
   printf("After sorting the list is: \n");
   for( n = 0 ; n < 5; n++ ) {
      printf("%d ", values[n]);
   }
   putchar('\n');

   return(0);
}
{% endhighlight %}


{% highlight c %}
{% endhighlight %}
