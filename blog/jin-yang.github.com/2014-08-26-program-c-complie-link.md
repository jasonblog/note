---
title: C 編譯鏈接
layout: post
comments: true
language: chinese
category: [program,misc]
keywords: c,編譯鏈接
description: 詳細介紹下與 C 語言相關的概念。
---

詳細介紹下與 C 語言相關的概念。

<!-- more -->

## 目標文件

目標文件是編譯器編譯完後，還沒有進行鏈接的文件，通常採用和可執行文件相同的存儲格式，如 Windows 平臺下的 `Portable Executable, PE`，Linux 平臺下的 `Executable Linkale Format, ELF`，它們都是 `Common File Format, COFF` 的變種。

除可執行文件，動態鏈接庫 `(Dynamic Linking Library)` 和靜態鏈接庫 `(Statci Linking Library)` 也是按照可執行文件進行存儲。

對於文件的類型可以通過 file 命令進行查看，常見的類型包括了：

* 可重定位文件(Relocatable File)<br>主要包含了代碼和數據，主要用來鏈接成可執行文件或共享目標文件，如 `.o` 文件。
* 可執行文件(Executable File)<br>主要是可以直接執行的程序，如 `/bin/bash` 。
* 共享目標文件(Shared Object File)<br>包含了代碼和數據，常見的有動態和靜態鏈接庫，如 `/lib64/libc-2.17.so` 。
* 核心轉儲文件(Core Dump File)<br>進程意外終止時，系統將該進程的地址空間的內容及終止時的一些其他信息轉儲到該文件中。

### 示例程序

目標文件通過段 (Segment) 進行存儲，在 Windows 中可以通過 `Process Explorer` 查看進程相關信息，Linux 可以通過 `objdump` 查看。主要的段包括了 `.text` `.data` `.bss(Block Started by Symbol)`，當然還有一些其它段，如 `.rodata` `.comment` 等。

{% highlight c %}
int printf(const char* format, ...);

int global_init_var = 84;
int global_uninit_var;

void func1(int i)
{
    printf("%d\n", i);
}

int main(void)
{
    static int static_var = 85;
    static int static_var2;

    int a = 1;
    int b;

    func1(static_var + static_var2 + a + b);

    return a;
}
{% endhighlight %}

然後，可以通過 `gcc -c section.c` 編譯，編譯完上述的文件之後，可以通過 `objdump -h` 查看頭部信息，也可以通過 `-x` 參數查看更詳細的信息。

比較重要的是 `File off` 和 `Size` 信息，一般頭部信息的大小為 `0x34` ，因此第一個段的地址就會從 `0x34` 開始 (地址從 0 開始計數)，另外，由於需要 4bytes 對齊，因此會從 `54(0x36)` 開始。也可以通過 size 查看，採用的是十進制，最後會用十進制和十六進製表示總的大小。

數據段 `.data` 用來保存已經初始化了的全局變量和局部靜態變量，如上述的 `global_init_var` 和 `static_var` 。

只讀數據段 `.rodata` ，主要用於保存常量，如 `printf()` 中的字符串和 `const` 類型的變量，該段在加載時也會將其設置為只讀。

`BSS` 段保存了未初始化的全局變量和局部靜態變量，如上述 `global_uninit_var` 和 `static_var2` 。

<!--
正常應該是 8 字節，但是查看時只有 4 字節，通過符號表(Symbol Table)可以看到，只有 static_var2 保存在 .bss 段，而 global_uninit_var 未存放在任何段，只是一個未定義的 COMMON 符號。這與不同的語言和編譯器實現有關，有些編譯器會將全局的為初始化變量存放在目標文件 .bss 段，有些則不存放，只是預留一個未定義的全局變量符號，等到最終鏈接成可執行文件時再在 .bss 段分配空間。
-->

`.text` 為代碼段，`.data` 保存含初始值的變量，`.bss` 只保存了變量的符號。


### 添加一個段

將以個二進制文件，如圖片、MP3 音樂等作為目標文件的一個段。如下所示，此時可以直接聲明 `_binary_example_png_start` 和 `_binary_example_png_end` 並使用。

{% highlight text %}
$ objcopy -I binary -O elf32-i386 -B i386 example.png image.o
$ objdump -ht image.o

