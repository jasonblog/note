# gcc/g++鏈接時.o文件以及庫的順序問題


- 1.1 寫在前面
- 1.2 幾個名詞
- 1.3 技術鋪墊
    - 1.3.1 編譯動態庫時的符號解析
    - 1.3.2 linux下查看一個可執行文件或動態庫依賴哪些動態庫的辦法
    - 1.3.3 load 動態庫過程
- 1.4 gcc/g++鏈接時對庫的順序要求
    - 1.4.1 對於library的查找
    - 1.4.2 對於符號的查找
- 1.5 –as-needed對鏈接動態庫的影響
    - 1.5.1 鏈接主程序模塊或者是靜態庫的時的‘undefined reference to: xxx’
    - 1.5.2 編譯動態庫（shared library）的時候會導致一個比較隱晦的錯誤
- 1.6 對鏈接順序導致問題的解決方案
    - 1.6.1 在項目開發過層中儘量讓lib是垂直關係，避免循環依賴；越是底層的庫，越是往後面寫！
    - 1.6.2 通過-(和-)強制repeat


##1.1 寫在前面

最近換了xubuntu12.4，把原來的項目co出來編譯的時候報“undefined reference to”。猜測是gcc的版本問題，用-v跟蹤一下，發現gcc-4.6默認開 啟了ld的–as-needed選項。關閉該選項（–no-as-needed）後編譯正常。深 入挖掘發現還是一個比較有意思的問題。

##1.2 幾個名詞

- gcc： 後面不特殊說明gcc代表gcc/g++。
- 主程序塊： 只包含main的binary，可執行程序。

##1.3 技術鋪墊

 
###1.3.1 編譯動態庫時的符號解析

有符號解析不了的時候，gcc不會直接報錯而是假設load的時候地址重定位修正。

###1.3.2 linux下查看一個可執行文件或動態庫依賴哪些動態庫的辦法

你有一個library或者是可執行文件，你可以這樣查看他的依賴關係：

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
### 1.3.3 load 動態庫過程

基本的說就是符號重定位，然後合併到全局符號表。

##1.4 gcc/g++鏈接時對庫的順序要求

先看看gcc手冊對-L和-l的描述

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

基本的意思就是從左向右查找，如果是鏈接成動態庫會稍微不同一點。

###1.4.1 對於library的查找

查找需要連接的符號名是從前向後找，根據-L指定的路徑順序查找；不同 目錄下的同名的庫，只取第一個`（從左向右）`，後面同名庫被忽略；

###1.4.2 對於符號的查找

從左向右查找，如果是主程序塊和靜態庫，不能定位地址就報錯： ‘undefined reference to: xxx’如果是鏈接成動態庫，則假設該符號在load 的時候地址重定位。如果找不到對應的動態庫，則會在load的時候報：“undefined symbol: xxx“這樣的錯誤。

##1.5 –as-needed對鏈接動態庫的影響

先看看與動態庫鏈接相關的幾個選項的man說明：

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

再看看ld的幫助

```sh
--add-needed                Set DT_NEEDED tags for DT_NEEDED entries in   following dynamic libs
--no-add-needed             Do not set DT_NEEDED tags for DT_NEEDED entries   in following dynamic libs


--as-needed                 Only set DT_NEEDED for following dynamic libs if used
--no-as-needed              Always set DT_NEEDED for following dynamic libs
```

關鍵的看–as-needed，意思是說：只給用到的動態庫設置DT_NEEDED。比如：

```sh
g++ -shared  PyGalaxy.o -lGalaxyParser -lxxx  -lrt  -o PyGalaxy.so
```

像這樣鏈接一個PyGalaxy.so的時候，假設PyGalaxy.so裡面用到了libGalaxyParser.so但是沒 有用到libxxx.so。查看依賴關係如下：

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
 
當開啟–as-needed的時候，像

```sh
g++ -shared  -Wl,--as-needed PyGalaxy.o -lGalaxyParser -lxxx  -lrt  -o PyGalaxy.so
```

這樣鏈接PyGalaxy.so的時候，查看依賴關係如下：

```sh
ocaml@ocaml:~$ readelf -d PyGalaxy.so 

Dynamic section at offset 0x7dd8 contains 26 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libGalaxyParser.so]
 ...
 ```
 
–as-needed就是忽略鏈接時沒有用到的動態庫，只將用到的動態庫set NEEDED。

開啟–as-needed的一些常見的問題：

###1.5.1 鏈接主程序模塊或者是靜態庫的時的‘undefined reference to: xxx’

```sh
g++ -Wl,--as-needed -lGalaxyRT -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt -o mutex mutex.o
```

假設mutex依賴libGalaxyRT.so中的東西。想想，因為gcc對庫的順序要求 和–as-needed（因為libGalaxyRT.so在mutex.o的左邊，所以gcc認為沒有 用到它，–as-needed將其忽略），ld忽略libGalaxyRT.so，定位mutex.o的 符號的時候當然會找不到符號的定義！所以‘undefined reference to’這個 錯誤是正常地！

正確的鏈接方式是：

```sh
g++ -Wl,--as-needed mutex.o -lGalaxyRT -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt -o mutex
```

###1.5.2 編譯動態庫（shared library）的時候會導致一個比較隱晦的錯誤

編譯出來的動態庫的時候沒有問題，但是加載的時候有“undefined symbol: xxx”這樣的錯誤。假如像這也鏈接PyGalaxy.so

g++ -shared  -Wl,--as-needed -lGalaxyParser -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt  -o PyGalaxy.so PyGalaxy.o
load PyGalaxy.so的時候會有上面的運行時錯誤!

簡單分析原因：因為libGalaxyParser.so在mutex.o的左邊，所以gcc認為沒 有用到它，–as-needed將其忽略。但是前面說的動態庫符號解析的特點導 致ld認為某些符號是加載的時候才去地址重定位的。但是 libGalaxyParser.so已經被忽略了。所以就算你寫上了依賴的庫，load的時 候也會找不到符號。但是為什麼沒有-Wl–as-needed的時候是正確的呢？沒 有的話，ld會set NEEDED libGalaxyParser.so（用前面提到的查看動態庫 依賴關係的辦法可以驗證）。load的時候還是可以找到符號的，所以正確。

正確的鏈接方式是：

```sh
g++ -shared  -Wl,--as-needed PyGalaxy.o -lGalaxyParser -lc -lm -ldl -lpthread   -L/home/ocaml/lib/  -lrt  -o PyGalaxy.so
```

##1.6 對鏈接順序導致問題的解決方案

 
###1.6.1 在項目開發過層中儘量讓lib是垂直關係，避免循環依賴；越是底層的庫，越是往後面寫！

例如:

```sh
g++ ...  obj($?) -l(上層邏輯lib) -l(中間封裝lib) -l(基礎lib) -l(系統lib)  -o $@
```

這樣寫可以避免很多問題，這個是在搭建項目的構建環境的過程中需要考慮 清楚地，在編譯和鏈接上浪費太多的生命不值得！

###1.6.2 通過-(和-)強制repeat

-(和-),它能夠強制"The specified archives are searched repeatedly", 這就是我們要找的啦。比如：

```sh
g++ -shared  -Wl,--as-needed PyGalaxy.o Xlinker "-("-lGalaxyParser -lxxx  -lrt"-)"  -o PyGalaxy.so
```

簡單解釋一下，Xlinker是將後面的一個參數傳給ld（這裡就是 "-("-lGalaxyParser -lxxx -lrt"-)"），然後-(和-)強制repeat當然就 可以找到了。但是這樣的repeat需要浪費一些時間。