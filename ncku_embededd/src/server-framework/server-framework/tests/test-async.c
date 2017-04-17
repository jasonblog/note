#include "async.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static struct timespec start, now;

/** @return in ms */
static double time_diff(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec - t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000.0 + diff.tv_nsec / 1000000.0);
}

static void greeting(void *arg)
{
    static int i = 0;
    fprintf(stderr, "Hi! %d\n", i++);
}

static void schedule_tasks2(void *arg)
{
    clock_gettime(CLOCK_REALTIME, &now);
    fprintf(stderr, "# schedule task at (%lf) ms\n", time_diff(start, now));
    async_p async = arg;
    for (size_t i = 0; i < (8 * 1024); i++) {
        Async.run(async, greeting, NULL);
        printf("wrote task %lu\n", i);
    }
    Async.run(async, greeting, NULL);
    Async.signal(async);
    clock_gettime(CLOCK_REALTIME, &now);
    printf("# signal finish at (%lf) ms\n", time_diff(start, now));
}

static void schedule_tasks(void *arg)
{
    clock_gettime(CLOCK_REALTIME, &now);
    fprintf(stderr, "# schedule task at (%lf) ms\n", time_diff(start, now));
    async_p async = arg;
    for (size_t i = 0; i < (8 * 1024); i++)
        Async.run(async, greeting, NULL);
    Async.run(async,
              schedule_tasks2, async /* as the argument to tasks2 */);
}

int main(void)
{
    fprintf(stderr, "# Test async\n");

    /* create the thread pool with a single threads.
     * the callback is optional (we can pass NULL)
     */
    async_p async = Async.create(32);
    if (!async) {
        perror("Async creation failed");
        exit(1);
    }
    /* send a task */
    clock_gettime(CLOCK_REALTIME, &start);
    Async.run(async, schedule_tasks, async);

    /* wait for all tasks to finish, closing threads, clearing memory */
    Async.wait(async);
    clock_gettime(CLOCK_REALTIME, &now);
    fprintf(stderr, "# elapsed time: (%lf) ms\n", time_diff(start, now));
    return 0;
}
