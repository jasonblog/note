#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdalign.h>
/**/
struct DS {
	int alignas(64) a;
	int alignas(64) b;
};

void thread(void* local) {
	int* _local=(int*) local;
	int i;
	for (i=0; i<1000000000; i++)
		*_local+=1;
}

int main(void) {
	pthread_t id1, id2;
	struct DS ds;
	ds.a=0; ds.b=0;
	printf("sizeof(DS)=%d\n",(int)sizeof(ds));		
	pthread_create(&id1,NULL,(void *) thread,&(ds.a));
	pthread_create(&id2,NULL,(void *) thread,&(ds.b));
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	return (0);
}
