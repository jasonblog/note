---
title: Linux 自動編譯
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,automake,configure,cmake
description: 在 Linux 中，經常使用 GNU 構建系統，也就是利用腳本和 make 程序在特定平臺上構建軟件，這種方式幾乎成為一種習慣，被廣泛使用。這裡簡單介紹下這種構建方式的細節，以及開發者如何利用 autotools 創建兼容 GNU 構建系統的項目。
---

在 Linux 中，經常使用 GNU 構建系統，也就是利用腳本和 make 程序在特定平臺上構建軟件，這種方式幾乎成為一種習慣，被廣泛使用。

這裡簡單介紹下這種構建方式的細節，以及開發者如何利用 autotools 創建兼容 GNU 構建系統的項目。

<!-- more -->

## 簡介

在 Linux 平臺下，在編譯項目時通常使用 Makefile，簡單來說，Makefile 是用來定義整個工程的編譯規則，也就是文件的編譯、鏈接順序，如何生成可執行文件或者動態加載庫等等。

Makefile 帶來的最大好處就是 "自動化編譯"，一但編輯好文件，只需要一個 make 命令，整個工程完全自動編譯，極大的提高了軟件開發的效率。

如果項目比較小，那麼可以直接手動編輯該文件；不過一但項目比較大時，手動維護將變得極其複雜，為此，就可以使用 Autotools 或者 CMake 生成 Makefile 文件。

假設項目工程的使用源碼結構如下。

{% highlight text %}
src/
   子目錄，存放工程源碼；
doc/
   子目錄，用來存放工程文檔；
bin/
   子目錄，最後生成的二進制可執行文件；
COPYRIGHT
   版權信息；
README.md
   使用 markdown 編寫的自述文件；
{% endhighlight %}

默認，會將可執行文件安裝在 /usr/bin 目錄下，doc 安裝到 /usr/share/doc 目錄下。


## Makefile

詳細內容可以參考陳皓編寫的 [跟我一起寫 Makefile](/reference/linux/Makefile.pdf)，在此就不做過多介紹了，僅簡單記錄下。


### 變量

如下是 Makefile 中內置的變量。

{% highlight text %}
$@:  規則中的目標名（也就是規則名）；
$<:  規則中的依賴項目,只代表規則所有依賴項目中的第一項；
$^:  規則中所有的依賴項目；
$?:  規則中時間新於目標的依賴項目。
{% endhighlight %}

關於變量通配符，與 shell 相同，如 1) `?` 任意單個字符；2) `*` 任意字符的字符串；3) `[set]` 任何在 set 裡的字符；4) `[!set]` 任何不在 set 裡的字符。

以如下為例 `touch {a,b,c}.c Makefile` 。

{% highlight makefile %}
.PHONY: all

src1=$(wildcard *.c)
src2=%.c
src3=*.c
objs:=$(patsubst $(src2),%.o,$(wildcard *.c))

all: $(objs)
    @echo $^
    @echo $(src1)
    @echo $(src2)
    @echo $(src3)
    @echo *.c

%o:%.c
    @echo $?

#cc    -c -o a.o a.c
#cc    -c -o c.o c.c
#cc    -c -o b.o b.c
#a.o c.o b.o
#a.c c.c b.c
#%.c
#a.c b.c c.c
#a.c b.c c.c
{% endhighlight %}


### PHONY 偽目標

通常來說 Makefile 會檢測 `:` 左側的目標是不是最新的，如果是最新的則不會更新，對應規則不會執行。假設目標為 clean，本意是做編譯後的清理，但是當目錄下有 clean 文件時，則對應的規則將不會執行，為了解決這一問題，定義了偽目標。

當定義了偽目標之後，make 在執行規則時不會去試圖去查找隱含規則來創建它，而是直接執行，這樣就提高了 make 的執行效率，也不用擔心由於目標和文件名重名了。

偽目標的另一種使用場合時在 make 的並行和遞歸執行過程中，第一個實際上時串行執行的；第二個會並行執行。

{% highlight text %}
### 1
SUBDIRS=foo bar baz
subdirs:
    for dir in $(SUBDIRS)
    do
    $(MAKE) –C $$dir
    done

### 2
.PHONY:subdirs $(SUBDIRS)
SUBDIRS=foo bar baz
subdirs: $(SUBDIRS)
$(SUBDIRS):
    $(MAKE) –C $@
{% endhighlight %}



## Autotools

在 Linux 平臺上，經常使用 `configure->make->make install` 從源碼開始編譯安裝，也就是 GNU 構建系統，利用腳本和 make 程序在特定平臺上編譯軟件，也就是利用 autotools 創建構建系統的項目。

注意，有些程序雖然也是採用上述的三步，但並未採用 autotools 實現，如 nginx 是作者編寫的構建程序。

在 CentOS 中可以直接通過 `yum install automake autoconf` 安裝相關的文件；利用 autotools 生成 Makefile 的過程如下圖所示。

![autotools]({{ site.url }}/images/linux/autotools-process-1.png "autotools"){: .pull-center }

