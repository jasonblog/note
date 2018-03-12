---
title: C 编译链接
layout: post
comments: true
language: chinese
category: [program,misc]
keywords: c,编译链接
description: 详细介绍下与 C 语言相关的概念。
---

详细介绍下与 C 语言相关的概念。

<!-- more -->

## 目标文件

目标文件是编译器编译完后，还没有进行链接的文件，通常采用和可执行文件相同的存储格式，如 Windows 平台下的 `Portable Executable, PE`，Linux 平台下的 `Executable Linkale Format, ELF`，它们都是 `Common File Format, COFF` 的变种。

除可执行文件，动态链接库 `(Dynamic Linking Library)` 和静态链接库 `(Statci Linking Library)` 也是按照可执行文件进行存储。

对于文件的类型可以通过 file 命令进行查看，常见的类型包括了：

* 可重定位文件(Relocatable File)<br>主要包含了代码和数据，主要用来链接成可执行文件或共享目标文件，如 `.o` 文件。
* 可执行文件(Executable File)<br>主要是可以直接执行的程序，如 `/bin/bash` 。
* 共享目标文件(Shared Object File)<br>包含了代码和数据，常见的有动态和静态链接库，如 `/lib64/libc-2.17.so` 。
* 核心转储文件(Core Dump File)<br>进程意外终止时，系统将该进程的地址空间的内容及终止时的一些其他信息转储到该文件中。

### 示例程序

目标文件通过段 (Segment) 进行存储，在 Windows 中可以通过 `Process Explorer` 查看进程相关信息，Linux 可以通过 `objdump` 查看。主要的段包括了 `.text` `.data` `.bss(Block Started by Symbol)`，当然还有一些其它段，如 `.rodata` `.comment` 等。

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

然后，可以通过 `gcc -c section.c` 编译，编译完上述的文件之后，可以通过 `objdump -h` 查看头部信息，也可以通过 `-x` 参数查看更详细的信息。

比较重要的是 `File off` 和 `Size` 信息，一般头部信息的大小为 `0x34` ，因此第一个段的地址就会从 `0x34` 开始 (地址从 0 开始计数)，另外，由于需要 4bytes 对齐，因此会从 `54(0x36)` 开始。也可以通过 size 查看，采用的是十进制，最后会用十进制和十六进制表示总的大小。

数据段 `.data` 用来保存已经初始化了的全局变量和局部静态变量，如上述的 `global_init_var` 和 `static_var` 。

只读数据段 `.rodata` ，主要用于保存常量，如 `printf()` 中的字符串和 `const` 类型的变量，该段在加载时也会将其设置为只读。

`BSS` 段保存了未初始化的全局变量和局部静态变量，如上述 `global_uninit_var` 和 `static_var2` 。

<!--
正常应该是 8 字节，但是查看时只有 4 字节，通过符号表(Symbol Table)可以看到，只有 static_var2 保存在 .bss 段，而 global_uninit_var 未存放在任何段，只是一个未定义的 COMMON 符号。这与不同的语言和编译器实现有关，有些编译器会将全局的为初始化变量存放在目标文件 .bss 段，有些则不存放，只是预留一个未定义的全局变量符号，等到最终链接成可执行文件时再在 .bss 段分配空间。
-->

`.text` 为代码段，`.data` 保存含初始值的变量，`.bss` 只保存了变量的符号。


### 添加一个段

将以个二进制文件，如图片、MP3 音乐等作为目标文件的一个段。如下所示，此时可以直接声明 `_binary_example_png_start` 和 `_binary_example_png_end` 并使用。

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

如果在编译时想将某个函数或者变量放置在一个段里，可以通过如下的方式进行。

{% highlight c %}
__attribute__((section("FOO"))) int global = 42;
__attribute__((section("BAR"))) void foo() { }
{% endhighlight %}

### 运行库

在 `main()` 运行之前通常会先执行一段代码，运行这些代码的函数称为 **入口函数** 或 **入口点** ，大致的步骤如下：

* 操作系统创建进程后，把控制权交给程序入口，这个入口往往是运行库中的某个入口函数。
* 入口函数对运行库和程序运行环境进行初始化，包括堆、I/O、线程、全局变量构造等。
* 入口函数在完成初始化之后，调用 main 函数，正式开始执行程序主体部分。
* `main()` 执行完后，返回到入口函数，入口函数进行清理工作，包括全局变量析构、堆销毁、关闭 IO 等，然后进行系统调用结束进程。

## 链接过程

