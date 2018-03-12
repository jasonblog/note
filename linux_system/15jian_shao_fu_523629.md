# 15(減少複製)


計算機的存儲結構是層次性的，從快到慢，代價從高到低，容量從小到大，寄存器，L1 cache，L2 cache，直到磁盤，甚至比磁盤更慢速的磁帶機，因此在程序運行時，不可避免的會有複製,這一點很重要，從優化的角度看，很多時候都是為了減少複製的代價，比如如果已知磁盤的讀寫是順序的，這樣採用DIRECTIO是較好的，直接讀到用戶內存上，而不需要先讀到內核內存上，然後再複製到用戶內存，這個例子我打算在未來的試驗中給出，當然DirectIO相當於程序員自己實現緩存，在小規模數據讀寫上並沒有優勢，因為節省的這些複製開銷微乎其微。

本文通過減少cache line回填（也是一種複製)，來說明減少複製獲得的收益。首先了解一些背景知識：

CPU通過芯片緩存(L1 Cache)和內存進行數據交互。而交互的單位叫做cache line,大小為2的冪，32或64字節,虛擬內存頁面大小為4KB。cache line的交互分為兩種回填(refill)和回寫(write-back)兩種。假定CPU需要從虛擬內存讀取一個字節的操作數，其地址為0xFFFFFFA3，cache line的大小為32字節，則CPU需要將地址0xFFFFFFA0地址開始的32個字節全部讀入，填充出一個完整的cache line後，然後從該cache line的第4個字節處取得該字節的內容。如果後繼的指令也需要同樣從cache行中讀，那麼填充cache line是值得的；否則，額外的填充cache line的時間就是浪費。因此指令和數據的局部性越好，越符合cache line的設計要求。


我們都非常熟悉的memset函數，如果我們手寫一個memset可能不如庫函數memset的實現性能高，為什麼呢？其中一個主要優化技術稱之為non-temporal，其基本思想是，如果要寫入的內存數據無用時，直接寫入，而不需要回填cache line。涉及的指令包括movnti,movntdq,sfence等。通俗點說，我們要將一個字符(char)，memset在一片內存上，而這片內存上原有的數據顯然沒有用了，即不需要將這片數據的內容先refill進cacheline，在cacheline中改寫瞭然後再寫回內存，只需要直接將數據寫入內存即可。庫函數的memset使用的是通用寄存器，而不是SSE寄存器，使用了movnti指令，通過objdump指令可以將庫函數的memset代碼導出，參見在本文的最後。

為什麼一定要refill呢，不refill不可以嗎？假定我們對一片內存寫入1個字節（假定一條cache line是32字節），數據交互的單位是cache Line,系統怎麼知道另外的31個字節是什麼呢？這一寫回，這1個字節是對的，另外的31個字節就未定義了。因此小數據的讀寫refill是有必要的，但是，對於大片內存的寫入，顯然refill是可以避免的，intel提供的non-temperal方法也就是為這個目的服務的，即本文的減少複製的優化思想。

代碼中還有一些技巧不再詳述，如果有反饋，我再寫個續篇解答，詳細請參見下列代碼。


```c
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
typedef unsigned char __attribute__((aligned(16))) fill_t[16];
using namespace std;
void naive_memset(void* page, unsigned char fill, size_t count)
{
    unsigned char* dst = (unsigned char*)page;
    unsigned char* end = dst + count;

    for (; dst < end;) {
        *dst++ = fill;
    };
};

void my_memset(void* page, unsigned char fill, size_t count)
{
    unsigned char* dst = (unsigned char*)page;
    fill_t dfill;

    for (size_t i = 0; i < 16;) {
        dfill[i++] = fill;
    }

    __asm__ __volatile__(
        " movdqa (%0),%%xmm0/n"
        " movdqa %%xmm0,%%xmm1/n"
        " movdqa %%xmm0,%%xmm2/n"
        " movdqa %%xmm0,%%xmm3/n"
        " movdqa %%xmm0,%%xmm4/n"
        " movdqa %%xmm0,%%xmm5/n"
        " movdqa %%xmm0,%%xmm6/n"
        " movdqa %%xmm0,%%xmm7/n"
        :: "r"(dfill)
    );

    while (((long)dst & 0xF) && (count > 0)) {
        *dst++ = fill;
        count--;
    }

    size_t m_loop = count / 128;
    size_t r = count % 128;

    for (size_t i = 0; i < m_loop; ++i) {
        __asm__(
            "  movntdq %%xmm0, (%0)/n"
            "  movntdq %%xmm1, 16(%0)/n"
            "  movntdq %%xmm2, 32(%0)/n"
            "  movntdq %%xmm3, 48(%0)/n"
            "  movntdq %%xmm4, 64(%0)/n"
            "  movntdq %%xmm5, 80(%0)/n"
            "  movntdq %%xmm6, 96(%0)/n"
            "  movntdq %%xmm7, 112(%0)/n"
            ::"r"(dst) :"memory");
        dst += 128;
    }

    for (int i = 0; i < r; ++i) {
        *dst++ = fill;
    }

    __asm__ __volatile__(
        " sfence /n "
        ::
    );
};
#if defined(__i386__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile("rdtsc" : "=A"(x));
    return x;
}
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned hi, lo;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((unsigned long long)lo) | (((unsigned long long)hi) << 32);
}
#endif
int main()
{
    const size_t s = 40 * 1024 * 1024;
    void* p = malloc(s);
    memset(p, 0x0, s);
    unsigned long long start = rdtsc();
    unsigned char* q = (unsigned char*)p;
#ifdef _NAIVE_MEM
    naive_memset(p, 0x1, s);
#endif
#ifdef _MY_MEM
    my_memset(p, 0x1, s);
#endif
#ifdef _MEM
    memset(p, 0x1, s);
#endif
    int sum = 0;

    for (int i = 0; i < s - 1; ++i) {
        sum  +=  *q;
        ++q;
    }

    cout << "sum:" << sum << endl;
    cout << "run time:" << rdtsc() - start << endl;
    free(p);
    return 0;
}

```

