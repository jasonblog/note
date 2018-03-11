# 7(Lock-free實驗)


從該博客開始，會有一些小系列預計有4-5篇博文來介紹，鎖的應用和實踐，我們常常聽到spin lock，wait-free，lock-free，這到底是怎麼回事，我們能不能自己實現一個spin lock，原理是什麼？這個小系列就討論這個內容。

 

首先我們來看兩個基本操作compare_and_swap和fetch_and_add，基本上lock-free的操作都會依賴這兩個基本的原子操作。特別是compare_and_swap這個原子操作，它源於IBM System 370，其包含三個參數：（1）共享內存的地址(*p)，（2）該地址期望的值(old_value)，（3）一個新值(new_value)。只有當*p == old_value時，才產生交換操作，返回真值，否則返回假值，相當於如下代碼 ：


```cpp
template<class T>

bool CAS(T* addr, T exp, T val) //只有在整個函數過程具有原子性時才正確，實際的代碼參照下面的彙編代碼。
{
    if（*addr == exp）{
        *addr = val;
        return true；
    }
  return false；
}
```

在下面的代碼中我們會看到compare_and_swap使用了lock指令，用於鎖總線，setz會判斷cmpxchg指令後ZF符號位是否置位，可以知道是否發生了一次交換。以下是一段可以執行的代碼，void* sum(void*)函數通過不同的編譯命令生成不同的代碼，其結果都是用10個線程對一個全局變量進行加和的簡單操作。但分別採用了pthread提供的mutex，fetch_and_add方法,完全無鎖的方法，應用cas的三種方式，其中sum_with_cas_imp_yield就基本是spinlock的實現了。

下一篇我來公佈在我的測試機的實驗結果，並且繼續探討其他lock-free的話題。


