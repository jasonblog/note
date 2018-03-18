#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
int main(int argc, char* argv[])
{
    char* ptr = "helloworld\n";
    int fd_open, fd_dup;

    if ((fd_open = open("tmp1", O_WRONLY | O_CREAT, 0644)) == -1) { //
        perror("open");
        exit(EXIT_FAILURE);
    }

    fd_dup = fcntl(fd_open, F_DUPFD);           //
    printf("fd_open=%d,fd_dup=%d\n", fd_open, fd_dup);
    printf("write %d bytes to fd_open\n", write(fd_open, ptr, strlen(ptr))); //
    printf("write %d bytes to fd_dup\n", write(fd_dup, ptr, strlen(ptr))); //
    close(fd_open);
    close(fd_dup);
    printf("now the content of file:\n");           //
    system("cat tmp1");
    return 0;
}