在程序由源码到可执行文件的编译过程实际有预处理 (Propressing)、编译 (Compilation)、汇编 (Assembly) 和链接 (Linking) 四步，在 `gcc` 中分别使用 `ccp`，`cc1`，`as`，`ld` 来完成。

关于链接方面可以直接从网上搜索 《linker and loader》。

![compile link gcc details]({{ site.url }}/images/linux/compile-link-gcc-details.jpg "compile link gcc details"){: .pull-center }

### 预编译

将源代码和头文件通过预编译成一个 `.i` 文件，相当与如下命令。

{% highlight text %}
$ gcc -E main.c -o main.i          # C
$ cpp main.c > main.i              # CPP
{% endhighlight %}

与编译主要是处理源码中以 `"#"` 开始的与编译指令，主要的处理规则是：

* 删除所有的 `"#define"` ，并且展开所有的宏定义。
* 处理所有条件预编译指令，比如 `"#if"`、`"#ifdef"`、`"#elif"`、`"#else"`、`"#endif"` 。
* 处理 `"#include"` ，将被包含的文件插入到该预编译指令的位置，该过程是递归的。
* 删除多有的注释 `"//"` 和 `"/* */"` 。
* 添加行号和文件名标识，如 `#2 "main.c" 2` ，用于编译时产生调试用的行号以及在编译时产生错误或警告时显示行号。
* 保留所有的 `"#pragma"` 编译器指令，因为编译器需要使用它们。

经过预编译后的 `.i` 文件不包含任何宏定义，因为所有的宏已经被展开，并且包含的文件也已经被插入到 `.i` 文件中。所以，当无法判断宏定义是否正确或头文件包含是否正确时，可以查看该文件。

### 编译

编译过程就是把预处理后的文件进行一系列的词法分析、语法分析、语义分析以及优化后生成相应的汇编代码文件，这个是核心部分，也是最复杂的部分。

gcc 把预编译和编译合并成一个步骤，对于 C 语言使用的是 `cc1` ，C++ 使用的是 `cc1obj` 。

{% highlight text %}
$ gcc -S hello.i -o hello.s
$ gcc -S main.c -o main.s
{% endhighlight %}

<!-- $ /usr/lib/gcc/i386-linux-gnu/4.7/cc1 main.c -->

### 汇编

汇编器是将汇编代码转化成机器码，每条汇编语句几乎都对应一条机器指令。汇编器不需要复杂的语法语义，也不用进行指令优化，只是根据汇编指令和机器指令的对照表一一翻译即可。

{% highlight text %}
$ gcc -c hello.s -o hello.o
$ as main.s -o main.o
$ gcc -c main.s -o main.o
$ gcc -c main.c -o main.o
{% endhighlight %}

### 链接

可以通过 `gcc hello.c -o hello -v` 查看。

{% highlight text %}
$ gcc hello.o -o hello.exe
{% endhighlight %}

## 静态库和动态库

库有动态与静态两种，Linux 中动态通常用 `.so` 为后缀，静态用 `.a` 为后缀，如：`libhello.so` `libhello.a`。为了在同一系统中使用不同版本的库，可以在库文件名后加上版本号为后缀，例如：`libhello.so.1.0`，然后，使用时通过符号链接指向不同版本。

{% highlight text %}
# ln -s libhello.so.1.0 libhello.so.1
# ln -s libhello.so.1 libhello.so
{% endhighlight %}

在动态链接的情况下，可执行文件中还有很多外部符号的引用还处于无效地址的状态，因此需要首先启一个 **动态链接器 (Dynamic Linker)**，该连接器的位置在程序的 `".interp"` (interpreter) 中指定，可以通过如下的命令查询。

{% highlight text %}
$ readelf -l a.out | grep interpreter
{% endhighlight %}

### 共享库的更新

对于共享库更新时通常会有兼容更新和不兼容更新，此处所说的兼容是指二进制接口，即 `ABI (Application Binary Interface)`。

为了保证共享库的兼容性， Linux 采用一套规则来命名系统中的共享库，简单来说，其规则如下 `libname.so.x.y.z`，name 为库的名称，x y z 的含义如下：

* x，主版本号(Major Version Number)，库的重大升级，不同的主版本号之间不兼容。
* y，次版本号(Minor Version Number)，库的增量升级，增加了一些新的接口，且保持原来的符号不变。
* z，发布版本号(Release Version Number)，库的一些错误的修正、性能的改进等，并不添加任何新的接口，也不对接口进行改进。

