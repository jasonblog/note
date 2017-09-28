# c++ typeid() 用法 如何正確的顯示 變數型態名稱的亂碼、異常


typeid() 用法在不同的編譯器不一定有正確的結果顯示
可能會出現亂碼，或是無法辨識到底是什麼名稱
如果顯示異常需要一點技巧，主要的手段來自於


```cpp
#include <typeinfo>
#include <cxxabi.h>

int status;
char *realname = abi::__cxa_demangle(typeid(obj).name(), 0, 0, &status);
std::cout << realname;
free(realname);
```

代碼要乾淨一點可以這樣封裝

```cpp
/*****************************************************************
Name : 正確的顯示型態名稱
Date : 2017/04/04
By   : CharlotteHonG
Final: 2017/04/04

http://stackoverflow.com/questions/789402
*****************************************************************/
#include <iostream>
using namespace std;

#include <typeinfo>
#include <cxxabi.h>
class type_name
{
public:
    template <class T>
    type_name(T const t): realname(
            abi::__cxa_demangle(typeid(T).name(), 0, 0, &status))
    {
        cout << realname << endl;
    }
    ~type_name()
    {
        free(realname);
    }
    int status;
    char* realname;
#define type_name(i) type_name(std::move(i));
};
//================================================================
int main(int argc, char const* argv[])
{
    type_name(1.0);
    type_name('a');
    type_name("a");

    void* i;
    type_name(i);

    class A {} a;
    type_name(a);
}
//================================================================
```
