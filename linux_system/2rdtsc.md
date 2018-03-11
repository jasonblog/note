# 2(RDTSC)


通常我們需要對程序運行的準確時間進行測量，但多線程，多核環境下，這變得很困難，我們有沒有一種比較通用簡單的方法來做到這一點呢？這些方法都存在哪些問題，如何改進可以抵消這些誤差呢？

本文將介紹這裡的來龍去脈，還是從一段小程序開始。


```c
#include <stdlib.h>
#include <stdio.h>

#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x)); //.byte 0x0f,0x31等價於rdtsc，是另一種原始取機器碼的方式
     return x;                                                               //改成__asm__ volatile ("rdtsc" : "=A" (x)); 效果一樣
}                                                                                //關於操作碼可以參考文獻[2]
#elif defined(__x86_64__)
static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#endif

int main(void)
{
        register int start = 0;
        register int end = 0;
        const int MAX_COUNT = 10000000;
        volatile int sum = 0;
        const float CPU_MHZ = 3000.164; //use cat /proc/cpuinfo get the value
        const float CPU_tick_count_per_second = CPU_MHZ*1000*1000;
        start = rdtsc();
        for(int i = 0 ; i< MAX_COUNT ; ++i)
        {
                sum+=1;
        }
        end = rdtsc();
        
        printf("sum:%d,run tick count:%d,run time:%f/n",sum,end - start,(end -start)/CPU_tick_count_per_second);
        return 0;
}
```

編譯方法：

編譯為32位程序：g++ test.cpp -o test_m32 -m32

編譯完後可以用file test_m32進行確認。

 

編譯為64位程序：g++ test.cpp -o test

編譯完後可以用file test進行確認。

 

注：我的試驗機為64位，如果是32位的話，直接使用g++ test.cpp -o test編譯出32位程序

 

執行方法：

time ./test

time ./test_m32

結果大家應該可以看到，希望大家都能在自己的機器上完成這個實驗。

 

相關解釋，預計下週一發博客進行解釋，有興趣的朋友可以先看下列推薦讀物。



---

有時候我們希望在x86平臺下獲得更加高的精度。如果我們想準確的知道一段程序，一個函數的執行時間，可以連續執行2次rdtsc，之間沒有一行代碼，來計算這兩段指令執行過程會有的cycle數，不同機器可能都會有不同，和機器的性能有關係，但和負載沒關係，也就是多進程，多線程情況下，連續兩個rdtsc之間不會插入很多cycle，這一點大家可以做實驗來驗證。

```c
start = rdtsc();
end = rdtsc();
```

在獲得這個數據後，我們對一段代碼的執行時間就可以做一個更加精確的估計。我的測試機比較一般大約是100個cycle，

用100/(3000.164*1000*1000)=0.033微秒，約合33納秒，這個時間段幾乎是不會發生什麼進程切換的，因此可以認為計算的精度是可接受的。

RDTSC只在X86下有效，其餘平臺會有類似指令來做準確計數，RDTSC指令的精度是可以接受的，裡面能插得cycle是很有限的。如果對計數要求沒那麼高，可以採用一些通用庫函數，當然你也可以用類似的方法來考察這些庫函數的精度，連續執行2次就行。


例如下面的代碼得到兩次rdtsc指令之間的cycle數。


```c
#include <stdlib.h>
#include <stdio.h>

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

int main(void)
{
        register int start = 0;
        register int end = 0;
        const int MAX_COUNT = 10000000;
        volatile int sum = 0;
        const float CPU_MHZ = 3000.164; //use cat /proc/cpuinfo get the value
        const float CPU_tick_count_per_second = CPU_MHZ*1000*1000;
        start = rdtsc();
        end = rdtsc();

        printf("sum:%d,run tick count:%d,run time:%f/n",sum,end - start,(end -start)/CPU_tick_count_per_second);
        return 0;
}
```