# 16（如何达到内存最大带宽，复杂指令）


内存的使用可以说对程序员来说极其重要，特别在大规模数据处理中，如何达到内存带宽的极限是程序员追求的目标。

从本节开始，通过一系列的例子，来进行探讨。本系列全部采用汇编语言，使用的例子只有一个就是内存拷贝，这个最容易理解，最容易举例的例子。



本节给出的代码是baseline，也就是最差的方法，将来的方法都比这个要强的多，包括了精简指令，内存的预取，MMX寄存器，块复制等多项技术，希望读者朋友持续阅读。希望达到的目的包括：

（1）读完该小系列后，对汇编和内存的一些基本指令会有一定了解和应用

（2）对内存的原理，寄存器有深入理解



按照奇淫巧计的惯例，代码详细讲解将在续篇中给出。以下代码编译方式gcc -g -o test main.c。限64位机。


```c
#include "stdio.h"
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
asm(".text");
asm(".type  m_b_64, @function       ");
asm("m_b_64:push   %rbp                  ");
asm("       mov    %rsp,%rbp             ");
asm("       mov    %rdx,%rcx");
asm("       rep    movsq        ");
asm("       leaveq      ");
asm("       retq        ");
asm(".text");
asm(".type  m_b_32, @function       ");
asm("m_b_32:push   %rbp                  ");
asm("       mov    %rsp,%rbp             ");
asm("       mov    %rdx,%rcx");
asm("       rep    movsd        ");
asm("       leaveq  ");
asm("       retq    ");
asm(".text");
asm(".type  m_b_16, @function       ");
asm("m_b_16:push   %rbp                  ");
asm("       mov    %rsp,%rbp             ");
asm("       mov    %rdx,%rcx");
asm("       rep    movsw        ");
asm("       leaveq  ");
asm("       retq    ");
asm(".text");
asm(".type  m_b_8, @function       ");
asm("m_b_8:   push   %rbp                  ");
asm("       mov    %rsp,%rbp             ");
asm("       mov    %rdx,%rcx");
asm("       rep    movsb        ");
asm("       leaveq  ");
asm("       retq    ");
int main(void)
{
    int bytes_cnt = 32 * 1024 * 1024; //32M bytes
    int word_cnt = bytes_cnt / 2; //16M words
    int dword_cnt = word_cnt / 2; //8M  double words
    int qdword_cnt = dword_cnt / 2; //4M  quad words
    char* from = (char*) malloc(bytes_cnt);
    char* to = (char*)malloc(bytes_cnt);
    memset(from, 0x2, bytes_cnt);
    memset(to, 0x0, bytes_cnt);
    unsigned long long start;
    unsigned long long end;
    int i;

    for (i = 0; i < 10; ++i) {
        start = rdtsc();
        m_b_8(to, from, bytes_cnt);
        end = rdtsc();
        printf("m_b_8 use time:/t/t%d/n", end - start);
    }

    for (i = 0; i < 10; ++i) {
        start = rdtsc();
        m_b_16(to, from, word_cnt);
        end = rdtsc();
        printf("m_b_16 use time:/t%d/n", end - start);
    }

    for (i = 0; i < 10; ++i) {
        start = rdtsc();
        m_b_32(to, from, dword_cnt);
        end = rdtsc();
        printf("m_b_32 use time:/t%d/n", end - start);
    }

    for (i = 0; i < 10; ++i) {
        start = rdtsc();
        m_b_64(to, from, qdword_cnt);
        end = rdtsc();
        printf("m_b_64 use time:/t%d/n", end - start);
    }

    /*use to make sure cpy is ok******
    int sum = 0;
    int i = 0;
    for(i=0;i<bytes_cnt;++i)
            sum+=to[i];
    printf("%d/n",sum);
    **********************************/
    return 0;
}
```

