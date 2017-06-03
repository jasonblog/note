#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#define THREADNUM 5
pthread_rwlock_t rwlock;
void* readers(void* arg)
{
    pthread_rwlock_rdlock(&rwlock);
    printf("reader %ld got the lock\n", (long)arg);
    pthread_rwlock_unlock(&rwlock);
    pthread_exit((void*)0);
}
void* writers(void* arg)
{
    pthread_rwlock_wrlock(&rwlock);
    printf("writer %ld got the lock\n", (long)arg);
    pthread_rwlock_unlock(&rwlock);
    pthread_exit((void*)0);
}
int main(int argc, char** argv)
{
    int iRet, i;
    pthread_t writer_id, reader_id;
    pthread_attr_t attr;
    int nreadercount = 1, nwritercount = 1;
    iRet = pthread_rwlock_init(&rwlock, NULL);

    if (iRet) {
        fprintf(stderr, "init lock failed\n");
        return iRet;
    }

    pthread_attr_init(&attr);
    /*pthread_attr_setdetachstate用来设置线程的分离状态
    也就是说一个线程怎么样终止自己，状态设置为PTHREAD_CREATE_DETACHED
    表示以分离状态启动线程*/
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    for (i = 0; i < THREADNUM; i++) {
        if (i % 3) {
            pthread_create(&reader_id, &attr, readers, (void*)nreadercount);
            printf("create reader %d\n", nreadercount++);
        } else {
            pthread_create(&writer_id, &attr, writers, (void*)nwritercount);
            printf("create writer %d\n", nwritercount++);
        }
    }

    sleep(5);/*sleep是为了等待另外的线程的执行*/
    return 0;
}
