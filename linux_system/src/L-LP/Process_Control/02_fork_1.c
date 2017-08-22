#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/*
Un processo esistente puo' creare un nuovo processo invocando la system call
fork(), il nuovo processo creato dalla fork() e' chiamato "Child Process"
(Processo figlio), mentre il processo generatore e' chiamato "Parent Process"
(Processo padre); la fork() e' chiamata una volta ma ritorna due volte, una
nel padre e una nel figlio, infatti il PID del figlio e' assegnato al padre
mentre il PID del figlio assume valore 0.

Perche' la fork() ritorna due volte? Un processo puo' avere uno o piu' figli, il
valore di ritorno della fork() e' l'unico modo per individuare l'ultimo 'nato';
un figlio invece ha sempre un solo padre, individuabile mediante getppid(), per
cui gli si assegna pid 0, che nessun altro processo puo' avere.

HEADER    : <unistd.h>
PROTOTYPE : pid_t fork(void);
SEMANTICS : La funzione fork() crea un processo duplicando il processo chiamante
            , il nuovo processo e' chiamato processo figlio, mentre il processo
        chiamante e' il processo genitore.
RETURNS   : 0 nel figlio e il PID del figlio nel padre in caso di successo, -1
            in caso di errore
--------------------------------------------------------------------------------
Il Child Process e' una copia del data segment, dello stack e dello heap del
Parent Process, tale copia e' ad uso e consumo del Child Process e pertanto non
vi e' condivisione col Parent Process; la sezione text segment invece e'
condivisa tra Parent Process e Child Process, per gli altri segmenti invece
il kernel Linux utilizza la tecnica del "copy on write", ossia ogni pagina
viene realmente ed effettivamente copiata per il nuovo processo se e solo se
tale pagina e' stata scritta.

Il Sistema Operativo assegna due porzioni di memoria private, una per il
Parent Process, l'altra per il Child Process, pertanto qualora uno dei due
processi dovesse modificare una *propria* variabile, tale modifica non
sarebbe visibile ad entrambi; i file aperti invece sono condivisi, per cui
una eventuale modifica ad un file sarebbe ad entrambi visibile.

L'implementazione della SystemCall fork() puo' essere diversa a seconda del
sistema UNIX utilizzato; l'header da utilizzare e' <unistd.h>.

pid_t e' un tipo di dato definito in <sys/types.h>, sarebbe stato lo stesso
se si fosse utilizzato un altro tipo di dato intero, tuttavia e' sempre
consigliabile utilizzare i tipi proposti per una maggiore portabilita'.

Condivisione dei file (File sharing):
I file descriptors aperti nel padre sono duplicati nel figlio, come se fosse
utilizzata la funzione dup()[1], infatti essi condividono sia la file table,
sia la posizione (offset) all'interno file stesso, che peraltro e' di
fondamentale importanza qualora due o piu' processi volessero scrivere sul
medesimo file. Tuttavia e' estremamente importante ricordare che se padre e
figlio volessero interagire con lo stesso file descriptor e senza
nessun meccanismo di sincronizzazione, il risultato sarebbe un mix delle due
scritture sul file, e quindi  poco coerente; pertanto, sebbene possibile,
non e' certamente il metodo piu' adatto.

Vi sono solo due casi in cui e' possibile gestire file despritors dopo una
chiamata a fork():

1 - Il padre aspetta che il figlio completi la propria esecuzione;
2 - Il padre ed il figlio proseguono indipendentemente l'un l'altro.

Implementazione:
Il kernel conserva una lista di tutti i processi attivi in una tabella, detta
"process table", ciascuna voce all'interno di questa tabella e' definita dalla
struttura "task_struct" implementata nell'header <linux/sched.h>, tale struttura
gestisce diverse informazioni, dallo stato del processo alla priorita', dal PID
alla policy e via discorrendo, in pratica tutto cio' che riguarda un processo.

Quale' il compito dello scheduler?
Non vi e' certezza se sara' eseguito prima il padre o prima il figlio, questo
dipende dal comportamento dello schedulatore dei processi; e' lo scheduler che
decide, secondo specifici algoritmi, basati anche sulla priorita', quale
processo debba essere messo in esecuzione - e' come se fosse un vigile urbano,
e' lui che dirige il traffico -; viene eseguito quando:
- Si invoca una System Call;
- Si verifica un Interruput;
- Si chiama esplicitamente;
- altre circostanze.

*/

int main(int argc, char* argv[])
{
    pid_t pid;

    /* Si crea un processo figlio */
    pid = fork();

    switch (pid) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:
        printf("(PID %ld) Figlio in esecuzione, il Padre: %ld - pid=%ld\n",
               (long)getpid(), (long)getppid(), (long)pid);
        exit(EXIT_SUCCESS);

    default:
        printf("(PID %ld) Padre  in esecuzione, il Padre: %ld - pid=%ld\n",
               (long)getpid(), (long)getppid(), (long)pid);
        sleep(1);
        exit(EXIT_SUCCESS);
    }


    return (EXIT_SUCCESS);
}

/*
[1] Files-and-Directories: 13_duplicate_file_descriptor.c
*/
