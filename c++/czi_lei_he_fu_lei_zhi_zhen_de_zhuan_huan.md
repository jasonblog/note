# c++子类和父类指针的转换


- 1.父类指针无法直接调用子类的新函数，需要转换为子类指针后才能调用。

c++编译器在编译的时候是做静态类型分析，父类指针是否真的指向一个子类类型，编译器并不会做这个假设。因此用父类的指针去调用子类的函数无非被识别，这里有一种安全和不安全的方式实现这种转化。

case1：不安全的方式

```cpp
#include <iostream>
using namespace std;

class Base
{
public:
    void virtual Func()
    {
        cout << "Base Func\n";
    }
};

class Child : public Base
{
public:
    void Func()
    {
        cout << "Child Func\n";
    }
    void NewFunc()
    {
        cout << "Child Newfunc\n";
    }

};

int main()
{
    Base* b = new Child;
    b->Func();//输出Child Func
    //b->NewFunc();//错误 这个函数不是虚函数，不能多态调用

#if 1
    Child* child = (Child*)b; //方式一不安全
    child->NewFunc();//Child NewFunc
#else

    Child* child = dynamic_cast<Child*>(b); //方式二安全方式

    if (child != NULL) {
        child->NewFunc();//Child NewFunc
    }
#endif

    return 0;
}
```

备注：

- 1.方式一强制转换方式不安全 

不安全是因为转换的时候无法得知是否转换成功。编译器强制把b当成Child类型去使用。比如说b本来是真的指向Base而不是Child类型那么强制转换后调用Child的NewFunc可能会导致程序崩溃。

- 2.方式二：动态转换方式

dynamic_cast是在运行时去做转换而非编译时，所以它可以给出是否转换成功的信息。如果转换不成功则返回NULL。所以可以判断转换结果是否为NULL来决定是否能使用该指针不会导致程序崩溃

转换情况二：

父类不存在虚函数：
