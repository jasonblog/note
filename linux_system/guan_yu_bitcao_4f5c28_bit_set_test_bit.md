# 关于bit操作(bit_set test_bit)


今天发现一个奇怪的现象，用linux内核的bitops.h里面的bit_set和test_bit居然没有不用汇编的代码快。记录下来，备忘。

下面是可执行程序，THUIRDB暂时没有bitops里面的实现，改用我自己写的实现了。当然bitops里的实现，nr可以很大，而我的代码nr只能是0-63之间的一个数，对THUIRDB的应用来说，足够了。


```c
#include "stdint.h"
#include <iostream>
using namespace std;
#define BITOP_ADDR(x) "=m" (*(volatile long *) (x))
#define ADDR                            BITOP_ADDR(addr)
#ifdef CONFIG_SMP
#define LOCK_PREFIX /
".section .smp_locks,/"a / "/n"   /
_ASM_ALIGN "/n"                 /
_ASM_PTR "661f/n" /
".previous/n"                   /
"661:/n/tlock; "
#else
#define LOCK_PREFIX ""
#endif
static inline void
set_bit(unsigned int nr, volatile unsigned long* addr)
{
    __asm__ volatile(LOCK_PREFIX "bts %1,%0"
                     : BITOP_ADDR(addr) : "Ir"(nr) : "memory");
}
static inline bool test_bit(int nr, volatile const unsigned long* addr)
{
    int oldbit;
    asm volatile("bt %2,%1/n/t"
                 "sbb %0,%0"
                 : "=r"(oldbit)
                 : "m"(*(unsigned long*)addr), "Ir"(nr));
    return oldbit != 0;
}
static  void _set_bit(int nr, uint64_t* addr)
{
    addr[0] |= (1UL << nr);
};
static bool _test_bit(int nr, uint64_t* addr)
{
    return (0 < (addr[0] & (1UL << nr)));
};
int main(void)
{
    uint64_t bit_arr = 0;
#ifdef T1

    for (int i = 0; i < 1000000; ++i) {
        _set_bit(id, &bit_arr);
    }

#endif
#ifdef T2

    for (int i = 0; i < 1000000; ++i) {
        set_bit(id, &bit_arr);
    }

#endif
#ifdef T3

    for (int i = 0; i < 1000000; ++i) {
        _test_bit(id, &bit_arr);
    }

#endif
#ifdef T4

    for (int i = 0; i < 1000000; ++i) {
        test_bit(id, &bit_arr);
    }

#endif
    return 0;
}
```

以下是反汇编的结果，可以看出用内核的方法，代码短小，但效率较低，用移位与或的方法代码多，但效率高。

```sh
000000000040067c <_ZL7set_bitjPVm>: //set_bit
  40067c:       55                      push   %rbp
  40067d:       48 89 e5                mov    %rsp,%rbp
  400680:       89 7d fc                mov    % edi,-0x4(%rbp)
  400683:       48 89 75 f0             mov    %rsi,-0x10(%rbp)
  400687:       48 8b 45 f0             mov    -0x10(%rbp),%rax
  40068b:       8b 55 fc                mov    -0x4(%rbp),% edx
  40068e:       0f ab 10                bts    % edx,(%rax)
  400691:       c9                      leaveq 
  400692:       c3                      retq   
0000000000400693 <_ZL8test_bitiPVKm>://test_bit
  400693:       55                      push   %rbp
  400694:       48 89 e5                mov    %rsp,%rbp
  400697:       89 7d ec                mov    % edi,-0x14(%rbp)
  40069a:       48 89 75 e0             mov    %rsi,-0x20(%rbp)
  40069e:       48 8b 45 e0             mov    -0x20(%rbp),%rax
  4006a2:       8b 55 ec                mov    -0x14(%rbp),% edx
  4006a5:       0f a3 10                bt     % edx,(%rax)
  4006a8:       19 c0                   sbb    % eax,% eax
  4006aa:       89 45 fc                mov    % eax,-0x4(%rbp)
  4006ad:       83 7d fc 00             cmpl   $0x0,-0x4(%rbp)
  4006b1:       0f 95 c0                setne  %al
  4006b4:       c9                      leaveq 
  4006b5:       c3                      retq   
00000000004006b6 <_ZL8_set_bitiPm>://_set_bit
  4006b6:       55                      push   %rbp
  4006b7:       48 89 e5                mov    %rsp,%rbp
  4006ba:       53                      push   %rbx
  4006bb:       89 7d f4                mov    % edi,-0xc(%rbp)
  4006be:       48 89 75 e8             mov    %rsi,-0x18(%rbp)
  4006c2:       48 8b 45 e8             mov    -0x18(%rbp),%rax
  4006c6:       48 8b 10                mov    (%rax),%rdx
  4006c9:       8b 45 f4                mov    -0xc(%rbp),% eax
  4006cc:       bb 01 00 00 00          mov    $0x1,% ebx
  4006d1:       48 89 de                mov    %rbx,%rsi
  4006d4:       89 c1                   mov    % eax,% ecx
  4006d6:       48 d3 e6                shl    %cl,%rsi
  4006d9:       48 89 f0                mov    %rsi,%rax
  4006dc:       48 09 c2                or     %rax,%rdx
  4006df:       48 8b 45 e8             mov    -0x18(%rbp),%rax
  4006e3:       48 89 10                mov    %rdx,(%rax)
  4006e6:       5b                      pop    %rbx
  4006e7:       c9                      leaveq 
  4006e8:       c3                      retq   
00000000004006e9 <_ZL9_test_bitiPm>://_test_bit
  4006e9:       55                      push   %rbp
  4006ea:       48 89 e5                mov    %rsp,%rbp
  4006ed:       53                      push   %rbx
  4006ee:       89 7d f4                mov    % edi,-0xc(%rbp)
  4006f1:       48 89 75 e8             mov    %rsi,-0x18(%rbp)
  4006f5:       48 8b 45 e8             mov    -0x18(%rbp),%rax
  4006f9:       48 8b 10                mov    (%rax),%rdx
  4006fc:       8b 45 f4                mov    -0xc(%rbp),% eax
  4006ff:       48 89 d3                mov    %rdx,%rbx
  400702:       89 c1                   mov    % eax,% ecx
  400704:       48 d3 eb                shr    %cl,%rbx
  400707:       48 89 d8                mov    %rbx,%rax
  40070a:       83 e0 01                and    $0x1,% eax
  40070d:       5b                      pop    %rbx
  40070e:       c9                      leaveq 
  40070f:       c3                      retq   
```

