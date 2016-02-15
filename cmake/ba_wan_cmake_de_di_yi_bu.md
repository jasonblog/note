# 把玩 CMake 的第一步


##實驗一：單一程式檔的 Hello World
先來個簡單的 Hello World 吧!! 準備了兩個檔案：一個 hello.c 與 CMakeLists.txt，這兩個檔放在同一個目錄。

- hello.c

```c
#include <stdio.h>
int main(){
  printf("Hello World!\n");
  return 0;
}
```

- CMakeLists.txt

```c
PROJECT (HELLO)
SET (HELLO_SRCS hello.c)
ADD_EXECUTABLE (hello ${HELLO_SRCS})
```

接下來就直接執行 cmake .，然後 cmake 就接著，產生了一些檔案出來（如下列表），包括最最重要的 Makefile。



```sh
drakes-computer:~/Code/cmake/hello Drake$ cmake .
-- Check for working C compiler: /usr/bin/gcc
-- Check for working C compiler: /usr/bin/gcc -- works
-- Check size of void*
-- Check size of void* - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/Drake/Code/cmake/hello
drakes-computer:~/Code/cmake/hello Drake$ ll
total 64
-rw-r--r--    1 Drake  Drake  10007 Mar 23 17:06 CMakeCache.txt
drwxr-xr-x   13 Drake  Drake    442 Mar 23 17:06 CMakeFiles
-rw-r--r--    1 Drake  Drake     78 Mar 18 23:47 CMakeLists.txt
-rw-r--r--    1 Drake  Drake   4125 Mar 23 17:06 Makefile
-rw-r--r--    1 Drake  Drake   1406 Mar 23 17:06 cmake_install.cmake
-rw-r--r--    1 Drake  Drake     74 Mar 18 23:36 hello.c
```

有了 Makefile，執行 make 後，就會產生一個叫 hello 的程式出來了。這好像沒什麼特別的，因為只有一個 hello.c，也沒用到什麼特別的函式庫之類的，自己直接呼叫 gcc 來 compile 也行，唯一的差別是，可以透過 cmake 幫我們生出一個 Makefile 而已。

##實驗二：把 Hello World 變成函式庫
緊接著，我想說來試試另一種情況（參考自官方範例）。把原來的 Hello World 改寫一下，寫一個 hello() （檔名就叫 hello_lib.c）的函式，然後產生一個靜態函式庫（static library）（在 Linux 下，副檔名是 .a），接著再寫一個主程式（檔名叫 hello_main.c），它會去 link 我們自己寫的函式庫，然後直接呼叫 hello()。

於是乎，我們需要 3+1 個檔案，分別是 hello_main.c, hello_lib.c 與 CMakeLists.txt，外加一個 hello_lib.h（給 hello_main.c include 用的）。又，我想在這邊試一下，讓函式庫的程式碼放在 lib 這個子目錄下，主程式放在 src 這個子目錄下，於是目錄結構與檔案內容分別如下所示：


```sh
drakes-computer:~/Code/cmake/hello_lib Drake$ tree 
.
|-- CMakeLists.txt
|-- lib
|   |-- CMakeLists.txt
|   |-- hello_lib.c
|   `-- hello_lib.h
`-- src
    |-- CMakeLists.txt
    `-- hello_main.c

