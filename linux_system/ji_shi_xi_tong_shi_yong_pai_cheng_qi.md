# 即時系統 使用排程器


安裝 linux kernel 2.6.32.68、練習 pthread 設定 scheduler 相關函數以利後續修改 kernel source code。

設定 scheduler 問題

- 設定 thread 可以在哪些 core 運行，由於要測試執行順序,強制讓 pthread 都在 CPU 0 運行。

編譯時，增加 ```#define _GNU_SOURCE``` 在 ```#include <pthread>``` 之前，確定 `cpu_set_t` 型別存在。


```c
cpu_set_t mask;
CPU_ZERO(&mask);
CPU_SET(0, &mask);
sched_setaffinity(0, sizeof(mask), &mask);
```

- 使用 sched_setscheduler() 時，需查閱 `int policy` 相關規定去設定 `struct sched_param` 的 priority 數值。例如 `SCHED_FIFO` 的 priority 介於 1 到 99 之間,若 priority 錯誤，sched_setscheduler() 會發生錯誤，錯誤訊息可以藉由 `printf("Message␣%s\n", mid, strerror(errno));` 查閱。


```c
sched_setaffinity(0, sizeof(mask), &mask);
int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param);
```


- 如果 pid = 0，相當於 caller 的 thread 會被設置對應的 policy 和 sched_param。在 policy 設定 規範中，有兩種 real time scheduler SCHED_FIFO 和 SCHED_RR。當 thread 設置 real time scheduler 時，需要用 root 權限來執行程式，否則會設置失敗。相反地，若使用 non-real time scheduler 則不需要 root 權限，如 SCHED_BATCH。


```c
struct sched_param param;
printf("Thread %d sched_setscheduler()\n", mid);
param.sched_priority = sched_get_priority_max(SCHED_FIFO);
s = sched_setscheduler(0, SCHED_FIFO, &param);
printf("Thread %d sched after %s\n", mid, strerror(errno));
```

## 三種方法設定 pthread

###方法一
各自的 pthread_create() 後，再呼叫 sched_setscheduler() 進行設定。這種寫法在本次實驗有順序問題，在 create 之後，無法保證執行 sched_setscheduler() 的順序，無法依靠 main thread 中執行 pthread_create() 的順序決定。幸運地，仍可以使用

```c
pthread_barrier_t barrier; 
pthread_barrier_wait(&barrier);
pthread_barrier_init(&barrier, NULL, MAX_THREAD);
```

確保每一個 thread 都設置好各自的 sched_setscheduler() 後統一運行，運行順序取決 priority。

###方法二

在 main thread 中，呼叫 pthread_attr_setinheritsched(attr, PTHREAD_INHERIT_SCHED); 接下來 create 出來的 pthread 都將繼承 main thread 的 scheduler 設定。這種寫法很方便，如果需要各自設置 priority 會不方便，倒不如直接用第三種寫法。


###方法三
在 main thread 中，手動設置每一個 pthread 的 scheduler。

```c
pthread_attr_t *attr = new pthread_attr_t;
struct sched_param *param = new struct sched_param;
// increasing order
param->sched_priority = sched_get_priority_max(SCHED_FIFO) - i;
// set scheduler
pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED);
pthread_attr_setschedpolicy(attr, SCHED_FIFO);
pthread_attr_setschedparam(attr, param);
// create thread
pthread_create(&tid[i], attr, running_print, create_arg(i+1, scheduler))
```

##計時設置

為確定每一個 thread 按照設定的 scheduler 運行，方式採用 print() 進行，但有可能輸出之間間隔距離過短，導致順序容易在輸出上發生無法預期的問題，適時地加入一秒間隔完成。

如何準確地停止一秒？不能呼叫 sleep() 因為這類的 system call 會產生 interrupt，此時 thread 將會降低 priority 或者進入隊列尾端。佔據 CPU time 有以下三種

###busy work

只要停留的夠久即可，下面是一種簡單的方法

```c
// busy 1 second
double a = 0;
for (int i = 0; i < 10000000; i++)	a += 0.1f;
```
###系統時間