其中用到的核心工具包括了 Autoconf 和 Automake ，首先從用戶和開發者角度看看兩者的區別。關於 autotools 的簡單處理流程可以參考 [automake](http://www.gnu.org/software/automake/manual/automake.html) 中的 `Setup Explained` 內容。

### 用戶視角

`configure` 腳本是由軟件開發者維護併發布給用戶使用的 shell 腳本，該腳本作用是檢測系統環境，最終目的是生成 Makefile 和 config.h 兩個文件。

開發者在分發源碼包時，除了源代碼 (.c .h)，還有許多支撐軟件構建的文件和工具，其中最重要的文件就是 Makefile.in 和 config.h.in 兩個，在 configure 腳本執行成功後，將為每個 `*.in` 文件處理成對應的非 `*.in` 文件。

#### configure

configure 腳本會檢查當前系統，而檢查項的多少取決於開發者，一般來說，主要檢查當前目標平臺的程序、庫、頭文件、函數等的兼容性，而檢查結果將作用於 config.h 和 Makefile 文件的生成，從而影響最終的編譯。

用戶可通過參數定製軟件所需要包含的組件、安裝路徑等，一般會被五部分，可以通過 `--help` 參數查看當前軟件提供了那些配置參數。

<!--
    *安裝路徑相關配置。最常見的是--prefix。
    *程序名配置。例如--program-suffix可用於為生成的程序添加後綴。
    *跨平臺編譯。不太常用。
    *動態庫靜態庫選項。用於控制是否生成某種類型的庫文件。
    程序組件選項。用於配置程序是否將某種功能編譯到程序中，一般形如--with-xxx。這可能是最常用的配置，而且由軟件開發者來定義。
（*表示這是幾乎所有軟件都支持的配置，因為這些配置是autotool生成的configure腳本默認支持的。）
-->

在 configure 在執行過程中，除了生成 Makefile 外，還會生成如下的臨時文件：

* config.log 日誌文件；
* config.cache 緩存，以提高下一次 configure 的速度，需通過 -C 來指定才會生成；
* config.status 實際調用編譯工具構建軟件的 shell 腳本。

如果軟件通過 libtool 構建，還會生成 libtool 腳本，關於 libtool 腳本如何生成，詳見如下。


### 開發者視角

開發者除了編寫軟件本身的代碼外，還需要負責生成構建軟件所需要文件和工具，通過 autotools 工具可以解決一些常見的平臺問題，但是編寫依舊複雜。為了生成 configure 腳本和 Makefile.in 等文件，開發者需要創建並維護一個 configure.ac 文件，以及一系列的 Makefile.am 文件。<!--autoreconf程序能夠自動按照合理的順序調用autoconf automake aclocal等程序。-->

#### Autoconf

該工具用於生成一個可在類 Unix 系統下工作的 Shell 腳本，該腳本可在不同 *nix 平臺下自動配置軟件源代碼包，也就是 configure 文件，生成腳本後與 autoconf 再無關係。

<!--
對於每個使用了Autoconf的軟件包，Autoconf從一個列舉了該軟件包需要的，或者可以使用的系統特徵的列表的模板文件中生成配置腳本。在 shell代碼識別並響應了一個被列出的系統特徵之後，Autoconf允許多個可能使用（或者需要）該特徵的軟件包共享該特徵。 如果後來因為某些原因需要調整shell代碼，就只要在一個地方進行修改； 所有的配置腳本都將被自動地重新生成以使用更新了的代碼。
-->

#### Automake

該工具通過 Makefile.am 文件自動生成 Makefile.in 文件，而 Makefile.am 基本上是一系列 make 宏定義，該文件需要手動編輯，也就是將對 Makefile 的編輯轉義到了 Makefile.am 文件。

<!--
Automake 支持三種目錄層次： “flat”、“shallow”和“deep”。一個flat（平）包指的是所有文件都在一個目錄中的包。為這類包提供的`Makefile.am' 缺少宏SUBDIRS。這類包的一個例子是termutils。一個deep（深）包指的是所有的源代碼都被儲存在子目錄中的包；頂層 目錄主要包含配置信息。GNU cpio 是這類包的一個很好的例子，GNU tar也是。deep包的頂層`Makefile.am'將包括 宏SUBDIRS，但沒有其它定義需要創建的對象的宏。一個shallow（淺）包指的是主要的源代碼儲存在頂層目錄中，而 各個部分（典型的是庫）則儲存在子目錄中的包。Automake本身就是這類包（GNU make也是如此，它現在已經不使用automake）。
-->

### 常見文件

接下來看下一些常用的文件。

#### configure.ac

用於通過 autoconf 命令生成 configure 腳本，如下是一個 configure.ac 的示例：

{% highlight text %}
AC_PREREQ([2.69])
AM_INIT_AUTOMAKE(hello, 1.0)
AC_INIT([hello], [1.0], [www.douniwan.com])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
# Checks for libraries.
# Checks for header files.
# Checks for typedefs, structures, and compiler characteristics.
# Checks for library functions.
AC_CONFIG_FILES([Makefile                     # 主要是通過*.in模板生成響應的文件
                 src/Makefile
                 src/a/Makefile
                 src/b/Makefile])
AC_OUTPUT
{% endhighlight %}

以 `AC_` 開頭的是一些宏調用，與 C 中的宏概念類似，會被替換展開；很多以 AC_PROG_XXX 開頭的宏用於檢查所需要的程序是否存在，詳細可以查看 [Particular Program Checks](https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Particular-Programs.html)；對於一些特殊的函數或者文件則可以通過 [Generic Program and File Checks](https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Generic-Programs.html) 中定義的宏進行檢查。

而 m4 是一個經典的宏工具，autoconf 正是構建在 m4 之上，可以簡單理解為 autoconf 預先實現了大量用於檢測系統可移植性的宏，這些宏在展開後就是大量的 shell 腳本。所以編寫 configure.ac 需要對這些宏熟練掌握，並且合理調用，有時，甚至可以自己實現自己的宏。

#### configure.scan

通過 autoscan 命令可以得到一個初始化的 configure.scan 文件，然後重命名為 configure.ac 後，在此基礎上編輯 configure.ac ，而 autoscan 通常 **只用於首次初始化 configure.ac** 。

autoscan 會掃描源碼，並生成一些通用的宏調用、輸入的聲明以及輸出的聲明。

#### config.h.in

可以通過 autoheader 命令掃描 configure.ac 中的內容，並生成 config.h.in 文件；每當 configure.ac 文件有所變化，都可以再次執行 autoheader 更新 config.h.in 。

在 configure.ac 中通過 `AC_CONFIG_HEADERS([config.h])` 告訴 autoheader 應當生成 config.h.in 的路徑；在最後的實際編譯階段，生成的編譯命令會加上 `-DHAVE_CONFIG_H` 定義宏。

{% highlight text %}
/bin/sh ../../libtool --tag=CC --mode=compile gcc -DHAVE_CONFIG_H ...
{% endhighlight %}

於是在代碼中，可以通過下面代碼安全的引用 config.h 。

{% highlight c %}
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
{% endhighlight %}

config.h 包含了大量的宏定義，其中包括軟件包的名字等信息，程序可以直接使用這些宏；更重要的是，程序可以根據其中的對目標平臺的可移植性相關的宏，通過條件編譯，動態的調整編譯行為。

#### Makfile.am

手工編寫 Makefile 是一件相當煩瑣的事情，而且，如果項目複雜的話，編寫難度將越來越大；為此，可以通過 automake+Makefile.am 生成 Makefile.in 文件，通常一個 Makefile.am 的示例如下。

{% highlight text %}
SUBDIRS         = a b
bin_PROGRAMS    = st
st_SOURCES      = main.c
st_LDADD        = $(top_builddir)/src/a/liba.la $(top_builddir)/src/b/libb.la
{% endhighlight %}

通過 SUBDIRS 聲明瞭兩個子目錄，子目錄的中的構建需要靠 a/Makefile.am 和 b/Makefile.am 來進行，這樣多目錄組織起來就方便多了。

bin_PROGRAMS 聲明一個可執行文件，st_SOURCES 指定所依賴的源代碼文件，st_LDADD 聲明瞭可執行文件在連接時，需要依賴的 Libtool 庫文件。

由於 automake 晚於 autoconf，所以 automake 是作為 autoconf 的擴展來實現的，在 configure.ac 中需要通過聲明 AM_INIT_AUTOMAKE 告訴 autoconf 需要配置和調用 automake 。


#### aclocal

如上所述，configure.ac 是依靠宏展開來得到 configure 的，因此，能否成功生成取決於宏定義能否找到；默認 autoconf 會從安裝路徑下來尋找事先定義好了宏，而對於像 automake、libtool 和 gettext 等第三方擴展宏，甚至是開發者自行編寫的宏就一無所知了。

於是，存在這個工具 aclocal，將在 configure.ac 同一目錄下生成 aclocal.m4，在掃描 configure.ac 的過程中，將第三方擴展和開發者自己編寫的宏定義複製進去；這樣，autoconf 在遇到不認識的宏時，就會從 aclocal.m4 中查找。


#### libtool

libtool 試圖解決不同平臺下庫文件的差異，實際是一個 shell 腳本，實際工作過程中，調用了目標平臺的 cc 編譯器和鏈接器，以及給予合適的命令行參數，libtool 可以單獨使用。

automake 支持 libtool 構建聲明，在 Makefile.am 中，普通的庫文件目標寫作 xxx_LIBRARIES ：

{% highlight text %}
noinst_LIBRARIES = liba.a
liba_SOURCES = ao1.c ao2.c ao3.c
{% endhighlight %}

而對於 libtool 目標，寫作 xxx_LTLIBRARIES，並以 .la 作為後綴聲明庫文件。

{% highlight text %}
noinst_LTLIBRARIES = liba.la
liba_la_SOURCES = ao1.c ao2.c ao3.c
{% endhighlight %}

在 configure.ac 中需要聲明 LT_INIT：

{% highlight text %}
...
AM_INIT_AUTOMAKE([foreign])
LT_INIT
...
{% endhighlight %}

有時，如果要用到 libtool 中的某些宏，則推薦將這些宏複製到項目中。首先，通過 `AC_CONFIG_MACRO_DIR([m4])` 指定使用 m4 目錄存放第三方宏；然後在最外層的 Makefile.am 中加入 `ACLOCAL_AMFLAGS = -I m4` 。

<!--
https://segmentfault.com/a/1190000006915719


Configure、Makefile.am、Makefile.in、Makefile之間的關係
http://www.51cos.com/?p=1649

可以參考官方文檔
http://inti.sourceforge.net/tutorial/libinti/autotoolsproject.html


通過--with-libmysql=/opt/mysql/lib指定MySQL庫的路徑；--enable-mysql編譯MySQL插件；

需要手動編譯：
1. Makefile.am
用於生成Makefile.in文件，需要手動書寫。

[s]bin_PROGRAMS  需要生成的可執行文件的文件名，多個文件以空格分割；
foobar_SOURCES 指定生成命令所需要的文件，包括源碼以及頭文件；

1. autoscan(autoconf)
掃描源代碼以搜尋普通的可移植性問題，比如檢查編譯器，庫，頭文件等，生成文件configure.scan，然後手動編輯生成configure.ac。
your source files --- [autoscan*] --- [configure.scan] -<edit>- configure.ac
常用函數

#### AC_ARG_WITH()

AC_ARG_WITH (package, help-string, [action-if-given], [action-if-not-given])
此時就是可以命令行中使用--with-package或者--without-package進行配置，其中第三個參數需要檢查yes/no；配置完成之後可以通過`./configure --help`查看。
AC_ARG_WITH(libmysql, [AS_HELP_STRING([--with-libmysql@<:@=PREFIX@:>@], [Path to libmysql.])],
[
  if test "x$withval" = "xno"; then
    with_libmysql="no"
  else if test "x$withval" = "xyes"; then
    with_libmysql="yes"
  else
    if test -f "$withval" && test -x "$withval";
    then
      with_mysql_config="$withval"
    else if test -x "$withval/bin/mysql_config"
    then
      with_mysql_config="$withval/bin/mysql_config"
    fi; fi
    with_libmysql="yes"
  fi; fi
],
[
  with_libmysql="yes"
])
# 接下來可以通過shell腳本再進行一些檢查，也可以使用AC_CHECK_LIB()、AC_CHECK_HEADERS()函數，失敗則可以用AC_MSG_FAILURE()打印信息，暫時省略
if test "x$with_libmysql" = "xyes"
then
  BUILD_WITH_LIBMYSQL_CFLAGS="$with_mysql_cflags"
  AC_SUBST(BUILD_WITH_LIBMYSQL_CFLAGS)
  AC_SUBST(BUILD_WITH_LIBMYSQL_LIBS, "$with_mysql_libs")
fi
# 設置條件變量，可以再Makefile.am中使用
AM_CONDITIONAL(BUILD_WITH_LIBMYSQL, test "x$with_libmysql" = "xyes")
-->



### Flat

這一模式就是所有的文件，包括源碼、頭文件等，都在同一目錄下。

如下示例中，目錄下存在三個文件：hello.h (聲明hello()方法)、hello.c (實現hello()方法)、main.c (主文件調用了hello()方法)。執行方法如下：

<ol><li>
autoscan<br>在目錄下，直接執行該命令，然後會生成 configure.scan 和 autoscan.log (可刪除) 兩個文件，將 configure.scan 重命名為 <font color="blue">configure.ac</font> 文件 (注意，不是 configure.in，這個是老版本)，修改文件的內容。
{% highlight text %}
AC_PREREQ([2.69])                             # 設置Autoconf的最小版本
AM_INIT_AUTOMAKE(hello, 1.0)                  # 表示用於automake調用，可為AM_INIT_AUTOMAKE
AC_INIT([hello], [1.0], [www.douniwan.com])   # 設置包名稱、版本、bug報告地址
AC_CONFIG_SRCDIR([main.c])                    # 可為包中的任意源碼，用於判斷是否存在源碼
AC_CONFIG_HEADERS([config.h])                 # 表示在調用automake時依賴config.h.in文件，如果不定義則會
                                              # 在編譯時會有大量的-DXXX=xxxd的定義參數
AC_PROG_CC                                    # 檢測是否存在C編譯器
AC_PROG_CXX                                   # 檢測是否存在C++編譯器
AC_OUTPUT(Makefile)                           # 指定configure輸出的文件
{% endhighlight %} </li><li>

aclocal<br>
是一個 perl 腳本，用於生成 aclocal.m4 文件供 automake 和 autoconf 調用，詳見 info aclocal。</li><li>

autoheader<br>用於產生 config.h.in 文件，感覺應該是配置通過 configure 輸入 config.h 文件時需要定義那些宏。</li><li>

automake --add-missing<br>
新建文件<font color="blue">Makefile.am</font>文件，通過Makefile.am生成Makefile.in供configure腳本調用生成Makefile。

{% highlight text %}
AUTOMAKE_OPTIONS=foreign                     # 不遵循gnu的標準
bin_PROGRAMS=hello                           # 最後生成的程序，多個空格間隔
hello_LDFLAGS = -Wall                        # 編譯選項，只用於可執行文件以及shared library
hello_SOURCES=main.c hello.c hello.h

noinst_LIBRARIES = libfoo.a
libfoo_a_CPPFLAGS = -g -Wall
libfoo_a_LIBADD = lpthread -lm            // 用於靜態鏈接庫
libfoo_a_SOURCES =  cmds.c cmds.h \
                    app.c app.h
foo_SOURCES = main.c
foo_LDADD = libfoo
{% endhighlight %}

autoconf<br>
主要是用來檢測所依賴的條件是否成立，檢測的結果可以保存為文件供其它應用使用，其腳本實際使用shell和M4(可以C語言中宏的加強版)組成，還有部分宏是通過aclocal生成的，保存在aclocal.m4中。<br><br>
在生成configure腳本的時候同時會保存日誌方便調試，第一次執行完之後可以通過config.status更新。通常是用來生成一個配置文件，否則通過編譯參數進行配置，此時編譯選項將非常大。</li><br><li>

configure<br>
到現在已經生成了 configure 文件，通過 ./configure 即可以生成Makefile文件。</li><br><li>

Makefile<br>
主要包括瞭如下的選項，install安裝，clean清除，dist打包軟件發佈，distcheck對發佈的包進行檢測。
</li></ol>
編輯 configure.ac 時，可以使用 case；其中 dnl 用於表示註釋。<br><br>



### 其它

在正式執行 configure 之前，會調用 `AC_DEFUN()` 執行一些函數的擴展，然後調用 `AC_INIT()` 執行初始化操作；在 `AC_INIT()` 宏中，會初始化一些變量參數值，包括 `AC_PACKAGE_NAME`、`PACKAGE_NAME` 等。

{% highlight text %}
AC_INIT (package, version, [bug-report], [tarname], [url])
AC_INIT ([collectd], [m4_esyscmd(./version-gen.sh)])
{% endhighlight %}

<!--
AC_PACKAGE_NAME, PACKAGE_NAME
  Exactly package.
AC_PACKAGE_TARNAME, PACKAGE_TARNAME
  Exactly tarname, possibly generated from package.
AC_PACKAGE_VERSION, PACKAGE_VERSION
  Exactly version.
AC_PACKAGE_STRING, PACKAGE_STRING
  Exactly ‘package version’.
AC_PACKAGE_BUGREPORT, PACKAGE_BUGREPORT
  Exactly bug-report, if one was provided.
AC_PACKAGE_URL, PACKAGE_URL
  Exactly url, if one was provided. If url was empty, but package begins with ‘GNU ’, then this defaults to ‘http://www.gnu.org/software/tarname/’, otherwise, no URL is assumed.
-->



#### 修改configure.ac

需要執行 `autoreconf -ivf` 命令來更新 configure 腳本文件，autoreconf 可以看做是 autoconf、autoheader、acloacl、automake、libtoolize、autopoint 的組合體，而且會以合適的順序來執行。

注意，在修改了 acinclude.m4、configure.ac 之後，就需要通過 aclocal 重新生成 aclocal.m4 文件；如果直接通過 autoconf 配置，部分庫會報 `aclocal-1.14: command not found` 錯誤。據說 [是由於時間戳導致](http://stackoverflow.com/questions/33278928/how-to-overcome-aclocal-1-15-is-missing-on-your-system-warning-when-compilin) 的，不過安裝了 automake、autoconf 工具後，最簡單的方式是執行 autoreconf 命令。

#### 修改Makefile.am

需要在源碼的跟目錄下執行 automake 命令更新，然後在 build 目錄下直接重新執行 make 命令即可。


#### 非標準庫

如果有些庫沒有安裝在標準路徑下，或者需要使用不同的版本庫，那麼就需要通過參數指定；以使用 MySQL 庫為例，通常可以通過 `--with-libmysql=/opt/mysql/lib` 參數指定特定庫路徑。

當然，這需要在製作 configure.ac 文件時通過 `AC_ARG_WITH()` 函數指定；如果使用 `./configure --help` 查看時會發現沒有提供該 `--with-libmysql` 參數，這時候就需要通過如下方式指定。

{% highlight text %}
----- 通過指定環境變量方式添加庫以及頭文件搜索路徑
env CPPFLAGS="-I/include/path"  LDFLAGS="-L/lib/path"  ./configure --prefix=/opt/foobar

----- 也可以把env省掉，作用相同
CPPFLAGS="-I/include/path"  LDFLAGS="-L/lib/path"  ./configure --prefix=/...
{% endhighlight %}

另外，還可以通過 `LIBRARY_PATH` 環境變量指定編譯期間搜索 lib 庫的路徑，使用冒號分割，此時會先搜索該變量指定的路徑，如果找不到則搜索系統默認搜索路徑；而 `LD_LIBRARY_PATH` 則用於指定程序運行期間查找so動態鏈接庫的搜索路徑。

#### 測試功能

automake 提供了簡單測試功能，在運行程序時返回非 0 則認為失敗，否則認為成功；示例如下，需要在 `Makefile.am` 中添加如下內容，然後通過 `make check` 執行檢查，相關可以參考 [Tests](https://www.gnu.org/software/automake/manual/html_node/Tests.html) 。

{% highlight text %}
TESTS = check_money
check_PROGRAMS = check_money
check_money_SOURCES = check_money.c $(top_builddir)/src/money.h
check_money_CFLAGS = @CHECK_CFLAGS@
check_money_LDADD = $(top_builddir)/src/libmoney.la @CHECK_LIBS@
{% endhighlight %}

另外，可以在 `Makefile.am` 中，可以增加其它功能測試，例如內存洩露，增加如下內容，詳細參考 [Parallel Test Harness](https://www.gnu.org/software/automake/manual/html_node/Parallel-Test-Harness.html) 。

<!-- LOG_COMPILER = env VALGRIND="@VALGRIND@" $(abs_top_srcdir)/testwrapper.sh  -->

#### LIBADD VS. LDADD

簡單來說 `LIBADD` 用於庫，`LDADD` 用於可執行文件。

例如，要通過 libtool 生成一個 libfoo.la 靜態庫，那麼可以使用如下方式。

{% highlight text %}
libfoo_la_LIBADD = libbar.la
{% endhighlight %}

如果使用了非 libtool 庫，那麼就需要通過 `-L` 和 `-l` 參數選項。

{% highlight text %}
libfoo_la_LIBADD = libbar.la -L/opt/local/lib -lpng
{% endhighlight %}

不過一般來說，回通過 configure 腳本查找所依賴的庫，然後通過 `AC_SUBST` 函數將其導出，然後使用如下方式引用。

{% highlight text %}
libfoo_la_LIBADD = libbar.la $(EXTRA_FOO_LIBS)
{% endhighlight %}

而對於一個程序則使用如下方式。

{% highlight text %}
myprog_LDADD = libfoo.la # links libfoo, libbar, and libpng to myprog.
{% endhighlight %}

<!--
Sometimes the boundaries are a bit vague. $(EXTRA_FOO_LIBS) could have been added to myprog_LDADD. Adding dependencies to a libtool (.la) library, and using libtool do all the platform-specific linker magic, is usually the best approach. It keeps all the linker metadata in the one place.
-->

### 總結

需要手動維護 configure.ac 和 Makefile.am 文件。

## CMake

在介紹示例之前，先說明一下 CMake 有兩種編譯方式：內部構建和外部構建。內部構建直接在源碼目錄下執行 `cmake .`，外部構建則會在一個目錄下構建，不會影響原源碼的結構。

其基本結構可以簡單描述為：

1. 依賴 `CMakeLists.txt` 文件，項目主目標一個，主目錄中可指定包含的子目錄；
2. 在項目 `CMakeLists.txt` 中使用 `PROJECT` 指定項目名稱，通過 `ADD_SUBDIRECTORY` 添加子目錄；
3. 子目錄 `CMakeLists.txt` 將從父目錄 `CMakeLists.txt` 繼承設置。

另外，上述通過 `PROJECT` 設置好工程後，可以通過 `${hello_SOURCE_DIR}` 引用，注意大小寫。

因為 `CMakeLists.txt` 可執行腳本並通過腳本生成一些臨時文件，因此 CMake 無法跟蹤到底產生了那些臨時文件，因此，沒有辦法提供一個可靠的 `make distclean` 方案，為此可以使用外部編譯。

另外，也沒提供 `make uninstall` 命令，卸載可以通過 `cat install_manifest.txt | sudo xargs rm`  命令執行刪除或卸載。

### 簡單示例

最簡單示例單文件輸出 `Hello World!`，只需創建兩個文件：`main.c` 和 `CMakeLists.txt`；然後通過 `cmake . && make` 進行編譯，要查看詳細信息可以 `make VERBOSE=1` 或者 `VERBOSE=1 make` 。

{% highlight text %}
$ cat CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(hello)                                              # 項目名稱
SET(SRC_LIST main.c)                                        # 添加源碼，可忽略
MESSAGE(STATUS "This is BINARY dir " ${HELLO_BINARY_DIR})   # 打印信息
MESSAGE(STATUS "This is SOURCE dir " ${HELLO_SOURCE_DIR})
ADD_EXECUTABLE(hello ${SRC_LIST})

$ cat main.c
#include <stdio.h>
int main()
{
    printf("Hello World!\n");
    return 0;
}
{% endhighlight %}

在上述 `MESSAGE()` 中，會打印變量用於調試，該變量是通過 `PROJECT()` 默認設定的變量，詳見 `PROJECT()` 的使用。

調試的話可以使用 `cmake . -DCMAKE_BUILD_TYPE=Debug` ，不過此時使用的絕對地址。

### 配置文件

通常 `autotools` 會根據平臺動態生成 `config.h` 文件，在 CMake 中同樣可以生成。

除了支持腳本外，還包括了一些常見的模塊 (Modules)，例如 CentOS 中保存在 `/usr/share/cmake/Modules/` 目錄下，在使用時需要先通過 `INCLUDE()` 指令包含相應的模塊。

{% highlight text %}
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
INCLUDE (CheckIncludeFiles)

CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES ("sys/param.h;sys/mount.h" HAVE_SYS_MOUNT_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
{% endhighlight %}

在執行時，會將結果保存在 `CMakeCache.txt` 文件中，類似如下；如果要重新生成則需要刪除。

{% highlight text %}
//Have include HAVE_MALLOC_H
HAVE_MALLOC_H:INTERNAL=1
{% endhighlight %}

在 `config.h.in` 中添加如下內容。

{% highlight text %}
#cmakedefine HAVE_MALLOC_H 1
#cmakedefine HAVE_SYS_MOUNT_H
{% endhighlight %}

在檢測後，會將 `#cmakedefine` 替換掉，如下。

{% highlight text %}
#define HAVE_MALLOC_H 1
#define HAVE_SYS_MOUNT_H

/* #undef HAVE_MALLOC_H 1 */
/* #define HAVE_SYS_MOUNT_H */
{% endhighlight %}

詳細內容可以參考 [CMake:How To Write Platform Checks](https://itk.org/Wiki/CMake:How_To_Write_Platform_Checks) 。

### 安裝

CMake 默認會在與源碼目錄相同的路徑下生成二進制文件或者庫文件，實際上可以通過如下方式進行設置。

{% highlight text %}
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
{% endhighlight %}

當然，對於單個可執行文件，也可以通過 `set_target_properties()` 指令設置如下的變量覆蓋全局的參數。

{% highlight text %}
RUNTIME_OUTPUT_DIRECTORY
LIBRARY_OUTPUT_DIRECTORY
ARCHIVE_OUTPUT_DIRECTORY
{% endhighlight %}

#### 安裝指令

在編譯完成之後，可以通過 `make install` 進行安裝，通過 `cat install_manifest.txt | xargs rm` 刪除即可，不過需要提前指定安裝規則。

{% highlight text %}
INSTALL(TARGETS foobar DESTINATION bin)
INSTALL(FILES foobar.h DESTINATION include)
{% endhighlight %}

如上的配置會將 `foobar` 和 `foobar.h` 安裝到 `/usr/local/{bin,include}` 目錄下即可，實際上對於安裝目錄而言，也可以直接使用 `INCLUDE(GNUInstallDirs)` 指定的目錄。

對於 `TARGETS` 的配置，主要有三個參數，分別為 `ARCHIVE`、`LIBRARY`、`RUNTIME` ，一般會類似如下的方式編寫。

{% highlight text %}
INSTALL(TARGETS targets...
    ARCHIVE
        DESTINATION     <dir>
        PERMISSIONS     permissions...
        CONFIGURATIONS  [Debug|Release|...]
        COMPONENT       <component>
    RUNTIME
        DESTINATION     <dir>
        PERMISSIONS     permissions...
        CONFIGURATIONS  [Debug|Release|...]
        COMPONENT       <component>
    PUBLIC_HEADER
        DESTINATION     <dir>
        PERMISSIONS     permissions...
        CONFIGURATIONS  [Debug|Release|...]
        COMPONENT       <component>
)
{% endhighlight %}

其中上述的 `targets` 可以指定多個，而且可以是不同的類型，如二進制、動態庫、靜態庫、頭文件等。

上述指定的 `DESTINATION` 一般是相對路徑，可以通過 `CMAKE_INSTALL_PREFIX` 指定其前綴，對於 Linux 默認是 `/usr/local/`；另外，其前面還可以指定 `DESTDIR` 目錄。

除了使用 `TARGETS` 外，還可以使用 `FILES` 或者 `DIRECTORY` ，對於 `DIRECTORY` 使用比較靈活，常見需要注意的關鍵點如下。

##### 後綴符號

需要注意其後綴的 `/` 符號。

{% highlight text %}
#----- 會將目錄複製成為 dst/src/{subdirs and files...}
install(DIRECTORY   myproj/src DESTINATION dst)

#----- 會將目錄複製成為 dst/{subdirs and files...}
install(DIRECTORY   myproj/src/ DESTINATION dst)
{% endhighlight %}

##### 文件過濾

可以通過參數 `FILES_MATCHING` 用於指定操作檔案的條件，可以使用 `PATTERN` 或 `REGEX` 兩種匹配方式，要注意 `PATTERN` 會比對全路徑而不只是文件名。

{% highlight text %}
INSTALL(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")
{% endhighlight %}

以上會把 `src/` 底下所有文件後綴為 `.h` 的文件複製到 `include` 文件夾下，並且會保留原本目錄樹的結構。

另外，還可以在匹配條件後面通過 `EXCLUDE` 排除符合條件的文件或目錄。

{% highlight text %}
INSTALL(DIRECTORY myapp/ mylib DESTINATION myproj PATTERN ".git" EXCLUDE)
{% endhighlight %}

### 打包

可以通過 CPack 進行打包，也就是將編譯後的二進制進行打包，當然也可以打包源碼，分別通過如下命令進行打包。

{% highlight text %}
make package_source
make package
{% endhighlight %}

### 宏和函數

同大多數腳本語言一樣，CMake 中也有宏和函數的概念，關鍵字分別為 `macro` 和 `function`，具體用法如下：

{% highlight text %}
MACRO( [arg1 [arg2 [arg3 ...]]])
	 COMMAND1(ARGS ...)
	 COMMAND2(ARGS ...)
	 ...
ENDMACRO()

FUNCTION( [arg1 [arg2 [arg3 ...]]])
	 COMMAND1(ARGS ...)
	 COMMAND2(ARGS ...)
	 ...
ENDFUNCTION()
{% endhighlight %}

如下是一個求和的宏定義。

{% highlight text %}
macro(sum outvar)
	set(_args ${ARGN})
	list(LENGTH _args argLength)
	if(NOT argLength LESS 4)　# 限制不能超過4個數字
		message(FATAL_ERROR "to much args!")
	endif()
	set(result 0)

	foreach(_var ${ARGN})
		math(EXPR result "${result}+${_var}")
	endforeach()

	set(${outvar} ${result})
endmacro()

sum(addResult 1 2 3 4 5)
message("Result is :${addResult}")
{% endhighlight %}

`${ARGN}` 是 CMake 中的一個變量，指代宏中傳入的多餘參數；上述宏只定義了一個參數 `outvar`，其餘需要求和的數字都是不定形式傳入的，所以需要先將多餘的參數傳入一個單獨的變量中。

函數與宏的區別是，函數中的變量是局部的，不能直接傳出。


### 常用示例

#### 內置變量

如下是設置 C 編譯器的參數，對於 CPP 則將 C 替換為 CXX 即可。

{% highlight text %}
set(CMAKE_C_COMPILER      "gcc" )               # 顯示指定使用的編譯器
set(CMAKE_C_FLAGS         "-std=c99 -Wall")     # 設置編譯選項，也可以通過add_definitions添加編譯選項
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g" )            # 調試包不優化
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG " )     # release包優化
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wno-sign-compare")   # 忽略某些告警
{% endhighlight %}

內置變量可在 cmake 命令中使用，如 `cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug` 。

<!--
EXECUTABLE_OUTPUT_PATH：可執行文件的存放路徑
LIBRARY_OUTPUT_PATH：庫文件路徑
CMAKE_BUILD_TYPE:：build 類型(Debug, Release, ...)，
BUILD_SHARED_LIBS：Switch between shared and static libraries
-->


#### Build Type

除了上述 `CMAKE_C_FLAGS_DEBUG` 指定不同類型的編譯選項外，還可以通過如下方式指定。

{% highlight text %}
set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
	SET(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    set(LIBRARIES Irrlicht_S.lib)
else()
    set(LIBRARIES Irrlicht.lib)
endif()
{% endhighlight %}

可以在命令行中通過如下方式編譯 `cmake -DCMAKE_BUILD_TYPE=Debug ..`，最終的編譯選項可以查看 `CMakeFiles/SRCFILE.dir/flags.make` 中的 `C_FLAGS` 選項，一般是 `CMAKE_C_FLAGS+CMAKE_C_FLAGS_MODE` 。

#### 測試用例

可以通過如下方式添加測試用例。

{% highlight text %}
option(WITH_UNIT_TESTS "Compile with unit tests" OFF)

# Setup testing
IF(WITH_UNIT_TESTS)
        ENABLE_TESTING()
        ADD_SUBDIRECTORY(test)
ENDIF()

# test/CMakeLists.txt
FILE(GLOB SRCS *.c)
ADD_EXECUTABLE(testfoo ${SRCS})
TARGET_LINK_LIBRARIES(testfoo libs)
ADD_TEST(
    NAME testfoo
    COMMAND testfoo
)
{% endhighlight %}

然後在編譯時通過 `cmake .. -DWITH_UNIT_TESTS=ON` 執行，並通過 `make test` 進行測試。

另外，可以指定測試的參數，以及輸出的匹配。

{% highlight text %}
ADD_TEST(test foobar 10 5)
SET_TESTS_PROPERTIES(test PROPERTIES PASS_REGULAR_EXPRESSION "ok")
{% endhighlight %}





















#### 文件渲染

可以通過如下腳本，動態生成。

{% highlight text %}
$ cat CMakeLists.txt
CONFIGURE_FILE(
    "${PROJECT_SOURCE_DIR}/gcc_debug_fix.sh.in"
    "${PROJECT_BINARY_DIR}/gcc_debug_fix.sh"
    @ONLY)
... ...
ADD_EXECUTABLE (my_exe ...)
... ...
SET_TARGET_PROPERTIES(my_exe PROPERTIES RULE_LAUNCH_COMPILE "${PROJECT_BINARY_DIR}/gcc_debug_fix.sh")

$ cat gcc_debug_fix.sh.in
#!/bin/sh
PROJECT_BINARY_DIR="@PROJECT_BINARY_DIR@"
PROJECT_SOURCE_DIR="@PROJECT_SOURCE_DIR@"

# shell script invoked with the following arguments
# $(CXX) $(CXX_DEFINES) $(CXX_FLAGS) -o OBJECT_FILE -c SOURCE_FILE

# extract parameters
SOURCE_FILE="${@: -1:1}"
OBJECT_FILE="${@: -3:1}"
COMPILER_AND_FLAGS=${@:1:$#-4}

# make source file path relative to project source dir
SOURCE_FILE_RELATIVE="${SOURCE_FILE:${#PROJECT_SOURCE_DIR} + 1}"

# make object file path absolute
OBJECT_FILE_ABSOLUTE="$PROJECT_BINARY_DIR/$OBJECT_FILE"

cd "$PROJECT_SOURCE_DIR"

# invoke compiler
exec $COMPILER_AND_FLAGS -c "${SOURCE_FILE_RELATIVE}" -o "${OBJECT_FILE_ABSOLUTE}"
{% endhighlight %}

通過 `CONFIGURE_FILE()` 將文件中的變量替換，然後在 `set_target_properties()` 設置，將在編譯之前將絕對路徑轉換為相對路徑，注意 `gcc_debug_fix.sh.in` 需要為可以執行文件。

#### 庫參數

在編譯不同的庫時，可能需要使用不同的編譯參數，那麼此時就可以使用如下的方式修改。


{% highlight text %}
ADD_LIBRARY(unity unity/src/unity.c)

# Disable -Werror for Unity
IF (FLAG_SUPPORTED_Werror)
	IF ("${CMAKE_VERSION}" VERSION_LESS "2.8.12")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
    ELSE()
        TARGET_COMPILE_OPTIONS(unity PRIVATE "-Wno-error")
    ENDIF()
ENDIF()
# Disable -fvisibility=hidden for Unity
IF (FLAG_SUPPORTED_fvisibilityhidden)
    IF ("${CMAKE_VERSION}" VERSION_LESS "2.8.12")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=default")
    ELSE()
        TARGET_COMPILE_OPTIONS(unity PRIVATE "-fvisibility=default")
    ENDIF()
ENDIF()
# Disable -fsanitize=float-divide-by-zero for Unity
#     (GCC bug on x86 https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80097)
IF (FLAG_SUPPORTED_fsanitizefloatdividebyzero AND (CMAKE_C_COMPILER_ID STREQUAL "GNU"))
    IF ("${CMAKE_VERSION}" VERSION_LESS "2.8.12")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-sanitize=float-divide-by-zero")
    ELSE()
        TARGET_COMPILE_OPTIONS(unity PRIVATE "-fno-sanitize=float-divide-by-zero")
    ENDIF()
ENDIF()
{% endhighlight %}

#### 添加 flex bison

默認是不支持 flex 和 bison 的，可以通過如下方式添加。


{% highlight text %}
#----- 生成的中間文件保存在${CMAKE_CURRENT_BINARY_DIR}目錄下
include_directories(${CMAKE_CURRENT_BINARY_DIR} )

#----- 定義源碼目錄以及中間文件
SET(PARSER_DIR ${CMAKE_SOURCE_DIR}/driver/mysql)
SET(GEN_SOURCES
	${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
	${CMAKE_CURRENT_BINARY_DIR}/parser.l.h
)

#----- 添加一個custom_target用於構建依賴，該target依賴${GEN_SOURCES}指向的文件
ADD_CUSTOM_TARGET(
        GenServerSource
        DEPENDS ${GEN_SOURCES}
)

#----- 告知${GEN_SOURCES}指向的文件是編譯過程中生成的，以避免執行cmake命令的時候報文件找不到
SET_SOURCE_FILES_PROPERTIES(${GEN_SOURCES} GENERATED)

#----- 需要注意的是OUTPUTS一定要和${GEN_SOURCES}中文件一致，並且target設置對否則無法確保執行順序
ADD_CUSTOM_COMMAND(
	SOURCE ${PARSER_DIR}/parser.y
	COMMAND bison -d ${PARSER_DIR}/parser.y
	TARGET GenServerSource
	OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
	WORKING_DIRECTORY ${PARSER_DIR}
)
ADD_CUSTOM_COMMAND(
	SOURCE ${PARSER_DIR}/parser.l
	COMMAND flex  ${PARSER_DIR}/parser.l
	TARGET GenServerSource
	OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c ${CMAKE_CURRENT_BINARY_DIR}/parser.l.h
	WORKING_DIRECTORY ${PARSER_DIR}
)

#----- 這裡將flex和bison生成的.c文件編譯到程序的動態庫中，需要注意指定路徑
add_library(driver SHARED
	sourcefile1.cc sourcefile2.cc sourcefile3.cc
	${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
)

#----- 指明動態庫driver所需要的依賴，所以可以保證cmake會先生成GenServerSource, 而GenServerSource又
#      依賴${GEN_SOURCES}指明的文件， 而這些文件又是由兩個ADD_CUSTOM_COMMAND命令來生成的
#      (outputs中給出的)，所以會先執行兩個ADD_CUSTOM_COMMAND命令
ADD_DEPENDENCIES(driver GenServerSource)
{% endhighlight %}

#### 庫使用

關於庫的使用常見有如下的操作。

{% highlight text %}
----- 增加庫的搜索路徑
LINK_DIRECTORIES(./lib)

----- 生成庫，可以是動態(SHARED)或者靜態庫(STATIC)
ADD_LIBRARY(hello SHARED ${SRC_LIST})

----- 指定生成對象時依賴的庫
TARGET_LINK_LIBRARIES(hello A B.a C.so)

----- 自定義鏈接選項，單獨對B.a使用--whole-archive選項
TARGET_LINK_LIBRARYIES(hello A -Wl,--whole-archive B.a -Wl,--no-whole-archive C.so)
{% endhighlight %}

在使用 `add_library(foo SHARED foo.c)` 時，不同平臺輸出有所區別，例如，`foo.dll(Windows)`、 `libfoo.so(Linux)`、 `libfoo.dylib(Mac)` 。

比如編寫用於 lua 擴展的 C 模塊，那麼在進行 require 時，需要執行如下調用：

{% highlight lua %}
require 'libfoo'     --默認加載libfoo.[so|dll]，並且執行luaopen_libluafoo
require 'foo'        --加載foo.so,並且執行luaopen_luafoo
{% endhighlight %}

並且各個平臺下各不相同，可以通過如下方式修改前綴以及後綴：

{% highlight text %}
SET_TARGET_PROPERTIES(foo PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(foo PROPERTIES SUFFIX "so")
{% endhighlight %}


<!--
 對了，吐槽一下mac的rpath也是件麻煩的事情


# enable @rpath in the install name for any shared library being built
set(CMAKE_MACOSX_RPATH 1)

# the install RPATH for bar to find foo in the install tree.
# if the install RPATH is not provided, the install bar will have none
set_target_properties(bar PROPERTIES INSTALL_RPATH "@loader_path/../lib")
-->

#### 文件路徑

CMake 在編譯時會使用到絕對路徑，而在打印日誌時可能會導致文件長度過大，有幾種方式修改。

{% highlight c %}
#include <string.h>
#define __FILENAME__                             \
        (strrchr(__FILE__, '/') ?                \
	strrchr(__FILE__, '/') + 1 : __FILE__)
{% endhighlight %}

不過這種方式會在執行時計算，也可以使用如下宏定義，此時會使用相對路徑。

{% highlight text %}
SET(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
{% endhighlight %}

或者只使用文件名。

{% highlight text %}
SET(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")
{% endhighlight %}

#### 常用命令

指令是大小寫無關的，參數和變量是大小寫相關的，參數使用空格或者分號隔開。如 `MESSAGE (STATUS "This is BINARY dir" ${HELLO_BINARY_DIR})` 和 `MESSAGE (STATUS "This is BINARY dir ${HELLO_BINARY_DIR}")` 相同。

變量使用 `${}` 方式取值，但是在 `IF` 控制語句中是直接使用變量名。

{% highlight text %}
PROJECT (projectname [CXX] [C] [Java])
  定義工程名稱以及語言，會隱式定義 projectname_BINARY_DIR、projectname_SOURCE_DIR變量，
  同時也會定義 PROJECT_BINARY_DIR、PROJECT_SOURCE_DIR 與前兩者相同，建議使用後者。

SET(VAR [VALUE] [CACHE TYPE DOCSTRING [FORCE]])
  可以用來顯式的定義變量。

MESSAGE([SEND_ERROR | STATUS | FATAL_ERROR] "message to display" ...)
  向終端輸出用戶定義的信息，SEND_ERROR(產生錯誤，生成過程被跳過)，SATUS(輸出前綴為-的信息)、
  FATAL_ERROR(立即終止所有cmake過程)。

ADD_DEFINITIONS(-DMICRO_1 ...)
  添加宏定義。

ADD_EXECUTABLE(hello ${SRC_LIST})
  該工程會生成一個文件名為 hello 的可執行文件，相關的源文件是 SRC_LIST 。

ADD_SUBDIRECTORY(source_dir [binary_dir] [EXCLUDE_FROM_ALL])
  指定當前工程的

CONFIGURE_FILE(intput output [COPYONLY] [ESCAPE_QUOTES] [@ONLY])
  將文件 input 拷貝到 output 然後替換文件內容中引用到的變量值；如果用相對路徑，則 input 相對
  的是當前源碼路徑，output 相對於二進制文件路徑。該命令替換掉在輸入文件中，以 ${VAR} 格式或
  @VAR@ 格式引用的任意變量，如同它們的值是由CMake確定的一樣。 如果一個變量還未定義，它會被替
  換為空。如果指定了COPYONLY選項，那麼變量就不會展開。如果指定了ESCAPE_QUOTES選項，那麼所有被
  替換的變量將會按照C語言的規則被轉義。該文件將會以CMake變量的當前值被配置。

----- 查看當前支持的選項
cmake .. -LH
{% endhighlight %}



## 其它

可以通過 `pkg-config` 命令來檢索系統中安裝庫文件的信息，通常用於庫的編譯和連接。

如果庫的頭文件不在 `/usr/include` 目錄中，那麼在編譯的時候需要用 `-I` 參數指定其路徑；同樣，鏈接時可以通過 `-L` 參數指定庫，這樣就導致了編譯命令界面的不統一。

為了保證統一，通過庫提供的一個 `.pc` 文件獲得庫的各種必要信息的，包括版本信息、編譯和連接需要的參數等，在需要的時候可以通過提供的參數，如 `--cflags`、`--libs`，將所需信息提取出來供編譯和連接使用。

主要包括了：A) 檢查庫的版本號；B) 獲得編譯預處理參數，如宏定義，頭文件的路徑；C) 獲得編譯參數，如庫及其依賴的其他庫的位置，文件名及其他一些連接參數；D) 自動加入所依賴的其他庫的設置。

在 CentOS 中默認會保存在 `/usr/lib64/pkgconfig`、`/usr/share/pkgconfig` 目錄下。

{% highlight text %}
----- 查看所有.pc
$ pkg-config --list-all

----- 編譯
$ gcc -c `pkg-config --cflags glib-2.0` sample.c

----- 鏈接
$ gcc sample.o -o sample `pkg-config --libs glib-2.0`

----- 編譯鏈接合併
$ gcc sample.c -o sample `pkg-config --cflags --libs glib-2.0`
{% endhighlight %}

另外，可以通過環境變量 `PKG_CONFIG_PATH` 指定搜索路徑。

## 參考

可以查看官方文檔 [automake](https://www.gnu.org/software/automake/manual/automake.html)，[GNU Autoconf - Creating Automatic Configuration Scripts](https://www.gnu.org/software/autoconf/manual/) 。

關於 CMake 相關的文檔可以參考 [CMake 實踐](/reference/linux/CMake_Practice.pdf) ，或者參考 [CMake 入門實戰](http://hahack.com/codes/cmake/) 以及 [CMake Tutorial](https://cmake.org/cmake-tutorial/)；搭建 GTest 環境可以參考 [Unit testing with GoogleTest and CMake](http://kaizou.org/2014/11/gtest-cmake/) 。

常見的命令參考 [cmake-commands](https://cmake.org/cmake/help/v3.0/manual/cmake-commands.7.html) 。


<!--
The new guide of the conversation in Autoconf and Make
http://www.edwardrosten.com/code/autoconf/

A tutorial for porting to autoconf & automake
http://mij.oltrelinux.com/devel/autoconf-automake/

運用Autoconf和Automake生成Makefile的學習之路
http://www.cnblogs.com/ericdream/archive/2011/12/09/2282359.html

如何根據configure.ac和Makefile.am為開源代碼產生當前平臺的Makefile
http://www.51cos.com/?p=1649

Colletcd的編譯系統介紹
https://collectd.org/wiki/index.php/Build_system

AC_CHECK_FUNCS
AC_CHECK_LIB
https://autotools.io/autoconf/finding.html
概念：GNU構建系統和Autotool
http://www.pchou.info/linux/2016/09/16/gnu-build-system-1.html

CMake如何查找鏈接庫
http://www.yeolar.com/note/2014/12/16/cmake-how-to-find-libraries/


http://huqunxing.site/2016/08/26/CMake%E5%85%A5%E9%97%A8%E6%8C%87%E5%8D%97/#編譯32位和64位程序

CPACK_GENERATOR RPM TBZ2 ZIP

cpack -D CPACK_GENERATOR="ZIP;TGZ" /path/to/build/tree

 AC_PLUGIN([cpu],                 [$plugin_cpu],             [CPU usage statistics])


http://cmake.3232098.n2.nabble.com/Adding-a-custom-line-to-CMake-s-makefiles-td5984979.html


%defattr(file mode, user, group, dir mode)

默認的是 `%defattr(644, user, group, 755)` 或者 `%defattr(-, user, group)` ，對於普通文件是 644 而可執行文件以及目錄使用 755 。


-->



{% highlight text %}
{% endhighlight %}
