# CMake 入門/加入編譯選項

## 顯示更多建置訊息

在預設的情況下，CMake 生成的 makefile 只會顯示編譯的進度，並不會把各步驟實際調用的命令、參數一一列出，但在很多時候我們需要確知編譯時倒底使用了哪些編譯選項。其中一個方法是直接在 CMakeList.txt 當中加入這一行

```c
set(CMAKE_VERBOSE_MAKEFILE ON)
```

不過若是我們不希望更動 CMakeList.txt，可以等到執行時再加入選項

```c
$ cmake -DCMAKE_VERBOSE_MAKEFILE=ON ...options...
$ make

或

$ cmake ...options...
$ make VERBOSE=1
```
## 預設的編譯組態


透過指定 CMAKE_BUILD_TYPE 變數即可改變編譯組態，預設的 CMAKE_BUILD_TYPE 是
None，即不加上任何額外編譯選項。內建的 CMAKE_BUILD_TYPE 支援以下幾種組態：
- None : 編譯器預設值
- Debug ： 產生除錯資訊
- Release ： 進行執行速度最佳化
- RelWithDebInfo ： 進行執行速度最佳化，但仍然會啟用 debug flag
- MinSizeRel ： 進行程式碼最小化

雖然 CMAKE_BUILD_TYPE 可以由 CMakeList.txt 設定，一般來說我們更偏好到了執行 CMake 時才由命令列指定。
以前一章的例子來說明：
- ex2/
    - build/
    - src/
        - CMakeLists.txt
        - calc.c
        - calc.h
        - main.c




假設我們現在工作目錄是 ex2/build，我們可以執行以下的指令產生 Release 組態的執行檔


```sh
$ mkdir release
$ cd release
$ cmake -DCMAKE_BUILD_TYPE=Release ../../src
```

## 控制編譯選項的變數和指令
### 加入編譯選項

變數 CMAKE_C_FLAGS 存放的內容會被傳給 C 編譯器，作用在所有的編譯組態上。如果希望只針對特定一種組態有效，可以設定 CMAKE_C_FLAGS_<編譯組態>，例如 CMAKE_C_FLAGS_RELEASE、CMAKE_C_FLAGS_DEBUG。
舉個具體的例子，在 Visual C++ 底下編譯含有 sprintf 的原始碼時，常常收到 C4996 警告，除了可以在原始碼中使用 #pragma 關閉，也可以透過編譯選項搞定。在 CMakeLists.txt 設定編譯選項的方式如下：

```sh
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996")
endif()
```
在加入編譯選項時，記得要先判斷編譯器的種類，因為每一種編譯器的參數格式都不一樣。在這裡 MSVC 是一個 BOOL 值，當指定 Generator 為 Visual C++ 時 MSVC 的值為 TRUE，否則為 FALSE。
針對 C++ 編譯器也有對應的變數 CMAKE_CXX_FLAGS 和 CMAKE_CXX_FLAGS_<編譯組態>。

### 加入連結選項
CMAKE_EXE_LINKER_FLAGS 變數決定了連結執行檔時傳給編譯器的選項，同樣也有各種不同編譯組態的變形。
舉個具體的例子，新版的 MinGW (GCC 4.5)預設動態連結基礎程式庫，然而在編譯時卻會因為沒有啟用 auto-import，使得編譯器發出警告，有時連結出來的執行檔有誤。其中一種解決方法是在編譯時加入「-Wl,--enable-auto- import」，另外我們也可以採用靜態連結：

```sh
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")
endif()
```
上面使得 MinGW 各種組態都採

### 加入 Preprocessor 定義

指令 add_definitions 可以用來加入 Preprocessor 定義，作用範圍為所在資料夾下的編譯單元。格式如下

```sh
add_definitions("-DFOO -DBAR")
```

這個指令原本只是單純用來加入 Preprocessor 定義，但事實上差不多所有編譯選項都可以透過 add_definitions 傳給編譯器。

### 加入編譯器搜尋路徑


以下兩個指令分別用來加入編譯器尋找標頭檔、程式庫的路徑，在 gcc 相當於 -I 和 -L 選項。


```sh
include_directories([AFTER|BEFORE] [SYSTEM] dir1 dir2 ...)
link_directories(dir1 dir2 ...)
```

include_directories(dir) 預設會將 dir 加入目前引入路徑列表的後面，我們可以利用 AFTER 和 BEFORE 調整 dir 應該被加到列表的前端還是後端


## 使用屬性控制編譯選項

以上介紹的變數和指令都是屬於全域範圍的設定，如果希望選項僅對一個 target 有效，就必須從 target 的屬性(properties)下手。
在 CMake 裡一個 target 最終的輸出檔名、編譯連結選項等等都是由屬性控制，若沒有手工設定這些屬性，則 CMake 會由相關的全域變數來決定屬性值。我們可以利用 set_target_properties() 指令單獨指定一個 target 的屬性，如此一來就可以蓋過全域變數的設定。
set_target_properties() 指令格式如下


```sh
set_target_properties(target1 target2 ...
    PROPERTIES
    屬性名稱1  值
    屬性名稱2  值
    ...
    )
```
控制編譯選項的屬性是 COMPILE_FLAGS，連結則由 LINK_FLAGS 和 LINK_FLAGS_<編譯組態> 屬性控制。另外我們也可以透過 COMPILE_DEFINITIONS 和 COMPILE_DEFINITIONS_<編譯組態> 加入 preprocessor 定義，如同 add_definitions() 指令一樣，這兩個屬性實際上也可以用來加入幾乎所有的編譯選項。
舉例來說

```sh
set_target_properties(app
    PROPERTIES
    LINK_FLAGS          -static
    LINK_FLAGS_RELEASE  -s
    )
```

會使 app 這個程式在所有情況下都採用 -static 選項，並且在 release build 時採用 -static -s 選項。這些屬性會蓋過原先全域變數的設定，而且僅作用在 app 這個 target 上。

## 在編譯期指定環境變數

除了透過變數和屬性之外，還可以等到編譯期才給定環境變數，如

```sh
$ export CXXFLAGS=-O2
$ cmake .
$ make
```

## 注意事項
### 小心覆寫變數
像 CMAKE_C_FLAGS_RELEASE、CMAKE_C_FLAGS_DEBUG 這些變數，CMake 會自動依照 Generator 指定的編譯器初始化。例如在 gcc 下 CMAKE_C_FLAGS_RELEASE 預設的值為 -O3 -DNDEBUG，而 CMAKE_C_FLAGS_DEBUG 預設值為 -g。
小心下面這種寫法：

```sh
set(CMAKE_C_FLAGS_RELEASE "-fno-inline")
```

原意可能是希望在做最佳化時忽略 inline 關鍵字，但是這樣一來其實會把原本的 -O3 -DNDEBUG 給覆寫掉，所以程式並不會做任何最佳化。
比較好的寫法應該是：
```sh
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-inline")
```

或者
```sh
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG -fno-inline")
```

##注意字串和串列的不同


```sh
set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")   # ... (1)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-s")   # ... (2)
```

- (1) 會傳遞 -static -s 給編譯器 (2) 會傳遞 -static;-s 給編譯器










## Reference

https://zh.wikibooks.org/zh-tw/CMake_%E5%85%A5%E9%96%80/%E5%8A%A0%E5%85%A5%E7%B7%A8%E8%AD%AF%E9%81%B8%E9%A0%85