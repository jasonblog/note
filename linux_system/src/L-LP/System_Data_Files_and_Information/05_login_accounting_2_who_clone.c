#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <utmp.h>
#include <unistd.h>

void get_time(long);
void get_info(struct utmp*);

/* Il programma simula il comportamento del comando who */
int main(void)
{
    struct utmp utbuf;
    int utmpfd;

    if ((utmpfd = open(UTMP_FILE, O_RDONLY)) < 0) {
        fprintf(stderr, "Err.(%s) open() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (read(utmpfd, &utbuf, sizeof(utbuf)) == sizeof(utbuf)) {
        get_info(&utbuf);
    }

    close(utmpfd);

    return (EXIT_SUCCESS);
}

void get_info(struct utmp* utbufp)
{
    if (utbufp->ut_type != USER_PROCESS) {
        return;
    }

    printf("%-8.8s", utbufp->ut_name);
    printf(" ");
    printf("%-8.8s", utbufp->ut_line);
    printf(" ");

    get_time(utbufp->ut_time);
#ifdef SHOWHOST

    if (utbufp->ut_host[0] != '\0') {
        printf(" (%s)", utbufp->ut_host);    /* the host    */
    }

#endif

    printf("\n");
}

void get_time(long timeval)
{
    char* cp;
    cp = ctime(&timeval);
    printf("%12.12s", cp + 4);
}