image.o:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .data         000293d6  00000000  00000000  00000034  2**0
                  CONTENTS, ALLOC, LOAD, DATA
SYMBOL TABLE:
00000000 l    d  .data	00000000 .data
00000000 g       .data	00000000 _binary_example_png_start
000293d6 g       .data	00000000 _binary_example_png_end
000293d6 g       *ABS*	00000000 _binary_example_png_size
{% endhighlight %}

如果在編譯時想將某個函數或者變量放置在一個段裡，可以通過如下的方式進行。

{% highlight c %}
__attribute__((section("FOO"))) int global = 42;
__attribute__((section("BAR"))) void foo() { }
{% endhighlight %}

### 運行庫

在 `main()` 運行之前通常會先執行一段代碼，運行這些代碼的函數稱為 **入口函數** 或 **入口點** ，大致的步驟如下：

* 操作系統創建進程後，把控制權交給程序入口，這個入口往往是運行庫中的某個入口函數。
* 入口函數對運行庫和程序運行環境進行初始化，包括堆、I/O、線程、全局變量構造等。
* 入口函數在完成初始化之後，調用 main 函數，正式開始執行程序主體部分。
* `main()` 執行完後，返回到入口函數，入口函數進行清理工作，包括全局變量析構、堆銷燬、關閉 IO 等，然後進行系統調用結束進程。

## 鏈接過程

在程序由源碼到可執行文件的編譯過程實際有預處理 (Propressing)、編譯 (Compilation)、彙編 (Assembly) 和鏈接 (Linking) 四步，在 `gcc` 中分別使用 `ccp`，`cc1`，`as`，`ld` 來完成。

關於鏈接方面可以直接從網上搜索 《linker and loader》。

![compile link gcc details]({{ site.url }}/images/linux/compile-link-gcc-details.jpg "compile link gcc details"){: .pull-center }

### 預編譯

將源代碼和頭文件通過預編譯成一個 `.i` 文件，相當與如下命令。

{% highlight text %}
$ gcc -E main.c -o main.i          # C
$ cpp main.c > main.i              # CPP
{% endhighlight %}

與編譯主要是處理源碼中以 `"#"` 開始的與編譯指令，主要的處理規則是：

* 刪除所有的 `"#define"` ，並且展開所有的宏定義。
* 處理所有條件預編譯指令，比如 `"#if"`、`"#ifdef"`、`"#elif"`、`"#else"`、`"#endif"` 。
* 處理 `"#include"` ，將被包含的文件插入到該預編譯指令的位置，該過程是遞歸的。
* 刪除多有的註釋 `"//"` 和 `"/* */"` 。
* 添加行號和文件名標識，如 `#2 "main.c" 2` ，用於編譯時產生調試用的行號以及在編譯時產生錯誤或警告時顯示行號。
* 保留所有的 `"#pragma"` 編譯器指令，因為編譯器需要使用它們。

經過預編譯後的 `.i` 文件不包含任何宏定義，因為所有的宏已經被展開，並且包含的文件也已經被插入到 `.i` 文件中。所以，當無法判斷宏定義是否正確或頭文件包含是否正確時，可以查看該文件。

### 編譯

編譯過程就是把預處理後的文件進行一系列的詞法分析、語法分析、語義分析以及優化後生成相應的彙編代碼文件，這個是核心部分，也是最複雜的部分。

gcc 把預編譯和編譯合併成一個步驟，對於 C 語言使用的是 `cc1` ，C++ 使用的是 `cc1obj` 。

{% highlight text %}
$ gcc -S hello.i -o hello.s
$ gcc -S main.c -o main.s
{% endhighlight %}

<!-- $ /usr/lib/gcc/i386-linux-gnu/4.7/cc1 main.c -->

### 彙編

彙編器是將彙編代碼轉化成機器碼，每條彙編語句幾乎都對應一條機器指令。彙編器不需要複雜的語法語義，也不用進行指令優化，只是根據彙編指令和機器指令的對照表一一翻譯即可。

{% highlight text %}
$ gcc -c hello.s -o hello.o
$ as main.s -o main.o
$ gcc -c main.s -o main.o
$ gcc -c main.c -o main.o
{% endhighlight %}

### 鏈接

