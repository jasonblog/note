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

