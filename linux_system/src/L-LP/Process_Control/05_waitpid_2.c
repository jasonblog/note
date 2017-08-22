#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Il processo figlio legge un file e lo copia in un nuovo file, il processo
padre, attende che il figlio porti a termine tutte le sue operazioni, dopodiche'
stampa sullo stdout il contenuto del file creato dal processo figlio.
*/
int main(int argc, char* argv[])
{
    pid_t pid;
    int status, c;
    FILE* fp, *fp2;

    pid = fork();

    switch (pid) {
    case -1:
        fprintf(stderr, "Err.(%s) fork failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0: /* Processo figlio */
        printf("Processo figlio: legge e crea un nuovo file\n\n");
        sleep(2);

        if ((fp = fopen("/etc/fstab", "r")) == NULL) {
            fprintf(stderr, "Err.(%s) fopen() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if ((fp2 = fopen("fstab.txt", "w")) == NULL) {
            fprintf(stderr, "Err.(%s) fopen() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        while ((c = fgetc(fp)) != EOF) {
            fputc(c, fp2);
        }

        fclose(fp);
        fclose(fp2);

        exit(EXIT_SUCCESS);

    default: /* Processo padre */
        /* Attende che il processo figlio termini l'esecuzione */
        waitpid(pid, &status, 0);

        printf("Padre padre: legge il file creato dal figlio\n");

        if ((fp = fopen("fstab.txt", "r")) == NULL) {
            fprintf(stderr, "Err.(%s) fopen() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        while ((c = fgetc(fp)) != EOF) {
            fputc(c, stdout);
        }

        fclose(fp);

        exit(EXIT_SUCCESS);
    }

    return (EXIT_SUCCESS);
}