可以通過 `gcc hello.c -o hello -v` 查看。

{% highlight text %}
$ gcc hello.o -o hello.exe
{% endhighlight %}

## 靜態庫和動態庫

庫有動態與靜態兩種，Linux 中動態通常用 `.so` 為後綴，靜態用 `.a` 為後綴，如：`libhello.so` `libhello.a`。為了在同一系統中使用不同版本的庫，可以在庫文件名後加上版本號為後綴，例如：`libhello.so.1.0`，然後，使用時通過符號鏈接指向不同版本。

{% highlight text %}
# ln -s libhello.so.1.0 libhello.so.1
# ln -s libhello.so.1 libhello.so
{% endhighlight %}

在動態鏈接的情況下，可執行文件中還有很多外部符號的引用還處於無效地址的狀態，因此需要首先啟一個 **動態鏈接器 (Dynamic Linker)**，該連接器的位置在程序的 `".interp"` (interpreter) 中指定，可以通過如下的命令查詢。

{% highlight text %}
$ readelf -l a.out | grep interpreter
{% endhighlight %}

### 共享庫的更新

對於共享庫更新時通常會有兼容更新和不兼容更新，此處所說的兼容是指二進制接口，即 `ABI (Application Binary Interface)`。

為了保證共享庫的兼容性， Linux 採用一套規則來命名系統中的共享庫，簡單來說，其規則如下 `libname.so.x.y.z`，name 為庫的名稱，x y z 的含義如下：

* x，主版本號(Major Version Number)，庫的重大升級，不同的主版本號之間不兼容。
* y，次版本號(Minor Version Number)，庫的增量升級，增加了一些新的接口，且保持原來的符號不變。
* z，發佈版本號(Release Version Number)，庫的一些錯誤的修正、性能的改進等，並不添加任何新的接口，也不對接口進行改進。

由於歷史的原因最基本的 C 語言庫 glibc 動態鏈接庫不使用這種規則，如 `libc-x.y.z.so` 、`ld-x.y.z.so` 。

下面這篇論文， Library Interface Versioning in Solaris and Linux ，對 Salaris 和 Linux 的共享庫版本機制和符號版本機製做了非常詳細的介紹。

在 Linux 中採用 SO-NAME 的命名機制，每個庫會對應一個 SO-NAME ，這個 SO-NAME 只保留主版本號，也即 SO-NAME 規定了共享庫的接口。

### 路徑問題

如果動態庫不在搜索路徑中，則會報 `cannot open shared object file: No such file or directory` 的錯誤。可以通過 `gcc --print-search-dirs` 命令查看默認的搜索路徑。

查找順序通常為：

1. 查找程序編譯指定的路徑，保存在 `.dynstr` 段，其中包含了一個以冒號分割的目錄搜索列表。
2. 查找環境變量 `LD_LIBRARY_PATH`，以冒號分割的目錄搜索列表。
3. 查找 `/etc/ld.so.conf` 。
4. 默認路徑 `/lib` 和 `/usr/lib` 。

為了讓執行程序順利找到動態庫，有三種方法：

##### 1. 複製到指定路徑

把庫拷貝到查找路徑下，通常為 `/usr/lib` 和 `/lib` 目錄下，或者通過 `gcc --print-search-dirs` 查看動態庫的搜索路徑。

##### 2. 添加鏈接選項

編譯時添加鏈接選項，指定鏈接庫的目錄，此時會將該路徑保存在二進制文件中。

{% highlight text %}
$ gcc -o test test.o -L. -lhello -Wl,-rpath,/home/lib:.
$ readelf -d test | grep RPATH
$ objdump -s -j .dynstr test                     // 查看.dynstr段的內容
{% endhighlight %}

##### 3. 設置環境變量

執行時在 `LD_LIBRARY_PATH` 環境變量中加上庫所在路徑，例如動態庫 `libhello.so` 在 `/home/test/lib` 目錄下。

{% highlight text %}
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/test/lib
{% endhighlight %}

##### 4. 修改配置文件

修改 `/etc/ld.so.conf` 文件，把庫所在的路徑加到文件中，並執行 `ldconfig` 刷新配置。動態鏈接庫通常保存在 `/etc/ld.so.cache` 文件中，執行 `ldconfig` 可以對其進行刷新。


