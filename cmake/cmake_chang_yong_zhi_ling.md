# CMake 常用指令


文檔：http://www.cmake.org/Wiki/CMake

簡單好用的構建工具，主頁：http://www.cmake.org

步驟如下：

##第一步，編寫 CMakeLists.txt 文件


```sh
PROJECT(main)
SET(SRC_LIST main.cpp)
SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Werror -Weverything -Wno-deprecated-declarations -Wno-disabled-macro-expansion -Wno-float-equal -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -Wno-padded -Wno-old-style-cast")
SET(CMAKE_EXE_LINKER_FLAGS "-lc++ -lc++abi")
SET(CMAKE_BUILD_TYPE Debug)
ADD_EXECUTABLE(main ${SRC_LIST})
```

其中，PROJECT 指定工程名、 SET 是 cmake 變量賦值命令、 ADD_EXECUTABLE 指定生成可執行程序的名字。 括號內的大寫字符串是 cmake 內部預定義變量，這是 CMakeLists.txt 腳本的重點，下面詳細講述：


- SRC_LIST 指定參與編譯的源碼文件列表，如果有多個文件請用空格隔開， 如，你工程有 main.cpp, lib/MyClass.cpp, lib/MyClass.h 三個文件， 那麼可以指定為 SET(SRC_LIST main.cpp lib/MyClass.cpp)
- CMAKE_CXX_COMPILER 指定選用何種編譯器
- CMAKE_CXX_FLAGS 設定編譯選項
- CMAKE_EXE_LINKER_FLAGS 設定鏈接選項
- 一定要將 -lc++ 和 -lc++abi 獨立設定到 CMAKE_EXE_LINKER_FLAGS 變量中而不能放在 CMAKE_CXX_FLAGS, 否則無法通過鏈接
- CMAKE_BUILD_TYPE 設定生成的可執行程序中是否包含調試信息


另外，對於編譯選項，我的原則是嚴己寬人。 也就是說，在我本機上使用最嚴格的編譯選項以發現儘量多 bug, 發佈給其他人的源碼包使用最寬鬆的編譯選項以減少環境差異導致編譯失敗的可能。 前面羅列出來的就是嚴格版的 CMakeLists.txt, 寬鬆版我會考慮： 編譯器改用 GCC（很多人沒裝 clang），忽略所有編譯警告，讓編譯器進行代碼優化，去掉調試信息，添加安裝路徑等要素，具體如下：


```sh
PROJECT(main)
SET(SRC_LIST main.cpp)
SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")
SET(CMAKE_BUILD_TYPE Release)
ADD_EXECUTABLE(porgram_name ${SRC_LIST})
INSTALL(PROGRAMS porgram_name DESTINATION /usr/bin/)
```


## 第二步，基於 CMakeLists.txt 生成 Makefile


在 CMakeLists.txt 所在目錄執行

```sh
cmake CMakeLists.txt
```

執行成功的話，你將在該目錄下看到 Makefile 文件

## 第三步，基於 Makefile 生成可執行程序

相同目錄下執行

```sh
make
```

這一步，就是在調用編譯器進行編譯，如果存在代碼問題，修正錯誤後重新執行這一步即可，不用再次執行第一、二步


基本上，你的新工程，可以在基於上面的 CMakeLists.txt 進行修改，執行一次第二步後，每次代碼調整隻需執行第三步即可

##使其更像一個工程

採用外部構建 (out-of-source), 構建目錄是工程目錄下的 build 自錄

- 添加子目錄 src, build

- 為 src 子目錄建立 CMakeLists.txt, 內容如下：

```sh
SET(CMAKE_CXX_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-long-long -Wno-variadic-macros -fexceptions -DNDEBUG -std=c99")
SET(CMAKE_BUILD_TYPE Debug)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
ADD_EXECUTABLE(gdt generate_dt.c)
ADD_EXECUTABLE(dt digital_triangle.c)
```

PROJECT_BINARY_DIR 變量指的編譯發生的當前目錄

- 修改根目錄中的 CMakeLists.txt 修改為如下內容：


```sh
PROJECT(dt)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ADD_SUBDIRECTORY(src bin)
```

ADD_SUBDIRECTORY(source_dir [binary_dir] [EXCLUDE_FROM_ALL])
這個指令用於向當前工程添加存放源文件的子目錄，並可以指定中間二進制和目標二進制存放的位置。 EXCLUDE_FROM_ALL 參數的含義是將這個目錄從編譯過程中排除

