#include <stdio.h>
#include <unistd.h>
#include <signal.h>
void sig_usr(int sig);

int main(int argc, char* argv[])
{
    int i = 0;

    if (signal(SIGUSR1, sig_usr) == SIG_ERR) {
        printf("Cannot catch SIGUSR1\n");
    }

    if (signal(SIGUSR2, sig_usr) == SIG_ERR) {
        printf("Cannot catch SIGUSR2\n");
    }

    while (1) {
        printf("%2d\n", i);
        pause();
        /* pause until signal handler
           has processed signal */
        i++;
    }

    return 0;
}

void sig_usr(int sig)
{
    if (sig == SIGUSR1) {
        printf("Received SIGUSR1\n");
    } else if (sig == SIGUSR2) {
        printf("Received SIGUSR2\n");
    } else {
        printf("Undeclared signal %d\n", sig);
    }
}
