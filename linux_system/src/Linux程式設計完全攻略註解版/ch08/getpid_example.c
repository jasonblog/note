#include<stdio.h>
#include<unistd.h>
int main(int argc, char* argv[])
{
    printf("the current program's pid is %d\n", getpid());
    return 0;
}
