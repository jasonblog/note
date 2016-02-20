# linux系统编程之文件与IO（一）：文件描述符、open，close


## 1. 什么是IO？
- 输入/输出是主存和外部设备之间拷贝数据的过程
    - 设备->内存（输入操作）
    - 内存->设备（输出操作）

- 高级I/O
    - ANSI C提供的标准I/O库称为高级I/O，通常也称为带缓冲的I/O

- 低级I/O
    - 通常也称为不带缓冲的I/O

## 2. 文件描述符：fd

- 对于Linux而言，所有对设备或文件的操作都是通过文件描述符进行的。
- 当打开或者创建一个文件的时候，内核向进程返回一个文件描述符（非负整数）。后续对文件的操作只需通过该文件描述符，内核记录有关这个打开文件的信息。
- 一个进程启动时，默认打开了3个文件，标准输入、标准输出、标准错误，对应文件描述符是0（STDIN_FILENO）、1（STDOUT_FILENO）、2（STDERR_FILENO）,这些常量定义在unistd.h头文件中。C库函数中与之对应的是：stdin,stdout,stderr,不过这三个是FILE指针类型。

## 3.文件描述符与文件指针相互转换

可以通过以下两个函数实现：

- fileno：将文件指针转换为文件描述符

```c
#include <stdio.h>
int fileno(FILE *stream)
```
测试程序：

```c
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    printf("fileno(stdin) = %d\n", fileno(stdin));
    printf("fileno(stdout) = %d\n", fileno(stdout));
    printf("fileno(stderr) = %d\n", fileno(stderr));
    return 0;
}
```

