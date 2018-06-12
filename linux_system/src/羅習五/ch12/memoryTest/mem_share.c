#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

volatile int share=0;
void thread(void* local) {
	unsigned long long i;
	for (i=0; i<10000000000; i++)
		share+=1;
	printf("%d\n", share);
}

int main(void) {
	pthread_t id1, id2;
	printf("sizeof(DS)=%d\n",(int)sizeof(int));		
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	return (0);
}