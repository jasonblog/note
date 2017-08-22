#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int glob_var = 10;
char buf[] = "scrivo su stdout\n";

/* L'obiettivo del programma e' di far notare che le modifiche delle variabili
nel child process non hanno effetto sulle variabili del parent process  */

int main(int argc, char* argv[], char* envp[])
{
    int var;
    pid_t pid;
    var = 100;

    if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1) {
        fprintf(stderr, "Err.(%d) - I/O - %s\n", errno, strerror(errno));
    }

    printf("Prima della fork()\n");

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Err.(%d) - fork - %s\n", errno, strerror(errno));
    } else if (pid == 0) {
        /* Child Process */
        glob_var++;
        var++;
    } else
        /* Parent Process
        Si mette in attesa per due secondi, dando modo al figlio di essere
        eseguito */
    {
        sleep(2);
    }

    /*
     Da notare che in realta' non si sa se Child Process sara' eseguito prima del
     Parent Process o viceversa, l'esecuzione di ciascun processo infatti e'
     gestita dall'algoritmo di scheduling implementato nel kernel.
    */

    printf("pid = %d, glob_var = %d, var = %d\n", getpid(), glob_var, var);

    return (EXIT_SUCCESS);
}
/*

Si testa il programma con la redirezione dell'ouptut verso un file:

$ ./a.out > test.txt
$ cat test.txt
scrivo su stdout
Prima della fork()
pid = 1396, glob_var = 11, var = 101
Prima della fork()
pid = 1395, glob_var = 10, var = 100

Senza redirezione:

$ ./a.out
scrivo su stdout
Prima della fork()
pid = 1477, glob_var = 11, var = 101
pid = 1476, glob_var = 10, var = 101


Nel primo caso il processo figlio, poiche' ottiene una duplicazione della
memoria del processo padre, riceve anche il contenuto del buffer I/O; la
printf() prima della funzione fork() e' chiamata una volta ma il contenuto della
linea resta nel buffer e copiato anche per il processo figlio.

Si potrebbe evitare la doppia scrittura inserendo fflush(stdout) alla riga 22.
*/
