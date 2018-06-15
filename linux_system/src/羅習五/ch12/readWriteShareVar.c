/*
使用方法：有參數就會呼叫sleep
沒參數，就會做密集的spin
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdatomic.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>


volatile atomic_int complete=0; //complete是二個thread共用的變數
const long long nspersec = 1000000000;

long long timespec2ns(struct timespec ts) {
    long long ns = ts.tv_nsec;
    ns += ts.tv_sec * nspersec;
    return ns;
}

void workingThread(void) {
    volatile double result;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (volatile long long j=0; j<100000; j++)
	for (volatile long long i=0; i<100000; i++)
		result += (double)i*j;
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long delta = timespec2ns(end) - timespec2ns(start);
    atomic_store_explicit(&complete, 1, memory_order_relaxed);
    printf("res=%f, time = %lld.%lld\n", result, delta/nspersec, delta%nspersec);
}

volatile int doSleep=0;
void waitingThread(void) {
    int local=0;
    while (local != 1) {    //密集迴圈不斷的讀取complete
        //local=atomic_load_explicit(&complete, memory_order_relaxed);
        local=atomic_load_explicit(&complete, memory_order_seq_cst);
        if (doSleep==1) sleep(1);   //如果doSleep設定為1，則不會執行密集的讀取，每秒讀取一次
    }
}

int main(int argc, char** argv) {
    printf("argc=%d\n", argc);
    if (argc == 2) doSleep =1;
	pthread_t id1, id2;
	pthread_create(&id1,NULL,(void *) workingThread,NULL);
	pthread_create(&id2,NULL,(void *) waitingThread,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
}