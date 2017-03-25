# （三）：posix線程實現單個生產者和單個消費者模型


在第一節說到了生產者消費者問題，這一節我們來實現這樣一個稍作修改的模型： 初始時緩衝區為空，生產者向緩衝區寫入數據，消費者在緩衝區為空的情況下睡眠，當生產者寫滿緩衝區一半之後後通知消費者可以開始消費，生產者開始睡眠，直到消費者消費完緩衝區一半後通知生產者可以開始生產為止，其中生產者和消費者對緩衝區的訪問時互斥的。
下面來看一下實現：


```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER 6

int buffer = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_productor = PTHREAD_COND_INITIALIZER;

void* consumer_th(void* arg)
{

    for (;;) {
        pthread_mutex_lock(&mutex);

        if (buffer <= 0) {
            printf("=%s->%d=====consumer wait productor to product, start sleep..., buffer=%d\n",
                   __func__, __LINE__, buffer);
            pthread_cond_wait(&cond_consumer,
                              &mutex);//當消費者發現緩衝區為空時開始睡眠
        }

        buffer--; //消費者消耗一個緩衝區
        printf("=%s->%d====consumer consume a buffer, buffer=%d\n", __func__, __LINE__,
               buffer);

        if (buffer < MAX_BUFFER / 2) {
            pthread_cond_signal(
                &cond_productor); //當消費者發現緩衝區不足一半時通知生產者開始生產
            printf("=%s->%d====Notify productor...,, buffer: %d\n", __func__, __LINE__,
                   buffer);
        }

        pthread_mutex_unlock(&mutex);

        sleep(3);
    }


    return NULL;
}

void* productor_th(void* arg)
{

    for (;;) {
        sleep(1);

        pthread_mutex_lock(&mutex);

        if (buffer >= MAX_BUFFER) {
            printf("=%s->%d=====productor wait consumer to consume, start sleep..., buffer=%d\n",
                   __func__, __LINE__, buffer);
            pthread_cond_wait(&cond_productor, &mutex);//當緩衝區滿時，暫停生產
        }

        buffer++;//生產者增加一個緩衝區
        printf("=%s->%d====Product add a buffer..., buffer: %d\n", __func__, __LINE__,
               buffer);

        if (buffer > MAX_BUFFER / 2) {
            pthread_cond_signal(
                &cond_consumer);//當緩衝區增加到一半時，通知消費者可以開始消費
            printf("=%s->%d====Notify consumer...,, buffer: %d\n", __func__, __LINE__,
                   buffer);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, const char* argv[])
{
    pthread_t tid_productor, tid_consumer;

    if (0 != pthread_create(&tid_consumer, NULL, consumer_th, NULL)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    if (0 != pthread_create(&tid_productor, NULL, productor_th, NULL)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    pthread_join(tid_productor, NULL);
    pthread_join(tid_consumer, NULL);

    return 0;
}
```

下面是程序運行輸出結果：


```sh
=consumer_th->20=====consumer wait productor to product, start sleep..., buffer=0//消費者開始睡眠等待生產者喚醒  
=productor_th->54====Product add a buffer..., buffer: 1  
=productor_th->54====Product add a buffer..., buffer: 2  
=productor_th->54====Product add a buffer..., buffer: 3  
=productor_th->54====Product add a buffer..., buffer: 4//生產者開始寫緩衝區，當寫到第4（超過一半）個通知消費者  
=productor_th->58====Notify consumer...,, buffer: 4  
=consumer_th->25====consumer consume a buffer, buffer=3//消費者一邊消費緩衝區  
=productor_th->54====Product add a buffer..., buffer: 4//生產者一邊生產緩衝區  
=productor_th->58====Notify consumer...,, buffer: 4  
=productor_th->54====Product add a buffer..., buffer: 5  
=productor_th->58====Notify consumer...,, buffer: 5  
=consumer_th->25====consumer consume a buffer, buffer=4  
=productor_th->54====Product add a buffer..., buffer: 5  
=productor_th->58====Notify consumer...,, buffer: 5  
=productor_th->54====Product add a buffer..., buffer: 6  
=productor_th->58====Notify consumer...,, buffer: 6  
=productor_th->49=====productor wait consumer to consume, start sleep..., buffer=6//當生產者生產緩衝區滿時，開始睡眠  
=consumer_th->25====consumer consume a buffer, buffer=5  
=consumer_th->25====consumer consume a buffer, buffer=4  
=consumer_th->25====consumer consume a buffer, buffer=3  
=consumer_th->25====consumer consume a buffer, buffer=2//當消費者消費緩衝區到不足一半時，喚醒生產者開始生產  
=consumer_th->28====Notify productor...,, buffer: 2  
=productor_th->54====Product add a buffer..., buffer: 3  
=productor_th->54====Product add a buffer..., buffer: 4  
=productor_th->58====Notify consumer...,, buffer: 4  
=productor_th->54====Product add a buffer..., buffer: 5  
=productor_th->58====Notify consumer...,, buffer: 5  
=consumer_th->25====consumer consume a buffer, buffer=4  
=productor_th->54====Product add a buffer..., buffer: 5  
=productor_th->58====Notify consumer...,, buffer: 5  
=productor_th->54====Product add a buffer..., buffer: 6  
=productor_th->58====Notify consumer...,, buffer: 6  
=productor_th->49=====productor wait consumer to consume, start sleep..., buffer=6  
```

http://download.csdn.net/detail/gentleliu/8290615