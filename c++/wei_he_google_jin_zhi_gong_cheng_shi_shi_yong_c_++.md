# 為何 Google 禁止工程師使用 C++ static object?

網路流傳的 Google C++ Style Guide 裡提到：Google 禁止旗下工程師使用 static object，僅允許使用 static Plain Old Data(ints, chars, floats, pointers)，或 POD array/struct。可惜的是文中缺少實際範例指出這樣做的危害，剛好小弟最近踩到這個地雷，來跟大家分享一下心得 :)

這個例子用到了三個自定 classes：
- class MyArray
- class Foo
- class Bar

這三個 classes 都極其平凡又短小，沒有用到任何進階技巧，就連初學者都看得懂。我們先對三個 classes 稍做解釋，然後再來看這三個 classes 混在一起時會產生何種化學變化。


class MyArray & Foo



```cpp
//foo.h
#ifndef FOO_H
#define FOO_H
 
#include <vector>
 
class MyArray
{
    int size_;
    int *ary_;
public:
    MyArray();
    ~MyArray();
    void resize(int n);
    int size();
};
 
 
class Foo
{
public:
    Foo();
    ~Foo();
    void doSomething();
     
private:
    typedef std::vector<int> IntVector;
    static int cnt_;
    static IntVector v_;
    static MyArray obj_;
};
 
#endif
```

```cpp
//foo.cpp
#include <stdio.h>
#include "foo.h"

Foo::IntVector Foo::v_;
int Foo::cnt_;
MyArray Foo::obj_;

MyArray::MyArray():
    size_(0),
    ary_(0)
{

}

MyArray::~MyArray()
{
    if (ary_) {
        delete[]ary_;
    }
}

void MyArray::resize(int n)
{
    if (ary_) {
        delete[]ary_;
    }

    ary_ = new int[n];
    size_ = n;
}

int MyArray::size()
{
    return size_;
}

Foo::Foo()
{
    printf("enter Foo::Foo()\n");
    printf("    v_.size() = %d\n", v_.size());
    printf("    cnt_ = %d\n", ++cnt_);
    printf("    obj_.size() = %d\n", obj_.size());

    if (v_.size() == 0) {
        v_.resize(100);
    }

    if (obj_.size() == 0) {
        obj_.resize(200);
    }

    printf("exit Foo::Foo()\n");
}

Foo::~Foo()
{

}

void Foo::doSomething()
{
    printf("enter Foo::dosSomething\n");
    printf("    v_.size() = %d\n", v_.size());
    printf("    cnt_ = %d\n", cnt_);
    printf("    obj_.size() = %d\n", obj_.size());
    printf("exit Foo::dosSomething\n");
}
```

##class MyArray
由上面程式可以很清楚的看到，MyArray object 剛建立時兩個 member data: size_, ary_ 都是 0 (foo.cpp:10-11)，直到呼叫 MyArray::resize() 才會動態產生 int array (foo.cpp:22-29)，MyArray 的用處只是用來比對 std::vector<int>，看看兇手是否與 template 有關。

##class Foo
三個 member data 皆為 static：

- cnt_: POD
- v_: std::vector<int>
- obj_: MyArray

重點在於 Foo constructor，他會對三個 static member data 分別進行 (foo.cpp:36-49)：

- cnt_: 每次呼叫 constructor 就 + 1
- v_: 如果 v_.size() == 0，則 resize(100)
- obj_: 如果 obj_.size() == 0，則 resize(200)

以大部分人對 static object 的了解，無論 Foo constructor 被呼叫多少次，v_, obj_ 應該只會 resize 一次(這裡可是 single thread 喔)，真的是這樣嗎？請各位先保持耐心繼續往下看。

## class Bar

```cpp
//bar.h
#ifndef BAR_H
#define BAR_H
 
class Bar
{
public:    
    Bar();
    ~Bar();    
};
 
#endif
```

```cpp
//bar.cpp
#include <stdio.h>
#include "bar.h"
#include "foo.h"
 
static Bar bar;
 
Bar::Bar()
{
    printf("enter Bar::Bar()\n");
    Foo().doSomething();
    printf("exit Bar::Bar()\n");
}
     
Bar::~Bar()
{
     
}
```

class Bar 更是平凡無奇，他只是在 bar.cpp 內建立了一個 static Bar object (bar.cpp:6)，在 Bar constructor 內建立了 Foo 臨時物件，並且呼叫了 Foo::doSomething() 列印 Foo static member data 資訊 (bar.cpp:11)。 

##見證奇蹟的時刻

我們建立一個 main.cpp，觀察進入 int main() 前跟後的行為...

```cpp
#include <stdio.h>
#include "foo.h"
 
int main()
{
    printf("enter main()\n");
    Foo().doSomething();
    printf("exit main()\n");
    return 0;
}
```

按照以下步驟編譯連結(測試環境為 Ubuntu 14.04)：

```sh
g++ -c foo.cpp
g++ -c bar.cpp
g++ -c main.cpp
g++ bar.o foo.o main.o -o mytest
./mytest 的執行結果：
```

