# 6(shellcode)


 shellcode已经接近病毒的范畴了，但本文不打算让大家学习病毒，只是通过一个简单的例子，告诉大家shellcode是怎么来的。

 知道的人觉得太naive了，不知道的知道是怎么来的就好，不必深究。

 奇淫巧计的宗旨就是在于尽可能系统地给出一些不常见的代码，初步了解是怎么回事，如果有兴趣，引发读者继续研究，只是铺路石，不是拐棍。

 如果大家需要继续展开，我会在后文中继续补充，如果大家都认为很naive，巧计6只此一篇，不再展开了。

 
```c
#include<stdio.h>
/*
08048464 <_Z5helloii>:
 8048464:       55                      push   %ebp
 8048465:       89 e5                   mov    %esp,%ebp
 8048467:       8b 45 0c                mov    0xc(%ebp),%eax
 804846a:       03 45 08                add    0x8(%ebp),%eax
 804846d:       5d                      pop    %ebp
 804846e:       c3                      ret    
 804846f:       90                      nop 
*/
int hello(int a,int b)
{
        return a+b;
}
char shellcode[]="/x55"
                 "/x89/xe5"
                 "/x8b/x45/x0c"
                 "/x03/x45/x08"
                 "/x5d"
                 "/xc3"
                 "/x90";
int main(void)
{
        int (*func)(int a, int b);
        func = (int(*)(int, int)) shellcode;
        int ret = (int)(*func)(4,7);
        printf("ret:%d/n",ret);
        return 0;
}
```
 

//用g++ -g -m32 test.cpp -o test编译

//用objdump -d test看汇编代码，不同的环境hello的解释可能有差异，只需要按葫芦画瓢修改好就ok，上面注释的代码是本测试环境

//的汇编代码，从而形成了shellcode，并正确执行。