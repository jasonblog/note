# memory layout C++ objects for clang


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
clang -cc1 -fdump-record-layouts layout.cpp
```


```sh

*** Dumping AST Record Layout
   0 | class SBase1
   0 |   (SBase1 vtable pointer)
   0 |   (SBase1 vftable pointer)
   8 |   int k
     | [sizeof=16, dsize=12, align=8
     |  nvsize=12, nvalign=8]


*** Dumping AST Record Layout
   0 | class SBase2
   0 |   (SBase2 vtable pointer)
   0 |   (SBase2 vftable pointer)
   8 |   int k
     | [sizeof=16, dsize=12, align=8
     |  nvsize=12, nvalign=8]


*** Dumping AST Record Layout
   0 | class SBase3
   0 |   int k
     | [sizeof=4, dsize=4, align=4
     |  nvsize=4, nvalign=4]


*** Dumping AST Record Layout
   0 | class VBase
   0 |   (VBase vtable pointer)
   0 |   (VBase vftable pointer)
   8 |   int j
     | [sizeof=16, dsize=12, align=8
     |  nvsize=12, nvalign=8]


*** Dumping AST Record Layout
   0 | class Class
   0 |   class SBase1 (primary base)
   0 |     (SBase1 vtable pointer)
   0 |     (SBase1 vftable pointer)
   8 |     int k
  16 |   class SBase2 (base)
  16 |     (SBase2 vtable pointer)
  16 |     (SBase2 vftable pointer)
  24 |     int k
  28 |   class SBase3 (base)
  28 |     int k
  32 |   int i
  36 |   char c
  40 |   float f
  48 |   double d
  56 |   short s
  64 |   class VBase (virtual base)
  64 |     (VBase vtable pointer)
  64 |     (VBase vftable pointer)
  72 |     int j
     | [sizeof=80, dsize=76, align=8
     |  nvsize=58, nvalign=8]


*** Dumping AST Record Layout
   0 | class Derived
   0 |   class Class (primary base)
   0 |     class SBase1 (primary base)
   0 |       (SBase1 vtable pointer)
   0 |       (SBase1 vftable pointer)
   8 |       int k
  16 |     class SBase2 (base)
  16 |       (SBase2 vtable pointer)
  16 |       (SBase2 vftable pointer)
  24 |       int k
  28 |     class SBase3 (base)
  28 |       int k
  32 |     int i
  36 |     char c
  40 |     float f
  48 |     double d
  56 |     short s
  64 |   class VBase (virtual base)
  64 |     (VBase vtable pointer)
  64 |     (VBase vftable pointer)
  72 |     int j
     | [sizeof=80, dsize=76, align=8
     |  nvsize=58, nvalign=8]


```