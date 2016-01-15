# 如何使用pkg-config工具編譯鏈接第三方庫

我們在使用第三方庫（如OpenCV,Boost等）時，難免會遇到相關的庫的編譯連接問題，而每次重複繁雜的配置的確讓人懊惱，而pkg-config1工具提供了一個相對簡單的庫的編譯鏈接方法。這裡就簡單介紹一下pkg-config的使用方法。

我們知道，第三方庫的使用主要涉及頭文件的路徑設置，庫的路徑設置以及動態庫的環境變量設置。當然有的庫的使用是僅僅需要頭文件，例如Eigen以及大部分Boost，這樣就不需要設置庫路徑和動態庫的環境變量。pkg-config通過讀取一個*.pc的文件，獲取了庫的頭文件位置和庫的路徑等信息，然後告知編譯器，實現庫的自動使用。一般來說，*.pc文件的大體內容如下格式：

```c
prefix=/usr/local
exec_prefix=${prefix}
includedir=${prefix}/include
libdir=${exec_prefix}/lib

Name: foo
Description: The foo library
Version: 1.0.0
Cflags: -I${includedir}/foo
Libs: -L${libdir} -lfoo
```

其中，

- prefix一般是指定庫的默認安裝路徑
- exec_prefix一般是指庫的另外指定的安裝路徑
- inludedir指定庫的頭文件路徑
- libdir指定庫的lib文件的路徑
- Name指定庫的名稱，比如OpenCV，Boost等
- Description表示庫的描述
- Version是版本號
- Cflags是gcc鏈接頭文件的指令,以-I緊接頭文件路徑設置
- Libs是gcc鏈接lib文件的指令, 是-L緊接lib文件路徑，-l緊接所使用的lib的名字。除此之外還有另外一種設置方法就是直接寫成所引用的庫文件路徑加庫的名稱，即Libs: ${libdir}/foo.a,OpenCV提供的*.pc文件就是這樣寫的。


當然並不是每個庫都會提供寫好的*.pc文件，OpenCV的linux版就提供了opencv.pc文件，而Boost就沒有提供，這裡就需要自己寫一個，下面貼出一個我寫好的boost.pc文件作為參考。boost庫大部分庫是不需要編譯的，僅需頭文件的，而我最近正好用到需要編譯的庫文件filesystem和system，所以這裡的示例一個單獨使用這兩個庫文件的方法。

```c
prefix=/usr/local
exec_prefix=${prefix}/boost-1.55.0
libdir=${exec_prefix}/lib
includedir=${exec_prefix}/include

Name: Boost
Description: A C++ Libraries
Version: 1.55.0
Cflags: -I${includedir}
Libs:  -L${libdir}/ -lboost_filesystem -lboost_system
```

這裡需要注意一個問題，就是boost庫的Libs設置在使用-l方法是需要去掉前面的lib前綴（實際生成的這兩個文件分別是libboost_filesystem.a和libboost_system.a，原因可參考Link Your Program to a Boost Library），而在使用第二種方法的時候則不需要，即


```sh
Libs:  ${libdir}/libboost_filesystem.a  ${libdir}/libboost_system.a
```

上面的方法使用的是boost的靜態庫，而對於第一種方法鏈接的是動態庫，這是需要設置環境變量告知系統動態庫的位置，即使用ldconfig工具，以boost為例，首先打開一個boost.conf文件，

```sh
$ sudo gedit /etc/ld.so.conf.d/boost.conf
```

加入boost的庫文件路徑,例如
```sh
/usr/local/boost-1.55.0/lib
```
然後執行
```sh
$ sudo ldconfig
```
把boost庫路徑讀入緩存，使系統能搜尋到庫的位置。如果不設置環境運行程序時，就會出現cannot open shared object file: No such file or directory的報錯。

另一個很重的問題是告訴pkg-config工具你為庫所寫的*.pc文件的路徑，這樣pkg-config才能找到*.pc文件並讀取庫的路徑鏈接信息。設置環境變量


```sh
$ sudo gedit /etc/profile
```
加入
```sh
export PKG_CONFIG_PATH=/usr/local/boost-1.55.0/pkgconfig:$PKG_CONFIG_PATH
```
這裡假設boost-1.55.0的pkgconfig文件boost.pc放在了/usr/local/boost-1.55.0/pkgconfig下。

以上所有步驟準備好了以後就可以使用pkg-config了。以上面已經寫好的boost.pc文件文件為例，在命令行下直接輸入

```sh
$ pkg-config --cflags boost
```
其輸出為
```sh
-I/usr/local/boost-1.55.0/include
```
輸入
```sh
$ pkg-config --libs boost
```
輸出

```sh
-L/usr/local/boost-1.55.0/lib/ -lboost_filesystem -lboost_system
```

對於鏈接靜態庫時，可以加入--static選項，即
```sh
$ pkg-config --libs --static boost 
```
這樣通過這些命令就可以很方便的實現程序的編譯鏈接，例如使用g++編譯個foo文件，調用了boost庫，則可以執行如下命令
```sh
$ g++ `pkg-config --cflags --libs boost` -o foo foo.cpp
```
以上語句相當於用pkg-config --cflags --libs boost替換了-I/usr/local/boost-1.55.0/include -L/usr/local/boost-1.55.0/lib/ -lboost_filesystem -lboost_system，其實還是g++的那些命令選項。

對於支持g++編譯命令選項的開發環境，使用pkg-config也是非常簡單的，比如CodeBlocks和QtCreator就支持g++，gcc編譯命令，具體使用方法可以參考另一篇博客如何在CodeBlocks和Qt Creator開發環境下使用OpenCV

-L/usr/local/boost-1.55.0/lib/ -lboost_filesystem -lboost_system