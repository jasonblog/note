#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void* helloworld(char* argc);
int main(int argc, int argv[])
{
    int error;
    int* temptr;

    pthread_t thread_id;

    pthread_create(&thread_id, NULL, (void*)*helloworld, "helloworld");
    printf("*p=%x,p=%x\n", *helloworld, helloworld);

    if (error = pthread_join(thread_id, (void**)&temptr)) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    printf("temp=%x,*temp=%c\n", temptr, *temptr);
    *temptr = 'd';
    printf("%c\n", *temptr);
    free(temptr);
    return 0;
}

void* helloworld(char* argc)
{
    int* p;
    p = (int*)malloc(10 * sizeof(int));
    printf("the message is %s\n", argc);
    printf("the child id is %u\n", pthread_self());
    memset(p, 'c', 10);
    printf("p=%x\n", p);
    pthread_exit(p);
    //return 0;
}