### 靜態連接庫

當要使用靜態的程序庫時，連接器會找出程序所需的函數，然後將它們拷貝到執行文件，由於這種拷貝是完整的，所以一旦連接成功，靜態程序庫也就不再需要了，缺點是佔用的空間比較大。通常，靜態鏈接的程序要比共享函數庫的程序運行速度上快一些，大概 1-5％ 。

<!--
動態庫會在執行程序內留下一個標記指明當程序執行時，首先必須載入這個庫。
-->

注意，對於 CentOS 需要安裝 `yum install glibc-static` 庫。

Linux 下進行連接的缺省操作是首先連接動態庫，也就是說，如果同時存在靜態和動態庫，不特別指定的話，將與動態庫相連接。

現在假設有一個 hello 程序開發包，它提供一個靜態庫 `libhello.a`，一個動態庫 `libhello.so`，一個頭文件 `hello.h`，頭文件中提供 `foobar()` 這個函數的聲明。

下面這段程序 `main.c` 使用 hello 庫中的 `foobar()` 函數。

{% highlight c %}
/* filename: foobar.c */
#include "hello.h"
#include <stdio.h>
void foobar()
{
   printf("FooBar!\n");
}
{% endhighlight %}

{% highlight c %}
/* filename: hello.c */
#include "hello.h"
#include <stdio.h>
void hello()
{
   printf("Hello world!\n");
}
{% endhighlight %}

{% highlight c %}
/* filename: hello.h */
#ifndef _HELLO_H__
#define _HELLO_H__
void hello();
void foobar();
#endif
{% endhighlight %}

{% highlight c %}
/* filename: main.c */
#include "hello.h"
int main(int argc, char **argv)
{
   foobar();
   hello();
   return 0;
}
{% endhighlight %}

生成靜態庫，先對源文件進行編譯；然後使用 `ar(archive)` 命令連接成靜態庫。

{% highlight text %}
$ gcc -c hello.c -o hello.o
$ gcc -c foobar.c -o foobar.o
$ ar crv libhello.a hello.o foobar.o
$ ar -t libhello.a                              // 查看打包的文件
{% endhighlight %}

