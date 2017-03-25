#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* func_th(void* arg)
{
    unsigned int*     val = (unsigned int*)arg;

    printf("=======%s->%d==thread%d: %u====\n", __func__, __LINE__,
           *val, (unsigned int)pthread_self());
    return NULL;
}

int main(int argc, const char* argv[])
{
    pthread_t   tid1, tid2;
    int         a, b;

    a = 1;

    if (0 != pthread_create(&tid1, NULL, func_th, &a)) {
        printf("pthread_create failed!\n");
        return -1;
    }

    b = 2;

    if (0 != pthread_create(&tid2, NULL, func_th, &b)) {
        printf("pthread_create failed!\n");
        return -1;
    }


    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
