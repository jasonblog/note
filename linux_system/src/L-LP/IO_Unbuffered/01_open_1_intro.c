#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/*
Il termine Unbuffered I/O (definito anche I/O di basso livello) indica che le
varie routine (open, read, write, lseek, close, etc...) sono chiamate dirette
al sistema operativo e pertanto gestite dal Kernel stesso, inoltre l'accesso
in lettura e in scrittura sul disco comporta un  "semplice" trasferimento di
byte, naturalmente senza formattazione.

L'I/O Unbuffered non e' standard ISO C, bensi' standard POSIX 1., nonche'
Single Unix Specification.

HEADER    : <fcntl.h>
PROTOTYPE : int open(const char *pathname, int oflag, ...  mode_t mode);
SEMANTICS : La funzione open() apre o crea il file definito in 'pathname',
            secondo la modalita' indicata da 'oflags' ed eventualmente con i
        permessi 'mode'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Allorquando si apre un file, il kernel rilascia un file descriptor al processo,
ossia un intero non negativo che identifica un file aperto; nei sistemi UNIX ad
ogni processo sono associati 3 file descriptor: standard input 0, standard
output 1, standard error 2.

POISX peraltro ne specifica anche le costanti simboliche in:
- standard input  = 0 = STDIN_FILENO;
- stanbdar output = 1 = STDOUT_FILENO;
- standard error  = 2 = STDERR_FILENO;

Nota: per cio' che concerne la shell, lo standard input corrisponde ai dati
      ricevuti mediante tastiera, lo standard output e lo standard error
      corrispondono ai dati inviati al terminale.

Per poter iniziare a lavorare con un file e' necessario per prima cosa aprirlo,
la open() ha questo compito, ed e' peraltro anche il momento in cui sono
associati ad esso i 3 file descriptor di default; volendo essere ancor piu'
precisi la open() alloca il file, lo inserisce nella file table e crea il
riferimento nella struttura files_struct del processo.

Nota: Il valore di 'oflag' e' ottenuto mediante un OR aritmetico tra le
      varie costanti simboliche, quando i bit costituiscono un file status
      flag.

Il comportamento della funzione open() e le diverse modalita' con cui un file
puo' essere aperto sono definite dal valore di 'oflag', il cui contenuto e'
ottenuto come maschera binaria in cui ciascun bit ha un significato ben
preciso.

I bit che caratterizzano le costanti simboliche sono suddivisi in tre categorie:

1 - Flag delle modalita' di accesso sui file (solo uno);
- O_RDONLY    - Apre il file in lettura;
- O_WRONLY    - Apre il file in scrittura;
- O_RDWR      - Apre il file il lettura e scrittura;

2 - Flag delle modalita' di apertura sui file (in combinazione):
- O_CREAT     - Se il file non esiste sara' creato;
- O_DIRECTORY - Se 'pathname' non e' una directory la chiamata fallisce;
- O_EXCL      - E' utilizzato in concomitanza con O_CREAT, in modo che se il
                file dovesse essere gia' presente nel filesystem, ritornerebbe
            un errore EEXIST, ossia file gia' esistente nel filesystem;
- O_LARGEFILE - Apertura di file molto grandi;
- O_NOCTTY    - Se 'pathname' si riferisce ad un terminale, esso non diventera'
                il terminale di controllo, anche qualora il processo ancora non
        ne abbia uno;
- O_NOFOLLOW  - Se 'pathname' e' un collegamento simbolico la chiamata fallisce;
- O_TRUNC     - Se usato un file aperto in scrittura, tronca la grandezza a 0;

3 - Flag delle modalita' di operazione sui file (in combinazione):
- O_APPEND    - La posizione corrente viene impostata alla fine del file;
- O_ASYNC     - Apre il file in modalita' asincrona;
- O_CLOEXEC   - Attiva la modalita' close-on-exec sul file;
- O_DIRECT    - Esegue l'I/O dalla memoria in user-space in modo asincrono;
- O_NOATIME   - Blocca l'aggiornamento dei tempi d'accesso del file;
- O_NONBLOCK  - Apre il file in modalita' non bloccante per l'I/O;
- O_NDELAY    - Su Linux e' simile alla O_NONBLOCK, altrimenti causa il ritorno
                da una read() con un valore nullo;
- O_SYNC      - Apre il file per l'I/O sincrono. Qualsiasi scrittura di dati si
                blocchera' all'arrivo di tutti i dati sull'hardware sottostante;
- O_DSYNC     - Apre il file per l'I/O sincrono. Qualsiasi scrittura di dati si
                blocchera' alla conferma dell'arrivo sull'hardware sottostante.
*/

int main(int argc, char* argv[])
{
    int fd;
    char* filename = "/etc/fstab";

    /* Si apre un file in sola lettura, il terzo parametro non e' necessario*/
    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /*
     Si chiude subito il canale di comunicazione stabilito con il kernel, senza
     eseguire alcuna operazione aggiuntiva
    */
    close(fd);

    /* Esempio di utilizzo di due costanti simboliche in relazione tra loro;
    nel caso specifico se il file e' gia' presente sul filesystem la funzione
    open() ritorna un errore EEXIST */
    if ((fd = open("01_open.c", O_CREAT | O_EXCL, 0644)) < 0) {
        fprintf(stderr, "Err: (%d) - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(fd);

    return (EXIT_SUCCESS);
}
