# C++源码的调用图生成


##主要思路

利用`gcc/g++的-finstrument-functions`的注入选项，
得到每个函数的调用地址信息，生成一个`trace文件`，
然后利用`addr2line和c++filt将函数名及其所在源码位置`从地址中解析出来，
从而得到程序的`Call Stack`，
然后用`pygraphviz`画出来

使用示例

比如我现在有`A.hpp、B.hpp、C.hpp、ABCTest.cpp`这几个文件，
我想看他们的`Call Graph`

- A.hpp

```cpp
#ifndef _A_H_
#define _A_H_
#include "B.hpp"
#include "C.hpp"

struct A {
    B b;

    A() {}
    int Aone()
    {
        ATwo();
        b.BTwo();
        return 1;
    }

    int ATwo()
    {

        AThree();
        return 1;
    }

    int AThree()
    {
        return 1;
    }
};
#endif // _A_H_
```
- B.hpp

```cpp
#ifndef _B_H_
#define _B_H_
#include "C.hpp"

struct B {
    C c;
    B() {}
    int Bone()
    {
        BTwo();
        c.CTwo();
        return 1;
    }

    int BTwo()
    {

        BThree();
        return 1;
    }

    int BThree()
    {
        return 1;
    }
};
#endif // _B_H_

```

- C.hpp

```cpp
#ifndef _C_H_
#define _C_H_

struct C {

    C() {}
    int Cone()
    {
        CTwo();
        CThree();
        return 1;
    }

    int CTwo()
    {

        CThree();
        return 1;
    }

    int CThree()
    {
        return 1;
    }
};
#endif // _C_H_
```

- ABCTest.cpp

```cpp
#include "A.hpp"

int main(int argc, char *argv[])
{
    A a;
    a.Aone();
    return 0;
}
```

然后按下面编译(instrument.c在上面github地址中可以下载，用于注入地址信息）：

```sh
g++ -g -finstrument-functions -O0 instrument.c ABCTest.cpp -o test
```

然后运行程序，得到trace.txt
输入shell命令./test
最后
输入shell命令
```sh
python CallGraph.py trace.txt test
```


图上标注含义:

- 绿线表示程序启动后的第一次调用

- 红线表示进入当前上下文的最后一次调用

- 每一条线表示一次调用，#符号后面的数字是序号，at XXX表示该次调用发生在这个文件（文件路径在框上方）的第几行

- 在圆圈里，XXX:YYY，YYY是调用的函数名，XXX表示这个函数是在该文件的第几行被定义的

##获取C/C++调用关系

利用-finstrument-functions编译选项，
可以让编译器在每个函数的开头和结尾注入__cyg_profile_func_enter和 __cyg_profile_func_exit
这两个函数的实现由用户定义

在本例中，只用到__cyg_profile_func_enter，定义在instrument.c中，
其函数原型如下:
void __cyg_profile_func_enter (void *this_fn, void *call_site);
其中this_fn为 被调用的地址，call_site为 调用方的地址

显然，假如我们把所有的 调用方和被调用方的地址 都打印出来，
就可以得到一张完整的运行时Call Graph

因此，我们的instrument.c实现如下：


```c
/* Function prototypes with attributes */
void main_constructor( void )
    __attribute__ ((no_instrument_function, constructor));

void main_destructor( void )
    __attribute__ ((no_instrument_function, destructor));

void __cyg_profile_func_enter( void *, void * )
    __attribute__ ((no_instrument_function));

void __cyg_profile_func_exit( void *, void * )
    __attribute__ ((no_instrument_function));

static FILE *fp;

void main_constructor( void )
{
  fp = fopen( "trace.txt", "w" );
  if (fp == NULL) exit(-1);
}

void main_deconstructor( void )
{
  fclose( fp );
}

void __cyg_profile_func_enter( void *this_fn, void *call_site )
{
    /* fprintf(fp, "E %p %p\n", (int *)this_fn, (int *)call_site); */
    fprintf(fp, "%p %p\n", (int *)this_fn, (int *)call_site);
}
```
弹出一张Call Graph