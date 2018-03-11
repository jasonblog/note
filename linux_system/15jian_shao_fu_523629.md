# 15(减少复制)


计算机的存储结构是层次性的，从快到慢，代价从高到低，容量从小到大，寄存器，L1 cache，L2 cache，直到磁盘，甚至比磁盘更慢速的磁带机，因此在程序运行时，不可避免的会有复制,这一点很重要，从优化的角度看，很多时候都是为了减少复制的代价，比如如果已知磁盘的读写是顺序的，这样采用DIRECTIO是较好的，直接读到用户内存上，而不需要先读到内核内存上，然后再复制到用户内存，这个例子我打算在未来的试验中给出，当然DirectIO相当于程序员自己实现缓存，在小规模数据读写上并没有优势，因为节省的这些复制开销微乎其微。

本文通过减少cache line回填（也是一种复制)，来说明减少复制获得的收益。首先了解一些背景知识：

CPU通过芯片缓存(L1 Cache)和内存进行数据交互。而交互的单位叫做cache line,大小为2的幂，32或64字节,虚拟内存页面大小为4KB。cache line的交互分为两种回填(refill)和回写(write-back)两种。假定CPU需要从虚拟内存读取一个字节的操作数，其地址为0xFFFFFFA3，cache line的大小为32字节，则CPU需要将地址0xFFFFFFA0地址开始的32个字节全部读入，填充出一个完整的cache line后，然后从该cache line的第4个字节处取得该字节的内容。如果后继的指令也需要同样从cache行中读，那么填充cache line是值得的；否则，额外的填充cache line的时间就是浪费。因此指令和数据的局部性越好，越符合cache line的设计要求。


我们都非常熟悉的memset函数，如果我们手写一个memset可能不如库函数memset的实现性能高，为什么呢？其中一个主要优化技术称之为non-temporal，其基本思想是，如果要写入的内存数据无用时，直接写入，而不需要回填cache line。涉及的指令包括movnti,movntdq,sfence等。通俗点说，我们要将一个字符(char)，memset在一片内存上，而这片内存上原有的数据显然没有用了，即不需要将这片数据的内容先refill进cacheline，在cacheline中改写了然后再写回内存，只需要直接将数据写入内存即可。库函数的memset使用的是通用寄存器，而不是SSE寄存器，使用了movnti指令，通过objdump指令可以将库函数的memset代码导出，参见在本文的最后。

为什么一定要refill呢，不refill不可以吗？假定我们对一片内存写入1个字节（假定一条cache line是32字节），数据交互的单位是cache Line,系统怎么知道另外的31个字节是什么呢？这一写回，这1个字节是对的，另外的31个字节就未定义了。因此小数据的读写refill是有必要的，但是，对于大片内存的写入，显然refill是可以避免的，intel提供的non-temperal方法也就是为这个目的服务的，即本文的减少复制的优化思想。

代码中还有一些技巧不再详述，如果有反馈，我再写个续篇解答，详细请参见下列代码。


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
glibc的库函数memset的汇编代码：

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


上回提到了用movntdq指令写xmm寄存器的方式，可以获得性能上的收益，并且指出在默认的情况下，在发生写入的时候，需要先从低层次存储层次refill到高层次存储层次（可以简单认为从内存refill进cache)，然后再cache中更新，再write back，特别强调的是，这是默认的方式。

实际上，高一级存储层次，向低一级存储层次写入有两种方式，一种是默认的write back，一种是write through。而我们使用的movntdq指令就是write through方式，必须通过汇编的方式来实现，高级语言通常没有这个效果。怎么理解write back和write through的区别呢？

对于write back,因为我们知道机器存储层次分为多层，为了保证一致性，必须每一层相同的数据都是一致的，不可能分秒都是一致的，但是在不影响计算逻辑的前提下，需要有一致性的保证，那么write back方式在写的时候有两种情况：A）当写入的地址恰好在cache的某条cache line中，那么直接在cache中写入数据，并且标记该cache line为dirty，此后不发生对内存实际地址的写回，直到发生cache被替换策略时，才将dirty的这条cache line写入到内存中。B）当写入的地址恰好不在cache中（任何一条cache line都不match），首先需要执行一次refill操作，将内存中的数据读入到cache line中，然后将数据写入该cache line，此后不发生对内存地址的写回，直到发生cache被替换策略时，才将dirty的这条cache lien写入内存。

举个例子：

某个程序需要执行这样几条命令，按顺序如下：

（1）在地址0x00FFFF00处写入64个字节的数据[PENNYLIANG...]

（2）其他命令（64个字节保持在cache中，没有被替换掉)

（3）读取0x00FFFF00处的64个字节的数据

(4) 某条命令（该命令将[PENNYLIANG...]这段数据换出了cache(L1 cache只有32k，必然会有被换出的可能)

（5）读取0x00FFFF00处的64个字节的数据

(6) 某条命令（该命令将[PENNYLIANG...]这段数据又一次换出了cache

(7) 在地址0x00FFFF00处写入64个字节的数据[奇淫巧计...]

(8) 某条命令（该命令将[奇淫巧计...]这段数据又一次换出了cache



从此前介绍的内容，我们不难得到这样几个结论：

1）在命令(4)之前，写入的数据总是保持在cache中，命令3从cache中读取的[PENNYLIANG...]

2) 命令(4)导致了cache中数据和内存的一次同步

3) 命令(5)发生了一次cache read miss，需要从内存中refill[PENNYLIANG...]到cache中。

4）命令(7)发生了一次cache write miss,需要两次内存访问实现写入，第一次refill，另一次在指令(8)时write back。



不难得到，write back在程序局部性好的情况下，是非常有利的，只要cache 命中了，就可以只在cache中转，而不发生写回，比如指令(3)就直接在cache中得到，如果对这个数据多次写，那么直接在cache中发生，根本不会每次写都体现在内存上。



在理解了write back后，就很容易理解write through了，write through直接从cache写穿到内存，只有单向的过程，没有内存到cache的refill，相当于减少了低层存储层次数据向高层存储层次的复制过程，在大规模数据写入的时候才能体现威力。



我们这里举得例子是cache，或者说L1 cache和内存的关系，广义上看是高层次存储结构和低层次存储结构的关系，内存和磁盘也可以构成这样一个write back和write through的情况，可以看到在小数据量的情况下write back总是最好的，而当发生大量数据写入时write through的方式会更有优势，以后我们会给出这个例子。



参考阅读：

 http://www-903.ibm.com/kr/techinfo/xseries/download/write_back_vs_write_through.pdf

 http://en.wikipedia.org/wiki/CPU_cache
