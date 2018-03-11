# 16（如何達到內存最大帶寬，複雜指令）


內存的使用可以說對程序員來說極其重要，特別在大規模數據處理中，如何達到內存帶寬的極限是程序員追求的目標。

從本節開始，通過一系列的例子，來進行探討。本系列全部採用彙編語言，使用的例子只有一個就是內存拷貝，這個最容易理解，最容易舉例的例子。



本節給出的代碼是baseline，也就是最差的方法，將來的方法都比這個要強的多，包括了精簡指令，內存的預取，MMX寄存器，塊複製等多項技術，希望讀者朋友持續閱讀。希望達到的目的包括：

（1）讀完該小系列後，對彙編和內存的一些基本指令會有一定了解和應用

（2）對內存的原理，寄存器有深入理解



按照奇淫巧計的慣例，代碼詳細講解將在續篇中給出。以下代碼編譯方式gcc -g -o test main.c。限64位機。


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

rdtsc函數是獲取機器開機的時間戳。下面這段文字來自intel指令手冊：

Loads the current value of the processor’s time-stamp counter (a 64-bit MSR) into
the EDX:EAX registers and also loads the IA32_TSC_AUX MSR (address
C000_0103H) into the ECX register. The EDX register is loaded with the high-order
32 bits of the IA32_TSC MSR; the EAX register is loaded with the low-order 32 bits of
the IA32_TSC MSR; and the ECX register is loaded with the low-order 32-bits of
IA32_TSC_AUX MSR. On processors that support the Intel 64 architecture, the highorder
32 bits of each of RAX, RDX, and RCX are cleared.

其中=a是EAX的簡寫和=d是EDX寄存器的簡寫，表示該指令的結果EAX寄存器存放在lo中，EDX寄存器存放在hi中。這並不難理解，該系列第二篇博客也介紹過rdtsc指令。


```c
static __inline__ unsigned long long rdtsc(void)   
{   
  unsigned hi, lo;  
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));   
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );   
}   
```

64位程序參數傳遞的方法和32位有很大不同，是通過寄存器來傳遞的，m_b_64(to,from,qdword_cnt);   這段函數在執行前，首先將to指向的地址存放在RDI寄存器中，然後將from指向的地址存放在RSI寄存器中，qdword_cnt存放在RDX寄存器中。而REP MOVSQ的含義是：Move RCX quadwords from[RSI] to [RDI].因此需要將存放在RDX寄存器的qdword_cnt存放在RCX寄存器內。

  

關於REP MOVSQ/MOVSW/MOVSD/MOVSB命令的詳細情況，可查詢intel指令手冊。



以上內存拷貝方法差距不大，MOVSQ並沒有比MOVSB有顯著提升，後面還會介紹更多的方法，會大幅度提高內存拷貝的性能，待續。

