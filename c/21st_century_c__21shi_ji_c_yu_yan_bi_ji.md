# 21st Century C / 21世紀 C 語言 筆記

C 語言的演進, K&R (1978) -> ANSI C89 (1989) -> ISO C99 (1999) -> C11 (2011)

POSIX (Portable Operating System Interface of UNIX) 標準, 由 IEEE 制定類 Unix 的作業系統在實作上共通的基礎 (API 的關聯、Shell的運作、命令列工具的的執行相關規範, 跟本篇最大的關聯是符合 POSIX 標準的系統, 也包含了提供 C 語言的編譯器)。


##介紹內容
Basic Tool: GDB、Valgrind、gprof、make、pkg-config、doxygen 
Autotools: Autoconf、Automake、libtool 
Useful Lib: libcURL、libGlib、libGSL、libSQLite3、libXML2

###編譯階段
使用 pkg-config 顯示出使用該lib需要的 include、link 參數

```sh
pkg-config --libs [lib name]  
pkg-config --cflags [lib name]  
```

一般在編譯時, 使用額外 lib 在編譯時所需的下參數

```sh
gcc -I[header path] -L[library path] -l[library name]  
-I: 將路徑加入 include header 搜尋 path
-L: 將路徑加入 Library 搜尋 path
-l: 指定連結函式庫
-D: 等同於 define symbol (ex. -Dxxx)
```

可以搭配 pkg-config 使用 (但並非所有 lib 都有對 pkg-config 註冊)

```sh
gcc ｀pkg-config --cflags --libs [lib name]` -o [output name] [source file name]  
```

##執行期間的 Link (Share Library)
- 使用 Autotools, Libtool 會自動加入正確的旗標來搜尋使用的 lib (後面章節會介紹)
- 透過 export LD_LIBRARY_PATH 來指定執行時的搜尋路徑
- 在 makefile 中添加 LDADD=-L[lib path] -Wl, -R[lib path] (-Wl 用來將後續用'逗號'隔開的 list 轉換成'空白'的 list 後傳給 linker)


## 使用 Makefile 簡化編譯時冗長繁瑣的指令

```sh
P=program_name  
OBJECT=  
CFLAGS= -g -Wall -O3  
LDLIBS=  
CC=C99

