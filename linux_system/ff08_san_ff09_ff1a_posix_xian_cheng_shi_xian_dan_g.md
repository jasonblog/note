# （三）：posix线程实现单个生产者和单个消费者模型


在第一节说到了生产者消费者问题，这一节我们来实现这样一个稍作修改的模型： 初始时缓冲区为空，生产者向缓冲区写入数据，消费者在缓冲区为空的情况下睡眠，当生产者写满缓冲区一半之后后通知消费者可以开始消费，生产者开始睡眠，直到消费者消费完缓冲区一半后通知生产者可以开始生产为止，其中生产者和消费者对缓冲区的访问时互斥的。
下面来看一下实现：


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
                              &mutex);//当消费者发现缓冲区为空时开始睡眠
        }

        buffer--; //消费者消耗一个缓冲区
        printf("=%s->%d====consumer consume a buffer, buffer=%d\n", __func__, __LINE__,
               buffer);

        if (buffer < MAX_BUFFER / 2) {
            pthread_cond_signal(
                &cond_productor); //当消费者发现缓冲区不足一半时通知生产者开始生产
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
            pthread_cond_wait(&cond_productor, &mutex);//当缓冲区满时，暂停生产
        }

        buffer++;//生产者增加一个缓冲区
        printf("=%s->%d====Product add a buffer..., buffer: %d\n", __func__, __LINE__,
               buffer);

        if (buffer > MAX_BUFFER / 2) {
            pthread_cond_signal(
                &cond_consumer);//当缓冲区增加到一半时，通知消费者可以开始消费
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

下面是程序运行输出结果：


```sh
=consumer_th->20=====consumer wait productor to product, start sleep..., buffer=0//消费者开始睡眠等待生产者唤醒  
=productor_th->54====Product add a buffer..., buffer: 1  
=productor_th->54====Product add a buffer..., buffer: 2  
=productor_th->54====Product add a buffer..., buffer: 3  
=productor_th->54====Product add a buffer..., buffer: 4//生产者开始写缓冲区，当写到第4（超过一半）个通知消费者  
=productor_th->58====Notify consumer...,, buffer: 4  
=consumer_th->25====consumer consume a buffer, buffer=3//消费者一边消费缓冲区  
=productor_th->54====Product add a buffer..., buffer: 4//生产者一边生产缓冲区  
=productor_th->58====Notify consumer...,, buffer: 4  
=productor_th->54====Product add a buffer..., buffer: 5  
=productor_th->58====Notify consumer...,, buffer: 5  
=consumer_th->25====consumer consume a buffer, buffer=4  
=productor_th->54====Product add a buffer..., buffer: 5  
=productor_th->58====Notify consumer...,, buffer: 5  
=productor_th->54====Product add a buffer..., buffer: 6  
=productor_th->58====Notify consumer...,, buffer: 6  
=productor_th->49=====productor wait consumer to consume, start sleep..., buffer=6//当生产者生产缓冲区满时，开始睡眠  
=consumer_th->25====consumer consume a buffer, buffer=5  
=consumer_th->25====consumer consume a buffer, buffer=4  
=consumer_th->25====consumer consume a buffer, buffer=3  
=consumer_th->25====consumer consume a buffer, buffer=2//当消费者消费缓冲区到不足一半时，唤醒生产者开始生产  
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