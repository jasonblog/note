#include<sys/ipc.h>
#include<stdio.h>
#include<sys/stat.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
    key_t key;
    int i;
    struct stat buf;

    if (argc != 3) {
        printf("use: command path number\n");
        return 1;
    }

    i = atoi(argv[2]);

    if ((stat(argv[1], &buf)) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("file st_dev=0x%x\n", buf.st_dev);
    printf("file st_ino=0x%x\n", buf.st_ino);
    printf("number=0x%x\n", i);
    key = ftok(argv[1], i);

    printf("key=0x%x \tkey>>24=%x \tkey&0xffff=%x \t(key>>16)&0xff=%x\n", key,
           key >> 24, key & 0xffff, (key >> 16) & 0xff);
}
