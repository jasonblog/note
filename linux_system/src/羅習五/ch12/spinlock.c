#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

int global=0;
pthread_spinlock_t spinlock;

void thread(void) {
	int i;
	for (i=0; i<1000000000; i++) {
		pthread_spin_lock(&spinlock);
		global+=1;
		pthread_spin_unlock(&spinlock);
	}
}
int main(void) {
	pthread_t id1, id2;
	pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	printf("1000000000+1000000000 = %d\n", global);
	return (0);
}

