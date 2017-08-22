#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
    pid_t pid;   /* PID del processo figlio */
    pid_t wpid;  /* PID dalla wait() */
    int status;  /* Stato di uscita dalla wait() */

    pid = fork();    /* Si crea un nuovo processo figlio */

    if (pid == -1) {
        fprintf(stderr, "%s: Fallimento della fork()\n", strerror(errno));
        exit(13);
    }

    if (pid == 0) {
        printf("PID %ld: Figlio partito, il padre è: %ld\n",
               (long)getpid(),     /* Il nostro PID */
               (long)getppid());   /* Il PID del padre */
    } else {
        printf("PID %ld: Il figlio partito PID %ld\n",
               (long)getpid(), /* Il nostro PID */
               (long)pid);     /* Il PID del figlio */

        wpid = wait(&status); /* Exit status del processo figlio */

        if (wpid == -1) {
            fprintf(stderr, "%s: wait()\n", strerror(errno));
            return 1;
        }

        if (wpid != pid) {
            abort();    /* non puo' mai accadere in questo programma */
        } else
            printf("PID figlio %ld stato di uscita 0x%04X\n",
                   (long)pid,   /* PID figlio */
                   status);     /* Stato di uscita */
    }

    sleep(5);


    return (EXIT_SUCCESS);
}
