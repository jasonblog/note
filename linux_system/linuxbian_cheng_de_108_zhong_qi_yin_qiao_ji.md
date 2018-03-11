# linux编程的108种奇淫巧计



今天继续介绍第4种常见技巧，编译展开，代码见本文最后，重要的部分有注释说明。

我们知道对于循环，编译器会自动进行展开，但是如何展开，这完全不可控，如果我们自行码代码，代码会显得臃肿难看，大段重复代码，因此本为介绍了一种常见的方法进行这种代码展开，可以写到任意层次，我这里只写到了DO16,见代码加粗的部分。

实验表明在手动展开后，加上O3的编译优化，依然能够比不手动展开要快，大家可以实验以下的代码。



可能会有读者问，这能节省多少时间，会有多少好处，那么请读者朋友们做实验来验证吧，一切真知来自实践。如果经常阅读高质量开源代码会常常看到这个技巧，希望读者在深入理解后在工作中多多采用。

有兴趣的朋友还可以用不同层次（4，16，32，64）的展开，看看展开多少是最优的，并解释原因，那恭喜您，您的境界又上了一个新的台阶了。



注：

1)里面会使用到一些此前介绍过的代码，因此新读者建议阅读此前系列的内容，链接在本文最后。

2)本文的续篇参见：http://blog.csdn.net/pennyliang/archive/2010/10/30/5975678.aspx



在用-O3编译后，用objdump -d test_m1_o3观察代码的情况[两段rdtsc之间的代码，为主要计算过程的代码]


```sh
400730:       83 fd 02                cmp    $0x2,%ebp

  400733:       89 c6                   mov    %eax,%esi
  400735:       4c 8d 63 fc             lea    0xfffffffffffffffc(%rbx),%r12 //0xfffffffffffffffc为16进制的 -4，未来会有专门博客介绍这段代码的具体含义，不在本文展开。
  400739:       7e 21                   jle    40075c <main+0xac>
  40073b:       8d 45 fd                lea    0xfffffffffffffffd(%rbp),%eax
  40073e:       4c 8d 63 fc             lea    0xfffffffffffffffc(%rbx),%r12
  400742:       31 d2                   xor    %edx,%edx
  400744:       48 8d 48 01             lea    0x1(%rax),%rcx
  400748:       8b 44 93 04             mov    0x4(%rbx,%rdx,4),%eax
  40074c:       03 04 93                add    (%rbx,%rdx,4),%eax          //循环并没有被展开      
  40074f:       89 44 93 08             mov    %eax,0x8(%rbx,%rdx,4)
  400753:       48 83 c2 01             add    $0x1,%rdx                        //相当于i++
  400757:       48 39 ca                cmp    %rcx,%rdx
  40075a:       75 ec                   jne    400748 <main+0x98>
     用objdump -d test_m3_o3观察代码的情况[两段rdtsc之间的代码，为主要计算过程的代码]
 400726:       89 c7                   mov    %eax,%edi
  400728:       8d 45 0f                lea    0xf(%rbp),%eax
  40072b:       85 ed                   test   %ebp,%ebp
  40072d:       89 ea                   mov    %ebp,%edx
  40072f:       4d 8d 6c 24 fc          lea    0xfffffffffffffffc(%r12),%r13
  400734:       be 02 00 00 00          mov    $0x2,%esi
  400739:       0f 48 d0                cmovs  %eax,%edx
  40073c:       c1 fa 04                sar    $0x4,%edx
  40073f:       83 fa 02                cmp    $0x2,%edx
  400742:       7e 79                   jle    4007bd <main+0x10d>
  400744:       4d 8d 6c 24 fc          lea    0xfffffffffffffffc(%r12),%r13
  400749:       be 02 00 00 00          mov    $0x2,%esi
  40074e:       66 90                   xchg   %ax,%ax
  400750:       8b 43 04                mov    0x4(%rbx),%eax    //eax是累加器，可以看到明显的代码展开
  400753:       03 03                   add    (%rbx),%eax
  400755:       83 c6 10                add    $0x10,%esi
  400758:       89 43 08                mov    %eax,0x8(%rbx)
  40075b:       03 43 04                add    0x4(%rbx),%eax
  40075e:       89 43 0c                mov    %eax,0xc(%rbx)
  400761:       03 43 08                add    0x8(%rbx),%eax
  400764:       89 43 10                mov    %eax,0x10(%rbx)
  400767:       03 43 0c                add    0xc(%rbx),%eax
  40076a:       89 43 14                mov    %eax,0x14(%rbx)
  40076d:       03 43 10                add    0x10(%rbx),%eax
  400770:       89 43 18                mov    %eax,0x18(%rbx)
  400773:       03 43 14                add    0x14(%rbx),%eax
  400776:       89 43 1c                mov    %eax,0x1c(%rbx)
  400779:       03 43 18                add    0x18(%rbx),%eax
  40077c:       89 43 20                mov    %eax,0x20(%rbx)
  40077f:       03 43 1c                add    0x1c(%rbx),%eax
  400782:       89 43 24                mov    %eax,0x24(%rbx)
  400785:       03 43 20                add    0x20(%rbx),%eax
  400788:       89 43 28                mov    %eax,0x28(%rbx)
  40078b:       03 43 24                add    0x24(%rbx),%eax
  40078e:       89 43 2c                mov    %eax,0x2c(%rbx)
  400791:       03 43 28                add    0x28(%rbx),%eax
  400794:       89 43 30                mov    %eax,0x30(%rbx)
  400797:       03 43 2c                add    0x2c(%rbx),%eax
  40079a:       89 43 34                mov    %eax,0x34(%rbx)
  40079d:       03 43 30                add    0x30(%rbx),%eax
  4007a0:       89 43 38                mov    %eax,0x38(%rbx)
  4007a3:       03 43 34                add    0x34(%rbx),%eax
  4007a6:       89 43 3c                mov    %eax,0x3c(%rbx)
  4007a9:       03 43 38                add    0x38(%rbx),%eax
  4007ac:       89 43 40                mov    %eax,0x40(%rbx)
  4007af:       03 43 3c                add    0x3c(%rbx),%eax
  4007b2:       89 43 44                mov    %eax,0x44(%rbx)
  4penny:       48 83 c3 40             add    $0x40,%rbx
  4007b9:       39 f2                   cmp    %esi,%edx
  4007bb:       7f 93                   jg     400750 <main+0xa0>
  4007bd:       39 f5                   cmp    %esi,%ebp
  4007bf:       7e 27                   jle    4007e8 <main+0x138>
  4007c1:       48 63 c6                movslq %esi,%rax
  4007c4:       48 c1 e0 02             shl    $0x2,%rax
  4liang:       49 8d 4c 05 00          lea    0x0(%r13,%rax,1),%rcx
  4007cd:       49 8d 54 04 f8          lea    0xfffffffffffffff8(%r12,%rax,1),%rdx
  4007d2:       8b 01                   mov    (%rcx),%eax
  4007d4:       03 02                   add    (%rdx),%eax
  4007d6:       83 c6 01                add    $0x1,%esi
  4007d9:       48 83 c1 04             add    $0x4,%rcx
  4007dd:       89 42 08                mov    %eax,0x8(%rdx)
  4007e0:       48 83 c2 04             add    $0x4,%rdx
  4007e4:       39 f5                   cmp    %esi,%ebp
  4007e6:       7f ea                   jg     4007d2 <main+0x122>

```

