#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
HEADER    : <sys/wait.h>
PROTOTYPE : pid_t waitpid(pid_t pid, int status, int options);
SEMANTICS : La funzione waitpid() attende la conclusione del processo figlio
RETURNS   : Il PID del processo terminato, -1 in caso di errore
--------------------------------------------------------------------------------
La funzione wait() e' deprecata in favore della funzione waitpid(), la seconda
infatti e' molto piu' flessibile in quanto consente di poter gestire diverse
caratteristiche, tra cui la definizione del processo da 'attendere'.

La funzione waitpid() puo' specificare il processo da attendere in base al
valore di 'pid', che puo' assumere i seguenti valori:
- pid == -1 : attende un figlio qualsiasi (equivalente alla wait());
- pid > 0   : attende un figlio il cui PID e' uguale a 'pid';
- pid == 0  : attende qualsiasi figlio il cui Group ID e' uguale al chiamante;
- pid < -1  : attende qualsiasi figlio il cui Group ID e' uguale al valore
              assoluto di 'pid'.

La funzione waitpid() ritorna il PID del figlio terminato e salva il valore del
"termination status" del figlio stesso nella locazione di memoria puntata da
'status', essa inoltre puo' modificare ulteriormente il proprio comportamento
mediante il parametro 'options', i cui possibili valori, oltre a 0, possono
essere:
- WCONTINUED : lo stato di qualsiasi figlio che continua l'esecuzione dopo
               essere stato arrestato, ma il cui "termination status" non e'
           stato ancora ritornato;
- WNOHANG    : previene il blocco della funzione quando il processo figlio non
               e' terminato;
- WUNTRACED  : consente di tracciare i processi bloccati.

La funzione waitpid() e' una versione non bloccante della funzione wait().
*/

int main(int argc, char* argv[])
{
    pid_t pid;
    int status, c;
    FILE* fp;

    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Err.(%s) fork failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        sleep(2);

        /* Il processo figlio legge e stampa sullo stdout il risultato */
        printf("Processo Figlio:\n");

        if ((fp = fopen("/etc/fstab", "r")) == NULL) {
            fprintf(stderr, "Err.(%s) fopen() failed\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        while ((c = fgetc(fp)) != EOF) {
            fputc(c, stdout);
        }

        fclose(fp);
        exit(EXIT_SUCCESS);

    } else {
        /* Attende che il processo figlio termini l'esecuzione */
        waitpid(pid, &status, 0);

        printf("Processo Padre\n");
    }

    return (EXIT_SUCCESS);
}
