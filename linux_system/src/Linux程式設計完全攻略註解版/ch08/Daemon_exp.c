#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/syslog.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

int init_daemon(const char* pname, int facility)
{
    int pid;
    int i;
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGHUP , SIG_IGN);

    if (pid = fork()) {
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    setsid();

    if (pid = fork()) {
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < NOFILE; ++i) {
        close(i);
    }

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    chdir("/tmp");
    umask(0);
    signal(SIGCHLD, SIG_IGN);
    openlog(pname, LOG_PID, facility);

    return;
}

int main(int argc, char* argv[])
{
    time_t ticks;
    init_daemon(argv[0], LOG_KERN);

    while (1) {
        sleep(1);
        ticks = time(NULL);
        syslog(LOG_INFO, "%s", asctime(localtime(&ticks)));
    }

}