```c
#include <pthread.h>

#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#if defined(__x86_64__)  
        #define ATOMICOPS_WORD_SUFFIX "q"  //64位環境下使用cmpxchgq命令
#else
        #define ATOMICOPS_WORD_SUFFIX "l"   //32位環境下使用cmpxchgl命令
#endif
static inline bool compare_and_swap(volatile size_t *p, size_t val_old, size_t val_new){
        char ret;
        __asm__ __volatile__("lock; cmpxchg" ATOMICOPS_WORD_SUFFIX " %3, %0; setz %1"//lock命令鎖總線，因此可以保證多核同步
                        : "=m"(*p), "=q"(ret)      //setz為ZF符號位是否置位，用於設置返回值
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
void* sum_with_f_and_a(void*)  //用fetch_and_add原子操作來保證結果正確性。
{
        for(int i=0;i < cnt_num;++i) {
                fetch_and_add(&g_uCount,1);
        }
};
void* sum_with_cas(void*)  //用CAS原子操作來模擬鎖操作。
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
                        if(compare_and_swap(&g_uCount,u,u+1)){   //在上一條語句和本條語句之間，g_uCount無篡改則進行加1，
                                break;        //break出該循環，否則重試，直到成功。
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
                                syscall(SYS_sched_yield); //增加一次讓渡CPU的機會，spin lock通常應有這種策略
                        }
                }
        }
}
void* sum_just_free(void*)
{       
        for(int i=0;i < cnt_num;++i) {  //完全無鎖，無等待，但執行結果通常是錯誤的。
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

用以下編譯命令編譯出6個程序

```sh
g++ test.cpp -o test_free -D FREE -lpthread
g++ test.cpp -o test_fetchandadd -D FETCH_AND_ADD -lpthread
g++ test.cpp -o test_mlock -D M_LOCK -lpthread
g++ test.cpp -o test_cas -D CAS -lpthread
g++ test.cpp -o test_cas_imp -D CAS_IMP –lpthread
g++ test.cpp –o test_cas_imp_yield –D CAS_IMP_YIELD -lpthread
```

## Lock-free實驗 問答


size_t u=g_uCount這個必須是原子操作才可以保證正確性，這裡做一個答覆。


首先注意我對g_uCount的變量定義加了volatile，告訴編譯器不要將這個變量優化進寄存器，也就是在多核情況下，每個核上的線程在從g_uCount取值時都是從內存中取的，也就是會鎖總線，這顯然是一個原子操作。在扯遠一點從內存地址取值怎麼就相當於鎖總線呢？因為我們目前都是SMP架構，多個核共享一個內存，對稱多處理的架構，核多個，L1Cache多個，但內存是一個，在同一時刻只有一個核可以從內存中取出來數據。

如果大家覺得不是原子操作，經不起推敲，可以舉個例子來說服我，只一句經不起推敲是否不太妥當？



其次，即便把volatile修飾去掉，結果也是正確的，這個確實很難解釋，我目前的水平還達不到完全解釋的能力，有興趣的讀者可以參見：http://www.newsmth.net/bbstcon.php?board=CSArch&gid=34641。我想做芯片或者做編譯器的同學應該能知道原因。



最後，這裡舉得例子都是很簡單的，比如我的試驗機只有4個核，在這個例子中，4個線程是最好的，多了反而無益，但實踐中，是很難避免有慢速的設備和慢速的操作，超過4個線程通常都是有益的，可以在分時操作系統時間片的作用下，獲得更好的吞吐率。



我們可以舉個例子，比如銀行有4個服務員（看成4core Cpu），有一票人（看做多個線程)在請求服務，現在的策略是分時服務，在某個老人（看做是一個線程）在窗口辦一張銀行卡，需要填表，該服務員每一個週期回頭來檢查一下老人是否填好了，沒填好就處理別人的請求。這樣總的吞吐率比服務員死等老人填完表要好得多。



最後我還是特別期待看到在g_uCount被修飾為volatile，且size_t u=g_uCount 不是原子操作的可運行的程序來證明，期待這兩位網友和其他能作出這個實驗的朋友們發給我，我貼出來，讓大家一起學習一下，共同把這個問題深入搞清楚，如果不是常見的計算機或者操作系統也沒關係，至少大家多長個見識。


---

對於是否原子性這個問題，我們在水木體系結構版曾討論過這個話題，參見yifanw的發言，“在所有mordern archiecture上，對齊的machine word讀寫肯定是atomic的，這點無須質疑”，當然體系架構變化後這個假設變化了，整個都要發生變化，正確性就不被保證了。但是這個體系架構變化的可能性太小了，未來很長一段時間不太可能，即便變了也會考慮兼容性，
__sig_atomic_在linux裡定義為一個ordinary int : typedef int __sig_atomic_t;當然如果寫__sig_atomic_t會更好，當前沒什麼價值，但為了將來的移植性好，這是對的，大家都遵守__sig_atomic_t語義，將來肯定是有好處的，這是毫無疑問的。嚴謹的角度看的確應該如此寫。

你說的“也許有個實現碰巧出現 size_t 需要兩次 bus access才能取到完整的值。”，因為都是在棧上定義的，所以肯定都是對齊的，所以真的不會出現碰巧需要兩次bus access的可能性，如果這一點我說的不能讓你信服，你可以查查資料，這個都是公知的。我這個人也比較較真，但我要看到實踐，你說月球上可能碰巧有水，我不能證明月球上無水，因為不可能遍歷地球每一片土地，但要說月球有水需要拿出證據才好。因為目前沒有一個人能拿出月球上有水的證據，而大部分證據證明月球上無水，所以現在公認月球上無水，應該是合理的。



對spin lock來說，我是比較經驗主義的，相信第一手的實踐的東西，但是實踐的環境總是有限的，不免陷入侷限，以後體系結構變了不兼容現在了這種情況也是有可能的。超高併發，或者多核心的情況慢可能會存在，我想也一定會存在，應該具體問題具體分析，spin lock是有應用場合的，比如一次慢速的磁盤訪問還spin
lock顯然就不合適了。就應該把自己放到阻塞態去。 



關於你說的題外話，http://blog.csdn.net/pennyliang/archive/2010/10/28/5971059.aspx上提到了-O3的優化編譯，還貼了代碼，來證明需要手工優化，因為加了-OX的優化後，彙編代碼就很難看懂了，這是我不用-O的原因，如果需要，以後我都加上。

再比如，結構體建議是2的冪（第二篇），以及false shareing問題都是經典的優化問題，這個應該不存在什麼疑問吧，即便在-O3，-O2也是避免不了的，不信的話可以實驗看看，呵呵。

很多時候，我們做的東西在一個平臺上好用了，移植到另一個平臺可能就出問題了，優化在一個平臺上有效，換了另一個平臺可能出問題，甚至會有機器性能提升了，反而程序性能下降了，但遺憾的是，真的沒有辦法去照顧到所有可能的平臺，一些general的東西應該寫入教材，而不是寫在博客中，博客中是難免侷限的。好在我的博客中有申明：“不代表學術立場，只是非正式交流”呵呵。

到目前為止機器的很多問題，我還沒有能力完全解釋，相信有這個能力完全解釋的人且願意分享的人也會很多，因為操作系統太複雜，涉及的問題太多，我們的研究只能固定住其他的點，只對一個點進行研究討論，但實踐環節中，比如搜索引擎，影響性能的原因太多了，要找到全部的原因，進行深入的定量的分析和解釋，需要對操作系統，指令集等等一系列深入理解，我有一個心願，在有限的將來可以寫一本《計算原理》這本書，能夠把問題講明白，遺憾的是我現在還在努力學習中，不斷探索，只是我有時候停止腳步，趴下來，把肩膀露出來，墊更多的人從我的肩膀上去，去看到曾讓我著迷的東西，和我一起去探索背後的原理。



## (Lock-free之二)

本節增加test_and_set的原語和一個spinlock比較完整的實現（參照nginx spin lock），主要的變化在於插入了__asm__ ("pause")指令，且插入次數是嘗試鎖的次數的2次冪，有助於在減少重試次數，通過這一變化可以對比看出CPU100%佔用的問題得到了緩解。test_and_set指令在未來的博客中還會繼續提到，cas，tas，fetch_and_add是這一系列中需要使用的僅有的3條原子操作。

test_and_set的語義為傳入一個地址將其無條件置1，並返回該地址值的原來值，如果原來是1，返回1，如果原來是非1，返回非1，這提供了一個關門的好處，如果變量是非1，在進入臨界區以後，該變量自動關門，後面的嘗試全部失敗因為是while(tas(V)==1)的操作，在使用完臨界區以後，將該值請零（reset），必然有一個會在spin的過程中拿到，但是這種調度是有偶然性的，因此可能會出現飢餓，而且併發數越多的情況下，越容易出現飢餓，spin的次數不易過大，本文指定為4096，該值可以根據實際環境調整，參見nginx源碼。


```sh
//編譯方法

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

我搜了一下也有一些不錯的文章供大家參考

我要啦免費統計
（1）http://student.csdn.net/space.php?uid=45153&do=thread&id=7403

（2）http://www.ibm.com/developerworks/cn/linux/l-rwlock_writing/