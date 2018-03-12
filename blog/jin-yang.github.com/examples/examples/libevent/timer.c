#include <event2/event.h>
#include <stdio.h>

static void timeout_cb(int fd, short event, void *arg)
{
    struct timeval tv;
    struct event *timeout = arg;
    int newtime = time(NULL);

    printf("%s: called at %d: %d\n", "timeout_cb", newtime,
        newtime - lasttime);
lasttime = newtime;

evutil_timerclear(&tv);
tv.tv_sec = 2;
//重新注册event
event_add(timeout, &tv);
}

int main (int argc, char **argv)
{
    struct event timeout;
    struct timeval tv;

    /* Initalize the event library */
    event_init();

    /* Initalize one event */
    evtimer_set(&timeout, timeout_cb, &timeout);

    evutil_timerclear(&tv);
    tv.tv_sec = 2;

    event_add(&timeout, &tv);

    lasttime = time(NULL);

    event_dispatch();

    return (0);
}
