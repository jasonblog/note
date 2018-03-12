/* catch a SIGINT signal, ctrl-c */
#include <ev.h>       // a single header file is required
#include <stdio.h>    // for puts

ev_stat makefile;
static void filestat_cb (EV_P_ ev_stat *w, int revents)
{
    /* "Makefile" changed in some way */
    if (w->attr.st_nlink) {
        printf ("Makefile current size  %ld\n", (long)w->attr.st_size);
        printf ("Makefile current atime %ld\n", (long)w->attr.st_mtime);
        printf ("Makefile current mtime %ld\n", (long)w->attr.st_mtime);
    } else {
        /* you shalt not abuse printf for puts */
        puts ("wow, Makefile is not there, expect problems. "
              "if this is windows, they already arrived\n");
    }
}

int main (void)
{
    // use the default event loop unless you have special needs
    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */

    ev_stat_init (&makefile, filestat_cb, "Makefile", 0.);
    ev_stat_start (loop, &makefile);

    // now wait for events to arrive
    ev_run (loop, 0);

    // break was called, so exit
    return 0;
}
