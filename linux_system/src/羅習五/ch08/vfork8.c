#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int n_fork=0;
    int pid;
    for (n_fork=0; n_fork<=7; n_fork++) {
        pid=fork();
        if (pid==0) continue;
        else break;
    }
    while(1);
}