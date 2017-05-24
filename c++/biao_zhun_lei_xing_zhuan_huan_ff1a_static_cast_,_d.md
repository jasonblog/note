# 標準類型轉換：static_cast, dynamic_cast, reinterpret_cast, and const_cast


##static_cast
可用於轉換基底類別指標為衍生類別指標，也可用於傳統的資料型態轉換。
舉例來說，在指定的動作時，如果右邊的數值型態比左邊的數值型態型態長度大時，超出可儲存範圍的部份會被自動消去，例如將浮點數指定給整數變數，則小數的部份會被自動消去，例子如下，程式會顯示3而不是3.14：


```cpp
int num = 0;
double number = 3.14;
num = number;
cout << num;
```

`由於失去的精度`，在編譯時編譯器會提出警訊： `[Warning] converting to `int' from `double'` 如果確定這個動作是您想要的，那麼您可以使用顯式型態轉換（Explicit type conversion）或稱之為強制轉型（Cast），例如：

```cpp
int num = 0; 
double number = 3.14; 
num = static_cast<int>(number);
```

在兩個整數型態相除時，您也可以進行型態轉換，將其中一個型態轉換至double型態再進行運算，下例會顯示3.3333：

```cpp
int number = 10;
cout << static_cast<double>(number) / 3;
```

static_cast是Standard C++新增加的轉型語法，在這之前顯式型態轉換可以使用以下的舊語法，基於向後相容，Standard C++仍支援這種舊語法，但鼓勵使用新風格的語法

```cpp
int number = 10;
cout << (double) number/ 3;
```

##dynamic_cast
使用static_cast（甚至是傳統的C轉型方式）將基底類別指標轉換為衍生類別指標，這種轉型方式稱為強制轉型，但是在執行時期使用強制轉型有危險性，因為編譯器無法得知轉型是否正確，執行時期真正指向的物件型態是未知的，透過簡單的檢查是避免錯誤的一種方式：


```cpp
if(typeid(*base) == typeid(Derived1)) {
    Derived1 *derived1 = static_cast<derived1*>(base);
    derived1->showOne();
}
```

為了支援執行時期的型態轉換動作，C++提供了dynamic_cast用來將一個基底類別的指標轉型至衍生類別指標，稱之為「安全向下轉型」（Safe downcasting），它在執行時期進行型態轉換動作，首先會確定轉換目標與來源是否屬同一個類別階層，接著才真正進行轉換的動作，檢驗動作在執行時期完成，如果是一個指標，則轉換成功時傳回位址，失敗的話會傳回0，如果是參考的話，轉換失敗會丟出bad_cast例外。 可以將執行時期型態資訊（RTTI）中的showWho()函式如下修改：


```cpp
void showWho(Base *base) {
    base->foo();
     
    if(Derived1 *derived1 = dynamic_cast>derived1*<(base)) {
        derived1->showOne();
    }
    else if(Derived2 *derived2 = static_cast>derived2*<(base)) {
        derived2->showTwo();    
    } 
}
```

由於dynamic_cast轉換失敗的話會傳回0，因而運算的結果不可使用，必須先行對轉換結果作檢查才可以，如上例所示。 如果使用參考的話，dynamic_cast在轉換失敗之後會丟出bad_cast例外，所以您必須使用try...catch來處理例外，例如：

```cpp
#include <iostream>
#include <typeinfo>
using namespace std;

class Base
{
public:
    virtual void foo() = 0;
};

class Derived1 : public Base
{
public:
    void foo()
    {
        cout << "Derived1" << endl;
    }

    void showOne()
    {
        cout << "Yes! It's Derived1." << endl;
    }
};

class Derived2 : public Base
{
public:
    void foo()
    {
        cout << "Derived2" << endl;
    }

    void showTwo()
    {
        cout << "Yes! It's Derived2." << endl;
    }
};

void showWho(Base& base)
{
    try {
        Derived1 derived1 = dynamic_cast<Derived1&>(base);
        derived1.showOne();
    } catch (bad_cast) {
        cout << "bad_cast 轉型失敗" << endl;
    }
}

int main()
{
    Derived1 derived1;
    Derived2 derived2;

    showWho(derived1);
    showWho(derived2);

    return 0;
}
```

執行結果(使用dynamic_cast)：

```sh
Yes! It's Derived1.
bad_cast 轉型失敗
```

執行結果(使用static_cast)：

```sh
Yes! It's Derived1.
Yes! It's Derived1.
```
##reinterpret_cast
用於將一種型態的指標轉換為另一種型態的指標，例如將`char*`轉換為`int*`

```cpp
#include <iostream>
using namespace std;
 
int main() {
    int* i;
    char* str = "test";
 
    i = reinterpret_cast<int>(str);
    cout << i << endl;
 
    return 0;
} 
```

(可能)執行結果：

```sh
134514704
```


## const_cast
const_cast用於一些特殊場合可以覆寫變數的const屬性，利用cast後的指標就可以更改變數的內部。
使用格式：

```cpp
const_cast<常量(const)型態(指標) / 非常量型態(指標)>( 非常量變數(指標) / 常量變數(指標) );
```

使用範例：

```cpp
#include <iostream>
using namespace std;

void foo(const int*);

int main()
{
    int var = 10;

    cout << var << endl;

    foo(&var);

    cout << var << endl;

    return 0;
}

void foo(const int* p)
{
    int* v = const_cast<int*>(p);
    *v = 20;
}
```

執行結果：

```sh
10
20
```