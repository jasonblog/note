# gcc/g++链接时.o文件以及库的顺序问题


- 1.1 写在前面
- 1.2 几个名词
- 1.3 技术铺垫
    - 1.3.1 编译动态库时的符号解析
    - 1.3.2 linux下查看一个可执行文件或动态库依赖哪些动态库的办法
    - 1.3.3 load 动态库过程
- 1.4 gcc/g++链接时对库的顺序要求
    - 1.4.1 对于library的查找
    - 1.4.2 对于符号的查找
- 1.5 –as-needed对链接动态库的影响
    - 1.5.1 链接主程序模块或者是静态库的时的‘undefined reference to: xxx’
    - 1.5.2 编译动态库（shared library）的时候会导致一个比较隐晦的错误
- 1.6 对链接顺序导致问题的解决方案
    - 1.6.1 在项目开发过层中尽量让lib是垂直关系，避免循环依赖；越是底层的库，越是往后面写！
    - 1.6.2 通过-(和-)强制repeat


##1.1 写在前面

最近换了xubuntu12.4，把原来的项目co出来编译的时候报“undefined reference to”。猜测是gcc的版本问题，用-v跟踪一下，发现gcc-4.6默认开 启了ld的–as-needed选项。关闭该选项（–no-as-needed）后编译正常。深 入挖掘发现还是一个比较有意思的问题。

##1.2 几个名词

- gcc： 后面不特殊说明gcc代表gcc/g++。
- 主程序块： 只包含main的binary，可执行程序。

##1.3 技术铺垫

 
###1.3.1 编译动态库时的符号解析

有符号解析不了的时候，gcc不会直接报错而是假设load的时候地址重定位修正。

###1.3.2 linux下查看一个可执行文件或动态库依赖哪些动态库的办法

你有一个library或者是可执行文件，你可以这样查看他的依赖关系：

- readelf -d

```sh
ocaml@ocaml:~$ readelf -d PyGalaxy.so 

Dynamic section at offset 0x7dd8 contains 26 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libGalaxyParser.so]
 ...
 0x000000000000000c (INIT)               0x18e0
 0x000000000000000d (FINI)               0x6398
```

- ldd工具,如下：

```sh
ocaml@ocaml:~$ ldd PyGalaxy.so 
    linux-vdso.so.1 =>  (0x00007fffc8ad3000)
    libGalaxyParser.so => /home/ocaml/lib/libGalaxyParser.so (0x00007f1736e6d000)
    ...
```
### 1.3.3 load 动态库过程

基本的说就是符号重定位，然后合并到全局符号表。

##1.4 gcc/g++链接时对库的顺序要求

先看看gcc手册对-L和-l的描述

- -Ldir
```sh
    Add directory dir to the list of directories to be searched for -l. 
```
- -llibrary
- -l library
```sh
    Search the library named library when linking. (The second
    alternative with the library as a separate argument is only for POSIX
    compliance and is not recommended.)

    It makes a difference where in the command you write this option;
    the linker searches and processes libraries and object files in
    the order they are specified. Thus, `foo.o -lz bar.o' searches
    library `z' after file foo.o but before bar.o. If bar.o refers to
    functions in `z', those functions may not be loaded.

    The linker searches a standard list of directories for the
    library, which is actually a file named liblibrary.a. The linker
    then uses this file as if it had been specified precisely by name.

    The directories searched include several standard system
    directories plus any that you specify with -L.

    Normally the files found this way are library files—archive files
    whose members are object files. The linker handles an archive file
    by scanning through it for members which define symbols that have
    so far been referenced but not defined. But if the file that is
    found is an ordinary object file, it is linked in the usual
    fashion. The only difference between using an -l option and
    specifying a file name is that -l surrounds library with `lib' and
    `.a' and searches several directories.    
```

基本的意思就是从左向右查找，如果是链接成动态库会稍微不同一点。

###1.4.1 对于library的查找

查找需要连接的符号名是从前向后找，根据-L指定的路径顺序查找；不同 目录下的同名的库，只取第一个`（从左向右）`，后面同名库被忽略；

###1.4.2 对于符号的查找

从左向右查找，如果是主程序块和静态库，不能定位地址就报错： ‘undefined reference to: xxx’如果是链接成动态库，则假设该符号在load 的时候地址重定位。如果找不到对应的动态库，则会在load的时候报：“undefined symbol: xxx“这样的错误。

##1.5 –as-needed对链接动态库的影响

先看看与动态库链接相关的几个选项的man说明：

```sh
--as-needed
--no-as-needed
    This option affects ELF DT_NEEDED tags for dynamic libraries mentioned on the command line after the --as-needed
    option.  Normally the linker will add a DT_NEEDED tag for each dynamic library mentioned on the command line,
    regardless of whether the library is actually needed or not.  --as-needed causes a DT_NEEDED tag to only be emitted for
    a library that satisfies an undefined symbol reference from a regular object file or, if the library is not found in
    the DT_NEEDED lists of other libraries linked up to that point, an undefined symbol reference from another dynamic
    library.  --no-as-needed restores the default behaviour.

--add-needed
--no-add-needed
    These two options have been deprecated because of the similarity of their names to the --as-needed and --no-as-needed
    options.  They have been replaced by --copy-dt-needed-entries and --no-copy-dt-needed-entries.

