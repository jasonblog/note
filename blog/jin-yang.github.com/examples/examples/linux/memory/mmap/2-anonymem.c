#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define BUF_SIZE 100

int main(int argc, char** argv)
{
    char    *p_map;

    /* 匿名映射,创建一块内存供父子进程通信 */
    p_map = (char *)mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if(fork() == 0) {
        sleep(1);
        printf("child got a message: %s\n", p_map);
        sprintf(p_map, "%s", "hi, dad, this is son");
        munmap(p_map, BUF_SIZE); // 实际上，进程终止时，会自动解除映射。
        exit(0);
    }

    sprintf(p_map, "%s", "hi, this is father");
    sleep(2);
    printf("parent got a message: %s\n", p_map);

    return 0;
}

