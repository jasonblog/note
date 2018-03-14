---
title: Linux 時間函數
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: linux,program
description: 簡單介紹下 Linux 中與時間相關的函數。
---

簡單介紹下 Linux 中與時間相關的函數。


<!-- more -->

## 獲取時間函數

簡單介紹下與獲取時間相關的系統調用。

### time() 秒級

{% highlight c %}
#include <time.h>

//----- time_t一般為long int，不過不同平臺可能不同，打印可通過printf("%ju", (uintmax_t)ret)打印
time_t time(time_t *tloc);

char *ctime(const time_t *timep);                // 錯誤返回NULL
char *ctime_r(const time_t *timep, char *buf);   // buf至少26bytes，返回與buf相同
{% endhighlight %}

如果 tloc 不為 NULL，那麼數據同時會保存在 tloc 中，成功返回從 epoch 開始的時間，單位為秒；否則返回 -1 。

### ftime() 毫秒級

{% highlight c %}
#include <sys/timeb.h>
struct timeb {
    time_t   time;                // 為1970-01-01至今的秒數
    unsigned   short   millitm;   // 毫秒
    short   timezonel;            // 為目前時區和Greenwich相差的時間，單位為分鐘，東區為負
    short   dstflag;              // 非0代表啟用夏時制
};
int ftime(struct timeb *tp);
{% endhighlight %}

總是返回 0 。


### gettimeofday() 微秒級

`gettimeofday()` 函數可以獲得當前系統的絕對時間。

{% highlight c %}
struct timeval {
    time_t      tv_sec;
    suseconds_t tv_usec;
};
int gettimeofday ( struct timeval * tv , struct timezone * tz )
{% endhighlight %}

可以通過如下函數測試。

{% highlight c %}
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
int main(void)
{
    int i=10000000;
    struct timeval begin, end;
    gettimeofday(&begin, NULL);
    while (--i);
    gettimeofday(&end, NULL);
    double span = end.tv_sec-begin.tv_sec + (end.tv_usec-begin.tv_usec)/1000000.0;
    printf("time: %.12f\n", span);
    return 0;
}
{% endhighlight %}




### clock_gettime() 納秒級

編譯連接時需要加上 ```-lrt```，不過不加也可以編譯，應該是非實時的。```struct timespect *tp``` 用來存儲當前的時間，其結構和函數聲明如下，返回 0 表示成功，-1 表示失敗。

{% highlight c %}
struct timespec {
    time_t tv_sec;    /* seconds */
    long tv_nsec;     /* nanoseconds */
};
int clock_gettime(clockid_t clk_id, struct timespect *tp);
{% endhighlight %}

clk_id 用於指定計時時鐘的類型，簡單列舉如下幾種，詳見 ```man 2 clock_gettime``` 。

* CLOCK_REALTIME/CLOCK_REALTIME_COARSE<br>
系統實時時間 (wall-clock time)，即從 ```UTC 1970.01.01 00:00:00``` 開始計時，隨系統實時時間改變而改變，包括通過系統函數手動調整系統時間，或者通過 ```adjtime()``` 或者 NTP 調整時間。

* CLOCK_MONOTONIC/CLOCK_MONOTONIC_COARSE<br>
從系統啟動開始計時，不受系統時間被用戶改變的影響，但會受像 ```adjtime()``` 或者 NTP 之類漸進調整的影響。

* CLOCK_MONOTONIC_RAW<br>
與上述的 ```CLOCK_MONOTONIC``` 相同，只是不會受 ```adjtime()``` 以及 NTP 的影響。

* CLOCK_PROCESS_CPUTIME_ID<br>
本進程到當前代碼系統 CPU 花費的時間。

* CLOCK_THREAD_CPUTIME_ID<br>
本線程到當前代碼系統 CPU 花費的時間。

可以通過如下程序測試。

{% highlight c %}
#include <time.h>
#include <stdio.h>

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

