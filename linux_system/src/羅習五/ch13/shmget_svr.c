#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char **argv)
{
	key_t mem_key;
	int shmid;
	char* addr;
	/*key_t ftok(const char *pathname, int proj_id);*/
	mem_key = ftok("/tmp", 0);
	printf("%d\n", mem_key);
	/*int shmget(key_t key, size_t size, int shmflg);*/
	shmid = shmget(mem_key, 4096, IPC_CREAT | 0666);
	perror("shmget");
	printf("shmid = %d", shmid);
	/*void *shmat(int shmid, const void *shmaddr, int shmflg);*/
	addr=shmat(shmid, 0, 0);
	printf("%p", addr);
	getchar();
	sprintf(addr, "shm ready");
	return 0;
}

