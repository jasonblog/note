#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int gval = 100;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

void* func_add_th(void* arg)
{
    int*     val = (int*)arg;

    pthread_mutex_lock(&mutex);
    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    printf("before add 1, gval=%d\n", gval);
    gval += 1;

    sleep(4);

    printf("after add 1, gval=%d\n", gval);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* func_sub_th(void* arg)
{
    int*     val = (int*)arg;

    if (0 != pthread_mutex_trylock(&mutex)) {
        printf("failed to lock!\n");
        return NULL;
    }

    printf("==do %s==thread%d: %u====\n", __func__,
           *val, (unsigned int)pthread_self());

    gval -= 1;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, const char* argv[])
{
    pthread_t   tid1, tid2;
    int         a, b;

    a = 1;

    if (0 != pthread_create(&tid1, NULL, func_add_th, &a)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    sleep(1);
    b = 2;

    if (0 != pthread_create(&tid2, NULL, func_sub_th, &b)) {
        printf("pthread_create failed!\n");
        return -1;
    }


    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
