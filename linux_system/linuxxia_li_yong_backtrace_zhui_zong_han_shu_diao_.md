# Linux下利用backtrace追踪函数调用堆栈以及定位段错误


一般察看函数运行时堆栈的方法是使用GDB（bt命令）之类的外部调试器,但是,有些时候为了分析程序的BUG,(主要针对长时间运行程序的分析),在程序出错时打印出函数的调用堆栈是非常有用的。

在glibc头文件"execinfo.h"中声明了三个函数用于获取当前线程的函数调用堆栈。

```c
int backtrace(void **buffer,int size) 
```

该函数用于获取当前线程的调用堆栈,获取的信息将会被存放在buffer中,它是一个指针列表。参数 size 用来指定buffer中可以保存多少个void* 元素。函数返回值是实际获取的指针个数,最大不超过size大小

在buffer中的指针实际是从堆栈中获取的返回地址,每一个堆栈框架有一个返回地址

注意:某些编译器的优化选项对获取正确的调用堆栈有干扰,另外内联函数没有堆栈框架;删除框架指针也会导致无法正确解析堆栈内容

```c
char ** backtrace_symbols (void *const *buffer, int size) 
```

backtrace_symbols将从backtrace函数获取的信息转化为一个字符串数组. 参数buffer应该是从backtrace函数获取的指针数组,size是该数组中的元素个数(backtrace的返回值)

函数返回值是一个指向字符串数组的指针,它的大小同buffer相同.每个字符串包含了一个相对于buffer中对应元素的可打印信息.它包括函数名，函数的偏移地址,和实际的返回地址

现在,只有使用ELF二进制格式的程序才能获取函数名称和偏移地址.在其他系统,只有16进制的返回地址能被获取.另外,你可能需要传递相应的符号给链接器,以能支持函数名功能(比如,在使用GNU ld链接器的系统中,你需要传递(-rdynamic)， -rdynamic可用来通知链接器将所有符号添加到动态符号表中，如果你的链接器支持-rdynamic的话，建议将其加上！)

该函数的返回值是通过malloc函数申请的空间,因此调用者必须使用free函数来释放指针.

注意:如果不能为字符串获取足够的空间函数的返回值将会为NULL

```c
void backtrace_symbols_fd (void *const *buffer, int size, int fd) 
```

backtrace_symbols_fd与backtrace_symbols 函数具有相同的功能,不同的是它不会给调用者返回字符串数组,而是将结果写入文件描述符为fd的文件中,每个函数对应一行.它不需要调用malloc函数,因此适用于有可能调用该函数会失败的情况
 
下面是glibc中的实例（稍有修改）：

```c
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

/* Obtain a backtrace and print it to @code{stdout}. */
void print_trace(void)
{
    void* array[10];
    size_t size;
    char** strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    if (NULL == strings) {
        exit(EXIT_FAILURE);
    }

    printf("Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings);
    strings = NULL;
}

/* A dummy function to make the backtrace more interesting. */
void dummy_function(void)
{
    print_trace();
}

int main(int argc, char* argv[])
{
    dummy_function();
    return 0;
}
```

