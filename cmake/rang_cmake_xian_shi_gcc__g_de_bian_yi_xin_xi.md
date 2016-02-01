# 讓cmake顯示gcc/g++的編譯信息

最近學習了一下cmake，終於再也不用擔心不會寫Makefile了。趕緊來炫一下hello world！

   項目目錄結構
第一個項目，肯定是大家都懂的「hello world」了。那就先來看一下這個「龐大」的項目結構吧。

```c
[study@leoox hello]$ tree
.
|-- CMakeLists.txt
|-- build
`-- main.c
 
1 directory, 2 files
```

哈哈，其實就只有一個代碼文件 main.c。源碼內容嘛，程序員都知道。

- main.c

```c
#include <stdio.h>

int main(int argc, char** argv)
{
  printf("hello world\n");
  return 0;
}
```

至於CMakeLists.txt嘛，那當然就是今天的主角了。換句話說，以前的Makefile(makefile)已經被CMakeLists.txt取代了。稍後再重點介紹。

還有一個空文件夾build，幹嘛用的呢。它就相當於一個垃圾桶，我們實用cmake的外部構建的方式的時候，編譯過程中產生的一些中間文件，比如.o文件，cmake本身的中間文件等等都會放到build裡面。而我們的源碼目錄乾乾淨淨，是不是很適合有代碼潔癖的你呢。


##  擼起袖子寫cmake

按照以前，我就得去找其他項目的Makefile過來改，然後湊合的用了。可現在，我可以「驕傲」的說，我可以白手起家搞定CMakeLists.txt。一起見證奇蹟！


```c
project(hello)
cmake_minimum_required(VERSION 2.8)
add_executable(hello main.c)
```

哈哈，就是這麼簡單。就3行cmake語法。其實前面2行可以不寫，你只寫一行就可以了。既然寫了，就稍微解釋一下語法吧（太深入，我也不懂）。


## PROJECT(projectname [CXX] [C] [JAVA])

用於指定工程名字，[]為可選內容，默認表示支持所有語言。注意這條指令還隱式定義了另外兩個變量<projectName>_BINARY_DIR 和<projectName>_SOURCE_DIR。我們這裡的project定義為了hello，所以這兩個變量就是${hello_BINARY_DIR} 和 ${hello_SOURCE_DIR}。什麼意思呢，可以用message命令打印出來看看他的值。

${hello_BINARY_DIR} ：就是cmake要（構建）編譯我們的項目（main.c）的具體路徑。這裡當然就是build。

${hello_SOURCE_DIR} ：就是我們項目的源碼的具體路徑，這裡當然是項目的根目錄。

如果採用的是內部構建的方式，即直接在項目根目錄下運行cmake，那這兩個變量的值是一樣的。不過一般很少使用這兩個變量。大家都比較喜歡用 ${CMAKE_SOURCE_DIR} 和 ${CMAKE_BINARY_DIR}.


### cmake_minimum_required

這個就比較好理解了，就是這個項目要求的cmake版本不能低於2.8版本。當使用新版本的cmake才有的特性的時候，就需要加上這個了。

###    add_executable

顧名思義，就是要生成一個可執行程序hello唄。注意這裡的hello和project裡面的hello是沒有關係的。這裡的hello，是說最終編譯出來的二進制文件名字叫hello。你可以改成你喜歡的名字。那hello當然是通過main.c來編譯的了。如果有多個.c文件，一併在後面加入就行。但是如果你深入學習了cmake，其實是不用一個一個手工寫.c文件的。這裡只有一個main.c就無所謂了。

 

加上message打印cmake調試信息後的cmake如下：

```c
project(hello)
cmake_minimum_required(VERSION 2.8)
message(STATUS "src dir = ${hello_SOURCE_DIR}")
message(STATUS "binary dir = ${hello_BINARY_DIR}")
message(STATUS "leoox src dir = ${CMAKE_SOURCE_DIR}")
message(STATUS "leoox binary dir = ${CMAKE_BINARY_DIR}")
add_executable(hello main.c)
```

## 愉快的編譯項目吧
運行cmake的方式就是 ：

cmake CMakeLists.txt所在的路徑


我們採用外部構建的方式。到build文件夾裡面（cd build）

```c
[study@leoox build]$ cmake ..
-- The C compiler identification is GNU 4.1.2
-- The CXX compiler identification is GNU 4.1.2
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- src dir = /home/study/program/cmake/hello
-- binary dir = /home/study/program/cmake/hello/build
-- leoox src dir = /home/study/program/cmake/hello
-- leoox binary dir = /home/study/program/cmake/hello/build
-- Configuring done
-- Generating done
-- Build files have been written to: /home/study/program/cmake/hello/build
[study@Thrift build]$ ls
CMakeCache.txt  CMakeFiles  Makefile  cmake_install.cmake
[study@leoox build]$ ls ..
CMakeLists.txt  build  main.c
[study@leoox build]$
```
哈哈，太好了。cmake幫我生成了Makefile。而代碼外面還是非常乾淨的，就一個mian.c。通過message打印出來的信息看，果然驗證了上面的解釋。

那接下來就可以開心的make了

```c
[study@leooxbuild]$ make
Scanning dependencies of target hello
[100%] Building C object CMakeFiles/hello.dir/main.c.o
Linking C executable hello
[100%] Built target hello
[study@Thrift build]$ ls ..
CMakeLists.txt  build  main.c
[study@leoox build]$ ls
CMakeCache.txt  CMakeFiles  Makefile  cmake_install.cmake  hello
[study@leoox build]$ ./hello
hello world
```


### 讓make更透明些
make順利的執行成功了，也得到了我們期待的「hello world」。但是make的信息太簡單了，感覺空落落的。以前的Makefile可都是打印出gcc 等詳細編譯過程的哦。

不要擔心，make的時候加個參數就行了。

`make VERBOSE=1  // CMake-generated Makefiles only support VERBOSE=1, not V=1`

這樣你就可以在編譯信息裡面看到你想要的詳細的gcc/g++的編譯參數了。比如：

[100%] Building C object CMakeFiles/hello.dir/main.c.o
/usr/bin/cc -o CMakeFiles/hello.dir/main.c.o -c /home/study/program/cmake/hello/main.c
Linking C executable hello

 

什麼，要在make的時候寫參數。太不友好了。cmake知道你懶，所以也幫你搞定了。在CMakeLists.txt裡面加上這句指令就可以了。

set(CMAKE_VERBOSE_MAKEFILE ON)


```c
project(hello)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)
message(STATUS "src dir = ${hello_SOURCE_DIR}")
message(STATUS "binary dir = ${hello_BINARY_DIR}")
message(STATUS "leoox src dir = ${CMAKE_SOURCE_DIR}")
message(STATUS "leoox binary dir = ${CMAKE_BINARY_DIR}")
add_executable(hello main.c)

```
