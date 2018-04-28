#include<stdio.h>
#include<unistd.h>
int main(int argc, char* argv[])
{
    printf("\tuid\tgid\teuid\tegid\n");
    printf("parent\t%d\t%d\t%d\t%d\n", getuid(), getgid(), geteuid(), getegid());

    if (fork() == 0) {
        printf("child\t%d\t%d\t%d\t%d\n", getuid(), getgid(), geteuid(), getegid());
    }

    return 0;
}
