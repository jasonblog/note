# 在pthread的線程函數中，使用類的非靜態成員函數來執行


大家知道，pthread_create()函數的線程函數必須是`靜態的函數`，`以標準的__cdecl的方式調用`的，而`C++的成員函數是以__thiscall的方式調用`的，`相當於一個普通函數有一個默認的const ClassType* this參數`。

為數據封裝的需要，我常常把線程函數封裝在一個類的內部，定義一個類的`私有靜態成員函數`來作為pthread的線程函數，通常如下：

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

可以看見，創建線程的時候，把this指針作為線程的參數傳入，然後在線程函數中用一個變量pThis來代替this指針。這樣的做法比較醜陋，而且每次都要手動強制轉換，很麻煩。

後來看了一些模版的資料，找到了一個簡便些的技巧：

```cpp
template <typename TYPE, void (TYPE::*_RunThread)() >
void* _thread_t(void* param)
{
    TYPE* This = (TYPE*)param;
    // 多 wrapper 一層讓_RunThread 不需要是static 函數
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


函數模版不單可以替換類型本身，還能替換類型的成員函數。

##注意：
1、名稱只能是_RunThread，不能在指定模版參數的時候修改；<br>
2、_RunThread只能是public的，除非把_thread_t定義到MyClass的內部。