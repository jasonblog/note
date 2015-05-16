# Android 各層中日誌打印功能的應用


### 1. HAL層
```
頭文件：
#include <utils/Log.h>

對應的級別 打印方法
VERBOSE LOGV()
DEBUG LOGD()
INFO LOGI()
WARN LOGW()
ERROR LOGE()

方法：
LOGD("%d, %s", int, char* )
```

### 2. JNI層
```
頭文件：
#include <utils/Log.h>

對應的級別 打印方法
VERBOSE LOGV()
DEBUG LOGD()
INFO LOGI()
WARN LOGW()
ERROR LOGE()

方法：
LOGD("%d, %s", int, char* )
```

### 3. FRAMEWORK層
```
import android.util.Slog;

對應的級別 打印方法
VERBOSE Slog.v()
DEBUG Slog.d()
INFO Slog.i()
WARN Slog.w()
ERROR Slog.e()

方法：
Slog.d(TAG, "something to say.");
```

### 4. JAVA層
```
import android.util.Log;

對應的級別 打印方法
VERBOSE Log.v()
DEBUG Log.d()
INFO Log.i()
WARN Log.w()
ERROR Log.e()

方法：
Log.d(TAG, "something to say.");
```


### Java打印文件名、函數名、行號
```
Log.d(TAG, new Exception().getStackTrace()[0].getMethodName()); //函數名
Log.d(TAG,
      Thread.currentThread().getStackTrace()[2].getMethodName()); //函數名
Log.d(TAG, "" +
      Thread.currentThread().getStackTrace()[2].getLineNumber()); //行號
Log.d(TAG, Thread.currentThread().getStackTrace()[2].getFileName()); //文件名

Log.d(TAG, "[" + Thread.currentThread().getStackTrace()[2].getFileName()
      + "," + Thread.currentThread().getStackTrace()[2].getLineNumber() +
      "]"); //文件名+行號

```

### JAVA 獲取類名，函數名

### 獲取以下獲取方法所在函數的相關信息

- 1.獲取當前函數名：Thread.currentThread().getStackTrace()[1].getMethodName();
- 2.獲取當前類名：Thread.currentThread().getStackTrace()[1].getClassName();
- 3.獲取當前類的文件名：Thread.currentThread().getStackTrace()[1].getFileName();



### 獲取調用方法的所在函數的相關信息
- 1.獲取當前函數名：Thread.currentThread().getStackTrace()[2].getMethodName();
- 2.獲取當前類名：Thread.currentThread().getStackTrace()[2].getClassName();
- 3.獲取當前類的文件名：Thread.currentThread().getStackTrace()[2].getFileName();

### Demo:這是獲取方法
```
public class NameProxy
{

    public static void nowMethod()
    {
        String clazz = Thread.currentThread().getStackTrace()[1].getClassName();
        String method = Thread.currentThread().getStackTrace()[1]
                        .getMethodName();
        System.out.println("class name: " + clazz + " Method Name " + method);
    }

    public static void parentMethod()
    {
        String clazz = Thread.currentThread().getStackTrace()[2].getClassName();
        String method = Thread.currentThread().getStackTrace()[2]
                        .getMethodName();
        System.out.println("class name: " + clazz + " Method Name " + method);
    }

}
```

### Test:

```
public class MethodName
{
    @Test
    public void showMethodName()
    {
        LogProxyName.nowMethod();
        LogProxyName.parentMethod();
    }
}
```
### 顯示結果：
```
1 class name: com.XXX.name.NameProxy Method Name nowMethod
2 class name: com.XXX.name.MethodName Method Name showMethodName
```


### insert_log
- find . -name '*.java' -exec python test.py {} \;
- python test.py  xxxx.java

```
#! /usr/bin/python
# -*- coding: utf-8 -*-
import os
import sys

def InsertToFunc(lines, funcLines, FileName):
    #print lines
    #print funcLines

    for i in funcLines:
        for j in range(i-1, len(lines)):
            if str(lines[j]).find(';') != -1:
                break

            if str(lines[j]).find('{') != -1:
                if str(lines[j]).find('}') != -1:
                    break

                if str(FileName).find('.java') != -1:
                    lines.insert(j+1, '\tSlog.d(Thread.currentThread().getStackTrace()[2].getClassName(), "YAO ["+ Thread.currentThread().getStackTrace()[2].getMethodName() +" | "+Thread.currentThread().getStackTrace()[2].getFileName()+":"+Thread.currentThread().getStackTrace()[2].getLineNumber()+"]");\n');
                elif str(FileName).find('.cpp') != -1 or str(FileName).find('.c') != -1:
                    lines.insert(j+1, '\t::printf ("This is line %d of file %s (function %s)\\n", __LINE__, __FILE__, __func__);\n')

                break
    return lines

def main():
    FileName = sys.argv[1]
    print FileName

    if str(FileName).find('.java') != -1:
        os.system("ctags-exuberant -x " + FileName + " | ack -o -w 'method\s+.*' | ack -o '\d+\s+.*' | ack -o '^\d+\s+' | sort -k 1 -nr > /tmp/test.txt")
    elif str(FileName).find('.cpp') != -1 or str(FileName).find('.c') != -1:
        os.system("ctags-exuberant -x " + FileName + " | ack -o -w 'function\s+.*' | ack -o '\d+\s+.*' | ack -o '^\d+\s+' | sort -k 1 -nr > /tmp/test.txt")
    else:
        print 'noknow type'
        sys.exit(0)

    f = open('/tmp/test.txt', 'r+')
    #funcLines = map(int, funcLines)
    funcLines = [int(i)for i in f.read().splitlines()]
    f.close()

    f = open(str(FileName), 'r+')
    lines = [i for i in f.read().splitlines()]
    f.close()

    InsertListfinish = InsertToFunc(lines, funcLines, FileName)

    newfile = open(str(FileName), "w+")
    newfile.truncate()

    for l in InsertListfinish:
        newfile.write(l + '\n')

    newfile.close()

if __name__=='__main__':
    if len(sys.argv) > 1:
        main()
    else:
        pass
        print "please input python test.py filename"


```
