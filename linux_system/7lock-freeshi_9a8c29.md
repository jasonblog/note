# 7(Lock-free实验)


从该博客开始，会有一些小系列预计有4-5篇博文来介绍，锁的应用和实践，我们常常听到spin lock，wait-free，lock-free，这到底是怎么回事，我们能不能自己实现一个spin lock，原理是什么？这个小系列就讨论这个内容。

 

首先我们来看两个基本操作compare_and_swap和fetch_and_add，基本上lock-free的操作都会依赖这两个基本的原子操作。特别是compare_and_swap这个原子操作，它源于IBM System 370，其包含三个参数：（1）共享内存的地址(*p)，（2）该地址期望的值(old_value)，（3）一个新值(new_value)。只有当*p == old_value时，才产生交换操作，返回真值，否则返回假值，相当于如下代码 ：


```cpp
template<class T>

bool CAS(T* addr, T exp, T val) //只有在整个函数过程具有原子性时才正确，实际的代码参照下面的汇编代码。
{
    if（*addr == exp）{
        *addr = val;
        return true；
    }
  return false；
}
```

在下面的代码中我们会看到compare_and_swap使用了lock指令，用于锁总线，setz会判断cmpxchg指令后ZF符号位是否置位，可以知道是否发生了一次交换。以下是一段可以执行的代码，void* sum(void*)函数通过不同的编译命令生成不同的代码，其结果都是用10个线程对一个全局变量进行加和的简单操作。但分别采用了pthread提供的mutex，fetch_and_add方法,完全无锁的方法，应用cas的三种方式，其中sum_with_cas_imp_yield就基本是spinlock的实现了。

下一篇我来公布在我的测试机的实验结果，并且继续探讨其他lock-free的话题。


```c
#include <pthread.h>

#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#if defined(__x86_64__)  
        #define ATOMICOPS_WORD_SUFFIX "q"  //64位环境下使用cmpxchgq命令
#else
        #define ATOMICOPS_WORD_SUFFIX "l"   //32位环境下使用cmpxchgl命令
#endif
static inline bool compare_and_swap(volatile size_t *p, size_t val_old, size_t val_new){
        char ret;
        __asm__ __volatile__("lock; cmpxchg" ATOMICOPS_WORD_SUFFIX " %3, %0; setz %1"//lock命令锁总线，因此可以保证多核同步
                        : "=m"(*p), "=q"(ret)      //setz为ZF符号位是否置位，用于设置返回值
                        : "m"(*p), "r" (val_new), "a"(val_old)
                        : "memory");
        return (bool)ret;
}
static inline size_t fetch_and_add(volatile size_t *p, size_t add){
        unsigned int ret;
        __asm__ __volatile__("lock; xaddl %0, %1"
                        :"=r" (ret), "=m" (*p)
                        : "0" (add), "m" (*p)
                        : "memory");
        return ret;
};
struct my_cas
{
        my_cas(unsigned char t):m_val_old(t){}
        size_t m_val_old;
        inline void try_continue(size_t val_old,size_t val_new){
                while(!compare_and_swap(&m_val_old,val_old,val_new)){};
        }
inline void add(size_t val_new){
                fetch_and_add(&m_val_old,val_new);
        }
};
volatile size_t g_uCount;
pthread_mutex_t g_tLck=PTHREAD_MUTEX_INITIALIZER;
my_cas mutex(1);
const size_t cnt_num = 10000000;
void* sum_with_mutex_lock(void*)
{
        for(int i=0;i < cnt_num;++i) {
                pthread_mutex_lock(&g_tLck);
                g_uCount += 1;
                pthread_mutex_unlock(&g_tLck);
        }
};
void* sum_with_f_and_a(void*)  //用fetch_and_add原子操作来保证结果正确性。
{
        for(int i=0;i < cnt_num;++i) {
                fetch_and_add(&g_uCount,1);
        }
};
void* sum_with_cas(void*)  //用CAS原子操作来模拟锁操作。
{       
        for(int i=0;i< cnt_num;++i)
        {       
                mutex.try_continue(1,0);
                g_uCount += 1;
                mutex.try_continue(0,1);   
        }
}
void* sum_with_cas_imp(void*)
{
        for(int i=0;i< cnt_num;++i) {
                for(;;) {
                        size_t u = g_uCount;
                        if(compare_and_swap(&g_uCount,u,u+1)){   //在上一条语句和本条语句之间，g_uCount无篡改则进行加1，
                                break;        //break出该循环，否则重试，直到成功。
                        }
                }
        }
}
void* sum_with_cas_imp_yield(void*)
{
        for(int i=0;i< cnt_num;++i) {
                for(;;) {
                        register size_t c = 1000; //
                        while(c){
                                size_t u = g_uCount;
                                if(compare_and_swap(&g_uCount,u,u+1)){
                                        break;
                                }
                                c--;
                        }
                        if(!c){
                                syscall(SYS_sched_yield); //增加一次让渡CPU的机会，spin lock通常应有这种策略
                        }
                }
        }
}
void* sum_just_free(void*)
{       
        for(int i=0;i < cnt_num;++i) {  //完全无锁，无等待，但执行结果通常是错误的。
                g_uCount += 1;
        }
}
void* sum(void*)
{
        #ifdef M_LOCK
         sum_with_mutex_lock(NULL);
        #endif
        #ifdef FETCH_AND_ADD
         sum_with_f_and_a(NULL);
        #endif
        #ifdef FREE
         sum_just_free(NULL);
        #endif
        #ifdef CAS
         sum_with_cas(NULL);
        #endif
        #ifdef CAS_IMP
         sum_with_cas_imp(NULL);
        #endif
#ifdef CAS_IMP_YIELD
         sum_with_cas_imp_yield(NULL);
        #endif
};
int main()
{       
        pthread_t* thread = (pthread_t*) malloc(10*sizeof( pthread_t));
        for(int i=0;i<10;++i){       
                pthread_create(&thread[i],NULL,sum,NULL);
        }
        for(int i=0;i<10;++i){       
                pthread_join(thread[i],NULL);
        }
        printf("g_uCount:%d/n",g_uCount);
}
```

