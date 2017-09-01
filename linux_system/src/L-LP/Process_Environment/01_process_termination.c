#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
Un processo termina normalmente:
1 - Con una chiamata a return() dal main();
2 - Con una chiamata ad exit();
3 - Con una chiamata ad _exit() oppure all'equivalente _Exit();
4 - Con una chiamata a return() dalla routine dell'ultimo thread nel processo;
5 - Con una chiamata a pthread_exit() dalla routine dell'ultimo thread nel
    processo.

Un processo termina in maniera anomala:
1 - Con una chiamata ad abort();
2 - Con la ricezione di un segnale;
3 - Un thread che risponde alla richiesta di cancellazione.

La funzione _exit() o _Exit() restituisce immediatamente il controllo al kernel,
la funzione exit() invece esegue prima talune operazioni di pulizia, come ad
esempio la chiusura di tutti gli stream, dopodiche' anch'essa restituisce il
controllo al kernel chiamando la funzione _exit().

L'argomento di ciascuna funzione indica lo "stato di uscita" del processo,
solitamente 0 corrisponde a true, 1 a false; la shell prevede il comando
"echo $?" che stampa sullo standard output lo stato di uscita dell'ultimo
processo eseguito.

Exit Status vs Termination Status:
Quando un processo termina la propria esecuzione in maniera normale deve
notificare (al padre) in che modo e' avvenuta la terminazione, le funzioni
exit(), _exit() ed _Exit() lo fanno passando un "exit status" come argomento
alla funzione; se tuttavia il processo termina in maniera anomala non puo'
inviare nessun "exit status", in questo caso e' il kernel stesso che entra in
gioco, generando un "termination status" indicante le ragioni della uscita
anomala. In ogni caso, il processo padre puo' ottenere il "termination status"
o da wait() o da waitpid().

Nota: La funzione main() e' una sorta di chiave di accensione per ciascun
programma, senza di essa infatti non ci sarebbe esecuzione; una routine di avvio
e' chiamata poco prima della funzione main(), il cui indirizzo e' il punto di
partenza dell'eseguibile. All'interno del main() le funzioni exit(0) e return(0)
sono del tutto equivalenti.

Talvolta e' estremamente comodo ma anche necessario eseguire diverse operazioni
di pulizia prima della chiusura di un programma, ed e' ancor meglio se tali
operazioni fosse il programmatore stesso a gestirle; la funzione atexit() ad
esempio e' davvero utile in tal senso.

HEADER    : <stdlib.h>
PROTOTYPE : int atexit(void (*func)(void);
SEMANTICS : Registra la funzione 'func' da eseguire all'uscita del programma
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Nota: L'argomento richiede l'indirizzo di una funzione, senza argomenti e senza
valore di ritorno, del tipo void func(void);

La funzione atexit() consente la registrazione di una funzione di pulizia da
usarsi all'uscita del programma; nella sequenza di chiusura tutte le
informazioni registrate saranno chiamate in ordine inverso rispetto alla
registrazione.
*/

static void my_exit_func_1(void);
static void my_exit_func_2(void);
static void my_exit_func_3(void);

int main(int argc, char* argv[])
{

    if (atexit(my_exit_func_1) != 0) {
        fprintf(stderr, "Err: exit 1\n");
        exit(EXIT_FAILURE);
    }

    if (atexit(my_exit_func_2) != 0) {
        fprintf(stderr, "Err: exit 2\n");
        exit(EXIT_FAILURE);
    }

    if (atexit(my_exit_func_3) != 0) {
        fprintf(stderr, "Err: exit 3\n");
        exit(EXIT_FAILURE);
    }

    printf("Da notare le chiamate in reverse-order\n");

    return (EXIT_SUCCESS);
}

static void my_exit_func_1(void)
{
    printf("my_exit_func_1\n");
}

static void my_exit_func_2(void)
{
    printf("my_exit_func_2\n");
}


static void my_exit_func_3(void)
{
    printf("my_exit_func_3\n");
}