`ar` 實際是一個打包工具，可以用來打包常見文件，不過現在被 `tar` 替代，目前主要是用於生成靜態庫，詳細格式可以參考 [ar(Unix) wiki](http://en.wikipedia.org/wiki/Ar_(Unix)) 。

{% highlight text %}
$ echo "hello" > a.txt && echo "world" > b.txt
$ ar crv text.a a.txt b.txt
$ cat text.a
{% endhighlight %}

與靜態庫連接麻煩一些，主要是參數問題。

{% highlight text %}
$ gcc main.c -o test -lhello                    // 庫在默認路徑下，如/usr/lib
$ gcc main.c -lhello -L. -static -o main        // 通過-L指定庫的路徑

$ gcc main.o -o main -WI,-Bstatic -lhello       // 報錯，顯示找不到-lgcc_s
{% endhighlight %}

注意：這個特別的 `"-WI,-Bstatic"` 參數，實際上是傳給了連接器 `ld`，指示它與靜態庫連接，如果系統中只有靜態庫可以不需要這個參數； 如果要和多個庫相連接，而每個庫的連接方式不一樣，比如上面的程序既要和 `libhello` 進行靜態連接，又要和 `libbye` 進行動態連接，其命令應為：

{% highlight text %}
$ gcc testlib.o -o test -WI,-Bstatic -lhello -WI,-Bdynamic -lbye
{% endhighlight %}

最好不要進行分別編譯、鏈接，因為在生成可執行文件時往往需要很多的其他文件，可以通過 `-v` 選項進行查看，如果通過如下方式進行編譯通常會出現錯誤。

{% highlight text %}
$ gcc -c main.c
$ ld main.o -L. -lhello
{% endhighlight %}

### 動態庫

用 gcc 編繹該文件，在編繹時可以使用任何編繹參數，例如 `-g` 加入調試代碼等；`-fPIC` 生成與位置無關的代碼（可以在任何地址被連接和裝載）。

{% highlight text %}
$ gcc -c -fPIC hello.c -o hello.o

$ gcc -shared -Wl,-soname,libhello.so.1 -o libhello.so.1.0 hello.o // 生成動態庫，可能存在多個版本，通常指定版本號

$ ln  -s  libhello.so.1.0  libhello.so.1                           // 另外再建立兩個符號連接
$ ln  -s  libhello.so.1  libhello.so

$ gcc -fPIC -shared -o libhello.so hello.c                         // 當然對於上述的步驟可以通過一步完成

$ readelf -d libhello.so.1.0 | grep SONAME                         // 查看對應的soname
$ nm -D libhello.so                                                // 查看符號
{% endhighlight %}

最重要的是傳 `-shared` 參數使其生成是動態庫而不是普通執行程序； `-Wl` 表示後面的參數也就是 `-soname,libhello.so.1` 直接傳給連接器 `ld` 進行處理。

實際上，每一個庫都有一個 `soname` ，當連接器發現它正在查找的程序庫中有這樣一個名稱，連接器便會將 `soname` 嵌入連結中的二進制文件內，而不是它正在運行的實際文件名，在程序執行期間，程序會查找擁有 `soname` 名字的文件，而不是庫的文件名，換句話說，`soname` 是庫的區分標誌。

其目的主要是允許系統中多個版本的庫文件共存，習慣上在命名庫文件的時候通常與 `soname` 相同 `libxxxx.so.major.minor` 其中，`xxxx` 是庫的名字， `major` 是主版本號， `minor` 是次版本號。

### 查看庫中的符號

有時候可能需要查看一個庫中到底有哪些函數，`nm` 命令可以打印出庫中的涉及到的所有符號，庫既可以是靜態的也可以是動態的。

`nm` 列出的符號有很多，常見的有三種：

* 在庫中被調用，但並沒有在庫中定義(表明需要其他庫支持)，用U表示；
* 庫中定義的函數，用T表示，這是最常見的；
* 所謂的“弱態”符號，它們雖然在庫中被定義，但是可能被其他庫中的同名符號覆蓋，用W表示。

例如，希望知道上文提到的 `hello` 庫中是否定義了 `printf()` 。

{% highlight text %}
$ nm libhello.so
{% endhighlight %}

發現其中沒有 `printf()` 的定義，取而代之的是 `puts()` 函數，而且為 `U` ，表示符號 `puts` 被引用，但是並沒有在函數內定義，由此可以推斷，要正常使用 `hello` 庫，必須有其它庫支持，再使用 `ldd` 命令查看 `hello` 依賴於哪些庫：

{% highlight text %}
$ ldd -v hello
$ readelf -d hello     直接使用readelf
{% endhighlight %}

每行 `=>` 前面的，為動態鏈接程序所需的動態鏈接庫的名字；而 `=>` 後面的，則是運行時系統實際調用的動態鏈接庫的名字。所需的動態鏈接庫在系統中不存在時，`=>` 後面將顯示 `"not found"`，括號所括的數字為虛擬的執行地址。

常用的系統動態鏈接庫有：

{% highlight text %}
libc.so.x        基本C庫
ld-linux.so.2    動態裝入庫(用於動態鏈接庫的裝入及運行)
{% endhighlight %}


## 動態庫加載API

對於 Linux 下的可執行文件 ELF 使用如下命令查看，可以發現其中有一個 `.interp` 段，它指明瞭將要被使用的動態鏈接器 (`/lib/ld-linux.so`)。

{% highlight text %}
$ readelf -l EXECUTABLE
{% endhighlight %}

對於動態加載的函數主要包括了下面的四個函數，需要 `dlfcn.h` 頭文件，定義在 `libdl.so` 庫中。

{% highlight text %}
void *dlopen( const char *file, int mode );
  用來打開一個文件，使對象文件可被程序訪問，同時還會自動解析共享庫中的依賴項，這樣，如果打開了一個
    依賴於其他共享庫的對象，它就會自動加載它們，該函數返回一個句柄，該句柄用於後續的 API 調用。
  mode 參數通知動態鏈接器何時執行再定位，有兩個可能的值：
    A) RTLD_NOW，表明動態鏈接器將會在調用 dlopen 時完成所有必要的再定位；
    B) RTLD_LAZY，只在需要時執行再定位。

void *dlsym( void *restrict handle, const char *restrict name );
  通過句柄和連接符名稱獲取函數名或者變量名。

