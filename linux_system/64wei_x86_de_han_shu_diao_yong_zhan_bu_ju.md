# 64位x86的函数调用栈布局


在看本文之前，如果不了解x86的32位机的函数布局的话，建议先阅读一下前一篇文章《如何手工展开函数栈定位问题》—— http://blog.chinaunix.net/space.php?uid=23629988&do=blog&id=3029639

为啥还要就64位的情况单开一篇文章呢，难道64位与32位不一样吗？


还是先看测试代码：

```c
#include <stdlib.h>
#include <stdio.h>


static void test(void *p1, void *p2, int p3)
{
    p1 = p1;
    p2 = p2;
    p3 = p3;
}

int main()
{
    void *p1 = (void*)1;
    void *p2 = (void*)2;
    int p3 = 3;

    test(p1, p2, p3);

    return 0;
}
```


编译gcc -g -Wall test.c，调试进入test

```sh
(gdb) bt
#0 test (p1=0x1, p2=0x2, p3=3) at test.c:10
#1 0x0000000000400488 in main () at test.c:18
```

那么检查栈的内容

```sh
(gdb) x /16xg 0x7fffab620d00
0x7fffab620d00: 0x00007fffab620d30 0x0000000000400488
0x7fffab620d10: 0x00000000004004a0 0x0000000000000002
0x7fffab620d20: 0x0000000000000001 0x0000000300000000
0x7fffab620d30: 0x0000000000000000 0x00007f93bbaa11c4
0x7fffab620d40: 0x0000000000400390 0x00007fffab620e18
0x7fffab620d50: 0x0000000100000000 0x0000000000400459
0x7fffab620d60: 0x00007f93bc002c00 0x85b4aff07d2e87c7
0x7fffab620d70: 0x0000000000000000 0x00007fffab620e10
```

开始分析栈的内容：

```sh
1. 0x00007fffab620d30：为test调用者main的BP内容，没有问题；
2. 0x0000000000400488：为test的返回地址，与前面的bt输出相符，没有问题；
3. 0x00000000004004a0：——这个是什么东东？？！！
4. 0x0000000000000002， 0x0000000000000001， 0x0000000300000000：这里也有不少疑问啊？！
1. 这个0x00000003是第3个参数？因为是整数所以在64位的机器上，只使用栈的一个单元的一半空间？
2. 参数的顺序为什么是3,1,2呢？难道是因为前两个参数为指针，第三个参数为int有关？
```

我在工作中遇到了类似的问题，所以才特意写了上面的测试代码，就为了测试相同参数原型的函数调用栈的问题。看到这里，感觉很奇怪，对于上面两个问题很困惑啊。上网也没有找到64位的x86函数调用栈的特别的资料。


难道64位机与32位机有这么大的不同？！大家先想一下，答案马上揭晓。

当遇到疑难杂症时，汇编则是王道：


<div id="codeText" class="codeText" style="width: 645px; "><ol start="1" class="dp-css"><li>(gdb) disassemble main</li><li>Dump of assembler code for function main:</li><li>0x0000000000400459 <main+0>: push %rbp</main+0></li><li>0x000000000040045a <main+1>: mov %rsp,%rbp</main+1></li><li><font class="Apple-style-span" color="#0000f0">0x000000000040045d <main+4>: sub $0x20,%rsp</main+4></font></li><li><font class="Apple-style-span" color="#0000f0">0x0000000000400461 <main+8>: movq $0x1,-0x10(%rbp)</main+8></font></li><li><font class="Apple-style-span" color="#0000f0">0x0000000000400469 <main+16>: movq $0x2,-0x18(%rbp)</main+16></font></li><li><font class="Apple-style-span" color="#0000f0">0x0000000000400471 <main+24>: movl $0x3,-0x4(%rbp)</main+24></font></li><li><font class="Apple-style-span" color="#f00000">0x0000000000400478 <main+31>: mov -0x4(%rbp),%edx</main+31></font></li><li><font class="Apple-style-span" color="#f00000">0x000000000040047b <main+34>: mov -0x18(%rbp),%rsi</main+34></font></li><li><font class="Apple-style-span" color="#f00000">0x000000000040047f <main+38>: mov -0x10(%rbp),%rdi</main+38></font></li><li><font class="Apple-style-span" color="#f00000">0x0000000000400483 <main+42>: callq 0x400448 <test></test></main+42></font></li><li>0x0000000000400488 <main+47>: mov $0x0,%eax</main+47></li><li>0x000000000040048d <main+52>: leaveq</main+52></li><li>0x000000000040048e <main+53>: retq</main+53></li><li>End of assembler dump.</li></ol></div>

看红色部分的汇编代码，为调用test时的处理，原来64位机器上，调用test时，根本没有对参数进行压栈，所以上面对于栈内容的分析有误。后面的内存中存放的根本不是test的参数。看到汇编代码，我突然想起，由于64位cpu的寄存器比32位cpu的寄存器要多，所以gcc会尽量使用寄存器来传递参数来提高效率。

让我们重新运行程序，再次在test下查看寄存器内容：

<ol start="1" class="dp-css"><li>(gdb) info registers</li><li>rax 0x7f141fea1a60 139724411509344</li><li>rbx 0x7f14200c2c00 139724413742080</li><li>rcx 0x4004a0 4195488</li><li><font class="Apple-style-span" color="#f00000">rdx 0x3 3</font></li><li><font class="Apple-style-span" color="#f00000">rsi 0x2 2</font></li><li><font class="Apple-style-span" color="#f00000">rdi 0x1 1</font></li><li>rbp 0x7fff9c08d380 0x7fff9c08d380</li><li>rsp 0x7fff9c08d380 0x7fff9c08d380</li></ol>


与大家分享一下64位机器上调试时需要注意的一个问题：函数调用时，编译器会尽量使用寄存器来传递参数，这点与32位机有很大不同。在我们的调试中，要特别注意这点。


注：关于压栈顺序，参数的传递方式等等，都可以通过编译选项来指定或者禁止的。本文的情况为GCC的默认行为。
