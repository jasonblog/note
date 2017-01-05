# 通過C++ wrapper 實現存取圖像像素


```c
#ifndef _OPENCV_H_
#define _OPENCV_H_

#include <opencv2/opencv.hpp>
//#include "opencv/cv.h"
//#include "opencv/highgui.h"

#ifdef _WIN32
#ifdef _DEBUG
//Debug   mode
#pragma comment(lib,"opencv_core231d.lib")
#pragma comment(lib,"opencv_highgui231d.lib")
#pragma comment(lib,"opencv_imgproc231d.lib")
#pragma comment(lib,"opencv_video231d.lib")
#pragma comment(lib,"opencv_ml231d.lib")
#pragma comment(lib,"opencv_legacy231d.lib")
#else
//Release   mode
#pragma comment(lib,"opencv_core231.lib")
#pragma comment(lib,"opencv_highgui231.lib")
#pragma comment(lib,"opencv_imgproc231.lib")
#pragma comment(lib,"opencv_video231.lib")
#pragma comment(lib,"opencv_ml231.lib")
#pragma comment(lib,"opencv_legacy231.lib")
#endif
#else
// other
#endif

template<class T> class Image
{
private:
    IplImage* imgp;
public:
    Image(IplImage* img = 0)
    {
        imgp = img;
    }

    ~Image()
    {
        imgp = 0;
    }

    void operator=(IplImage* img)
    {
        imgp = img;
    }

    inline T* operator[](const int rowIndx)
    {
        return ((T*)(imgp->imageData + rowIndx * imgp->widthStep));
    }
};

typedef struct {
    unsigned char b, g, r;
} RgbPixel;

typedef struct {
    float b, g, r;
} RgbPixelFloat;

typedef struct {
    double b, g, r;
} RgbPixelDouble;

typedef Image<RgbPixel>       RgbImage;
typedef Image<RgbPixelFloat>  RgbImageFloat;
typedef Image<RgbPixelDouble> RgbImageDouble;
typedef Image<unsigned char>  BwImage;
typedef Image<int>            BwImageInt;
typedef Image<float>          BwImageFloat;
typedef Image<double>         BwImageDouble;

#endif
```

例：對3通道的字節型圖像訪問如下：

```c
IplImage* img = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
RgbImage imgA(img);
//假設訪問第K通道、第i行、第j列的像素
imgA[i][j].b = 111;
imgA[i][j].g = 111;
imgA[i][j].r = 111;
```