```sh
glibc的庫函數memset的彙編代碼：

0000003fb6279540 <memset>:
3fb6279540:   48 83 fa 07             cmp    $0x7,%rdx
3fb6279544:   48 89 f9                  mov    %rdi,%rcx
3fb6279547:   0f 86 96 00 00 00   jbe    3fb62795e3 <memset+0xa3>
3fb627954d:   49 b8 01 01 01 01 01    mov    $0x101010101010101,%r8
3fb6279554:   01 01 01
3fb6279557:   40 0f b6 c6             movzbl %sil,%eax
3fb627955b:   4c 0f af c0               imul   %rax,%r8
3fb627955f:   f7 c7 07 00 00 00     test   $0x7,%edi
3fb6279565:   74 1a                      je     3fb6279581 <memset+0x41>
3fb6279567:   66 0f 1f 84 00 00 00    nopw   0x0(%rax,%rax,1)
3fb627956e:   00 00
3fb6279570:   40 88 31                mov    %sil,(%rcx)
3fb6279573:   48 ff ca                  dec    %rdx
3fb6279576:   48 ff c1                  inc    %rcx
3fb6279579:   f7 c1 07 00 00 00   test   $0x7,%ecx
3fb627957f:   75 ef                       jne    3fb6279570 <memset+0x30>
3fb6279581:   48 89 d0                mov    %rdx,%rax
pennyliang5:   48 c1 e8 06           shr    $0x6,%rax
3fb6279588:   74 3e                     je     3fb62795c8 <memset+0x88>
3fb627958a:   48 81 fa c0 d4 01 00    cmp    $0x1d4c0,%rdx
3fb6279591:   73 6d                    jae    3fb6279600 <memset+0xc0>
3fb6279593:   66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)
3fb6279599:   0f 1f 80 00 00 00 00    nopl   0x0(%rax)
3fb62795a0:   4c 89 01                mov    %r8,(%rcx)
3fb62795a3:   4c 89 41 08             mov    %r8,0x8(%rcx)
3fb62795a7:   4c 89 41 10             mov    %r8,0x10(%rcx)
3fb62795ab:   4c 89 41 18             mov    %r8,0x18(%rcx)
3fb62795af:   4c 89 41 20             mov    %r8,0x20(%rcx)
3fb62795b3:   4c 89 41 28             mov    %r8,0x28(%rcx)
3fb62795b7:   4c 89 41 30             mov    %r8,0x30(%rcx)
3fb62795bb:   4c 89 41 38             mov    %r8,0x38(%rcx)

3fb62795bf:   48 83 c1 40             add    $0x40,%rcx
3fb62795c3:   48 ff c8                dec    %rax
3fb62795c6:   75 d8                   jne    3fb62795a0 <memset+0x60>
3fb62795c8:   83 e2 3f                and    $0x3f,%edx
3fb62795cb:   48 89 d0                mov    %rdx,%rax
3fb62795ce:   48 c1 e8 03             shr    $0x3,%rax
3fb62795d2:   74 0c                   je     3fb62795e0 <memset+0xa0>
3fb62795d4:   4c 89 01                mov    %r8,(%rcx)
3fb62795d7:   48 83 c1 08             add    $0x8,%rcx
3fb62795db:   48 ff c8                dec    %rax
3fb62795de:   75 f4                   jne    3fb62795d4 <memset+0x94>
3fb62795e0:   83 e2 07                and    $0x7,%edx
3fb62795e3:   48 85 d2                test   %rdx,%rdx
3fb62795e6:   74 0b                   je     3fb62795f3 <memset+0xb3>
3fb62795e8:   40 88 31                mov    %sil,(%rcx)
3fb62795eb:   48 ff c1                inc    %rcx
3fb62795ee:   48 ff ca                dec    %rdx
3fb62795f1:   75 f5                   jne    3fb62795e8 <memset+0xa8>
3fb62795f3:   48 89 f8                mov    %rdi,%rax
3fb62795f6:   c3                      retq
3fb62795f7:   66 0f 1f 84 00 00 00    nopw   0x0(%rax,%rax,1)
3fb62795fe:   00 00
3fb6279600:   4c 0f c3 01             movnti %r8,(%rcx)
3fb6279604:   4c 0f c3 41 08          movnti %r8,0x8(%rcx)
3fb6279609:   4c 0f c3 41 10          movnti %r8,0x10(%rcx)
3fb627960e:   4c 0f c3 41 18          movnti %r8,0x18(%rcx)
3fb6279613:   4c 0f c3 41 20          movnti %r8,0x20(%rcx)
3fb6279618:   4c 0f c3 41 28          movnti %r8,0x28(%rcx)
3fb627961d:   4c 0f c3 41 30          movnti %r8,0x30(%rcx)
3fb6279622:   4c 0f c3 41 38          movnti %r8,0x38(%rcx)
3fb6279627:   48 83 c1 40             add    $0x40,%rcx
3fb627962b:   48 ff c8                dec    %rax
3fb627962e:   75 d0                   jne    3fb6279600 <memset+0xc0>
3fb6279630:   0f ae f8                sfence

3fb6279633:   eb 93                   jmp    3fb62795c8 <memset+0x88>
3fb6279635:   90                      nop
3fb6279636:   90                      nop
3fb6279637:   90                      nop
3fb6279638:   90                      nop
3fb6279639:   90                      nop
3fb627963a:   90                      nop
3fb627963b:   90                      nop
3fb627963c:   90                      nop
3fb627963d:   90                      nop
3fb627963e:   90                      nop
3fb627963f:   90                      nop

```


