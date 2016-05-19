# Linux C++编译及 静态/动态 链接库 笔记


##1、C++文件的编译 
-E  预处理， 将#include的文件展开等  <br>
-S 生成汇编文件  <br>
-c  生成object文件  <br>
gcc XX.o  -o XX   链接，，输出文件名为XX  <br>

-O1,  -O2,  -O3  优化代码 ，-O3级别最高，   -O1为默认  <br>
-g 及   -ggdb  等，，生成调试信息  <br>

-fPIC 及 -fpic ，  生成动态链接库时使用，， 前者在多平台通用，但程序运行速度慢  <br>

-M ,  -MM,   输出 文件的依赖关系(比如各种.h)文件（写makefile时可以使用）  <br>

-std=c++14（或-std=gnu++14）  -std=c++11（或-std=gnu++11）启用 <br>

c++14/11的信息，其中使用gnu++14会启用gcc的扩展  ： <br>
https://gcc.gnu.org/projects/cxx0x.html <br>
https://gcc.gnu.org/projects/cxx1y.html <br>


一般直接用-c 生成object文件，再多个object文件链接一下即可  <br>
或者不指定参数，直接从 多个.cpp 文件生成可执行文件 

-Wall  输出尽可能多的警告  <br>

-I 头文件路径  <br>
-L 库文件路径（静态、动态）  <br>
-l 库文件（无需包含扩展名.so 或.a ）（但是我自己在Ubuntu14.04.1中直接使用XX.so即可，而不是-lXX.so） 

##2、静态链接库、动态链接库的编译 
Program Library HOWTO：http://www.dwheeler.com/program-library/ <br>

C++ dlopen mini HOWTOhttp://www.tldp.org/HOWTO/C++-dlopen/ <br>
这篇文章中讲了由于C++的Name Mangling，，需要 extern "C"等封装才能正常使用`fopen函数调用C++函数的问题` <br>

很多网上的教程会用到 -l 选项指定库文件！！！但是我试的时候直接指定库文件全名就行！！！（具体见makefile内容） 


nm  查看exe、.o、.so、 .a文件中的符号信息，比如C++函数名被编译器mangle之后的名称 

nm XX.so | c++filt  可以查看函数的原始名称 <br>
关于mangling更多请参见http://blog.csdn.net/chenxintao/article/details/7585483 

关于库文件搜索路径的问题，没详细实验，在Ubuntu下貌似当前路径也可以~ 这个，用到再试了 
只是简单记一下`LD_LIBRARY_PATH` 的修改方式：http://baike.baidu.com/view/1270749.htm 

不多说了，上代码（完整代码见附件）： 

###库文件： 
Sharedlib.h代码
```c
#pragma once  
int plus(int a,int b);  
```

Sharedlib.cpp代码 

```c
#include "sharedLib.h"  
int plus(int a,int b){  
    return a+b;  
}  
```

###采用静态链接库： 
Main_link_a.cpp代码

```cpp
#include "sharedLib.h"  
#include <iostream>  
  
int main(void)
{  
    std::cout<<"Running "<<__FILE__<<std::endl;  
    int p=plus(2,3);  
    std::cout<<"The sum is "<<p<<std::endl<<std::endl;  
}  
```

###采用动态链接库（源文件和采用静态链接库完全相同）： 

Main_link_so.cpp代码
```cpp
#include "sharedLib.h"  
#include <iostream>  
  
int main(void)
{  
    std::cout<<"Running "<<__FILE__<<std::endl;  
    int p=plus(2,3);  
    std::cout<<"The sum is "<<p<<std::endl<<std::endl;  
}  
```
### 采用动态加载.so文件(fopen)： 

Main_dynamicload.cpp代码

```cpp
#include <iostream>
#include <dlfcn.h>
#include "sharedLib.h"

typedef int (*F)(int, int);
using FF = int (*)(int, int);
int checkDLLState(void* dp)
{
    const char* dlsym_error = dlerror();

    if (dlsym_error) {
        std::cerr << "Cannot load symbol : " << dlsym_error << std::endl;
        dlclose(dp);
        return 1;
    }

    return 0;
}

int main(void)
{
    std::cout << "Running " << __FILE__ << std::endl;
    const char* dllName = "./sharedLib.so";
    // reset errors
    dlerror();
    void* dp = dlopen(dllName, RTLD_LAZY);

    if (checkDLLState(dp)) {
        return -1;
    }

    std::cout << "dp = " << (unsigned long long)(dp) << std::endl;
    F f;
    f = (F)(dlsym(dp,
                  "_Z4plusii")); // if do not want to use this name, add extern "C" before the function "plus"

    if (checkDLLState(dp)) {
        return -1;
    }

    int sum = f(1, 2);
    std::cout << "The sum is " << sum << std::endl << std::endl;
    dlclose(dp);
}
```

###makefile:  
看不懂的搜下 “跟我一起写makefile”，，我也是简单学了一下而已 
终端内输入  make runall就可以执行程序了 
Makefile代码 

```sh
all: main_link_a main_link_so main_dynamicLoad  
  
runall: all  
    echo "now beggin running all executations"  
    ./main_link_a   
    ./main_link_so   
    ./main_dynamicLoad  
  
#use static library  
main_link_a: main_link_a.cpp sharedLib.a  
    g++ -Wall -o main_link_a main_link_a.cpp  ./sharedLib.a   
  
sharedLib.a: sharedLib.cpp sharedLib.h  
    g++ -Wall -static -c -o sharedLib.o sharedLib.cpp  
    ar rcs sharedLib.a sharedLib.o  
  
#use dynamic library  
main_link_so: main_link_so.cpp sharedLib.so  
    g++ -Wall -o main_link_so main_link_so.cpp  ./sharedLib.so -ldl  
  
sharedLib.so: sharedLib.cpp sharedLib.h  
    g++ -Wall -shared -fPIC -o sharedLib.so sharedLib.cpp  
  
#use fopen,  dynamic load .so libarary, use function by name  
main_dynamicLoad: main_dynamicLoad.cpp sharedLib.so  
    g++ -Wall -std=c++11 -o main_dynamicLoad main_dynamicLoad.cpp ./sharedLib.so -ldl #-ldl for dynamic link  
  
#view mangled c++ function name in sharedLib.so  
viewFuncName: sharedLib.so  
    echo viewing mangled function name  
    nm sharedLib.so #view mangled function name  
    echo   
    echo viewing demangled function name  
    nm sharedLib.so | c++filt #view demangled name  

clean:   
    -rm *.so *.o *.a #the minus sign at the beginning means the commnand after this one will be executed even error occurs.  
    -rm main_link_a main_link_so main_dynamicLoad  
    -rm *~
```