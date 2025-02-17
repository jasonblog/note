#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "shmipc.h"


int nms_shm_get(char* shm_file, void** shm, int mem_len)
{
    int         fd;

    fd = shm_open(shm_file, O_RDWR | O_CREAT, 0666);

    if (fd < 0) {
        printf("shm_pen <%s> failed: %s\n", shm_file, strerror(errno));
        return -1;
    }

    ftruncate(fd, mem_len);

    *shm = mmap(NULL, mem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (MAP_FAILED == *shm) {
        printf("mmap: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

int main(int argc, const char* argv[])
{
    sem_t*       sem_mutex = NULL;
    char*        str = NULL;

    SLN_MUTEX_SHM_LOCK(SEM_MUTEX_FILE, sem_mutex);

    nms_shm_get(SHM_FILE, (void**)&str, SHM_MAX_LEN);
    sleep(6);
    snprintf(str, SHM_MAX_LEN, "posix semphore server!");

    SLN_MUTEX_SHM_UNLOCK(sem_mutex);
    sleep(6);

    shm_unlink(SHM_FILE);
    sem_unlink(SEM_MUTEX_FILE);

    return 0;
}
