# ubuntu編譯安裝qt5.0.1


1、進入官網：http://qt-project.org/downloads下載對應的源碼包；
or 

```sh
http://download.qt.io/archive/qt/
http://download.qt.io/archive/qt/5.8/5.8.0/single/
```


2、解壓

```sh
tar xvf qt-everywhere-opensource-src-5.8.0.tar.xz
```

3、
```sh
qt-everywhere-opensource-src-5.8.0
```

4、
```sh
./configure -prefix /home/shihyu/Qt/src/qt-everywhere-opensource-src-5.8.0/build/ \
-debug -nomake tests
```

5、
```sh
make -j4
```

6、
```sh
sudo make install
```

完成編譯安裝

7.
- ~/.bashrc

因為我是將Qt 裝在自己的home directory, 所以$HOME/Qt/src/qt-everywhere-opensource-src-5.8.0/build/bin 裡面有一個qmake的程式, 通常Linux也會內建一個qmake程式, 有時這兩個版本會不相容. 所以當你要執行qmake這個程式時, 請確定是執行到$HOME/Qt/src/qt-everywhere-opensource-src-5.8.0/build/bin/qmake. 通常只要注意path設定的先後順序, 如下, 應該就沒問題:

```sh
export PATH=$HOME/Qt/src/qt-everywhere-opensource-src-5.8.0/build/bin/:$PATH 
```

```sh
QTDIR=/home/shihyu/Qt/src/qt-everywhere-opensource-src-5.8.0/build
PATH=$QTDIR/bin:$PATH
QMAKESPEC=$QTDIR/mkspecs/linux-g++
MANPATH=$QTDIR/doc/man:$MANPATH
LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
export QTDIR PATH QMAKESPEC MANPATH LD_LIBRARY_PATH
```

