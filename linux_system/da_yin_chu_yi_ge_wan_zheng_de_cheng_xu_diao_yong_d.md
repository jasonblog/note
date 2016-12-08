# 打印出一個完整的程序調用堆棧信息


## 如何通過函數的地址獲取到函數的名字

```c
#include <stdio.h>
#include <execinfo.h>

void my_name_is_fucking(void) {
    printf("fucking\n");
}

int main(int argc, char *argv[]) {
    void    *funptr = &my_name_is_fucking;

    backtrace_symbols_fd(&funptr, 1, 1);

    return 0;
}
```

通過gcc編譯的時候增加-rdynamic編譯選項，該編譯選項的含義是：使得程序中的共享庫的全局符號可用


## 如何打印出一個程序的函數調用鏈路

`no_instrument_function`
如果使用了-finstrument-functions ，將在絕大多數用戶編譯的函數的入口和出口點調用profiling函數。使用該屬性，`將不進行instrument操作。`

```c
#include <stdio.h>


#define debug_print(fmt, args...) do { \
            fprintf(stderr, fmt, ##args); \
        }while(0)


void __cyg_profile_func_exit(void* callee, void* callsite) __attribute__((no_instrument_function));
void __cyg_profile_func_enter(void* callee, void* callsite) __attribute__((no_instrument_function));


void __cyg_profile_func_enter(void* callee, void* callsite) {
    debug_print("Entering %p in %p\n", callee, callsite);
}


void __cyg_profile_func_exit(void* callee, void* callsite) {
    debug_print("Exiting %p in %p\n", callee, callsite);
}


void foo4() {
    printf("foo()\n");
}


void foo3() {
    foo4();
}


void foo2() {
    foo3();
}


void foo1() {
    foo2();
}


int main() {
    foo1();
    return 0;
}
```

打印程序的調用鏈路，主要是利用到__attribute__可以用來設置 Function-Attributes函數屬性 我們使用的是：no_instrument_function這個函數屬性，在gcc編譯的時候加上：-finstrument-functions編譯選項就會在每一個用戶自定義函數中添加下面兩個函數調用:

```c
void __cyg_profile_func_enter(void *this, void *callsite);
void __cyg_profile_func_exit(void *this, void *callsite);
```

這兩個函數我們用戶可以自己實現，其中this指針指向當前函數的地址，callsite是指向上一級調用函數的地址

##綜合前面兩個問題就可以打印出一個完整的程序調用堆棧信息



- trace_func.c


```c
#include <stdio.h>
#include <malloc.h>
#include <execinfo.h>


void __cyg_profile_func_exit(void* callee, void* callsite) __attribute__((no_instrument_function));
void __cyg_profile_func_enter(void* callee, void* callsite) __attribute__((no_instrument_function));


void __cyg_profile_func_enter(void* callee, void* callsite) {
    void    *funptr = callee;
    char **p = backtrace_symbols(&funptr, 1);
    printf("Entering: %s\n", *p);
    free(p);
}


void __cyg_profile_func_exit(void* callee, void* callsite) {
    void    *funptr = callee;
    char **p = backtrace_symbols(&funptr, 1);
    printf("Exiting: %s\n", *p);
    free(p);
}


void foo4() {
    printf("foo()\n");
}


void foo3() {
    foo4();
}


void foo2() {
    foo3();
}


void foo1() {
    foo2();
}


int main() {
    foo1();
    return 0;
}
```

```sh
gcc trace_func.c -rdynamic -finstrument-functions
```


## 最終就會輸出：

```sh
Entering: ./a.out(main+0) [0x400a32]
Entering: ./a.out(foo1+0) [0x400a06]
Entering: ./a.out(foo2+0) [0x4009da]
Entering: ./a.out(foo3+0) [0x4009ae]
Entering: ./a.out(foo4+0) [0x400982]
foo()
Exiting: ./a.out(foo4+0) [0x400982]
Exiting: ./a.out(foo3+0) [0x4009ae]
Exiting: ./a.out(foo2+0) [0x4009da]
Exiting: ./a.out(foo1+0) [0x400a06]
Exiting: ./a.out(main+0) [0x400a32]
```