#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#define SHR_MEMSIZE           4096
#define MEM_CLEAR               0x0
#define MEM_RESET               0x1
#define CHAR_DEV_FILENAME       "/dev/sln_chardev"

int main()
{
    int     fd;
    char    shm[SHR_MEMSIZE];

    /* open device */
    fd = open(CHAR_DEV_FILENAME, O_RDWR);

    if (fd < 0) {
        printf("open <%s> failed!\n", CHAR_DEV_FILENAME);
        return -1;
    }

    /* set shmfer data, which will be stored into device */
    snprintf(shm, sizeof(shm), "this data is writed by user!");

    /* write data */
    printf("======== Write data========\n");

    if (write(fd, shm, strlen(shm)) < 0) {
        printf("write(): %s\n", strerror(errno));
        return -1;
    }

    /* read data */
    printf("======== Read data========\n");

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("llseek(): %s\n", strerror(errno));
        return -1;
    }

    if (read(fd, shm, SHR_MEMSIZE) < 0) {
        printf("read(): %s\n", strerror(errno));
        return -1;
    }

    printf("read data: %s\n", shm);

    /* clear all data to zero, read it and check whether it is ok */
    printf("========= Clear it now: =======\n");

    if (ioctl(fd, MEM_CLEAR, NULL) < 0) {
        printf("ioctl(): %s\n", strerror(errno));
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("llseek(): %s\n", strerror(errno));
        return -1;
    }

    if (read(fd, shm, SHR_MEMSIZE) < 0) {
        printf("read(): %s\n", strerror(errno));
        return -1;
    }

    printf("read data: %s\n", shm);


    /* reset all data, read it and check whether it is ok */
    printf("========= Reset it now: =======\n");

    if (ioctl(fd, MEM_RESET, NULL) < 0) {
        printf("ioctl(): %s\n", strerror(errno));
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        printf("llseek(): %s\n", strerror(errno));
        return -1;
    }

    if (read(fd, shm, SHR_MEMSIZE) < 0) {
        printf("read(): %s\n", strerror(errno));
        return -1;
    }

    printf("read data: %s\n", shm);

    close(fd);
    return 0;
}
