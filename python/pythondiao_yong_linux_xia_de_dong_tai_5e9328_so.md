# Python調用Linux下的動態庫(so)

- linuxany.c

```c
#include "stdio.h"
void display(char* msg)
{
    printf("%s\n", msg);
}

int add(int a, int b)
{
    return a + b;
}
```

```sh
gcc -c -fPIC linuxany.c
gcc -shared linuxany.o -o linuxany.so
```

```py
#!/usr/bin/python
 
from ctypes import *
import os 
libtest = cdll.LoadLibrary(os.getcwd() + '/linuxany.so') 
print 
libtest.display('Hello,I am linuxany.com') 
print libtest.add(2,2010)
```

```sh
Hello,I am linuxany.com
2012
```