int main(void)
{
    struct timespec t, begin, end;
    int temp, i;
    clock_gettime(CLOCK_REALTIME, &t);
    printf("CLOCK_REALTIME: %d, %d\n", t.tv_sec, t.tv_nsec);
    clock_gettime(CLOCK_MONOTONIC, &t);
    printf("CLOCK_MONOTONIC: %d, %d\n", t.tv_sec, t.tv_nsec);
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
    printf("CLOCK_THREAD_CPUTIME_ID: %d, %d\n", t.tv_sec, t.tv_nsec);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin);
    printf("CLOCK_PROCESS_CPUTIME_ID: %d, %d\n", begin.tv_sec, begin.tv_nsec);
    for (i = 0; i < 242000000; i++)
        temp+=temp;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    t = diff(begin, end);
    printf("diff CLOCK_PROCESS_CPUTIME_ID: %d, %d\n", t.tv_sec, t.tv_nsec);
}
{% endhighlight %}

<!--

### 時間轉換相關

將時間轉換為自 1970.01.01 以來逝去時間的秒數，發生錯誤時返回 -1 。

{% highlight text %}
struct tm {
    int tm_sec;         /* seconds */
    int tm_min;         /* minutes */
    int tm_hour;        /* hours */
    int tm_mday;        /* day of the month */
    int tm_mon;         /* month */
    int tm_year;        /* year */
    int tm_wday;        /* day of the week */
    int tm_yday;        /* day in the year */
    int tm_isdst;       /* daylight saving time */
};

time_t mktime(struct tm * timeptr);
{% endhighlight %}

如果時間在夏令時，tm_isdst 設置為1，否則設置為0，若未知，則設置為 -1。



### ANSI clock()

clock() 返回值類型是 clock_t，該值除以 CLOCKS_PER_SEC (GNU 定義為 1000000) 得出消耗的 CPU 時間，一般用兩次 clock() 來計算進程自身運行的時間。不過，該函數存在如下的問題：

* 對於 32bits 如果超過 72 分鐘，就有可能會導致溢出；
* 該函數沒有考慮 CPU 被子進程使用的情況；
* 也不能區分用戶空間和內核空間。

因此，該函數在 Linux 系統上幾乎沒有意義，可以通過如下程序進行簡單的測試。

{% highlight c %}
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int i = 1000;
    clock_t start, finish;
    double  duration;
    printf( "Time to do %ld empty loops is ", i );
    start = clock();
    while (--i){
        system("cd");
    }
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    printf("%f seconds\n", duration);
    return 0;
}
{% endhighlight %}

接下來，可以通過如下方式進行測試。

{% highlight text %}
$ gcc test.c -o test
$ time ./test
Time to do 1000 empty loops is 0.070000 seconds

real    0m1.471s
user    0m0.551s
sys     0m0.749s
{% endhighlight %}

實際上，程序調用 ```system("cd");``` 主要是系統模式子進程的消耗，如上程序不能體現這一點 。




二)times()時間函數
1)概述:
原型如下：
clock_t times(struct tms *buf);


tms結構體如下:
strace tms{
 clock_t tms_utime;
 clock_t tms_stime;
 clock_t tms_cutime;
 clock_t tms_cstime;
}

註釋:
tms_utime記錄的是進程執行用戶代碼的時間.
tms_stime記錄的是進程執行內核代碼的時間.
tms_cutime記錄的是子進程執行用戶代碼的時間.
tms_cstime記錄的是子進程執行內核代碼的時間.

2)測試:
vi test2.c
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static void do_cmd(char *);
static void pr_times(clock_t, struct tms *, struct tms *);


