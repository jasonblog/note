#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define PAGER "${PAGER:-less}"
#define MAX_BUF 512

/* Il lavoro fatto dalla funzione pipe() viene svolto interamente dalle funzioni
popen() e pclose().

La funzione popen() e' molto simile alla fopen(), la differenza sostanziale e'
che il primo parametro dev'essere un comando eseguibile dalla shell, invece di
un pathname, il secondo parametro  si occupa delle regolarizzazione della
pipe in lettura 'r' o in scrittura 'w'; oltre a questa differenziazione la
pipe puo' essere letta e scritta come se fosse un file e con le medesime
funzioni della libreria Standard I/O.

HEADER    : <stdio.h>
PROTOTYPE : FILE *popen(const char *cmdstring, const char *type);
SEMANTICS : La funzione popen() lavora in questa maniera:
            - Apre innanzitutto una una pipe;
            - Chiama le funzioni fork() ed exec() per la creazione prima di un
              nuovo processo e per l'avvio della shell poi;
            - La shell si occupa dell'esecuzione del comando contenuto in
              'cmdstring', primo parametro della popen();
            - Il parametro 'type' puo' essere 'r' o 'w', lettura e scrittura
              rispettivamente.
RETURNS   : Un puntatore a FILE in caso di successo, NULL in caso di errore
--------------------------------------------------------------------------------
*/

int main(int argc, char* argv[])
{
    char buf[MAX_BUF];
    FILE* fp_in, *fp_out;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Apertura di un file in letura */
    if ((fp_in = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Err.: %d fopen() %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Crea un pipe, definendo il programma da aprire */
    if ((fp_out = popen(PAGER, "w")) == NULL) {
        fprintf(stderr, "Err.: %d popen() %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Copia il contenuto del file passato ad argv[1] al pager */
    while (fgets(buf, MAX_BUF, fp_in) != NULL) {
        if (fputs(buf, fp_out) == EOF) {
            fprintf(stderr, "Err.: %d fputs() %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Verifica degli errori */
    if (ferror(fp_in)) {
        fprintf(stderr, "Err.: %d ferror() %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pclose(fp_out) == -1) {
        fprintf(stderr, "Err.: %d pclose() %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
