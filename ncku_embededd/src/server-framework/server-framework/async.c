#include "async.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

/* suppress compilation warnings */
static inline ssize_t write_wrapper(int fd, const void* buf, size_t count)
{
    ssize_t s;

    if ((s = write(fd, buf, count)) < count) {
        perror("write");
    }

    return s;
}
#undef write
#define write write_wrapper

/* the actual working thread */
static void* worker_thread_cycle(void* async);

/* signaling to finish */
static void async_signal(async_p async);

/* the destructor */
static void async_destroy(async_p queue);

static void* join_thread(pthread_t thr)
{
    void* ret;
    pthread_join(thr, &ret);
    return ret;
}

static int create_thread(pthread_t* thr,
                         void* (*thread_func)(void*),
                         void* async)
{
    return pthread_create(thr, NULL, thread_func, async);
}

/** A task node */
struct AsyncTask {
    struct AsyncTask* next;
    void (*task)(void*);
    void* arg;
};

/** The Async struct */
struct Async {
    /** the task queue - MUST be first in the struct */
    pthread_mutex_t lock;              /**< a mutex for data integrity */
    struct AsyncTask* volatile tasks;   /**< active tasks */
    struct AsyncTask* volatile pool;    /**< a task node pool */
    struct AsyncTask** volatile pos;    /**< the position for new tasks */

    /** The pipe used for thread wakeup */
    struct {
        int in;  /**< read incoming data (opaque data), used for wakeup */
        int out; /**< write opaque data (single byte),
                      used for wakeup signaling */
    } pipe;

    int count; /**< the number of initialized threads */

    unsigned run : 1; /**< the running flag */

    /** the thread pool */
    pthread_t threads[];
};

/* Task Management - add a task and perform all tasks in queue */

static int async_run(async_p async, void (*task)(void*), void* arg)
{
    struct AsyncTask* c;  /* the container, storing the task */

    if (!async || !task) {
        return -1;
    }

    pthread_mutex_lock(&(async->lock));

    /* get a container from the pool of grab a new container */
    if (async->pool) {
        c = async->pool;
        async->pool = async->pool->next;
    } else {
        c = malloc(sizeof(*c));

        if (!c) {
            pthread_mutex_unlock(&async->lock);
            return -1;
        }
    }

    c->next = NULL;
    c->task = task;
    c->arg = arg;

    if (async->tasks) {
        *(async->pos) = c;
    } else {
        async->tasks = c;
    }

    async->pos = &(c->next);
    pthread_mutex_unlock(&async->lock);
    /* wake up any sleeping threads
     * any activated threads will ask to require the mutex
     * as soon as we write.
     * we need to unlock before we write, or we will have excess
     * context switches.
     */
    write(async->pipe.out, c->task, 1);
    return 0;
}

/** Performs all the existing tasks in the queue. */
static void perform_tasks(async_p async)
{
    struct AsyncTask* c = NULL;  /* c == container, will store the task */

    do {
        /* grab a task from the queue. */
        pthread_mutex_lock(&(async->lock));

        /* move the old task container to the pool. */
        if (c) {
            c->next = async->pool;
            async->pool = c;
        }

        c = async->tasks;

        if (c) {
            /* move the queue forward. */
            async->tasks = async->tasks->next;
        }

        pthread_mutex_unlock(&(async->lock));

        /* perform the task */
        if (c) {
            c->task(c->arg);
        }
    } while (c);
}

/* The worker threads */

/* The worker cycle */
static void* worker_thread_cycle(void* _async)
{
    /* setup signal and thread's local-storage async variable. */
    struct Async* async = _async;
    char sig_buf;

    /* pause for signal for as long as we're active. */
    while (async->run && (read(async->pipe.in, &sig_buf, 1) >= 0)) {
        perform_tasks(async);
        sched_yield();
    }

    perform_tasks(async);
    return 0;
}

/* Signal and finish */

static void async_signal(async_p async)
{
    async->run = 0;
    /* send `async->count` number of wakeup signales.
     * data content is irrelevant. */
    write(async->pipe.out, async, async->count);
}

static void async_wait(async_p async)
{
    if (!async) {
        return;
    }

    /* wake threads (just in case) by sending `async->count`
     * number of wakeups
     */
    if (async->pipe.out) {
        write(async->pipe.out, async, async->count);
    }

    /* join threads */
    for (int i = 0; i < async->count; i++) {
        join_thread(async->threads[i]);
    }

    /* perform any pending tasks */
    perform_tasks(async);
    /* release queue memory and resources */
    async_destroy(async);
}

static void async_finish(async_p async)
{
    async_signal(async);
    async_wait(async);
}

/* Object creation and destruction */

/** Destroys the Async object, releasing its memory. */
static void async_destroy(async_p async)
{
    pthread_mutex_lock(&async->lock);
    struct AsyncTask* to_free;
    async->pos = NULL;
    /* free all tasks */
    struct AsyncTask* pos = async->tasks;

    while ((to_free = pos)) {
        pos = pos->next;
        free(to_free);
    }

    async->tasks = NULL;
    /* free task pool */
    pos = async->pool;

    while ((to_free = pos)) {
        pos = pos->next;
        free(to_free);
    }

    async->pool = NULL;

    /* close pipe */
    if (async->pipe.in) {
        close(async->pipe.in);
        async->pipe.in = 0;
    }

    if (async->pipe.out) {
        close(async->pipe.out);
        async->pipe.out = 0;
    }

    pthread_mutex_unlock(&async->lock);
    pthread_mutex_destroy(&async->lock);
    free(async);
}

static async_p async_create(int threads)
{
    async_p async = malloc(sizeof(*async) + (threads * sizeof(pthread_t)));
    async->tasks = NULL;
    async->pool = NULL;
    async->pipe.in = 0;
    async->pipe.out = 0;

    if (pthread_mutex_init(&(async->lock), NULL)) {
        free(async);
        return NULL;
    };

    if (pipe((int*) & (async->pipe))) {
        free(async);
        return NULL;
    };

    fcntl(async->pipe.out, F_SETFL, O_NONBLOCK | O_WRONLY);

    async->run = 1;

    /* create threads */
    for (async->count = 0; async->count < threads; async->count++) {
        if (create_thread(async->threads + async->count,
                          worker_thread_cycle, async)) {
            /* signal */
            async_signal(async);
            /* wait for threads and destroy object */
            async_wait(async);
            /* return error */
            return NULL;
        };
    }

    return async;
}

/* API gateway */
struct __ASYNC_API__ Async = {
    .create = async_create,
    .signal = async_signal,
    .wait = async_wait,
    .finish = async_finish,
    .run = async_run,
};
