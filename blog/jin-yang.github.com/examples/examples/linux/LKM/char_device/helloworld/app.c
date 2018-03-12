#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int dev;

    if ((dev = open("/dev/mychar", O_RDWR|O_NDELAY)) < 0) {
        perror("Open /dev/mychar");
        return -1;
    }

    read(dev, (void*)0x30, 0x31);
    write(dev, (void*)0x41, 0x42);
    close(dev);
    return 0;
}
