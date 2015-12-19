#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define MEGABYTE 1024*1024


void signal_handler(int signum)
{
    printf("signal %d \n", signum);
    if (signum == SIGUSR2) {
        printf("oom_test SIGUSR2\n");
    } else if (signum == SIGUSR1) {
        printf("SIGUSR1\n");
    }
}

int main(int argc, char* argv[])
{
    void* myblock = NULL;
    int count = 0;
    signal(SIGUSR2, signal_handler);

    while (1) {
        myblock = (void*) malloc(MEGABYTE);

        if (!myblock) {
            break;
        }

        //sleep(1);

        memset(myblock, 1, MEGABYTE);
        printf("11 Currently allocating %d MB\n", ++count);
    }

    exit(0);
}

