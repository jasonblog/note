/*
usage:
1st terminal 
$ ./zombie 10000

2nd terminal
$ ps -a
*/
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int i, num, pid, wstatus;
    sscanf(argv[1], "%d", &num);
    for (int i=0; i<num; i++) {
        pid = vfork();
        if (pid==0) exit(0);
        if (pid != 0) continue;
    }
    if (pid != 0)
        getchar();
}