# 17（如何達到內存最大帶寬，精簡指令，預取和NT）


用複雜指令的方法，沒有任何優化的餘地，優化的工作全仰仗intel工程師了，採用精簡指令，將循環展開，可以使用更多的技巧包括預取，NT，關於non-temp該系列前面的一篇文章已經詳細介紹不在多說。另外這裡用到了r8開始的寄存器，這些是64位新增的，通常在這種流式數據中普遍採用。

本系列讀者主要面向在校學生，希望同學們用我給出的代碼多做實驗，多想實驗改進的方法，提高編碼能力，我們所學的體系結構等教材，大部分是沒有提供可執行的源碼，如果沒有系統的實驗，很難有深入的理解。

“深入理解計算機系統”這本書不錯，但很多例子都不是可執行的程序，大部分同學看過即忘，很難形成系統的認識，把心定下來，潛心研究，系統實驗，是本系列博客想達到的目的，如果能影響到一些人，那真是善莫大焉。



下面一篇第18集，將會探討SSE指令集，XMM寄存器，和內存讀寫模式。最後將給出這幾種方法的實驗數據。



注：因為是實驗，因此做了對齊的假定，只限於實驗使用，因為如果要做對齊勢必增加代碼，而失去了對主旨的關注。


```c
#include "stdlib.h"
#include "string.h"
#if defined(__i386__)
static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile(".byte 0x0f, 0x31" : "=A"(x));
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
__asm__(".text/n"
        ".type  m_b_64, @function/n"
        "m_b_64:push   %rbp/n"
        "mov    %rsp,%rbp/n"
        "mov    %rdx,%rcx/n"
        "rep    movsq/n"
        "leaveq/n"
        "retq/n");
__asm__(".text/n"
        ".type m_c ,@function/n"
        "m_c:push %rbp/n"
        "mov    %rsp,%rbp/n"
        "cp: movq (%rsi),%rax/n"
        "    movq %rax,(%rdi)/n"
        "    addq  $8,%rsi/n"
        "    addq  $8,%rdi/n"
        "    dec  %rdx/n"
        "    jnz  cp/n"
        "    leaveq/n"
        "    retq/n");
__asm__(".text/n"
        ".type m_c_2 ,@function/n"
        "m_c_2:push %rbp/n"
        "mov    %rsp,%rbp/n"
        "co: prefetcht0 1024(%rsi)/n"
        "    movq (%rsi),%rax/n"
        "    movq 8(%rsi),%rbx/n"
        "    movq 16(%rsi),%rcx/n"
        "    movq 24(%rsi),%r8/n"
        "    movq 32(%rsi),%r9/n"
        "    movq 40(%rsi),%r10/n"
        "    movq 48(%rsi),%r11/n"
        "    movq 56(%rsi),%r12/n"
        "    movnti %rax,(%rdi)/n"
        "    movnti %rbx,8(%rdi)/n"
        "    movnti %rcx,16(%rdi)/n"
        "    movnti %r8,24(%rdi)/n"
        "    movnti %r9,32(%rdi)/n"
        "    movnti %r10,40(%rdi)/n"
        "    movnti %r11,48(%rdi)/n"
        "    movnti %r12,56(%rdi)/n"
        "    addq  $64,%rsi/n"
        "    addq  $64,%rdi/n"
        "    subq $8, %rdx/n"   //一次處理8個quad words
        "    jnz  co/n"
        "    leaveq/n"
        "    retq/n");
int main(void)
{
    int bytes_cnt = 32 * 1024 * 1024; //32M bytes
    int word_cnt = bytes_cnt / 2; //16M words
    int dword_cnt = word_cnt / 2; //8M  double words
    int qdword_cnt = dword_cnt / 2; //4M  quad words
    char* from = (char*) malloc(bytes_cnt);
    char* to = (char*)malloc(bytes_cnt);
    memset(from, 0x1, bytes_cnt);
    memset(to, 0x0, bytes_cnt);
    unsigned long long start;
    unsigned long long end;
    int i;

    for (i = 0; i < 10; ++i) {
        start = rdtsc();
        m_b_64(to, from, qdword_cnt);
        end = rdtsc();
        printf("m_b_64 use time:/t%d/n", end - start);
    }

    for (i = 0; i < 10; ++i) {
        start = rdtsc();
        m_c(to, from, qdword_cnt);
        end = rdtsc();
        printf("m_c use time:/t%d/n", end - start);
    }

    for (i = 0; i < 10; ++i) {
        start = rdtsc();
        m_c_2(to, from, qdword_cnt);
        end = rdtsc();
        printf("m_c_2 use time:/t%d/n", end - start);
    }

    /*********use to make sure cpy is ok*********

    int sum = 0;
    for(i=0;i<bytes_cnt;++i)
            sum+=to[i];
    printf("sum:%d/n",sum);
    ********************************************/
    return 0;
}
```


