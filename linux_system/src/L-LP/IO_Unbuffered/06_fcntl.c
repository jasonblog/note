#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/*
HEADER    : <fcntl.h>
PROTOTYPE : int fcntl(int fd, int cmd, ... < arg > );
SEMANTICS : La funzione fcntl() esegue diverse operazioni sul file descriptor
            'fd' a seconda del valore di 'cmd'; il terzo argomento 'arg' puo'
        indicare un intero oppure un puntatore alla struttura struct flock.
RETURNS   : I valori di ritorno sono diversi a seconda dell'operazione eseguita,
            sempre -1 in caso di errore
--------------------------------------------------------------------------------
La funzione fcntl() puo' modificare le proprieta' di un file descriptor, nel
caso specifico 'fd', tuttavia il numero degli argomenti, il valore di ritorno
e gli errori sono strettamente legati al valore della variabile intera 'cmd',
che puo' assumere i seguenti valori:

F_DUPFD , duplica il file descriptor;
F_GETFD , ritorna il valore del file descriptor flag;
F_SETFD , setta il valore del file descpritor flag;
F_GETFL , ritorna il valore del file status flag;
F_SETFL , setta il valore del file status flag;
F_GETLK , ritorna informazioni sul record locking;
F_SETLK , setta informazioni sul record locking;
F_SETLKW, come sopra, non ritorna subito ma attende il rilascio del blocco;
F_GETOWN, ritorna il PID o il PGID, preposto alla ricezione dei segnali SIGURS;
F_SETOWN, setta il PID o il PGID, preposto alla ricezione dei segnali SIGURS;
F_GETSIG, ritorna il segnale inviato quando l'input o l'output e' disponibile;
F_SETSIG, setta il segnale inviato quando l'input o l'output e' disponibile;
F_SETLEASE, imposta o rimuove un file lease;
F_GETLEASE, setta un file lease;
F_NOTIFY, attiva un meccanismo di notifica.
*/

int main(int argc, char* argv[])
{
    int val;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <descriptor>\n", argv[0]);
        /* es: ./a.out 0 < /etc/fstab */
        exit(EXIT_FAILURE);
    }

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0) {
        fprintf(stderr, "Err.(%s) fcntl() failed\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    switch (val & O_ACCMODE) {
    case O_RDONLY:
        printf("solo lettura");
        break;

    case O_WRONLY:
        printf("solo scrittura");
        break;

    case O_RDWR:
        printf("lettura scrittura");
        break;

    default:
        printf("access mode sconosciuto");
        exit(EXIT_FAILURE);
    }

    if (val & O_APPEND) {
        printf(", append");
    }

    if (val & O_NONBLOCK) {
        printf(", non bloccante");
    }

#if defined(O_SYNC)

    if (val & O_SYNC) {
        printf(", scrittura sincrona");
    }

#endif
    printf("\n");

    return (EXIT_SUCCESS);
}
