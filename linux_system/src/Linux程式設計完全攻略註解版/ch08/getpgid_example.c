#include<stdio.h>
#include<unistd.h>
int main(int argc, char* argv[])
{
    int i;
    printf("\tpid\t ppid \t pgid\n");
    printf("parent\t%d\t%d\t%d\n", getpid(), getppid(), getpgid(0));

    for (i = 0; i < 2; i++)
        if (fork() == 0) {
            printf("child\t%d\t%d\t%d\n", getpid(), getppid(), getpgid(0));
        }

    return 0;
}
