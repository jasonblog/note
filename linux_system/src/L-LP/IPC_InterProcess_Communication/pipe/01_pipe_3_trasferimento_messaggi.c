#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF     256
#define MAX_MSG_LEN 72
#define MAX_MSG_NUM 3

/* Si procede alla creazione di una pipe il cui compito e' il trasferimento di
messaggi - stringhe ottenute in input di lunghezza massima predefinita - all'
interno del medesimo  processo.
*/

void write_msg(int fd[]);
void read_msg(int fd[]);
void set_last_char(char* str);

int main(int argc, char* argv[])
{
    int status;
    int fd[2];

    /* Creazione della pipe */
    if ((status = pipe(fd)) < 0) {
        fprintf(stderr, "Err.: %d pipe() - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* scrive il messaggio nella pipe */
    write_msg(fd);

    /* legge messaggio dalla pipe */
    read_msg(fd);

    /* Chiusura dei file descriptor della pipe */
    close(fd[0]);
    close(fd[1]);

    return (EXIT_SUCCESS);
}

/* La funzione write_msg() scrive un messaggio nella pipe */
void write_msg(int fd[])
{
    int i;
    char buf[MAX_BUF];

    for (i = 0; i < MAX_MSG_NUM; i++) {
        printf("write - msg[%d] = ", i);
        /*scanf("%s", buf);*/
        fgets(buf, MAX_MSG_LEN, stdin);

        /* Rimozione '\n' e aggiunta del terminatore stringa '\n' finale */
        set_last_char(buf);

        /* Scrittura nella pipe */
        if (write(fd[1], buf, MAX_MSG_LEN) != MAX_MSG_LEN) {
            fprintf(stderr, "Err.: %d write() - %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

/* La funzione read_msg() si occupa della lettura dei messaggi dalla pipe */
void read_msg(int fd[])
{
    int i;
    char buf[MAX_BUF];

    for (i = 0; i < MAX_MSG_NUM; i++) {
        if (read(fd[0], buf, MAX_MSG_LEN) <= 0) {
            fprintf(stderr, "Err.: %d read() %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        printf("read - msg[%d] = %s\n", i, buf);
    }

}

/* La funzione set_last_char() elimina il carattere '\n' finale inserendo il
carattere di terminazione stringa '\0' */
void set_last_char(char* str)
{
    int i;

    for (i = strlen(str); i >= 0; i--) {
        if (str[i] == '\n') {
            str[i] = '\0';    /* carattere di terminazione stringa */
        }
    }
}
