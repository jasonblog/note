# example

- 強制多執行緒共用一個cpu thread_cpu.c
-  thread.c
-  merge sort.c
-  cond.c pthread_cond_wait
---

- thread_cpu.c

正在構思一個程式，load其實不是很重，但是有3-4個thread必須輪流對單一device存取但是想做到lock-free+節省load+空出其他core做其他的事，
所以希望把該程式生出的所有的thread都集中在單一個core跑。
raspberry pi的case簡單(本來就是single core)，可是想在rp3上也這樣做。

```c
// gcc thread.c -Wall -pedantic -lpthread -o thread

#include <stdlib.h>
#include <stdatomic.h>
#define __USE_GNU 
#include <sched.h>
#include <pthread.h>

typedef struct {
    char name[16];
    int sleepTime;
} Node;

void _usleep(int);
void* generateSingleThread(void*);
void  printAndSleep(Node*);

atomic_int acnt;
int main(void) {
    cpu_set_t cmask;
    unsigned long len = sizeof(cmask);
    CPU_ZERO(&cmask);   /* 初始化 cmask */
    CPU_SET(0, &cmask); /* 指定第一個處理器 */
    /* 設定自己由指定的處理器執行 */
        
    if (sched_setaffinity(0, len, &cmask)) {
        printf("Could not set cpu affinity for current process.\n");
        exit(1);
    }   

    Node nodes[] = {{"One", 100000}, {"Two", 200000}, {"Three", 300000}};
    pthread_t thread[3];
    for (int i=0; i<3; ++i) {
        pthread_create(&thread[i], NULL, generateSingleThread, (void*) &nodes[i]);
    }   
    for (int i=0; i<3; ++i) pthread_join(thread[i],NULL);
    return 0;
}

void* generateSingleThread(void* nodes) {
    Node* node = (Node*)nodes;
    for (int i=0; i<1000; ++i) {
        while (acnt) sched_yield() ;
        atomic_fetch_add(&acnt, 1);
        printf("%dth Time: ", i);
        atomic_fetch_sub(&acnt, 1);
        printAndSleep(node);
    }
    pthread_exit(NULL);
}

void  printAndSleep(Node* node) {
    printf("%s\n", node->name);
    _usleep(node->sleepTime);
}
void _usleep(int micro)
{
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = micro * 1000L;
    nanosleep(&req, (struct timespec *)NULL);
}
```
- thread.c

```c
// gcc thread.c -Wall -pedantic -O3 -std=gnu11 -o thread -lpthread
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

typedef struct {
    char name[16];
    int sleepTime;
} Node;

void _usleep(int);
void* generateSingleThread(void*);
void  printAndSleep(Node*);

atomic_int acnt;
int main(void) {
    Node nodes[] = {{"One", 1000}, {"Two", 2000}, {"Three", 3000}};
    pthread_t thread[3];
    for (int i=0; i<3; ++i) {
        pthread_create(&thread[i], NULL, generateSingleThread, (void*) &nodes[i]);
    }   
    
    for (int i=0; i<3; ++i) pthread_join(thread[i],NULL);
    return 0;
}

void* generateSingleThread(void* nodes) {
    Node* node = (Node*)nodes;
    for (int i=0; i<10; ++i) {
        while (acnt) sched_yield() ;
        ++acnt;
        printf("%dth Time: ", i); 
        --acnt;
        printAndSleep(node);
    }   
    pthread_exit(NULL);
}

void  printAndSleep(Node* node) {
    printf("%s\n", node->name);
    _usleep(node->sleepTime);
}

void _usleep(int micro)
{
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = micro * 1000L;
    nanosleep(&req, (struct timespec *)NULL);
}
```

- merge_sort.c

