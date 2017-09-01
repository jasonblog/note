#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/* Funzione getopt() - Parsing delle opzioni - Header <getopt.h>
----------------------------------------------------------------
Un programma  ha la possibilita' di ottenere argomenti ed opzioni allorquando
viene eseguito, il passaggio degli argomenti viene effettuato mediante i
parametri di default argc e argv[], della funzione main().

extern char *optarg;
--------------------
Il puntatore optarg referenzia l'indirizzo all'interno del quale e' collocato
il parametro relativo all'opzione in fase di elaborazione.

extern int optind;
------------------
La funzione getopt() incrementa il valore di optind di volta in volta che
sono elaborate tutte le opzioni presenti nelle successive chiamate; di
default optind assume il valore di 1, per cui il primo parametro ad essere
elaborato sara' argv[1], dopodiche', elaborate le restanti opzioni, optind
conterra' l'indice del successivo parametro.

extern int opterr;
------------------
La variabile esterna opterr assume anch'essa il valore di default 1 (true)
ed e' utilizzata come dato di input per la funzione getopt(). Se opterr e'
uguale a true sara' elaborato il carattere di opzione non valido e la
funzione getopt() fara' stampare su stderr un messaggio con l'opzione
corretta. Se invcece la variabile opterr risulta falsa, ossia impostata a 0,
non ci saranno messaggi di errore, pur avendo elaborato un'opzione non
valida. In tal modo si puo' controllare in maniera molto efficiente se
inserire un messaggio di errore e su quale stream inviarlo, invece impostando
opterr a 0 non saranno inviati messaggi d'errore.

extern int optopt;
-------------------
La variabile esterna optopt contiene il carattere dell'opzione non
riconosciuta.

extern int getopt(int argc, char *const *argv, const char *shortopts);
----------------------------------------------------------------------
Il primo parametro argc contiene il numero totale dei parametri contenuti in
argv[]; il secondo parametro argv e' un array di puntatori a stringa, contiene
le opzioni ed i parametri inseriti; il terzo ed ultimo parametro shortopts e'
un puntatore a stringa, che in un certo senso gestisce l'intera operazione
della getopt(), poiche' determina quali sono le opzioni valide e quali
accettano parametri propri. Esemplificando si potrebbe affermare che shortopts
contiene l'interfaccia del programma.

Come lavora la funzione getopt()
-----------------------------------------------------------------------------
Le opzioni solitamente sono precedute dal carattere '-', esso stesso e'
considerato un'opzione se inserito dopo '-', le opzioni sono formate da un
singolo carattere precedute da '-' e possono avere o meno un parametro.

La funzione getopt() prende le due variabili argc ed argv come argomenti
della funzione main(), inoltre lavora con una stringa per verificare quali
sono le opzioni valide; la getopt() effettua una sorta di scansione nella
lista degli argomenti (argv) cercando ogni stringa che cominci con '-',
shortopts indica alla getopt() quali sono le opzioni accettabili, da notare
che se un opzione deve ricevere un parametro, al carattere deve seguire il
simbolo ":".

Se il carattere non e' presente nella lista delle opzioni shortopts, non e'
valida e la getopt() restituisce '?'; se invece l'opzione e' presente nella
stringa shortopts, la getopt() verifica se questa opzione accetta o meno un
parametro, controllando la presenza del simbolo ':' dopo il carattere stesso,
in shortopts.

L'uso della getopt() appare ora piuttosto evidente, tutto sta nel richiamarla
in un ciclo fino a quando non restiuisce il valore -1, che identifica il caso
in cui non ci sono piu' opzioni.

Se non si inserisse un parametro a un'opzione che lo richieda, la funzione
getopt() restituirebbe ':'.
*/

int main(int argc, char* argv[])
{
    /* Si disabilita la stampa di errori sullo stdout per le opzioni che non
    sono state rinosciute */
    opterr = 0;
    int opt;

    // La stringa contenente le opzioni, una con parametro
    static char shortopts[] = "hU:v";

    while ((opt = getopt(argc, argv, shortopts)) != -1)
        switch (opt) {
        case 'h':
            puts("Processata l'opzione -h");
            printf("optind = %d - argc = %d\n", optind, argc);
            break;

        case 'v':
            puts("Processata l'opzione -v");
            printf("optind = %d - argc = %d\n", optind, argc);
            break;

        case 'U':
            printf("Opzione -U \"%s\" parametro \n", optarg);
            break;

        case '?':
            printf("Parametro non valido \"?\"\n");
            break;

        default:
            puts("Opzione sconosciuta");
        }

    for (; optind < argc; ++optind) {
        printf("argv[%d] = %s\n", optind, argv[optind]);
        printf("optind = %d - argc = %d\n", optind, argc);
    }

    return (EXIT_SUCCESS);
}
