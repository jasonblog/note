# 利用函數指針實現父類函數調用子類函數


##父子類關係
對於繼承關係中的父類和子類，我們可以說子類是父類的一種，子類繼承了父類的屬性和行為。因此，子類可以訪問父類的所有非私有成員。相反，父類一般情況下是不能訪問子類成員的。然而，我們可以通過一些方法間接的實現父類訪問子類，即父類函數訪問子類函數。

###方法一

利用多態機制，一個指向子類的父類指針或引用，當調用被子類重寫的虛函數時，實際上調用的是子類函數，這是通過多態的方式來實現父類調用子類，該方法需要一個引用或者指針調用虛函數來實現。如下面所示：


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
base->fun();    //該結果輸出為： "Derived::fun()"
```


上面簡單的例子就是通過多態實現的父類調用子類成員函數，這裡面必須要有虛函數，並且在子類裡面重寫。

###方法二
通過函數指針同樣可以實現父類函數訪問子類函數，示例代碼如下：


```cpp
class Base
{
public:
    typedef void (Base::*pfn)();

    void CallFunction()
    {
        if (fn != NULL) {
            (this->*fn)();  //調用Derived類中的fun()函數
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

//這裡調用的是父類的成員函數，該函數通過函數指針調用了子類的普通成員函數，
調用結果輸出："Derived::fun be called in Base function!"，即父類成員函數調用了子類成員函數。
```

