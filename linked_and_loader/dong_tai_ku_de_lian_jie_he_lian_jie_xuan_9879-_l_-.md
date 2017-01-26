# 动态库的链接和链接选项-L，-rpath-link，-rpath


如何程序在连接时使用了共享库，就必须在运行的时候能够找到共享库的位置。linux的可执行程序在执行的时候默认是先搜索/lib和/usr/lib这两个目录，然后按照/etc/ld.so.conf里面的配置搜索绝对路径。同时，Linux也提供了环境变量LD_LIBRARY_PATH供用户选择使用，用户可以通过设定它来查找除默认路径之外的其他路径，如查找/work/lib路径,你可以在/etc/rc.d/rc.local或其他系统启动后即可执行到的脚本添加如下语句：LD_LIBRARY_PATH =/work/lib:$(LD_LIBRARY_PATH)。并且LD_LIBRARY_PATH路径优先于系统默认路径之前查找（详细参考《使用LD_LIBRARY_PATH》）。

不过LD_LIBRARY_PATH的设定作用是全局的，过多的使用可能会影响到其他应用程序的运行，所以多用在调试。（LD_LIBRARY_PATH的缺陷和使用准则，可以参考《Why LD_LIBRARY_PATH is bad》 ）。通常情况下推荐还是使用gcc的-R或-rpath选项来在编译时就指定库的查找路径，并且该库的路径信息保存在可执行文件中，运行时它会直接到该路径查找库，避免了使用LD_LIBRARY_PATH环境变量查找。

##链接选项和路径
现代连接器在处理动态库时将链接时路径（Link-time path）和运行时路径（Run-time path）分开,用户可以通过-L指定连接时库的路径，通过-R（或-rpath）指定程序运行时库的路径，大大提高了库应用的灵活性。比如我们做嵌入式移植时#arm-linux-gcc $(CFLAGS) –o target –L/work/lib/zlib/ -llibz-1.2.3 (work/lib/zlib下是交叉编译好的zlib库)，将target编译好后我们只要把zlib库拷贝到开发板的系统默认路径下即可。或者通过-rpath（或-R ）、LD_LIBRARY_PATH指定查找路径。

链接器ld的选项有 -L，-rpath 和 -rpath-link，看了下 man ld，大致是这个意思：

-L: “链接”的时候，去找的目录，也就是所有的 -lFOO 选项里的库，都会先从 -L 指定的目录去找，然后是默认的地方。编译时的-L选项并不影响环境变量LD_LIBRARY_PATH，-L只是指定了程序编译连接时库的路径，并不影响程序执行时库的路径，系统还是会到默认路径下查找该程序所需要的库，如果找不到，还是会报错，类似cannot open shared object file。

-rpath-link：这个也是用于“链接”的时候的，例如你显示指定的需要 FOO.so，但是 FOO.so 本身是需要 BAR.so 的，后者你并没有指定，而是 FOO.so 引用到它，这个时候，会先从 -rpath-link 给的路径里找。

-rpath: “运行”的时候，去找的目录。运行的时候，要找 .so 文件，会从这个选项里指定的地方去找。对于交叉编译，交叉编译链接器需已经配置 –with-sysroot 选项才能起作用。也就是说，-rpath指定的路径会被记录在生成的可执行程序中，用于运行时查找需要加载的动态库。-rpath-link 则只用于链接时查找。

##链接搜索顺序
直接man ld。The linker uses the following search paths to locate required shared libraries:


```sh
1.  Any directories specified by -rpath-link options.
2.  Any directories specified by -rpath options.  The difference between -rpath and -rpath-link is that directories specified by -rpath options are included in the executable and used at runtime, whereas the -rpath-link option is only effective at link time. Searching -rpath in this way is only supported by native linkers and cross linkers which have been configured with the --with-sysroot option.
3.  On an ELF system, for native linkers, if the -rpath and -rpath-link options were not used, search the contents of the environment variable "LD_RUN_PATH".
4.  On SunOS, if the -rpath option was not used, search any directories specified using -L options.
5.  For a native linker, the search the contents of the environment variable "LD_LIBRARY_PATH".
6.  For a native ELF linker, the directories in "DT_RUNPATH" or "DT_RPATH" of a shared library are searched for shared libraries needed by it. The "DT_RPATH" entries are ignored if "DT_RUNPATH" entries exist.
7.  The default directories, normally /lib and /usr/lib.
8.  For a native linker on an ELF system, if the file /etc/ld.so.conf exists, the list of directories found in that file.
If the required shared library is not found, the linker will issue a warning and continue with the link.

```


