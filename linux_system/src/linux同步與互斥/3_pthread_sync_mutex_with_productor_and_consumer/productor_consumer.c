#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_BUFFER  5

int buffer = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_productor = PTHREAD_COND_INITIALIZER;

void* consumer_th(void* arg)
{
    pthread_mutex_lock(&mutex);


    for (;;) {
        sleep(1);

        if (buffer <= 0) {
            printf("=%s->%d=====consumer wait productor to product..., buffer=%d\n",
                   __func__, __LINE__, buffer);
            pthread_cond_wait(&cond_consumer, &mutex);
        }

        buffer--;
        printf("=%s->%d====consumer consume a buffer, buffer=%d\n", __func__, __LINE__,
               buffer);

        if (0 == buffer) {
            pthread_cond_signal(&cond_productor);
            printf("=%s->%d====Notify productor...,, buffer: %d\n", __func__, __LINE__,
                   buffer);
        }
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* productor_th(void* arg)
{

    for (;;) {
        sleep(1);

        pthread_mutex_lock(&mutex);

        if (buffer >= MAX_BUFFER) {
            printf("=%s->%d=====productor wait consume buffer to 0..., buffer=%d\n",
                   __func__, __LINE__, buffer);
            pthread_cond_signal(&cond_productor);
        }

        buffer++;
        printf("=%s->%d====Product add a buffer..., buffer: %d\n", __func__, __LINE__,
               buffer);

        if (MAX_BUFFER == buffer) {
            pthread_cond_signal(&cond_consumer);
            printf("=%s->%d====Notify consumer...,, buffer: %d\n", __func__, __LINE__,
                   buffer);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, const char* argv[])
{
    pthread_t   tid_productor, tid_consumer;

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
