# 通过 GDB 调试理解 GOT/PLT


关于 Linux 中 ELF 文件格式可参考详细文档《ELF_Format》，本文仅记录笔者理解GOT/PLT的过程。

GOT（Global Offset Table）：全局偏移表用于记录在 ELF 文件中所用到的共享库中符号的绝对地址。在程序刚开始运行时，GOT 表项是空的，当符号第一次被调用时会动态解析符号的绝对地址然后转去执行，并将被解析符号的绝对地址记录在 GOT 中，第二次调用同一符号时，由于 GOT 中已经记录了其绝对地址，直接转去执行即可（不用重新解析）。

PLT（Procedure Linkage Table）：过程链接表的作用是将位置无关的符号转移到绝对地址。当一个外部符号被调用时，PLT 去引用 GOT 中的其符号对应的绝对地址，然后转入并执行。

GOT 位于 `.got.plt` section 中，而 PLT 位于 `.plt` section中。下面给出一示例程序：

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("argv[1] required!\n");
        exit(0);
    }
    printf("You input: ");
    printf(argv[1]);
    printf("Down\n");

    return 0;
}
```

编译该程序：

```sh
gcc -o format format.c
```

然后我们通过 `readelf` 命令来查看 `format` 程序的 sectioin 信息，并检查 GOT：

