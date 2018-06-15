/*
usage debugFork1
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    if ((pid = fork()) == 0) {
	    printf("child");
    } else {
	    printf("child's pid = %d\n", pid);
	    printf("parent");
    }
    printf("end\n");
    return 0;
}
