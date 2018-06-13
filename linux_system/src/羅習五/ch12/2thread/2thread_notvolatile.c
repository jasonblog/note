#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

int prt = 0;
void sig_handler(int signum) {
    prt = 1;
}

int main(int arg, char** argv) {
    signal(SIGINT, sig_handler);
    int vol=0;
    while(1) {
        vol=vol+1;
        if (prt==1) {
            printf("%d\n", vol);
            prt = 0;
        }
    }
}
