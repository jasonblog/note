/*
usage: ./NoZombie 10000
*/
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void manyChild(int num) {
    int i, pid;
    for (int i=0; i<num; i++) {
        pid = vfork();
        if (pid == 0) exit(0);
        if (pid != 0) continue;
    }
}

int main(int argc, char** argv) {
    int pid, num;
    sscanf(argv[1], "%d", &num);
    pid = fork();
    if (pid == 0) {
        manyChild(num);
        exit(0);
    }
    getchar();
}