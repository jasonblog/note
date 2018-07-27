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
#endif
    if (child != NULL) {
        child->NewFunc();//Child NewFunc
    }

    return 0;
}
```

备注：

- 1.方式一强制转换方式不安全 

不安全是因为转换的时候无法得知是否转换成功。编译器强制把b当成Child类型去使用。比如说b本来是真的指向Base而不是Child类型那么强制转换后调用Child的NewFunc可能会导致程序崩溃。

- 2.方式二：动态转换方式 

`dynamic_cast`是在运行时去做转换而非编译时，所以它可以给出是否转换成功的信息。如果转换不成功则返回NULL。所以可以判断转换结果是否为NULL来决定是否能使用该指针不会导致程序崩溃

- 转换情况二：

父类不存在虚函数：


```c
#include <iostream>
using namespace std;

typedef void(*Fun)(void);
class Base
{
public:
    Base(int i): a(i)
    {
        b = 3;
        c = 3.14;
    }
    void fun()
    {
        cout << "Base's funciton" << endl;
    }
    void this_fun(Base* b)
    {
        if (b == this) {
            cout << "it is Base's this" << endl;
        }
    }
private:
    int b;
    int a;
    float c;

};

class Derived: public Base
{
public:
    Derived(): Base(2), d(0) {}
    void fun()
    {
        cout << "Derived's function" << endl;
    }
    void this_fun(Derived* d)
    {
        if (d == this) {
            cout << "it is Derived's this" << endl;
        }
    }


private:
    int d;
};

#if 1
int main()
{
    Base b(1);
    Derived* d = (Derived*) &b;

    // 输出： it is Derived‘s 证明：d指针仍然为指向子类
    (*d).this_fun(d);

    // 输出：Base 的成员b的值 3 证明：d指向的仍然为父类对象
    cout << *((int*)d) << endl;

    // 输出：Base 的成员a 的值 1 证明：d指向的仍然为父类对象
    //  *((int *)d) 是将对象指针强转，访问第一个成员 ((int *)b+1)访问对象的第二个成员变量
    cout << *((int*)d + 1) << endl;

    cout << *((float*)d + 2) << endl;

    // d 为指向子类的指针
    d->fun();

    return 0;
}
#else

int main()
{
    Base b(1);
    Derived* d = (Derived*) &b;

	// 输出的:it is Derived's this  证明：this指针仍然为指向子类
    (*d).this_fun(d);

	// 定义一个函数指针 值为d的虚函数
	Fun f1=(Fun)*(int*)*(int *)d;

	// 输出：Base’s function 证明：f1指向父类虚函数表的第一个函数
    f1();

	// 输出：一个一个地址值 17384060
    cout << *((int*)d) << endl; 

	// 输出：Base 的成员b 的值 3 证明：d所指向的对象仍然为父类对象
    cout << *((int*)d + 1) << endl;

	// 输出：Base的成员a 的值 1 证明：d所指向的对象仍然为父类对象
    cout << *((int*)d + 2) << endl; 

	// fun是虚函数，因此寻找虚函数表的第一个
    d->fun();
    return 0;
}
#endif
```

