
# ubuntu下多版本opencv切換

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

## 查看opencv版本：

```sh
pkg-config --modversion opencv249
pkg-config --modversion opencv310
```


## 編譯工程時：

```sh
g++ main.cpp -o main `pkg-config --libs --cflags opencv249`
//選擇249版本編譯
g++ main.cpp -o main `pkg-config --libs --cflags opencv310`
//選擇310版本編譯
```


```sh
/etc/ld.so.conf.d
opencv-2.4.13.conf
opencv-3.1.0.conf

~/.mybin/opencv-2.4.13/lib/pkgconfig
opencv-2.4.13.pc

/home/shihyu/.mybin/opencv-3.1.0/lib/pkgconfig
opencv-3.1.0.pc

PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/shihyu/.mybin/opencv-2.4.13/lib/pkgconfig
export PKG_CONFIG_PATH
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
