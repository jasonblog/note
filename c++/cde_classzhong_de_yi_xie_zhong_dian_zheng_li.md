# C++的Class中的一些重點整理


`class與struct有很明顯的的差別是，class可以定義member function，但struct不行。另外，class預設的member權限是private，而struct預設則是public。`

以下是我看螞蟻書的重點整理。另外，也有參考這篇。

- friend function: 簡單來說就是你在class裡面定義了一個friend function，這個function是在class的外面，同時這個function可以修改private data。

- const function: 在function後面如果加了const的話，就表示它不會修改任何的menber data，所以，const object不能夠呼叫non const function。

以下講權限範圍，如果沒有定義的話，則預設表示private。其實跟java很類似，差別在於java沒有friend class，而且java的預設權限是package scope。

- private: 只有自己或者是friend class看得到(其實就是可不可以直接存取的意思啦)。
- protected: 只有自己，friend class或者子類別才可以看得到。
- public: 任何可以看到這個class的地方都可以使用。


 
建構子跟解構子也很重要，以前不太清楚解構子可以幹嘛，只知道物件被回收的時候解構子會自動被呼叫。 以下這個例子我覺得很好，應該是說當物件內部有動態配置的時候，可以一起把動態

```cpp
// example on constructors and destructors rect area: 12
#include <iostream.h> rectb area: 30
class CRectangle
{
    int* width, *height;
public:
    //overloading
    CRectangle();
    CRectangle(int, int);
    ~CRectangle();
    //實作內容定義在class以內的有機會被編譯器視為inline function，編譯器會自己作判斷
    //inline function會直接將程式碼展開來，與macro非常類似
    //程式碼很短，只有return statement，用inline較有效率
    int area(void)
    {
        return (*width * *height);
    }
};

CRectangle::CRectangle()
{
    width = new int;
    height = new int;
    *width = 5;
    *height = 6;
}

CRectangle::CRectangle(int a, int b)
{
    width = new int;
    height = new int;
    *width = a;
    *height = b;
}

CRectangle::~CRectangle()
{
    delete width;
    delete height;
}

int main()
{
    //若不傳參數，則不需要括弧，java則需要
    CRectangle rect(3, 4), rectb(5, 6), rectc;
    cout << "rect area: " << rect.area() << endl;
    cout << "rectb area: " << rectb.area() << endl;
    return 0;
}
```

實際上,當我們定義一個 class 而沒有定義建構子的時候,編譯器會自動假設兩 個重載的建構子 (預設建構子"default constructor" 和複製建構子"copy constructor")，又與java類似，只是在java中我沒聽過copy constructor。


```cpp
class CExample
{
public:
    //沒有定義建構子
    int a, b, c;
    void multiply(int n, int m)
    {
        a = n;
        b = m;
        c = a * b;
    };
};
```

因此defalut constructor以及copy constructor會被自動定義，copy constructor是一個只有一個參數的建構子,該參數是這個 class 的一個物件,這個函數的功能是將被傳入的物件(object)的所有`非靜態` (non-static)成員變數的值都複製給自身這個 object。


```cpp
//default constructor
CExample::CExample() { };
//copy constructor
//一定要pass by reference，否則會導致遞迴呼叫copy constructor而形成無窮迴圈。
//若是有用到指標，則copy constructor預設只會複製指標的address，所以一定要視情況改寫
CExample::CExample(const CExample& rv)
{
    a = rv.a;
    b = rv.b;
    c = rv.c;
}
```

為什麼copy constructor一定要pass by reference?，這個問題我大一的時候有想過，去問了ptt的版友之後終於恍然大悟。 問題點出在`pass by value`的時候並不是直接用等號來作assignment，而是會先呼叫copy constructor。

假設有一個function foo長這樣

```cpp
//pass by reference
foo(Bar bar)
```
當有人要呼叫foo()

```cpp
Bar mybar;
foo(mybar)
```

進入 foo 之前, 因為是 pass by value, 所以要生成一個 myBar 的 copy. 要怎樣生成呢? compiler 會利用 copy constructor 生成一個 myBar 的複製品, 然後供 foo() 裡面使用。

要是Bar這個class的copy constructor 是 pass-by-value 的話, 比如

```cpp
Bar myBar;
Bar barCopy(myBar);
```

生成 barCopy 的時候, 因為 myBar 是 pass by value, 所以 compiler 要生成一個 myBar 的複製品 (假設叫 tmpBarA) 才傳進去生成 barCopy.

但生成 tmpBarA, 也是利用 copy ctor, 即是 compiler 是在做 Bar tmpBarA(myBar); 由於是 pass-by-val, 那麼要生成 tmpBarA, 其實傳進去的是 myBar 的一個 複製品 (tmpBarB).

要生成 tmpBarB......(遞迴呼叫，無窮迴圈)，`而如果是copy by reference，則不會呼叫copy constructor。`

## 繼承

繼承的語法如下。

```cpp
//public表示子類別繼承過來的最低的權限
//例如如果把public改成protected，如果父類別有一個public member，則在子類別子類別會自動被改成protected
class derived_class_name: public base_class_name;
```

例如

```cpp
class daughter: protected mother;
```

protected 將使得 daughter 從 mother 處繼承的成員其最低訪問限制為 protected。也就是說, 原來mother中的所有public成員到daughter中將會成為protected 成員。 最常用的繼承限制除了 public 外就是 private , `它被用來將父類別完全封裝起來`, 因為在這種情況下,除了子類別自身外,其它任何的繼承子類別的子類別(孫子)都不能訪問那些從父類別(爺爺)繼承而來的成員。不過大多數情況下繼承都是使用 public。

