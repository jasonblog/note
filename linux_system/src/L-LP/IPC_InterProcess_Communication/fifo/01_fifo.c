#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE    256
#define PERMS   (S_IRUSR | S_IWUSR)

int fifo_child(const char* fifo_name, const char* id_str);
int fifo_parent(const char* fifo_name);

/* Una fifo o altrimenti detta 'named pipe' - pipe con nome - a differenza di
una pipe e' identificata mediante un nome, gode di permessi specifici ed e'
vista a tutti gli effetti come fosse un file, per cui le  comuni funzioni per
l'I/O di basso livello non bufferizzato come open o read  possono essere
utilizzate.

Creare una fifo e' come creare un file - in realta' la definizione corretta e'
file speciale - e puo' essere utilizzata da processi differenti.

HEADER    : <sys/stat.h>
PROTOTYPE : int mkfifo(const char *pathname, mode_t mode);
SEMANTICS : La funzione mkfifo() crea la FIFO, corrispondente al file speciale
            indicato da 'pathname' con modalita' di accesso definite da 'mode'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
*/

/* Il programma crea un file speciale fifo o named pipe, specificando il
pathname come primo e unico argomento, dopodiche' il processo figlio scrivera'
sulla pipe e il processo padre leggera' il contenuto. */

int main(int argc, char* argv[])
{
    pid_t pid;

    /* Si verifica che vi sia un argomento fornito al programma */
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <nome della pipe>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Creazione del file speciale FIFO o named pipe */
    if (mkfifo(argv[1], PERMS) == -1) {
        fprintf(stderr, "[%ld]Err: mkfifo() %s: %s\n", (long)getpid(),
                argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Creazione di un nuovo processo */
    if ((pid = fork()) == -1) {
        fprintf(stderr, "[%ld]Err: fork(): %s\n", (long)getpid(),
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
        /* Processo figlio: si occupa di scrivere */
    {
        return fifo_child(argv[1], "\"MESSAGGIO DAL PROCESSO FIGLIO\"\n");
    } else
        /* Processo padre */
    {
        return fifo_parent(argv[1]);
    }

    return (EXIT_SUCCESS);
}

/* Lo scopo della funzione fifo_child() e' di consentire al processo figlio di
scrivere su una named pipe. */
int fifo_child(const char* fifo_name, const char* id_str)
{
    char buf[BUF_SIZE];
    int fd, rval;
    ssize_t str_size;

    fprintf(stderr, "[%ld]: (processo figlio) apertura FIFO %s...\n",
            (long)getpid(), fifo_name);

    while ((fd = open(fifo_name, O_WRONLY)) == -1) {
        fprintf(stderr, "[%ld]Err: open() FIFO scrittura %s: %s\n",
                (long)getpid(), fifo_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Scrittura in buf del messaggio, compreso pid del processo */
    rval = snprintf(buf, BUF_SIZE, "[%ld]:%s\n", (long)getpid(), id_str);

    if (rval < 0) {
        fprintf(stderr, "[%ld ]Err: scrittura stringa\n", (long)getpid());
        exit(EXIT_FAILURE);
    }

    /* Calcolo della lunghezza del messaggio */
    str_size = strlen(buf) + 1;

    fprintf(stderr, "[%ld]: scrittura...\n", (long)getpid());

    /* Si scrive 'buf' sulla FIFO */
    if ((rval = write(fd, buf, str_size)) != str_size) {
        fprintf(stderr, "[%ld]Err: scrittura pipe\n", (long)getpid());
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[%ld]: conclusione...\n", (long)getpid());

    return (EXIT_SUCCESS);
}

/* Lo scopo della funzione fifo_parent() e' di consentire al processo padre di
leggere da una named pipe. */
int fifo_parent(const char* fifo_name)
{
    char buf[BUF_SIZE];
    int fd, rval;

    fprintf(stderr, "[%ld]: (processo padre) apertura FIFO %s...\n",
            (long)getpid(), fifo_name);

    while ((fd = open(fifo_name, O_RDONLY)) == -1) {
        fprintf(stderr, "[%ld]Err: open() FIFO lettura %s: %s\n",
                (long)getpid(), fifo_name, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[%ld]: lettura...\n", (long)getpid());

    /* Lettura */
    if ((rval = read(fd, buf, BUF_SIZE)) == -1) {
        fprintf(stderr, "[%ld ]Err: lettura dalla pipe\n", (long)getpid());
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "[%ld]: lettura %.*s\n", (long)getpid(), rval, buf);

    return (EXIT_SUCCESS);
}
