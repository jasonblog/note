#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char **argv)
{
	char *shm_ary;
	key_t mem_key;
	int shmid;
	mem_key = ftok("/tmp", 0);
	shmid=shmget(mem_key, 4096, 0666);
	shm_ary=shmat(shmid, 0, 0);
	printf("%s\n", shm_ary);
	return 0;
}

