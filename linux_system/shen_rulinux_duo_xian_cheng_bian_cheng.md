# 深入 Linux 多线程编程


NPTL 线程模型

NPTL，也即 Native POSIX Thread Library，是 Linux 2.6 引入的新的线程库实现，用来替代旧的 LinuxThreads 线程库。在 NPTL 实现中，用户创建的每个线程都对应着一个内核态的线程，内核态线程也是 Linux 的最小调度单元。

在 NPTL 实现中，线程的创建相当于调用clone()，并指定下面的参数：

```sh
CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_SIGHAND | CLONE_THREAD | CLONE_SETTLS | 
CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID | CLONE_SYSVSEM
```

下面解释一下这些参数的含义：


- CLONE_VM 所有线程都共享同一个进程地址空间。
- CLONE_FILES 所有线程都共享进程的文件描述符列表 (file descriptor table)。
- CLONE_FS 所有线程都共享同一个文件系统的信息。
- CLONE_SIGHAND 所有线程都共享同一个信号 handler 列表。
- CLONE_THREAD 所有线程都共享同一个进程 ID 以及 父进程 ID。


在 Linux 可以通过下面命令查看线程库的实现方式：

```sh
$ getconf GNU_LIBPTHREAD_VERSION
NPTL 2.23
```

##线程的栈

在 Linux 中，一个进程可以包含多个线程，这些线程将共享进程的全局变量，以及进程的堆，但每个线程都拥有它自己的栈。正如下图所示：


![](images/thread-stack.png)


在 64 位系统中，除了主线程之外，其它线程的栈默认大小为 8M，而主线程的栈则没有这个限制，因为主线程的栈可以动态增长。可以用下面的命令查看线程栈的大小：

```sh
$ ulimit -s
8192
```

通常来说，默认的线程栈大小可以满足大部分程序的需求，然而在一些特殊的场景下，譬如说，子线程需要在栈上分配大量的变量，或者执行深度比较大的递归调用，这时候就需要改变线程栈的大小了。下面的代码展示了如何修改线程栈的大小：


```c
#include <pthread.h>
#include <stdio.h>
void *thread_func(void *) {  return NULL;  }
int main(int argc, char *argv[])
{
    size_t size;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &size);
    printf("Default stack size = %li\n", size);
    size = 32 * 1024 * 1024;
    pthread_attr_setstacksize (&attr, size);
    pthread_t tid;
    pthread_create(&tid, &attr, thread_func, NULL);
    pthread_attr_getstacksize(&attr, &size);
    printf("New stack size = %li\n", size);
    pthread_attr_destroy(&attr);
    pthread_join(tid, NULL);
    return 0;
}
```

运行程序将输出下面的结果：


```sh
Default stack size = 8388608    # 默认为 8M
New stack size = 33554432       # 改成了 32 M
```

## 线程局部存储


Linux 提供了__thread关键字，用来表示线程局部存储。下面是__thread的使用规则：

- __thread可以用来修饰全局变量，以及函数内的 static 变量。
- 初始化__thread变量时，只能使用编译期常量。
- 在 C++ 中，__thread只能用于修饰 POD 类型，而不能用于修饰 class 类型。


当定义了一个__thread变量之后，每个线程都拥有了这个变量的一份副本。由于每个线程都拥有一份副本，所以在多线程中并发地访问__thread变量是安全的。

__thread可以帮助我们将不是线程安全的函数，改造成线程安全的。譬如说，标准库的strerror()函数会返回一个指针，指向一个全局变量，所以它不是线程安全的。利用__thread，我们可以实现一个线程安全的strerror()函数：


```cpp
#define _GNU_SOURCE
#include <stdio.h>  // Get '_sys_nerr' and '_sys_errlist' declarations 
#define MAX_ERROR_LEN 256
static __thread char buf[MAX_ERROR_LEN];
char *my_strerror_r( int err )
{
    if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL) 
    {
        snprintf(buf, MAX_ERROR_LEN, "Unknown error %d", err);
    } else {
        strncpy(buf, _sys_errlist[err], MAX_ERROR_LEN - 1);
        buf[MAX_ERROR_LEN - 1] = '\0';
    }
    return buf;
}
```

前面我们说到，__thread不能用于修饰 C++ 中的 class 类型，不过 C++11 提供了thread_local，除了可以修饰 POD 类型外，还可以修饰 class 类型：

```cpp
__thread std::string s1 = "Hello";        // 错误，__thread 不能修饰 class 类型
thread_local std::string s2 = "Hello";    // 正确
```

## 单例模式

在多线程编程中，有时候我们需要保证，无论程序创建了多少个线程，某些操作只执行了一次。Linux 提供了pthread_once()系统调用，我们可以借助pthread_once()实现单例模式：


```cpp
#include <pthread.h>
#include <stdlib.h>
template<typename T>
class Singleton
{
public:
    static T &instance()
    {
        pthread_once(&ponce_, &Singleton::init);
        return *value_;
    }
    // disable the copy operations
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
private:
    Singleton();
    ~Singleton();
    static void init()
    {
        value_ = new T();
        ::atexit(destroy);
    }
    static void destroy()
    {
        delete value_;
        value_ = nullptr;
    }
    static pthread_once_t  ponce_;
    static T              *value_;
};
template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;
template<typename T>
T *Singleton<T>::value_ = nullptr;
```

##参考资料

- The Native POSIX Thread Library for Linux
- Linux threading models compared: LinuxThreads and NPTL
- POSIX Threads Programming
- The Linux Programming Interface: A Linux and UNIX System Programming Handbook
- Linux多线程服务端编程 - 使用muduo C++网络库


