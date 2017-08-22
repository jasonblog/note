#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[])
{
    int fd = open(argv[1], O_RDONLY);

    // Eject CDROM
    ioctl(fd, CDROMEJECT);

    close(fd);
    return (EXIT_SUCCESS);
}
