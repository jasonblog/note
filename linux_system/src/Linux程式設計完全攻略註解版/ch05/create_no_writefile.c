#include<stdio.h>
#include<fcntl.h>

int main(int argc, char* argv[])
{
    int fd;
    fd = open(argv[1], O_CREAT | O_WRONLY, 0000);
    write(fd, "test message!\n", 14);
    close(fd);
}


