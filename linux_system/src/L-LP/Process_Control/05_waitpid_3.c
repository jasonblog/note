#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <errno.h>
#define PS_PATH     "/bin/ps"

extern char** environ;

/* Si rimpiazza questo processo con il comando ps */
static void exec_ps_cmd(void);

int main(int argc, char* argv[])
{
    pid_t pid;       /* PID del processo figlio */
    pid_t wpid;      /* PID dalla wait() */
    int status;      /* Stato di uscita dalla wait() */

    /* Si crea un nuovo processo figlio */
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "%s: fork() fallita\n", strerror(errno));
        exit(13);
    }

    if (pid == 0) {
        /* Il processo figlio in esecuzione */
        printf("PID %ld: Processo figlio parte - Il processo padre %ld\n",
               (long)getpid(), /* PID processo figlio */
               (long)getppid());   /* PID processo padre */

        exec_ps_cmd();        /* Inizia il comando ps */
    } else {
        /* Il processo padre in esecuzione */
        printf("PID %ld: Processo figlio partito - Il processo padre %ld\n",
               (long)getpid(), /* PID processo padre */
               (long)pid); /* PID processo figlio */

        /* Attende la terminazione del processo figlio */
        wpid = waitpid(-1, &status, 0);

        if (wpid == -1) {
            fprintf(stderr, "%s: wait()\n", strerror(errno));
            return 1;
        }

        if (wpid != pid) {
            abort();    /* Non potra' mai accadere in questo programma */
        } else {
            /* Il processo figlio e' terminato */
            if (WIFEXITED(status)) {
                /* uscita normale - stampa lo stato */
                printf("Uscita $? = %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                /* Processo abortito, kill o signal */
                printf("Signal: %d%s\n", WTERMSIG(status), WCOREDUMP(status)
                       ? " with core  file."
                       : "");
            } else {
                /* Stoppato il processo figlio */
                printf("Stopped\n");
            }
        }
    }

    return (EXIT_SUCCESS);
}

static void exec_ps_cmd(void)
{
    static char* argv[] = {"ps", "f", NULL};

    /* Esegue il comando ps: ps f */
    execve(PS_PATH, argv, environ);

    /* Se il controllo arriva qua allora la execve() ha fallito */
    fprintf(stderr, "%s: execve()\n", strerror(errno));
}
