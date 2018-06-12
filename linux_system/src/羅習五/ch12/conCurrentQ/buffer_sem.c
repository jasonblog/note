#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdatomic.h>

#define bufsize 10
long buffer[bufsize];
int in=0, out=0;    //新進的資料放在buffer[in]，讀取資料從buffer[out]
atomic_int num;     //用於統計buf內共有多少資料，因為producer和consumer都可能同時修改num，因此需要宣告為atomic_int，後面會介紹
sem_t notFull, notEmpty;    //使用semaphore判斷buffer的狀態

void printBufStatus(int signo) {    //每100usec印出buffer資料
    printf("buf status is %d\n", num);
}

void put() {
	static int item=1;  //用於除錯，放入buffer的資料都是嚴格遞增的數列
    sem_wait(&notFull); //等待buffer有空間
    //printf("put %d\n", item);
	buffer[in]=item++;  //將資料放入
    in++; in=in%bufsize;    //規劃下筆資料應該擺放的地點
    atomic_fetch_add(&num, 1);  //讀取、加一、寫入，三個動作一次完成
    //if (num==bufsize) printf("buffer full\n");
    sem_post(&notEmpty);    //放入資料了，所以就不是empty
}

void get() {
	static int item=0;  //用於除錯，紀錄上一次讀取的數字為何
	int tmpItem;    //讀取的數字暫時放在tmpItem
    sem_wait(&notEmpty);    //等待buffer有東西
	tmpItem=buffer[out];    //讀出buffer的東西
    //printf("get %d\n", tmpItem);
    out++; out=out%bufsize; //規劃下次要提取資料的地方
	if ((tmpItem - item) != 1) fprintf(stderr, "error: tmpItem - item = %d\n", tmpItem - item); //用於除錯，如果拿到的資料不是嚴格遞增，表示程式邏輯有錯
	item=tmpItem;   //用於除錯，紀錄這次拿到的數字
    atomic_fetch_sub(&num, 1);  //讀取、減一、寫入，三個動作一次完成
    //if (num==0) printf("buffer empty\n");
    sem_post(&notFull); //拿出資料了，因此一定不是full
}

void producer(void* name) {     //生產資料的執行緒
    for (int i=0; i<100000000; i++)
        put();
}

void consumer(void* name) {     //消化資料的執行緒
    for (int i=0; i<100000000; i++)
        get();
}
int main(void) {
	pthread_t id1, id2, id3, id4;

    struct itimerval itime = {0};
	itime.it_interval.tv_usec=1000;
	itime.it_value.tv_usec=1000;
	signal(SIGPROF, printBufStatus);
    //setitimer(ITIMER_PROF, &itime, NULL);

	sem_init(&notFull, 0, bufsize);     //剛開始全部是空，因此有bufsize的空間可以放
    sem_init(&notEmpty, 0, 0);          //剛開始沒有東西放在buffer內
	pthread_create(&id1,NULL,(void *) producer,NULL);
    //pthread_create(&id2,NULL,(void *) producer,NULL);     //這個程式只支援一個producer一個consumer，拿掉這個註解會發生錯誤
	pthread_create(&id3,NULL,(void *) consumer,NULL);
    //pthread_create(&id4,NULL,(void *) consumer,NULL);     //這個程式只支援一個producer一個consumer，拿掉這個註解會發生錯誤
	pthread_join(id1,NULL);
	//pthread_join(id2,NULL);
    pthread_join(id3,NULL);
	//pthread_join(id4,NULL);
}
