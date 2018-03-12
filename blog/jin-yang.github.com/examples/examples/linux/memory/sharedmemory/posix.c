/* gcc -o shmopen shmopen.c -lrt*/
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#define MAP_SIZE 68157440

int main(int argc, char *argv[])
{
    int fd;
    void* result;

    fd = shm_open("/shm1", O_RDWR|O_CREAT, 0644);
    if (fd < 0) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(fd, MAP_SIZE) < 0){
        printf("ftruncate failed\n");
        exit(1);
    }

    result = mmap(NULL, MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(result == MAP_FAILED){
        printf("mapped failed\n");
        exit(1);
    }

    printf("memset\n");
    memset(result, 0, MAP_SIZE);

    getchar();

    shm_unlink("/shm1");
    return 0;
}
