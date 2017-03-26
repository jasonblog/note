#include <stdio.h>
#include <sys/mman.h> //mmap
#include <errno.h>
#include <unistd.h> //sysconf()

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shm_ipc.h"


int sln_shm_get(char* shm_file, void** shm, int mem_len)
{
    int         fd;

    fd = shm_open(shm_file, O_RDWR, 0);

    if (fd < 0) {
        printf("shm_open <%s> failed: %s\n", shm_file, strerror(errno));
        return -1;
    }

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
    int         i;
    int         pagesize = sysconf(_SC_PAGESIZE);

    if (sln_shm_get(SHM_IPC_FILENAME, (void**)&shm_buf, SHM_IPC_MAX_LEN) < 0) {
        return -1;
    }

    printf("ipc client get: %s\n", shm_buf);

    /*
    printf("Share memory address: 0x%08X, share size: %d\n",
            shm_buf, SHM_IPC_MAX_LEN);

    for (i = 0; i < 20; i++) {
        printf("Try to access address: 08%08X - (offset: %d)\n",
                shm_buf + pagesize * i + 1,  pagesize * i + 1);
        *(shm_buf + pagesize * i + 1) = '\0';
        printf("OK!\n");
    }
    */


    munmap(shm_buf, SHM_IPC_MAX_LEN);
    //shm_unlink(SHM_IPC_FILENAME);

    return 0;
}
