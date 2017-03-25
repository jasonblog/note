#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void sig_func(int signo, siginfo_t* info, void* arg)
{
    //    sleep(6);
    printf("====%s== [child] handle signo: %d==arg: %d=\n", __func__, signo,
           info->si_int);
}

void child_process_do(void)
{
    struct sigaction    act;

    printf("====%s==child pid: %d===\n", __func__, getpid());

    //signal(SIGRTMIN, sig_func);
    //signal(SIGALRM, sig_func);

    act.sa_sigaction = sig_func;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGALRM, &act, NULL) < 0) {
        fprintf(stderr, "sigaction: %s\n", strerror(errno));
        return;
    }

    while (1) {
        sleep(10);
    }
}

void parent_process_do(pid_t pid)
{
    int             i, val = 100;
    union sigval    sigarg;

    sleep(1);
    printf("====%s==parent pid: %d===\n", __func__, getpid());

    for (i = 0; i < 5; i++) {
        printf("====%s==[parent] send signal <%d> to pid <%d>==\n", __func__, SIGRTMIN,
               pid);
        //kill(pid, SIGRTMIN);
        //kill(pid, SIGALRM);
        sigarg.sival_int = val + i;
        sigqueue(pid, SIGALRM, sigarg);

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
