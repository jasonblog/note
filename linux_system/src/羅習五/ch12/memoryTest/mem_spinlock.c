#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

int global=0;
pthread_spinlock_t spinlock;

void thread(void* local) {
	unsigned long long i;
	for (i=0; i<100000000; i++) {
		pthread_spin_lock(&spinlock);
		global+=1;
		pthread_spin_unlock(&spinlock);
	}
	printf("%d\n", global);
}

int main(void) {
	pthread_t id1, id2;
	pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
	printf("sizeof(DS)=%d\n",(int)sizeof(atomic_int));		
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	return (0);
}