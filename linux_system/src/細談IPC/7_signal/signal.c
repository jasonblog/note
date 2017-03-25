#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void sig_func(int signo)
{
    //sleep(6);
    printf("====%s== [child] handle signo: %d===\n", __func__, signo);
}

void child_process_do(void)
{
    printf("====%s==child pid: %d===\n", __func__, getpid());

    //signal(SIGRTMIN, sig_func);
    signal(SIGALRM, sig_func);

    /*
    while (1) {
        sleep(10);
    }
    */
    while (sleep(10) > 0) {
    }
}

void parent_process_do(pid_t pid)
{
    int     i;

    sleep(1);
    printf("====%s==parent pid: %d===\n", __func__, getpid());

    for (i = 0; i < 5; i++) {
        printf("====%s==[parent] send signal <%d> to pid <%d>==\n", __func__, SIGRTMIN,
               pid);
        //kill(pid, SIGRTMIN);
        kill(pid, SIGALRM);
        sleep(1);
    }

    waitpid(pid, NULL, 0);
}


int main(int argc, const char* argv[])
{
    pid_t       pid;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork: %s\n", strerror(errno));
        return -1;
    }

    if (0 == pid) {
        child_process_do();
    } else {
        parent_process_do(pid);
    }

    return 0;
}
