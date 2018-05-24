#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "Mutex.h"

int ret = 1234;
Mutex mLock;

void* show_message(void* ptr)
{
    char* message;
    int x = 5;

    Mutex::Autolock _l(mLock);

    message = (char*) ptr;

    for (x = 5 ; x > 0 ; --x) {
        printf("%s \n", message);
        usleep(300);
    }

    pthread_exit((void*)&ret);
}

int main()
{
    pthread_t thread1;
    pthread_t thread2;
    int* ret1 = NULL;
    int* ret2 = NULL;

    pthread_create(&thread1, NULL, show_message, (void*) "Thread 1");
    pthread_create(&thread2, NULL, show_message, (void*) "Thread 2");
    pthread_join(thread1, (void**)&ret1);
    pthread_join(thread2, (void**)&ret2);
    printf("return value from thread1 = %d\n", *ret1);
    printf("return value from thread2 = %d\n", *ret2);

    return 0;
}
