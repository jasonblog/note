# C中调用C++与C++调用C


首先，在C中调用C++：

将 C++ 函数声明为``extern "C"''（在你的 C++ 代码里做这个声明），然后调用它（在你的 C 或者 C++ 代码里调用）。例如：

```cpp
// C++ code:  
extern "C" void f(int);  
void f(int i)  
{  
}
```

然后，你可以这样使用 f()：

```cpp
/* C code: */  
void f(int);  
void cc(int i)  
{  
    f(i);  
}  
```

当然，这招只适用于非成员函数。如果你想要在 C 里调用成员函数（包括虚函数），则需要提供一个简单的包装（wrapper）。例如：

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

然后，你就可以这样调用 C::f()：

```cpp
/* C code: */
double call_C_f(struct C* p, int i);
void ccc(struct C* p, int i)
{
    double d = call_C_f(p, i);
}
```

如果你想在 C 里调用重载函数，则必须提供不同名字的包装，这样才能被 C 代码调用。例如：


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

然后，你可以这样使用每个重载的 f()：


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

注意，这些技巧也适用于在 C 里调用 C++ 类库，即使你不能（或者不想）修改 C++ 头文件。

该翻译的文档Bjarne Stroustrup的原文链接地址是：

http://www.research.att.com/~bs/bs_faq2.html#callCpp


## 可以总结为：


- 1、 对于C++中非类的成员函数，可以简单的在函数声明前面加extern “C”，通常函数声明位于头文件中，当然也可以将声明和函数定义一起放在cpp中，在没有声明的情况下，直接在定义前添加extern “C”也可。

- 2、 对于C++类的成员函数，则需要另外做一个cpp文件，将需要调用的函数进行包装。

其次，要实现C++中调用C的代码，具体操作：

对于C中的函数代码，要么将C代码的头文件进行修改，在其被含入C++代码时在声明中加入extern “C”或者在C++代码中重新声明一下C函数，重新声明时添加上extern “C”头。

通过以上的说明，我明白一点，那就是加extern “C”头一定是加在C++的代码文件中才能起作用的。

使用extern "C" 主要是因为C编译器编译函数时不带参数的类型信息，只包含函数的符号名字。如 int foo( float x )C编译器会将此函数编译成类似_foo的符号，C连接器只要找到了调用函数的符号，就认为连接成功。而C++编译器为了实现函数重载，会在编译时带上函数的参数信息。如它可以把上面的函数编译成类似于_foo_float这样的符号。所以，C调用C++，使用extern "C"则是告诉编译器依照C的方式来编译封装接口，当然接口函数里面的C++语法还是按C++方式编译。

如：

```cpp
// C++ Code
extern "C" int foo(int x);
int foo(int x)
{
}
```

这样,编译器会将foo函数编译成类似_foo符号，而不会编译成类似_foo_int符号

则C可以这样调用C++函数

```cpp
// C Codeint foo( int x );
void cc(int x)
{
    foo(x);
}
```

如果想调用重载的C++函数，则须封装单独的接口共C调用。如


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

则C中可这样调用

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
而C++调用C，extern "C" 的作用是：让C++连接器找调用函数的符号时采用C的方式 如：

```cpp
// C Code
void foo( int x );
```

C++这样调用C函数

```cpp
// C++ Code
extern "C" void foo( int x );
```

就是让C++连接器能过类似于_foo来查找此函数，而非类似于_foo_int这样的符号。
