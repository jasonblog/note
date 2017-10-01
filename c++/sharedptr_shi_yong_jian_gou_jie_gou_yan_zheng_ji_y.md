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

```cpp
#include <iostream>
#include <memory>

class Base
{
public:
    virtual void testtest() = 0;
};

class Derived1 : public Base
{
public:
    ~Derived1() {
        std::cout << "~Derived1" << std::endl;
    }

    void testtest() {
        std::cout << "Derived1::testtest()" << std::endl;
    }
};

class Derived2 : public Base
{
public:
    ~Derived2() {
        std::cout << "~Derived2" << std::endl;
    }

    void testtest() {
        std::cout << "Derived2::testtest()" << std::endl;
    }
};

int main(int argc, char* argv[])
{
    std::shared_ptr<Base> b = NULL;

    b = std::shared_ptr<Base>(new Derived1());
    b->testtest();
    b = std::shared_ptr<Base>(new Derived2());
    b->testtest();

    return 0;
}
```