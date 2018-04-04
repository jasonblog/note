#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
char buf1[] = "1234567890";         //
char buf2[] = "ABCDEFGHIJ";         //
char buf3[] = "abcdefghij";         //
int main(int argc, char* argv[])
{
    int fd;

    if ((fd = creat("/tmp/test", 0644)) < 0) {           //
        perror("create");
        exit(EXIT_FAILURE);
    }

    if (write(fd, buf1, 10) != 10) {                //
        perror("write");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, 20, SEEK_SET) == -1) {            //
        perror("lseek");
        exit(EXIT_FAILURE);
    }

    if (write(fd, buf2, 10) != 10) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    if (lseek(fd, 10, SEEK_SET) == -1) {            //
        perror("lseek");
        exit(EXIT_FAILURE);
    }

    if (write(fd, buf3, 10) != 10) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    return 0;
}


