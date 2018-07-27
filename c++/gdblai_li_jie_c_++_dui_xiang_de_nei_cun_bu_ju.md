# GDB來理解C++ 對象的內存佈局


```cpp
#include <iostream>
using namespace std;

class Parent
{
public:
    Parent(): numInParent(1111)
    {}
    virtual void Foo()
    {
    };
    virtual void Boo()
    {
    };
private:
    int numInParent;
};

class Child: public Parent
{
public:
    Child(): numInChild(2222) {}
    virtual void Foo()
    {
    }
    int numInChild;
};


int main(int argc, char *argv[])
{

    Child c;
    
    return 0;
}
```


```sh
(gdb) info vtbl this
vtable for 'Parent' @ 0x555555755d68 (subobject @ 0x7fffffffded0):
[0]: 0x555555554a22 <Parent::Foo()>
[1]: 0x555555554a2e <Parent::Boo()>
```



```sh
(gdb) p *this
$1 = (Parent) {
  _vptr.Parent = 0x555555755d68 <vtable for Parent+16>,
  numInParent = 1111
}
```

虚函数表 vtable for 'A' @0x405188

```sh
(gdb) x/16x 0x555555755d68
0x555555755d68 <vtable for Parent+16>:  0x55554a22  0x00005555  0x55554a2e  0x00005555
0x555555755d78 <typeinfo for Child>:    0xf7dc7438  0x00007fff  0x55554b05  0x00005555
0x555555755d88 <typeinfo for Child+16>: 0x55755d90  0x00005555  0xf7dc67f8  0x00007fff
0x555555755d98 <typeinfo for Parent+8>: 0x55554b10  0x00005555  0x00000001  0x00000000
```

`0x55554a22 補齊為 0x555555554a22`

```sh
(gdb) x/32x 0x555555554a22
0x555555554a22 <Parent::Foo()>: 0xe5894855  0xf87d8948  0x90c35d90  0xe5894855
0x555555554a32 <Parent::Boo()+4>:   0xf87d8948  0x90c35d90  0xe5894855  0x10ec8348
0x555555554a42 <Child::Child()+8>:  0xf87d8948  0xf8458b48  0xe8c78948  0xffffffac
0x555555554a52 <Child::Child()+24>: 0xef158d48  0x48002012  0x48f8458b  0x8b481089
0x555555554a62 <Child::Child()+40>: 0x40c7f845  0x0008ae0c  0xc3c99000  0xe5894855
0x555555554a72 <Child::Foo()+4>:    0xf87d8948  0x0fc35d90  0x0000801f  0x57410000
0x555555554a82 <__libc_csu_init+2>: 0x89495641  0x415541d7  0x258d4c54  0x0020128e
0x555555554a92 <__libc_csu_init+18>:    0x2d8d4855  0x00201296  0xfd894153  0x4cf68949
```


```sh
(gdb) p /a (*(void ***)this)[0]@3
$6 = {[0x0] = 0x555555554a22 <Parent::Foo()>, [0x1] = 0x555555554a2e <Parent::Boo()>, [0x2] = 0x7ffff7dc7438 <vtable for __cxxabiv1::__si_class_type_info+16>}
```
