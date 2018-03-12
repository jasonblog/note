# 4(編譯展開)


今天繼續介紹第4種常見技巧，編譯展開，代碼見本文最後，重要的部分有註釋說明。

我們知道對於循環，編譯器會自動進行展開，但是如何展開，這完全不可控，如果我們自行碼代碼，代碼會顯得臃腫難看，大段重複代碼，因此本為介紹了一種常見的方法進行這種代碼展開，可以寫到任意層次，我這裡只寫到了DO16,見代碼加粗的部分。

實驗表明在手動展開後，加上O3的編譯優化，依然能夠比不手動展開要快，大家可以實驗以下的代碼。



可能會有讀者問，這能節省多少時間，會有多少好處，那麼請讀者朋友們做實驗來驗證吧，一切真知來自實踐。如果經常閱讀高質量開源代碼會常常看到這個技巧，希望讀者在深入理解後在工作中多多采用。

有興趣的朋友還可以用不同層次（4，16，32，64）的展開，看看展開多少是最優的，並解釋原因，那恭喜您，您的境界又上了一個新的臺階了。



注：

1)裡面會使用到一些此前介紹過的代碼，因此新讀者建議閱讀此前系列的內容，鏈接在本文最後。

2)本文的續篇參見：http://blog.csdn.net/pennyliang/archive/2010/10/30/5975678.aspx



在用-O3編譯後，用objdump -d test_m1_o3觀察代碼的情況[兩段rdtsc之間的代碼，為主要計算過程的代碼]


```sh
400730:       83 fd 02                cmp    $0x2,%ebp

  400733:       89 c6                   mov    %eax,%esi
  400735:       4c 8d 63 fc             lea    0xfffffffffffffffc(%rbx),%r12 //0xfffffffffffffffc為16進制的 -4，未來會有專門博客介紹這段代碼的具體含義，不在本文展開。
  400739:       7e 21                   jle    40075c <main+0xac>
  40073b:       8d 45 fd                lea    0xfffffffffffffffd(%rbp),%eax
  40073e:       4c 8d 63 fc             lea    0xfffffffffffffffc(%rbx),%r12
  400742:       31 d2                   xor    %edx,%edx
  400744:       48 8d 48 01             lea    0x1(%rax),%rcx
  400748:       8b 44 93 04             mov    0x4(%rbx,%rdx,4),%eax
  40074c:       03 04 93                add    (%rbx,%rdx,4),%eax          //循環並沒有被展開      
  40074f:       89 44 93 08             mov    %eax,0x8(%rbx,%rdx,4)
  400753:       48 83 c2 01             add    $0x1,%rdx                        //相當於i++
  400757:       48 39 ca                cmp    %rcx,%rdx
  40075a:       75 ec                   jne    400748 <main+0x98>
     用objdump -d test_m3_o3觀察代碼的情況[兩段rdtsc之間的代碼，為主要計算過程的代碼]
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
  400750:       8b 43 04                mov    0x4(%rbx),%eax    //eax是累加器，可以看到明顯的代碼展開
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

明顯看出展開後的代碼，另外從編譯出的可執行程序的大小也可看出這種差異，越是內聯展開的代碼可執行程序越大。

 

------------------------------------------編譯方法---------------------------------

在debug模式下的編譯
```sh

g++ -g  test.cpp -o test_m1 -D M_1

g++ -g  test.cpp -o test_m2 -D M_2

g++ -g  test.cpp -o test_m3 -D M_3
```
 

在-O3條件下的優化編譯：

```sh
g++ -O3  test.cpp -o test_m1_o3 -D M_1

g++ -O3  test.cpp -o test_m2_o3 -D M_2

g++ -O3  test.cpp -o test_m3_o3 -D M_3
```
 

-----------------------------------------運行方法-------------------------------------

```sh
./test_m1 1000000 //求1000000的斐波拉契數

./test_m2 1000000

