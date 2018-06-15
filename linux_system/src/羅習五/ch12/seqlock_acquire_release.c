/*
使用方法：seqlock [reader的數量]
如果不指定reader的數量，reader的數量為3
writer的數量為1

＊＊＊＊＊＊特別注意＊＊＊＊＊＊
這個演算法頂多就只能一個writer，reader的數量不限
*/


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
#include <assert.h>

__thread int thread_local_id;

volatile atomic_int g_version;	//一定要加上volatile，否則-O3會出錯，實現seqlock必要變數
volatile int nReader = 0;
volatile int nReaderComplete = 0;
volatile int wrtBreak=0;

/*
volatile atomic_int nRead_in_cs;    //for debugging
volatile atomic_int nRedo;
volatile atomic_int nNoRedo;
volatile atomic_int nWriter;
*/

void seq_wrt_lock() {
    atomic_fetch_add_explicit(&g_version, 1, memory_order_relaxed);
}

void seq_wrt_unlock() {
    atomic_fetch_add_explicit(&g_version, 1, memory_order_relaxed);
    if (g_version%2 ==1) {
        printf("ERR: version?\n");
        getchar();
    }
    //printf("unlock version = %d\n", g_version);
}

//for debugging
/*
void alarmHandler(int sigNo) {  //for debugger
    //printf("reader REDO 的次數 = %lld\n", redoCount);
    //printf("reader想進去CS但writer在CS = %lld\n", wrt_in_cs);
    printf("reader在CS的數量\t%d\n", nRead_in_cs);
    printf("reader的redo的數量\t%d/sec\n", nRedo);
    printf("reader的noredo的數量\t%d/sec\n", nNoRedo);
    printf("writer的寫入數量\t%d/sec\n", nWriter);
    atomic_store(&nRedo, 0);
    atomic_store(&nNoRedo, 0);
    atomic_store(&nWriter, 0);
    alarm(1);
}
*/

pid_t lastRdThreadID = 0;
long long totoalRetry = 0;
//可以有多個reader
void rd_thread(void* para) {

    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET((int)para, &cpu_mask);
    int ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask);
    assert(ret==0);

    for (int k=0; k< random()%1000; k++)    //隨機化開始時間
        ;

    long long retry=0;
    for (int i=0; i <1000000000; i++) {
        //下面程式碼是為了要讀取資料，如果讀取的過程當中，writer更新了資料，這時候reader就重新讀
        while (1) { //進入CS準備讀取資料
            int init_version;
            //printf("before testing writer g_version = %d\n", atomic_load(&g_version));
            if (atomic_load_explicit(&g_version, memory_order_acquire)%2==1) {    //有writer正在critical section，再試一次
                retry++;
                continue;
            }
            init_version = atomic_load_explicit(&g_version, memory_order_relaxed);
            //printf("init_version = %d, version=%d\n", init_version, atomic_load(&g_version));
            /*
            pid_t tid = syscall(__NR_gettid);   //glibc不支援gettid函數，直接呼叫system call    //for debugging
            if (lastRdThreadID != tid) {
                //printf("Reader %d in CS\n", tid);
                lastRdThreadID = tid;
            }*/

            //atomic_fetch_add(&nRead_in_cs, 1);  //for debugging，嘗試要讀
            //底下這個for loop要置換成讀取資料的程式碼
            for (int i=0; i< 100; i++)    //computing, read-only
                ;
            //atomic_fetch_sub(&nRead_in_cs, 1);    //for debugging，讀完了，等一下再看看要不要重讀

            //判斷要不要重新讀取
            if (init_version == atomic_load_explicit(&g_version,memory_order_acquire)) {  //版本編號一樣，表示讀取過程當中，writer沒有更新資料，不用重讀
                //printf("success\n");    //for debugging
                //atomic_fetch_add(&nNoRedo, 1);
                break;   //success
            }
            else {  //版本編號不一樣，讀取過程中writer更新資料，reader重新讀一次
                //printf("redo\n");    //for debugging
                //atomic_fetch_add(&nRedo, 1);
                retry++;
                continue;   //redo
            }
        }
        //remainder seciton
        for (int i=0; i< 100; i++)
            ;
    }
    printf("rd complete\n");
    totoalRetry+=retry;
    nReaderComplete++;
    if(nReaderComplete == nReader) wrtBreak = 1;
}



//注意，只可以有一個writer
void wrt_thread(void* para) {
    cpu_set_t cpu_mask;
    CPU_ZERO(&cpu_mask);
    CPU_SET(0, &cpu_mask);
    int ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpu_mask);
    assert(ret==0);

    while(wrtBreak == 0)  {
        //atomic_fetch_add(&nWriter, 1);
        seq_wrt_lock();
        //printf("wrtier enter CS\n");
        for (int i=0; i< 100; i++)    //computing, read-write
            ;
        seq_wrt_unlock();
        
        //remainder seciton
        for (int i=0; i< 100; i++)
            ;
        //printf("writer exit CS\n");
    }
    printf("wrt complete\n");
}

int main(int argc, char** argv) {
	pthread_t* r_id;
    pthread_t w_id;
    int nThread = 3;
    if (argc == 2)
        sscanf(argv[1], "%d", &nThread);
    nReader=nThread;
    printf("建立 %d 個 reader threads\n", nThread);
    printf("建立 1 個 writer thread\n");

    r_id = (pthread_t*)malloc(sizeof(pthread_t) * nThread);

    for (int i=0; i<nThread; i++) {
	    int ret = pthread_create(&r_id[i], NULL, (void *)rd_thread, (void*)((i*2+2)%8));
        assert(ret==0);
    }
    int ret = pthread_create(&w_id, NULL, (void *)wrt_thread, NULL);
    assert(ret==0);

    //signal(SIGALRM, alarmHandler);
    //alarm(1);

    for (int i=0; i<nThread; i++) {
	    pthread_join(r_id[i],NULL);
    }
    pthread_join(w_id,NULL);
    printf("retry = %lld\n", totoalRetry);
}