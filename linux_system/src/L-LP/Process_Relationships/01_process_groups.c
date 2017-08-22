#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/*
Sui sistemi unix-like i processi, oltre ad avere il proprio PID, possono far
parte di uno o piu' gruppi, e pertanto essere identificati con il relativo
gruppo di appartenenza; un gruppo di processi (Process Groups) e' un insieme di
uno o piu' processi generalmente associati al medesimo job che puo' ricevere
segnali dallo stesso terminale.

Ad ogni gruppo di processi e' associato un identificativo univoco, che nella
sostanza e' come il PID, infatti si usa il medesimo tipo di dato pid_t, ossia un
un intero positivo, il Process Group-ID o PGID.

HEADER    : <unistd.h>
PROTOTYPE : pid_t getpgrp(void);
            pid_t getpgid(pid_t pid);
SEMANTICS : La funzione getpgrp() restituisce il PGID del processo chiamante;
            la funzione getpgid() restituisce il PGID del processo indicato da
            'pid'.
RETURNS   : Il PGID in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Nota: getpgrp() e' equivalente a getpgid(0).

Ciascun gruppo di processi puo' avere un processo leader, una sorta di capo
famiglia, il PGID del quale corrisponde al proprio PID; solitamente il primo
processo ad entrare nel gruppo diviene anche il 'process group leader'.

Il process group leader puo' sia creare nuovi gruppi, sia creare nuovi processi
all'interno del gruppo, che continueranno ad esistere anche qualora il process
group leader dovesse terminare; il periodo di vita di un gruppo di processi
ha inizio alla creazione del gruppo e termina solo quando l'ultimo processo
del gruppo porta a termine la propria esecuzione (process group lifetime).

L'ultimo processo di un gruppo di processi ha due alternative:
- terminare;
- entrare in un nuovo gruppo di processi.

La funzione deputata a far si che un processo possa creare un nuovo gruppo
di processi o che possa entrare in un gruppo di processi esistente e' setpgid(),
tuttavia e' bene ricordare che un processo puo' settare solo il PGID proprio e
dei processi figli, nell'ultimo caso solo dopo una chiamata ad una delle
funzioni della famiglia exec.

HEADER    : <unistd.h>
PROTOTYPE : pid_t setpgid(pid_t pid, pid_t pgid);
SEMANTICS : La funzione setpgid() assegna al processo 'pid' il gruppo di
            processi 'pgid'.
RETURNS   : 0 in caso di successo, -1 in caso di errore
--------------------------------------------------------------------------------
Per cio' che concerne la funzione setpgid(), vi sono delle regole specifiche:
- setta il PGID a 'pgid' del processo il cui PID e' uguale a 'pid';
- Se 'pgid' e' uguale a 'pid' , il processo indicato da 'pid' diventa il process
  group leader;
- se 'pid' e' uguale a 0 si usa il PID del processo chiamante;
- se 'pgid' e' uguale a 0, il PID usato da 'pid' e' usato come PGID.
*/

void* err(char* str_err);
void get_process_info(pid_t pgid);

int main(int argc, char* argv[])
{
    pid_t pid;
    pid_t pgid;

    switch (pid = fork()) {
    case -1:
        err("fork() failed");

    case 0:
        if ((pgid = getpgrp()) == -1) {
            err("getpgrp() failed");
        }

        /* Si estraggono PID, PPID e PGID del 1° processo figlio */
        get_process_info(pgid);

        if ((pid = fork()) < 0) {
            err("fork() failed");
        } else if (pid == 0) {
            /* Si assegna il gruppo pgid al nuovo processo; in realta'
               ci sarebbe andato lo stesso */
            if (setpgid(pid, pgid) < 0) {
                err("setpgid() failed");
            }

            /* Si estraggono PID, PPID e PGID del 2° processo figlio */
            get_process_info(pgid);
        } else {
            waitpid(pid, NULL, 0);
            exit(EXIT_SUCCESS);
        }

        exit(EXIT_SUCCESS);

    default:
        waitpid(pid, NULL, 0);
    }

    return (EXIT_SUCCESS);
}

void* err(char* str_err)
{
    fprintf(stderr, "Err.(%s) - %s\n", strerror(errno), str_err);
    exit(EXIT_FAILURE);
}

void get_process_info(pid_t pgid)
{
    printf("\n             PID: %ld\n", (long)getpid());
    printf("            PPID: %ld\n", (long)getppid());
    printf("Process Group-ID: %ld +\n", (long)pgid);
}