int main(int argc, char *argv[]){
        int i;
        for(i=1; argv[i]!=NULL; i++){
                do_cmd(argv[i]);
        }
        exit(1);
}
static void do_cmd(char *cmd){
        struct tms tmsstart, tmsend;
        clock_t start, end;
        int status;
        if((start=times(&tmsstart))== -1)
                puts("times error");
        if((status=system(cmd))<0)
                puts("system error");
        if((end=times(&tmsend))== -1)
                puts("times error");
        pr_times(end-start, &tmsstart, &tmsend);
        exit(0);
}
static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend){
        static long clktck=0;
        if(0 == clktck)
                if((clktck=sysconf(_SC_CLK_TCK))<0)
                           puts("sysconf err");
        printf("real:%7.2f\n", real/(double)clktck);
        printf("user-cpu:%7.2f\n", (tmsend->tms_utime - tmsstart->tms_utime)/(double)clktck);
        printf("system-cpu:%7.2f\n", (tmsend->tms_stime - tmsstart->tms_stime)/(double)clktck);
        printf("child-user-cpu:%7.2f\n", (tmsend->tms_cutime - tmsstart->tms_cutime)/(double)clktck);
        printf("child-system-cpu:%7.2f\n", (tmsend->tms_cstime - tmsstart->tms_cstime)/(double)clktck);
}

編譯:
gcc test2.c -o test2

測試這個程序:
time ./test2 "dd if=/dev/zero f=/dev/null bs=1M count=10000"
10000+0 records in
10000+0 records out
10485760000 bytes (10 GB) copied, 4.93028 s, 2.1 GB/s
real:   4.94
user-cpu:   0.00
system-cpu:   0.00
child-user-cpu:   0.01
child-system-cpu:   4.82

real    0m4.943s
user    0m0.016s
sys     0m4.828s

3)總結:
(1)通過這個測試,系統的time程序與test2程序輸出基本一致了.
(2)(double)clktck是通過clktck=sysconf(_SC_CLK_TCK)來取的,也就是要得到user-cpu所佔用的時間,就要用
(tmsend->tms_utime - tmsstart->tms_utime)/(double)clktck);
(3)clock_t times(struct tms *buf);返回值是過去一段時間內時鐘嘀嗒的次數.
(4)times()函數返回值也是一個相對時間.


三)實時函數clock_gettime
在POSIX1003.1中增添了這個函數,它的原型如下：
int clock_gettime(clockid_t clk_id, struct timespec *tp);

它有以下的特點:
1)它也有一個時間結構體:timespec ,timespec計算時間次數的單位是十億分之一秒.
strace timespec{
 time_t tv_sec;
 long tv_nsec;
}

2)clockid_t是確定哪個時鐘類型.
CLOCK_REALTIME: 標準POSIX實時時鐘
CLOCK_MONOTONIC: POSIX時鐘,以恆定速率運行;不會復位和調整,它的取值和CLOCK_REALTIME是一樣的.
CLOCK_PROCESS_CPUTIME_ID和CLOCK_THREAD_CPUTIME_ID是CPU中的硬件計時器中實現的.

3)測試:
#include<time.h>
#include<stdio.h>
#include<stdlib.h>

#define MILLION 1000000

int main(void)
{
        long int loop = 1000;
        struct timespec tpstart;
        struct timespec tpend;
        long timedif;
        clock_gettime(CLOCK_MONOTONIC, &tpstart);
        while (--loop){
                system("cd");
        }

        clock_gettime(CLOCK_MONOTONIC, &tpend);
        timedif = MILLION*(tpend.tv_sec-tpstart.tv_sec)+(tpend.tv_nsec-tpstart.tv_nsec)/1000;
        fprintf(stdout, "it took %ld microseconds\n", timedif);

        return 0;
}

編譯:
gcc test3.c -lrt -o test3


計算時間:
time ./test3
it took 3463843 microseconds

real    0m3.467s
user    0m0.512s
sys     0m2.936s


2)
clock()函數的精確度是10毫秒(ms)
times()函數的精確度是10毫秒(ms)


3)測試4種函數的精確度:
vi test4.c

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <time.h>
#include    <sys/times.h>
#include    <sys/time.h>
#define WAIT for(i=0;i<298765432;i++);
#define MILLION    1000000
    int
