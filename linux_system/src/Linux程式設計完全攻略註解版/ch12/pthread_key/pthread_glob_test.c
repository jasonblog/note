#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

int key = 100;
void* helloworld_one(char* argc)
{
    printf("the message is %s\n", argc);
    key = 10;
    printf("key=%d,the child id is %u\n", key, pthread_self());
    return 0;
}

void* helloworld_two(char* argc)
{
    printf("the message is %s\n", argc);
    sleep(1);
    printf("key=%d,the child id is %u\n", key, pthread_self());
    return 0;
}
int main()
{

    pthread_t thread_id_one;
    pthread_t thread_id_two;

    pthread_create(&thread_id_one, NULL, (void*)*helloworld_one, "helloworld");
    pthread_create(&thread_id_two, NULL, (void*)*helloworld_two, "helloworld");
    pthread_join(thread_id_one, NULL);
    pthread_join(thread_id_two, NULL);
}
