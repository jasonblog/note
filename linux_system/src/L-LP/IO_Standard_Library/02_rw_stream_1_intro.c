#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
Aperto uno stream possono essere utilizzate 3 modalita' di I/O non bufferizzato:
- Character at a time I/O, lettura/scrittura di un carattere alla volta;
- Line at a time I/O     , lettura/scrittura di una linea alla volta;
- Direct I/O             , lettura/scrittura di blocchi, in genere binari.

Le funzioni inerenti l'INPUT - Character at a time I/O:
================================================================================
HEADER    : <stdio.h>
PROTOTYPE : int getc(FILE *fp);
            int fgetc(FILE *fp);
            int getchar(void);
SEMANTICS : La funzione getc() legge il carattere dallo stream 'fp'; la funzione
            fgetc() legge il carattere dallo stream 'fp'; la funzione getchar()
        e' equivalente a getc(stdin).
RETURNS   : c in caso di successo, EOF in caso di errore
--------------------------------------------------------------------------------
Nota: la funzione getc() puo' essere implementata come una macro.

Ogni funzione sopra citata ritorna un "unsigned char" convertito in "int"; da
notare che poiche' ritornano lo stesso valore, sia se si dovesse raggiungere la
fine di un file sia se si dovesse riscontrare un errore, vi sono due funzioni
create ad hoc per consentire la verifica dell'uno o dell'altro:

HEADER    : <stdio.h>
PROTOTYPE : int ferror(FILE *fp);
            int feof(FILE *fp);
SEMANTICS : La funzione ferror() verifica se vi e' stato un errore nello stream
            'fp'; la funzione feof() verifica se lo stream 'fp' e' giunto alla
            fine del file (EOF).
RETURNS   : true, non zero , se la condizione e' vera,  0 altrimenti
--------------------------------------------------------------------------------
L'oggetto FILE della standard library contiene il flag "End of File", che indica
il raggiungimento della fine del file, e per lo meno anche il flag di errore
indicante appunto un errore nelle operazioni I/O; tali flag sono letti dalle
funzioni su citate. Il valore di EOF corrisponde ad un intero negativo,
generalmente -1.


Le funzioni inerenti l'OUTPUT - Character at a time I/O:
================================================================================
HEADER    : <stdio.h>
PROTOTYPE : int putc(int c, FILE *fp);
            int fputc(int c, FILE *fp);
            int putchar(int c);
SEMANTICS : La funzione putc() stampa il carattere 'c' sullo stream 'fp'; la
            funzione fputc() e' equivalente a putc(); la funzione putchar()
        stampa il carattere 'c'.
RETURNS   : c in caso di successo, EOF in caso di errore
--------------------------------------------------------------------------------
Nota: la funzione putc() puo' essere implementata come una macro.


Le funzioni inerenti l'INPUT - Line at a time I/O:
================================================================================
HEADER    : <stdio.h>
PROTOTYPE : int fgets(char *buf, int size, FILE *fp);
            int gets(char *buf);
SEMANTICS : La funzione fgets() legge al massimo 'size' caratteri dallo stream
            'fp' copiandoli nella stringa 'buf'; la funzione gets() legge la
        riga in input e la scrive in 'buf'.
RETURNS   : buf in caso di successo, NULL o EOF in caso di errore
--------------------------------------------------------------------------------
Nota: La differenza sostanziale tra le due e' che fgets() legge da uno stream
mentre gets() dallo standard input, inoltre la fgets() pone comunque un limite,
mediante il parametro 'size', alla grandezza della riga da leggere, pertanto i
caratteri letti saranno size-1, questo e' fondamentale per la sicurezza; da
ricordare infine che la fgets() pone il carattere null-byte '\0' alla fine della
stringa 'buf'.

Si consiglia di non utilizzare la funzione gets(), se non per scopi didattiti.


Le funzioni inerenti l'OUTPUT - Line at a time I/O:
================================================================================
HEADER    : <stdio.h>
PROTOTYPE : int fputs(char *buf, FILE *fp);
            int puts(char *buf);
SEMANTICS : La funzione fputs() scrive la stringa 'buf' nello sream 'fp'; la
            funzione puts() scrive la stringa 'buf' sullo standard output.
RETURNS   : buf in caso di successo, NULL o EOF in caso di errore
--------------------------------------------------------------------------------
Nota: La stringa stampata in output con la puts() comprende il carattere di
new-line, cosa che non avviene con fputs(); entrambe scrivono una stringa
comprendente il carattere di terminazione nullo (null-byte -  null terminated
string).
*/

int main(int argc, char* argv[])
{
    FILE* fp;
    const char* input_file = "/etc/fstab";
    int c;

    /* Si utilizza la funzione per aprire un file ASCII */
    if ((fp = fopen(input_file, "r")) == NULL) {
        fprintf(stderr, "Err(%d) - %s: %s\n", errno, strerror(errno), input_file);
        exit(EXIT_FAILURE);
    }

    /* Un carattere per volta si visualizza il file input_file */
    while ((c = getc(fp)) != EOF) {
        putc(c, stdout);
    }

    fclose(fp);

    return (EXIT_SUCCESS);
}
