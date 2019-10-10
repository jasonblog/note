#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
using namespace cv;

int main(int argc, char** argv)
{
    Mat srcImage, dstImage;
    Mat map_x, map_y;
    namedWindow("程式視窗");
    //載入原始圖
    srcImage = imread("./30.jpeg", 0);
    //建立和原始圖一樣的效果圖，x重對映圖，y重對映圖
    dstImage.create(srcImage.size(), srcImage.type());
    map_x.create(srcImage.size(), CV_32FC1);
    map_y.create(srcImage.size(), CV_32FC1);

    //雙層迴圈，遍歷每一個畫素點，改變map_x & map_y的值
    for (int j = 0; j < srcImage.rows; j++) {
        for (int i = 0; i < srcImage.cols; i++) {
            //改變map_x & map_y的值.
            map_x.at<float>(j, i) = static_cast<float>(i);
            map_y.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
        }
    }

    //進行重對映操作
    remap(srcImage, dstImage, map_x, map_y, CV_INTER_CUBIC, BORDER_CONSTANT, Scalar(0, 0, 0));
    imshow("程式視窗", dstImage);
    waitKey(0);
    return (0);
}
