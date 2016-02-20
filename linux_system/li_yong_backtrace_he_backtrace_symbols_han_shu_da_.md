# 利用backtrace和backtrace_symbols函数打印调用栈信息


在头文件"execinfo.h"中声明了三个函数用于获取当前线程的函数调用堆栈。

```c
#include <execinfo.h>
int backtrace(void **buffer, int size);
char **backtrace_symbols(void *const *buffer, int size);
void backtrace_symbols_fd(void *const *buffer, int size, int fd);
```
man 帮助：

```c
DESCRIPTION        
       backtrace() returns a backtrace for the calling program, in the array
       pointed to by buffer.  A backtrace is the series of currently active
       function calls for the program.  Each item in the array pointed to by
       buffer is of type void *, and is the return address from the
       corresponding stack frame.  The size argument specifies the maximum
       number of addresses that can be stored in buffer.  If the backtrace
       is larger than size, then the addresses corresponding to the size
       most recent function calls are returned; to obtain the complete
       backtrace, make sure that buffer and size are large enough.

       Given the set of addresses returned by backtrace() in buffer,
       backtrace_symbols() translates the addresses into an array of strings
       that describe the addresses symbolically.  The size argument
       specifies the number of addresses in buffer.  The symbolic
       representation of each address consists of the function name (if this
       can be determined), a hexadecimal offset into the function, and the
       actual return address (in hexadecimal).  The address of the array of
       string pointers is returned as the function result of
       backtrace_symbols().  This array is malloc(3)ed by
       backtrace_symbols(), and must be freed by the caller.  (The strings
       pointed to by the array of pointers need not and should not be
       freed.)

       backtrace_symbols_fd() takes the same buffer and size arguments as
       backtrace_symbols(), but instead of returning an array of strings to
       the caller, it writes the strings, one per line, to the file
       descriptor fd.  backtrace_symbols_fd() does not call malloc(3), and
       so can be employed in situations where the latter function might
       fail.

```


```c
int backtrace(void **buffer,int size)
```

该函数用与获取当前线程的调用堆栈,获取的信息将会被存放在buffer中,它是一个指针数组。参数 size 用来指定buffer中可以保存多少个void* 元素。函数返回值是实际获取的指针个数,最大不超过size大小在buffer中的指针实际是从堆栈中获取的返回地址,每一个堆栈框架有一个返回地址。
注意某些编译器的优化选项对获取正确的调用堆栈有干扰,另外内联函数没有堆栈框架;删除框架指针也会使无法正确解析堆栈内容

```c
char ** backtrace_symbols (void *const *buffer, int size)
```

backtrace_symbols将从backtrace函数获取的信息转化为一个字符串数组. 参数buffer应该是从backtrace函数获取的数组指针,size是该数组中的元素个数(backtrace的返回值)，函数返回值是一个指向字符串数组的指针,它的大小同buffer相同.每个字符串包含了一个相对于buffer中对应元素的可打印信息.它包括函数名，函数的偏移地址,和实际的返回地址
现在,只有使用ELF二进制格式的程序和苦衷才能获取函数名称和偏移地址.在其他系统,只有16进制的返回地址能被获取.另外,你可能需要传递相应的标志给链接器,以能支持函数名功能(比如,在使用GNU ld的系统中,你需要传递(-rdynamic))

backtrace_symbols生成的字符串都是malloc出来的，但是不要最后一个一个的free，因为backtrace_symbols是根据backtrace给出的call stack层数，一次性的malloc出来一块内存来存放结果字符串的，所以，像上面代码一样，只需要在最后，free backtrace_symbols的返回指针就OK了。这一点backtrace的manual中也是特别提到的。
注意:如果不能为字符串获取足够的空间函数的返回值将会为NULL

```c
void backtrace_symbols_fd (void *const *buffer, int size, int fd)
```

backtrace_symbols_fd与backtrace_symbols 函数具有相同的功能,不同的是它不会给调用者返回字符串数组,而是将结果写入文件描述符为fd的文件中,每个函数对应一行.它不需要调用malloc函数,因此适用于有可能调用该函数会失败的情况。
man手册中示例：

```c
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
myfunc3(void)
{
   int j, nptrs;
#define SIZE 100
   void *buffer[100];
   char **strings;

   nptrs = backtrace(buffer, SIZE);
   printf("backtrace() returned %d addresses\n", nptrs);

   /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
      would produce similar output to the following: */

   strings = backtrace_symbols(buffer, nptrs);
   if (strings == NULL) {
       perror("backtrace_symbols");
       exit(EXIT_FAILURE);
   }

   for (j = 0; j < nptrs; j++)
       printf("%s\n", strings[j]);

   free(strings);
}

static void   /* "static" means don't export the symbol... */
myfunc2(void)
{
   myfunc3();
}

void
myfunc(int ncalls)
{
   if (ncalls > 1)
       myfunc(ncalls - 1);
   else
       myfunc2();
}

int
main(int argc, char *argv[])
{
   if (argc != 2) {
       fprintf(stderr, "%s num-calls\n", argv[0]);
       exit(EXIT_FAILURE);
   }

   myfunc(atoi(argv[1]));
   exit(EXIT_SUCCESS);
}
```

结果：