char *dlerror();
  返回一個可讀的錯誤字符串，該函數沒有參數，它會在發生前面的錯誤時返回一個字符串，在沒有錯誤發生時返回NULL。

char *dlclose( void *handle ); 
  通知操作系統不再需要句柄和對象引用了。它完全是按引用來計數的，所以同一個共享對象的多個用戶相互間
    不會發生衝突（只要還有一個用戶在使用它，它就會待在內存中）。
    任何通過已關閉的對象的 dlsym 解析的符號都將不再可用。
{% endhighlight %}

有了 ELF 對象的句柄，就可以通過調用 dlsym 來識別這個對象內的符號的地址了。該函數採用一個符號名稱，如對象內的一個函數的名稱，返回值為對象符號的解析地址。

下面是一個動態加載的示例 [github libdl.c]({{ site.example_repository }}/c_cpp/c/libdl.c)，通過如下的命令進行編譯，其中選項 `-rdynamic` 用來通知鏈接器將所有符號添加到動態符號表中（目的是能夠通過使用 dlopen 來實現向後跟蹤）。

{% highlight text %}
$ gcc -rdynamic -o dl library_libdl.c -ldl        # 編譯
$ ./dl                                            # 測試
> libm.so cosf 0.0
   1.000000
> libm.so sinf 0.0
   0.000000
> libm.so tanf 1.0
   1.557408
> bye
{% endhighlight %}

另外，可以通過如下方式簡單使用。

{% highlight text %}
$ cat caculate.c                                     # 查看動態庫源碼
int add(int a, int b) {
    return (a + b);
}
int sub(int a, int b) {
    return (a - b);
}
$ gcc -fPIC -shared caculate.c -o libcaculate.so     # 生成動態庫
$ cat foobar.c                                       # 測試源碼
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef int (*CAC_FUNC)(int, int);                           // 定義函數指針類型
int main(int argc, char** argv) {
    void *handle;
    char *error;
    CAC_FUNC cac_func = NULL;

    if ( !(handle=dlopen("./libcaculate.so", RTLD_LAZY)) ) { // 打開動態鏈接庫
        fprintf(stderr, "!!! %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    cac_func = dlsym(handle, "add");                         // 獲取一個函數
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "!!! %s\n", error);
        exit(EXIT_FAILURE);
    }
    printf("add: %d\n", (cac_func)(2,7));

    dlclose(handle);                                         // 關閉動態鏈接庫
    exit(EXIT_SUCCESS);
}
$ gcc -rdynamic -o foobar foobar.c -ldl              # 編譯測試
{% endhighlight %}

<!-- https://www.ibm.com/developerworks/cn/linux/l-elf/part1/index.html -->

## 常用命令

### objdump

詳細參考 `man objdump` 。

{% highlight text %}
-h, --section-headers, --headers
  查看目標文件的頭部信息。
-x, --all-headers
  顯示所有的頭部信息，包括了符號表和重定位表，等價於 -a -f -h -p -r -t 。
-s, --full-contents
  顯示所請求段的全部信息，通常用十六進製表示，默認只會顯示非空段。
-d, --disassemble
  反彙編，一般只反彙編含有指令的段。
-t, --syms
  顯示符號表，與nm類似，只是顯示的格式不同，當然顯示與文件的格式相關，對於ELF如下所示。
  00000000 l    d  .bss   00000000 .bss
  00000000 g       .text  00000000 fred
{% endhighlight %}

<!--
第一列為符號的值，有時是地址；下一個是用字符表示的標誌位；接著是與符號相關的段，*ABS* 表示段是絕對的（沒和任何段相關聯）， *UND* 表示未定義；對於普通符號(Common Symbols)表示對齊，其它的表示大小；最後是符號的名字。<br><br>

