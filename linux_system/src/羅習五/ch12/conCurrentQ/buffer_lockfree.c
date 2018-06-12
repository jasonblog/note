#define _GNU_SOURCE             /* See feature_test_macros(7) */
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

#define bufsize 10
unsigned long long buffer[bufsize];
//__Alignas(int)
_Alignas(int) volatile int in=0, out=0, num=0;;    //新進的資料放在buffer[in]，讀取資料從buffer[out]
unsigned long long putWaitingOverhead =0;
unsigned long long getWaitingOverhead =0;
int producerProgress = 0;
int consumerProgress = 0;

void printBufStatus(int signo) {    //每100usec印出buffer資料
        printf("buf size = %d, in =%d, out =%d\n", num, in, out);
        printf("producerProgress = %d, consumerProgress = %d\n", producerProgress, consumerProgress);
}

void put() {
    //puts("put: enter\n");
	static int item=1;  //用於除錯，放入buffer的資料都是嚴格遞增的數列
    while ((in+1)%bufsize == out) {
        //puts("put: waiting\n");
        //putWaitingOverhead++;
        ;
    }
    //atomic_fetch_add(&num, 1);
    //puts("put: add a new item\n");
	buffer[in]=item++;  //將資料放入
    in = (in + 1)%bufsize;
    //atomic_thread_fence(???)
    //atomic_fetch_add(&in, 1);    //規劃下筆資料應該擺放的地點
    
}

void get() {
	static int item=0;  //用於除錯，紀錄上一次讀取的數字為何
	int tmpItem;    //讀取的數字暫時放在tmpItem
    while (in == out) {
        //getWaitingOverhead++;
        ;
    }
    //atomic_fetch_sub(&num, 1);
	tmpItem=buffer[out];    //讀出buffer的東西
    out = (out + 1)%bufsize;
    //atomic_thread_fence(???)
    //atomic_fetch_add(&out, 1); //規劃下次要提取資料的地方
    
	if ((tmpItem - item) != 1) {
        fprintf(stderr, "error: tmpItem - item = %d, in=%d, out=%d\n", tmpItem - item, in, out); //用於除錯，如果拿到的資料不是嚴格遞增，表示程式邏輯有錯
        getchar();
    }
	item=tmpItem;   //用於除錯，紀錄這次拿到的數字
}

void producer(void* name) {     //生產資料的執行緒
    //ULLONG_MAX
    for (producerProgress=0; producerProgress<100000000; producerProgress++) {
        put();
        //delay loop
        //for (int k=0; i<random()%1000;k++)
            ;
    }
    
}

void consumer(void* name) {     //消化資料的執行緒
    //nice(5);
    for (consumerProgress=0; consumerProgress<100000000; consumerProgress++) {
        get();
        //delay loop
        //for (int k=0; i<random()%1000;k++)
            ;
    }
}
int main(void) {
	pthread_t id1, id2, id3, id4;

    struct itimerval itime = {0};
	itime.it_interval.tv_usec=1000;
	itime.it_value.tv_usec=1000;
	signal(SIGPROF, printBufStatus);
    //setitimer(ITIMER_PROF, &itime, NULL);

	pthread_create(&id1,NULL,(void *) producer,NULL);
    //pthread_create(&id2,NULL,(void *) producer,NULL);     //這個程式只支援一個producer一個consumer，拿掉這個註解會發生錯誤
	pthread_create(&id3,NULL,(void *) consumer,NULL);
    //pthread_create(&id4,NULL,(void *) consumer,NULL);     //這個程式只支援一個producer一個consumer，拿掉這個註解會發生錯誤
	pthread_join(id1,NULL);
	//pthread_join(id2,NULL);
    pthread_join(id3,NULL);
	//pthread_join(id4,NULL);
    //printf("waiting overhead is %lld", putWaitingOverhead+getWaitingOverhead);
}
