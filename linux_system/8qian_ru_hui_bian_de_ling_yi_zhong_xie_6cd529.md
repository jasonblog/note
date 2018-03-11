# 8(嵌入彙編的另一種寫法)


可能大家厭倦了傳統的在C，C++中嵌入代碼的痛苦，%要寫兩遍，而且一大堆規範，嵌套起來看得很不爽，好，下面讓大家看另一種純正的嵌入，大家可能會問，太“淫”了吧，毫無價值啊，要知道如果你想要做一些亂序的話，先寫一遍函數，然後Objdump例如我們先寫了這個add函數，然後用下面提到的這種方法，控制好計算順序，你懂的，再多寫就廢話了。

關鍵一點：asm("add2: ");是一定要加的，否則編不過，將來說道亂序還會用到這裡的方法，先做個引子，如果大家都懂，就當我班門弄斧，不過我在網上確實還鮮有看到這種代碼。


如果改成c++的代碼，還需要加一個函數說明，大家自己嘗試吧。


```cpp
#include <stdio.h>
/* objdmp -d test後摘錄的add函數的代碼段，直接複製到就可以構成一個函數嵌入
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
 

編譯方法：gcc test.c -o test