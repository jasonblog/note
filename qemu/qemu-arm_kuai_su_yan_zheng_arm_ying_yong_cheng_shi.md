# qemu-arm 快速驗證arm應用程式


- test.cpp

```cpp
#include <iostream>
#include <memory>
using  namespace std;

class Example
{
public:
    Example() : e(1)
    {
        cout << "Example Constructor..." << endl;
    }
    ~Example()
    {
        cout << "Example Destructor..." << endl;
    }

    int e;
};

int main()
{

    shared_ptr<Example> pInt(new Example());
    cout << (*pInt).e << endl;
    cout << "pInt引用计数: " << pInt.use_count() << endl;

    shared_ptr<Example> pInt2 = pInt;
    cout << "pInt引用计数: " << pInt.use_count() << endl;
    cout << "pInt2引用计数: " << pInt2.use_count() << endl;
}
```

```sh
arm-linux-gnueabi-g++ -std=c++11 -o test test.cpp -static
```

```sh
qemu-arm ./test
```
```sh
Example Constructor...
1
pInt引用计数: 1
pInt引用计数: 2
pInt2引用计数: 2
Example Destructor...
```