# Linux平臺延時之sleep、usleep、nanosleep、select比較

- sleep的精度是秒
- usleep的精度是微妙，不精確
- select的精度是微妙，精確

```c 
struct timeval delay;
 delay.tv_sec = 0;
 delay.tv_usec = 20 * 1000; // 20 ms
 select(0, NULL, NULL, NULL, &delay);
```
- nanosleep的精度是納秒，不精確
- unix、linux系統盡量不要使用usleep和sleep而應該使用nanosleep，使用nanosleep應注意判斷返回值和錯誤代碼，否則容易造成cpu佔用率100%。

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

void* pthread_do(void* arg)
{
    struct timespec ts, ts1;
    int count = 0;

    ts.tv_nsec = 500000000;    // 1500ms
    ts.tv_sec = 1;

    while (1) {
        printf("sub pthread %d\n", count);
        count++;

        if (nanosleep(&ts, &ts1) == -1) {
            printf("error!\n");
            exit(1);
        }
    }

    return NULL;
}

int main(void)
{
    pthread_t pthd;
    int count = 0;

    pthd = pthread_create(&pthd, NULL, pthread_do, NULL);

    while (1) {
        printf("main pthread %d\n", count);
        count++;
        sleep(1);
    }

    return 0;

}
```

```sh
gcc -W -Wall -pedantic -ansi -std=gnu99 -g -O0 test_nanosleep.c -o test_nanosleep -lm -lpthread
```