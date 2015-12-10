# gcc/g++ -instrument-functions 參數


为项目太大，线程太多，gprof不可靠，于是找到了**finstrument-functions**
-finstrument-functions 會在每次進入與退出函式前呼叫 "__cyg_profile_func_enter" 與
"__cyg_profile_func_exit" 這兩個 hook function。等等，「進入」與「退出」是又何解？
C Programming Language 最經典之處在於，雖然沒有定義語言實做的方式，但實際上 function
call 皆以 stack frame 的形式存在，去年在「深入淺出 Hello World」Part II
有提過。所以上述那一大段英文就是說，如果我們不透過 GCC 內建函式
"__builtin_return_address" 取得 caller 與 callee 相關的動態位址，那麼仍可透過
-finstrument-functions，讓 GCC 合成相關的處理指令，讓我們得以追蹤。而看到 __cyg
開頭的函式，就知道是來自 Cygnus 的貢獻，在 gcc 2.x 內部設計可瞥見不少。

## C
- hello.c

```c
#include <stdio.h>

#define DUMP(func, call) printf("%s: func = %p, called by = %p/n", __FUNCTION__, func, call)

void __attribute__((__no_instrument_function__))
__cyg_profile_func_enter(void *this_func, void *call_site)
{
    DUMP(this_func, call_site);
}
void __attribute__((__no_instrument_function__))
__cyg_profile_func_exit(void *this_func, void *call_site)
{
    DUMP(this_func, call_site);
}


int main()
{
    puts("Hello World!");
    return 0;
}
```
- Makefile

```c
all:hello.o
    gcc -finstrument-functions -finstrument-functions hello.o -o hello
hello.o:
    gcc  -finstrument-functions  -c hello.c
clean:
    rm *.o hello -rf
```

- Run

```c
 ./hello
__cyg_profile_func_enter: func = 0x8048438, called by = 0x658dec/nHello World!
__cyg_profile_func_exit: func = 0x8048438, called by = 0x658dec/n
```

## C++

- hello.cc


```cpp
#include    <cstdlib>
#include    <cstdlib>
#include    <fstream>
#include    <iomanip>
#include    <iostream>

using namespace std;
#define DUMP(func, call)         printf("%s: func = %p, called by = %p\n", __FUNCTION__, func, call)

#ifdef __cplusplus
extern "C" {
#endif
/* Static functions. */
static FILE *openlogfile (const char *filename)
__attribute__ ((no_instrument_function));
static void closelogfile (void)
__attribute__ ((no_instrument_function));

/* Note that these are linked internally by the compiler.
 * Don't call them directly! */
void __cyg_profile_func_enter (void *this_fn, void *call_site)
__attribute__ ((no_instrument_function));
void __cyg_profile_func_exit (void *this_fn, void *call_site)
__attribute__ ((no_instrument_function));

#ifdef __cplusplus
};
#endif

void
__cyg_profile_func_enter (void *this_fn, void *call_site)
{
    DUMP(this_fn, call_site);
}

void
__cyg_profile_func_exit (void *this_fn, void *call_site)
{
    DUMP(this_fn, call_site);
}


int
main ( int argc, char *argv[] )
{
    cout    << "\nProgram " << argv[0] << endl << endl;

    return EXIT_SUCCESS;
}       // ----------  end of function main  ----------
```
- Makefile

```cpp
all:
    g++ -finstrument-functions  hello.cc
```

- Run

```c
./a.out
__cyg_profile_func_enter: func = 0x80487ae, called by = 0x8048a3b
__cyg_profile_func_enter: func = 0x8048714, called by = 0x80487d7
__cyg_profile_func_exit: func = 0x8048714, called by = 0x80487d7
__cyg_profile_func_exit: func = 0x80487ae, called by = 0x8048a3b
__cyg_profile_func_enter: func = 0x8048882, called by = 0x658dec

Program ./a.out

__cyg_profile_func_exit: func = 0x8048882, called by = 0x658dec
__cyg_profile_func_enter: func = 0x804881a, called by = 0x66e7f9
__cyg_profile_func_exit: func = 0x804881a, called by = 0x66e7f9
```

- hello.c

```c
#include <stdio.h>
#include "test.h"

#define DUMP(func, call) printf("%s: func = %p, called by = %p\n", __FUNCTION__, func, call)

void __attribute__((__no_instrument_function__))
__cyg_profile_func_enter(void *this_func, void *call_site)
{
    DUMP(this_func, call_site);
}
void __attribute__((__no_instrument_function__))
__cyg_profile_func_exit(void *this_func, void *call_site)
{
    DUMP(this_func, call_site);
}

void test()
{
    printf("nihao\n");
}

main()
{
    test();
    puts("Hello World!");
    return 0;
}
```

- Makefile

```
CC=gcc44
all:hello.o
    $(CC) -g -finstrument-functions  hello.o -o hello
hello.o:
	$(CC) -g -finstrument-functions -finstrument-functions-exclude-function-list=test -c hello.c

clean:
	rm *.o hello -rf

```

- Run

```c
./hello
__cyg_profile_func_enter: func = 0x804847e, called by = 0xb7e6adec
nihao
Hello World!
__cyg_profile_func_exit: func = 0x804847e, called by = 0xb7e6adec
```

##参考

GCC 函式追蹤功能 finstrument-functions __attribute__ +用 Graphviz 可视化函数调用

ibm 用 Graphviz 可视化函数调用  

gcc/g++使用-finstrument-functions來觀察code每個function的呼叫

http://www.logix.cz/michal/devel/CygProfiler/cyg-profile.c.xp

http://gcc.gnu.org/onlinedocs/gcc-4.3.3/gcc/Code-Gen-Options.html#Code-Gen-Options  官方参数说明