--copy-dt-needed-entries
--no-copy-dt-needed-entries
    This option affects the treatment of dynamic libraries referred to by DT_NEEDED tags inside ELF dynamic libraries
    mentioned on the command line.  Normally the linker won't add a DT_NEEDED tag to the output binary for each library
    mentioned in a DT_NEEDED tag in an input dynamic library.  With --copy-dt-needed-entries specified on the command line
    however any dynamic libraries that follow it will have their DT_NEEDED entries added.  The default behaviour can be
    restored with --no-copy-dt-needed-entries.

    This option also has an effect on the resolution of symbols in dynamic libraries.  With --copy-dt-needed-entries
    dynamic libraries mentioned on the command line will be recursively searched, following their DT_NEEDED tags to other
    libraries, in order to resolve symbols required by the output binary.  With the default setting however the searching
    of dynamic libraries that follow it will stop with the dynamic library itself.  No DT_NEEDED links will be traversed to
    resolve symbols.
```

再看看ld的帮助

```sh
--add-needed                Set DT_NEEDED tags for DT_NEEDED entries in   following dynamic libs
--no-add-needed             Do not set DT_NEEDED tags for DT_NEEDED entries   in following dynamic libs


--as-needed                 Only set DT_NEEDED for following dynamic libs if used
--no-as-needed              Always set DT_NEEDED for following dynamic libs
```

关键的看–as-needed，意思是说：只给用到的动态库设置DT_NEEDED。比如：

```sh
g++ -shared  PyGalaxy.o -lGalaxyParser -lxxx  -lrt  -o PyGalaxy.so
```

像这样链接一个PyGalaxy.so的时候，假设PyGalaxy.so里面用到了libGalaxyParser.so但是没 有用到libxxx.so。查看依赖关系如下：

```sh
ocaml@ocaml:~$ readelf -d PyGalaxy.so 
```
```sh
Dynamic section at offset 0x7dd8 contains 26 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libGalaxyParser.so]
 0x0000000000000001 (NEEDED)             Shared library: [libxxx.so]
 ...
 ```
 
当开启–as-needed的时候，像

```sh
g++ -shared  -Wl,--as-needed PyGalaxy.o -lGalaxyParser -lxxx  -lrt  -o PyGalaxy.so
```

这样链接PyGalaxy.so的时候，查看依赖关系如下：

```sh
ocaml@ocaml:~$ readelf -d PyGalaxy.so 

Dynamic section at offset 0x7dd8 contains 26 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libGalaxyParser.so]
 ...
 ```
 
–as-needed就是忽略链接时没有用到的动态库，只将用到的动态库set NEEDED。

开启–as-needed的一些常见的问题：

###1.5.1 链接主程序模块或者是静态库的时的‘undefined reference to: xxx’

```sh
g++ -Wl,--as-needed -lGalaxyRT -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt -o mutex mutex.o
```

假设mutex依赖libGalaxyRT.so中的东西。想想，因为gcc对库的顺序要求 和–as-needed（因为libGalaxyRT.so在mutex.o的左边，所以gcc认为没有 用到它，–as-needed将其忽略），ld忽略libGalaxyRT.so，定位mutex.o的 符号的时候当然会找不到符号的定义！所以‘undefined reference to’这个 错误是正常地！

正确的链接方式是：

```sh
g++ -Wl,--as-needed mutex.o -lGalaxyRT -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt -o mutex
```

###1.5.2 编译动态库（shared library）的时候会导致一个比较隐晦的错误

编译出来的动态库的时候没有问题，但是加载的时候有“undefined symbol: xxx”这样的错误。假如像这也链接PyGalaxy.so

g++ -shared  -Wl,--as-needed -lGalaxyParser -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt  -o PyGalaxy.so PyGalaxy.o
load PyGalaxy.so的时候会有上面的运行时错误!

简单分析原因：因为libGalaxyParser.so在mutex.o的左边，所以gcc认为没 有用到它，–as-needed将其忽略。但是前面说的动态库符号解析的特点导 致ld认为某些符号是加载的时候才去地址重定位的。但是 libGalaxyParser.so已经被忽略了。所以就算你写上了依赖的库，load的时 候也会找不到符号。但是为什么没有-Wl–as-needed的时候是正确的呢？没 有的话，ld会set NEEDED libGalaxyParser.so（用前面提到的查看动态库 依赖关系的办法可以验证）。load的时候还是可以找到符号的，所以正确。

正确的链接方式是：

```sh
g++ -shared  -Wl,--as-needed PyGalaxy.o -lGalaxyParser -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt  -o PyGalaxy.so
```

##1.6 对链接顺序导致问题的解决方案

 
###1.6.1 在项目开发过层中尽量让lib是垂直关系，避免循环依赖；越是底层的库，越是往后面写！

例如:

```sh
g++ ...  obj($?) -l(上层逻辑lib) -l(中间封装lib) -l(基础lib) -l(系统lib)  -o $@
```

这样写可以避免很多问题，这个是在搭建项目的构建环境的过程中需要考虑 清楚地，在编译和链接上浪费太多的生命不值得！

###1.6.2 通过-(和-)强制repeat

-(和-),它能够强制"The specified archives are searched repeatedly", 这就是我们要找的啦。比如：

```sh
g++ -shared  -Wl,--as-needed PyGalaxy.o Xlinker "-("-lGalaxyParser -lxxx  -lrt"-)"  -o PyGalaxy.so
```

简单解释一下，Xlinker是将后面的一个参数传给ld（这里就是 "-("-lGalaxyParser -lxxx -lrt"-)"），然后-(和-)强制repeat当然就 可以找到了。但是这样的repeat需要浪费一些时间。