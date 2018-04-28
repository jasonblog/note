#include<stdio.h>
#include<unistd.h>
int main(int argc, char* argv[])
{
    printf("the current program's ppid is %d\n", getppid());
    return 0;
}
