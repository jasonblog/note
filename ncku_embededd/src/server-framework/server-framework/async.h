#ifndef _ASYNC_H
#define _ASYNC_H

/** Async Pointer */
typedef struct Async* async_p;

/** \file
 * @example test-async.c
 */

/**
 * \brief A simple thread pool utilizing POSIX threads
 *
 * The thread pool can take any function and split it across a set number
 * of threads. It uses a combination of a pipe (for wakeup signals) and
 * mutexes (for managing the task queue) to give a basic layer of protection
 * to any server implementation.
 *
 * The Async global object allows us access to the Async thread pool API.
 *
 * Use:
 * @code
 *   async_p async = Async.create(4); // 4 worker threads
 *   Async.finish(async); // signal and wait, then the object self-destructs
 * @endcode
 *
 * @note The implementation is not fork-friendly. In general, mixing `fork`
 *       with multi-threading is not safe nor trivial - always fork before
 *       multi-threading.
 */
extern struct __ASYNC_API__ {
    /**
     * \brief Create a new Async object (thread pool)
     *        a pointer using the Async pointer type.
     * @param threads the number of new threads to be initialized
     *
     * Use:
     * @code
     *   async_p async = Async.create(8);
     * @endcode
     */
    async_p(*create)(int threads);

    /**
     * \brief Signal an Async object to finish up.
     *
     * The threads in the thread pool will continue perfoming all the tasks
     * in the queue until the queue is empty. Once the queue is empty, the
     * threads will exit. If new tasks are created after the signal, they
     * will be added to the queue and processed until the last thread is
     * done. Once the last thread exists, future tasks will not be processed.
     *
     * Use:
     * @code
     *   Async.signal(async);
     * @endcode
     */
    void (*signal)(async_p);

    /**
     * \brief Waits for an Async object to finish up (joins all the threads
     *        in the thread pool).
     *
     * This function will wait forever or until a signal is received and
     * all the tasks in the queue have been processed.
     *
     * Use:
     * @code
     *   Async.wait(async);
     * @endcode
     */
    void (*wait)(async_p);

    /**
     * \brief Schedules a task to be performed by an Async thread pool group.
     *
     * The Task should be a function such as `void task(void *arg)`.
     *
     * Use:
     * @code
     *   void task(void *arg) { printf("%s", arg); }
     *   char arg[] = "Demo Task";
     *   Async.run(async, task, arg);
     * @endcode
     */
    int (*run)(async_p async, void (*task)(void*), void* arg);

    /**
     * \brief Both signals for an Async object to finish up and waits
     *        for it to finish.
     *
     * This is akin to calling both `signal` and `wait` in succession:
     *   - Async.signal(async);
     *   - Async.wait(async);
     *
     * Use:
     * @code
     *   Async.finish(async);
     * @endcode
     *
     * @return  0 on success.
     * @return -1 on error.
     */
    void (*finish)(async_p);
} Async;

#endif
