#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void* thread_function(void* arg);

int main(int argc, char* argv[])
{
    int res;
    pthread_t a_thread;
    void* thread_result;

    res = pthread_create(&a_thread, NULL, thread_function, NULL);

    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Cancelling thread...\n");
    sleep(10);
    res = pthread_cancel(a_thread);

    if (res != 0) {
        perror("Thread cancelation failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for thread to finish...\n");
    sleep(10);
    res = pthread_join(a_thread, &thread_result);

    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void* thread_function(void* arg)
{
    int i, res, j;
    sleep(1);
    res = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    if (res != 0) {
        perror("Thread pthread_setcancelstate failed");
        exit(EXIT_FAILURE);
    }

    printf("thread cancle type is disable,can't cancle this thread\n");

    for (i = 0; i < 3; i++) {
        printf("Thread is running (%d)...\n", i);
        sleep(1);
    }

    res = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    if (res != 0) {
        perror("Thread pthread_setcancelstate failed");
        exit(EXIT_FAILURE);
    } else {
        printf("Now change ths canclestate is ENABLE\n");
    }

    sleep(200);
    pthread_exit(0);
}

