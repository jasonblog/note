# gdb 展開 C/C++ 的 macro

- test.cpp

```cpp
#include <iostream>
using namespace std;

#define MA(x) MB(x) { cout << "Hello World. I am " << #x << "." << endl; }
#define MB(x) void fun_ ## x ()

MA(Mozzenior)

int main(int argc, char** argv)
{
    fun_Mozzenior();
    return 0;
}
```

- -g3

```sh
g++ test.cpp  -g3

cgdb ./a.out

(gdb) macro expand MA(Mozzenior)
expands to: void fun_Mozzenior () { cout << "Hello World. I am " << "Mozzenior" << "." << endl; }


```

