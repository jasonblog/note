#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

int global=0;
pthread_mutex_t mutex;

void* thread(void* para) {
	int i;
	for (i=0; i<1000000; i++) {
		pthread_mutex_lock(&mutex);
		global+=1;
		pthread_mutex_unlock(&mutex);
	}
}

int main(void) {
	pthread_t id1, id2;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ADAPTIVE_NP);
	pthread_mutex_init(&mutex, &attr);
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	printf("1000000+1000000 = %d\n", global);
	return (0);
}

