#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
void myHandler(int sig);
int main(int argc, char* argv[])
{
    struct sigaction act, oact;

    act.sa_handler = myHandler;
    sigemptyset(&act.sa_mask); /*initial. to empty mask*/
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, &oact);

    while (1) {
        printf("Hello world.\n");
        pause();
    }
}

void myHandler(int sig)
{
    printf("I got signal: %d.\n", sig);
}
// to end program, <Ctrl + \> to generate SIGQUIT



