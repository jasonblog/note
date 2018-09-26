# C++靜態成員函數訪問非靜態成員的幾種方法


大家都知道C++中類的成員函數默認都提供了this指針，在非靜態成員函數中當你調用函數的時候，編譯器都會“自動”幫你把這個this指針加到函數形參裡去。當然在C++靈活性下面，類還具備了靜態成員和靜態函數，即


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

此時你的test函數只能去訪問m_staticA成員，而不能去訪問m_a。同學可能會問，這算什麼問題？問題都是在應用場景中才能體現的，我一開始也是不以為意，直到我遇到了回調函數這個煩人的問題我才真正靜下心來去考慮這個知識點。

先簡單說下回調，在座的應該都知道回調的含義，在C中回調主要體現就是回調函數，當然C++中也有仿函數等其他用法，拋開這些，單純在回調函數這個點上我們進行如下討論。

由於C++類的成員函數都隱含了this指針，如果我直接註冊，比如

```cpp
typedef void (A::*FunPtr)();
FunPtr p = A::hello;
p();
```

此時程序會報錯，提示信息是你缺少一個this指針，意味著你要真的想使用p，你必須有一個分配好空間的實例才能來調用

```cpp
typedef void (A::*FunPtr)();
FunPtr p = A::hello;

A a;
A *pA = new A();

(a.*p)();
(pA->*p)();
```

當然，如果僅僅是對C++的類靜態函數進行回調函數註冊，你是不需要考慮this指針的

```cpp
typedef void (A::*FunPtr)();
FunPtr p = A::test;
p();
```

但問題就是，你此時的靜態函數是不能擁有你的成員變量的，看到了吧，問題來了。面對這種需求，我們就真正應該靜下心來好好想想，究竟如何才能讓靜態函數去訪問非靜態成員變量這個問題了。

###方法一：

有一個很取巧的辦法，就是在靜態函數的形參表裡加上實例的地址，也就是

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
這樣在你回調函數的時候，你可以通過這個來讓本身不能訪問成員非靜態變量的靜態函數（太拗口）來訪問非靜態成員變量。

###方法二：

其實這個方法在GLIB中用的很多，就是放上全局變量地址即


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

這種方法我們瞭解就好，全局變量我們並不推薦。

###方法三：

大家都知道靜態成員函數不能訪問非靜態成員，但別忘了，他們可以訪問靜態成員，也就是說，如果我們的這個類是個單例，我們完全可以在創建的時候把this指針賦值給那個靜態成員，然後在靜態成員函數內部就可以放心大膽的使用了。


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

和方法一比較像，但他的方向思想更多的是針對內存塊這個概念，意思就是在靜態函數的形參比加上一個void *的內存首地址，然後在內部做轉換

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


如上，我整理了4種方法，當然方法還有很多，其實繞了這麼大遠路，我們的希望就是不破壞回調函數整潔的函數接口（加上自己的實例指針）而做的妥協，如果你更喜歡通過改變接口或者通過用Java類似的interface方式來實現，那也沒有問題，這裡主要就是提供給大家一個思路，C++確實很靈活，我們要用好這把雙刃劍 ： ）
