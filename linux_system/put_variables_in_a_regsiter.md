# put variables in a regsiter


```c

#define max(a,b) ({     \
        typeof (a) _a = (a);    \
        typeof (b) _b = (b);    \
        _a > _b ? _a:_b;        \
        })

#include "stdio.h"
int main(void)
{
        register int a asm("eax") = 2;
        int b = 3;
        register int c = 4;

        printf("%d,%d",a,max(a,c));
        return 0;
}
```

上面這個例子中a，b，c 那個能將變量存儲在寄存器中，而不是棧中呢？答案是a,b。但是a的好處是可以指定寄存器，而c只能放在寄存器中，無法指定

objdump -d出來就能看到

```sh
00000000004005ac <main>:
  4005ac:       55                      push   %rbp
  4005ad:       48 89 e5                mov    %rsp,%rbp
  4005b0:       53                      push   %rbx
  4005b1:       48 83 ec 18             sub    $0x18,%rsp                      
  4005b5:       b8 02 00 00 00          mov    $0x2,%eax                //a
  4005ba:       c7 45 e4 03 00 00 00    movl   $0x3,-0x1c(%rbp)        //b
  4005c1:       bb 04 00 00 00          mov    $0x4,%ebx                //c
  4005c6:       89 45 e8                mov    %eax,-0x18(%rbp)
  4005c9:       89 5d ec                mov    %ebx,-0x14(%rbp)
  4005cc:       8b 55 e8                mov    -0x18(%rbp),%edx
  4005cf:       3b 55 ec                cmp    -0x14(%rbp),%edx
  4005d2:       7e 05       

```

另外增加一個typeof的例子充實下代碼，typeof是在編譯時獲取變量類型的一種方法，