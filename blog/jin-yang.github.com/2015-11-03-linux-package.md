---
title: Linux 自动编译
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,automake,configure,cmake
description: 在 Linux 中，经常使用 GNU 构建系统，也就是利用脚本和 make 程序在特定平台上构建软件，这种方式几乎成为一种习惯，被广泛使用。这里简单介绍下这种构建方式的细节，以及开发者如何利用 autotools 创建兼容 GNU 构建系统的项目。
---

在 Linux 中，经常使用 GNU 构建系统，也就是利用脚本和 make 程序在特定平台上构建软件，这种方式几乎成为一种习惯，被广泛使用。

这里简单介绍下这种构建方式的细节，以及开发者如何利用 autotools 创建兼容 GNU 构建系统的项目。

<!-- more -->

## 简介

在 Linux 平台下，在编译项目时通常使用 Makefile，简单来说，Makefile 是用来定义整个工程的编译规则，也就是文件的编译、链接顺序，如何生成可执行文件或者动态加载库等等。

Makefile 带来的最大好处就是 "自动化编译"，一但编辑好文件，只需要一个 make 命令，整个工程完全自动编译，极大的提高了软件开发的效率。

如果项目比较小，那么可以直接手动编辑该文件；不过一但项目比较大时，手动维护将变得极其复杂，为此，就可以使用 Autotools 或者 CMake 生成 Makefile 文件。

假设项目工程的使用源码结构如下。

{% highlight text %}
src/
   子目录，存放工程源码；
doc/
   子目录，用来存放工程文档；
bin/
   子目录，最后生成的二进制可执行文件；
COPYRIGHT
   版权信息；
README.md
   使用 markdown 编写的自述文件；
{% endhighlight %}

默认，会将可执行文件安装在 /usr/bin 目录下，doc 安装到 /usr/share/doc 目录下。


## Makefile

详细内容可以参考陈皓编写的 [跟我一起写 Makefile](/reference/linux/Makefile.pdf)，在此就不做过多介绍了，仅简单记录下。


### 变量

如下是 Makefile 中内置的变量。

{% highlight text %}
$@:  规则中的目标名（也就是规则名）；
$<:  规则中的依赖项目,只代表规则所有依赖项目中的第一项；
$^:  规则中所有的依赖项目；
$?:  规则中时间新于目标的依赖项目。
{% endhighlight %}

关于变量通配符，与 shell 相同，如 1) `?` 任意单个字符；2) `*` 任意字符的字符串；3) `[set]` 任何在 set 里的字符；4) `[!set]` 任何不在 set 里的字符。

以如下为例 `touch {a,b,c}.c Makefile` 。

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


### PHONY 伪目标

通常来说 Makefile 会检测 `:` 左侧的目标是不是最新的，如果是最新的则不会更新，对应规则不会执行。假设目标为 clean，本意是做编译后的清理，但是当目录下有 clean 文件时，则对应的规则将不会执行，为了解决这一问题，定义了伪目标。

当定义了伪目标之后，make 在执行规则时不会去试图去查找隐含规则来创建它，而是直接执行，这样就提高了 make 的执行效率，也不用担心由于目标和文件名重名了。

伪目标的另一种使用场合时在 make 的并行和递归执行过程中，第一个实际上时串行执行的；第二个会并行执行。

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

在 Linux 平台上，经常使用 `configure->make->make install` 从源码开始编译安装，也就是 GNU 构建系统，利用脚本和 make 程序在特定平台上编译软件，也就是利用 autotools 创建构建系统的项目。

注意，有些程序虽然也是采用上述的三步，但并未采用 autotools 实现，如 nginx 是作者编写的构建程序。

在 CentOS 中可以直接通过 `yum install automake autoconf` 安装相关的文件；利用 autotools 生成 Makefile 的过程如下图所示。

![autotools]({{ site.url }}/images/linux/autotools-process-1.png "autotools"){: .pull-center }

