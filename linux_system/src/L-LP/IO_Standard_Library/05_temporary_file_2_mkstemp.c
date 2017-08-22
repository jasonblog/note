#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_FILE 64
#define MAX_BUF 512

/*
HEADER    : <stdlib.h>

PROTOTYPE : int mkstemp(char *template);

SEMANTICS : La funzione mkstemp() un file temporaneo unico dalla stringa
            'template'

RETURNS   : Il file descriptor del file temporaneo in caso di successo, -1 in
            caso di errore
--------------------------------------------------------------------------------
Nota: 'template' e' utilizzata come modello per il percorso temporaneo, gli
ultimi 6 caratteri devono essere necessariamente composti da "XXXXXX",
dopodiche' saranno sovrascritti affinche' il percorso risulti unico.
*/

int main(int argc, char* argv[])
{
    /* Path del file temporaneo */
    char file_temp[MAX_FILE];
    char buf[MAX_BUF];
    int fd;
    FILE* f_tmp;

    /* Template della stringa */
    char* temp_template = "/tmp/00-XXXXXX";

    /* Si copia il template nel path temporaneo */
    strcpy(file_temp, temp_template);


    /* Si passa a mkstemp() il modello del percorso del file temporaneo; la
     funzione quindi modifica il contenuto del buffer, sostituendo gli ultimi sei
     caratteri con una stringa univoca */
    if ((fd = mkstemp(file_temp)) < 0) {
        fprintf(stderr, "%s: Creazione file temp.\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Si stampa a video il file temporaneo modificato */
    printf("Temp file : %s\n", file_temp);

    /* Si apre il file temporaneo in lettura e scrittura; da notare che si sta
    utilizzando la funzione fdopen(), poiche' si sta lavorando su un file
    descriptor */
    if ((f_tmp = fdopen(fd, "w+")) == NULL) {
        fprintf(stderr, "%s: file descriptor", strerror(errno));
        exit(EXIT_SUCCESS);
    }

    printf("Inserisci la riga da scrivere sul file temp : %s\n", file_temp);
    fgets(buf, MAX_BUF, stdin);

    /* Si scrive sul file temporaneo */
    fputs(buf, f_tmp);

    rewind(f_tmp);

    if (fgets(buf, sizeof(buf), f_tmp) == NULL) {
        fprintf(stderr, "Err. %s: %s\n", strerror(errno), buf);
        exit(EXIT_FAILURE);
    }

    fputs(buf, stdout);

    /* Si chiude il file temporaneo */
    fclose(f_tmp);

    return (EXIT_SUCCESS);
}
