#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;
    if ((pid = fork()) == 0) {
	    printf("child's pid = %d\n", getpid());
    } else {
	    printf("parent's pid = %d\n", getpid());
    }
    printf("breakpoint\n");
    if (pid ==0)
        system("touch child");
    return 0;
}
