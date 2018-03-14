#include <ev.h>       // a single header file is required
#include <time.h>     // for time()
#include <stdio.h>    // for printf()
#include <stdint.h>   // for uintmax_t

// every watcher type has its own typedef'd struct with the name ev_TYPE
ev_timer timeout_watcher;
ev_timer repeate_watcher;
ev_timer oneshot_watcher;

// another callback, this time for a time-out
static void timeout_cb (EV_P_ ev_timer *w, int revents)
{
    printf("timeout at %ju\n", (uintmax_t)time(NULL));
    // this causes the innermost ev_run to stop iterating
    ev_break (EV_A_ EVBREAK_ONE);
}
static void repeate_cb (EV_P_ ev_timer *w, int revents)
{
    printf("repeate at %ju\n", (uintmax_t)time(NULL));
}
static void oneshot_cb (EV_P_ ev_timer *w, int revents)
{
    printf("oneshot at %ju\n", (uintmax_t)time(NULL));
    ev_timer_stop(EV_A_ w);
}

int main (void)
{
    time_t result;
    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */

    result = time(NULL);
    printf("  start at %ju\n", (uintmax_t)result);

    // 2秒后执行函数
    //ev_timer_init (&oneshot_watcher, oneshot_cb, 2.0, 0.);
    ev_timer_init (&oneshot_watcher, oneshot_cb, 2.0, 1.);
    ev_timer_start (loop, &oneshot_watcher);

    // 5秒后开始循环，每次间隔1秒，如果最后一个参数为0，则只执行一次
    ev_timer_init (&repeate_watcher, repeate_cb, 5., 1.);
    ev_timer_start (loop, &repeate_watcher);

    // 10秒后执行超时，设置为-1表示不退出
    ev_timer_init (&timeout_watcher, timeout_cb, 10., 0.);
    ev_timer_start (loop, &timeout_watcher);

    // now wait for events to arrive
    ev_run (loop, 0);

    // break was called, so exit
    return 0;
}
