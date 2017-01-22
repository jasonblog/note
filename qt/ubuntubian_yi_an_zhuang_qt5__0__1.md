# ubuntu编译安装qt5.0.1


1、进入官网：http://qt-project.org/downloads下载对应的源码包；

2、解压;tar -xvzf  qt-everywhere-opensource-src-5.0.1.tar.gz

3、cd  qt-everywhere-opensource-src-5.0.1

4、./configure --prefix=\`pwd\`/../qt5  -nomake tests


出现：The test for linking against libxcb and support libraries failed!
 You might need install dependency packages, or pass -qt-xcb.
 See src/plugins/platforms/xcb/README.
*** qtbase/configure exited with non-zero status.

查看/home/shiyan/Desktop/QT/qt-everywhere-opensource-src-5.0.1/qtbase/src/plugins/platforms/xcb/README

On Ubuntu 12.04 icccm1 is replaced by icccm4 and xcb-render-util can be installed automatically:

```sh
libxcb1 libxcb1-dev libx11-xcb1 libx11-xcb-dev libxcb-keysyms1 \
libxcb-keysyms1-dev libxcb-image0 libxcb-image0-dev libxcb-shm0 \
libxcb-shm0-dev libxcb-icccm4 libxcb-icccm4-dev libxcb-sync0 \
libxcb-sync0-dev libxcb-xfixes0-dev libxrender-dev libxcb-shape0-dev \
libxcb-randr0-dev libxcb-render-util0 libxcb-render-util0-dev libxcb-glx0-dev

```
解决： 

```sh
sudo apt-get install  libxcb1 libxcb1-dev libx11-xcb1 libx11-xcb-dev \
libxcb-keysyms1 libxcb-keysyms1-dev libxcb-image0 libxcb-image0-dev \
libxcb-shm0 libxcb-shm0-dev libxcb-icccm4 libxcb-icccm4-dev libxcb-sync0 \
libxcb-sync0-dev libxcb-xfixes0-dev libxrender-dev libxcb-shape0-dev \
libxcb-randr0-dev libxcb-render-util0 libxcb-render-util0-dev libxcb-glx0-dev
```

5、
```sh
make -j4
```

6、
```sh
sudo make install
```

完成编译安装