上回提到了用movntdq指令寫xmm寄存器的方式，可以獲得性能上的收益，並且指出在默認的情況下，在發生寫入的時候，需要先從低層次存儲層次refill到高層次存儲層次（可以簡單認為從內存refill進cache)，然後再cache中更新，再write back，特別強調的是，這是默認的方式。

實際上，高一級存儲層次，向低一級存儲層次寫入有兩種方式，一種是默認的write back，一種是write through。而我們使用的movntdq指令就是write through方式，必須通過彙編的方式來實現，高級語言通常沒有這個效果。怎麼理解write back和write through的區別呢？

對於write back,因為我們知道機器存儲層次分為多層，為了保證一致性，必須每一層相同的數據都是一致的，不可能分秒都是一致的，但是在不影響計算邏輯的前提下，需要有一致性的保證，那麼write back方式在寫的時候有兩種情況：A）當寫入的地址恰好在cache的某條cache line中，那麼直接在cache中寫入數據，並且標記該cache line為dirty，此後不發生對內存實際地址的寫回，直到發生cache被替換策略時，才將dirty的這條cache line寫入到內存中。B）當寫入的地址恰好不在cache中（任何一條cache line都不match），首先需要執行一次refill操作，將內存中的數據讀入到cache line中，然後將數據寫入該cache line，此後不發生對內存地址的寫回，直到發生cache被替換策略時，才將dirty的這條cache lien寫入內存。

舉個例子：

某個程序需要執行這樣幾條命令，按順序如下：

（1）在地址0x00FFFF00處寫入64個字節的數據[PENNYLIANG...]

（2）其他命令（64個字節保持在cache中，沒有被替換掉)

（3）讀取0x00FFFF00處的64個字節的數據

(4) 某條命令（該命令將[PENNYLIANG...]這段數據換出了cache(L1 cache只有32k，必然會有被換出的可能)

（5）讀取0x00FFFF00處的64個字節的數據

(6) 某條命令（該命令將[PENNYLIANG...]這段數據又一次換出了cache

(7) 在地址0x00FFFF00處寫入64個字節的數據[奇淫巧計...]

(8) 某條命令（該命令將[奇淫巧計...]這段數據又一次換出了cache



從此前介紹的內容，我們不難得到這樣幾個結論：

1）在命令(4)之前，寫入的數據總是保持在cache中，命令3從cache中讀取的[PENNYLIANG...]

2) 命令(4)導致了cache中數據和內存的一次同步

3) 命令(5)發生了一次cache read miss，需要從內存中refill[PENNYLIANG...]到cache中。

4）命令(7)發生了一次cache write miss,需要兩次內存訪問實現寫入，第一次refill，另一次在指令(8)時write back。



不難得到，write back在程序局部性好的情況下，是非常有利的，只要cache 命中了，就可以只在cache中轉，而不發生寫回，比如指令(3)就直接在cache中得到，如果對這個數據多次寫，那麼直接在cache中發生，根本不會每次寫都體現在內存上。



在理解了write back後，就很容易理解write through了，write through直接從cache寫穿到內存，只有單向的過程，沒有內存到cache的refill，相當於減少了低層存儲層次數據向高層存儲層次的複製過程，在大規模數據寫入的時候才能體現威力。



我們這裡舉得例子是cache，或者說L1 cache和內存的關係，廣義上看是高層次存儲結構和低層次存儲結構的關係，內存和磁盤也可以構成這樣一個write back和write through的情況，可以看到在小數據量的情況下write back總是最好的，而當發生大量數據寫入時write through的方式會更有優勢，以後我們會給出這個例子。



參考閱讀：

http://www-903.ibm.com/kr/techinfo/xseries/download/write_back_vs_write_through.pdf

http://en.wikipedia.org/wiki/CPU_cache
