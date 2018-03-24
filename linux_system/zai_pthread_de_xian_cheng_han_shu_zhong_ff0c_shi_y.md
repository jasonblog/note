# 在pthread的线程函数中，使用类的非静态成员函数来执行


大家知道，pthread_create()函数的线程函数必须是`静态的函数`，`以标准的__cdecl的方式调用`的，而`C++的成员函数是以__thiscall的方式调用`的，`相当于一个普通函数有一个默认的const ClassType* this参数`。

为数据封装的需要，我常常把线程函数封装在一个类的内部，定义一个类的`私有静态成员函数`来作为pthread的线程函数，通常如下：

```cpp
class MyClass
{
public:
    MyClass();
private:
    pthread_t tid;
    static void* _thread_t(void* param);
};

void* MyClass::_thread_t(void* param)
{
    MyClass* pThis = (MyClass*)param;
    pThis->DoSomeThing();
    //...
    return NULL;
}

MyClass::MyClass()
{
    pthread_create(&tid, NULL, _thread_t, this);
}
```

可以看见，创建线程的时候，把this指针作为线程的参数传入，然后在线程函数中用一个变量pThis来代替this指针。这样的做法比较丑陋，而且每次都要手动强制转换，很麻烦。

后来看了一些模版的资料，找到了一个简便些的技巧：

```cpp
template <typename TYPE, void (TYPE::*_RunThread)() >
void* _thread_t(void* param)
{
    TYPE* This = (TYPE*)param;
    This->_RunThread();
    return NULL;
}

class MyClass
{
public:
    MyClass();
    void _RunThread();
private:
    pthread_t tid;

};

void MyClass::_RunThread()
{
    this->DoSomeThing();
    //...
}

MyClass::MyClass()
{
    pthread_create(&tid, NULL, _thread_t<MyClass, &MyClass::_RunThread>, this);
}
```


函数模版不单可以替换类型本身，还能替换类型的成员函数。

##注意：
1、名称只能是_RunThread，不能在指定模版参数的时候修改；<br>
2、_RunThread只能是public的，除非把_thread_t定义到MyClass的内部。