# Opencv 3.4 + gdb


```sh
sudo apt -y remove x264 libx264-dev
 
## Install dependencies
sudo apt -y install build-essential checkinstall cmake pkg-config yasm
sudo apt -y install git gfortran
sudo apt -y install libjpeg8-dev libpng-dev
 
sudo apt -y install software-properties-common
sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"
sudo apt -y update
 
sudo apt -y install libjasper1
sudo apt -y install libtiff-dev
 
sudo apt -y install libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev
sudo apt -y install libxine2-dev libv4l-dev
cd /usr/include/linux
sudo ln -s -f ../libv4l1-videodev.h videodev.h
cd "$cwd"
 
sudo apt -y install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
sudo apt -y install libgtk2.0-dev libtbb-dev qt5-default
sudo apt -y install libatlas-base-dev
sudo apt -y install libfaac-dev libmp3lame-dev libtheora-dev
sudo apt -y install libvorbis-dev libxvidcore-dev
sudo apt -y install libopencore-amrnb-dev libopencore-amrwb-dev
sudo apt -y install libavresample-dev
sudo apt -y install x264 v4l-utils
 
# Optional dependencies
sudo apt -y install libprotobuf-dev protobuf-compiler
sudo apt -y install libgoogle-glog-dev libgflags-dev
sudo apt -y install libgphoto2-dev libeigen3-dev libhdf5-dev doxygen


git clone https://github.com/opencv/opencv.git
cd opencv
git checkout 3.4
cd ..
 
git clone https://github.com/opencv/opencv_contrib.git
cd opencv_contrib
git checkout 3.4
cd ..


cd opencv
mkdir build
cd build


cmake -D CMAKE_BUILD_TYPE=DEBUG \
            -D CMAKE_INSTALL_PREFIX=/home/shihyu/.mybin/opencv347 \
            -D INSTALL_C_EXAMPLES=ON \
            -D INSTALL_PYTHON_EXAMPLES=ON \
            -D WITH_TBB=ON \
            -D WITH_V4L=ON \
	    -D WITH_CUDA=OFF \
            -D WITH_QT=ON \
            -D WITH_OPENGL=ON \
            -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules \
            -D BUILD_opencv_xfeatures2d=OFF \
            -D BUILD_EXAMPLES=ON ..


make install
```

```cpp
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv)
{

    Mat image;
    image = imread("Sinsinawa_640_480.jpg", 1);

    if (!image.data) {
        printf("No image data \n");
        return -1;
    }

    namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
    imshow("Display Image", image);

    waitKey(0);

    return 0;
}
```


```sh
CFLAGS=-g -I/home/shihyu/.mybin/opencv347/include
LIBS=/home/shihyu/.mybin/opencv347/lib

test4:test4.cpp
	g++ $(CFLAGS) -o $@ $< -Wl,-rpath=/home/shihyu/.mybin/opencv347/lib -L$(LIBS)  -lopencv_highgui -lopencv_core -lopencv_imgcodecs 
```

```sh
gdb  `find /home/shihyu/opencv347/opencv/ -type d -printf '-d %p '`  ./test4
b imread // 不能 b *imread
run
```

```sh
// g++ 編譯執行檔已經決定 lib 路徑,所以一般編譯執行檔不需要用 solib-search-path

set  solib-search-path /home/shihyu/.mybin/opencv347/lib
```

## cgdb 

```sh
cgdb ./test4 -- -ex="`find /home/shihyu/opencv347/opencv/modules/ -type d -printf 'dir %p '`"
```

---

```
g++ -g -O0 0927_block.cpp -o main  `pkg-config opencv --cflags --libs` && ./main ./test.csv

gdb  `find /home/shihyu/opencv347/opencv/ -type d -printf '-d %p '` --args ./main ./test.csv

b remap // 不能 b *remap
```




