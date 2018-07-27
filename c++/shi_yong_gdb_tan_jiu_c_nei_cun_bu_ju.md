# 使用gdb探究C++内存布局


行打印一个结构体成员
可以执行set print pretty on命令，这样每行只会显示结构体的一名成员，而且还会根据成员的定义层次进行缩进

按照派生类打印对象
```sh
set print object on
```

`set p obj <on/off>`: 
在C++中，如果一个对象指针指向其派生类，如果打开这个选项，GDB会自动按照虚方法调用的规则显示输出，如果关闭这个选项的话，GDB就不管虚函数表了。这个选项默认是off。 使用show print object查看对象选项的设置。

查看虚函数表
在 GDB 中还可以直接查看虚函数表，通过如下设置：`set print vtbl on`

之后执行如下命令查看虚函数表：`info vtbl 对象或者info vtbl 指针或引用所指向或绑定的对象`

- c++filt

GNU提供的从name mangling后的名字来找原函数的方法，如c++filt _ZTV1A，在命令行下运行

- 打印内存的值

gdb中使用“x”命令来打印内存的值，格式为`“x/nfu addr”`。含义为以f格式打印从addr开始的n个长度单元为u的内存值。参数具体含义如下：

```sh
a）n：输出单元的个数。

b）f：是输出格式。比如x是以16进制形式输出，o是以8进制形式输出,a 表示将值当成一个地址打印,i 表示将值当作一条指令打印，等等。

c）u：标明一个单元的长度。b是一个byte，h是两个byte（halfword），w是四个byte（word），g是八个byte（giant word）。
```

打印表达式的值

p 命令可以用来打印一个表达式的值。

使用如下：p/f 表达式

f 代表格式控制符，同上。

###单继承

```cpp
#include <iostream>

class A
{
public:
    int a;
    A(): a(0x1) {}
    virtual void foo()
    {
        std::cout << "A::foo()" << std::endl;
    }
    void bar()
    {
        std::cout << "A::bar()" << std::endl;
    }
};

class B: public A
{
public:
    int b;
    B(): A(), b(0x2) {}
    void foo()
    {
        std::cout << "B::foo()" << std::endl;
    }
};

class C: public B
{
public:
    int c;
    C(): B(), c(0x3) {}
    void foo()
    {
        std::cout << "C::foo()" << std::endl;
    }
};

int main()
{
    A a;
    B b;
    C c;
    B* p = &c;
    p->foo();
    std::cout << sizeof(int) << " " << sizeof(int*) << std::endl;
    return 0;
}
```

运行结果


```sh
C::foo()
4 8
```

`int和int* 都是4个字节是在32bit 平台 64位元是8个字节 `，且p为基类B的指针，指向派生类C，virtual foo()函数运行时多态

对象内存布局

```sh
(gdb) set p pre on
(gdb) p a
$1 = (A) {
  _vptr.A = 0x405188 <vtable for A+8>, 
  a = 1
}

(gdb) p/a &a 
$2 = 0x28ff24
(gdb) p/a &a.a  
$3 = 0x28ff28
(gdb) p sizeof(a)
$4 = 8
(gdb) x/2xw &a
0x28ff24:	0x00405188	0x00000001

(gdb) set p vtbl on
(gdb) info vtbl a
vtable for 'A' @ 0x405188 (subobject @ 0x28ff24):
[0]: 0x403bf8 <A::foo()>
```

- _vptr.A 代表a对象所含有的虚函数表指针，0x405188为第一个虚函数也即foo()的地址，真正虚函数表的起始地址为0x405188 - 8，还会有一些虚函数表头信息

- vptr 总是指向 虚函数表的第一个函数入口
对象a所在的地址为0x28ff24，整个对象占8个字节，其中4个字节为vptr虚函数表指针，4个字节为数据int a

- 虚函数表 vtable for 'A' @0x405188


```sh
(gdb) p b
$5 = (B) {
  <A> = {
    _vptr.A = 0x405194 <vtable for B+8>, 
    a = 1
  }, 
  members of B: 
  b = 2
}
(gdb) p sizeof(b)
$6 = 12
(gdb) p c
$7 = (C) {
  <B> = {
    <A> = {
      _vptr.A = 0x4051a0 <vtable for C+8>, 
      a = 1
    }, 
    members of B: 
    b = 2
  }, 
  members of C: 
  c = 3
}
(gdb) p sizeof(c)
$8 = 16
```

- 如果class B中申明了新的虚函数（比如foo2），class B中依然只有一个虚函数表，只不过会把foo2加入到该表中。此时class A的虚函数表不会包含foo2。


### 多重继承

