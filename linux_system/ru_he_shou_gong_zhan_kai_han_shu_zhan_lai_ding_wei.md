# 如何手工展开函数栈来定位问题



当程序crash的时候，我们可以通过coredump文件，来定位问题。比如使用bt命令可以完整的展开函数的调用栈。但是有些时候，部分栈的数据可能被损坏，导致gdb无法直接显示函数的调用栈。那么这时就需要我们手工展开函数栈。

关于x86的函数调用栈的示意图基本如下图所示：

![](./images/23629988_1322490468QN79.gif)


关于参数的压栈顺序，上图为cdecl方式，这个可以通过编译选项修改。GCC默认使用cdecl。

下面看一下例子：

```c
#include <stdlib.h>
#include <stdio.h>


static int test(int a, int b, int c)
{
    return a+b+c;
}

int main()
{
    int a = 1;
    int b = 2;
    int c = 3;

    int d = test(a, b, c);

    printf("%d\n", d);

    return 0;
}
```

编译：gcc -g -Wall test.c
进入test，查看函数调用栈：
Breakpoint 1, test (a=1, b=2, c=3) at test.c:7
7 return a+b+c;
Missing separate debuginfos, use: debuginfo-install glibc-2.11-2.i686

```
(gdb) bt
#0 test (a=1, b=2, c=3) at test.c:7
#1 0x08048412 in main () at test.c:16
```

那么现在查看一下寄存器：

```sh
eax 0x1 1
ecx 0x2c0187d8 738297816
edx 0x1 1
ebx 0x73fff4 7602164
esp 0xbffff048 0xbffff048
ebp 0xbffff048 0xbffff048
esi 0x0 0
edi 0x0 0
eip 0x80483c7 0x80483c7 <test+3>
eflags 0x286 [ PF SF IF ]
cs 0x73 115
ss 0x7b 123
ds 0x7b 123
es 0x7b 123
fs 0x0 0
gs 0x33 51
```

得到`ebp的地址为0xbffff048`，现在检查这个地址的内存

```sh
(gdb) x /8x 0xbffff048
0xbffff048: 0xbffff078 0x08048412 0x00000001 0x00000002
0xbffff058: 0x00000003 0x0073fff4 0x00000001 0x00000002
```

下面分析一下这些内存的内容：

> 1. 0xbffff078：为test的调用者，即main函数的bp地址；BP地址即为该函数的栈顶指针。
2. 0x08048412：为test的返回地址，与前面的bt的输出相符；
3. 后面的0x00000001，0x00000002，0x00000003，为传给test的三个参数，且参数顺序为由右向左压栈——注意这个顺序是可以通过改变编译参数改变的。

回到main中，验证一下bp寄存器的内容：

```sh
0x08048412 in main () at test.c:16
16 int d = test(a, b, c);
Value returned is $2 = 6
(gdb) info registers
eax 0x6 6
ecx 0x39ff7a48 973044296
edx 0x1 1
ebx 0x73fff4 7602164
esp 0xbffff050 0xbffff050
ebp 0xbffff078 0xbffff078
```

可见BP的地址确实为0xbffff078，与之前的分析相符。

注：关于压栈顺序，参数的传递方式等等，都可以通过编译选项来指定或者禁止的。本文的情况为GCC的默认行为。



