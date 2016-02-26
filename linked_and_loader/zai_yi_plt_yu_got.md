# 再议PLT与GOT


PLT（Procedure Linkage Table）的作用是将位置无关的符号转移到绝对地址。当一个外部符号被调用时，PLT 去引用 GOT 中的其符号对应的绝对地址，然后转入并执行。

GOT（Global Offset Table）：用于记录在 ELF 文件中所用到的共享库中符号的绝对地址。在程序刚开始运行时，GOT 表项是空的，当符号第一次被调用时会动态解析符号的绝对地址然后转去执行，并将被解析符号的绝对地址记录在 GOT 中，第二次调用同一符号时，由于 GOT 中已经记录了其绝对地址，直接转去执行即可（不用重新解析）。

其中PLT 对应.plt section ，而GOT对应 .got.plt ，主要对应函数的绝对地址。通过readelf查看可执行文件，我们发现还存在.got section，这个section主要对应动态链接库中变量的绝对地址。我们还要注意PLT section在代码链接的时候已经存在，存在于代码段中，而GOT存在于数据段中。

在我先前的博文ELF文件的加载，我简单的对PLT和GOT进行了介绍。这里我们增加复杂度，重新对这个动态链接机制进行分析。

- foo.c

```c
#include <stdio.h>
 
void foo(int i)
{
     printf("Test %d\n",i);
}
```

- main.c

```c
#include <stdio.h>
#include <unistd.h>
 
extern void foo(int i);
 
int main()
{
     printf("Test 1\n");
     printf("Test 2\n");
     foo(3);
     foo(4);
     return 0;
}
```

然后我们将foo.c 编译成为liba.so: gcc -shared -fPIC -g3 -o libfoo.so foo.c
下面编译主main函数：gcc -Wall -g3 -o main main.c -lfoo -L /tmp/libfoo.so

下面我们对这个main进行调试,我们在printf与foo上都打上断点,然后开始运行，调试


```sh
(gdb) disassemble
Dump of assembler code for function main:
   0x00000000004006e6 <+0>:   push   %rbp
   0x00000000004006e7 <+1>:   mov    %rsp,%rbp
=> 0x00000000004006ea <+4>:    mov    $0x4007a0,%edi
   0x00000000004006ef <+9>:   callq  0x4005b0 <puts@plt>
   0x00000000004006f4 <+14>:  mov    $0x4007a0,%edi
   0x00000000004006f9 <+19>:  callq  0x4005b0 <puts@plt>
   0x00000000004006fe <+24>:  callq  0x4005e0 <foo@plt>
   0x0000000000400703 <+29>:  callq  0x4005e0 <foo@plt>
   0x0000000000400708 <+34>:  mov    $0x0,%eax
   0x000000000040070d <+39>:  pop    %rbp
   0x000000000040070e <+40>:  retq
End of assembler dump.
...
(gdb) si
0x00000000004005b0 in puts@plt ()
```

这里我们进入了.plt 中寻找puts函数，查看0x601018中的值，我们发现地址0x004005b6，而0x004005b6正是jmpq的下一条指令，执行完跳如GOT表中查找函数绝对地址。这样做避免了GOT表表是否为真实值检查，如果为空那么寻址，否则直接调用。

```sh
(gdb) disassemble
Dump of assembler code for function puts@plt:
=> 0x00000000004005b0 <+0>:    jmpq   *0x200a62(%rip)        # 0x601018 <puts@got.plt>
   0x00000000004005b6 <+6>:   pushq  $0x0
   0x00000000004005bb <+11>:  jmpq   0x4005a0
End of assembler dump.
(gdb) x/32x 0x601018
0x601018 <puts@got.plt>:  0x004005b6  0x00000000  0x25e20610  0x0000003e
0x601028 <__gmon_start__@got.plt>:    0x004005d6  0x00000000  0x004005e6  0x00000000
...
End of assembler dump.
(gdb) si
0x00000000004005a0 in ?? ()
(gdb)
0x00000000004005a6 in ?? ()
(gdb)
0x0000003e25615b70 in _dl_runtime_resolve () from /lib64/ld-linux-x86-64.so.2
(gdb) disassemble
Dump of assembler code for function _dl_runtime_resolve:
=> 0x0000003e25615b70 <+0>:    sub    $0x78,%rsp
   0x0000003e25615b74 <+4>:   mov    %rax,0x40(%rsp)
...
   0x0000003e25615b9c <+44>:  bndmov %bnd1,0x10(%rsp)
   0x0000003e25615ba2 <+50>:  bndmov %bnd2,0x20(%rsp)
   0x0000003e25615ba8 <+56>:  bndmov %bnd3,0x30(%rsp)
   0x0000003e25615bae <+62>:  mov    0x8i0(%rsp),%rsi
   0x0000003e25615bb6 <+70>:  mov    0x78(%rsp),%rd
   0x0000003e25615bbb <+75>:  callq  0x3e2560e990 <_dl_fixup>
...
```