T2用-O2优化后的代码，内联到main里面从400910到40093b

```sh

00000000004008f0 <main>:
  4008f0:       55                      push   %rbp
  4008f1:       31 c0                   xor    %eax,%eax
  4008f3:       53                      push   %rbx
  4008f4:       48 83 ec 18             sub    $0x18,%rsp
  4008f8:       48 8d 4c 24 08          lea    0x8(%rsp),%rcx
  4008fd:       48 c7 44 24 08 00 00    movq   $0x0,0x8(%rsp)
  400904:       00 00 
  400906:       66 2e 0f 1f 84 00 00    nopw   %cs:0x0(%rax,%rax,1)
  40090d:       00 00 00 
  400910:       89 c2                   mov    %eax,%edx
  400912:       83 e2 3f                and    $0x3f,%edx
  400915:       0f ab 11                bts    %edx,(%rcx)
  400918:       83 c0 01                add    $0x1,%eax
  40091b:       3d 40 42 0f 00          cmp    $0xf4240,%eax
  400920:       75 ee                   jne    400910 <main+0x20>
  400922:       48 8b 74 24 08          mov    0x8(%rsp),%rsi
  400927:       bf e0 0d 60 00          mov    $0x600de0,%edi
  40092c:       e8 67 fe ff ff          callq  400798 <_ZNSo9_M_insertImEERSoT_@plt>
  400931:       48 89 c3                mov    %rax,%rbx
  400934:       48 8b 00                mov    (%rax),%rax
  400937:       48 8b 40 e8             mov    -0x18(%rax),%rax
  40093b:       48 8b ac 03 f0 00 00    mov    0xf0(%rbx,%rax,1),%rbp
  400942:       00 
  400943:       48 85 ed                test   %rbp,%rbp
  400946:       74 3f                   je     400987 <main+0x97>
  400948:       80 7d 38 00             cmpb   $0x0,0x38(%rbp)
  40094c:       74 20                   je     40096e <main+0x7e>
  40094e:       0f b6 45 43             movzbl 0x43(%rbp),%eax
  400952:       48 89 df                mov    %rbx,%rdi
  400955:       0f be f0                movsbl %al,%esi
  400958:       e8 5b fe ff ff          callq  4007b8 <_ZNSo3putEc@plt>
  40095d:       48 89 c7                mov    %rax,%rdi
  400960:       e8 23 fe ff ff          callq  400788 <_ZNSo5flushEv@plt>
  400965:       48 83 c4 18             add    $0x18,%rsp
  400969:       31 c0                   xor    %eax,%eax
  40096b:       5b                      pop    %rbx
  40096c:       5d                      pop    %rbp
  40096d:       c3                      retq   
  40096e:       48 89 ef                mov    %rbp,%rdi
  400971:       e8 c2 fd ff ff          callq  400738 <_ZNKSt5ctypeIcE13_M_widen_initEv@plt>
  400976:       48 8b 45 00             mov    0x0(%rbp),%rax
  40097a:       be 0a 00 00 00          mov    $0xa,%esi
  40097f:       48 89 ef                mov    %rbp,%rdi
  400982:       ff 50 30                callq  *0x30(%rax)
  400985:       eb cb                   jmp    400952 <main+0x62>
  400987:       e8 9c fd ff ff          callq  400728 <_ZSt16__throw_bad_castv@plt>
  40098c:       90                      nop
  40098d:       90                      nop
  40098e:       90                      nop
  40098f:       90                      nop
  ```
  
  