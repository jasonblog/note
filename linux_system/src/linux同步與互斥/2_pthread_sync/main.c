#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int gval = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* func1_th(void* arg)
{
    pthread_mutex_lock(&mutex);


    if (5 != gval) {
        printf("=======%s->%d===thread 1 wait start!====\n", __func__, __LINE__);
        pthread_cond_wait(&cond, &mutex);
    }

    /**
     * do something here
     */
    printf("=======%s->%d====thread 1 do something!===\n", __func__, __LINE__);

    pthread_mutex_unlock(&mutex);

    return NULL;
}

#if 0
void* func1_th(void* arg)
{

    //for (;;) {
    sleep(2);
    pthread_mutex_lock(&mutex);

    /*
    printf("=======%s->%d=======\n", __func__, __LINE__);

    if (5 != gval) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    */
    /**
     * do something here
     */
    printf("=======%s->%d====thread 1 do something!===\n", __func__, __LINE__);
    //}

    return NULL;
}
#endif

void* func2_th(void* arg)
{
    int i;

    pthread_mutex_lock(&mutex);

    for (i = 0; i < 10; i++) {

        printf("=======%s->%d=======\n", __func__, __LINE__);

        sleep(1);
        gval++;

        if (5 == gval) {
            pthread_cond_signal(&cond);
            printf("=======%s->%d===signal====\n", __func__, __LINE__);
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(int argc, const char* argv[])
{
    pthread_t   tid1, tid2;

    if (0 != pthread_create(&tid1, NULL, func1_th, NULL)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    sleep(1);

    if (0 != pthread_create(&tid2, NULL, func2_th, NULL)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}
