#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 

int gettid() {
	return syscall(SYS_gettid);
}

void thread(void) {
	printf("pthread_t tid = %p\n", (void*)pthread_self());
	printf("tid = %d\n", gettid());
	while(1);
}

int main(void) {
	pthread_t id1, id2;
	printf("my pid = %d\n", getpid());
	pthread_create(&id1,NULL,(void *) thread,NULL);
	pthread_create(&id2,NULL,(void *) thread,NULL);
	//pthread_join(id1,NULL);
	//pthread_join(id2,NULL);
	getchar();
	return (0);
}
