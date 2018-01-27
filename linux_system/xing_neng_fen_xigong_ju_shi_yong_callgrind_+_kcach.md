# 性能分析工具使用 callgrind+kcachegrind


##一、简述
本文讲述如何应用callgrind和kcachegrind进行性能分析。该工具可以方便分析出可执行文件函数的执行时间及调用关系。
    
##二、开发环境
操作系统
Mint17
内核
3.13.0
编译器
gcc4.8.0
CPU
VIA Nano X2 L4530 @ 1.6+ GHz
内存
4G
多核
2个
 
三、安装
sudo apt-get install valgrind  kcachegrind
四、使用
 原始调试文件：
perf.c:

```c
#include <stdio.h>
void hello()
{
        printf("hello world!\n");
}
void test()
{
        hello();
        printf("test \n");
}
void main()
{
        hello();
        hello();
        test();
        test();
        test();
}
```

 编译：
 
 ```sh
 gcc  -W -Wall  -pedantic -ansi -std=gnu99 -g -O0 -pg perf.c -o perf 
 ```
 
 
 利用valgrind生成性能日志
 
 ```sh
 valgrind --tool=callgrind ./perf
```

利用kcachegrind 打开生成的callgrind.out.2622。

```sh
kcachegrind callgrind.out.2622
```

可以看出分析出了每个函数调用的时间及比例以调用关系图。
 