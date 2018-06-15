#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

int global=0;
pthread_mutex_t mutex;

void thread(void* local) {
	unsigned long long i;
	for (i=0; i<100000000; i++) {
		pthread_mutex_lock(&mutex);     //進入critical section
		global+=1;
		pthread_mutex_unlock(&mutex);   //離開critical section
    }
	printf("%d\n",global);
}

int main(void) {
	pthread_t id1, id2;
	pthread_mutex_init(&mutex, NULL);	//mutex預設是unlock
	printf("sizeof(DS)=%d\n",(int)sizeof(atomic_int));		
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	return (0);
}