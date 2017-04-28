#include<stdlib.h>
#include<sys/wait.h>
#include<stdio.h>
int main(int argc, char* argv[])
{
    int status;
    status = system("pwd");

    if (!WIFEXITED(status)) {
        printf("abnormal exit\n");
    } else {
        printf("the exit status is %d\n", status);
    }

    return 0;
}