main ( int argc, char *argv[] )
{
    int i;
    long ttt;
    clock_t s,e;
    struct tms aaa;

    s=clock();
    WAIT;
    e=clock();
    printf("clock time : %.12f\n",(e-s)/(double)CLOCKS_PER_SEC);

    long tps = sysconf(_SC_CLK_TCK);
    s=times(&aaa);
    WAIT;
    e=times(&aaa);
    printf("times time : %.12f\n",(e-s)/(double)tps);

    struct timeval tvs,tve;
    gettimeofday(&tvs,NULL);
    WAIT;
    gettimeofday(&tve,NULL);
    double span = tve.tv_sec-tvs.tv_sec + (tve.tv_usec-tvs.tv_usec)/1000000.0;
    printf("gettimeofday time: %.12f\n",span);

    struct timespec tpstart;
    struct timespec tpend;

    clock_gettime(CLOCK_REALTIME, &tpstart);
    WAIT;
    clock_gettime(CLOCK_REALTIME, &tpend);
    double timedif = (tpend.tv_sec-tpstart.tv_sec)+(tpend.tv_nsec-tpstart.tv_nsec)/1000000000.0;
    printf("clock_gettime time: %.12f\n", timedif);

    return EXIT_SUCCESS;
}

gcc -lrt test4.c -o test4
debian:/tmp# ./test4
clock time : 1.190000000000
times time : 1.180000000000
gettimeofday time: 1.186477000000
clock_gettime time: 1.179271718000

六)內核時鐘

默認的Linux時鐘週期是100HZ,而現在最新的內核時鐘週期默認為250HZ.
如何得到內核的時鐘週期呢?

grep ^CONFIG_HZ /boot/config-2.6.26-1-xen-amd64

CONFIG_HZ_250=y
CONFIG_HZ=250

結果就是250HZ.

而用sysconf(_SC_CLK_TCK);得到的卻是100HZ
例如:
#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <time.h>
#include    <sys/times.h>
#include    <sys/time.h>

int
main ( int argc, char *argv[] )
{
    long tps = sysconf(_SC_CLK_TCK);
    printf("%ld\n", tps);

    return EXIT_SUCCESS;
}

為什麼得到的是不同的值呢？
因為sysconf(_SC_CLK_TCK)和CONFIG_HZ所代表的意義是不同的.
sysconf(_SC_CLK_TCK)是GNU標準庫的clock_t頻率.
它的定義位置在:/usr/include/asm/param.h

例如:
#ifndef HZ
#define HZ 100
#endif


最後總結一下內核時間:
內核的標準時間是jiffy,一個jiffy就是一個內部時鐘週期,而內部時鐘週期是由250HZ的頻率所產生中的,也就是一個時鐘滴答,間隔時間是4毫秒(ms).
也就是說:
1個jiffy=1個內部時鐘週期=250HZ=1個時鐘滴答=4毫秒

每經過一個時鐘滴答就會調用一次時鐘中斷處理程序，處理程序用jiffy來累計時鐘滴答數,每發生一次時鐘中斷就增1.
而每個中斷之後,系統通過調度程序跟據時間片選擇是否要進程繼續運行,或讓進程進入就緒狀態.
最後需要說明的是每個操作系統的時鐘滴答頻率都是不一樣的,LINUX可以選擇(100,250,1000)HZ,而DOS的頻率是55HZ.

七)為應用程序計時
用time程序可以監視任何命令或腳本佔用CPU的情況.

1)bash內置命令time
例如:
time sleep 1
real    0m1.016s
user    0m0.000s
sys     0m0.004s


2)/usr/bin/time的一般命令行
例如:
\time sleep 1
0.00user 0.00system 0:01.01elapsed 0%CPU (0avgtext+0avgdata 0maxresident)k
0inputs+0outputs (1major+176minor)pagefaults 0swaps


注：
在命令前加上斜槓可以繞過內部命令.
/usr/bin/time還可以加上-v看到更具體的輸出:
\time -v sleep 1
        Command being timed: "sleep 1"
        User time (seconds): 0.00
        System time (seconds): 0.00
        Percent of CPU this job got: 0%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:01.00
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 0
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 178
        Voluntary context switches: 2
        Involuntary context switches: 0
        Swaps: 0
        File system inputs: 0
        File system outputs: 0
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0

這裡的輸出更多來源於結構體rusage.


最後，我們看到real time大於user time和sys time的總和，這說明進程不是在系統調用中阻塞,就是得不到運行的機會.
而sleep()的運用，也說明瞭這一點.





