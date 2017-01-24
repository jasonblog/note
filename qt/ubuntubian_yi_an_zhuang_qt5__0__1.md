# ubuntu編譯安裝 Qt 5.8.0

## Requirement package

```sh
sudo apt-get install bison build-essential flex gperf ibgstreamer-plugins-base0.10-dev \
libasound2-dev libatkmm-1.6-dev libbz2-dev libcap-dev libcups2-dev libdrm-dev \
libegl1-mesa-dev libfontconfig1-dev libfreetype6-dev libgcrypt11-dev libglu1-mesa-dev \
libgstreamer0.10-dev libicu-dev libnss3-dev libpci-dev libpulse-dev libssl-dev libudev-dev \
libx11-dev libx11-xcb-dev libxcb-composite0 libxcb-composite0-dev libxcb-cursor-dev \
libxcb-cursor0 libxcb-damage0 libxcb-damage0-dev libxcb-dpms0 libxcb-dpms0-dev \
libxcb-dri2-0 libxcb-dri2-0-dev libxcb-dri3-0 libxcb-dri3-dev libxcb-ewmh-dev libxcb-ewmh2 \
libxcb-glx0 libxcb-glx0-dev libxcb-icccm4 libxcb-icccm4-dev libxcb-image0 libxcb-image0-dev \
libxcb-keysyms1 libxcb-keysyms1-dev libxcb-present-dev libxcb-present0 libxcb-randr0 \
libxcb-randr0-dev libxcb-record0 libxcb-record0-dev libxcb-render-util0 \
libxcb-render-util0-dev libxcb-render0 libxcb-render0-dev libxcb-res0 libxcb-res0-dev \
libxcb-screensaver0 libxcb-screensaver0-dev libxcb-shape0 libxcb-shape0-dev libxcb-shm0 \
libxcb-shm0-dev libxcb-sync-dev libxcb-sync0-dev libxcb-sync1 libxcb-util-dev \
libxcb-util0-dev libxcb-util1 libxcb-xevie0 libxcb-xevie0-dev libxcb-xf86dri0 \
libxcb-xf86dri0-dev libxcb-xfixes0 libxcb-xfixes0-dev libxcb-xinerama0 \
libxcb-xinerama0-dev libxcb-xkb-dev libxcb-xkb1 libxcb-xprint0 libxcb-xprint0-dev \
libxcb-xtest0 libxcb-xtest0-dev libxcb-xv0 libxcb-xv0-dev libxcb-xvmc0 libxcb-xvmc0-dev \
libxcb1 libxcb1-dev libxcomposite-dev libxcursor-dev libxdamage-dev libxext-dev \
libxfixes-dev libxi-dev libxrandr-dev libxrender-dev libxslt-dev libxss-dev \
libxtst-dev perl python ruby
```


## Download source code

```sh
http://download.qt.io/archive/qt/
http://download.qt.io/archive/qt/5.8/5.8.0/single/
```



```sh
tar xvf qt-everywhere-opensource-src-5.8.0.tar.xz
cd qt-everywhere-opensource-src-5.8.0
./configure -prefix `pwd`/build/ -debug -nomake tests
make -j8
make install
```

完成編譯安裝

- ~/.bashrc

因為我是將Qt 裝在自己的home directory, 所以$HOME/Qt/src/qt-everywhere-opensource-src-5.8.0/build/bin 裡面有一個qmake的程式, 通常Linux也會內建一個qmake程式, 有時這兩個版本會不相容. 所以當你要執行qmake這個程式時, 請確定是執行到$HOME/Qt/src/qt-everywhere-opensource-src-5.8.0/build/bin/qmake. 通常只要注意path設定的先後順序, 如下, 應該就沒問題:

```sh
export PATH=$HOME/Qt/src/qt-everywhere-opensource-src-5.8.0/build/bin/:$PATH 
```

在執行qmake -v的話，會有qmake 會把lib路徑指定編譯的路徑

```sh
# qmake -v
QMake version 3.1
Using Qt version 5.8.0 in /home/shihyu/qt-everywhere-opensource-src-5.8.0/build/lib
```


## Hello Qt test

- hello.cpp

```cpp
#include <QApplication>
#include <QLabel>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QLabel* label = new QLabel(
        "<h1><font color=blue>Hello!World!</font><font color=red>Orz...</font></h1>");
    label->setWindowTitle("First Qt!");
    label->resize(200, 50);
    label->show();

    return app.exec();
}
```

```sh
mkdir hello_qt
cd hello_qt
qmake -project
vim hello_qt.pro
QT+=widgets
qmake
make
./hello_qt
```



- 通常只有指定PATH, qmake 就會指定編譯的lib 路徑 

```sh
QTDIR=/home/shihyu/Qt/src/qt-everywhere-opensource-src-5.8.0/build
PATH=$QTDIR/bin:$PATH
QMAKESPEC=$QTDIR/mkspecs/linux-g++
MANPATH=$QTDIR/doc/man:$MANPATH
LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
export QTDIR PATH QMAKESPEC MANPATH LD_LIBRARY_PATH
```