用以下编译命令编译出6个程序

```sh
g++ test.cpp -o test_free -D FREE -lpthread
g++ test.cpp -o test_fetchandadd -D FETCH_AND_ADD -lpthread
g++ test.cpp -o test_mlock -D M_LOCK -lpthread
g++ test.cpp -o test_cas -D CAS -lpthread
g++ test.cpp -o test_cas_imp -D CAS_IMP –lpthread
g++ test.cpp –o test_cas_imp_yield –D CAS_IMP_YIELD -lpthread
```

## Lock-free实验 問答


size_t u=g_uCount这个必须是原子操作才可以保证正确性，这里做一个答复。


首先注意我对g_uCount的变量定义加了volatile，告诉编译器不要将这个变量优化进寄存器，也就是在多核情况下，每个核上的线程在从g_uCount取值时都是从内存中取的，也就是会锁总线，这显然是一个原子操作。在扯远一点从内存地址取值怎么就相当于锁总线呢？因为我们目前都是SMP架构，多个核共享一个内存，对称多处理的架构，核多个，L1Cache多个，但内存是一个，在同一时刻只有一个核可以从内存中取出来数据。

如果大家觉得不是原子操作，经不起推敲，可以举个例子来说服我，只一句经不起推敲是否不太妥当？



其次，即便把volatile修饰去掉，结果也是正确的，这个确实很难解释，我目前的水平还达不到完全解释的能力，有兴趣的读者可以参见：http://www.newsmth.net/bbstcon.php?board=CSArch&gid=34641。我想做芯片或者做编译器的同学应该能知道原因。



最后，这里举得例子都是很简单的，比如我的试验机只有4个核，在这个例子中，4个线程是最好的，多了反而无益，但实践中，是很难避免有慢速的设备和慢速的操作，超过4个线程通常都是有益的，可以在分时操作系统时间片的作用下，获得更好的吞吐率。



我们可以举个例子，比如银行有4个服务员（看成4core Cpu），有一票人（看做多个线程)在请求服务，现在的策略是分时服务，在某个老人（看做是一个线程）在窗口办一张银行卡，需要填表，该服务员每一个周期回头来检查一下老人是否填好了，没填好就处理别人的请求。这样总的吞吐率比服务员死等老人填完表要好得多。



最后我还是特别期待看到在g_uCount被修饰为volatile，且size_t u=g_uCount 不是原子操作的可运行的程序来证明，期待这两位网友和其他能作出这个实验的朋友们发给我，我贴出来，让大家一起学习一下，共同把这个问题深入搞清楚，如果不是常见的计算机或者操作系统也没关系，至少大家多长个见识。


---

