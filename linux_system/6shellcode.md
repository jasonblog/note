# 6(shellcode)


 shellcode已經接近病毒的範疇了，但本文不打算讓大家學習病毒，只是通過一個簡單的例子，告訴大家shellcode是怎麼來的。

 知道的人覺得太naive了，不知道的知道是怎麼來的就好，不必深究。

 奇淫巧計的宗旨就是在於儘可能系統地給出一些不常見的代碼，初步瞭解是怎麼回事，如果有興趣，引發讀者繼續研究，只是鋪路石，不是柺棍。

 如果大家需要繼續展開，我會在後文中繼續補充，如果大家都認為很naive，巧計6只此一篇，不再展開了。

 
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
 

//用g++ -g -m32 test.cpp -o test編譯

//用objdump -d test看彙編代碼，不同的環境hello的解釋可能有差異，只需要按葫蘆畫瓢修改好就ok，上面註釋的代碼是本測試環境

//的彙編代碼，從而形成了shellcode，並正確執行。