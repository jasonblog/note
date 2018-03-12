/* catch a SIGINT signal, ctrl-c */
#include <ev.h>       // a single header file is required
#include <stdio.h>    // for puts

ev_signal signal_watcher;
static void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
    puts("catch SIGINT");
    ev_break (EV_A_ EVBREAK_ALL);
}

int main (void)
{
    // use the default event loop unless you have special needs
    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */

    ev_signal_init (&signal_watcher, sigint_cb, SIGINT);
    ev_signal_start (loop, &signal_watcher);

    // now wait for events to arrive
    ev_run (loop, 0);

    // break was called, so exit
    return 0;
}