http://en.wikipedia.org/wiki/Year_2038_problem


    Identifier  Description
    Time
    manipulation    difftime    computes the difference between times
    time    returns the current time of the system as time since the epoch (which is usually the Unix epoch)
    clock   returns a processor tick count associated with the process

    Format
    conversions     asctime     converts a tm object to a textual representation (deprecated)
    strftime    converts a tm object to custom textual representation
    wcsftime    converts a tm object to custom wide string textual representation
    gmtime  converts time since the epoch to calendar time expressed as Coordinated Universal Time[2]
    localtime   converts time since the epoch to calendar time expressed as local time
    mktime  converts calendar time to time since the epoch
    Constants   CLOCKS_PER_SEC  number of processor clock ticks per second


    tm  calendar time type
    time_t  time since the epoch type
    clock_t     process running time type

http://www.cnblogs.com/wenqiang/p/5678451.html
http://www.cnblogs.com/xmphoenix/archive/2011/05/09/2041546.html
-->


## gettimeofday() 效率

很多時候需要獲取當前時間，如計算 http 耗時，數據庫事務 ID 等，那麼 `gettimeofday()` 這個函數做了些什麼？內核 1ms 一次的時鐘中斷可以支持微秒精度嗎？如果在系統繁忙時，頻繁的調用它是否有問題嗎？

`gettimeofday()` 是 C 庫提供的函數，它封裝了內核裡的 `sys_gettimeofday()` 系統調用。

在 x86_64 體繫上，使用 `vsyscall` 實現了 `gettimeofday()` 這個系統調用，簡單來說，就是創建了一個共享的內存頁面，它的數據由內核來維護，但是，用戶態也有權限訪問這個內核頁面，由此，不通過中斷 `gettimeofday()` 也就拿到了系統時間。

### 函數作用

`gettimeofday()` 會把內核保存的牆上時間和 jiffies 綜合處理後返回給用戶。<!--解釋下牆上時間和jiffies是什麼：1、牆上時間就是實際時間（1970/1/1號以來的時間），它是由我們主板電池供電的（裝過PC機的同學都瞭解）RTC單元存儲的，這樣即使機器斷電了時間也不用重設。當操作系統啟動時，會用這個RTC來初始化牆上時間，接著，內核會在一定精度內根據jiffies維護這個牆上時間。2、jiffies就是操作系統啟動後經過的時間，它的單位是節拍數。有些體系架構，1個節拍數是10ms，但我們常用的x86體系下，1個節拍數是1ms。也就是說，jiffies這個全局變量存儲了操作系統啟動以來共經歷了多少毫秒。-->先看看 `gettimeofday()` 是如何做的，首先它調用了 `sys_gettimeofday()` 系統調用。

{% highlight c %}
asmlinkage long sys_gettimeofday(struct timeval __user *tv, struct timezone __user *tz)
{
    if (likely(tv != NULL)) {
        struct timeval ktv;
        do_gettimeofday(&ktv);
        if (copy_to_user(tv, &ktv, sizeof(ktv)))
            return -EFAULT;
    }
    if (unlikely(tz != NULL)) {
        if (copy_to_user(tz, &sys_tz, sizeof(sys_tz)))
            return -EFAULT;
    }
    return 0;
}
{% endhighlight %}

調用 `do_gettimeofday()` 取得當前時間存儲到變量 `ktv` 上，並調用 `copy_to_user()` 複製到用戶空間，每個體系都有自己的實現，這裡就簡單看下 x86_64 體系下 `do_gettimeofday()` 的實現：

