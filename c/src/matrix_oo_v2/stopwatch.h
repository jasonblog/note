#ifndef STOPWATCH_H_
#define STOPWATCH_H_

typedef struct StopwatchInternal *watch_p;

extern struct __STOPWATCH_API__ {
    watch_p (*create)();
    void (*destroy)(watch_p);

    void (*start)(watch_p);
    void (*stop)(watch_p);
    void (*reset)(watch_p);

    double (*read)(watch_p);
} Stopwatch;

#endif
