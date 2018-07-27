# 利用函数指针实现父类函数调用子类函数


##父子类关系
对于继承关系中的父类和子类，我们可以说子类是父类的一种，子类继承了父类的属性和行为。因此，子类可以访问父类的所有非私有成员。相反，父类一般情况下是不能访问子类成员的。然而，我们可以通过一些方法间接的实现父类访问子类，即父类函数访问子类函数。

###方法一

利用多态机制，一个指向子类的父类指针或引用，当调用被子类重写的虚函数时，实际上调用的是子类函数，这是通过多态的方式来实现父类调用子类，该方法需要一个引用或者指针调用虚函数来实现。如下面所示：


```cpp
Class Base {
public:
    virtual void fun()
    {
        std::cout << "Base::fun()" << std::endl;
    }
};

class Derived : public Base
{
public:
    virtual void fun()
    {
        std::cout << "Derived::fun()" << std::endl;
    }
};

Base* base = new Derived;
base->fun();    //该结果输出为： "Derived::fun()"
```


上面简单的例子就是通过多态实现的父类调用子类成员函数，这里面必须要有虚函数，并且在子类里面重写。

###方法二
通过函数指针同样可以实现父类函数访问子类函数，示例代码如下：


```cpp
class Base
{
public:
    typedef void (Base::*pfn)();

    void CallFunction()
    {
        if (fn != NULL) {
            (this->*fn)();  //调用Derived类中的fun()函数
        }
    }
    void SetPfn(pfn fn)
    {
        this->fn = fn;
    }

private:
    pfn fn;

};

class Derived : public Base
{
public:
    void Init()
    {
        SetPfn((pfn)&Derived::fun);
    }

    void fun()
    {
        std::cout << "Derived::fun be called in Base function!" << std::endl;
    }

};

Derived derived;
derived.Init();
derived.CallFunction();

//这里调用的是父类的成员函数，该函数通过函数指针调用了子类的普通成员函数，
调用结果输出："Derived::fun be called in Base function!"，即父类成员函数调用了子类成员函数。
```

