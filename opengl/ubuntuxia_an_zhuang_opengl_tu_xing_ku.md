# Ubuntu下安裝OpenGL圖形庫


## 安裝基本編譯環境

```sh
sudo apt-get install build-essential
```

## 安裝OpenGL Library

```sh
sudo apt-get install libgl1-mesa-dev
```

## 安裝OpenGL Utilities[1]

OpenGL Utilities 是一組建構於OpenGL Library 之上的工具組，提供許多很方便的函式，使OpenGL 更強大且更容易使用。

```sh
sudo apt-get install libglu1-mesa-dev
```

## 安裝OpenGL Utility Toolkit[2]


OpenGL Utility Toolkit 是建立在 OpenGL Utilities 上面的工具箱，除了強化了 OpenGL Utilities 的不足之外，也增加了 OpenGL 對於視窗界面支援

```sh
sudo apt-get install freeglut3-dev
```

## 安裝glew[3]
glew是一個跨平臺的C++庫，是一個OpenGL圖形接口擴展庫

```sh
sudo apt-get install libglew1.8 libglew-dev
```

##安裝glx[4]
glx是linux下OpenGL的X Window System接口擴展庫，它允許通過x調用OpenGL庫

```sh
sudo apt-get install libgl1-mesa-glx
```

##補充有的也可能需要安裝Xmu
Xmu即X11 miscellaneous utility library（X11實用工具庫）

```sh
sudo apt-get install libxmu-dev
```

##說明：
OpenGL不同頭文件及庫的說明

###[1] OpenGL Utilities

glu是實用庫，包含有43個函數，函數名的前綴為glu。Glu 為了減輕繁重的編程工作，封裝了OpenGL函數，Glu函數通過調用核心庫的函數，為開發者提供相對簡單的用法，實現一些較為複雜的操作。

###[2] OpenGL Utility Toolkit

glut是實用工具庫，基本上是用於做窗口界面的，並且是跨平臺（所以有時你喜歡做簡單的demo的話，可以光用glut就ok了）

###[3] glew

glew是一個跨平臺的C++擴展庫，基於OpenGL圖形接口。使用OpenGL的朋友都知道，window目前只支持OpenGL1.1的涵數，但 OpenGL現在都發展到2.0以上了，要使用這些OpenGL的高級特性，就必須下載最新的擴展，另外，不同的顯卡公司，也會發布一些只有自家顯卡才支 持的擴展函數，你要想用這數涵數，不得不去尋找最新的glext.h,有了GLEW擴展庫，你就再也不用為找不到函數的接口而煩惱，因為GLEW能自動識 別你的平臺所支持的全部OpenGL高級擴展涵數。也就是說，只要包含一個glew.h頭文件，你就能使用gl,glu,glext,wgl,glx的全 部函數。GLEW支持目前流行的各種操作系統（including Windows, Linux, Mac OS X, FreeBSD, Irix, and Solaris）。

###[4] glx

glx:OpenGL extension for X.

對於X窗口系統，它所使用的的OpenGL擴展（GLX）是作為OpenGL的一個附件提供的，所有的GLX函數都使用前綴glX。

glx是OpenGL Extension to the X Window System的縮寫。它作為x的擴展，是x協議和X server的一部分，已經包含在X server的代碼中了。GLX提供了x window system使用的OpenGL接口，允許通過x調用OpenGL庫。OpenGL 在使用時，需要與一個實際的窗口系統關聯起來。在不同平臺上有不同的機制以關聯窗口系統，在Windows上是WGL，在Linux上是GLX，在Apple OS上是AGL等。

###[5] 頭文件

`<GL/gl.h>`：OpenGL所使用的函數和常量聲明。

`<GL/glu.h>`：OpenGL Utility Toolkit所使用的函數和常量聲明。GLU庫屬於OpenGL標準的一部分。（以下各種庫則不屬於）