$(P): $(OBJECT)
```

(Makefile 範例)

- (varname)表示變數,Shell使用(varname)表示變數,Shell使用Var 而 Makefile則要求必須以小括號括起來
- 以冒號 ":" 區隔表示相依的關係
- 以上的變數會在 make 的隱含規則中展開成執行的命令


### 除了上述範例中, 將變數值直接宣告在 Makefile 內, 額外將變數值傳給 Makefile 的方式有


- export 進環境變數
- 將變數指派命令放在執行命令之前 (只限當次執行有效) C 語言編譯的程式, 可在程式碼中透過 getenv 取得 1. 或 2. 指定的值
- make 也允許從命令列設定變數


```sh
CFLAGS="-g -Wall" make // 針對 make 及 子程序設定環境變數 (上述方法2)  
make CFLAGS="-g -Wall" // 設定 makefile 變數 (上述方法3)  
```
```sh
make -p > rule.txt  
```


可以查看 rule.txt 瞭解 make 的隱含規則


##Manual Page

Manual Page

- man 指令可以搜尋函式的使用手冊 (ex. man printf)
- man -k keyword 可以搜尋簡介
- manual 分許多 section, section 1 為命令列、3為函式庫名稱 (man 3 printf 會列出 C 函式庫的文件)
- vi 可將遊標移到 keyword 上, 按 K 查閱 manual pageman 指令可以搜尋函式的使用手冊 (ex. man printf)

## Debug / 除錯

GDB vs LLDB

```sh
set $ptr=&x[3] //expr int* $ptr = &x[3]  
p *($ptr++)  
```

之後再重複按 Enter 會執行前一動指令, 此技巧可走訪陣列, 甚至套用 link-list

##p $  
$ 可表示前一次輸出變數存放的值, 但這邊 LLDB 測試無效

在 .gdbinit 中可以 define macro 來整理複雜資料結構的輸出

##Profiling
Compile option 加上 -pg 會為 gprof 準備程式, 指令 gprof binary > profile 即可開始記錄, 結束後再打開 profile 看 log。 
mac 上的分析可以參考 StackOverflow 提到的 Instruments

##Valgrind
可以使用 Valgrind 偵測記憶體的使用錯誤(例子省略)

##Unit Test
使用 Glib 內建的 tool 來實作(省略), 求覆蓋率可透過在 Compile option 加上  - fprofile-arcs -ftest-coverage -O0, 在執行程式後下指令 gcov program.gcda 可顯示執行行數的百分比, 另外 program.c.cov 會顯示被測試到的次數

##Doxygen
使用 Doxygen 來產生文件, 比起在程式碼中添加的說明, 搭配 Graphviz 產生的各類圖表在 trace code 幫助更大。

##Autotool 章節
在打包程式的部分目前常見的還有 CMake 可以使用, autotool的部分等以後有時間再回過頭來看吧... Ker Ker

##Git 版本控管章節
Git 中文參考資料

##類似書中範例的參考
Wrapping C/C++ for Python

##變數的存取
Reference

- Life Time
    - static: 直到程式結束為止才會釋放
    - automatic: 離開 Scope (block) 的時候即會馬上釋放
    - dynamic: 要手動 free 才會釋放
- Scope
    - Local scope: 在 block 內可被看見
    - Class scope: Class 內的成員才可看見
    - Namespace scope: 同一個 namespace 才可看見
    - File scope: 相同檔案內才可看見
    - Global scope: 任何地方皆可看見, 除非被隱藏

## Struct vs Array
在記憶體的使用上, 傳遞 struct 時會整個被複製出去, 但 array 僅會傳遞別名 (指向相同的記憶體空間)。

另外, Array 目前已可提供執行期才決定大小 (透過 run time 得知的值來宣告 array), 並且不用手動 free。

##Point
```c
ptr[0] = *(ptr+0) // 這邊僅表示左右兩者相等, 非賦值  
```

(但是 void 的 point 是無法以陣列的方式取值, 因為缺少了型別無法計算位移量)。

###NaN
`N`ot-`A`-`N`umber, 使用 NaN 作為例外回傳值, 另外 NaN 具備的特性, 即使 NaN == NaN 也會回傳 false, 只能用 isnan() 來檢查。

NaN 的值可透過 0/0. 來得到 (0/0 會產生錯誤, 因為整數無法表示 NaN)。

##介紹容易犯錯的章節
###Macro
在巨集的外部需加上大括號, 好處是可限制展開內容的變數生存空間, 也可降低展開後造成問題的風險。
```c
gcc -E [source file name]  
```

加上`-E` option 之後只會執行前置處理器的部分, 並從標準輸出中印出結果, 可搜尋巨集擴展的部分來除錯。

##前置處理器中的小技巧

- 如果有兩個連續的字串出現, 箝制處理器會將兩個字串合併為一個
- sizeof() 是個 compile time 的 operator

##const

- int const : 常數整數
- int const * : 一個可變動的指標, 指向常數整數
- int * const : 一個常數指標, 指向可變動的整數
- int * const * : 一個指向常數指標的指標
- int const * * : 一個指標的指標, 指向常數整數
- int const * const * : 一個指向常數指標的指標, 常數指標指向常數整數

前後相鄰的 int 與 const 可以互換, 意義仍不變。

struct 內的變數, 透過將 pointer 傳出之後仍舊無法受到 const 的保護, 需特別注意。

```c
int *ptr;  
int const **constptr = &ptr  
int const const_var = 10;  
*constprt = &const_var // 等同 ptr = &const_var
*prt = 20 // 等同於改變了 const_var 的值
```

範例顯示透過指標操作 const 需要小心

## 好用工具介紹
- asprintf 來做文字轉換 (函式會自行 malloc, 但需手動 free)
- Stopif 巨集的使用可在函式 return 異常值時提供保護
- strdup 可以回傳複製字串後的內容
- strtok 以 strtok_r or strtok_s 取代 (thread safe)
- mmap 可以解決讀取檔案比記憶體大的問題
- libxml 提供處理 unicode 的函式庫
    - 語言相關處理的 keywords : Gettext 提供翻譯框架、xgettext 產生需要翻譯的字串索引
##特殊技巧

```c
define test(i, ...) for(int index=0; index<i; i++) {__VA__ARGS__}  
```

- 展開時, 在第二個參數之後 (從 ... 刪節號之後的參數都會被展開至 __VA__ARGS__ 的位置

```c
int iArray[] = {0, 1, 2, 3}; //賦值用  
function( (iArray[]){0,1,2,3} ) //傳遞用  
```

- 複合常量的寫法
- 透過前兩項技巧可以寫出向量化的 macro (展開成多個處理相同事情的 Snippets)

```c
struct stTest  
{
    int first;
    int second;
};
struct stTest temp = {.first=1};  
```

```c
int list[5] = {[3]=4, [4]=5};  
```

指定初始子, 未指定會賦值 0
```c
__attribute__((format(printf,m,n)))  
```

- 使用 __attribute__ 來控制變量輸入的問題, 參考資訊
(最後兩章的翻譯讀起來有點怪, 加上應用層面比較難參考, 就整理到這邊吧 : ) 收工~)