对于是否原子性这个问题，我们在水木体系结构版曾讨论过这个话题，参见yifanw的发言，“在所有mordern archiecture上，对齐的machine word读写肯定是atomic的，这点无须质疑”，当然体系架构变化后这个假设变化了，整个都要发生变化，正确性就不被保证了。但是这个体系架构变化的可能性太小了，未来很长一段时间不太可能，即便变了也会考虑兼容性，
__sig_atomic_在linux里定义为一个ordinary int : typedef int __sig_atomic_t;当然如果写__sig_atomic_t会更好，当前没什么价值，但为了将来的移植性好，这是对的，大家都遵守__sig_atomic_t语义，将来肯定是有好处的，这是毫无疑问的。严谨的角度看的确应该如此写。

你说的“也许有个实现碰巧出现 size_t 需要两次 bus access才能取到完整的值。”，因为都是在栈上定义的，所以肯定都是对齐的，所以真的不会出现碰巧需要两次bus access的可能性，如果这一点我说的不能让你信服，你可以查查资料，这个都是公知的。我这个人也比较较真，但我要看到实践，你说月球上可能碰巧有水，我不能证明月球上无水，因为不可能遍历地球每一片土地，但要说月球有水需要拿出证据才好。因为目前没有一个人能拿出月球上有水的证据，而大部分证据证明月球上无水，所以现在公认月球上无水，应该是合理的。



对spin lock来说，我是比较经验主义的，相信第一手的实践的东西，但是实践的环境总是有限的，不免陷入局限，以后体系结构变了不兼容现在了这种情况也是有可能的。超高并发，或者多核心的情况慢可能会存在，我想也一定会存在，应该具体问题具体分析，spin lock是有应用场合的，比如一次慢速的磁盘访问还spin
lock显然就不合适了。就应该把自己放到阻塞态去。 



关于你说的题外话，http://blog.csdn.net/pennyliang/archive/2010/10/28/5971059.aspx上提到了-O3的优化编译，还贴了代码，来证明需要手工优化，因为加了-OX的优化后，汇编代码就很难看懂了，这是我不用-O的原因，如果需要，以后我都加上。

再比如，结构体建议是2的幂（第二篇），以及false shareing问题都是经典的优化问题，这个应该不存在什么疑问吧，即便在-O3，-O2也是避免不了的，不信的话可以实验看看，呵呵。

很多时候，我们做的东西在一个平台上好用了，移植到另一个平台可能就出问题了，优化在一个平台上有效，换了另一个平台可能出问题，甚至会有机器性能提升了，反而程序性能下降了，但遗憾的是，真的没有办法去照顾到所有可能的平台，一些general的东西应该写入教材，而不是写在博客中，博客中是难免局限的。好在我的博客中有申明：“不代表学术立场，只是非正式交流”呵呵。

到目前为止机器的很多问题，我还没有能力完全解释，相信有这个能力完全解释的人且愿意分享的人也会很多，因为操作系统太复杂，涉及的问题太多，我们的研究只能固定住其他的点，只对一个点进行研究讨论，但实践环节中，比如搜索引擎，影响性能的原因太多了，要找到全部的原因，进行深入的定量的分析和解释，需要对操作系统，指令集等等一系列深入理解，我有一个心愿，在有限的将来可以写一本《计算原理》这本书，能够把问题讲明白，遗憾的是我现在还在努力学习中，不断探索，只是我有时候停止脚步，趴下来，把肩膀露出来，垫更多的人从我的肩膀上去，去看到曾让我着迷的东西，和我一起去探索背后的原理。



## (Lock-free之二)

本节增加test_and_set的原语和一个spinlock比较完整的实现（参照nginx spin lock），主要的变化在于插入了__asm__ ("pause")指令，且插入次数是尝试锁的次数的2次幂，有助于在减少重试次数，通过这一变化可以对比看出CPU100%占用的问题得到了缓解。test_and_set指令在未来的博客中还会继续提到，cas，tas，fetch_and_add是这一系列中需要使用的仅有的3条原子操作。

test_and_set的语义为传入一个地址将其无条件置1，并返回该地址值的原来值，如果原来是1，返回1，如果原来是非1，返回非1，这提供了一个关门的好处，如果变量是非1，在进入临界区以后，该变量自动关门，后面的尝试全部失败因为是while(tas(V)==1)的操作，在使用完临界区以后，将该值请零（reset），必然有一个会在spin的过程中拿到，但是这种调度是有偶然性的，因此可能会出现饥饿，而且并发数越多的情况下，越容易出现饥饿，spin的次数不易过大，本文指定为4096，该值可以根据实际环境调整，参见nginx源码。


