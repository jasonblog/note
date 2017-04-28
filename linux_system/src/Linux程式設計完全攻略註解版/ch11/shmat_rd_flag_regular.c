#include<sys/shm.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/ipc.h>
#include<string.h>

int main(int argc, char* argv[])
{
    key_t key;
    int shm_id;
    char* ptr;
    key = ftok("/", 10);

    shm_id = shmget(key, 100, IPC_CREAT | SHM_R);

    printf("get the shm id is %d\n", shm_id);

    if ((ptr = (char*)shmat(shm_id, NULL, SHM_RDONLY)) == NULL) {
        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            perror("Failed to  remove memory segment");
        }

        exit(EXIT_FAILURE);
    }

    printf("in yangzd the attach add is %p\n", ptr);


    printf("*ptr=%c\n", *ptr);
    printf("now ,try to write the memory\n");
    *ptr = 'd';
    printf("*ptr=%c\n", *ptr);

    shmdt(ptr);
    shmctl(shm_id, IPC_RMID, 0);
}
