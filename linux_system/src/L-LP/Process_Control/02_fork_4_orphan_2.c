#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t ppid;

    switch (fork()) {
    case -1:
        fprintf(stderr, "Err.(%s) fork() failed\n", strerror(errno));
        exit(EXIT_FAILURE);

    case 0:

        /* Processo Figlio */

        /* Si esegue un loop con un attesa di un secondo per far si che il padre
        possa terminare prima del figlio, in attesa che init adotti il
        processo */
        while ((ppid = getppid()) != 1) {
            printf("(PID %ld) Figlio in esecuzione, il Padre: %ld\n",
                   (long)getpid(), (long)getppid());
            sleep(1);
        }

        printf("Il figlio e' orfano (PPID=%ld)\n", (long)ppid);

        exit(EXIT_SUCCESS);

    default:
        /* Processo padre*/
        printf("(PID %ld) Padre in esecuzione, il Padre: %ld\n",
               (long)getpid(), (long)getppid());

        /* Un po' di scena con qualche linea di output */
        sleep(2);

        exit(EXIT_SUCCESS);
    }
}
