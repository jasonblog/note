#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>

pthread_mutex_t mutex;

//子线程运行的函数。它首先获得互斥锁mutex，然后暂停5s, 在释放该互斥锁
void *another(void *arg)
{
    printf("in child thread, lock the mutex\n");
    pthread_mutex_lock(&mutex);
    sleep(5);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, const char *argv[])
{
    pthread_mutex_init(&mutex, NULL);
    pthread_t id;
    pthread_create(&id, NULL, another, NULL);
    
    //父进程的主线程暂停1s，以确保在执行fork操作之前，子线程已经开始运行并获得了互斥变量mutex
    sleep(1);
    int pid = fork();
    if(pid < 0)
    {
        pthread_join(id, NULL);
        pthread_mutex_destroy(&mutex);
        return 1;
    }
    else if (pid == 0) 
    {
        printf("I am in the child, want to get the lock\n");
        //子进程从父进程继承了互斥mutex的状态，该互斥处于锁住的状态，这是由父进程中的子线程执行pthread_mutex_lock引起的，
        //因此，下面这句加锁操作会一直阻塞，尽管从逻辑上来说它是不应该阻塞的
        pthread_mutex_lock(&mutex);
        printf("I can not run to here, oop .....\n");
        pthread_mutex_unlock(&mutex);
        exit(0);
    }
    else
    {
        wait(NULL);
    }
    pthread_join(id, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}
