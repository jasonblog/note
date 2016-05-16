# tcc 原始碼分析技巧


- source code 

```sh
http://download.savannah.gnu.org/releases/tinycc/
```

```sh
wget http://download.savannah.gnu.org/releases/tinycc/tcc-0.9.26.tar.bz2
tar xvf tcc-0.9.26.tar.bz2
cd tcc-0.9.26
./configure
make CFLAGS='-g -O0'
```

```c
#include <stdio.h>

int main(int argc, char* argv[])
{
    printf("Hello world\n");
    return 0;
}
```

- gdb 分析編譯過程

```sh
cgdb --args ./tcc -B. hello.c -o hello
```