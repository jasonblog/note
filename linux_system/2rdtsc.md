# 2(RDTSC)


通常我们需要对程序运行的准确时间进行测量，但多线程，多核环境下，这变得很困难，我们有没有一种比较通用简单的方法来做到这一点呢？这些方法都存在哪些问题，如何改进可以抵消这些误差呢？

本文将介绍这里的来龙去脉，还是从一段小程序开始。


```c
#include <stdlib.h>
#include <stdio.h>

#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
  unsigned long long int x;
     __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x)); //.byte 0x0f,0x31等价于rdtsc，是另一种原始取机器码的方式
     return x;                                                               //改成__asm__ volatile ("rdtsc" : "=A" (x)); 效果一样
}                                                                                //关于操作码可以参考文献[2]
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

编译方法：

编译为32位程序：g++ test.cpp -o test_m32 -m32

编译完后可以用file test_m32进行确认。

 

编译为64位程序：g++ test.cpp -o test

编译完后可以用file test进行确认。

 

注：我的试验机为64位，如果是32位的话，直接使用g++ test.cpp -o test编译出32位程序

 

执行方法：

time ./test

time ./test_m32

结果大家应该可以看到，希望大家都能在自己的机器上完成这个实验。

 

相关解释，预计下周一发博客进行解释，有兴趣的朋友可以先看下列推荐读物。



---

有时候我们希望在x86平台下获得更加高的精度。如果我们想准确的知道一段程序，一个函数的执行时间，可以连续执行2次rdtsc，之间没有一行代码，来计算这两段指令执行过程会有的cycle数，不同机器可能都会有不同，和机器的性能有关系，但和负载没关系，也就是多进程，多线程情况下，连续两个rdtsc之间不会插入很多cycle，这一点大家可以做实验来验证。

```c
start = rdtsc();
end = rdtsc();
```

在获得这个数据后，我们对一段代码的执行时间就可以做一个更加精确的估计。我的测试机比较一般大约是100个cycle，

用100/(3000.164*1000*1000)=0.033微秒，约合33纳秒，这个时间段几乎是不会发生什么进程切换的，因此可以认为计算的精度是可接受的。

RDTSC只在X86下有效，其余平台会有类似指令来做准确计数，RDTSC指令的精度是可以接受的，里面能插得cycle是很有限的。如果对计数要求没那么高，可以采用一些通用库函数，当然你也可以用类似的方法来考察这些库函数的精度，连续执行2次就行。


例如下面的代码得到两次rdtsc指令之间的cycle数。


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