./test_m3 1000000
```
-------------------------------------------代碼-------------------------------------


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
        for(int i=2;i<Fx;++i)        //通過循環的方法，遞進地計算
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
                DO4(F[i]=F[i-1]+F[i-2];i++;);  //通過循環展開的方法，展成4段代碼，循環規模降低到原來的1/4
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
                DO16(F[i]=F[i-1]+F[i-2];i++;); //展開成16段代碼
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

## (續）

編譯展開的這篇博客被CSDN推了首頁http://blog.csdn.net/pennyliang/archive/2010/10/28/5971059.aspx，有些讀者反映有些太難，考慮到有些地方沒有講得太清楚，本文一併進行深入討論。

首先關鍵的代碼是：

```c
#define DO(x)  x
#define DO4(x)  x  x  x  x
#define DO8(x)  DO4(x) DO4(x)
#define DO16(x) DO8(x) DO8(x)
```

讀者可以按葫蘆畫瓢，繼續展開，這並不難理解。



我們選取了計算斐波那契數作為例子，將代碼展開為16的倍數，但是因為Fx是用戶輸入的，可能是16的倍數也可能不是，因此需要做一些轉換，將Fx變為Fx=16idx+r，這樣可以確保可以做idx次展開，最後尾部再用一個循環計算完，如下：

```c
int r= Fx%16;   
int idx = Fx/16;
int i=2;
for(;i<idx;)
{
    DO16(F[i]=F[i-1]+F[i-2];i++;); //展開成16段代碼
}
for(;i<Fx;i++)
{
    F[i]=F[i-1]+F[i-2];
}
如果我們不用編譯來做循環展開，我們的代碼可能就得是

for(;i<idx;)
{
    F[i]=F[i-1]+F[i-2];i++;

    F[i]=F[i-1]+F[i-2];i++;

    F[i]=F[i-1]+F[i-2];i++;

    ....寫16遍相同的代碼，多難看啊。
}
for(;i<Fx;i++)
{
    F[i]=F[i-1]+F[i-2];
}
```

第二，我們要特別注意memset的使用，memset在計時之前進行，是因為這樣的計算更為準確，malloc分配大內存是採用mmap的方式，即只分配虛地址，而沒有實際的調頁，而我們做一個memset是為了調頁，大家可以做這樣的實驗，做兩次相同的malloc，用rdtsc的方式進行計時，你會發現第一次malloc會慢一些，而第二次會快，因為第一次有調頁，而第二次幾乎無調頁（內存要足夠大，否則可能會swap，也可能有少量調頁）。

用memset相信也不難理解，因為這個時間混在裡面可能會看不出誤差，假定某市一季度GDP為10，二季度為15，看上去提高了50%，但是因為在計算過程中，有5份的GDP（可以想象成memset的代價）是多算在裡面的，如果扣除這5份，則實際上是從5提升了100%。因此我們在設計實驗時，需要把一些幹擾項扣除，來比較，實驗的數據才具有價值。



第三，我的實驗結果是展開為4次是比較快的，我認為有這樣一些主要原因，但還需要設計進一步的實驗來證明：

（1）編譯展開的層次過大，代碼會變大，而代碼存儲在文件中，進入執行需要有一個讀磁盤的過程，另外代碼大，代碼局部性就不強，L1 cache的一部分是存放代碼段的，如果代碼越小越緊湊，那麼執行起來就會越開，展開出1024層，代碼的局部性肯定很差。

（2）編譯展開的層次過小，代碼雖然緊湊，但是流水線不通暢，跳轉太多，因此也容易導致性能變低。



因此總會有一種展開的層次可以trade off的流水線和代碼段的緊湊性，我的實驗結果是展開到4層是最快的，這是受機器環境影響的，不知道其他同學的實驗結果是怎樣的。



最後提到的細節是DO16(F[i]=F[i-1]+F[i-2];i++;); 的最後一個分號是不必要的，但是添加在這裡是為了讓代碼更自然，相信大部分細心的讀者都能看到這一點。