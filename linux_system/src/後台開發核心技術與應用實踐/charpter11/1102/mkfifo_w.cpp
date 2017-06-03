#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#define P_FIFO "/tmp/p_fifo"
int main(int argc, char** argv)
{
    int fd;

    if (argc < 2) {
        printf("please input the write data.\n");
    }

    fd = open(P_FIFO, O_WRONLY | O_NONBLOCK); /*非阻塞方式*/
    write(fd, argv[1], 100); /*将argv[1]写道fd里面去*/
    close(fd);
    return 0;
}
