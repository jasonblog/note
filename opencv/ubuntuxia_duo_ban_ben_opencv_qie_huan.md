# ubuntu下多版本opencv切換


的第三方库当中，所以我决定再装2.4版本。
在win平台上面，多版本只需要添加多个环境变量即可，同理，linux下面也是通过改变环境变量设置达到多版本的目的。

首先需要了解pkg-config这个包管理工具，关于opencv的包含和库的路径的识别，都是依赖于这个管理工具的.pc文件，具体内容自行google。

还需要一提的是，我们要分清楚一下几个关键文件的作用和不同。


```sh
/etc/bash.bashrc                                         //系统变量，（方法1）可通过向其添加指向opencv.pc的变量路径以达到系统识别的目的
/home/jiluobo/.bashrc                               //用户变量，（方法2）可通过向其添加指向opencv.pc的变量路径以达到系统识别的目的
/etc/ld.so.conf                                              //编译时链接库的路径，一般内容为：include /etc/ld.so.conf.d/*.conf
/etc/ld.so.conf.d/*.conf                               //存放各.pc文件的链接库路径
/usr/lib/pkgconfig/*.pc                              //pkg-config工具识别的.pc文件
```

好了，不费话，多版本切换的方法有许多，好处也是多多滴，这里我选择一种最方便，而且需要卸载时都没有任何残留的方式。

这里选择从编译opencv开始，可以按自己需要选择性观看，以便出错时回头查错：



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
    
    

//添加pkg-config包路径信息
sudo cp ~/.mybin/opencv-2.4.13/lib/pkgconfig/opencv.pc /usr/lib/pkgconfig/opencv2.pc
sudo cp ~/.mybin/opencv-3.3.1/lib/pkgconfig/opencv.pc /usr/lib/pkgconfig/opencv3.pc

//添加链接库路径
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


##编译工程时：

```sh
g++ main.cpp -o main `pkg-config --libs --cflags opencv2`
//选择2版本编译
g++ main.cpp -o main `pkg-config --libs --cflags opencv3`
//选择3版本编译
```

##使用qt建立工程时，在工程.pro文件中添加：


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

##使用CMake编译工程时：


在相应版本的opencv安装目录下（/usr/local/opencv-2.4.9/share/OpenCV）查找OpenCVConfig.cmake，然后使用文件所在的那个路径


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
