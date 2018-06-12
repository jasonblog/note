#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

atomic_int atomic=0;
void thread(void* local) {
	unsigned long long i;
	for (i=0; i<100000000; i++)
		atomic_fetch_add(&atomic, 1);
	printf("%d\n", atomic);
}

int main(void) {
	pthread_t id1, id2;
	printf("sizeof(DS)=%d\n",(int)sizeof(atomic_int));		
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	return (0);
}