對於標誌組的字符被分為如下的 7 組。
<ol type="A"><li>
    "l(local)" "g(global)" "u(unique global)" " (neither global nor local)" "!(both global and local)"<br>
    通常一個符號應該是 local 或 global ，但還有其他的一些原因，如用於調試、"!"表示一個bug、"u"是 ELF 的擴展，表示整個進程中只有一個同類型同名的變量。</li><br><li>

    "w(weak)" " (strong)"<br>
    表示強或弱符號。</li><br><li>

    "C(constructor)" " (ordinary)"<br>
    為構造函數還是普通符號。</li><br><li>

    "W(warning)" " (normal symbol)"<br>
    如果一個含有警告標誌的符號被引用時，將會輸出警告信息。</li><br><li>

    "I"
   "i" The symbol is an indirect reference to another symbol (I), a function to be evaluated
       during reloc processing (i) or a normal symbol (a space).

   "d(debugging symbol)" "D(dynamic symbol)" " (normal symbol)"<br>
    表示調試符號、動態符號還是普通的符號。</li><br><li>

   "F(function)" "f(file)" "O(object)" " (normal)"<br>
    表示函數、文件、對象或只是一個普通的符號。
-->

### strip

我們知道二進制的程序中包含了大量的符號表格(symbol table)，有一部分是用來 gdb 調試提供必要信息的，可以通過如下命令查看這些符號信息。

{% highlight text %}
$ readelf -S hello
{% endhighlight %}

其中類似與 `.debug_xxxx` 的就是 gdb 調試用的。去掉它們不會影響程序的執行。

{% highlight text %}
$ strip hello
{% endhighlight %}

### objcopy

用於轉換目標文件。

{% highlight text %}
常用參數：
  -S / --strip-all
    不從源文件中拷貝重定位信息和符號信息到輸出文件(目的文件)中去。
  -I bfdname/--input-target=bfdname
    明確告訴程序源文件的格式是什麼，bfdname是BFD庫中描述的標準格式名。
  -O bfdname/--output-target=bfdname
    使用指定的格式來寫輸出文件(即目標文件)，bfdname是BFD庫中描述的標準格式名，
    如binary(raw binary 格式)、srec(s-record 文件)。
  -R sectionname/--remove-section=sectionname
    從輸出文件中刪掉所有名為section-name的段。
{% endhighlight %}

上一步的 strip 命令只能拿掉一般 symbol table，有些信息還是沒拿掉，而這些信息對於程序的最終執行沒有影響，如: `.comment` `.note.ABI-tag` `.gnu.version` 就是完全可以去掉的。

所以說程序還有簡化的餘地，我們可以使用 objcopy 命令把它們抽取掉。

{% highlight text %}
$ objcopy -R .comment -R .note.ABI-tag -R .gnu.version hello hello1
{% endhighlight %}

### readelf

用於讀取 ELF 格式文件，包括可執行程序和動態庫。

{% highlight text %}
常用參數：
  -a --all
    等價於-h -l -S -s -r -d -V -A -I
  -h --file-header
    文件頭信息；
  -l --program-headers
    程序的頭部信息；
  -S --section-headers
    各個段的頭部信息；
  -e --headers
    全部頭信息，等價於-h -l -S；

示例用法：
----- 讀取dynstr段，包含了很多需要加載的符號，每個動態庫後跟著需要加載函數
$ readelf -p .dynstr hello
----- 查看是否含有調試信息
$ readelf -S hello | grep debug
{% endhighlight %}

<!--
readelf  -S hello
readelf -d hello

  --sections
An alias for –section-headers
-s –syms 符號表 Display the symbol table
--symbols
An alias for –syms
-n –notes 內核註釋 Display the core notes (if present)
-r –relocs 重定位 Display the relocations (if present)
-u –unwind Display the unwind info (if present)
-d --dynamic
  顯示動態段的內容；
-V –version-info 版本 Display the version sections (if present)
-A –arch-specific CPU構架 Display architecture specific information (if any).
-D –use-dynamic 動態段 Use the dynamic section info when displaying symbols
-x –hex-dump=<number> 顯示 段內內容Dump the contents of section <number>
-w[liaprmfFso] or
-I –histogram Display histogram of bucket list lengths
-W –wide 寬行輸出 Allow output width to exceed 80 characters
-H –help Display this information
-v –version Display the version number of readelf
-->


## 其它

#### 1. 靜態庫生成動態庫

可以通過多個靜態庫生成動態庫，而實際上靜態庫是一堆 `.o` 庫的壓縮集合，而生成動態庫需要保證 `.o` 編譯後是與地址無關的，也就是添加 `-fPIC` 參數。


{% highlight text %}
{% endhighlight %}

