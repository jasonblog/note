#include <ev.h>       // a single header file is required
#include <stdio.h>    // for puts
#include <assert.h>

int main (void)
{
    /* check libev version */
    printf("current libev version: %d.%d (or get through macro, %d.%d)\n",
            ev_version_major(), ev_version_minor(),
            EV_VERSION_MAJOR, EV_VERSION_MINOR);
    assert ((ev_version_major () == EV_VERSION_MAJOR &&
            ev_version_minor () >= EV_VERSION_MINOR));

    /* sorry, no epoll, no sex */
    assert ((ev_supported_backends () & EVBACKEND_EPOLL));

    // use the default event loop unless you have special needs
    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */

    /* time relative, ev_tstamp equal to double */
    printf("get time from ev_time(): %.15g, or ev_now(): %.15g\n",
            ev_time(), ev_now(EV_A));
    ev_sleep(2.3);    /* sleep for a while */

    // break was called, so exit
    return 0;
}
