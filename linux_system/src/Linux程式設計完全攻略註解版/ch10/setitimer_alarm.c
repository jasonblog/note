//purpose: 显示了设置闹钟的方法
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

void sigroutine(int signo)
{
    switch (signo) {
    case SIGALRM:
        printf("Catch a signal -- SIGALRM\n");
        break;

    case SIGVTALRM:
        printf("Catch a signal -- SIGVTALRM\n");
        break;
    }
}

int main(int argc, char** argv)
{
    struct itimerval value, ovalue, value2;

    printf("process id is %d\n", getpid());
    signal(SIGALRM, sigroutine);
    signal(SIGVTALRM, sigroutine);

    value.it_value.tv_sec = 1;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 1;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &ovalue);

    value2.it_value.tv_sec = 0;
    value2.it_value.tv_usec = 500000;
    value2.it_interval.tv_sec = 0;
    value2.it_interval.tv_usec = 500000;
    setitimer(ITIMER_VIRTUAL, &value2, &ovalue);

    for (;;) ;
}
