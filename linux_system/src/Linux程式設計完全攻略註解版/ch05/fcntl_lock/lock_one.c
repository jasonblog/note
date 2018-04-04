#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
struct flock* file_lock(short type, short whence)
{
    static struct flock ret;
    ret.l_type = type ;
    ret.l_start = 0;
    ret.l_whence = whence;
    ret.l_len = 0;
    ret.l_pid = getpid();
    return &ret;
}

int main(int argc, char* argv[])
{
    int fd = open(argv[1], O_WRONLY | O_APPEND);
    int i;
    time_t now;

    for (i = 0; i < 1000; ++i) {
        fcntl(fd, F_SETLKW, file_lock(F_WRLCK, SEEK_SET));
        time(&now);
        printf("%s\t%s F_SETLKW  lock file %s for 5s\n", ctime(&now), argv[0], argv[1]);
        char buf[1024] = {0};
        sprintf(buf, "hello world %d\n", i);
        int len = strlen(buf);

        if (write(fd, buf, len)) {
            printf("%s\t%s write file sccess\n", ctime(&now), argv[0], argv[1]);
        }

        sleep(5);
        fcntl(fd, F_SETLKW, file_lock(F_UNLCK, SEEK_SET));
        sleep(1);
    }

    close(fd);
}