2 directories, 6 files
```

- hello_lib.c

```c
#include <stdio.h>
int hello() {
  printf("Hello World!\n");
  return 0;
}
```

- hello_lib.h

```c
int hello();
```

- hello_main.c

```c
#include <hello_lib.h>
int main(){
  hello();
  return 0;
}
```

因為新增了兩個子目錄（lib, src），於是這邊利用到 cmake 的另一個功能，讓每個子目錄有各自的 CMakeLists.txt 去描述各自的狀況。

- ~/CMakeLists.txt

```sh
PROJECT (HELLO)
ADD_SUBDIRECTORY (lib)
ADD_SUBDIRECTORY (src)
```

- ~/src/CMakeLists.txt

```c
INCLUDE_DIRECTORIES (${HELLO_SOURCE_DIR}/lib)
LINK_DIRECTORIES (${HELLO_BINARY_DIR}/lib)
ADD_EXECUTABLE (hello hello_main.c)
TARGET_LINK_LIBRARIES (hello hello_lib)
```

- ~/lib/CMakeLists.txt

```c
ADD_LIBRARY (hello_lib hello_lib.c)
```

接著，我在根目錄下建了一個 build 的子目錄，然後在裏頭執行起了 cmake，試試 out-source building。

```sh
drakes-computer:~/Code/cmake/hello_lib/build Drake$ cmake ..
-- Check for working C compiler: /usr/bin/gcc
-- Check for working C compiler: /usr/bin/gcc -- works
-- Check size of void*
-- Check size of void* - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/Drake/Code/cmake/hello_lib/build
drakes-computer:~/Code/cmake/hello_lib/build Drake$ make
Scanning dependencies of target hello_lib
[ 50%] Building C object lib/CMakeFiles/hello_lib.dir/hello_lib.o
Linking C static library libhello_lib.a
[ 50%] Built target hello_lib
Scanning dependencies of target hello
[100%] Building C object src/CMakeFiles/hello.dir/hello_main.o
Linking C executable hello
[100%] Built target hello
drakes-computer:~/Code/cmake/hello_lib/build Drake$ src/hello 
Hello World!
```
經過這樣的實驗後，發覺 cmake 產生出來的 Makefile 會自動地幫我設定好 libhello_lib.a (from hello_lib.c & hello_lib.h）與 hello（from hello_main.c）的設定，雖然還不是很了解怎麼控制 hello 與 libhello_lib.a 的目錄位置，不過應該就是${HELLO_SOURCE_DIR} 或是 ${HELLO_BINARY_DIR} 這幾個變數決定的吧，這晚一點真的需要時，再去查一下就 ok 了（我猜啦@@）。

##實驗三：試試使用外部函式庫 libpng 的情況

接著來試試使用外部函式庫的情況好了，就拿 libpng 這個函數式，然後寫個簡單的測試程式好了。我上網 google 了一下，借了[ Guillaume Cottenceau](http://zarb.org/~gc/html/libpng.html) 寫的 libpng-short-example.c 這個例子來用。首先需要安裝好 libpng（與 zlib），在 Debian/Ubuntu 上頭，透過 apt-get install libpng 之類的就搞定了，而在 Mac 上的話，我是透過 MacPorts 的 port install libpng 來安裝的。在這邊要感謝 apt 與 ports 團隊的努力，讓我可以一下子就處理好這類可能會很棘手的問題。

現在問題來了，我要怎麼寫 CMakeLists.txt 檔呢? 至少要讓它產生的 Makefile 有把 libpng/zlib 的 header including path 與 library path 給加進來才行，如果還要自己去找出來的話…那我還要 cmake 幹麻 = = 在這邊，我做了一點點弊。

依 Jserv 投影片上的說法，cmake 使用 modules 的方式來得知要怎麼產生 Makefile（當然不止產生 Makefile 這件事，不過我覺得一開始單就這樣想也不妨），於是我去看了一下 cmake 預設的 modules 有哪些，發現到有個叫 FindPNG.cmake 的檔，太棒了，就是它了，一定可以從裏頭找到些什麼線索。（這也是為什麼我選擇拿 libpng 做實驗）

在這個檔的一開始看到如下的解釋：

```sh
# - Find the native PNG includes and library
#

# This module defines
#  PNG_INCLUDE_DIR, where to find png.h, etc.
#  PNG_LIBRARIES, the libraries to link against to use PNG.
#  PNG_DEFINITIONS - You should ADD_DEFINITONS(${PNG_DEFINITIONS}) before compiling code that includes png library files.
#  PNG_FOUND, If false, do not try to use PNG.
# also defined, but not for general use are
#  PNG_LIBRARY, where to find the PNG library.
# None of the above will be defined unles zlib can be found.
# PNG depends on Zlib

```

於是我很快地依著這說明，寫出個 CMakeLists.txt 檔了 ^>^

```sh
PROJECT (PNGSAMPLE)
FIND_PACKAGE (PNG REQUIRED)
ADD_DEFINITIONS (${PNG_DEFINITIONS})
#LINK_DIRECTORIES (${PNG_LIBRARY})
INCLUDE_DIRECTORIES (${PNG_INCLUDE_DIR})
LINK_LIBRARIES (${PNG_LIBRARIES})
ADD_EXECUTABLE (pngsample libpng-short-example.c)
```

實驗四：換試試寫個簡單的 Qt4 程式
程式碼取自良葛格的第一個 Qt4 程式。

```c
#include <QApplication>
#include <QLabel>
int main(int argc, char *argv[])
{
    QApplication app (argc, argv);
    QLabel label ("Hello World!");
    label.setWindowTitle ("Qt First!!");
    label.resize (200, 100);
    label.show ();
    return app.exec();
}
```

- CMakeLists.txt

```sh
project (qthelloworld)

cmake_minimum_required (VERSION 2.4.0)

find_package (Qt4 REQUIRED)
#add_definitions (${QT_DEFINITIONS})
#link_directories (${QT_LIBRARY_DIR})
#include_directories (${QT_INCLUDES})
include (${QT_USE_FILE})

add_executable (qthelloworld hello.cpp)
target_link_libraries (qthelloworld ${QT_LIBRARIES} ${QT_QTGUI_LIBRARY})
```
##實驗五：把玩一下 Jserv 提供的 cmake-samples
下次再寫好了…

##心得
感覺 cmake 的確是好用很多，比 automake 來得簡單太多了。


