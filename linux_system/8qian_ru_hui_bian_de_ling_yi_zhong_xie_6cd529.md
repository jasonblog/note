# 8(嵌入汇编的另一种写法)


可能大家厌倦了传统的在C，C++中嵌入代码的痛苦，%要写两遍，而且一大堆规范，嵌套起来看得很不爽，好，下面让大家看另一种纯正的嵌入，大家可能会问，太“淫”了吧，毫无价值啊，要知道如果你想要做一些乱序的话，先写一遍函数，然后Objdump例如我们先写了这个add函数，然后用下面提到的这种方法，控制好计算顺序，你懂的，再多写就废话了。

关键一点：asm("add2: ");是一定要加的，否则编不过，将来说道乱序还会用到这里的方法，先做个引子，如果大家都懂，就当我班门弄斧，不过我在网上确实还鲜有看到这种代码。


如果改成c++的代码，还需要加一个函数说明，大家自己尝试吧。


```cpp
#include <stdio.h>
/* objdmp -d test后摘录的add函数的代码段，直接复制到就可以构成一个函数嵌入
0000000000400498 <add>:
  400498:      55                      push   %rbp
  400499:      48 89 e5            mov    %rsp,%rbp
  40049c:       89 7d ec            mov    %edi,0xffffffffffffffec(%rbp)
  40049f:        89 75 e8           mov    %esi,0xffffffffffffffe8(%rbp)
  4004a2:       8b 45 e8           mov    0xffffffffffffffe8(%rbp),%eax
  4004a5:       03 45 ec            add    0xffffffffffffffec(%rbp),%eax
  4004a8:       89 45 fc             mov    %eax,0xfffffffffffffffc(%rbp)
  4004ab:       8b 45 fc             mov    0xfffffffffffffffc(%rbp),%eax
  4004ae:       c9                      leaveq 
  4004af:       c3                       retq   
*/
int add(int a, int b)
{
        int ret = a+b;
        return ret;
};
asm("   .text                        ");
asm("   .type  add2, @function       ");
asm("add2:                            ");
asm("   push   %rbp                  ");
asm("   mov    %rsp,%rbp             ");
asm("   mov    %edi,0xffffffffffffffec(%rbp) ");
asm("   mov    %esi,0xffffffffffffffe8(%rbp) ");
asm("   mov    0xffffffffffffffe8(%rbp),%eax ");
asm("   add    0xffffffffffffffec(%rbp),%eax ");
asm("   mov    %eax,0xfffffffffffffffc(%rbp) ");
asm("   mov    0xfffffffffffffffc(%rbp),%eax ");
asm("   leaveq                               ");
asm("   retq                                 ");


int main(void)
{
        int a=1,b=1;
        printf("sum1:%d,sum2:%d/n",add(a,b),add2(a,b));
        return 0;
}
```
 

编译方法：gcc test.c -o test