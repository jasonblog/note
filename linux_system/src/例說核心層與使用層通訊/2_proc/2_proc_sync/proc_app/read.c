#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
    int fd;
    char buf[1024];

    for (;;) {
        fd = open("/proc/slndir/slnfile", O_RDWR);

        if (fd < 0) {
            printf("open: %s\n", strerror(errno));
            continue;
        }

        if (read(fd, buf, sizeof(buf)) < 0) {
            printf("read: %s\n", strerror(errno));
            continue;
        }

        close(fd);

        printf("app read: %s\n", buf);
    }

    return 0;
}
