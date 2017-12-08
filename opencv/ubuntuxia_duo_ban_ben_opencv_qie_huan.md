# ubuntu下多版本opencv切換


的第三方庫當中，所以我決定再裝2.4版本。
在win平臺上面，多版本只需要添加多個環境變量即可，同理，linux下面也是通過改變環境變量設置達到多版本的目的。

首先需要了解pkg-config這個包管理工具，關於opencv的包含和庫的路徑的識別，都是依賴於這個管理工具的.pc文件，具體內容自行google。

還需要一提的是，我們要分清楚一下幾個關鍵文件的作用和不同。


```sh
/etc/bash.bashrc                                         //系統變量，（方法1）可通過向其添加指向opencv.pc的變量路徑以達到系統識別的目的
/home/jiluobo/.bashrc                               //用戶變量，（方法2）可通過向其添加指向opencv.pc的變量路徑以達到系統識別的目的
/etc/ld.so.conf                                              //編譯時鏈接庫的路徑，一般內容為：include /etc/ld.so.conf.d/*.conf
/etc/ld.so.conf.d/*.conf                               //存放各.pc文件的鏈接庫路徑
/usr/lib/pkgconfig/*.pc                              //pkg-config工具識別的.pc文件
```

好了，不費話，多版本切換的方法有許多，好處也是多多滴，這裡我選擇一種最方便，而且需要卸載時都沒有任何殘留的方式。

這裡選擇從編譯opencv開始，可以按自己需要選擇性觀看，以便出錯時回頭查錯：



```sh

cmake -D CMAKE_INSTALL_PREFIX=/home/shihyu/.mybin/opencv-2.4.13 \
      -D CMAKE_PREFIX_PATH=/home/shihyu/.mybin/opencv-2.4.13/share/OpenCV \
      -D BUILD_NEW_PYTHON_SUPPORT=ON \
      -D CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -D_FORCE_INLINES" \
      -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
      -D CMAKE_BUILD_TYPE=DEBUG \
      -D WITH_CUDA=OFF ..


cmake -D CMAKE_INSTALL_PREFIX=/home/shihyu/.mybin/opencv-3.3.1 \
      -D CMAKE_PREFIX_PATH=/home/shihyu/.mybin/opencv-3.3.1/share/OpenCV \
      -D BUILD_NEW_PYTHON_SUPPORT=ON \
      -D CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -D_FORCE_INLINES" \
      -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
      -D CMAKE_BUILD_TYPE=DEBUG \
      -D WITH_CUDA=OFF ..
    
    

//添加pkg-config包路徑信息
sudo cp ~/.mybin/opencv-2.4.13/lib/pkgconfig/opencv.pc /usr/lib/pkgconfig/opencv2.pc
sudo cp ~/.mybin/opencv-3.3.1/lib/pkgconfig/opencv.pc /usr/lib/pkgconfig/opencv3.pc

//添加鏈接庫路徑
sudo vim /etc/ld.so.conf.d/opencv2.conf
/home/shihyu/.mybin/opencv-2.4.13/lib/

sudo vim /etc/ld.so.conf.d/opencv3.conf
/home/shihyu/.mybin/opencv-3.3.1/lib
 
sudo ldconfig
//刷新
```


##查看opencv版本：

```sh
pkg-config --modversion opencv2
pkg-config --modversion opencv3
```


##編譯工程時：

```sh
g++ main.cpp -o main `pkg-config --libs --cflags opencv2`
//選擇2版本編譯
g++ main.cpp -o main `pkg-config --libs --cflags opencv3`
//選擇3版本編譯
```

##使用qt建立工程時，在工程.pro文件中添加：


```sh
INCLUDEPATH += /usr/local/opencv2.4.9/include \
                               /usr/local/opencv2.4.9/include/opencv \
                               /usr/local/opencv2.4.9/include/opencv2
LIBS += /usr/local/opencv2.4.9/lib/*.so
//249版本
INCLUDEPATH += /usr/local/opencv3.1.0/include \
                               /usr/local/opencv3.1.0/include/opencv \
                               /usr/local/opencv3.1.0/include/opencv2
LIBS += /usr/local/opencv3.1.0/lib/*.so
//310版本
```

##使用CMake編譯工程時：
在CMakeLists.txt 裡面加入不同版本路徑 
set(OpenCV_DIR /home/shihyu/.mybin/opencv-3.3.1/share/OpenCV)
find_package(OpenCV)

在相應版本的opencv安裝目錄下（/usr/local/opencv-2.4.9/share/OpenCV）查找OpenCVConfig.cmake，然後使用文件所在的那個路徑



```sh
#!/bin/sh

if [ $# -gt 0 ] ; then
    base=`basename $1 .cpp`
    echo "compiling $base"
    g++ -ggdb `pkg-config --cflags opencv` $base.cpp -o $base `pkg-config --libs opencv`
else
    for i in *.c; do
        echo "compiling $i"
        gcc -ggdb `pkg-config --cflags opencv` -o `basename $i .c` $i `pkg-config --libs opencv`;
    done
    for i in *.cpp; do
        echo "compiling $i"
        g++ -ggdb `pkg-config --cflags opencv` -o `basename $i .cpp` $i `pkg-config --libs opencv`;
    done
fi
```


---


