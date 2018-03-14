#include <ev.h>       // a single header file is required
#include <stdio.h>    // for puts
#include <unistd.h>
#include <stdlib.h>

ev_child cw;
static void child_cb (EV_P_ ev_child *w, int revents)
{
    ev_child_stop (EV_A_ w);
    printf ("process %d exited with status %x\n", w->rpid, w->rstatus);
}

int main (void)
{
    // use the default event loop unless you have special needs
    struct ev_loop *loop = EV_DEFAULT; /* OR ev_default_loop(0) */

    pid_t pid = fork ();
    if (pid < 0) {            // error
        perror("fork()");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {    // child
        // the forked child executes here
        sleep(1);
        exit (EXIT_SUCCESS);
    } else {                  // parent
        ev_child_init (&cw, child_cb, pid, 0);
        ev_child_start (EV_DEFAULT_ &cw);
    }

    // now wait for events to arrive
    ev_run (loop, 0);

    // break was called, so exit
    return 0;
}