`<GL/glut.h>`：OpenGL Utility Toolkit所使用的函數和常量聲明。目前許多OpenGL教程使用這個庫來編寫演示程序。一些編譯系統可能不直接提供這個庫（例如VC系列），需要單獨下載安裝。這個頭文件自動包含了`<GL/gl.h>`和`<GL/glu.h>`，編程時不必再次包含它們。

`<GL/glaux.h>`：GLAUX（OpenGL輔助庫）所使用的函數和常量聲明。這個庫的功能大致與`<GL/glut.h>`類似，這個庫提供了創建窗口，處理鍵盤和鼠標事件，設置調色板等OpenGL本身不提供，但在編寫OpenGL程序時又經常用到的功能。目前這個庫已經過時，只有比較少的編譯環境中有提供，例如VC系列。在VC系列編譯器中，使用這個頭文件之前必須使用#include `<windows.h>`或者具有類似功能的頭文件。

`<GL/glext.h>`：擴展頭文件。因為微軟公司對OpenGL的支持不太積極，VC系列編譯器雖然有`<GL/gl.h>`這個頭文件，但是裡面只有OpenGL 1.1版本中所規定的內容，而沒有OpenGL 1.2及其以後版本。對當前的計算機配置而言，幾乎都支持OpenGL 1.4版本，更高的則到1.5, 2.0, 2.1，而VC無法直接使用這些功能。為了解決這一問題，就有了`<GL/glext.h>`頭文件。這個頭文件提供了高版本OpenGL所需要的各種常數聲明以及函數指針聲明。

`<GL/wglext.h>`：擴展頭文件。與`<GL/glext.h>`類似，但這個頭文件中只提供適用於Windows系統的各種OpenGL擴展所使用的函數和常量，不適用於其它操作系統。

"glee.h"：GLEE開源庫的頭文件。它的出現是因為`<GL/glext.h>`雖然可以使用高版本的OpenGL函數，但是使用的形式不太方便。GLEE庫則讓高版本的OpenGL函數與其它OpenGL函數在使用上同樣方便。需要注意的是，這個頭文件與`<GL/gl.h>`是衝突的，在包含"glee.h"之前，不應該包含`<GL/gl.h>`。

```c
#include <GL/glut.h>
#include "glee.h" // 錯誤，因為glut.h中含有gl.h，它與glee.h衝突
                          // 但是如果把兩個include順序交換，則正確
```

`"glos.h"`：不明，雖然這個也時常見到，可能是與系統相關的各種功能，也可能只是自己編寫的一個文件。


## 測試文件

```c
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

void init();
void display();

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(300, 300);
    glutCreateWindow("OpenGL 3D View");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glOrtho(-5, 5, -5, 5, 5, 15);
    glMatrixMode(GL_MODELVIEW);
    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 0, 0);
    glutWireTeapot(3);
    glFlush();
}
```

## 編譯命令

- C

```sh

gcc example.c -o example.out -lGL -lGLU -lglut
```

- C++


```sh
g++ -lGL -lGLU -glut -lGLEW example.cpp -o example
```

- Makefile


```sh
INCLUDE = -I/usr/X11R6/include/
LIBDIR  = -L/usr/X11R6/lib

FLAGS = -Wall
CC = g++                                  # change to gcc if using C
CFLAGS = $(FLAGS) $(INCLUDE)
LIBS =  -lglut -lGL -lGLU -lGLEW -lm

All: your_app                             # change your_app.

your_app: your_app.o
    $(CC) $(CFLAGS) -o $@ $(LIBDIR) $< $(LIBS) # The initial white space is a tab
```
或者

```sh
LDFLAGS=-lglut -lGL -lGLU -lGLEW -lm
all: your_app
```

## 參考：
(http://wiki.ubuntu-tw.org/index.php?title=Howto_Install_OpenGL_Development_Environment)<br>
(http://hi.baidu.com/niujingqian/item/58afbbcc6305c215b77a24db)<br>
(http://baike.baidu.com/view/1994675.htm?fr=aladdin)<br>
(http://stackoverflow.com/questions/859501/learning-opengl-in-ubuntu)<br>