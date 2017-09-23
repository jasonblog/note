# 父子類別之間函數呼叫

```cpp
#include <iostream>

template <typename T>
class BaseInterface
{
protected:
    virtual void toImplement(T& t) = 0;
};

template <typename T>
class Base : public BaseInterface<T>
{
    bool m_useImplemented;

public:
    explicit Base(bool useImplemented) : m_useImplemented(useImplemented) { 
    }

    virtual void toImplement(T& t) = 0;

    void tryUsingImplemented(T& t)
    {
        std::cout << "Doing stuff for Base " << t << std::endl;
        if (m_useImplemented) {
            toImplement(t);
        }
    }
};

template <typename T>
class Derived : public Base<T>
{
public:
    explicit Derived(bool useImplemented) : Base<T>(useImplemented) {}

// protected:
    // 3. implementing toImplement in Derived
    void toImplement(T& t)
    {
        std::cout << "Doing stuff for Derived " << t << std::endl;
    }

    void call_tryUsingImplemented(T& t) {
        Base<T>::tryUsingImplemented(t);
    }
};

int main(int argc, char *argv[])
{
    int i = 10;
    Derived<int> a(true);
    a.toImplement(i);

    double d = 12.456;
    Derived<double> b(true);
    b.toImplement(d);
    b.call_tryUsingImplemented(d);
    
    return 0;
}
```

父類別函數要 call 子類別函數必須要使用 virtual 
`virtual void toImplement(T& t) = 0;`

子類別call父類別用Base::functionName()
`Base<T>::tryUsingImplemented(t);`


