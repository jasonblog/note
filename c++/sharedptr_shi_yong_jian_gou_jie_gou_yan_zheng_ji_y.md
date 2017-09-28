# shared_ptr 使用建構解構驗證記憶體回收


```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>
#include <cxxabi.h>
using namespace std;

class test
{
public:
    test()
    {
        cout << "test()" << endl;
    }

    ~test()
    {

        cout << "~test()" << endl;
    }
};

int main()
{
    test *pp = new test();
    shared_ptr<test> p(new test());

    return 0;
}
```