```sh
cmake -D CMAKE_INSTALL_PREFIX=/home/shihyu/.mybin/opencv-2.4.13 \
      -D CMAKE_PREFIX_PATH=/home/shihyu/.mybin/opencv-2.4.13/share/OpenCV \
      -D BUILD_NEW_PYTHON_SUPPORT=ON \
      -D CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -D_FORCE_INLINES" \
      -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
      -D CMAKE_BUILD_TYPE=DEBUG \
      -D WITH_CUDA=OFF ..



cmake -D CMAKE_INSTALL_PREFIX=/home/shihyu/.mybin/opencv-3.2.0 \
      -D CMAKE_PREFIX_PATH=/home/shihyu/.mybin/opencv-3.2.0/share/OpenCV \
      -D BUILD_NEW_PYTHON_SUPPORT=ON \
      -D CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -D_FORCE_INLINES" \
      -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo \
      -D CMAKE_BUILD_TYPE=DEBUG \
      -D WITH_CUDA=OFF ..


-D WITH_CUDA=OFF // cuda disable
```



## 編譯工程時：

```sh
g++ -ggdb `pkg-config --cflags opencv-2.4.13`  main.cpp -o main `pkg-config --libs opencv-2.4.13`
//選擇2.4.13版本編譯

g++ -ggdb `pkg-config --cflags opencv-3.2.0`  main.cpp -o main `pkg-config --libs opencv-3.2.0`
//選擇3.2.0版本編譯
```


```sh
cd /home/shihyu/.mybin/opencv-2.4.13/lib/pkgconfig
mv opencv.pc opencv-2.4.13.pc

cd /home/shihyu/.mybin/opencv-3.2.0/lib/pkgconfig
mv opencv.pc opencv-3.2.0.pc


cd /etc/ld.so.conf.d

vim opencv2.conf
/home/shihyu/.mybin/opencv-2.4.13/lib/

vim opencv3.conf
/home/shihyu/.mybin/opencv-3.2.0/lib/

~/.bashrc
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/shihyu/.mybin/opencv-2.4.13/lib/pkgconfig
export PKG_CONFIG_PATH

PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/shihyu/.mybin/opencv-3.2.0/lib/pkgconfig
export PKG_CONFIG_PATH
```

## 查看opencv版本：

```sh
pkg-config --modversion opencv-2.4.13
pkg-config --modversion opencv-3.2.0
```

- build_all.sh

```sh
#!/bin/sh

if [ $# -gt 0 ] ; then
    base=`basename $1 .c`
    echo "compiling $base"
    gcc -ggdb `pkg-config opencv --cflags --libs` $base.c -o $base
else
    for i in *.c; do
        echo "compiling $i"
        gcc -ggdb `pkg-config --cflags opencv` -o `basename $i .c` $i `pkg-config --libs opencv`;
    done
    for i in *.cpp; do
        echo "compiling $i"
        g++ -ggdb `pkg-config --cflags opencv` -o `basename $i .cpp` $i `pkg-config --libs opencv`;
    done
fi
```

```sh
cd /home/jiluobo/
git clone https://github.com/opencv/opencv.git
//git opencv 源碼
cp -rf opencv opencvbackup
//備份防止出錯
cd opencv
mkdir opencv249
mkdir opencv310
//添加生成不同版本目錄
git checkout 2.4.9
//切換版本
cd opencv249
cmake -D CMAKE_INSTALL_PREFIX=/usr/local/opencv2.4.9 -D CMAKE_PREFIX_PATH=/usr/local/opencv2.4.9/share/OpenCV CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -D_FORCE_INLINES" ../
//兩個-D選項為多版本關鍵設置，此處省略了其他可選設置，根據自己情況添加，默認也可以
make -j8
//編譯
sudo make install
//安裝
cd ../
git checkout 3.1.0
cd opencv310
cmake -D CMAKE_INSTALL_PREFIX=/usr/local/opencv3.1.0 -D CMAKE_PREFIX_PATH=/usr/local/opencv3.1.0/share/OpenCV CMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -D_FORCE_INLINES" ../
//兩個-D選項為多版本關鍵設置，此處省略了其他可選設置，根據自己情況添加，默認也可以
make -j8
//編譯
sudo make install
//安裝
 
sudo cp /usr/local/opencv2.4.9/lib/pkgconfig/opencv.pc /usr/lib/pkgconfig/opencv249.pc
//添加pkg-config包路徑信息
sudo cp /usr/local/opencv3.1.0/lib/pkgconfig/opencv.pc /usr/lib/pkgconfig/opencv310.pc
//添加pkg-config包路徑信息
 
sudo nano /etc/ld.so.conf.d/opencv249.conf
>>/usr/local/opencv2.4.9/lib
//添加鏈接庫路徑
sudo nano /etc/ld.so.conf.d/opencv310.conf
>>/usr/local/opencv3.1.0/lib
 
sudo ldconfig
//刷新
```

--- 

##1、假如安裝了opencv以下兩個版本：

```sh
opencv-2.4.13 
opencv-3.2.0
```

##2、安裝目錄分別為：

```sh
/usr/local/opencv-2.4.13 
/usr/local/opencv-3.2.0
```

##3、opencv-2.4.13切換為opencv-3.2.0

默認版本為/usr/local/opencv-2.4.13

##4、打開終端，輸入以下命令：

###1、打開文件：

```sh
sudo gedit ~/.bashrc 
```

###2、在文件末尾添加以下內容，然後保存：

```sh
export PKG_CONFIG_PATH=/usr/local/opencv-3.2.0/lib/pkgconfig  
export LD_LIBRARY_PATH=/usr/local/opencv-3.2.0/lib  
```

###3、輸入命令：

```sh
source ~/.bashrc  
```

###4、繼續輸入

```sh
pkg-config --modversion opencv  
pkg-config --cflags opencv  
pkg-config --libs opencv  
```

以上步驟完成，即為替換完成，如果想從opencv-3.2.0切換回去，步驟同上。