其中用到的核心工具包括了 Autoconf 和 Automake ，首先从用户和开发者角度看看两者的区别。关于 autotools 的简单处理流程可以参考 [automake](http://www.gnu.org/software/automake/manual/automake.html) 中的 `Setup Explained` 内容。

### 用户视角

`configure` 脚本是由软件开发者维护并发布给用户使用的 shell 脚本，该脚本作用是检测系统环境，最终目的是生成 Makefile 和 config.h 两个文件。

开发者在分发源码包时，除了源代码 (.c .h)，还有许多支撑软件构建的文件和工具，其中最重要的文件就是 Makefile.in 和 config.h.in 两个，在 configure 脚本执行成功后，将为每个 `*.in` 文件处理成对应的非 `*.in` 文件。

#### configure

configure 脚本会检查当前系统，而检查项的多少取决于开发者，一般来说，主要检查当前目标平台的程序、库、头文件、函数等的兼容性，而检查结果将作用于 config.h 和 Makefile 文件的生成，从而影响最终的编译。

用户可通过参数定制软件所需要包含的组件、安装路径等，一般会被五部分，可以通过 `--help` 参数查看当前软件提供了那些配置参数。

<!--
    *安装路径相关配置。最常见的是--prefix。
    *程序名配置。例如--program-suffix可用于为生成的程序添加后缀。
    *跨平台编译。不太常用。
    *动态库静态库选项。用于控制是否生成某种类型的库文件。
    程序组件选项。用于配置程序是否将某种功能编译到程序中，一般形如--with-xxx。这可能是最常用的配置，而且由软件开发者来定义。
（*表示这是几乎所有软件都支持的配置，因为这些配置是autotool生成的configure脚本默认支持的。）
-->

在 configure 在执行过程中，除了生成 Makefile 外，还会生成如下的临时文件：

* config.log 日志文件；
* config.cache 缓存，以提高下一次 configure 的速度，需通过 -C 来指定才会生成；
* config.status 实际调用编译工具构建软件的 shell 脚本。

如果软件通过 libtool 构建，还会生成 libtool 脚本，关于 libtool 脚本如何生成，详见如下。


### 开发者视角

开发者除了编写软件本身的代码外，还需要负责生成构建软件所需要文件和工具，通过 autotools 工具可以解决一些常见的平台问题，但是编写依旧复杂。为了生成 configure 脚本和 Makefile.in 等文件，开发者需要创建并维护一个 configure.ac 文件，以及一系列的 Makefile.am 文件。<!--autoreconf程序能够自动按照合理的顺序调用autoconf automake aclocal等程序。-->

#### Autoconf

该工具用于生成一个可在类 Unix 系统下工作的 Shell 脚本，该脚本可在不同 *nix 平台下自动配置软件源代码包，也就是 configure 文件，生成脚本后与 autoconf 再无关系。

<!--
对于每个使用了Autoconf的软件包，Autoconf从一个列举了该软件包需要的，或者可以使用的系统特征的列表的模板文件中生成配置脚本。在 shell代码识别并响应了一个被列出的系统特征之后，Autoconf允许多个可能使用（或者需要）该特征的软件包共享该特征。 如果后来因为某些原因需要调整shell代码，就只要在一个地方进行修改； 所有的配置脚本都将被自动地重新生成以使用更新了的代码。
-->

#### Automake

该工具通过 Makefile.am 文件自动生成 Makefile.in 文件，而 Makefile.am 基本上是一系列 make 宏定义，该文件需要手动编辑，也就是将对 Makefile 的编辑转义到了 Makefile.am 文件。

<!--
Automake 支持三种目录层次： “flat”、“shallow”和“deep”。一个flat（平）包指的是所有文件都在一个目录中的包。为这类包提供的`Makefile.am' 缺少宏SUBDIRS。这类包的一个例子是termutils。一个deep（深）包指的是所有的源代码都被储存在子目录中的包；顶层 目录主要包含配置信息。GNU cpio 是这类包的一个很好的例子，GNU tar也是。deep包的顶层`Makefile.am'将包括 宏SUBDIRS，但没有其它定义需要创建的对象的宏。一个shallow（浅）包指的是主要的源代码储存在顶层目录中，而 各个部分（典型的是库）则储存在子目录中的包。Automake本身就是这类包（GNU make也是如此，它现在已经不使用automake）。
-->

### 常见文件

接下来看下一些常用的文件。

#### configure.ac

用于通过 autoconf 命令生成 configure 脚本，如下是一个 configure.ac 的示例：

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
AC_CONFIG_FILES([Makefile                     # 主要是通过*.in模板生成响应的文件
                 src/Makefile
                 src/a/Makefile
                 src/b/Makefile])
AC_OUTPUT
{% endhighlight %}

以 `AC_` 开头的是一些宏调用，与 C 中的宏概念类似，会被替换展开；很多以 AC_PROG_XXX 开头的宏用于检查所需要的程序是否存在，详细可以查看 [Particular Program Checks](https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Particular-Programs.html)；对于一些特殊的函数或者文件则可以通过 [Generic Program and File Checks](https://www.gnu.org/software/autoconf/manual/autoconf-2.69/html_node/Generic-Programs.html) 中定义的宏进行检查。

而 m4 是一个经典的宏工具，autoconf 正是构建在 m4 之上，可以简单理解为 autoconf 预先实现了大量用于检测系统可移植性的宏，这些宏在展开后就是大量的 shell 脚本。所以编写 configure.ac 需要对这些宏熟练掌握，并且合理调用，有时，甚至可以自己实现自己的宏。

#### configure.scan

通过 autoscan 命令可以得到一个初始化的 configure.scan 文件，然后重命名为 configure.ac 后，在此基础上编辑 configure.ac ，而 autoscan 通常 **只用于首次初始化 configure.ac** 。

autoscan 会扫描源码，并生成一些通用的宏调用、输入的声明以及输出的声明。

#### config.h.in

可以通过 autoheader 命令扫描 configure.ac 中的内容，并生成 config.h.in 文件；每当 configure.ac 文件有所变化，都可以再次执行 autoheader 更新 config.h.in 。

在 configure.ac 中通过 `AC_CONFIG_HEADERS([config.h])` 告诉 autoheader 应当生成 config.h.in 的路径；在最后的实际编译阶段，生成的编译命令会加上 `-DHAVE_CONFIG_H` 定义宏。

{% highlight text %}
/bin/sh ../../libtool --tag=CC --mode=compile gcc -DHAVE_CONFIG_H ...
{% endhighlight %}

于是在代码中，可以通过下面代码安全的引用 config.h 。

{% highlight c %}
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
{% endhighlight %}

config.h 包含了大量的宏定义，其中包括软件包的名字等信息，程序可以直接使用这些宏；更重要的是，程序可以根据其中的对目标平台的可移植性相关的宏，通过条件编译，动态的调整编译行为。

#### Makfile.am

手工编写 Makefile 是一件相当烦琐的事情，而且，如果项目复杂的话，编写难度将越来越大；为此，可以通过 automake+Makefile.am 生成 Makefile.in 文件，通常一个 Makefile.am 的示例如下。

{% highlight text %}
SUBDIRS         = a b
bin_PROGRAMS    = st
st_SOURCES      = main.c
st_LDADD        = $(top_builddir)/src/a/liba.la $(top_builddir)/src/b/libb.la
{% endhighlight %}

通过 SUBDIRS 声明了两个子目录，子目录的中的构建需要靠 a/Makefile.am 和 b/Makefile.am 来进行，这样多目录组织起来就方便多了。

bin_PROGRAMS 声明一个可执行文件，st_SOURCES 指定所依赖的源代码文件，st_LDADD 声明了可执行文件在连接时，需要依赖的 Libtool 库文件。

由于 automake 晚于 autoconf，所以 automake 是作为 autoconf 的扩展来实现的，在 configure.ac 中需要通过声明 AM_INIT_AUTOMAKE 告诉 autoconf 需要配置和调用 automake 。


#### aclocal

如上所述，configure.ac 是依靠宏展开来得到 configure 的，因此，能否成功生成取决于宏定义能否找到；默认 autoconf 会从安装路径下来寻找事先定义好了宏，而对于像 automake、libtool 和 gettext 等第三方扩展宏，甚至是开发者自行编写的宏就一无所知了。

于是，存在这个工具 aclocal，将在 configure.ac 同一目录下生成 aclocal.m4，在扫描 configure.ac 的过程中，将第三方扩展和开发者自己编写的宏定义复制进去；这样，autoconf 在遇到不认识的宏时，就会从 aclocal.m4 中查找。


#### libtool

libtool 试图解决不同平台下库文件的差异，实际是一个 shell 脚本，实际工作过程中，调用了目标平台的 cc 编译器和链接器，以及给予合适的命令行参数，libtool 可以单独使用。

automake 支持 libtool 构建声明，在 Makefile.am 中，普通的库文件目标写作 xxx_LIBRARIES ：

{% highlight text %}
noinst_LIBRARIES = liba.a
liba_SOURCES = ao1.c ao2.c ao3.c
{% endhighlight %}

而对于 libtool 目标，写作 xxx_LTLIBRARIES，并以 .la 作为后缀声明库文件。

{% highlight text %}
noinst_LTLIBRARIES = liba.la
liba_la_SOURCES = ao1.c ao2.c ao3.c
{% endhighlight %}

在 configure.ac 中需要声明 LT_INIT：

{% highlight text %}
...
AM_INIT_AUTOMAKE([foreign])
LT_INIT
...
{% endhighlight %}

有时，如果要用到 libtool 中的某些宏，则推荐将这些宏复制到项目中。首先，通过 `AC_CONFIG_MACRO_DIR([m4])` 指定使用 m4 目录存放第三方宏；然后在最外层的 Makefile.am 中加入 `ACLOCAL_AMFLAGS = -I m4` 。

<!--
https://segmentfault.com/a/1190000006915719


Configure、Makefile.am、Makefile.in、Makefile之间的关系
http://www.51cos.com/?p=1649

可以参考官方文档
http://inti.sourceforge.net/tutorial/libinti/autotoolsproject.html


通过--with-libmysql=/opt/mysql/lib指定MySQL库的路径；--enable-mysql编译MySQL插件；

需要手动编译：
1. Makefile.am
用于生成Makefile.in文件，需要手动书写。

[s]bin_PROGRAMS  需要生成的可执行文件的文件名，多个文件以空格分割；
foobar_SOURCES 指定生成命令所需要的文件，包括源码以及头文件；

1. autoscan(autoconf)
扫描源代码以搜寻普通的可移植性问题，比如检查编译器，库，头文件等，生成文件configure.scan，然后手动编辑生成configure.ac。
your source files --- [autoscan*] --- [configure.scan] -<edit>- configure.ac
常用函数

#### AC_ARG_WITH()

AC_ARG_WITH (package, help-string, [action-if-given], [action-if-not-given])
此时就是可以命令行中使用--with-package或者--without-package进行配置，其中第三个参数需要检查yes/no；配置完成之后可以通过`./configure --help`查看。
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
# 接下来可以通过shell脚本再进行一些检查，也可以使用AC_CHECK_LIB()、AC_CHECK_HEADERS()函数，失败则可以用AC_MSG_FAILURE()打印信息，暂时省略
if test "x$with_libmysql" = "xyes"
then
  BUILD_WITH_LIBMYSQL_CFLAGS="$with_mysql_cflags"
  AC_SUBST(BUILD_WITH_LIBMYSQL_CFLAGS)
  AC_SUBST(BUILD_WITH_LIBMYSQL_LIBS, "$with_mysql_libs")
fi
# 设置条件变量，可以再Makefile.am中使用
AM_CONDITIONAL(BUILD_WITH_LIBMYSQL, test "x$with_libmysql" = "xyes")
-->



### Flat

这一模式就是所有的文件，包括源码、头文件等，都在同一目录下。

如下示例中，目录下存在三个文件：hello.h (声明hello()方法)、hello.c (实现hello()方法)、main.c (主文件调用了hello()方法)。执行方法如下：

<ol><li>
autoscan<br>在目录下，直接执行该命令，然后会生成 configure.scan 和 autoscan.log (可删除) 两个文件，将 configure.scan 重命名为 <font color="blue">configure.ac</font> 文件 (注意，不是 configure.in，这个是老版本)，修改文件的内容。
{% highlight text %}
AC_PREREQ([2.69])                             # 设置Autoconf的最小版本
AM_INIT_AUTOMAKE(hello, 1.0)                  # 表示用于automake调用，可为AM_INIT_AUTOMAKE
AC_INIT([hello], [1.0], [www.douniwan.com])   # 设置包名称、版本、bug报告地址
AC_CONFIG_SRCDIR([main.c])                    # 可为包中的任意源码，用于判断是否存在源码
AC_CONFIG_HEADERS([config.h])                 # 表示在调用automake时依赖config.h.in文件，如果不定义则会
                                              # 在编译时会有大量的-DXXX=xxxd的定义参数
AC_PROG_CC                                    # 检测是否存在C编译器
AC_PROG_CXX                                   # 检测是否存在C++编译器
AC_OUTPUT(Makefile)                           # 指定configure输出的文件
{% endhighlight %} </li><li>

aclocal<br>
是一个 perl 脚本，用于生成 aclocal.m4 文件供 automake 和 autoconf 调用，详见 info aclocal。</li><li>

autoheader<br>用于产生 config.h.in 文件，感觉应该是配置通过 configure 输入 config.h 文件时需要定义那些宏。</li><li>

automake --add-missing<br>
新建文件<font color="blue">Makefile.am</font>文件，通过Makefile.am生成Makefile.in供configure脚本调用生成Makefile。

{% highlight text %}
AUTOMAKE_OPTIONS=foreign                     # 不遵循gnu的标准
bin_PROGRAMS=hello                           # 最后生成的程序，多个空格间隔
hello_LDFLAGS = -Wall                        # 编译选项，只用于可执行文件以及shared library
hello_SOURCES=main.c hello.c hello.h

noinst_LIBRARIES = libfoo.a
libfoo_a_CPPFLAGS = -g -Wall
libfoo_a_LIBADD = lpthread -lm            // 用于静态链接库
libfoo_a_SOURCES =  cmds.c cmds.h \
                    app.c app.h
foo_SOURCES = main.c
foo_LDADD = libfoo
{% endhighlight %}

autoconf<br>
主要是用来检测所依赖的条件是否成立，检测的结果可以保存为文件供其它应用使用，其脚本实际使用shell和M4(可以C语言中宏的加强版)组成，还有部分宏是通过aclocal生成的，保存在aclocal.m4中。<br><br>
在生成configure脚本的时候同时会保存日志方便调试，第一次执行完之后可以通过config.status更新。通常是用来生成一个配置文件，否则通过编译参数进行配置，此时编译选项将非常大。</li><br><li>

configure<br>
到现在已经生成了 configure 文件，通过 ./configure 即可以生成Makefile文件。</li><br><li>

Makefile<br>
主要包括了如下的选项，install安装，clean清除，dist打包软件发布，distcheck对发布的包进行检测。
</li></ol>
编辑 configure.ac 时，可以使用 case；其中 dnl 用于表示注释。<br><br>



### 其它

在正式执行 configure 之前，会调用 `AC_DEFUN()` 执行一些函数的扩展，然后调用 `AC_INIT()` 执行初始化操作；在 `AC_INIT()` 宏中，会初始化一些变量参数值，包括 `AC_PACKAGE_NAME`、`PACKAGE_NAME` 等。

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

需要执行 `autoreconf -ivf` 命令来更新 configure 脚本文件，autoreconf 可以看做是 autoconf、autoheader、acloacl、automake、libtoolize、autopoint 的组合体，而且会以合适的顺序来执行。

注意，在修改了 acinclude.m4、configure.ac 之后，就需要通过 aclocal 重新生成 aclocal.m4 文件；如果直接通过 autoconf 配置，部分库会报 `aclocal-1.14: command not found` 错误。据说 [是由于时间戳导致](http://stackoverflow.com/questions/33278928/how-to-overcome-aclocal-1-15-is-missing-on-your-system-warning-when-compilin) 的，不过安装了 automake、autoconf 工具后，最简单的方式是执行 autoreconf 命令。

#### 修改Makefile.am

需要在源码的跟目录下执行 automake 命令更新，然后在 build 目录下直接重新执行 make 命令即可。


#### 非标准库

如果有些库没有安装在标准路径下，或者需要使用不同的版本库，那么就需要通过参数指定；以使用 MySQL 库为例，通常可以通过 `--with-libmysql=/opt/mysql/lib` 参数指定特定库路径。

当然，这需要在制作 configure.ac 文件时通过 `AC_ARG_WITH()` 函数指定；如果使用 `./configure --help` 查看时会发现没有提供该 `--with-libmysql` 参数，这时候就需要通过如下方式指定。

{% highlight text %}
----- 通过指定环境变量方式添加库以及头文件搜索路径
env CPPFLAGS="-I/include/path"  LDFLAGS="-L/lib/path"  ./configure --prefix=/opt/foobar

----- 也可以把env省掉，作用相同
CPPFLAGS="-I/include/path"  LDFLAGS="-L/lib/path"  ./configure --prefix=/...
{% endhighlight %}

另外，还可以通过 `LIBRARY_PATH` 环境变量指定编译期间搜索 lib 库的路径，使用冒号分割，此时会先搜索该变量指定的路径，如果找不到则搜索系统默认搜索路径；而 `LD_LIBRARY_PATH` 则用于指定程序运行期间查找so动态链接库的搜索路径。

#### 测试功能

automake 提供了简单测试功能，在运行程序时返回非 0 则认为失败，否则认为成功；示例如下，需要在 `Makefile.am` 中添加如下内容，然后通过 `make check` 执行检查，相关可以参考 [Tests](https://www.gnu.org/software/automake/manual/html_node/Tests.html) 。

{% highlight text %}
TESTS = check_money
check_PROGRAMS = check_money
check_money_SOURCES = check_money.c $(top_builddir)/src/money.h
check_money_CFLAGS = @CHECK_CFLAGS@
check_money_LDADD = $(top_builddir)/src/libmoney.la @CHECK_LIBS@
{% endhighlight %}

另外，可以在 `Makefile.am` 中，可以增加其它功能测试，例如内存泄露，增加如下内容，详细参考 [Parallel Test Harness](https://www.gnu.org/software/automake/manual/html_node/Parallel-Test-Harness.html) 。

<!-- LOG_COMPILER = env VALGRIND="@VALGRIND@" $(abs_top_srcdir)/testwrapper.sh  -->

#### LIBADD VS. LDADD

简单来说 `LIBADD` 用于库，`LDADD` 用于可执行文件。

例如，要通过 libtool 生成一个 libfoo.la 静态库，那么可以使用如下方式。

{% highlight text %}
libfoo_la_LIBADD = libbar.la
{% endhighlight %}

如果使用了非 libtool 库，那么就需要通过 `-L` 和 `-l` 参数选项。

{% highlight text %}
libfoo_la_LIBADD = libbar.la -L/opt/local/lib -lpng
{% endhighlight %}

不过一般来说，回通过 configure 脚本查找所依赖的库，然后通过 `AC_SUBST` 函数将其导出，然后使用如下方式引用。

{% highlight text %}
libfoo_la_LIBADD = libbar.la $(EXTRA_FOO_LIBS)
{% endhighlight %}

而对于一个程序则使用如下方式。

{% highlight text %}
myprog_LDADD = libfoo.la # links libfoo, libbar, and libpng to myprog.
{% endhighlight %}

<!--
Sometimes the boundaries are a bit vague. $(EXTRA_FOO_LIBS) could have been added to myprog_LDADD. Adding dependencies to a libtool (.la) library, and using libtool do all the platform-specific linker magic, is usually the best approach. It keeps all the linker metadata in the one place.
-->

### 总结

需要手动维护 configure.ac 和 Makefile.am 文件。

## CMake

在介绍示例之前，先说明一下 CMake 有两种编译方式：内部构建和外部构建。内部构建直接在源码目录下执行 `cmake .`，外部构建则会在一个目录下构建，不会影响原源码的结构。

其基本结构可以简单描述为：

1. 依赖 `CMakeLists.txt` 文件，项目主目标一个，主目录中可指定包含的子目录；
2. 在项目 `CMakeLists.txt` 中使用 `PROJECT` 指定项目名称，通过 `ADD_SUBDIRECTORY` 添加子目录；
3. 子目录 `CMakeLists.txt` 将从父目录 `CMakeLists.txt` 继承设置。

另外，上述通过 `PROJECT` 设置好工程后，可以通过 `${hello_SOURCE_DIR}` 引用，注意大小写。

因为 `CMakeLists.txt` 可执行脚本并通过脚本生成一些临时文件，因此 CMake 无法跟踪到底产生了那些临时文件，因此，没有办法提供一个可靠的 `make distclean` 方案，为此可以使用外部编译。

另外，也没提供 `make uninstall` 命令，卸载可以通过 `cat install_manifest.txt | sudo xargs rm`  命令执行删除或卸载。

### 简单示例

最简单示例单文件输出 `Hello World!`，只需创建两个文件：`main.c` 和 `CMakeLists.txt`；然后通过 `cmake . && make` 进行编译，要查看详细信息可以 `make VERBOSE=1` 或者 `VERBOSE=1 make` 。

{% highlight text %}
$ cat CMakeLists.txt
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(hello)                                              # 项目名称
SET(SRC_LIST main.c)                                        # 添加源码，可忽略
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

在上述 `MESSAGE()` 中，会打印变量用于调试，该变量是通过 `PROJECT()` 默认设定的变量，详见 `PROJECT()` 的使用。

调试的话可以使用 `cmake . -DCMAKE_BUILD_TYPE=Debug` ，不过此时使用的绝对地址。

### 配置文件

通常 `autotools` 会根据平台动态生成 `config.h` 文件，在 CMake 中同样可以生成。

除了支持脚本外，还包括了一些常见的模块 (Modules)，例如 CentOS 中保存在 `/usr/share/cmake/Modules/` 目录下，在使用时需要先通过 `INCLUDE()` 指令包含相应的模块。

{% highlight text %}
# usage: CHECK_INCLUDE_FILES (<header> <RESULT_VARIABLE> )
INCLUDE (CheckIncludeFiles)

CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES ("sys/param.h;sys/mount.h" HAVE_SYS_MOUNT_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
{% endhighlight %}

在执行时，会将结果保存在 `CMakeCache.txt` 文件中，类似如下；如果要重新生成则需要删除。

{% highlight text %}
//Have include HAVE_MALLOC_H
HAVE_MALLOC_H:INTERNAL=1
{% endhighlight %}

在 `config.h.in` 中添加如下内容。

{% highlight text %}
#cmakedefine HAVE_MALLOC_H 1
#cmakedefine HAVE_SYS_MOUNT_H
{% endhighlight %}

在检测后，会将 `#cmakedefine` 替换掉，如下。

{% highlight text %}
#define HAVE_MALLOC_H 1
#define HAVE_SYS_MOUNT_H

/* #undef HAVE_MALLOC_H 1 */
/* #define HAVE_SYS_MOUNT_H */
{% endhighlight %}

详细内容可以参考 [CMake:How To Write Platform Checks](https://itk.org/Wiki/CMake:How_To_Write_Platform_Checks) 。

### 安装

CMake 默认会在与源码目录相同的路径下生成二进制文件或者库文件，实际上可以通过如下方式进行设置。

{% highlight text %}
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
{% endhighlight %}

当然，对于单个可执行文件，也可以通过 `set_target_properties()` 指令设置如下的变量覆盖全局的参数。

{% highlight text %}
RUNTIME_OUTPUT_DIRECTORY
LIBRARY_OUTPUT_DIRECTORY
ARCHIVE_OUTPUT_DIRECTORY
{% endhighlight %}

#### 安装指令

在编译完成之后，可以通过 `make install` 进行安装，通过 `cat install_manifest.txt | xargs rm` 删除即可，不过需要提前指定安装规则。

{% highlight text %}
INSTALL(TARGETS foobar DESTINATION bin)
INSTALL(FILES foobar.h DESTINATION include)
{% endhighlight %}

如上的配置会将 `foobar` 和 `foobar.h` 安装到 `/usr/local/{bin,include}` 目录下即可，实际上对于安装目录而言，也可以直接使用 `INCLUDE(GNUInstallDirs)` 指定的目录。

对于 `TARGETS` 的配置，主要有三个参数，分别为 `ARCHIVE`、`LIBRARY`、`RUNTIME` ，一般会类似如下的方式编写。

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

其中上述的 `targets` 可以指定多个，而且可以是不同的类型，如二进制、动态库、静态库、头文件等。

上述指定的 `DESTINATION` 一般是相对路径，可以通过 `CMAKE_INSTALL_PREFIX` 指定其前缀，对于 Linux 默认是 `/usr/local/`；另外，其前面还可以指定 `DESTDIR` 目录。

除了使用 `TARGETS` 外，还可以使用 `FILES` 或者 `DIRECTORY` ，对于 `DIRECTORY` 使用比较灵活，常见需要注意的关键点如下。

##### 后缀符号

需要注意其后缀的 `/` 符号。

{% highlight text %}
#----- 会将目录复制成为 dst/src/{subdirs and files...}
install(DIRECTORY   myproj/src DESTINATION dst)

#----- 会将目录复制成为 dst/{subdirs and files...}
install(DIRECTORY   myproj/src/ DESTINATION dst)
{% endhighlight %}

##### 文件过滤

可以通过参数 `FILES_MATCHING` 用于指定操作档案的条件，可以使用 `PATTERN` 或 `REGEX` 两种匹配方式，要注意 `PATTERN` 会比对全路径而不只是文件名。

{% highlight text %}
INSTALL(DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")
{% endhighlight %}

以上会把 `src/` 底下所有文件后缀为 `.h` 的文件复制到 `include` 文件夹下，并且会保留原本目录树的结构。

另外，还可以在匹配条件后面通过 `EXCLUDE` 排除符合条件的文件或目录。

{% highlight text %}
INSTALL(DIRECTORY myapp/ mylib DESTINATION myproj PATTERN ".git" EXCLUDE)
{% endhighlight %}

### 打包

可以通过 CPack 进行打包，也就是将编译后的二进制进行打包，当然也可以打包源码，分别通过如下命令进行打包。

{% highlight text %}
make package_source
make package
{% endhighlight %}

### 宏和函数

同大多数脚本语言一样，CMake 中也有宏和函数的概念，关键字分别为 `macro` 和 `function`，具体用法如下：

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

如下是一个求和的宏定义。

{% highlight text %}
macro(sum outvar)
	set(_args ${ARGN})
	list(LENGTH _args argLength)
	if(NOT argLength LESS 4)　# 限制不能超过4个数字
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

`${ARGN}` 是 CMake 中的一个变量，指代宏中传入的多余参数；上述宏只定义了一个参数 `outvar`，其余需要求和的数字都是不定形式传入的，所以需要先将多余的参数传入一个单独的变量中。

函数与宏的区别是，函数中的变量是局部的，不能直接传出。


### 常用示例

#### 内置变量

如下是设置 C 编译器的参数，对于 CPP 则将 C 替换为 CXX 即可。

{% highlight text %}
set(CMAKE_C_COMPILER      "gcc" )               # 显示指定使用的编译器
set(CMAKE_C_FLAGS         "-std=c99 -Wall")     # 设置编译选项，也可以通过add_definitions添加编译选项
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g" )            # 调试包不优化
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG " )     # release包优化
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wno-sign-compare")   # 忽略某些告警
{% endhighlight %}

内置变量可在 cmake 命令中使用，如 `cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug` 。

<!--
EXECUTABLE_OUTPUT_PATH：可执行文件的存放路径
LIBRARY_OUTPUT_PATH：库文件路径
CMAKE_BUILD_TYPE:：build 类型(Debug, Release, ...)，
BUILD_SHARED_LIBS：Switch between shared and static libraries
-->


#### Build Type

除了上述 `CMAKE_C_FLAGS_DEBUG` 指定不同类型的编译选项外，还可以通过如下方式指定。

{% highlight text %}
set(CMAKE_BUILD_TYPE Debug CACHE STRING "set build type to debug")
if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Debug")
	SET(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    set(LIBRARIES Irrlicht_S.lib)
else()
    set(LIBRARIES Irrlicht.lib)
endif()
{% endhighlight %}

可以在命令行中通过如下方式编译 `cmake -DCMAKE_BUILD_TYPE=Debug ..`，最终的编译选项可以查看 `CMakeFiles/SRCFILE.dir/flags.make` 中的 `C_FLAGS` 选项，一般是 `CMAKE_C_FLAGS+CMAKE_C_FLAGS_MODE` 。

#### 测试用例

可以通过如下方式添加测试用例。

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

然后在编译时通过 `cmake .. -DWITH_UNIT_TESTS=ON` 执行，并通过 `make test` 进行测试。

另外，可以指定测试的参数，以及输出的匹配。

{% highlight text %}
ADD_TEST(test foobar 10 5)
SET_TESTS_PROPERTIES(test PROPERTIES PASS_REGULAR_EXPRESSION "ok")
{% endhighlight %}





















#### 文件渲染

可以通过如下脚本，动态生成。

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

通过 `CONFIGURE_FILE()` 将文件中的变量替换，然后在 `set_target_properties()` 设置，将在编译之前将绝对路径转换为相对路径，注意 `gcc_debug_fix.sh.in` 需要为可以执行文件。

#### 库参数

在编译不同的库时，可能需要使用不同的编译参数，那么此时就可以使用如下的方式修改。


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

默认是不支持 flex 和 bison 的，可以通过如下方式添加。


{% highlight text %}
#----- 生成的中间文件保存在${CMAKE_CURRENT_BINARY_DIR}目录下
include_directories(${CMAKE_CURRENT_BINARY_DIR} )

#----- 定义源码目录以及中间文件
SET(PARSER_DIR ${CMAKE_SOURCE_DIR}/driver/mysql)
SET(GEN_SOURCES
	${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h
	${CMAKE_CURRENT_BINARY_DIR}/parser.l.h
)

#----- 添加一个custom_target用于构建依赖，该target依赖${GEN_SOURCES}指向的文件
ADD_CUSTOM_TARGET(
        GenServerSource
        DEPENDS ${GEN_SOURCES}
)

#----- 告知${GEN_SOURCES}指向的文件是编译过程中生成的，以避免执行cmake命令的时候报文件找不到
SET_SOURCE_FILES_PROPERTIES(${GEN_SOURCES} GENERATED)

#----- 需要注意的是OUTPUTS一定要和${GEN_SOURCES}中文件一致，并且target设置对否则无法确保执行顺序
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

#----- 这里将flex和bison生成的.c文件编译到程序的动态库中，需要注意指定路径
add_library(driver SHARED
	sourcefile1.cc sourcefile2.cc sourcefile3.cc
	${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
	${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
)

#----- 指明动态库driver所需要的依赖，所以可以保证cmake会先生成GenServerSource, 而GenServerSource又
#      依赖${GEN_SOURCES}指明的文件， 而这些文件又是由两个ADD_CUSTOM_COMMAND命令来生成的
#      (outputs中给出的)，所以会先执行两个ADD_CUSTOM_COMMAND命令
ADD_DEPENDENCIES(driver GenServerSource)
{% endhighlight %}

#### 库使用

关于库的使用常见有如下的操作。

{% highlight text %}
----- 增加库的搜索路径
LINK_DIRECTORIES(./lib)

----- 生成库，可以是动态(SHARED)或者静态库(STATIC)
ADD_LIBRARY(hello SHARED ${SRC_LIST})

----- 指定生成对象时依赖的库
TARGET_LINK_LIBRARIES(hello A B.a C.so)

----- 自定义链接选项，单独对B.a使用--whole-archive选项
TARGET_LINK_LIBRARYIES(hello A -Wl,--whole-archive B.a -Wl,--no-whole-archive C.so)
{% endhighlight %}

在使用 `add_library(foo SHARED foo.c)` 时，不同平台输出有所区别，例如，`foo.dll(Windows)`、 `libfoo.so(Linux)`、 `libfoo.dylib(Mac)` 。

比如编写用于 lua 扩展的 C 模块，那么在进行 require 时，需要执行如下调用：

{% highlight lua %}
require 'libfoo'     --默认加载libfoo.[so|dll]，并且执行luaopen_libluafoo
require 'foo'        --加载foo.so,并且执行luaopen_luafoo
{% endhighlight %}

并且各个平台下各不相同，可以通过如下方式修改前缀以及后缀：

{% highlight text %}
SET_TARGET_PROPERTIES(foo PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(foo PROPERTIES SUFFIX "so")
{% endhighlight %}


<!--
 对了，吐槽一下mac的rpath也是件麻烦的事情


# enable @rpath in the install name for any shared library being built
set(CMAKE_MACOSX_RPATH 1)

# the install RPATH for bar to find foo in the install tree.
# if the install RPATH is not provided, the install bar will have none
set_target_properties(bar PROPERTIES INSTALL_RPATH "@loader_path/../lib")
-->

#### 文件路径

CMake 在编译时会使用到绝对路径，而在打印日志时可能会导致文件长度过大，有几种方式修改。

{% highlight c %}
#include <string.h>
#define __FILENAME__                             \
        (strrchr(__FILE__, '/') ?                \
	strrchr(__FILE__, '/') + 1 : __FILE__)
{% endhighlight %}

不过这种方式会在执行时计算，也可以使用如下宏定义，此时会使用相对路径。

{% highlight text %}
SET(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
{% endhighlight %}

或者只使用文件名。

{% highlight text %}
SET(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(notdir $(abspath $<))\"'")
{% endhighlight %}

#### 常用命令

指令是大小写无关的，参数和变量是大小写相关的，参数使用空格或者分号隔开。如 `MESSAGE (STATUS "This is BINARY dir" ${HELLO_BINARY_DIR})` 和 `MESSAGE (STATUS "This is BINARY dir ${HELLO_BINARY_DIR}")` 相同。

变量使用 `${}` 方式取值，但是在 `IF` 控制语句中是直接使用变量名。

{% highlight text %}
PROJECT (projectname [CXX] [C] [Java])
  定义工程名称以及语言，会隐式定义 projectname_BINARY_DIR、projectname_SOURCE_DIR变量，
  同时也会定义 PROJECT_BINARY_DIR、PROJECT_SOURCE_DIR 与前两者相同，建议使用后者。

SET(VAR [VALUE] [CACHE TYPE DOCSTRING [FORCE]])
  可以用来显式的定义变量。

MESSAGE([SEND_ERROR | STATUS | FATAL_ERROR] "message to display" ...)
  向终端输出用户定义的信息，SEND_ERROR(产生错误，生成过程被跳过)，SATUS(输出前缀为-的信息)、
  FATAL_ERROR(立即终止所有cmake过程)。

ADD_DEFINITIONS(-DMICRO_1 ...)
  添加宏定义。

ADD_EXECUTABLE(hello ${SRC_LIST})
  该工程会生成一个文件名为 hello 的可执行文件，相关的源文件是 SRC_LIST 。

ADD_SUBDIRECTORY(source_dir [binary_dir] [EXCLUDE_FROM_ALL])
  指定当前工程的

CONFIGURE_FILE(intput output [COPYONLY] [ESCAPE_QUOTES] [@ONLY])
  将文件 input 拷贝到 output 然后替换文件内容中引用到的变量值；如果用相对路径，则 input 相对
  的是当前源码路径，output 相对于二进制文件路径。该命令替换掉在输入文件中，以 ${VAR} 格式或
  @VAR@ 格式引用的任意变量，如同它们的值是由CMake确定的一样。 如果一个变量还未定义，它会被替
  换为空。如果指定了COPYONLY选项，那么变量就不会展开。如果指定了ESCAPE_QUOTES选项，那么所有被
  替换的变量将会按照C语言的规则被转义。该文件将会以CMake变量的当前值被配置。

----- 查看当前支持的选项
cmake .. -LH
{% endhighlight %}



## 其它

可以通过 `pkg-config` 命令来检索系统中安装库文件的信息，通常用于库的编译和连接。

如果库的头文件不在 `/usr/include` 目录中，那么在编译的时候需要用 `-I` 参数指定其路径；同样，链接时可以通过 `-L` 参数指定库，这样就导致了编译命令界面的不统一。

为了保证统一，通过库提供的一个 `.pc` 文件获得库的各种必要信息的，包括版本信息、编译和连接需要的参数等，在需要的时候可以通过提供的参数，如 `--cflags`、`--libs`，将所需信息提取出来供编译和连接使用。

主要包括了：A) 检查库的版本号；B) 获得编译预处理参数，如宏定义，头文件的路径；C) 获得编译参数，如库及其依赖的其他库的位置，文件名及其他一些连接参数；D) 自动加入所依赖的其他库的设置。

在 CentOS 中默认会保存在 `/usr/lib64/pkgconfig`、`/usr/share/pkgconfig` 目录下。

{% highlight text %}
----- 查看所有.pc
$ pkg-config --list-all

----- 编译
$ gcc -c `pkg-config --cflags glib-2.0` sample.c

----- 链接
$ gcc sample.o -o sample `pkg-config --libs glib-2.0`

----- 编译链接合并
$ gcc sample.c -o sample `pkg-config --cflags --libs glib-2.0`
{% endhighlight %}

另外，可以通过环境变量 `PKG_CONFIG_PATH` 指定搜索路径。

## 参考

可以查看官方文档 [automake](https://www.gnu.org/software/automake/manual/automake.html)，[GNU Autoconf - Creating Automatic Configuration Scripts](https://www.gnu.org/software/autoconf/manual/) 。

关于 CMake 相关的文档可以参考 [CMake 实践](/reference/linux/CMake_Practice.pdf) ，或者参考 [CMake 入门实战](http://hahack.com/codes/cmake/) 以及 [CMake Tutorial](https://cmake.org/cmake-tutorial/)；搭建 GTest 环境可以参考 [Unit testing with GoogleTest and CMake](http://kaizou.org/2014/11/gtest-cmake/) 。

常见的命令参考 [cmake-commands](https://cmake.org/cmake/help/v3.0/manual/cmake-commands.7.html) 。


<!--
The new guide of the conversation in Autoconf and Make
http://www.edwardrosten.com/code/autoconf/

A tutorial for porting to autoconf & automake
http://mij.oltrelinux.com/devel/autoconf-automake/

运用Autoconf和Automake生成Makefile的学习之路
http://www.cnblogs.com/ericdream/archive/2011/12/09/2282359.html

如何根据configure.ac和Makefile.am为开源代码产生当前平台的Makefile
http://www.51cos.com/?p=1649

Colletcd的编译系统介绍
https://collectd.org/wiki/index.php/Build_system

AC_CHECK_FUNCS
AC_CHECK_LIB
https://autotools.io/autoconf/finding.html
概念：GNU构建系统和Autotool
http://www.pchou.info/linux/2016/09/16/gnu-build-system-1.html

CMake如何查找链接库
http://www.yeolar.com/note/2014/12/16/cmake-how-to-find-libraries/


http://huqunxing.site/2016/08/26/CMake%E5%85%A5%E9%97%A8%E6%8C%87%E5%8D%97/#编译32位和64位程序

CPACK_GENERATOR RPM TBZ2 ZIP

cpack -D CPACK_GENERATOR="ZIP;TGZ" /path/to/build/tree

 AC_PLUGIN([cpu],                 [$plugin_cpu],             [CPU usage statistics])


http://cmake.3232098.n2.nabble.com/Adding-a-custom-line-to-CMake-s-makefiles-td5984979.html


%defattr(file mode, user, group, dir mode)

默认的是 `%defattr(644, user, group, 755)` 或者 `%defattr(-, user, group)` ，对于普通文件是 644 而可执行文件以及目录使用 755 。


-->



{% highlight text %}
{% endhighlight %}
