#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
Nota: la libreria Standard I/O <stdio.h> e' stata  scritta nel 1975 dal
      compianto "Dennis Ritchie", venuto a mancare nell'ottobre del 2011.

Nella sezione "IO_Unbuffered" si e' discusso della gestione dell'I/O mediante
le primitive di basso livello fornite dal sistema operativo, che interagiscono
direttamente con il kernel, ora invece si affronta l'I/O bufferizzato gestito
con le funzioni della libreria ISO C standard, implementate peraltro proprio
sopra le primitive di basso livello.

Allorquando si apre un file in lettura o in scrittura utilizzando la libreria
I/O standard associamo ad esso uno stream, il cui orientamento
determina se la lettura o scrittura dei caratteri sul file sara' single-byte
oppure multi-byte (wide).

La funzione fopen() apre uno stream e ritorna un puntatore ad un oggetto FILE,
ovvero una struttura al cui interno vi sono svariate informazioni che
consentono la gestione dello stream, tra cui:
- Posizione corrente all'interno del file;
- Indicatore di errore;
- Indicatori di stato (lettura, scrittura o entrambi);
- grandezza del buffer;
- Fine del file (EOF).

Nota: Il programmatore non ha alcuna necessita' di interagire direttamente con
      i vari campi della struttura FILE, poiche' ciascuna operazione di I/O
      potra' gestirla mediante un puntatore alla struttura stessa.

Come accennato in IO_Unbuffered, ogni processo dispone di 3 stream predefiniti,
o file descriptor, nella sostanza sono gli stessi:
- standard input  = stdin  - POSIX(STDIN_LINENO);
- standard output = stdout - POSIX(STDOUT_LINENO;
- standard error  = stderr - POSIX(STDERR_LINENO).

Cosa si intende per I/O bufferizzato? I caratteri scritti su uno stream sono
conservati in un buffer, essi tuttavia saranno trasmessi in blocco solo quando
il buffer risultera' essere pieno - la bufferizzazione e' gestita
automaticamente dalla standard library. Il lavoro di bufferizzazione e di
trasmissione dei dati in blocco e' svolto in modalita' asincrona, per cui e'
possibile che, qualora due processi accedano contemporaneamente ad un file,
possano sorgere delle incongruenze.

Vi sono 3 tipi di buffering (modalita' di bufferizzazione):
- Fully Buffered, I caratteri sono trasmessi non appena il buffer e' pieno;
- Line buffered , I caratteri sono trasmessi non appena si incontra un '\n';
- Unbuffered    , Non vi e' bufferizzazione; i caratteri sono trasmessi non
                  appena e' possibile.

Di norma lo standard error e' sempre trasmesso in modalita' unbuffered, mentre
lo standard input e lo standard output sono in modalita' line buffered se
associati ad un terminale, fully bufferd altrimenti.
--------------------------------------------------------------------------------
HEADER    : <stdio.h>

PROTOTYPE : FILE *fopen(const char *path, const char *mode);
            FILE *freopen(const char *path, const char *mode, FILE *stream);
            FILE *fdopen(int fd, const char *mode);

SEMANTICS : La funzione fopen() apre il file puntato da 'path', secondo i
            permessi 'mode';
            la funzione freopen() apre il file 'path' secondo i permessi 'mode'
            e lo associa allo stream 'stream';
            la funzione fdopen() apre lo stream definito dal file descriptor
            'fd', secondo i permessi 'mode'.

RETURNS   : Un puntatore al file in caso di successo, NULL in caso di errore
--------------------------------------------------------------------------------
La funzione fopen() naturalmente e' la piu' adoperata, per chiudere uno stream
invece si adopera fclose().
*/

int main(int argc, char* argv[])
{
    FILE* fs, *frp;

    if ((fs = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Err.:(%d) - %s: fopen()\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* si chiude lo stream */
    fclose(frp);

    return (EXIT_SUCCESS);
}