子類別從父類別繼承了public及protected成員，但以下三個例外。

- `建構子 Constructor 和解構子 destructor`
- `operator=() 成員`
- `friends`


雖然父類別的建構子和解構子沒有被繼承,但是當一個子類別的 object 被生成或銷毀的時候,其父類別的預設建構子 (即,沒有任何參數的建構子)和解構子總是被自動調用的。

如果父類別沒有預設建構子,或你希望當子類別生成新的 object 時,父類別的某個重載的建構子被調用,你需要在子類別的每一個建構子的定義中指定它:

```cpp
class mother
{
public:
    mother()
    {
        cout << "mother: no parameters\n";
    }
    mother(int a)
    {
        cout << "mother: int parameter\n";
    }
}
class daughter : public mother
{
public:
    daughter(int a)
    //daughter建構子會自動呼叫mother()
    {
        cout << "daughter: int parameter\n\n";
    }
};
class son : public mother
{
public:
    //son建構子會呼叫mother(a)
    son(int a) : mother(a)
    {
        cout << "son: int parameter\n\n";
    }
};
int main()
{
    daughter cynthia(1);
    son daniel(1);
    return 0;
}
```

##多型
多型的好處就是一個interface可以操控不同的物件，要達成多型的手段就是繼承，讓父類別的指標可以指到子類別的物件。

而一般來說，如果沒有用到virtual function，則父類別的指標只能夠呼叫那些被子類別所繼承的方法。

例如，
```cpp
class CPolygon
{
protected:
    int width, height;
public:
    void set_values(int a, int b)
    {
        width = a;
        height = b;
    }
};
class CRectangle: public CPolygon
{
public:
    int area(void)
    {
        return (width * height);
    }
};
class CTriangle: public CPolygon
{
public:
    int area(void)
    {
        return (width * height / 2);
    }
};

int main()
{
    CRectangle rect;
    CTriangle trgl;
    //父類別指向子類別
    CPolygon* ppoly1 = &rect;
    CPolygon* ppoly2 = &trgl;
    //但只能夠呼叫父類別自己有定義的
    ppoly1->set_values(4, 5);
    ppoly2->set_values(4, 5);
    //例如父類別沒定義area()，就不能呼叫
    cout << rect.area() << endl;
    cout << trgl.area() << endl;
    return 0;
}
```

如果要讓父類別呼叫area，要怎麼做呢？方法就是在父類別中定義virtual function。


```cpp
class CPolygon
{
protected:
    int width, height;
public:
    void set_values(int a, int b)
    {
        width = a;
        height = b;
    }
    virtual int area(void)
    {
        return (0);
    }
};

class CRectangle: public CPolygon
{
public:
    int area(void)
    {
        return (width * height);
    }
};
class CTriangle: public CPolygon
{
public:
    int area(void)
    {
        return (width * height / 2);
    }
};
int main()
{
    CRectangle rect;
    CTriangle trgl;
    CPolygon poly;
    CPolygon* ppoly1 = &rect;
    CPolygon* ppoly2 = &trgl;
    CPolygon* ppoly3 = &poly;
    ppoly1->set_values(4, 5);
    ppoly2->set_values(4, 5);
    ppoly3->set_values(4, 5);
    //呼叫的是rect的area()
    cout << ppoly1->area() << endl;
    //呼叫的是trgl的area()
    cout << ppoly2->area() << endl;
    //呼叫的是poly的area()
    cout << ppoly3->area() << endl;
    return 0;
}
```



因此,`關鍵字 virtual` 的作用就是在當使用父類別的指標的時候,使子類別中與父類別同名的成員在適當的時候被調用,如前面例子中所示。

這個又叫做dynamic binding。在執行階段決定要執行哪個實體的方法。

`其實這邊跟java稍微不同，因為在java裡面只要子類別有覆寫父類別的方法的話，就會自動去呼叫子類別自己所定義的方法，不用再特別去寫virtual關鍵字。`

如果我們的子類別覆寫了父類別的virtual function，但是名字卻打錯了，這樣就還是只會呼叫父類別的方法。 要怎麼避免呢？答案就是採用pure virtual function，若子類別沒有正確override，則編譯失敗。 `在java中，則是用abstract function來做到，所有包含abstract function的class又稱為interface。` 例如，


```cpp
// abstract class CPolygon
class CPolygon
{
protected:
    int width, height;
public:
    void set_values(int a, int b)
    {
        width = a;
        height = b;
    }
    virtual int area(void) = 0;
};
```

抽象父類別的最大不同是它不能夠有實例(物件),但我們可以定義指向它的指標。`(這跟java的interface很像)`

以下是一個多型的完整的例子，


```cpp
class CPolygon
{
protected:
    int width, height;
public:
    void set_values(int a, int b)
    {
        width = a;
        height = b;
    }
    virtual int area(void) = 0;
    //我們可以寫一個 CPolygon 的成員函數,使得它可以將函數
    //area()的結果列印到螢幕上,而不必考慮具體是為哪一個子類別。
    //加上了printarea，記住,this 代表正在被程式執行的這一個物件的指標。
    void printarea(void)
    {
        cout << this->area() << endl;
    }
};
class CRectangle: public CPolygon
{
public:
    int area(void)
    {
        return (width * height);
    }
};
class CTriangle: public CPolygon
{
public:
    int area(void)
    {
        return (width * height / 2);
    }
};
int main()
{
    CRectangle rect;
    CTriangle trgl;
    CPolygon* ppoly1 = &rect;
    CPolygon* ppoly2 = &trgl;
    ppoly1->set_values(4, 5);
    ppoly2->set_values(4, 5);
    ppoly1->printarea();
    ppoly2->printarea();
    return 0;
}
```