##gcc和链接选项的使用
在gcc中使用ld链接选项时，需要在选项前面加上前缀-Wl(是字母l，不是1，我曾多次弄错），以区别不是编译器的选项。

if the linker is being invoked indirectly, via a compiler driver (e.g. gcc) then all the linker command line options should be prefixed by -Wl, (or whatever is appropriate for the particular compiler driver) like this:

```sh
gcc -Wl,--start-group foo.o bar.o -Wl,--end-group
```

##rpath-link rptah說明
 
跨平台編譯的時候時常會出現
rpath-link與rpath這兩個參數
 
簡單的來說:
rpath-link用途用來連結編譯的路徑與-L相同
rpath用途為編譯完成後執行環境的路徑指定
 
2012年9月13日星期四

## misc: options -rpath-link and -L
 
-L 用來指定搜尋路徑，根據這個路徑搜尋 -l 選項中的 library
 
假設:

```sh
1. main.c depends on liba.so
2. liba.so depends on libb.so
3. liba.so locates at /lib
4. libb.so locates at /usr/lib
```

則要編出 main.c 的 executable，正確的作法是:
 
gcc main.c -o main -L/lib -la -L/usr/lib -lb
 
PS: -L 所指定的目錄，不管順序與出現如何，會對所有的 -l library 生效，
也就是在上面的例子中雖然 -L/usr/lib 晚於 -la 出現，
但在找 liba.so 的時後是 /lib 與 /usr/lib 都會搜尋的。
 
今天問題是開發 main.cpp 時只用到了 liba.so 的 symbol，
我們不見得會知道 liba.so 的 dependency libb.so，
甚至或許 libb.so 還有其它的 dependency。
 
很自然的人的直覺只會這樣寫:  gcc main.c -o main -L/lib -la
 
link 階段需要 resolve 所有的 symbols (解決所有 dependency)
要把所有相關的 dependent libraries 全找出來又是不切實際的。
 
GNU ld 的設計中有個預設路徑會去自動搜尋在 link 階段所需要的 library，
在這個淺規則之下 gcc main.c -o main -L/lib -la 這個指令依舊能產生可執行檔，
原因是 /usr/lib 正好就是 GNU ld x86 版本的預設路徑之一 (另一個是 /lib)。
 
link liba.so 的時後會從 liba.so 的 dynamic section 中得知 liba.so depends on libb.so，
然後在 /usr/lib 中找到 libb.so，然後再 link 所有 libb.so。
 
 
Linux 系統上只要安裝 library，in default 一定會丟進 /usr/lib 下，
因此開發 x86 的程式只要有 library 加上版本有對，
不太會有 link 時找不到 library 的困擾。
 
embedded 開發裡就沒這麼爽，
因為在 cross compile 的時後 ld 的預設路徑裡 (<ld 執行檔路徑>/../lib 之類的)
通常只有基本的 libraries。
 
所以我們需要使用到 GNU ld 的 -rpath-link 這個 option，
這個 option 就是用來補強預設路徑。
 
假設:

```sh
1. main.c depends on liba.so
2. liba.so depends on libb.so
3. liba.so locates at /home/some_one/lib
4. libb.so locates at /home/some_one/lib
```

則 mips-linux-gcc main.cpp -o main -L/home/some_one/lib -la -rpath-link /home/some_one/lib
可以編出可執行檔。
 
-L/home/some_one/lib 用來對應 -la，找 liba.so 時用，
-rpath-link /home/some_one/lib 用來補充 mips-linux-gcc 預設路徑，
解決所有的 dependency 所用 (在這個 case 可以找到 libb.so)。

