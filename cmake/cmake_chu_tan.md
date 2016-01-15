# CMake 初探

## 目錄

* [概論](#ov)
* [CMakeLists.txt語法簡介](#cmx-syntax)
 	* [字串和list](#cmx-syntax-str)
  * [流程控制](#cmx-syntax-flow)
  * [巨集和函數](#cmx-syntax-macro)
  * [Quotation](#cmx-syntax-quo)
  * [專案產生檔案安裝](#cmx-syntax-install)
* [範例: 產生執行檔和函式庫](#ex)
  * [範例程式](#ex-prg)
	* [第一版：單一程式沒有函數庫](#ex-1)
  * [第二版：加入編譯函式庫](#ex-2)
  * [第三版：每個目錄單獨編譯](#ex-3)
  * [第四版：加入安裝產生的檔案描述](#ex-4)
* [總結](#concl)  
* [參考資料](#ref)

---
## 概論
CMake是1999年推出的開源自由軟體計劃，目的是提供不同平台之間共同的編譯環境。他的特點有：

* 支援不同平台。
* **可以**將Build和原本程式碼分開。不分開稱為in-place build，而分開的情況稱為out-place build。out-place build的附加功能就是同樣一包套件可以同時編譯成不同平台的binary並且分別放在不同的目錄中。
* 支援cache加快編譯速度。

CMake的執行流程簡單來說是

* 開發者使用CMake語法寫編譯描述，存到CMakeLists.txt。
* 使用者執行cmake，cmake首先會根據開發描述的規格產生該平台對應的編譯環境檔案如Makefile等。
* 使用者執行make或是平台上的編譯方法產生最後的結果。
* 使用者執行cmake install安裝軟體。

另外一點值得注意的是cmake本身沒有提供uninstall功能。

---
<a name="cmx-syntax"></a>
## CMakeLists.txt 語法簡介

CMake語法的格式為`命令字串(參數)`，而相關規範可以分為「list和字串」、「變數」、「流程控制」、「Quotation」。分別討論如下：
  
---
<a name="cmx-syntax-str"></a>
### list和字串
CMake的基本單位是字串，而多個字串可以透過空白或是`;`組合成字串list。

有興趣的可以直接剪貼下面程式存成CMakeLists.txt後下cmake .看看結果。

```CMake CMakeLists.txt
set(xxx a b c d)
message(${xxx})
set(xxx e;f;g;h)
message(${xxx})
```
 
---
<a name="cmx-syntax-var"></a>
### 變數
變數使用`set`命令設定，格式為`set(變數名稱　指定的值)`。使用`${變數名稱}`取值。

有興趣的可以直接剪貼下面程式存成CMakeLists.txt後下cmake .看看結果。

```CMake CMakeLists.txt
set(xxx a b c d)
message(${xxx})
set(xxx e;f;g;h)
message(${xxx})
```
另外[這邊](http://www.cmake.org/Wiki/CMake_Useful_Variables)也列出了CMake內建好用的變數。

---
<a name="cmx-syntax-flow"></a>
### 流程控制
流程控制又可以分成條件執行、迴圈、和巨集等情況討論：

---
#### 條件執行
直接看範例，這個範例單純從command line吃變數值，做字串比對。

```CMake CMakeLists.txt
if ( NOT DEFINED test)
    message("Use: cmake -Dtest:STRING=val to test")
elseif(${test} STREQUAL yes)
    message("if: ${test}")
elseif(${test} STREQUAL test1)
    message("else if: ${test}")
else()
    message("else: ${test}")
endif()
```
有幾點需要說明：

* `DEFINED`判斷是否該變數有被定義。
* command透過`-D變數名稱：變數型態=變數值`來設定CMakeList.txt內部的變數。
* 條件判斷方法順序可以參考[這邊](http://www.cmake.org/cmake/help/v2.8.7/cmake.html#command:if)。簡單翻譯一下：
	* 先處理：EXISTS, COMMAND, DEFINED
  * 再來是：EQUAL, LESS, GREATER, STRLESS, STRGREATER, STREQUAL, MATCHES
  * 接下來是：NOT
  * 最後才是：AND, OR

---
##### cache的補充
我們第一次 **不從command 帶參數** 執行結果: 
```
$ cmake  .
-- The C compiler identification is GNU
...
Use: cmake -Dtest:STRING=val to test
```

接下來我們從command **帶參數** 重新執行一次：
```
$ cmake  . -Dtest:STRING=yes
if: yes
```
然後 **不從command 帶參數** 再執行會發現test變數變成yes了
```
$ cmake  .
if: yes
```
看一下目前目錄會發現新的檔案`CMakeCache.txt`，找一下裏面的字串`test`會看到
```
$ grep test CMakeCache.txt 
...
test:STRING=yes
...

```
**結論就是CMake的確有cache，而且不小心cache會影響到執行的結果。**另外其實`set(..)`裏面也可以cache行為的相關參數，這邊就先跳過不談。

---
#### 迴圈
兩種為主，`foreach`和`while`，直接看範例。

* `foreach`
為何有`cmake_minimum_required(VERSION 2.8)`呢？因為不打執行`cmake .`會產生警告。有興趣的可以打`cmake --help-policy CMP0000`看說明。

```CMake CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

set(xxx e;f;g;h)

foreach(i ${xxx})
    message(${i})
endforeach()
```
執行結果如下：
```
$ cmake .
e
f
g
h
```

* while
一個0~9的迴圈，需要透過[math command](http://www.cmake.org/cmake/help/v2.8.7/cmake.html#command:math)做四則運算。

```CMake CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
set(i 0)
while(i LESS 10)
    message(${i})
    math(EXPR i "${i} + 1")
endwhile()
```

---
<a name="cmx-syntax-macro"></a>
#### 巨集和函數
這兩個差別是**在函數內產生的變數scope只存在函數內，而巨集是全域的。**直接看範例，範例中的巨集和函數都是在內部產生變數並且印出傳進來的參數。可以仔細看輸出結果的確函數內的變數呼叫完後就消失了。

```CMake CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
macro(mac_print msg)
    set(mac "macro")
    message("${mac}: ${msg}")
endmacro(mac_print)

function(func_print msg)
    set (func "func")
    message("${func}: ${msg}")
endfunction(func_print)

mac_print("test macro")
message("check var in macro: ${mac}")
func_print("test function")
message("check var in function: ${func}")
```

執行結果如下
```
$ cmake .
macro: test macro
check var in macro: macro
func: test function
check var in function: 
```

---
<a name="cmx-syntax-quo"></a>
### Quotation

* 字串可以用成對的`"`表示
* 支援C語言型態控制字元如`\n``\t`
* 可以使用跳脫字元顯示特殊意義符號如`\${var}`印出來就是`${VAR}`

--
<a name="cmx-syntax-install"></a>
### 專案產生檔案安裝
簡單的語法如下，詳細資料請參考[這邊](http://www.cmake.org/Wiki/CMake:Install_Commands)

* 執行檔安裝(一般安裝目錄路徑：bin)
	* `install(TARGETS 執行檔名稱 DESTINATION 安裝目錄路徑)`
* 函式庫安裝(一般安裝目錄路徑：lib)
	* `install(TARGETS 函式庫名稱 LIBRARY DESTINATION 安裝目錄路徑)`
* Header 檔安裝(一般安裝目錄路徑：include)
	* `install(FILES Header檔名稱 DESTINATION 安裝目錄路徑)`

這些安裝描述都是允許多個檔案。另外你可以在執行cmake帶`-DCMAKE_INSTALL_PREFIX=安裝目錄`指定安裝的top目錄，或是`make DESTDIR=安裝目錄`也有同樣效果。

---
<a name="ex"></a>
## 範例: 產生執行檔和函式庫
前面有提到**in-place**和**out-place**的編譯方式。他們方式的差別是：

* in-place: 直接在CMakeLists.txt那層下`cmake . && make`
* out-place: 直接在CMakeLists.txt那層下`mkdir build && cd build && cmake ../ && make`
	* build是慣用名稱，不需要強制使用。
  
---
<a name="ex-prg"></a>
### 範例程式 
[範例程式細節在這邊](http://wen00072-blog.logdown.com/posts/203304-dry-test-file-template)，檔案各別分配到`src`, `include`, `libs`這三個目錄。不想看code只要知道每個檔案都有參考到某個自訂的header file就好了。

* 測試環境：Ubuntu 12.04

- 原始測試程式樹狀架構

```
├── include
│   ├── liba.h
│   └── libb.h
├── libs
│   ├── liba.c
│   └── libb.c
└── src
    └── test.c

```


---
<a name="ex-1"></a>
### 第一版：單一程式沒有函數庫
先暖身一下，只要在project最上層放一個CMakeLists.txt就好了。

這版本CMakeLists.txt不難理解，就做

* 填寫project資訊描述。
* 設定project相關header file路徑。
* 指定編譯要顯示細節，這是個人偏好習慣。
* 設定共用編譯參數，CMake可以更進一步地指定release mocde或debug mode的參數，以及指定套用這些參數檔案。
* 指定要編譯哪些檔案。
* 指定要編譯成執行檔

top 目錄的CMakeLists.txt如下：

```sh
CMake top 目錄的CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
# Project data
project(testcmake)

# Directories
set(SRC_DIR src)
set(LIB_DIR libs)
set(INC_DIR include)

# Release mode
set(CMAKE_BUILD_TYPE Debug)

# Compile flags
set(CMAKE_C_FLAGS "-Wall -Werror")

# I like verbose, must after project, do not know why
set(CMAKE_VERBOSE_MAKEFILE true)

# Where to include?
include_directories(${INC_DIR})

# Files to compile
set(test_SRCS ${SRC_DIR}/test.c ${LIB_DIR}/liba.c ${LIB_DIR}/libb.c)
add_executable(${PROJECT_NAME} ${test_SRCS})
```

這邊可以看到和原本的差別只有多了CMakeLists.txt檔而已。

```
├── CMakeLists.txt
├── include
│   ├── liba.h
│   └── libb.h
├── libs
│   ├── liba.c
│   └── libb.c
└── src
    └── test.c
```


---
<a name="ex-2"></a>
### 第二版：加入編譯函式庫

要編譯函式庫，要在top目錄下的CMakeLists.txt做以下的修改

* `add_library(檔案名稱 函式庫名稱)`告訴CMake要搬把哪些檔案編譯函式庫
	* 改成`add_library(檔案名稱 SHARED 函式庫名稱)`就變成shared library了。
* `target_link_libraries(執行檔名稱 函式庫名稱)`
	* 告訴系統最後link要把函式庫一起link進來。

top 目錄的CMakeLists.txt如下：
```sh
CMake top 目錄的CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
# Project data
project(testcmake)

# Directories
set(SRC_DIR src)
set(LIB_DIR libs)
set(INC_DIR include)

# Release mode
set(CMAKE_BUILD_TYPE Debug)

# Compile flags
set(CMAKE_C_FLAGS "-Wall -Werror")

# I like verbose, must after project, do not know why
set(CMAKE_VERBOSE_MAKEFILE true)

# Where to include?
include_directories(${INC_DIR})

# Build libraries
set(liba_SRCS ${LIB_DIR}/liba.c)
set(libb_SRCS ${LIB_DIR}/libb.c)

add_library(a SHARED ${liba_SRCS})
add_library(b SHARED ${libb_SRCS})
    
# Build binary
set(test_SRCS ${SRC_DIR}/test.c)
add_executable(${PROJECT_NAME} ${test_SRCS})
target_link_libraries(${PROJECT_NAME} a b)
```

---
<a name="ex-3"></a>
### 第三版：每個目錄單獨編譯
要做的事情很簡單，就是

* 在`src`和`libs`下面加入CMakeLists.txt，描述編譯行為
* 把根目錄的對應編譯行為搬到子目錄的CMakeLists.txt
* 使用`add_subdirectory(子目錄名稱)`把要編譯的子目錄加進去

所以我們現在目錄樹狀結構會變成src和lib目錄都有CMakeLists.txt

```
├── CMakeLists.txt
├── include
│   ├── liba.h
│   └── libb.h
├── libs
│   ├── CMakeLists.txt
│   ├── liba.c
│   └── libb.c
├── readme.txt
└── src
    ├── CMakeLists.txt
    └── test.c

```

每個目錄的CMakeLists.txt列出如下

* CMakeLists.txt

```sh
CMake CMakeLists.txt
cmake_minimum_required(VERSION 2.8)
# Project data
project(testcmake)

# Directories
set(SRC_DIR src)
set(LIB_DIR libs)
set(INC_DIR include)

# Release mode
set(CMAKE_BUILD_TYPE Debug)

# Compile flags
set(CMAKE_C_FLAGS "-Wall -Werror")

# I like verbose, must after project, do not know why
set(CMAKE_VERBOSE_MAKEFILE true)

# Where to include?
include_directories(${INC_DIR})

# Build library in libs directory or not?
# Dive into libs directory
add_subdirectory(${SRC_DIR})
add_subdirectory(${LIB_DIR})
```

* libs/CMakeLists.txt

```sh
CMake libs/CMakeLists.txt
# Build binary, inherit setting from parent
set(liba_SRCS liba.c)
set(libb_SRCS libb.c)

add_library(a ${liba_SRCS})
add_library(b ${libb_SRCS})
```

* src/CMakeLists.txt

```sh
CMake src/CMakeLists.txt
# Build binary
set(test_SRCS test.c)
add_executable(${PROJECT_NAME} ${test_SRCS})
target_link_libraries(${PROJECT_NAME} a b)
```

---
## 第四版：加入安裝產生的檔案描述
這邊就是單純把前面的install()命令套用到每一個目錄下的CMakeLists.txt。由於我們也要安裝header檔，所以在include目錄下面會新增CMakeLists.txt描述安裝header的細節。

所以我們現在目錄樹狀結構會變成每個目錄都有CMakeLists.txt
```
├── CMakeLists.txt
├── include
│   ├── CMakeLists.txt
│   ├── liba.h
│   └── libb.h
├── libs
│   ├── CMakeLists.txt
│   ├── liba.c
│   └── libb.c
├── readme.txt
└── src
    ├── CMakeLists.txt
    └── test.c
```

而各CMakeLists.txt新增的描述為

* CMakeLists.txt

```sh
CMake CMakeLists.txt
add_subdirectory(${INC_DIR})
```

* libs/CMakeLists.txt

```sh
CMake libs/CMakeLists.txt
install(TARGETS a b LIBRARY DESTINATION lib)
```

* src/CMakeLists.txt

```sh
CMake src/CMakeLists.txt
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
```

* include/CMakeLists.txt

```sh
CMake include/CMakeLists.txt
install(FILES liba.h libb.h DESTINATION include)
```

---
#### 第四版執行結果
- 第四版執行結果

```sh
$ cmake  ../  -DCMAKE_INSTALL_PREFIX=`pwd`/test && make && make install
...
$ tree test
test/
├── bin
│   └── testcmake
├── include
│   ├── liba.h
│   └── libb.h
└── lib
    ├── liba.so
    └── libb.so
```


---
<a name="concl"></a>
## 結論
本篇文章簡單介紹了CMake的語法，以及示範用CMake產生執行檔和函式庫。但是CMake還有太多東西值得去注意，例如把字串代換到程式碼，config.h的建立，搜尋depend 套件等。這部份以後有緣份會用到再跟各位分享。

---
<a name="ref"></a>
## 參考資料

* [CMake 官方網站](http://www.cmake.org/)
* [CMake-tutorial 投影片，強力推荐](https://github.com/TheErk/CMake-tutorial)
* [CMake Syntax](http://www.cmake.org/cmake/help/syntax.html)
* [「貓也會的 CMake」簡報上線](http://blog.linux.org.tw/~jserv/archives/001991.html)
* [cmake 官方網站 wiki](http://www.cmake.org/Wiki/CMake)
* [CMake 官方網站:Tutorial](http://www.cmake.org/cmake/help/cmake_tutorial.html)
* [Wikipedia: CMake](http://en.wikipedia.org/wiki/CMake)
* [CMake Howto](http://www.cs.swarthmore.edu/~adanner/tips/cmake.php)
* [CMake 入門/運算子](http://zh.wikibooks.org/zh-tw/CMake_%E5%85%A5%E9%96%80/%E9%81%8B%E7%AE%97%E5%AD%90)
* [Stackoverflow: Passing the argument to CMAKE via command prompt](http://stackoverflow.com/questions/12896988/passing-the-argument-to-cmake-via-command-prompt)
* [CMake: Useful variables](http://www.cmake.org/Wiki/CMake_Useful_Variables)
* [CMake: Install_Commands](http://www.cmake.org/Wiki/CMake:Install_Commands)
