#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>

int main(void)
{
    struct itimerval setvalue;
    setvalue.it_interval.tv_sec = 3;
    setvalue.it_interval.tv_usec = 0;
    setvalue.it_value.tv_sec = 3;
    setvalue.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &setvalue, NULL);

    setvalue.it_interval.tv_sec = 3;
    setvalue.it_interval.tv_usec = 0;
    setvalue.it_value.tv_sec = 3;
    setvalue.it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &setvalue, NULL);

    setvalue.it_interval.tv_sec = 3;
    setvalue.it_interval.tv_usec = 0;
    setvalue.it_value.tv_sec = 1;
    setvalue.it_value.tv_usec = 0;
    setitimer(ITIMER_PROF, &setvalue, NULL);

    while (1) {
        struct itimerval value;
        getitimer(ITIMER_REAL, &value);
        printf("ITIMER_REAL: internal:%ds%dms,remain:%ds%dms\n",
               value.it_interval.tv_sec, value.it_interval.tv_usec, value.it_value.tv_sec,
               value.it_value.tv_usec);

        getitimer(ITIMER_VIRTUAL, &value);
        printf("ITIMER_VIRTUAL:internal:%ds%dms,remain:%ds%dms\n",
               value.it_interval.tv_sec, value.it_interval.tv_usec, value.it_value.tv_sec,
               value.it_value.tv_usec);

        getitimer(ITIMER_PROF, &value);
        printf("ITIMER_PROF: internal:%ds%dms,remain:%ds%dms\n\n",
               value.it_interval.tv_sec, value.it_interval.tv_usec, value.it_value.tv_sec,
               value.it_value.tv_usec);
        sleep(1);
    }
}
