# C++静态成员函数访问非静态成员的几种方法


大家都知道C++中类的成员函数默认都提供了this指针，在非静态成员函数中当你调用函数的时候，编译器都会“自动”帮你把这个this指针加到函数形参里去。当然在C++灵活性下面，类还具备了静态成员和静态函数，即


```cpp
class A
{
public:
    static void test()
    {
        m_staticA += 1;
    }
private:
    static int m_staticA;
    int m_a
};
```

此时你的test函数只能去访问m_staticA成员，而不能去访问m_a。同学可能会问，这算什么问题？问题都是在应用场景中才能体现的，我一开始也是不以为意，直到我遇到了回调函数这个烦人的问题我才真正静下心来去考虑这个知识点。

先简单说下回调，在座的应该都知道回调的含义，在C中回调主要体现就是回调函数，当然C++中也有仿函数等其他用法，抛开这些，单纯在回调函数这个点上我们进行如下讨论。

由于C++类的成员函数都隐含了this指针，如果我直接注册，比如

```cpp
typedef void (A::*FunPtr)();
FunPtr p = A::hello;
p();
```

此时程序会报错，提示信息是你缺少一个this指针，意味着你要真的想使用p，你必须有一个分配好空间的实例才能来调用

```cpp
typedef void (A::*FunPtr)();
FunPtr p = A::hello;

A a;
A *pA = new A();

(a.*p)();
(pA->*p)();
```

当然，如果仅仅是对C++的类静态函数进行回调函数注册，你是不需要考虑this指针的

```cpp
typedef void (A::*FunPtr)();
FunPtr p = A::test;
p();
```

但问题就是，你此时的静态函数是不能拥有你的成员变量的，看到了吧，问题来了。面对这种需求，我们就真正应该静下心来好好想想，究竟如何才能让静态函数去访问非静态成员变量这个问题了。

###方法一：

有一个很取巧的办法，就是在静态函数的形参表里加上实例的地址，也就是

```cpp
class A
{
public:
    static void test(A *a)
    {
        a->m_a += 1;
    }
    void hello()
    {
    }
private:
    static int m_staticA;
    int m_a
};

```
这样在你回调函数的时候，你可以通过这个来让本身不能访问成员非静态变量的静态函数（太拗口）来访问非静态成员变量。

###方法二：

其实这个方法在GLIB中用的很多，就是放上全局变量地址即


```cpp
A g_a;

class A
{
public:
    static void test()
    {
        g_a.m_a += 1;
    }
    void hello()
    {
    }
private:
    static int m_staticA;
    int m_a
};
```

这种方法我们了解就好，全局变量我们并不推荐。

###方法三：

大家都知道静态成员函数不能访问非静态成员，但别忘了，他们可以访问静态成员，也就是说，如果我们的这个类是个单例，我们完全可以在创建的时候把this指针赋值给那个静态成员，然后在静态成员函数内部就可以放心大胆的使用了。


```cpp
#include <iostream>
using namespace std;

class A
{
public:
    A()
    {
        m_a = 100;
        m_gA = this;
    }
    static void test()
    {
        m_gA->m_a += 1;
    }
    void show()
    {
        cout << m_a;
    }
    static int m_staticA;
    static A* m_gA;
private:
    int m_a;
};

A* A::m_gA = nullptr;
int A::m_staticA = 0;

int main()
{
    A a;
    A::test();
    a.show();
    return 0;
}
```

###方法四：

和方法一比较像，但他的方向思想更多的是针对内存块这个概念，意思就是在静态函数的形参比加上一个void *的内存首地址，然后在内部做转换

```cpp
class A
{
public:
    static void test(void *pData)
    {
        A *a = (A *)pData;
        a->m_a += 1;
    }
    void hello()
    {
    }
private:
    static int m_staticA;
    int m_a
};

A a;
test(&a);

```


如上，我整理了4种方法，当然方法还有很多，其实绕了这么大远路，我们的希望就是不破坏回调函数整洁的函数接口（加上自己的实例指针）而做的妥协，如果你更喜欢通过改变接口或者通过用Java类似的interface方式来实现，那也没有问题，这里主要就是提供给大家一个思路，C++确实很灵活，我们要用好这把双刃剑 ： ）
