# c++子類和父類指針的轉換


- 1.父類指針無法直接調用子類的新函數，需要轉換為子類指針後才能調用。

c++編譯器在編譯的時候是做靜態類型分析，父類指針是否真的指向一個子類類型，編譯器並不會做這個假設。因此用父類的指針去調用子類的函數無非被識別，這裡有一種安全和不安全的方式實現這種轉化。

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
    b->Func();//輸出Child Func
    //b->NewFunc();//錯誤 這個函數不是虛函數，不能多態調用

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

備註：

- 1.方式一強制轉換方式不安全 

不安全是因為轉換的時候無法得知是否轉換成功。編譯器強制把b當成Child類型去使用。比如說b本來是真的指向Base而不是Child類型那麼強制轉換後調用Child的NewFunc可能會導致程序崩潰。

- 2.方式二：動態轉換方式 

`dynamic_cast`是在運行時去做轉換而非編譯時，所以它可以給出是否轉換成功的信息。如果轉換不成功則返回NULL。所以可以判斷轉換結果是否為NULL來決定是否能使用該指針不會導致程序崩潰

- 轉換情況二：

父類不存在虛函數：


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

    // 輸出： it is Derived‘s 證明：d指針仍然為指向子類
    (*d).this_fun(d);

    // 輸出：Base 的成員b的值 3 證明：d指向的仍然為父類對象
    cout << *((int*)d) << endl;

    // 輸出：Base 的成員a 的值 1 證明：d指向的仍然為父類對象
    //  *((int *)d) 是將對象指針強轉，訪問第一個成員 ((int *)b+1)訪問對象的第二個成員變量
    cout << *((int*)d + 1) << endl;

    cout << *((float*)d + 2) << endl;

    // d 為指向子類的指針
    d->fun();

    return 0;
}
#else

int main()
{
    Base b(1);
    Derived* d = (Derived*) &b;

	// 輸出的:it is Derived's this  證明：this指針仍然為指向子類
    (*d).this_fun(d);

	// 定義一個函數指針 值為d的虛函數
	Fun f1=(Fun)*(int*)*(int *)d;

	// 輸出：Base’s function 證明：f1指向父類虛函數表的第一個函數
    f1();

	// 輸出：一個一個地址值 17384060
    cout << *((int*)d) << endl; 

	// 輸出：Base 的成員b 的值 3 證明：d所指向的對象仍然為父類對象
    cout << *((int*)d + 1) << endl;

	// 輸出：Base的成員a 的值 1 證明：d所指向的對象仍然為父類對象
    cout << *((int*)d + 2) << endl; 

	// fun是虛函數，因此尋找虛函數表的第一個
    d->fun();
    return 0;
}
#endif
```

