#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_SIZE 68157440

int main(int argc, char **argv)
{
    int shm_id;
    key_t key;
    char *p_map;

    char *name = "/tmp/foobar";
    key = ftok(name, 0);
    if (key < 0) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    shm_id = shmget(key, MAP_SIZE, IPC_CREAT);
    if (shm_id < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    p_map = (char *)shmat(shm_id, NULL, 0);
    memset(p_map, 0, MAP_SIZE);

    getchar();

    if(shmdt(p_map) < 0)
        perror("detach");

    return 0;
}
