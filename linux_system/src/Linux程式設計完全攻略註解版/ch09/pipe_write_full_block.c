#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

// O_NONBLOCK disable： write調用阻塞，直到有進程讀走數據

int main(void)
{
    int fds[2];

    if (pipe(fds) == -1) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    int ret;
    int count = 0;

    while (1) {
        ret = write(fds[1], "A", 1); //fds[1]默认是阻塞模式

        if (ret == -1) {
            perror("write error");
            break;
        }

        count++;
    }

    return 0;
}
