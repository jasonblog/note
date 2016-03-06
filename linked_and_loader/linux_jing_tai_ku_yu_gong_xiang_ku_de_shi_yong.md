# Linux 静态库与共享库的使用


##一、静态库

###1、概念：
静态库指将所有相关的目标文件打包成为一个单独的文件-即静态库文件，以.a结尾。`静态库可作为链接器的输入`，链接器会将程序中使用的到函数的代码从库文件中拷贝到应用程序中。`一旦链接完成，在执行程序的时候就不需要静态库了`。

- 注1：由于每个使用静态库的应用程序都需要拷贝所用函数的代码，所以静态链接的文件会比较大。
- 注2：在Unix系统中，静态库以一种称为存档（archive）的特殊文件格式存放在磁盘中。存档文件是一组连接起来的可重定位目标文件的集合，有一个头部用来描述每个成员目标文件的大小和位置（存档文件名由后缀.a标识）。

###2、创建与应用

假设我们想在一个叫做libvector.a的静态库中提供以下向量函数：

```c
// addvec.c
void addvec(int* x, int* y, int* z, int n)
{
    int i = 0;

    for (; i < n; ++i) {
        z[i] = x[i] + y[i];
    }
}
```

```c
// multvec.c
void multvec(int* x, int* y, int*  z, int n)
{
    int i = 0;

    for (; i < n; ++i) {
        z[i] = x[i] * y[i];
    }
}
```

使用AR工具创建静态库文件：

![](./images/20130914170235109.jpg)

为了使用这个库，编写一个应用（其调用addvec库中的函数）：

```c
/* main2.c */
#include <stdio.h>

int x[2] = {1, 2};
int y[2] = {3, 4};
int z[2] = {0};

int main()
{
    addvec(x, y, z, 2);
    printf("z = [%d %d]\n", z[0], z[1]);
    return 0;
}
```
编译-链接-运行程序：
![](./images/20130914170342765.jpg)

- 注1：`-static` 参数告诉编译器驱动程序，链接器应该构建一个完全的可执行目标文件，它可以加载到存储器并运行，在加载时无需进一步的链接 -`即一次性静态链接完毕，不允许存在动态链接`。

- 注2：当链接器运行时，它判定addvec.o定义的addvec符号是被main2.o引用的，所以它拷贝addvec.o到可执行文件。因为程序中没有引用任何由multvec.o定义的符号，所以链接器就不会拷贝这个模块到可执行文件。同时，链接器还会拷贝libc.a中的pirintf.o模块，以及许多C运行时系统中的其他模块。链接器完整的行为可如下图所示：

![](./images/20130914172958578.jpg)

##二、共享库

`1、概念`：共享库是一个目标模块（以.so后缀表示），在运行时，可以加载到任意的存储器地址，并和一个在存储器中的程序链接起来，这个过程称为动态链接，是由一个叫做动态链接器的程序来执行的。

`2、分类`：根据加载和链接共享库的时机又可分为：
`A）`应用程序自身加载时动态链接和加载共享库；
`B）`应用程序运行过程中动态链接和加载共享库两种情况。

2-A：`应用程序自身加载时动态链接和加载共享库`

2-A.1 `基本思路是`：当创建可执行文件时，静态执行一些链接（共享库的重定位和符号表信息，而非代码和数据），然后在应用程序加载时，动态完成链接过程。

2-A.2 创建与应用

创建类似于静态库的创建，假设我们现在想在一个叫做libvector.so的共享库库中提供以下addvec和multvec函数：
下面使用`-shared`选项来指示链接器创建一个共享的目标文件（即共享库），链接并运行程序：
![](./images/20130914173036234.jpg)

- 注1：`-fPIC`选项指示编译器生成与位置无关的代码
其动态链接过程可如下图所示：

![](./images/20130914173103921.jpg)

- 注2：在可执行文件p2中没有拷贝任何libvector.so真正的代码和数据节，而是由链接器拷贝了一些重定位和符号表信息，它们使得运行时动态链接器可以解析libvector.so中代码和数据的引用，重定位完成链接任务。其中需要重定位的有：

- 1）重定位libc.so的文本和数据到某个存储器段；
- 2）重定位libvector.so的文本和数据到另一个存储器段；
- 3）重定位p2中所有对libc.so和libvector.so定义的符号的引用。

最后链接器将控制传递给应用程序。`从这个时刻开始，共享库的位置就固定了，并在在程序的执行过程中都不会再改变！`

2-B：`应用程序运行过程中动态链接和加载共享库`


`2-B.1 概念`：与A情况不同，此情况下：`应用程序在运行过程中要求动态链接器加载和链接任意共享库，而无需编译时链接那些库到应用中。`

`2-B.2 应用实例`
Linux系统为应用程序在运行过程中加载和链接共享库提供了一组API：


```c
#include<dlfcn.h>

/* 加载和链接共享库 filename
    filename：共享库的名字
    flag有：RTLD_LAZY, RTLD_NOW,二者均可以和RTLD_GLOBAL表示取或
*/
void* dlopen(const char* filename,
             int flag); // 若成功则返回执行句柄的指针，否则返回NULL

/*根据共享库操作句柄与符号，返回符号对应的地址
    handle:共享库操作句柄
    symbol：需要引用的符号名字
*/
void* dlsym(void* handle,
            char* symbol); // 若成功则返回执行符号的指针（即地址），若出错则返回NULL

/* 如果没有程序正在使用这个共享库，卸载该共享库 */
int dlclose(void* handle); // 若卸载成功，则返回0，否则返回-1

/* 捕捉最近发生的错误 */
const char* dlerror(
    void); // 若前面对dlopen，dlsym或dlclose调用失败，则返回错误消息，否则返回NULL

```

例子：

```c
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int x[2] = {1, 2};
int y[2] = {3, 4};
int z[2] = {0};

int main()
{
    void* handle;
    void (*addvec)(int*, int*, int*, int);
    char* error;

    handle = dlopen("./libvector.so", RTLD_LAZY);

    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    addvec = dlsym(handle, "addvec");

    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    addvec(x, y, z, 2);
    printf("z = [%d %d]\n", z[0], z[1]);

    if (dlclose(handle) < 0) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    return 0;
}

```

运行结果：`-ldl`参数：表示生成的对象模块需要用到共享库
![](./images/20130914181417500.jpg)



##Referebces:

1.《深入理解计算机系统》第7章：链接 P448-P479
2. 静态库、共享库、动态库的创建和使用 ：http://bbs.chinaunix.net/thread-2037617-1-1.html
3. Linux 动态库剖析：http://www.ibm.com/developerworks/cn/linux/l-dynamic-libraries/
4. dlopen: http://baike.baidu.com/link?url=VswI42A-IxFuF5SelbJxDREXuY0BvYWHEdcCYozSNH93ark0nTMi4YdhHrvt-bIo2_F-swU2onuYMNwXeUGVMq