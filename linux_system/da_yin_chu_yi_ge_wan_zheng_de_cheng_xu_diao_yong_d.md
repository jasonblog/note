# 打印出一个完整的程序调用堆栈信息


## 如何通过函数的地址获取到函数的名字

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

通过gcc编译的时候增加-rdynamic编译选项，该编译选项的含义是：使得程序中的共享库的全局符号可用


## 如何打印出一个程序的函数调用链路

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

打印程序的调用链路，主要是利用到__attribute__可以用来设置 Function-Attributes函数属性 我们使用的是：no_instrument_function这个函数属性，在gcc编译的时候加上：-finstrument-functions编译选项就会在每一个用户自定义函数中添加下面两个函数调用:

```c
void __cyg_profile_func_enter(void *this, void *callsite);
void __cyg_profile_func_exit(void *this, void *callsite);
```

这两个函数我们用户可以自己实现，其中this指针指向当前函数的地址，callsite是指向上一级调用函数的地址

##综合前面两个问题就可以打印出一个完整的程序调用堆栈信息



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


## 最终就会输出：

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