#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_NAME "/tmp/my_fifo"

int main(int argc, char* argv[])
{
    int pipe_fd;
    int res;
    char buffer[4096];
    int bytes_read = 0;

    memset(buffer, '\0', sizeof(buffer));

    printf("Process %d opening FIFO O_RDONLY\n", getpid());
    pipe_fd = open(FIFO_NAME, O_RDONLY);
    printf("the file's descriptor is %d\n", pipe_fd);

    if (pipe_fd != -1) {
        bytes_read = read(pipe_fd, buffer, sizeof(buffer));
        printf("the read data is %s\n", buffer);
        close(pipe_fd);
    } else {
        exit(EXIT_FAILURE);
    }

    printf("Process %d finished, %d bytes read\n", getpid(), bytes_read);
    exit(EXIT_SUCCESS);
}
