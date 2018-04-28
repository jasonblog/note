#include<unistd.h>
#include<stdio.h>
#include<string.h>
int main(int argc, char* argv[])
{
    int i;
    int fd;
    char* ptr = "helloworld\n";
    fd = atoi(argv[1]);
    i = write(fd, ptr, strlen(ptr));

    if (i <= 0) {
        perror("write");
    }

    close(fd);
}
