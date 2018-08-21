# C中調用C++與C++調用C


首先，在C中調用C++：

將 C++ 函數聲明為``extern "C"''（在你的 C++ 代碼裡做這個聲明），然後調用它（在你的 C 或者 C++ 代碼裡調用）。例如：

```cpp
// C++ code:  
extern "C" void f(int);  
void f(int i)  
{  
}
```

然後，你可以這樣使用 f()：

```cpp
/* C code: */  
void f(int);  
void cc(int i)  
{  
    f(i);  
}  
```

當然，這招只適用於非成員函數。如果你想要在 C 裡調用成員函數（包括虛函數），則需要提供一個簡單的包裝（wrapper）。例如：

// C++ code:

```cpp
class C
{
    virtual double f(int);
};

extern "C" double call_C_f(C* p, int i) // wrapper function
{
    return p->f(i);
}
```

然後，你就可以這樣調用 C::f()：

```cpp
/* C code: */
double call_C_f(struct C* p, int i);
void ccc(struct C* p, int i)
{
    double d = call_C_f(p, i);
}
```

如果你想在 C 裡調用重載函數，則必須提供不同名字的包裝，這樣才能被 C 代碼調用。例如：


```cpp
// C++ code:
void f(int);
void f(double);

extern "C" void f_i(int i)
{
    f(i);
}

extern "C" void f_d(double d)
{
    f(d);
}
```

然後，你可以這樣使用每個重載的 f()：


```cpp
/* C code: */
void f_i(int);
void f_d(double);
void cccc(int i, double d)
{
    f_i(i);
    f_d(d);
    /* ... */
}
```

注意，這些技巧也適用於在 C 裡調用 C++ 類庫，即使你不能（或者不想）修改 C++ 頭文件。

該翻譯的文檔Bjarne Stroustrup的原文鏈接地址是：

http://www.research.att.com/~bs/bs_faq2.html#callCpp


## 可以總結為：


- 1、 對於C++中非類的成員函數，可以簡單的在函數聲明前面加extern “C”，通常函數聲明位於頭文件中，當然也可以將聲明和函數定義一起放在cpp中，在沒有聲明的情況下，直接在定義前添加extern “C”也可。

- 2、 對於C++類的成員函數，則需要另外做一個cpp文件，將需要調用的函數進行包裝。

其次，要實現C++中調用C的代碼，具體操作：

對於C中的函數代碼，要麼將C代碼的頭文件進行修改，在其被含入C++代碼時在聲明中加入extern “C”或者在C++代碼中重新聲明一下C函數，重新聲明時添加上extern “C”頭。

通過以上的說明，我明白一點，那就是加extern “C”頭一定是加在C++的代碼文件中才能起作用的。

使用extern "C" 主要是因為C編譯器編譯函數時不帶參數的類型信息，只包含函數的符號名字。如 int foo( float x )C編譯器會將此函數編譯成類似_foo的符號，C連接器只要找到了調用函數的符號，就認為連接成功。而C++編譯器為了實現函數重載，會在編譯時帶上函數的參數信息。如它可以把上面的函數編譯成類似於_foo_float這樣的符號。所以，C調用C++，使用extern "C"則是告訴編譯器依照C的方式來編譯封裝接口，當然接口函數裡面的C++語法還是按C++方式編譯。

如：

```cpp
// C++ Code
extern "C" int foo(int x);
int foo(int x)
{
}
```

這樣,編譯器會將foo函數編譯成類似_foo符號，而不會編譯成類似_foo_int符號

則C可以這樣調用C++函數

```cpp
// C Codeint foo( int x );
void cc(int x)
{
    foo(x);
}
```

如果想調用重載的C++函數，則須封裝單獨的接口共C調用。如


```cpp
// C++ Code
void foo(int x);
void foo(float x);
extern "C" void foo_i(int x)
{
    foo(x);
}

extern "C" void foo_f(float x)
{
    foo(x);
}
```

則C中可這樣調用

```cpp
// C Code
void foo_i(int x);
void foo_f(float x);
void ccc(int x1, float x2)
{
    foo_i(x1);
    foo_f(x2);
    // ...
}

```
而C++調用C，extern "C" 的作用是：讓C++連接器找調用函數的符號時採用C的方式 如：

```cpp
// C Code
void foo( int x );
```

C++這樣調用C函數

```cpp
// C++ Code
extern "C" void foo( int x );
```

就是讓C++連接器能過類似於_foo來查找此函數，而非類似於_foo_int這樣的符號。
