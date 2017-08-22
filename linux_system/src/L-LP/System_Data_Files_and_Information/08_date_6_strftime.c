#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define MAX_STR 150

/*
HEADER    : <time.h>
PROTOTYPE : size_s strftime(char *buf, size_t maxsize, const char *format,
                            const struct tm *ptm);
SEMANTICS : La funzione strftime() converte un tempo broken-down ime 'ptm' in
            una stringa da collocare in buf, di dimensioni 'maxsize' e secondo
        il formato definito da 'format'.
RETURNS   : Il numero di caratteri in buf, 0 in caso di errore
--------------------------------------------------------------------------------
Nota: La costante stringa 'format' puo' contenere diversi specificatori di
conversione (come la printf), ciascuno di essi introdotto dal carattere '%',
sono facilmente reperibili mediante il classico man strftime.
*/

int main(int argc, char* argv[])
{
    char buf[MAX_STR];
    struct tm* ptm;
    time_t t;

    /* Ad esempio se si volesse ottenere il giorno, mese ed anno attuale si
    utilizzerebbe il seguente formato:
    $ ./a.out '%d %b %Y'
    */
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <specificatore/i di conversione>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((t = time(NULL)) < 0) {
        fprintf(stderr, "Err.(%s) getting time\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((ptm = localtime(&t)) == NULL) {
        fprintf(stderr, "Err.(%s) conversion localtime()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* La funzione strftime() consente una gran varieta' di specificatori di
    conversione, peraltro utilizzati ampiamente dalla printf. */
    if (strftime(buf, sizeof(buf), argv[1], ptm) == 0) {
        fprintf(stderr, "Err.(%s) - set string: strftime()\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("%s", buf);

    return (EXIT_SUCCESS);
}
