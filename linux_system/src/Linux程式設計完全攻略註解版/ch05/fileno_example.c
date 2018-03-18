#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(int argc, char* argv[])
{
    int fp1, fp2;
    printf("stdin is:\t%d\n", fileno(stdin));           //
    printf("stdout is:\t%d\n", fileno(stdout));         //
    printf("stderr is:\t%d\n", fileno(stderr));         //

    if ((fp1 = open("/etc/xinetd.d/cvs", O_WRONLY)) == -1) { //
        //
        perror("open");
        exit(EXIT_FAILURE);
    }

    if ((fp2 = open("/etc/xinetd.d/kshell", O_WRONLY)) == -1) { //
        //
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("cvs file is :\t%d\n", fp1);                 //
    printf("kshell file is:\t%d\n", fp2);                   //
    close(fp1);
    close(fp2);
    return 0;
}


