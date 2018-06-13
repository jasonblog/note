#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

int global=0;
pthread_mutex_t mutex;
__thread char* threadID;  //thread local storage，後面會介紹

void printCPU(int sigNum) {
    //cpu_set_t cpu_set;
    //sched_getaffinity(0, sizeof(cpu_set_t), &cpu_set);
	if (strcmp(threadID, "1"))
    	printf("cpu# is    %d\n", sched_getcpu());
	else
		printf("cpu# is    \t%d\n", sched_getcpu());
}

void thread(char* id) {
    threadID = id;
	int i;
	for (i=0; i<1000000000; i++) {
		pthread_mutex_lock(&mutex);
		global+=1;
		pthread_mutex_unlock(&mutex);
	}
}

int main(void) {
	pthread_t id1, id2;
    struct itimerval itime = {0};
	itime.it_interval.tv_sec=1;
	itime.it_value.tv_sec=1;
	signal(SIGPROF, printCPU);
    setitimer(ITIMER_PROF, &itime, NULL);
	pthread_mutex_init(&mutex, NULL);	//mutex預設是unlock
	pthread_create(&id1,NULL,(void *) thread, "1"); 
	pthread_create(&id2,NULL,(void *) thread, "2");
	pthread_join(id1,NULL); 
	pthread_join(id2,NULL);
	printf("1000000000+1000000000 = %d\n", global);
}