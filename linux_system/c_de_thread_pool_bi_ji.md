# C 的 Thread Pool 筆記


最近被丟到 FreeBSD 跟 C 的世界裡面，沒有 Scala 的 Actor 可以用。 所以只好參考別人的 Thread Pool 來看一下，在 C 的世界裡面 Thread Pool 是怎麼實作的。

首先先去 StackOverFlow 中尋找是否有人問過類似的問題

Existing threadpool C implementation

裡面有提到幾個 C Thread Pool 的實作範例與可參考的文件

- [threadpool-mbrossard](https://github.com/mbrossard/threadpool)
- [threadpool-jmatthew](http://people.clarkson.edu/~jmatthew/cs644.archive/cs644.fa2001/proj/locksmith/code/ExampleTest/)
- [cthreadpool](http://sourceforge.net/projects/cthpool/)
- [C-Thread-Pool](https://github.com/Pithikos/C-Thread-Pool)

我這邊是直接應該是會以 threadpool-mbrossard 作為第一個研究的版本，因為他一直有在維護。 而且作者就是 [Existing threadpool C implementation](http://stackoverflow.com/questions/6297428/existing-threadpool-c-implementation) 的發文者，感覺他還蠻熱心的。


##threadpool
`A simple C thread pool implementation`

Currently, the implementation:

- Works with pthreads only, but API is intentionally opaque to allow other implementations (Windows for instance).
- Starts all threads on creation of the thread pool.
- Reserves one task for signaling the queue is full.
- Stops and joins all worker threads on destroy.



![](./images/580px-Thread_pool.svg.png)

from wikipedia

##Thread Pool 的資料結構
首先 Thread Pool 要有的東西就是 job 或者是 task 讓 Thread 知道他們要做什麼事情。


```c
typedef struct {
    void (*function)(void *);
    void *argument;
} threadpool_task_t;
```

所以只要有一個資料結構紀錄要執行的 function pointer 與要傳遞的參數即可。 接下來就是 Thread Pool 本身，他必須存放所有的 Thread 與 Job Queue


```c
struct threadpool_t {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    threadpool_task_t *queue;
    int thread_count;
    int queue_size;
    int head;
    int tail;
    int count;
    int shutdown;
    int started;
};
```

這邊他使用了一個 pthread_t 的 pointer 來紀錄所有的 Thread，簡單來說就是一個 pthread_t 的 array，而 head, tail 就是紀錄 array 的 offset。 threadpool_task_t 也是一樣的原理，真是出乎意料的簡單。

##ThreadPool 的建立與工作的執行
再來就是 Thread Pool 的建立，由於剛剛提到的他其實是使用一個 pthread array 與一個 job array 來存放所有的 thread 與 jobs。 因此需要在一開始的時候就決定 Thread Pool 與 Jobs 的最大數量。

```c
/* Allocate thread and task queue */
pool->threads = (pthread_t *) malloc(sizeof(pthread_t) * thread_count);
pool->queue = (threadpool_task_t *) malloc(sizeof(threadpool_task_t) * queue_size);
```

而每個 Thread 要執行的 Function 是

```c
static void *threadpool_thread(void *threadpool)
 {
     threadpool_t *pool = (threadpool_t *)threadpool;
     threadpool_task_t task;

     for(;;) {
         /* Lock must be taken to wait on conditional variable */
         pthread_mutex_lock(&(pool->lock));

         /* Wait on condition variable, check for spurious wakeups.
            When returning from pthread_cond_wait(), we own the lock. */
         while((pool->count == 0) && (!pool->shutdown)) {
             pthread_cond_wait(&(pool->notify), &(pool->lock));
         }

         if((pool->shutdown == immediate_shutdown) ||
            ((pool->shutdown == graceful_shutdown) &&
             (pool->count == 0))) {
             break;
         }

         /* Grab our task */
         task.function = pool->queue[pool->head].function;
         task.argument = pool->queue[pool->head].argument;
         pool->head += 1;
         pool->head = (pool->head == pool->queue_size) ? 0 : pool->head;
         pool->count -= 1;

         /* Unlock */
         pthread_mutex_unlock(&(pool->lock));

         /* Get to work */
         (*(task.function))(task.argument);
     }

     pool->started--;

     pthread_mutex_unlock(&(pool->lock));
     pthread_exit(NULL);
     return(NULL);
 }
```

在 for(;;) 裡面，Thread 第一件要做的事情就是去搶奪 pool 的 lock，當搶到 lock 的 Thread 發現沒有工作可以做的時候， 就會執行 pthread_cond_wait 來等待通知。這時候 pool->lock 會被 Unlock，因此這時候其他 Thread 也可以進來這個區域。 所以在完全沒有工作的情況下，所有的 Thread 都會在這邊 Waiting。

當 Thread 被透過 pthread_cond_signal 喚醒的時候，該 Thread 就會重新取得 pool->lock。 這時他就可以安心的取出 queue 中的 task，等待取出完畢之後，再 unlock 讓其他被喚醒的 Thread 也可以去取得 Task。

之後就是執行 task 中的 function pointer 做該做的工作。


##ThreadPool 的 destory
destory 就更簡單了，只要使用 pthread_cond_broadcast 通知所有的 Thread 起來，由於 shoutdown 的確認會在執行工作之前。 所以該 thread 就會離開執行工作的迴圈，並且結束。

[mbrossard 完整的 ThreadPool 原始碼](https://github.com/mbrossard/threadpool/blob/master/src/threadpool.c)

其實寫這篇筆記應該只是想貼這個