{% highlight c %}
void do_gettimeofday(struct timeval *tv)
{
    unsigned long seq, t;
    unsigned int sec, usec;

    do {
        seq = read_seqbegin(&xtime_lock);

        sec = xtime.tv_sec;
        usec = xtime.tv_nsec / 1000;

        /* i386 does some correction here to keep the clock
           monotonous even when ntpd is fixing drift.
           But they didn't work for me, there is a non monotonic
           clock anyways with ntp.
           I dropped all corrections now until a real solution can
           be found. Note when you fix it here you need to do the same
           in arch/x86_64/kernel/vsyscall.c and export all needed
           variables in vmlinux.lds. -AK */

        t = (jiffies - wall_jiffies) * (1000000L / HZ) +
            do_gettimeoffset();
        usec += t;

    } while (read_seqretry(&xtime_lock, seq));

    tv->tv_sec = sec + usec / 1000000;
    tv->tv_usec = usec % 1000000;
}
{% endhighlight %}

可以看到，該函數只是把 `xtime` 與 `jiffies` 修正後返回給用戶，而 `xtime` 變量和 `jiffies` 的維護更新頻率，就決定了時間精度，而 `jiffies` 一般每 10ms 或者 1ms 才處理一次時鐘中斷，那麼這是不是意味著精度只到 1ms ？

### 微秒級精度

獲取時間是通過 High Precision Event Timer 維護，這個模塊會提供微秒級的中斷，並更新 xtime 和 jiffies 變量；接著，看下 x86_64 體系結構下的維護代碼：

{% highlight c %}
static struct irqaction irq0 = {
    timer_interrupt, SA_INTERRUPT, CPU_MASK_NONE, "timer", NULL, NULL
};
{% endhighlight %}

這個 `timer_interrupt()` 函數會處理 HPET 時間中斷，來更新 xtime 變量。

<!--
三、它的調用成本在所有的操作系統上代價一樣嗎？如果在系統繁忙時，1毫秒內調用多次有問題嗎？

最上面已經說了，對於x86_64系統來說，這是個虛擬系統調用vsyscall！所以，這裡它不用發送中斷！速度很快，成本低，調用一次的成本大概不到一微秒！

對於i386體系來說，這就是系統調用了！最簡單的系統調用都有無法避免的成本：陷入內核態。當我們調用gettimeofday時，將會向內核發送軟中斷，然後將陷入內核態，這時內核至少要做下列事：處理軟中斷、保存所有寄存器值、從用戶態複製函數參數到內核態、執行、將結果複製到用戶態。這些成本至少在1微秒以上！

四、關於jiffies值得一提的兩點

先看看它的定義：

[cpp] view plain copy

    volatile unsigned long __jiffies;


只談兩點。

1、它用了一個C語言裡比較罕見的關鍵字volatile，這個關鍵字用於解決併發問題。c語言編譯器很喜歡做優化的，它不清楚某個變量可能會被併發的修改，例如上面的jiffies變量首先是0，如果首先一個CPU修改了它的值為1，緊接著另一個CPU在讀它的值，例如 __jiffies = 0; while (__jiffies == 1)，那麼在內核的C代碼中，如果不加volatile字段，那麼第二個CPU裡的循環體可能不會被執行到，因為C編譯器在對代碼做優化時，生成的彙編代碼不一定每次都會去讀內存！它會根據代碼把變量__jiffies設為0，並一直使用下去！而加了volatile字段後，就會要求編譯器，每次使用到__jiffies時，都要到內存裡真實的讀取這個值。


2、它的類型是unsigned long，在32位系統中，最大值也只有43億不到，從系統啟動後49天就到達最大值了，之後就會清0重新開始。那麼jiffies達到最大值時的迴轉問題是怎麼解決的呢？或者換句話說，我們需要保證當jiffies迴轉為一個小的正數時，例如1，要比幾十秒毫秒前的大正數大，例如4294967290，要達到jiffies(1)>jiffies(4294967290)這種效果。

內核是通過定義了兩個宏來解決的：

[cpp] view plain copy

    #define time_after(a,b)     \
        (typecheck(unsigned long, a) && \
         typecheck(unsigned long, b) && \
         ((long)(b) - (long)(a) < 0))
    #define time_before(a,b)    time_after(b,a)


很巧妙的設計！僅僅把unsigned long轉為long類型後相減比較，就達到了jiffies(1)>jiffies(4294967290)效果，簡單的解決了jiffies的迴轉問題，贊一個。
-->




{% highlight text %}
{% endhighlight %}
