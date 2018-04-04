#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
int main(int argc, char* argv[])
{
    int i;

    if ((i = access("/etc/exports", X_OK)) == -1) {
        perror("access");
        exit(EXIT_FAILURE);
    } else {
        printf("the user have write permission\n");
    }

    return 0;
}
