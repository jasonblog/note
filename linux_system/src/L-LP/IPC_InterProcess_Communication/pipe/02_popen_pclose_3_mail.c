#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

#define MAX_BUF 256

int main(int argc, char* argv[])
{
    struct passwd* pwd = 0;     /* Password info */
    char cmd[MAX_BUF];
    FILE* fp;

    /* Verifica userid */
    if ((pwd = getpwuid(geteuid())) == NULL) {
        fprintf(stderr, "Err.: %d getpwuid() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /*
    printf("%s %s\n", pwd->pw_name, pwd->pw_passwd);
    */

    /* Comando da eseguire, definizione del formato per mailx */
    sprintf(cmd, "mailx -s 'Un messaggio dal processo %ld' %s",
            (long)getpid(), pwd->pw_name);

    /*
    printf("%s\n", cmd);
    */

    /* Apertura della pipe in scrittura verso mailx */
    if ((fp = fopen(cmd, "w")) == NULL) {
        fprintf(stderr, "Err.: %d fopen() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Scrittura del messaggio nella pipe */
    fprintf(fp, "Questo e' il comando %s \n", argv[0]);
    fprintf(fp, "Sto lavorando per %s\n", pwd->pw_gecos);

    /* Verifica che il programma sia in esecuzione come root */
    if (getuid() != 0) {
        fprintf(fp, "Dovresti essere root\n");
    } else {
        fprintf(fp, "Sei root\n");
    }

    fprintf(fp, "Grazie, PID: %ld\n", (long)getpid());

    if (pclose(fp) == -1) {
        fprintf(stderr, "errore\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Messaggio a studente 1");
    }


    return (EXIT_SUCCESS);
}