由于历史的原因最基本的 C 语言库 glibc 动态链接库不使用这种规则，如 `libc-x.y.z.so` 、`ld-x.y.z.so` 。

下面这篇论文， Library Interface Versioning in Solaris and Linux ，对 Salaris 和 Linux 的共享库版本机制和符号版本机制做了非常详细的介绍。

在 Linux 中采用 SO-NAME 的命名机制，每个库会对应一个 SO-NAME ，这个 SO-NAME 只保留主版本号，也即 SO-NAME 规定了共享库的接口。

### 路径问题

如果动态库不在搜索路径中，则会报 `cannot open shared object file: No such file or directory` 的错误。可以通过 `gcc --print-search-dirs` 命令查看默认的搜索路径。

查找顺序通常为：

1. 查找程序编译指定的路径，保存在 `.dynstr` 段，其中包含了一个以冒号分割的目录搜索列表。
2. 查找环境变量 `LD_LIBRARY_PATH`，以冒号分割的目录搜索列表。
3. 查找 `/etc/ld.so.conf` 。
4. 默认路径 `/lib` 和 `/usr/lib` 。

为了让执行程序顺利找到动态库，有三种方法：

##### 1. 复制到指定路径

把库拷贝到查找路径下，通常为 `/usr/lib` 和 `/lib` 目录下，或者通过 `gcc --print-search-dirs` 查看动态库的搜索路径。

##### 2. 添加链接选项

编译时添加链接选项，指定链接库的目录，此时会将该路径保存在二进制文件中。

{% highlight text %}
$ gcc -o test test.o -L. -lhello -Wl,-rpath,/home/lib:.
$ readelf -d test | grep RPATH
$ objdump -s -j .dynstr test                     // 查看.dynstr段的内容
{% endhighlight %}

##### 3. 设置环境变量

执行时在 `LD_LIBRARY_PATH` 环境变量中加上库所在路径，例如动态库 `libhello.so` 在 `/home/test/lib` 目录下。

{% highlight text %}
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/test/lib
{% endhighlight %}

##### 4. 修改配置文件

修改 `/etc/ld.so.conf` 文件，把库所在的路径加到文件中，并执行 `ldconfig` 刷新配置。动态链接库通常保存在 `/etc/ld.so.cache` 文件中，执行 `ldconfig` 可以对其进行刷新。


### 静态连接库

当要使用静态的程序库时，连接器会找出程序所需的函数，然后将它们拷贝到执行文件，由于这种拷贝是完整的，所以一旦连接成功，静态程序库也就不再需要了，缺点是占用的空间比较大。通常，静态链接的程序要比共享函数库的程序运行速度上快一些，大概 1-5％ 。

<!--
动态库会在执行程序内留下一个标记指明当程序执行时，首先必须载入这个库。
-->

注意，对于 CentOS 需要安装 `yum install glibc-static` 库。

Linux 下进行连接的缺省操作是首先连接动态库，也就是说，如果同时存在静态和动态库，不特别指定的话，将与动态库相连接。

现在假设有一个 hello 程序开发包，它提供一个静态库 `libhello.a`，一个动态库 `libhello.so`，一个头文件 `hello.h`，头文件中提供 `foobar()` 这个函数的声明。

下面这段程序 `main.c` 使用 hello 库中的 `foobar()` 函数。

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

生成静态库，先对源文件进行编译；然后使用 `ar(archive)` 命令连接成静态库。

{% highlight text %}
$ gcc -c hello.c -o hello.o
$ gcc -c foobar.c -o foobar.o
$ ar crv libhello.a hello.o foobar.o
$ ar -t libhello.a                              // 查看打包的文件
{% endhighlight %}

