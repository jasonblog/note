#ifndef __PTHREAD_POOL_H__

#define __PTHREAD_POOL_H__

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>

#define THREAD_MAX_NUM  100     /* max number of thread. */
#define THREAD_DEF_NUM  20      /* by default ,number of thread. */
#define THREAD_MIN_NUM  5       /* min number of thread pool. */

#define PORT    9001

/*
*ds of the every task. make all task in a single link
*/
typedef struct task_node {
    void* arg;                    /* fun arg. */
    void* (*fun)(void*);          /* the real work of the task. */
    pthread_t tid;                /* which thread exec this task. */
    int work_id;                  /* task id. */
    int flag;                     /* 1: assigned, 0: unassigned. */
    struct task_node* next;
    pthread_mutex_t
    mutex;        /* when modify this ds and exec the work,lock the task ds. */
} TASK_NODE;

/*
*the ds  of the task_queue
*/
typedef struct task_queue {
    pthread_mutex_t mutex;
    pthread_cond_t
    cond;          /* when no task, the manager thread wait for ;when a new task come, signal. */
    struct task_node* head;       /* point to the task_link. */
    int number;                   /* current number of task, include unassinged and assigned but no finished. */
} TASK_QUEUE_T;

/*
*the ds of every thread, make all thread in a double link queue.
*/
typedef struct pthread_node {
    pthread_t tid;               /* the pid of this thread in kernel,the value is  syscall return . */
    int flag;                    /*  1:busy, 0:free. */
    struct task_node* work;      /*  if exec a work, which work. */
    struct pthread_node* next;
    struct pthread_node* prev;
    pthread_cond_t
    cond;        /* when assigned a task, signal this child thread by manager. */
    pthread_mutex_t mutex;
} THREAD_NODE;

/*
*the ds of the thread queue
*/
typedef struct pthread_queue {
    int number;                  /* the number of thread in this queue. */
    struct pthread_node* head;
    struct pthread_node* rear;
    pthread_cond_t
    cond;        /* when no idle thread, the manager wait for ,or when a thread return with idle, signal. */
    pthread_mutex_t mutex;
} PTHREAD_QUEUE_T;

/*
* the ds of send msg between server and client.
*/
struct info {
    char flag;                    /* 1:file attribute; 2:file content. */
    char buf[256];                /* require file name. */
    int local_begin;              /* if get attribute, no used. */
    int length;                   /* if get attribute, no used. */
};

extern PTHREAD_QUEUE_T*
pthread_queue_idle; /* the idle thread double link queue. */
extern PTHREAD_QUEUE_T*
pthread_queue_busy; /* the work thread double link queue. */
extern TASK_QUEUE_T*
task_queue_head;       /* the task queuee single link list. */

void* child_work(void* ptr);
void create_pthread_pool(void);
void init_system(void);
void* thread_manager(void* ptr);
void* prcoess_client(void* ptr);
void* task_manager(void* ptr);
void* monitor(void* ptr);
void sys_clean(void);

#endif


