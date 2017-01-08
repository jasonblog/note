# 如何把OpenCV Python獲取的圖像傳遞到C層處理


用OpenCV Python來開發，如果想要用到一些C/C++的圖像處理庫，就需要創建Python的C/C++擴展，然後把數據從Python傳遞到底層處理。這裡分享下如何在C/C++層獲取數據。

參考原文：[How to Convert OpenCV Image Data from Python to C](http://www.codepool.biz/convert-opencv-image-python-c.html)


##開發環境

- Dynamsoft Barcode Reader 4.2
- Python version: 2.7.0
- OpenCV version: 2.4.10. Download cv2.pyd
- Windows 10
- USB webcam

## Python C/C++擴展

把DynamsoftBarcodeReaderx86.dll和cv2.pyd拷貝到目錄Python27\Lib\site-packages。

OpenCV Python獲取的圖像數據類型是numpy.ndarray:

```py
> rval, frame = vc.read();
> print type(frame)
> <type 'numpy.ndarray'>
```

在C層我們希望能獲取到數據的指針。查看OpenCV源碼文件opencv\modules\python\src2\cv2.cv.hpp可以找到方法：

```cpp
PyObject *o;
if (!PyArg_ParseTuple(args, "O", &o))
    return NULL;
 
PyObject *ao = PyObject_GetAttrString(o, "__array_struct__");
PyObject *retval;
 
if ((ao == NULL) || !PyCObject_Check(ao)) {
    PyErr_SetString(PyExc_TypeError, "object does not have array interface");
    return NULL;
}
 
PyArrayInterface *pai = (PyArrayInterface*)PyCObject_AsVoidPtr(ao);
if (pai->two != 2) {
    PyErr_SetString(PyExc_TypeError, "object does not have array interface");
    Py_DECREF(ao);
    return NULL;
}
 
// Construct data with header info and image data 
char *buffer = (char*)pai->data; // The address of image data
int width = pai->shape[1];       // image width
int height = pai->shape[0];      // image height
int size = pai->strides[0] * pai->shape[0]; // image size = stride * height
```

這樣就可以了。現在可以用這個數據做點事情，比如調用barcode接口來做檢測。我依然用Dynamsoft Barcode Reader SDK做示例。首先需要構建一下數據：

```cpp
char *total = (char *)malloc(size + 40); // buffer size = image size + header size
memset(total, 0, size + 40);
BITMAPINFOHEADER bitmap_info = {40, width, height, 0, 24, 0, size, 0, 0, 0, 0};
memcpy(total, &bitmap_info, 40);
 
// Copy image data to buffer from bottom to top
char *data = total + 40;
int stride = pai->strides[0];
for (int i = 1; i <= height; i++) {
    memcpy(data, buffer + stride * (height - i), stride);
    data += stride;
}
```

接下來就可以檢測barcode了：

```cpp
// Dynamsoft Barcode Reader initialization
    __int64 llFormat = (OneD | QR_CODE | PDF417 | DATAMATRIX);
    int iMaxCount = 0x7FFFFFFF;
    ReaderOptions ro = {0};
    pBarcodeResultArray pResults = NULL;
    ro.llBarcodeFormat = llFormat;
    ro.iMaxBarcodesNumPerPage = iMaxCount;
    printf("width: %d, height: %d, size:%d\n", width, height, size);
    int iRet = DBR_DecodeBuffer((unsigned char *)total, size + 40, &ro, &pResults);
    printf("DBR_DecodeBuffer ret: %d\n", iRet);
    free(total); // Do not forget to release the constructed buffer 
     
    // Get results
    int count = pResults->iBarcodeCount;
    pBarcodeResult* ppBarcodes = pResults->ppBarcodes;
    pBarcodeResult tmp = NULL;
    retval = PyList_New(count); // The returned Python object
    PyObject* result = NULL;
    for (int i = 0; i < count; i++)
    {
        tmp = ppBarcodes[i];
        result = PyString_FromString(tmp->pBarcodeData);
        printf("result: %s\n", tmp->pBarcodeData);
        PyList_SetItem(retval, i, Py_BuildValue("iN", (int)tmp->llFormat, result)); // Add results to list
    }
    // release memory
    DBR_FreeBarcodeResults(&pResults);
```

在Windows上構建Python擴展需要先設置一下，不然會出錯。我使用Visual Studio 2015。命令行如下：

```py
SET VS90COMNTOOLS=%VS140COMNTOOLS%
python setup.py build install
```

好了。現在可以用Python腳本來調用了。首先打開攝像頭：

```py
import cv2
from dbr import *
import time
 
vc = cv2.VideoCapture(0)
```

接下來讀取一幀的數據：

```py
cv2.imshow(windowName, frame)
rval, frame = vc.read();
```

現在可以實時檢測barcode了：

```py
initLicense("<license>") # Invalid license is fine.
results = decodeBuffer(frame)
if (len(results) > 0):
    print "Total count: " + str(len(results))
    for result in results:
        print "Type: " + types[result[0]]
        print "Value: " + result[1] + "\n"
```

##源碼

https://github.com/yushulx/opencv-python-webcam-barcode-reader

