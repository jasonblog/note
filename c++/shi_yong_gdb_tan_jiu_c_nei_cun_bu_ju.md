# 使用gdb探究C++內存佈局


行打印一個結構體成員
可以執行set print pretty on命令，這樣每行只會顯示結構體的一名成員，而且還會根據成員的定義層次進行縮進

按照派生類打印對象
```sh
set print object on
```

`set p obj <on/off>`: 
在C++中，如果一個對象指針指向其派生類，如果打開這個選項，GDB會自動按照虛方法調用的規則顯示輸出，如果關閉這個選項的話，GDB就不管虛函數表了。這個選項默認是off。 使用show print object查看對象選項的設置。

查看虛函數表
在 GDB 中還可以直接查看虛函數表，通過如下設置：`set print vtbl on`

之後執行如下命令查看虛函數表：`info vtbl 對象或者info vtbl 指針或引用所指向或綁定的對象`

- c++filt

GNU提供的從name mangling後的名字來找原函數的方法，如c++filt _ZTV1A，在命令行下運行

- 打印內存的值

gdb中使用“x”命令來打印內存的值，格式為`“x/nfu addr”`。含義為以f格式打印從addr開始的n個長度單元為u的內存值。參數具體含義如下：

```sh
a）n：輸出單元的個數。

b）f：是輸出格式。比如x是以16進制形式輸出，o是以8進制形式輸出,a 表示將值當成一個地址打印,i 表示將值當作一條指令打印，等等。

c）u：標明一個單元的長度。b是一個byte，h是兩個byte（halfword），w是四個byte（word），g是八個byte（giant word）。
```

打印表達式的值

p 命令可以用來打印一個表達式的值。

使用如下：p/f 表達式

f 代表格式控制符，同上。

###單繼承

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

運行結果


```sh
C::foo()
4 8
```

`int和int* 都是4個字節是在32bit 平臺 64位元是8個字節 `，且p為基類B的指針，指向派生類C，virtual foo()函數運行時多態

對象內存佈局

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

- _vptr.A 代表a對象所含有的虛函數表指針，0x405188為第一個虛函數也即foo()的地址，真正虛函數表的起始地址為0x405188 - 8，還會有一些虛函數表頭信息

- vptr 總是指向 虛函數表的第一個函數入口
對象a所在的地址為0x28ff24，整個對象佔8個字節，其中4個字節為vptr虛函數表指針，4個字節為數據int a

- 虛函數表 vtable for 'A' @0x405188


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

- 如果class B中申明瞭新的虛函數（比如foo2），class B中依然只有一個虛函數表，只不過會把foo2加入到該表中。此時class A的虛函數表不會包含foo2。


### 多重繼承

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


對象內存佈局


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

- 數據成員int a, int b, int c都未初始化，此時是UB未定義行為
- 對象c含有兩個虛函數表指針_vptr.A和_vptr.B，佔用20個字節內存，3個int數據成員，兩個虛函數表指針
- 對象c的內存佈局為 c: vptr.A | a | vptr.B | b | c

### 虛繼承

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

對象內存佈局


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

- 對象內存佈局

```sh
a: vptr.A | a
b: vptr.A | a | vptr.B | b
c: vptr.A | a | vptr.C | c
d: vptr.A | a | vptr.B | b | vptr.C | c | d
```

- A *pa = &d;B *pb = &d;C*p c= &d;，都指向d的起始地址&d = 0x28fee8 。假如d類裡實現的虛


### 虛函數表裡到底有些什麼

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

我們可以利用`g++ -fdump-class-hierarchy vtable.cpp`得到class A和class B的vtable（結果在文件vtable.cpp.002t.class裡），如下所示：

```sh
Vtable for A
A::_ZTV1A: 4u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1A)
8     (int (*)(...))A::foo
12    (int (*)(...))A::bar
```


