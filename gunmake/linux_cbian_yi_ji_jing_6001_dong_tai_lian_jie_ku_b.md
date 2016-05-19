# Linux C++編譯及 靜態/動態 鏈接庫 筆記


##1、C++文件的編譯 
-E  預處理， 將#include的文件展開等  <br>
-S 生成彙編文件  <br>
-c  生成object文件  <br>
gcc XX.o  -o XX   鏈接，，輸出文件名為XX  <br>

-O1,  -O2,  -O3  優化代碼 ，-O3級別最高，   -O1為默認  <br>
-g 及   -ggdb  等，，生成調試信息  <br>

-fPIC 及 -fpic ，  生成動態鏈接庫時使用，， 前者在多平臺通用，但程序運行速度慢  <br>

-M ,  -MM,   輸出 文件的依賴關係(比如各種.h)文件（寫makefile時可以使用）  <br>

-std=c++14（或-std=gnu++14）  -std=c++11（或-std=gnu++11）啟用 <br>

c++14/11的信息，其中使用gnu++14會啟用gcc的擴展  ： <br>
https://gcc.gnu.org/projects/cxx0x.html <br>
https://gcc.gnu.org/projects/cxx1y.html <br>


一般直接用-c 生成object文件，再多個object文件鏈接一下即可  <br>
或者不指定參數，直接從 多個.cpp 文件生成可執行文件 

-Wall  輸出儘可能多的警告  <br>

-I 頭文件路徑  <br>
-L 庫文件路徑（靜態、動態）  <br>
-l 庫文件（無需包含擴展名.so 或.a ）（但是我自己在Ubuntu14.04.1中直接使用XX.so即可，而不是-lXX.so） 

##2、靜態鏈接庫、動態鏈接庫的編譯 
Program Library HOWTO：http://www.dwheeler.com/program-library/ <br>

C++ dlopen mini HOWTOhttp://www.tldp.org/HOWTO/C++-dlopen/ <br>
這篇文章中講了由於C++的Name Mangling，，需要 extern "C"等封裝才能正常使用`fopen函數調用C++函數的問題` <br>

很多網上的教程會用到 -l 選項指定庫文件！！！但是我試的時候直接指定庫文件全名就行！！！（具體見makefile內容） 


nm  查看exe、.o、.so、 .a文件中的符號信息，比如C++函數名被編譯器mangle之後的名稱 

nm XX.so | c++filt  可以查看函數的原始名稱 <br>
關於mangling更多請參見http://blog.csdn.net/chenxintao/article/details/7585483 

關於庫文件搜索路徑的問題，沒詳細實驗，在Ubuntu下貌似當前路徑也可以~ 這個，用到再試了 
只是簡單記一下`LD_LIBRARY_PATH` 的修改方式：http://baike.baidu.com/view/1270749.htm 

不多說了，上代碼（完整代碼見附件）： 

###庫文件： 
Sharedlib.h代碼
```c
#pragma once  
int plus(int a,int b);  
```

Sharedlib.cpp代碼 

```c
#include "sharedLib.h"  
int plus(int a,int b){  
    return a+b;  
}  
```

###採用靜態鏈接庫： 
Main_link_a.cpp代碼

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

###採用動態鏈接庫（源文件和採用靜態鏈接庫完全相同）： 

Main_link_so.cpp代碼
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
### 採用動態加載.so文件(fopen)： 

Main_dynamicload.cpp代碼

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
看不懂的搜下 “跟我一起寫makefile”，，我也是簡單學了一下而已 
終端內輸入  make runall就可以執行程序了 
Makefile代碼 

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