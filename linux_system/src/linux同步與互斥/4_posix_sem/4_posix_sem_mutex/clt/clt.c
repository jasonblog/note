#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>

#include "shmipc.h"


int nms_shm_get(char* shm_file, void** shm, int mem_len)
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
    sem_t*               sem_mutex;
    char*                str = NULL;

    SLN_MUTEX_SHM_LOCK(SEM_MUTEX_FILE, sem_mutex);

    nms_shm_get(SHM_FILE, (void**)&str, SHM_MAX_LEN);
    printf("client get: %s\n", str);

    SLN_MUTEX_SHM_UNLOCK(sem_mutex);

    return 0;
}
