#include <stdio.h>
#include <sys/mman.h> //mmap
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shm_ipc.h"


int sln_shm_get(char* shm_file, void** shm, int mem_len)
{
    int         fd;

    fd = open(shm_file, O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        printf("open <%s> failed: %s\n", shm_file, strerror(errno));
        return -1;
    }

    ftruncate(fd, mem_len);

    *shm = mmap(NULL, mem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (MAP_FAILED == *shm) {
        printf("mmap: %s\n", strerror(errno));
        return -1;
    }

    close(fd);

    return 0;
}

int main(int argc, const char* argv[])
{
    char*        shm_buf = NULL;

    sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN);

    printf("ipc client get: %s\n", shm_buf);

    munmap(shm_buf, SHM_IPC_MAX_LEN);
    return 0;
}
