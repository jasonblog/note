# ubuntu 18.04 安裝 OpenCV3.4  contrib 編譯 C++


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
pkg-config --modversion opencv

# 察看路徑與函式庫
pkg-config opencv --libs --cflags
```

這邊都ok有看到東西就安裝好了，你應該會看到


```sh
3.4.7

-I/usr/local/opencv/include/opencv -I/usr/local/opencv/include -L/usr/local/opencv/lib -lopencv_superres -lopencv_stitching -lopencv_shape -lopencv_videostab -lopencv_tracking -lopencv_plot -lopencv_img_hash -lopencv_ccalib -lopencv_aruco -lopencv_structured_light -lopencv_line_descriptor -lopencv_bioinspired -lopencv_reg -lopencv_saliency -lopencv_hfs -lopencv_freetype -lopencv_hdf -lopencv_surface_matching -lopencv_dpm -lopencv_phase_unwrapping -lopencv_xobjdetect -lopencv_dnn_objdetect -lopencv_rgbd -lopencv_face -lopencv_objdetect -lopencv_optflow -lopencv_ximgproc -lopencv_stereo -lopencv_xphoto -lopencv_photo -lopencv_fuzzy -lopencv_bgsegm -lopencv_calib3d -lopencv_video -lopencv_datasets -lopencv_text -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_features2d -lopencv_dnn -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core
```

再來讓我們編譯CPP檔案吧，下面是範例代碼，把他儲存為 opencv.cpp

```cpp
/**********************************************************
Name :
Date : 2016/05/29
By   : CharlotteHonG
Final: 2018/05/23
**********************************************************/
#include <iostream>
using namespace std;

#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char const *argv[]) {
    /* 畫布 */
    Mat img(270, 720, CV_8UC3, Scalar(56, 50, 38));
    /* 直線 */
    line(img, Point(20, 40), Point(120, 140), Scalar(255, 0, 0), 3);
    /* 實心方塊 */
    rectangle(img, Point(150, 40), Point(250, 140), Scalar(0, 0, 255), -1);
    /* 實心圓 */
    circle(img, Point(330, 90), 50, Scalar(0, 255, 0), -1);
    /* 空心橢圓 */
    ellipse(img, Point(460, 90), Size(60, 40), 45, 0, 360, Scalar(255, 255, 0), 2);
    /* 不規則圖形 */
    Point points[1][5];
    int x = 40, y = 540;
    points[0][0] = Point(0 + y, 50 + x);
    points[0][1] = Point(40 + y, 0 + x);
    points[0][2] = Point(110 + y, 35 + x);
    points[0][3] = Point(74 + y, 76 + x);
    points[0][4] = Point(28 + y, 96 + x);
    const Point* ppt[1] = { points[0] };
    int npt[] = { 5 };
    polylines(img, ppt, npt, 1, 1, Scalar(0, 255, 255), 3);
    /* 繪出文字 */
    putText(img, "Test Passed !!", Point(10, 230), 0, 3, Scalar(255, 170, 130), 3);
    /* 開啟畫布 */
    imshow("OpenCV Test By:Charlotte.HonG", img);
    waitKey(0);
    return 0;
}
```

長得很奇怪這沒有打錯，是蚯蚓上的那一撇
```sh
g++ opencv.cpp -o opencv  `pkg-config opencv --cflags --libs`

執行確認結果

./opencv
```

執行出問題 未定義參考到「cv::String::deallocate()」 opencv

可能是剛剛你的設置函式庫路徑沒執行，再重新執行一次 sudo ldconfig


最後是 makefile 檔不過我也正在學習，還不知道通常正規是怎麼寫的，就是一個能跑的而已~



`CXX = g++

CXXFLAG :=
CXXFLAG += -O3
CXXFLAG += -std=c++11
CXXFLAG += -Wall

CXXLIBS :=
CXXLIBS += `pkg-config opencv --libs`

CXXINCS :=
CXXINCS += `pkg-config opencv --cflags`

CPPFILES :=
CPPFILES := opencv.cpp

EXEFILE := main

all: opencv
opencv: opencv.cpp
    g++ $(CXXFLAG) $(CPPFILES) -o $(EXEFILE) $(CXXINCS) $(CXXLIBS)

run: opencv
    ./$(EXEFILE)
```

