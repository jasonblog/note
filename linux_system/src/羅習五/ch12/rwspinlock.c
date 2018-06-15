#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h> 
#include <stdatomic.h>
#include <sys/syscall.h>
#include <signal.h>

__thread int thread_local_id;
//atomic_int count;

const int maxTicket = 1000;
volatile atomic_int nTicket;	//一定要加上volatile，否則-O3會出錯

volatile atomic_int wrt_in_cs_success;
volatile atomic_int rd_in_cs_success;
//volatile atomic_int rd_in_parallel;
volatile atomic_int n_rd_parallel;
volatile int total_parallel;
volatile atomic_int rd_count;

void alarmHandler(int sigNo) {  //for debugging
    printf("nTicket = %d\n", nTicket);
    printf("total number of reader entering CS = %d/sec\n", rd_in_cs_success);
    printf("total number of writer entering CS = %d/sec\n", wrt_in_cs_success);
    printf("parallel level of read = %f\n", ((float)total_parallel)/rd_count);
    atomic_store(&rd_in_cs_success,0);
    atomic_store(&wrt_in_cs_success,0);
    alarm(1);
}

void init_rwspinlock() {
    atomic_store(&nTicket, maxTicket);
}

void wrt_lock() {
    while(1) {
        atomic_fetch_sub(&nTicket, maxTicket);
        int ret=atomic_load(&nTicket);
        if (ret == 0) {   //success
            atomic_fetch_add(&wrt_in_cs_success, 1);    //for debugging
            return;
        }
        else {
            atomic_fetch_add(&nTicket, maxTicket);
        }
    }
}

void wrt_unlock() {
    atomic_fetch_add(&nTicket, maxTicket);
    //printf("wrt_unlock, nTicket = %d\n", nTicket);
}



void rd_lock() {
    while(1) {
        atomic_fetch_sub(&nTicket, 1);
        int ret = atomic_load(&nTicket);
        if (ret > 0) {    //success
            atomic_fetch_add(&n_rd_parallel, 1);   //for debugging
            total_parallel += atomic_load(&n_rd_parallel);  //for debugging
            atomic_fetch_add(&rd_count, 1);    //for debugging
            return;
        }
        else
            atomic_fetch_add(&nTicket, 1);   //redo
    }
}

void rd_unlock() {
    atomic_fetch_sub(&n_rd_parallel, 1);
    //atomic_fetch_sub(&rd_in_parallel, 1);   //for debugging
    atomic_fetch_add(&nTicket, 1);
}

void rd_thread(void* para) {
    for (;;) {
        rd_lock();
        atomic_fetch_add_explicit(&rd_in_cs_success, 1, memory_order_relaxed);  //for debugging
        for (int i=0; i< random()%100; i++) //computing
            ;
        rd_unlock();

        for (int i=0; i< random()%100; i++) //remainder section
            ;
    }
}

void wrt_thread(void* para) {   //注意，可以有多個writer
    while(1) {
        wrt_lock();
        //printf("enter writer's CS\n");
        for (int i=0; i< random()%100; i++)    //computing, read-write
            ;
        wrt_unlock();
        //printf("writer exit CS\n");
        for (int i=0; i< random()%100; i++) //remainder section
            ;
    }
}

int main(int argc, char** argv) {
	pthread_t* r_id;
    pthread_t* w_id;
    int nThread = 4;
    if (argc == 2)
        sscanf(argv[1], "%d", &nThread);
    printf("建立%d個reader\n", nThread);
    printf("建立%d個writer\n", nThread);
    printf("按下Enter鍵繼續");
    getchar();
    r_id = (pthread_t*)malloc(sizeof(pthread_t) * nThread);
    w_id = (pthread_t*)malloc(sizeof(pthread_t) * nThread);

    init_rwspinlock();
    for (int i=0; i<nThread; i++) {
	    pthread_create(&r_id[i], NULL, (void *)wrt_thread, NULL);
        pthread_create(&w_id[i], NULL, (void *)rd_thread, NULL);
    }

    signal(SIGALRM, alarmHandler);
    alarm(1);

    for (int i=0; i<nThread; i++) {
	    pthread_join(r_id[i],NULL);
        pthread_join(w_id[i],NULL);
    }
}