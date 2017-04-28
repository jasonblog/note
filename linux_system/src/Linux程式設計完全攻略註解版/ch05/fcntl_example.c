#include<sys/types.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdio.h>
int main(int argc, char* argv[])
{
    int accmode, val;

    if (argc != 2) {                    //
        printf("the argc must equal to 2");
    }

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0) { //
        perror("fcntl");
        exit(EXIT_FAILURE);
    }

    accmode = val & O_ACCMODE;              //

    if (accmode == O_RDONLY) {
        printf("read only\n");
    } else if (accmode == O_WRONLY) {
        printf("write only\n");
    } else if (accmode == O_RDWR) {
        printf("read write\n");
    } else {
        printf("unknown mode\n");
    }

    if (val & O_APPEND) {
        printf(", append");
    }

    if (val & O_NONBLOCK) {
        printf(", nonblocking");
    }

    return 0;
}


