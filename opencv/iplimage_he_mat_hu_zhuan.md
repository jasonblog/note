# IplImage 和 Mat 互轉


寫OpenCV相關程式，我會使用兩種IDE，一是為了簡單方便用Dev C++，二是為了測試高竿程式用Visual
C++。前者我通常用C來寫，後者則以C++來寫，更有時候會把兩者兜在一起。


就如同string 和 char[]互轉這篇文章一樣，時常需要將字串來回轉換，在OpenCV中處理對象為影像，有時也需要來回轉換，於是我想記錄一下：Ｐ
比較專業的講法是：將IplImage圖形資料空間(imageData)轉成CvMat空間(data)，或是將CvMat矩陣空間轉成IplImage的圖形空間。
程式碼相當簡單，如下：



```c
// IplImage to Mat
IplImage *img;
 
cv::Mat mat(img, 0);
 
// Mat to IplImage
cv::Mat mat;
 
IplImage *img = IplImage(mat);
```

`需要注意的是，兩者互轉皆為共用記憶體位置！`