`ar` 实际是一个打包工具，可以用来打包常见文件，不过现在被 `tar` 替代，目前主要是用于生成静态库，详细格式可以参考 [ar(Unix) wiki](http://en.wikipedia.org/wiki/Ar_(Unix)) 。

{% highlight text %}
$ echo "hello" > a.txt && echo "world" > b.txt
$ ar crv text.a a.txt b.txt
$ cat text.a
{% endhighlight %}

与静态库连接麻烦一些，主要是参数问题。

{% highlight text %}
$ gcc main.c -o test -lhello                    // 库在默认路径下，如/usr/lib
$ gcc main.c -lhello -L. -static -o main        // 通过-L指定库的路径

$ gcc main.o -o main -WI,-Bstatic -lhello       // 报错，显示找不到-lgcc_s
{% endhighlight %}

注意：这个特别的 `"-WI,-Bstatic"` 参数，实际上是传给了连接器 `ld`，指示它与静态库连接，如果系统中只有静态库可以不需要这个参数； 如果要和多个库相连接，而每个库的连接方式不一样，比如上面的程序既要和 `libhello` 进行静态连接，又要和 `libbye` 进行动态连接，其命令应为：

{% highlight text %}
$ gcc testlib.o -o test -WI,-Bstatic -lhello -WI,-Bdynamic -lbye
{% endhighlight %}

最好不要进行分别编译、链接，因为在生成可执行文件时往往需要很多的其他文件，可以通过 `-v` 选项进行查看，如果通过如下方式进行编译通常会出现错误。

{% highlight text %}
$ gcc -c main.c
$ ld main.o -L. -lhello
{% endhighlight %}

### 动态库

用 gcc 编绎该文件，在编绎时可以使用任何编绎参数，例如 `-g` 加入调试代码等；`-fPIC` 生成与位置无关的代码（可以在任何地址被连接和装载）。

{% highlight text %}
$ gcc -c -fPIC hello.c -o hello.o

$ gcc -shared -Wl,-soname,libhello.so.1 -o libhello.so.1.0 hello.o // 生成动态库，可能存在多个版本，通常指定版本号

$ ln  -s  libhello.so.1.0  libhello.so.1                           // 另外再建立两个符号连接
$ ln  -s  libhello.so.1  libhello.so

$ gcc -fPIC -shared -o libhello.so hello.c                         // 当然对于上述的步骤可以通过一步完成

$ readelf -d libhello.so.1.0 | grep SONAME                         // 查看对应的soname
$ nm -D libhello.so                                                // 查看符号
{% endhighlight %}

最重要的是传 `-shared` 参数使其生成是动态库而不是普通执行程序； `-Wl` 表示后面的参数也就是 `-soname,libhello.so.1` 直接传给连接器 `ld` 进行处理。

实际上，每一个库都有一个 `soname` ，当连接器发现它正在查找的程序库中有这样一个名称，连接器便会将 `soname` 嵌入连结中的二进制文件内，而不是它正在运行的实际文件名，在程序执行期间，程序会查找拥有 `soname` 名字的文件，而不是库的文件名，换句话说，`soname` 是库的区分标志。

其目的主要是允许系统中多个版本的库文件共存，习惯上在命名库文件的时候通常与 `soname` 相同 `libxxxx.so.major.minor` 其中，`xxxx` 是库的名字， `major` 是主版本号， `minor` 是次版本号。

### 查看库中的符号

有时候可能需要查看一个库中到底有哪些函数，`nm` 命令可以打印出库中的涉及到的所有符号，库既可以是静态的也可以是动态的。

`nm` 列出的符号有很多，常见的有三种：

* 在库中被调用，但并没有在库中定义(表明需要其他库支持)，用U表示；
* 库中定义的函数，用T表示，这是最常见的；
* 所谓的“弱态”符号，它们虽然在库中被定义，但是可能被其他库中的同名符号覆盖，用W表示。

例如，希望知道上文提到的 `hello` 库中是否定义了 `printf()` 。

{% highlight text %}
$ nm libhello.so
{% endhighlight %}

发现其中没有 `printf()` 的定义，取而代之的是 `puts()` 函数，而且为 `U` ，表示符号 `puts` 被引用，但是并没有在函数内定义，由此可以推断，要正常使用 `hello` 库，必须有其它库支持，再使用 `ldd` 命令查看 `hello` 依赖于哪些库：

{% highlight text %}
$ ldd -v hello
$ readelf -d hello     直接使用readelf
{% endhighlight %}

每行 `=>` 前面的，为动态链接程序所需的动态链接库的名字；而 `=>` 后面的，则是运行时系统实际调用的动态链接库的名字。所需的动态链接库在系统中不存在时，`=>` 后面将显示 `"not found"`，括号所括的数字为虚拟的执行地址。

常用的系统动态链接库有：

{% highlight text %}
libc.so.x        基本C库
ld-linux.so.2    动态装入库(用于动态链接库的装入及运行)
{% endhighlight %}


## 动态库加载API

对于 Linux 下的可执行文件 ELF 使用如下命令查看，可以发现其中有一个 `.interp` 段，它指明了将要被使用的动态链接器 (`/lib/ld-linux.so`)。

{% highlight text %}
$ readelf -l EXECUTABLE
{% endhighlight %}

对于动态加载的函数主要包括了下面的四个函数，需要 `dlfcn.h` 头文件，定义在 `libdl.so` 库中。

{% highlight text %}
void *dlopen( const char *file, int mode );
  用来打开一个文件，使对象文件可被程序访问，同时还会自动解析共享库中的依赖项，这样，如果打开了一个
    依赖于其他共享库的对象，它就会自动加载它们，该函数返回一个句柄，该句柄用于后续的 API 调用。
  mode 参数通知动态链接器何时执行再定位，有两个可能的值：
    A) RTLD_NOW，表明动态链接器将会在调用 dlopen 时完成所有必要的再定位；
    B) RTLD_LAZY，只在需要时执行再定位。