- 進入 build 目錄，執行如下命令

```sh
cmake ..
make
```

可執行文件就生成在 build/bin 目錄

##一些變量

- LINK_LIBRARIES 設置鏈接庫

##靜態與動態庫的構建

###簡單的靜態庫的構建

例如源文件的目錄結構如下：

```sh
slist
├── build
└── src
    ├── slist.c
    └── slist.h
```


首先新建文件 slist/CMakeLists.txt, 內容如下：

```sh
PROJECT(slist)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ADD_SUBDIRECTORY(src lib)
```

然後新建文件 slist/src/CMakeLists.txt, 內容如下：


```sh
SET(SRC_LIST slist.c)
SET(INSTALL_PREFIX /Users/mwum/code)
SET(CMAKE_CXX_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-std=c11")
SET(CMAKE_BUILD_TYPE Release)

ADD_LIBRARY(slist STATIC ${SRC_LIST})

INSTALL(TARGETS slist ARCHIVE DESTINATION ${INSTALL_PREFIX}/lib)
INSTALL(FILES slist.h DESTINATION ${INSTALL_PREFIX}/include)
```

其中的一些變量解釋如下：


- ADD_LIBRARY(libname [SHARED|STATIC|MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)
    -  SHARED 動態庫
    - STATIC 靜態庫
    - MODULE 在使用 dyld 的系統有效，如果不支持 dyld, 則被當作 SHARED 對待
    - EXCLUDE_FROM_ALL 這個庫不會被默認構建，除非有其他的組件依賴或者手工構建
    

安裝方法如下：

```sh
cd build
cmake ..
make
make install
```

執行完上面的命令之後，生成的靜態庫文件 libslist.a 被安裝到 /Users/mwum/code/lib 目錄， src/slist.h 文件被安裝到 /Users/mwum/code/include 目錄

或者將 slist/src/CMakeLists.txt 文件中關於 INSTALL_PREFIX 這個變量的信息刪除，如下：


```sh
SET(SRC_LIST slist.c)
SET(CMAKE_CXX_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-std=c11")
SET(CMAKE_BUILD_TYPE Release)

ADD_LIBRARY(slist STATIC ${SRC_LIST})

INSTALL(FILES slist.h DESTINATION include)
INSTALL(TARGETS slist ARCHIVE DESTINATION lib)
```

安裝的時候執行如下命令來手動指定要安裝到的目錄



```sh
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/Users/mwum/code
make
make install
```

如果要刪除被安裝的文件，可執行命令

```sh
cat install_manifest.txt | sudo xargs rm
```

install_manifest.txt 文件記錄了安裝的所有東西的路徑

## 同時構建靜態庫和動態庫

slist/CMakeLists.txt, 內容如下：

```sh
PROJECT(slist)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ADD_SUBDIRECTORY(src lib)
```

slist/src/CMakeLists.txt, 內容如下：


```sh
SET(SRC_LIST slist.c)
SET(INSTALL_PREFIX /Users/mwum/code)
SET(CMAKE_CXX_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-std=c11")
SET(CMAKE_BUILD_TYPE Release)

ADD_LIBRARY(slist_static STATIC ${SRC_LIST})
SET_TARGET_PROPERTIES(slist_static PROPERTIES OUTPUT_NAME "slist")

ADD_LIBRARY(slist_shared SHARED ${SRC_LIST})
SET_TARGET_PROPERTIES(slist_shared PROPERTIES OUTPUT_NAME "slist")
SET_TARGET_PROPERTIES(slist_shared PROPERTIES VERSION 1.0 SOVERSION 1)

INSTALL(FILES slist.h DESTINATION ${INSTALL_PREFIX}/include)
INSTALL(TARGETS slist_static ARCHIVE DESTINATION ${INSTALL_PREFIX}/lib)
INSTALL(TARGETS slist_shared LIBRARY DESTINATION ${INSTALL_PREFIX}/lib)
```

其中的一些變量解釋如下：

- 這條指令可以用來設置輸出的名稱，對於動態庫，還可以用來指定動態庫版本和 API 版本

```sh
SET_TARGET_PROPERTIES(target1 target2 ...
                      PROPERTIES prop1 value1
                      prop2 value2 ...)
                      
```

- VERSION 表示動態庫的版本，SOVERSION 表示 API 的版本

