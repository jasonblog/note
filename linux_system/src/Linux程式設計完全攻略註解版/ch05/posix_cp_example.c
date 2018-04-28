#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>

int main(int argc, char* argv[])
{
    int fd_src, fd_des;
    char buf[128];
    int num;

    if (argc != 3) {        //
        printf("the format must be: posix_cp_example file_src file_des\n");
        exit(EXIT_FAILURE);
    }

    if ((fd_src = open(argv[1], O_RDONLY)) == -1) { //
        perror("open1");
        exit(EXIT_FAILURE);
    }

    if ((fd_des = open(argv[2], O_CREAT | O_EXCL | O_WRONLY, 0644)) == -1) {
        //fd=src=open(argv[2],O_WRONLY|O_TRUNC)
        perror("open2");
        exit(EXIT_FAILURE);
    }

    do {
        num = read(fd_src, buf, 128); //
        write(fd_des, buf, num);    //
    } while (num == 128);        //

    close(fd_src);
    close(fd_des);
}


