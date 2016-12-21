# CMake


https://zh.wikibooks.org/wiki/Category:CMake_%E5%85%A5%E9%96%80


## 打印訊息
```
MESSAGE(STATUS "Project DirectoryTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT: ${PROJECT_SOURCE_DIR}")
```

## cmake 常用變數


CMAKE_SOURCE_DIR ( 相當於工程根目錄 ) 
this is the directory, from which cmake was started, i.e. the top level source directory

CMAKE_CURRENT_SOURCE_DIR 
this is the directory where the currently processed CMakeLists.txt is located in

PROJECT_SOURCE_DIR ( =CMAKE_SOURCE_DIR 相當於工程根目錄 ) 
contains the full path to the root of your project source directory, i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command

CMAKE_PREFIX_PATH （用於找 Findxxx.cmake文件，找 庫 和 頭文件） 
Path used for searching by FIND_XXX(), with appropriate suffixes added.

CMAKE_INSTALL_PREFIX ( 安裝目錄 ) 
Install directory used by install. 
If 「make install」 is invoked or INSTALL is built, this directory is prepended onto all install directories. This variable defaults to /usr/local on UNIX and c:/Program Files on Windows. 
例如： cmake .. -DCMAKE_INSTALL_PREFIX=/my/paht/to/install 
