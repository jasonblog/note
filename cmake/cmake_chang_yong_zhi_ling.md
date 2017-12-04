# CMake 常用指令


文档：http://www.cmake.org/Wiki/CMake

简单好用的构建工具，主页：http://www.cmake.org

步骤如下：

##第一步，编写 CMakeLists.txt 文件


```sh
PROJECT(main)
SET(SRC_LIST main.cpp)
SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -Werror -Weverything -Wno-deprecated-declarations -Wno-disabled-macro-expansion -Wno-float-equal -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-prototypes -Wno-padded -Wno-old-style-cast")
SET(CMAKE_EXE_LINKER_FLAGS "-lc++ -lc++abi")
SET(CMAKE_BUILD_TYPE Debug)
ADD_EXECUTABLE(main ${SRC_LIST})
```

其中，PROJECT 指定工程名、 SET 是 cmake 变量赋值命令、 ADD_EXECUTABLE 指定生成可执行程序的名字。 括号内的大写字符串是 cmake 内部预定义变量，这是 CMakeLists.txt 脚本的重点，下面详细讲述：


- SRC_LIST 指定参与编译的源码文件列表，如果有多个文件请用空格隔开， 如，你工程有 main.cpp, lib/MyClass.cpp, lib/MyClass.h 三个文件， 那么可以指定为 SET(SRC_LIST main.cpp lib/MyClass.cpp)
- CMAKE_CXX_COMPILER 指定选用何种编译器
- CMAKE_CXX_FLAGS 设定编译选项
- CMAKE_EXE_LINKER_FLAGS 设定链接选项
- 一定要将 -lc++ 和 -lc++abi 独立设定到 CMAKE_EXE_LINKER_FLAGS 变量中而不能放在 CMAKE_CXX_FLAGS, 否则无法通过链接
- CMAKE_BUILD_TYPE 设定生成的可执行程序中是否包含调试信息


另外，对于编译选项，我的原则是严己宽人。 也就是说，在我本机上使用最严格的编译选项以发现尽量多 bug, 发布给其他人的源码包使用最宽松的编译选项以减少环境差异导致编译失败的可能。 前面罗列出来的就是严格版的 CMakeLists.txt, 宽松版我会考虑： 编译器改用 GCC（很多人没装 clang），忽略所有编译警告，让编译器进行代码优化，去掉调试信息，添加安装路径等要素，具体如下：


```sh
PROJECT(main)
SET(SRC_LIST main.cpp)
SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")
SET(CMAKE_BUILD_TYPE Release)
ADD_EXECUTABLE(porgram_name ${SRC_LIST})
INSTALL(PROGRAMS porgram_name DESTINATION /usr/bin/)
```


## 第二步，基于 CMakeLists.txt 生成 Makefile


在 CMakeLists.txt 所在目录执行

```sh
cmake CMakeLists.txt
```

执行成功的话，你将在该目录下看到 Makefile 文件

## 第三步，基于 Makefile 生成可执行程序

相同目录下执行

```sh
make
```

这一步，就是在调用编译器进行编译，如果存在代码问题，修正错误后重新执行这一步即可，不用再次执行第一、二步


基本上，你的新工程，可以在基于上面的 CMakeLists.txt 进行修改，执行一次第二步后，每次代码调整只需执行第三步即可

##使其更像一个工程

采用外部构建 (out-of-source), 构建目录是工程目录下的 build 自录

- 添加子目录 src, build

- 为 src 子目录建立 CMakeLists.txt, 内容如下：

```sh
SET(CMAKE_CXX_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-Wall -Werror -Wno-long-long -Wno-variadic-macros -fexceptions -DNDEBUG -std=c99")
SET(CMAKE_BUILD_TYPE Debug)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
ADD_EXECUTABLE(gdt generate_dt.c)
ADD_EXECUTABLE(dt digital_triangle.c)
```

PROJECT_BINARY_DIR 变量指的编译发生的当前目录

- 修改根目录中的 CMakeLists.txt 修改为如下内容：


```sh
PROJECT(dt)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ADD_SUBDIRECTORY(src bin)
```

ADD_SUBDIRECTORY(source_dir [binary_dir] [EXCLUDE_FROM_ALL])
这个指令用于向当前工程添加存放源文件的子目录，并可以指定中间二进制和目标二进制存放的位置。 EXCLUDE_FROM_ALL 参数的含义是将这个目录从编译过程中排除

- 进入 build 目录，执行如下命令

```sh
cmake ..
make
```

可执行文件就生成在 build/bin 目录

##一些变量

- LINK_LIBRARIES 设置链接库

##静态与动态库的构建

###简单的静态库的构建

例如源文件的目录结构如下：

```sh
slist
├── build
└── src
    ├── slist.c
    └── slist.h
```


首先新建文件 slist/CMakeLists.txt, 内容如下：

```sh
PROJECT(slist)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ADD_SUBDIRECTORY(src lib)
```

然后新建文件 slist/src/CMakeLists.txt, 内容如下：


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

其中的一些变量解释如下：


- ADD_LIBRARY(libname [SHARED|STATIC|MODULE] [EXCLUDE_FROM_ALL] source1 source2 ... sourceN)
    -  SHARED 动态库
    - STATIC 静态库
    - MODULE 在使用 dyld 的系统有效，如果不支持 dyld, 则被当作 SHARED 对待
    - EXCLUDE_FROM_ALL 这个库不会被默认构建，除非有其他的组件依赖或者手工构建
    

安装方法如下：

```sh
cd build
cmake ..
make
make install
```

执行完上面的命令之后，生成的静态库文件 libslist.a 被安装到 /Users/mwum/code/lib 目录， src/slist.h 文件被安装到 /Users/mwum/code/include 目录

或者将 slist/src/CMakeLists.txt 文件中关于 INSTALL_PREFIX 这个变量的信息删除，如下：


```sh
SET(SRC_LIST slist.c)
SET(CMAKE_CXX_COMPILER "clang")
SET(CMAKE_CXX_FLAGS "-std=c11")
SET(CMAKE_BUILD_TYPE Release)

ADD_LIBRARY(slist STATIC ${SRC_LIST})

INSTALL(FILES slist.h DESTINATION include)
INSTALL(TARGETS slist ARCHIVE DESTINATION lib)
```

安装的时候执行如下命令来手动指定要安装到的目录



```sh
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/Users/mwum/code
make
make install
```

如果要删除被安装的文件，可执行命令

```sh
cat install_manifest.txt | sudo xargs rm
```

install_manifest.txt 文件记录了安装的所有东西的路径

## 同时构建静态库和动态库

slist/CMakeLists.txt, 内容如下：

```sh
PROJECT(slist)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
ADD_SUBDIRECTORY(src lib)
```

slist/src/CMakeLists.txt, 内容如下：


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

其中的一些变量解释如下：

- 这条指令可以用来设置输出的名称，对于动态库，还可以用来指定动态库版本和 API 版本

```sh
SET_TARGET_PROPERTIES(target1 target2 ...
                      PROPERTIES prop1 value1
                      prop2 value2 ...)
                      
```

- VERSION 表示动态库的版本，SOVERSION 表示 API 的版本