```c
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define NDATA   100000

void swap(uint32_t* );
uint32_t** sepaTwo(uint32_t*, uint32_t);
uint32_t* mergeTwo(uint32_t*, uint32_t, uint32_t*, uint32_t);
uint32_t* mergeSort(uint32_t*, uint32_t);

int main(void) {
    FILE *f = fopen("IntegerArray.txt","r");
    uint32_t *data = (uint32_t*) malloc(sizeof(uint32_t)*NDATA);
    uint32_t iArray = 0;
    while (!feof(f) && iArray < NDATA) {
        fscanf (f, "%d\n", &data[iArray++]);
    }

    for (int i=0; i<iArray; ++i) printf("%d\n", data[i]);
    uint32_t *dataSort = mergeSort(data, iArray);

    for (int i=0; i<iArray; ++i) printf("%d\n", dataSort[i]);
    free(data);
    free(dataSort);
    fclose(f);
    return 0;
}

void swap(uint32_t* data) {
    uint32_t b = data[0];
    data[0] = data[1];
    data[1] = b;
}

uint32_t** sepaTwo(uint32_t* data, uint32_t size_data){
    uint32_t** sepData = (uint32_t**) malloc(sizeof(uint32_t*)*2);
    sepData[0] = &data[0];
    sepData[1] = &data[size_data/2];

    return sepData;
}

uint32_t* mergeTwo(uint32_t* A, uint32_t nA, uint32_t* B, uint32_t nB){
    uint32_t iA = 0, iB = 0, iC = nA+nB;
    uint32_t* data_merged = (uint32_t*) malloc(sizeof(uint32_t)*iC);
    for (int i=0; i<iC; ++i) {
        if ( (iB==nB) || (iA != nA && A[iA]<B[iB]) ) {
            data_merged[i] = A[iA++];
        } else{
            data_merged[i] = B[iB++];
        }
    }
    return data_merged;
}


uint32_t* mergeSort(uint32_t* data, uint32_t size_data) {
    uint32_t* data_sorted;

    if (size_data <=2) {
        data_sorted = (uint32_t*) malloc(sizeof(uint32_t)*size_data);
        for (int i=0; i<size_data; ++i) {
            data_sorted[i] = data[i];
        }

        if (size_data == 1) {
            return data_sorted;
        }

        if (size_data == 2) {
            if (data[0] > data[1]) swap(data_sorted);
        }
        return data_sorted;
    }

    uint32_t** sepData = sepaTwo(data, size_data);
    uint32_t* data1 = mergeSort(sepData[0], size_data/2);
    uint32_t* data2 = mergeSort(sepData[1], (size_data+1)/2);
    data_sorted = mergeTwo(data1, size_data/2, data2, (size_data+1)/2);

    free(data1);
    free(data2);
    free(sepData);

    return data_sorted;
}
```

- cond.c

```c
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>

#define NLOOP   30
int global_thread = 0;
pthread_mutex_t mutex;
pthread_cond_t cond[5];

void _usleep(int micro)
{
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = micro * 1000L;
    nanosleep(&req, (struct timespec *)NULL);
}

void* Thread(void* x) {
    int i;
    int *X = (int*) x;
    for (i=0; i<5; ++i) {
        pthread_mutex_lock(&mutex);
        printf("This thread should sleep %d000 microsecond\n", *X);
        while (global_thread != *X) {
            printf("Thread %d wait\n", *X);
            pthread_cond_wait(&cond[*X], &mutex);
        }
        printf("Thread %d Enter!\n", *X);
        global_thread = 0;
        pthread_mutex_unlock(&mutex);
        _usleep(*X * 1000);
    }
    printf("Thread %d Exit! \n", *X);
    pthread_exit(NULL);
}

int main(void) {

    struct sched_param sp;
    memset(&sp, 0, sizeof(sp));
    sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
    //sp.sched_priority = 49;
    sched_setscheduler(0, SCHED_FIFO, &sp);

    mlockall(MCL_CURRENT | MCL_FUTURE);

    int i;

    int a=1, b=2, c=3, d=4;
    float dt[NLOOP];
    struct timespec tp1, tp2;
    unsigned long startTime, procesTime;
    pthread_t pid_1, pid_2, pid_3, pid_4;

    for (i=0; i<5; ++i) {
        pthread_cond_init(&cond[i],NULL);
//      pthread_mutex_init(&mutex[i],NULL);
    }
    pthread_mutex_init(&mutex,NULL);

    pthread_create(&pid_1,NULL,Thread, (void*)&a);
    pthread_create(&pid_2,NULL,Thread, (void*)&b);
    pthread_create(&pid_3,NULL,Thread, (void*)&c);
    pthread_create(&pid_4,NULL,Thread, (void*)&d);

    sleep(5);
    puts("============================================");
    clock_gettime(CLOCK_REALTIME, &tp1);
    startTime = tp1.tv_sec*1000000000 + tp1.tv_nsec;
    for (i=0; i<NLOOP; ++i) {
        puts("");
        printf("LOOP %d\n", i);
        global_thread = i%5;
        if (global_thread <5) pthread_cond_signal(&cond[global_thread]);
        _usleep(5000);
        clock_gettime(CLOCK_REALTIME, &tp2);
        startTime = tp1.tv_sec*1000000000 + tp1.tv_nsec;
        procesTime = tp2.tv_sec*1000000000 + tp2.tv_nsec - startTime;
        dt[i] = (float)procesTime /1000.0;
        tp1 = tp2;
    }

    pthread_join(pid_1, NULL);
    pthread_join(pid_2, NULL);
    pthread_join(pid_3, NULL);
    pthread_join(pid_4, NULL);

    for (i=0; i<NLOOP; ++i) {
        printf("%d, %f\n", i, dt[i]);
    }

    return 0;
}
```