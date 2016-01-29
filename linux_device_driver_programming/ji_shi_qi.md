# 計時器

介紹驅動程式特有的時間管理機制。 



###9-1、Kernel 的計時器管理 
kernel 管理的 timer 大致分成兩類：
- 現在的日期與時間
- jiffies

其中 jiffies 指的就是 kernel 的 timer。

###9-2、現在的日期與時間 
「現在的日期與時間」指的是 1970 年 1月1日0時0分0秒至今為止的秒數，進而換算成日期與時間。 


#### 9-2.1、do_gettimeofday() 
在 kernel 內想取得目前時刻，可以呼叫 do_gettimeofday()： 

```c
void do_gettimeofday(struct timeval *tv); 
```
timeval 結構定義在「linux/time.h」標頭檔內： 
```c
struct timeval {
    time_t tv_sec; /* seconds */
    suseconds_t tv_usec; /*microseconds */
};
```
do_gettimeofday 的範例： 

```c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/delay.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sample_init(void)
{
    struct timeval now;
    suseconds_t diff;
    volatile int i;

    printk(KERN_ALERT "driver loaded: %s\n", __func__);

    do_gettimeofday(&now);
    diff = now.tv_usec;     /* microseconds */

#if 0
    printk("Current UTC: %lu (%lu)\n", now.tv_sec, now.tv_usec);
    for (i = 0 ; i < 9999 ; i++)
        ;
#else
    udelay(100);

#endif

    do_gettimeofday(&now);
    diff = now.tv_usec - diff;

    printk("Elapsed time: %lu\n", diff);

    return 0;
}

static void sample_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

####9-2.2、2038 年問題 
如果以 32-bit 變數存放經過的秒數，則會在 2038年1月19日 產生溢位，有可能導致程式運作出錯，稱為「2038年問題(Year 2038 problem; Y2K38)」。  <br>
解決的方式之一，是把 time_t 改成 64-bit，Linux 也是採用這個解法。  <br>

### 9-3、jiffies 
OS 內部會使用 timer interrupt 定期執行某些 kernel 的工作。 <br>
timer interrupt 是使用硬體的計時功能，在指定的時間間隔向 OS 發出 interrupt。  <br>
linux 在收到 timer interrupt 時，會遞增 jiffies 變數。  <br>

#### 9-3.1、HZ 巨集 
前面提到 jiffies 是 timer interrupt 的次數紀錄。 
timer interrupt 的間隔是由 HZ 巨集決定的，HZ 的意義就是「hertz」。 

```c
#ifdef __KERNEL__ 
define HZ CONFIG_HZ /* Internal kernel timer frequency */ 
define USER_HZ 100 /* .. some suer interfaces are in "ticks" */ 
define CLOCKS_PER_SEC /* like times() */ 
#endif
```

假設 HZ 的值是 100，這樣代表每秒 timer interrupt 會發生 100 次，也就是每 0.01 秒發生一次：

- Timer interrupt 發生間隔 = 1/HZ (秒)

增加 HZ 的值時，就會增加 timer interrupt 的次數，也就是增加 kernel 內部需要處理的工作量。 
kernel 內部需要定期處理各種工作，這些工作就是由 timer interrupt 定期引發的。 
`要縮短 kernel 的反應間隔，方法只有增加 timer interrupt 一途，但這樣會讓 kernel 消秏更多 CPU 資源。 
`
#### 9-3.2、遞增計數 
實際增加 jiffies 的工作是由 do_timer() 完成的。 
```c
void do_timer(unsigned long ticks) {
    jiffies_64 += ticks;
    update_times(ticks);
}
```

####9-3.3、驅動程式的取用方式 
驅動程式想取用「jiffies」或「jiffies_64」，必須先 include 進「linux/jiffies.h」。 
不過只要 include 進 linux/module.h 就會一併載入 jiffies.h ，所以不必明確 include 進 jiffies.h。 

使用 jiffies 的範例： 

```c
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sample_init(void)
{
    printk("driver loaded: %lu\n", jiffies);

    return 0;
}

static void sample_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

#### 9-3.4、497日問題 
jiffies 會隨會時間不斷向上遞增，如果 HZ 巨集的時間設為 「100」 的話，每 10 毫秒就會有一次 timer interrupt，如果 jiffies 的值增加了「200」的話，就知道是經過「2秒」。 
jiffies 定義為 unsigned long，在 IA-32 上是 32-bit 整數，而32-bit 整數的最大值是 4294967295(2^32-1)，所以

```c
4294967295/100 => 42949672(秒)/86400 => 497(日)
```

這就稱為「497日問題」。 

驅動程式如果用到 jiffies 的話，一定要特別注意這個問題，應對方式為：
- 以 unsigned long 變數儲存 jiffies
- 計算 timeout 時，只從 jiffies 減去先前的值


