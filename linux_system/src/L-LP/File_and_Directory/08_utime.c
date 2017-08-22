#include <stdio.h>
#include <stdlib.h>
#include <utime.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/*
HEADER    : <utime.h>

PROTOTYPE : int utime(const char *pathname, const struct utimbuf *times);

SEMANTICS : aggiorna il tempo di accesso e il tempo di modifica di 'pathname'

RETURNS   : 0 In caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
UNIX assegna a ciascun file 3 specifici valori inerenti il tempo, ognuno di essi
e' dichiarato nella struttura stat, e pertanto sono informazioni contenute
nell'inode:

1 - actime : la data dell'ultimo accesso (lettura del file);
2 - st_mtime : la data dell'ultima modifica (scrittura del file);
3 - st_ctime : la data dell'ultima modifica dello stato dell'inode (non riguarda
               il contenuto del file, ma piuttosto la modifica dei permessi o
           del proprietario del file mediante chmod() o chown()).

La funzione utime() consente la gestione dei primi due, ossia ultimo accesso ed
ultima modifica, mediante il puntatore 'times' alla struttura 'utimbuf':

struct utimbuf {
   time_t actime;   "Data di accesso"
   time_t modtime;  "Data di modifica"
};

Da notare che invocando il  comando "ls" con opportuni parametri e' possibile
visualizzare gli orari riferiti a ciascuna voce di directory (lista dei file),
ad esempio:

$ ls -l  (il campo dell'orario si riferisce all'ultima modifica);
$ ls -lt (il campo dell'orario si riferisce all'ultima modifica);
$ ls -lu (il campo dell'orario si riferisce all'ultimo accesso);
$ ls -lc (il campo dell'orario si riferisce all'ultimo cambiamento di stato).
*/

int main(int argc, char* argv[])
{
    int fd, i;
    struct stat bufstat;
    struct utimbuf bufutime;

    if (argc < 2) {
        fprintf(stderr, "Uso: %s <filename1>...<filenameN>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*
     La funzione stat() verifica la presenza del file ricevuto in input, qualora
     dovesse filare tutto liscio esso passa alla successiva funzione open(), che
     provvede ad aprirlo in lettura e scrittura, nonche', con O_TRUNC, ne azzera
     il contenuto; il file pertanto e' stato acceduto e modificato, per cui, in
     teoria, dovrebbe essere modificato sia l'orario di accesso sia l'orario di
     modifica.
    */
    for (i = 1; i < argc; i++) {
        if (stat(argv[i], &bufstat) < 0) {
            fprintf(stderr, "Err.:(%d) - %s on %s\n", errno, strerror(errno), argv[i]);
            continue;
        }

        if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {
            fprintf(stderr, "Err.:(%d) - %s on %s\n", errno, strerror(errno), argv[i]);
            continue;
        }

        close(fd);

        /*
         Nonostante il file sia stato azzerato, le date di accesso e di modifica
         non saranno toccate, o meglio, i rispettivi campi saranno impostati con
         l'orario di creazione del file stessi, per cui "apparentemente"
         resteranno immutati.
        */
        bufutime.actime = bufstat.st_atime;
        bufutime.modtime =  bufstat.st_mtime;

        /*
         Le date ora sono resettate al valore attuale, tuttavia solo la data
         relativa al cambiamento di stato sara' resettata, le altre due, come si
         e' mostrato pocanzi, non saranno toccate.
        */
        if (utime(argv[i], &bufutime) < 0) {
            fprintf(stderr, "Err.:(%d) - %s on %s\n", errno, strerror(errno), argv[i]);
            continue;
        }

        /*
         Per verificare il buon esito e' sufficiente eseguire i seguenti passi:
         $ >FILE_1 && >FILE_2
         $ stat FILE_*
         $ ./a.out FILE_1 FILE_2
         Si notera' che i campi Access e Modify non sono stato modificati.
        */
    }

    return (EXIT_SUCCESS);
}
