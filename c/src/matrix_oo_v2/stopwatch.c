#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "stopwatch.h"

struct StopwatchInternal {
    bool running;
    struct timespec last_time;
    struct timespec total;
};

static struct timespec clock_time()
{
    struct timespec time_now;
    clock_gettime(CLOCK_REALTIME, &time_now);
    return time_now;
}

static struct timespec time_diff(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec - t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return diff;
}

static struct timespec time_add(struct timespec t1, struct timespec t2)
{
    long sec = t2.tv_sec + t1.tv_sec;
    long nsec = t2.tv_nsec + t1.tv_nsec;
    if (nsec >= 1000000000) {
        nsec -= 1000000000;
        sec++;
    }
    return (struct timespec) {
        .tv_sec = sec, .tv_nsec = nsec
    };
}

void reset(watch_p Q)
{
    Q->running = false;
    Q->last_time = (struct timespec) {
        0, 0
    };
    Q->total = (struct timespec) {
        0, 0
    };
}

watch_p create(void)
{
    watch_p S = malloc(sizeof(struct StopwatchInternal));
    if (!S)
        return NULL;

    reset(S);
    return S;
}

void destroy(watch_p S)
{
    free(S);
}

void start(watch_p Q)
{
    if (!(Q->running)) {
        Q->running = true;
        Q->total = (struct timespec) {
            0, 0
        };
        Q->last_time = clock_time();
    }
}

void stop(watch_p Q)
{
    if (Q->running) {
        Q->total = time_add(Q->total, time_diff((Q->last_time), clock_time()));
        Q->running = false;
    }
}

double read(watch_p Q)
{
    if (Q->running) {
        struct timespec t = clock_time();
        Q->total = time_add(Q->total, time_diff(Q->last_time, t));
        Q->last_time = t;
    }
    return (Q->total.tv_sec * 1000000.0 + Q->total.tv_nsec / 1000.0) / 1000000.0;
}

/* API gateway */
struct __STOPWATCH_API__ Stopwatch = {
    .create = create,
    .destroy = destroy,
    .start = start,
    .stop = stop,
    .reset = reset,
    .read = read,
};
