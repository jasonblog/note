#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

int global=0;
pthread_mutex_t mutex;

void thread(void) {
	int i;
	for (i=0; i<1000000000; i++) {
		pthread_mutex_lock(&mutex);     //進入critical section
		global+=1;
		pthread_mutex_unlock(&mutex);   //離開critical section
	}
}

int main(void) {
	pthread_t id1, id2;
	pthread_mutex_init(&mutex, NULL);	//mutex預設是unlock
	pthread_create(&id1,NULL,(void *) thread,NULL); 
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL); 
	pthread_join(id2,NULL);
	printf("1000000000+1000000000 = %d\n", global);
}