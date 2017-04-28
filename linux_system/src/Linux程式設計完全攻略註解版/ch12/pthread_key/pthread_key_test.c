//this is the test code for pthread_key

#include <stdio.h>
#include <pthread.h>

pthread_key_t key;
void echomsg(void* t)
{
    printf("destructor excuted in thread %u,param=%u\n", pthread_self(), ((int*)t));
}

void* child1(void* arg)
{
    int i = 10;
    int tid = pthread_self();
    printf("\nset key value %d in thread %u\n", i, tid);
    pthread_setspecific(key, &i);
    printf("thread one sleep 2 until thread two finish\n");
    sleep(2);
    printf("\nthread %u returns %d,add is %u\n", tid,
           *((int*)pthread_getspecific(key)), (int*)pthread_getspecific(key));
}

void* child2(void* arg)
{
    int temp = 20;
    int tid = pthread_self();
    printf("\nset key value %d in thread %u\n", temp, tid);
    pthread_setspecific(key, &temp);
    sleep(1);
    printf("thread %u returns %d,add is %u\n", tid,
           *((int*)pthread_getspecific(key)), (int*)pthread_getspecific(key));
}

int main(void)
{
    pthread_t tid1, tid2;
    pthread_key_create(&key, echomsg);
    pthread_create(&tid1, NULL, (void*)child1, NULL);
    pthread_create(&tid2, NULL, (void*)child2, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_key_delete(key);
    return 0;
}