```cpp
class A{
    int a;
    virtual void foo(){ std::cout << "A::foo()" << std::endl; }
};

class B{
    int b;
    virtual void bar(){ std::cout << "B::bar()" << std::endl; }
};

class C: public A, public B{
    int c;
    void foo(){ std::cout << "C::foo()" << std::endl; }
    void bar(){ std::cout << "C::bar()" << std::endl; }
};
```


对象内存布局


```sh
(gdb) set p pre on
(gdb) p a
$1 = (A) {
  _vptr.A = 0x4051a0 <vtable for A+8>, 
  a = 4201067
}
(gdb) p b
$2 = (B) {
  _vptr.B = 0x4051ac <vtable for B+8>, 
  b = 4200976
}
(gdb) p/a c 
$3 = (C) {
  <A> = {
    _vptr.A = 0x4051b8 <vtable for C+8>, 
    a = 0x75e78cd5 <msvcrt!atan2+431>
  }, 
  <B> = {
    _vptr.B = 0x4051c8 <vtable for C+24>, 
    b = 0xfffffffe
  }, 
  members of C: 
  c = 0x75e61162 <onexit+53>
}
(gdb) p sizeof(c)
$4 = 20
(gdb) x/5aw &c
0x28ff0c:	0x4051b8 <_ZTV1C+8>	0x75e78cd5 <msvcrt!atan2+431>	0x4051c8 <_ZTV1C+24>	0xfffffffe
0x28ff1c:	0x75e61162 <onexit+53>
```

- 数据成员int a, int b, int c都未初始化，此时是UB未定义行为
- 对象c含有两个虚函数表指针_vptr.A和_vptr.B，占用20个字节内存，3个int数据成员，两个虚函数表指针
- 对象c的内存布局为 c: vptr.A | a | vptr.B | b | c

### 虚继承

```cpp
#include <iostream>
using namespace std;

class A
{
    virtual void foo()
    {
        cout << "A::foo()" << endl;
    }
};

class B: virtual public A
{
    virtual void foo()
    {
        cout << "B::foo()" << endl;
    }
};

class C: virtual public A
{
    virtual void foo()
    {
        cout << "C::foo()" << endl;
    }
};

class D: public B, public C
{
    virtual void foo()
    {
        cout << "D::foo()" << endl;
    }
};

int main()
{
	D d;
    return 0;
}
```

对象内存布局


```sh
(gdb) set p pre on
(gdb) p a
$1 = (A) {
  _vptr.A = 0x405238 <vtable for A+8>, 
  a = 4201067
}
(gdb) p b
$2 = (B) {
  <A> = {
    _vptr.A = 0x405258 <vtable for B+28>, 
    a = 4200976
  }, 
  members of B: 
  _vptr.B = 0x405248 <vtable for B+12>, 
  b = 1978012002
}
(gdb) p c
$3 = (C) {
  <A> = {
    _vptr.A = 0x405278 <vtable for C+28>, 
    a = -2079145649
  }, 
  members of C: 
  _vptr.C = 0x405268 <vtable for C+12>, 
  c = 2686916
}
(gdb) p d
$4 = (D) {
  <B> = {
    <A> = {
      _vptr.A = 0x4052a8 <vtable for D+44>, 
      a = 2686704
    }, 
    members of B: 
    _vptr.B = 0x405288 <vtable for D+12>, 
    b = -237228229
  }, 
  <C> = {
    members of C: 
    _vptr.C = 0x405298 <vtable for D+28>, 
    c = 0
  }, 
  members of D: 
  d = 0
}
(gdb) p &d
$5 = (D *) 0x28fee8
```

- 对象内存布局

```sh
a: vptr.A | a
b: vptr.A | a | vptr.B | b
c: vptr.A | a | vptr.C | c
d: vptr.A | a | vptr.B | b | vptr.C | c | d
```

- A *pa = &d;B *pb = &d;C*p c= &d;，都指向d的起始地址&d = 0x28fee8 。假如d类里实现的虚


### 虚函数表里到底有些什么

```cpp
/* vtable.cpp */
class A
{
public:
    int ia;
    virtual void foo()
    {
        cout << "A::foo()" << endl;
    }
    virtual void bar()
    {
        cout << "A::bar()" << endl;
    }
};

class B: public A
{
public:
    int ib;
    virtual void foo()
    {
        cout << "B::foo()" << endl;
    }
};
```

我们可以利用`g++ -fdump-class-hierarchy vtable.cpp`得到class A和class B的vtable（结果在文件vtable.cpp.002t.class里），如下所示：

```sh
Vtable for A
A::_ZTV1A: 4u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1A)
8     (int (*)(...))A::foo
12    (int (*)(...))A::bar
```


