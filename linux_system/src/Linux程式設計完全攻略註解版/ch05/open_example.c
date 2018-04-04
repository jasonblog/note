#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
    int fd_open, fd_open_create, fd_create;

    if ((fd_open = open("/bin/ls", O_RDONLY)) == -1) { //
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("the file's descriptor is:%d\n", fd_open);

    if ((fd_open_create = open("./tmp1", O_CREAT | O_EXCL, 0644)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("the tmp1 file descriptor is:%d\n", fd_open_create);

    if ((fd_create = creat("./tmp2", 0644)) == -1) {    //
        perror("create");
        exit(EXIT_FAILURE);
    }

    printf("the tmp2 file descriptor is:%d\n", fd_create);

    close(fd_open);                         //
    close(fd_create);                       //
    close(fd_open_create);                      //
    return 0;
}


