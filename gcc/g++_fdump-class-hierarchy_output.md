# G++ fdump-class-hierarchy output

- layout.cpp

```cpp
class VBase
{
    virtual void corge();
    int j;
};

class SBase1
{
    virtual void grault();
    int k;
};

class SBase2
{
    virtual void grault();
    int k;
};

class SBase3
{
    void grault();
    int k;
};

class Class : public SBase1, SBase2, SBase3, virtual VBase
{
public:
    void bar();
    virtual void baz();
    // virtual member function templates not allowed, thinking about memory
    // layout and vtables will tell you why
    // template<typename T>
    // virtual void quux();
private:
    int i;
    char c;
public:
    float f;
private:
    double d;
public:
    short s;
};

class Derived : public Class
{
    virtual void qux();
};

int main()
{
    return sizeof(Derived);
}

```

```sh
g++ -fdump-class-hierarchy layout.cpp
```

- layout.cpp.002t.class

```sh
Vtable for VBase
VBase::_ZTV5VBase: 3u entries
0     (int (*)(...))0
8     (int (*)(...))(& _ZTI5VBase)
16    (int (*)(...))VBase::corge

Class VBase
   size=16 align=8
   base size=12 base align=8
VBase (0x0x7f693a34a4e0) 0
    vptr=((& VBase::_ZTV5VBase) + 16u)

Vtable for SBase1
SBase1::_ZTV6SBase1: 3u entries
0     (int (*)(...))0
8     (int (*)(...))(& _ZTI6SBase1)
16    (int (*)(...))SBase1::grault

Class SBase1
   size=16 align=8
   base size=12 base align=8
SBase1 (0x0x7f693a34a540) 0
    vptr=((& SBase1::_ZTV6SBase1) + 16u)

Vtable for SBase2
SBase2::_ZTV6SBase2: 3u entries
0     (int (*)(...))0
8     (int (*)(...))(& _ZTI6SBase2)
16    (int (*)(...))SBase2::grault

Class SBase2
   size=16 align=8
   base size=12 base align=8
SBase2 (0x0x7f693a34a5a0) 0
    vptr=((& SBase2::_ZTV6SBase2) + 16u)

Class SBase3
   size=4 align=4
   base size=4 base align=4
SBase3 (0x0x7f693a34a600) 0

Vtable for Class
Class::_ZTV5Class: 12u entries
0     64u
8     (int (*)(...))0
16    (int (*)(...))(& _ZTI5Class)
24    (int (*)(...))SBase1::grault
32    (int (*)(...))Class::baz
40    (int (*)(...))-16
48    (int (*)(...))(& _ZTI5Class)
56    (int (*)(...))SBase2::grault
64    0u
72    (int (*)(...))-64
80    (int (*)(...))(& _ZTI5Class)
88    (int (*)(...))VBase::corge

VTT for Class
Class::_ZTT5Class: 2u entries
0     ((& Class::_ZTV5Class) + 24u)
8     ((& Class::_ZTV5Class) + 88u)

Class Class
   size=80 align=8
   base size=58 base align=8
Class (0x0x7f693a352200) 0
    vptridx=0u vptr=((& Class::_ZTV5Class) + 24u)
  SBase1 (0x0x7f693a34a660) 0
      primary-for Class (0x0x7f693a352200)
  SBase2 (0x0x7f693a34a6c0) 16
      vptr=((& Class::_ZTV5Class) + 56u)
  SBase3 (0x0x7f693a34a720) 28
  VBase (0x0x7f693a34a780) 64 virtual
      vptridx=8u vbaseoffset=-24 vptr=((& Class::_ZTV5Class) + 88u)

Vtable for Derived
Derived::_ZTV7Derived: 13u entries
0     64u
8     (int (*)(...))0
16    (int (*)(...))(& _ZTI7Derived)
24    (int (*)(...))SBase1::grault
32    (int (*)(...))Class::baz
40    (int (*)(...))Derived::qux
48    (int (*)(...))-16
56    (int (*)(...))(& _ZTI7Derived)
64    (int (*)(...))SBase2::grault
72    0u
80    (int (*)(...))-64
88    (int (*)(...))(& _ZTI7Derived)
96    (int (*)(...))VBase::corge

Construction vtable for Class (0x0x7f693a352380 instance) in Derived
Derived::_ZTC7Derived0_5Class: 9u entries
0     64u
8     (int (*)(...))0
16    (int (*)(...))(& _ZTI5Class)
24    (int (*)(...))SBase1::grault
32    (int (*)(...))Class::baz
40    0u
48    (int (*)(...))-64
56    (int (*)(...))(& _ZTI5Class)
64    (int (*)(...))VBase::corge

VTT for Derived
Derived::_ZTT7Derived: 4u entries
0     ((& Derived::_ZTV7Derived) + 24u)
8     ((& Derived::_ZTC7Derived0_5Class) + 24u)
16    ((& Derived::_ZTC7Derived0_5Class) + 64u)
24    ((& Derived::_ZTV7Derived) + 96u)

Class Derived
   size=80 align=8
   base size=58 base align=8
Derived (0x0x7f693a33f138) 0
    vptridx=0u vptr=((& Derived::_ZTV7Derived) + 24u)
  Class (0x0x7f693a352380) 0
      primary-for Derived (0x0x7f693a33f138)
      subvttidx=8u
    SBase1 (0x0x7f693a34a840) 0
        primary-for Class (0x0x7f693a352380)
    SBase2 (0x0x7f693a34a8a0) 16
        vptr=((& Derived::_ZTV7Derived) + 64u)
    SBase3 (0x0x7f693a34a900) 28
    VBase (0x0x7f693a34a960) 64 virtual
        vptridx=24u vbaseoffset=-24 vptr=((& Derived::_ZTV7Derived) + 96u)


```

