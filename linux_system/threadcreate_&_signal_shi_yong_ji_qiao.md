# thread_create &  signal  使用技巧


- example.c 

```c
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <signal.h>
#include <time.h>

#define TIMES 5
#define ITERA 10000000000

#define gettid() syscall(__NR_gettid)
#define TAG "JASON"

#define dlog() do \
    { \
        fprintf(stderr, "%s:PID=[%d] TID=[%ld] func=[%s] [%s:%d]\n", \
                TAG, \
                getpid(), \
                gettid(), \
                __FUNCTION__, \
                __FILE__, \
                __LINE__); \
    } while(0)


static int g_count1 = 0;
static int g_count2 = 0;
static int g_count3 = 0;
static int g_flag = 1;

void sigroutine(int dunno)
{
    switch (dunno) {
    case 1:
        printf("Get a signal -- SIGHUP ");
        break;

    case 2:
        dlog();
        printf("Get a signal -- SIGINT ");
        g_flag = 0;

        /* 恢復 SIGINT 預設不接收 */
        signal(SIGINT, SIG_DFL);
        kill(getpid(), SIGINT);
        break;

    case 3:
        printf("Get a signal -- SIGQUIT ");
        break;
    }

    return;
}

void test_func()
{
    /*do something*/
#if 0
    printf("Hello\n");
#else
    g_count1++;
    g_count2++;
    g_count3++;
#endif
}

void thread2_f(void* args)
{
    unsigned itera, times;
    clock_t t1, td, ts;

    t1 = td = ts = 0;

    for (times = 0, ts = 0; times != TIMES; ++times) {
        t1 = clock();

        for (itera = 0; itera != ITERA; ++itera) {
            test_func();
        }

        td = clock() - t1;
        ts += td;  /* 計算總測時 */

        /* 輸出詳細資料 */
        printf("%u: %ld\n", times, td);
    }

    g_flag = 0;
    printf("tid=%ld, ave : %f\n", gettid(), (double)td / TIMES);
}

void thread1_f(void* args)
{
    while (g_flag) {
        printf("tid=%ld, g_count1=%d, g_count2=%d, g_count3=%d\n", gettid(), g_count1,
               g_count2, g_count3);
    }
}

int main(void)
{
    int  ret;
    pthread_t thread1, thread2;

    signal(SIGINT, sigroutine);

    ret = pthread_create(&thread1, NULL, (void* (*)(void*))thread1_f, NULL);

    if (ret != 0) {
        printf("Create pthread error!\n");
        exit(1);
    }

    ret = pthread_create(&thread2, NULL, (void* (*)(void*))thread2_f, NULL);

    if (ret != 0) {
        printf("Create pthread error!\n");
        exit(1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return (0);
}
```


```sh
gcc example.c -lpthread -o example
```