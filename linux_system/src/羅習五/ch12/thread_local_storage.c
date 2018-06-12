#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

volatile int global=0;
pthread_mutex_t mutex;
__thread int thread_local = 0;

void thread(void) {
	int i;
	for (i=0; i<1000000000; i++)
		thread_local+=1;
	printf("@thread_local = %p\n", &thread_local);
		
	pthread_mutex_lock(&mutex);
		global+=thread_local;
	pthread_mutex_unlock(&mutex);
}

int main(void) {
	pthread_t id1, id2;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	printf("1000000000+1000000000 = %d\n", global);
	return (0);
}
