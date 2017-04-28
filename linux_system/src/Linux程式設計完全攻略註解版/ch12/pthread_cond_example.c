#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pthread.h"

#define BUFFER_SIZE 2

/* Circular buffer of integers. */
struct prodcons {
    int buffer[BUFFER_SIZE];      /* the actual data */
    pthread_mutex_t lock;         /* mutex ensuring exclusive access to buffer */
    int readpos, writepos;        /* positions for reading and writing */
    pthread_cond_t notempty;      /* signaled when buffer is not empty */
    pthread_cond_t notfull;       /* signaled when buffer is not full */
};

/* Initialize a buffer */
void init(struct prodcons* prod)
{
    pthread_mutex_init(&prod->lock, NULL);
    pthread_cond_init(&prod->notempty, NULL);
    pthread_cond_init(&prod->notfull, NULL);
    prod->readpos = 0;
    prod->writepos = 0;
}
/* Store an integer in the buffer */
void put(struct prodcons* prod, int data)
{
    pthread_mutex_lock(&prod->lock);

    /* Wait until buffer is not full */
    while ((prod->writepos + 1) % BUFFER_SIZE == prod->readpos) {
        printf("producer wait for not full\n");
        pthread_cond_wait(&prod->notfull, &prod->lock);
    }

    /* Write the data and advance write pointer */
    prod->buffer[prod->writepos] = data;
    prod->writepos++;

    if (prod->writepos >= BUFFER_SIZE) {
        prod->writepos = 0;
    }

    /*Signal that the buffer is now not empty */
    pthread_cond_signal(&prod->notempty);
    pthread_mutex_unlock(&prod->lock);
}
/* Read and remove an integer from the buffer */
int get(struct prodcons* prod)
{
    int data;
    pthread_mutex_lock(&prod->lock);

    /* Wait until buffer is not empty */
    while (prod->writepos == prod->readpos) {
        printf("consumer wait for not empty\n");
        pthread_cond_wait(&prod->notempty, &prod->lock);
    }

    /* Read the data and advance read pointer */
    data = prod->buffer[prod->readpos];
    prod->readpos++;

    if (prod->readpos >= BUFFER_SIZE) {
        prod->readpos = 0;
    }

    /* Signal that the buffer is now not full */
    pthread_cond_signal(&prod->notfull);
    pthread_mutex_unlock(&prod->lock);
    return data;
}

#define OVER (-1)
struct prodcons buffer;
/*--------------------------------------------------------*/
void* producer(void* data)
{
    int n;

    for (n = 0; n < 5; n++) {
        printf("producer sleep 1 second......\n");
        sleep(1);
        printf("put the %d product\n", n);
        put(&buffer, n);
    }

    for (n = 5; n < 10; n++) {
        printf("producer sleep 3 second......\n");
        sleep(3);
        printf("put the %d product\n", n);
        put(&buffer, n);
    }

    put(&buffer, OVER);
    printf("producer stopped!\n");
    return NULL;
}
/*--------------------------------------------------------*/
void* consumer(void* data)
{
    int d = 0;

    while (1) {
        printf("consumer sleep 2 second......\n");
        sleep(2);
        d = get(&buffer);
        printf("get the %d product\n", d);

        //      d = get(&buffer);
        if (d == OVER) {
            break;
        }
    }

    printf("consumer stopped!\n");
    return NULL;
}
/*--------------------------------------------------------*/
int main(int argc, char* argv[])
{
    pthread_t th_a, th_b;
    void* retval;
    init(&buffer);
    pthread_create(&th_a, NULL, producer, 0);
    pthread_create(&th_b, NULL, consumer, 0);
    /* Wait until producer and consumer finish. */
    pthread_join(th_a, &retval);
    pthread_join(th_b, &retval);
    return 0;
}

