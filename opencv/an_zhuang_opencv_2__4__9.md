# 安裝 OpenCV 2.4.9


## install package


```sh
sudo apt-get install build-essential libgtk2.0-dev libjpeg-dev libtiff4-dev \
                     libjasper-dev libopenexr-dev cmake python-dev python-numpy \
                     python-tk libtbb-dev libeigen2-dev yasm libfaac-dev \
                     libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev \
                     libvorbis-dev libxvidcore-dev libx264-dev libqt4-dev \
                     libqt4-opengl-dev sphinx-common texlive-latex-extra libv4l-dev \
                     libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev
```

## download opencv-2.4.9

```sh 
wget -O OpenCV-2.4.9.zip http://fossies.org/linux/misc/opencv-2.4.9.zip
unzip OpenCV-2.4.9.zip
```

## build


```sh
cd opencv-2.4.9
mkdir debug ; cd debug

cmake -D CMAKE_INSTALL_PREFIX=/usr/local/opencv \
      -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
      -D CMAKE_BUILD_TYPE=DEBUG ..

make -j8 ; sudo make install
```

- cuda off

```sh
cmake -D CMAKE_INSTALL_PREFIX=/usr/local/opencv-2.4.13_cuda_off \
      -D WITH_CUDA=OFF -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
      -D CMAKE_BUILD_TYPE=DEBUG ..
```

## add library path

```sh
sudo vim /etc/ld.so.conf.d/opencv.conf
/usr/local/opencv/lib
sudo ldconfig
ldconfig -p | ag 'opencv'
```

## bashrc configure

```sh
~/.bashrc
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/opencv/lib/pkgconfig
export PKG_CONFIG_PATH
```

## test

```sh
cd opencv-2.4.9/samples/c
./build_all.sh
./facedetect lena.jpg
```
