#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#define SHR_MEMSIZE             4096
#define MEM_CLEAR               0x0
#define MEM_RESET               0x1
#define MEM_DEV_FILENAME       "/dev/sln_memdev"

int main()
{
    int         fd;
    char*        shm = NULL;

    fd = open(MEM_DEV_FILENAME, O_RDWR);

    if (fd < 0) {
        printf("open(): %s\n", strerror(errno));
        return -1;
    }

    shm = mmap(NULL, SHR_MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (MAP_FAILED == shm) {
        printf("mmap: %s\n", strerror(errno));
    }

    printf("Before Write, shm = %s\n", shm);

    strcpy(shm, "User write to share memory!");

    printf("After write, shm = %s\n", shm);

    if (0 > ioctl(fd, MEM_CLEAR, NULL)) {
        printf("ioctl: %s\n", strerror(errno));
        return -1;
    }

    printf("After clear, shm = %s\n", shm);

    if (0 > ioctl(fd, MEM_RESET, NULL)) {
        printf("ioctl: %s\n", strerror(errno));
        return -1;
    }

    printf("After reset, shm = %s\n", shm);

    munmap(shm, SHR_MEMSIZE);
    close(fd);
    return 0;
}