这个_dl_runtime_resolve 来自于ld-linux-x86-64.so.2文件，然后在ld中调用_dl_fixup 将真实的puts函数地址填入GOT表中,当程序再次调入puts函数中时，直接jmpq跳转到0x25e6fa70地址执行。

```sh
(gdb) disassemble 
Dump of assembler code for function puts@plt:
=> 0x00000000004005b0 <+0>:    jmpq   *0x200a62(%rip)        # 0x601018 <puts@got.plt>
   0x00000000004005b6 <+6>:   pushq  $0x0
   0x00000000004005bb <+11>:  jmpq   0x4005a0
End of assembler dump.
(gdb) x/32 0x601018
0x601018 <puts@got.plt>:  0x25e6fa70  0x0000003e  0x25e20610  0x0000003e
0x601028 <__gmon_start__@got.plt>:    0x004005d6  0x00000000  0x004005e6  0x00000000
(gdb) n
Single stepping until exit from function _dl_runtime_resolve,
which has no line number information.
0x0000003e25e6fa70 in puts () from /lib64/libc.so.6
```

下面来说明foo的执行：当代码第一次执行foo函数，进程查找GOT表，找不到该函数，这个时候跳转到PLT[0] 使用_dl_runtime_resolve查找foo函数的绝对地址，当找到该函数绝对地址后，进入foo函数执行，foo函数中存在printf () 函数，这个函数和之前main函数中的printf() 不同，重新使用_dl_runtime_resolve 查找libc中的puts函数，将其插入到GOT表中。


```sh
(gdb) disassemble
Dump of assembler code for function foo@plt:
   0x00000000004005e0 <+0>:   jmpq   *0x200a4a(%rip)        # 0x601030 <foo@got.plt>
=> 0x00000000004005e6 <+6>:    pushq  $0x3
   0x00000000004005eb <+11>:  jmpq   0x4005a0
End of assembler dump.
...
(gdb)
0x00007ffff7df85a0 in puts@plt () from libfoo.so
(gdb)
0x00007ffff7df85a6 in puts@plt () from libfoo.so
(gdb)
0x00007ffff7df85ab in puts@plt () from libfoo.so
(gdb)
0x00007ffff7df8590 in ?? () from libfoo.so
(gdb)
0x00007ffff7df8596 in ?? () from libfoo.so
(gdb)
0x0000003e25615b70 in _dl_runtime_resolve () from /lib64/ld-linux-x86-64.so.2
```

当再次使用libfoo.so 中的foo函数，直接跳转GOT执行即可,无需再次查找。


```sh
(gdb)
0x00007ffff7df86db  5       printf(...);
(gdb)
0x00007ffff7df85a0 in puts@plt () from libfoo.so
(gdb)
0x0000003e25e6fa70 in puts () from /lib64/libc.so.6
...
```

##总结：

`ld-linux-x86-64.so.2 是一个动态链接库，负责查找程序所使用的函数绝对地址，并将其写入到GOT表中，以供后续调用`。其中GOT[0]为空，GOT[1]和GOT[2]用于保存查找的绝对函数地址，GOT[1]保存的是一个地址，指向已经加载的共享库的链表地址；GOT[2]保存的是一个函数的地址，定义如下：GOT[2] = &_dl_runtime_resolve，这个函数的主要作用就是找到某个符号的地址，并把它写到与此符号相关的GOT项中，然后将控制转移到目标函数，而后面的GOT[3]，GOT[4]…都是通过_dl_fixup 添加的。



![](./images/289baeed-3f91-3651-b81b-159632d1cf45.png)





##参考：
http://www.lizhaozhong.info/archives/524
http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf
http://rickgray.me/2015/08/07/use-gdb-to-study-got-and-plt.html
http://blog.csdn.net/anzhsoft/article/details/18776111