程式碼範例為： 
```c
#define OVER_TICKS (3*HZ) 
unsigned long start_time = jiffies; 
/* .... */ 
if (jiffies - start_time >= OVER_TICKS) { 
... /* 經過三秒後要做的事 */ 
} 
```
#### 9-3.5、jiffies 初始值

linux 2.4 的 jiffies 是以 0 作為初始值，也知道 jiffies 會在第 497 天 overflow。 
但為了徹底篩選出沒有處理 overflow 的程式碼， linux 2.6 開始把 jiffies 的初始值設為： 「-300秒」。 
也就是說，只要五分鐘就會 overflow(變成 0 的時候就會 overflow)。 

### 9-4、等待 

有時為了讓硬體完成工作，驅動程式需要等待一段固定時間，此時可以使用定義在 linux/delay.h 的函式： 

```c
void mdelay(unsigned long msecs); 
void udelay(unsigned long usecs); 
void ndelay(unsigned long nsecs); 
```

延遲的單位分別為毫秒(10^-3)、微秒(10^-6)、奈秒(10^-9)。 


這些等待函式，`在等待的過程中都會佔住 CPU，不會讓 CPU 去處理其它工作，這樣子的方式，稱為「busy waiting(忙錄等待)」。 `
系統如果只有一個 CPU 的話，等待時，會讓整個 OS 停住(但是可以處理 interrupt)，所以一般的認知為：
不要長時間 busy waiting
busy waiting 的好處是可以在 interrupt context 使用。 

busy waiting 的範例： 

```c
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sample_init(void)
{
    printk("driver loaded: %lu\n", jiffies);

    mdelay(100);  /* 100 msec */
    udelay(1000); /* 1000 micro second(0.001 msec) */
    ndelay(1000); /* 1000 nano second(0.001 micro second) */

    printk("%lu (HZ %d)\n", jiffies, HZ);

    return 0;
}

static void sample_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);

```


#### 9-4.1、不忙碌的等待 

如果在等待過程中，希望 CPU 去做其它事的話，可以用定義在「linux/delay.h」中 sleep 系列的等待函式： 

```c
void ssleep(unsigned int seconds); 
void msleep(unsigned int msecs); 
unsigned long msleep_interruptible(unsigned int msecs); 
```

延遲的單位分別為秒、毫秒(10^-3)、毫秒(10^-3)。 
msleep_interruptible() 與 msleep() 不同的地方在於等待過程可能被中斷，`中斷指的是 process 可以接收 signal。 `
如果在 msleep_interruptible() 中收到 signal 而中斷等待的話，則會回傳距離原始時限的時間(正值)，否則的話傳回「0」。 

這幾個函式都會使 driver context 進入 sleep，所以不能在 interrupt context 中使用，因為 interrupt context 位於最高級，如果在 interrupt 處理時式之內 sleep 的話，就不會執行其它 process，也不會有其它人來叫醒 interrupt 處理程式，會造成全系統鎖死。 

sleep() 的範例： 

```c
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("Dual BSD/GPL");

static int sample_init(void)
{
    printk("driver loaded: %lu\n", jiffies);

    ssleep(10);   /* sleep for 10 seconds. */

    printk("%lu (HZ %d)\n", jiffies, HZ);

    return 0;
}

static void sample_exit(void)
{
    printk(KERN_ALERT "driver unloaded\n");
}

module_init(sample_init);
module_exit(sample_exit);
```

在執行 ssleep() 時，終端就就算按「Ctrl+C」鍵送出訊號也不會造成中斷。 
以 ps 來看 insmod 的狀態，會發現是 「D(uninterruptible sleep)」。 
而在執行 msleep_interruptible() 時，若按「Ctrl+C」鍵送出中斷訊號，則會發現 process 被中斷了，並回傳正值。 
以 ps 來看 insmod 的狀態，會發現是 「S(interruptible sleep)」。 

### 9-5、Kernel Timer 

驅動程式在對硬體送出 DMA 指令後，為了監視 DMA 執行完成，有時需要在一定時間過後呼叫某些函式，linux kernel 有為這類計時需求提供機制，這個機制就是「kernel timer」。 
舉例來說，驅動程式對 I/O 裝置下令開始 DMA 後，通常在 I/O 裝置完工後，都會送出 interrupt 讓驅動程式繼續作處理。 
不過驅動程式有個鐵則，那就是：

- 不能假定硬體一定能正確完成工作

I/O 裝置有可能故障或發生其它問題，這時也不能讓 OS 當掉，必須重新，讓應用程式可以繼續執行，或是通知應用程式硬體發生問題。 
應用程式對驅動程式發出請求的情況下，驅動程式必須對這個請求送出回應，如果沒有在時限內收到 interrupt，必須能偵測出這種情形，並將錯誤碼回報給應用程式。 