```sh
//编译方法

//g++ test.cpp -o test_cas_imp -D CAS_IMP -lpthread
//g++ test.cpp -o test_cas_imp_all -D CAS_IMP_ALL -lpthread
//g++ test.cpp -o test_tas_imp -D TAS_IMP -lpthread
//g++ test.cpp -o test_tas_imp_all -D CAS_IMP_ALL -lpthread
```

```cpp
# include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#if defined(__x86_64__)
#define ATOMICOPS_WORD_SUFFIX "q"
#else
#define ATOMICOPS_WORD_SUFFIX "l"
#endif
static inline bool compare_and_swap(volatile size_t* p, size_t val_old,
                                    size_t val_new)
{
    char ret;
    __asm__ __volatile__("lock; cmpxchg" ATOMICOPS_WORD_SUFFIX " %3, %0; setz %1"
                         : "=m"(*p), "=q"(ret)
                         : "m"(*p), "r"(val_new), "a"(val_old)
                         : "memory");
    return (bool)ret;
}
static inline size_t fetch_and_add(volatile size_t* p, size_t add)
{
    unsigned int ret;
    __asm__ __volatile__("lock; xaddl %0, %1"
                         :"=r"(ret), "=m"(*p)
                         : "0"(add), "m"(*p)
                         : "memory");
    return ret;
};
static inline int test_and_set(volatile int* s)    /* tested */
{
    int r;
    __asm__ __volatile__(
        "xchgl %0, %1"
        : "=r"(r), "=m"(*s)
        : "0"(1), "m"(*s)
        : "memory");

    return r;
}
static inline int reset(volatile int* s)
{
    *s = 0;
}

volatile size_t g_uCount;
pthread_mutex_t g_tLck = PTHREAD_MUTEX_INITIALIZER;
const size_t cnt_num = 10000000;
volatile int tas_lock = 0;


void* sum_with_cas_imp(void*)
{
    for (int i = 0; i < cnt_num; ++i) {
        for (;;) {
            size_t u = g_uCount;

            if (compare_and_swap(&g_uCount, u, u + 1)) {
                break;
            }
        }
    }
}
void* sum_with_tas_imp(void*)
{
    for (int i = 0; i < cnt_num; ++i) {
        while ((test_and_set(&tas_lock)) == 1) {}

        ++g_uCount;
        reset(&tas_lock);
    }
}

void* sum_with_cas_imp_all(void*)
{
    for (int i = 0; i < cnt_num;) {
        for (;;) {
            size_t u = g_uCount;

            if (compare_and_swap(&g_uCount, u, u + 1)) {
                goto L1;
            }

            for (size_t n = 1; n < 4096; n <<= 1) {
                for (size_t i = 0; i < n; i++) {
                    __asm__("pause") ;
                }

                u = g_uCount;

                if (compare_and_swap(&g_uCount, u, u + 1)) {
                    goto L1;
                }

            }

            syscall(SYS_sched_yield);
        }

L1:
        ++i;
    }
}

// reference:http://nginx.sourcearchive.com/documentation/0.7.59-1/ngx__spinlock_8c-source.html

void* sum_with_tas_imp_all(void*)

{

    for (int i = 0; i < cnt_num; ++i) {
        for (size_t n = 1; (test_and_set(&tas_lock)) == 1; n <<= 1) {
            if (n < 4096) {
                for (size_t i = 0; i < n; i++) {
                    __asm__("pause") ;
                }
            }
            else {
                syscall(SYS_sched_yield);
                n = 1;
            }
        }

        ++g_uCount;
        reset(&tas_lock);

    }

}

void* sum(void*)

{
#ifdef CAS_IMP
    sum_with_cas_imp(NULL);
#endif

#ifdef TAS_IMP
    sum_with_tas_imp(NULL);
#endif

#ifdef CAS_IMP_ALL
    sum_with_cas_imp_all(NULL);
#endif

#ifdef TAS_IMP_ALL
    sum_with_tas_imp_all(NULL);
#endif
};

int main()
{
    pthread_t* thread = (pthread_t*) malloc(10 * sizeof(pthread_t));

    for (int i = 0; i < 10; ++i) {
        pthread_create(&thread[i], NULL, sum, NULL);
    }

    for (int i = 0; i < 10; ++i) {
        pthread_join(thread[i], NULL);
    }

    printf("g_uCount:%d/n", g_uCount);
    free(thread);
}
```

我搜了一下也有一些不错的文章供大家参考

我要啦免费统计
（1）http://student.csdn.net/space.php?uid=45153&do=thread&id=7403

（2）http://www.ibm.com/developerworks/cn/linux/l-rwlock_writing/