void *dlsym( void *restrict handle, const char *restrict name );
  通过句柄和连接符名称获取函数名或者变量名。

char *dlerror();
  返回一个可读的错误字符串，该函数没有参数，它会在发生前面的错误时返回一个字符串，在没有错误发生时返回NULL。

char *dlclose( void *handle ); 
  通知操作系统不再需要句柄和对象引用了。它完全是按引用来计数的，所以同一个共享对象的多个用户相互间
    不会发生冲突（只要还有一个用户在使用它，它就会待在内存中）。
    任何通过已关闭的对象的 dlsym 解析的符号都将不再可用。
{% endhighlight %}

有了 ELF 对象的句柄，就可以通过调用 dlsym 来识别这个对象内的符号的地址了。该函数采用一个符号名称，如对象内的一个函数的名称，返回值为对象符号的解析地址。

下面是一个动态加载的示例 [github libdl.c]({{ site.example_repository }}/c_cpp/c/libdl.c)，通过如下的命令进行编译，其中选项 `-rdynamic` 用来通知链接器将所有符号添加到动态符号表中（目的是能够通过使用 dlopen 来实现向后跟踪）。

{% highlight text %}
$ gcc -rdynamic -o dl library_libdl.c -ldl        # 编译
$ ./dl                                            # 测试
> libm.so cosf 0.0
   1.000000
> libm.so sinf 0.0
   0.000000
> libm.so tanf 1.0
   1.557408
> bye
{% endhighlight %}

另外，可以通过如下方式简单使用。

{% highlight text %}
$ cat caculate.c                                     # 查看动态库源码
int add(int a, int b) {
    return (a + b);
}
int sub(int a, int b) {
    return (a - b);
}
$ gcc -fPIC -shared caculate.c -o libcaculate.so     # 生成动态库
$ cat foobar.c                                       # 测试源码
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef int (*CAC_FUNC)(int, int);                           // 定义函数指针类型
int main(int argc, char** argv) {
    void *handle;
    char *error;
    CAC_FUNC cac_func = NULL;

    if ( !(handle=dlopen("./libcaculate.so", RTLD_LAZY)) ) { // 打开动态链接库
        fprintf(stderr, "!!! %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    cac_func = dlsym(handle, "add");                         // 获取一个函数
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "!!! %s\n", error);
        exit(EXIT_FAILURE);
    }
    printf("add: %d\n", (cac_func)(2,7));

    dlclose(handle);                                         // 关闭动态链接库
    exit(EXIT_SUCCESS);
}
$ gcc -rdynamic -o foobar foobar.c -ldl              # 编译测试
{% endhighlight %}

<!-- https://www.ibm.com/developerworks/cn/linux/l-elf/part1/index.html -->

## 常用命令

### objdump

详细参考 `man objdump` 。

{% highlight text %}
-h, --section-headers, --headers
  查看目标文件的头部信息。
-x, --all-headers
  显示所有的头部信息，包括了符号表和重定位表，等价于 -a -f -h -p -r -t 。
-s, --full-contents
  显示所请求段的全部信息，通常用十六进制表示，默认只会显示非空段。
-d, --disassemble
  反汇编，一般只反汇编含有指令的段。
-t, --syms
  显示符号表，与nm类似，只是显示的格式不同，当然显示与文件的格式相关，对于ELF如下所示。
  00000000 l    d  .bss   00000000 .bss
  00000000 g       .text  00000000 fred
{% endhighlight %}

<!--
第一列为符号的值，有时是地址；下一个是用字符表示的标志位；接着是与符号相关的段，*ABS* 表示段是绝对的（没和任何段相关联）， *UND* 表示未定义；对于普通符号(Common Symbols)表示对齐，其它的表示大小；最后是符号的名字。<br><br>

