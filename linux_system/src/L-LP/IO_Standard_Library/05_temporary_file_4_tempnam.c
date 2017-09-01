#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
HEADER    : <stdio.h>

PROTOTYPE : char *tempnam(const char *dir, const char *pfx);

SEMANTICS : Il suffisso del file temporaneo generato e' indicato da 'pfx', 'dir'
            invece indica una directory.

RETURNS   : Il puntatore ad una stringa unica indicante il file temporaneo in
            in caso di successo, NULL in caso di errore
--------------------------------------------------------------------------------
Nota: La funzione tempnam() accetta due parametri, il secondo dei quali e'
facoltativo e puo' essere definito NULL, allorquando non e' nullo invece puo'
determinare fino ad un massimo di 5 caratteri da utilizzarsi come prefisso per
la generazione del file temporaneo.

Il parametro dir puo' essere definito NULL oppure puo' fare riferimento ad una
directory, nel secondo caso ci sono 4 metodi da poter eseguire per individuare
la  directory di riferimento:

1 - tempnam() legge la variabile di ambiente TMPDIR, se e' definita e il
    programmatore ha pieno accesso ad essa, e avra' la precedenza su tutti gli
    altri metodi definiti;

2 - Se 1 fallice, si verifica la validita' del parametro dir, il quale, se non
    e' nullo, sara' utilizzato come directory;

3 - Se 2 fallisce si tentera' con la directory indicata dalla macro P_tmpdir;

4 - Se anche 3 fallisce, si utilizzera' la directory /tmp per la collocazione
    del file temporaneo.

Testare il programma con diversi parametri, ad esempio si tenti di esportare la
variabile di ambiente TMPDIR
$ export TMPDIR=DIR

Poi lanciare il programma e verificare il percorso, tentare anche di fornire
dei permessi estremamente limitativi alla directory stessa in modo tale da
testare ciascuno dei 4 passi, di seguito il test prodotto:

$ ./a.out
Nome del file temporaneo: /home/b3h3moth/DIR/tmp-3a5JLs

$ export TMPDIR=/home/b3h3moth/DIR
$ ./a.out
Nome del file temporaneo: /home/b3h3moth/DIR/tmp-Wg5pBa

$ chmod 444 $TMPDIR
$ ./a.out
Permission denied: Generazione del nome del file temporaneo

$ chmod 755 $TMPDIR
$ ./a.out
Nome del file temporaneo: /home/b3h3moth/DIR/tmp-jwtQ3w

$ unset TMPDIR
$ ./a.out
Nome del file temporaneo: /tmp/tmp-Chx5cK
*/

int main(int argc, char* argv[])
{
    /* Percorso del file temporaneo */
    char* f_tmp_path;

    /* File stream temporaneo */
    FILE* f_tmp;

    if ((f_tmp_path = tempnam(".MIADIR", "tmp-")) == NULL) {
        fprintf(stderr, "%s: Generazione file temporaneo\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Nome del file temporaneo: %s\n", f_tmp_path);

    if ((f_tmp = fopen(f_tmp_path, "w+")) == NULL) {
        fprintf(stderr, "%s: Apertura file\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si scrive una riga nel file temporaneo */
    fputs("The art of *nix programming\n", f_tmp);

    fclose(f_tmp);
    free(f_tmp_path);

    return (EXIT_SUCCESS);
}
