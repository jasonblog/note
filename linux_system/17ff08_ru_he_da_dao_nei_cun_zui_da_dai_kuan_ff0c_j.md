# 17（如何达到内存最大带宽，精简指令，预取和NT）


用复杂指令的方法，没有任何优化的余地，优化的工作全仰仗intel工程师了，采用精简指令，将循环展开，可以使用更多的技巧包括预取，NT，关于non-temp该系列前面的一篇文章已经详细介绍不在多说。另外这里用到了r8开始的寄存器，这些是64位新增的，通常在这种流式数据中普遍采用。

本系列读者主要面向在校学生，希望同学们用我给出的代码多做实验，多想实验改进的方法，提高编码能力，我们所学的体系结构等教材，大部分是没有提供可执行的源码，如果没有系统的实验，很难有深入的理解。

“深入理解计算机系统”这本书不错，但很多例子都不是可执行的程序，大部分同学看过即忘，很难形成系统的认识，把心定下来，潜心研究，系统实验，是本系列博客想达到的目的，如果能影响到一些人，那真是善莫大焉。



下面一篇第18集，将会探讨SSE指令集，XMM寄存器，和内存读写模式。最后将给出这几种方法的实验数据。



注：因为是实验，因此做了对齐的假定，只限于实验使用，因为如果要做对齐势必增加代码，而失去了对主旨的关注。


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
        "    subq $8, %rdx/n"   //一次处理8个quad words
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


