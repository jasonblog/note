# OpenCV


## 更新系統

```sh
sudo apt-get update
sudo apt-get dist-upgrade -y

# 安裝必要軟件
sudo apt-get install build-essentia
sudo apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
sudo apt-get install python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
```

## 安裝 OpenCV

檔案從 OpenCV 的官方git下載，下載指令如下

```sh
mkdir opencv34
cd opencv34

git clone -b 3.4 https://github.com/opencv/opencv
git clone -b 3.4 https://github.com/opencv/opencv_contrib
```

再來就會自己下載好了然後我們建立一個資料夾準備編譯

```sh
mkdir build

cmake -D CMAKE_INSTALL_PREFIX=/usr/local/opencv -D CMAKE_BUILD_TYPE:STRING=RelWithDebInfo -D CMAKE_BUILD_TYPE=DEBUG -DBUILD_opencv_xfeatures2d=OFF -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules ../opencv


# -DBUILD_opencv_xfeatures2d=OFF 套件有問題關閉不要編譯

make -j8
sudo make install


# 新增執行檔所需要的 lib 庫路徑到環境變數上
sudo ldconfig

```


## 測試
編譯的時候使用 pkg-config 幫我們設置路徑，我們先測試一下是否正確安裝OpenCV了


```sh
# 察看OpenCV版本
pkg-config -- modversion

# 察看路徑與函式庫
pkg-config opencv --libs --cflags
```