对于标志组的字符被分为如下的 7 组。
<ol type="A"><li>
    "l(local)" "g(global)" "u(unique global)" " (neither global nor local)" "!(both global and local)"<br>
    通常一个符号应该是 local 或 global ，但还有其他的一些原因，如用于调试、"!"表示一个bug、"u"是 ELF 的扩展，表示整个进程中只有一个同类型同名的变量。</li><br><li>

    "w(weak)" " (strong)"<br>
    表示强或弱符号。</li><br><li>

    "C(constructor)" " (ordinary)"<br>
    为构造函数还是普通符号。</li><br><li>

    "W(warning)" " (normal symbol)"<br>
    如果一个含有警告标志的符号被引用时，将会输出警告信息。</li><br><li>

    "I"
   "i" The symbol is an indirect reference to another symbol (I), a function to be evaluated
       during reloc processing (i) or a normal symbol (a space).

   "d(debugging symbol)" "D(dynamic symbol)" " (normal symbol)"<br>
    表示调试符号、动态符号还是普通的符号。</li><br><li>

   "F(function)" "f(file)" "O(object)" " (normal)"<br>
    表示函数、文件、对象或只是一个普通的符号。
-->

### strip

我们知道二进制的程序中包含了大量的符号表格(symbol table)，有一部分是用来 gdb 调试提供必要信息的，可以通过如下命令查看这些符号信息。

{% highlight text %}
$ readelf -S hello
{% endhighlight %}

其中类似与 `.debug_xxxx` 的就是 gdb 调试用的。去掉它们不会影响程序的执行。

{% highlight text %}
$ strip hello
{% endhighlight %}

### objcopy

用于转换目标文件。

{% highlight text %}
常用参数：
  -S / --strip-all
    不从源文件中拷贝重定位信息和符号信息到输出文件(目的文件)中去。
  -I bfdname/--input-target=bfdname
    明确告诉程序源文件的格式是什么，bfdname是BFD库中描述的标准格式名。
  -O bfdname/--output-target=bfdname
    使用指定的格式来写输出文件(即目标文件)，bfdname是BFD库中描述的标准格式名，
    如binary(raw binary 格式)、srec(s-record 文件)。
  -R sectionname/--remove-section=sectionname
    从输出文件中删掉所有名为section-name的段。
{% endhighlight %}

上一步的 strip 命令只能拿掉一般 symbol table，有些信息还是沒拿掉，而这些信息对于程序的最终执行没有影响，如: `.comment` `.note.ABI-tag` `.gnu.version` 就是完全可以去掉的。

所以说程序还有简化的余地，我们可以使用 objcopy 命令把它们抽取掉。

{% highlight text %}
$ objcopy -R .comment -R .note.ABI-tag -R .gnu.version hello hello1
{% endhighlight %}

### readelf

用于读取 ELF 格式文件，包括可执行程序和动态库。

{% highlight text %}
常用参数：
  -a --all
    等价于-h -l -S -s -r -d -V -A -I
  -h --file-header
    文件头信息；
  -l --program-headers
    程序的头部信息；
  -S --section-headers
    各个段的头部信息；
  -e --headers
    全部头信息，等价于-h -l -S；

示例用法：
----- 读取dynstr段，包含了很多需要加载的符号，每个动态库后跟着需要加载函数
$ readelf -p .dynstr hello
----- 查看是否含有调试信息
$ readelf -S hello | grep debug
{% endhighlight %}

<!--
readelf  -S hello
readelf -d hello

  --sections
An alias for –section-headers
-s –syms 符号表 Display the symbol table
--symbols
An alias for –syms
-n –notes 内核注释 Display the core notes (if present)
-r –relocs 重定位 Display the relocations (if present)
-u –unwind Display the unwind info (if present)
-d --dynamic
  显示动态段的内容；
-V –version-info 版本 Display the version sections (if present)
-A –arch-specific CPU构架 Display architecture specific information (if any).
-D –use-dynamic 动态段 Use the dynamic section info when displaying symbols
-x –hex-dump=<number> 显示 段内内容Dump the contents of section <number>
-w[liaprmfFso] or
-I –histogram Display histogram of bucket list lengths
-W –wide 宽行输出 Allow output width to exceed 80 characters
-H –help Display this information
-v –version Display the version number of readelf
-->


## 其它

#### 1. 静态库生成动态库

可以通过多个静态库生成动态库，而实际上静态库是一堆 `.o` 库的压缩集合，而生成动态库需要保证 `.o` 编译后是与地址无关的，也就是添加 `-fPIC` 参数。


{% highlight text %}
{% endhighlight %}