明显看出展开后的代码，另外从编译出的可执行程序的大小也可看出这种差异，越是内联展开的代码可执行程序越大。

 

------------------------------------------编译方法---------------------------------

在debug模式下的编译
```sh

g++ -g  test.cpp -o test_m1 -D M_1

g++ -g  test.cpp -o test_m2 -D M_2

g++ -g  test.cpp -o test_m3 -D M_3
```
 

在-O3条件下的优化编译：

```sh
g++ -O3  test.cpp -o test_m1_o3 -D M_1

g++ -O3  test.cpp -o test_m2_o3 -D M_2

g++ -O3  test.cpp -o test_m3_o3 -D M_3
```
 

-----------------------------------------运行方法-------------------------------------

```sh
./test_m1 1000000 //求1000000的斐波拉契数

./test_m2 1000000

./test_m3 1000000
```
-------------------------------------------代码-------------------------------------


```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DO(x)  x
#define DO4(x)  x  x  x  x
#define DO8(x)  DO4(x) DO4(x)
#define DO16(x) DO8(x) DO8(x)
const int MAX = 512*1024*1024;
const float CPU_MHZ = 3000.164; //use cat /proc/cpuinfo get the value
const float CPU_tick_count_per_msecond = CPU_MHZ*1000;
#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile ("rdtsc" : "=A" (x));
     return x;
}
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#endif

int main(int argc,char **argv)
{
        if(argc!=2)
        {
                printf("command penny line: test N,N no more than%d/n",MAX);
                return 0;
        }
        
        int* F = (int*)malloc(MAX*sizeof(int));
        memset(F,0,MAX*sizeof(int)); //just warm up cache, to make calculate more accurate!!!   
        F[0]=1;
        F[1]=1;
        int Fx=atoi(argv[1]);
        int start = 0;
        int end = 0;
        start = rdtsc();
        #ifdef M_1
        for(int i=2;i<Fx;++i)        //通过循环的方法，递进地计算
        {
                F[i]=F[i-1]+F[i-2];
        }
        #endif


        #ifdef M_2
        int r= Fx%4;
        int idx = Fx/4;
        int i =2;

        int j=0;
        for(;j<idx;++j)
        {
                DO4(F[i]=F[i-1]+F[i-2];i++;);  //通过循环展开的方法，展成4段代码，循环规模降低到原来的1/4
        }
        for(;i<Fx;i++)
        {
                F[i]=F[i-1]+F[i-2];
        }
        #endif

        #ifdef M_3
        int r= Fx%16;   
        int idx = Fx/16;
        int i=2;

        int j=0;
        for(;j<idx;++j)
        {
                DO16(F[i]=F[i-1]+F[i-2];i++;); //展开成16段代码
        }
        for(;i<Fx;i++)
        {
                F[i]=F[i-1]+F[i-2];
        }
        #endif
        end = rdtsc();
        printf("run tick count:%d/n",(end -start));
        printf("ret:%d/n",F[Fx-1]);
        free(F);
        return 0;
}
```