利用 critical section 抓系統時間 int gettimeofday (struct timeval * tv, struct timezone * tz);，這寫法會共用同一份時間，因此需要使用 mutex 完成。仍使用一個迴圈來判定時間是否超過，實際運行時間會超過一秒。

```c
static double my_clock(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return 1e-6 * t.tv_usec + t.tv_sec;
}
pthread_mutex_lock(&outputlock);
{
    double sttime = my_clock();
    while (1) {
        if (my_clock() - sttime > 1)
            break;
    }
}
pthread_mutex_unlock(&outputlock);
```

###執行緒運行時間

事實上 Linux 有提供每一個 thread 各自的 clock，用來計算 thread 的執行時間。使用 clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t)，在舊版本需在編譯參數中加入 -lrt 才能使用。


```c
static double my_clock(void) {
    struct timespec t;
    assert(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
    return 1e-9 * t.tv_nsec + t.tv_sec;
}
```

### 輸出 console 問題

在 SCHED_FIFO 這類的 real time scheduler 情況下，thread 輸出無法即時顯示在 terminal 上。

從 http://man7.org/linux/man-pages/man7/sched.7.html 可以見到 real-time scheduler 安排工作的比例，由於 print() 之後的顯示處理並不是 real-time process，當 real-time process 完全用盡 95\% 的 CPU time 時，顯示處理可能沒辦法在一秒內完成，可能要用好幾秒中的 5% 來完成。這也就是造成一開始會停頓一陣子，隨後才一次輸出數行結果。(註：並不是每一種裝置都會造成這問題，跟 CPU 時脈有關。)


The value in this file specifies how much of the period time can be used by all real-time and deadline scheduled processes on the system. The value in this file can range from -1 to INT_MAX-1. Specifying -1 makes the runtime the same as the period; that is, no CPU time is set aside for non-real-time processes (which was the Linux behavior before kernel 2.6.25). The default value in this file is 950,000 (0.95 seconds), meaning that 5% of the CPU time is reserved for processes that don’t run under a real-time or deadline scheduling policy.

解決這類的問題，可以從 kernel 設定著手，降低 real-time 最大佔有量，讓 print() 處理能即刻印出。這不是好的解決辦法，但可用於實驗。

```sh
// default
$ sudo /sbin/sysctl -w kernel.sched_rt_runtime_us=950000
// modify
$ sudo /sbin/sysctl -w kernel.sched_rt_runtime_us=500000
```

又或者採用拉大間隔，讓每一個 thread 都停止數十秒以上。


```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

static double my_clock(void)
{
    struct timespec t;
    assert(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
    return 1e-9 * t.tv_nsec + t.tv_sec;
}

static void busy_work(int mid)
{
    // pthread_mutex_lock(&outputlock);
    {
        double sttime = my_clock();

        while (1) {
            if (my_clock() - sttime > mid) {
                break;
            }
        }

        // pthread_mutex_unlock(&outputlock);
    }
}

void* pthread_do(void* arg)
{
    struct timeval begin, end;
    //int count = 0;
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    sched_setscheduler(0, SCHED_FIFO, &param);

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);

    while (1) {
        //count++;
        gettimeofday(&begin, NULL);
        busy_work(1);
        gettimeofday(&end, NULL);
        double span = end.tv_sec - begin.tv_sec +
                      (end.tv_usec - begin.tv_usec) / 1000000.0;
        printf("sub pthread diff time: %.12f\n", 1 - span);
    }

    return NULL;
}

int main(void)
{
    struct timeval begin, end;
    pthread_t pthd;
    //int count = 0;

    pthd = pthread_create(&pthd, NULL, pthread_do, NULL);

    while (1) {
        //count++;
        gettimeofday(&begin, NULL);
        sleep(1);
        gettimeofday(&end, NULL);
        double span = end.tv_sec - begin.tv_sec +
                      (end.tv_usec - begin.tv_usec) / 1000000.0;
        printf("main pthread diff time: %.12f\n", 1 - span);
    }

    return 0;
}
```
